#include "rtos_tasks.h"

// Scheduler
voidfuncptr sch_tab[] = {empty_task};
voidfuncptr priv_task=empty_task;

int sch_tab_size = sizeof(sch_tab);

void empty_task(){
}

char lcd_buff[33];
char *lcd_string=lcd_buff;
void lcd_driver(){
	int i, done=0;
	lcd_write_comm(DDRAM | 0x00);  
	for(i = 0; i < 16; i++){
    if(lcd_string[i]=='\0')
      done=1;
    if(done)
      lcd_write_data(' ');
    else
      lcd_write_data(lcd_string[i]);
  }
	lcd_write_comm(DDRAM | 0x40);
	for(; i < 32; i++){
    if(lcd_string[i]=='\0')
      done=1;
    if(done)
      lcd_write_data(' ');
    else
      lcd_write_data(lcd_string[i]);    
  }
}
void lcd_driver_1(){
  static int lcd_index=0;
  static int nula=0;
  
  if(lcd_string[lcd_index]==0)
    nula=1;
  
  if(nula==0)
    lcd_write_data(lcd_string[lcd_index]); 
  else
    lcd_write_data(' ');
  lcd_index++;
  if(lcd_index==16){
    lcd_write_comm(DDRAM | 0x40); 
  }
  if(lcd_index==32){
    nula=0;
    lcd_index=0;
    lcd_write_comm(DDRAM | 0x00); 
  }
}

char keys_buff[11];
struct rtos_pipe keys_pipe={keys_buff,10,0,0,0};
void keys_driver(){
  static int ready[]={1,1,1,1};
  int keys_mask[4] = {T0, T1, T2, T3};
  int keys=get_keys();
  char data;
  int i;
  for(i=0; i<4; i++){
    if(keys & keys_mask[i]){
      if(ready[i]==1){
        data=i+'0';
        rtos_pipe_write(&keys_pipe, &data, 1);
      }
      ready[i]=0;
    }
    else{
      ready[i]=1;
    }
  }  
}

int rtos_pipe_write(struct rtos_pipe *pipe, char *data, unsigned int size)
{
	int i;
  int index;
	for(i = 0; i < size; i++)
	{
		index = pipe->head + 1;
		if(index > pipe->size) index = 0;
		if(index == pipe->tail) break;
		else
		{
			pipe->buffer[pipe->head] = data[i];
			pipe->head = index;
		}
	}
	return i;
}

int rtos_pipe_read(struct rtos_pipe *pipe, char *buffer, unsigned int size)
{
	int i;
	for(i = 0; pipe->tail != pipe->head && i < size; i++)
	{
		buffer[i] = pipe->buffer[pipe->tail];
		pipe->tail = pipe->tail + 1;
		if(pipe->tail > pipe->size) pipe->tail = 0;
	}
	return i;
}




