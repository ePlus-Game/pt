/**********************************************************************
** Description : 升级版本选择
** FileName    : EditionSelector.h
** Author      : wangbin
** Datetime    : 2004-05-15 19:40
** Comment     : 读取配置文件，选择一个升级版本
**********************************************************************/
// EditionSelector.h: interface for the CEditionSelector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITIONSELECTOR_H__5F3A6DB1_0A09_499F_AA0C_81E4626F0C91__INCLUDED_)
#define AFX_EDITIONSELECTOR_H__5F3A6DB1_0A09_499F_AA0C_81E4626F0C91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IniFile.h"
#include "DirSelectDlg.h"
#include <string>
using namespace std;

class CEditionSelector  
{
public:
	CEditionSelector(CWnd *pWnd, LPCSTR pcszIniFile);
	~CEditionSelector();
	typedef CDirSelectDlg::ListEntry ListEntry;
	//---------------------------------------------------------------------
	// function : 获取升级版本的目录和程序名字
	// return	: TRUE-成功；FALSE-失败
	//---------------------------------------------------------------------
	BOOL GetEdition(string &strDirectory, string &strApplication);
private:
	//---------------------------------------------------------------------
	// function : 从区块中中读取配置信息
	//---------------------------------------------------------------------
	BOOL GetEdition(CIniSection *pSection, ListEntry &entry);
	//---------------------------------------------------------------------
	// function : 从列表中选择升级目录
	//---------------------------------------------------------------------
	BOOL GetEdition(const list<ListEntry> &listEntries, ListEntry &entry);
	//---------------------------------------------------------------------
	// function : 读取升级版本记录
	//---------------------------------------------------------------------
	void GetEditionList(list<ListEntry> &listEntries);
private:
	enum {BUFF_MAXSIZE = 1024};	// 最大缓冲区
	string	m_strIniFile;
	CWnd   *m_pWnd;				// 创建当前类的环境窗口
};

#endif // !defined(AFX_EDITIONSELECTOR_H__5F3A6DB1_0A09_499F_AA0C_81E4626F0C91__INCLUDED_)
