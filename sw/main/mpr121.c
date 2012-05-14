// Date:   13.5.2012
// Author: Blaž Grah

/* Start up

*/

#include "mpr121.h"
#include "i2c.h"

/* VIC globals
  i2c0_address_rw;	  // int Device address with read/write bit
  i2c0_num_of_bytes;	// int Number of bytes to be received/transmitted
  i2c0_status;		    // int Current master status
  *i2c0_buf;			    // char Receiving/transmitting buffer 
  
  I2C0CONSET = sta;   // To start?
*/

unsigned char mpr121_buf[20];
void mpr121_init(void)
{ 
/* Opombe:
   -> ELEx_T/R registre zapiši v for zanki, naslov poveèuj za ena (samo toliko kot jih rabiš
  
*/  
  // This group controls filtering when data is > baseline.
  mpr121_write(MHD_R, 0x01);
  mpr121_write(NHD_R, 0x01);
  mpr121_write(NCL_R, 0x00);
  mpr121_write(FDL_R, 0x00);

  // This group controls filtering when data is < baseline.
  mpr121_write(MHD_F, 0x01);
  mpr121_write(NHD_F, 0x01);
  mpr121_write(NCL_F, 0xFF);
  mpr121_write(FDL_F, 0x02);

  // This group sets touch and release thresholds for each electrode
  mpr121_write(ELE0_T, TOU_THRESH);
  mpr121_write(ELE0_R, REL_THRESH);
  mpr121_write(ELE1_T, TOU_THRESH);
  mpr121_write(ELE1_R, REL_THRESH);
  mpr121_write(ELE2_T, TOU_THRESH);
  mpr121_write(ELE2_R, REL_THRESH);
  mpr121_write(ELE3_T, TOU_THRESH);
  mpr121_write(ELE3_R, REL_THRESH);

  // Enable Auto Config and auto Reconfig
  mpr121_write(ATO_CFGU, 0xC9);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   
  mpr121_write(ATO_CFGL, 0x82);	// LSL = 0.65*USL = 0x82 @3.3V
  mpr121_write(ATO_CFGT, 0xb5);  // Target = 0.9*USL = 0xB5 @3.3V
  mpr121_write(ATO_CFG0, 0x1B);
  
  // Electrode Configuration
  mpr121_write(ELE_CFG, 0x84);   // Enable 4 Electrodes and set to run mode
}

void mpr121_write(unsigned char address_reg, unsigned char data)
{
  // i2c START
  // i2c send (MPR121_W)
  // i2c send (reg address)
  // i2c send (data)
  // i2c STOP
  
  while(i2c0_num_of_bytes); // ?? Rabim
  
  // 13.9.2 Start master transmit function
                                // 1. Initialize Master data counter.  DONE globally
  i2c0_address_rw = MPR121_W;   // 2. Set up the Slave Address to which data will be transmitted, and add the Write bit.
  I2C0CONSET = sta;             // 3. Write 0x20 to I2CONSET to set the STA bit.  
  i2c0_buf[0] = address_reg;  // 4. Set up data to be transmitted in Master Transmit buffer.
  i2c0_buf[1] = data;
  i2c0_num_of_bytes = 2;        // 5. Initialize the Master data counter to match the length of the message being sent.
                                // 6. Exit
}

void mpr121_read(unsigned char address_reg)
{
  // i2c START
  // i2c send (MPR121_W)
  // i2c send (address_reg)
  
  // i2c repeated START
  // i2c send (MPR121_R)
  // i2c receive
  // i2c STOP
  
  unsigned char data;
  
  while(i2c0_num_of_bytes); // ?? Rabim
  
  // 13.9.2 Start master transmit function
                                // 1. Initialize Master data counter.   DONE globally
  i2c0_address_rw = MPR121_W;   // 2. Set up the Slave Address to which data will be transmitted, and add the Write bit.
  I2C0CONSET = sta;             // 3. Write 0x20 to I2CONSET to set the STA bit.  
  i2c0_buf[0] = address_reg;  // 4. Set up data to be transmitted in Master Transmit buffer.
  i2c0_num_of_bytes = 1;        // 5. Initialize the Master data counter to match the length of the message being sent.
                                // 6. Exit
  // Restart! MPR121
  while(i2c0_num_of_bytes);
  // 13.9.3 Start master receive function
                                // 1. Initialize Master data counter.   DONE globally
  i2c0_address_rw = MPR121_R;   // 2. Set up the Slave Address to which data will be transmitted, and add the Read bit.
  I2C0CONSET = sta;             // 3. Write 0x20 to I2CONSET to set the STA bit.
                                // 4. Set up the Master Receive buffer. DONE globally
  i2c0_num_of_bytes = 1;        // 5. Initialize the Master data counter to match the length of the message to be received.
                                // 6. Exit
  
  while(i2c0_num_of_bytes);
  data = i2c0_buf[0];
  
  GLCD_gotoxy(0,0);
  GLCD_putch(data+64);      // @ not, A - 1, B - 2, D - 4, H - 8
  //mpr121_action(data);
}

void mpr121_action(unsigned char data)
{
  static char button0_state=0;
  static char button1_state=0;
  static char button2_state=0;
  static char button3_state=0;
  
// 0   
  if((data&0x01) && !button0_state)
  {
    GLCD_putch(data); button0_state=1;
  }
  else button0_state=0;
// 1  
  if((data&0x02) && !button1_state)
  {
    GLCD_putch(data); button1_state=1;
  }
  else button1_state=0;
// 2  
  if((data&0x04) && !button2_state)
  {
    GLCD_putch(data); button2_state=1;
  }
  else button2_state=0;
// 3  
  if((data&0x08) && !button2_state)
  {
    GLCD_putch(data); button2_state=1;
  }
  else button3_state=0;
}

void mpr121_irq(void)
{
  // External interrupt when touch/release detected  
}


