// Date:   15.2.2007
// Author: Janez Puhan
// Modified: Blaž Grah (13. 5. 2012)

#include "i2c.h"

// Configure i2c0
// slave_address ... 7 bit address of this i2c device in slave mode, if
//                   slave_address = 0x00 and general_call = 0x00, then this device
//                   operates only in master mode
// general_call  ... if not zero, then this i2c device recognises general call
//                   address (0x00)
// duty          ... upper 16 bits define duration of high state in number of VPB
//                   cycles and lower 16 bits define duration of low state in number
//                   of VPB cycles
// tx_buf        ... pointer to data for slave transmit mode
void i2c0_init(int slave_address, int general_call, int duty, char *tx_buf)
{
	i2c0_rxslv_begin = 0;
	i2c0_rxslv_end = 0;
	i2c0_txslv = tx_buf;
	i2c0_status = i2c_idle;
	PINSEL0 = (PINSEL0 & ~i2c0_bits) | i2c0_values;
	I2C0SCLH = (duty & 0xffff0000) >> 16;
	I2C0SCLL = duty & 0x0000ffff;
	I2C0ADR = 2 * slave_address;
	if(general_call) I2C0ADR = I2C0ADR | gc;
	I2C0CONCLR = i2enc | stac | sic | aac;
	if(slave_address || general_call) I2C0CONSET = aa;
}

// Configure i2c1
// slave_address ... 7 bit address of this i2c device in slave mode, if
//                   slave_address = 0x00 and general_call = 0x00, then this device
//                   operates only in master mode
// general_call  ... if not zero, then this i2c device recognises general call
//                   address (0x00)
// duty          ... upper 16 bits define duration of high state in number of VPB
//                   cycles and lower 16 bits define duration of low state in number
//                   of VPB cycles
// tx_buf        ... pointer to data for slave transmit mode
void i2c1_init(int slave_address, int general_call, int duty, char *tx_buf)
{
	i2c1_rxslv_begin = 0;
	i2c1_rxslv_end = 0;
	i2c1_txslv = tx_buf;
	i2c1_status = i2c_idle;
	PINSEL0 = (PINSEL0 & ~i2c1_bits) | i2c1_values;
	I2C1SCLH = (duty & 0xffff0000) >> 16;
	I2C1SCLL = duty & 0x0000ffff;
	I2C1ADR = 2 * slave_address;
	if(general_call) I2C1ADR = I2C1ADR | gc;
	I2C1CONCLR = i2enc | stac | sic | aac;
	if(slave_address || general_call) I2C1CONSET = aa;
}

