#include <stdio.h>

#include "assembler.h"
#include "commands.h"
#include "disassembler.h"

int main(int argc, char *argv[])
{
    const char *src_file  = argv[1];
    const char *dest_file = "disassembled.txt";

    if (argc == 3)
    {
        dest_file = argv[2];
    }

    DisassembleBinFile(src_file, dest_file);

    return 0;
}
