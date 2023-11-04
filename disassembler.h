#ifndef DISAMBLER_HEADER
#define DISAMBLER_HEADER

CompileErr_t DisassembleBinFile(const char *input_file_name,
                                const char *output_file_name);

CompileErr_t ReadCode(Code *code,
                      const char *file_name);

#endif
