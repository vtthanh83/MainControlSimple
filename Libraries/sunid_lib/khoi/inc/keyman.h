#ifndef __KEYMAN_H
#define __KEYMAN_H

#include "stm32sunid.h"

ErrorCode GetProtocolKey(uint8_t* keybuf, int* keylen);
ErrorCode SetNewProtocolKey(uint8_t* keybuf, int keylen);
ErrorCode GetRFIDKey(uint8_t* keybuf, int* keylen);
ErrorCode GetOldRFIDKey(uint8_t* keybuf, int* keylen);







#endif
