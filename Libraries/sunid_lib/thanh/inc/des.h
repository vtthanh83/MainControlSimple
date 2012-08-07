/*
 *	cryptoApi.h
 *	Release $Name: MATRIXSSL-3-2-1-OPEN $
 *
 *	Prototypes for the PeerSec crypto public APIs
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2011. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PS_CRYPTOAPI
#define _h_PS_CRYPTOAPI

#include "symmetric.h" /* Must be first included */

/******************************************************************************/
/*	Public return codes */
/******************************************************************************/
/******************************************************************************/
/*
	Universal return codes
*/
#define PS_SUCCESS			0
#define PS_FAILURE			-1	

/*	NOTE: Failure return codes MUST be < 0 */
/*	NOTE: The range for core error codes should be between -2 and -29 */
#define PS_ARG_FAIL			-6	/* Failure due to bad function param */
#define PS_PLATFORM_FAIL	-7	/* Failure as a result of system call error */
#define PS_MEM_FAIL			-8	/* Failure to allocate requested memory */	
#define PS_LIMIT_FAIL		-9	/* Failure on sanity/limit tests */	
#define PS_UNSUPPORTED_FAIL	-10 /* Unimplemented feature error */	
#define PS_DISABLED_FEATURE_FAIL -11 /* Incorrect #define toggle for feature */
#define PS_PROTOCOL_FAIL	-12 /* A protocol error occurred */
#define PS_TIMEOUT_FAIL		-13 /* A timeout occurred and MAY be an error */
#define PS_INTERRUPT_FAIL	-14 /* An interrupt occurred and MAY be an error */
#define PS_PENDING			-15 /* In process. Not necessarily an error */ 

#define	PS_TRUE		1
#define	PS_FALSE 	0
/*	Failure codses MUST be < 0  */
/*	NOTE: The range for crypto error codes must be between -30 and -49  */
#define	PS_PARSE_FAIL			-31

/*
	PS NOTE:  Any future additions to certificate authentication failures
	must be carried through to MatrixSSL code
*/	
#define PS_CERT_AUTH_PASS	PS_TRUE
#define	PS_CERT_AUTH_FAIL_BC	-32 /* BasicConstraint failure */
#define	PS_CERT_AUTH_FAIL_DN	-33 /* DistinguishedName failure */
#define	PS_CERT_AUTH_FAIL_SIG	-34 /* Signature validation failure */
#define	PS_CERT_AUTH_FAIL		-35 /* Generic cert auth fail */

#define PS_SIGNATURE_MISMATCH	-36 /* Alorithms all work but sig not a match */

/******************************************************************************/

#define	CRYPT_INVALID_KEYSIZE	-21 
#define	CRYPT_INVALID_ROUNDS	-22

/******************************************************************************/

/* rotates the hard way */
#define ROL(x, y) \
	( (((unsigned int)(x)<<(unsigned int)((y)&31)) | \
	(((unsigned int)(x)&0xFFFFFFFFUL)>>(unsigned int)(32-((y)&31)))) & \
	0xFFFFFFFFUL)
#define ROR(x, y) \
	( ((((unsigned int)(x)&0xFFFFFFFFUL)>>(unsigned int)((y)&31)) | \
	((unsigned int)(x)<<(unsigned int)(32-((y)&31)))) & 0xFFFFFFFFUL)
#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned int)((y)[0] & 255)<<24) | \
((unsigned int)((y)[1] & 255)<<16) | \
((unsigned int)((y)[2] & 255)<<8)  | \
((unsigned int)((y)[3] & 255)); \
}
/******************************************************************************/
/* Public APIs */

/******************************************************************************/

/******************************************************************************/
//#ifdef USE_DES
/******************************************************************************/
int psDesInitKey(const unsigned char *key, int keylen,
						psDes3Key_t *skey);
void psDesEncryptBlock(const unsigned char *pt, unsigned char *ct,
						psDes3Key_t *skey);
void psDesDecryptBlock(const unsigned char *ct, unsigned char *pt,
						psDes3Key_t *skey);
//#endif /* USE_DES */
/******************************************************************************/

/******************************************************************************/
//int psInitPrng(psRandom_t *ctx);
int psGetPrng(unsigned char *bytes, unsigned char size);
				
/******************************************************************************/
unsigned short CRC16_Generate(unsigned char* message, unsigned int len);
void iso14443a_crc (unsigned char * pbtData, int szLen, unsigned char * pbtCrc);

#endif /* _h_PS_CRYPTOAPI */
/******************************************************************************/

