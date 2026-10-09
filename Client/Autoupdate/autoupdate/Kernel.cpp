// Kernel.cpp: implementation of the CKernel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "Kernel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKernel::CKernel() : m_bInKernel(FALSE), m_bCanceled(FALSE)
{

}

CKernel::~CKernel()
{

}

//*********************************************************************
// function : 设置是否在升级内核中
//*********************************************************************
void CKernel::InKernel(BOOL bInKernel)
{
	LockKernel(TRUE);
	m_bInKernel = bInKernel;
	LockKernel(FALSE);
}

//*********************************************************************
// function : 查询是否在升级内核中
//*********************************************************************
BOOL CKernel::InKernel()
{
	return m_bInKernel;
}

//*********************************************************************
// function	: 锁定升级内核状态
// parameter: bLock TRUE为锁定，FALSE为解锁
//*********************************************************************
void CKernel::LockKernel(BOOL bLock)
{
	if (bLock)
		m_secKernel.Lock();
	else
		m_secKernel.Unlock();
}

//*********************************************************************
// function : 重置
//*********************************************************************
void CKernel::Reset()
{
	m_bInKernel = FALSE;
	m_bCanceled = FALSE;
}

//*********************************************************************
// function : 取消
//*********************************************************************
void CKernel::Cancel()
{
	m_secCancel.Lock();
	m_bCanceled = TRUE;
	m_secCancel.Unlock();
}

//*********************************************************************
// function : 是否取消
//*********************************************************************
BOOL CKernel::IsCanceled()
{
	m_secCancel.Lock();
	BOOL bCanceled = m_bCanceled;
	m_secCancel.Unlock();
	return bCanceled;
}
