// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile() : m_iterSections(m_listSections.begin())
{

}

CIniFile::~CIniFile()
{
	Clear();
}

void CIniFile::Parse(LPCSTR pcszFileName)
{
	ASSERT(pcszFileName);
	CFile file;
	BOOL bResult = file.Open(pcszFileName, CFile::modeRead);
	ASSERT(bResult);
	if (bResult)
	{
		// 读取文件内容
		string strBulletin;
		char szBuffer[BUFFER_MAXSIZE] = {0};
		DWORD dwReadSize = file.Read(szBuffer, BUFFER_MAXSIZE - 1);
		while (dwReadSize)
		{
			szBuffer[dwReadSize] = 0;
			strBulletin.append(szBuffer);
			dwReadSize = file.Read(szBuffer, BUFFER_MAXSIZE - 1);
		}
		ASSERT(strBulletin.length() == file.GetLength());
		file.Close();
		// 显示公告文件
		Parse(strBulletin.c_str(), strBulletin.length());
	}
}

void CIniFile::Parse(LPCSTR pcszBuffer, DWORD cbBufSize)
{
	ASSERT(pcszBuffer && cbBufSize);
	Clear();
	LPSTR pszHead = (LPSTR)pcszBuffer;
	LPSTR pszTail = pszHead + cbBufSize;
	char szBuffer[BUFFER_MAXSIZE] = {0};
	CIniSection *pOldSection = NULL;
	while (pszHead < pszTail)
	{
		DWORD cbFetched = 0;
		// 复制字符串到缓冲区
		Parse(pszHead, pszTail - pszHead, szBuffer, BUFFER_MAXSIZE, &cbFetched);
		// 分析缓冲区中的字符串
		if (cbFetched != 0)
		{
			ASSERT(szBuffer[0] != 0);
			CIniSection *pNewSection = NULL;
			Parse(szBuffer, cbFetched, pOldSection, &pNewSection);
			if (pNewSection)
			{
				if (pOldSection)
					AddSection(pOldSection);
				pOldSection = pNewSection;
			}
			pszHead += cbFetched;
		}
		else
		{
			pszHead++;
		}
	}
	if (pOldSection)
		AddSection(pOldSection);
}

ULONG CIniFile::GetSectionCount()
{
	return m_listSections.size();
}

CIniSection *CIniFile::GetSection()
{
	CIniSection *pSection = NULL;
	if (m_iterSections != m_listSections.end())
		pSection = *m_iterSections;
	return pSection;
}

CIniSection *CIniFile::GetSection(LPCSTR pcszName)
{
	ASSERT(pcszName);
	CIniSection *pSection = NULL;
	map<string, CIniSection*>::const_iterator it = m_mapSections.find(pcszName);
	if (it != m_mapSections.end())
		pSection = it->second;
	return pSection;
}

void CIniFile::Reset()
{
	m_iterSections = m_listSections.begin();
}

void CIniFile::Step()
{
	if (m_iterSections != m_listSections.end())
		m_iterSections++;
}

BOOL CIniFile::Eof()
{
	return m_iterSections == m_listSections.end();
}

void CIniFile::Clear()
{
	list<CIniSection*>::const_iterator it = m_listSections.begin();
	for (; it != m_listSections.end(); it++)
	{
		CIniSection *pSection = *it;
		ASSERT(pSection);
		delete pSection;
	}
	m_listSections.clear();
	m_iterSections = m_listSections.begin();
	m_mapSections.clear();
}

