#include "gpio.h"   // For pin macros and IO0SET/IO0CLR registers

/* LCD pinout (5 GPIO pins + 3 supply pins) */
//   1    2    3      4      5    6     7     8
//  vcc  gnd  sce    rst    d/c  mosi  sck   led
//  3V3  gnd  P0.7  P0.15  P0.5  P0.6  P0.4  3V3*
// *-optional

/* Pin setting and clearing macros */
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

/* Instruction set mask */
#define function_set  0x20      // Set function     [sPD, sV, SH]  (H=0/1)
#define display_cr    0x08      // Display config   [display_blank, normal_mode, display_all_on, inverse_video]  (H=0)
#define set_y_addr    0x40      // Y addr for RAM   [0-5]   (H=0)
#define set_x_addr    0xF0      // X addr for RAM   [0-83]  (H=0)
#define temp_cr       0x04      // Set temp coeff.  [0-3]   (H=1)
#define bias_system   0x10      // Set bias system  [0-7]   (H=1)
#define set_Vop       0x80      // Set Vop          [0-127] (H=1)

/* Function set options mask */
#define sPD   0x04              // Power down[1] / Active[0] mode
#define sV    0x02              // Vertical[1] / Horizontal[0] addressing
#define sH    0x01              // Extended[1] / Basic[0] instruction set

/* Display configuration options mask */
#define display_blank   0x00
#define normal_mode     0x04
#define display_all_on  0x01
#define inverse_video   0x05

/* Function declaration */
extern void GLCD_init(int Vop, int tempC, int biasS);
extern void SPI_write(char data);
extern void GLCD_write_command(char command);
extern void GLCD_write_data(char data);
extern void GLCD_gotoxy(char xloc, char yloc);
extern void GLCD_clean_ddram(void);
extern void GLCD_putc(char data);
extern void GLCD_putstring(char* string);
extern void GLCD_putgraphic(char locx, char locy, char xpixel, char ypixel, char* graphic);
