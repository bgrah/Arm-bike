// Date:   17.10.2011
// Author: Janez Puhan
// Modified: Blaž Grah - 29.4.2012 

/*
    int clock_mhz = 60;       // MCU clk speed      [12,24,36,48,60] (MHz)
  	int vpbdiv = cclk_4;      // fvpb divider       [cclk,cclk_2,cclk_4]
	init(clock_mhz, vpbdiv);
*/

#include "init.h"

// MAM, VPB and PLL initialisation
// clock_mhz ... clock rate in MHz [12, 24, 36, 48 or 60]
// div       ... divider value [cclk_4, cclk_2 or cclk]
void init(int clock_mhz, int div)
{
	mam_init(clock_mhz);
	set_vpbdiv(div);
	pll_init(clock_mhz);
}
