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
    kRegA,
    kRegB,
    kRegC,
    kRegD,
    kEmmNum,
} ArgCode_t;

typedef enum
{
    kSucces = 0,
    kOpenError = 1,
    kAllocError = 2,
    kCloseError = 3,
} CompError_t;

typedef enum
{


# 1 "code_gen.h" 1
kPush = 0,
kAdd = 1,
kSub = 2,
kMult = 3,
kDiv = 4,
kOut = 5,
kIn = 6,
kSqrt = 7,
kSin = 8,
kCos = 9,
kPop = 10,
kHtl = 11,
kNotACommand = 12,
# 28 "commands.h" 2


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
    long long buf_size;
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

static const int kCommandCount = 12;

const Command CommandArray[kCommandCount] =
{
    kPush , "push",
    kAdd , "add" ,
    kSub , "sub" ,
    kMult , "mult",
    kDiv , "div" ,
    kOut , "out" ,
    kIn , "in" ,
    kSqrt , "sqrt",
    kSin , "sin" ,
    kCos , "cos" ,
    kPop , "pop" ,
    kHtl , "htl" ,
};


const Register RegisterArray[kRegCount] =
{
    kRegA, "rax",
    kRegB, "rbx",
    kRegC, "rcx",
    kRegD, "rdx",
};
