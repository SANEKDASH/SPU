#include <stdio.h>

int factorial(unsigned int a)
{
    int fact = a;

    if (a > 1)
    {
        fact *= factorial(a - 1);
    }

    return fact;
}

int main()
{
    printf("%d", factorial(0));

    return 0;
}
