/**
  ******************************************************************************
  * @file    main.c 
  * @author  Tuan Thanh Vu - SunID Team
  * @version V1.0.0
  * @date    01/08/2011
  * @brief   Main program body.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm32sunidMainSmall.h"
#include "VTimer.h"
#include "khoitest.h"
/** @addtogroup Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

RCC_ClocksTypeDef RCC_ClockFreq;



/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void SetSysClock(void);
void SetSysClockTo24(void);


/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{	   
   	  
   /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
	/* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
	SetSysClock();

	/* This function fills the RCC_ClockFreq structure with the current
	 frequencies of different on chip clocks (for debug purpose) */
	RCC_GetClocksFreq(&RCC_ClockFreq);

	/* Initialize Leds, Switches, Button, system timer mounted on STM32SUNID board --------------------------*/
	
	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
	    /* Capture error */ 
	  	while (1);
	}
	GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE);
	/* SunID libraries init ------------------------------------------------------*/
	VTimer_InitController();
	/* NVIC configuration ------------------------------------------------------*/
	NVIC_Configuration();

	test_USARTCOM();
	while(1);
	
}

/*============================================================*/
/*============================================================*/
/* Initialization related function      */
/*============================================================*/

/**
  * @brief  Configures the System clock frequency, HCLK, PCLK2 and PCLK1
  *   prescalers.
  * @param  None
  * @retval None
  */
void SetSysClock()
{    
/* The System clock configuration functions defined below assume that:
        - SunID use internal 8MHz crystal is
          used to drive the System clock.
     If you are using different crystal you have to adapt those functions accordingly.*/

  SetSysClockTo24();  
 
 /* If none of the define above is enabled, the HSI is used as System clock
    source (default after reset) */ 
}

/**
  * @brief  Sets System clock frequency to 24MHz and configure HCLK, PCLK2 
  *   and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockTo24()
{
//ErrorStatus HSEStartUpStatus;
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  //RCC_HSEConfig(RCC_HSE_ON);
  /* Wait till HSE is ready */
  //HSEStartUpStatus = RCC_WaitForHSEStartUp();

  //if (HSEStartUpStatus == SUCCESS)
  //{
        /* Flash 0 wait state */
    //FLASH_SetLatency(FLASH_Latency_0);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);
    
    /* PLLCLK = (8MHz/2) * 6 = 24 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_6);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  //}
  //else
  //{ /* If HSE fails to start-up, the application will have wrong clock configuration.
  //     User can add here some code to deal with this error */    

    /* Go to infinite loop */
  //  while (1)
  //  {
  //  }
  //}
   /* Enable the BUTTON Clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

	/* Configure and enable ADC interrupt */
	//NVIC_InitStructure.NVIC_IRQChannel = 18;  /*!< ADC1 and ADC2 global Interrupt                       */
	//
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&NVIC_InitStructure);
}



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

