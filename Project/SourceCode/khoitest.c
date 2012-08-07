#include "khoitest.h"

ErrorCode err;
int i,j;
unsigned char x  = 3;
uint8_t buf[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
				,0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x10
			,0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
				,0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x10
				,0x01, 0x02, 0x03, 0x04, 0x05};
uint8_t recv[64];


void newline(){
	USART_SendData(UART_CAN, 0x0a);
	while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;	
	USART_SendData(UART_CAN, 0x0d);
	while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
}


void testAES(){
		  UARTInit();
		  
		  //UARTSendData(buf, 3, 100);
		  //test <16 byte case
		  SendMessage(buf, 9, 100);
		  newline();
		  //test 16byte case
		  SendMessage(buf, 16, 100);
		  newline();
		  //test >16byte case
		  SendMessage(buf, sizeof(buf), 100);
		  newline();
		  while (1)
		  { 			  
	  /*
			  if(IsComingData(&i)) {
				  x = i%256;
				  USART_SendData(UART_CAN, x);
				  while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
				  err = UARTGetData(recv, &i, 100);
				  for(j = 0; j<i; j++) {
					  USART_SendData(UART_CAN, recv[j]);
					  while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
				  }
				  //UARTSendData(recv, i, 100);
			  }
	  */
			  
			  if(IsComingMessage()) {
				  err = GetMessage(recv, &i, 100);
				  x = i%256;
				  USART_SendData(UART_CAN, x);
				  while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
				  x = err;
				  USART_SendData(UART_CAN, x);
				  while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
				  for(j = 0; j<i; j++) {
					  USART_SendData(UART_CAN, recv[j]);
					  while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
				  }
			  }
		 }
}



void testLED(){	
	LEDInit(ALL_LED);
	while(1){
		LEDToggle(ALL_LED);
		delay_ms(250);
	}
}



void testSwitch(){	
	LEDInit(ALL_LED);
	SWInit(ALL_SWITCH, SWITCH_MODE_GPIO);	
	PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	BuzzerInit();

	while(1){
		if (SWGetState(SWITCH1)) LEDOn(LED1);
		else LEDOff(LED1);
		if (SWGetState(SWITCH2)) LEDOn(LED2);
		else LEDOff(LED2);
		if (SWGetState(SWITCH3)) LEDOn(LED3);
		else LEDOff(LED3);
		if (SWGetState(SWITCH4)) LEDOn(LED4);
		else LEDOff(LED4);

		if (!PBGetState(BUTTON_USER)) BuzzerOn(50);
	}
}


void testTouch(){
	LEDInit(ALL_LED);
	BuzzerInit();
	I2CInit();
	TouchPadInit();
	LEDOn(LEDBGND1);
	LEDOn(LEDBGND2);
	while(1){
		err = GetTouchKey(&x, 10000);
		if (err == ER_TMO){
			BuzzerOn(50);
			delay_ms(100);
			BuzzerOn(50);			
			continue;
		}
		if (err == ER_OK) {
			BuzzerOn(50);
		}
		switch (x) {
			case KEY1:
				LEDOff(ALL_LED);
				LEDOn(LED1);
				break;
			case KEY2:
				LEDOff(ALL_LED);
				LEDOn(LEDRED);
				break;
			case KEY3:
				LEDOff(ALL_LED);
				LEDOn(LED2);
				break;
			case KEY4:
				LEDOff(ALL_LED);
				LEDOn(LED3);
				break;
			case KEY5:
				LEDOff(ALL_LED);
				LEDOn(LED4);
				break;
			default:
				break;
		}
		delay_ms(10);
	}
}


