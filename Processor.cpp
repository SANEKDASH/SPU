#include <stdio.h>

#include "commands.h"
#include "assembler.h"
#include "stack.h"
#include "executor.h"
#include "disassembler.h"

int main(int argc, char *argv[])
{
    INIT_LOG;
    if (argc < 2)
    {
        printf(">>Processor.cpp: you should write a file you want to execute.\n");
        return -1;
    }
    else if (argc > 2)
    {
        printf(">>Processor.cpp: too much arguments.\n");

        return -1;
    }

    const char *src_file = argv[1];

    CPU cpu = {};
    CpuInit(&cpu);

    Code codes = {};

    StackInit(&cpu.stack);

    ReadCode(&codes, src_file);

    ExecuteCommands(&cpu, &codes);

    CpuDtor(&cpu);

    CLOSE_LOG;

    return 0;
}

