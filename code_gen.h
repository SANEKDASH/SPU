DEF_CMD(kPush,  0, "push", true,

    StackType_t reg_arg = 0;
    int num_arg = 0;
//if there are two arguments, first must be register, the last - number
    if (GET_REG_BIT(code))
    {
        reg_arg = codes->codes_array[i++];
    }
    if (GET_NUM_BIT(code))
    {
        num_arg = codes->codes_array[i++];
    }

    if (GET_RAM_BIT(code))
    {
        if (GET_REG_BIT(code) && GET_NUM_BIT(code))
        {
            PUSH(cpu->RAM[num_arg + cpu->reg_array[reg_arg]]);
        }
        else if (GET_NUM_BIT(code))
        {
            printf("RAM[%d] = %d\n",num_arg, cpu->RAM[num_arg]);
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

DEF_CMD(kAdd ,  1, "add", false,
    StackType_t arg = codes->codes_array[++i];

    if (POP(&cpu->reg_array[arg]) != kStackClear)
    {
        printf("POP: Stack is empty\n");
    }
)

DEF_CMD(kSub ,  2, "sub", false,
    StackType_t lhs = 0;
    StackType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);
    PUSH(lhs - rhs);
)

DEF_CMD(kMult,  3, "mult", false,
    StackType_t l_arg = 0;
    StackType_t r_arg = 0;

    POP(&l_arg);
    POP(&r_arg);

    PUSH((StackType_t)l_arg * r_arg / kPrecision);
)

DEF_CMD(kDiv ,  4, "div", false,
    StackType_t l_arg = 0;
    StackType_t r_arg = 0;

    POP((StackType_t *) &r_arg);
    POP((StackType_t *) &l_arg);

    PUSH((l_arg / r_arg) * kPrecision);
)

DEF_CMD(kOut ,  5, "out", false,
    StackType_t arg = 0;
    POP(&arg);

    printf(">>%d\n", arg / kPrecision);
)

DEF_CMD(kIn  ,  6, "in", false,
    StackType_t arg = 0;

    scanf("%d", &arg);

    PUSH(arg * kPrecision);
)

DEF_CMD(kSqrt,  7, "sqrt", false,
    StackType_t arg = 0;

    POP((StackType_t *) &arg);
    arg =  arg * kPrecision;
    arg = sqrt(arg);
    PUSH(arg);
)

DEF_CMD(kSin ,  8, "sin", false,
    double arg = 0;

    POP((StackType_t *) &arg);

    PUSH(sin(arg / kPrecision) * kPrecision);
)

DEF_CMD(kCos ,  9, "cos", false,
    double arg = 0;

    POP((StackType_t *) &arg);

    PUSH(cos(arg / kPrecision) * kPrecision);
)

DEF_CMD(kPop , 10, "pop", true,
    StackType_t num_arg = 0;
    StackType_t reg_arg = 0;

    if (GET_REG_BIT(code))
    {
        reg_arg = codes->codes_array[i++];
    }
    if (GET_NUM_BIT(code))
    {
        num_arg = codes->codes_array[i++];
    }

    if (GET_RAM_BIT(code))
    {
        if (GET_REG_BIT(code) && GET_NUM_BIT(code))
        {
            POP(&cpu->RAM[cpu->reg_array[reg_arg] + num_arg]);
        }
        else if (GET_REG_BIT(code))
        {
            POP(&cpu->RAM[cpu->reg_array[reg_arg]]);
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
DEF_CMD(kSum , 11, "sum", false,
    StackType_t lhs = 0;
    StackType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);

    PUSH(lhs + rhs);
)

DEF_CMD(kHtl , 12, "htl", false,
    return kSuccess;
)
DEF_CMD(kCall, 13, "call:", true,
    Push(&call_stack, i + 1);

    StackType_t pos = codes->codes_array[i];
    i = pos;
)
DEF_CMD(kRet , 14, "ret", false,
    StackType_t pos = 0;
    Pop(&call_stack, &pos);
    i = pos;
)
