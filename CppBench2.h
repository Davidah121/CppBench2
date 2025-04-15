#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "CppBench2Util.h"

//llvm-objdump -t output.exe
//Gives the offsets of functions starting from 0.

//Profile information written out gives function and caller function as offsets from __cyg_profile_func_enter making it
    //easier to find these in a list and not have to worry about any odd offsets of the addresses.
extern "C"
{
    [[gnu::no_instrument_function]] void __cyg_profile_func_enter(void *func, void *callsite);
    [[gnu::no_instrument_function]] void __cyg_profile_func_exit(void *func, void *callsite);
    
    thread_local static BenchmarkFile threadBenchFile = BenchmarkFile();
    void __cyg_profile_func_enter(void *func, void *callsite)
    {
        FILE* fileP = threadBenchFile.getFile();
        if(fileP != nullptr)
        {
            long long startAddr = (long long)__cyg_profile_func_enter;
            char code = 'e';
            size_t currTime = benchmarkTimeNano();
            long long actualAddr1 = (long long)func - startAddr;
            // long long actualAddr2 = (long long)callsite - startAddr;
            fwrite(&code, 1, 1, fileP);
            fwrite(&actualAddr1, sizeof(long long), 1, fileP);
            // fwrite(&actualAddr2, sizeof(long long), 1, fileP);
            fwrite(&currTime, sizeof(size_t), 1, fileP);
        }
    }
    void __cyg_profile_func_exit(void *func, void *callsite)
    {
        FILE* fileP = threadBenchFile.getFile();
        if(fileP != nullptr)
        {
            char code = 'x';
            size_t currTime = benchmarkTimeNano();
            fwrite(&code, 1, 1, fileP);
            fwrite(&currTime, sizeof(size_t), 1, fileP);
        }
    }
}