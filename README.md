# AcidRain


**AcidRain** is a malware project inspired by the 2016 **Petya** ransomware. It has no connection to the wiper targeting modems. 
This project is developed purely for fun, aiming to analyze disk encryption techniques, bootloader manipulation, and bare metal programming.

## Features
- **Master Boot Record (MBR) Overwrite** – A usermode app running on a windows machine will write to sector 0 on the main disk.
- **Custom Bootloader** – Moves from Real Mode to Protected Mode and then loads the kernel written in C into memory and jumps to it's entrypoint.
- **Original MBR backup** - The usermode app encrypts and backs up the content of the original boot sector.
- **Disk I/O operations** - The kernel can read and write the disk using ATA I/O ports.

## Description
The usermode app running on windows (if the required previliges are avaliable) writes the custom bootloader to sector 0 and the kernel starting from sector 1 on the primary disk . Once done it extis. (TODO: trigger restart (bsod maybe))
The bootloader, once executed, loads the kernel into memory. It then waits for a keypress and then jumps to the kernel entrypoint.

The kernel is able to get user keyboard input and handle it. Once the correct key is entered, it proceeds by reading sector 18 (the encrypted orgiginal MBR) from the disk into memory, XOR each byte of the sector with the decryption key (0x28), and then write it back to sector 0. (implement salsa20 for MBR encryption and decryption).

### Screenshots

![Bootloader](https://github.com/user-attachments/assets/7fa3d93e-bb1c-4290-ab82-1fc70de173ea)
![Black](https://github.com/user-attachments/assets/2af410b5-17be-4fdb-b95f-1ea740771f93)
![Orange](https://github.com/user-attachments/assets/90dcbaec-5dea-445f-a01d-47d13ab312ea)
![Main](https://github.com/user-attachments/assets/21ad9890-42ad-4cf4-b817-b9e6459d9598)
![Decryption](https://github.com/user-attachments/assets/03e85c72-72fc-461a-b4e8-7eac14a3a064)

### Prerequisites
- **Virtual Machine or emulator for just the bootloader and kernel (VirtualBox, QEMU, Bochs)** – Required for testing.
- **Assembly & C Compiler** (NASM + GCC)
- **Disk Imaging Tools** (HxD)


 ### Compilation
 - Use the included **build.bat**

### Downloads
 - [GCC + NASM download](https://drive.google.com/drive/folders/1TjOJSpnz9Bj8TVHjr1bmG9Abp9VYs74g?usp=drive_link)






