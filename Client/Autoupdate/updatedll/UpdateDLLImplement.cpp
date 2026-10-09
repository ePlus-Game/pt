//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateDLLImplement.cpp
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-9-25 15:43:50
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UpdateDLL.h"
#include "DataDefine.h"
#include "PublicFun.h"
#include "UpdateData.h"
#include "SourceDef.h"
#include "getproxysetting.h"
#include "UpdatePublic.h"

extern KPATH_TABLE g_PathTable;

/******************************************************对外函数**************************************/
int CUpdateDLLApp::Init(const KUPDATE_SETTING& UpdateSetting)
{
	int nRetCode = 0;  
	m_nResultCode = nRetCode;

	//注册消息窗口
	if (!RegisterMyWindow())
		return m_nResultCode;
	//初始化socket
	if (!InitWinSock())
	{
		m_nResultCode = defUPDATE_RESULT_INIT_FAILED;
		return m_nResultCode;
	}

	if(NULL == g_hUpdateMutex)
	{
		m_nResultCode = defUPDATE_RESULT_INIT_FAILED;
		return m_nResultCode;
	}

	//设置回调函数
    m_pfnCallBackProc = UpdateSetting.pfnCallBackProc;
    if (m_pfnCallBackProc)
    {
        m_pfnCallBackProc(defUPDATE_STATUS_INITIALIZING, 0);
    }
	
	//初始化路径
	nRetCode = InitPaths();
	if (!nRetCode)
	{
		m_nResultCode = defUPDATE_RESULT_INIT_FAILED;
		return m_nResultCode;
	}

	::InitUpdateData(UpdateSetting.bLog);
	LoadUpdateData(UpdateSetting);

	//设置线程同步消息
    CDownNotify::SetNotifyMessage(WM_DOWNNOTIFY_DEFAULT);
	m_hExitNotify = CreateEvent(NULL, TRUE, FALSE, NULL);

	//初始化成功
	m_nInitFlag = 1;
	m_nResultCode = defUPDATE_RESULT_INIT_SUCCESS;
	if (m_pfnCallBackProc)
    {
        m_pfnCallBackProc(defUPDATE_STATUS_INITIALIZING, 100);
    }

	m_bExServerMode = UpdateSetting.bExServerMode; //<------EXVERSION

	InterlockedExchange(&m_bUpdate,TRUE);
	return m_nResultCode;
}

int CUpdateDLLApp::UnInit()
{
	if (m_wndMessage.GetSafeHwnd())
        DestroyWindow(m_wndMessage.GetSafeHwnd());
    
    CDownNotify::SetNotifyMessage(0);
    
    if (m_hExitNotify)
    {
        CloseHandle(m_hExitNotify);
        m_hExitNotify = NULL;
    }

	if (m_nInitWSA)
    {
        WSACleanup();
        m_nInitWSA = 0;
    }
 
	return 1;
}

int CUpdateDLLApp::CancelDownload()
{
	if (!m_nInitFlag)
		return 0;

    if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
		return 0;
	
	m_DownloadFile.StopDownload();
	SetEvent(m_hExitNotify);

    return 0;
}

int CUpdateDLLApp::UserVerify()
{
    if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;
	
    if (!g_UpdateData.bUseVerify)
	{
		m_nResultCode = defUPDATE_RESULT_USER_VERIFY_SUCCESS;
		goto Exit1;
	}
		
Exit1:
    return m_nResultCode;
}

//游戏下载直接通过广域网下载
int CUpdateDLLApp::CheckNeedUpdate()
{
    int nRetCode = 0;
    MSG stMsg = { 0 }; 
	
	if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;

    m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED;
    
	ResetEvent(m_hExitNotify);
	// Download Index.dat
    m_DownloadFile.SetNotifyWnd(m_wndMessage, WM_DOWNNOTIFY_DEFAULT);
    SetDownloadProxy(g_UpdateData.szHostURL);
	nRetCode = ProcessNextStep(DOWNINDEX_STEPINDEX);
	if (!nRetCode)
    {
        m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED;
        goto Exit0;
    }

    //检查下载INDEX.DAT有没有成功，通过消息来传递一个文件下载成功
    do
    {//线程退出了
        nRetCode = ::WaitForSingleObject(m_hExitNotify, 0);
        if (WAIT_OBJECT_0 == nRetCode)
            break;
        //从消息队列中获得消息
        nRetCode = ::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE);
        if (0 == nRetCode)
            continue;        
        
        if (-1 == nRetCode)
        {
            // handle the error and possibly exit
        }
        else
        {
            ::TranslateMessage(&stMsg); 
            ::DispatchMessage(&stMsg); 
        }
    } while (true);
    
