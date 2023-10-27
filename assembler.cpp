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

    free(text->dirty_buf);
    text->dirty_buf = nullptr;


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
    text->dirty_buf = (char *) calloc(text->buf_size, sizeof(char));

    if (!text->buf)
    {
        perror("\n>>ReadTextFromFile() failed to allocate memory for buf");

        return kAllocError;
    }

    if (!text->dirty_buf)
    {
        perror("\n>>ReadTextFromFile() failed to allocate memory for dirty_buf");

        return kAllocError;
    }

    long long check_read = fread(text->buf, sizeof(char), text->buf_size - 1, input_file);
    strcpy(text->dirty_buf, text->buf);

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

CompileErr_t GetArgument(char *token,
                        StackType_t *op_code,
                        StackType_t *num_arg,
                        StackType_t *reg_arg,
                        LabelArray *labels,
                        size_t line_number)
{
    size_t code = 0;

    if (*op_code >= kCall && *op_code <= kJne)
    {
        *num_arg = GetLabelIp(token, labels);

        if (*num_arg == -1)
        {
            printf("\n>>misiing label: '%s'! LINE: %d!\n", token, line_number + 1);

            return kMissingLabel;
        }

        SET_NUM_BIT(*op_code);
        return kSuccess;//sakes
    }

    if (*token == '[')
    {
        SET_RAM_BIT(*op_code);
        char *num = nullptr;
        char *reg = nullptr;
        //this need to bee another function
        ++token;
        reg = token;

        while(*token != '+')
        {
            if (*token == ']')
            {
                *token = '\0';
                *reg_arg = SeekRegister(reg);

                if (*reg_arg == kEmmNum)
                {
                    *num_arg = atoi(reg);

                    SET_NUM_BIT(*op_code);

                    return kSuccess;
                }

                SET_REG_BIT(*op_code);

                return kSuccess;
            }
            ++token;
        }

        *(token++) = '\0';

        num = token;

        while (*token != ']')
        {
            if (*token == '\0')
            {
                printf(">>unclosed breket! LINE: %d!\n", line_number);
            }
            ++token;
        }

        *token = '\0';
        //end of needing


        if (num_arg != nullptr)
        {
            //if (*num_arg != '\0')
            //{
            *num_arg = atof(num);
            SET_NUM_BIT(*op_code);
            //}
        }

        if (reg_arg != nullptr)
        {
            //if (*reg_arg != '\0')
            //{

            *reg_arg = SeekRegister(reg);
            SET_REG_BIT(*op_code);
            //}
        }

    }
    else if ((code = SeekRegister(token)) != kEmmNum)
    {
        *reg_arg = SeekRegister(token);
        SET_REG_BIT(*op_code);
    }
    else
    {
        *num_arg = atof(token);

        SET_NUM_BIT(*op_code);
    }
    return kSuccess;
}

CompileErr_t ParseLine(char *line,
                      StackType_t *op_code,
                      StackType_t *num_arg,
                      StackType_t *reg_arg,
                      LabelArray *labels,
                      size_t line_number)
{
    char *token = strtok(line, " \n\r");

    size_t code = SeekCommand(token);

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
        token = strtok(NULL, " ");

        if (token)
        {
            GetArgument(token,
                        op_code,
                        num_arg,
                        reg_arg,
                        labels,
                        line_number);
        }
        else
        {
            printf("\n>>command must have argument, line: %d!\n", line_number);
        }
    }
    return kSuccess;
}

char EncodeText(Text *text, LabelArray *labels, Code *codes)
{
    char *token = nullptr;

    StackType_t  op_code = 0;
    StackType_t  num_arg = 0;
    StackType_t  reg_arg = 0;

    size_t ip = 0;

    for (size_t i = 0; i < text->lines_count; i++)
    {
        op_code = 0;
        num_arg = 0;
        reg_arg = 0;

        if (ParseLine(text->lines_ptr[i],
                      &op_code,
                      &num_arg,
                      &reg_arg,
                      labels,
                      i) == kFoundLabel)
        {
            continue;
            //check compile errors
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

        fprintf(output_file, "%s %s\n", CommandArray[op_code].command_name,
                                        RegisterArray[atoi(token)].reg_name);
    }
    else
    {
        fprintf(output_file, "%s\n", CommandArray[code].command_name);
    }

    return code;
}

CompileErr_t CompileText(Text *text, const char *file_name)
{
    Code codes = {};
    LabelArray labels = {};
    codes.capacity = GetTokenNumber(text, &labels);

    codes.codes_array = (StackType_t *) calloc(codes.capacity, sizeof(StackType_t));

    if (codes.codes_array == nullptr)
    {
        perror("\n>>CompileText() failed to allocate memory with 'calloc'\n");

        return kAllocError;
    }

    EncodeText(text, &labels, &codes);

    WriteInTxt(file_name, &codes);

    WriteInBin("output.bin", &codes);

    free(codes.codes_array);//

    return kSuccess;
}

size_t GetTokenNumber(Text *text, LabelArray *labels)
{
    size_t token_number = 0;
    char *token = nullptr;

    const char *delim = " \n\r";

    token = strtok(text->dirty_buf, delim);

    while (token)
    {
        if (SeekCommand(token) == kLabel)
        {
            *(token + strlen(token) - 1) = '\0';

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

    fprintf(output_file, "\t\t\t\t\t\tSPU listing by Icebergold\n");
    fprintf(output_file, "+------------+------------+------------+------------+------------+------------+------------+\n"
                         "|  op name   |   opcode   | hex opcode |reg argument|  reg code  |num argument|hex argument|\n"
                         "+------------+------------+------------+------------+------------+------------+------------+\n");
    for (size_t i = 0; i < codes->size; i++)
    {
        StackType_t code = codes->codes_array[i];

        fprintf(output_file, "|%-12s|%-12d|0x%-10X|",
                                 CommandArray[GET_OPCODE(codes->codes_array[i])].command_name,
                                 codes->codes_array[i],
                                 codes->codes_array[i]);
        if (GET_REG_BIT(code))
        {
            ++i;
            fprintf(output_file, "%-12s|%-12d|", RegisterArray[codes->codes_array[i]].reg_name, codes->codes_array[i]);
        }
        else
        {
            fprintf(output_file, "------------|------------|");
        }

        if (GET_NUM_BIT(code))
        {
            ++i;
            fprintf(output_file, "%-12d|0x%-8X  |\n",
                                 codes->codes_array[i],
                                 codes->codes_array[i]);
        }
        else
        {
            fprintf(output_file, "------------|------------|\n");
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

CompileErr_t WriteInBin(const char *file_name, Code *codes)
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

}

