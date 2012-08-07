#ifndef __AES_H
#define __AES_H

#include "stm32sunid.h"

//prototype
ErrorCode InitAes128(void);
ErrorCode SendMessage(uint8_t* buffer, int len, int timeout);
ErrorCode GetMessage(uint8_t* buffer, int* retlen, int timeout);
bool IsComingMessage(void); //ham nay lam cai giong gi?


#endif

