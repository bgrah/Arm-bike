#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"
//#include "pwm.h"
#include "mpr121.h"

#define premikUre 2

void readGpsData(void);

// Zunanje uporabljene spremenljivke
extern unsigned char mpr121_rx_buf[20];
extern unsigned char rmcStr[100];
extern unsigned char ggaStr[100];
extern unsigned char gsaStr[100];
extern int gpsEndFlag;

// Prebrani podatki iz GPS stringov
char utcTime[10];             // hh:mm:ss'\0
int gpsFix=1;                 // 1,2,3
int satUsed=0;                // 0 -> 12
char kmhSpeed[10]="    km/h";            // Hitrost v vozlih
char latitudeDeg[15];         // g.širina N dd°mm',mmmm
char longitudeDeg[15];        // g.dolžina E ddd°mm',mmmm
char visinaStr[10]="     m";  // Nadmorska višina (znakovni niz)
int  visinaNum;               // Nadmorska višina (številka)


/* ----- Main ----- */
int main()
{
  int i=1;
  while(1)
  { 
    if(gpsFix == 1)
    {
      GLCD_gotoxy(0,0);
      GLCD_putstring("Cakam na GPS.");
      if((i%300) == 0) {GLCD_clean_ddram(); GLCD_putgraphic(0, 1, 48, 5, space1);}
      if((i%300) == 30) {GLCD_clean_ddram(); GLCD_putgraphic(9, 1, 48, 5, space1);}
      if((i%300) == 60) {GLCD_clean_ddram(); GLCD_putgraphic(18, 1, 48, 5, space1);}
      if((i%300) == 90) {GLCD_clean_ddram(); GLCD_putgraphic(27, 1, 48, 5, space1);}
      if((i%300) == 120) {GLCD_clean_ddram(); GLCD_putgraphic(35, 1, 48, 5, space1);}
      if((i%300) == 150) {GLCD_clean_ddram(); GLCD_putgraphic(35, 1, 48, 5, space2);}
      if((i%300) == 180) {GLCD_clean_ddram(); GLCD_putgraphic(27, 1, 48, 5, space2);}
      if((i%300) == 210) {GLCD_clean_ddram(); GLCD_putgraphic(18, 1, 48, 5, space2);}
      if((i%300) == 240) {GLCD_clean_ddram(); GLCD_putgraphic(9, 1, 48, 5, space2);}
      if((i%300) == 270) {GLCD_clean_ddram(); GLCD_putgraphic(0, 1, 48, 5, space2);}
      i++;
    }
    else
    {
      if(i>0)
      {
        GLCD_clean_ddram();
        i=0;
      }
      GLCD_gotoxy(0,0);      // Time
      GLCD_putstring(utcTime);
      
      switch(gpsFix)   // Èe je GPS fix, prikaz 2D, 3D
      {
        case 1: GLCD_putgraphic(55, 0, 8, 1, fixno); break; // No fix
        case 2: GLCD_putgraphic(55, 0, 8, 1, fix2D); break; // 2D fix
        case 3: GLCD_putgraphic(55, 0, 8, 1, fix3D); break; // 3D fix
      }
        
      switch(satUsed)  // Število uporabljenih satelitov, prikazano z stolpci
      {
        case 3: GLCD_putgraphic(67, 0, 9, 1, sig1); break;
        case 4: GLCD_putgraphic(67, 0, 9, 1, sig2); break;
        case 5: GLCD_putgraphic(67, 0, 9, 1, sig3); break;
        case 6: GLCD_putgraphic(67, 0, 9, 1, sig4); break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12: GLCD_putgraphic(68, 0, 9, 1, sig5); break;
        default: GLCD_putgraphic(68, 0, 9, 1, sig0); break;
      }
      
      switch(1)  // Battery status =) -- trenutno še ne dela animirano
      {
        case 1: GLCD_putgraphic(80, 0, 4, 1, bat3); break;
      } 
      
      GLCD_gotoxy(0,2);              // Prikaz hitrosti
      GLCD_putstring("v =  ");
      GLCD_putstring(kmhSpeed);
      
      GLCD_gotoxy(0,3);              // Prikaz nadmorske višine
      GLCD_putstring("h = ");
      GLCD_putstring(visinaStr); 
      
      GLCD_gotoxy(0,4);              // Prikaz g.širine
      GLCD_putstring(latitudeDeg);
      
      GLCD_gotoxy(0,5);              // Prikaz g.dolžine
      GLCD_putstring(longitudeDeg);

      GLCD_gotoxy(78,5);
      mpr121_read(ELE0_TS);
      GLCD_putch(mpr121_rx_buf[0]+64);      // @ not, A - 1, B - 2, D - 4, H - 8
    }
   
    wait(300000);
    
    if(gpsEndFlag)
    {
      readGpsData();
      gpsEndFlag = 0;
    } 
    
  }
  return 0;
}

