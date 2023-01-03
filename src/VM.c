#include <stdlib.h>
#include <stdio.h>
#include "inc/struct.h"
#include "inc/CPU.h"

VirtualMachine* initVM(int size){
    printf("[+] Initializing Virtual Machine with %d bytes of RAM\n", size);
    VirtualMachine* VM = malloc(sizeof(VirtualMachine));

    // Load General Information
    VM->info = malloc(sizeof(INFO));
    VM->info->memsize=size;

    /* Init CPU */
    initCPU(VM);

    /* Init RAM */
    VM->RAM = malloc(size);
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

void dump(unsigned char* arr, int size){
    printf("=====[Dumping RAM]=====\n");
    for(int i=0; i<size; i++){
        if(i%8==0) printf("\n");
        printf("0x%02X  ", arr[i]);
    }
    printf("\n");
}

void showVMInfo(VirtualMachine* VM){
    showReg(VM->CPU);
    dump(VM->RAM, VM->info->memsize);
}

unsigned char mem_get(VirtualMachine* VM, int addr){
    return VM->RAM[addr];
}

unsigned char* mem_read(VirtualMachine* VM, int addr, int size){
    unsigned char* data = malloc(size*sizeof(char));
    for(int i=0;i<size;i++){
        data[i]=mem_get(VM,addr++);
    }
    return data;
}

void mem_write(VirtualMachine* VM, int address, unsigned char data){
    VM->RAM[address]=data;
}

void mem_copy(VirtualMachine* VM, int address, unsigned char* data, int size){
    for(int i=0;i<size;i++){
        mem_write(VM,address++,data[i]);
    }
}

void loadProgramInVM(VirtualMachine* VM, unsigned char* program, int program_size){
    mem_copy(VM,0x0,program,program_size); 
    *(VM->CPU->esp) = program_size;
}

void exitVM(VirtualMachine* vm) {
    printf("[+] Stopping VM...\n");
    vm->info->running=0;
}

int* getRegister(CPU* cpu, char reg){
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


unsigned char readByte(VirtualMachine* VM, int addr){
    return VM->RAM[addr];
}

int readInt(VirtualMachine* VM, int addr){
    int res = 0x00000000;
    res |= readByte(VM, addr)<<24;
    res |= readByte(VM, addr+1)<<16;
    res |= readByte(VM, addr+2)<<8;
    res |= readByte(VM, addr+3);
    return res;
}

int fetch(VirtualMachine* VM){
    unsigned char operation = readByte(VM, *VM->CPU->eip);

    switch(operation){
        case INSTRUCTION_MOV: {
            char reg_code = readByte(VM, *VM->CPU->eip+0x01);
            int value = readInt(VM, *VM->CPU->eip+0x02);
            int* reg = getRegister(VM->CPU, reg_code);
            mov(reg, value);
            return 0x06;
        }

        case INSTRUCTION_ADD: {
            char reg1_code = readByte(VM, *VM->CPU->eip+0x01);
            int* reg1 = getRegister(VM->CPU, reg1_code);
            char reg2_code = readByte(VM, *VM->CPU->eip+0x02);
            int* reg2 = getRegister(VM->CPU, reg2_code);
            add(reg1, reg2);
            return 0x03;
        }

        case INSTRUCTION_SUBR: {
            char reg_code = readByte(VM, *VM->CPU->eip+0x01);
            int value = readInt(VM, *VM->CPU->eip+0x02);
            int* reg = getRegister(VM->CPU, reg_code);
            subr(reg, value);
            return 0x06;
        }

        case INSTRUCTION_CMP: {
            char reg1_code = readByte(VM, *VM->CPU->eip+0x01);
            int* reg1 = getRegister(VM->CPU, reg1_code);
            char reg2_code = readByte(VM, *VM->CPU->eip+0x02);
            int* reg2 = getRegister(VM->CPU, reg2_code);
            cmp(VM->CPU, reg1, reg2);
            return 0x03;
        }

        case INSTRUCTION_JNZ: {
            int addr = readInt(VM, *VM->CPU->eip+0x01);
            jnz(VM->CPU, addr*8);
            return 0x01;
        }


        case TRAP_BREAKPOINT: { //breakpoint
            printf("[+] Breakpoint hit!\n");
            showReg(VM->CPU);
            dump(VM->RAM, VM->info->memsize);
            return 0x01;
        }

        case TRAP_NOP: { //NOP
            return 0x01;
        }

        case TRAP_EXIT: { //Exit
            exitVM(VM);
            showReg(VM->CPU);
            return 0x00;
        }
        default:
            printf("[!] ERROR : 0x%X not recognized\n", operation);
            showVMInfo(VM);
            VM->info->running=0;
            return 0x00;
    }
}

void run(VirtualMachine* vm){
    printf("[+] Starting VM...\n");
    vm->info->running=1;
    while(vm->info->running==1){
        int next = fetch(vm);
        *(vm->CPU->eip)+=next;
    }
}