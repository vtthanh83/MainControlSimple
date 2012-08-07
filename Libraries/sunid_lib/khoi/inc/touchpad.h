#ifndef _touchpad_H_
#define _touchpad_H_

#include "stm32sunid.h"

// MPR121 Register Defines
#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32
#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E
#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A
#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F
// Global Constants
#define TOU_THRESH	0x02
#define	REL_THRESH	0x00


// Match key inputs with electrode numbers
#define KEY1 3
#define KEY2 7
#define KEY3 8
#define KEY4 2
#define KEY5 4
#define KEY6 9
#define KEY7 1
#define KEY8 6
#define KEY9 10
#define KEY0 5
#define KEY_CANCEL 0
#define KEY_ENTER 11
#define KEY_DUMP 30

#define touch_i2c_addr (0x5a<<1)

//define IRQ pin
#define TOUCH_IRQ_PIN                GPIO_Pin_10
#define TOUCH_IRQ_PORT          GPIOB
#define TOUCH_IRQ_GPIO_CLK           RCC_APB2Periph_GPIOB  

void TouchPadInit(void);
void TouchPadWriteReg( uint8_t reg, uint8_t data);
uint8_t TouchPadReadReg(uint8_t reg);
ErrorCode GetTouchKey(uint8_t *key, int timeout);
bool IsKeypadPressed(void);

#endif

