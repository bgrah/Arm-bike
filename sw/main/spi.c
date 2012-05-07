// Date:   12.7.2011
// Author: Janez Puhan
// Modified: Blaž Grah - 29.4.2012

#include "spi.h"

// Configure SPI0
// length    ... number of bits of data per transfer
//               [8, 9, 10, 11, 12, 13, 14, 15 or 16]
// mode      ... defines active clock level and sampling edge
//               [high_rising, high_falling, low_rising or low_falling]
// type      ... device type [master or slave]
// first     ... bit to be transferred first [lsb or msb]
// interrupt ... enable interrupt requests [0 or 1]
// div       ... fvpb divider, clock = fvpb / div [even number on interval [8, 254]]
// miso      ... enables MISO [0,1]
void spi_init(int length, int mode, int type, int first, int interrupt, int div, int miso)
{
	S0SPCR = bit_enable | ((length & 0x000000ff) << 8) | mode | type | first |
		(interrupt << 7);
	S0SPCCR = div;
  if(miso)	PINSEL0 = (PINSEL0 & ~pinsel0_spi_bits) | pinsel0_spi_mask;             // All signal lines
  else PINSEL0 = (PINSEL0 & ~pinsel0_spi_bits_no_MISO) | pinsel0_spi_mask_no_MISO;  // Without MISO0
}
