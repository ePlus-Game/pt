////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   WndNotify.h
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-31  11:18:20
//  Comment     :   CWndNotify class header file
//  
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDNOTIFY_H__9DF9A233_B0C8_4EFA_9BF3_FB2594BE0419__INCLUDED_)
#define AFX_WNDNOTIFY_H__9DF9A233_B0C8_4EFA_9BF3_FB2594BE0419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CWndNotify  
{
public:
    CWndNotify();
    CWndNotify(ULONG ulMessage);
    
    virtual ~CWndNotify();
    
public:
    int SetNotifyMessage(ULONG ulMessage);
    int GetNotifyMessage(ULONG ulMessage);
    
protected:
    virtual int IsNotifyMessage(const MSG *pMsg, ULONG *pulResult);
    
protected:
    ULONG m_ulNotifyMessage;
};

#endif // !defined(AFX_WNDNOTIFY_H__9DF9A233_B0C8_4EFA_9BF3_FB2594BE0419__INCLUDED_)
