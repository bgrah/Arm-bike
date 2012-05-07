//#include "PCD5544.h"

#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"
#include "pwm.h"

void pwm_duty(int duty);

int main()
{
  int smer;
  int i=0;
  
  GLCD_init();
  while(1)
  {
    if((IO0PIN & P0_16)==P0_16) GLCD_putgraphic(16,0, 52, 6, trololo);
    else  GLCD_clean_ddram();
    
    if(i==100) smer=-1;
    else if(i==0) smer=1;
    i+=smer;
    pwm_duty(i);
    wait(100000);
    
      

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
