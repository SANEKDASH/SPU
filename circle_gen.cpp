#include <stdio.h>
#include <stdlib.h>
#include <math.h>


struct Point
{
    int x;
    int y;
};

double step = 2 * 3.14 / 60;

int radius = 10;

Point first_o  = {10, 10};
Point second_o = {10, 10};

void PrintCircle(FILE *file, int radius)
{
    for (size_t i = 0; i < 60; i++)
    {
        int x_c = first_o.x + cos(step * i) * radius;
        int y_c = first_o.y + sin(step * i) * radius;

        fprintf(file, "push 1\npop [%d]\n", y_c * 10 * 2 + x_c);
    }
}


int main()
{
    FILE *output = fopen("circle.txt", "w");

    for (size_t i = 0; i < 10; i++)
    {
        PrintCircle(output, i);

        fprintf(output, "show\n");
        for (size_t i = 0; i < 400; i++)
        {
            fprintf(output, "push 0\npop [%d]\n", i);
        }
    }
    for (size_t i = 10; i > 0; i--)
    {
        PrintCircle(output, i);

        fprintf(output, "show\n");
        for (size_t i = 0; i < 400; i++)
        {
            fprintf(output, "push 0\npop [%d]\n", i);
        }
    }
    fprintf(output, "\nhtl");


    return 0;
}
