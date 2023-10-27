#ifndef EXECUTOR
#define EXECUTOR

CompileErr_t ExecuteCommands(CPU *cpu, Code *codes);

CommandCode_t SeekByteCommand(const int code);

#endif
