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
    kSuccess        = 0,
    kOpenError      = 1,
    kAllocError     = 2,
    kFreeError      = 3,
    kCloseError     = 4,
    kMissingLabel   = 5,
    kWrongCommand   = 6,
    kUnclosedBraket = 7,
    kFoundLabel     = 8,
    kCpmpileError   = 9,
} CompileErr_t;

typedef enum
{
    #define DEF_CMD(name, num, ...) name = num,

    #include "code_gen.h"
    #include "jumps.h"

    #undef DEF_CMD
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

static const size_t kRegCount   = 4;
static const size_t kMaxRamSize = 200;

struct CPU
{
    Stack stack;
    StackType_t reg_array[kRegCount];
    StackType_t RAM[kMaxRamSize] = {0};
};

struct Command
{
    CommandCode_t command_code;
    const char *command_name;
    bool have_arg;
};

struct Code
{
    StackType_t *codes_array = nullptr;
    size_t capacity = 0;
    size_t size     = 0;
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

static const int kCommandCount = 22;

const Command CommandArray[kCommandCount] =
{
#define DEF_CMD(const, num, str, have_arg,  ...) const, str, have_arg,

#include "code_gen.h"
#include "jumps.h"

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
