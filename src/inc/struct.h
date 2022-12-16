/* Trap Set */
#define TRAP_EXIT                   0x00
#define TRAP_BREAKPOINT             0x3C

/* Instruction Set */
#define INSTRUCTION_MOV             0x01
#define INSTRUCTION_MOVR            0x02
#define INSTRUCTION_ADD             0x03
#define INSTRUCTION_SUB             0x04
#define INSTRUCTION_CMP             0x05
#define INSTRUCTION_JMP             0x06
#define INSTRUCTION_SUBR            0x07
#define INSTRUCTION_JZ              0x09
#define INSTRUCTION_JNZ             0x10

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
    int running;
    int memsize;
} INFO;

typedef struct VirtualMachine{
    INFO* info;
    /* Control Process Unit */
    CPU* CPU;
    /* Random Access Memory */
    int *RAM;
} VirtualMachine;

