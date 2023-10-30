#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

#include "commands.h"
#include "color_print.h"
#include "assembler.h"
#include "debug.h"
#include "stack.h"



void TextDtor(Text *text)
{
    CHECK(text);

    free(text->buf);
    text->buf = nullptr;

    free(text->lines_ptr);
    text->lines_ptr = nullptr;

    text->lines_count = 0;
}



size_t GetFileSize(FILE *ptr_file)
{
    CHECK(ptr_file);

    struct _stat file_info = {};

    _fstat(fileno(ptr_file), &file_info);

    return  file_info.st_size;
}



CompileErr_t ReadTextFromFile(Text *text, const char *file_name)
{
    FILE *input_file = fopen(file_name, "rb");

    if (input_file == nullptr)
    {
        perror("\nReadTextFromFile() failed to open input file\n");

        return kOpenError;
    }
    text->buf_size = GetFileSize(input_file) + 1;
    text->buf       = (char *) calloc(text->buf_size, sizeof(char));

    if (!text->buf)
    {
        perror("\n>>ReadTextFromFile() failed to allocate memory for buf");

        return kAllocError;
    }

    size_t check_read = fread(text->buf, sizeof(char), text->buf_size - 1, input_file);
    if (check_read != (text->buf_size - 1))
    {
        perror("\n>>ReadTextFromFile() failed to read from file");

        return kReadingError;
    }

    text->lines_count = SplitBufIntoWords(text->buf);

    FillText(text);

    if (fclose(input_file))
    {
        perror("\n>>ReadTextFromFile() failed to close input file");

        return kCloseError;
    }

    return kSuccess;
}



size_t SplitBufIntoWords(char *buf)
{
    CHECK(buf);

    size_t lines_count = 0;

    while (*buf != '\0')
    {
        if ((*buf) == '\n' || (*buf) == '\r')
        {
            while (*buf == '\n' || *buf == '\r')
            {
                *(buf++) = '\0';
            }

        }
        else
        {
            while ((*buf) != '\n' && *buf != '\0' && *buf != '\r')
            {
                ++buf;
            }

            ++lines_count;
        }
    }

    return lines_count;
}



void FillText(Text *text)
{
    CHECK(text);

    text->lines_ptr = (char **) calloc(text->lines_count, sizeof(char *));

    char *cur_word = text->buf;

    size_t i = 0;

    size_t words_pos = 0;

    while (i < text->buf_size)
    {
        if (*(text->buf + i) == '\0')
        {
            while (*(text->buf + i) == '\0')
            {
                ++i;
            }
            *(text->lines_ptr + words_pos++) = cur_word;

            cur_word = text->buf + i;
        }

        ++i;
    }

}



void PrintTextInFile(FILE *output_file, Text *text)
{
    for (size_t i = 0; i < text->lines_count; i++)
    {
        fprintf(output_file, "[%s]\n", *(text->lines_ptr + i));
    }
}






CommandCode_t SeekCommand(const char *word)
{

    for (size_t i = 0; i < kCommandCount; ++i)
    {
        if (strcmp(word, CommandArray[i].command_name) == 0)
        {
            return CommandArray[i].command_code;
        }
    }

    if (*(word + strlen(word) - 1) == ':')
    {
        return kLabel;
    }

    return kNotACommand; // ?

}



ArgCode_t SeekRegister(const char *token)
{
    for (size_t i = 0; i < kRegCount; i++)
    {
        if (strcmp(token, RegisterArray[i].reg_name) == 0)
        {
            return RegisterArray[i].reg_code;
        }
    }

    return kEmmNum;
}



static bool IsJmpOpCode(StackElemType_t op_code)
{
    return op_code == kCall ||
           op_code == kJmp  ||
           op_code == kJa   ||
           op_code == kJae  ||
           op_code == kJb   ||
           op_code == kJbe  ||
           op_code == kJe   ||
           op_code == kJne;
}

