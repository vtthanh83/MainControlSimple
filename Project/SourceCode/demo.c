#include "demo.h"
#include "string.h"
//default key of new card
static uint8_t defaultMKey[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//master key
static uint8_t newMKey[16] = {0xab,0x12,0xcd,0x34,0xef,0x56,0x78,0x44,0xab,0x12,0xcd,0x34,0xef,0x56,0x78,0x44};
//master app key
static uint8_t newAKey[16] = {0x23,0x27,0x56,0x37,0xa3,0x89,0x3b,0xf2,0x23,0x27,0x56,0x37,0xa3,0x89,0x3b,0xf2};
//file access key
static uint8_t newF1Key[16] = {0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82,0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82};
static uint8_t newF2Key[16] = {0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82,0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82};
static uint8_t newF3Key[16] = {0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82,0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82};
static uint8_t newF4Key[16] = {0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82,0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82};
static uint8_t newF5Key[16] = {0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82,0x13,0x24,0xa6,0x36,0x23,0x59,0x6b,0x82};
/*
 * Start reader to active RF and operation mode	and other peripherals
 */
DemoErrorCode_t StartReader(void)
{
	pn532_error_t err;
	UARTInit();
	LEDInit(ALL_LED);
	SWInit(ALL_SWITCH, SWITCH_MODE_GPIO);	
	PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	BuzzerInit();
	pn532Init();
	BuzzerOn(125);
	delay_ms(100);
	//Get firmware version
	err = pn532GetFirmware();
	if(err != PN532_ERROR_NONE)
		return DER_READER;
	delay_ms(10);
	
	err = pn532SAMConfigure();
	if(err != PN532_ERROR_NONE)
		return DER_READER;
	delay_ms(10);
	return DER_NONE;
}
//Restore to default card whether it is new or old card to reuse
DemoErrorCode_t RestoreToDefaultCard(void)
{
		pn532_error_t err;
	DESFIRE_CreateAID app;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, key no = 0)
 	app.AID[0] = 0x00;
	app.AID[1] = 0x00;
	app.AID[2] = 0x00;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	err = pn532DesfireAuthenticate(defaultMKey, 0);
	//if this user defualt key
	if(err == PN532_ERROR_NONE)
 	{
		//Format card to release all user memory
	 	err = pn532FormatPICC();
		if(err != PN532_ERROR_NONE)
			return DER_CARD;
	}
	else{	
	//Try to Authen ticate with new master key
	//Change to default key and setting
		err = pn532DesfireAuthenticate(newMKey, 0);
		//if this user defualt key
		if(err != PN532_ERROR_NONE)
			return DER_INVPASS;
		err = pn532DesfireChangeKey(0,newMKey,defaultMKey);
		if(err != PN532_ERROR_NONE)
			return DER_INVPASS;
		//Authenticate to new Key
		err = pn532DesfireAuthenticate(defaultMKey, 0);
		if(err != PN532_ERROR_NONE)
	 		return DER_INVPASS;
		//change Key setting
		app.keySetting = 0x0f;
		err = pn532DesfireChangeKeySetting(&(app.keySetting));
		if(err != PN532_ERROR_NONE)
			return DER_CARD;
	}
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
/*
 * Formart very new card
 * Default master card Key: 00 00 00 00 00 00 00 00
 * this function format new card and change master key and key setting
 */
DemoErrorCode_t FormatNewCard(void)
{
	pn532_error_t err;
	DESFIRE_CreateAID app;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, key no = 0)
 	app.AID[0] = 0x00;
	app.AID[1] = 0x00;
	app.AID[2] = 0x00;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	err = pn532DesfireAuthenticate(defaultMKey, 0);
	if(err != PN532_ERROR_NONE)
 		return DER_INVPASS;
		
	//Change default key to new key
	//change key 1 from default value to new key
	err = pn532DesfireChangeKey(0,defaultMKey,newMKey);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
		
	//Authenticate to new Key
	err = pn532DesfireAuthenticate(newMKey, 0);
	if(err != PN532_ERROR_NONE)
 		return DER_INVPASS;
	//change Key setting
	app.keySetting = KS_ALLOW_MASTER|KS_CONF_CHANGE;
	err = pn532DesfireChangeKeySetting(&(app.keySetting));
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//Format card to release all user memory
 	err = pn532FormatPICC();
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
		
		
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
/*
 * Formart an used card
 */
DemoErrorCode_t FormatOldCard(void)
{
	pn532_error_t err;
	DESFIRE_CreateAID app;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, newMKey)
 	app.AID[0] = 0x00;
	app.AID[1] = 0x00;
	app.AID[2] = 0x00;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	err = pn532DesfireAuthenticate(newMKey, 0);
	if(err != PN532_ERROR_NONE)
 		return DER_INVPASS;
	//Format card to release all user memory
 	err = pn532FormatPICC();
	if(err != PN532_ERROR_NONE)
		return DER_CARD;	
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
/*
 * Create new employee card (that has been format before)
 * detail: consist of employee data
 */
DemoErrorCode_t CreateCard(const EmployeeCard_st * detail)
{
	pn532_error_t err;
	int	filesize;
	uint8_t rdata[48];
	DESFIRE_CreateAID app;
	DESFIRE_FileSetting fileSetting;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, newMKey)
 	app.AID[0] = 0x00;
	app.AID[1] = 0x00;
	app.AID[2] = 0x00;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	err = pn532DesfireAuthenticate(newMKey, 0);
	if(err != PN532_ERROR_NONE)
 		return DER_INVPASS;
	//create new app
	app.AID[0] = APP_ID & 0xff;
	app.AID[1] = (APP_ID>>8) & 0xff;
	app.AID[2] = (APP_ID>>16) & 0xff;
	app.keySetting = 0x9;	//require authenticate app key to create file, delete file, get setting...
	app.numKey = 6;			//MasterKey and two user keys			
	err = pn532CreateApplication(&app);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//after creating successfully, switch to app 	
	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//Authenticated with default app master key
	err = pn532DesfireAuthenticate(defaultMKey, 0);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//change app master key from default value to new key
	err = pn532DesfireChangeKey(0,defaultMKey,newAKey);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//re authenticate with new key
	err = pn532DesfireAuthenticate(newAKey, 0);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//change file access key from default value to new key
	err = pn532DesfireChangeKey(1,defaultMKey,newF1Key);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	err = pn532DesfireChangeKey(2,defaultMKey,newF2Key);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	err = pn532DesfireChangeKey(3,defaultMKey,newF3Key);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	err = pn532DesfireChangeKey(4,defaultMKey,newF4Key);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	err = pn532DesfireChangeKey(5,defaultMKey,newF5Key);
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
  //Create standard file to hold card type 
	fileSetting.fileNo = FTYPE_ID; //file number 0 (file number range form 0  to 0x0f)
	fileSetting.fileType = DESFIRE_STANDARD_FILE;	//creat standard file
	fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
	//using key 1 to change access right, read, write right
	fileSetting.accessRight = (1<<DESFIRE_FILE_READ)|(1<<DESFIRE_FILE_WRITE)
							|(1<<DESFIRE_FILE_RDWR)|(1<<DESFIRE_FILE_CHANGE);
	fileSetting.file.stdFile.size = FTYPE_SZ+FEMPID_SZ+FDATE_SZ+FWORKING_SZ; //length of file is 1
	err = pn532DesfireCreateFile(&fileSetting);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
//	//Create standard file to hold employ ID 
//	fileSetting.fileNo = FEMPID_ID; //file number 1 (file number range form 0  to 0x0f)
//	fileSetting.fileType = DESFIRE_STANDARD_FILE;	//creat standard file
//	fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
//	//using key 1 to change access right, read, write right
//	fileSetting.accessRight = (1<<DESFIRE_FILE_READ)|(1<<DESFIRE_FILE_WRITE)
//							|(1<<DESFIRE_FILE_RDWR)|(1<<DESFIRE_FILE_CHANGE);
//	fileSetting.file.stdFile.size = FEMPID_SZ; //length of file is 10
//	err = pn532DesfireCreateFile(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//Create standard file to hold date issue and expire 
//	fileSetting.fileNo = FDATE_ID; //file number 1 (file number range form 0  to 0x0f)
//	fileSetting.fileType = DESFIRE_STANDARD_FILE;	//creat standard file
//	fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
//	//using key 1 to change access right, read, write right
//	fileSetting.accessRight = (1<<DESFIRE_FILE_READ)|(1<<DESFIRE_FILE_WRITE)
//							|(1<<DESFIRE_FILE_RDWR)|(1<<DESFIRE_FILE_CHANGE);
//	fileSetting.file.stdFile.size = FDATE_SZ; //length of file is 8
//	err = pn532DesfireCreateFile(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//Create standard file to hold Working day and hour 
//	fileSetting.fileNo = FWORKING_ID; //file number 1 (file number range form 0  to 0x0f)
//	fileSetting.fileType = DESFIRE_STANDARD_FILE;	//creat standard file
//	fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
//	//using key 1 to change access right, read, write right
//	fileSetting.accessRight = (1<<DESFIRE_FILE_READ)|(4<<DESFIRE_FILE_WRITE)
//							|(1<<DESFIRE_FILE_RDWR)|(1<<DESFIRE_FILE_CHANGE);
//	fileSetting.file.stdFile.size = FWORKING_SZ; //length of file is 5
//	err = pn532DesfireCreateFile(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
	//Create standard file to hold user data 
	fileSetting.fileNo = FUSER_ID; //file number 1 (file number range form 0  to 0x0f)
	fileSetting.fileType = DESFIRE_STANDARD_FILE;	//creat standard file
	fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
	//using key 1 to change access right, read, write right
	fileSetting.accessRight = (5<<DESFIRE_FILE_READ)|(5<<DESFIRE_FILE_WRITE)
							|(5<<DESFIRE_FILE_RDWR)|(5<<DESFIRE_FILE_CHANGE);
	fileSetting.file.stdFile.size = FUSER_SZ; //length of file is 32
	err = pn532DesfireCreateFile(&fileSetting);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//Create a vlue file to hold money
	fileSetting.fileNo = FVALUE_ID; //file number 0 (file number range form 0  to 0x0f)
	fileSetting.fileType = DESFIRE_VALUE_FILE;	//creat value file
	fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
	//using key 5 to change access right, read, 2 to write right
	fileSetting.accessRight = (5<<DESFIRE_FILE_READ)|(5<<DESFIRE_FILE_WRITE)
							|(5<<DESFIRE_FILE_RDWR)|(5<<DESFIRE_FILE_CHANGE);
	fileSetting.file.valueFile.lowerLimit = 0; //min value
	fileSetting.file.valueFile.upperLimit = 5000000; //max value
	fileSetting.file.valueFile.value = detail->value; //init value
	fileSetting.file.valueFile.limitedCreditEn = 0; 
	err = pn532DesfireCreateFile(&fileSetting);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//write data to card
	//Authenticate to key 1 to write card type
	err = pn532DesfireAuthenticate(newF1Key, 1);	 
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//write data to this file
	err = pn532DesfireWriteData(FTYPE_ID, 0, FTYPE_SZ+FEMPID_SZ+FDATE_SZ+FWORKING_SZ, fileSetting.commSetting, (const uint8_t *)(detail), &filesize);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//read back to verify
//	err = pn532DesfireReadData(FTYPE_ID, 0, FTYPE_SZ+FEMPID_SZ+FDATE_SZ+FWORKING_SZ, fileSetting.commSetting, rdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	if (rdata[0]!=(detail->type))
//		return DER_VERIFY;
	//Authenticate to key 2 to write employeeID
	//err = pn532DesfireAuthenticate(newF2Key, 2);	 
	//if(err != PN532_ERROR_NONE)
	//	return DER_INVPASS;
	//write data to this file
//	err = pn532DesfireWriteData(FEMPID_ID, 0, FEMPID_SZ, fileSetting.commSetting, (const uint8_t *)(detail->empID), &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//read back to verify
//	err = pn532DesfireReadData(FEMPID_ID, 0, FEMPID_SZ, fileSetting.commSetting, rdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	if (arraycmp(detail->empID,rdata+1,0,FEMPID_SZ))
//		return DER_VERIFY;
	//Authenticate to key 3 to write Date
	//err = pn532DesfireAuthenticate(newF3Key, 3);	 
	//if(err != PN532_ERROR_NONE)
	//	return DER_INVPASS;
	//write data to this file
	//err = pn532DesfireWriteData(FDATE_ID, 0, FDATE_SZ, fileSetting.commSetting, (const uint8_t *)(detail->date), &filesize);
	//if(err != PN532_ERROR_NONE)
	//	return DER_CARD;
	//read back to verify
	//err = pn532DesfireReadData(FDATE_ID, 0, FDATE_SZ, fileSetting.commSetting, rdata, &filesize);
	//if(err != PN532_ERROR_NONE)
	//	return DER_CARD;
//	if (arraycmp(detail->date,rdata+11,0,FDATE_SZ))
//		return DER_VERIFY; 
	//Authenticate to key 4 to write workingday and hour
	//err = pn532DesfireAuthenticate(newF4Key, 4);	 
	//if(err != PN532_ERROR_NONE)
	//	return DER_INVPASS;
	//write data to this file
	//err = pn532DesfireWriteData(FWORKING_ID, 0, FWORKING_SZ, fileSetting.commSetting, (const uint8_t *)(detail->working), &filesize);
	//if(err != PN532_ERROR_NONE)
	//	return DER_CARD;
	//read back to verify
	//err = pn532DesfireReadData(FWORKING_ID, 0, FWORKING_SZ, fileSetting.commSetting, rdata, &filesize);
	//if(err != PN532_ERROR_NONE)
	//	return DER_CARD;
//	if (arraycmp(detail->working,rdata+19,0,FWORKING_SZ))
//		return DER_VERIFY;
	//Authenticate to key 5 to write userdata
	err = pn532DesfireAuthenticate(newF5Key, 5);	 
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//write data to this file
	//err = pn532DesfireWriteData(FUSER_ID, 0, FUSER_SZ, fileSetting.commSetting, (const uint8_t *)(detail->user), &filesize);
	//if(err != PN532_ERROR_NONE)
	//	return DER_CARD;
	//read back to verify
	//err = pn532DesfireReadData(FUSER_ID, 0, FUSER_SZ, fileSetting.commSetting, rdata, &filesize);
	//if(err != PN532_ERROR_NONE)
	//	return DER_CARD;
//	if (arraycmp(detail->user,rdata,0,FUSER_SZ))
//		return DER_VERIFY;
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;

}
///*
// * Change type of a present card
// */
//
//DemoErrorCode_t WriteType(uint8_t type)
//{
//	pn532_error_t err;
//	uint8_t rdata[32];
//	int	filesize;
//	DESFIRE_CreateAID app;
//	DESFIRE_FileSetting fileSetting;
//	//select mifare card
//  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
//  if(err != PN532_ERROR_NONE)
//	 	return DER_NOCARD;
//	//Authenticate to PICC Master key (AID = 0x00, newMKey)
// 	app.AID[0] = APP_ID & 0xff;
//	app.AID[1] = (APP_ID>>8) & 0xff;
//	app.AID[2] = (APP_ID>>16) & 0xff;
// 	err = pn532SelectApplication(&app);
//	if(err != PN532_ERROR_NONE)
// 		return DER_CARD;
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FTYPE_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//Authenticate to key 1 to write card type
//	err = pn532DesfireAuthenticate(newF1Key, 1);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	//write data to this file
//	err = pn532DesfireWriteData(FTYPE_ID, 0, FTYPE_SZ, fileSetting.commSetting, (const uint8_t *)(&type), &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//read back to verify
//	err = pn532DesfireReadData(FTYPE_ID, 0, FTYPE_SZ, fileSetting.commSetting, rdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	if (rdata[0]!=type)
//		return DER_VERIFY;
//	//successful
//	pn532CardRelease();
//	delay_ms(10);
//	return DER_NONE;
//}
///*
// * Change employeeID of a present card
// */
//DemoErrorCode_t WriteEmpID(const uint8_t * empID)
//{
//	pn532_error_t err;
//	uint8_t rdata[32];
//	int	filesize;
//	DESFIRE_CreateAID app;
//	DESFIRE_FileSetting fileSetting;
//	//select mifare card
//  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
//  if(err != PN532_ERROR_NONE)
//	 	return DER_NOCARD;
//	//Authenticate to PICC Master key (AID = 0x00, newMKey)
// 	app.AID[0] = APP_ID & 0xff;
//	app.AID[1] = (APP_ID>>8) & 0xff;
//	app.AID[2] = (APP_ID>>16) & 0xff;
// 	err = pn532SelectApplication(&app);
//	if(err != PN532_ERROR_NONE)
// 		return DER_CARD;
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FEMPID_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//Authenticate to key 1 to write card type
//	err = pn532DesfireAuthenticate(newF2Key, 1);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	//write data to this file
//	err = pn532DesfireWriteData(FEMPID_ID, 0, FEMPID_SZ, fileSetting.commSetting, (const uint8_t *)empID, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//read back to verify
//	err = pn532DesfireReadData(FEMPID_ID, 0, FEMPID_SZ, fileSetting.commSetting, rdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	if (arraycmp(empID,rdata,0,FEMPID_SZ))
//		return DER_VERIFY;
//	//successful
//	pn532CardRelease();
//	delay_ms(10);
//	return DER_NONE;
//}
//DemoErrorCode_t WriteDate(const uint8_t * date)
//{
//	pn532_error_t err;
//	uint8_t rdata[32];
//	int	filesize;
//	DESFIRE_CreateAID app;
//	DESFIRE_FileSetting fileSetting;
//	//select mifare card
//  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
//  if(err != PN532_ERROR_NONE)
//	 	return DER_NOCARD;
//	//Authenticate to PICC Master key (AID = 0x00, newMKey)
// 	app.AID[0] = APP_ID & 0xff;
//	app.AID[1] = (APP_ID>>8) & 0xff;
//	app.AID[2] = (APP_ID>>16) & 0xff;
// 	err = pn532SelectApplication(&app);
//	if(err != PN532_ERROR_NONE)
// 		return DER_CARD;
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FDATE_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//Authenticate to key 3 to write date
//	err = pn532DesfireAuthenticate(newF3Key, 1);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	//write data to this file
//	err = pn532DesfireWriteData(FDATE_ID, 0, FDATE_SZ, fileSetting.commSetting, (const uint8_t *)date, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//read back to verify
//	err = pn532DesfireReadData(FDATE_ID, 0, FDATE_SZ, fileSetting.commSetting, rdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	if (arraycmp(date,rdata,0,FDATE_SZ))
//		return DER_VERIFY;
//	//successful
//	pn532CardRelease();
//	delay_ms(10);
//	return DER_NONE;
//}
//DemoErrorCode_t WriteWorking(uint8_t day, uint16_t start, uint16_t end)
//{
//	pn532_error_t err;
//	uint8_t rdata[32];
//	uint8_t wdata[16];
//	int	filesize;
//	DESFIRE_CreateAID app;
//	DESFIRE_FileSetting fileSetting;
//	//select mifare card
//  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
//  if(err != PN532_ERROR_NONE)
//	 	return DER_NOCARD;
//	//Authenticate to PICC Master key (AID = 0x00, newMKey)
// 	app.AID[0] = APP_ID & 0xff;
//	app.AID[1] = (APP_ID>>8) & 0xff;
//	app.AID[2] = (APP_ID>>16) & 0xff;
// 	err = pn532SelectApplication(&app);
//	if(err != PN532_ERROR_NONE)
// 		return DER_CARD;
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FWORKING_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//Authenticate to key 4 to write date
//	err = pn532DesfireAuthenticate(newF4Key, 1);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	//write data to this file
//	wdata[0] = day;
//	wdata[1] = (start >> 8)&0xff;
//	wdata[2] = (start)&0xff;
//	wdata[3] = (end >> 8)&0xff;
//	wdata[4] = (start)&0xff;
//	err = pn532DesfireWriteData(FWORKING_ID, 0, FWORKING_SZ, fileSetting.commSetting, (const uint8_t *)wdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	//read back to verify
//	err = pn532DesfireReadData(FWORKING_ID, 0, FWORKING_SZ, fileSetting.commSetting, rdata, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
//	if (arraycmp(wdata,rdata,0,FWORKING_SZ))
//		return DER_VERIFY;
//	//successful
//	pn532CardRelease();
//	delay_ms(10);
//	return DER_NONE;
//}
DemoErrorCode_t WriteUser(const uint8_t * userData)
{
	pn532_error_t err;
	uint8_t rdata[32];
	int	filesize;
	DESFIRE_CreateAID app;
	DESFIRE_FileSetting fileSetting;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, newMKey)
 	app.AID[0] = APP_ID & 0xff;
	app.AID[1] = (APP_ID>>8) & 0xff;
	app.AID[2] = (APP_ID>>16) & 0xff;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	err = pn532DesfireAuthenticate(newAKey, 0);
	if(err != PN532_ERROR_NONE)
 		return DER_INVPASS;
	//Get file Setting
	fileSetting.fileNo = FUSER_ID;
	err = pn532DesfireGetFileSettings(&fileSetting);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//Authenticate to key 5 to write date
	err = pn532DesfireAuthenticate(newF5Key,5);	 
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//write data to this file
	err = pn532DesfireWriteData(FUSER_ID, 0, FUSER_SZ, fileSetting.commSetting, (const uint8_t *)userData, &filesize);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//read back to verify
	err = pn532DesfireReadData(FUSER_ID, 0, FUSER_SZ, fileSetting.commSetting, rdata, &filesize);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	if (arraycmp(userData,rdata,0,FUSER_SZ))
		return DER_VERIFY;
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
DemoErrorCode_t CreditValue(const int32_t data)
{
	pn532_error_t err;
	
	DESFIRE_CreateAID app;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, newMKey)
 	app.AID[0] = APP_ID & 0xff;
	app.AID[1] = (APP_ID>>8) & 0xff;
	app.AID[2] = (APP_ID>>16) & 0xff;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	
	//Authenticate to key 5 to write date
	err = pn532DesfireAuthenticate(newF5Key,5);	 
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//debit value
	err = pn532DesfireCredit(FVALUE_ID, data, DESFIRE_FILE_DES);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;;
	//Commit transaction
	err = pn532DesfireCommitTransaction();
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
DemoErrorCode_t DebitValue(const int32_t data)
{
	pn532_error_t err;
	
	DESFIRE_CreateAID app;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, newMKey)
 	app.AID[0] = APP_ID & 0xff;
	app.AID[1] = (APP_ID>>8) & 0xff;
	app.AID[2] = (APP_ID>>16) & 0xff;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
 		return DER_CARD;
	
	//Authenticate to key 5 to write date
	err = pn532DesfireAuthenticate(newF5Key,5);	 
	if(err != PN532_ERROR_NONE)
		return DER_INVPASS;
	//debit value
	err = pn532DesfireDebit(FVALUE_ID, data, DESFIRE_FILE_DES);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	//Commit transaction
	err = pn532DesfireCommitTransaction();
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
	
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
DemoErrorCode_t ReadCard(EmployeeCard_st * detail)
{
	pn532_error_t err;
	int	filesize;
	DESFIRE_CreateAID app;
	DESFIRE_FileSetting fileSetting;
	fileSetting.commSetting = DESFIRE_FILE_DES;
	//select mifare card
  err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
  if(err != PN532_ERROR_NONE)
	 	return DER_NOCARD;
	//Authenticate to PICC Master key (AID = 0x00, newMKey)
 	app.AID[0] = APP_ID & 0xff;
	app.AID[1] = (APP_ID>>8) & 0xff;
	app.AID[2] = (APP_ID>>16) & 0xff;
 	err = pn532SelectApplication(&app);
	if(err != PN532_ERROR_NONE)
	{
 		pn532CardRelease();
		return DER_CARD;
	}
	/*
	err = pn532DesfireAuthenticate(newAKey, 0);
	if(err != PN532_ERROR_NONE)
 		return DER_INVPASS;
	//Get file Setting
	fileSetting.fileNo = FTYPE_ID;
	err = pn532DesfireGetFileSettings(&fileSetting);
	if(err != PN532_ERROR_NONE)
		return DER_CARD;
		*/
	//Authenticate to key 1 to read type
	err = pn532DesfireAuthenticate(newF1Key,1);	 
	if(err != PN532_ERROR_NONE)
	{
		pn532CardRelease();
		return DER_INVPASS;
	}
//	
//	//read type
	err = pn532DesfireReadData(FTYPE_ID, 0, FTYPE_SZ+FEMPID_SZ+FDATE_SZ+FWORKING_SZ, fileSetting.commSetting, (uint8_t*)detail, &filesize);
	if(err != PN532_ERROR_NONE)
	{
		pn532CardRelease();
		return DER_CARD;
	}
	//turn back to master app key
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FEMPID_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
	//Authenticate to key 2 to read type
//	err = pn532DesfireAuthenticate(newF2Key,2);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	
//	//read type
//	err = pn532DesfireReadData(FEMPID_ID, 0, FEMPID_SZ, fileSetting.commSetting, detail->empID, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;

	//turn back to master app key
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FDATE_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
	//Authenticate to key 3 to read type
//	err = pn532DesfireAuthenticate(newF3Key,3);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	
//	//read type
//	err = pn532DesfireReadData(FDATE_ID, 0, FDATE_SZ, fileSetting.commSetting, detail->date, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;

	//turn back to master app key
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FWORKING_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
	//Authenticate to key 4 to read type
//	err = pn532DesfireAuthenticate(newF4Key,4);	 
//	if(err != PN532_ERROR_NONE)
//		return DER_INVPASS;
//	
//	//read type
//	err = pn532DesfireReadData(FWORKING_ID, 0, FWORKING_SZ, fileSetting.commSetting, detail->working, &filesize);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;

	//turn back to master app key
//	err = pn532DesfireAuthenticate(newAKey, 0);
//	if(err != PN532_ERROR_NONE)
// 		return DER_INVPASS;
//	//Get file Setting
//	fileSetting.fileNo = FUSER_ID;
//	err = pn532DesfireGetFileSettings(&fileSetting);
//	if(err != PN532_ERROR_NONE)
//		return DER_CARD;
	//Authenticate to key 5 to read type
	err = pn532DesfireAuthenticate(newF5Key,5);	 
	if(err != PN532_ERROR_NONE)
	{
	pn532CardRelease();
		return DER_INVPASS;
		}
	
	//read type
	//err = pn532DesfireReadData(FUSER_ID, 0, FUSER_SZ, fileSetting.commSetting, (detail->user), &filesize);
	//if(err != PN532_ERROR_NONE)
	//{
	//	pn532CardRelease();
	//	return DER_CARD;
	//	}
	//read value
	err = pn532DesfireGetValue(FVALUE_ID, &(detail->value), fileSetting.commSetting);
	if(err != PN532_ERROR_NONE)
	{
		pn532CardRelease();
		return DER_CARD;
		}
	//successful
	pn532CardRelease();
	delay_ms(10);
	return DER_NONE;
}
void DemoTest(void)
{
	int i;
	int waitForMess = 0;
	int value = 0;
	int ledIndex = 0;
	uint8_t Id[FEMPID_SZ];
	uint8_t User[FUSER_SZ];
	uint8_t rxMessage[64];
	int messsz;
	EmployeeCard_st detail;
	StartReader();
	//for(i = 0; i< FUSER_SZ; i++)
	//	User[i] = 0x30;
 //	UARTSendData((uint8_t*)User,32,4000);
	for(i = 0; i< FEMPID_SZ; i++)
		Id[i] = 0x30+i;
	memset(User,0,FUSER_SZ);
	InitEmployee(&detail,ADMIN_CARD,Id,(26<<24)|(10<<16)|2011,(28<<24)|(10<<16)|2011,
							REGULAR,0x0800,0x1200,User,10000);
	while(1)
	{
		
		//LEDOff(ALL_LED);
		//BuzzerOn(125);
		if(SWGetState(SWITCH1))
		{
			//UARTSendData("SWITCH1",7,4000);

			while(PBGetState(Button_USER) == SET);
			if(FormatNewCard()==DER_NONE)
			{
				BuzzerOn(125);
				detail.type = 0x00;
				if(CreateCard((const EmployeeCard_st *)&detail)==DER_NONE)
					BuzzerOn(125);
			}
		}
		else if(SWGetState(SWITCH2))
		{
			//UARTSendData("SWITCH2",7,4000);
			
			while(PBGetState(Button_USER) == SET);
			if(FormatOldCard()==DER_NONE)
			{
				detail.type = 0x00;
				detail.empID[9] = 'a';
				detail.value = 20000;
				BuzzerOn(125);
				if(CreateCard((const EmployeeCard_st *)&detail)==DER_NONE)
					BuzzerOn(125);
			}
		}
		else if(SWGetState(SWITCH3))
		{
			//UARTSendData("SWITCH3",7,4000);
			
			while(PBGetState(Button_USER) == SET);
			if(FormatOldCard()==DER_NONE)
			{
				detail.type = detail.type = 0x01;
				detail.empID[9] = 'b';
				detail.value = 30000;
				BuzzerOn(125);
				if(CreateCard((const EmployeeCard_st *)&detail)==DER_NONE)
					BuzzerOn(125);
			}
		}
		else if(SWGetState(SWITCH4))
		{
			//UARTSendData("SWITCH4",7,4000);
			memset((uint8_t*)&detail,0,sizeof(EmployeeCard_st));
			//BuzzerOn(125);
			EmptyMessBuff();
			switch(ReadCard(&detail))
			{
				case DER_NOCARD:
					if(ledIndex == 4)
						LEDOff(ALL_LED);
					else
						LEDOn(ledIndex);
					ledIndex = (ledIndex+1)%5	;
					waitForMess = 0;
					break;
				case DER_NONE:
					BuzzerOn(200);
					UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
					waitForMess = 1;
					break;
				case DER_CARD:
				case DER_INVPASS:
					//error card
					detail.type = 10;
					UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
					waitForMess = 1;
					break;
				default:
				waitForMess = 0;
					break;
			}
			if(waitForMess == 1)
			{
					i = 0;
					delay_ms(100);
					while(IsComingData(&messsz) == FALSE)
					{
					delay_ms(100);
						i++;
						if(i>100){
							LEDOn(LED1);
						 break;
						 }
					}
					if(i<=100)
					{
						UARTGetData(rxMessage, &messsz, 7000);
						switch(rxMessage[0])
						{
							case 0: //none
								BuzzerOn(125);
								delay_ms(500);
								break;
							case 1:
								LEDOn(LED1);
								detail.type = 0;
								//while(PBGetState(Button_USER) == SET);
								//format card
								if(RestoreToDefaultCard()==DER_NONE)
								{
									BuzzerOn(125);
									
								}
								else
								{
									LEDOn(LEDRED);
									detail.type = 10;
									
								}
								UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
								delay_ms(2000);
								break;
							case 2:
							//create card
								memcpy((uint8_t*)&detail,rxMessage+1,messsz-5);
								LEDOn(LED2);
								//detail.type = 0;
								detail.value = rxMessage[messsz-1];
								detail.value = (detail.value<<8)|rxMessage[messsz-2];
								detail.value = (detail.value<<8)|rxMessage[messsz-3];
								detail.value = (detail.value<<8)|rxMessage[messsz-4];
								if(FormatNewCard()==DER_NONE)
								{
									
									detail.type = 0;
									BuzzerOn(125);
								//pn532CardRelease();
									if(CreateCard((const EmployeeCard_st *)&detail)==DER_NONE)
									{
										BuzzerOn(125);
										detail.type = 0;
										}
									else
									{
										LEDOn(LEDRED);
										detail.type = 10;
										
									}
								}
								else
								{
											LEDOn(LEDRED);
											detail.type = 10;
								}
								UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
								delay_ms(2000);
								break;
							case 3:
								//credit value
								LEDOn(LED3);
								detail.type = 0;
								value = rxMessage[4];
								value = (value<<8)|rxMessage[3];
								value = (value<<8)|rxMessage[2];
								value = (value<<8)|rxMessage[1];
								if(CreditValue(value)==DER_NONE)
								{
									BuzzerOn(125);
									detail.value = detail.value + value;
								}
								else
								{
											LEDOn(LEDRED);
											detail.type = 10;
										
								}
								UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
								delay_ms(1000);
								break;
							case 4:
								LEDOn(LED4);
								//credit value
								
								detail.type = 0;
								value = rxMessage[4];
								value = (value<<8)|rxMessage[3];
								value = (value<<8)|rxMessage[2];
								value = (value<<8)|rxMessage[1];
								if(value > detail.value)
								{
									detail.type = 9;
								}
								if(DebitValue(value)==DER_NONE)
								{
									BuzzerOn(125);
									detail.value = detail.value - value;
								}
								else
								{
											LEDOn(LEDRED);
											detail.type = 10;
											
								}
								UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
								delay_ms(1000);
								break;
							case 5:
							//create card
								memcpy((uint8_t*)&detail,rxMessage+1,messsz-5);
								LEDOn(LED2);
								//detail.type = 0;
								detail.value = rxMessage[messsz-1];
								detail.value = (detail.value<<8)|rxMessage[messsz-2];
								detail.value = (detail.value<<8)|rxMessage[messsz-3];
								detail.value = (detail.value<<8)|rxMessage[messsz-4];
								if(FormatOldCard()==DER_NONE)
								{
									BuzzerOn(125);
									if(CreateCard((const EmployeeCard_st *)&detail)==DER_NONE)
									{
										BuzzerOn(125);
										detail.type = 0;
										}
									else
									{
										LEDOn(LEDRED);
										detail.type = 10;
										
									}
								}
								else
								{
											LEDOn(LEDRED);
											detail.type = 10;
								}
								UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
								delay_ms(2000);
								break;
							
							default:
								break;
						}
						
						
					}
					ledIndex = 0;
			}
//			if(ReadCard(&detail)==DER_NONE)
//			{
//				BuzzerOn(125);
//				if(detail.type == ADMIN_CARD)
//					LEDOn(LED4);
//				else if (detail.type == HEADDEP_CARD)
//					LEDOn(LED3);
//				else if (detail.type == EMPLOYEE_CARD)
//					LEDOn(LED2);
//				else
//					LEDOn(LEDRED);
//				UARTSendData((uint8_t*)&detail,sizeof(EmployeeCard_st),4000);
			//	while(IsComingData(&messsz) == FALSE);
				
			//		UARTGetData(rxMessage, &messsz, 2000);
			//		if(messsz == sizeof(EmployeeCard_st))
			//			LEDOn(LEDBGND1); 
				
			//}
			
		}
		else
		{
			 //then restore to default card
			 //UARTSendData("SWITCH5",7,4000);
			 while(PBGetState(Button_USER) == SET);
				if(RestoreToDefaultCard()==DER_NONE)
					BuzzerOn(125);
		}
		delay_ms(200);
	}
}
//conver date issue and date expire to byte array
//dateissue : ddmmyyyy
void DateToByteArray(uint32_t dateIssue,uint32_t dateExpire,uint8_t * buf)
{
	buf[0] = (dateIssue>>24)&0xff;
	buf[1] = (dateIssue>>16)&0xff;
	buf[2] = (dateIssue>>8)&0xff;
	buf[3] = (dateIssue)&0xff;
	buf[4] = (dateExpire>>24)&0xff;
	buf[5] = (dateExpire>>16)&0xff;
	buf[6] = (dateExpire>>8)&0xff;
	buf[7] = (dateExpire)&0xff;
}

