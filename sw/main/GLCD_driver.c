// Date:   1.5.2012
// Author: Blaž Grah

// Driver for Nokia 5510 graphical LCD screen (84*48px)
// Communication is made in software (bit-banging)
// Must set output pins in GLCD_driver.h and set pin directions!

/* 
  set_pin_direction(0, (P0_4|P0_5|P0_6|P0_7|P0_13));
    int Vop = 49;             // Sets Vop           [0-127] (def. 49 (0x31))  !!!
    int tempC = 3;            // Sets temp coeff.   [0-3]   (def. 3)
    int biasS = 3;            // Sets bias system   [0-7]   (def. 3)
  GLCD_init(Vop, tempC, biasS);
*/

#include "GLCD_driver.h"
#include "ascii.h"           // For ascii to 8x5 character conversion

//=======================================================================
//GLCD initialization function
//input: no input is needed
//return: no return value
//function description:
//this function initilization the LCD; reset and prepare the LCD to be 
//used, and clear the LCD to clear of the unknown random value in DDRAM
//=======================================================================

/* Initializes PCD5544 controler */
  // Input:       Vop      ... Sets Vop           [0-127] (def. 59 (0x3B))  !!! Read datasheet before change
  //              tempC    ... Sets temp coeff.   [0-3]   (def. 0)
  //              biasS    ... Sets bias system   [0-7]   (def. 3)
  // Output:      none
  // Description: LCD initialisation. Resets LCD, prepare the LCD with Vop, tempC, biasS parameters
  //              and clears unknown values in LCD RAM.
  // Important!:  Be carefull when setting parameters, check the datasheet!
  // WCET:        Unknown
  void GLCD_init(int Vop, int tempC, int biasS) 
  {
  	int delay;
    
    DC_H;     // D/C=0  Data
    SCE_H;    // SSEL=1 Chip disabled
  
    RST_L;                                // Reset and wait a bit 
    delay=50000; while(delay>0)delay--;
    RST_H; 
    delay=50000; while(delay>0)delay--;
  
    GLCD_write_command(function_set | sH);    // Active mode, horizontal, extended instructions 
    GLCD_write_command(set_Vop | Vop);        // Sets voltage for LCD 
    GLCD_write_command(bias_system | biasS);  // Sets bias system
    GLCD_write_command(temp_cr | tempC);      // Sets temp. coefficient
    GLCD_write_command(function_set);         // Active mode, horizontal, normal instructions
    GLCD_write_command(display_cr | display_all_on);    // All display segments on 
  
    GLCD_clean_ddram();         // Clears LCD's RAM
  
    GLCD_write_command(display_cr | display_blank);     // All display segments off  
    GLCD_write_command(display_cr | normal_mode);       // Normal display mode
  }


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
    SCK_L;    // SCK=0
    if (data&0x80)  SDA_H;  // MOSI=1
    else            SDA_L;  // MOSI=0
    SCK_H;    // SCK=1
    data=data<<1;  
  }
  SCK_L;      // SCK=0
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
  DC_L;        // D/C=0  Command
  SCE_L;        // SSEL=0 Chip enabled 
  //SPI_write(command);
  char i; 
  for (i=0;i<8;i++) 
  {
    SCK_L;    // SCK=0
    if (command&0x80) SDA_H;  // MOSI=1
    else              SDA_L;  // MOSI=0
    SCK_H;    // SCK=1
    command=command<<1;  
  }
  SCK_L;      // SCK=0
  SCE_H;      // SSEL=1 Chip disabled 
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
  DC_H;        // D/C=1  Data
  SCE_L;        // SSEL=0 Chip enabled 
  char i; 
  for (i=0;i<8;i++) 
  {
    SCK_L;    // SCK=0
    if (data&0x80)  SDA_H;  // MOSI=1
    else            SDA_L;  // MOSI=0
    SCK_H;    // SCK=1
    data=data<<1;  
  }
  SCK_L;        // SCK=0 
  SCE_H;        // SSEL=1 Chip disabled   
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
  DC_L;        // D/C=0  Command
  SCE_L;        // SSEL=0 Chip enabled 

  command=0x40|(yloc&0x07);
  for (i=0;i<8;i++) 
  {
    SCK_L;    // SCK=0
    if (command&0x80) SDA_H;  // MOSI=1
    else              SDA_L;  // MOSI=0
    SCK_H;    // SCK=1
    command=command<<1;  
  }
  command=0x80|(xloc&0x7f);
  for (i=0;i<8;i++) 
  {
    SCK_L;    // SCK=0
    if (command&0x80) SDA_H;  // MOSI=1
    else              SDA_L;  // MOSI=0
    SCK_H;    // SCK=1
    command=command<<1;  
  }
  SCK_L;      // SCK=0
  SCE_H;      // SSEL=1 Chip disabled
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
  
  DC_H;        // D/C=1  Data
  SCE_L;        // SSEL=0 Chip enabled
  SDA_L;        // MOSI=0
  for (ddram=0;ddram<504*8;ddram++)      // 84x6 8-bit cells
  {
    SCK_L;      // SCK=0
    SCK_H;      // SCK=1 
  }
  SCK_L;        // SCK=0 
  SCE_H;        // SSEL=1 Chip disabled
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
  DC_H;        // D/C=1  Data
  SCE_L;        // SSEL=0 Chip enabled 
  for (i=0;i<5;i++)   // 8x5 font
 	{
    lcddata = asciicharacter[locate][i]; 
    for (j=0;j<8;j++) 
    {
      SCK_L;    // SCK=0
      if (lcddata&0x80)  SDA_H;  // MOSI=1
      else                SDA_L;  // MOSI=0
      SCK_H;    // SCK=1
      lcddata=lcddata<<1;  
    }
  }
  SDA_L;  // MOSI=0
  for (j=0;j<8;j++)             // One extra col. for space
  {
    SCK_L;    // SCK=0          
    SCK_H;    // SCK=1  
  }
  SCK_L;        // SCK=0 
  SCE_H;        // SSEL=1 Chip disabled
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
  
  SCE_L;        // SSEL=0 Chip enabled
	for(j=locy;j<endpage;j++)
	{
    DC_L;      // D/C=0  Command
    command=0x40|(j&0x07);
    for (k=0;k<8;k++) 
    {
      SCK_L;    // SCK=0
      if (command&0x80) SDA_H;  // MOSI=1
      else              SDA_L;  // MOSI=0
      SCK_H;    // SCK=1
      command=command<<1;  
    }
    command=0x80|(locx&0x7f);
    for (k=0;k<8;k++) 
    {
      SCK_L;    // SCK=0
      if (command&0x80) SDA_H;  // MOSI=1
      else              SDA_L;  // MOSI=0
      SCK_H;    // SCK=1
      command=command<<1;  
    }

    DC_H;        // D/C=1  Data
		for (i=0; i<xpixel; i++)
    {
      //GLCD_write_data(*(graphic++));
      data=*(graphic++);
      for (k=0;k<8;k++) 
      {
        SCK_L;    // SCK=0
        if (data&0x80)  SDA_H;  // MOSI=1
        else            SDA_L;  // MOSI=0
        SCK_H;    // SCK=1
        data=data<<1;  
      }
    }
	}
  SCK_L;        // SCK=0 
  SCE_H;        // SSEL=1 Chip disabled
}