Exit0:
	m_DownloadFile.StopDownload();
	return m_nResultCode;
}

//确定下载列表,下载列表在g_processIndex.m_pUpdateItemList
int CUpdateDLLApp::Download()
{
	int nRetCode = 0;
    MSG stMsg = { 0 };

    if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;

    if (g_UpdateData.nMethod == defUPDATE_METHOD_LAN)
        m_nResultCode = defUPDATE_RESULT_DOWNLOAD_SUCCESS;
	
	KUPDATE_ITEM* pDownItem = g_ProcessIndex.m_pUpdateItemList;
	while (pDownItem)
	{
		if (pDownItem->DownloadStatus != enumDOWNLOADSTATUS_DOWNLOADED)
			break;
		pDownItem = pDownItem->pNext;
	}
	//找到一个需要下载的文件,或者其中为空
	if (pDownItem == NULL)
		m_nResultCode = defUPDATE_RESULT_DOWNLOAD_SUCCESS;

	ResetEvent(m_hExitNotify);
	//对下载的变量进行设置
    m_DownloadFile.SetNotifyWnd(m_wndMessage.GetSafeHwnd(), WM_DOWNNOTIFY_DEFAULT);
    m_DownloadFile.SetSavePath((CString)g_PathTable.szDownloadDestPath);

    SetDownloadProxy(g_UpdateData.szHostURL);
    m_nCurEnableResume = g_UpdateData.bAutoResume;
    nRetCode = DownNextItem(false);
    KAV_PROCESS_ERROR(nRetCode);
    
    do
    {
        nRetCode = ::WaitForSingleObject(m_hExitNotify, 0);
        if (WAIT_OBJECT_0 == nRetCode)
            break;
        
        nRetCode = ::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE);
        if (0 == nRetCode)
            continue;
       
        if (-1 == nRetCode)
        {
            // handle the error and possibly exit
        }
        else
        {
            ::TranslateMessage(&stMsg); 
            ::DispatchMessage(&stMsg); 
        }
    } while (true);
    
Exit0:
	m_DownloadFile.StopDownload();
    return m_nResultCode;
}

int CUpdateDLLApp::Update()
{
	int m_nResultCode	= defUPDATE_RESULT_UPDATE_SUCCESS;
	int nRetCode		= 0;	
	CString sMsg;
                                                     

	if (!m_nInitFlag)
		return defUPDATE_RESULT_INIT_FAILED;
    
    nRetCode = UpdateFiles();
    if (!nRetCode)
    {
		sMsg = defIDS_UPDATE_FINISH_FAILED;
		m_nResultCode = defUPDATE_RESULT_UPDATE_FAILED;
		g_UpdateData.SaveLog.WriteLogString(sMsg, true);
		return m_nResultCode;
	}

    sMsg = defIDS_UPDATE_FINISH_SUCCESS;
    g_UpdateData.SaveLog.WriteLogString(sMsg, true);

    if (g_UpdateData.bUpdateFailed)
    {
        sMsg = defIDS_NOTIFY_REUPDATE;
        g_UpdateData.SaveLog.WriteLogString(sMsg, true);
		m_nResultCode = defUPDATE_RESULT_UPDATE_FAILED;
    }   
	else
	{
		if (g_UpdateData.bNeedRebootFalg)
		{
			sMsg = defIDS_NEED_REBOOT;
			g_UpdateData.SaveLog.WriteLogString(sMsg, true);

			m_nResultCode = defUPDATE_RESULT_UPDATE_SUCCESS_NEED_REBOOT;
		}
		else if (g_UpdateData.bNeedUpdateSelfFirst)
			m_nResultCode = defUPDATE_RESULT_UPDATESELF_SUCCESS;
		else
			m_nResultCode = defUPDATE_RESULT_UPDATE_SUCCESS;
	}

	return m_nResultCode;
}

