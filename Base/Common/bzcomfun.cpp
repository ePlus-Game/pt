// bzcomfun.cpp: Base functions for blaze game studio
// Version: 1.0
// by Cooler 2003-11-07 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#include "bzcomfun.h"


// Write 16-bit integer to memory
// Note: none null pointer check
//		 high bytes lays ahead
void mwrite16(CHAR *p, WORD v)
{
	BYTE b[2];

	b[1] = (BYTE) (v >> 0);
	b[0] = (BYTE) (v >> 8);

	p[0] = b[0];
	p[1] = b[1];
}

// Read 16-bit integer from memory
// Note: none null pointer check
//		 high bytes lays ahead
WORD mread16(CONST CHAR *p)
{
	WORD v;

	v  = (WORD)((BYTE)p[1]) <<  0;
	v |= (WORD)((BYTE)p[0]) <<  8;
	
	return v;
}

// Write 32-bit integer to memory
// Note: none null pointer check
//		 high bytes lays ahead
void mwrite32(CHAR *p, DWORD v)
{
	BYTE b[4];

	b[3] = (BYTE) (v >> 0);
	b[2] = (BYTE) (v >> 8);
	b[1] = (BYTE) (v >> 16);
	b[0] = (BYTE) (v >> 24);

	p[0] = b[0];
	p[1] = b[1];
	p[2] = b[2];
	p[3] = b[3];
}

// Read 32-bit integer from memory
// Note: none null pointer check
//		 high bytes lays ahead
DWORD mread32(CONST CHAR *p)
{
	DWORD v;

	v  = (DWORD)((BYTE)p[3]) <<  0;
	v |= (DWORD)((BYTE)p[2]) <<  8;
	v |= (DWORD)((BYTE)p[1]) <<  16;
	v |= (DWORD)((BYTE)p[0]) <<  24;
	
	return v;
}

// Generate MD5 summary code
BOOL gensummary(CONST CHAR *in, DWORD inlen, CHAR *out)
{
	if(in == NULL || out == NULL)
	{
		return FALSE;
	}

	MD5_CTX tagContext;
	memset(&tagContext, 0, sizeof(MD5_CTX));

	MD5Init(&tagContext);
	MD5Update(&tagContext, (BYTE *)in, inlen);
	MD5Final((BYTE *)out, &tagContext);

	return TRUE;
}

// Blaze compress funtion
// Note: at first dwOutDataLen indicate pOutData buffer size
//		 at end dwOutDataLen indicate pOutData data size
int bzcompress(LPCSTR pcInData, DWORD dwInDataLen, 
			   LPSTR pOutData, DWORD &dwOutDataLen)
{
	if(dwInDataLen >= 1024*1024L || 
		lzo_init() != LZO_E_OK || 
		dwInDataLen+8 > dwOutDataLen)
	{
		return -1;
	}

	if(dwInDataLen == 0)
	{
		return 0;
	}

	LPSTR pWrkmem = (LPSTR)malloc(LZO1X_1_MEM_COMPRESS);
	if(!pWrkmem)
	{
		return -1;
	}
	
	memset(pWrkmem, 0, LZO1X_1_MEM_COMPRESS);
	
	lzo_uint out_Len = 0;
	int nRet = lzo1x_1_compress((BYTE*)pcInData, dwInDataLen, 
		(BYTE*)pOutData+4, &out_Len, pWrkmem);
	mwrite32(pOutData, dwInDataLen);
	free(pWrkmem);
	if(nRet == LZO_E_OK)
	{
		dwOutDataLen = out_Len+4;
		return dwOutDataLen;
	}
	else
	{
		dwOutDataLen = 0;
		return -1;
	}
}

