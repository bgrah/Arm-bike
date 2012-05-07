Arm-bike/sw
===========

TO-DO:
------
+ Driver for Nokia 5510 LCD with HW SPI
+ Driver for MPR121 I2C touch sensor
+ Driver for STLM75M2E I2C temp sensor
+ Driver for UART GPS communication
+ Driver for PC data logging (FTDI230XS UART to USB serial)
+ Driver for PWM -> power LED
+ User interface (txt)
+ User interface (graphic)
+ Testing
+ RTOS & interrupts

x_driver.h
----------

### Graphical LCD driver (`GLCD_driver.h/.c`)
##### Done: 60%

+ 1. 5.2012		Working LCD driver with bit-banging. Write_: data, command, string, graphic; clear RAM; gotoxy                    
+ 2. 5.2012		Improved GLCD lib. -> from 14% to 50% better timing. Still 92ms for full screen. HW SPI!	

### PWM LED driver (`pwm.h/.c`)
##### Done: 30%

+ 7. 5.2012  	1 channel working. Need to incorporate Fvpb and set duyt-cycle functions into pwm.h. Free 1 more PWM channel!!                    
