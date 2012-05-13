// Date:   12.5.2012
// Author: Blaž Grah

// Register addresses of MPR121 (Freescale) touch sensor IC

// i2c transmitt/receive buffer
extern unsigned char mpr121_buf[20];
  
// MPR121 i2c address
#define MPR121addr  0x5a  // Address
#define MPR121_R    (MPR121addr << 1) | 0x01	// Address+Read
#define MPR121_W    (MPR121addr << 1) | 0x00	// Address+Write


// Touch status register addresses
#define ELE0_TS   0x00
#define ELE1_TS   0x01

// Threshold values
#define TOU_THRESH	0x0F    // Touch threshold
#define	REL_THRESH	0x0A    // Release threshold

// Electrodes touch and release threshold register addresses
#define	ELE0_T	  0x41
#define	ELE0_R	  0x42
#define	ELE1_T	  0x43
#define	ELE1_R	  0x44
#define	ELE2_T	  0x45
#define	ELE2_R	  0x46
#define	ELE3_T	  0x47
#define	ELE3_R	  0x48
#define	ELE4_T	  0x49
#define	ELE4_R	  0x4A
#define	ELE5_T	  0x4B
#define	ELE5_R	  0x4C
#define	ELE6_T	  0x4D
#define	ELE6_R	  0x4E
#define	ELE7_T	  0x4F
#define	ELE7_R	  0x50
#define	ELE8_T	  0x51
#define	ELE8_R	  0x52
#define	ELE9_T	  0x53
#define	ELE9_R	  0x54
#define	ELE10_T	  0x55
#define	ELE10_R	  0x56
#define	ELE11_T	  0x57
#define	ELE11_R	  0x58

// Various register addresses
#define MHD_R	    0x2B    // MHD Rising
#define NHD_R	    0x2C    // NHD Rising
#define	NCL_R 	  0x2D    // NCL Rising
#define	FDL_R	    0x2E    // FDL Rising
#define	MHD_F	    0x2F    // MHD Falling
#define	NHD_F	    0x30    // NHD Falling
#define	NCL_F	    0x31    // NCL Falling
#define	FDL_F	    0x32    // FDL Falling
#define	FIL_CFG	  0x5D    // Config?
#define	ELE_CFG	  0x5E    // Electrode configuration

// GPIO register addresses
#define GPIO_CTRL0	0x73  // GPIO control 0
#define	GPIO_CTRL1	0x74  // GPIO control 1
#define GPIO_DATA	0x75    // GPIO data
#define	GPIO_DIR	0x76    // GPIO direction
#define	GPIO_EN		0x77    // GPIO enable
#define	GPIO_SET	0x78    // GPIO set
#define	GPIO_CLEAR	0x79  // GPIO clear
#define	GPIO_TOGGLE	0x7A  // GPIO toggle

// Auto-config
#define SRTS      0x80    // Soft reset register 
#define	ATO_CFG0	0x7B    // Control 0
#define	ATO_CFG1	0x7C    // Control 1
#define	ATO_CFGU	0x7D    // USL
#define	ATO_CFGL	0x7E    // LSL
#define	ATO_CFGT	0x7F    // Target level

// Function declarations
extern void mpr121_init(void);
extern void mpr121_write(unsigned char address_reg, unsigned char data);
extern void mpr121_read(unsigned char address_reg);
extern void mpr121_action(unsigned char data);
extern void mpr121_irq(void);
