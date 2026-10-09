/**********************************************************************
** Description : 升级过程
** FileName    : UpdateProcess.h
** Author      : wangbin
** Datetime    : 2004-05-17 11:40
** Comment     : 从网络下载升级包
**********************************************************************/
// UpdateProcess.h: interface for the CUpdateProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPDATEPROCESS_H__B69AFBB1_6287_4D21_91A8_46E5EA5437FC__INCLUDED_)
#define AFX_UPDATEPROCESS_H__B69AFBB1_6287_4D21_91A8_46E5EA5437FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../UPDATEDLL/UpdateExport.h"
//#include "../Engine/Src/KWin32.h"
//#include "../Engine/Src/KPakList.h"
#include "WndCallBack.h"
#include <AfxMt.h>
#include "LastUpdateSelection.h"
#include "Kernel.h"

class KPakList;
class CUpdateProcess  
{
private:
	CUpdateProcess();
	~CUpdateProcess();
	//*********************************************************************
	// function : 只能执行一次的初始化
	// comment	: 相当于FinalConstruct
	//*********************************************************************
	BOOL InitOnce();
public:
	//*********************************************************************
	// function : 创建对象实例
	//*********************************************************************
	static CUpdateProcess *CreateInstance();
	//*********************************************************************
	// function : 释放对象
	//*********************************************************************
	void Release()
	{ delete this; }
	//*********************************************************************
	// function : 设置是否正在运行
	//*********************************************************************
	void Activate(BOOL bActive);
	//*********************************************************************
	// function : 是否正在运行
	//*********************************************************************
	BOOL IsActive();
	//*********************************************************************
	// function : 设置回调接口
	//*********************************************************************
	void Attach(CWnd *pWnd, CWndCallBack *pCallBack)
	{
		ASSERT(pCallBack && pWnd);
		m_pCallBack = pCallBack;
		m_pOwnerWnd = pWnd;
	}
	void Run();
	//*********************************************************************
	// function : 初始化
	//*********************************************************************
	BOOL InitAutoUpdate();
	LPCSTR GetNextUpdateServer(int& nCurrent);
	//*********************************************************************
	// function : 要求进程退出
	//*********************************************************************
	void ToQuit();
	//*********************************************************************
	// function : 是否要进程退出
	//*********************************************************************
	BOOL IsToQuit();
	//*********************************************************************
	// function : 重试
	//*********************************************************************
	void Retry();
	LPCSTR GetUpdateSelf()
	{
		return m_strUpdateSelf;
	}
	//*********************************************************************
	// function : 取消升级
	// return	: void
	//*********************************************************************
	void Cancel();
	//*********************************************************************
	// function : 获取版本号
	// parameter: pnMajor 主版本号
	// parameter: pnMinor 副版本号
	// return void
	//*********************************************************************
	void GetVersion(int *pnMajor, int *pnMinor);
	//*********************************************************************
	// function	: 复位
	//*********************************************************************
	void Reset();
	//*********************************************************************
	// function : 记录日志信息
	//*********************************************************************
	void Log(LPCSTR pcszMessage);
	//*********************************************************************
	// function : 记录日志信息
	//*********************************************************************
	void Log(LPCSTR pcszPrefix, LPCSTR pcszMessage);
	//*********************************************************************
	// function : 等待线程结束
	//*********************************************************************
	BOOL Wait();
private:
	//*********************************************************************
	// function : 升级主函数，被子线程调用
	//*********************************************************************
	static DWORD WINAPI UpdateProcess(LPVOID pParam);
	//*********************************************************************
	// function : 下载服务器状态的信息文件
	//*********************************************************************
	BOOL ServerStateDownThread();
	//*********************************************************************
	// function : 下载升级服务器列表的信息文件
	//*********************************************************************
	UINT ServerDirDownThread(LPVOID p);
	//*********************************************************************
	// function : 执行升级
	//*********************************************************************
	UINT AutoUpdateDLL(LPCSTR pcszSite);
	//*********************************************************************
	// function : 删除以前升级遗留的文件
	//*********************************************************************
	void DeleteOldFiles();
	//*********************************************************************
	// function : 初始化升级选项
	//*********************************************************************
	void InitializeSelection();
	//*********************************************************************
	// function : 初始化升级选项
	// add by zuolizhi 2004/09/02
	//*********************************************************************
	void StartDownLoadBackGround();
	DWORD WINAPI  DownLoadBackGroundProc();
	unsigned long ProcessCRC(HANDLE hFile);
	bool          CheckCRC(const char * szFileName,const unsigned long dwMatchCRC);
	unsigned      CRC32(unsigned CRC, const void *pvBuf, int nLen);
	BOOL          DownLoadFromPath(const char * szPath,const char * szDestPath);
private:
	BOOL m_bActive;
	static const char szEOF[];	// 回车符
	CFile m_logFile;
	CString m_strUpdateSelf;
	CString m_strSiteList;
	CStringArray m_strHosts;
	CKernel		 m_kernel;		// 升级内核状态
	int	m_bToQuit;
	int m_nCurrentHost;
	BOOL m_bEnableRun;
	CWndCallBack		*m_pCallBack;
	CWnd				*m_pOwnerWnd;
	CCriticalSection	m_sec;					// 用来保持标志变量原子访问的临界区
	CLastUpdateSelection m_clsUpdateSelection;	// 上次选择的升级项
};

typedef int (__stdcall *UPDATEA_INIT)(KUPDATE_SETTING UpdateSetting);
typedef int (__stdcall *UPDATE_UNINIT)();
typedef int (__stdcall *UPDATE_START)();
typedef int (__stdcall *UPDATE_CANCEL)();
typedef int (__stdcall *UPDATE_NEXTIME )();
typedef bool (__stdcall *UPDATE_GETLOCK)(DWORD dwTime);
typedef void (__stdcall *UPDATE_RELEASELOCK)();

extern HMODULE			g_hModule;
extern BOOL				g_bLog;
extern BOOL				g_bExServerMode; //<------EXVERSION
extern KPakList			g_PakList;
extern UPDATEA_INIT		g_Update_Init;
extern UPDATE_UNINIT	g_Update_UnInit;
extern UPDATE_START		g_Update_Start;
extern UPDATE_START		g_Update_Cancel;
extern UPDATE_NEXTIME   g_Update_NextTime;
extern UPDATE_GETLOCK	g_Update_GetLock;
extern UPDATE_RELEASELOCK	g_Update_ReleaseLock;

#define INIFILE_UPDATE			"update.ini"		// 版本选择INI文件名
#define CONNECTION_TIMEOUT		30000				// 网络超时毫秒数
#define VERSION_CITYBATTLE		0					// 攻城战版本号

#define UPDATE_LOG(obj, message)					(obj)->Log(message);
#define UPDATE_PREFIX_LOG(obj, prefix, message)		(obj)->Log((prefix), (message));

#endif // !defined(AFX_UPDATEPROCESS_H__B69AFBB1_6287_4D21_91A8_46E5EA5437FC__INCLUDED_)
