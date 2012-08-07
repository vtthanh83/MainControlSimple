#include "thanhtest.h"

void testMifareClassic(void)
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
		LEDOn(LEDRED);
	delay_ms(10);
	
	err = pn532SAMConfigure();
	if(err != PN532_ERROR_NONE)
		LEDOn(LEDBGND1);
	delay_ms(10);

	while(1)
	{
	   while(PBGetState(Button_USER) == SET);
	   LEDOff(ALL_LED);
	   delay_ms(1000);
	   //select mifare card
	   err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
	   
	   if(err != PN532_ERROR_NONE)
		 LEDOn(LED1);
	   else
	   {
	   		unsigned char key[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
			delay_ms(10);
	   		// login
	   
	   		err = pn532MifareLogin(3,PN532_MIFARE_AUTHENTICATION_A,key);
			if(err!= PN532_ERROR_NONE)
				LEDOn(LED2);
			else
			{
				unsigned char mac[16];
				//read block 0
				err = pn532MifareRead16(3, mac);
				if(err!= PN532_ERROR_NONE)
					LEDOn(LED3);
				else
					BuzzerOn(125);
				//write to block 1
			}
			//login block 1
			/*
			err = pn532MifareLogin(1,PN532_MIFARE_AUTHENTICATION_A,key);
			if(err!= PN532_ERROR_NONE)
				LEDOn(LED2);
			else
			{
				unsigned char write[16] = {0x11,0x22,0x33,0x44,0x55,0x66,0x88,0x99,0xaa,0xbb,0xcc, 0xdd,0xee,0xfe,0xff};
				//read block 0
				err = pn532MifareWrite16(1, 1, write);
				if(err!= PN532_ERROR_NONE)
					LEDOn(LED4);
				else
					BuzzerOn(125);
				//write to block 1
			}
			*/
			pn532CardRelease();
		}
	} 
}
void testMifareDesfire(void)
{
	pn532_error_t err;
	//Single DES key (2nd 8 bytes are the same to first 8 bytes) 
	unsigned char key[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char newKey[16] = {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0};
	unsigned char newKey1[16] = {2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0};
	unsigned char wrdata[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	unsigned char rddata[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	unsigned char version[32];
	uint32_t max;
	int	filesize;
	DESFIRE_CreateAID app;
	DESFIRE_FileSetting fileSetting;
	UARTInit();
	LEDInit(ALL_LED);
	LEDOn(ALL_LED);
	SWInit(ALL_SWITCH, SWITCH_MODE_GPIO);	
	PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	BuzzerInit();
	pn532Init();
	BuzzerOn(125);
	delay_ms(100);
	//Get firmware version
	err = pn532GetFirmware();
	if(err != PN532_ERROR_NONE)
		LEDOn(LEDRED);
	delay_ms(10);
	
	err = pn532SAMConfigure();
	if(err != PN532_ERROR_NONE)
		LEDOn(LEDBGND1);
	delay_ms(10);
	
	if(err != PN532_ERROR_NONE)
		LEDOn(LEDBGND1);
	delay_ms(10);
	while(1)
	{
	   while(PBGetState(Button_USER) == SET);
	   LEDOff(ALL_LED);
	   
	   delay_ms(1000);
	   //select mifare card
	   err = pn532CardList(PN532_MODULATION_ISO14443A_106KBPS);
	   
	   if(err != PN532_ERROR_NONE)
		 LEDOn(LED1);
	   else
	   {   
	   		//Authenticate to PICC Master key (AID = 0x00, key no = 0)
	   		app.AID[0] = 0x00;
			app.AID[1] = 0x00;
			app.AID[2] = 0x00;
	   		err = pn532SelectApplication(&app);
			if(err != PN532_ERROR_NONE)
		 		LEDOn(LED1);
			else{
	   			err = pn532DesfireAuthenticate(key, 0);
				if(err != PN532_ERROR_NONE)
		 			LEDOn(LED2);
			}
	   		if(SWGetState(SWITCH1)&&(err == PN532_ERROR_NONE)) //format Picc
			{
				err = pn532FormatPICC();
				if(err != PN532_ERROR_NONE)
	 				LEDOn(LED3);
				//change key 0 from default value to new key
				//	err = pn532DesfireChangeKey(0,key,newKey);
				//	if(err != PN532_ERROR_NONE)
		 		//		LEDOn(LEDRED);	
			}	 
			else if(SWGetState(SWITCH2)&&(err == PN532_ERROR_NONE))
			{
				//Create application id = 0x01,change key 1, create standard file, write and read
   			uint8_t keySetting,num;
				app.AID[0] = 0x01;
				app.AID[1] = 0x00;
				app.AID[2] = 0x00;
				app.keySetting = 0xf;	//free to create file, delete file, get setting...
				app.numKey = 3;			//MasterKey and two user keys			
				err = pn532CreateApplication(&app);
				if(err != PN532_ERROR_NONE)
		 			LEDOn(LED4);
					
				else
				{
					//after create successfully, switch to app 0x01	
					err = pn532SelectApplication(&app);
					//Authenticated with master key of app 0x01
					err = pn532DesfireAuthenticate(key, 0);
					if(err != PN532_ERROR_NONE)
			 			LEDOn(LEDRED);
					//Get key setting of this app
					err = pn532DesfireGetKeySetting(&keySetting,&num);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED4);
					//chage key setting for this app
					app.keySetting = KS_ALLOW_MASTER|KS_CONF_CHANGE;
					err = pn532DesfireChangeKeySetting(&(app.keySetting));
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDBGND1);	
					//Get back key setting
					err = pn532DesfireGetKeySetting(&keySetting,&num);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDBGND2);
					//change key 1 from default value to new key
					err = pn532DesfireChangeKey(1,key,newKey);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDRED);
					//Create standard file  
					fileSetting.fileNo = 0; //file number 0 (file number range form 0  t oxof)
					fileSetting.fileType = DESFIRE_STANDARD_FILE;	//creat standard file
					fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
					//using key 1 to change access right, read, write right
					fileSetting.accessRight = (1<<DESFIRE_FILE_READ)|(1<<DESFIRE_FILE_WRITE)
											|(1<<DESFIRE_FILE_RDWR)|(1<<DESFIRE_FILE_CHANGE);
					fileSetting.file.stdFile.size = 32; //length of file is 32
					err = pn532DesfireCreateFile(&fileSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDRED);
					err = pn532DesfireGetFileSettings(&fileSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED1);
					//Authenticate to key 1 to write and read from file
					err = pn532DesfireAuthenticate(newKey, 1);	 
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					//write data to this file
					err = pn532DesfireWriteData(fileSetting.fileNo, 0, 16, fileSetting.commSetting, wrdata, &filesize);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					UARTSendData(wrdata,16,2000);
					//read back from this file
					err = pn532DesfireReadData(fileSetting.fileNo, 0, 16, fileSetting.commSetting, rddata, &filesize);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					
					UARTSendData(rddata,16,2000);
					delay_ms(10);
				}
			} 
			
			else if(SWGetState(SWITCH3)&&(err == PN532_ERROR_NONE))
			{
				//Create application id = 0x02,change key 1, create value file, credit and debit
   				
				app.AID[0] = 0x02;
				app.AID[1] = 0x00;
				app.AID[2] = 0x00;
				//nedd authenticate to create file, delete file, get setting...
				app.keySetting = KS_ALLOW_MASTER|KS_CONF_CHANGE;
				app.numKey = 3;			//MasterKey and two user keys			
				err = pn532CreateApplication(&app);
				if(err != PN532_ERROR_NONE)
		 			LEDOn(LED4);
					
				else
				{
					int32_t value;
					//after create successfully, switch to app 0x02	
					err = pn532SelectApplication(&app);
					//Authenticated with master key of app 0x01
					err = pn532DesfireAuthenticate(key, 0);
					if(err != PN532_ERROR_NONE)
			 			LEDOn(LEDRED);
					
					
					//change key 1 from default value to new key
					err = pn532DesfireChangeKey(1,key,newKey);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDRED);
					//change key 2 from default value to new key
					err = pn532DesfireChangeKey(2,key,newKey1);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDRED);
					//Create a value file  
					fileSetting.fileNo = 0; //file number 0 (file number range form 0  to 0x0f)
					fileSetting.fileType = DESFIRE_VALUE_FILE;	//creat value file
					fileSetting.commSetting = DESFIRE_FILE_DES; 	//Data are communicate using DES encrypt
					//using key 1 to change access right, read, 2 to write right
					fileSetting.accessRight = (1<<DESFIRE_FILE_READ)|(2<<DESFIRE_FILE_WRITE)
											|(2<<DESFIRE_FILE_RDWR)|(1<<DESFIRE_FILE_CHANGE);
					fileSetting.file.valueFile.lowerLimit = 0; //min value
					fileSetting.file.valueFile.upperLimit = 5000000; //max value
					fileSetting.file.valueFile.value = 10000; //init value
					fileSetting.file.valueFile.limitedCreditEn = 0; 
					err = pn532DesfireCreateFile(&fileSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LEDRED);
					
					//Authenticate to key 1 to read value from file
					err = pn532DesfireAuthenticate(newKey, 1);	 
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					//Get value from this file
					err = pn532DesfireGetValue(fileSetting.fileNo, &value, fileSetting.commSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					UARTSendData((uint8_t *) &value,4,2000);
					//Authenticate to key 2 to write value from file
					err = pn532DesfireAuthenticate(newKey1, 2);	 
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					//credit value
					value = 300000;
					err = pn532DesfireCredit(fileSetting.fileNo, value, fileSetting.commSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					//debit value
					value = 100000;
					err = pn532DesfireDebit(fileSetting.fileNo, value, fileSetting.commSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					//Commit transaction
					err = pn532DesfireCommitTransaction();
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					//read back value (right if returned value = 210000 = 0x33450
					//Get value from this file
					err = pn532DesfireGetValue(fileSetting.fileNo, &value, fileSetting.commSetting);
					if(err != PN532_ERROR_NONE)
		 				LEDOn(LED3);
					UARTSendData((uint8_t *) &value,4,2000);
					delay_ms(10);
				}
			}	  
			pn532CardRelease();
			while(PBGetState(Button_USER) == SET);
	   //LEDOff(ALL_LED);
	   delay_ms(1000);
	   }
	} 
}

