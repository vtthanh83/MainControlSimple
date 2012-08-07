/**************************************************************************/
/*!
    @file     pn532.h
*/
/**************************************************************************/

#ifndef __PN532_H__
#define __PN532_H__
#include "stm32sunid.h"

#define PN532_DEBUG(fmt, args...)             
#define MAX_UID_LEN	7
#define MIFARE_KEY_LEN	6
typedef enum pn532_state_e
{
  PN532_STATE_SLEEP   = 0x00,
  PN532_STATE_READY   = 0x01,
  PN532_STATE_BUSY    = 0x02
}
pn532_state_t;

/* Error messages generate by the stack (not to be confused with app level errors from the PN532) */
typedef enum pn532_error_e
{
  PN532_ERROR_NONE                    = 0x00,
  PN532_ERROR_UNABLETOINIT            = 0x01,   // Unable to initialise or wakeup the device
  PN532_ERROR_APPLEVELERROR           = 0x02,   // Application level error detected
  PN532_ERROR_BUSY                    = 0x03,   // Busy executing a previous command
  PN532_ERROR_NOACK                   = 0x04,   // No ack message received
  PN532_ERROR_INVALIDACK              = 0x05,   // Ack != 00 00 FF 00 FF 00
  PN532_ERROR_PREAMBLEMISMATCH        = 0x06,   // Frame preamble + start code mismatch
  PN532_ERROR_EXTENDEDFRAME           = 0x07,   // Extended frames currently unsupported
  PN532_ERROR_LENCHECKSUMMISMATCH     = 0x08,
  PN532_ERROR_RESPONSEBUFFEREMPTY     = 0x09,    // No response data received
  PN532_ERROR_NOCARD                  = 0x0A,	 //No card appear
  PN532_ERROR_LOGINFAIL				  = 0x0B,	 // authenticate fail
  PN532_ERROR_VERIFY				  = 0x0C,	 // verify read after write is fail
  PN532_ERROR_PICCLEVELERROR		  = 0x0D	 // verify read after write is fai
} pn532_error_t;

typedef enum pn532_modulation_e
{
  PN532_MODULATION_ISO14443A_106KBPS  = 0x00,
  PN532_MODULATION_FELICA_212KBPS     = 0x01,
  PN532_MODULATION_FELICA_424KBPS     = 0x02,
  PN532_MODULATION_ISO14443B_106KBPS  = 0x03,
  PN532_MODULATION_JEWEL_106KBPS      = 0x04
} pn532_modulation_t;

enum
{
  MIFARE_STANDARD_1K 	= 0x08,
  MIFARE_STANDARD_4K	= 0X18,
  MIFARE_ULTRALIGHT		= 0X00,
  MIFARE_DESFIRE		= 0x20
};

/* PN532 Protocol control block */
typedef struct
{
  bool                initialised;
  pn532_state_t       state;
  pn532_modulation_t  modulation;
  uint32_t            lastCommand;
  uint32_t            appError;
} pn532_pcb_t;
/* PN532 Card info */
 //define for AUTHENTICATED
#define		PICC_NOT_AUTHENTICATED	0
#define		PICC_AUTHENTICATED		1
typedef struct
{
  uint8_t           Tg;
  uint8_t           type;
  uint8_t           NFCIDLength;
  uint8_t           NFCID[MAX_UID_LEN];
  uint8_t           authenticated;
	uint8_t           keyNo;
  uint8_t			sessionKey[16];
  uint8_t			piccError;
} pn532_cardinfo_t;
void          pn532Init(void);
pn532_pcb_t * pn532GetPCB(void);
pn532_error_t pn532SetModulation(pn532_modulation_t mod);

void          pn532PrintHex(const uint8_t * pbtData, const uint32_t szBytes);
//Comand for mifare card
enum{
  PN532_MIFARE_AUTHENTICATION_A			= 0x60,
  PN532_MIFARE_AUTHENTICATION_B			= 0x61,
  PN532_MIFARE_READ16					= 0x30,
  PN532_MIFARE_WRITE16					= 0xA0,
  PN532_MIFARE_WRITE4					= 0xA2,
  PN532_MIFARE_INCREMENT				= 0xC1,
  PN532_MIFARE_DECREMENT				= 0xC0,
  PN532_MIFARE_TRANSFER					= 0xB0,
  PN532_MIFARE_RESTORE					= 0xC2
};

