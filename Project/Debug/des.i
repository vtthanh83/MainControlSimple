#line 1 "..\\Libraries\\sunid_lib\\thanh\\src\\des.c"





 






















 
 

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


 

#line 33 "..\\Libraries\\sunid_lib\\thanh\\src\\des.c"
#line 1 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"
 
 
 
 




 








 











#line 37 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"


  
  typedef unsigned int size_t;








extern __declspec(__nothrow) void *memcpy(void * __restrict  ,
                    const void * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) void *memmove(void *  ,
                    const void *  , size_t  ) __attribute__((__nonnull__(1,2)));
   







 
extern __declspec(__nothrow) char *strcpy(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) char *strncpy(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   





 

extern __declspec(__nothrow) char *strcat(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) char *strncat(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 






 

extern __declspec(__nothrow) int memcmp(const void *  , const void *  , size_t  ) __attribute__((__nonnull__(1,2)));
   





 
extern __declspec(__nothrow) int strcmp(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   




 
extern __declspec(__nothrow) int strncmp(const char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int strcasecmp(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   





 
extern __declspec(__nothrow) int strncasecmp(const char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






 
extern __declspec(__nothrow) int strcoll(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   







 

extern __declspec(__nothrow) size_t strxfrm(char * __restrict  , const char * __restrict  , size_t  ) __attribute__((__nonnull__(2)));
   













 


#line 184 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"
extern __declspec(__nothrow) void *memchr(const void *  , int  , size_t  ) __attribute__((__nonnull__(1)));

   





 

#line 200 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"
extern __declspec(__nothrow) char *strchr(const char *  , int  ) __attribute__((__nonnull__(1)));

   




 

extern __declspec(__nothrow) size_t strcspn(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   




 

#line 223 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"
extern __declspec(__nothrow) char *strpbrk(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));

   




 

#line 238 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"
extern __declspec(__nothrow) char *strrchr(const char *  , int  ) __attribute__((__nonnull__(1)));

   





 

extern __declspec(__nothrow) size_t strspn(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));
   



 

#line 261 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"
extern __declspec(__nothrow) char *strstr(const char *  , const char *  ) __attribute__((__nonnull__(1,2)));

   





 

extern __declspec(__nothrow) char *strtok(char * __restrict  , const char * __restrict  ) __attribute__((__nonnull__(2)));
extern __declspec(__nothrow) char *_strtok_r(char *  , const char *  , char **  ) __attribute__((__nonnull__(2,3)));

extern __declspec(__nothrow) char *strtok_r(char *  , const char *  , char **  ) __attribute__((__nonnull__(2,3)));

   

































 

extern __declspec(__nothrow) void *memset(void *  , int  , size_t  ) __attribute__((__nonnull__(1)));
   



 
extern __declspec(__nothrow) char *strerror(int  );
   





 
extern __declspec(__nothrow) size_t strlen(const char *  ) __attribute__((__nonnull__(1)));
   



 

extern __declspec(__nothrow) size_t strlcpy(char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   
















 

extern __declspec(__nothrow) size_t strlcat(char *  , const char *  , size_t  ) __attribute__((__nonnull__(1,2)));
   






















 

extern __declspec(__nothrow) void _membitcpybl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpybb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpyhl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpyhb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpywl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitcpywb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovebl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovebb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovehl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovehb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovewl(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
extern __declspec(__nothrow) void _membitmovewb(void *  , const void *  , int  , int  , size_t  ) __attribute__((__nonnull__(1,2)));
    














































 







#line 493 "D:\\Installed Programs\\Keil\\ARM\\RV31\\Inc\\string.h"



 
#line 34 "..\\Libraries\\sunid_lib\\thanh\\src\\des.c"

 

 




static const unsigned int bytebit[8] =
{
	0200, 0100, 040, 020, 010, 04, 02, 01 
};

static const unsigned int bigbyte[24] =
{
	0x800000UL,		0x400000UL,		0x200000UL,		0x100000UL,
	0x80000UL,		0x40000UL,		0x20000UL,		0x10000UL,
	0x8000UL,		0x4000UL,		0x2000UL,		0x1000UL,
	0x800UL,		0x400UL,		0x200UL,		0x100UL,
	0x80UL,			0x40UL,			0x20UL,			0x10UL,
	0x8UL,			0x4UL,			0x2UL,			0x1L 
};

static const unsigned char pc1[56] = {
    56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,  
     9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35, 
    62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3 
};

static const unsigned char pc2[48] = {
    13, 16, 10, 23,  0,  4,      2, 27, 14,  5, 20,  9,
    22, 18, 11,  3, 25,  7,     15,  6, 26, 19, 12,  1,
    40, 51, 30, 36, 46, 54,     29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,     45, 41, 49, 35, 28, 31
};

static const unsigned char totrot[16] = {
    1,   2,  4,  6,
    8,  10, 12, 14, 
    15, 17, 19, 21, 
    23, 25, 27, 28
};

static const unsigned int SP1[64] =
{
	0x01010400UL, 0x00000000UL, 0x00010000UL, 0x01010404UL,
	0x01010004UL, 0x00010404UL, 0x00000004UL, 0x00010000UL,
	0x00000400UL, 0x01010400UL, 0x01010404UL, 0x00000400UL,
	0x01000404UL, 0x01010004UL, 0x01000000UL, 0x00000004UL,
	0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00010400UL,
	0x00010400UL, 0x01010000UL, 0x01010000UL, 0x01000404UL,
	0x00010004UL, 0x01000004UL, 0x01000004UL, 0x00010004UL,
	0x00000000UL, 0x00000404UL, 0x00010404UL, 0x01000000UL,
	0x00010000UL, 0x01010404UL, 0x00000004UL, 0x01010000UL,
	0x01010400UL, 0x01000000UL, 0x01000000UL, 0x00000400UL,
	0x01010004UL, 0x00010000UL, 0x00010400UL, 0x01000004UL,
	0x00000400UL, 0x00000004UL, 0x01000404UL, 0x00010404UL,
	0x01010404UL, 0x00010004UL, 0x01010000UL, 0x01000404UL,
	0x01000004UL, 0x00000404UL, 0x00010404UL, 0x01010400UL,
	0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00000000UL,
	0x00010004UL, 0x00010400UL, 0x00000000UL, 0x01010004UL
};

static const unsigned int SP2[64] =
{
	0x80108020UL, 0x80008000UL, 0x00008000UL, 0x00108020UL,
	0x00100000UL, 0x00000020UL, 0x80100020UL, 0x80008020UL,
	0x80000020UL, 0x80108020UL, 0x80108000UL, 0x80000000UL,
	0x80008000UL, 0x00100000UL, 0x00000020UL, 0x80100020UL,
	0x00108000UL, 0x00100020UL, 0x80008020UL, 0x00000000UL,
	0x80000000UL, 0x00008000UL, 0x00108020UL, 0x80100000UL,
	0x00100020UL, 0x80000020UL, 0x00000000UL, 0x00108000UL,
	0x00008020UL, 0x80108000UL, 0x80100000UL, 0x00008020UL,
	0x00000000UL, 0x00108020UL, 0x80100020UL, 0x00100000UL,
	0x80008020UL, 0x80100000UL, 0x80108000UL, 0x00008000UL,
	0x80100000UL, 0x80008000UL, 0x00000020UL, 0x80108020UL,
	0x00108020UL, 0x00000020UL, 0x00008000UL, 0x80000000UL,
	0x00008020UL, 0x80108000UL, 0x00100000UL, 0x80000020UL,
	0x00100020UL, 0x80008020UL, 0x80000020UL, 0x00100020UL,
	0x00108000UL, 0x00000000UL, 0x80008000UL, 0x00008020UL,
	0x80000000UL, 0x80100020UL, 0x80108020UL, 0x00108000UL
};

static const unsigned int SP3[64] =
{
	0x00000208UL, 0x08020200UL, 0x00000000UL, 0x08020008UL,
	0x08000200UL, 0x00000000UL, 0x00020208UL, 0x08000200UL,
	0x00020008UL, 0x08000008UL, 0x08000008UL, 0x00020000UL,
	0x08020208UL, 0x00020008UL, 0x08020000UL, 0x00000208UL,
	0x08000000UL, 0x00000008UL, 0x08020200UL, 0x00000200UL,
	0x00020200UL, 0x08020000UL, 0x08020008UL, 0x00020208UL,
	0x08000208UL, 0x00020200UL, 0x00020000UL, 0x08000208UL,
	0x00000008UL, 0x08020208UL, 0x00000200UL, 0x08000000UL,
	0x08020200UL, 0x08000000UL, 0x00020008UL, 0x00000208UL,
	0x00020000UL, 0x08020200UL, 0x08000200UL, 0x00000000UL,
	0x00000200UL, 0x00020008UL, 0x08020208UL, 0x08000200UL,
	0x08000008UL, 0x00000200UL, 0x00000000UL, 0x08020008UL,
	0x08000208UL, 0x00020000UL, 0x08000000UL, 0x08020208UL,
	0x00000008UL, 0x00020208UL, 0x00020200UL, 0x08000008UL,
	0x08020000UL, 0x08000208UL, 0x00000208UL, 0x08020000UL,
	0x00020208UL, 0x00000008UL, 0x08020008UL, 0x00020200UL
};

static const unsigned int SP4[64] =
{
	0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
	0x00802080UL, 0x00800081UL, 0x00800001UL, 0x00002001UL,
	0x00000000UL, 0x00802000UL, 0x00802000UL, 0x00802081UL,
	0x00000081UL, 0x00000000UL, 0x00800080UL, 0x00800001UL,
	0x00000001UL, 0x00002000UL, 0x00800000UL, 0x00802001UL,
	0x00000080UL, 0x00800000UL, 0x00002001UL, 0x00002080UL,
	0x00800081UL, 0x00000001UL, 0x00002080UL, 0x00800080UL,
	0x00002000UL, 0x00802080UL, 0x00802081UL, 0x00000081UL,
	0x00800080UL, 0x00800001UL, 0x00802000UL, 0x00802081UL,
	0x00000081UL, 0x00000000UL, 0x00000000UL, 0x00802000UL,
	0x00002080UL, 0x00800080UL, 0x00800081UL, 0x00000001UL,
	0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
	0x00802081UL, 0x00000081UL, 0x00000001UL, 0x00002000UL,
	0x00800001UL, 0x00002001UL, 0x00802080UL, 0x00800081UL,
	0x00002001UL, 0x00002080UL, 0x00800000UL, 0x00802001UL,
	0x00000080UL, 0x00800000UL, 0x00002000UL, 0x00802080UL
};

static const unsigned int SP5[64] =
{
	0x00000100UL, 0x02080100UL, 0x02080000UL, 0x42000100UL,
	0x00080000UL, 0x00000100UL, 0x40000000UL, 0x02080000UL,
	0x40080100UL, 0x00080000UL, 0x02000100UL, 0x40080100UL,
	0x42000100UL, 0x42080000UL, 0x00080100UL, 0x40000000UL,
	0x02000000UL, 0x40080000UL, 0x40080000UL, 0x00000000UL,
	0x40000100UL, 0x42080100UL, 0x42080100UL, 0x02000100UL,
	0x42080000UL, 0x40000100UL, 0x00000000UL, 0x42000000UL,
	0x02080100UL, 0x02000000UL, 0x42000000UL, 0x00080100UL,
	0x00080000UL, 0x42000100UL, 0x00000100UL, 0x02000000UL,
	0x40000000UL, 0x02080000UL, 0x42000100UL, 0x40080100UL,
	0x02000100UL, 0x40000000UL, 0x42080000UL, 0x02080100UL,
	0x40080100UL, 0x00000100UL, 0x02000000UL, 0x42080000UL,
	0x42080100UL, 0x00080100UL, 0x42000000UL, 0x42080100UL,
	0x02080000UL, 0x00000000UL, 0x40080000UL, 0x42000000UL,
	0x00080100UL, 0x02000100UL, 0x40000100UL, 0x00080000UL,
	0x00000000UL, 0x40080000UL, 0x02080100UL, 0x40000100UL
};

static const unsigned int SP6[64] =
{
	0x20000010UL, 0x20400000UL, 0x00004000UL, 0x20404010UL,
	0x20400000UL, 0x00000010UL, 0x20404010UL, 0x00400000UL,
	0x20004000UL, 0x00404010UL, 0x00400000UL, 0x20000010UL,
	0x00400010UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
	0x00000000UL, 0x00400010UL, 0x20004010UL, 0x00004000UL,
	0x00404000UL, 0x20004010UL, 0x00000010UL, 0x20400010UL,
	0x20400010UL, 0x00000000UL, 0x00404010UL, 0x20404000UL,
	0x00004010UL, 0x00404000UL, 0x20404000UL, 0x20000000UL,
	0x20004000UL, 0x00000010UL, 0x20400010UL, 0x00404000UL,
	0x20404010UL, 0x00400000UL, 0x00004010UL, 0x20000010UL,
	0x00400000UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
	0x20000010UL, 0x20404010UL, 0x00404000UL, 0x20400000UL,
	0x00404010UL, 0x20404000UL, 0x00000000UL, 0x20400010UL,
	0x00000010UL, 0x00004000UL, 0x20400000UL, 0x00404010UL,
	0x00004000UL, 0x00400010UL, 0x20004010UL, 0x00000000UL,
	0x20404000UL, 0x20000000UL, 0x00400010UL, 0x20004010UL
};

static const unsigned int SP7[64] =
{
	0x00200000UL, 0x04200002UL, 0x04000802UL, 0x00000000UL,
	0x00000800UL, 0x04000802UL, 0x00200802UL, 0x04200800UL,
	0x04200802UL, 0x00200000UL, 0x00000000UL, 0x04000002UL,
	0x00000002UL, 0x04000000UL, 0x04200002UL, 0x00000802UL,
	0x04000800UL, 0x00200802UL, 0x00200002UL, 0x04000800UL,
	0x04000002UL, 0x04200000UL, 0x04200800UL, 0x00200002UL,
	0x04200000UL, 0x00000800UL, 0x00000802UL, 0x04200802UL,
	0x00200800UL, 0x00000002UL, 0x04000000UL, 0x00200800UL,
	0x04000000UL, 0x00200800UL, 0x00200000UL, 0x04000802UL,
	0x04000802UL, 0x04200002UL, 0x04200002UL, 0x00000002UL,
	0x00200002UL, 0x04000000UL, 0x04000800UL, 0x00200000UL,
	0x04200800UL, 0x00000802UL, 0x00200802UL, 0x04200800UL,
	0x00000802UL, 0x04000002UL, 0x04200802UL, 0x04200000UL,
	0x00200800UL, 0x00000000UL, 0x00000002UL, 0x04200802UL,
	0x00000000UL, 0x00200802UL, 0x04200000UL, 0x00000800UL,
	0x04000002UL, 0x04000800UL, 0x00000800UL, 0x00200002UL
};

static const unsigned int SP8[68] =
{
	0x10001040UL, 0x00001000UL, 0x00040000UL, 0x10041040UL,
	0x10000000UL, 0x10001040UL, 0x00000040UL, 0x10000000UL,
	0x00040040UL, 0x10040000UL, 0x10041040UL, 0x00041000UL,
	0x10041000UL, 0x00041040UL, 0x00001000UL, 0x00000040UL,
	0x10040000UL, 0x10000040UL, 0x10001000UL, 0x00001040UL,
	0x00041000UL, 0x00040040UL, 0x10040040UL, 0x10041000UL,
	0x00001040UL, 0x00000000UL, 0x00000000UL, 0x10040040UL,
	0x10000040UL, 0x10001000UL, 0x00041040UL, 0x00040000UL,
	0x00041040UL, 0x00040000UL, 0x10041000UL, 0x00001000UL,
	0x00000040UL, 0x10040040UL, 0x00001000UL, 0x00041040UL,
	0x10001000UL, 0x00000040UL, 0x10000040UL, 0x10040000UL,
	0x10040040UL, 0x10000000UL, 0x00040000UL, 0x10001040UL,
	0x00000000UL, 0x10041040UL, 0x00040040UL, 0x10000040UL,
	0x10040000UL, 0x10001000UL, 0x10001040UL, 0x00000000UL,
	0x10041040UL, 0x00041000UL, 0x00041000UL, 0x00001040UL,
	0x00001040UL, 0x00040040UL, 0x10000000UL, 0x10041000UL,
	0xe1f27f3aUL, 0xf5710fb0UL, 0xada0e5c4UL, 0x98e4c919UL
};

 


 
void cookey(const unsigned int *raw1, unsigned int *keyout)
{
	unsigned int			*cook;
	const unsigned int	*raw0;
	unsigned int			dough[32];
	int				i;
	
	cook = dough;
	for(i=0; i < 16; i++, raw1++) {
		raw0 = raw1++;
		*cook    = (*raw0 & 0x00fc0000L) << 6;
		*cook   |= (*raw0 & 0x00000fc0L) << 10;
		*cook   |= (*raw1 & 0x00fc0000L) >> 10;
		*cook++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cook    = (*raw0 & 0x0003f000L) << 12;
		*cook   |= (*raw0 & 0x0000003fL) << 16;
		*cook   |= (*raw1 & 0x0003f000L) >> 4;
		*cook++ |= (*raw1 & 0x0000003fL);
	}
	
	memcpy(keyout, dough, sizeof dough);
}

void deskey(const unsigned char *key, int edf, unsigned int *keyout)
{
	unsigned int				i, j, l, m, n, kn[32];
	unsigned char		pc1m[56], pcr[56];
	
	for (j=0; j < 56; j++) {
		l = (unsigned int)pc1[j];
		m = l & 7;
		pc1m[j] = (unsigned char)((key[l >> 3U] & bytebit[m]) == 
								  bytebit[m] ? 1 : 0);
	}
	
	for (i=0; i < 16; i++) {
		if (edf == 1) {
			m = (15 - i) << 1;
		} else {
			m = i << 1;
		}
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for (j=0; j < 28; j++) {
			l = j + (unsigned int)totrot[i];
			if (l < 28) {
				pcr[j] = pc1m[l];
			} else {
				pcr[j] = pc1m[l - 28];
			}
		}
		for ( ; j < 56; j++) {
			l = j + (unsigned int)totrot[i];
			if (l < 56) {
				pcr[j] = pc1m[l];
			} else {
				pcr[j] = pc1m[l - 28];
			}
		}
		for (j=0; j < 24; j++)  {
			if ((int)pcr[(int)pc2[j]] != 0) {
				kn[m] |= bigbyte[j];
			}
			if ((int)pcr[(int)pc2[j+24]] != 0) {
				kn[n] |= bigbyte[j];
			}
		}
	}
	cookey(kn, keyout);
}

void desfunc(unsigned int *block, const unsigned int *keys)
{
	unsigned int work, right, leftt;
	int cur_round;
	
	leftt = block[0];
	right = block[1];
	

	work = ((leftt >> 4)  ^ right) & 0x0f0f0f0fL;
	right ^= work;
	leftt ^= (work << 4);
	
	work = ((leftt >> 16) ^ right) & 0x0000ffffL;
	right ^= work;
	leftt ^= (work << 16);
	
	work = ((right >> 2)  ^ leftt) & 0x33333333L;
	leftt ^= work;
	right ^= (work << 2);
	
	work = ((right >> 8)  ^ leftt) & 0x00ff00ffL;
	leftt ^= work;
	right ^= (work << 8);
	
	right = ( (((unsigned int)(right)<<(unsigned int)((1)&31)) | (((unsigned int)(right)&0xFFFFFFFFUL)>>(unsigned int)(32-((1)&31)))) & 0xFFFFFFFFUL);
	work = (leftt ^ right) & 0xaaaaaaaaL;
	
	leftt ^= work;
	right ^= work;
	leftt = ( (((unsigned int)(leftt)<<(unsigned int)((1)&31)) | (((unsigned int)(leftt)&0xFFFFFFFFUL)>>(unsigned int)(32-((1)&31)))) & 0xFFFFFFFFUL);

	
	for (cur_round = 0; cur_round < 8; cur_round++) {
		work	= ( ((((unsigned int)(right)&0xFFFFFFFFUL)>>(unsigned int)((4)&31)) | ((unsigned int)(right)<<(unsigned int)(32-((4)&31)))) & 0xFFFFFFFFUL) ^ *keys++;
		leftt	^= SP7[work        & 0x3fL]
		^ SP5[(work >>  8) & 0x3fL]
		^ SP3[(work >> 16) & 0x3fL]
		^ SP1[(work >> 24) & 0x3fL];
		work	 = right ^ *keys++;
		leftt	^= SP8[ work        & 0x3fL]
		^  SP6[(work >>  8) & 0x3fL]
		^  SP4[(work >> 16) & 0x3fL]
		^  SP2[(work >> 24) & 0x3fL];
		
		work	= ( ((((unsigned int)(leftt)&0xFFFFFFFFUL)>>(unsigned int)((4)&31)) | ((unsigned int)(leftt)<<(unsigned int)(32-((4)&31)))) & 0xFFFFFFFFUL) ^ *keys++;
		right	^= SP7[ work        & 0x3fL]
		^  SP5[(work >>  8) & 0x3fL]
		^  SP3[(work >> 16) & 0x3fL]
		^  SP1[(work >> 24) & 0x3fL];
		work	 = leftt ^ *keys++;
		right	^= SP8[ work        & 0x3fL]
		^  SP6[(work >>  8) & 0x3fL]
		^  SP4[(work >> 16) & 0x3fL]
		^  SP2[(work >> 24) & 0x3fL];
	}
	

	right = ( ((((unsigned int)(right)&0xFFFFFFFFUL)>>(unsigned int)((1)&31)) | ((unsigned int)(right)<<(unsigned int)(32-((1)&31)))) & 0xFFFFFFFFUL);
	work = (leftt ^ right) & 0xaaaaaaaaL;
	leftt ^= work;
	right ^= work;
	leftt = ( ((((unsigned int)(leftt)&0xFFFFFFFFUL)>>(unsigned int)((1)&31)) | ((unsigned int)(leftt)<<(unsigned int)(32-((1)&31)))) & 0xFFFFFFFFUL);
	work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
	right ^= work;
	leftt ^= (work << 8);
	
	work = ((leftt >> 2) ^ right) & 0x33333333L;
	right ^= work;
	leftt ^= (work << 2);
	work = ((right >> 16) ^ leftt) & 0x0000ffffL;
	leftt ^= work;
	right ^= (work << 16);
	work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
	leftt ^= work;
	right ^= (work << 4);


	
	block[0] = right;
	block[1] = leftt;
}


 

 

 

 


 
int psDesInitKey(const unsigned char *key, int keylen, psDes3Key_t *skey)
{
    if (keylen != 8) {
		
        return -21;
    }

    deskey(key, 0, skey->ek[0]);
    deskey(key, 1, skey->dk[0]);

    return 0;
}
void psDesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *key)
{
    unsigned int work[2];

    { work[0] = ((unsigned int)((pt+0)[0] & 255)<<24) | ((unsigned int)((pt+0)[1] & 255)<<16) | ((unsigned int)((pt+0)[2] & 255)<<8) | ((unsigned int)((pt+0)[3] & 255)); };
    { work[1] = ((unsigned int)((pt+4)[0] & 255)<<24) | ((unsigned int)((pt+4)[1] & 255)<<16) | ((unsigned int)((pt+4)[2] & 255)<<8) | ((unsigned int)((pt+4)[3] & 255)); };
    desfunc(work, key->ek[0]);
    { (ct+0)[0] = (unsigned char)(((work[0])>>24)&255); (ct+0)[1] = (unsigned char)(((work[0])>>16)&255); (ct+0)[2] = (unsigned char)(((work[0])>>8)&255); (ct+0)[3] = (unsigned char)((work[0])&255); };
    { (ct+4)[0] = (unsigned char)(((work[1])>>24)&255); (ct+4)[1] = (unsigned char)(((work[1])>>16)&255); (ct+4)[2] = (unsigned char)(((work[1])>>8)&255); (ct+4)[3] = (unsigned char)((work[1])&255); };
}
void psDesDecryptBlock(const unsigned char *ct, unsigned char *pt,
							psDes3Key_t *key)
{
    unsigned int work[2];
    { work[0] = ((unsigned int)((ct+0)[0] & 255)<<24) | ((unsigned int)((ct+0)[1] & 255)<<16) | ((unsigned int)((ct+0)[2] & 255)<<8) | ((unsigned int)((ct+0)[3] & 255)); };
    { work[1] = ((unsigned int)((ct+4)[0] & 255)<<24) | ((unsigned int)((ct+4)[1] & 255)<<16) | ((unsigned int)((ct+4)[2] & 255)<<8) | ((unsigned int)((ct+4)[3] & 255)); };
    desfunc(work, key->dk[0]);
    { (pt+0)[0] = (unsigned char)(((work[0])>>24)&255); (pt+0)[1] = (unsigned char)(((work[0])>>16)&255); (pt+0)[2] = (unsigned char)(((work[0])>>8)&255); (pt+0)[3] = (unsigned char)((work[0])&255); };
    { (pt+4)[0] = (unsigned char)(((work[1])>>24)&255); (pt+4)[1] = (unsigned char)(((work[1])>>16)&255); (pt+4)[2] = (unsigned char)(((work[1])>>8)&255); (pt+4)[3] = (unsigned char)((work[1])&255); };
}
int psGetPrng(unsigned char *bytes, unsigned char size)
{
	int i;
	
	
	bytes[0] = 0xc0;
	bytes[1] = 0xfe;
	bytes[2] = 0xa8;
	bytes[3] = 0x12;
	bytes[4] = 0x39;
	bytes[5] = 0xe4;
	bytes[6] = 0xd1;
	bytes[7] = 0xef;

	return 0;
}



 