/******************************************辅助函数****************************************/
BOOL CUpdateDLLApp::RegisterMyWindow()
{
	CString strMyClass;
	
	try
	{
		strMyClass = AfxRegisterWndClass(CS_DBLCLKS | CS_VREDRAW,
						0,
						(HBRUSH) ::GetStockObject(BLACK_BRUSH));
	}
   	catch (CResourceException* pEx)
	{
       pEx->Delete();
	}

    RECT rect = { 0, 0, 100, 20};
	BOOL nRetCode = m_wndMessage.Create(
		strMyClass,
        "KingSoft Update Notify Window",
        WS_OVERLAPPEDWINDOW & ~WS_VISIBLE,
        rect,
        CWnd::GetDesktopWindow(),
        NULL
    );

    if (!nRetCode)
		return nRetCode;

	m_wndMessage.ShowWindow(SW_HIDE);
    m_wndMessage.UpdateWindow();

	return nRetCode;
}

BOOL CUpdateDLLApp::InitWinSock()
{
	WSADATA wsaData = {0};    
	WORD dwVersionRequested = MAKEWORD(2, 2);

    int nRetCode = WSAStartup(dwVersionRequested, &wsaData);
    if (0 == nRetCode)
    {
        m_nInitWSA = 1;
		return TRUE;
    }
    else
    {
		return FALSE;   
    }
}

BOOL CUpdateDLLApp::InitPaths()
{
	BOOL Result = FALSE;
    BOOL ErrorFlag = FALSE;
    unsigned uRetCode = 0;

    char *pszOffset = NULL;
    
    uRetCode = ::GetWindowsDirectory(g_PathTable.szWindowsPath, MAX_PATH);
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = TRUE;    
    AddPathChar(g_PathTable.szWindowsPath);

    uRetCode = (unsigned)::GetSystemDirectory(g_PathTable.szSystemPath, MAX_PATH);
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = TRUE;
    AddPathChar(g_PathTable.szSystemPath);

    uRetCode = ::GetCurrentDirectory(MAX_PATH, g_PathTable.szCurrentPath);
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = TRUE;
    AddPathChar(g_PathTable.szCurrentPath);

    uRetCode = ::GetModuleFileName(NULL, g_PathTable.szModulePath, (MAX_PATH + 1));
    ASSERT(uRetCode);
    if (!uRetCode) ErrorFlag = TRUE;
    pszOffset = strrchr(g_PathTable.szModulePath, '\\');
    ASSERT(pszOffset);
    pszOffset[1] = '\0';  


    uRetCode = ::GetTempPath(MAX_PATH, g_PathTable.szTempPath);
    ASSERT(uRetCode);    
    if (!uRetCode)
    {
        MkDirEx("C:\\Temp");    
        strcpy(g_PathTable.szTempPath, "C:\\Temp\\");
    }
    else
        AddPathChar(g_PathTable.szTempPath);   

    // Initialize the download destination directory
    strcpy(g_PathTable.szDownloadDestPath, g_PathTable.szModulePath);    
    strcat(g_PathTable.szDownloadDestPath, defUPDATE_DIRECTORY);
    
    // INitialize the Update destionation directory
    strcpy(g_PathTable.szUpdateDestPath, g_PathTable.szModulePath);
    
    if (ErrorFlag)
        Result = FALSE;
    else
        Result = TRUE;
	
    return Result;
}

