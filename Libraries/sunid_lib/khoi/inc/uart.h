#ifndef __UART_H
#define __UART_H

#include "stm32sunid.h"

#define UART_CAN                   USART2
#define UART_CAN_GPIO              GPIOA
#define UART_CAN_CLK               RCC_APB1Periph_USART2
#define UART_CAN_GPIO_CLK          RCC_APB2Periph_GPIOA
#define UART_CAN_RxPin             GPIO_Pin_3
#define UART_CAN_TxPin             GPIO_Pin_2
#define UART_CAN_IRQn              USART2_IRQn
#define UART_CAN_IRQHandler        USART2_IRQHandler

#define UART_CAN_PREAMBLE 0x00
#define UART_CAN_POSTAMBLE 0xFF


ErrorCode UARTInit(void);
ErrorCode UARTSendData(uint8_t * buffer, int len, int timeout);
ErrorCode UARTGetData(uint8_t* buf, int* retlen, int timeout);
bool IsComingData(int* retlen);
void EmptyMessBuff(void);
void UART_ISR(void);


#endif
