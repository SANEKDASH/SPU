#include <stdio.h>

#include "assembler.h"
#include "commands.h"
#include "disassembler.h"

int main()
{
    DisassembleBinFile("output.bin", "disassembled.txt");

    return 0;
}
