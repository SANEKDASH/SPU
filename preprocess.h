# 1 "commands.h"
# 1 "<command-line>"
# 1 "commands.h"



# 1 "stack.h" 1



struct LogInfo
{
    const char *stack_name;
    const int line;
    const char *func;
    const char *file;
    const char *date;
    const char *time;
};
# 39 "stack.h"
typedef unsigned StackErr_t;
typedef int StackElemType_t;
typedef unsigned long long CanaryType_t;
# 53 "stack.h"
struct StackData
{
    StackElemType_t *data;
    int size;
    int capacity;
    unsigned int status;
};

struct Stack
{




    StackData stack_data;
# 76 "stack.h"
};

typedef enum
{
    kStackClear = 0,
    kNullData = 1 << 0,
    kWrongPos = 1 << 1,
    kWrongSize = 1 << 2,
    kPosHigherSize = 1 << 3,
    kDestroyedStack = 1 << 4,
    kLeftCanaryScreams = 1 << 5,
    kRightCanaryScreams = 1 << 6,
    kRightDataCanaryFuck = 1 << 7,
    kLeftDataCanaryFuck = 1 << 8,
    kDataHashError = 1 << 9,
    kStructHashError = 1 << 10,

    kWrongUsingOfStack = 1 << 11,
} StackStatus;



const size_t kStatusNumber = 13;

struct Error
{
    StackStatus err_code;
    const char* error_expl;
};

const Error ErrorArray[kStatusNumber] =
{
    kStackClear , "there is no troubles with stack",
    kNullData , "data = nullptr",
    kWrongPos , "current position in stack is wrong",
    kWrongSize , "size of stack is wrong",
    kPosHigherSize , "position on stack is higher than it's size",
    kDestroyedStack , "stack is destroyed",
    kLeftCanaryScreams , "left structure canary is broken, your data is in danger",
    kRightCanaryScreams , "right structure canary is broken, your data is in danger",
    kLeftDataCanaryFuck , "left data canary is fucked, your data may be fucked up",
    kRightDataCanaryFuck, "right data canary is fucked, your data may be fucked up",
    kDataHashError , "data hash has wrong value, your data is in danger",
    kStructHashError , "structure hash has wrong value, your data is in danger",

    kWrongUsingOfStack , "stack is clear but using is wrong"
};

StackErr_t StackVerify(Stack *stk);

StackErr_t StackInit(Stack *stk);

StackErr_t StackDtor(Stack *stk);

StackErr_t Push(Stack *stk, StackElemType_t in_value);

StackErr_t Pop(Stack *stk, StackElemType_t *ret_value);

void StackDump(const Stack *stk, LogInfo info);

CanaryType_t *GetRightDataCanaryPtr(const Stack *stk);

CanaryType_t *GetLeftDataCanaryPtr(const Stack *stk);

int GetBit(int num, size_t pos);
# 5 "commands.h" 2

typedef enum
{
    kRegA = 0,
    kRegB = 1,
    kRegC = 2,
    kRegD = 3,
    kEmmNum,
} ArgCode_t;

typedef enum
{
    kSucces = 0,
    kOpenError = 1,
    kAllocError = 2,
    kFreeError = 3,
    kCloseError = 4,
    kMissingLabel ,
    kWrongCommand ,
    kFoundLabel ,
} CompError_t;

typedef enum
{


# 1 "code_gen.h" 1
kPush = 0,
# 14 "code_gen.h"
kAdd = 1,
# 23 "code_gen.h"
kSub = 2,
# 33 "code_gen.h"
kMult = 3,
# 43 "code_gen.h"
kDiv = 4,
# 53 "code_gen.h"
kOut = 5,







kIn = 6,







kSqrt = 7,







kSin = 8,







kCos = 9,







kPop = 10,
# 102 "code_gen.h"
kHtl = 11,


kCall = 12,
# 32 "commands.h" 2
# 1 "jumps.h" 1
kJmp = 13,
kJa = 14,
kJae = 15,
kJb = 16,
kJbe = 17,
kJe = 18,
kJne = 19,
# 33 "commands.h" 2


    kLabel,
    kNotACommand,
} CommandCode_t;

struct Register
{
    ArgCode_t reg_code;
    const char* reg_name;
};

struct Text
{
    char **lines_ptr;
    size_t lines_count;
    char *dirty_buf;
    char* buf;
    size_t buf_size;
};

static const size_t kRegCount = 4;

struct CPU
{
    Stack stack;
    StackElemType_t reg_array[kRegCount];
};

struct Command
{
    CommandCode_t command_code;
    const char *command_name;
};

struct Code
{
    StackElemType_t *codes_array = nullptr;
    size_t capacity = 0;
};

struct Label
{
    char *label_name;
    size_t ip;
};

static const size_t kMaxLabelCount = 20;

struct LabelArray
{
    Label array[kMaxLabelCount];
    size_t label_count;
};

static const int kCommandCount = 20;

const Command CommandArray[kCommandCount] =
{


# 1 "code_gen.h" 1
kPush, "push",
# 14 "code_gen.h"
kAdd, "add",
# 23 "code_gen.h"
kSub, "sub",
# 33 "code_gen.h"
kMult, "mult",
# 43 "code_gen.h"
kDiv, "div",
# 53 "code_gen.h"
kOut, "out",







kIn, "in",







kSqrt, "sqrt",







kSin, "sin",







kCos, "cos",







kPop, "pop",
# 102 "code_gen.h"
kHtl, "htl",


kCall, "call:",
# 95 "commands.h" 2
# 1 "jumps.h" 1
kJmp, "jmp:",
kJa, "ja:",
kJae, "jae:",
kJb, "jb:",
kJbe, "jbe:",
kJe, "je:",
kJne, "jne:",
# 96 "commands.h" 2


};


const Register RegisterArray[kRegCount] =
{
    kRegA, "rax",
    kRegB, "rbx",
    kRegC, "rcx",
    kRegD, "rdx",
};
