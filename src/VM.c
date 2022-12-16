#include <stdlib.h>
#include <stdio.h>
#include "inc/struct.h"


VirtualMachine* initVM(int size){
    printf("[+] Initializing Virtual Machine with %d bytes of RAM\n", size);
    VirtualMachine* VM = malloc(sizeof(VirtualMachine));

    // Load General Information
    VM->info = malloc(sizeof(INFO));
    VM->info->memsize=size;

    /* Init CPU */
    VM->CPU = malloc(sizeof(CPU));
    VM->CPU->eax=malloc(sizeof(int));
    VM->CPU->ebx=malloc(sizeof(int));
    VM->CPU->ecx=malloc(sizeof(int));
    VM->CPU->edx=malloc(sizeof(int));
    VM->CPU->eip=malloc(sizeof(int));
    VM->CPU->esp=malloc(sizeof(int));
    VM->CPU->flags=malloc(sizeof(int));

    /* Init RAM */
    VM->RAM = malloc(size*sizeof(int));
    return VM;
}


void showReg(CPU* cpu){
    printf("=====[VM Info]=====\n");
    printf("=====[General registers]\n");
    printf("eax\t0x%08X\n",*cpu->eax);
    printf("ebx\t0x%08X\n",*cpu->ebx);
    printf("ecx\t0x%08X\n",*cpu->ecx);
    printf("edx\t0x%08X\n",*cpu->edx);
    printf("=====[Control registers]\n");
    printf("eip\t0x%08X\n",*cpu->eip);
    printf("esp\t0x%08X\n",*cpu->esp);
    printf("=====[Flags]\n");
    printf("Z\t%d\n",(*cpu->flags)&0x1);
}

void dump(int* arr, int size){
    printf("=====[Dumping RAM]=====\n");
    for(int i=0; i<size; i++){
        if(i%8==0) printf("\n");
        printf("0x%08X  ", arr[i]);
    }
    printf("\n");
}

void showVMInfo(VirtualMachine* VM){
    showReg(VM->CPU);
    dump(VM->RAM, VM->info->memsize);
}

int mem_get(VirtualMachine* VM, int addr){
    return VM->RAM[addr];
}

int* mem_read(VirtualMachine* VM, int addr, int size){
    int* data = malloc(size*sizeof(int));
    for(int i=0;i<size;i++){
        data[i]=mem_get(VM,addr++);
    }
    return data;
}

void mem_write(VirtualMachine* VM, int address, int data){
    VM->RAM[address]=data;
}

void mem_copy(VirtualMachine* VM, int address, int* data, int size){
    for(int i=0;i<size;i++){
        mem_write(VM,address++,data[i]);
    }
}

void loadProgramInVM(VirtualMachine* VM, int* program, int program_size){
    mem_copy(VM,0x0,program,program_size); 
    *(VM->CPU->esp) = program_size;
}

void exitVM(VirtualMachine* vm) {
    printf("[+] Stopping VM...\n");
    showReg(vm->CPU);
    vm->info->running=0;
}

void mov(int* a, int b){ *a=b; }            //Load B in reg A
void movr(int* a, int* b){ mov(a,*b); }     //Load value in reg B in reg A
void add(int* a, int* b){ *a=*a+*b; }       //Load A+B in reg A
void sub(int* a, int* b){ *a=*a-*b; }
void subr(int* a, int b){ *a=*a-b; }
void xor(int* a, int* b){ *a=*a^*b; }
void and(int* a, int* b){ *a=*a&*b; }
void or(int* a, int* b){ *a=*a|*b; }
void not(int* a){ *a=~(*a);}
void jmp(CPU* cpu, int addr) { *cpu->eip = addr; }
void cmp(CPU* cpu, int* a, int* b){
    if(*a==*b){
        *cpu->flags|=1;
    }else{
        *cpu->flags&=0;
    }
}
void jz(CPU* cpu, int addr) { 
    if(((*cpu->flags)&1)==1){
        *cpu->eip = addr;
    }
}
void jnz(CPU* cpu, int addr) {
    if(((*cpu->flags)&1)==0){
        *cpu->eip = addr; 
    }
}

int* getRegister(CPU* cpu, int reg){
    switch(reg){
        case 0: return cpu->eax;
        case 1: return cpu->ebx;
        case 2: return cpu->ecx;
        case 3: return cpu->edx;
        case 4: return cpu->esp;
        default:
            return NULL;
    }
}

void op_parse(VirtualMachine* VM, int op){
    int operation = (op>>24)&0xFF;
    switch(operation){
        case INSTRUCTION_MOV: { //mov
            int operand1 = (op>>16)&0xFF;
            int* reg1 = getRegister(VM->CPU, operand1);
            int operand2 = op&0xFFFF;
            mov(reg1,operand2);
            break;
        }
        case INSTRUCTION_MOVR : { //movr
            int operand1 = (op>>16)&0xFF;
            int* reg1 = getRegister(VM->CPU, operand1);
            int operand2 = (op>>8)&0xFF;
            int* reg2 = getRegister(VM->CPU, operand2);
            movr(reg1,reg2);
            break;
        }
        case INSTRUCTION_ADD: { //add
            int operand1 = (op>>16)&0xFF;
            int* reg1 = getRegister(VM->CPU, operand1);
            int operand2 = (op>>8)&0xFF;
            int* reg2 = getRegister(VM->CPU, operand2);
            add(reg1,reg2);
            break;
        }
        case INSTRUCTION_SUB: { //sub
            int operand1 = (op>>16)&0xFF;
            int* reg1 = getRegister(VM->CPU, operand1);
            int operand2 = (op>>8)&0xFF;
            int* reg2 = getRegister(VM->CPU, operand2);
            sub(reg1,reg2);
            break;
        }

        case INSTRUCTION_CMP: { //cmp
            int operand1 = (op>>16)&0xFF;
            int* reg1 = getRegister(VM->CPU, operand1);
            int operand2 = (op>>8)&0xFF;
            int* reg2 = getRegister(VM->CPU, operand2);
            cmp(VM->CPU, reg1, reg2);
            break;
        }

        case INSTRUCTION_JMP: { //jmp
            int operand1 = op&0xFFFFFF;
            jmp(VM->CPU, operand1);
            break;
        }

        case INSTRUCTION_SUBR : { //subr
            int operand1 = (op>>16)&0xFF;
            int* reg1 = getRegister(VM->CPU, operand1);
            int operand2 = op&0xFFFF;
            subr(reg1,operand2);
            break;
        }

        case INSTRUCTION_JZ: { //jz
            int operand1 = op&0xFFFFFF;
            jz(VM->CPU, operand1);
            break;
        }

        case INSTRUCTION_JNZ: { //jnz
            int operand1 = op&0xFFFFFF;
            jnz(VM->CPU, operand1);
            break;
        }

        case TRAP_BREAKPOINT: { //breakpoint
            printf("[+] Breakpoint hit!\n");
            showReg(VM->CPU);
            break;
        }

        case TRAP_EXIT: { //breakpoint
            exitVM(VM);
            break;
        }

        default:
            printf("[!] ERROR : %X not recognized\n", op);
            showVMInfo(VM);
            VM->info->running=0;
            break;
    }
}

void run(VirtualMachine* vm){
    printf("[+] Starting VM...\n");
    vm->info->running=1;
    while(vm->info->running==1){
        int d = mem_get(vm, *vm->CPU->eip);
        op_parse(vm, d);
        *(vm->CPU->eip)+=1;
    }
}