// Date:   19.3.2008
// Author: Janez Puhan

#include "vic.h"

// Undefined instruction exception routine
void und(void) __attribute__((interrupt("UNDEF")));
void und(void)
{
	while(1);
}

// Software interrupt routine
void swi(void) __attribute__((interrupt("SWI")));
void swi(void)
{
	while(1);
}

// Prefetch abort exception routine
void pref_abt(void) __attribute__((interrupt("ABORT")));
void pref_abt(void)
{
	while(1);
}

// Data abort exception routine
void data_abt(void) __attribute__((interrupt("ABORT")));
void data_abt(void)
{
	while(1);
}

// Interrupt request interrupt service routine (ISR)
void irq(void) __attribute__((interrupt("IRQ")));
void irq(void)
{
	(*((voidfuncptr)VICVectAddr))();
  VICVectAddr = 0;
}

// Fast interrupt request interrupt service routine (ISR)
void fiq(void) __attribute__((interrupt("FIQ")));
void fiq(void)
{
	VICVectAddr = 0;
	while(1);
}
