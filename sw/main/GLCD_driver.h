#include "gpio.h"

//definitions and Macros==============================================
#define DC_H    IO0SET=P0_5
#define DC_L    IO0CLR=P0_5
#define SCE_H   IO0SET=P0_7
#define SCE_L   IO0CLR=P0_7
#define RST_H	  IO0SET=P0_13
#define RST_L	  IO0CLR=P0_13
#define SCK_H	  IO0SET=P0_4
#define SCK_L	  IO0CLR=P0_4
#define SDA_H	  IO0SET=P0_6
#define SDA_L	  IO0CLR=P0_6

extern void SPI_write(char data);
extern void GLCD_write_command(char command);
extern void GLCD_write_data(char data);
extern void GLCD_gotoxy(char xloc, char yloc);
extern void GLCD_clean_ddram(void);
extern void GLCD_init(void);
extern void GLCD_putc(char data);
extern void GLCD_putstring(char* string);
extern void GLCD_putgraphic(char locx, char locy, char xpixel, char ypixel, char* graphic);