// Handles events on i2c0 bus
// Slave receiver mode (cyclic FIFO buffer)
int i2c0_rxslv_begin;		// Index of the first received byte
int i2c0_rxslv_end;			// Index of the first empty place
char i2c0_rxslv[bufsize];	// Buffer
// Slave transmitter mode
char *i2c0_txslv;	// Pointer to the string of bytes to be transmitted
// Master modes
int i2c0_address_rw;	// Device address with read/write bit
int i2c0_num_of_bytes;	// Number of bytes to be received/transmitted
int i2c0_status=i2c_idle;		// Current master status
//x char *i2c0_buf;			// Receiving/transmitting buffer
char i2c0_buf[20];
void handle_i2c0_state()
{
	static int i2c0_txslv_cnt, i2c0_num_of_bytes_internal, i2c0_cnt;
	int tmp = I2C0ADR, status = I2C0STAT;
	switch(status)
	{
    
// Bus error
	case 0x00000000:      // Bus error has occurred.
		  i2c0_status = i2c_error;   // Mark i2c0 status as error
// I2C0CONSET = sto | aa;
//x		I2C0CONCLR = stac;         // Clear STA - not start
		I2C0CONSET = sto;          // Set STO - stop
//x		if(tmp) I2C0CONSET = aa;
//x		else I2C0CONCLR = aac;
	  	break;
      
// General Master
	case 0x00000008:      // A START condition has been transmitted.
	case 0x00000010:      // A repeated START condition has been transmitted.
/*x*/ i2c0_status = i2c_busy;
      I2C0DAT = i2c0_address_rw;    // Sent slave address and R/W bit    
// I2C0CONSET = aa;
      i2c0_cnt = 0;       // i2c data buffer index
		  break;
      
// Master transmitt
	case 0x00000018:      // SLA+W has been transmitted; ACK has been received.
		  i2c0_num_of_bytes_internal = i2c0_num_of_bytes;   // Number of bytes to transmit/receive
/*x*/ i2c0_num_of_bytes_internal = i2c0_num_of_bytes_internal - 1;    
/*x*/ I2C0DAT = i2c0_buf[i2c0_cnt];   // Sent one byte of data
// I2C0CONSET = aa;    
/*x*/ i2c0_cnt = i2c0_cnt + 1;
/*x*/ break;
	case 0x00000020:      // SLA+W has been transmitted; NOT ACK has been received.
		  i2c0_status = i2c_idle;       // Set current master status to idle
// I2C0CONSET = sto | aa;
//x		I2C0CONCLR = stac;  // Clear STA - not start
		I2C0CONSET = sto;   // Set STO - stop
		  break;
  case 0x00000028:      // Data byte in I2DAT has been transmitted; ACK has been received.
//x		I2C0CONCLR = stac;  // Clear STA - not start
		  if(i2c0_num_of_bytes_internal)    // If still data to transmitt
		  {
			  i2c0_num_of_bytes_internal = i2c0_num_of_bytes_internal - 1;   // Left data -1
			  I2C0DAT = i2c0_buf[i2c0_cnt];   // Sent one byte of data
			  i2c0_cnt = i2c0_cnt + 1;        // Increase array index
//   I2C0CONSET = aa;
		  } else              // If all sent
		  {
			  i2c0_status = i2c_idle;     // Set current master status to idle
			  i2c0_num_of_bytes = 0;      // Set number of bytes to transmitt to zero
//   I2C0CONSET = sto | aa;
			I2C0CONSET = sto;           // Set STO - stop
		  }
		  break;
	case 0x00000030:      // Data byte in I2DAT has been transmitted; NOT ACK has been received.
/*x*/ I2C0CONSET = sto;
//x		i2c0_num_of_bytes = i2c0_num_of_bytes_internal + 1;   // To sent last byte again in next transmission
/*x*/ break;
	case 0x00000038:      // Arbitration lost in SLA+R/W or Data bytes.
/*x*/ I2C0CONSET = sta;    
//x		I2C0CONSET = sta;   // Set STA - Start will be transmited when bus becomes free.
		  break;
      
// Master receive
	case 0x00000040:      // SLA+R has been transmitted; ACK has been received.
/*x*/	i2c0_num_of_bytes = i2c0_num_of_bytes - 1;  // Prepare data buffer index
		  if(i2c0_num_of_bytes) I2C0CONSET = aa;      // If data still to receive keep returning ACK
		  else I2C0CONCLR = aac;                      // When no more data to receive, return NOT ACK
//x		I2C0CONCLR = stac;  // Clear STA - not start
		  break;
	case 0x00000048:      // SLA+R has been transmitted; NOT ACK has been received.
//x		if(tmp) I2C0CONSET = aa;  // Set AA it slave address is present
//x		else I2C0CONCLR = aac;    // If slave address (tmp) == 0 (master) than clear AA
		  i2c0_status = i2c_idle;   // Set current master status to idle
      I2C0CONSET = sto | aa;
//x		I2C0CONCLR = stac;  // Clear STA
//x		I2C0CONSET = sto;   // Set STO - stop
		  break;
	case 0x00000050:      // Data byte has been received; ACK has been returned.
/*x*/ i2c0_buf[i2c0_cnt] = I2C0DAT;   // Store data in data buffer
/*x*/	i2c0_cnt = i2c0_cnt + 1;        // Increase data buffer index
      
/*x*/	i2c0_num_of_bytes = i2c0_num_of_bytes - 1;  // Prepare data buffer index
/*x*/	if(i2c0_num_of_bytes) I2C0CONSET = aa;      // If data still to receive keep returning ACK
/*x*/	else I2C0CONCLR = aac;
      i2c0_status = i2c_idle;
//x		i2c0_buf[i2c0_cnt] = I2C0DAT;   // Store data in data buffer
//x		i2c0_cnt = i2c0_cnt + 1;        // Increase data buffer index
/*x*/ break;
  case 0x00000058:      // Data byte has been received; NOT ACK has been returned.
		  i2c0_buf[i2c0_cnt] = I2C0DAT;   // Last byte received
/*x*/ I2C0CONSET = sto | aa;
      i2c0_status = i2c_idle;

//Slave receive (check!!!)
  case 0x00000060:
      // Missing
      break;
  case 0x00000068:
      // Missing
      break;
  case 0x00000070:
      // Missing
      break;
  case 0x00000078:
      // Missing
      break;    
	case 0x00000080:
      // Missing
      break;
	case 0x00000088:
      // Missing
      break;
  case 0x00000090:      // Previously addressed with General Call; DATA byte has been received; ACK has been returned.
  		tmp = i2c0_rxslv_end;
  		i2c0_rxslv[tmp] = I2C0DAT;
  		tmp = tmp + 1;
  		if(tmp == bufsize) tmp = 0;
  		if(tmp != i2c0_rxslv_begin) i2c0_rxslv_end = tmp;
  		break;
	case 0x00000098:
      // Missing
      break;
	case 0x000000a0:
      // Missing
      break;
      
// Slave transmitt (check!!!)
	case 0x000000a8:
      // Missing
      break;
	case 0x000000b0:      // Arbitration lost in SLA+R/W as master; Own SLA+R has been received, ACK has been returned.
		  i2c0_txslv_cnt = 0;
	case 0x000000b8:      // Data byte in I2DAT has been transmitted; ACK has been received.
  		I2C0DAT = i2c0_txslv[i2c0_txslv_cnt];
  		i2c0_txslv_cnt = i2c0_txslv_cnt + 1;
  		break;
	case 0x000000c0:
      // Missing
      break;
	case 0x000000c8:
      // Missing
      break;
	}
	I2C0CONCLR = sic;   // Clear SI (serial interrupt flag)
}