static CompileErr_t ParseSquareBrakets(char *line,
                                       char **num_str,
                                       char **reg_str,
                                       StackElemType_t *num_arg,
                                       StackElemType_t *reg_arg,
                                       StackElemType_t *op_code,
                                       size_t line_number)
{

    *reg_str = ++line;

    while (*line != '+' && *line != ']' && *line != '\0')
    {
        ++line;
    }

    switch (*line)
    {
        case '+':
        {
            *(line++) = '\0';

            *num_str = line;
    printf("num - %s\n", *num_str);

            break;
        }
        case ']':
        {

            *line = '\0';
            if (SeekRegister(*reg_str) != kEmmNum)
            {
                *reg_arg = SeekRegister(*reg_str);

                SET_REG_BIT(*op_code);
            }
            else
            {
                *num_str = *reg_str;
                *reg_str = nullptr;
                *num_arg = atoi(*num_str);

                SET_NUM_BIT(*op_code);
            }

            return kSuccess;

            break;
        }

        case '\0':
        {
            printf(">>Unclosed brakets! LINE: %d.\n", line_number);

            return kUnclosedBraket;
        }

        default:
        {
            printf("kavo?\n");

            break;
        }
    }

    while (*line != ']')
    {
        if (*line == '\0')
        {
            printf(">>unclosed braket! LINE : %d!\n", line_number);

            return kUnclosedBraket;
        }
        ++line;
    }
    *line = '\0';
    return kSuccess;
}

CompileErr_t GetArgument(char *token,
                         StackElemType_t *op_code,
                         StackElemType_t *num_arg,
                         StackElemType_t *reg_arg,
                         LabelArray *labels,
                         size_t line_number)
{
    size_t code = 0;
    if (IsJmpOpCode(*op_code))
    {
        *num_arg = GetLabelIp(token, labels);

        if (*num_arg == -1)
        {
            printf("\n>>misiing label: '%s'! LINE: %d!\n", token, line_number + 1);

            return kMissingLabel;
        }

        SET_NUM_BIT(*op_code);
        return kSuccess;
    }

    char *num = nullptr;
    char *reg = nullptr;

    if (*token == '[')
    {
        SET_RAM_BIT(*op_code);

        ParseSquareBrakets(token, &num, &reg, num_arg, reg_arg, op_code, line_number);

        if (reg != nullptr)
        {
            if (*reg == '\0')
            {
                printf(">>Incorect input! LINE: %d.\n", line_number);

                return kIncorrectInput;
            }
            CHECK(reg_arg);
            *reg_arg = SeekRegister(reg);
            SET_REG_BIT(*op_code);

        }
        if (num != nullptr)
        {

            if (*num == '\0')
            {
                printf(">>Incorect input! LINE: %d.\n", line_number);

                return kIncorrectInput;
            }
            CHECK(num_arg);
            *num_arg = atoi(num);
            SET_NUM_BIT(*op_code);

        }

        return kSuccess;
    }
    else if ((code = SeekRegister(token)) != kEmmNum)
    {
        *reg_arg = SeekRegister(token);

        SET_REG_BIT(*op_code);
    }
    else
    {
        *num_arg = atoi(token);

        SET_NUM_BIT(*op_code);
    }
    return kSuccess;
}

static void SkipSpaces(char **line)
{
    while ((**line == ' ' || **line == '\t') && **line != '\0')
    {
        ++(*line);
    }
}

void GetCommandAndArgsFromStr(char *line, char **command, char **args)
{
    SkipSpaces(&line);

    if (*line != '\0')
    {
        *command = line;
    }

    while (*line != ' ' && *line != '\t' && *line != '\0')
    {
        ++line;
    }

    *line++ = '\0';

    SkipSpaces(&line);

    if (*line != '\0')
    {
        *args = line;
    }

}

CompileErr_t ParseLine(char *line,
                       StackElemType_t *op_code,
                       StackElemType_t *num_arg,
                       StackElemType_t *reg_arg,
                       LabelArray *labels,
                       size_t line_number)
{
    char *comment = strchr(line, ';');

    if (comment != nullptr)
    {
        *comment = '\0';
    }

    char *command = nullptr;
    char *args    = nullptr;


    GetCommandAndArgsFromStr(line, &command, &args);

    size_t code = SeekCommand(command);

    if (code != kNotACommand && code != kLabel)
    {
        *op_code |= code;
    }
    else if (code == kLabel)
    {
        return kFoundLabel;
    }

    if (CommandArray[code].have_arg)
    {
        if (args)
        {
            GetArgument(args, op_code, num_arg, reg_arg, labels, line_number);
        }
        else
        {
            printf("\n>>command must have argument, line: %d!\n", line_number);
        }
    }
    return kSuccess;
}



