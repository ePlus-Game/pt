// DownLoadFile.cpp: implementation of the CDownLoadFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpdate.h"
#include "DownLoadFile.h"
#include ".\downloadtmp\ftpdownload.h"
#include ".\downloadtmp\httpdownload.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDownLoadFile::CDownLoadFile()
{

}

CDownLoadFile::~CDownLoadFile()
{

}

BOOL CDownLoadFile::GetFileByFtp(LPCTSTR lpszDownloadUrl,
				  LPCTSTR lpszSavePath,
				  BOOL bForceDownload/* = TRUE*/,
				  DWORD dwSendTimeout/* = 3 * 1000*/,
				  DWORD dwRecvTimeout/* = 3 * 1000*/,
				  DWORD dwConnTimeout/* = 3 * 1000*/)
{
	CFtpDownload downer;
	downer.SetTimeout(dwSendTimeout, dwRecvTimeout, dwConnTimeout);	
	int nRet = downer.Download(lpszDownloadUrl, lpszSavePath, bForceDownload);
	
	return (nRet == 0);
}

BOOL CDownLoadFile::GetFileByHttp(LPCTSTR lpszDownloadUrl,
				   LPCTSTR lpszSavePath,
				   BOOL bForceDownload/* = TRUE*/,
				   DWORD dwSendTimeout/* = 3 * 1000*/,
				   DWORD dwRecvTimeout/*= 3 * 1000*/,
				   DWORD dwConnTimeout/* = 3 * 1000*/)
{
	CHttpDownload httper;
	httper.SetTimeout(dwSendTimeout, dwRecvTimeout, dwConnTimeout);
	int nRet = httper.Download(lpszDownloadUrl, lpszSavePath, bForceDownload);
	
	if (nRet == 0)
		return TRUE;
	else
		return FALSE;
}
