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


const char* videoMemoryPtr = (char*)0xb8000;


void ClearScreen(enum color color);
void Print(const char* string, int x, int y, enum color color);
void Stage1(int col);



void _start()
{
	ClearScreen(VGA_COLOR_BLACK);
	
	Stage1(2);
	


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

void ClearScreen(enum color color)
{
	char* addr = videoMemoryPtr;

	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		*addr = ' ';
		addr++;
		*addr = color;
		addr++;
	}
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



	if (col == 1) //black bg
	{

		//ClearScreen(VGA_COLOR_BLACK);


		Print("AcidRain 2025", 33, 0, VGA_COLOR_LIGHT_BROWN);

		int j = 2;
		for (int i = 0; i < 20; i++)
		{
			Print(art[i], 13, j, VGA_COLOR_LIGHT_BROWN);
			j++;
		}

		Print("Press any key...", 31, 23, VGA_COLOR_LIGHT_BROWN);
	}
	else if(col == 2) //yellow bg
	{
		ClearScreen(VGA_COLOR_LIGHT_BROWN);
		Print("AcidRain 2025", 33, 0, VGA_COLOR_BLACK);

		int j = 2;
		for (int i = 0; i < 20; i++)
		{
			Print(art[i], 13, j, VGA_COLOR_BLACK);
			j++;
		}

		Print("Press any key...", 31, 23, VGA_COLOR_BLACK);
	}

}