                                 //#include "PCD5544.h"

#include "GLCD_driver.h"
#include "graphics.h"
#include "gpio.h"

#define CAS_MAX 10
#define DOLZINA_MAX 16

void izpis_casa(char casi[][CAS_MAX], int od_cas, int do_cas);
void izracun_casa(int num, int start, int stop, char casi[][CAS_MAX]);

int main()
{
  int delay;
  int start=0, stop=0;
  int i;
  
  char casi[DOLZINA_MAX][CAS_MAX];
  
  while(1)
  {
/* SPI_write 0*/
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) SPI_write(0xFF);
                   
    stop = T1TC;
    izracun_casa(1, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_write_command 1*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_write_command(0x0C);
                   
    stop = T1TC;
    izracun_casa(2, start, stop, casi);
    delay=2500000; while(delay>0)delay--;

/* GLCD_write_data 2*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_write_data(0xff);
                   
    stop = T1TC;
    izracun_casa(3, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_gotoxy 3*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_gotoxy(0x00,0x00);
                   
    stop = T1TC;
    izracun_casa(4, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_clean_ddram 4*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_clean_ddram();
                   
    stop = T1TC;
    izracun_casa(5, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_putch 5*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_putch('A');
                   
    stop = T1TC;
    izracun_casa(6, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_putstring 16char long 6*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_putstring("AbCdEfGhIjKlMnOp");
                   
    stop = T1TC;
    izracun_casa(7, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_GLCD_putgraphic - black 7*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_putgraphic(0,0,84,6,black);
                   
    stop = T1TC;
    izracun_casa(8, start, stop, casi);
    delay=2500000; while(delay>0)delay--;
    
/* GLCD_GLCD_putgraphic - white 8*/    
    GLCD_clean_ddram();
    start = T1TC;
    
    for(i=0;i<10;i++) GLCD_putgraphic(0,0,84,6,white);
                   
    stop = T1TC;
    izracun_casa(9, start, stop, casi);
    delay=2500000; while(delay>0)delay--;

    /* GLCD_init 9*/    
    GLCD_clean_ddram();
    int Vop = 0x3B;           // Sets Vop           [0-127] (def. 59 (0x3B))  !!!
    int tempC = 0;            // Sets temp coeff.   [0-3]   (def. 0)
    int biasS = 3;            // Sets bias system   [0-7]   (def. 3)
    start = T1TC;

    for(i=0;i<10;i++) GLCD_init(Vop, tempC, biasS);
                   
    stop = T1TC;
    izracun_casa(10, start, stop, casi);
    delay=2500000; while(delay>0)delay--;    
   
    
    izpis_casa(casi, 1, 5);
    izpis_casa(casi, 6, 10);
    }
  return 0;
}

/* Izpiše èas trajanja funkcije v us */
void izpis_casa(char casi[][CAS_MAX], int od_cas, int do_cas)
{  
  int zamik=od_cas;
  char novistr[DOLZINA_MAX];
  int i;
  
  GLCD_write_command(0x0C);
  GLCD_clean_ddram();
  
  for(;od_cas<=do_cas;od_cas++)
  {
    for(i=0;i<DOLZINA_MAX;i++)
    {
      novistr[i]=casi[i][od_cas-1];
    }
    
    GLCD_gotoxy(0, od_cas - zamik);
    GLCD_putstring(novistr);
  }
  
  int delay=50000000; while(delay>0)delay--;
}

/* izraèuna èas trajanja in ga shrani v string tabelo */
void izracun_casa(int num, int start, int stop, char casi[][CAS_MAX])
{
  int trajanje = stop - start;
  num = num-1;
  casi[14][num]=0;
  casi[13][num]='s';
  casi[12][num]='u';
  casi[11][num]=48+(trajanje%10);
  casi[10][num]=48+(trajanje%100)/10;
  casi[9][num] =48+(trajanje%1000)/100;
  casi[8][num] ='.';
  casi[7][num] =48+(trajanje%10000)/1000;
  casi[6][num] =48+(trajanje%100000)/10000;
  casi[5][num] =48+(trajanje%1000000)/100000;
  casi[4][num] ='.';
  casi[3][num] =48+(trajanje%10000000)/1000000;
  casi[2][num] =48+(trajanje%100000000)/10000000;
  casi[1][num] =' ';
  casi[0][num] =48+num;
}
