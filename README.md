# C++Bench2.0
An improved version of my previous attempt [cppBenchmark](https://github.com/Davidah121/cppBenchmark).

# Outline
This approach takes advantage of the compiler flag -finstrument-functions and the attributes [[no_instrument_function]].
This method uses these functions as apposed to the windows verision:
```C++
void __cyg_profile_func_enter(void *func, void *callsite);
void __cyg_profile_func_exit(void *func, void *callsite);
```

If you need to use _penter and _pexit instead, you must add an implementation for those directly and retrieve the correct start address.

# Usage
By default, all functions in a file compiled with -finstument-functions will be profiled. In order to avoid this, either add the appropriate attribute, i.e. 
[[no_instument_function]], or just compile the file without that flag which will include all functions in that file.

Upon running an application compiled with CppBench2, a folder will be created with the results with each thread getting its own file.

For easy processing of that information, code is included that will process the information however, the file structure is very simple so it is not necessary. The structure is as follows:
```C++
struct FunctionEnterInfo
{
    char identifier;
    long long functionID;
    size_t startTime;
};

struct FunctionExitInfo
{
    char identifier;
    size_t endTime;
};
```
From here, it shows that the space required is 26 bytes per function since an identifier is needed to specify if its entering or exiting. Stack ID can be solved later and does not need to be recorded. Note that the identifier can be removed from the structures as well.
Each identifier is either 'e' or 'x' where 'e' is a function enter and 'x' is a function exit.

Lastly, and most importantly, the functionID is an offset from a known function. In this case, __cyg_profile_func_enter. This results in simpler post processing. A list of functions and their offsets in the .text section of the programs code is all that is needed. Due to this, nm or llvm-nm is needed. Windows objdump can also be used.

The provided processor for the benchmark trace files uses llvm-nm in a non portable way currently.

The intended purpose is to provide a simple approach to creating a GUI or converting the files into a new format that is more convenient.

# Usage Examples
The usages of this project is very simple. Just include the necessary files and compile correctly, then run.
test.cpp shows a perfect example of all that is expected from the programmer.

For reference, test.cpp:
```C++
#include "CppBench2.h"

void benchmarkThis()
{
    double sum = 1;
    for(int i=0; i<10000; i++)
        sum += (double)i / 0.00001;
    printf("%.9f\n", sum);
}
int main()
{
    benchmarkThis();
    return 0;
}
```

Then when processing the files, you'll end up with something like this:
- main (time taken in ns)
    - benchmarkThis (time taken in ns)

# Additional Notes
- NO DEFAULT GUI
    - Though it is being worked on, it results in a data that can't be view directly which is not as useful as existing tools regardless of how lightweight this tool is.
- How lightweight it is has not been measured properly
    - Though it only requires 26 bytes per function, writing out to a file per function call is slower than just keeping that in memory and writing it out later. Unknown how much this affects things
- Does not profile everything. Just what source code it was provided with.
    - No profiling of function calls coming from an external library nor can you profile each individual operation so hot spots within a function may be missed.
- Requires changes to code
    - While obvious, some profilers like Visual Studio's or VerySleepy don't have this issue but inorder to have fine grain measurements, those need higher polling rates which results in lower performance.
    - There are cases where those polling based profilers can result in inaccurate measurements.
- REQUIRES NM
    - The biggest problem is the reliance of external tools which means that it can't be easily used in any framework. For example, additional processing is needed to use HTML, JS, and CSS solely to view the data as it needs the function names and their offsets.
    - An approach to this is just to run nm on the application, save that file in the desired folder, then run your GUI but it still requires an additional step inbetween which is not as convenient.
