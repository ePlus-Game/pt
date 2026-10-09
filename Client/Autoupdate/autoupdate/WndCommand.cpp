// WndCommand.cpp: implementation of the CWndCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autoupdate.h"
#include "WndCommand.h"
#include "BitmapSlider.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------
// function : 消息处理器数组，注意元素索引位置必须和enum Protocol中的索引相对应
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
CWndCommand::fnWndCommand CWndCommand::m_fnWndCommands[COMMAND_EOF + 1] =
{
	&CWndCommand::Command_DialogDoModal,
	&CWndCommand::Command_ModifyWindowText,
	&CWndCommand::Command_SetProgressRate,
	&CWndCommand::Command_ShowWindow,
	&CWndCommand::Command_MoveWindow,
	&CWndCommand::Command_MessageBox,
	&CWndCommand::Command_CmdHandler,
	// Add new protocol hander here
	NULL	// 结束标志
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWndCommand::CWndCommand()
{

}

CWndCommand::~CWndCommand()
{

}

//---------------------------------------------------------------------
// function: 消息响应函数
//---------------------------------------------------------------------
void CWndCommand::OnMessage(WPARAM wParam, LPARAM lParam)
{
	// wParam是协议类型，lParam是协议数据
	ASSERT(wParam < CWndCommand::COMMAND_EOF);
	if (wParam < CWndCommand::COMMAND_EOF)
	{
		CWndCommand::fnWndCommand pCommand = CWndCommand::m_fnWndCommands[wParam];
		ASSERT(pCommand);
		ASSERT(
			pCommand == Command_DialogDoModal ||
			pCommand == Command_ModifyWindowText ||	// 修改窗口文本信息
			pCommand == Command_SetProgressRate || // 修改进度控件的百分比显示
			pCommand == Command_ShowWindow ||	// 显示窗口
			pCommand == Command_MoveWindow ||	// 移动窗口
			pCommand == Command_MessageBox ||	// 消息对话框
			pCommand == Command_CmdHandler);	// 命令处理器
		// 调用协议处理器
		if (pCommand)
			pCommand((void*)lParam);
	}
}

//---------------------------------------------------------------------
// function: 显示选择版本对话框处理器
//---------------------------------------------------------------------
void CWndCommand::Command_DialogDoModal(void *pData)
{
	ASSERT(pData);
	// 该消息数据包含两个指针，分别是CDirSelectDlg*和UINT*
	void **ppData = (void**)pData;
	CDialog *pDlg = (CDialog*)ppData[0];
	UINT *pnResult = (UINT*)ppData[1];
	ASSERT(pDlg);
	// 显示选择版本对话框
	UINT nResult = pDlg->DoModal();
	if (pnResult)
		*pnResult = nResult;
}

//---------------------------------------------------------------------
// function: 修改进度相关的CStatic信息
//---------------------------------------------------------------------
void CWndCommand::Command_ModifyWindowText(void *pData)
{
	ASSERT(pData);
	// 该消息包含两个指针，分别是CWnd*和LPCSTR
	void **ppData = (void**)pData;
	CWnd *pWnd = (CWnd*)ppData[0];
	LPCSTR pcszMessage = (LPCSTR)ppData[1];
	ASSERT(pWnd && pcszMessage);
	pWnd->SetWindowText(pcszMessage);
	//pWnd->Invalidate(TRUE);
}

//---------------------------------------------------------------------
// function: 修改进度控件的百分比显示
//---------------------------------------------------------------------
void CWndCommand::Command_SetProgressRate(void *pData)
{
	ASSERT(pData);
	// 该消息包含两个数据，分别是CProgressCtrl*和int
	void **ppData = (void**)pData;
	CBitmapSlider *pCtl = (CBitmapSlider*)ppData[0];
	int nRate = (int)ppData[1];
	ASSERT(pCtl);
	pCtl->SetPos(nRate);
}

//---------------------------------------------------------------------
// function: 显示窗口
//---------------------------------------------------------------------
void CWndCommand::Command_ShowWindow(void *pData)
{
	ASSERT(pData);
	// 该消息只包含两个数据，分别是CWnd*和int
	void **ppData = (void**)pData;
	CWnd *pWnd = (CWnd*)ppData[0];
	int nCmdShow = (int)ppData[1];
	ASSERT(pWnd);
	pWnd->ShowWindow(nCmdShow);
}

//---------------------------------------------------------------------
// function: 移动窗口
//---------------------------------------------------------------------
void CWndCommand::Command_MoveWindow(void *pData)
{
	ASSERT(pData);
	// 该消息包含五个数据，分别是CWnd*, int x, int y, int nWidth, int nHeight
	void **ppData = (void**)pData;
	CWnd *pWnd = (CWnd*)ppData[0];
	int nX = (int)ppData[1];
	int nY = (int)ppData[2];
	int nWidth = (int)ppData[3];
	int nHeight = (int)ppData[4];
	ASSERT(pWnd);
	pWnd->MoveWindow(nX, nY, nWidth, nHeight);
}

//---------------------------------------------------------------------
// function: 消息对话框
//---------------------------------------------------------------------
void CWndCommand::Command_MessageBox(void *pData)
{
	ASSERT(pData);
	// 消息包含两个数据CWnd*, PCSTR, PCSTR, UINT
	CWnd *pWnd = (CWnd*)((void**)pData)[0];			// 父窗口
	PCSTR pcszCaption = (PCSTR)((void**)pData)[1];	// 标题
	PCSTR pcszMessage = (PCSTR)((void**)pData)[2];	// 消息
	UINT  uType = (UINT)((void**)pData)[3];			// 类型
	ASSERT(pWnd && pcszMessage);
	pWnd->MessageBox(pcszMessage, pcszCaption, uType);
}

//---------------------------------------------------------------------
// function: 命令处理器
//---------------------------------------------------------------------
void CWndCommand::Command_CmdHandler(void *pData)
{
	ASSERT(pData);
	// 消息包含两个数据函数指针和void*数据
	fnWndCommand pCmd = (fnWndCommand)((void**)pData)[0];
	void *pParam = ((void**)pData)[1];
	ASSERT(pCmd);
	pCmd(pParam);
}

//---------------------------------------------------------------------
// function	: 消息循环
//---------------------------------------------------------------------
void CWndCommand::FlushMessages(CWinThread *pApp, HWND hWnd)
{
	ASSERT(pApp);
	MSG msg;
	while (::PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE))
	{
		pApp->PumpMessage();
	}
}
