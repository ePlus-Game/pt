// EASYCrypt.cpp: implementation of the CPinCrypt class.
// Version: 1.2
// by Cooler 2003-06-02 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#include "PinCrypt.h"
#include "bzcomfun.h"


CPinCrypt::CPinCrypt()
{

}

CPinCrypt::~CPinCrypt()
{

}

BOOL CPinCrypt::Encrypt(LPSTR pEncryptData, 
						 DWORD dwEncryptDataLen, 
						 DWORD dwCryptSign)
{
	if(pEncryptData == NULL)
	{
		return FALSE;
	}

	if(dwEncryptDataLen == 0)
	{
		return TRUE;
	}

	EASYCRYPTKEY tagEncryptKey;
	memset(&tagEncryptKey, 
		0, 
		sizeof(EASYCRYPTKEY));
	if(dwEncryptDataLen > 2)
	{
		// Generate encrypt key
		tagEncryptKey.szEncryptKey[0] = pEncryptData[0];
		tagEncryptKey.szEncryptKey[1] = pEncryptData[dwEncryptDataLen-1];
		tagEncryptKey.wEncryptKeyLen += 2;
		memcpy(tagEncryptKey.szEncryptKey + tagEncryptKey.wEncryptKeyLen, 
			EASYCRYPT_INITCODE, 
			EASYCRYPT_INITCODELEN);
		tagEncryptKey.wEncryptKeyLen += EASYCRYPT_INITCODELEN;
		itoa(dwCryptSign, 
			tagEncryptKey.szEncryptKey+tagEncryptKey.wEncryptKeyLen, 
			16);
		tagEncryptKey.wEncryptKeyLen += 
			strlen(tagEncryptKey.szEncryptKey + 
				tagEncryptKey.wEncryptKeyLen);

		pEncryptData[0] = ~pEncryptData[0];
		pEncryptData[dwEncryptDataLen-1] = ~pEncryptData[dwEncryptDataLen-1];
		char chSwap;
		chSwap = pEncryptData[0];
		pEncryptData[0] = pEncryptData[dwEncryptDataLen-1];
		pEncryptData[dwEncryptDataLen-1] = chSwap;

		CryptData(pEncryptData + 1, 
				dwEncryptDataLen - 2, 
				tagEncryptKey);
	}
	else
	{
		for(DWORD i=0; i<dwEncryptDataLen; i++)
			pEncryptData[i] = ~pEncryptData[i];
	}

	return TRUE;
}

BOOL CPinCrypt::Decrypt(LPSTR pDecryptData, 
						 DWORD dwDecryptDataLen, 
						 DWORD dwCryptSign)
{
	if(pDecryptData == NULL)
	{
		return FALSE;
	}

	if(dwDecryptDataLen == 0)
	{
		return TRUE;
	}

	EASYCRYPTKEY tagDecryptKey;
	memset(&tagDecryptKey, 
		0, 
		sizeof(EASYCRYPTKEY));
	if(dwDecryptDataLen > 2)
	{
		char chSwap;
		chSwap = pDecryptData[0];
		pDecryptData[0] = pDecryptData[dwDecryptDataLen-1];
		pDecryptData[dwDecryptDataLen-1] = chSwap;
		pDecryptData[0] = ~pDecryptData[0];
		pDecryptData[dwDecryptDataLen-1] = ~pDecryptData[dwDecryptDataLen-1];

		// Generate decrypt key
		tagDecryptKey.szEncryptKey[0] = pDecryptData[0];
		tagDecryptKey.szEncryptKey[1] = pDecryptData[dwDecryptDataLen-1];
		tagDecryptKey.wEncryptKeyLen += 2;
		memcpy(tagDecryptKey.szEncryptKey + tagDecryptKey.wEncryptKeyLen, 
			EASYCRYPT_INITCODE, 
			EASYCRYPT_INITCODELEN);
		tagDecryptKey.wEncryptKeyLen += EASYCRYPT_INITCODELEN;
		itoa(dwCryptSign, 
			tagDecryptKey.szEncryptKey+tagDecryptKey.wEncryptKeyLen, 
			16);
		tagDecryptKey.wEncryptKeyLen += 
			strlen(tagDecryptKey.szEncryptKey + 
				tagDecryptKey.wEncryptKeyLen);

		CryptData(pDecryptData + 1, 
				dwDecryptDataLen - 2, 
				tagDecryptKey);
	}
	else
	{
		for(DWORD i=0; i<dwDecryptDataLen; i++)
			pDecryptData[i] = ~pDecryptData[i];
	}

	return TRUE;
}

void CPinCrypt::CryptData(LPSTR pData, 
						   DWORD dwDataLen, 
						   CONST EASYCRYPTKEY tagKey)
{
	char szCryptSummary[SIZE_CRYPTSUMMARY] = {0};
	GenCryptSummary(tagKey.szEncryptKey, 
				tagKey.wEncryptKeyLen, 
				szCryptSummary);

	for(DWORD i=0; 
		i<dwDataLen; 
		i += SIZE_CRYPTSUMMARY)
	{
		DWORD dwChunkSize = 
			dwDataLen-i < SIZE_CRYPTSUMMARY ? 
			dwDataLen-i : SIZE_CRYPTSUMMARY;
		for(DWORD j=0; j<dwChunkSize; j++)
		{
			pData[i+j] = 
				pData[i+j] ^ szCryptSummary[j];
		}
	}
}

void CPinCrypt::GenCryptSummary(LPCSTR pData, 
								 DWORD dwDataLen, 
								 LPSTR pSummary)
{
	gensummary(pData, dwDataLen, pSummary);

	for(int i=0; i<SIZE_MD5SUMMARY; i++)
	{
		pSummary[i] = ~pSummary[i];
	}

	DWORD dwGroupCount = 
		SIZE_MD5SUMMARY / SIZE_CRYPTSUMMARYCHUNK;
	if(SIZE_MD5SUMMARY % SIZE_CRYPTSUMMARYCHUNK != 0)
	{
		dwGroupCount++;
	}
	
	for(DWORD n=0; n<dwGroupCount; n++)
	{
		DWORD dwSwapCount;
		if(n == dwGroupCount - 1)
		{
			dwSwapCount = 
				(SIZE_MD5SUMMARY % SIZE_CRYPTSUMMARYCHUNK) / 2;
		}
		else
		{
			dwSwapCount = SIZE_CRYPTSUMMARYCHUNK / 2;
		}

		char chSwap;
		int nLeftIndex, nRightIndex;
		for(DWORD m=0; m<dwSwapCount; m++)
		{
			nLeftIndex = n * SIZE_CRYPTSUMMARYCHUNK + m;
			nRightIndex = n * SIZE_CRYPTSUMMARYCHUNK + 
				SIZE_CRYPTSUMMARYCHUNK - m - 1;
			chSwap = pSummary[nLeftIndex];
			pSummary[nLeftIndex] = pSummary[nRightIndex];
			pSummary[nRightIndex] = chSwap;
		}
	}
}
