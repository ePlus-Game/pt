// IniSection.cpp: implementation of the CIniSection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "IniSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniSection::CIniSection() : m_iterRecords(m_listRecords.begin())
{

}

CIniSection::~CIniSection()
{
	Clear();
}

ULONG CIniSection::GetEntryCount()
{
	return m_mapRecords.size();
}

BOOL CIniSection::HasEntry(LPCSTR pcszKey)
{
	ASSERT(pcszKey);
	return m_mapRecords.find(pcszKey) != m_mapRecords.end();
}

BOOL CIniSection::GetEntry(LPCSTR pcszKey, LPSTR pszVal, DWORD cbMaxSize)
{
	ASSERT(pcszKey && pszVal && cbMaxSize);
	BOOL bResult = FALSE;
	map<string, RECORD*>::const_iterator it = m_mapRecords.find(pcszKey);
	if (it != m_mapRecords.end())
	{
		RECORD *pRecord = it->second;
		ASSERT(pRecord);
		DWORD cbBufSize = ::strlen(pRecord->strVal.c_str()) + 1;
		if (cbBufSize > cbMaxSize)
			cbBufSize = cbMaxSize;
		::strncpy(pszVal, pRecord->strVal.c_str(), cbBufSize);
		bResult = TRUE;
	}
	return bResult;
}

BOOL CIniSection::GetEntry(LPSTR pszKey, DWORD cbKeyMaxSize, LPSTR pszVal, DWORD cbValMaxSize)
{
	ASSERT(pszKey && cbKeyMaxSize);
	ASSERT(pszVal && cbValMaxSize);
	BOOL bResult = FALSE;
	if (m_iterRecords != m_listRecords.end())
	{
		RECORD *pRecord = *m_iterRecords;
		ASSERT(pRecord);
		DWORD cbKeyBufSize = ::strlen(pRecord->strKey.c_str()) + 1;
		if (cbKeyBufSize > cbKeyMaxSize)
			cbKeyBufSize = cbKeyMaxSize;
		DWORD cbValBufSize = ::strlen(pRecord->strVal.c_str()) + 1;
		if (cbValBufSize > cbKeyMaxSize)
			cbValBufSize = cbKeyMaxSize;
		::strncpy(pszKey, pRecord->strKey.c_str(), cbKeyBufSize);
		::strncpy(pszVal, pRecord->strVal.c_str(), cbValBufSize);

		bResult = TRUE;
	}
	return bResult;
}

void CIniSection::Step()
{
	if (m_iterRecords != m_listRecords.end())
		m_iterRecords++;
}

BOOL CIniSection::Eof()
{
	return m_iterRecords == m_listRecords.end();
}

void CIniSection::Reset()
{
	m_iterRecords = m_listRecords.begin();
}

BOOL CIniSection::AddEntry(LPCSTR pcszKey, LPCSTR pcszVal)
{
	BOOL bResult = FALSE;
	if (m_mapRecords.find(pcszKey) == m_mapRecords.end())
	{
		RECORD *pRecord = new RECORD(pcszKey, pcszVal);
		ASSERT(pRecord);
		m_mapRecords[pcszKey] = pRecord;
		m_listRecords.push_back(pRecord);
		bResult = TRUE;
	}
	return bResult;
}

BOOL CIniSection::GetSectionName(LPSTR pszSectionName, DWORD cbMaxSize)
{
	if (m_strSection.empty())
		return FALSE;
	DWORD cbBufSize = ::strlen(m_strSection.c_str()) + 1;
	if (cbMaxSize < cbBufSize)
		cbBufSize = cbMaxSize;
	::strncpy(pszSectionName, m_strSection.c_str(), cbBufSize - 1);
	return TRUE;
}

void CIniSection::PutSectionName(LPCSTR pcszSectionName)
{
	ASSERT(pcszSectionName);
	m_strSection = pcszSectionName;
}

void CIniSection::Clear()
{
	list<RECORD*>::const_iterator it = m_listRecords.begin();
	for (; it != m_listRecords.end(); it++)
	{
		RECORD *pRecord = *it;
		ASSERT(pRecord);
		if ( pRecord != NULL )
		{
			delete pRecord;
		}
	}
	m_listRecords.clear();
	m_mapRecords.clear();
	m_iterRecords = m_listRecords.begin();
	m_strSection = "";
}

void CIniSection::Clone(CIniSection *pSection) const
{
	ASSERT(pSection);
	pSection->Clear();
	pSection->m_strSection = m_strSection;	// 区块名字
	list<RECORD*>::const_iterator it = m_listRecords.begin();
	for (; it != m_listRecords.end(); it++)
	{
		RECORD *pRecord = *it;
		ASSERT(pRecord);
		RECORD *pClone = new RECORD(pRecord->strKey.c_str(), pRecord->strVal.c_str());
		ASSERT(pClone && !pClone->strKey.empty());
		pSection->m_listRecords.push_back(pClone);
		pSection->m_mapRecords[pClone->strKey.c_str()] = pClone;
	}
	// 定位迭代器
	it = m_listRecords.begin();
	pSection->m_iterRecords = pSection->m_listRecords.begin();
	while (it != m_listRecords.end())
	{
		it++;
		pSection->m_iterRecords++;
	}
}