void WorkingToByteArray(uint8_t day,uint16_t start,uint16_t end,uint8_t * buf)
{
	buf[0] = day;
	buf[1] = (start>>8)&0xff;
	buf[2] = (start)&0xff;
	buf[3] = (end>>8)&0xff;
	buf[4] = (end)&0xff;
}
void InitEmployee(EmployeeCard_st * detail,uint8_t type,uint8_t * empID, 
									uint32_t dateIssue,uint32_t dateExpire,
									uint8_t day,uint32_t start, uint16_t end, uint8_t * user, uint32_t value)
{
	detail->type = type;
	memcpy(detail->empID,empID,FEMPID_SZ);
  DateToByteArray(dateIssue,dateExpire,detail->date);
	WorkingToByteArray(day,start,end,detail->working);
	memcpy(detail->user,user,FUSER_SZ);
	detail->value = value;
}
//-------------------------------------------------------------------------------

void DuyTest_1(void) //test delay_ms, and send UARTSendData 
{
	 int i;
	int messsz;
	uint8_t Id[FEMPID_SZ];
	uint8_t User[FUSER_SZ];
	uint8_t rxMessage[64];
	
	EmployeeCard_st detail;
	StartReader();
	while(1)
	{
		BuzzerOn(125);
		Delay_ms(5000);
	}
	for(i = 0; i< FEMPID_SZ; i++)
		Id[i] = i;
	memset(User,0,FUSER_SZ);
	InitEmployee(&detail,ADMIN_CARD,Id,(26<<24)|(10<<16)|2011,(28<<24)|(10<<16)|2011,
							REGULAR,0x0800,0x1200,User,10000);

	for(i = 0; i<64; i++)
	{
		 rxMessage[i] = 48 + i;
	}
	rxMessage[62] = 13;
	rxMessage[63] = 10;
	while(1)
	{
		LEDOn(LEDBGND1);
		BuzzerOn(125);
		for (i = 0; i<1; i++)
		{
			UARTSendData((uint8_t*)&rxMessage,64,4000);
		}
		LEDOff(LEDBGND1);
		BuzzerOn(125);

		Delay_ms(5000);
	}
}
void DuyTest_2(void) //test read, write card
{
	int i;
	int messsz;
	uint8_t j = 0;

	uint8_t Id[FEMPID_SZ];
	uint8_t User[10][FUSER_SZ];
	uint8_t rxMessage[64];
	
	EmployeeCard_st detail;
	StartReader();
	for(i = 0; i< FEMPID_SZ; i++)
		Id[i] = i;
	memset(User[0],0,FUSER_SZ);
	for(i = 0; i<32; i++)
	{
		//rxMessage[i] = (uint8_t*)&detail[i];
		User[0][i] = 48 + i;
		User[1][i] = 49 + i;
		User[2][i] = 50 + i;
		User[3][i] = 51 + i;
		User[4][i] = 52 + i;
		User[5][i] = 53 + i;
	}
	rxMessage[0] = 13;
	rxMessage[1] = 10;
	InitEmployee(&detail,ADMIN_CARD,Id,(26<<24)|(10<<16)|2011,(28<<24)|(10<<16)|2011,
							REGULAR,0x0800,0x1200,User[0],10000);
	
	WriteUser(User[0]);
	while(1)
	{
		if(ReadCard(&detail)==DER_NONE)
			{
				BuzzerOn(10);
				if(detail.type == ADMIN_CARD)
					LEDOn(LED4);
				else if (detail.type == HEADDEP_CARD)
					LEDOn(LED3);
				else if (detail.type == EMPLOYEE_CARD)
					LEDOn(LED2);
				else
					LEDOn(LEDRED);

				UARTSendData((uint8_t*)(detail.user),32,4000);
				UARTSendData(rxMessage,2,4000);	
				
				j = (j+1)%6;
				WriteUser(User[j]);
				BuzzerOn(10);
				Delay_ms(1000);
			}
	}
}

void DuyTest(void)
{
	//DuyTest_1();
	DuyTest_2();
		
}
