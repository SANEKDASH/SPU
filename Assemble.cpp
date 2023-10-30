#include <stdio.h>
#include <string.h>

#include "assembler.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf(">>You should put here name of input file.\n");

        return 0;
    }

    Text highlevel_code = {};
    Text code_copy = {};

    ReadTextFromFile(&code_copy, argv[1]);
    ReadTextFromFile(&highlevel_code, argv[1]);
    CompileText(&highlevel_code, &code_copy, "output.txt");

    TextDtor(&highlevel_code);
    TextDtor(&code_copy);

    return 0;
}
