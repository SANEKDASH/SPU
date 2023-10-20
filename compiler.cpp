#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

#include "commands.h"
#include "color_print.h"
#include "compiler.h"
#include "debug.h"
#include "stack.h"


#define GET_NUM_BIT(code) GetBits(code, 1, 5)
#define GET_REG_BIT(code) GetBits(code, 1, 6)
#define GET_RAM_BIT(code) GetBits(code, 1, 7)
#define GET_OPCODE(code)  GetBits(code, 5, 0)

#define SET_NUM_BIT(code) code |= 1 << 5
#define SET_REG_BIT(code) code |= 1 << 6
#define SET_RAM_BIT(code) code |= 1 << 7


void TextDtor(Text *text)
{
    CHECK(text);

    free(text->buf);
    free(text->lines_ptr);

    text->lines_ptr = nullptr;
    text->buf = nullptr;

    text->lines_count = 0;
}

long long GetFileSize(FILE *ptr_file)
{
    CHECK(ptr_file);

    struct _stat file_info = {};

    _fstat(fileno(ptr_file), &file_info);

    return  file_info.st_size;
}

CompError_t ReadTextFromFile(Text *text, const char *file_name)
{
    FILE *input_file = fopen(file_name, "rb");

    if (input_file == nullptr)
    {
        perror("ReadTextFromFile() failed to open input file\n");

        return kOpenError;
    }

    text->buf_size = GetFileSize(input_file) + 1;
    text->buf       = (char *) calloc(text->buf_size, sizeof(char));
    text->dirty_buf = (char *) calloc(text->buf_size, sizeof(char));

    if (!text->buf)
    {
        perror(">>ReadTextFromFile() failed to allocate memory for buf");

        return kAllocError;
    }

    if (!text->dirty_buf)
    {
        perror(">>ReadTextFromFile() failed to allocate memory for dirty_buf");

        return kAllocError;
    }

    long long check_read = fread(text->buf, sizeof(char), text->buf_size - 1, input_file);
    strcpy(text->dirty_buf, text->buf);

    text->lines_count = SplitBufIntoWords(text->buf);

    FillText(text);

    if (fclose(input_file))
    {
        perror(">>ReadTextFromFile() failed to close input file");

        return kCloseError;
    }

    return kSucces;
}

int SplitBufIntoWords(char *buf)
{
    CHECK(buf);

    int lines_count = 0;

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

// Parse?
CommandCode_t SeekCommand(const char *word)
{
    for (size_t i = 0; i < kCommandCount; ++i)
    {
        if (strcmp(word, CommandArray[i].command_name) == 0)
        {
            return CommandArray[i].command_code;
        }
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

// it's possible to add error checker
char EncodeText(Text *text, LabelArray *labels, StackType_t *codes)
{
    char *token = nullptr;

    char op_code  = 0;
    char code     = 0;
    int  arg_code = 0;

    size_t label_count = 0;
    size_t ip = 0;

    for (size_t i = 0; i < text->lines_count; i++)
    {
        op_code  = 0;
        code     = 0;
        arg_code = 0;

        token = strtok(text->lines_ptr[i], " ");

        if ((code = SeekCommand(token) == kNotACommand) && *(token + strlen(token) - 1) == ':')
        {
            continue;
        }

        while (token != nullptr)
        {
            if ((code = SeekCommand(token))!= kNotACommand)
            {
                op_code |= code;
            }
            else if ((code = SeekRegister(token)) != kEmmNum)
            {
                SET_REG_BIT(op_code);

                arg_code = code;
            }
            else
            {
                SET_NUM_BIT(op_code);

                arg_code = atoi(token);
            }
            token = strtok(NULL, " ");
        }

        if (!GET_NUM_BIT(op_code) && !GET_REG_BIT(op_code))
        {
            codes[ip++] = op_code;
        }
        else
        {
            codes[ip++] = op_code;
            codes[ip++] = arg_code;
        }
    }

    return op_code;
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

        fprintf(output_file, "%s %s\n", CommandArray[op_code].command_name, RegisterArray[atoi(token)].reg_name);
    }
    else
    {
        fprintf(output_file, "%s\n", CommandArray[code].command_name);
    }

    return code;
}

CompError_t CompileText(Text *text, const char *file_name)
{
    Code codes = {};
    LabelArray labels = {};

    codes.capacity = GetTokenNumber(text, &labels);

    codes.codes_array = (StackType_t *) calloc(codes.capacity, sizeof(StackType_t));

    if (codes.codes_array == nullptr)
    {
        perror(">>CompileText() failed to allocate memory with 'calloc'\n");

        return kAllocError;
    }

    EncodeText(text, &labels, codes.codes_array);

    WriteInTxt(file_name, &codes);

    WriteInBin("output.bin", &codes);

    free(codes.codes_array);//

    return kSucces;
}

size_t GetTokenNumber(Text *text, LabelArray *labels)
{
    size_t token_number = 0;
    char *token = nullptr;

    const char *delim = " \n\r";

    token = strtok(text->dirty_buf, delim);

    while (token)
    {
        if (SeekCommand(token) == kNotACommand && *(token + strlen(token) - 1) == ':')
        {
            *(token + strlen(token) - 1) = '\0';
            labels->array[labels->label_count].ip = token_number + 1;
            labels->array[labels->label_count++].label_name = token;
        }
        else
        {
            ++token_number;
        }
        token = strtok(NULL, delim);
    }

    for (size_t i = 0; i < labels->label_count; ++i)
    {
        printf("label %s, ip = %d\n", labels->array[i].label_name,
                                      labels->array[i].ip);
    }
    free(text->dirty_buf);//

    return token_number;
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

    printf(">>error: missing label");

    return -1;
}

CompError_t WriteInTxt(const char *file_name, Code *codes)
{
    FILE *output_file = fopen(file_name, "w");

    if (!output_file)
    {
        perror(">>WriteInTxt() failed to open output file\n");

        return kOpenError;
    }

    for (size_t i = 0; i < codes->capacity; i++)
    {
        if (!GET_NUM_BIT(codes->codes_array[i]) && !GET_REG_BIT(codes->codes_array[i]))
        {
            fprintf(output_file, "%d\n", codes->codes_array[i]);
        }
        else
        {
            fprintf(output_file, "%d %d\n", codes->codes_array[i], codes->codes_array[i + 1]);
            ++i;
        }
    }

    if (fclose(output_file) == EOF)
    {
        perror(">>WriteInTxt() failed to close output file\n");
        return kCloseError;
    }

    return kSucces;
}

CompError_t WriteInBin(const char *file_name, Code *codes)
{
    FILE *output_file = fopen(file_name, "wb");

    if (output_file == nullptr)
    {
        perror(">>WriteInBin() error with opening output file");

        return kOpenError;
    }

    fwrite(codes->codes_array, sizeof(codes->codes_array[0]), codes->capacity, output_file);

    if (fclose(output_file) == EOF)
    {
        perror(">>WriteInBin() error with closing output file");

        return kCloseError;
    }

}

