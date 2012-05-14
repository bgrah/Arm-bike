// Date:   14.5.2012
// Author: Blaž Grah

#include "gps_uart.h"
#include "uart.h"
#include "vic.h"

void uart0_read(void)   // Interrupt
{
  static int i;
  if((U0IIR & interrupt_pending) == 0)    //Je prisotna zahteva po prekinitvi
  {
    if((U0IIR & interrupt_id) == rx_data_available_id)  //Je na voljo podatek
    {
      gps_buf[i]=U0RBR;
      i++;
      if(i>=50)  U0TER = 0x00;
    }
    else
    {
      if((U0IIR & interrupt_id) == character_time_out_id) //Je bil sprožen char timeout?
      {
        U0THR;  // "Preberi" zakasneli podatek, da konèa z timeout prekinitev
      } 
    }
  }
  
  U0IIR=interrupt_pending; //Brisanje zahteve po prekinitvi
  VICVectAddr=0;
}
