#include "des.h"

unsigned short UpdateCrc(unsigned char ch, unsigned short *lpwCrc)
{
	ch = (ch^(unsigned char)((*lpwCrc) & 0x00FF));
	ch = (ch^(ch<<4));
	*lpwCrc = (*lpwCrc >> 8)^((unsigned short)ch << 8)^((unsigned short)ch<<3)^((unsigned short)ch>>4);
	return(*lpwCrc);
}

unsigned short CRC16_Generate(unsigned char* message, unsigned int len)
{
	unsigned short result = 0x6363;
	int i = 0;
	
	if(len>0)
	{
	   for(i = 0;i<len;i++)
	   	 //result = crc16_update(result,message[i]);
		 UpdateCrc(message[i],&result);
	}
	return result;
}
void iso14443a_crc (unsigned char * pbtData, int szLen, unsigned char * pbtCrc)
{
   unsigned char  bt;
   unsigned int wCrc = 0x6363;
 
   do {
     bt = *pbtData++;
     bt = (bt ^ (unsigned char) (wCrc & 0x00FF));
     bt = (bt ^ (bt << 4));
     wCrc = (wCrc >> 8) ^ ((unsigned int) bt << 8) ^ ((unsigned int) bt << 3) ^ ((unsigned int) bt >> 4);
   } while (--szLen);
 
   *pbtCrc++ = (unsigned char) (wCrc & 0xFF);
   *pbtCrc = (unsigned char) ((wCrc >> 8) & 0xFF);
}