pn532_error_t pn532GetRegister (uint16_t ui16Reg, uint8_t * ui8Value);
pn532_error_t pn532SetRegister (uint16_t ui16Reg, uint8_t ui8SymbolMask, uint8_t ui8Value);
pn532_error_t pn532SAMConfigure(void);
pn532_error_t pn532GetFirmware(void);
pn532_error_t pn532CardList(pn532_modulation_t mod);
pn532_error_t pn532CardSelect(void);
pn532_error_t pn532CardRelease(void);
pn532_error_t pn532MifareLogin(uint8_t blockAddr, uint8_t keyType, uint8_t * key);
pn532_error_t pn532MifareRead16(uint8_t blockAddr, uint8_t * buf);
pn532_error_t pn532MifareWrite16(uint8_t blockAddr, uint8_t verify, uint8_t * buf);
pn532_error_t pn532MifareWriteKeyA(uint8_t sectorAddr, uint8_t * newKey);

enum
{
	DESFIRE_ERROR_NONE				=	0x00,
	DESFIRE_ERROR_NOCHANGES			=	0x0C,
	DESFIRE_ERROR_OUTOFEEPROM		=	0x0E,	//insuffincient memory
	DESFIRE_ERROR_ILLEGALCOMMAND	=	0x1C,	//command code not supported
	DESFIRE_ERROR_INTEGRITY			=	0x1E, 	//CRC or MAC does not match data
	DESFIRE_ERROR_NOSUCHKEY			=	0x40,	//Invalid key number specified
	DESFIRE_ERROR_LENGTH			=	0x7E,	//Length of command string invalid
	DESFIRE_ERROR_PERMISSION		=	0x9D,	//Permission denied
	DESFIRE_ERROR_PARAMETER			=	0x9E,	//PINVALID PARAMETER
	DESFIRE_ERROR_APPLICATION		=	0xA0,	//Application not found
	DESFIRE_ERROR_APPINTEGRITY		=	0xA1,	//Error with app
	DESFIRE_ERROR_AUTHENTICATE		=	0xAE,	//Current Authenticated not support command
	DESFIRE_ERROR_ADDITIONALFRAME	=	0xAF,	//Addition Data frame is expected to be sent
	DESFIRE_ERROR_BOUNDARY			=	0xBE,	
	DESFIRE_ERROR_PICCINTEGRITY		=	0xC1,
	DESFIRE_ERROR_COMMANDABORTED	=	0xCA,
	DESFIRE_ERROR_PICCDISABLED		=	0xCD,
	DESFIRE_ERROR_COUNT				=	0xCE,				
	DESFIRE_ERROR_DUPLICATE			=	0xDE,	
	DESFIRE_ERROR_EEPROM			=	0xEE,	
	DESFIRE_ERROR_FILENOTFOUND		=	0xF0,	
	DESFIRE_ERROR_FILEINTEGRITY		=	0xF1
};
//Desfire Key setting parameter
#define	KS_ALLOW_MASTER	0x01	//Application master key (key 0) changeable
#define	KS_FREE_LIST	0x02	//Free Directory list without master key
#define KS_FREE_CREATE	0x04	//Free create/delete file(app) withou authenticated
#define KS_CONF_CHANGE	0x08	//Configuration changeable
#define	KS_CHANGE_ANY	0x00	//master key authenticate required to change any key
#define	KS_CHANGE_1		0x10	//key 1 authenticate required to change any key
#define	KS_CHANGE_2		0x20	//key 2 authenticate required to change any key
#define	KS_CHANGE_3		0x30	//key 3 authenticate required to change any key
#define	KS_CHANGE_4		0x40	//key 4 authenticate required to change any key
#define	KS_CHANGE_5		0x50	//key 5 authenticate required to change any key
#define	KS_CHANGE_6		0x60	//key 6 authenticate required to change any key
#define	KS_CHANGE_7		0x70	//key 7 authenticate required to change any key
#define	KS_CHANGE_8		0x80	//key 8 authenticate required to change any key
#define	KS_CHANGE_9		0x90	//key 9 authenticate required to change any key
#define	KS_CHANGE_A		0xA0	//key 10 authenticate required to change any key
#define	KS_CHANGE_B		0xB0	//key 11 authenticate required to change any key
#define	KS_CHANGE_C		0xC0	//key 12 authenticate required to change any key
#define	KS_CHANGE_D		0xD0	//key 13 authenticate required to change any key
#define	KS_CHANGE_SAME	0xE0	//the same key authenticate required to change that key
#define	KS_CHANGE_NONE	0xF0	//All keys are frozen 