BOOL CUpdateDLLApp::LoadUpdateData(const KUPDATE_SETTING& UpdateSetting)
{
	g_UpdateData.nMainVersion = UpdateSetting.nVersion;
	g_UpdateData.nMajorVersion = UpdateSetting.nMajorVersion;
    g_UpdateData.nMethod = UpdateSetting.nUpdateMode;
    strcpy(g_UpdateData.szHostURL, UpdateSetting.szUpdateSite);
    strcpy(g_UpdateData.szDefHostURL, UpdateSetting.szUpdateSite);
    g_UpdateData.ulTryTimes = UpdateSetting.ulTryTimes;
    g_UpdateData.bAutoTryNextHost = UpdateSetting.bAutoTryNextHost;
    g_UpdateData.bUseFastestHost = UpdateSetting.bUseFastestHost;
    g_UpdateData.bUseVerify = UpdateSetting.bUseVerify;
    strcpy(g_UpdateData.szVerifyInfo, UpdateSetting.szVerifyInfo);
    // Setting the proxy setting
    g_UpdateData.ProxySetting.nProxyMethod  = UpdateSetting.ProxySetting.nProxyMethod;
    g_UpdateData.ProxySetting.nHostPort     = UpdateSetting.ProxySetting.nHostPort;
    strcpy(g_UpdateData.ProxySetting.szHostAddr, UpdateSetting.ProxySetting.szHostAddr);
    strcpy(g_UpdateData.ProxySetting.szPassword, UpdateSetting.ProxySetting.szPassword);
    strcpy(g_UpdateData.ProxySetting.szUserName, UpdateSetting.ProxySetting.szUserName);
    // setting the download destination directory
    strcpy(g_PathTable.szDownloadDestPath, UpdateSetting.szDownloadPath);        
    strcpy(g_UpdateData.szLocalPath, UpdateSetting.szDownloadPath);
    // INitialize the Update destionation directory
    strcpy(g_PathTable.szUpdateDestPath, UpdateSetting.szUpdatePath);

	strcpy(g_UpdateData.szExecuteProgram, UpdateSetting.szMainExecute);

	return TRUE;
}

//消息处理函数，用来接受下载状态和下载结束的信息
BOOL CUpdateDLLApp::DownDispatch(const MSG *pMsg)
{
	ASSERT(pMsg);
	
	BOOL bRetCode;
    ULONG ulResult = 0;
    
    bRetCode =  CDownNotify::IsNotifyMessage(pMsg, &ulResult);
    if (bRetCode)
        return (BOOL)ulResult;
    
    return FALSE;
}

//下载文件的状态处理函数
//这个函数处理下载index.dat文件结束后的事情
//以及在下载更新文件结束后的处理
ULONG CUpdateDLLApp::OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus)
{
	ULONG ulResult = 0;
    
    if (DOWNINDEX_STEPOVER == m_nProcessStep)
	{
		if (m_pfnCallBackProc && pDownStatus)
		{
			DOWNLOADFILESTATUS info;
			strncpy(info.strFileName, pDownStatus->strFileName, MAX_PATH);
			info.dwFileSize = pDownStatus->dwFileSize;
			info.dwFileDownloadedSize = pDownStatus->dwFileDownloadedSize;
			m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING_FILE, (long)&info);
		}
	}

    return ulResult;
}

