# GPS bike system

## ARM microcontroller design competition

Blaž Grah <br />
Date: 31 December 2011

<br />
##Contents
+ <a href="#intro">Introduction</a>	
+ <a href="#idea">Idea</a>	
+ <a href="#design">Design</a>
	+ <a href="#l0">Layer Zero: Lights</a>	
	+ <a href="#l1">Layer One: Bike</a>	
	+ <a href="#l2"> Two: ARM7</a>		
	+ <a href="#l3">Layer Three: GPS</a>		
	+ <a href="#l4">Layer Four: LCD</a>		
+ <a href="#ui">User interface</a>		
+ <a href="#end">Conclusion</a>

<br /> 
## <a name="intro"></a>Introduction
I’m currently in the third year of my undergraduate course at Faculty of electrical engineering in Ljubljana. From the very start of my study I was very interested in microcontroller systems for their ease of creating something, but at the same time the complexity that lies behind. My first “hands-on” introduction to microcontrollers was in my first year of study in class “Programming microcontrollers”. We were working with Philips LPC2138 development board (Š-ARM) supplied by the faculty. 

In the last year and a half I have been working on many usually small projects with Š-ARM. Some of them have seen the end of the development phase (interface for dual-color LED matrix, small robot controlled with analog joystick, etc.) while others are still waiting for that moment. In that time I was also gaining interest in designing electronic boards and I hope that I could merge both my interests in my project.
 
<br /> 
## <a name="idea">Idea
I got the idea for this project in the middle of October, while cycling back home from late evening class. It was dark and I only had one small red light at the back of my bike and I decided that it was time for me to get some proper lights. Despite knowing that it would probably be cheaper to buy a pair of lights, I decided to build my own. But already the next day the idea started growing and at the end I ended up not only with brightness controlled lights, but also with GPS, graphical LCD, capacitive sensing pads for input, break sensing for additional backlight control, temperature and humidity sensor and a RTC module. 

<br /> 
## <a name="design">Design
To use ŠARM board with this project would be impractical, so I decided to make the whole system as modular as possible with my own ARM board. The plan is to make the whole system like a [sandwich](/ARMsandwich.png). The bottom layer would be connectors on the bike’s handlebar, next layer would be the control layer with LPC2138 ARM7 microprocessor with minimal accessories, next layer will include a GPS receiver and a temperature and humidity sensor, and on top a layer with graphical LCD and capacitive touch pads.

The modularity of the system would ensure that boards could be also used for other applications like GPS or temperature logger or a normal development board for various other applications.

### <a name="l0">Layer Zero: Lights
Layer Zero consists of all hi-power components. Both front and rear lights and its driving circuits and battery pack. I will probably use 3W to 5W hi-power LED in the front and 1W red LED in the rear. Driving circuits will provide constant current to LEDs and also PWM compatibilities. In case that the microcontroller won’t be used, there will also be a manual switch to turn on or off the lights. The battery will be sealed in a water bottle and placed into bike’s bottle holder for easy access and removal if needed (to refill).
### <a name="l1">Layer One: Bike
This is the first PCB layer. It will be securely attached to the bike’s handlebar. Its only purpose is to connect control signals for lights with the microcontroller and to provide power for the rest of the layers from the battery pack. 
### <a name="l2">Layer Two: ARM7
Brains of the project. A board consisting of LPC2138 ARM7 microprocessor, coin battery for RTC, 3.3V regulator, JTAG connector for programming and few status LEDs. Size of this board will determinate the size of the whole project. All pins from the microprocessor will be broken out to a 2.54mm connector, which will connect all layers together, providing both power (form layer One) and communication.
### <a name="l3">Layer Three: GPS
PCB with GPS module, temperature and humidity sensor. GPS module and antenna will be bought as a working unit and later just attached to this layer. Communication between GPS and ARM will be via serial interface. Like GPS, temperature sensor will also be connected to ARM over most likely serial interface (may change to I2C, depends on sensor choosen).
### <a name="l4">Layer Four: LCD
Layer consisting of a 128 x 64 monochrome LCD display (from old Nokia phones) in the middle and four capacitive touch-sense pads for input on the left and right of it. Capacitive sensing will be realized with the use of a dedicated controller, which will connect to ARM over I2C interface. For now the controller I have in mind is Freescale MPR121. LCD will be connected with serial interface.

<br /> 
## <a name="ui">User interface
I anticipate three problems in this project. The first one with the hardware design and realization, the second problem will be interfacing all components with the microcontroller and  the last problem will be the user interface.

For now the idea of what will be displayed on LCD is that when moving, there will be information about speed, altitude, date, time, temperature and lights indicators. When stationary the LCD will display information about geographical position (latitude and longitude), altitude, date, time and temperature. The user will be able to select different information and access settings with the use of capacitive touch pads.

<br /> 
## <a name="end">Conclusion
I hope that I’ll be able to realize this idea. I’ve already got hands on a GPS receiver and LCD module so I’m starting to interface them now. And I’m also working on PCB design for LCP2138.

For any questions or explanations I’m available on my e-mail and I’ll also try to post some more information of my progress on my blog.
