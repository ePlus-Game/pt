// LastUpdateSelection.cpp: implementation of the CLastUpdateSelection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "LastUpdateSelection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLastUpdateSelection::CLastUpdateSelection()
{
}

CLastUpdateSelection::~CLastUpdateSelection()
{

}

//*********************************************************************
// function : 初始化，设置ini文件所在的根路径
//*********************************************************************
void CLastUpdateSelection::Initialize(LPCSTR pcszPath)
{
	ASSERT(pcszPath && m_strIniFile.empty() && ::strlen(pcszPath));
	m_strIniFile = pcszPath;
	if (pcszPath[::strlen(pcszPath) - 1] != '\\')
		m_strIniFile.append("\\");
	m_strIniFile.append(INIFILE_UPDATEDIR_DIR);	
	m_strIniFile.append("\\");
	m_strIniFile.append(INIFILE_UPDATEDIR);
}

//*********************************************************************
// function : 读取上次选择的目录和程序
//*********************************************************************
void CLastUpdateSelection::GetLastSelection(string &strDir, string &strApp)
{
	ASSERT(!m_strIniFile.empty());
	char szPath[MAX_PATH] = {0};
	GetPrivateProfileString(INIFILE_UPDATEDIR_SECTION, INIFILE_UPDATEDIR_KEY_DIR, "", szPath, MAX_PATH, m_strIniFile.c_str());
	strDir = szPath;
	GetPrivateProfileString(INIFILE_UPDATEDIR_SECTION, INIFILE_UPDATEDIR_KEY_APP, "", szPath, MAX_PATH, m_strIniFile.c_str());
	strApp = szPath;
}

//*********************************************************************
// function : 设置上次选择的目录和程序
//*********************************************************************
void CLastUpdateSelection::SetLastSelection(const string &strDir, const string &strApp)
{
	ASSERT(!m_strIniFile.empty());
	WritePrivateProfileString(
		INIFILE_UPDATEDIR_SECTION, INIFILE_UPDATEDIR_KEY_DIR, strDir.c_str(), m_strIniFile.c_str());
	WritePrivateProfileString(
		INIFILE_UPDATEDIR_SECTION, INIFILE_UPDATEDIR_KEY_APP, strApp.c_str(), m_strIniFile.c_str());
}

//*********************************************************************
// function : 删除上次选择的目录和程序
//*********************************************************************
void CLastUpdateSelection::DelLastSelection()
{
	ASSERT(!m_strIniFile.empty());
	SetLastSelection("", "");
}

//*********************************************************************
// function : 重置，清空m_strIniFile
//*********************************************************************
void CLastUpdateSelection::Reset()
{
	m_strIniFile = "";
}
