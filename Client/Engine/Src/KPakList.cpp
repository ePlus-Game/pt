//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KPakList.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Pack Data List Class
//---------------------------------------------------------------------------
#include <windows.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KFilePath.h"
#include "KIniFile.h"
#include "KPakList.h"
#include "crtdbg.h"
// add by chenshanglin on 2005-11-1 for multiple client
#include <io.h>
// add end

//---------------------------------------------------------------------------
ENGINE_API KPakList* g_pPakList = NULL;

//---------------------------------------------------------------------------
// 功能:	购造函数
//---------------------------------------------------------------------------
KPakList::KPakList()
{
	g_pPakList = this;
	m_nPakNumber = 0;
}

//---------------------------------------------------------------------------
// 功能:	分造函数
//---------------------------------------------------------------------------
KPakList::~KPakList()
{
	Close();
}

//---------------------------------------------------------------------------
// 功能:	关闭所有文件
//---------------------------------------------------------------------------
void KPakList::Close()
{
	for (int i = 0; i < m_nPakNumber; i++)
		delete m_PakFilePtrList[i];
	m_nPakNumber = 0;

	XPackFile::Terminate();
}

//---------------------------------------------------------------------------
// 功能:	在所有包中扫描指定文件
// 参数:	uId			文件名ID
//			ElemRef		用于存放（传出）文件信息
// 返回:	是否成功找到
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(unsigned long uId, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	for (int i = 0; i < m_nPakNumber; i++)
	{
		if (m_PakFilePtrList[i]->FindElemFile(uId, ElemRef))
		{
			bFounded = true;
			break;
		}
	}
	return bFounded;
}

//---------------------------------------------------------------------------
// 功能:	把文件名转换为包中的id
// 参数:	pszFileName	文件名
// 返回:	文件名对应的包中的id
//---------------------------------------------------------------------------
unsigned long KPakList::FileNameToId(const char* pszFileName)
{
	_ASSERT(pszFileName && pszFileName[0]);
	unsigned long id = 0;
	const char *ptr = pszFileName;
	int index = 0;
	while(*ptr)
	{
		if(*ptr >= 'A' && *ptr <= 'Z') id = (id + (++index) * (*ptr + 'a' - 'A')) % 0x8000000b * 0xffffffef;
		else id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		ptr++;
	}
	return (id ^ 0x12345678);
}

//---------------------------------------------------------------------------
// 功能:	在所有包中扫描指定文件
// 参数:	pszFileName	文件名
//			ElemRef	用于存放（传出）文件信息
// 返回:	是否成功找到
//---------------------------------------------------------------------------
bool KPakList::FindElemFile(const char* pszFileName, XPackElemFileRef& ElemRef)
{
	bool bFounded = false;
	if (pszFileName && pszFileName[0])
	{
		char szPackName[128];
		#ifdef WIN32
			szPackName[0] = '\\';
		#else
			szPackName[0] = '/';
		#endif
		g_GetPackPath(szPackName + 1, (char*)pszFileName);
		unsigned long uId = FileNameToId(szPackName);
		bFounded = FindElemFile(uId, ElemRef);
	}
	return bFounded;
}

