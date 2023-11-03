//if there are two arguments, first must be register, the last - number

DEF_CMD(kPush,  0, "push", true,

    ArgType_t instr_arg = 0;
    ArgType_t reg_arg   = 0;
    ArgType_t num_arg   = 0;

    if (GET_REG_BIT(code))
    {
        reg_arg = cpu->reg_array[codes->codes_array[cpu->ip++]];

        if (GET_RAM_BIT(code))
        {
            reg_arg /= kPrecision;
        }

        instr_arg += reg_arg;
    }
    if (GET_NUM_BIT(code))
    {
        num_arg = codes->codes_array[cpu->ip++];

        if (GET_RAM_BIT(code))
        {
            instr_arg += num_arg;
        }
        else
        {
            instr_arg += num_arg * kPrecision;
        }
    }

    if (GET_RAM_BIT(code))
    {
        PUSH(cpu->RAM[instr_arg]);
    }
    else
    {
        PUSH(instr_arg);
    }
)

DEF_CMD(kSub ,  1, "sub", false,
    ArgType_t lhs = 0;
    ArgType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);
    PUSH(lhs - rhs);
)

DEF_CMD(kMult,  2, "mult", false,
    ArgType_t l_arg = 0;
    ArgType_t r_arg = 0;

    POP(&l_arg);
    POP(&r_arg);

    ArgType_t res = (l_arg * r_arg) / (kPrecision * kPrecision);
    res *= 1000;
    PUSH(res);
    //PUSH(l_arg * r_arg / kPrecision);
)

DEF_CMD(kDiv ,  3, "div", false,
    StackElemType_t l_arg = 0;
    StackElemType_t r_arg = 0;

    POP((ArgType_t *) &r_arg);
    POP((ArgType_t *) &l_arg);

    double res = ((double) l_arg / (double) r_arg) * kPrecision;
    PUSH((int)res);
)

DEF_CMD(kOut ,  4, "out", false,
    ArgType_t arg = 0;
    POP(&arg);

    printf(">>%d\n", arg / kPrecision);
)

DEF_CMD(kIn  ,  5, "in", false,
    ArgType_t arg = 0;

    scanf("%d", &arg);

    PUSH(arg * kPrecision);
)

DEF_CMD(kSqrt,  6, "sqrt", false,
    ArgType_t arg = 0;

    POP((ArgType_t *) &arg);
    arg =  arg * kPrecision;
    arg = sqrt(arg);
    PUSH(arg);
)

DEF_CMD(kSin ,  7, "sin", false,
    ArgType_t arg = 0;

    POP(&arg);

    PUSH(sin((double)arg / kPrecision) * kPrecision);
)

DEF_CMD(kCos ,  8, "cos", false,
    ArgType_t arg = 0;

    POP(&arg);

    PUSH(cos((double)arg / kPrecision) * kPrecision);
)

DEF_CMD(kPop , 9, "pop", true,
    ArgType_t ram_arg  = 0;
    ArgType_t num_arg  = 0;
    ArgType_t reg_arg  = 0;

    if (GET_REG_BIT(code))
    {
        reg_arg = codes->codes_array[cpu->ip++];

        ram_arg += cpu->reg_array[reg_arg] / kPrecision;
    }
    if (GET_NUM_BIT(code))
    {
        num_arg = codes->codes_array[cpu->ip++];

        ram_arg += num_arg;
    }

    if (GET_RAM_BIT(code))
    {
        POP(&cpu->RAM[ram_arg]);
    }
    else
    {
        POP(&cpu->reg_array[reg_arg]);
    }
)
DEF_CMD(kAdd , 10, "add", false,
    ArgType_t lhs = 0;
    ArgType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);

    PUSH(lhs + rhs);
)

DEF_CMD(kHlt , 11, "hlt", false,
    return kSuccess;
)
DEF_CMD(kCall, 12, "call", true,
    Push(&call_stack, cpu->ip + 1);

    ArgType_t pos = codes->codes_array[cpu->ip];
    cpu->ip = pos;
)
DEF_CMD(kRet , 13, "ret", false,
    ArgType_t pos = 0;
    Pop(&call_stack, &pos);
    cpu->ip = pos;
)

DEF_CMD(kDraw, 14, "draw", false,
    //system("cls");
    for (size_t i = 0; i < kMaxRamSize; i++)
    {
        if (cpu->RAM[i] / kPrecision == 0)
        {
            ColorPrintf(kGrey, "[.]");
        }
        else if (cpu->RAM[i] / kPrecision == 1)
        {
            ColorPrintf(kWhite, "[*]");
        }

        if (((i + 1) % 20) == 0)
        {
            putchar('\n');
        }
    }
    //system("cls");
)

DEF_CMD(kShow, 15, "show", false,
    system("cls");
    for (size_t i = 0; i < kMaxRamSize; i++)
    {
        if (cpu->RAM[i] / kPrecision == 0)
        {
            ColorPrintf(kGrey, "[.]");
        }
        else if (cpu->RAM[i] / kPrecision == 1)
        {
            ColorPrintf(kWhite, "[*]");
        }

        if (((i + 1) % 20) == 0)
        {
            putchar('\n');
        }
    }
    system("cls");
)