/* Vzame znakovne nize iz GPS in shrani uporabne podatke v posamezne znakovne nize ter številène vrednosti */
void readGpsData(void)
{
  int i,j;
/* GGA stavek */
  // UTC time
    i = (ggaStr[0]-48)*10 + (ggaStr[1]-48) + premikUre;
    if(i>23) i=i-24;
    utcTime[0] = (char) ((i/10)+48);    // Hour
    utcTime[1] = (char) ((i%10)+48);
    utcTime[2] = ':';
    utcTime[3] = ggaStr[2];    // Min
    utcTime[4] = ggaStr[3];
    utcTime[5] = ':';
    utcTime[6] = ggaStr[4];    // Sec
    utcTime[7] = ggaStr[5];
    utcTime[8] = 0;
  
  // Satelites used
    satUsed = (ggaStr[39]-48)*10 + (ggaStr[40]-48);
  
  // Mean sea level altitude
    j=0;
    for(i=0;ggaStr[i] != 0; i++)    // Gre z indeksom i do osme vejice
    {
      if(ggaStr[i] == ',') j++;
      if(j==8) break;
    }
    for(j=1; ggaStr[i+j] != '.'; j++);  // J vrne koliko števk ima višina
    
    switch(j-1) // j kaže na mesto pike, j-1 pa pove število cifer pred piko
    {
      case 1: visinaStr[0] = ' '; visinaStr[1] = ' '; visinaStr[2] = ' '; visinaStr[3] = ggaStr[i+1];
              visinaNum = ggaStr[i+1]-48; break;
      case 2: visinaStr[0] = ' '; visinaStr[1] = ' '; visinaStr[2] = ggaStr[i+1]; visinaStr[3] = ggaStr[i+2];
              visinaNum = (ggaStr[i+1]-48)*10 + (ggaStr[i+2]-48); break;
      case 3: visinaStr[0] = ' '; visinaStr[1] = ggaStr[i+1]; visinaStr[2] = ggaStr[i+2]; visinaStr[3] = ggaStr[i+3];
              visinaNum = (ggaStr[i+1]-48)*100 + (ggaStr[i+2]-48)*10 + (ggaStr[i+3]-48); break;
      case 4: visinaStr[0] = ggaStr[i+1]; visinaStr[1] = ggaStr[i+2]; visinaStr[2] = ggaStr[i+3]; visinaStr[3] = ggaStr[i+4];
              visinaNum = (ggaStr[i+1]-48)*1000 + (ggaStr[i+2]-48)*100 + (ggaStr[i+3]-48)*10 + (ggaStr[i+4]-48); break;
    }
  
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
    if(i>=100)
    {
      kmhSpeed[0] = (char) ((i / 100) + 48);
      kmhSpeed[1] = (char) ((i-(i/100)*100)/10 + 48);
      kmhSpeed[2] = (char) ((i % 10) + 48);
    }
    else if(i>=10)
    {
      kmhSpeed[0] = ' ';
      kmhSpeed[1] = (char) ((i-(i/100)*100)/10 + 48);
      kmhSpeed[2] = (char) ((i % 10) + 48);
    }
    else
    {
      kmhSpeed[0] = ' ';
      kmhSpeed[1] = ' ';
      kmhSpeed[2] = (char) ((i % 10) + 48);
    }

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
