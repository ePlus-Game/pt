///////////////////////////////////////////////////////////////
//	File        : CoreUtil.cpp
//	Author      : chenshanglin
//	Create Time : 2006-2-7
//	Project     : Core_Lib
//	Platform    : Win32 & Linux
//	Remark      :
//	History     : 
///////////////////////////////////////////////////////////////
#include "KCore.h"
#include "CoreUtil.h"

int	StrToIntArray(char *szStr, const char *szTag, int *pOutArray, int nCapacity)
{
	int	nCount = 0;
	const char *pToken = strtok(szStr, szTag);
	
	while(pToken && nCount < nCapacity)
	{
		*pOutArray = atoi(pToken);
		pToken = strtok(NULL, szTag);
		++pOutArray;
		++nCount;
	}

	return nCount;
}

bool GetDword(DWORD &dwOutVal, const char *pColData, int nSize)
{
	const int	BUF_SIZE = 64;
	char		buf[BUF_SIZE];

	_ASSERT(nSize < BUF_SIZE);
	_ASSERT(NULL != pColData);

	if(nSize < BUF_SIZE && NULL != pColData)
	{
		memcpy(buf, pColData, nSize);
		buf[nSize] = '\0';
		dwOutVal = (DWORD)atoi(buf);
		return true;
	}
	else
		return false;
}

void SafeCopyStr(char *pDest, int nDestSize, char *pSrc, int nSrcSize)
{
	if(NULL != pDest && NULL != pSrc)
	{
		strncpy(pDest, pSrc, nDestSize);

		if(nSrcSize < nDestSize)
			pDest[nSrcSize] = '\0';
		else
			pDest[nDestSize - 1] = '\0';
	}
}