//---------------------------------------------------------------------
// function : 解析缓冲区中的字符串，如果是新的区块则返回非空指针，如果是区块记录则加入pSection
//---------------------------------------------------------------------
void CIniFile::Parse(LPSTR pszBuffer, DWORD cbBuffSize, CIniSection *pCurSection, CIniSection **ppNewSection)
{
	ASSERT(pszBuffer && cbBuffSize && ppNewSection);
	if (cbBuffSize < 2)
		return;
	if (pszBuffer[0] == '[' && pszBuffer[cbBuffSize - 1] == ']')
	{
		// 区块开始
		CIniSection *pNewSection = NULL;
		if (cbBuffSize > 2)
		{
			pszBuffer[cbBuffSize - 1] = 0;
			pNewSection = new CIniSection;
			ASSERT(pNewSection);
			pNewSection->PutSectionName(&pszBuffer[1]);
			*ppNewSection = pNewSection;
		}
	}
	else if ( pCurSection != NULL )
	{
		// 记录键
		char *pszSeparator = ::strchr(pszBuffer, '=');
		if (pszSeparator != NULL && pszSeparator != pszBuffer)
		{
			*pszSeparator = 0;
			char *pszKey = Trim(pszBuffer);
			if (pszKey != NULL)
			{
				char *pszVal = Trim(pszSeparator + 1);
				if (pszVal == NULL)
					pszVal = "";
				pCurSection->AddEntry(pszKey, pszVal);
			}
		}
	}
}

//---------------------------------------------------------------------
// function	: 去除字符串首尾的空白字符，如果全是空白字符返回NULL，否则返回子字符串
//---------------------------------------------------------------------
LPSTR CIniFile::Trim(LPSTR pszMessage)
{
	ASSERT(pszMessage);
	char *pszResult = NULL;
	char *pszHead = pszMessage;
	char *pszTail = pszMessage + ::strlen(pszMessage);
	if (pszTail == pszHead)
		return pszResult;
	pszTail--;
	while (pszHead <= pszTail && IsBlank(*pszHead))
		pszHead++;
	while (pszHead <= pszTail && IsBlank(*pszTail))
		pszTail--;
	if (pszHead <= pszTail)
	{
		pszTail[1] = 0;
		if (*pszHead != 0)
			pszResult = pszHead;
	}
	return pszResult;
}

BOOL CIniFile::HasSection(LPCSTR pcszName)
{
	ASSERT(pcszName);
	return m_mapSections.find(pcszName) != m_mapSections.end();
}

//---------------------------------------------------------------------
// function : 增加区块
//---------------------------------------------------------------------
void CIniFile::AddSection(CIniSection *pSection)
{
	ASSERT(pSection);
	char szSectionName[BUFFER_MAXSIZE] = {0};
	BOOL bOK = pSection->GetSectionName(szSectionName, BUFFER_MAXSIZE) && !HasSection(szSectionName);
	ASSERT(bOK);
	if (bOK)
	{
		m_listSections.push_back(pSection);
		m_mapSections[szSectionName] = pSection;
	}
}

//---------------------------------------------------------------------
// function : 把pszMsgHead到pszMsgTail之间的第一个字符串复制到指定最大长度的缓冲区中，并返回后继指针
//---------------------------------------------------------------------
void CIniFile::Parse(LPSTR pszMessage, DWORD dwMsgSize, LPSTR pszBuffer, DWORD cbMaxSize, DWORD *pcbFetched)
{
	ASSERT(pszMessage && dwMsgSize && pszBuffer && cbMaxSize && pcbFetched);
	// 定位字符串尾部位置
	char *pszTail = pszMessage;
	char *pszNull = pszMessage + dwMsgSize;
	while (pszTail < pszNull)
	{
		char chTemp = *pszTail;
		if (chTemp == '\r' || chTemp == '\n')
			break;
		else
			pszTail++;
	}
	// 复制字符串，过短则忽略，过长则截断
	int cbSize = pszTail - pszMessage;
	if (cbSize < 2)
	{
		pszBuffer[0] = 0;
		*pcbFetched = 0;
	}
	else
	{
		if (cbSize > cbMaxSize - 1)
			cbSize = cbMaxSize - 1;
		::memcpy(pszBuffer, pszMessage, cbSize);
		pszBuffer[cbSize] = 0;
		*pcbFetched = cbSize;
	}
	ASSERT(*pcbFetched <= dwMsgSize);
}
