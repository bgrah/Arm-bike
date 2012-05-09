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

/* GPIO Pin directions */
    int input = P0_16;                          // Mag. switch, 
    int output = (P0_4|P0_5|P0_6|P0_7|P0_13);   // GLCD (5), 
  set_pin_direction(input, output);
    
/* Timer1 init */
    int t_prescale = 0;
    int t_match[4] = {0,0,0,0}; // prekinitev se naredi naslednji urin cikel, kasneje spreminjaš T1MR0
    int control = 0;  // prekinitev in reset
    int count = timer; 
  timer1_init(t_prescale, t_match, control, count);    // Inicializacija timer1
    T1TCR = counter_enable;
    
/* PWM init */
    int p_output = pwmena5;     // Enabled outputs    [pwmenaX, X=1,2,3,4,5,6]
    int p_prescale = 0;       // Prescale value
    int p_match[7] = {15000,0,0,0,0,1*150,0};   // Match values
    int match_control = 0;    // Match control      [0,pwmmrXi,pwmmrXr or pwmmrXs, X=0,1,2,3,4,5,6]
    int edge_control = 0;     // Double edge controlled [pwmselX, X=2,3,4,5,6]
  pwm_init(p_output, p_prescale, p_match, match_control, edge_control);
    PWMTCR = pwm_enable;
  
/* NOKIA 5510 LCD init */
    int Vop = 0x3B;           // Sets Vop           [0-127] (def. 59 (0x3B))  !!!
    int tempC = 0;            // Sets temp coeff.   [0-3]   (def. 0)
    int biasS = 3;            // Sets bias system   [0-7]   (def. 3)
  GLCD_init(Vop, tempC, biasS);
    
	main();	// Start main program
	while(1);
}
