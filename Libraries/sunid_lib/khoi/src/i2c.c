#include "i2c.h"


/**
  * @brief  Initiate I2C1 to communicate with touchpad and eeprom
  *		- BaudRate = 100 Kpbs  
  * @param  none
  * @retval none
  *     
  */

void I2CInit(){
	GPIO_InitTypeDef  GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	
	  RCC_APB2PeriphClockCmd(sunid_I2C_SCL_GPIO_CLK | sunid_I2C_SDA_GPIO_CLK, ENABLE);

	  /*!< sEE_I2C Periph clock enable */
	  RCC_APB1PeriphClockCmd(sunid_I2C_CLK, ENABLE);
	    
	  /*!< GPIO configuration */  
	  /*!< Configure sEE_I2C pins: SCL */
	  GPIO_InitStructure.GPIO_Pin = sunid_I2C_SCL_PIN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	  GPIO_Init(sunid_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	  /*!< Configure sEE_I2C pins: SDA */
	  GPIO_InitStructure.GPIO_Pin = sunid_I2C_SDA_PIN;
	  GPIO_Init(sunid_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 

	/*!< I2C configuration */
	  
	  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	  I2C_InitStructure.I2C_OwnAddress1 = 0;
	  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	  I2C_InitStructure.I2C_ClockSpeed = 100000;
	  
	  /* sEE_I2C Peripheral Enable */
	  I2C_Cmd(sunid_I2C, ENABLE);
	  /* Apply sEE_I2C configuration after enabling it */
	  I2C_Init(sunid_I2C, &I2C_InitStructure);
	  
}



