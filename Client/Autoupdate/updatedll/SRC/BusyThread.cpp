// BusyThread.cpp: implementation of the CBusyThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BusyThread.h"
#include "ftpdownload.h"
#include "httpdownload.h"


#define  WAIT_TIME 100
extern CFtpDownload m_FtpDownload;
extern CHttpDownload m_HttpDownload;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBusyThread::CBusyThread()
{
	m_hStop = NULL;
	m_hProcessor = NULL;
}

CBusyThread::~CBusyThread()
{
	ASSERT(m_hStop == NULL);
	ASSERT(m_hProcessor == NULL);
}


int CBusyThread::IsThreadOK()
{
    return (m_hProcessor != NULL);  //这是进行判断线程有没有启动m_hProcessor是线程的句柄
}


int CBusyThread::StartThread()
{
	int nRet = 0;
	if ( NULL != m_hProcessor )	// The thread has been running.
	{
		return nRet;
	}
	if (NULL == m_hStop)
	{
		m_hStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	DWORD dwThreadID = 0;
	m_hProcessor = CreateThread(NULL, 0, CBusyThread::InnerThreadProc, (LPVOID)this, 0, &dwThreadID);

	nRet = m_hProcessor != NULL;

	return nRet;
}

int CBusyThread::StopThread()
{
	int bRet = FALSE;

	if (NULL == m_hProcessor
		|| NULL == m_hStop)
	{
		bRet = TRUE;
		return bRet;
	}

	SetEvent(m_hStop);
	//ADD BY HOLY 12 - 11 2003 16:26  由于取消以后,系统在这里停了,我取时间为WAIT_TIME
	//修改方案2:  设置m_hstopdownload事件为false


//	DWORD dwResult = WaitForSingleObject(m_hProcessor, INFINITE);
	DWORD dwResult = WaitForSingleObject(m_hProcessor,WAIT_TIME);
	if (WAIT_FAILED == dwResult)
	{
		bRet = FALSE;
	}
	else if (WAIT_OBJECT_0 == dwResult)
	{
		bRet = TRUE;
	}
	else if (WAIT_TIMEOUT == dwResult)	// Time out.
	{
		//modify by holy 12-12 2003 10:22
		m_FtpDownload.m_bStopDownload = false;
		m_HttpDownload.m_bStopDownload = false;
		if (TerminateThread(m_hProcessor, 0))
		{
				bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}

	}

	if (bRet)
	{
		CloseHandle(m_hStop);
		m_hStop = NULL;
		CloseHandle(m_hProcessor);
		m_hProcessor = NULL;
	}

	return bRet;//注意升级完成以后如何,下面在里面修改,第一个BUG就修改了
}

int CBusyThread::PreExecution()
{
    return true;
}

void CBusyThread::PostExecution()
{
}

DWORD WINAPI CBusyThread::InnerThreadProc(LPVOID lpThisParam)
{
    ULONG ulResult = -1;
    
    int nRetCode = false;
    CBusyThread *pThis = (CBusyThread *)lpThisParam;
    
    ASSERT_POINTER(pThis, CBusyThread);
    
    if (NULL == pThis)
        goto Exit0;
    
	//为何在PreExecution 函数启动的是CBusyThread对象,而 MainExecution启动的是CDownloadFile对象.

    nRetCode = pThis->PreExecution();
    if (!nRetCode)
        goto Exit0;
    
    ulResult = pThis->MainExecution();
    pThis->PostExecution();
    
Exit0:
    return ulResult;
}
