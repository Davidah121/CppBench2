

#pragma once
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <vector>

class BenchmarkFile 
{
public:
    static std::string BenchmarkFolder;

    [[gnu::no_instrument_function]] BenchmarkFile();
    [[gnu::no_instrument_function]] BenchmarkFile(BenchmarkFile&& f) noexcept;
    [[gnu::no_instrument_function]] ~BenchmarkFile();

    [[gnu::no_instrument_function]] FILE* getFile();
private:
    FILE* f = nullptr;
};

struct CppBench_FunctionInfo
{
    size_t functionID;
    size_t line;
    
    size_t callCount;
    size_t totalTime;

    std::string functionSignature;
    std::string file;
};

struct CppBench_CallInfo
{
    size_t functionID;
    size_t stackID;
    size_t startTime;
    size_t endTime;
};

class CppBenchProcessor
{
public:
    CppBenchProcessor();
    CppBenchProcessor(const std::string& exeFile, const std::string& folder);
    ~CppBenchProcessor();

    std::vector<CppBench_CallInfo>& getAllFunctionCalls(size_t threadID);
    std::unordered_map<size_t, CppBench_FunctionInfo>& getAllFunctions();
    CppBench_FunctionInfo getFunctionInfo(size_t functionID);
    std::vector<size_t> getAllThreadIDs();

private:
    size_t baseFunctionLocation = 0;
    void getFunctionNames(const std::string& exeOrDebugFile);
    void getAllCallsFromFolder(const std::string& folder);
    void processCallTrace(const std::string& file);
    std::unordered_map<size_t, CppBench_FunctionInfo> allFunctions;
    std::unordered_map<size_t, std::vector<CppBench_CallInfo>> allCalls;
};

[[gnu::no_instrument_function]] size_t benchmarkTimeNano();
[[gnu::no_instrument_function]] size_t benchmarkGetPID();