/**************************************************************************/
/*! 
    @file     pn532.c
*/
/**************************************************************************/
#include "string.h"
#include "pn532.h"
#include "pn532_drvr.h"
#include "stm32sunid.h"
#include "VTimer.h"
#include "uart.h"
#include "des.h"
#define PN532_DEFAULT_TIMEOUT		100


static pn532_pcb_t pcb;
pn532_cardinfo_t card;
uint8_t	sessionKey[8];
void DesfireCryptoPostProcessData(uint8_t * data, int32_t * nbytes, uint8_t communication_settings);
void DesfireCryptoPreProcessData (const uint8_t *data, uint8_t *res, int *nbytes, int communication_settings);

/**************************************************************************/
/*! 
    @brief  Prints a hexadecimal value in plain characters

    @param  pbtData   Pointer to the byte data
    @param  szBytes   Data length in bytes
*/
/**************************************************************************/
void pn532PrintHex(const uint8_t * pbtData, const uint32_t szBytes)
{
  uint32_t szPos;
  for (szPos=0; szPos < szBytes; szPos++) 
  {
    PN532_DEBUG("%02x ", pbtData[szPos]);
  }
  PN532_DEBUG(CFG_PRINTF_NEWLINE);
}

/**************************************************************************/
/*! 
    @brief      Gets a reference to the PN532 peripheral control block,
                which can be used to determine that state of the PN532
                IC, buffers, etc.
*/
/**************************************************************************/
pn532_pcb_t * pn532GetPCB()
{
  return &pcb;
}
/**************************************************************************/
/*! 
    @brief      Gets a reference to cuurent card info.
*/
/**************************************************************************/
pn532_cardinfo_t * pn532GetCardInfo()
{
  return &card;
}