//下载结果的处理函数
ULONG CUpdateDLLApp::OnDownResult(ULONG ulOverResult)
{
	ULONG ulResult		= CDownNotify::OnDownResult(ulOverResult);
    int nRetCode		= 0;   	
	int nCurDownOK      = 0;
	int nRedownCurrent  = 0;
	CString sLog, sFormat;
        
    if (DOWNINDEX_STEPINDEX == m_nProcessStep)
    {
		//处理下载index.data后的相关流程
		if (DOWN_RESULT_SUCCESS == ulOverResult)
		{
			m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_SUCCESS;
			nRetCode = ProcessNextStep(DOWNINDEX_STEPOVER);
			if(nRetCode == defVersionNotenough)
			{
				m_nResultCode = nRetCode;
			}
			PostQuitMessage(0);
			SetEvent(m_hExitNotify);
			goto Exit1;
		}
		else
		{
			m_nResultCode = defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED;

            if (!g_UpdateData.bAutoTryNextHost)
            {
				PostQuitMessage(0);
				SetEvent(m_hExitNotify);
				goto Exit1;
			}

			// here to get Next host to download index
			//g_UpdateData.szHostURL
			SetDownloadProxy(g_UpdateData.szHostURL);
			nRetCode = ProcessNextStep(DOWNINDEX_STEPINDEX);
		}
	}
	else
	{//处理其余文件下载后的流程
		ASSERT(m_pCurDownItem != NULL);    
		m_nCurEnableResume = g_UpdateData.bAutoResume;
    
		switch (ulOverResult)
		{
		case DOWN_RESULT_SUCCESS:
		case DOWN_RESULT_SAMEAS:
			m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_DOWNLOADED;
        	nCurDownOK = 1;						

			if (!nCurDownOK)
			{
				if (!g_UpdateData.bAutoTryNextHost) //if (!ExistsOtherHost())
				{
					m_nResultCode = defUPDATE_RESULT_DOWNLOAD_FAILED;
					m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_ERROR;                
					sFormat = defIDS_DOWNLOAD_FILE_FAILED;
					sLog.Format(sFormat, m_pCurDownItem->szFileName, m_pCurDownItem->szDownloadTmpFileName);
					g_UpdateData.SaveLog.WriteLogString(sLog, true);
					if (!g_UpdateData.bDownloadFailed)
						g_UpdateData.bDownloadFailed = m_pCurDownItem->bNeedDownload;
				}
				else
				{
					//do not skip download this update file
					nRedownCurrent = true;
					m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_QUEUE;
					m_nCurEnableResume = false;
				}
			}
			else
			{
                m_nResultCode = defUPDATE_RESULT_DOWNLOAD_SUCCESS;
				sFormat = defIDS_DOWNLOAD_FILE_SUCCESSFUL;
				sLog.Format(sFormat, m_pCurDownItem->szFileName, m_pCurDownItem->szDownloadTmpFileName);
				g_UpdateData.SaveLog.WriteLogString(sLog, true);
			}
			break;
        
		case DOWN_RESULT_FAIL:
			if (m_pCurDownItem->nDownTryTimes < (int)g_UpdateData.ulTryTimes)
			{
				nRedownCurrent = true;
			}
			if (!g_UpdateData.bAutoTryNextHost) //if (!ExistsOtherHost())
			{
				m_nResultCode = defUPDATE_RESULT_DOWNLOAD_FAILED;
				m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_ERROR;
				sFormat = defIDS_DOWNLOAD_FILE_FAILED;
				sLog.Format(sFormat, m_pCurDownItem->szFileName, m_pCurDownItem->szDownloadTmpFileName);
				g_UpdateData.SaveLog.WriteLogString(sLog, true);
				if (!g_UpdateData.bDownloadFailed)
					g_UpdateData.bDownloadFailed = m_pCurDownItem->bNeedDownload;
			}
			else
			{
				//do not skip download this update file
				nRedownCurrent = true;
				m_pCurDownItem->nDownTryTimes = 0;
				m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_QUEUE;
			}

			break;
        
		default:
			ASSERT(FALSE);
			break;
		}
    
		nRetCode = DownNextItem(nRedownCurrent);
		if (!nRetCode)
		{
			if (nRedownCurrent)
				ASSERT(FALSE);
			else
            {                   
                PostQuitMessage(0);
				SetEvent(m_hExitNotify);
                goto Exit1;
            }
        }

        if (m_nResultCode == defUPDATE_RESULT_CANCEL)
        {
	        PostQuitMessage(0);
			SetEvent(m_hExitNotify);
			goto Exit1;
        }
	}
        
Exit1:
    return ulResult;
}

