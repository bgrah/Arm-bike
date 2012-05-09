// Date:   1.5.2012
// Author: Blaž Grah

// Driver for Nokia 5510 graphical LCD screen (84*48px)
// Communication is made in software (bit-banging)
// Must set output pins in GLCD_driver.h and set pin directions!

/* Start up
  set_pin_direction(0, (P0_4|P0_5|P0_6|P0_7|P0_13));
    int Vop = 49;             // Sets Vop           [0-127] (def. 49 (0x31))  !!!
    int tempC = 3;            // Sets temp coeff.   [0-3]   (def. 3)
    int biasS = 3;            // Sets bias system   [0-7]   (def. 3)
  GLCD_init(Vop, tempC, biasS);
*/

#include "GLCD_driver.h"
#include "ascii.h"           // For ascii to 8x5 character conversion

/*************************************************************************************************/
/* Initializes PCD5544 controler */
  // Input:       Vop      ... Sets Vop           [0-127] (def. 59 (0x3B))  !!! Read datasheet before change
  //              tempC    ... Sets temp coeff.   [0-3]   (def. 0)
  //              biasS    ... Sets bias system   [0-7]   (def. 3)
  // Output:      none
  // Description: LCD initialisation. Resets LCD, prepare the LCD with Vop, tempC, biasS parameters
  //              and clears unknown values in LCD RAM.
  // Important!:  Be carefull when setting parameters, check the datasheet!
  // WCET:        379.401 us
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
  
    GLCD_clean_ddram();                       // Clears LCD's RAM
  
    GLCD_write_command(display_cr | display_blank);     // All display segments off  
    GLCD_write_command(display_cr | normal_mode);       // Normal display mode
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* SPI write function */
  // Input:       data     ... 8-bit of data
  // Output:      none
  // Description: Writes 8-bits of data to LCD, MSB first. SW-SPI.
  // Important!:  Not used ATM.
  // WCET:        177 us
  void SPI_write(char data)
  {
    char i; 
    for (i=0;i<8;i++)         // For 8-bits
    {
      SCK_L;                  // SCK=0
      if (data&0x80)  SDA_H;  // MOSI=1
      else            SDA_L;  // MOSI=0
      SCK_H;                  // SCK=1
      data=data<<1;  
    }
    SCK_L;                    // SCK=0
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* SPI write command function */
  // Input:       data     ... 8-bit of command data
  // Output:      none
  // Description: Writes 8-bits of command to LCD.
  // Important!:  /
  // WCET:        197 us
  void GLCD_write_command(char command) 
  {  
    DC_L;                       // D/C=0  Command
    SCE_L;                      // SSEL=0 Chip enabled 
    char i; 
    for (i=0;i<8;i++) 
    {
      SCK_L;                    // SCK=0
      if (command&0x80) SDA_H;  // MOSI=1
      else              SDA_L;  // MOSI=0
      SCK_H;                    // SCK=1
      command=command<<1;  
    }
    SCK_L;                      // SCK=0
    SCE_H;                      // SSEL=1 Chip disabled 
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* SPI write data function */
  // Input:       data     ... 8-bit of data
  // Output:      none
  // Description: Writes 8-bits of data to LCD.
  // Important!:  /
  // WCET:        191 us
  void GLCD_write_data(char data) 
  {  
    DC_H;                     // D/C=1  Data
    SCE_L;                    // SSEL=0 Chip enabled 
    char i; 
    for (i=0;i<8;i++) 
    {
      SCK_L;                  // SCK=0
      if (data&0x80)  SDA_H;  // MOSI=1
      else            SDA_L;  // MOSI=0
      SCK_H;                  // SCK=1
      data=data<<1;  
    }
    SCK_L;                    // SCK=0 
    SCE_H;                    // SSEL=1 Chip disabled   
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* GotoXY position function */
  // Input:       xloc      ... 8-bit of x position [0-84]  (px)
  //              yloc      ... 8-bit of y position [0-6]   (6 bytes -> 48px)
  // Output:      none
  // Description: Sets position on LCD
  // Important!:  Y coordinate must be form 0-6 and not in pixels (0-48).
  // WCET:        373 us
  void GLCD_gotoxy(char xloc, char yloc)
  {
    char i;
    char command;
    
    DC_L;                       // D/C=0  Command
    SCE_L;                      // SSEL=0 Chip enabled 
  
    command=0x40|(yloc&0x07);   // Y position command
    for (i=0;i<8;i++) 
    {
      SCK_L;                    // SCK=0
      if (command&0x80) SDA_H;  // MOSI=1
      else              SDA_L;  // MOSI=0
      SCK_H;                    // SCK=1
      command=command<<1;  
    }
    command=0x80|(xloc&0x7f);   // X position command
    for (i=0;i<8;i++) 
    {
      SCK_L;                    // SCK=0
      if (command&0x80) SDA_H;  // MOSI=1
      else              SDA_L;  // MOSI=0
      SCK_H;                    // SCK=1
      command=command<<1;  
    }
    SCK_L;                      // SCK=0
    SCE_H;                      // SSEL=1 Chip disabled
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* Clear LCD's RAM */
  // Input:       none
  // Output:      none
  // Description: Rewrites LCD's RAM with zeros.
  // Important!:  /
  // WCET:        52.820 us
  void GLCD_clean_ddram(void) 
  { 
  	int ddram;
    char i;
    GLCD_gotoxy(0,0);           // 0,0 position
    
    DC_H;                       // D/C=1  Data
    SCE_L;                      // SSEL=0 Chip enabled
    SDA_L;                      // MOSI=0 Writing only zeros
    for (ddram=0;ddram<504*8;ddram++)      // 84x6 8-bit cells
    {
      SCK_L;                    // SCK=0
      SCK_H;                    // SCK=1 
    }
    SCK_L;                      // SCK=0 
    SCE_H;                      // SSEL=1 Chip disabled
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* Put character function */
  // Input:       data     ... Byte of ASCII code
  // Output:      none
  // Description: Looks up ASCII code in table (asciicharacter[][]) and sends it to LCD.
  // Important!:  Must set x,y position before calling this function.
  // WCET:        1021 us
  void GLCD_putch(char data) 
  {
  	unsigned char lcddata;
  	unsigned short locate;
  	unsigned char i,j;
  
  	locate=(data-32);
    DC_H;                           // D/C=1  Data
    SCE_L;                          // SSEL=0 Chip enabled 
    for (i=0;i<5;i++)               // 8x5 font
   	{
      lcddata = asciicharacter[locate][i];    // Get ASCII character
      for (j=0;j<8;j++) 
      {
        SCK_L;                      // SCK=0
        if (lcddata&0x80)  SDA_H;   // MOSI=1
        else               SDA_L;   // MOSI=0
        SCK_H;                      // SCK=1
        lcddata=lcddata<<1;  
      }
    }
    SDA_L;                // MOSI=0 Writing only zeros
    for (j=0;j<8;j++)     // One extra col. for space
    {
      SCK_L;              // SCK=0          
      SCK_H;              // SCK=1  
    }
    SCK_L;                // SCK=0 
    SCE_H;                // SSEL=1 Chip disabled
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* Put string function */
  // Input:       stringg  ... Array of null terminated character (C string type)
  // Output:      none
  // Description: Sends characters one by one to GLCD_putch untill null termination
  // Important!:  Must set x,y position before calling this function.
  // WCET:        16.427 us (16 char long)
  void GLCD_putstring(char* stringg)
  {
  	while (*stringg)							// Till the end off string
  		GLCD_putch(*(stringg++));		// Calls GLCD_putch to send	
  }
/*************************************************************************************************/
/*************************************************************************************************/
/* Display bitmap graphic function */
  // Input:       locx      ... Byte of x start position in pixels          [0-84]
  //              locy      ... Byte of Y start position in bytes of pixels [0-6]     (0-48)
  //              xpixel    ... Graphic width in pixels                     [max 84]
  //              ypixel    ... Graphic height in bytes of pixels           [max 6]   (max 48)
  //              graphic   ... Pointer to array of bitmap graphic (preprocessed as array of bytes of value)
  // Output:      none
  // Description: Displays bitmap graphic on LCD
  // Important!:  /
  // WCET:        92.402 us
  void GLCD_putgraphic(char locx, char locy, char xpixel, char ypixel, char* graphic)
  {
  	int i,j,k;
  	char endpage=locy+ypixel;
    char data, command;
    
    SCE_L;                        // SSEL=0 Chip enabled
  	for(j=locy;j<endpage;j++)     // Go through all y lines 
  	{
      DC_L;                       // D/C=0  Command
      command=0x40|(j&0x07);      // GotoY
      for (k=0;k<8;k++) 
      {
        SCK_L;                    // SCK=0
        if (command&0x80) SDA_H;  // MOSI=1
        else              SDA_L;  // MOSI=0
        SCK_H;                    // SCK=1
        command=command<<1;  
      }
      command=0x80|(locx&0x7f);   // GotoX
      for (k=0;k<8;k++) 
      {
        SCK_L;                    // SCK=0
        if (command&0x80) SDA_H;  // MOSI=1
        else              SDA_L;  // MOSI=0
        SCK_H;                    // SCK=1
        command=command<<1;  
      }
  
      DC_H;                       // D/C=1  Data
  		for (i=0; i<xpixel; i++)    // For each x coordinate in y line
      {
        data=*(graphic++);        // Write one byte of data
        for (k=0;k<8;k++) 
        {
          SCK_L;                  // SCK=0
          if (data&0x80)  SDA_H;  // MOSI=1
          else            SDA_L;  // MOSI=0
          SCK_H;                  // SCK=1
          data=data<<1;  
        }
      }
  	}
    SCK_L;                        // SCK=0 
    SCE_H;                        // SSEL=1 Chip disabled
  }
/*************************************************************************************************/

