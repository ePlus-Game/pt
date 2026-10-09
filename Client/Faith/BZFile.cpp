// BZFile.cpp: implementation of the CBZFile class.
// Version 1.0
// by Cooler 2003.11.11 liuyujun@263.net
// Crossing platform (Win32/Linux)
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "BZFile.h"

CBZFile::CBZFile()
{
	m_pFOpen = NULL;
}

CBZFile::~CBZFile()
{
	Close();
}

BOOL CBZFile::Open(LPCSTR lpszFileName, DWORD dwOpenFlags)
{
	if(lpszFileName == NULL)
	{
		return FALSE;
	}

	char szFlags[64] = {0};
	if(dwOpenFlags & modeCreate)
	{
		strcat(szFlags, "w+");
	}
	if(dwOpenFlags & modeRead)
	{
		strcat(szFlags, "r");
	}
	if(dwOpenFlags & modeWrite)
	{
		strcat(szFlags, "w");
	}
	if(dwOpenFlags & modeReadWrite)
	{
		strcat(szFlags, "r+");
	}
	if(dwOpenFlags & modeAppend)
	{
		strcat(szFlags, "a+");
	}

	if((m_pFOpen = fopen(lpszFileName, szFlags)) == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

void CBZFile::Close()
{
	if(m_pFOpen != NULL)
	{
		fclose(m_pFOpen);
		m_pFOpen = NULL;
	}
}

long CBZFile::GetLength()
{
	if(m_pFOpen == NULL)
	{
		return -1;
	}

	long lOldPos = ftell(m_pFOpen);
	SeekToEnd();
	long lLength = ftell(m_pFOpen);
	fseek(m_pFOpen, lOldPos, SEEK_SET);

	return lLength;
}

int CBZFile::SeekToBegin()
{
	if(m_pFOpen == NULL)
	{
		return -1;
	}

	return fseek(m_pFOpen, 0, SEEK_SET);
}

int CBZFile::SeekToEnd()
{
	if(m_pFOpen == NULL)
	{
		return -1;
	}

	return fseek(m_pFOpen, 0, SEEK_END);
}

int CBZFile::SeekTo(int nOffset)
{
	if(m_pFOpen == NULL)
	{
		return -1;
	}

	return fseek(m_pFOpen, nOffset, SEEK_CUR);
}

int CBZFile::Read(LPSTR pBuf, 
				  DWORD dwBufSize)
{
	if(m_pFOpen == NULL || 
		pBuf == NULL)
	{
		return -1;
	}

	// Note: in standard C fread function, "\r\n" will read to "\n"
	return fread(pBuf, 1, dwBufSize, m_pFOpen);
}

int CBZFile::Write(LPCSTR pcBuf, 
				   DWORD dwBufSize)
{
	if(m_pFOpen == NULL || 
		pcBuf == NULL)
	{
		return -1;
	}
	
	// Note: in standard C fwrite function, "\n" will write to "\r\n"
	return fwrite(pcBuf, 1, dwBufSize, m_pFOpen);
}
