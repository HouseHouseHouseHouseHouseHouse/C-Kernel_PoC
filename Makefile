gcc-flags = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

init.o: init.s
	i686-elf-as init.s -o bin/init.o

main.o: main.c
	i686-elf-gcc -c main.c -o bin/main.o $(gcc-flags)

%.o: inc/%.c
	i686-elf-gcc -c $< -o bin/$@ $(gcc-flags)

main.bin: bin/*.o linker.ld
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib bin/*.o -lgcc
