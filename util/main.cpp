#include <iostream>
#include <Windows.h>



void CreateImage()
{

    HANDLE bootloaderHandle;
    HANDLE kernelHandle;
    HANDLE diskImageHandle;

    bootloaderHandle = CreateFile(L"output\\bootloader.bin", GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
    kernelHandle = CreateFile(L"output\\kernel.bin", GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
    diskImageHandle = CreateFile(L"output\\disk.img", GENERIC_ALL, NULL, NULL, OPEN_EXISTING, NULL, NULL);


    const int kernelSize     = GetFileSize(kernelHandle, NULL);
    const int bootloaderSize = GetFileSize(bootloaderHandle, NULL);



    char* bootloader = new char[bootloaderSize];
    char* kernel     = new char[kernelSize];
    char* merged     = new char[bootloaderSize + kernelSize];


    for (int i = 0; i < sizeof(merged); i++)
    {
        merged[i] = 0;
    }


    if (!ReadFile(bootloaderHandle, bootloader, bootloaderSize, NULL, NULL))
    {
        printf("Readfile failed: bootloader.bin   %i", GetLastError());
    }

    if (!ReadFile(kernelHandle, kernel, kernelSize, NULL, NULL))
    {
        printf("Readfile failed: kernel.bin   %i", GetLastError());
    }



    memcpy(merged, bootloader, bootloaderSize);
    memcpy(merged + bootloaderSize, kernel, kernelSize);

    if (merged == nullptr)
    {
        printf("memcpy failed\r\n");
    }

    if (!WriteFile(diskImageHandle, merged, bootloaderSize + kernelSize, NULL, NULL))
    {
        printf("WriteFile failed: disk.img   %i\r\n", GetLastError());

    }

}

int main() 
{

    CreateImage();

    return 0;
}


