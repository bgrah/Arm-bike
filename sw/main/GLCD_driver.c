#include "GLCD_driver.h"
#include "ascii.h"



//===========================================================================
//bit banging SPI function
//input: 1 byte of data to be sent
//return: no return value
//function description:
//sending a byte of data using SPI
//===========================================================================
void SPI_write(char data)
{
  char i; 
  for (i=0;i<8;i++) 
  {
    IO0CLR=P0_4;    // SCK=0
    if (data&0x80)  IO0SET=P0_6;  // MOSI=1
    else            IO0CLR=P0_6;  // MOSI=0
    IO0SET=P0_4;    // SCK=1
    data=data<<1;  
  }
  IO0CLR=P0_4;      // SCK=0
}

//=======================================================================
//GLCD write command function
//input: one byte of command data
//return: no return value
//function description:
//this function write a byte of LCD command
//=======================================================================   
void GLCD_write_command(char command) 
{  
  IO0CLR=P0_5;        // D/C=0  Command
  IO0CLR=P0_7;        // SSEL=0 Chip enabled 
  //SPI_write(command);
  char i; 
  for (i=0;i<8;i++) 
  {
    IO0CLR=P0_4;    // SCK=0
    if (command&0x80) IO0SET=P0_6;  // MOSI=1
    else              IO0CLR=P0_6;  // MOSI=0
    IO0SET=P0_4;    // SCK=1
    command=command<<1;  
  }
  IO0CLR=P0_4;      // SCK=0
  IO0SET=P0_7;      // SSEL=1 Chip disabled 
}

//=======================================================================
//GLCD write data function
//input: one byte of data
//return: no return value
//function description:
//this function write a byte of LCD data to be display on LCD
// GLCD_gotoxy needed to be called to set the position prior to this function
//=======================================================================
void GLCD_write_data(char data) 
{  
  IO0SET=P0_5;        // D/C=1  Data
  IO0CLR=P0_7;        // SSEL=0 Chip enabled 
  char i; 
  for (i=0;i<8;i++) 
  {
    IO0CLR=P0_4;    // SCK=0
    if (data&0x80)  IO0SET=P0_6;  // MOSI=1
    else            IO0CLR=P0_6;  // MOSI=0
    IO0SET=P0_4;    // SCK=1
    data=data<<1;  
  }
  IO0CLR=P0_4;        // SCK=0 
  IO0SET=P0_7;        // SSEL=1 Chip disabled   
}
//=======================================================================
//GLCD gotoXY position function
//input: 2 bytes of data describing the coordinate of LCD 
//return: no return value
//function description:
//this function set the position on LCD 
//=======================================================================
void GLCD_gotoxy(char xloc, char yloc)
{
  char i;
  char command;
  IO0CLR=P0_5;        // D/C=0  Command
  IO0CLR=P0_7;        // SSEL=0 Chip enabled 

  command=0x40|(yloc&0x07);
  for (i=0;i<8;i++) 
  {
    IO0CLR=P0_4;    // SCK=0
    if (command&0x80) IO0SET=P0_6;  // MOSI=1
    else              IO0CLR=P0_6;  // MOSI=0
    IO0SET=P0_4;    // SCK=1
    command=command<<1;  
  }
  command=0x80|(xloc&0x7f);
  for (i=0;i<8;i++) 
  {
    IO0CLR=P0_4;    // SCK=0
    if (command&0x80) IO0SET=P0_6;  // MOSI=1
    else              IO0CLR=P0_6;  // MOSI=0
    IO0SET=P0_4;    // SCK=1
    command=command<<1;  
  }
  IO0CLR=P0_4;      // SCK=0
  IO0SET=P0_7;      // SSEL=1 Chip disabled
}
//=======================================================================
//GLCD clear screen function
//input: no input is needed
//return: no return value
//function description:
//this function clear the whole GLCD
//=======================================================================
void GLCD_clean_ddram(void) 
{ 
	int ddram;
  char i;
  GLCD_gotoxy(0,0);   // 84*6=504      clear LCD
  
  IO0SET=P0_5;        // D/C=1  Data
  IO0CLR=P0_7;        // SSEL=0 Chip enabled
  IO0CLR=P0_6;        // MOSI=0
  for (ddram=0;ddram<504*8;ddram++)      // 84x6 8-bit cells
  {
    IO0CLR=P0_4;      // SCK=0
    IO0SET=P0_4;      // SCK=1 
  }
  IO0CLR=P0_4;        // SCK=0 
  IO0SET=P0_7;        // SSEL=1 Chip disabled
}
//=======================================================================
//GLCD initialization function
//input: no input is needed
//return: no return value
//function description:
//this function initilization the LCD; reset and prepare the LCD to be 
//used, and clear the LCD to clear of the unknown random value in DDRAM
//=======================================================================
void GLCD_init(void) 
{
	int delay;
  
  IO0SET=P0_5;                // ?? bytes are stored in the display data ram, address counter, incremented automatically
  IO0SET=P0_7;                // chip disabled 

  IO0CLR=P0_13;               // reset chip during 250ms 
  delay=50000; while(delay>0)delay--;        // works with less..... 
  IO0SET=P0_13; 
  delay=50000; while(delay>0)delay--;

  GLCD_write_command(0x21);   // set extins extended instruction set 
  GLCD_write_command(0xbb);   // Vop  v1: 0xc8 (for 3V)// v2: 0xa0 (for 3V) // v3: 0xc2 (2v6-5v)   ******************************************************************************************************************** 
  GLCD_write_command(0x13);   // bias 
  GLCD_write_command(0x20);   // horizontal mode from left to right, X axe are incremented automatically , 0x22 for vertical addressing ,back on normal instruction set too 
  GLCD_write_command(0x09);   // all on 

  GLCD_clean_ddram();         // reset DDRAM, otherwise the lcd is blurred with random pixels 

  GLCD_write_command(0x08);   // mod control blank change (all off) 
  GLCD_write_command(0x0c);   // mod control normal change 
	
}
//=======================================================================
//GLCD Putchar function
//input: one byte of ASCII, use single quote eg. 'A'
//return: no return value
//function description:
//this function write an ASCII character to LCD
// GLCD_gotoxy needed to be called to set the position prior to this function
//=======================================================================
void GLCD_putc(char data)					// Write 1 character to LCD 
{
	unsigned char lcddata;
	unsigned short locate;
	unsigned char i,j;

	locate=(data-32);
  IO0SET=P0_5;        // D/C=1  Data
  IO0CLR=P0_7;        // SSEL=0 Chip enabled 
  for (i=0;i<5;i++)   // 8x5 font
 	{
    lcddata = asciicharacter[locate][i]; 
    for (j=0;j<8;j++) 
    {
      IO0CLR=P0_4;    // SCK=0
      if (lcddata&0x80)  IO0SET=P0_6;  // MOSI=1
      else            IO0CLR=P0_6;  // MOSI=0
      IO0SET=P0_4;    // SCK=1
      lcddata=lcddata<<1;  
    }
  }
  IO0CLR=P0_6;  // MOSI=0
  for (j=0;j<8;j++)             // One extra col. for space
  {
    IO0CLR=P0_4;    // SCK=0          
    IO0SET=P0_4;    // SCK=1  
  }
  IO0CLR=P0_4;        // SCK=0 
  IO0SET=P0_7;        // SSEL=1 Chip disabled
}
//=======================================================================
//GLCD putstring function
//input: array of null terminated character i.e string; use double quote
//		 e.g. "MEL 1123"
//return: no return value
//function description:
//this function write strings to LCD
// GLCD_gotoxy needed to be called to set the position prior to this function
//=======================================================================

