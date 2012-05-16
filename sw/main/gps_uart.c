// Date:   14.5.2012
// Author: Blaž Grah

#include "gps_uart.h"
#include "uart.h"
#include "vic.h"
unsigned char gps_buf[80];
void uart0_read(void)   // Interrupt
{
  static int i,j;
  unsigned char data_rx;
  if((U0IIR & interrupt_pending) == 0)    //Je prisotna zahteva po prekinitvi
  {
    if((U0IIR & interrupt_id) == rx_data_available_id)  //Je na voljo podatek
    {
      data_rx=U0RBR;
      if(data_rx == '$') {i=0; j=0;}
      if((data_rx == 'G') && (i==1))  {j++; }
      if((data_rx == 'P') && (i==2))  {j++; }
      if((data_rx == 'G') && (i==3))  {j++; }
      if((data_rx == 'G') && (i==4))  {j++; }
      if((data_rx == 'A') && (i==5))  {j++;  i=0;}
      if(j==5){   if((data_rx != 0x0A) && (data_rx != 0x0D)){  // CR and NL
                    gps_buf[i]=data_rx;}
                  else
                    gps_buf[i]=0; // NULL
                  uart1_sent(data_rx);
                  
              }
      i++;
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

void uart1_sent(unsigned char data)
{
  
  U1THR = data;

}