typedef struct
{
	unsigned char AID[3];
	unsigned char keySetting;
	unsigned char numKey;
}DESFIRE_CreateAID;
//definition for file structure
enum
{
	DESFIRE_STANDARD_FILE		= 0x00,
	DESFIRE_BACKUP_FILE			= 0x01,
	DESFIRE_VALUE_FILE			= 0x02,
	DESFIRE_LINEAR_FILE			= 0x03,
	DESFIRE_CYCLIC_FILE			= 0x04,
};
//File communication setting
#define	DESFIRE_FILE_PLAIN	0	//data are not de/encrypt
#define	DESFIRE_FILE_MACED	1	//data are de/encrypt using MACed
#define	DESFIRE_FILE_DES	3	//data are de/encrypt using (3)DES
//Assecc Right Mask
#define DESFIRE_FILE_CHANGE		0
#define DESFIRE_FILE_RDWR		4
#define DESFIRE_FILE_WRITE		8
#define DESFIRE_FILE_READ		12
typedef struct
{
	uint32_t	size;
}DESFIRE_STDFile;
typedef struct
{
	int32_t	lowerLimit;
	int32_t	upperLimit;
	int32_t	value;
	uint8_t	limitedCreditEn;
}DESFIRE_ValueFile;
typedef struct
{
	uint32_t	size;
	uint32_t	maxRecord;
	uint32_t	currentRecord;
}DESFIRE_RecordFile;