void GLCD_putstring(char* stringg)	// Write message to LCD (C string type)
{
	while (*stringg)							// Look for end of string
		GLCD_putc(*(stringg++));			//	
}

//=======================================================================
//GLCD display bitmap graphic function
//input:
//locx-x-coordinate to display (0-84); locy-y-coordinate to display(0-5)
//xpixel-pixel width of the graphic ypixel-height of graphic in byte 
//graphic: pointer to array of bitmap graphic (preprocessed as array of bytes of value)
//return: no return value
//function description:
//this function display bitmap picture on screen
//=======================================================================
void GLCD_putgraphic(char locx, char locy, char xpixel, char ypixel, char* graphic)
{
	int i,j,k;
	char endpage=locy+ypixel;
  char data, command;
  
  IO0CLR=P0_7;        // SSEL=0 Chip enabled
	for(j=locy;j<endpage;j++)
	{
    IO0CLR=P0_5;      // D/C=0  Command
    command=0x40|(j&0x07);
    for (k=0;k<8;k++) 
    {
      IO0CLR=P0_4;    // SCK=0
      if (command&0x80) IO0SET=P0_6;  // MOSI=1
      else              IO0CLR=P0_6;  // MOSI=0
      IO0SET=P0_4;    // SCK=1
      command=command<<1;  
    }
    command=0x80|(locx&0x7f);
    for (k=0;k<8;k++) 
    {
      IO0CLR=P0_4;    // SCK=0
      if (command&0x80) IO0SET=P0_6;  // MOSI=1
      else              IO0CLR=P0_6;  // MOSI=0
      IO0SET=P0_4;    // SCK=1
      command=command<<1;  
    }

    IO0SET=P0_5;        // D/C=1  Data
		for (i=0; i<xpixel; i++)
    {
      //GLCD_write_data(*(graphic++));
      data=*(graphic++);
      for (k=0;k<8;k++) 
      {
        IO0CLR=P0_4;    // SCK=0
        if (data&0x80)  IO0SET=P0_6;  // MOSI=1
        else            IO0CLR=P0_6;  // MOSI=0
        IO0SET=P0_4;    // SCK=1
        data=data<<1;  
      }
    }
	}
  IO0CLR=P0_4;        // SCK=0 
  IO0SET=P0_7;        // SSEL=1 Chip disabled
}


