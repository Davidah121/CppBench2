CppBench2Util.o: CppBench2Util.h CppBench2Util.cpp
	clang++ -O2 -c CppBench2Util.cpp

test.o: CppBench2.h CppBench2Util.o
	clang++ -O2 -c -finstrument-functions test.cpp

output: test.o
	clang++ -O2 test.o CppBench2Util.o -o output
