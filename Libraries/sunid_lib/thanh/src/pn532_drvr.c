/**************************************************************************/
/*! 
    @file     pn532_drvr.c
*/
/**************************************************************************/
#include "string.h"

#include "pn532.h"
#include "pn532_drvr.h"
#include "VTimer.h"
#define	SPI_HANDSHAKE
/**
  * @brief  Select sPN532: Chip Select pin low
  */
#define sPN532_CS_LOW()       GPIO_ResetBits(sPN532_CS_GPIO_PORT, sPN532_CS_PIN)
/**
  * @brief  Deselect sPN532: Chip Select pin high
  */
#define sPN532_CS_HIGH()      GPIO_SetBits(sPN532_CS_GPIO_PORT, sPN532_CS_PIN)
/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sPN532_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sPN532_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sPN532_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sPN532_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(sPN532_SPI);
}
/**
  * @brief  Sends a frame through the SPI interface 
  *         
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sPN532_SendFrame(uint8_t * outFrame, uint32_t pSize)
{
  int i = 0;
  /*!< Select the FLASH: Chip Select low */
  sPN532_CS_LOW();
  delay_ms(1);
  for(i = 0; i< pSize; i++)
  	sPN532_SendByte(outFrame[i]);
  delay_ms(1);
  sPN532_CS_HIGH();
  return 0;
}
/**
  * @brief  Sends a frame through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sPN532_ReadFrame(uint8_t * outFrame, uint8_t * intFrame,uint32_t pSize)
{
  int i = 0;
  /*!< Select the FLASH: Chip Select low */
  sPN532_CS_LOW();
  delay_ms(1);
  for(i = 0; i< pSize; i++)
  	intFrame[i] = sPN532_SendByte(outFrame[i]);
  delay_ms(1);
  sPN532_CS_HIGH();
  return 0;
} 
/**
  * @brief  Reset PN532.
  * @param  none  
  * @retval None
  */
void pn532RST(void)
{
  PN532_RST_GPIO_PORT->BRR = PN532_RST_PIN;
  delay_ms(100);
  PN532_RST_GPIO_PORT->BSRR = PN532_RST_PIN;   
  delay_ms(100);
}


/**
  * @brief  Returns the PN532 IRQ state.
  * @param  Switch: none    
  * @retval The Button PN532 IRQ pin value.
  */
uint32_t PN532_IRQGetState(void)
{
  return GPIO_ReadInputDataBit(PN532_IRQ_GPIO_PORT, PN532_IRQ_PIN);
}
/**************************************************************************/
/*! 
    @brief  Initialises SPI and configures the PN532
*/
/**************************************************************************/
void pn532HWInit(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  PN532_DEBUG("Initialising SPI %s", PN532_UART_BAUDRATE, CFG_PRINTF_NEWLINE);
  
  sPN532_LowLevel_Init();
    
  /*!< Deselect the PN532: Chip Select high */
  sPN532_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
#else
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
#endif

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sPN532_SPI, &SPI_InitStructure);

  /*!< Enable the sPN532_SPI  */
  SPI_Cmd(sPN532_SPI, ENABLE);
  //setup PN532 IRQ
 
  /* Enable the PN532 IRQ Clock */
  RCC_APB2PeriphClockCmd( PN532_IRQ_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input pull up */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = PN532_IRQ_PIN;
  GPIO_Init(PN532_IRQ_GPIO_PORT, &GPIO_InitStructure);
  //setup PN532 HREQ
 
  /* Enable the PN532 IRQ Clock */
  RCC_APB2PeriphClockCmd( PN532_RST_GPIO_CLK, ENABLE);

  
  // ToDo: Reset PN532
  pn532RST();
}

