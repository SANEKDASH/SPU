#include <stdio.h>

#include "compiler.h"
#include "commands.h"
#include "disassembler.h"

int main()
{
    Text byte_code = {};

    ReadTextFromFile(&byte_code, "output.txt");

    DisassembleBinFile("output.bin", "disassembled.txt");

    TextDtor(&byte_code);

    return 0;
}
