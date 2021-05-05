PATH="/usr/local/cross/bin:$PATH"

i686-elf-as init.s -o bin/init.o

i686-elf-gcc -c main.c -o bin/main.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i686-elf-gcc -T linker.ld -o main.bin -ffreestanding -O2 -nostdlib bin/init.o bin/main.o -lgcc