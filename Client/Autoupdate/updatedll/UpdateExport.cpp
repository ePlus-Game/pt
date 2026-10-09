//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateExport.cpp
//  Version     :   1.0
//  Creater     :   Cheng bitao
//  Date        :   2002-11-28 17:24:46
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UpdateExport.h"
#include "UpdateDLL.h"
#include "UpdateData.h"
#include "Global.h"

int __stdcall Update_Init(const KUPDATE_SETTING UpdateSetting)
{
	return g_theApp.Init(UpdateSetting);
}

int __stdcall Update_UnInit()
{
	return g_theApp.UnInit();
}

int __stdcall Update_Start()
{
	int nRetCode = 0;
	CString sLog;
	sLog = g_UpdateData.szHostURL;
	g_UpdateData.SaveLog.WriteLogString(sLog, true);

    if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_VERIFING, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }

	TRACE0("UserVerify\n");
    nRetCode = g_theApp.UserVerify();
	TRACE0("UserVerify End\n");
    if (nRetCode != defUPDATE_RESULT_USER_VERIFY_SUCCESS)
        return nRetCode;
    if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_VERIFING, 100);
    }

    if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }
	
	TRACE0("CheckNeedUpdate\n");
    nRetCode = g_theApp.CheckNeedUpdate();
	TRACE0("CheckNeedUpdate End\n");
    if (nRetCode != defUPDATE_RESULT_PROCESS_INDEX_SUCCESS)
    {
		if (nRetCode == defUPDATE_RESULT_NOT_UPDATE_FILE)
		{
			sLog = "Not Update File OK!";
			g_UpdateData.SaveLog.WriteLogString(sLog, true);
			
			if (g_theApp.m_pfnCallBackProc)
			{
				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_VERSION_LATEST, 0);
		    }

			return nRetCode;
		}
		if(nRetCode == 1)
		{
			if (g_theApp.m_pfnCallBackProc)
			{
				return defUPDATE_RESULT_UPDATE_SUCCESS;
		    }
		}
		if( nRetCode == defVersionNotenough)
		{
			if (g_theApp.m_pfnCallBackProc)
			{
				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_VERSION_NOT_ENOUGH, (long)&g_ProcessIndex.url[0]);
		    }
			return nRetCode;
		}
		if( nRetCode == defVersionMore)
		{
			if (g_theApp.m_pfnCallBackProc)
			{
				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_VERSION_MORE, 0);
		    }
			return nRetCode;
		}
		if( nRetCode == defVersionLatest)
		{
			if (g_theApp.m_pfnCallBackProc)
			{
				g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_VERSION_LATEST, 0);
		    }
			return nRetCode;
		}		
	
		sLog = "Process Index.dat Failed!";
		g_UpdateData.SaveLog.WriteLogString(sLog, true);
		if (g_theApp.m_pfnCallBackProc)
		{
			g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 0);
		}
		
		return nRetCode;
	}
	sLog = "Process Index.dat OK!";
	g_UpdateData.SaveLog.WriteLogString(sLog, true);
	if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_PROCESSING_INDEX, 100);
		//add hehongpeng
		g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_TOTALFILENUM, g_UpdateData.nTotalNum);
		//hehongpengend
    }

	//Modified by Fellow, 2003.12.9
	//运行前程序
	if (g_theApp.m_pfnCallBackProc && g_UpdateData.strRunBefore != _T(""))
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_RUNBEFORE,
							(long)g_UpdateData.strRunBefore.GetBuffer(1)))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }
	
	if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }

	TRACE0("Download\n");
    nRetCode = g_theApp.Download();
	
	//Recheck the downloaded thing ! 
	if (g_UpdateData.nMethod == defUPDATE_METHOD_INTERNET)
	{
		KUPDATE_ITEM* pDownItem = g_ProcessIndex.m_pUpdateItemList;
		while (pDownItem)
		{
			if (pDownItem->bNeedUpdate && pDownItem->DownloadStatus == enumDOWNLOADSTATUS_DOWNLOADED)
			{
				 int nRet = CheckFileCRC(atoi(pDownItem->szCRC), pDownItem->szDownloadTmpFileName);
				 if (nRet == 0)
				 {
					 nRetCode = defUPDATE_RESULT_DOWNLOAD_FAILED;
					 break;
				 }//endif

			}//endif
			
			pDownItem = pDownItem->pNext;
		}//end while

	}//endif

	TRACE0("Download End\n");
    if (nRetCode != defUPDATE_RESULT_DOWNLOAD_SUCCESS)
    {
		sLog = "Download Failed!";
		g_UpdateData.SaveLog.WriteLogString(sLog, true);
		return nRetCode;
	}
	sLog = "Download OK!";
	g_UpdateData.SaveLog.WriteLogString(sLog, true);
	if (g_theApp.m_pfnCallBackProc)
    {
        g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_DOWNLOADING, 100);
    }

	if (g_theApp.m_pfnCallBackProc)
    {
        if (g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_UPDATING, 0))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
    }


	if( GetUpdateLock(INFINITE) )
	{
		TRACE0("Update\n");
		if (g_theApp.GetCanUpdate())
			nRetCode = g_theApp.Update();
		else//Add by brianyao 2007 :增加用户执行的逻辑
			nRetCode = defUPDATE_RESULT_UPDATE_SUCCESS;

		ReleaseUpdateLock();
	}
		
	TRACE0("Update End\n");
	if (g_theApp.m_pfnCallBackProc)
	{
		g_theApp.m_pfnCallBackProc(defUPDATE_STATUS_UPDATING, 100);
	}
	
	//Modified by Fellow, 2003.12.9
	//运行后程序
	if (nRetCode == defUPDATE_RESULT_UPDATE_SUCCESS &&
			g_theApp.m_pfnCallBackProc &&
			g_UpdateData.strRunBefore != _T(""))
	{
		if (g_theApp.m_pfnCallBackProc(defUPDATE_RESULT_RUNAFTER,
							(long)g_UpdateData.strRunAfter.GetBuffer(1)))
		{
			nRetCode = defUPDATE_RESULT_CANCEL;
			goto Exit0;
		}
	}

Exit0: 
    return nRetCode;
}

int __stdcall Update_Cancel()
{
	
	return g_theApp.CancelDownload();
}

//被Dlg调用 设置更新的逻辑，使下次更新，这次只Download
//Add by Brianyao2007+

int __stdcall Update_NextTime()
{
	g_theApp.ReSetCanUpdate();
	return 0;
}

bool __stdcall GetUpdateLock(DWORD dwTime)
{
	return WAIT_OBJECT_0 == WaitForSingleObject(g_hUpdateMutex, dwTime);
}

void __stdcall ReleaseUpdateLock()
{
	ReleaseMutex(g_hUpdateMutex);
}
