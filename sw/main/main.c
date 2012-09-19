#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"
#include "mpr121.h"
#include "timer.h"

// Macroji
#define premikUre 2
#define osTimeSlice 200000  // 200ms
#define max_dispStates 4

// Funkcije
void readGpsData(void);
void dispGps(void);
void mpr121read(void);
void izracunStats(void);
void izracunGrafa(void);

// Razlièni prikazi na ekranu
void osnovniZaslon(void);
void grafVisina(void);
void grafHitrost(void);
void statusBar(void);
void dispStats(void);

// Zunanje uporabljene spremenljivke
extern unsigned char mpr121_rx_buf[20];
extern unsigned char rmcStr[100];
extern unsigned char ggaStr[100];
extern unsigned char gsaStr[100];
extern int gpsEndFlag;
extern int sekundeOn;   // St. sekund. Od vsakem koncu oddajanja GPS -> +1

// Tipke
int touchPress;     // Pritisnjene tipke
int dispState=0;    // Trenutno stanje prikaza
int dispChange=1;   // Zastavica o spremembi zaslona za brisanje
int dispChangeF=0;  // Zastavica o spremembi zaslona za posamezne funkcije prikaza

// Sateliti dobljeni?
int firstFixOk = 0;

// Shranjena statistika
int maxSpeed=0;
int maxAltitude=0;
int skupnaRazdalja=0;

// Prebrani podatki iz GPS stringov
char utcTime[10];             // hh:mm:ss'\0
int gpsFix=1;                 // 1,2,3
int satUsed=0;                // 0 -> 12
char kmhSpeed[10]="    km/h"; // Hitrost v km/h (string)
int kmhSpeedNum;              // Hitrost v km/h (int)
char latitudeDeg[15];         // g.širina N dd°mm',mmmm
char longitudeDeg[15];        // g.dolžina E ddd°mm',mmmm
char visinaStr[10]="     m";  // Nadmorska višina (znakovni niz)
int  visinaNum;               // Nadmorska višina (številka)



/* ----- Main ----- */
int main()
{
  int i=0;
  unsigned int timeStart=T1TC;
  
  while(1)
  { 
    
    if((T1TC-timeStart)>=osTimeSlice)   // Ko preteèe èasovna rezina
    {
      timeStart = T1TC;                 // Shrani si trenutni èas
      
      switch(i%5)                       // Opravila
      {
      case 0: mpr121read(); break;
      case 1: 
        if(gpsEndFlag)
        {
          readGpsData();
          gpsEndFlag = 0;
        } 
        break;
      case 2: mpr121read(); izracunStats(); izracunGrafa(); break;
      case 3: dispGps(); break;
      case 4: mpr121read(); break;
      }
      
      if(gpsFix > 1) firstFixOk = 1;
      
      i++;
    }   
    
  }
  return 0;
}

/* Branje tipk I2C */
void mpr121read()
{
  static int pressOld=0;
  int i;
  
  mpr121_read(ELE0_TS);
  touchPress = (int) mpr121_rx_buf[0];
  
  if(touchPress == pressOld);
  else
  {
    pressOld = touchPress;
    dispChange = 1;           // Potrebno brisanje ekrana, zaradi spremembe vsebine
    switch(touchPress)                                                         // Premikaj se z tipkami po razliènih ekranih (dispState)
    {
      case 1: if(dispState == (max_dispStates - 1)); else dispState++; break;
      case 2: if(dispState == 0); else dispState--; break;
      case 4: break;
      case 8: if(dispState == 1)  // Graf visina reset
                for(i=0;i<180;i++) grafVisinaData[i]=0;   
              if(dispState == 2)  // Graf hitrost reset
                for(i=0;i<180;i++) grafHitrostData[i]=0;
              if(dispState == 3)  // Statistika reset
              {
                maxSpeed=0;
                maxAltitude=0;
                skupnaRazdalja=0;
              }     
              break;
      case 15: break; //Troll :)
    }
  }
}

