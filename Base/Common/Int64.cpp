// Int64.cpp: implementation of the CUInt64 class.
// UInt64
// by Cooler liuyujun@263.net 2004.10.07
//////////////////////////////////////////////////////////////////////

#include "Int64.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUInt64::CUInt64()
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = 0;
}

CUInt64::CUInt64(unsigned char nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = nValue;
}

CUInt64::CUInt64(unsigned short nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = nValue;
}

CUInt64::CUInt64(unsigned int nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = nValue;
}

CUInt64::CUInt64(unsigned long nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = nValue;
}

CUInt64::CUInt64(char nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = (DWORD)nValue;
}

CUInt64::CUInt64(short nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = (DWORD)nValue;
}

CUInt64::CUInt64(int nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = (DWORD)nValue;
}

CUInt64::CUInt64(long nValue)
{
	m_tagDDWORD.dwHighPart = 0;
	m_tagDDWORD.dwLowPart = (DWORD)nValue;
}

CUInt64::CUInt64(const CUInt64 &nValue)
{
	m_tagDDWORD = nValue.m_tagDDWORD;
}

CUInt64::~CUInt64()
{

}

CUInt64& CUInt64::operator+=(const CUInt64& nValue)
{
	int nCarry = 0;

	DWORD dwTemp = 0;
	dwTemp = m_tagDDWORD.dwLowPart + 
		nValue.m_tagDDWORD.dwLowPart;
	if(dwTemp < m_tagDDWORD.dwLowPart || 
		dwTemp < nValue.m_tagDDWORD.dwLowPart)
	{
		nCarry = 1;
	}
	
	m_tagDDWORD.dwLowPart = dwTemp;

	m_tagDDWORD.dwHighPart += 
		nValue.m_tagDDWORD.dwHighPart + nCarry;

	return *this;
}

CUInt64& CUInt64::operator-=(const CUInt64& nValue)
{
	int nBorrow = 0;

	if(m_tagDDWORD.dwLowPart < nValue.m_tagDDWORD.dwLowPart)
	{
		m_tagDDWORD.dwLowPart = 0xFFFFFFFF - 
			(nValue.m_tagDDWORD.dwLowPart - m_tagDDWORD.dwLowPart);

		nBorrow = 1;
	}
	else
	{
		m_tagDDWORD.dwLowPart -= nValue.m_tagDDWORD.dwLowPart;
	}

	m_tagDDWORD.dwHighPart -= nValue.m_tagDDWORD.dwHighPart + nBorrow;

	return *this;
}

int operator<(const CUInt64& nLeftValue, const CUInt64& nRightValue)
{
	if(nLeftValue.m_tagDDWORD.dwHighPart < 
		nRightValue.m_tagDDWORD.dwHighPart)
	{
		return 1;
	}
	else if(nLeftValue.m_tagDDWORD.dwHighPart > 
		nRightValue.m_tagDDWORD.dwHighPart)
	{
		return 0;
	}
	else
	{
		if(nLeftValue.m_tagDDWORD.dwLowPart < 
			nRightValue.m_tagDDWORD.dwLowPart)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return 0;
}

int operator>(const CUInt64& nLeftValue, const CUInt64& nRightValue)
{
	if(nLeftValue.m_tagDDWORD.dwHighPart > 
		nRightValue.m_tagDDWORD.dwHighPart)
	{
		return 1;
	}
	else if(nLeftValue.m_tagDDWORD.dwHighPart < 
		nRightValue.m_tagDDWORD.dwHighPart)
	{
		return 0;
	}
	else
	{
		if(nLeftValue.m_tagDDWORD.dwLowPart > 
			nRightValue.m_tagDDWORD.dwLowPart)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return 0;
}
