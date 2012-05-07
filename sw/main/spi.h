// Date:   12.7.2011
// Author: Janez Puhan
// Modified: Blaž Grah - 29.4.2012

#include "gpio.h"

// Bit masks in PINSEL0 register
#define pinsel0_spi_mask	        0x00005500   
#define pinsel0_spi_bits	        0x0000ff00
#define pinsel0_spi_mask_no_MISO	0x00005100   
#define pinsel0_spi_bits_no_MISO	0x0000f300

// Bit masks in S0SPCR register
#define bit_enable		0x00000004
#define cpha				  0x00000008
#define cpol				  0x00000010
#define mstr				  0x00000020
#define lsbf				  0x00000040
#define spie				  0x00000080
#define bits				  0x00000f00

// Settings in S0SPCR register
#define high_rising		0x00000000
#define high_falling	cpha
#define low_rising		(cpha & cpol)
#define low_falling		cpol
#define master				mstr
#define slave				  0x00000000
#define lsb					  lsbf
#define msb					  0x00000000

// Bit masks in S0SPSR register
#define abrt				  0x00000008
#define modf				  0x00000010
#define povr				  0x00000020
#define wcol				  0x00000040
#define spif				  0x00000080

// Bit mask in S0SPINT register
#define spi_interrupt	0x00000001

// Register addresses
#define S0SPCR	(*((volatile int *)0xe0020000))	// SPI Control
#define S0SPSR	(*((volatile int *)0xe0020004))	// SPI Status
#define S0SPDR	(*((volatile int *)0xe0020008))	// SPI Data
#define S0SPCCR	(*((volatile int *)0xe002000c))	// SPI Clock Counter
#define S0SPINT	(*((volatile int *)0xe002001c))	// SPI Interrupt

// Function declarations
extern void spi_init(int length, int mode, int type, int first, int interrupt,
	int div, int miso);
