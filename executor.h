#ifndef EXECUTOR
#define EXECUTOR

CompError_t ExecuteCommands(CPU *cpu, Code *codes);

CommandCode_t SeekByteCommand(const int code);

#endif
