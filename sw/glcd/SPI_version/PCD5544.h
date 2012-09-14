// Date:   29.4.2012
// Author: Blaž Grah

#include "gpio.h"   // For pin masks
#include "spi.h"    // For SPI

// Instruction set mask
#define function_set  0x20      // Set function     [sPD, sV, SH]  (H=0/1)
#define display_cr    0x08      // Display config   [display_blank, normal_mode, display_all_on, inverse_video]  (H=0)
#define set_y_addr    0x40      // Y addr for RAM   [0-5]   (H=0)
#define set_x_addr    0xf0      // X addr for RAM   [0-83]  (H=0)
#define temp_cr       0x04      // Set temp coeff.  [0-3]   (H=1)
#define bias_system   0x10      // Set bias system  [0-7]   (H=1)
#define set_Vop       0x80      // Set Vop          [0-127] (H=1)

// Function set options mask
#define sPD   0x04    // Power down[1] / Active[0] mode
#define sV    0x02    // Vertical[1] / Horizontal[0] addressing
#define sH    0x01    // Extended[1] / Basic[0] instruction set

// Display configuration options mask
#define display_blank   0x00
#define normal_mode     0x04
#define display_all_on  0x01
#define inverse_video   0x05

// Function declaration
extern int dc_pinM;          // Holds dc pin mask
extern void LCD_init(int reset, int d_c, int Vop, int tempC, int biasS);
extern void LCD_write(int data);
extern void LCD_write_comm(int type, int param);
          
