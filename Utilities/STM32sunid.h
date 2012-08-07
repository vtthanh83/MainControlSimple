/**
  ******************************************************************************
  * @file    STM32sunid.h
  * @author  Tuan Thanh Vu - SunID Team
  * @version V1.0.0
  * @date    31/07/2011
  * @brief   Header file for STM32sunid.c module.
  ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32SUNID_H
#define __STM32SUNID_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_i2c.h"

/** @defgroup STM32sunid_Exported_Types
  * @{
  */
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
  LEDRED = 4,
  LEDBGND1 = 5,
  LEDBGND2 = 6,
  ALL_LED = 7
} Led_TypeDef;

typedef enum 
{  
  SWITCH1 = 0,
  SWITCH2 = 1,
  SWITCH3 = 2,
  SWITCH4 = 3,
  ALL_SWITCH = 4
} Switch_TypeDef;
typedef enum 
{  
  SWITCH_MODE_GPIO = 0,
  SWITCH_MODE_EXTI = 1
} SwitchMode_TypeDef;

typedef enum 
{  
  BUTTON_USER = 0,
  ALL_BUTTON = 1
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;              



/** @defgroup STM32sunid error code
  * @{
  */
typedef enum
{
	ER_OK 	= 0,   //success
	ER_TMO 	= -1, 	//time out
	ER_NACK 	= -2, 	//no ack
	ER_IO	= -3,  // hardware IO error
	ER_ILL 	= -4,	//illegal command, keypad….
	ER_RFID	= -5,	// RFID Tag operation error
	ER_NTMO 	= -6, 	//no virtual timer
	ER_EMPTY 	= -7,	//read empty buffer
} ErrorCode;
//Need every body to add specific error


/** 
  * @brief  STM32F100 Button Defines Legacy  
  */ 

#define Button_USER          BUTTON_USER
#define Mode_GPIO            BUTTON_MODE_GPIO
#define Mode_EXTI            BUTTON_MODE_EXTI
#define Button_Mode_TypeDef  ButtonMode_TypeDef


/** @addtogroup STM32sunid_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             7

#define LEDRED_PIN                         GPIO_Pin_1
#define LEDRED_GPIO_PORT                   GPIOD
#define LEDRED_GPIO_CLK                    RCC_APB2Periph_GPIOD
  
#define LED2_PIN                         GPIO_Pin_13
#define LED2_GPIO_PORT                   GPIOC
#define LED2_GPIO_CLK                    RCC_APB2Periph_GPIOC  

#define LED3_PIN                         GPIO_Pin_15  
#define LED3_GPIO_PORT                   GPIOC
#define LED3_GPIO_CLK                    RCC_APB2Periph_GPIOC

#define LED4_PIN                         GPIO_Pin_0  
#define LED4_GPIO_PORT                   GPIOD
#define LED4_GPIO_CLK                    RCC_APB2Periph_GPIOD 

#define LED1_PIN                       GPIO_Pin_14  
#define LED1_GPIO_PORT                 GPIOC
#define LED1_GPIO_CLK                  RCC_APB2Periph_GPIOC
 
#define LEDBGND1_PIN                     GPIO_Pin_9  
#define LEDBGND1_GPIO_PORT               GPIOB
#define LEDBGND1_GPIO_CLK                RCC_APB2Periph_GPIOB
										 
#define LEDBGND2_PIN                     GPIO_Pin_1  
#define LEDBGND2_GPIO_PORT               GPIOB
#define LEDBGND2_GPIO_CLK                RCC_APB2Periph_GPIOB


												  
/** @addtogroup STM3210C_EVAL_LOW_LEVEL_SWITCH
  * @{
  */  
#define SWITCHn                    4 

/**
 * @brief switch 1
 */
#define SWITCH1_PIN                GPIO_Pin_12
#define SWITCH1_GPIO_PORT          GPIOB
#define SWITCH1_GPIO_CLK           RCC_APB2Periph_GPIOB


/**
 * @brief switch 2
 */
#define SWITCH2_PIN                GPIO_Pin_13
#define SWITCH2_GPIO_PORT          GPIOB
#define SWITCH2_GPIO_CLK           RCC_APB2Periph_GPIOB


/**
 * @brief switch 3
 */
#define SWITCH3_PIN                GPIO_Pin_14
#define SWITCH3_GPIO_PORT          GPIOB
#define SWITCH3_GPIO_CLK           RCC_APB2Periph_GPIOB

/**
 * @brief switch 4
 */
#define SWITCH4_PIN                GPIO_Pin_15
#define SWITCH4_GPIO_PORT          GPIOB
#define SWITCH4_GPIO_CLK           RCC_APB2Periph_GPIOB
/**
 * @brief buzzer
 */
#define BUZZER_PIN                GPIO_Pin_11
#define BUZZER_GPIO_PORT          GPIOB
#define BUZZER_GPIO_CLK           RCC_APB2Periph_GPIOB
/**
  * @}
  */ 
  
/** @addtogroup STM32vldiscovery_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          1

/* * @brief USER push-button
 */
#define USER_BUTTON_PIN                   GPIO_Pin_12
#define USER_BUTTON_GPIO_PORT             GPIOA
#define USER_BUTTON_GPIO_CLK              RCC_APB2Periph_GPIOA
#define USER_BUTTON_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE       GPIO_PinSource12
#define USER_BUTTON_EXTI_LINE             EXTI_Line12
#define USER_BUTTON_EXTI_IRQn             EXTI15_10_IRQn


/*
 * define timer and timeout timer
 */
#define SUNID_TIM				TIM2
#define SUNID_TIM_CLK           RCC_APB1Periph_TIM2
#define SUNID_TIM_IRQn			TIM2_IRQn

#define SUNID_TIM_TICK			1   //ms
/**
  * @}
  */ 
/** @addtogroup STM32SUNID_EVAL_LOW_LEVEL_PN532_SPI
  * @{
  */
/**
  * @brief  PN532 SPI Interface pins
  */  
#define sPN532_SPI                       SPI1
#define sPN532_SPI_CLK                   RCC_APB2Periph_SPI1
#define sPN532_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define sPN532_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define sPN532_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOA
#define sPN532_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.06 */
#define sPN532_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define sPN532_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sPN532_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.07 */
#define sPN532_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define sPN532_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sPN532_CS_PIN                    GPIO_Pin_4                  /* PA.04 */
#define sPN532_CS_GPIO_PORT              GPIOA                       /* GPIOA */
#define sPN532_CS_GPIO_CLK               RCC_APB2Periph_GPIOA

/* * @brief PN532 IRQ
 */
#define PN532_IRQ_PIN                   GPIO_Pin_0
#define PN532_IRQ_GPIO_PORT             GPIOA
#define PN532_IRQ_GPIO_CLK              RCC_APB2Periph_GPIOA
#define PN532_IRQ_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOA
#define PN532_IRQ_EXTI_PIN_SOURCE       GPIO_PinSource0
#define PN532_IRQ_EXTI_LINE             EXTI_Line0
#define PN532_IRQ_EXTI_IRQn             EXTI0_IRQn
/* * @brief PN532 Reset
 */
#define PN532_RST_PIN                   GPIO_Pin_1
#define PN532_RST_GPIO_PORT             GPIOA
#define PN532_RST_GPIO_CLK              RCC_APB2Periph_GPIOA

/** @defgroup STM32vldiscovery_LOW_LEVEL__Exported_Functions
  * @{
  */ 
void LEDInit(Led_TypeDef Led);
void LEDOn(Led_TypeDef Led);
void LEDOff(Led_TypeDef Led);
void LEDToggle(Led_TypeDef Led);

void TimerInit(uint32_t period);

void PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t PBGetState(Button_TypeDef Button);

void SWInit(Switch_TypeDef Switch, SwitchMode_TypeDef Switch_Mode);
uint32_t SWGetState(Switch_TypeDef Switch);

void BuzzerInit(void);
void BuzzerOn(int time);

void sPN532_LowLevel_DeInit(void);
void sPN532_LowLevel_Init(void);
										   
uint32_t arraycmp(const uint8_t * src, const uint8_t * des, uint32_t startIndex, uint32_t len);
/**
  * @}
  */ 
    
#ifdef __cplusplus
}
#endif


#endif /* __STM32vldiscovery_H */

/**
  * @}
  */ 

/**
  * @}
  */  

/**
  * @}
  */
  
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
