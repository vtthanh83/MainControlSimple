#include "TI_aes.h"
#include "aes.h"
#include "keyman.h"
#include "uart.h"
#include "stdlib.h"
#include "string.h"

/**
  * @brief  Initiate the AES encrypt and decrypt module. Actually do nothing, no need to call
  * @param  none
  * @retval ErrorCode
  */
ErrorCode InitAes128(){
	//do nothing
	return ER_OK;
}


/**
  * @brief  Send out a message from user after encoding it using AES128.
  *		Before sending out message to UART:
  *		- Encode message using AES128
  * 		- Add 2 more bytes to cyphertext to specify the length of plaintext 
  * @param  buffer: buffer containing plain message (plain text).
  * @param  len: specifies the length of message buffer.
  * @param  timeout: specifies the time (in ms) to wait for the function to return. 
  * @retval ErrorCode
  *   This parameter can be one of the following values:
  *     @arg ER_OK: message is sent successfully
  *     @arg ER_TMO: time out before sending out message, the message may not be sent  
  *     @arg ER_NTMO: cannot allocate a free timer for this function
  */
ErrorCode SendMessage(uint8_t* buffer, int len, int timeout){
	//encrypt and send
	unsigned char n, q;
	unsigned char state[16];
	unsigned char key[16];
	int keylen;
	unsigned char* cipherText;
	int i;
	int cipherLen;
	ErrorCode err;

	err = GetProtocolKey(key, &keylen);
	if((err != ER_OK)) return err;	
	n = len / 16;
	q = len % 16;
	if(n == 0) { //len < 16
		 memcpy(state, buffer, len);
		 //fill with 0
		 for(i = len; i<16; i++) state[i] = 0; 
		 cipherText = (unsigned char*) malloc(16+2);
		 aes_encrypt(state, key);
		 memcpy(&cipherText[2], state, 16);
		 cipherLen = 16;
	}
	else if(q == 0){ //len mod 16 = 0
		 cipherText = (unsigned char*) malloc(16*n + 2);
		 cipherLen = 16*n;
		 for (i = 0; i< n; i++){
			  memcpy(state, &buffer[i*16], 16);
			  aes_encrypt(state, key);
			  memcpy(&cipherText[2+i*16], state, 16);
		 }
	}
	else { //len mod 16 != 0
		 cipherText = (unsigned char*) malloc(16*(n + 1) + 2);
		 cipherLen = 16*(n+1);
		 for (i = 0; i< n; i++){
			  memcpy(state, &buffer[i*16], 16);
			  aes_encrypt(state, key);
			  memcpy(&cipherText[2+i*16], state, 16);
		 }
		 memcpy(state, &buffer[i*16], q);
		 for(i = q; i<16; i++) state[i] = 0; //fill 0
		 aes_encrypt(state, key);
		 memcpy(&cipherText[2+n*16], state, 16);
	}

	cipherText[0] = len/256;
	cipherText[1] = len%256;
	err =  UARTSendData(cipherText, (cipherLen + 2), timeout); 
	free(cipherText);
	return err;
}



/**
  * @brief  Retrieve a message from message queue after decoding it using AES128.
  *		After retrieving a message (encrypted) from UART:
  *		- Strip out 2 length bytes
  *		- Decrypt the cyphertext
  *		- Return the original message
  * @param  buffer: buffer to contain plain message (plain text).
  * @param  retlen: specifies the length of message returned.
  * @param  timeout: specifies the time (in ms) to wait for the function to return. 
  * @retval ErrorCode
  *   This parameter can be one of the following values:
  *     @arg ER_OK: message is retrieved successfully
  *     @arg ER_TMO: time out before retrieving message, the message may not be in return buffer  
  *     @arg ER_NTMO: cannot allocate a free timer for this function
  *     @arg ER_EMPTY: there is no message to retrieve
  *     @arg ER_IO: cannot allocate a free buffer for this function
  */
ErrorCode GetMessage(uint8_t * buffer, int* retlen, int timeout){
	ErrorCode err = ER_OK;
	unsigned char state[16];
	unsigned char key[16];
	int keylen;
	unsigned char* cipherText;
	int i;
	int cipherLen;	 //including 2 bytes len of plaintext
	unsigned char n,q;
	
	if(!IsComingData(&cipherLen)) return ER_EMPTY;
	err = GetProtocolKey(key, &keylen);
	if((err != ER_OK)) return err;	
	cipherText =  (unsigned char*) malloc(cipherLen);
	if (cipherText == NULL) return ER_IO;
	err = UARTGetData(cipherText, &cipherLen, timeout);
	if(err != ER_OK) return err;
	n = (cipherLen - 2) / 16;
	for (i = 0; i< n; i++){
		memcpy(state, &cipherText[2 + i*16], 16);
		aes_decrypt(state, key);
		memcpy(&buffer[i*16], state, 16);
	}
	memcpy(state, &cipherText[2 + i*16], 16);
	aes_decrypt(state, key);
	*retlen = (cipherText[0] << 8) + cipherText[1];
	q = *retlen % 16;
	memcpy(&buffer[i*16], state, q);

	free(cipherText);
	//debug
	//USART_SendData(UART_CAN, 0xAA);
	//while(USART_GetFlagStatus(UART_CAN, USART_FLAG_TXE) == RESET) ;
	return ER_OK;
}



/**
  * @brief  Check if there is any message waiting in message queue
  * @param  none 
  * @retval TRUE if there is any message in message queue, FALSE if there is no message   
  */
bool IsComingMessage(){
	int len; 
	return IsComingData(&len);
}

