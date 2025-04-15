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