/* Izraèun statistike */
void izracunStats()
{
    if(kmhSpeedNum > maxSpeed) maxSpeed = kmhSpeedNum;    // Max speed
    if(visinaNum > maxAltitude) maxAltitude = visinaNum;  // Max višina
    skupnaRazdalja += (kmhSpeedNum*10)/36;                // Èas 1sec, razdalja ~~ hitrost v m/s
}

/* Zapis na graf */
void izracunGrafa()
{
  int temp;
  
  if(sekundeOn%60 == 0)
  {
    
   // Graf visine 
    switch(visinaNum/334)      // Do 1000m.
    {
    case 0: temp = visinaNum*10 / 425;            // x/42 da dobimo iz 334 vrednosti ven 8 vrednosti
            grafVisinaData[(sekundeOn%3600)/60 + 120] = 128 >> temp;  // Zapiši piko v pravi stolpec sestavljen iz 8 pik. 1 zašiftaš po višini
            break;
    case 1: temp = (visinaNum - 334)*10 / 425;
            grafVisinaData[(sekundeOn%3600)/60 + 60] = 128 >> temp;   // Index zbirke = 0-59 (minut) --> /60, max 60min -> % 3600sec, druga vrsta --> +60
            break;
    case 2: temp = (visinaNum - 668)*10 / 425;
            grafVisinaData[(sekundeOn%3600)/60] = 128 >> temp;
            break;
    }

  // Graf hitrosti
    switch(kmhSpeedNum/34)      // Do 100km/h.
    {
    case 0: temp = kmhSpeedNum*100 / 425;            // x/42 da dobimo iz 334 vrednosti ven 8 vrednosti
            grafHitrostData[(sekundeOn%3600)/60 + 120] = 128 >> temp;  // Zapiši piko v pravi stolpec sestavljen iz 8 pik. 1 zašiftaš po višini
            break;
    case 1: temp = (kmhSpeedNum - 34)*100 / 425;
            grafHitrostData[(sekundeOn%3600)/60 + 60] = 128 >> temp;   // Index zbirke = 0-59 (minut) --> /60, max 60min -> % 3600sec, druga vrsta --> +60
            break;
    case 2: temp = (kmhSpeedNum - 68)*100 / 425;
            grafHitrostData[(sekundeOn%3600)/60] = 128 >> temp;
            break;
    }
  }
}

/* Izpis/Izris podatkov na zaslon */
void dispGps()
{
  if(dispChange == 1)
  {
    dispChangeF = 1;
    GLCD_clean_ddram();
    dispChange = 0;
  }
  if(touchPress == 15){ GLCD_putgraphic(16, 0, 52, 6, trololo);return;}
  switch(dispState)
  {
    case 0: osnovniZaslon(); break;
    case 1: grafVisina();    break;
    case 2: grafHitrost();   break;
    case 3: dispStats();     break;
  }
}

/* Razliène konfiguracije na zaslonu */
void osnovniZaslon()
{
  static int i=0;
  
  if(gpsFix == 1)
  { 
    switch(i%50)
    {
      case 0:  GLCD_clean_ddram(); GLCD_putgraphic(0, 1, 48, 5, space1);  break;
      case 5:  GLCD_clean_ddram(); GLCD_putgraphic(9, 1, 48, 5, space1);  break;
      case 10: GLCD_clean_ddram(); GLCD_putgraphic(18, 1, 48, 5, space1); break;
      case 15: GLCD_clean_ddram(); GLCD_putgraphic(27, 1, 48, 5, space1); break;
      case 20: GLCD_clean_ddram(); GLCD_putgraphic(36, 1, 48, 5, space1); break;
      case 25: GLCD_clean_ddram(); GLCD_putgraphic(36, 1, 48, 5, space2); break;
      case 30: GLCD_clean_ddram(); GLCD_putgraphic(27, 1, 48, 5, space2); break;
      case 35: GLCD_clean_ddram(); GLCD_putgraphic(18, 1, 48, 5, space2); break;
      case 40: GLCD_clean_ddram(); GLCD_putgraphic(9, 1, 48, 5, space2);  break;
      case 45: GLCD_clean_ddram(); GLCD_putgraphic(0, 1, 48, 5, space2);  break;             
    }
    GLCD_gotoxy(0,0);
    GLCD_putstring("Cakam na GPS.");
    
    i++;
  }
  else
  {
    if(i>0)
    {
      GLCD_clean_ddram();
      i=0;
    }
    statusBar();
    
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
    GLCD_putch(mpr121_rx_buf[0]+64);      // @ not, A - 1, B - 2, D - 4, H - 8
  }
}

