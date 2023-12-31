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



static StackElemType_t GetArgument(CPU *cpu,
                                   Code *codes,
                                   StackElemType_t op_code);



CompileErr_t CpuInit(CPU *cpu)
{
    StackInit(&cpu->stack);

    cpu->RAM = (StackElemType_t *) calloc(kMaxRamSize, sizeof(StackElemType_t));

    if (cpu->RAM == nullptr)
    {
        perror("CpuInit() failed to allocate memory for RAM");

        return kAllocError;
    }

    cpu->ip = 0;

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



static StackElemType_t GetArgument(CPU *cpu,
                                   Code *codes,
                                   StackElemType_t op_code)
{
    ArgType_t instr_arg = 0;
    ArgType_t reg_arg   = 0;
    ArgType_t num_arg   = 0;

    if (GET_REG_BIT(op_code))
    {
        reg_arg = cpu->reg_array[codes->codes_array[cpu->ip++]];

        if (GET_RAM_BIT(op_code))
        {
            reg_arg /= kPrecision;
        }

        instr_arg += reg_arg;
    }

    if (GET_NUM_BIT(op_code))
    {
        num_arg = codes->codes_array[cpu->ip++];

        if (GET_RAM_BIT(op_code))
        {
            instr_arg += num_arg;
        }
        else
        {
            instr_arg += num_arg * kPrecision;
        }
    }

    return instr_arg;
}



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



CompileErr_t ExecuteCommands(CPU *cpu,
                             Code *codes)
{
    #define PUSH(expr) Push(&cpu->stack, expr)
    #define POP(arg)   Pop(&cpu->stack, arg)

    Stack call_stack = {};

    StackInit(&call_stack);

    for (; cpu->ip < codes->capacity;)
    {
        StackElemType_t code = codes->codes_array[cpu->ip++];

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
                                                                 \
                size_t pos = codes->codes_array[cpu->ip++];      \
                if (expr)                                        \
                {                                                \
                    cpu->ip = pos;                               \
                }                                                \
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