CompileErr_t EncodeText(Text *text, LabelArray *labels, Code *codes)
{
    StackElemType_t  op_code = 0;
    StackElemType_t  num_arg = 0;
    StackElemType_t  reg_arg = 0;

    for (size_t i = 0; i < text->lines_count; i++)
    {
        char *comment = nullptr;
        if ((comment = strchr(text->lines_ptr[i], ';')) != nullptr)
        {
            *comment = '\0';
        }

        op_code = 0;
        num_arg = 0;
        reg_arg = 0;

        CompileErr_t status = kSuccess;

        if ((status = ParseLine(text->lines_ptr[i],
                                &op_code,
                                &num_arg,
                                &reg_arg,
                                labels,
                                i)) == kFoundLabel)
        {
            continue;
        }

        if (status != kSuccess)
        {
            return status;
        }

        codes->codes_array[codes->size++] = op_code;

        if (GET_REG_BIT(op_code))
        {
            codes->codes_array[codes->size++] = reg_arg;
        }
        if (GET_NUM_BIT(op_code))
        {
            codes->codes_array[codes->size++] = num_arg;
        }
    }

    return kSuccess;
}



size_t GetBits(int num, size_t bit_count, size_t bit_pos)
{
    return num & ((~((~0) << bit_count)) << bit_pos);
}



char DecodeLine(FILE *output_file, char *line)
{
    char *token = nullptr;
    token = strtok(line, " ");

    char code = atoi(token);

    char op_code = GET_OPCODE(code);

    if (GET_NUM_BIT(code))
    {
        token = strtok(NULL, " ");

        fprintf(output_file, "%s %s\n", CommandArray[op_code].command_name, token);
    }
    else if (GET_REG_BIT(code))
    {
        token = strtok(NULL, " ");

        fprintf(output_file, "%s %s\n", CommandArray[op_code].command_name,
                                        RegisterArray[atoi(token)].reg_name);
    }
    else
    {
        fprintf(output_file, "%s\n", CommandArray[code].command_name);
    }

    return code;
}



CompileErr_t CompileText(Text *text, Text *text_copy ,const char *file_name)
{
    LabelArray labels = {};
    InitLabelArray(&labels);

    Code codes = {};

    codes.capacity = GetTokenNumber(text_copy, &labels);
    codes.codes_array = (StackElemType_t *) calloc(codes.capacity, sizeof(StackElemType_t));

    if (codes.codes_array == nullptr)
    {
        perror("\n>>CompileText() failed to allocate memory with 'calloc'\n");

        return kAllocError;
    }
    if (EncodeText(text, &labels, &codes) != kSuccess)
    {
        printf(">>Compile error...\n");
    }

    WriteInTxt(file_name, &codes);

    WriteInBin("output.bin", &codes);
    WriteListing("listing.txt", &codes);

    LabelArrayDtor(&labels);
    CodeDtor(&codes);

    return kSuccess;
}


CompileErr_t CodeDtor(Code *codes)
{
    free(codes->codes_array);

    return kSuccess;
}


size_t GetTokenNumber(Text *text_copy, LabelArray *labels)
{
    size_t token_number = 0;
    char *token = nullptr;

    const char *delim = " \n\r";

    for (size_t i = 0; i < text_copy->lines_count; i++)
    {
        char *comment = strchr(text_copy->lines_ptr[i], ';');

        if (comment != nullptr)
        {
            *comment = '\0';
        }

        token = strtok(text_copy->lines_ptr[i], delim);

        while (token)
        {
            if (SeekCommand(token) == kLabel)
            {
                *(token + strlen(token) - 1) = '\0';

                if (labels->label_count == labels->capacity)
                {
                    ReallocLabelArray(labels);
                }
                labels->array[labels->label_count].label_name = token;
                labels->array[labels->label_count].ip = token_number;

                ++labels->label_count;
            }
            else
            {
                token_number++;
            }

            token = strtok(NULL, delim);
        }
    }

    return token_number;
}

static const size_t kMaxLabelCount = 20;

CompileErr_t InitLabelArray(LabelArray *labels)
{
    labels->capacity = kMaxLabelCount;
    labels->array = (Label *) calloc(labels->capacity, sizeof(Label));

    if (labels->array == nullptr)
    {
        perror("InitLabelArray() failed to allocate memory");

        return kAllocError;
    }

    labels->label_count = 0;

    return kSuccess;
}

CompileErr_t ReallocLabelArray(LabelArray *labels)
{
    labels->capacity *= 2;

    labels->array = (Label *) realloc(labels->array, labels->capacity);

    if (labels->array == nullptr)
    {
        perror("ReallocLabelArray() failed to reallocate memory");

        return kReallocError;
    }

    return kSuccess;
}

CompileErr_t LabelArrayDtor(LabelArray *labels)
{
    free(labels->array);

    return kSuccess;
}

int GetLabelIp(char *token, LabelArray *labels)
{
    for (size_t i = 0; i < labels->label_count; i++)
    {
        if (strcmp(token, labels->array[i].label_name) == 0)
        {
            return labels->array[i].ip;
        }
    }

    return -1;
}



