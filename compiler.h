#ifndef COMPILER_HEADER
#define COMPILER_HEADER

#include "commands.h"

long long GetFileSize(FILE *ptr_file);

CompError_t ReadTextFromFile(Text *text, const char* file_name);

int SplitBufIntoWords(char *buf);

void FillText(Text *text);

void TextDtor(Text *text);

void PrintTextInFile(FILE *output_file, Text *text);


CommandCode_t SeekCommand(const char *word);

ArgCode_t SeekRegister(const char *token);

CompError_t CompileText(Text *text, const char *file_name);

CompError_t WriteInBin(const char *file_name, Code *codes);

char EncodeText(Text *text, LabelArray *labels, StackType_t *codes);

int GetLabelIp(char *token, LabelArray *labels);

char DecodeLine(FILE *output_file, char *line);

size_t GetTokenNumber(Text *text, LabelArray *labels);

CompError_t WriteInTxt(const char *file_name, Code *codes);

size_t GetBits(int num, size_t bit_count, size_t bit_pos);

#endif
