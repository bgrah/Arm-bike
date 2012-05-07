// Date:   9.11.2011
// Author: Janez Puhan

// System status
#define task_completed	0
#define task_running	1

#include "gpio.h"
#include "vpbdiv.h"
#include "pll.h"
#include "timer.h"
#include "rtos_tasks.h"

// Function declarations
extern void sch_on(unsigned int slice);
extern void sch_int();

// sync with OS scheduler - wait untill enough cycles are available before next time slice
extern void sch_sync(int len);