// Blaze decompress function
// Note: at first dwOutDataLen indicate pOutData buffer size
//		 at end dwOutDataLen indicate pOutData data size
int bzdecompress(LPSTR pcInData, DWORD dwInDataLen, 
				 LPSTR pOutData, DWORD &dwOutDataLen)
{
	if(dwInDataLen >= 1024*1024L || 
		lzo_init() != LZO_E_OK || 
		dwInDataLen > dwOutDataLen)
	{
		return -1;
	}

	if(dwInDataLen == 0)
	{
		return 0;
	}

	lzo_uint out_Len = 0;
	DWORD dwNeedBufLen = mread32(pcInData);
	if(dwOutDataLen < dwNeedBufLen)
	{
		return -1;
	}
	int nRet = lzo1x_decompress((BYTE*)pcInData+4, dwInDataLen-4, 
		(BYTE*)pOutData, &out_Len, NULL);

	if(nRet == LZO_E_OK)
	{
		dwOutDataLen = out_Len;
		return out_Len;
	}
	else
	{
		dwOutDataLen = 0;
		return -1;
	}
}

#ifndef WIN32 /* LINUX */

char* strlwr(char* string)
{
	char* cp;

	for(cp = string; *cp; ++cp)
	{
		if('A' <= *cp && *cp <= 'Z')
		{
			*cp += 'a' - 'A';
		}
	}

	return string;
}

char* strupr(char* string)
{
	char * cp;

	for(cp = string; *cp; ++cp)
	{
		if('a' <= *cp && *cp <= 'z')
		{
			*cp += 'A' - 'a';
		}
	}

	return string;
}

void xtoa(unsigned long val, char *buf, 
		  unsigned radix, int is_neg)
{
	char *p;                /* pointer to traverse string */
	char *firstdig;         /* pointer to first digit */
	char temp;              /* temp char */
	unsigned digval;        /* value of digit */

	p = buf;

	if (is_neg)
	{
		/* negative, so output '-' and negate */
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	firstdig = p;           /* save pointer to first digit */

	do
	{
		digval = (unsigned) (val % radix);
		val /= radix;       /* get next digit */

		/* convert to ascii and store */
		if (digval > 9)
		{
			*p++ = (char) (digval - 10 + 'a');  /* a letter */
		}
		else
		{
			*p++ = (char) (digval + '0');       /* a digit */
		}
	}while (val > 0);

	/* We now have the digit of the number in the buffer, but in reverse
	   order.  Thus we reverse them now. */

	*p-- = '\0';            /* terminate string; p points to last digit */

	do
	{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;   /* swap *p and *firstdig */
		--p;
		++firstdig;         /* advance to next two digits */
	}while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char* itoa(int val, char *buf, int radix)
{
	if(radix == 10 && val < 0)
	{
		xtoa((unsigned long)val, buf, radix, 1);
	}
	else
	{
		xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
	}

	return buf;
}

#endif /* LINUX */

// Convert hex string to int
int hexatoi(LPCSTR pcstrHex)
{
	if(pcstrHex == NULL)
	{
		return -1;
	}

	int iResult = 0;
	int iCurIndex = strlen(pcstrHex)-1;
	int iCurWeigh = 1;

	char szCurChar[2] = {0};
	while(iCurIndex >= 0)
	{
		szCurChar[0] = pcstrHex[iCurIndex];
		strlwr(szCurChar);
		if(isdigit(szCurChar[0]))
		{
			int iCurValue = 0;
			iCurValue = szCurChar[0] - '0';
			iCurValue *= iCurWeigh;
			iResult += iCurValue;
		}
		else if(isalpha(szCurChar[0]))
		{
			if(szCurChar[0]-'a' >= 0 && 
				szCurChar[0]-'f' <= 0)
			{
				int iCurValue = 0;
				iCurValue = 10 + szCurChar[0]-'a';
				iCurValue *= iCurWeigh;
				iResult += iCurValue;
			}
			else
			{
				break;
				iResult = -1;
			}
		}
		else
		{
			break;
			iResult = -1;
		}

		iCurIndex--;
		if(iCurWeigh == 1)
			iCurWeigh = 16;
		else
			iCurWeigh *= 16;
	}

	return iResult;
}
