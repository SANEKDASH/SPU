# 1 "executor.cpp"
# 1 "<command-line>"
# 1 "executor.cpp"
# 15 "executor.cpp"
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

CompileErr_t ExecuteCommands(CPU *cpu, Code *codes)
{
    Stack call_stack = {};
    StackInit(&call_stack);

    for (size_t i = 0; i < codes->capacity; i++)
    {
        char code = codes->codes_array[i];
        StackType_t arg = 0;

        int a = GET_OPCODE(code);

        switch (a)
        {







# 1 "code_gen.h" 1
case kPush: { arg = codes->codes_array[++i]; if (GET_NUM_BIT(code)) { Push(&cpu->stack, arg * kPrecision); } else if (GET_REG_BIT(code)) { Push(&cpu->stack, cpu->reg_array[arg]); } break; }
# 14 "code_gen.h"
case kAdd: { arg = codes->codes_array[++i]; if (Pop(&cpu->stack, &cpu->reg_array[arg]) != kStackClear) { printf("POP: Stack is empty\n"); } break; }
# 23 "code_gen.h"
case kSub: { StackType_t lhs = 0; StackType_t rhs = 0; Pop(&cpu->stack, &rhs); Pop(&cpu->stack, &lhs); Push(&cpu->stack, lhs - rhs); break; }
# 33 "code_gen.h"
case kMult: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); Push(&cpu->stack, l_arg * r_arg / kPrecision); break; }
# 43 "code_gen.h"
case kDiv: { double l_arg = 0; double r_arg = 0; Pop(&cpu->stack, (StackType_t *) &r_arg); Pop(&cpu->stack, (StackType_t *) &l_arg); Push(&cpu->stack, (l_arg / r_arg) * kPrecision); break; }
# 53 "code_gen.h"
case kOut: { StackType_t arg = 0; Pop(&cpu->stack, &arg); printf(">>%d\n", arg / kPrecision ); break; }







case kIn: { StackType_t arg = 0; scanf("%d", &arg); Push(&cpu->stack, a); break; }







case kSqrt: { double arg = 0; Pop(&cpu->stack, (StackType_t *) &arg); Push(&cpu->stack, sqrt(arg / kPrecision) * kPrecision); break; }







case kSin: { double arg = 0; Pop(&cpu->stack, (StackType_t *) &arg); Push(&cpu->stack, sin(arg / kPrecision) * kPrecision); break; }







case kCos: { double arg = 0; Pop(&cpu->stack, (StackType_t *) &arg); Push(&cpu->stack, cos(arg / kPrecision) * kPrecision); break; }







case kPop: { arg = codes->codes_array[++i]; if (Pop(&cpu->stack, &cpu->reg_array[arg]) != kStackClear) { printf("POP: Stack is empty\n"); } break; }
# 102 "code_gen.h"
case kHtl: { return kSucces; break; }


case kCall: { Push(&call_stack, i); StackType_t pos = codes->codes_array[++i]; i = pos - 1; break; }





case kRet: { StackType_t pos = 0; Pop(&call_stack, &pos); i = pos + 1; break; }
# 52 "executor.cpp" 2
# 76 "executor.cpp"
# 1 "jumps.h" 1
case kJmp: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (true) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
case kJa: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (l_arg >= r_arg) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
case kJae: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (l_arg >= r_arg) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
case kJb: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (l_arg < r_arg) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
case kJbe: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (l_arg <= r_arg) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
case kJe: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (l_arg == r_arg) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
case kJne: { StackType_t l_arg = 0; StackType_t r_arg = 0; Pop(&cpu->stack, &l_arg); Pop(&cpu->stack, &r_arg); size_t pos = codes->codes_array[++i]; if (l_arg != r_arg) { i = pos - 1; } Push(&cpu->stack, r_arg); Push(&cpu->stack, l_arg); break; }
# 77 "executor.cpp" 2


            default:
            {
                printf("kavo?\n");

                break;
            }
        }
    }

    StackDtor(&call_stack);

    return kSucces;
}
