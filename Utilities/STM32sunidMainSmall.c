/**
  ******************************************************************************
  * @file    stm32sunid.c
  * @author  Tuan Thanh Vu - SunID Team
  * @version V1.0.0
  * @date    31/07/2011
  * @brief   STM32SUNID abstraction layer. 
  *          This file should be added to the main application to use the provided
  *          functions that manage the Leds LD1,LD2,LD3 and LD4 and the USER switch.
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "STM32sunidMainSmall.h"
#include "VTimer.h"


/** @defgroup STM32sunid_Private_Variables
  * @{
  */ 
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT};

const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN};

const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK};



const uint16_t BUTTON_PIN_SOURCE[BUTTONn] = {USER_BUTTON_EXTI_PIN_SOURCE};

const uint16_t BUTTON_PORT_SOURCE[BUTTONn] = {USER_BUTTON_EXTI_PORT_SOURCE};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT}; 

const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN}; 

const uint32_t BUTTON_CLK[BUTTONn] = {USER_BUTTON_GPIO_CLK};

const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {USER_BUTTON_EXTI_LINE};

const uint16_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};	


GPIO_TypeDef* SWITCH_PORT[SWITCHn] = {SWITCH1_GPIO_PORT, SWITCH2_GPIO_PORT, SWITCH3_GPIO_PORT, SWITCH4_GPIO_PORT}; 

const uint16_t SWITCH_PIN[SWITCHn] = {SWITCH1_PIN, SWITCH2_PIN, SWITCH3_PIN, SWITCH4_PIN}; 

const uint32_t SWITCH_CLK[SWITCHn] = {SWITCH1_GPIO_CLK, SWITCH2_GPIO_CLK, SWITCH3_GPIO_CLK, SWITCH4_GPIO_CLK};				 

/** @defgroup STM32vldiscovery_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32sunid_Private_Functions
  * @{
  */ 

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  *     @arg LEDRED
  *     @arg LEDBGND1
  *     @arg LEDBGND2
  * @retval None
  */
  //LED  is active low
void LEDInit(Led_TypeDef Led)
{
	unsigned char i;
  GPIO_InitTypeDef  GPIO_InitStructure;
  //remap pin PD0, PD1
  GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);
  if(Led == ALL_LED){
	  for(i = 0; i<LEDn; i++){
	  	  RCC_APB2PeriphClockCmd(GPIO_CLK[i], ENABLE);
	  	  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[i]; 	  
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		  GPIO_Init(GPIO_PORT[i], &GPIO_InitStructure);
		  GPIO_PORT[i]->BSRR = GPIO_PIN[i]; 			  
	  }
  }
  else{
	  /* Enable the GPIO_LED Clock */
	  RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);
	
	  /* Configure the GPIO_LED pin */
	  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led]; 	  
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
	  GPIO_PORT[Led]->BSRR = GPIO_PIN[Led]; 
  }
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  *     @arg LEDRED
  *     @arg LEDBGND1
  *     @arg LEDBGND2  
  * @retval None
  */
void LEDOn(Led_TypeDef Led)
{
	if(Led == ALL_LED){
		unsigned char i;
	  for(i = 0; i<LEDn; i++){
		  GPIO_PORT[i]->BRR = GPIO_PIN[i]; 
	  }
	}
	else GPIO_PORT[Led]->BRR = GPIO_PIN[Led];   
}



void BuzzerInit(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);
}
void BuzzerOn(int time)
{
  BUZZER_GPIO_PORT->BSRR = BUZZER_PIN;   
  delay_ms(time);
  BUZZER_GPIO_PORT->BRR = BUZZER_PIN; 
	delay_ms(100);
}
/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  *     @arg LEDRED
  *     @arg LEDBGND1
  *     @arg LEDBGND2 
  * @retval None
  */
void LEDOff(Led_TypeDef Led)
{
	if(Led == ALL_LED){
		unsigned char i;
	  for(i = 0; i<LEDn; i++){
		  GPIO_PORT[i]->BSRR = GPIO_PIN[i];  
	  }
	}
  	else GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];   
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  *     @arg LEDRED
  *     @arg LEDBGND1
  *     @arg LEDBGND2  
  * @retval None
  */
void LEDToggle(Led_TypeDef Led)
{
	if(Led == ALL_LED){
		unsigned char i;
	  for(i = 0; i<LEDn; i++){
		  GPIO_PORT[i]->ODR ^= GPIO_PIN[i]; 
	  }
	}
	else GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}


/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_USER: USER Push Button 
  * @param  Button_Mode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                     generation capability  
  * @retval None
  */
void PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(BUTTON_CLK[Button] | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
  GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);

  if (Button_Mode == BUTTON_MODE_EXTI)
  {
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(BUTTON_PORT_SOURCE[Button], BUTTON_PIN_SOURCE[Button]);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  

    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure); 
  }
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter can be one of following parameters:    
  *     @arg BUTTON_USER: USER Push Button 
  * @retval The Button GPIO pin value.
  */
