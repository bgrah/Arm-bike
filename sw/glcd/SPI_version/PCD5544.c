// Date:   29.4.2012
// Author: Blaž Grah

#include "PCD5544.h"

// Must initialize SPI0 (no need for MISO)

// Pinout (SPI0 and 2 GPIO pins)
//   1    2    3      4       5     6     7     8
//  vcc  gnd  sce    rst     d/c   mosi  sck   led
//  3V3  gnd  P0.7  P0.15*  P0.5*  P0.6  P0.4  3V3
// *-optional

/* Initializes PCD5544 controler */
  // reset    ... Pin mask for /RST signal
  // d_c      ... Pin mask for D/C signal
  // Vop      ... Sets Vop           [0-127] (def. 49 (0x31))  !!! Read datasheet before change
  // tempC    ... Sets temp coeff.   [0-3]   (def. 3)
  // biasS    ... Sets bias system   [0-7]   (def. 3)
  int dc_pinM    = 0;
  void LCD_init(int reset, int d_c, int Vop, int tempC, int biasS)
  {
    dc_pinM = d_c;        // Save D/C pin to global    
    set_pin_direction(0, reset | d_c);  // Set reset and d/c pins to outputs
    
    IO0CLR = reset;   // Resets LCD
    wait(50000);      // Wait 50ms to reset
    IO0SET = reset;   // End reset
    
    LCD_write_comm(function_set, 0);          // Active mode, horizontal, extended command
    LCD_write_comm(set_Vop, Vop);             // Sets voltage for LCD   !!! datasheet
    LCD_write_comm(temp_cr, tempC);           // Sets temp. coefficient
    LCD_write_comm(bias_system, biasS);       // Sets bias system
    LCD_write_comm(display_cr, normal_mode);  // Normal LCD mode
  }

/* Writes one byte data to LCD */
  // data     ... Data to send
  void LCD_write(int data)
  { 
    int delay;
    IO0SET = dc_pinM;   // D/C = 1 (data)
      
    S0SPDR = data;      // Send data
    //while(!(S0SPSR & spif));  // Wait to transmitt
    delay=50; while(delay>0)delay--;
  }

/* Writes one byte command to LCD */
  // type     ... Function type mask
  // param    ... Funtion parameter mask
  void LCD_write_comm(int type, int param)
  {
    int delay;
    IO0CLR = dc_pinM;   // D/C = 0 (command)
    
    if((type == temp_cr) || (type == bias_system) || (type == set_Vop))
    {
      S0SPDR = function_set | sH;       // Extended commands
      //while(!(S0SPSR & spif));            // Wait to transmitt
      delay=50; while(delay>0)delay--;
      S0SPDR = type | param;            // Send command
      //while(!(S0SPSR & spif));            // Wait to transmitt
      delay=50; while(delay>0)delay--;
      S0SPDR = function_set;            // Back to normal commands
      //while(!(S0SPSR & spif));            // Wait to transmitt
      delay=50; while(delay>0)delay--;
    }
    else
    {
      S0SPDR = (type | param);          // Send command
      //while(!(S0SPSR & spif));            // Wait to transmitt
      delay=50; while(delay>0)delay--;
    }
  } 
