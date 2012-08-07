#include "uart.h"
#include "stm32f10x_usart.h"
#include "VTimer.h"
#include "string.h"

#define UART_BUF_SIZE 64
#define UART_MAX_MESG 4
#define UART_CANID		0x01

unsigned char UARTRxbuf[UART_MAX_MESG][UART_BUF_SIZE];
unsigned char UART_tempBuf [UART_BUF_SIZE];
int UART_index = 0;
int UART_BytesToRx = 0;
unsigned char UART_bufPushIndex = 0;
unsigned char UART_bufPopIndex = 0;
bool UART_bufEmpty = TRUE;
bool UART_bufFull = FALSE;
bool UART_reflection = FALSE;
/*
  Use 3 array to keep messages from UART, each is 128 byte (can change later)
  pushIndex: current empty buffer to push mesg
  popIndex: current filled buffer to pop mesg
  if buffer is full, incoming mesg will be discarded
  not solve data integrity problem (read/write simultaneously)
*/

/**
  * @brief  Initiate USART2 to communicate with main control board
  *		- BaudRate = 9600 baud  
  *	        - Word Length = 8 Bits
  *	        - One Stop Bit
  *	        - Even parity
  *	        - Hardware flow control disabled (RTS and CTS signals)
  *	        - Receive and transmit enabled
  *	        - Receive interrupt enabled
  * @param  none
  * @retval ErrorCode
  *   This parameter can be one of the following values:
  *     @arg ER_OK: message is retrieved successfully
  *     
  */
