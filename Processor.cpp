#include <stdio.h>

#include "commands.h"
#include "assembler.h"
#include "stack.h"
#include "executor.h"
#include "disassembler.h"

int main()
{

    INIT_LOG;//()

    CPU cpu = {};
    CpuInit(&cpu);

    Code codes = {};

    StackInit(&cpu.stack);

    ReadCode(&codes, "output.bin");

    ExecuteCommands(&cpu, &codes);

    CpuDtor(&cpu);

    CLOSE_LOG;

    return 0;
}