CompileErr_t WriteInTxt(const char *file_name, Code *codes)
{
    FILE *output_file = fopen(file_name, "w");

    if (!output_file)
    {
        perror("\n>>WriteInTxt() failed to open output file\n");

        return kOpenError;
    }

    fprintf(output_file, "\t\t\t\t\t\t\tSPU listing by SANEKDASH\n");
    fprintf(output_file, "+------------+------------+------------+------------+------------+------------+------------+\n"
                         "|  op name   |   opcode   | hex opcode |reg argument|  reg code  |num argument|hex argument|\n"
                         "+------------+------------+------------+------------+------------+------------+------------+\n");
    for (size_t i = 0; i < codes->size; i++)
    {
        StackElemType_t code = codes->codes_array[i];

        fprintf(output_file, "|%-6s(%4d)|%-12d|0x%-10X|",
                CommandArray[GET_OPCODE(codes->codes_array[i])].command_name,
                i,
                codes->codes_array[i],
                codes->codes_array[i]);
        if (GET_REG_BIT(code))
        {
            ++i;
            fprintf(output_file, "%-6s(%-4d)|%-12d|",
                    RegisterArray[codes->codes_array[i]].reg_name,
                    i,
                    codes->codes_array[i]);
        }
        else
        {
            fprintf(output_file, "    ----    |    ----    |");
        }

        if (GET_NUM_BIT(code))
        {
            ++i;
            fprintf(output_file, "%-6d(%-4d)|0x%-8X  |\n",
                    codes->codes_array[i],
                    i,
                    codes->codes_array[i]);
        }
        else
        {
            fprintf(output_file, "    ----    |    ----    |\n");
        }

        fprintf(output_file, "+------------+------------+------------+------------+------------+------------+------------+\n");
    }

    if (fclose(output_file) == EOF)
    {
        perror("\n>>WriteInTxt() failed to close output file\n");

        return kCloseError;
    }

    return kSuccess;
}



CompileErr_t WriteInBin(const char *file_name,
                        Code *codes)
{
    FILE *output_file = fopen(file_name, "wb");

    if (output_file == nullptr)
    {
        perror("\n>>WriteInBin() error with opening output file");

        return kOpenError;
    }

    fwrite(codes->codes_array, sizeof(codes->codes_array[0]), codes->size, output_file);

    if (fclose(output_file) == EOF)
    {
        perror("\n>>WriteInBin() error with closing output file");

        return kCloseError;
    }

    return kSuccess;
}

CompileErr_t WriteListing(const char *file_name,
                          Code *codes)
{
    FILE *output_file = fopen(file_name, "w");

    if (!output_file)
    {
        perror("\n>>WriteInTxt() failed to open output file\n");

        return kOpenError;
    }

    for (size_t i = 0; i < codes->size;)
    {
        StackElemType_t op_code  = codes->codes_array[i++];
        StackElemType_t num      = 0;
        StackElemType_t reg_code = 0;

        fprintf(output_file, "%08X ", op_code);

        if (GET_REG_BIT(op_code))
        {
            reg_code = codes->codes_array[i++];
            fprintf(output_file, "%08X ", reg_code);
        }
        if (GET_NUM_BIT(op_code))
        {
            num = codes->codes_array[i++];
            fprintf(output_file, "%08X", num);
        }

        fprintf(output_file, "\t\t\t\t%s ",
                CommandArray[GET_OPCODE(op_code)].command_name);

        if (GET_RAM_BIT(op_code))
        {
            if (GET_NUM_BIT(op_code) && GET_REG_BIT(op_code))
            {
                fprintf(output_file, "[%s + %d]",
                        RegisterArray[reg_code].reg_name,
                        num);
            }
            else if (GET_REG_BIT(op_code))
            {
                fprintf(output_file, "[%s]",
                        RegisterArray[reg_code].reg_name);
            }
            else if (GET_NUM_BIT(op_code))
            {
                fprintf(output_file, "[%d]",
                        num);
            }
        }
        else
        {
            if (GET_REG_BIT(op_code))
            {
                fprintf(output_file, "%s",
                        RegisterArray[reg_code].reg_name);
            }
            else if (GET_NUM_BIT(op_code))
            {
                fprintf(output_file, "%d",
                        num);
            }
        }

        fprintf(output_file ,"\n\n");



    }

    if (fclose(output_file) == EOF)
    {
        perror("\n>>WriteInBin() error with closing output file");

        return kCloseError;
    }

    return kSuccess;
}