/**************************************************************************/
/*! 
    @brief      Initialises the appropriate serial bus (UART, etc.),and
                sets up any buffers or peripherals required by the PN532.
*/
/**************************************************************************/
void pn532Init(void)
{
  // Clear protocol control blocks
  memset(&pcb, 0, sizeof(pn532_pcb_t));

  // Initialise the underlying HW
  pn532HWInit();
  pn532Wakeup(0);
  // Set the PCB flags to an appropriate state
  pcb.initialised = TRUE;
}
/**************************************************************************/
/*! 
    @brief      Configures the PN532 for normal mode
*/
/**************************************************************************/
pn532_error_t pn532GetRegister (uint16_t ui16Reg, uint8_t * ui8Value)
{
	uint8_t cmd[4];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send SAM config 14 01
	cmd[0] = PN532_COMMAND_READREGISTER;
	cmd[1] = ui16Reg>>8;
	cmd[2] = ui16Reg & 0xff;
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
	return err;
	////UARTSendData(rec,size,2000);
	if (rec[6] != (PN532_COMMAND_READREGISTER+1))
		{
		   return PN532_ERROR_APPLEVELERROR;
		}
	*ui8Value = rec[7];
	
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Configures the PN532 for normal mode
*/
/**************************************************************************/
pn532_error_t pn532SetRegister (uint16_t ui16Reg, uint8_t ui8SymbolMask, uint8_t ui8Value)
{
	uint8_t cmd[4];
	uint8_t rec[32];
	uint8_t read;
	uint32_t size;
	pn532_error_t err;
  	// Get register
  	//send SAM config 14 01
  	err = pn532GetRegister(ui16Reg,&read);
	if (err != PN532_ERROR_NONE)
		return err;
	cmd[0] = PN532_COMMAND_WRITEREGISTER;
	cmd[1] = ui16Reg>>8;
	cmd[2] = ui16Reg & 0xff;
	if(ui8Value!=0)
		cmd[3] = read |	ui8SymbolMask;
	else
		cmd[3] = read &	(~ui8SymbolMask);
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;

	////UARTSendData(rec,size,2000);
	if (rec[6] != (PN532_COMMAND_WRITEREGISTER+1))
	{
	   return PN532_ERROR_APPLEVELERROR;
	}

  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Configures the PN532 for normal mode
*/
/**************************************************************************/
pn532_error_t pn532SAMConfigure(void)
{
	uint8_t cmd[4];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send SAM config 14 01
	cmd[0] = PN532_COMMAND_SAMCONFIGURATION;
	cmd[1] = 0x1;
	cmd[2] = 0x0;
	cmd[3] = 0x1;
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err == PN532_ERROR_NONE)
	{
		//UARTSendData(rec,size,2000);
		if (rec[6] != 0x15)
		{
		   return PN532_ERROR_APPLEVELERROR;
		}
	}
	else
		return err;
	pn532SetRegister(REG_CIU_TX_MODE,SYMBOL_TX_CRC_ENABLE,1);
	pn532SetRegister(REG_CIU_RX_MODE,SYMBOL_RX_CRC_ENABLE,1);
	pn532SetRegister(REG_CIU_MANUAL_RCV, SYMBOL_PARITY_DISABLE, 0);
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Get firmware version
*/
/**************************************************************************/
pn532_error_t pn532GetFirmware(void)
{
	uint8_t cmd[1];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send SAM config 14 01
	cmd[0] = PN532_COMMAND_GETFIRMWAREVERSION;

	err = pn532SendCommand(cmd,1,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err == PN532_ERROR_NONE)
	{
		//UARTSendData(rec,size,2000);
		if (rec[6] != 0x03)
		{
		   return PN532_ERROR_APPLEVELERROR;
		}
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      list Availaible cards with specific modulation and
                baud rate
*/
/**************************************************************************/
pn532_error_t pn532CardList(pn532_modulation_t mod)
{
  // ToDo
  	uint8_t i;
	uint8_t cmd[3];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send InListPassiveTarget command
	cmd[0] = PN532_COMMAND_INLISTPASSIVETARGET;
	cmd[1] = 0x1;  //Maximum number card can be select 
	cmd[2] = mod;
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err == PN532_ERROR_NONE)
	{
		//UARTSendData(rec,size,2000);
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INLISTPASSIVETARGET+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check the number detected cards
		if (rec[7] == 0)
			return PN532_ERROR_NOCARD;
		//else read card information
		card.Tg = rec[8];
		card.type = rec[11];
		card.NFCIDLength = rec[12]; 
		card.authenticated = PICC_NOT_AUTHENTICATED;
		if (card.NFCIDLength > MAX_UID_LEN)
			return PN532_ERROR_APPLEVELERROR;
		for(i = 0; i<card.NFCIDLength;i++)
			card.NFCID[i] = rec[13+i]; 
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Select current card
*/
/**************************************************************************/
pn532_error_t pn532CardSelect(void)
{
  // ToDo
	uint8_t cmd[2];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send InListPassiveTarget command
	cmd[0] = PN532_COMMAND_INSELECT;
	cmd[1] = 1;  //Maximum number card can be select 
	err = pn532SendCommand(cmd,2,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	//UARTSendData(rec,size,2000);
	if (err == PN532_ERROR_NONE)
	{
		
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INSELECT+1))
		   return PN532_ERROR_APPLEVELERROR;
		pcb.appError = rec[7];
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR; 
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      release current card
*/
/**************************************************************************/
pn532_error_t pn532CardRelease(void)
{
  // ToDo
	uint8_t cmd[2];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send InListPassiveTarget command
	cmd[0] = PN532_COMMAND_INRELEASE;
	cmd[1] = 1;  //Maximum number card can be select 
	err = pn532SendCommand(cmd,2,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	//UARTSendData(rec,size,2000);
	if (err == PN532_ERROR_NONE)
	{
		
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INRELEASE+1))
		   return PN532_ERROR_APPLEVELERROR;
		pcb.appError = rec[7];
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR; 
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      DeSelect current card
*/
/**************************************************************************/
pn532_error_t pn532CardDeselect(void)
{
  // ToDo
	uint8_t cmd[2];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send InListPassiveTarget command
	cmd[0] = PN532_COMMAND_INDESELECT;
	cmd[1] = 0;  //Maximum number card can be select 
	err = pn532SendCommand(cmd,2,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	//UARTSendData(rec,size,2000);
	if (err == PN532_ERROR_NONE)
	{
		
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INDESELECT+1))
		   return PN532_ERROR_APPLEVELERROR;
		pcb.appError = rec[7];
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR; 
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/* 
    Mifare Classic section
*/
/**************************************************************************/
/**************************************************************************/
/*! 
    @brief      Login to a card sector of a mifare card
	@blockAddr : sector address
	@keyType	: PN532_MIFARE_AUTHENTICATION_A = KEY A
				  PN532_MIFARE_AUTHENTICATION_B = KEY B
	@key		: buffer contain key
*/
/**************************************************************************/
pn532_error_t pn532MifareLogin(uint8_t blockAddr, uint8_t keyType, uint8_t * key)
{
  // ToDo
  	uint8_t i, cmdLen;
	uint8_t cmd[32];
	uint8_t rec[64];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    if ((keyType != PN532_MIFARE_AUTHENTICATION_A) && (keyType != PN532_MIFARE_AUTHENTICATION_B))
		return PN532_ERROR_LOGINFAIL; 
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  		//Card number returned by previous card select command 
	cmd[2] = keyType;  		//Authentication command
	cmd[3] = blockAddr;	//sector address
	cmd[4] = key[0];		//key
	cmd[5] = key[1];
	cmd[6] = key[2];
	cmd[7] = key[3];
	cmd[8] = key[4];
	cmd[9] = key[5];
	for(i = 0; i<card.NFCIDLength; i++)
		cmd[10+i] = card.NFCID[i];
	cmdLen = 10 + card.NFCIDLength;
	err = pn532SendCommand(cmd,cmdLen,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err == PN532_ERROR_NONE)
	{
		//UARTSendData(rec,size,2000);
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check response status
		pcb.appError = rec[7];
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_LOGINFAIL;
		
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Read 16 bytes of a card sector of a mifare card
	@blockAddr : sector address
	@buf		: buffer address will consist of result
	
*/
/**************************************************************************/
pn532_error_t pn532MifareRead16(uint8_t blockAddr, uint8_t * buf)
{
  // ToDo
  	uint8_t i;
	uint8_t cmd[4];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_MIFARE_READ16;  	//Mifare read command
	cmd[3] = blockAddr;			//sector address
	
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err == PN532_ERROR_NONE)
	{
		//UARTSendData(rec,size,2000);
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check response status
		pcb.appError = rec[7];
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR;
		else
		{
			//read result
			for(i = 0; i<16;i++)
				buf[i] = rec[8+i];
		}
		
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Write to 16 bytes of a card sector of a mifare card
	@blockAddr : sector address
	@verify		: indicate that if verify needed
	@buf		: buffer address consist of writen data
	
*/
/**************************************************************************/
pn532_error_t pn532MifareWrite16(uint8_t blockAddr, uint8_t verify, uint8_t * buf)
{
  // ToDo
  	uint8_t i;
	uint8_t cmd[20];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_MIFARE_WRITE16; 	//Mifare read command
	cmd[3] = blockAddr;			//sector address
	for(i = 0; i<16;i++)
	  cmd[i+4] = buf[i];
	err = pn532SendCommand(cmd,20,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err == PN532_ERROR_NONE)
	{
		//UARTSendData(rec,size,2000);
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check response status
		pcb.appError = rec[7];
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR;
		else
		{
			//read back to verify
			if (verify != 0)
			{
				uint8_t  read[16];
				if(pn532MifareRead16(blockAddr,read) != PN532_ERROR_NONE)
					return PN532_ERROR_VERIFY;
				if (arraycmp(buf, read, 0, 16)!=0)
					return PN532_ERROR_VERIFY; 
			}
		}
		
	}
	else
		return err;
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Write to 16 bytes of a card sector of a mifare card
	@sectorAddr : sector address
	@verify		: indicate that if verify needed
	@buf		: buffer address consist of writen data
	
*/
/**************************************************************************/
pn532_error_t pn532MifareWriteKeyA(uint8_t sectorAddr, uint8_t * newKey)
{
  // ToDo
	uint8_t trailer[16];
	pn532_error_t err;
  // ToDo
  err = pn532MifareRead16((sectorAddr << 2) + 3, trailer);
  if(err != PN532_ERROR_NONE)
  	return err; 
  //setup new key
  trailer[0] = newKey[0];
  trailer[1] = newKey[1];
  trailer[2] = newKey[2];
  trailer[3] = newKey[3];
  trailer[4] = newKey[4];
  trailer[5] = newKey[5];
  err = pn532MifareWrite16 ((sectorAddr << 2) + 3,0,trailer);
  return err;
}
/**************************************************************************/
/*
 *	Mifare Desfire Card Section	
 */
/**************************************************************************/
void xor8 (uint8_t *ivect, uint8_t *data)
{
	int i;
    for (i = 0; i < 8; i++) {
        data[i] ^= ivect[i];
    }  
}
void rol8(uint8_t *data)
{
    int i;
	uint8_t first = data[0];
    for (i = 0; i < 7; i++) {
        data[i] = data[i+1];
    }  
    data[7] = first;
}
//Data Encryption Standard-Cipher Block Chaining
void mifare_cbc_des (uint8_t *key, uint8_t *data, uint8_t *ivect, short direction)
{
    uint8_t ovect[8];
	uint8_t edata[8];
    psDes3Key_t skey;   
    if (direction == 1) {
        int i;
	    for (i = 0; i < 8; i++) {
	        data[i] ^= ivect[i];
	    }
    } else {
        memcpy (ovect, data, 8);
    }
	////UARTSendData(key,8,2000);
	////UARTSendData(ivect,8,2000);
	////UARTSendData(data,8,2000);  
    psDesInitKey((const unsigned char *)key, 8, &skey);
    psDesDecryptBlock((const unsigned char *)data, edata,&skey);
    memcpy (data, edata, 8);
    ////UARTSendData(edata,8,2000);   
    if (direction == 1) {
        memcpy (ivect, data, 8);
    } else {
        xor8 (ivect, data);
        memcpy (ivect, ovect, 8);
    }
	
}
void mifare_cbc_block_des (uint8_t * key, uint8_t *data, uint32_t data_size, uint8_t direction)
{
    uint32_t offset = 0;
    uint8_t ivect[8];
    memset (ivect, 0, 8);

    while (offset < data_size) {
	mifare_cbc_des (key, data + offset, ivect, direction);
	offset += 8;
    }

}
/**************************************************************************/
/*! 
    @brief      Configures the PN532 for a specific modulation and
                baud rate
*/
/**************************************************************************/
pn532_error_t pn532DesfileSelect(pn532_modulation_t mod)
{
  // ToDo
  	uint8_t i;
	uint8_t cmd[16];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
  //send InListPassiveTarget command
	cmd[0] = PN532_COMMAND_INLISTPASSIVETARGET;
	cmd[1] = 1;  //Passive 
	cmd[2] = mod;
	cmd[3] = 0x88;
	memcpy(cmd+4,card.NFCID,7);
	err = pn532SendCommand(cmd,11,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	//UARTSendData(rec,size,2000);
	if (err == PN532_ERROR_NONE)
	{
		
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INSELECT+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check the number detected cards
		if (rec[7] == 0)
			return PN532_ERROR_NOCARD;
		//else read card information
		card.Tg = rec[8];
		card.type = rec[11];
		card.NFCIDLength = rec[12];
		card.authenticated = PICC_NOT_AUTHENTICATED; 
		if (card.NFCIDLength > MAX_UID_LEN)
			return PN532_ERROR_APPLEVELERROR;
		for(i = 0; i<card.NFCIDLength;i++)
			card.NFCID[i] = rec[13+i]; 
	}
	else
		return err;
  return PN532_ERROR_NONE;
}

/**************************************************************************/
/*! 
    @brief  Authentication with Desfire card
	@key	: buffer contains key (8bytes)
	@keyno	: Application ID	
*/
/**************************************************************************/
pn532_error_t pn532DesfireAuthenticate(uint8_t * key, uint8_t keyno)
{
    uint8_t ivec[8];
	uint8_t cmd[32];
	uint8_t rec[32];

	uint8_t PICC_E_RndB[8];
	uint8_t PICC_RndB[8];
	uint8_t PCD_r_RndB[8];
	uint8_t PCD_RndA[8];
	uint8_t PICC_E_RndA_s[8];
	uint8_t PICC_RndA_s[8];
	uint8_t PCD_RndA_s[8];
	uint8_t token[16];

	pn532_error_t err;
	uint32_t size;
    memset (ivec, 0, 8);
    //DESFire Native TDES authentication
    //A trusted communication channel will be established
    //Access conditions related to the key are enabled
    //A session key will be generated
    //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_AUTHENTICATION;  	//Desfire Authenticate
	cmd[3] = keyno;
	
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);    
    if (err != PN532_ERROR_NONE)
		return err;
	
	////UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	card.piccError = rec[8];
	if (rec[8] != 0xAF) //status from desfire   
 		return PN532_ERROR_PICCLEVELERROR;

    memcpy (PICC_E_RndB, &rec[9], 8);
    ////UARTSendData(PICC_E_RndB,8,2000);
       
    memcpy (PICC_RndB, PICC_E_RndB, 8);
    mifare_cbc_des (key, PICC_RndB, ivec, 0);
    ////UARTSendData(PICC_RndB,8,2000);
       
    psGetPrng(PCD_RndA,8);
    ////UARTSendData(PCD_RndA,8,2000);
    
    memcpy (PCD_r_RndB, PICC_RndB, 8);
    rol8 (PCD_r_RndB);
       
    
    memcpy (token, PCD_RndA, 8);
    memcpy (token+8, PCD_r_RndB, 8);
       
    ////UARTSendData(token,16,2000);
       
    memset (ivec, 0, sizeof (ivec));
    mifare_cbc_des (key, token, ivec, 1);
    mifare_cbc_des (key, token+8, ivec, 1);
       
    ////UARTSendData(token,16,2000);
       
    //send back RndA+RndB'
    cmd[2] = 0xAF;
    memcpy (cmd+3, token, 16);
       
    err = pn532SendCommand(cmd,19,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);    
    if (err != PN532_ERROR_NONE)
		return err;
	
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	card.piccError = rec[8];
	if (rec[8] != 0x00) //status from desfire   
 		return PN532_ERROR_PICCLEVELERROR;
 
        
    memcpy (PICC_E_RndA_s, &rec[9], 8);
    ////UARTSendData(PICC_E_RndA_s,8,2000);
    memcpy (PICC_RndA_s, PICC_E_RndA_s, 8);
    memset (ivec, 0, sizeof (ivec));
    mifare_cbc_des (key, PICC_RndA_s, ivec, 0);
    ////UARTSendData(PICC_RndA_s,8,2000);
       
    
    memcpy (PCD_RndA_s, PCD_RndA, 8);
    rol8 (PCD_RndA_s);
	memcpy(card.sessionKey,PCD_RndA,4);
	memcpy((card.sessionKey)+4,PICC_RndB,4);
	memcpy((card.sessionKey)+8,PCD_RndA+4,4);
	memcpy((card.sessionKey)+12,PICC_RndB+4,4);
    ////UARTSendData(card.sessionKey,8,2000);
	card.authenticated = PICC_AUTHENTICATED;
	card.keyNo = keyno;
	return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief  change key setting for the current authenticated app
	@key	: buffer contains key (8bytes)
	@keySetting	: Application key Setting	
*/
/**************************************************************************/
pn532_error_t pn532DesfireChangeKeySetting(uint8_t * keySetting)
{
    uint8_t ivec[16];
	uint8_t cmd[20];
	uint8_t rec[32];
	uint8_t token[16];
	uint16_t  checksum;
	pn532_error_t err;
	uint32_t size;
    memset (ivec, 0, 16);
	memset (token, 0,16);
    
	token[0] = *keySetting;
	checksum = CRC16_Generate(token,1);
	token[1] = checksum & 0xff;
	token[2] = (checksum>>8) & 0xff;
	////UARTSendData(token,8,2000);
	////UARTSendData(card.sessionKey,8,2000);
	mifare_cbc_des (card.sessionKey, token, ivec, 1);
	////UARTSendData(token,8,2000);
    //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_CHANGEKEYSETTING;  	//Desfire Authenticate
	memcpy(cmd+3,token,8);
	////UARTSendData(cmd,11,2000);	
	err = pn532SendCommand(cmd,11,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);    
    if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	card.piccError = rec[8];
	if (rec[8] != 0x00) //status from desfire   
 		return PN532_ERROR_PICCLEVELERROR;

    return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief  change key setting for the current authenticated app
	@authenticated: indicate if using current session key or not
	@key	: buffer contains key (8bytes)
	@keySetting	: Application key Setting	
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetKeySetting(uint8_t * keySetting, uint8_t * maxKey)
{
	uint8_t cmd[20];
	uint8_t rec[32];
	pn532_error_t err;
	uint32_t size;

    //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;//card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETKEYSETTING;  	//Desfire Authenticate
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);

	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT); 
	//UARTSendData(rec,size,2000);   
    if (err != PN532_ERROR_NONE)
		return err;
	
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	card.piccError = rec[8];
	if (rec[8] != 0x00) //status from desfire   
 		return PN532_ERROR_PICCLEVELERROR;
	
	*keySetting = rec[9];
	*maxKey = rec[10];
    return PN532_ERROR_NONE;
	
}
/**************************************************************************/
/*! 
    @brief  change key setting for the current authenticated app
	@key	: buffer contains key (8bytes)
	@keySetting	: Application key Setting	
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetKeyVersion(uint8_t keyNo, uint8_t * keyVersion)
{
	uint8_t cmd[20];
	uint8_t rec[32];
	pn532_error_t err;
	uint32_t size;

    //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETKEYVERSION;  	//Desfire Authenticate
	cmd[3] = keyNo;	
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT+500); 
	//UARTSendData(rec,size,2000);   
    if (err != PN532_ERROR_NONE)
		return err;
	
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	card.piccError = rec[8];
	if (rec[8] != 0x00) //status from desfire   
 		return PN532_ERROR_PICCLEVELERROR;
	
	*keyVersion = rec[9];
    return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief  change key for current authenticated app
	@oldKey	: buffer contains current key (16bytes)
	@newKey	: buffer contains new key (16bytes)
	@keyNo	: key number	
*/
/**************************************************************************/
pn532_error_t pn532DesfireChangeKey(uint8_t keyNo, uint8_t * oldKey, uint8_t * newKey)
{
  uint8_t ivec[8];
	uint8_t cmd[32];
	uint8_t rec[32];
	uint8_t token[24];
	uint16_t  checksum;
	pn532_error_t err;
	uint32_t size;
    memset (ivec, 0, 8);
	memset (token, 0, 24);
	if(keyNo != card.keyNo)
	{
		memcpy(token,oldKey,16);
		xor8(newKey,token);
		xor8(newKey+8,token+8);
		checksum = CRC16_Generate(token,16);
		
		token[16] = checksum & 0xff;
		token[17] = (checksum>>8) & 0xff;
		checksum = CRC16_Generate(newKey,16);
		token[18] = checksum & 0xff;
		token[19] = (checksum>>8) & 0xff;
	}
	else
	{
		memcpy(token,newKey,16);
		checksum = CRC16_Generate(token,16);
		
		token[16] = checksum & 0xff;
		token[17] = (checksum>>8) & 0xff;
		
	}
	////UARTSendData(token,24,2000);
	mifare_cbc_des (card.sessionKey, token, ivec, 1);
	mifare_cbc_des (card.sessionKey, token+8, ivec, 1);
	mifare_cbc_des (card.sessionKey, token+16, ivec, 1);
	////UARTSendData(token,24,2000);
    //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_CHANGEKEY;  	//Desfire Authenticate
	cmd[3] = keyNo;
	memcpy(cmd+4,token,24);	
	err = pn532SendCommand(cmd,28,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);    
    if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	card.piccError = rec[8];
	if (rec[8] != 0x00) //status from desfire   
 		return PN532_ERROR_PICCLEVELERROR;

    return PN532_ERROR_NONE;
}
//--------------------------------------------------------------------------
//PICC level command
/**************************************************************************/
/*! 
    @brief      Get version of Dresfire card
	@verbuf: contains return version data
			0-6: hardware info
			7-13: software version
			14-20: UID
			21-25: batch no
			26: cw prod
			27 year prod
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetVersion(uint8_t * verbuf)
{
   // ToDo
	uint8_t cmd[8];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETVERSION;
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0xAF)
		return PN532_ERROR_PICCLEVELERROR;
	//copy hardware info
	memcpy(verbuf,&rec[9],7);

	//continue software version
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;
	cmd[2] = 0xAF;
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0xAF)
		return PN532_ERROR_PICCLEVELERROR;
	//copy software info
	memcpy(verbuf+7,rec+9,7);

	//continue UID version
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;
	cmd[2] = 0xAF;
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x00)
		return PN532_ERROR_PICCLEVELERROR;
	//copy software info
	memcpy(verbuf+14,rec+9,14);
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      create Application
	@app:	buffer contain DESFIRE_CreateAID (5 byte)
*/
/**************************************************************************/
pn532_error_t pn532CreateApplication(DESFIRE_CreateAID * app)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_CREATEAPP;
	cmd[3] = app->AID[0];
	cmd[4] = app->AID[1];
	cmd[5] = app->AID[2];
	cmd[6] = app->keySetting;
	cmd[7] = app->numKey;
	
	err = pn532SendCommand(cmd,8,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
	return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      delete Application (require authenticated to picc master before)
	@app:	buffer contain DESFIRE_CreateAID (5 byte)
*/
/**************************************************************************/
pn532_error_t pn532DeleteApplication(DESFIRE_CreateAID * app)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_DELETEAPP;
	cmd[3] = app->AID[0];
	cmd[4] = app->AID[1];
	cmd[5] = app->AID[2];
		
	err = pn532SendCommand(cmd,6,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
	return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      select Application
	@app:	buffer contain DESFIRE_CreateAID (5 byte)
*/
/**************************************************************************/
pn532_error_t pn532SelectApplication(DESFIRE_CreateAID * app)
{
  // ToDo
	uint8_t cmd[8];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_SELECTAPP;
	cmd[3] = app->AID[0];
	cmd[4] = app->AID[1];
	cmd[5] = app->AID[2];
		
	err = pn532SendCommand(cmd,6,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
	card.authenticated = PICC_NOT_AUTHENTICATED;
	return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      FormatPICC, remain PICC masterkey
	@app:	buffer contain DESFIRE_CreateAID (5 byte)
*/
/**************************************************************************/
pn532_error_t pn532FormatPICC(void)
{
  // ToDo
	uint8_t cmd[8];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_FORMATPICC;
		
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status of pn532
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
	return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Get all installed Application ID
	@AIDs: 	buffer contain all application id (3bytes for each)
	@sizesz: return number of installed apps	
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetAIDs(uint8_t * AIDs, uint32_t * sizesz)
{
  // ToDo
	uint8_t cmd[8];
	uint8_t rec[128];
	uint32_t size;
	uint32_t index = 0;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETAPPID;  	//Mifare read command
	
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
		return PN532_ERROR_APPLEVELERROR;
    *sizesz = 0;
	if (rec[8] == 0xaf)
	{
		//read first frame
		//11 = 1(preamble) + 2(start)+ 1(len) + 1(LCS)+ 1(TFI)+1(pn532)+1(pnstatus)+1(desfirestatus)+1(DCS)+1(post)
		memcpy(AIDs,&rec[9],size-11);
		*sizesz = *sizesz + (size-11)/3;
		index = size-11;
		cmd[2] = 0xaf;
		err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
		delay_ms(1);
		if (err != PN532_ERROR_NONE)
			return err;
		err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
		if (err != PN532_ERROR_NONE)
			return err;
	}
	//read remain
	//status retrn by card
	card.piccError = rec[8];
	if(rec[8]!=0x00)
		return PN532_ERROR_PICCLEVELERROR;
	memcpy(AIDs+index,&rec[9],size-11);
	*sizesz = *sizesz + (size-11)/3;
  return PN532_ERROR_NONE;
}


//--------------------------------------------------------------------------
//Application level command
/**************************************************************************/
/*! 
    @brief      Get all File Id of an Application
	@FIDs: 	buffer contain all file id (1bytes for each)
	@sizesz: return number of file	
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetFileIDs(uint8_t * FIDs, uint32_t * sizesz)
{
  // ToDo
	uint8_t cmd[8];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETFILEID;  	//Mifare read command
	
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
    *sizesz = 0;

	//read ids
	memcpy(FIDs,&rec[9],size-11);
	*sizesz = (size-11);
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Get File setting
	@fileSetting: 	structure contain file number and returned file setting
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetFileSettings(DESFIRE_FileSetting * fileSetting)
{
  // ToDo
	uint8_t cmd[8];
	uint8_t rec[64];
	uint32_t size;
	uint32_t tmp;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETFILESETTING;  	//Mifare read command
	cmd[3] = fileSetting->fileNo;
	
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
    //read setting
	fileSetting->fileType = rec[9];
	fileSetting->commSetting = rec[10];
	fileSetting->accessRight = rec[12];
	fileSetting->accessRight = (fileSetting->accessRight << 8) | rec[11];
	switch(fileSetting->fileType)
	{
		case DESFIRE_STANDARD_FILE:
		case DESFIRE_BACKUP_FILE:
			tmp = 0;
			tmp = rec[15];
			tmp = (tmp<<8)|rec[14];
			tmp = (tmp<<8)|rec[13];
			(fileSetting->file).stdFile.size = tmp;
			break;
		case DESFIRE_VALUE_FILE:
			tmp = 0;
			tmp = rec[16];
			tmp = (tmp<<8)|rec[15];
			tmp = (tmp<<8)|rec[14];
			tmp = (tmp<<8)|rec[13];
			(fileSetting->file).valueFile.lowerLimit = tmp;
			tmp = rec[20];
			tmp = (tmp<<8)|rec[19];
			tmp = (tmp<<8)|rec[18];
			tmp = (tmp<<8)|rec[17];
			(fileSetting->file).valueFile.upperLimit = tmp;
			tmp = rec[24];
			tmp = (tmp<<8)|rec[23];
			tmp = (tmp<<8)|rec[22];
			tmp = (tmp<<8)|rec[21];
			(fileSetting->file).valueFile.value = tmp;
			(fileSetting->file).valueFile.limitedCreditEn = rec[25];
			break;
		case DESFIRE_LINEAR_FILE:
		case DESFIRE_CYCLIC_FILE:
			tmp = 0;
			tmp = rec[15];
			tmp = (tmp<<8)|rec[14];
			tmp = (tmp<<8)|rec[13];
			(fileSetting->file).recordFile.size = tmp;
			tmp = rec[18];
			tmp = (tmp<<8)|rec[17];
			tmp = (tmp<<8)|rec[16];
			(fileSetting->file).recordFile.maxRecord = tmp;
			tmp = rec[21];
			tmp = (tmp<<8)|rec[20];
			tmp = (tmp<<8)|rec[19];
			(fileSetting->file).recordFile.currentRecord = tmp;
			break;
		default:
			break;
	}
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Change File setting
	@fileSetting: 	structure contain file number and returned file setting

	free to access
*/
/**************************************************************************/
pn532_error_t pn532DesfireChangeFileSettings(uint8_t authenticated,DESFIRE_FileSetting * fileSetting)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[32];
	uint32_t size;
	uint8_t token[8];
	uint8_t ivec[8];
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_CHANGEFILESETTING;  	//Mifare read command
	cmd[3] = fileSetting->fileNo;
	if(authenticated == PICC_NOT_AUTHENTICATED)
	{
		
		cmd[4] = fileSetting->commSetting;
		cmd[5] = fileSetting->accessRight & 0xff;
		cmd[6] = (fileSetting->accessRight >> 8)& 0xff;
		
		err = pn532SendCommand(cmd,7,PN532_DEFAULT_TIMEOUT);
		delay_ms(1);
		if (err != PN532_ERROR_NONE)
			return err;
	}
	else
	{
		uint16_t checksum;
		memset(token,0,8);
		memset(ivec,0,8);
		token[0] = 	fileSetting->commSetting;
		token[1] = fileSetting->accessRight & 0xff;
		token[2] = (fileSetting->accessRight >> 8)& 0xff;
		checksum = CRC16_Generate(token,3);
		token[3] = checksum & 0xff;
		token[4] = (checksum>>8) & 0xff;
		mifare_cbc_des (card.sessionKey, token, ivec, 1);
		memcpy(cmd+4,token,8);
		err = pn532SendCommand(cmd,12,PN532_DEFAULT_TIMEOUT);
		delay_ms(1);
		if (err != PN532_ERROR_NONE)
			return err;
	}
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      creat File
	@fileSetting: 	structure contain file setting to create
	free to access
*/
/**************************************************************************/
pn532_error_t pn532DesfireCreateFile(DESFIRE_FileSetting * fileSetting)
{
  // ToDo
	uint8_t cmd[32];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 

	cmd[3] = fileSetting->fileNo;
	cmd[4] = fileSetting->commSetting;
	cmd[5] = fileSetting->accessRight & 0xff;
	cmd[6] = (fileSetting->accessRight >> 8)& 0xff;
			
	switch(fileSetting->fileType)
	{
		case DESFIRE_STANDARD_FILE:
			cmd[2] = PN532_DESFIRE_CREATESTDFILE; 
			cmd[7] = (((fileSetting->file).stdFile).size) & 0xff;
			cmd[8] = ((((fileSetting->file).stdFile).size) >> 8)& 0xff;		
			cmd[9] = ((((fileSetting->file).stdFile).size) >> 16)& 0xff;		
			err = pn532SendCommand(cmd,10,PN532_DEFAULT_TIMEOUT);
			break;
		case DESFIRE_BACKUP_FILE:
			cmd[2] = PN532_DESFIRE_CREATEBACKUPDATAFILE; 
			cmd[7] = (((fileSetting->file).stdFile).size) & 0xff;
			cmd[8] = ((((fileSetting->file).stdFile).size) >> 8)& 0xff;		
			cmd[9] = ((((fileSetting->file).stdFile).size) >> 16)& 0xff;		
			err = pn532SendCommand(cmd,10,PN532_DEFAULT_TIMEOUT);
			break;
		case DESFIRE_VALUE_FILE:
			cmd[2] = PN532_DESFIRE_CREATEVALUEFILE; 
			cmd[7] = (((fileSetting->file).valueFile).lowerLimit) & 0xff;
			cmd[8] = ((((fileSetting->file).valueFile).lowerLimit) >> 8)& 0xff;		
			cmd[9] = ((((fileSetting->file).valueFile).lowerLimit) >> 16)& 0xff;
			cmd[10] = ((((fileSetting->file).valueFile).lowerLimit) >> 24)& 0xff;
			cmd[11] = (((fileSetting->file).valueFile).upperLimit) & 0xff;
			cmd[12] = ((((fileSetting->file).valueFile).upperLimit) >> 8)& 0xff;		
			cmd[13] = ((((fileSetting->file).valueFile).upperLimit) >> 16)& 0xff;
			cmd[14] = ((((fileSetting->file).valueFile).upperLimit) >> 24)& 0xff;
			cmd[15] = (((fileSetting->file).valueFile).value) & 0xff;
			cmd[16] = ((((fileSetting->file).valueFile).value) >> 8)& 0xff;		
			cmd[17] = ((((fileSetting->file).valueFile).value) >> 16)& 0xff;
			cmd[18] = ((((fileSetting->file).valueFile).value) >> 24)& 0xff;	
			cmd[19] = ((fileSetting->file).valueFile).limitedCreditEn;	
			err = pn532SendCommand(cmd,20,PN532_DEFAULT_TIMEOUT);
			break;
		case DESFIRE_LINEAR_FILE:
			cmd[2] = PN532_DESFIRE_CREATELINEARRECORDFILE; 
			cmd[7] = (((fileSetting->file).recordFile).size) & 0xff;
			cmd[8] = ((((fileSetting->file).recordFile).size) >> 8)& 0xff;		
			cmd[9] = ((((fileSetting->file).recordFile).size) >> 16)& 0xff;
			
			cmd[10] = (((fileSetting->file).recordFile).maxRecord) & 0xff;
			cmd[11] = ((((fileSetting->file).recordFile).maxRecord) >> 8)& 0xff;		
			cmd[12] = ((((fileSetting->file).recordFile).maxRecord) >> 16)& 0xff;
			err = pn532SendCommand(cmd,13,PN532_DEFAULT_TIMEOUT);
			break;
		case DESFIRE_CYCLIC_FILE:
			cmd[2] = PN532_DESFIRE_CREATECYCLICRECORDFILE; 
			cmd[7] = (((fileSetting->file).recordFile).size) & 0xff;
			cmd[8] = ((((fileSetting->file).recordFile).size) >> 8)& 0xff;		
			cmd[9] = ((((fileSetting->file).recordFile).size) >> 16)& 0xff;
			
			cmd[10] = (((fileSetting->file).recordFile).maxRecord) & 0xff;
			cmd[11] = ((((fileSetting->file).recordFile).maxRecord) >> 8)& 0xff;		
			cmd[12] = ((((fileSetting->file).recordFile).maxRecord) >> 16)& 0xff;
			err = pn532SendCommand(cmd,13,PN532_DEFAULT_TIMEOUT);
			break;
		default:
			break;
	}
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Delete File
	@fileNo: 	file number to be deleted
	
*/
/**************************************************************************/
pn532_error_t pn532DesfireDeleteFile(uint8_t fileNo)
{
  // ToDo
	uint8_t cmd[8];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_DELETEFILE;  	//Mifare read command
	cmd[3] = fileNo;
	
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      read standard or backup file
	@fileNo : contain file no
	@offset: start postion to read
	@length: lenght to read
	@cs: communication setting
	@data	: 	buffer contain result
	@sizesz: return number of result data	
*/
/**************************************************************************/
pn532_error_t pn532DesfireReadData(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs, uint8_t * data, int32_t * sizesz)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[128];
	uint8_t encryptres[128];
	uint32_t size;
	uint32_t index = 0;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_READDATA;  	//Mifare read command
	cmd[3] = fileNo;
	cmd[4] = offset & 0xff;
	cmd[5] = (offset>>8) & 0xff;
	cmd[6] = (offset>>16) & 0xff;
	cmd[7] = length & 0xff;
	cmd[8] = (length>>8) & 0xff;
	cmd[9] = (length>>16) & 0xff;
	
	err = pn532SendCommand(cmd,10,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
		return PN532_ERROR_APPLEVELERROR;
    *sizesz = 0;
	index = 0;
	while(rec[8] == 0xaf)
	{
		//read first frame
		//11 = 1(preamble) + 2(start)+ 1(len) + 1(LCS)+ 1(TFI)+1(pn532)+1(pnstatus)+1(desfirestatus)+1(DCS)+1(post)
		memcpy(encryptres+index,&rec[9],size-11);
		*sizesz = *sizesz + (size-11);
		index += (size-11);
		cmd[2] = 0xaf;
		err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
		delay_ms(1);
		if (err != PN532_ERROR_NONE)
			return err;
		err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
		if (err != PN532_ERROR_NONE)
			return err;
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check response status
		pcb.appError = rec[7];								 
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR;
		//status retrn by card
		card.piccError = rec[8];
		if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
			return PN532_ERROR_APPLEVELERROR;
	}
	//read remain
	memcpy(encryptres+index,&rec[9],size-11);
	*sizesz = *sizesz + (size-11);
	DesfireCryptoPostProcessData(encryptres,sizesz,cs);
	memcpy(data,encryptres,length);
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      write standard or backup file
	@fileNo : contain file no
	@offset: start postion to read
	@length: lenght to read
	@data: 	buffer contain data
	@sizesz: return number of writen data	
*/
/**************************************************************************/
pn532_error_t pn532DesfireWriteData(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs,const uint8_t * data, int32_t * sizesz)
{
  // ToDo
	uint8_t cmd[128];
	uint8_t res[128];
	uint8_t rec[32];
	uint32_t size;
	uint32_t index = 0;
	uint32_t prepair = 0;
	pn532_error_t err;
  // ToDo
    *sizesz = length;
	DesfireCryptoPreProcessData(data,res,sizesz,cs);
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_WRITEDATA;  	//Mifare read command
	cmd[3] = fileNo;
	cmd[4] = offset & 0xff;
	cmd[5] = (offset>>8) & 0xff;
	cmd[6] = (offset>>16) & 0xff;
	cmd[7] = length & 0xff;
	cmd[8] = (length>>8) & 0xff;
	cmd[9] = (length>>16) & 0xff;
	if (*sizesz <= 52)
	{
		prepair = *sizesz;
		
	}
	else
		prepair = 52;
	memcpy(cmd+10,res,prepair);
	err = pn532SendCommand(cmd,10+prepair,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
		return PN532_ERROR_APPLEVELERROR;
    //*sizesz = prepair;
	index = prepair;
	if(*sizesz>52)
	{
		while(index < (*sizesz))
		{
			//write next frame
			if(((*sizesz)-index) > 59)
				prepair = 59;
			else
				prepair = (*sizesz)-index;
			cmd[2] = 0xaf;
			memcpy(cmd + 3, res+index,prepair);
			
			
			err = pn532SendCommand(cmd,3+prepair,PN532_DEFAULT_TIMEOUT);
			delay_ms(1);
			if (err != PN532_ERROR_NONE)
				return err;
			err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
			if (err != PN532_ERROR_NONE)
				return err;
			//check respone command 
			if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
			   return PN532_ERROR_APPLEVELERROR;
			//check response status
			pcb.appError = rec[7];								 
			if ((rec[7] & 0x3f) != 0)
				return PN532_ERROR_APPLEVELERROR;
			//status retrn by card
			card.piccError = rec[8];
			if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
				return PN532_ERROR_APPLEVELERROR;
			//*sizesz = *sizesz + prepair;
			index += prepair;
		}
	}

  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      read value file
	@fileNo : contain file no
	@value: 	return signed value
	@cs: file communicating setting: 0: plain data, 1: Maced, 3: des	
*/
/**************************************************************************/
pn532_error_t pn532DesfireGetValue(uint8_t fileNo, int32_t * value, uint8_t cs)
{
  // ToDo
	uint8_t cmd[10];
	uint8_t rec[32];
	uint8_t data[8];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_GETVALUE;  	//Mifare read command
	cmd[3] = fileNo;
		
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_APPLEVELERROR;
    //read remain
	memcpy(data,&rec[9],size-11);
	if (cs) {
		int32_t sizesz = size - 11;
		DesfireCryptoPostProcessData(data,&sizesz,cs);
		if(sizesz != 4)
			return PN532_ERROR_PICCLEVELERROR;
    }
	*value = ((unsigned int)data[3] << 24)|((unsigned int)data[2] << 16)
			|((unsigned int)data[1] << 8)|((unsigned int)data[0]);
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      increase value file
	@fileNo : contain file no
	@value: 	signed value to credit
	@cs: file communicating setting: 0: plain data, 1: Maced, 3: des
	
*/
/**************************************************************************/
pn532_error_t pn532DesfireCredit(uint8_t fileNo, int32_t value, uint8_t cs)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[32];
	uint8_t data[8];
	uint8_t edata[8];
	uint32_t size;
	int32_t nsize;
	pn532_error_t err;
  // Encrypt data before sending
  	memset(data,0,8);
    data[0] = ((uint32_t)value) & 0xff;
	data[1] = ((uint32_t)value >> 8) & 0xff;
	data[2] = ((uint32_t)value >> 16) & 0xff;
	data[3] = ((uint32_t)value >> 24) & 0xff;
	nsize = 4;
	DesfireCryptoPreProcessData(data,edata,&nsize,cs);
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_CREDIT;  	//Mifare read command
	cmd[3] = fileNo;
	memcpy(cmd+4,edata,nsize);	
	err = pn532SendCommand(cmd,4+nsize,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	if (rec[8]  != 0x0)
		return PN532_ERROR_APPLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      decrease value file
	@fileNo : contain file no
	@value: 	signed value to debit
	@cs: file communication setting 0: plain data, 1: MACed, 3: (3)DES
*/
/**************************************************************************/
pn532_error_t pn532DesfireDebit(uint8_t fileNo, int32_t value, uint8_t cs)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[32];
	uint8_t data[8];
	uint8_t edata[8];
	uint32_t size;
	int32_t nsize;
	pn532_error_t err;
  // Encrypt data before sending
  	memset(data,0,8);
    data[0] = (uint32_t)value & 0xff;
	data[1] = ((uint32_t)value >> 8) & 0xff;
	data[2] = ((uint32_t)value >> 16) & 0xff;
	data[3] = ((uint32_t)value >> 24) & 0xff;
	nsize = 4;
	DesfireCryptoPreProcessData(data,edata,&nsize,cs);
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_DEBIT;  	//Mifare read command
	cmd[3] = fileNo;
	memcpy(cmd+4,edata,nsize);	
	err = pn532SendCommand(cmd,4+nsize,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	if (rec[8]  != 0x0)
		return PN532_ERROR_APPLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      limited increase value file
	@fileNo : contain file no
	@value: 	signed value to credit
	@cs: file communicating setting: 0: plain data, 1: Maced, 3: des
*/
/**************************************************************************/
pn532_error_t pn532DesfireLimitedCredit(uint8_t fileNo, int32_t value, uint8_t cs)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[32];
	uint8_t data[8];
	uint8_t edata[8];
	uint32_t size;
	int32_t nsize;
	pn532_error_t err;
  // Encrypt data before sending
  	memset(data,0,8);
    data[0] = (uint32_t)value & 0xff;
	data[1] = ((uint32_t)value >> 8) & 0xff;
	data[2] = ((uint32_t)value >> 16) & 0xff;
	data[3] = ((uint32_t)value >> 24) & 0xff;
	nsize = 4;
	DesfireCryptoPreProcessData(data,edata,&nsize,cs);
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_LIMITTEDCREDIT;  	//Mifare read command
	cmd[3] = fileNo;
	memcpy(cmd+4,edata,nsize);	
	err = pn532SendCommand(cmd,4+nsize,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_APPLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      write record file
	@fileNo : contain file no
	@offset: start postion to read
	@length: lenght to read
	@data: 	buffer contain data
	@sizesz: return number of writen data	
*/
/**************************************************************************/
pn532_error_t pn532DesfireWriteRecord(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs, uint8_t * data, int32_t * sizesz)
{
  // ToDo
	uint8_t cmd[128];
	uint8_t res[128];
	uint8_t rec[32];
	uint32_t size;
	uint32_t index = 0;
	uint32_t prepair = 0;
	pn532_error_t err;
  // ToDo
    *sizesz = length;
	DesfireCryptoPreProcessData(data,res,sizesz,cs);
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_WRITERECORD;  	//Mifare read command
	cmd[3] = fileNo;
	cmd[4] = offset & 0xff;
	cmd[5] = (offset>>8) & 0xff;
	cmd[6] = (offset>>16) & 0xff;
	cmd[7] = length & 0xff;
	cmd[8] = (length>>8) & 0xff;
	cmd[9] = (length>>16) & 0xff;
	if (*sizesz <= 52)
	{
		prepair = *sizesz;
		
	}
	else
		prepair = 52;
	memcpy(cmd+10,res,prepair);
	err = pn532SendCommand(cmd,10+prepair,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
		return PN532_ERROR_APPLEVELERROR;
    //*sizesz = prepair;
	index = prepair;
	if(*sizesz>52)
	{
		while(index < (*sizesz))
		{
			//write next frame
			if(((*sizesz)-index) > 59)
				prepair = 59;
			else
				prepair = (*sizesz)-index;
			cmd[2] = 0xaf;
			memcpy(cmd + 3, res+index,prepair);
			
			
			err = pn532SendCommand(cmd,3+prepair,PN532_DEFAULT_TIMEOUT);
			delay_ms(1);
			if (err != PN532_ERROR_NONE)
				return err;
			err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
			if (err != PN532_ERROR_NONE)
				return err;
			//check respone command 
			if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
			   return PN532_ERROR_APPLEVELERROR;
			//check response status
			pcb.appError = rec[7];								 
			if ((rec[7] & 0x3f) != 0)
				return PN532_ERROR_APPLEVELERROR;
			//status retrn by card
			card.piccError = rec[8];
			if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
				return PN532_ERROR_APPLEVELERROR;
			//*sizesz = *sizesz + prepair;
			index += prepair;
		}
	}

  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      read record file
	@fileNo : contain file no
	@offset: start postion to read
	@length: lenght to read
	@cs:	communication setting 0: plain data, 1: MACed, 3:DES
	@data: 	buffer contain result
	@sizesz: return number of result data	
*/
/**************************************************************************/
pn532_error_t pn532DesfireReadRecord(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs,uint8_t * data, int32_t * sizesz)
{
  // ToDo
	uint8_t cmd[16];
	uint8_t rec[128];
	uint32_t size;
	uint32_t index = 0;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = 1;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_READRECORD;  	//Mifare read command
	cmd[3] = fileNo;
	cmd[4] = offset & 0xff;
	cmd[5] = (offset>>8) & 0xff;
	cmd[6] = (offset>>16) & 0xff;
	cmd[7] = length & 0xff;
	cmd[8] = (length>>8) & 0xff;
	cmd[9] = (length>>16) & 0xff;
	
	err = pn532SendCommand(cmd,10,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
		return PN532_ERROR_APPLEVELERROR;
    *sizesz = 0;
	index = 0;
	while(rec[8] == 0xaf)
	{
		//read first frame
		//11 = 1(preamble) + 2(start)+ 1(len) + 1(LCS)+ 1(TFI)+1(pn532)+1(pnstatus)+1(desfirestatus)+1(DCS)+1(post)
		memcpy(data+index,&rec[9],size-11);
		*sizesz = *sizesz + (size-11);
		index += (size-11);
		cmd[2] = 0xaf;
		err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
		delay_ms(1);
		if (err != PN532_ERROR_NONE)
			return err;
		err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
		if (err != PN532_ERROR_NONE)
			return err;
		//check respone command 
		if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
		   return PN532_ERROR_APPLEVELERROR;
		//check response status
		pcb.appError = rec[7];								 
		if ((rec[7] & 0x3f) != 0)
			return PN532_ERROR_APPLEVELERROR;
		//status retrn by card
		card.piccError = rec[8];
		if ((rec[8]  != 0x0) && (rec[8]  != 0xaf))
			return PN532_ERROR_APPLEVELERROR;
	}
	//read remain
	memcpy(data+index,&rec[9],size-11);
	*sizesz = *sizesz + (size-11);
	DesfireCryptoPostProcessData(data,sizesz,cs);
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Clear record File
	@fileNo: 	file number to be deleted
	
*/
/**************************************************************************/
pn532_error_t pn532DesfireClearRecord(uint8_t fileNo)
{
  // ToDo
	uint8_t cmd[4];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_CLEARRECORD;  	//Mifare read command
	cmd[3] = fileNo;
	
	err = pn532SendCommand(cmd,4,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_PICCLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Commit Transsaction

*/
/**************************************************************************/
pn532_error_t pn532DesfireCommitTransaction(void)
{
  // ToDo
	uint8_t cmd[3];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_COMMITTRANSACTION;  	//Mifare read command
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	card.piccError = rec[8];
	if (rec[8]  != 0x0)
		return PN532_ERROR_APPLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/**************************************************************************/
/*! 
    @brief      Commit Transsaction

*/
/**************************************************************************/
pn532_error_t pn532DesfireAbbortTransaction(void)
{
  // ToDo
	uint8_t cmd[3];
	uint8_t rec[32];
	uint32_t size;
	pn532_error_t err;
  // ToDo
    
  //send InDataExchange command
	cmd[0] = PN532_COMMAND_INDATAEXCHANGE;
	cmd[1] = card.Tg;  				//Card number returned by previous card select command 
	cmd[2] = PN532_DESFIRE_ABORTTRANSACTION;  	//Mifare read command
	
	err = pn532SendCommand(cmd,3,PN532_DEFAULT_TIMEOUT);
	delay_ms(1);
	if (err != PN532_ERROR_NONE)
		return err;
	err = pn532ReadResponse(rec,&size,PN532_DEFAULT_TIMEOUT);
	if (err != PN532_ERROR_NONE)
		return err;
	//UARTSendData(rec,size,2000);
	//check respone command 
	if (rec[6] != (PN532_COMMAND_INDATAEXCHANGE+1))
	   return PN532_ERROR_APPLEVELERROR;
	//check response status
	pcb.appError = rec[7];								 
	if ((rec[7] & 0x3f) != 0)
		return PN532_ERROR_APPLEVELERROR;
	//status retrn by card
	if (rec[8]  != 0x0)
		return PN532_ERROR_APPLEVELERROR;
    
  return PN532_ERROR_NONE;
}
/*
 * Size required to store nbytes of data in a buffer of size n*8.
 */
static uint32_t padded_data_length (uint32_t nbytes)
{
    if (nbytes % 8)
	return ((nbytes / 8) + 1) * 8;
    else
	return nbytes;
}

/*
 * Buffer size required to MAC nbytes of data
 */
static uint32_t maced_data_length (uint32_t nbytes)
{
    return nbytes + 4;
}
/*
 * Buffer size required to encipher nbytes of data and a two bytes CRC.
 */
static uint32_t enciphered_data_length (uint32_t nbytes)
{
    return padded_data_length (nbytes + 2);
}
static void bzero (uint8_t * data, int32_t size)
{
	int i;
	if(size>0)
	{
		for(i = 0;i<size;i++)
			data[i] = 0;
	}
}
void DesfireCryptoPreProcessData (const uint8_t *data, uint8_t *res, int *nbytes, int communication_settings)
{
    
    uint8_t mac[4];
    int edl, mdl;
	uint16_t crc;
    switch (communication_settings) {
	case 0:
	case 2:
	    memcpy(res,data,*nbytes);
	    break;
	case 1:
	    edl = padded_data_length (*nbytes);
	    

	    // Fill in the crypto buffer with data ...
	    memcpy (res, data, *nbytes);
	    // ... and 0 padding
	    bzero (res + *nbytes, edl - *nbytes);

	    mifare_cbc_block_des (card.sessionKey, res, edl, 1);

	    memcpy (mac, res + edl - 8, 4);

	    mdl = maced_data_length (*nbytes);
	    
	    memcpy (res, data, *nbytes);
	    memcpy (res + *nbytes, mac, 4);

	    *nbytes += 4;

	    break;
	case 3:
		
	    edl = enciphered_data_length (*nbytes);
	    
	    // Fill in the crypto buffer with data ...
	    memcpy (res, data, *nbytes);
	    // ... CRC ...
	    crc = CRC16_Generate(res, *nbytes);
	    // ... and 0 padding
		res[*nbytes] = crc & 0xff;
		res[*nbytes+1] = (crc>>8) & 0xff;
	    bzero ((uint8_t *)(res) + *nbytes + 2, edl - *nbytes - 2);

	    *nbytes = edl;

	    mifare_cbc_block_des (card.sessionKey, res, *nbytes, 1);

	    break;
	default:
	    //res = NULL;
	    break;
    }

    
}
void DesfireCryptoPostProcessData(uint8_t * data, int32_t * nbytes, uint8_t communication_settings)
{
	//check communication setting
	uint8_t *res = data;
    int32_t edl;
    uint8_t edata[128];
	uint8_t verified = 0;
	int end_crc_pos; // The CRC can be over two blocks

    switch (communication_settings) {
	case 0:
	case 2:
	    break;
	case 1:
	    *nbytes -= 4;

	    edl = enciphered_data_length (*nbytes);
	    //edata = malloc (edl);

	    memcpy (edata, data, *nbytes);
	    bzero (edata + *nbytes, edl - *nbytes);

	    mifare_cbc_block_des (card.sessionKey, edata, edl, 1);
	    /*                                                            ,^^^^^^^
	     * No!  This is not a typo! ---------------------------------'
	     */

	    if (0 != memcmp (data + *nbytes, edata + edl - 8, 4)) {
		//printf ("MACing not verified\n");
		*nbytes = -1;
		res = NULL;
	    }

	    //free (edata);

	    break;
	case 3:
	    mifare_cbc_block_des (card.sessionKey, res, *nbytes, 0);

	    /*
	     * Look for the CRC and ensure it is following by NULL padding.  We
	     * can't start by the end because the CRC is supposed to be 0 when
	     * verified, and accumulating 0's in it should not change it.
	     */
	    
		verified = 0;
	    end_crc_pos = *nbytes - 7; // The CRC can be over two blocks
	    do {
			int n;
			uint16_t crc;
			crc = CRC16_Generate(res, end_crc_pos);
			if (!crc) {
			    verified = 1;
			    for (n = end_crc_pos; n < *nbytes; n++) {
				uint8_t byte = ((uint8_t *)res)[n];
				if (!( (0x00 == byte) || ((0x80 == byte) && (n == end_crc_pos)) ))
				    verified = 0;
			    }
			}
			if (verified==1) {
			    *nbytes = end_crc_pos - 2;
			} else {
			    end_crc_pos++;
			}
	    } while ((verified==0) && (end_crc_pos < *nbytes));

	    if (verified==0) {
		//printf ("(3)DES not verified\n");
		*nbytes = -1;
		res = NULL;
	    }

	    break;
	default:
	    //printf ("Unknown communication settings\n");
	    *nbytes = -1;
	    res = NULL;
	    break;

    }
}
