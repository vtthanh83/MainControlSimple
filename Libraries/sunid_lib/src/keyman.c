#include "keyman.h"
#include "string.h"
//#include 	cryptomemory

unsigned char key[]   = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};


ErrorCode GetProtocolKey(uint8_t* keybuf, int* keylen){
	ErrorCode err = ER_OK;
	*keylen = 16;
	memcpy(keybuf, key, 16);
	return err; 	

}
ErrorCode SetNewProtocolKey(uint8_t* keybuf, int keylen){
	ErrorCode err = ER_OK;
	return err;

}
ErrorCode GetRFIDKey(uint8_t* keybuf, int* keylen){
	ErrorCode err = ER_OK;
	return err;

}
ErrorCode GetOldRFIDKey(uint8_t* keybuf, int* keylen){
	ErrorCode err = ER_OK;
	return err;

}



