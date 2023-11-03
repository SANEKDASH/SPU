#ifndef EXECUTOR
#define EXECUTOR

CompileErr_t CpuInit(CPU *cpu);

CompileErr_t CpuDtor(CPU *cpu);

CompileErr_t ExecuteCommands(CPU *cpu, Code *codes);

CommandCode_t SeekByteCommand(const int code);

#endif
