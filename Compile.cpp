#include <stdio.h>
#include <string.h>

#include "compiler.h"

int main()
{
    Text highlevel_code = {};
    ReadTextFromFile(&highlevel_code, "input.txt");

    CompileText(&highlevel_code, "output.txt");

    TextDtor(&highlevel_code);

    return 0;
}