//步骤控制函数
int CUpdateDLLApp::ProcessNextStep(int nDownStep)
{
    int nResult = 0;
    int nRetCode = 0;
    CString sMsg;
    CString sTitle;
    MSG stMsg = { 0 };
    HANDLE Handle = NULL;    
    CString strDownUrl;
    
    switch (nDownStep)
    {
    case DOWNINDEX_STEPINDEX:
        strDownUrl = g_UpdateData.szHostURL;
        strDownUrl += defINDEX_FILE_NAME;
        
        MkDirEx(g_PathTable.szDownloadDestPath);
        m_strIndexTempFile = (CString)g_PathTable.szDownloadDestPath + defINDEX_FILE_NAME;
        
        m_DownloadFile.SetTimeout(10000, 24000, 24000);
        nResult = m_DownloadFile.StartDownload(strDownUrl, false, m_strIndexTempFile);
        break;
        
    case DOWNINDEX_STEPOVER:
        if (m_pfnCallBackProc)
        {
            if (m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 0))
            {
                m_nResultCode = defUPDATE_RESULT_CANCEL;
				goto Exit0;
            }
        }
		
        nRetCode = ::ProcessIndexFile(m_strIndexTempFile);
		if (!nRetCode)
		{
			m_nResultCode = defUPDATE_RESULT_PROCESS_INDEX_FAILED;
			goto Exit0;
		}
		if(nRetCode == defVersionNotenough)
		{
			m_nResultCode = defVersionNotenough;
			return defVersionNotenough;
		}
		//Modified by Fellow, 2003.11.13
		if(nRetCode == defVersionLatest)
		{
			m_nResultCode = defVersionLatest;
			return defVersionLatest;
		}
		
		//判断有没有需要升级的信息
		if (g_ProcessIndex.IsNotUpdateItem())
			m_nResultCode = defUPDATE_RESULT_NOT_UPDATE_FILE; 
		else
		{

			if (m_nResultCode==defUPDATE_RESULT_DOWNLOAD_INDEX_SUCCESS)
			{
				//判断更新列表中所有东西是否已经下载完毕，如果是则直接更新不弹出对话框
				bool          bDownloadedAll=true;
				KUPDATE_ITEM* pDownItem = g_ProcessIndex.m_pUpdateItemList;
				while (pDownItem)
				{
					if (pDownItem->DownloadStatus != enumDOWNLOADSTATUS_DOWNLOADED)
					{
						bDownloadedAll=false;
					 	break;
					}//endif

					pDownItem = pDownItem->pNext;
				}

				if (!bDownloadedAll)  //只有当有文件需要下载的时候才让用户选择
				{
					
// 					HANDLE hWaitCustom=CreateEvent(NULL,TRUE,FALSE,NULL);
// 					//增加非强制更新逻辑 Add by Brianyao
// 					if (nRetCode == defUPDATE_RESULT_CANPLAY_UPDATE_NEXTIME)
// 					{
// 						m_pfnCallBackProc(defUPDATE_RESULT_CANPLAY_UPDATE_NEXTIME, (long)hWaitCustom);
// 					}
// 					else 
// 					{
// 						m_pfnCallBackProc(defUPDATE_RESULT_NEED_UPDATE, (long)hWaitCustom);
// 					}//endif
// 			
// 					WaitForSingleObject(hWaitCustom,INFINITE);

					//增加非强制更新逻辑 Add by Brianyao
					if (nRetCode == defUPDATE_RESULT_CANPLAY_UPDATE_NEXTIME)
					{
						m_pfnCallBackProc(defUPDATE_RESULT_CANPLAY_UPDATE_NEXTIME, 0);
					}
					else 
					{
						HANDLE hWaitCustom=CreateEvent(NULL,TRUE,FALSE,NULL);
						m_pfnCallBackProc(defUPDATE_RESULT_NEED_UPDATE, (long)hWaitCustom);
						WaitForSingleObject(hWaitCustom,INFINITE);
					}//endif
				}
			}//endif
			
			m_nResultCode = defUPDATE_RESULT_PROCESS_INDEX_SUCCESS;	 
		}//end else
		
        break;        
    default:
        ASSERT(FALSE);
        goto Exit0;
    }
    
    m_nProcessStep = nDownStep;
    nResult = true;
Exit0:
    return nResult;
}