/**************************************************************************/
/*! 
    @brief  Builds a standard PN532 frame using the supplied data

    @param  pbtFrame  Pointer to the field that will hold the frame data
    @param  pszFrame  Pointer to the field that will hold the frame length
    @param  pbtData   Pointer to the data to insert in a frame
    @param  swData    Length of the data to insert in bytes

    @note   Possible error messages are:

            - PN532_ERROR_EXTENDEDFRAME
*/
/**************************************************************************/
pn532_error_t pn532BuildFrame(uint8_t * pbtFrame, uint32_t * pszFrame, const uint8_t * pbtData, const uint32_t szData)
{
	// DCS - Calculate data payload checksum
  uint8_t btDCS = (256 - 0xD4);
  uint32_t szPos;
  if (szData > PN532_NORMAL_FRAME__DATA_MAX_LEN) 
  {
    // Extended frames currently unsupported
    return PN532_ERROR_EXTENDEDFRAME;
  }

  // LEN - Packet length = data length (len) + checksum (1) + end of stream marker (1)
  pbtFrame[4] = szData + 1;
  // LCS - Packet length checksum
  pbtFrame[5] = 256 - (szData + 1);
  // TFI
  pbtFrame[6] = 0xD4;
  // DATA - Copy the PN53X command into the packet buffer
  memcpy (pbtFrame + 7, pbtData, szData);

  
  for (szPos = 0; szPos < szData; szPos++) 
  {
    btDCS -= pbtData[szPos];
  }
  pbtFrame[7 + szData] = btDCS;

  // 0x00 - End of stream marker
  pbtFrame[szData + 8] = 0x00;

  (*pszFrame) = szData + PN532_NORMAL_FRAME__OVERHEAD;

  return PN532_ERROR_NONE;
}
/*! 
    @brief  check if PN532 is ready for send back ACK or respond

    @param  time out

    @note   Possible error messages are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_NONE
*/
pn532_error_t pn532IsReady(const int32_t tmo)
{

  unsigned char timerid;
  if (tmo > 0) //if time out is set
  {
  	timerid = VTimer_GetID();
	if(timerid == 0) return PN532_ERROR_BUSY;
	VTimer_Set(timerid, tmo);
#ifdef SPI_HANDSHAKE
	while((PN532_IRQGetState() == SET) 
		&& (VTimer_IsFired(timerid) == 0));
	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return PN532_ERROR_BUSY;
	}
#else //dont use IRQ, read status
	
	while((pn532GetStatus()== PN532_ERROR_BUSY ) && (VTimer_IsFired(timerid) == 0));

	if(VTimer_IsFired(timerid))	{	  //time out
		VTimer_Release(timerid);
		return PN532_ERROR_BUSY;
	}
#endif
	VTimer_Release(timerid);
  }
  else //wait forever
  {
#ifdef SPI_HANDSHAKE
	while(PN532_IRQGetState() == SET);
#else
	while(pn532GetStatus()== PN532_ERROR_BUSY );
#endif  	
  }
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief  Sends the specified command to the PN532, automatically
            creating an appropriate frame for it

    @param  pdbData   Pointer to the byte data to send
    @param  szData    Length in bytes of the data to send

    @note   Possible error messages are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_NOACK
            - PN532_ERROR_INVALIDACK
