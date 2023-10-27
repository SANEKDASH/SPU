#ifndef COMPILER_HEADER
#define COMPILER_HEADER

#include "commands.h"

#define GET_NUM_BIT(code) GetBits(code, 1, 5)
#define GET_REG_BIT(code) GetBits(code, 1, 6)
#define GET_RAM_BIT(code) GetBits(code, 1, 7)
#define GET_OPCODE(code)  GetBits(code, 5, 0)

#define SET_NUM_BIT(code) code |= 1 << 5
#define SET_REG_BIT(code) code |= 1 << 6
#define SET_RAM_BIT(code) code |= 1 << 7



size_t GetFileSize(FILE *ptr_file);

CompileErr_t ReadTextFromFile(Text *text,
                             const char* file_name);

size_t SplitBufIntoWords(char *buf);

void FillText(Text *text);

void TextDtor(Text *text);

void PrintTextInFile(FILE *output_file,
                     Text *text);



CommandCode_t SeekCommand(const char *word);

ArgCode_t SeekRegister(const char *token);

CompileErr_t CompileText(Text *text,
                        const char *file_name);

CompileErr_t WriteInBin(const char *file_name,
                       Code *codes);

char EncodeText(Text *text,
                LabelArray *labels,
                Code *codes);

int GetLabelIp(char *token,
               LabelArray *labels);

char DecodeLine(FILE *output_file,
                char *line);

CompileErr_t GetArgument(char *token,
                        StackType_t *op_code,
                        StackType_t *num_arg,
                        StackType_t *reg_arg,
                        LabelArray *labels,
                        size_t line_number);

CompileErr_t ParseLine(char *line,
                      StackType_t *op_code,
                      StackType_t *num_arg,
                      StackType_t *reg_arg,
                      LabelArray *labels,
                      size_t line_number);

size_t GetTokenNumber(Text *text,
                      LabelArray *labels);

CompileErr_t WriteInTxt(const char *file_name,
                       Code *codes);

size_t GetBits(int num,
               size_t bit_count,
               size_t bit_pos);

#endif