BOOL CUpdateDLLApp::SetDownloadProxy(const char *lpszHostUrl)
{
	ASSERT(lpszHostUrl);
	
	int nResult = false;
    int nRetCode = false;
    
    int nDownProto = DOWN_PROTOCOL_NONE;
    int nProxyMode = PROXY_MODE_NONE;
    
    switch (g_UpdateData.ProxySetting.nProxyMethod)
    {
    case PROXY_METHOD_USEIE:
        nDownProto = GetUrlProtocolType(lpszHostUrl);
        
        nRetCode = GetIEProxyValid();
        switch (nDownProto)
        {
        case DOWN_PROTOCOL_HTTP:
            if (nRetCode & (IEPROXY_HTTP | IEPROXY_SOCKS))
                nProxyMode = PROXY_MODE_USEIE;
            break;
        case DOWN_PROTOCOL_FTP:
            if (nRetCode & (IEPROXY_FTP  | IEPROXY_SOCKS))
                nProxyMode = PROXY_MODE_USEIE;
            break;
            
        default:
            break;
        }
        break;
        
    case PROXY_METHOD_CUSTOM:
        nProxyMode = g_UpdateData.ProxySetting.nProxyMode;
        break;
        
    case PROXY_METHOD_DIRECT:
    default:
        break;
    }
    
    m_DownloadFile.SetProxy(
        nProxyMode,
        g_UpdateData.ProxySetting.szHostAddr,
        g_UpdateData.ProxySetting.nHostPort,
        g_UpdateData.ProxySetting.szUserName,
        g_UpdateData.ProxySetting.szPassword
    );
    
    nResult = (nProxyMode != PROXY_MODE_NONE);
    
    return nResult;
}

int CUpdateDLLApp::DownNextItem(BOOL nRedownCurrent /* = FALSE */)
{
    int nResult = 0;
    int nRetCode = 0;
    
    if (nRedownCurrent)
    {
        if (NULL == m_pCurDownItem)
        {
            ASSERT(FALSE);
            goto Exit0;
        }
        
        ASSERT(m_pCurDownItem->DownloadStatus != enumDOWNLOADSTATUS_DOWNLOADED);
    }
    else
    {
        if (m_pCurDownItem != NULL)  
            m_pCurDownItem = m_pCurDownItem->pNext;
        //刚才仅仅是获得是否有需要下载的东东,现在进行正式判断下载的方式
        if (NULL == m_pCurDownItem)
            m_pCurDownItem = g_ProcessIndex.m_pUpdateItemList;
    }
    
    while (m_pCurDownItem != NULL)
    {
        switch (m_pCurDownItem->DownloadStatus)
        {
        case enumDOWNLOADSTATUS_ERROR:
        case enumDOWNLOADSTATUS_DOWNLOADED:
            if (!nRedownCurrent)
            {
                m_pCurDownItem = m_pCurDownItem->pNext;
                continue;
            }
            break;
            
        case enumDOWNLOADSTATUS_QUEUE:
            m_pCurDownItem->nDownTryTimes = 0;

			//在这里可以考虑使用启动下载进行下载.
            break;
            
        default:
            break;
        }
        
        m_pCurDownItem->nDownTryTimes++;
        break;
    }
    
    if (m_pCurDownItem != NULL)
    {
        CString strDownUrlPath;
        
        strDownUrlPath = g_UpdateData.szHostURL;
        strDownUrlPath += m_pCurDownItem->szRemotePath;
        strDownUrlPath += m_pCurDownItem->szFileName;
       
        m_pCurDownItem->DownloadStatus = enumDOWNLOADSTATUS_DOWNLOADING;

		if (m_pfnCallBackProc)
		{
			KUPDATE_ITEM * pItem = g_ProcessIndex.m_pUpdateItemList;
			int nCount = 1;
			int nIndex = 1;
			while (pItem)
			{	//一共有八个文件需要下载,而目前第三个目前需要下载.
				nCount++;
				if (m_pCurDownItem == pItem)
				{
					nIndex = nCount;
				}
				pItem = pItem->pNext;
			}

			if (m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING, nIndex / nCount * 100))
			{
				m_nResultCode = defUPDATE_RESULT_CANCEL;
				goto Exit0;
			}
		}
        
        nRetCode = m_DownloadFile.StartDownload(
            strDownUrlPath,
            m_nCurEnableResume,
            m_pCurDownItem->szDownloadTmpFileName
        );
        ASSERT(nRetCode);
        
        nResult = 1;
    }
    
Exit0:
    return nResult;
}
//Add by Brianyao 2007
//用来设置是否可以执行升级拷贝
BOOL CUpdateDLLApp::GetCanUpdate(void)const
{
	return m_bUpdate;
}

VOID CUpdateDLLApp::ReSetCanUpdate(void)
{
	InterlockedExchange(&m_bUpdate,0);
}
