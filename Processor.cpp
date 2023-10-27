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

    Text byte_code = {};

    Code codes = {};

    StackInit(&cpu.stack);

    ReadTextFromFile(&byte_code, "output.txt");

    ReadCode(&codes, "output.bin");

    ExecuteCommands(&cpu, &codes);

    StackDtor(&cpu.stack);
    TextDtor(&byte_code);

    CLOSE_LOG;

    return 0;
}