void statusBar()
{
  GLCD_gotoxy(0,0);      // Time
  
  if(firstFixOk) GLCD_putstring(utcTime);
  else GLCD_putstring("00:00:00");
  
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
}

void grafVisina()
{ 
  statusBar();
  if(dispChangeF)      // Naslov in osi grafa pošlji samo enkrat
  {
    GLCD_gotoxy(0,1);                        
    GLCD_putstring("Visina [m/min]");
    GLCD_putgraphic(0, 2, 84, 4, grafVisinaTmp);
    dispChangeF = 0;
  }   
  
  GLCD_putgraphic(23, 2, 60, 3, grafVisinaData);
}

void grafHitrost()
{ 
  statusBar();
  if(dispChangeF)      // Naslov in osi grafa pošlji samo enkrat
  {
    GLCD_gotoxy(0,1);      
    GLCD_putstring("Hit. [kmh/min]");
    GLCD_putgraphic(0, 2, 84, 4, grafHitrostTmp);
    dispChangeF = 0;
  }    

  GLCD_putgraphic(23, 2, 60, 3, grafHitrostData);  
}

void dispStats()
{
  char hitrost[10]  ="    km/h";
  char visina[10]   ="     m";
  char razdalja[10] ="       m";
  
  statusBar();
  GLCD_gotoxy(0,1);      
  GLCD_putstring("Statistika:");
  
  // Max hitrost
    GLCD_gotoxy(0,2);      
    if(maxSpeed>=100)
    {
      hitrost[0] = (char) ((maxSpeed / 100) + 48);
      hitrost[1] = (char) ((maxSpeed-(maxSpeed/100)*100)/10 + 48);
      hitrost[2] = (char) ((maxSpeed % 10) + 48);
    }
    else if(maxSpeed>=10)
    {
      hitrost[0] = ' ';
      hitrost[1] = (char) ((maxSpeed-(maxSpeed/100)*100)/10 + 48);
      hitrost[2] = (char) ((maxSpeed % 10) + 48);
    }
    else
    {
      hitrost[0] = ' ';
      hitrost[1] = ' ';
      hitrost[2] = (char) ((maxSpeed % 10) + 48);
    }
    GLCD_putstring("vmax= ");
    GLCD_putstring(hitrost);
    
  // Max višina
    GLCD_gotoxy(0,3);
    
    visina[0] = (char) ((maxAltitude / 1000) + 48);
    visina[1] = (char) ((maxAltitude-(maxAltitude/1000)*1000)/100 + 48);
    visina[2] = (char) ((maxAltitude % 100)/10 + 48);
    visina[3] = (char) ((maxAltitude % 10) + 48);
    
    GLCD_putstring("hmax= ");
    GLCD_putstring(visina);
    
  // Skupna razdalja
    GLCD_gotoxy(0,4);

    razdalja[0] = (char) ((skupnaRazdalja % 1000000)/100000 + 48);
    razdalja[1] = (char) ((skupnaRazdalja % 100000)/10000 + 48);
    razdalja[2] = (char) ((skupnaRazdalja % 10000)/1000 + 48);
    razdalja[3] = (char) ((skupnaRazdalja % 1000)/100 + 48);
    razdalja[4] = (char) ((skupnaRazdalja % 100)/10 + 48);
    razdalja[5] = (char) ((skupnaRazdalja % 10) + 48);    
    
    GLCD_putstring("odo= ");
    GLCD_putstring(razdalja);    
    
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
    kmhSpeedNum = i;
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
    if(rmcStr[25] == '0') longitudeDeg[2] = ' ';
    else longitudeDeg[2] = rmcStr[25];  // Degrees
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
