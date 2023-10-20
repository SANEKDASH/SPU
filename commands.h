#ifndef COMMANDS_HEADER
#define COMMANDS_HEADER

#include "stack.h"

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
    kSucces     = 0,
    kOpenError  = 1,
    kAllocError = 2,
    kFreeError  = 3,
    kCloseError = 4,
} CompError_t;

typedef enum
{
    #define DEF_CMD(name, num, ...) name = num,

    #include "code_gen.h"

    #undef DEF_CMD
    kJmp,
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
    long long buf_size;
};

static const size_t kRegCount = 4;

struct CPU
{
    Stack stack;
    StackType_t reg_array[kRegCount];
};

struct Command
{
    CommandCode_t command_code;
    const char *command_name;
};

struct Code
{
    StackType_t *codes_array = nullptr;
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

static const int kCommandCount = 12;

const Command CommandArray[kCommandCount] =
{
#define DEF_CMD(const, num, str, ...) const, str,

#include "code_gen.h"

#undef DEF_CMD
};


const Register RegisterArray[kRegCount] =
{
    kRegA, "rax",
    kRegB, "rbx",
    kRegC, "rcx",
    kRegD, "rdx",
};

#endif
