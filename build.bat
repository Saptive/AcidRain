
i686-elf-gcc -ffreestanding -fno-inline -c .\AcidRain\src\kernel.c -o kernel.o

i686-elf-ld -T linker.ld -s -o kernel.elf kernel.o

i686-elf-objcopy -O binary kernel.elf kernel.bin

nasm -f bin .\Bootloader\bootloader.asm -o .\Bootloader\bootloader.bin

