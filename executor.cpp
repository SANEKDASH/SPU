#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "commands.h"
#include "stack.h"
#include "executor.h"
#include "assembler.h"
#include "disassembler.h"
#include "color_print.h"

CompileErr_t CpuInit(CPU *cpu)
{
    StackInit(&cpu->stack);

    cpu->RAM = (StackElemType_t *) calloc(kMaxRamSize, sizeof(StackElemType_t));

    if (cpu->RAM == nullptr)
    {
        perror("CpuInit() failed to allocate memory for RAM");

        return kAllocError;
    }

    return kSuccess;
}

CompileErr_t CpuDtor(CPU *cpu)
{
    StackDtor(&cpu->stack);

    free(cpu->RAM);
    cpu->RAM = nullptr;

    return kSuccess;
}

static const StackElemType_t kPrecision = 1000;

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
#define PUSH(expr) Push(&cpu->stack, expr)
#define POP(arg)   Pop(&cpu->stack, arg)

    Stack call_stack = {};
    StackInit(&call_stack);
    for (size_t i = 0; i < codes->capacity;)
    {
        char code = codes->codes_array[i++];

        switch (GET_OPCODE(code))
        {
            #define DEF_CMD(name, num, str, have_arg, switch_code, ...)\
            case name:                                                 \
            {                                                          \
                switch_code                                            \
                break;                                                 \
            }

            #include "code.gen.h"

            #undef DEF_CMD

            #define DEF_CMD(name, num, str, have_arg, expr, ...) \
            case name:                                           \
            {                                                    \
                StackElemType_t l_arg = 0;                       \
                StackElemType_t r_arg = 0;                       \
                                                                 \
                POP(&l_arg);                                     \
                POP(&r_arg);                                     \
                size_t pos = codes->codes_array[i++];            \
                if (expr)                                        \
                {                                                \
                    i = pos;                                     \
                }                                                \
                                                                 \
                PUSH(r_arg);                                     \
                PUSH(l_arg);                                     \
                                                                 \
                break;                                           \
            }

            #include "jumps.gen.h"

            #undef DEF_CMD
            default:
            {
                printf("kavo?\n");

                break;
            }
        }

        #undef PUSH
        #undef POP
    }

    StackDtor(&call_stack);

    return kSuccess;

}