*/
/**************************************************************************/
pn532_error_t pn532SendCommand(const uint8_t * pbtData, const uint32_t szData, const int32_t tmo)
{
  
  uint8_t abtAck[7] = { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  
  uint8_t abtRxBuf[7];
  // Every packet must start with "01 00 00 ff" , SPI frame
  uint8_t  abtFrame[PN532_BUFFER_LEN] = { 0x01, 0x00, 0x00, 0xff };
  uint32_t szFrame = 0;
  

  // Build the frame
  pn532BuildFrame (abtFrame, &szFrame, pbtData, szData);

  // Keep track of the last command that was sent
  //pn532->lastCommand = pbtData[0];

  // Output the frame data for debugging if requested
  //PN532_DEBUG("Sending  (%02d): ", szFrame);
  //pn532PrintHex(abtFrame, szFrame);

  // Send data to the PN532
  sPN532_SendFrame(abtFrame, szFrame);

  // Wait for ACK
  if (pn532IsReady(tmo)!= PN532_ERROR_NONE)
  {
  	//pn532->state = PN532_STATE_READY;
	//pn532->appError = PN532_APPERROR_TIMEOUT;
  	return PN532_ERROR_NOACK;
  }
  // Read ACK ... this will also remove it from the buffer
  
  sPN532_ReadFrame(abtAck, abtRxBuf,7);
  abtAck[3]=0xff;
  abtAck[5]=0xff;
  // Make sure the received ACK matches the prototype
  if (0 != (arraycmp (abtRxBuf, abtAck,1, 6))) 
  {
//    PN532_DEBUG ("Invalid ACK: ");
//    pn532PrintHex(abtRxBuf, 7);
//    PN532_DEBUG("%s", CFG_PRINTF_NEWLINE);
//    pn532->state = PN532_STATE_READY;
	
    return PN532_ERROR_INVALIDACK;
  }

//  pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief  Sends the specified command to the PN532, automatically
            creating an appropriate frame for it

    @param  pdbData   Pointer to the byte data to send
    @param  szData    Length in bytes of the data to send

    @note   Possible error messages are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_NOACK
            - PN532_ERROR_INVALIDACK
*/
/**************************************************************************/
pn532_error_t pn532GetStatus(void)
{
  uint8_t abtStatus[2] = { 0x02, 0x00};  
  uint8_t abtRxStatus[2]= { 0x02, 0x00};
  //dont use IRQ, read status
	sPN532_ReadFrame(abtStatus, abtRxStatus,2);
	delay_ms(1);
	if((abtRxStatus[1] & 0x01)== 0)
		return PN532_ERROR_BUSY;
	else
		return PN532_ERROR_NONE;	
}
/**************************************************************************/
/*! 
    @brief  Reads a response from the PN532

    @note   Possible error message are:

            - PN532_ERROR_BUSY
            - PN532_ERROR_RESPONSEBUFFEREMPTY
            - PN532_ERROR_PREAMBLEMISMATCH
            - PN532_ERROR_APPLEVELERROR
            - PN532_ERROR_EXTENDEDFRAME
            - PN532_ERROR_LENCHECKSUMMISMATCH
*/
/**************************************************************************/
pn532_error_t pn532ReadResponse(uint8_t * pbtResponse, uint32_t * pszRxLen, const int32_t tmo)
{
  //pn532_pcb_t *pn532 = pn532GetPCB();
  int i, remainLen;
  uint8_t pn53x_preamble[3] = { 0x00, 0x00, 0xff };
  *pszRxLen = 0;
  // Check if we're busy
  //if (pn532->state == PN532_STATE_BUSY)
  //{
  //  return PN532_ERROR_BUSY;
  //}
  // check if PN532 is ready to send respond
  if (pn532IsReady(tmo)!= PN532_ERROR_NONE)
  {
	//pn532->appError = PN532_APPERROR_TIMEOUT;
  	return PN532_ERROR_BUSY;
  }
  // Flag the stack as busy
  //pn532->state = PN532_STATE_BUSY;

  // Reset the app error flag
  //pn532->appError = PN532_APPERROR_NONE;

  
  // Read response from uart
  /*!< Select the FLASH: Chip Select low */
  sPN532_CS_LOW();
  delay_ms(1);
  //send DR
  sPN532_SendByte(0x03);
  //read preamble
  pbtResponse[0] = sPN532_SendByte(0x00);
  pbtResponse[1] = sPN532_SendByte(0x00);
  pbtResponse[2] = sPN532_SendByte(0x00);
  //read respone data length
  pbtResponse[3] = sPN532_SendByte(0x00);
  //calculate remain frame length = LEN + 1(LCS) + 1 (DCS) + 1 (postample)
  remainLen = pbtResponse[3] + 3; 
  for (i = 0; i<remainLen; i++)
  	 pbtResponse[i+4]	= sPN532_SendByte(0x00); 
  //deselect pn532 
  delay_ms(1);
  sPN532_CS_HIGH();
  *pszRxLen = remainLen + 4;
  // Display the raw response data for debugging if requested
  //PN532_DEBUG("Received (%02d): ", *pszRxLen);
  //pn532PrintHex(pbtResponse, *pszRxLen);

  // Check preamble
  
  if (0 != (arraycmp(pbtResponse, pn53x_preamble,0,3))) 
  {
  //  PN532_DEBUG("Frame preamble + start code mismatch%s", CFG_PRINTF_NEWLINE);
  //  pn532->state = PN532_STATE_READY;
    return PN532_ERROR_PREAMBLEMISMATCH;
  }

  // Check the frame type
  if ((0x01 == pbtResponse[3]) && (0xff == pbtResponse[4])) 
  {
    // Error frame
    //PN532_DEBUG("Application level error (%02d)%s", pbtResponse[5], CFG_PRINTF_NEWLINE);
    // Set application error message ID
    //pn532->appError = pbtResponse[5];
    //pn532->state = PN532_STATE_READY;
    return PN532_ERROR_APPLEVELERROR;
  } 
  else if ((0xff == pbtResponse[3]) && (0xff == pbtResponse[4])) 
  {
    // Extended frame
    //PN532_DEBUG("Extended frames currently unsupported%s", CFG_PRINTF_NEWLINE);
    //pn532->state = PN532_STATE_READY;
    return PN532_ERROR_EXTENDEDFRAME;
  } 
  else 
  {
    // Normal frame
    if (256 != (pbtResponse[3] + pbtResponse[4])) 
    {
      // TODO: Retry
      //PN532_DEBUG("Length checksum mismatch%s", CFG_PRINTF_NEWLINE);
      //pn532->state = PN532_STATE_READY;
      return PN532_ERROR_LENCHECKSUMMISMATCH;
    }
    // uint32_t szPayloadLen = abtRx[3] - 2;
  }

  //pn532->state = PN532_STATE_READY;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief      Sends the wakeup sequence to the PN532.
*/
/**************************************************************************/
pn532_error_t pn532Wakeup(const int32_t tmo)
{
  pn532_pcb_t *pn532 = pn532GetPCB();
  pn532_error_t err = PN532_ERROR_NONE;
  PN532_DEBUG("Sending Wakeup Sequence%s", CFG_PRINTF_NEWLINE);

  /*!< Select the FLASH: Chip Select low */
  //select NSS for 1ms atleast
  sPN532_CS_LOW();
  delay_ms(10);
  // that isn't really an error
  pn532->state = PN532_STATE_READY;
  return err;
}
