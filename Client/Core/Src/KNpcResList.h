//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcResList.h
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------

#pragma once

#ifndef _SERVER

#include "KList.h"
#include "KNpcResNode.h"

#define  INVALID_ACTION_NO	0xFFFFFFFF
// 处理动作的种类名称
class CActionName
{
public:
	CActionName();
	~CActionName();
	// 获取动作种类、名称等信息
	BOOL Init(const char *lpszFileName);
	// 由动作名称得到动作编号
	unsigned int GetActionNo(const char *lpszName) const;
	// 得到动作种类数
	unsigned int GetActionCount() const;
	// 由动作编号得到动作名称
	BOOL GetActionName(unsigned int nNo,char *lpszName, int nSize) const;
private:
	typedef char SprFileName[FILE_NAME_LENGTH];
	SprFileName *m_szNames;	// 动作名称
	unsigned int m_nCurActionNo;				// 动作种类
};

class KNpcResList
{
private:
	CActionName m_cActionName;
	CActionName m_cNpcAction;
	KNpcResNode* m_cNpcRes;
	unsigned int m_nNpcResCount;
public:
	CStateMagicTable		m_cStateTable;
public:
    KNpcResList();
    ~KNpcResList();
	
    // 初始化 ActionName
	BOOL Init();

	unsigned int GetCount(void) const;
	const KNpcResNode* GetNpcRes(const char *lpszNpcName) const;
	const KNpcResNode* GetNpcRes(unsigned int nIndex) const;
};
#ifndef _SERVER
extern KNpcResList	g_NpcResList;
#endif

#endif