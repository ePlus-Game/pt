///////////////////////////////////////////////////////////////
//	File        : CoreUtil.h
//	Author      : chenshanglin
//	Create Time : 2006-2-7
//	Project     : Core_Lib
//	Platform    : Win32 & Linux
//	Remark      : Define some small utility classes
//	History     : 
///////////////////////////////////////////////////////////////
#ifndef CoreUtil_h
#define CoreUtil_h

#include "GlobalDef.h"

template<class T>
class ScopeBasicType
{
public:
	ScopeBasicType(T& ScopeVariable, T OriginalVal) 
				  : m_ScopeVariable(ScopeVariable), m_OriginalVal(OriginalVal)
	{
	}

	~ScopeBasicType()
	{
		m_ScopeVariable = m_OriginalVal;
	}

private:
	ScopeBasicType(const ScopeBasicType<T> &rhs);
	ScopeBasicType<T>& operator =(const ScopeBasicType<T> &rhs);

private:
	T& m_ScopeVariable;
	T  m_OriginalVal;
};

template<int VALUE, int LIMIT>
class ASSERT_LESS
{
private:
	char	m[VALUE < LIMIT ? 1 : -1];
};

template<int VALUE, int LIMIT>
class ASSERT_NOT_GREATER
{
private:
	char	m[VALUE <= LIMIT ? 1 : -1];
};

int	StrToIntArray(char *szStr, const char *szTag, int *pOutArray, int nCapacity);

// The following functions used to get data from db procedure return result.
bool GetDword(DWORD &dwOutVal, const char *pColData, int nSize);

void SafeCopyStr(char *pDest, int nDestSize, char *pSrc, int nSrcSize);

inline bool GetInt(int &nVal, const char *pColData, int nSize)
{
	DWORD	dwVal;
	bool	bRet = GetDword(dwVal, pColData, nSize);
	nVal	= (int)dwVal;
	return	bRet;
}

inline bool GetMemBuf(char *pOutBuf, int nBufSize, const char *pColData, int nDataSize)
{
	_ASSERT(nDataSize <= nBufSize);
	_ASSERT(NULL != pColData);

	if(nDataSize <= nBufSize && NULL != pColData)
	{
		memcpy(pOutBuf, pColData, nDataSize);
		return true;
	}
	else
		return false;
}

inline bool GetSzStr(char *pOutBuf, int nBufSize, const char *pColData, int nDataSize)
{
	_ASSERT(nDataSize < nBufSize);
	_ASSERT(NULL != pColData);

	if(nDataSize < nBufSize && NULL != pColData)
	{
		memcpy(pOutBuf, pColData, nDataSize);
		pOutBuf[nDataSize] = '\0';
		return true;
	}
	else
		return false;
}

//把颜色转化为颜色字符串
inline void ColorToString(unsigned int uColour, char* pString)
{
	if (pString == NULL)
		return;

	unsigned char byRed		= (uColour & 0x00ff0000)>>16;
	unsigned char byGreen	= (uColour & 0x0000ff00)>>8;
	unsigned char byBlue	= uColour & 0x000000ff;

	sprintf( pString, "%d,%d,%d", byRed, byGreen, byBlue );
}

//把颜色字符串转化为颜色
inline unsigned int StringToColor(LPCTSTR pString)
{
	if (pString == NULL)
		return 0;

	unsigned int Color = 0xFF000000;

	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pString[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}

inline int ComputePrice(int nOldPrice, int nDiscount)
{
	// nDiscount == 0 表示普通的地图上的商店，不考虑
	// 折扣计算
	if(nDiscount > 0)
		return nOldPrice * nDiscount / 100;
	else
		return nOldPrice;
}

inline int ComputeTax(int nOldPrice, int nDiscount)
{
	// nDiscount == 0 时表示普通地图上的商店，不考虑
	// 折扣计算
	if(nDiscount > 0 && nDiscount >= MIN_CITY_DISCOUNT)
		return nOldPrice * (nDiscount - MIN_CITY_DISCOUNT) / 100;
	else
		return 0;
}

inline bool IsBuffIdValid(int nBuffId)
{
	return nBuffId > 0;
}

#endif