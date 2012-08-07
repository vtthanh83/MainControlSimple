/*
 *	symmetric.h
 *	Release $Name: MATRIXSSL-3-2-1-OPEN $
 *
 *	Header for internal symmetric key cryptography support
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

#ifndef _h_PS_SYMMETRIC
#define _h_PS_SYMMETRIC

#define DES3_KEY_LEN	24
#define DES3_IV_LEN		8
#define DES_KEY_LEN		8

typedef struct {
	unsigned int ek[3][32], dk[3][32];
} psDes3Key_t;

/*
	A block cipher CBC structure
 */
typedef struct {
	int				blocklen;
	unsigned char		IV[8];
	psDes3Key_t			key;
	int				explicitIV; /* 1 if yes */
} des3_CBC;

/******************************************************************************/

/******************************************************************************/
/*	Universal types and defines */
/******************************************************************************/
#define MAXBLOCKSIZE	24

typedef union {
	des3_CBC	des3;
} psCipherContext_t;

#define byte(x, n) (((x) >> (8 * (n))) & 255)

#endif /* _h_PS_SYMMETRIC */
/******************************************************************************/
