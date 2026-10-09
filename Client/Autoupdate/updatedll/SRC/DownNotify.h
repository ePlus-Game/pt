// DownNotify.h: interface for the CDownNotify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOWNNOTIFY_H__B236C810_2DA8_4B08_B193_EC78936ABFBC__INCLUDED_)
#define AFX_DOWNNOTIFY_H__B236C810_2DA8_4B08_B193_EC78936ABFBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WndNotify.h"
#include "downloadfile.h"

#define WM_DOWNLOAD_NOTIFY                  (WM_USER + 20)
#define WM_DOWNNOTIFY_DEFAULT               (WM_DOWNLOAD_NOTIFY)

class CDownNotify : public CWndNotify  
{
public:
    CDownNotify();
    CDownNotify(ULONG ulMessage);
    
    virtual ~CDownNotify();
    
protected:
    virtual ULONG OnStatusFileName(PDOWNLOADSTATUS pDownStatus);
    virtual ULONG OnStatusFileSize(PDOWNLOADSTATUS pDownStatus);
    virtual ULONG OnStatusFileDowned(PDOWNLOADSTATUS pDownStatus);
    
    virtual ULONG OnDownStatus(PDOWNLOADSTATUS pDownStatus);
    virtual ULONG OnDownResult(ULONG ulDownResult);
    
protected:
    virtual int IsNotifyMessage(const MSG *pMsg, ULONG *pulResult);
};

#endif // !defined(AFX_DOWNNOTIFY_H__B236C810_2DA8_4B08_B193_EC78936ABFBC__INCLUDED_)
