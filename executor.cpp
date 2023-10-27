#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commands.h"
#include "stack.h"
#include "executor.h"
#include "assembler.h"
#include "disassembler.h"

#define PUSH(expr) Push(&cpu->stack, expr)
#define POP(arg)   Pop(&cpu->stack, arg)

static StackType_t kPrecision = 1000;

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

CompileErr_t ExecuteCommands(CPU *cpu, Code *codes)
{
    Stack call_stack = {};
    StackInit(&call_stack);
    for (size_t i = 0; i < codes->capacity;)
    {
        char code = codes->codes_array[i++];

        switch (GET_OPCODE(code))
        {
            #define DEF_CMD(name, num, str, have_arg, switch_code, ...)\
            case name:\
            {\
                switch_code\
                break;\
            }

            #include "code_gen.h"

            #undef DEF_CMD

            #define DEF_CMD(name, num, str, have_arg, expr, ...)\
            case name:              \
            {               \
                StackType_t l_arg = 0;  \
                StackType_t r_arg = 0;  \
                                    \
                POP(&l_arg);            \
                POP(&r_arg);            \
                                    \
                size_t pos = codes->codes_array[++i];\
                if (expr)           \
                {                   \
                    i = pos - 1;    \
                }                   \
                                    \
                PUSH(r_arg);            \
                PUSH(l_arg);            \
                                    \
                break;              \
            }

            #include "jumps.h"

            #undef DEF_CMD
            default:
            {
                printf("kavo?\n");

                break;
            }
        }
    }

    StackDtor(&call_stack);

    return kSuccess;
}
