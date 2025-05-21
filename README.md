# AcidRain


**AcidRain** is a malware project inspired by the 2016 **Petya** ransomware. It has no connection to the wiper targeting modems. 
This project is developed purely for fun, aiming to analyze disk encryption techniques, bootloader manipulation, and bare metal programming.

## Features
- **Master Boot Record (MBR) Overwrite** – A usermode app running on a windows machine will write to sector 0 on the main disk.
- **Custom Bootloader** – Moves from Real Mode to Protected Mode and then loads the kernel written in C into memory and jumps to it's entrypoint.
- **Original MBR backup** - The usermode app encrypts and backs up the content of the original boot sector.
- **Disk I/O operations** - The kernel can read and write the disk using ATA I/O ports.

## Description
A user-mode application running on Windows, given sufficient privileges, encrypts and backs up the original MBR, writes a custom bootloader to sector 0, and stores the kernel starting at sector 1 of the primary disk. Once completed, the application exits. *(TODO: trigger system restart.)*

When the system boots, the custom bootloader loads the kernel into memory, waits for a keypress, and then jumps to the kernel's entry point.

The kernel handles user keyboard input. Upon receiving the correct decryption key, it reads sector 18 (which holds the encrypted original MBR), decrypts it using XOR with the key `0x28`, and writes the result back to sector 0 to restore the original MBR. All disk operations are done via ATA PIO  *(TODO: replace XOR with Salsa20 for encryption and decryption.)*

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






