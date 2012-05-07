// Date:   24.11.2006
// Author: Janez Puhan

#include "extint.h"

// Configure and reset external interrupts
// pinmask0    ... external input pins mask in PINSEL0
// pinmask1    ... external input pins mask in PINSEL1
// wake        ... wake up allow mask
// sensitivity ... edge sensitivity mask
// trig        ... high level (rising edge) trig mask
void extint_init(int pinmask0, int pinmask1, int wake, int sensitivity, int trig)
{
	PINSEL0 = PINSEL0 | pinmask0;
	PINSEL1 = PINSEL1 | pinmask1;
	INTWAKE = wake;
	EXTMODE = sensitivity;
	EXTPOLAR = trig;
	EXTINT = EINT0 | EINT1 | EINT2 | EINT3;
}
