//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcResList.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------
#include "KCore.h"

#ifndef _SERVER

#include	"CoreUseNameDef.h"
#include	"KNpcResList.h"

#ifndef _SERVER
KNpcResList	g_NpcResList;
#endif

KNpcResList::KNpcResList()
{
	m_nNpcResCount = 0;
	m_cNpcRes = NULL;
}

KNpcResList::~KNpcResList()
{
	if (m_cNpcRes != NULL)
	{
		delete[] m_cNpcRes;
	}
	m_nNpcResCount = 0;
}

//-------------------------------------------------------------------------
//	功能：	初始化
//-------------------------------------------------------------------------
BOOL	KNpcResList::Init()
{
	if ( !m_cActionName.Init(ACTION_FILE_NAME) )
		return FALSE;
	if ( !m_cNpcAction.Init(NPC_ACTION_NAME) )
		return FALSE;
	if ( !m_cStateTable.Init() )
		return FALSE;

	KTabFile KindFile;
	if (KindFile.Load(NPC_RES_KIND_FILE_NAME))
	{
		const int nCount = KindFile.GetHeight() -1;
		if (nCount > 0)
		{
			m_cNpcRes = new KNpcResNode[nCount];
			if(m_cNpcRes != NULL)
			{
				for(unsigned int i = 0 ; i < nCount ; i++)
				{
					m_cNpcRes[i].Init(KindFile,i + 2,m_cActionName,m_cNpcAction);
					//KindFile.GetString(i + 2, 1, "", m_szNames[i], sizeof(m_szNames[i]));
				}
				m_nNpcResCount = nCount;
			}
		}
	}
	
	return TRUE;
}

//-------------------------------------------------------------------------
//	功能：取得一个NpcRes节点个数
//-------------------------------------------------------------------------
unsigned int KNpcResList::GetCount(void) const
{
	return m_nNpcResCount;
}

//-------------------------------------------------------------------------
//	功能：取得一个NpcRes节点
//-------------------------------------------------------------------------
const KNpcResNode* KNpcResList::GetNpcRes(unsigned int nIndex) const
{
	if (nIndex < m_nNpcResCount)
	{
		return &m_cNpcRes[nIndex];
	}
	return NULL;
}

//-------------------------------------------------------------------------
//	功能：取得一个NpcRes节点
//-------------------------------------------------------------------------
const KNpcResNode* KNpcResList::GetNpcRes(const char *lpszNpcName) const
{
	if ( !lpszNpcName || !lpszNpcName[0])
		return NULL;

	for(unsigned int i = m_nNpcResCount ; i-- ;)
	{
		if (0 == strcmp(m_cNpcRes[i].m_szNpcName,lpszNpcName))
		{
			return &m_cNpcRes[i];
		}
	}
	
	return NULL;
}


//---------------------------- class CActionName ----------------------------
//---------------------------------------------------------------------------
// 功能:	构造函数
//---------------------------------------------------------------------------
CActionName::CActionName()
{
	m_nCurActionNo = 0;
	m_szNames = NULL;
}
//---------------------------------------------------------------------------
// 功能:	析构函数
//---------------------------------------------------------------------------
CActionName::~CActionName()
{
	if (m_szNames != NULL)
	{
		delete[] m_szNames;
	}
}
//---------------------------------------------------------------------------
// 功能:	获取动作种类、名称等信息
//---------------------------------------------------------------------------
BOOL	CActionName::Init(const char *lpszFileName)
{
	if (!lpszFileName || !lpszFileName[0])
		return FALSE;

	char szBuf[FILE_NAME_LENGTH];
	strcpy_const(szBuf,RES_INI_FILE_PATH);
	strcat(szBuf,lpszFileName);

	KTabFile	cTabFile;
	if (cTabFile.Load(szBuf) )
	{
		const int nCount = cTabFile.GetHeight() - 1;
		if (nCount > 0)
		{
			m_szNames = new SprFileName[nCount];
			if (m_szNames != NULL)
			{
				for (unsigned int i = 0; i < nCount; i++)
				{
					cTabFile.GetString(i + 2, 1, "", m_szNames[i], sizeof(m_szNames[i]));
				}
				m_nCurActionNo = nCount;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//---------------------------------------------------------------------------
// 功能:	由动作名称得到动作编号
//---------------------------------------------------------------------------
unsigned int CActionName::GetActionNo(const char *lpszName) const
{
	if ( !lpszName || !lpszName[0] )
		return INVALID_ACTION_NO;

	for (unsigned int i = 0; i < m_nCurActionNo; i++)
	{
		if (strcmp(lpszName, m_szNames[i]) == 0)
			return i;
	}
	return INVALID_ACTION_NO;
}

//---------------------------------------------------------------------------
// 功能:	得到动作种类数
//---------------------------------------------------------------------------
unsigned int CActionName::GetActionCount() const
{
	return m_nCurActionNo;
}

//---------------------------------------------------------------------------
// 功能:	由动作编号得到动作名称
//---------------------------------------------------------------------------
BOOL CActionName::GetActionName(unsigned int nNo, char *lpszName, int nSize) const
{
	if (!lpszName)
		return FALSE;

	if (nNo < 0 || nNo >= m_nCurActionNo)
		return FALSE;

	if (strlen(m_szNames[nNo]) >= (DWORD)nSize)
		return FALSE;
	
	strcpy(lpszName, m_szNames[nNo]);

	return TRUE;
}
//-------------------------- class CActionName end --------------------------


#endif