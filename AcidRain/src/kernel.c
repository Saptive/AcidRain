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
#define ATA_DRIVE_SELECT 0x1F6  // Drive & Head Select
#define ATA_COMMAND      0x1F7  // Command Register (W) / Status Register (R)
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
void outb(uint16_t port, uint8_t data);
void SetCursorPos(int x, int y);
uint8_t GetScanCode();
uint8_t GetKeyPress();


void Stage1(int col);
void Stage2();



void _start()
{
	ClearScreen(VGA_COLOR_BLACK);
	
	Sleep(150000000);

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

// Simple I/O port read function
uint8_t inb(uint16_t port) 
{
	uint8_t data;
	__asm__ volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}

// Simple I/O port write function
void outb(uint16_t port, uint8_t data) 
{
	__asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
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

	int cursorPosX = 5;

	SetCursorPos(cursorPosX, 24);

	char keyBuffer[256] = {0};
	uint8_t buffer_head = 0;
	uint8_t buffer_tail = 0;


	int charCount = 0;

	

	Sleep(100000000);

	while (GetScanCode() != 0x1C) // Scancode for the Enter key
	{
		
		uint8_t keyPressed = GetKeyPress();
		

		if (keyPressed)
		{
			keyBuffer[charCount] = keyPressed;
			charCount++;
			keyBuffer[charCount] = '\0';

			Print(keyBuffer, cursorPosX, 24, VGA_COLOR_BROWN);
			SetCursorPos(cursorPosX + charCount, 24);
		}
	

		//Print(keyBuffer[charCount -1], cursorPosX -1, 24, VGA_COLOR_BROWN);
		//SetCursorPos(cursorPosX, 24);
	}

	Print("Enter key pressed", 0, 15, VGA_COLOR_BROWN);

	//Print(keyBuffer, 5, 24, VGA_COLOR_CYAN);







}