typedef struct
{
	uint8_t fileNo;	    //file number (0-0x0f)
	uint8_t fileType;
	uint8_t commSetting;//Communication Setting
	uint16_t accessRight;
	union 
	{
		DESFIRE_STDFile stdFile;
		DESFIRE_ValueFile valueFile;
		DESFIRE_RecordFile recordFile;
	}file;
} DESFIRE_FileSetting;
//Coomand for mifare desfire card
enum{
  PN532_DESFIRE_AUTHENTICATION			= 0x0A,
  PN532_DESFIRE_CHANGEKEYSETTING		= 0x54,
  PN532_DESFIRE_GETKEYSETTING			= 0x45,
  PN532_DESFIRE_GETKEYVERSION			= 0x64,
  PN532_DESFIRE_CHANGEKEY				= 0xC4,
  PN532_DESFIRE_GETVERSION				= 0x60,
  PN532_DESFIRE_CREATEAPP				= 0xCA,
  PN532_DESFIRE_DELETEAPP				= 0xDA,
  PN532_DESFIRE_SELECTAPP				= 0x5A,
  PN532_DESFIRE_FORMATPICC				= 0xFC,
  PN532_DESFIRE_GETAPPID				= 0x6A,
  PN532_DESFIRE_GETFILEID				= 0x6F,
  PN532_DESFIRE_GETFILESETTING			= 0xF5,
  PN532_DESFIRE_CHANGEFILESETTING		= 0x5F,
  PN532_DESFIRE_CREATESTDFILE			= 0xCD,
  PN532_DESFIRE_CREATEBACKUPDATAFILE	= 0xCB,
  PN532_DESFIRE_CREATEVALUEFILE			= 0xCC,
  PN532_DESFIRE_CREATELINEARRECORDFILE	= 0xC1,
  PN532_DESFIRE_CREATECYCLICRECORDFILE	= 0xC0,
  PN532_DESFIRE_DELETEFILE				= 0xDF,
  PN532_DESFIRE_READDATA				= 0xBD,
  PN532_DESFIRE_WRITEDATA				= 0x3D,
  PN532_DESFIRE_GETVALUE				= 0x6C,
  PN532_DESFIRE_CREDIT					= 0x0C,
  PN532_DESFIRE_DEBIT					= 0xDC,
  PN532_DESFIRE_LIMITTEDCREDIT			= 0x1C,
  PN532_DESFIRE_WRITERECORD				= 0x3B,
  PN532_DESFIRE_READRECORD				= 0xBB,
  PN532_DESFIRE_CLEARRECORD				= 0xEB,
  PN532_DESFIRE_COMMITTRANSACTION		= 0xC7,
  PN532_DESFIRE_ABORTTRANSACTION		= 0xA7
};
void mifare_cbc_des (uint8_t *key, uint8_t *data, uint8_t *ivect, short direction);
pn532_error_t pn532CardDeselect(void);
pn532_error_t pn532DesfileSelect(pn532_modulation_t mod);
pn532_error_t pn532DesfireAuthenticate(uint8_t * key, uint8_t keyno);
pn532_error_t pn532DesfireChangeKeySetting(uint8_t * keySetting);
pn532_error_t pn532DesfireGetKeySetting(uint8_t * keySetting, uint8_t * maxKey);
pn532_error_t pn532DesfireGetKeyVersion(uint8_t keyNo, uint8_t * keyVer);
pn532_error_t pn532DesfireChangeKey(uint8_t keyNo, uint8_t * oldKey, uint8_t * newKey);
pn532_error_t pn532DesfireGetAIDs(uint8_t * AIDs, uint32_t * sizesz);
pn532_error_t pn532DesfireGetVersion(uint8_t * verbuf);
pn532_error_t pn532CreateApplication(DESFIRE_CreateAID * app);
pn532_error_t pn532DeleteApplication(DESFIRE_CreateAID * app);
pn532_error_t pn532SelectApplication(DESFIRE_CreateAID * app);
pn532_error_t pn532FormatPICC(void);

pn532_error_t pn532DesfireGetFileIDs(uint8_t * FIDs, uint32_t * sizesz);
pn532_error_t pn532DesfireGetFileSettings(DESFIRE_FileSetting * fileSetting);
pn532_error_t pn532DesfireChangeFileSettings(uint8_t authenticated, DESFIRE_FileSetting * fileSetting);
pn532_error_t pn532DesfireCreateFile(DESFIRE_FileSetting * fileSetting);
pn532_error_t pn532DesfireDeleteFile(uint8_t fileNo);
pn532_error_t pn532DesfireReadData(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs, uint8_t * data, int32_t * sizesz);
pn532_error_t pn532DesfireWriteData(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs, const uint8_t * data, int32_t * sizesz);
pn532_error_t pn532DesfireGetValue(uint8_t fileNo, int32_t * value, uint8_t cs);
pn532_error_t pn532DesfireCredit(uint8_t fileNo, int32_t value, uint8_t cs);
pn532_error_t pn532DesfireDebit(uint8_t fileNo, int32_t value, uint8_t cs);
pn532_error_t pn532DesfireLimitedCredit(uint8_t fileNo,int32_t value, uint8_t cs);
pn532_error_t pn532DesfireWriteRecord(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs, uint8_t * data, int32_t * sizesz);
pn532_error_t pn532DesfireReadRecord(uint8_t fileNo, uint32_t offset, uint32_t length, uint8_t cs, uint8_t * data, int32_t * sizesz);
pn532_error_t pn532DesfireClearRecord(uint8_t fileNo);
pn532_error_t pn532DesfireCommitTransaction(void);
pn532_error_t pn532DesfireAbbortTransaction(void);
#endif

