#include "vic.h"
#include "gpio.h"
#include "uart.h"

extern voidfuncptr sch_tab[];
extern voidfuncptr priv_task;
extern int sch_tab_size;

/* prazno opravilo */
extern void empty_task();


struct rtos_pipe
{
	char *buffer;		// Pipe buffer.
	unsigned int size;	// Size of allocated pipe buffer.
	unsigned int head;	// Pipe write buffer position.
	unsigned int tail;	// Pipe read buffer position.
};
extern int rtos_pipe_write(struct rtos_pipe *pipe, char *data, unsigned int size);
extern int rtos_pipe_read(struct rtos_pipe *pipe, char *buffer, unsigned int size);

/* gonilnik LCD prikazovalnika */
extern char *lcd_string;
extern void lcd_driver();
extern void lcd_driver_1();

extern struct rtos_pipe keys_pipe;
extern void keys_driver();