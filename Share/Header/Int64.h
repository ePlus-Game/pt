// Int64.h: interface for the CUInt64 class.
// UInt64
// Only support +,- no time to implement *,/,%
// by Cooler liuyujun@263.net 2004.10.07
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INT64_H__466F6794_BD00_4259_ACEE_D8E5B6747016__INCLUDED_)
#define AFX_INT64_H__466F6794_BD00_4259_ACEE_D8E5B6747016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KWin32.h"

// Struct define region
typedef struct _KULARGE_INTEGER
{
	DWORD dwLowPart;
	DWORD dwHighPart;
}KULARGE_INTEGER;
typedef KULARGE_INTEGER DDWORD;

class CUInt64  
{
public:
	CUInt64();
	CUInt64(unsigned char nValue);
	CUInt64(unsigned short nValue);
	CUInt64(unsigned int nValue);
	CUInt64(unsigned long nValue);
	CUInt64(char nValue);
	CUInt64(short nValue);
	CUInt64(int nValue);
	CUInt64(long nValue);
	CUInt64(double dbValue);
	CUInt64(const CUInt64 &nValue);

//-------> Ray [Luoliang] 2004-12-1
#ifdef _WIN32
	UINT64& GetUInt64Value()
	{
		return *((UINT64*)&m_tagDDWORD);
	}
	void	GetUInt64Value(UINT64 &u64Value)
	{
		u64Value = *((UINT64*)&m_tagDDWORD);
	}
#endif
//<------- End [Ray]

	virtual ~CUInt64();

	CUInt64 GetMaxCap();
	
	// Get value
	void GetDDWORDValue(DDWORD &nValue);
	DDWORD &GetDDWORDValue();
	// Set value
	void SetDDWORDValue(DDWORD nValue);
	
	inline void SetDoubleValue(double dbValue);
	inline void GetDoubleValue(double& dbValue);

	DWORD GetHighDWORD();
	DWORD GetLowDWORD();
	void SetHighDWORD(DWORD dwValue);
	void SetLowDWORD(DWORD dwValue);

	CUInt64& operator=(const CUInt64& nValue);
	CUInt64& operator+=(const CUInt64& nValue);
	CUInt64& operator-=(const CUInt64& nValue);

	// Overloading binary operators
	friend const CUInt64 operator+(const CUInt64& nLeftValue, const CUInt64& nRightValue);
	friend const CUInt64 operator-(const CUInt64& nLeftValue, const CUInt64& nRightValue);

	friend int operator==(const CUInt64& nLeftValue, const CUInt64& nRightValue);
	friend int operator!=(const CUInt64& nLeftValue, const CUInt64& nRightValue);
	friend int operator<(const CUInt64& nLeftValue, const CUInt64& nRightValue);
	friend int operator>(const CUInt64& nLeftValue, const CUInt64& nRightValue);
	friend int operator<=(const CUInt64& nLeftValue, const CUInt64& nRightValue);
	friend int operator>=(const CUInt64& nLeftValue, const CUInt64& nRightValue);

protected:
	KULARGE_INTEGER m_tagDDWORD;
};

inline void CUInt64::GetDoubleValue(double& dbValue)
{
	dbValue = (double)m_tagDDWORD.dwHighPart * ((double)0xFFFFFFFF + (double)1) + (double)m_tagDDWORD.dwLowPart;
}

inline void CUInt64::SetDoubleValue(double dbValue)
{
	if (dbValue > 0xFFFFFFFF)
	{
		m_tagDDWORD.dwHighPart = (DWORD)(dbValue / ((double)0xFFFFFFFF + (double)1));
		m_tagDDWORD.dwLowPart = (DWORD)(dbValue - (double)m_tagDDWORD.dwHighPart * ((double)0xFFFFFFFF + (double)1));
	}
	else
	{
		m_tagDDWORD.dwHighPart = 0;
		m_tagDDWORD.dwLowPart = (DWORD)(dbValue);
	}
}

inline CUInt64::CUInt64(double dbValue)
{
	SetDoubleValue(dbValue);
}

inline CUInt64 CUInt64::GetMaxCap()
{
	CUInt64 u64Max;
	u64Max.m_tagDDWORD.dwHighPart = 0xFFFFFFFF;
	u64Max.m_tagDDWORD.dwLowPart = 0xFFFFFFFF;

	return u64Max;
}

inline void CUInt64::GetDDWORDValue(DDWORD &nValue)
{
	nValue = m_tagDDWORD;
}

inline DDWORD &CUInt64::GetDDWORDValue()
{
	return m_tagDDWORD;
}

inline void CUInt64::SetDDWORDValue(DDWORD nValue)
{
	m_tagDDWORD = nValue;
}

inline CUInt64& CUInt64::operator=(const CUInt64& nValue)
{
	m_tagDDWORD = nValue.m_tagDDWORD;

	return *this;
}

inline const CUInt64 operator+(const CUInt64& nLeftValue, 
							   const CUInt64& nRightValue)
{
	CUInt64 nReturnValue = nLeftValue;

	nReturnValue += nRightValue;

	return nReturnValue;
}

inline const CUInt64 operator-(const CUInt64& nLeftValue, 
							   const CUInt64& nRightValue)
{
	CUInt64 nReturnValue = nLeftValue;

	nReturnValue -= nRightValue;

	return nReturnValue;
}

inline int operator==(const CUInt64& nLeftValue, 
					  const CUInt64& nRightValue)
{
	if(nLeftValue.m_tagDDWORD.dwHighPart == 
		nRightValue.m_tagDDWORD.dwHighPart && 
		nLeftValue.m_tagDDWORD.dwLowPart == 
		nRightValue.m_tagDDWORD.dwLowPart)
	{
		return 1;
	}

	return 0;
}

inline int operator!=(const CUInt64& nLeftValue, 
					  const CUInt64& nRightValue)
{
	if(nLeftValue.m_tagDDWORD.dwHighPart != 
		nRightValue.m_tagDDWORD.dwHighPart || 
		nLeftValue.m_tagDDWORD.dwLowPart != 
		nRightValue.m_tagDDWORD.dwLowPart)
	{
		return 1;
	}

	return 0;
}

inline int operator<=(const CUInt64& nLeftValue, 
					  const CUInt64& nRightValue)
{
	return ((nLeftValue < nRightValue) || (nLeftValue == nRightValue));
}

inline int operator>=(const CUInt64& nLeftValue, 
					  const CUInt64& nRightValue)
{
	return ((nLeftValue > nRightValue) || (nLeftValue == nRightValue));
}

inline DWORD CUInt64::GetHighDWORD()
{
	return m_tagDDWORD.dwHighPart;
}

inline DWORD CUInt64::GetLowDWORD()
{
	return m_tagDDWORD.dwLowPart;
}

inline void CUInt64::SetHighDWORD(DWORD dwValue)
{
	m_tagDDWORD.dwHighPart = dwValue;
}

inline void CUInt64::SetLowDWORD(DWORD dwValue)
{
	m_tagDDWORD.dwLowPart = dwValue;
}

#endif // !defined(AFX_INT64_H__466F6794_BD00_4259_ACEE_D8E5B6747016__INCLUDED_)
