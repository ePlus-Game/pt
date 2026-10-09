////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   WndNotify.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-01-31  11:18:20
//  Comment     :   CWndNotify class source file
//  
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndNotify.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWndNotify::CWndNotify()
{
    m_ulNotifyMessage = -1;
}

CWndNotify::CWndNotify(ULONG ulMessage)
{
    m_ulNotifyMessage = ulMessage;
}

CWndNotify::~CWndNotify()
{
}

int CWndNotify::SetNotifyMessage(ULONG ulMessage)
{
    m_ulNotifyMessage = ulMessage;
    
    return true;
}

int CWndNotify::GetNotifyMessage(ULONG ulMessage)
{
    return m_ulNotifyMessage;
}

int CWndNotify::IsNotifyMessage(const MSG *pMsg, ULONG *pulResult)
{
    int nResult = false;
    
    ASSERT_POINTER(pMsg, MSG);
    
    if ((pMsg != NULL) && (m_ulNotifyMessage != -1))
    {
        if (m_ulNotifyMessage == pMsg->message)
            nResult = true;
    }
    
    return nResult;
}
