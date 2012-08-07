#ifndef __UART_H
#define __UART_H

#include "stm32sunidMainSmall.h"

#define UART_CAN                   USART2
#define UART_CAN_GPIO              GPIOA
#define UART_CAN_CLK               RCC_APB1Periph_USART2
#define UART_CAN_GPIO_CLK          RCC_APB2Periph_GPIOA
#define UART_CAN_RxPin             GPIO_Pin_3
#define UART_CAN_TxPin             GPIO_Pin_2
#define UART_CAN_IRQn              USART2_IRQn
#define UART_CAN_IRQHandler        USART2_IRQHandler

#define UART_COM                   USART1
#define UART_COM_GPIO              GPIOA
#define UART_COM_CLK               RCC_APB2Periph_USART1
#define UART_COM_GPIO_CLK          RCC_APB2Periph_GPIOA
#define UART_COM_RxPin             GPIO_Pin_10
#define UART_COM_TxPin             GPIO_Pin_9
#define UART_COM_IRQn              USART1_IRQn
#define UART_COM_IRQHandler        USART1_IRQHandler

#define UART_CAN_PREAMBLE 0x00
#define UART_CAN_POSTAMBLE 0xFF


ErrorCode UARTInit(void);
ErrorCode UARTSendData(uint8_t * buffer, int len, int timeout);
ErrorCode UARTGetData(uint8_t* buf, int* retlen, int timeout);
bool IsComingData(int* retlen);
void EmptyMessBuff(void);
void UART_ISR(void);
//Prototype for UART 1
ErrorCode USARTCOMInit(void);
unsigned char USART_putchar(unsigned char c);
unsigned char USART_getchar(void);
int USART_printf(const char *format, ...);
unsigned char USART_getche(void);
unsigned int USART_intScanf(int num, int min,int max);
unsigned char USART_IsRxEmpty(void);
ErrorCode USARTGetComData(uint8_t* buf, int len, int timeout);
void USART1_ISR(void);
#endif
