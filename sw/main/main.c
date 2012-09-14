#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"
//#include "pwm.h"
#include "mpr121.h"

//void pwm_duty(int duty);
extern unsigned char mpr121_rx_buf[20];
extern unsigned char rmcStr[100];
extern unsigned char ggaStr[100];
extern unsigned char gsaStr[100];

extern int dispFlag;

int main()
{
  while(1)
  {
    if(dispFlag)
    {
      GLCD_gotoxy(0,0);
      mpr121_read(ELE0_TS);
      GLCD_putch(mpr121_rx_buf[0]+64);      // @ not, A - 1, B - 2, D - 4, H - 8
      
      GLCD_gotoxy(0,1);
      GLCD_putstring(rmcStr);
      dispFlag = 0;
    }    
  }
  return 0;
}



/*
void pwm_duty(int duty)
{
   //int period=fvpb/frekvenca;
   int period = 15000;
   PWMMR0=period;       // Dolžina periode v fvpb
   period=(period*duty)/100;
   PWMMR5=period;   //vklop  signala 1 p0.7
   PWMLER=0x00000021;
}
*/
