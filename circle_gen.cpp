#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <share.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


struct Point
{
    int x;
    int y;
};

double step = 3.14 / 40;

int radius = 10;

Point first_o  = {10, 10};
Point second_o = {10, 10};
//Point third_0 = {};


void PrintFirstQuater(FILE *file, int radius)
{
    for (size_t i = 0; i < 21; i++)
    {
        int x_c = first_o.x + cos(step * i) * radius;
        int y_c = first_o.y - sin(step * i) * radius;

        fprintf(file, "push 1\npop [%d]\n", y_c * 10 * 2 + x_c);
    }
}

void PrintSecondQuater(FILE *file, int radius)
{
    for (size_t i = 0; i < 21; i++)
    {
        int x_c =  first_o.x - cos(step * i) * radius;
        int y_c = first_o.y - sin(step * i) * radius;
        fprintf(file, "push 1\npop [%d]\n", y_c * 10 * 2 + x_c);
    }
}

void PrintThirdQuater(FILE *file, int radius)
{
    for (size_t i = 0; i < 21; i++)
    {
        int x_c =  second_o.x  - cos(step * i) * radius ;
        int y_c = second_o.y + sin(step * i) * radius ;

        fprintf(file, "push 1\npop [%d]\n", y_c * 10 * 2 + x_c);
    }
}

void PrintFourthQuater(FILE *file, int radius)
{

    for (size_t i = 0; i < 21; i++)
    {
        int x_c =  second_o.x + cos(step * i) * radius ;
        int y_c = second_o.y + sin(step * i) * radius ;

        fprintf(file, "push 1\npop [%d]\n", y_c * 10 * 2 + x_c  );
    }

}

int main()
{
    FILE *output = fopen("circle.txt", "w");

    for(size_t i = 0; i < 10; i++)
    {
        PrintFirstQuater(output, i);
        PrintSecondQuater(output, i);
        PrintThirdQuater(output, i);
        PrintFourthQuater(output, i);

        fprintf(output, "draw\n");
    for (size_t i = 0; i < 400; i++)
    {
        fprintf(output, "push 0\npop [%d]\n", i);
    }
    }
    for(size_t i = 10; i > 0; i--)
    {
        PrintFirstQuater(output, i);
        PrintSecondQuater(output, i);
        PrintThirdQuater(output, i);
        PrintFourthQuater(output, i);

        fprintf(output, "draw\n");
    for (size_t i = 0; i < 400; i++)
    {
        fprintf(output, "push 0\npop [%d]\n", i);
    }
    }

    fprintf(output, "\nhtl");


    return 0;
}
