// EditionSelector.cpp: implementation of the CEditionSelector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "EditionSelector.h"
#include "WndCommand.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------
// macro	: INI文件键名
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
#define EDITION_NAME			"name"			// 版本名字
#define EDITION_DIR				"dir"			// 版本目录
#define EDITION_DESCRIPTION		"description"	// 版本描述
#define EDITION_APPLICATION		"application"	// 程序名字

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEditionSelector::CEditionSelector(CWnd *pWnd, LPCSTR pcszIniFile) :
	m_strIniFile(pcszIniFile), m_pWnd(pWnd)
{
	ASSERT(pWnd && pcszIniFile);
}

CEditionSelector::~CEditionSelector()
{

}

//---------------------------------------------------------------------
// function : 读取升级版本记录
//---------------------------------------------------------------------
void CEditionSelector::GetEditionList(list<ListEntry> &listEntries)
{
	ASSERT(!m_strIniFile.empty());
	CIniFile fileIni;
	fileIni.Parse(m_strIniFile.c_str());

	char szVal[BUFF_MAXSIZE] = {0};
	fileIni.Reset();
	while (!fileIni.Eof())
	{
		ListEntry entry;
		if (GetEdition(fileIni.GetSection(), entry))
		{
			listEntries.push_back(entry);
		}
		fileIni.Step();
	}
}

//---------------------------------------------------------------------
// function : 从列表中选择升级目录
//---------------------------------------------------------------------
BOOL CEditionSelector::GetEdition(const list<ListEntry> &listEntries, ListEntry &entry)
{
	ASSERT(!listEntries.empty());
	BOOL bOK = FALSE;
	CDirSelectDlg dlg(m_pWnd);
	list<ListEntry>::const_iterator it = listEntries.begin();
	for (; it != listEntries.end(); it++)
	{
		dlg.AddSelection(*it);
	}
	// 显示对话框。因为现在在非UI线程中，所以必须通过发送阻塞消息的方式来显示选择版本对话框
	UINT nResult = IDCANCEL;
	DIALOG_DOMODAL(m_pWnd, &dlg, &nResult);
	if (nResult == IDOK)
		bOK = dlg.GetSelection(entry);
	return bOK;
}

//---------------------------------------------------------------------
// function : 获取升级版本的目录和程序名字
// return	: TRUE-成功；FALSE-失败
//---------------------------------------------------------------------
BOOL CEditionSelector::GetEdition(string &strDirectory, string &strApplication)
{
	list<ListEntry> listEntries;
	GetEditionList(listEntries);
	if (listEntries.empty())
		return FALSE;

	BOOL bResult = TRUE;
	ListEntry entry;
	// 如果只有一个版本，直接返回数据；否则由用户从多个版本目录中选择
	if (listEntries.size() == 1)
	{
		entry = *listEntries.begin();
		strDirectory = entry.strDownDir;
		strApplication = entry.strApplication;
	}
	else if (GetEdition(listEntries, entry))
	{
		strDirectory = entry.strDownDir;
		strApplication = entry.strApplication;
	}
	else
	{
		bResult = FALSE;
	}
	ASSERT(!bResult || (!strDirectory.empty() && !strApplication.empty()));
	return bResult;
}

//---------------------------------------------------------------------
// function : 从区块中中读取配置信息
//---------------------------------------------------------------------
BOOL CEditionSelector::GetEdition(CIniSection *pSection, CDirSelectDlg::ListEntry &entry)
{
	ASSERT(pSection);
	struct ENTRY {LPCSTR pcszKey; string *pstrVal;} entries[] = {
		{EDITION_NAME,			&entry.strDownName},	// 名字
		{EDITION_DIR,			&entry.strDownDir},		// 目录
		{EDITION_APPLICATION,	&entry.strApplication},	// 描述
		{EDITION_DESCRIPTION,	&entry.strDescription},	// 程序名字
		{NULL, NULL}
	};
	
	BOOL bResult = TRUE;
	ENTRY *pEntry = &entries[0];
	ASSERT(pEntry);
	char szVal[BUFF_MAXSIZE] = {0};
	while (bResult && pEntry->pcszKey != NULL)
	{
		ENTRY *pCurrent = pEntry++;
		if (pSection->GetEntry(pCurrent->pcszKey, szVal, BUFF_MAXSIZE))
			*(pCurrent->pstrVal) = szVal;
		else
			bResult = FALSE;
	}
	ASSERT(bResult);
	return bResult;
}
