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

unsigned char mpr121_tx_buf[2];    // MPR121 register address and data
unsigned char mpr121_rx_buf[1];    // Data

void mpr121_init(void)
{  
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
//  mpr121_write(ATO_CFG1, 0x01);  //????
  
  // Electrode Configuration
  mpr121_write(ELE_CFG, 0x84);   // Enable 4 Electrodes and set to run mode
}

void mpr121_write(unsigned char address_reg, unsigned char data)
{
  mpr121_tx_buf[0] = address_reg;  
  mpr121_tx_buf[1] = data;
    
  while(i2c0_status != i2c_idle);
  i2c0_status = i2c_busy;
  i2c0_num_of_bytes = 2;
  i2c0_address_rw = MPR121_W;
  i2c0_buf = mpr121_tx_buf;    // i2c0_buf is pointer to mpr121_buf
  I2C0CONSET = sta;
}

void mpr121_read(unsigned char address_reg)
{
  mpr121_tx_buf[0] = address_reg;  
    
  while(i2c0_status != i2c_idle);
  i2c0_status = i2c_busy;
  i2c0_num_of_bytes = 1;
  i2c0_address_rw = MPR121_W;
  i2c0_buf = mpr121_tx_buf;    // i2c0_buf is pointer to mpr121_buf
  I2C0CONSET = sta;  
  
  while(i2c0_status != i2c_idle);
  i2c0_status = i2c_busy;
  i2c0_num_of_bytes = 1;
  i2c0_address_rw = MPR121_R;
  i2c0_buf = mpr121_rx_buf;    // naslov kamor bo zapisalo
  I2C0CONSET = sta;
}

void mpr121_action(unsigned char data)
{
  // Button action
}

void mpr121_irq(void)
{
  mpr121_read(ELE0_TS); // Read touch register  
}


