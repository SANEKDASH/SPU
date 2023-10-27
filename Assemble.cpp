#include <stdio.h>
#include <string.h>

#include "assembler.h"

int main()
{
    Text highlevel_code = {};
    ReadTextFromFile(&highlevel_code, "input.txt");
    //PrintTextInFile(stdout, &highlevel_code);
    CompileText(&highlevel_code, "output.txt");

    TextDtor(&highlevel_code);

    return 0;
}