uint32_t PBGetState(Button_TypeDef Button)
{
  return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  Configures Swithc GPIO.
  * @param  Button: Specifies the Switch to be configured.
  *   This parameter can be one of following parameters:   
  *     @arg SWITCH1
  *     @arg SWITCH2  
  *     @arg SWITCH3
  *     @arg SWITCH4
  *   This parameter can be one of following parameters:   
  *     @arg SWITCH_MODE_GPIO: Button will be used as simple IO 
  *     @arg SWITCH_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                     generation capability  
  * @retval None
  */
void SWInit(Switch_TypeDef Switch, SwitchMode_TypeDef Switch_Mode)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	unsigned char i = 0;
	 if(Switch == ALL_SWITCH){
	  for(i = 0; i<SWITCHn; i++){
		  RCC_APB2PeriphClockCmd(SWITCH_CLK[i] | RCC_APB2Periph_AFIO, ENABLE);
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		  GPIO_InitStructure.GPIO_Pin = SWITCH_PIN[i];
		  GPIO_Init(SWITCH_PORT[i], &GPIO_InitStructure);
	  }	
	  }
	  else{
		  /* Enable the BUTTON Clock */
		  RCC_APB2PeriphClockCmd(SWITCH_CLK[Switch] | RCC_APB2Periph_AFIO, ENABLE);

		  /* Configure Button pin as input pull up */
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		  GPIO_InitStructure.GPIO_Pin = SWITCH_PIN[Switch];
		  GPIO_Init(SWITCH_PORT[Switch], &GPIO_InitStructure);
	  }
}

/**
  * @brief  Returns the selected switch state.
  * @param  Switch: Specifies the Button to be checked.
  *   This parameter can be one of following parameters:    
  *     @arg SWITCH1
  *     @arg SWITCH2  
  *     @arg SWITCH3
  *     @arg SWITCH4    
  * @retval The Button GPIO pin value.
  */
uint32_t SWGetState(Switch_TypeDef Switch)
{
  return GPIO_ReadInputDataBit(SWITCH_PORT[Switch], SWITCH_PIN[Switch]);
}
/**
  * @brief  Configures Timer2 for general purpose.
  * @retval None
  */
void TimerInit(uint32_t period)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM2_TimeBaseInitStruct;
	//Basic settup
	/* Timer 2 Periph clock enable */
	RCC_APB1PeriphClockCmd(SUNID_TIM_CLK, ENABLE);
	TIM2_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM2_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM2_TimeBaseInitStruct.TIM_RepetitionCounter = TIM_OPMode_Repetitive;
	TIM2_TimeBaseInitStruct.TIM_Prescaler = 23;			//CLK_CNT = PCLK1/24= 1MHz
	TIM2_TimeBaseInitStruct.TIM_Period = period - 1;				
	TIM_TimeBaseInit(SUNID_TIM, &TIM2_TimeBaseInitStruct);
	//using internal clk
	TIM_InternalClockConfig(SUNID_TIM);
	//enable Auto reload buffer
	TIM_ARRPreloadConfig(SUNID_TIM, ENABLE);
	//enable update interrupt
	TIM_ITConfig(SUNID_TIM, TIM_IT_Update, ENABLE);
	//start timer 2
	TIM_Cmd(SUNID_TIM, ENABLE);

	//NVIC_Init(&NVIC_InitStructure); 
	NVIC_InitStructure.NVIC_IRQChannel = SUNID_TIM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
/**
  * @brief  DeInitializes the peripherals used by the SPI PN532 driver.
  * @param  None
  * @retval None
  */
void sPN532_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Disable the sPN532_SPI  */
  SPI_Cmd(sPN532_SPI, DISABLE);
  
  /*!< DeInitializes the sPN532_SPI */
  SPI_I2S_DeInit(sPN532_SPI);
  
  /*!< sPN532_SPI Periph clock disable */
  RCC_APB2PeriphClockCmd(sPN532_SPI_CLK, DISABLE);
  
  /*!< Configure sPN532_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = sPN532_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(sPN532_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sPN532_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sPN532_SPI_MISO_PIN;
  GPIO_Init(sPN532_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sPN532_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sPN532_SPI_MOSI_PIN;
  GPIO_Init(sPN532_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sPN532_CS_PIN pin: sPN532 Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sPN532_CS_PIN;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
  GPIO_Init(sPN532_CS_GPIO_PORT, &GPIO_InitStructure);

}

/**
  * @brief  Initializes the peripherals used by the SPI PN532 driver.
  * @param  None
  * @retval None
  */
void sPN532_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< sPN532_SPI_CS_GPIO, sPN532_SPI_MOSI_GPIO, sPN532_SPI_MISO_GPIO 
       and sPN532_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(sPN532_CS_GPIO_CLK | sPN532_SPI_MOSI_GPIO_CLK | sPN532_SPI_MISO_GPIO_CLK |
                         sPN532_SPI_SCK_GPIO_CLK, ENABLE);

  /*!< sPN532_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(sPN532_SPI_CLK, ENABLE);
  
  /*!< Configure sPN532_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = sPN532_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sPN532_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sPN532_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sPN532_SPI_MOSI_PIN;
  GPIO_Init(sPN532_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sPN532_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sPN532_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  GPIO_Init(sPN532_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  /* Configure Button pin as input pull up */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = PN532_RST_PIN;
  GPIO_Init(PN532_RST_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(PN532_RST_GPIO_PORT,PN532_RST_PIN);
  /*!< Configure sPN532_CS_PIN pin: sPN532 Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sPN532_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sPN532_CS_GPIO_PORT, &GPIO_InitStructure);
  GPIO_SetBits(sPN532_CS_GPIO_PORT, sPN532_CS_PIN);

  /* Configure Button pin as input pull up */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = PN532_RST_PIN;
  GPIO_Init(PN532_RST_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(PN532_RST_GPIO_PORT, PN532_RST_PIN);
}
/**	brief compare two array
  * return: 1 not equal
  *         0 equal
  * @}
  */ 
uint32_t arraycmp(const uint8_t * src,const uint8_t * des, uint32_t startIndex, uint32_t len)
{
	int i = 0;
	for(i = startIndex; i<len; i++)
	{
		if (src[i] != des[i]) return 1;
	}
	return 0;
}
/**
  * @}
  */ 


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
