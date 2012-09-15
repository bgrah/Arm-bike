// Date:   14.5.2012
// Author: Blaž Grah

#include "gps_uart.h"
#include "uart.h"
#include "vic.h"

unsigned char rmcStr[100] = {0};
unsigned char ggaStr[100] = {0};
unsigned char gsaStr[100] = {0};
int gpsEndFlag = 0;

void uart0_read(void)   // Interrupt
{
  static int i=0;
  static unsigned char gpsStrId[3];
  static short gpsId;
  unsigned char data_rx;
  
  if((U0IIR & interrupt_pending) == 0)    //Je prisotna zahteva po prekinitvi
  {
    if((U0IIR & interrupt_id) == rx_data_available_id)  //Je na voljo podatek
    {
      data_rx=U0RBR;        // Preberi podatek
      uart1_sent(data_rx);  // Pošlji na PC-serial

      if((data_rx == '$') && (i==0))  // Zaèetek GPS stavka
      {
        i = 1;  
        gpsId = 0;
      }
      
      if(i==5) gpsStrId[0] = data_rx;      // Id stavka (2 èrki)
      if(i==6)
      {
        gpsStrId[1] = data_rx;
        
        if((gpsStrId[0]=='M')&&(gpsStrId[1]=='C')) gpsId = 1;   // RMC stavek  Id = 1
        if((gpsStrId[0]=='G')&&(gpsStrId[1]=='A')) gpsId = 2;   // GGA stavek  Id = 2
        if((gpsStrId[0]=='S')&&(gpsStrId[1]=='A')) gpsId = 3;   // GSA stavek  Id = 3
        if((gpsStrId[0]=='L')&&(gpsStrId[1]=='L')) gpsId = 4;   // GLL stavek  Id = 4
      }

      if(data_rx == 13)   // <CR>, zadnji znak stavka
      {
        i = 0;
        if(gpsId == 4) gpsEndFlag = 1;  // Ko je konec GLL stavka, zaèni prikaz
      }      
      
      if(i>=8)    // Zaèetek podatkov
      {
        if(gpsId == 1)
        {
          rmcStr[i-8] = data_rx;
          rmcStr[i-7] = 0;
        }
        if(gpsId == 2)
        {
          ggaStr[i-8] = data_rx;
          ggaStr[i-7] = 0;
        }
        if(gpsId == 3)
        {
          gsaStr[i-8] = data_rx;
          gsaStr[i-7] = 0;
        }
      }
      
      if(i>0) i++;        // Poveèuj indeks, èe si zaèel stavek
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
