#include "init.h"
#include "spi.h"
#include "timer.h"
#include "pwm.h"


extern int main();

void start_up()
{
/* MCU init */
    int clock_mhz = 60;       // MCU clk speed      [12,24,36,48,60] (MHz)
  	int vpbdiv = cclk_4;      // fvpb divider       [cclk,cclk_2,cclk_4]
	init(clock_mhz, vpbdiv);
    
    set_pin_direction(P0_16, (P0_4|P0_5|P0_6|P0_7|P0_13));  // GLCD, magSwitch _temp
    
/* Timer1 init */
    int t_prescale = 0;
    int t_match[4] = {0,0,0,0}; // prekinitev se naredi naslednji urin cikel, kasneje spreminja� T1MR0
    int control = 0;  // prekinitev in reset
    int count = timer; 
  timer1_init(t_prescale, t_match, control, count);    // Inicializacija timer1
    T1TCR = counter_enable;
    
/* PWM init */
    int output = pwmena5;     // Enabled outputs    [pwmenaX, X=1,2,3,4,5,6]
    int p_prescale = 0;         // Prescale value
    int p_match[7] = {15000,0,0,0,0,1*150,0};   // Match values
    int match_control = 0;    // Match control      [0,pwmmrXi,pwmmrXr or pwmmrXs, X=0,1,2,3,4,5,6]
    int edge_control = 0;     // Double edge controlled [pwmselX, X=2,3,4,5,6]
  pwm_init(output, p_prescale, p_match, match_control, edge_control);
    PWMTCR = pwm_enable;
 
/* SPI0 Init 
    int length = 8;           // Data lenght        [8, 9, 10, 11, 12, 13, 14, 15 or 16]
    int mode = high_rising;   // CPOL and CPHA      [high_rising, high_falling, low_rising or low_falling]
    int type = master;        // Device type        [master, slave]
    int first = msb;          // First transferred  [lsb, msb]
    int interrupt = 0;        // Interrupt enable   [0,1]
    int div = 20;             // fvpb divider       [8 -> 254] (clk = fvpb/div)
    int miso = 0;             // MISO enable        [0,1]
  spi_init(length, mode, type, first, interrupt, div, miso);
*/  
/* NOKIA 5510 LCD init 
    int reset = P0_15;        // Reset pin          [gpio.h]
    int d_c = P0_5;           // D/C pin            [gpio.h]
    int Vop = 0xc8;             // Sets Vop           [0-127] (def. 49 (0x31))  !!!
    int tempC = 2;            // Sets temp coeff.   [0-3]   (def. 3)
    int biasS = 3;            // Sets bias system   [0-7]   (def. 3)
  LCD_init(reset, d_c, Vop, tempC, biasS);
*/
	main();	// Start main program
	while(1);
}
