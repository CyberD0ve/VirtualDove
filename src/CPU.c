#include "inc/struct.h"
#include <stdlib.h>

void initCPU(VirtualMachine* VM){
    VM->CPU = malloc(sizeof(CPU));
    VM->CPU->eax=malloc(sizeof(int));
    VM->CPU->ebx=malloc(sizeof(int));
    VM->CPU->ecx=malloc(sizeof(int));
    VM->CPU->edx=malloc(sizeof(int));
    VM->CPU->eip=malloc(sizeof(int));
    VM->CPU->esp=malloc(sizeof(int));
    VM->CPU->flags=malloc(sizeof(int));
}

/* ALU */
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
        *cpu->eip = addr-1; 
    }
}