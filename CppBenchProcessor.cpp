#include "CppBench2.h"
#include <stdio.h>
#include <string>
#include <filesystem>
#include <stack>

CppBenchProcessor::CppBenchProcessor()
{
}

CppBenchProcessor::CppBenchProcessor(const std::string& exeFile, const std::string& folder)
{
    //REQUIRES nm or llvm-nm.
    getFunctionNames(exeFile);
    getAllCallsFromFolder(folder);
}

CppBenchProcessor::~CppBenchProcessor()
{

}

std::vector<CppBench_CallInfo>& CppBenchProcessor::getAllFunctionCalls(size_t threadID)
{
    auto it = allCalls.find(threadID);
    if(it != allCalls.end())
    {
        return it->second;
    }

    throw nullptr;
}

std::vector<size_t> CppBenchProcessor::getAllThreadIDs()
{
    std::vector<size_t> threadIDs;
    for(auto& pair : allCalls)
    {
        threadIDs.push_back(pair.first);
    }
    return threadIDs;
}

std::unordered_map<size_t, CppBench_FunctionInfo>& CppBenchProcessor::getAllFunctions()
{
    return allFunctions;
}

CppBench_FunctionInfo CppBenchProcessor::getFunctionInfo(size_t functionID)
{
    auto it = allFunctions.find(functionID);
    if(it != allFunctions.end())
        return it->second;
    return {0, 0, 0, 0, "", ""};
}

size_t hexStringToSizeT(std::string s)
{
    size_t result = 0;
    for(int i=0; i<s.size(); i++)
    {
        result <<= 4;
        char c = tolower(s[i]);
        if(c >= '0' && c <= '9')
            result += c - '0';
        if(c >= 'a' && c <= 'f')
            result += (c - 'a') + 10;
    }
    return result;
}

void CppBenchProcessor::getFunctionNames(const std::string& exeOrDebugFile)
{
    std::string s = "llvm-nm -C -l " + exeOrDebugFile;
    FILE* nmPipe = _popen(s.c_str(), "rb");
    if(nmPipe == nullptr)
        throw s;
    
    //format - split by space (not that simple sadly)
        //address ??? function_name file_and_lineNumber
    
    std::vector<std::string> lineSplit;
    std::string temp = "";
    while(true)
    {
        char c = fgetc(nmPipe);
        if(feof(nmPipe))
            break;

        if(c > ' ')
            temp += c;
        else if(c == ' ')
        {
            if(lineSplit.size() < 2)
            {
                lineSplit.push_back(temp);
                temp = "";
            }
            else
                temp += c;
        }
        else if(c == '\t')
        {
            lineSplit.push_back(temp);
            temp = "";
        }
        else if(c == '\n')
        {
            if(temp.size() > 0)
                lineSplit.push_back(temp);
            
            if(lineSplit.size() >= 3)
            {
                CppBench_FunctionInfo cpfi;
                cpfi.callCount = 0;
                cpfi.totalTime = 0;
                cpfi.line = 0;
                cpfi.functionID = hexStringToSizeT(lineSplit[0]);
                cpfi.functionSignature = lineSplit[2];
                if(lineSplit.size() >= 4)
                {
                    //split by colon
                    size_t indexOfSplit = lineSplit[3].find_last_of(':');
                    cpfi.file = lineSplit[3].substr(0, indexOfSplit);
                    if(indexOfSplit+1 < lineSplit[3].size())
                        cpfi.line = std::stoll(lineSplit[3].substr(indexOfSplit+1));
                }
                allFunctions.insert({cpfi.functionID, cpfi});

                if(cpfi.functionSignature == "__cyg_profile_func_enter")
                    baseFunctionLocation = cpfi.functionID;
            }
            lineSplit.clear();
            temp = "";
        }
    }

    int err = _pclose(nmPipe);
    if(err != 0)
        throw err;
}


void CppBenchProcessor::getAllCallsFromFolder(const std::string& folder)
{
    std::filesystem::directory_iterator iterator = std::filesystem::directory_iterator(folder);
    for(auto& entry : iterator)
    {
        if(entry.is_regular_file())
            processCallTrace(entry.path().u8string());
    }
}


void CppBenchProcessor::processCallTrace(const std::string& file)
{
    FILE* f = fopen(file.c_str(), "rb");

    std::vector<CppBench_CallInfo> allThreadCalls;
    std::stack<CppBench_CallInfo> tempCallStack;

    size_t threadID = -1;
    size_t stackNum = 0;
    if(f != nullptr)
    {
        fread(&threadID, sizeof(size_t), 1, f);
        while(true)
        {
            char code = fgetc(f);
            if(feof(f))
                break;
                
            if(code == 'e')
            {
                CppBench_CallInfo c;
                size_t offsetFromBase;

                fread(&offsetFromBase, sizeof(size_t), 1, f);
                if(feof(f))
                    break;

                size_t functionID = baseFunctionLocation + offsetFromBase;
                auto iterator = allFunctions.find(functionID);
                if(iterator == allFunctions.end())
                    throw functionID;

                c.functionID = functionID;
                c.stackID = tempCallStack.size();
                
                fread(&c.startTime, sizeof(size_t), 1, f);
                if(feof(f))
                    break;

                tempCallStack.push(c);
            }
            else if(code == 'x')
            {
                CppBench_CallInfo lastCallInfo = tempCallStack.top();
                tempCallStack.pop();
                
                fread(&lastCallInfo.endTime, sizeof(size_t), 1, f);
                if(feof(f))
                    break;
                
                    
                auto iterator = allFunctions.find(lastCallInfo.functionID);
                if(iterator == allFunctions.end())
                    throw lastCallInfo.functionID;

                iterator->second.callCount++;
                iterator->second.totalTime += lastCallInfo.endTime - lastCallInfo.startTime;
                
                allThreadCalls.push_back(lastCallInfo);
            }
        }
        fclose(f);
    }

    if(allThreadCalls.size() > 0 && threadID != -1)
    {
        allCalls.insert({threadID, allThreadCalls});
    }
}