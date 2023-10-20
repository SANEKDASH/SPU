#ifndef DISAMBLER_HEADER
#define DISAMBLER_HEADER

CompError_t DisassembleText(Text *text, const char *file_name);

CompError_t DisassembleBinFile(const char *input_file_name, const char *output_file_name);

CompError_t ReadCode(Code *code, const char *file_name);

#endif
