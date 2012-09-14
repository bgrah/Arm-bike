#include "init.h"
//#include "spi.h"
#include "timer.h"
//#include "pwm.h"
#include "vic.h"
#include "i2c.h"
#include "mpr121.h"
#include "uart.h"
#include "gps_uart.h"
//#include "extint.h"

extern int main();

void start_up()
{
/* MCU init */
    int clock_mhz = 60;         // MCU clk speed      [12,24,36,48,60] (MHz)
  	int vpbdiv = cclk_4;        // fvpb divider       [cclk,cclk_2,cclk_4]
	init(clock_mhz, vpbdiv);

/* GPIO Pin directions */
    int input = 0;                           
    int output = (P0_4|P0_5|P0_6|P0_7|P0_13);   // GLCD (5), 
  set_pin_direction(input, output);
    
/* Timer1 init */
    int t_prescale = 0;         // Timer prescale
    int t_match[4] = {0,0,0,0}; // Match values
    int control = 0;            // Match control      [combination of 0,mrXi, mrXr and mrXs, X=0,1,2,3]
    int count = timer;          // Count control      [timer, counter_rising | capX, counter_falling | capX,or counter_both | capX, where X = 0,1,2,3]                
  timer1_init(t_prescale, t_match, control, count);
    
/* PWM init */
/*    int p_output = pwmena5;     // Enabled outputs        [pwmenaX, X=1,2,3,4,5,6]
    int p_prescale = 0;         // Prescale value
    int p_match[7] = {15000,0,0,0,0,1*150,0};   // Match values
    int match_control = 0;      // Match control          [0,pwmmrXi,pwmmrXr or pwmmrXs, X=0,1,2,3,4,5,6]
    int edge_control = 0;       // Double edge controlled [pwmselX, X=2,3,4,5,6]
  pwm_init(p_output, p_prescale, p_match, match_control, edge_control);
*/    
/* UART0 init */ 
    int baud_rate=9600;
    int word_length=word_length_8_bit;
    int stop_bit=one_stop_bit;
    int parity=disable_parity;
  	int parity_type=0;
    int interrupts=rx_data_available;
  uart0_init(baud_rate, word_length, stop_bit, parity,	parity_type, interrupts);    
    
/* UART1 init */ 
    int baud1_rate=57600;
    int word1_length=word_length_8_bit;
    int stop1_bit=one_stop_bit;
    int parity1=disable_parity;
  	int parity1_type=0;
    int handshake1=0;
    int interrupts1=0;
  uart1_init(baud1_rate, word1_length, stop1_bit, parity1,	parity1_type, handshake1, interrupts1);   
    
/* I2C0 init */
    int slave_address = 0;    // 7 bit address of this i2c in slave mode
    int general_call = 0;     // If 1 -> recognise general call     (if slave_address and general_call == 0 -> master only mode)
    int i_duty_H = 200;       // SCK high length in fvpb ticks  [200/200 for 37,5kHz, 160/160 for 50kHz, 30/30 for 250kHz]  (check the bus for signal rise time!!!)
    int i_duty_L = 200;       // SCK low length in fvpb ticks   [formula: i2c_speed=fvpb/(high_ticks+low_ticks)]  (max i2c speed = 400kHz)
    int i_duty = (i_duty_H << 16) | i_duty_L;
    char tx_buf[20];          // Pointer to data for slave transmitt mode   (not used)
  i2c0_init(slave_address, general_call, i_duty, tx_buf);   
    
/* Externa interrupt init */
/*    int pinmask0 = P0_15_EINT2;
    int pinmask1 = 0;    // P0.16
    int wake = 0;
    int sensitivity = EXTMODE2;    // Proženje z fronto
    int trig = 0;                  // Padajoèa fronta
  extint_init(pinmask0, pinmask1, wake, sensitivity, trig); 
*/    
/* VIC init */
    int fiq = 0;              // FIQ mask determines which interrupts are FIQ
    int irq = i2c0 | uart0;   // IRQ mask determines which interrupts are IRQ
    voidfuncptr funk[16] = {handle_i2c0_state,uart0_read,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // Array of pointers to ISRs for each sloted IRQ
    int interrupt[16] = {i2c0,uart0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // Array of sloted IRQs
    voidfuncptr def = 0;      // pointer to unsloted ISR
  vic_init(fiq, irq, funk, interrupt, def);  

  T1TCR = counter_enable;   // Timer1
//  PWMTCR = pwm_enable;      // PWM  
  U0TER = txen;             // UART0
  U1TER = txen;             // UART1
  I2C0CONSET = i2enc;       // I2C0
    
/* MPR121 Touch sensor init */
  mpr121_init();
    
/* NOKIA 5510 LCD init */
    int Vop = 0x3B;           // Sets Vop           [0-127] (def. 59 (0x3B))  !!!
    int tempC = 0;            // Sets temp coeff.   [0-3]   (def. 0)
    int biasS = 3;            // Sets bias system   [0-7]   (def. 3)
  GLCD_init(Vop, tempC, biasS);
    
	main();	// Start main program
	while(1);
}
