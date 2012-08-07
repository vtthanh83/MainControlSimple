#include "touchpad.h"
#include "i2c.h"
#include "VTimer.h"


/**
  * @brief  Read a value from a specific register of MPR121.
  * @param  reg: adress of the register
  * @retval the value of the register
  */
uint8_t TouchPadReadReg(uint8_t reg){
	uint8_t x;
	while(I2C_GetFlagStatus(sunid_I2C, I2C_FLAG_BUSY));
	/*!< Send START condition */
  	I2C_GenerateSTART(sunid_I2C, ENABLE);	
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(sunid_I2C, touch_i2c_addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(sunid_I2C, reg);
	while(I2C_GetFlagStatus(sunid_I2C, I2C_FLAG_BTF) == RESET);
	/*!< Send START condition a second time */  
  	I2C_GenerateSTART(sunid_I2C, ENABLE);	
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(sunid_I2C, touch_i2c_addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	I2C_AcknowledgeConfig(sunid_I2C, DISABLE);
	/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
    	(void)sunid_I2C->SR2;	
	I2C_GenerateSTOP(sunid_I2C, ENABLE);
	while(I2C_GetFlagStatus(sunid_I2C, I2C_FLAG_RXNE) == RESET);
	x = I2C_ReceiveData(sunid_I2C);
	/*!< Send STOP Condition */   	
	//Wait to make sure that STOP control bit has been cleared
	while(sunid_I2C->CR1 & I2C_CR1_STOP);
	I2C_AcknowledgeConfig(sunid_I2C, ENABLE);
	return x;
}




/**
  * @brief  Write a value to a specific register of MPR121.
  * @param  reg: adress of the register
  * @param  data: value to write
  * @retval none
  */
void TouchPadWriteReg(uint8_t reg,uint8_t data){
	while(I2C_GetFlagStatus(sunid_I2C, I2C_FLAG_BUSY));
	/*!< Send START condition */
  	I2C_GenerateSTART(sunid_I2C, ENABLE);	
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(sunid_I2C, touch_i2c_addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(sunid_I2C, reg);
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(sunid_I2C, data);
	while(!I2C_CheckEvent(sunid_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/* Send STOP Condition */
  	I2C_GenerateSTOP(sunid_I2C, ENABLE);
}



/**
  * @brief  Initiate necessary registers of MPR121
  		Before calling this function, one MUST call I2CInit to init I2C module
  * @param  none
  * @retval none
  *     
  */
void TouchPadInit(){
	//I2CInit();
	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 	  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//soft reset touchpad
	TouchPadWriteReg(0x80, 0x63);
	delay_ms(1);
	// Section A  
	// This group controls filtering when data is > baseline.  
	TouchPadWriteReg(MHD_R, 0x01);  
	TouchPadWriteReg(NHD_R, 0x01);  
	TouchPadWriteReg(NCL_R, 0x00);  
	TouchPadWriteReg(FDL_R, 0x00);  
	// Section B  
	// This group controls filtering when data is < baseline.  
	TouchPadWriteReg(MHD_F, 0x01);  
	TouchPadWriteReg(NHD_F, 0x01);  
	TouchPadWriteReg(NCL_F, 0xFF);  
	TouchPadWriteReg(FDL_F, 0x02);    
	// Section C  
	// This group sets touch and release thresholds for each electrode  
	TouchPadWriteReg(ELE0_T, TOU_THRESH);  
	TouchPadWriteReg(ELE0_R, REL_THRESH);  
	TouchPadWriteReg(ELE1_T, TOU_THRESH); 
	TouchPadWriteReg(ELE1_R, REL_THRESH);  
	TouchPadWriteReg(ELE2_T, TOU_THRESH);  
	TouchPadWriteReg(ELE2_R, REL_THRESH);  
	TouchPadWriteReg(ELE3_T, TOU_THRESH);  
	TouchPadWriteReg(ELE3_R, REL_THRESH);  
	TouchPadWriteReg(ELE4_T, TOU_THRESH);  
	TouchPadWriteReg(ELE4_R, REL_THRESH);  
	TouchPadWriteReg(ELE5_T, TOU_THRESH);  
	TouchPadWriteReg(ELE5_R, REL_THRESH);  
	TouchPadWriteReg(ELE6_T, TOU_THRESH);  
	TouchPadWriteReg(ELE6_R, REL_THRESH);  
	TouchPadWriteReg(ELE7_T, TOU_THRESH);  
	TouchPadWriteReg(ELE7_R, REL_THRESH);  
	TouchPadWriteReg(ELE8_T, TOU_THRESH);  
	TouchPadWriteReg(ELE8_R, REL_THRESH);  
	TouchPadWriteReg(ELE9_T, TOU_THRESH);  
	TouchPadWriteReg(ELE9_R, REL_THRESH);  
	TouchPadWriteReg(ELE10_T, TOU_THRESH);  
	TouchPadWriteReg(ELE10_R, REL_THRESH);  
	TouchPadWriteReg(ELE11_T, TOU_THRESH);  
	TouchPadWriteReg(ELE11_R, REL_THRESH);    
	// Section D  // Set the Filter Configuration  
	// Set ESI2  
	TouchPadWriteReg(FIL_CFG, 0x04);    
	// Section E  
	// Electrode Configuration  
	// Enable 6 Electrodes and set to run mode  
	// Set ELE_CFG to 0x00 to return to standby mode  
	TouchPadWriteReg(ELE_CFG, 0x0C);	// Enables all 12 Electrodes  
	//TouchPadWriteReg(ELE_CFG, 0x06);	// Enable first 6 electrodes    
	
}



/**
  * @brief  Check if there is any key pressed recently
  * @param  none
  * @retval TRUE if there is any key pressed, FALSE if there is no key pressed   
  */
bool IsKeypadPressed(){
	//check IRQ pin
	//low = key pressed
	//high = no key pressed
	return (!GPIO_ReadInputDataBit(TOUCH_IRQ_PORT, TOUCH_IRQ_PIN));
}





/**
  * @brief  Retrieve a key that has been pressed.
  *		This function already wait and check if there is any key pressed so there is no need to call IsKeypadPressed() before calling this
  * @param  key: key that has been pressed.
  *   		This parameter can be one of the following values:
  *     	@arg KEY0 ~ KEY9, KEY_ENTER, KEY_CANCEL
  *     	@arg KEY_DUMP: if there are 2 or more keys pressed at the same time or there is no key pressed after time out  
  * @param  timeout: specifies the time (in ms) to wait for the function to return. 
  * @retval ErrorCode
  *   This parameter can be one of the following values:
  *     @arg ER_OK: key is retrieved successfully
  *     @arg ER_TMO: time out before having a key pressed event  
  *     @arg ER_NTMO: cannot allocate a free timer for this function
  *     @arg ER_IO: there are 2 or more keys press at the same time.
  */
ErrorCode GetTouchKey(uint8_t *key, int timeout){
	uint8_t touchNumber = 0;
	uint8_t i;	
	uint16_t touchStatus = 0;
	unsigned char timerid;
	ErrorCode err = ER_OK;

	timerid = VTimer_GetID();
	if(timerid == 0) return ER_NTMO;
	VTimer_Set(timerid, timeout);
	
	while((!IsKeypadPressed()) && (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return ER_TMO;
	}

	
	touchStatus = TouchPadReadReg(0x01) << 8;   
	touchStatus |= TouchPadReadReg(0x00);
	// Check how many electrodes were pressed 
	for (i=0; i<12; i++)     {
		if ((touchStatus & (1<<i)))   touchNumber++; 
		if (touchNumber >=2) break;
	}
	if (touchNumber == 1){
		if (touchStatus & (1<<KEY0)) *key =  KEY0;
		else if (touchStatus & (1<<KEY1)) *key = KEY1;
		else if (touchStatus & (1<<KEY2)) *key = KEY2;
		else if (touchStatus & (1<<KEY3)) *key = KEY3;
		else if (touchStatus & (1<<KEY4)) *key = KEY4;
		else if (touchStatus & (1<<KEY5)) *key = KEY5;
		else if (touchStatus & (1<<KEY6)) *key = KEY6;
		else if (touchStatus & (1<<KEY7)) *key = KEY7;
		else if (touchStatus & (1<<KEY8)) *key = KEY8;
		else if (touchStatus & (1<<KEY9)) *key = KEY9;
		else if (touchStatus & (1<<KEY_ENTER)) *key = KEY_ENTER;
		else if (touchStatus & (1<<KEY_CANCEL)) *key = KEY_CANCEL;
	}
	else {
		*key = KEY_DUMP;
		err = ER_IO;
	}
	VTimer_Release(timerid);
	return err;
}


