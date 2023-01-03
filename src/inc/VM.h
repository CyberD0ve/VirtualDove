VirtualMachine* initVM();
void loadProgramInVM(VirtualMachine* VM, unsigned char* program, int program_size);
void showReg(CPU* cpu);
void showVMInfo(VirtualMachine* VM);
void run(VirtualMachine* vm);