//--------------------------------------------------------------------
// 功能:	Open package ini file
// 参数:	char* filename
// 返回:	BOOL
//---------------------------------------------------------------------------
bool KPakList::Open(const char* pPakListFile)
{
	// changed by chenshanglin for multiple client on 2005-11-1

	if(pPakListFile == NULL)
	{
		return false;
	}
	
	Close();

	if (XPackFile::Initialize() == false)
		return false;

	bool bSubGameDir = false;				// 是否为游戏子目录，即体服目录
	char szMainPath[MAX_PATH] = { 0 };		// 游戏主目录，外网的主目录
	char szSubPath[MAX_PATH] = { 0 };		// 游戏子目录，即体服的目录名
	char szTempBuffer[MAX_PATH] = { 0 };

	g_GetRootPath(szSubPath);

	char *pLast = strrchr(szSubPath, '\\');

	if(pLast == NULL)
	{
		pLast = strchr(szSubPath, '/');
	}
	if(pLast == NULL)
	{
		//return false;
		pLast = szSubPath;
	}
			
	strncpy(szMainPath, szSubPath, pLast - szSubPath);

	strncpy(szTempBuffer, szSubPath, MAX_PATH);
	if(pPakListFile[0] != '/' && pPakListFile[0] != '\\')
	{
		strncat(szTempBuffer, "/", MAX_PATH);
	}
	strncat(szTempBuffer, pPakListFile, MAX_PATH);
	
	// 如果当前目下不存在 "package.ini"，且上一级目录里有
	// 那么我们就认为该目录是游戏子目录
	int nTmp;
	if((nTmp = _access(szTempBuffer, 0)) == -1)
	{
		memset(szTempBuffer, 0, sizeof(szTempBuffer));
		strncpy(szTempBuffer, szMainPath, MAX_PATH);
		if(pPakListFile[0] != '/' && pPakListFile[0] != '\\')
		{
			strncat(szTempBuffer, "/", MAX_PATH);
		}
		
		strncat(szTempBuffer, pPakListFile, MAX_PATH);

		if((nTmp = _access(szTempBuffer, 0)) == -1)
		{
			return false;
		}
		else
		{
			bSubGameDir = true;
		}
	}

	if(!bSubGameDir)
	{
		memset(szMainPath, 0, sizeof(szMainPath));
		strncpy(szMainPath, szSubPath, MAX_PATH);
	}

	KIniFile IniFile;
	#define	SECTION "Package"

	bool bResult = false;

	if (IniFile.Load(szTempBuffer))
	{
		char	szBuffer[MAX_PAK][32] = { 0 }, szKey[16] = { 0 }, szFile[MAX_PATH] = { 0 };

		if (IniFile.GetString(SECTION, "Path", "", szTempBuffer, sizeof(szTempBuffer)))
		{
			int nPak, i, nNameStartPos;

			for (nPak = 0; nPak < MAX_PAK; nPak++)
			{
				itoa(nPak, szKey, 10);
				if (!IniFile.GetString(SECTION, szKey, "", szBuffer[nPak], sizeof(szBuffer[nPak])))
					break;
				if (szBuffer[nPak][0] == 0)
					break;
			}

			// 如果当前目录是体服目录，那么先读取当前目下的pak包，然后再读取上一级目录的pak包
			if(bSubGameDir)
			{
				memset(szFile, 0, sizeof(szFile));
				strncpy(szFile, szSubPath, MAX_PATH);
				if(szTempBuffer[0] != '/' && szTempBuffer[0] != '\\')
				{
					strncat(szFile, "/", MAX_PATH);
				}
				strncat(szFile, szTempBuffer, MAX_PATH);
				strncat(szFile, "/", MAX_PATH);
				
				nNameStartPos = strlen(szFile);

				for(i = 0; i < nPak; ++i)
				{
					// 子目录中并不是都有package.ini中的包
					strcpy(szFile + nNameStartPos, szBuffer[i]);
					if(_access(szFile, 0) != -1)
					{
						OpenPakFile(szFile);
					}
				}
			}

			memset(szFile, 0, sizeof(szFile));
			strncpy(szFile, szMainPath, MAX_PATH);
			if(szTempBuffer[0] != '/' && szTempBuffer[0] != '\\')
			{
				strncat(szFile, "/", MAX_PATH);
			}
			strncat(szFile, szTempBuffer, MAX_PATH);
			strncat(szFile, "/", MAX_PATH);
			nNameStartPos = strlen(szFile);
			
			for(i = 0; i < nPak; ++i)
			{
				strcpy(szFile + nNameStartPos, szBuffer[i]);
				OpenPakFile(szFile);
			}
			
			bResult = true;
		}
	}
	
	return bResult;

/*
	Close();

	if (XPackFile::Initialize() == false)
		return false;

	KIniFile IniFile;
	#define	SECTION "Package"

	bool bResult = false;
	if (IniFile.Load(pPakListFile))
	{
		char	szBuffer[32], szKey[16], szFile[MAX_PATH];

		if (IniFile.GetString(SECTION, "Path", "", szBuffer, sizeof(szBuffer)))
		{
			g_GetFullPath(szFile, szBuffer);
			int nNameStartPos = strlen(szFile);
			if (szFile[nNameStartPos - 1] != '\\' || szFile[nNameStartPos - 1] != '/')
			{
				#ifdef WIN32
					szFile[nNameStartPos++] = '\\';
				#else
					szFile[nNameStartPos++] = '/';
				#endif
				szFile[nNameStartPos] = 0;
			}

			for (int i = 0; i < MAX_PAK; i++)
			{
				itoa(i, szKey, 10);
				if (!IniFile.GetString(SECTION, szKey, "", szBuffer, sizeof(szBuffer)))
					break;
				if (szBuffer[0] == 0)
					break;
				strcpy(szFile + nNameStartPos, szBuffer);
				m_PakFilePtrList[m_nPakNumber] = new XPackFile;
				if (m_PakFilePtrList[m_nPakNumber])
				{
					if (m_PakFilePtrList[m_nPakNumber]->Open(szFile, m_nPakNumber))
					{
						m_nPakNumber++;
						g_DebugLog("PakList Open : %s ... Ok", szFile);
					}
					else
					{
						delete (m_PakFilePtrList[m_nPakNumber]);
					}
				}
			}
			bResult = true;
		}
	}
	return bResult;
*/
}

//读取包内的子文件
int KPakList::ElemFileRead(XPackElemFileRef& ElemRef,
					void* pBuffer, unsigned uSize)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[ElemRef.nPackIndex]->ElemFileRead(ElemRef, pBuffer, uSize);
	return 0;
}

//读取spr文件头部或整个spr
SPRHEAD* KPakList::GetSprHeader(XPackElemFileRef& ElemRef, SPROFFS*& pOffsetTable)
{
	if (ElemRef.nPackIndex >= 0 && ElemRef.nPackIndex < m_nPakNumber)
		return (m_PakFilePtrList[ElemRef.nPackIndex]->GetSprHeader(ElemRef, pOffsetTable));
	return NULL;
}

//读取按帧压缩的spr的一帧的数据
SPRFRAME* KPakList::GetSprFrame(int nPackIndex, SPRHEAD* pSprHeader, int nFrame)
{
	if (nPackIndex >= 0 && nPackIndex < m_nPakNumber)
		return m_PakFilePtrList[nPackIndex]->GetSprFrame(pSprHeader, nFrame);
	return NULL;
}

// add by chenshanglin for multiple client on 2005-11-1
void KPakList::OpenPakFile(const char *szFile)
{
	m_PakFilePtrList[m_nPakNumber] = new XPackFile;
	if (m_PakFilePtrList[m_nPakNumber])
	{
		if (m_PakFilePtrList[m_nPakNumber]->Open(szFile, m_nPakNumber))
		{
			m_nPakNumber++;
			g_DebugLog("PakList Open : %s ... Ok", szFile);
		}
		else
		{
			delete (m_PakFilePtrList[m_nPakNumber]);
		}
	}	
}