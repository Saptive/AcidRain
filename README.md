# AcidRain


**AcidRain** is a research-focused malware project inspired by **Petya** ransomware. This project is developed purely for fun, aiming to analyze disk encryption techniques, bootloader manipulation, and bare metal programming.

## Features
- **Master Boot Record (MBR) Overwrite** – A usermode app running on a windows machine will write to sector 0 on the main disk.
- **Custom Bootloader** – Loads a kernel written in C into memory and jumps to it's entrypoint
- **Protected Mode** – Moves from Real Mode to Protected Mode before jumping to the kernel.
- **Original MBR backup** - The usermode app backs up the content of the original boot sector
