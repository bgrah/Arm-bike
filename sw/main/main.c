#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"
//#include "pwm.h"
#include "mpr121.h"

void readGpsData(void);

//void pwm_duty(int duty);
extern unsigned char mpr121_rx_buf[20];
extern unsigned char rmcStr[100];
extern unsigned char ggaStr[100];
extern unsigned char gsaStr[100];

extern int gpsEndFlag;

// Prebrani podatki iz GPS stringov
char utcTime[10];       // hh:mm:ss'\0
int gpsFix=1;             // 1,2,3
char satUsed[3];        // "00" -> "12"
char kmhSpeed[10];      // Hitrost v vozlih
char latitudeDeg[15];   // g.širina N dd°mm',mmmm
char longitudeDeg[15];  // g.dolžina E ddd°mm',mmmm

int main()
{
  while(1)
  {
    GLCD_gotoxy(77,1);
    mpr121_read(ELE0_TS);
    GLCD_putch(mpr121_rx_buf[0]+64);      // @ not, A - 1, B - 2, D - 4, H - 8
    
    GLCD_gotoxy(0,0);
    GLCD_putstring(utcTime);
    
    GLCD_gotoxy(0,1);
    GLCD_putstring("Fix: ");
    GLCD_putch((char)(gpsFix+48));
    
    GLCD_gotoxy(0,2);
    GLCD_putstring("Satelites: ");
    GLCD_putstring(satUsed);
    
    GLCD_gotoxy(0,3);
    GLCD_putstring("Speed: ");
    GLCD_putstring(kmhSpeed);
    
    GLCD_gotoxy(0,4);
    GLCD_putstring(latitudeDeg);
    
    GLCD_gotoxy(0,5);
    GLCD_putstring(longitudeDeg);
    
    wait(300000);
    
    if(gpsEndFlag)
    {
      readGpsData();
      gpsEndFlag = 0;
    } 
    
  }
  return 0;
}

void readGpsData(void)
{
  int i;
/* GGA stavek */
  // UTC time
    utcTime[0] = ggaStr[0];    // Hour
    utcTime[1] = ggaStr[1];
    utcTime[2] = ':';
    utcTime[3] = ggaStr[2];    // Min
    utcTime[4] = ggaStr[3];
    utcTime[5] = ':';
    utcTime[6] = ggaStr[4];    // Sec
    utcTime[7] = ggaStr[5];
    utcTime[8] = 0;
  // Satelites used
    satUsed[0] = ggaStr[39];
    satUsed[1] = ggaStr[40];
    satUsed[2] = 0;
  
/* GSA stavek */
  // FIX status
    gpsFix = gsaStr[2]-48;     // 1-no, 2-2D, 3-3D
/* RMC stavek */
  // km/h speed 
    for(i=39;rmcStr[i] != '.';i++); // Do decimalne pike
    switch(i-40)     // Prikaz odvisen koliko cifer pred decimalko. Max 2 -> max 184km/h
    {
      case 1: i = (rmcStr[39]-48)*10 + (rmcStr[41]-48); break;                          // Hitrost v vozlih * 10.
      case 2: i = (rmcStr[39]-48)*100 + (rmcStr[40]-48)*10 + (rmcStr[42]-48); break;    // Upoštevam samo eno decimalko.
      default: i = 0;
    }
    i = (i*1852)/10000;             // Iz vozlov v km/h (x1.852), upoštevaj še eno decimalko.
    kmhSpeed[0] = (char) ((i / 100) + 48);
    kmhSpeed[1] = (char) ((i-(i/100)*100)/10 + 48);
    kmhSpeed[2] = (char) ((i % 10) + 48);
    kmhSpeed[3] = 'k';
    kmhSpeed[4] = 'm';
    kmhSpeed[5] = '/';
    kmhSpeed[6] = 'h';
    kmhSpeed[7] = 0;
  // Longitude (g.dolžina)
    longitudeDeg[0] = rmcStr[37];  // West/East
    longitudeDeg[1] = ' ';
    longitudeDeg[2] = rmcStr[25];  // Degrees
    longitudeDeg[3] = rmcStr[26];
    longitudeDeg[4] = rmcStr[27];
    longitudeDeg[5] = 123;         // °
    longitudeDeg[6] = rmcStr[28];  // Minutes
    longitudeDeg[7] = rmcStr[29];
    longitudeDeg[8] = 39;          // '   
    i = (((rmcStr[31]-48)*10000 + (rmcStr[32]-48)*1000 + (rmcStr[33]-48)*100 + (rmcStr[34]-48)*10 + (rmcStr[35]-48))*60)/100000;  // iz decimalk minut v sekunde
    longitudeDeg[9] = (char) ((i / 10) + 48);    // Seconds
    longitudeDeg[10] = (char) ((i % 10) + 48);
    longitudeDeg[11] = 34;         // "
    longitudeDeg[12] = 0;    
  // Latitude (g.širina)
    latitudeDeg[0] = rmcStr[23];  // North/South
    latitudeDeg[1] = ' ';
    latitudeDeg[2] = ' ';    
    latitudeDeg[3] = rmcStr[12];  // Degrees
    latitudeDeg[4] = rmcStr[13];
    latitudeDeg[5] = 123;         // °
    latitudeDeg[6] = rmcStr[14];  // Minutes
    latitudeDeg[7] = rmcStr[15];
    latitudeDeg[8] = 39;          // '   
    i = (((rmcStr[17]-48)*10000 + (rmcStr[18]-48)*1000 + (rmcStr[19]-48)*100 + (rmcStr[20]-48)*10 + (rmcStr[21]-48))*60)/100000;  // iz decimalk minut v sekunde
    latitudeDeg[9] = (char) ((i / 10) + 48);    // Seconds
    latitudeDeg[10] = (char) ((i % 10) + 48);
    latitudeDeg[11] = 34;          // "
    latitudeDeg[12] = 0;
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
