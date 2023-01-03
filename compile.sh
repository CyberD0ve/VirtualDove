gcc -Wall -c ./src/CPU.c -o  ./bin/CPU.o
gcc -Wall -c ./src/VM.c -o  ./bin/VM.o
gcc -Wall -c ./src/main.c -o ./bin/main.o
gcc -Wall ./bin/main.o ./bin/VM.o ./bin/CPU.o -o ./bin/VM
