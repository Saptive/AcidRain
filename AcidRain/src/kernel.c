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
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

const char* videoMemoryPtr = (char*)0xb8000;

//void WriteChar(char* character, enum color Color)
//{
//
//	//volatile char* videoMemoryPtr = (char*)0xb8000;
//
//
//	//first byte is the character we want to print
//	*videoMemoryPtr = *character;
//	//increment by one byte
//	videoMemoryPtr++;
//	//second byte is the color
//	*videoMemoryPtr = (unsigned char)Color;
//	//increment to next character
//	videoMemoryPtr++;
//}

//__attribute__((noinline)) void WriteString(const char* string, enum color color, int x, int y)
//{
//	char* videoMemoryPtr = (char*)0xb8000;
//	char* addr = videoMemoryPtr + 2 * (80 * y + x);
//
//	for (unsigned int i = 0; string[i] != '\0'; i++)
//	{
//		*addr = string[i];
//		addr++;
//		*addr = (char)color;
//		addr++;
//	}
//
//}

void ClearScreen();
void Print(const char* string, int x, int y, enum color color);



void _start()
{
	ClearScreen();
	Print("AcidRain 2025", 0, 0, VGA_COLOR_BROWN);


	char seperator = 219;

	

	for (int i = 0; i < 25; i++)
	{
		Print(seperator, i, 2, VGA_COLOR_WHITE);
	}



	while (1)
	{

	}
}



void Print(const char* string, int x, int y, enum color color)
{
	char* addr = videoMemoryPtr + 2 * (80 * y + x);

	for (unsigned int i = 0; string[i] != '\0'; i++)
	{
		*addr = string[i];
		addr++;
		*addr = (char)color;
		addr++;
	}
}

void ClearScreen()
{
	char* addr = videoMemoryPtr;

	for (int i = 0; i < 80 * 25; i++)
	{
		*addr = ' ';
		addr += 2;
	}
}