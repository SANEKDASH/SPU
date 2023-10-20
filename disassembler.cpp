#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "stack.h"
#include "commands.h"
#include "compiler.h"
#include "disassembler.h"

CompError_t DisassembleText(Text* text, const char *file_name)
{
    FILE *output_file = nullptr;
    output_file = fopen(file_name, "w");

    if (output_file == nullptr)
    {
        perror("DisambleText() failed");

        return kOpenError;
    }

    for (size_t i = 0; i < text->lines_count; ++i)
    {
        DecodeLine(output_file, text->lines_ptr[i]);
    }

    fclose(output_file);
}

CompError_t ReadCode(Code *codes, const char *file_name)
{
    FILE *input_file = fopen(file_name, "rb");
    //check opening


    codes->capacity = GetFileSize(input_file) / sizeof(StackType_t);

    codes->codes_array = (StackType_t *) calloc(codes->capacity, sizeof(StackType_t));
    fread(codes->codes_array, sizeof(StackType_t), codes->capacity, input_file);


    //check close
    fclose(input_file);
}

CompError_t DisassembleBinFile(const char *input_file_name, const char *output_file_name)
{
    Code codes = {};

    ReadCode(&codes, input_file_name);

    FILE *output_file = fopen(output_file_name, "w");

    for (size_t i = 0; i < codes.capacity; ++i)
    {
        char op_code = GetBits(codes.codes_array[i], 4, 0);

        if (GetBits(codes.codes_array[i], 1, 4))//constants on bits positions
        {
            i++;

            fprintf(output_file, "%s %d\n", CommandArray[op_code].command_name, codes.codes_array[i]);
        }
        else if (GetBits(codes.codes_array[i], 1, 5))//constants on bits positions
        {
            size_t reg_code = codes.codes_array[++i];

            fprintf(output_file, "%s %s\n", CommandArray[op_code].command_name, RegisterArray[reg_code].reg_name);
        }
        else
        {
            fprintf(output_file, "%s\n", CommandArray[op_code].command_name);
        }
    }

    free(codes.codes_array);

    fclose(output_file);//check
}
