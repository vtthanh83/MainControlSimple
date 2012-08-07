#include "uart.h"
#include "stm32f10x_usart.h"
#include "VTimer.h"
#include "string.h"

#define MAX_BUFFER_SIZE 64
#define UART_COMID		0x01


typedef struct{
	unsigned char buffer[MAX_BUFFER_SIZE];
	unsigned int count;
	unsigned int rd_start;
	unsigned int wr_start;
} STDIO_BUF;
volatile STDIO_BUF std_buf;
void EmptyComMessBuff(void);
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
ErrorCode USARTCOMInit(){
	ErrorCode err = ER_OK;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(UART_COM_GPIO_CLK  |RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(UART_COM_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = UART_COM_RxPin;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(UART_COM_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART_COM_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UART_COM_GPIO, &GPIO_InitStructure);

	//Configure the NVIC Preemption Priority Bits   
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	  //init interrupt vector
	  NVIC_InitStructure.NVIC_IRQChannel = UART_COM_IRQn;
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

	  //UART_COM defined in uart.h
	  USART_Init(UART_COM, &USART_InitStructure);
	  //enable rx interrupt
	  USART_ITConfig(UART_COM, USART_IT_RXNE, ENABLE);
		//USART_ITConfig(UART_COM, USART_IT_TXE, ENABLE);
	  //enable uart1
	  USART_Cmd(UART_COM, ENABLE);
	  
	  

	  EmptyComMessBuff();
	return err;
}
void EmptyComMessBuff(void)
{
		std_buf.count = 0;
		std_buf.rd_start = 0;
		std_buf.wr_start = 0;
}


/*******************************************************************************
* Function Name  : USART_IsRxEmpty
* Description    : check if there is remain chars in rx buffer.
* Input          : none
* Output         : None
* Return         : 0: not empty, 1: empty
*******************************************************************************/
unsigned char USART_IsRxEmpty(void)
{
	if (std_buf.count > 0)
		return 0;
	else
		return 1;
}
/*******************************************************************************
* Function Name  : USART1_putchar
* Description    : send a character to TX buffer of USART1 (blocking).
* Input          : 16bit character
* Output         : None
* Return         : 16bit character
*******************************************************************************/
unsigned char USART_putchar(unsigned char c)
{
	while (USART_GetFlagStatus(UART_COM,USART_FLAG_TXE) != SET); // USART1 TX buffer ready?
	USART_SendData(UART_COM,c);                  // TX -> RXed character
    return c;
}
/*******************************************************************************
* Function Name  : USART1_getchar
* Description    : get a character from RX buffer (blocking).
* Input          : 16bit character
* Output         : None
* Return         : 16bit character
*******************************************************************************/
unsigned char USART_getchar(void)							
{
	unsigned char c;
//	while(USART_GetFlagStatus(UART_COM,USART_FLAG_RXNE) !=SET);
//	USART_ClearFlag(UART_COM,USART_IT_RXNE);
//	c =  USART_ReceiveData(UART_COM);
	while (std_buf.count <= 0);        	// RX buffer ready?
	std_buf.count--;
	c = std_buf.buffer[std_buf.rd_start++];
	std_buf.rd_start = (std_buf.rd_start == MAX_BUFFER_SIZE)?0:std_buf.rd_start;

	return c;
}
/*******************************************************************************
* Function Name  : USART_getche
* Description    : get a character from RX buffer (blocking) and echo back.
* Input          : 16bit character
* Output         : None
* Return         : 16bit character
*******************************************************************************/
unsigned char USART_getche(void)
{
	unsigned char c;
	c = USART_getchar();
	USART_putchar(c);
	return c;
}
/*******************************************************************************
* Function Name  :UARTGetComData
* Description    : get buffer from RX buffer (non-blocking)
* Input          : 16bit character
* Output         : None
* Return         : 16bit character
*******************************************************************************/
ErrorCode USARTGetComData(uint8_t* buf, int len, int timeout){
	int i;
	unsigned char timerid;
	if(std_buf.count == 0) return ER_EMPTY;
	//setup timeout
	timerid = VTimer_GetID();
	if(timerid == 0) return ER_NTMO;
	VTimer_Set(timerid, timeout);
	// Loop until get enough data 
  while((std_buf.count<len)	&& (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
			VTimer_Release(timerid);
			return ER_TMO;
	}
	for(i = 0; i<len; i++){
	 	buf[i] = USART_getchar();	
	}
	
	return ER_OK; 	
}
/*******************************************************************************
* Function Name  : USART_intScanf
* Description    : get a number character from RX buffer and convert them to an integer (blocking).
* Input          : number of characters, min, max
* Output         : None
* Return         : 16bit character
*******************************************************************************/
unsigned int USART_intScanf(int num, int min,int max)
{
  unsigned char c[4];
  int i;
  unsigned int  result = 0;
  int done = 0;
  int err = 0;
  while(done == 0)
  {
	  for(i = 0; i< num; i++)
	  {
	    c[i] = USART_getchar();
		if((c[i]<0x30) || (c[i]>0x39))
		{
			USART_printf("\r\n  Invalid, Please try again: ");	
			err = 1;
		}
		else
		{
		  c[i] = c[i] - 0x30;
		  result = result*10+c[i];
		}
	  }
	  if (err == 1)
	  	err = 0;
	  else	
	  {
      	if ((result < min) || (result > max))
		{
			USART_printf("\r\n  Invalid, Please try again: ");
		}
		else
			done = 1;
	  }
  }
  return result;

}
/*******************************************************************************
* Function Name  : USART2_ISR
* Description    : This function should be called to handle USART2 RX interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_ISR(void)
{
	char c;
	if (USART_GetITStatus(UART_COM,USART_IT_RXNE) == SET)
	{
	//STM_EVAL_LEDToggle(LED3);
		USART_ClearITPendingBit(UART_COM,USART_IT_RXNE);
		if (std_buf.count < MAX_BUFFER_SIZE)
		{
			std_buf.count++;
			c = USART_ReceiveData(UART_COM);
			std_buf.buffer[std_buf.wr_start++] = c;
			//USART_putchar(USART_getchar());
			std_buf.wr_start = (std_buf.wr_start == MAX_BUFFER_SIZE)?0:std_buf.wr_start;
		}
	}
}
