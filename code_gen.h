DEF_CMD(kPush,  0, "push",
    arg = codes->codes_array[++i];

    if (GetBits(code, 1, 4))
    {
        PUSH(arg * kPrecision);
    }
    else
    {
        PUSH(cpu->reg_array[arg]);
    }
)

DEF_CMD(kAdd ,  1, "add",
    arg = codes->codes_array[++i];

    if (POP(&cpu->reg_array[arg]) != kStackClear)
    {
        printf("POP: Stack is empty\n");
    }
)

DEF_CMD(kSub ,  2, "sub",
    StackType_t lhs = 0;
    StackType_t rhs = 0;

    POP(&rhs);
    POP(&lhs);

    PUSH(lhs - rhs);
)

DEF_CMD(kMult,  3, "mult",
    StackType_t a = 0;
    StackType_t b = 0;

    POP(&a);
    POP(&b);

    PUSH(a * b / kPrecision);
)

DEF_CMD(kDiv ,  4, "div",
    double a = 0;
    double b = 0;

    POP((StackType_t *) &b);
    POP((StackType_t *) &a);

    PUSH((a / b) * kPrecision);
)

DEF_CMD(kOut ,  5, "out",
    StackType_t a = 0;

    POP(&a);

    printf("%d\n", a / kPrecision);
)

DEF_CMD(kIn  ,  6, "in",
    StackType_t a = 0;

    scanf("%d", &a);

    PUSH(a);
)

DEF_CMD(kSqrt,  7, "sqrt",
    double a = 0;

    POP((StackType_t *) &a);

    PUSH(sqrt(a / kPrecision) * kPrecision);
)

DEF_CMD(kSin ,  8, "sin",
    double a = 0;

    POP((StackType_t *) &a);

    PUSH(sin(a / kPrecision) * kPrecision);
)

DEF_CMD(kCos ,  9, "cos",
    double a = 0;

    POP((StackType_t *) &a);

    PUSH(cos(a / kPrecision) * kPrecision);
)

DEF_CMD(kPop , 10, "pop",
    arg = codes->codes_array[++i];

    if (POP(&cpu->reg_array[arg]) != kStackClear)
    {
        printf("POP: Stack is empty\n");
    }
)

DEF_CMD(kHtl , 11, "htl",
    return kSucces;
)
