#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "stack.h"
#include "commands.h"
#include "assembler.h"
#include "disassembler.h"

CompileErr_t DisassembleText(Text* text, const char *file_name)
{
    FILE *output_file = fopen(file_name, "w");

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

CompileErr_t ReadCode(Code *codes, const char *file_name)
{
    FILE *input_file = fopen(file_name, "rb");
    if (!input_file)
    {
        perror("ReadCode() failed to open input file");

        return kOpenError;
    }

    codes->capacity = codes->size = GetFileSize(input_file) / sizeof(StackType_t);

    codes->codes_array = (StackType_t *) calloc(codes->capacity, sizeof(StackType_t));
    fread(codes->codes_array, sizeof(StackType_t), codes->capacity, input_file);

    if (fclose(input_file) == EOF)
    {
        perror("ReadCode() failed to close input file");

        return kCloseError;
    }

}

CompileErr_t DisassembleBinFile(const char *input_file_name, const char *output_file_name)
{
    Code codes = {};

    ReadCode(&codes, input_file_name);
    FILE *output_file = fopen(output_file_name, "w");

    for (size_t i = 0; i < codes.capacity; ++i)
    {
        char op_code = GET_OPCODE(codes.codes_array[i]);

        if (GET_NUM_BIT(codes.codes_array[i]))//constants on bits positions
        {
            i++;

            fprintf(output_file, "%s %d\n", CommandArray[op_code].command_name, codes.codes_array[i]);
        }
        else if (GET_REG_BIT(codes.codes_array[i]))//constants on bits positions
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
