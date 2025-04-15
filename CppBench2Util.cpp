#include "CppBench2Util.h"
#include <chrono>
#include <thread>
#include <string>
#include <filesystem>

size_t benchmarkTimeNano()
{
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}
size_t benchmarkGetPID()
{
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

std::string getCurrTimeAsString()
{
    char buffer[128];
    time_t currTime = time(nullptr);
    tm* timeInfo = localtime(&currTime);
    strftime(buffer, 128, "%d-%m-%Y %H_%M_%S", timeInfo);

    return std::string(buffer);
}

std::string BenchmarkFile::BenchmarkFolder = "BenchmarkResults " + getCurrTimeAsString();

BenchmarkFile::BenchmarkFile()
{
    size_t tid = benchmarkGetPID();
    std::filesystem::create_directory(BenchmarkFolder);
    std::string s = BenchmarkFolder+"/benchTrace_"+std::to_string(tid)+".cppb";
    this->f = fopen(s.c_str(), "wb");
    fwrite(&tid, sizeof(size_t), 1, f);
}

BenchmarkFile::BenchmarkFile(BenchmarkFile&& other) noexcept
{
    this->f = other.f;
    other.f = nullptr;
}

BenchmarkFile::~BenchmarkFile()
{
    if(f != nullptr)
        fclose(f);
    f = nullptr;
}
FILE* BenchmarkFile::getFile()
{
    return f;
}