// Handles events on i2c1 bus
// Slave receiver mode (cyclic FIFO buffer)
int i2c1_rxslv_begin;		// Index of the first received byte
int i2c1_rxslv_end;			// Index of the first empty place
char i2c1_rxslv[bufsize];	// Buffer
// Slave transmitter mode
char *i2c1_txslv;	// Pointer to the string of bytes to be transmitted
// Master modes
int i2c1_address_rw;	// Device address with read/write bit
int i2c1_num_of_bytes;	// Number of bytes to be received/transmitted
int i2c1_status;		// Current master status
char *i2c1_buf;			// Receiving/transmitting buffer
void handle_i2c1_state()
{
	static int i2c1_txslv_cnt, i2c1_num_of_bytes_internal, i2c1_cnt;
	int tmp = I2C1ADR, status = I2C1STAT;
	switch(status)
	{
	case 0x00000000:
		i2c1_status = i2c_error;
		I2C1CONCLR = stac;
		I2C1CONSET = sto;
		if(tmp) I2C1CONSET = aa;
		else I2C1CONCLR = aac;
		break;
	case 0x00000008:
	case 0x00000010:
		i2c1_cnt = 0;
		I2C1DAT = i2c1_address_rw;
		break;
	case 0x00000018:
		i2c1_num_of_bytes_internal = i2c1_num_of_bytes;
	case 0x00000028:
		I2C1CONCLR = stac;
		if(i2c1_num_of_bytes_internal)
		{
			i2c1_num_of_bytes_internal = i2c1_num_of_bytes_internal - 1;
			I2C1DAT = i2c1_buf[i2c1_cnt];
			i2c1_cnt = i2c1_cnt + 1;
		} else
		{
			i2c1_status = i2c_idle;
			i2c1_num_of_bytes = 0;
			I2C1CONSET = sto;
		}
		break;
	case 0x00000030:
		i2c1_num_of_bytes = i2c1_num_of_bytes_internal + 1;
	case 0x00000020:
		i2c1_status = i2c_idle;
		I2C1CONCLR = stac;
		I2C1CONSET = sto;
		break;
	case 0x00000038:
		I2C1CONSET = sta;
		break;
	case 0x00000050:
		i2c1_buf[i2c1_cnt] = I2C1DAT;
		i2c1_cnt = i2c1_cnt + 1;
	case 0x00000040:
		i2c1_num_of_bytes = i2c1_num_of_bytes - 1;
		if(i2c1_num_of_bytes) I2C1CONSET = aa;
		else I2C1CONCLR = aac;
		I2C1CONCLR = stac;
		break;
	case 0x00000058:
		i2c1_buf[i2c1_cnt] = I2C1DAT;
	case 0x00000048:
		if(tmp) I2C1CONSET = aa;
		else I2C1CONCLR = aac;
		i2c1_status = i2c_idle;
		I2C1CONCLR = stac;
		I2C1CONSET = sto;
		break;
	case 0x00000080:
	case 0x00000090:
		tmp = i2c1_rxslv_end;
		i2c1_rxslv[tmp] = I2C1DAT;
		tmp = tmp + 1;
		if(tmp == bufsize) tmp = 0;
		if(tmp != i2c1_rxslv_begin) i2c1_rxslv_end = tmp;
		break;
	case 0x000000a8:
	case 0x000000b0:
		i2c1_txslv_cnt = 0;
	case 0x000000b8:
		I2C1DAT = i2c1_txslv[i2c1_txslv_cnt];
		i2c1_txslv_cnt = i2c1_txslv_cnt + 1;
		break;
	}
	I2C1CONCLR = sic;
}
