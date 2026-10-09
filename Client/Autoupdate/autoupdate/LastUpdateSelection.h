/**********************************************************************
** Description : 上次选择的升级目录选项（服务器目录名和应用程序名字）
** FileName    : LastUpdateSelection.h
** Author      : wangbin
** Datetime    : 2004-05-28 11:40
** Comment     : 把上次选择的升级目录选项持久化同步到ini文件中
**********************************************************************/
// LastUpdateSelection.h: interface for the CLastUpdateSelection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LASTUPDATESELECTION_H__1BD77640_CEF2_44F6_91D2_92A9D2E188BA__INCLUDED_)
#define AFX_LASTUPDATESELECTION_H__1BD77640_CEF2_44F6_91D2_92A9D2E188BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using namespace std;

#define INIFILE_UPDATEDIR			"updatedir2.ini"	// 保存目前升级目录的INI文件
#define INIFILE_UPDATEDIR_KEY_DIR	"Dir"				// 升级目录键名
#define INIFILE_UPDATEDIR_KEY_APP	"App"				// 升级程序键名
#define INIFILE_UPDATEDIR_DIR		"Update"			// 升级目录ini文件所在目录
#define INIFILE_UPDATEDIR_SECTION	"UpdateDir"			// 升级目录区块名字

class CLastUpdateSelection  
{
public:
	CLastUpdateSelection();
	~CLastUpdateSelection();
	//*********************************************************************
	// function : 初始化，设置ini文件所在的根路径
	//*********************************************************************
	void Initialize(LPCSTR pcszPath);
	//*********************************************************************
	// function : 读取上次选择的目录和程序
	//*********************************************************************
	void GetLastSelection(string &strDir, string &strApp);
	//*********************************************************************
	// function : 设置上次选择的目录和程序
	//*********************************************************************
	void SetLastSelection(const string &strDir, const string &strApp);
	//*********************************************************************
	// function : 删除上次选择的目录和程序
	//*********************************************************************
	void DelLastSelection();
	//*********************************************************************
	// function : 重置，清空m_strIniFile
	//*********************************************************************
	void Reset();
private:
	string m_strIniFile;
};

#endif // !defined(AFX_LASTUPDATESELECTION_H__1BD77640_CEF2_44F6_91D2_92A9D2E188BA__INCLUDED_)
