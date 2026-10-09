// DownLoadFile.h: interface for the CDownLoadFile class.
// 使用update.dll里面的函数,下载单个文件
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNLOADFILE_H__5D59FEB4_F9A5_4368_B2B4_61A4E7F3E0D4__INCLUDED_)
#define AFX_DOWNLOADFILE_H__5D59FEB4_F9A5_4368_B2B4_61A4E7F3E0D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDownLoadFile  
{
public:
	CDownLoadFile();
	virtual ~CDownLoadFile();
	
	static BOOL GetFileByFtp(LPCTSTR lpszDownloadUrl,
		LPCTSTR lpszSavePath,
		BOOL bForceDownload = TRUE,
		DWORD dwSendTimeout = 3 * 1000,
		DWORD dwRecvTimeout = 3 * 1000,
		DWORD dwConnTimeout = 3 * 1000);

	static BOOL GetFileByHttp(LPCTSTR lpszDownloadUrl,
		LPCTSTR lpszSavePath,
		BOOL bForceDownload = TRUE,
		DWORD dwSendTimeout = 3 * 1000,
		DWORD dwRecvTimeout = 3 * 1000,
		DWORD dwConnTimeout = 3 * 1000
		);
};

#endif // !defined(AFX_DOWNLOADFILE_H__5D59FEB4_F9A5_4368_B2B4_61A4E7F3E0D4__INCLUDED_)
