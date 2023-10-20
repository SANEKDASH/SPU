#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commands.h"
#include "stack.h"
#include "executor.h"
#include "compiler.h"
#include "disassembler.h"

#define PUSH(expr) Push(&cpu->stack, expr)
#define POP(arg)   Pop(&cpu->stack, arg)

static size_t kPrecision = 1000;

CommandCode_t SeekByteCommand(const int code)
{
    for (size_t i = 0; i < kCommandCount; ++i)
    {
        if (code == CommandArray[i].command_code)
        {
            return CommandArray[i].command_code;
        }
    }

    return kNotACommand;
}

CompError_t ExecuteCommands(CPU *cpu, Code *codes)
{
    for (size_t i = 0; i < codes->capacity; i++)
    {
        char code = codes->codes_array[i];

        StackType_t arg = 0;

        switch (GetBits(code, 4, 0))
        {

        #define DEF_CMD(name, num, str, switch_seg, ...)\
            case name:\
            {\
                switch_seg\
                break;\
            }

        #include "code_gen.h"

        #undef DEF_CMD

        }
    }

    return kSucces;
}
