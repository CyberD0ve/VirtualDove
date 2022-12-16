#include <stdlib.h>
#include "inc/struct.h"
#include "inc/VM.h"

// Multiply two number
int program[10] = {
    0x01000000,     // mov eax, 0x0;
    0x01010000,     // mov ebx, 0x0;
    0x01020005,     // mov ecx, 0x5;
    0x010300FF,     // mov edx, 0x7;
    0x03010300,     // add ebx, edx;
    0x07020001,     // subr ecx, 0x01;
    0x05020000,     // cmp ecx, eax;
    0x10000003,     // jnz 0x004;
    0x00,           // exit
};

int main(){
    VirtualMachine* VM = initVM(256); //Init VM with 265 bytes of RAM
    loadProgramInVM(VM, program, 10); //Load program in RAM
    run(VM);                          //Run the VM
}