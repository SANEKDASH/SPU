#include <stdio.h>
#include <string.h>

#include "assembler.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf(">>You should put here name of input file.\n");

        return -1;
    }

    Text highlevel_code = {};
    Text code_copy = {};

    const char* src_file  = argv[1];

    const char* dest_file = "a.bin";

    if (argc == 3)
    {
        dest_file = argv[2];
    }

    ReadTextFromFile(&code_copy, src_file);
    ReadTextFromFile(&highlevel_code, src_file);
    CompileText(&highlevel_code, &code_copy, dest_file);

    TextDtor(&highlevel_code);
    TextDtor(&code_copy);

    return 0;
}
