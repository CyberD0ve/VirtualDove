typedef struct CPU{
    /* General register */
    int* eax;
    int* ebx;
    int* ecx;
    int* edx;
    /* Control pointer */
    int* eip;                        //Instruction pointer
    int* esp;                        //Stack pointer
    int* flags;
} CPU;

typedef struct INFO{
    int memsize;
} INFO;

typedef struct VirtualMachine{
    INFO* info;
    /* Control Process Unit */
    CPU* CPU;
    /* Random Access Memory */
    int *RAM;
} VirtualMachine;