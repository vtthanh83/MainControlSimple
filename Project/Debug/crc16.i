#line 1 "..\\Libraries\\sunid_lib\\thanh\\src\\crc16.c"
#line 1 "..\\Libraries\\sunid_lib\\thanh\\inc\\des.h"





 






















 
 




#line 1 "..\\Libraries\\sunid_lib\\thanh\\inc\\symmetric.h"





 






















 
 








typedef struct {
	unsigned int ek[3][32], dk[3][32];
} psDes3Key_t;



 
typedef struct {
	int				blocklen;
	unsigned char		IV[8];
	psDes3Key_t			key;
	int				explicitIV;  
} des3_CBC;

 

 
 
 


typedef union {
	des3_CBC	des3;
} psCipherContext_t;




 
#line 36 "..\\Libraries\\sunid_lib\\thanh\\inc\\des.h"

 
 
 
 


 



 
 
#line 59 "..\\Libraries\\sunid_lib\\thanh\\inc\\des.h"



 
 





 	








 




 

 
#line 99 "..\\Libraries\\sunid_lib\\thanh\\inc\\des.h"

#line 106 "..\\Libraries\\sunid_lib\\thanh\\inc\\des.h"
 
 

 

 

 
int psDesInitKey(const unsigned char *key, int keylen,
						psDes3Key_t *skey);
void psDesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *skey);
void psDesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psDes3Key_t *skey);

 

 

int psGetPrng(unsigned char *bytes, unsigned char size);
				
 
unsigned short CRC16_Generate(unsigned char* message, unsigned int len);
void iso14443a_crc (unsigned char * pbtData, int szLen, unsigned char * pbtCrc);


 

#line 2 "..\\Libraries\\sunid_lib\\thanh\\src\\crc16.c"

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