ErrorCode UARTInit(){
	ErrorCode err = ER_OK;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(UART_CAN_GPIO_CLK  |RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(UART_CAN_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = UART_CAN_RxPin;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(UART_CAN_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART_CAN_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UART_CAN_GPIO, &GPIO_InitStructure);

	//Configure the NVIC Preemption Priority Bits   
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	  //init interrupt vector
	  NVIC_InitStructure.NVIC_IRQChannel = UART_CAN_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
	 /* UART config:
	        - BaudRate = 9600 baud  
	        - Word Length = 8 Bits
	        - One Stop Bit
	        - Even parity
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Receive and transmit enabled
	  */
	  USART_InitStructure.USART_BaudRate = 9600;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	  //UART_CAN defined in uart.h
	  USART_Init(UART_CAN, &USART_InitStructure);
	  //enable rx interrupt
	  USART_ITConfig(UART_CAN, USART_IT_RXNE, ENABLE);
		//USART_ITConfig(UART_CAN, USART_IT_TXE, ENABLE);
	  //enable uart1
	  USART_Cmd(UART_CAN, ENABLE);
	  
	  

	  UART_index = 0;
	  UART_BytesToRx = 0;
	  UART_bufPushIndex = 0;
	  UART_bufPopIndex = 0;
	  UART_bufEmpty = TRUE;
	return err;
}
void EmptyMessBuff(void)
{
		UART_index = 0;
	  UART_BytesToRx = 0;
	  UART_bufPushIndex = 0;
	  UART_bufPopIndex = 0;
	  UART_bufEmpty = TRUE;
}

/**
  * @brief  Send out a message from upper layer.
  *		Before sending out message to UART:
  *		- Add 1 byte preamble (0x00)
  * 		- Add 2 more bytes to the message to specify its length
  *		- Add 1 byte postamble (0xff)
  * @param  buffer: buffer containing message to send.
  * @param  len: specifies the length of message buffer.
  * @param  timeout: specifies the time (in ms) to wait for the function to return. 
  * @retval ErrorCode
  *   This parameter can be one of the following values:
  *     @arg ER_OK: message is sent successfully
  *     @arg ER_TMO: time out before sending out message, the message may not be sent  
  *     @arg ER_NTMO: cannot allocate a free timer for this function
  */
ErrorCode UARTSendData(uint8_t * buffer, int len, int timeout){
	int i;
	unsigned char timerid;
	unsigned char data;

	timerid = VTimer_GetID();
	if(timerid == 0) return ER_NTMO;
	VTimer_Set(timerid, timeout);

	//send CANID
	//USART_SendData(UART_CAN, UART_CAN_PREAMBLE);	    
	USART_SendData(UART_CAN, UART_CANID);	    
    // Loop until USARTy DR register is empty 
    while((USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) 
		&& (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return ER_TMO;
	}

	//send data length
	data = len/256;
	USART_SendData(UART_CAN, data);	    
    // Loop until USARTy DR register is empty 
    while((USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) 
		&& (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return ER_TMO;
	}
	data = len%256;
	USART_SendData(UART_CAN, data);	    
    // Loop until USARTy DR register is empty 
    while((USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) 
		&& (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return ER_TMO;
	}

	for(i = 0; i<len; i++){
		USART_SendData(UART_CAN, buffer[i]);	    
	    // Loop until USARTy DR register is empty 
	    while((USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) 
			&& (VTimer_IsFired(timerid) == 0));
		if(VTimer_IsFired(timerid))	{	  //time out
			VTimer_Release(timerid);
			return ER_TMO;
		}
	}
	
	//send post amble
	data = UART_CAN_POSTAMBLE;
	USART_SendData(UART_CAN, data);	    
    // Loop until USARTy DR register is empty 
    while((USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) 
		&& (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return ER_TMO;
	}

	VTimer_Release(timerid);
	return ER_OK;
}



/**
  * @brief  Retrieve a message from message queue.
  *		After retrieving a message from UART:
  *		- Strip out 1 byte preamble
  *		- Strip out 2 length bytes
  *		- Strip out 1 byte postamble
  * @param  buffer: buffer to contain message.
  * @param  retlen: specifies the length of message returned.
  * @param  timeout: specifies the time (in ms) to wait for the function to return. 
  * @retval ErrorCode
  *   This parameter can be one of the following values:
  *     @arg ER_OK: message is retrieved successfully
  *     @arg ER_TMO: time out before retrieving message, the message may not be in return buffer  
  *     @arg ER_NTMO: cannot allocate a free timer for this function
  *     @arg ER_EMPTY: there is no message to retrieve
  */
ErrorCode UARTGetData(uint8_t* buf, int* retlen, int timeout){
	int i;
	int len;
	if(UART_bufEmpty) return ER_EMPTY;
	len = (UARTRxbuf[UART_bufPopIndex][1] << 8) + UARTRxbuf[UART_bufPopIndex][2];
	for(i = 0; i<len; i++){
	 	buf[i] = UARTRxbuf[UART_bufPopIndex][i+3];	
	}
	*retlen = len;
	UART_bufPopIndex = (UART_bufPopIndex + 1)% UART_MAX_MESG;
	if(UART_bufFull) UART_bufFull = FALSE;
	if(UART_bufPopIndex == UART_bufPushIndex) UART_bufEmpty = TRUE;
	return ER_OK; 	
}

/**
  * @brief  Check if there is any message waiting in message queue
  * @param  retlen: length of the waiting message 
  * @retval TRUE if there is any message in message queue, FALSE if there is no message   
  */
bool IsComingData(int* retlen){
	if(UART_bufEmpty) return FALSE;
	*retlen = (UARTRxbuf[UART_bufPopIndex][1] << 8) + UARTRxbuf[UART_bufPopIndex][2];
	return TRUE;	 
}



/**
  * @brief  UART receive interrupt service routine. This function is called by the interrupt handler of UART2 in stm32f10x_it.c
  * @param  none 
  * @retval none  
  */
void UART_ISR(){ 
	
	if(USART_GetITStatus(UART_CAN, USART_IT_RXNE) != RESET)
  	{	    
		       
		UART_tempBuf[UART_index] = USART_ReceiveData(UART_CAN);
		//check if this is relection of out coming message
		if(UART_index == 0)
		{
			if(UART_tempBuf[UART_index] == UART_CANID)
				UART_reflection = TRUE;
			else
				UART_reflection = FALSE;
		}
		
		
			UART_index ++;
			if(UART_index == 3)	{
			 	UART_BytesToRx = (UART_tempBuf[1] << 8) + UART_tempBuf[2] + 4; //4byte header + trailer
			}
			if(UART_index ==  UART_BytesToRx){		//end of one mesg
				UART_index = 0;
				if((!UART_bufFull)&&(UART_reflection == FALSE)){
					memcpy(UARTRxbuf[UART_bufPushIndex], UART_tempBuf, (UART_BytesToRx));				
					UART_bufPushIndex = (UART_bufPushIndex + 1)% UART_MAX_MESG;
					if(	UART_bufPushIndex == UART_bufPopIndex) UART_bufFull = TRUE;
					if(UART_bufEmpty) UART_bufEmpty = FALSE;
				}
			}
		
		
	}


//	if(USART_GetITStatus(UART_CAN, USART_IT_RXNE) != RESET)
//	{
//		uint8_t temp = USART_ReceiveData(UART_CAN);
//		USART_SendData(UART_CAN, temp);
//	}

}
