#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"
#include "pwm.h"
#include "mpr121.h"

void pwm_duty(int duty);
extern unsigned char mpr121_rx_buf[];
unsigned char gps_buf[50];

int main()
{
  int smer;
  int i=0;

  //GLCD_putgraphic(0,0, 84, 6, symbols);
  //wait(1000000);

  while(1)
  {
//   if((IO0PIN & P0_16)==P0_16) GLCD_putgraphic(0,0, 84, 6, symbols);
//   else  GLCD_clean_ddram();

   /* 
    if(i==100) smer=-1;
    else if(i==0) smer=1;
    i+=smer;
    pwm_duty(i);
    wait(100000);
   */
    
  mpr121_read(ELE0_TS);
  if((mpr121_rx_buf[0]+64)=='O') GLCD_putgraphic(16,0, 52, 6, trololo);

  GLCD_gotoxy(0,0);
  GLCD_putch(mpr121_rx_buf[0]+64);      // @ not, A - 1, B - 2, D - 4, H - 8
 //GLCD_putstring(gps_buf);
  }
  return 0;
}

void pwm_duty(int duty)
{
   //int period=fvpb/frekvenca;
   int period = 15000;
   PWMMR0=period;       // Dolžina periode v fvpb
   period=(period*duty)/100;
   PWMMR5=period;   //vklop  signala 1 p0.7
   PWMLER=0x00000021;
}
