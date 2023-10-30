DEF_CMD(kPush,  0, "push", true,

    StackElemType_t reg_arg = 0;
    int num_arg = 0;
//if there are two arguments, first must be register, the last - number
    if (GET_REG_BIT(code))
    {
        reg_arg = codes->codes_array[cpu->ip++];
    }
    if (GET_NUM_BIT(code))
    {
        num_arg = codes->codes_array[cpu->ip++];
    }

    if (GET_RAM_BIT(code))
    {
        if (GET_REG_BIT(code) && GET_NUM_BIT(code))
        {
            PUSH(cpu->RAM[num_arg + cpu->reg_array[reg_arg]]);
        }
        else if (GET_NUM_BIT(code))
        {
            PUSH(cpu->RAM[num_arg]);
        }
        else if (GET_REG_BIT(code))
        {
            PUSH(cpu->RAM[cpu->reg_array[reg_arg]]);
        }
    }
    else
    {
        if (GET_REG_BIT(code) && GET_NUM_BIT(code))
        {
            PUSH(kPrecision * num_arg + cpu->reg_array[reg_arg]);
        }
        else if (GET_REG_BIT(code))
        {
            PUSH(cpu->reg_array[reg_arg]);
        }
        else if (GET_NUM_BIT(code))
        {
            PUSH(kPrecision * num_arg);
        }
    }
)

DEF_CMD(kSub ,  1, "sub", false,
    StackElemType_t lhs = 0;
    StackElemType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);
    PUSH(lhs - rhs);
)

DEF_CMD(kMult,  2, "mult", false,
    StackElemType_t l_arg = 0;
    StackElemType_t r_arg = 0;

    POP(&l_arg);
    POP(&r_arg);

    PUSH((StackElemType_t)l_arg * r_arg / kPrecision);
)

DEF_CMD(kDiv ,  3, "div", false,
    StackElemType_t l_arg = 0;
    StackElemType_t r_arg = 0;

    POP((StackElemType_t *) &r_arg);
    POP((StackElemType_t *) &l_arg);

    PUSH((l_arg / r_arg) * kPrecision);
)

DEF_CMD(kOut ,  4, "out", false,
    StackElemType_t arg = 0;
    POP(&arg);

    printf(">>%d\n", arg / kPrecision);
)

DEF_CMD(kIn  ,  5, "in", false,
    StackElemType_t arg = 0;

    scanf("%d", &arg);

    PUSH(arg * kPrecision);
)

DEF_CMD(kSqrt,  6, "sqrt", false,
    StackElemType_t arg = 0;

    POP((StackElemType_t *) &arg);
    arg =  arg * kPrecision;
    arg = sqrt(arg);
    PUSH(arg);
)

DEF_CMD(kSin ,  7, "sin", false,
    double arg = 0;

    POP((StackElemType_t *) &arg);

    PUSH(sin(arg / kPrecision) * kPrecision);
)

DEF_CMD(kCos ,  8, "cos", false,
    double arg = 0;

    POP((StackElemType_t *) &arg);

    PUSH(cos(arg / kPrecision) * kPrecision);
)

DEF_CMD(kPop , 9, "pop", true,
    StackElemType_t num_arg = 0;
    StackElemType_t reg_arg = 0;

    if (GET_REG_BIT(code))
    {
        reg_arg = codes->codes_array[cpu->ip++];
    }
    if (GET_NUM_BIT(code))
    {
        num_arg = codes->codes_array[cpu->ip++];
    }

    if (GET_RAM_BIT(code))
    {
        if (GET_REG_BIT(code) && GET_NUM_BIT(code))
        {
            POP(&cpu->RAM[cpu->reg_array[reg_arg] + num_arg]);
        }
        else if (GET_REG_BIT(code))
        {
            POP(&cpu->RAM[cpu->reg_array[reg_arg] / kPrecision]);
        }
        else if (GET_NUM_BIT(code))
        {
            POP(&cpu->RAM[num_arg]);
        }
    }
    else
    {
        POP(&cpu->reg_array[reg_arg]);
    }
)
DEF_CMD(kAdd , 10, "add", false,
    StackElemType_t lhs = 0;
    StackElemType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);

    PUSH(lhs + rhs);
)

DEF_CMD(kHlt , 11, "hlt", false,
    return kSuccess;
)
DEF_CMD(kCall, 12, "call", true,
    Push(&call_stack, cpu->ip + 1);

    StackElemType_t pos = codes->codes_array[cpu->ip];
    cpu->ip = pos;
)
DEF_CMD(kRet , 13, "ret", false,
    StackElemType_t pos = 0;
    Pop(&call_stack, &pos);
    cpu->ip = pos;
)

DEF_CMD(kDraw, 14, "draw", false,
    system("cls");
    for (size_t i = 0; i < kMaxRamSize; i++)
    {
        if (cpu->RAM[i] / kPrecision == 0)
        {
            ColorPrintf(kGreen, "[.]");
        }
        else if (cpu->RAM[i] / kPrecision == 1)
        {
            ColorPrintf(kRed, "[*]");
        }

        if (((i + 1) % 20) == 0)
        {
            putchar('\n');
        }
    }
    system("cls");
)
