// bzcomfun.h: Base functions define for blaze game studio
// Version: 1.0
// by Cooler 2003-11-07 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#ifndef _BZCOMFUN_H_
#define _BZCOMFUN_H_

#include "bzcomdef.h"
extern "C"
{
#include "md5.h"
#include "minilzo.h"
}

// Write 16-bit integer to memory
// Note: none null pointer check
//		 high bytes lays ahead
void mwrite16(CHAR *p, WORD v);

// Read 16-bit integer from memory
// Note: none null pointer check
//		 high bytes lays ahead
WORD mread16(CONST CHAR *p);

// Write 32-bit integer to memory
// Note: none null pointer check
//		 high bytes lays ahead
void mwrite32(CHAR *p, DWORD v);

// Read 32-bit integer from memory
// Note: none null pointer check
//		 high bytes lays ahead
DWORD mread32(CONST CHAR *p);


// Generate MD5 summary code
#define SIZE_MD5SUMMARY			16
BOOL gensummary(CONST CHAR *in, DWORD inlen, CHAR *out);

// Blaze compress funtion
// Note: at first dwOutDataLen indicate pOutData buffer size
//		 at end dwOutDataLen indicate pOutData data size
int bzcompress(LPCSTR pcInData, DWORD dwInDataLen, 
			   LPSTR pOutData, DWORD &dwOutDataLen);

// Blaze decompress function
// Note: at first dwOutDataLen indicate pOutData buffer size
//		 at end dwOutDataLen indicate pOutData data size
int bzdecompress(LPSTR pcInData, DWORD dwInDataLen, 
				 LPSTR pOutData, DWORD &dwOutDataLen);


#ifndef WIN32 /* LINUX */

// Some lost c functions in gcc
char* strlwr(char* string);
char* strupr(char* string);
char* itoa(int val, char *buf, int radix);

#endif /* LINUX */


// Convert hex string to int
int hexatoi(LPCSTR pcstrHex);

#endif // _BZCOMFUN_H_
