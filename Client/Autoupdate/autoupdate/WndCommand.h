/**********************************************************************
** Description : 需要在窗口过程中执行的命令
** FileName    : WndCommand.h
** Author      : wangbin
** Datetime    : 2004-05-15 21:40
** Comment     : 使命令在UI线程中执行
**********************************************************************/
// WndCommand.h: interface for the CWndCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDCOMMAND_H__573C859F_53D6_4482_AB34_8F867B8BF321__INCLUDED_)
#define AFX_WNDCOMMAND_H__573C859F_53D6_4482_AB34_8F867B8BF321__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndCommand  
{
public:
	CWndCommand();
	~CWndCommand();
public:
	enum {
		WM_USERCOMMAND = WM_USER + 100,	// 自定义消息ID
		WM_USERAPPQUIT = WM_USER + 101,	// 退出消息
	};
	// 消息协议类型，必须从0开始递增
	enum Command
	{
		COMMAND_DialogDoModal,		// 显示对话框
		COMMAND_ModifyWindowText,	// 修改窗口文本信息
		COMMAND_SetProgressRate,	// 修改进度控件的百分比显示
		COMMAND_ShowWindow,			// 显示窗口
		COMMAND_MoveWindow,			// 移动窗口
		COMMAND_MessageBox,			// 消息对话框 
		COMMAND_CmdHandler,			// 命令处理器
									// Add new protocol here
		COMMAND_EOF,				// 结束标志，必须放置在最后
	};
	// 消息协议处理器
	typedef void (*fnWndCommand)(void*);
	static fnWndCommand m_fnWndCommands[];
	// 消息响应函数
	static void OnMessage(WPARAM wParam, LPARAM lParam);
	// 消息循环
	static void FlushMessages(CWinThread *pApp, HWND hWnd);
private:
	static void Command_DialogDoModal(void *pData);		// 显示对话框
	static void Command_ModifyWindowText(void *pData);	// 修改窗口文本信息
	static void Command_SetProgressRate(void *pData);	// 修改进度控件的百分比显示
	static void Command_ShowWindow(void *pData);		// 显示窗口
	static void Command_MoveWindow(void *pData);		// 移动窗口
	static void Command_MessageBox(void *pData);		// 消息对话框
	static void Command_CmdHandler(void *pData);		// 命令处理器
};

//---------------------------------------------------------------------
// macro	: DIALOG_DOMODAL
// function : 显示对话框
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
#define DIALOG_DOMODAL(pWnd, pDlg, pnRes)						\
	{															\
		void *pCmdMsg[2] = {									\
			(void*)static_cast<CDialog*>(pDlg), (void*)(pnRes)};\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_DialogDoModal,			\
			(LPARAM)&pCmdMsg[0]);								\
	}

//---------------------------------------------------------------------
// macro	: SET_WINDOW_TEXT
// function : 修改窗口文本
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
#define SET_WINDOW_TEXT(pWnd, pCtl, pMsg)						\
	{															\
		void *pCmdMsg[2] = {									\
			(void*)static_cast<CWnd*>(pCtl), (void*)(pMsg)};	\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_ModifyWindowText,		\
			(LPARAM)&pCmdMsg[0]);								\
	}

//---------------------------------------------------------------------
// macro	: SET_WINDOW_TEXT
// function : 修改窗口文本
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
#define SET_PROGRESS_RATE(pWnd, pCtl, nRate)					\
	{															\
		void *pCmdMsg[2] = {									\
			(void*)static_cast<CWnd*>(pCtl), (void*)(nRate)};	\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_SetProgressRate,		\
			(LPARAM)&pCmdMsg[0]);								\
	}

//---------------------------------------------------------------------
// macro	: SHOW_WINDOW
// function : 显示窗口
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
#define SHOW_WINDOW(pWnd, pCtl, nCmdShow)						\
	{															\
		void *pCmdMsg[2] = {									\
			(void*)static_cast<CWnd*>(pCtl), (void*)(nCmdShow)};\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_ShowWindow,			\
			(LPARAM)&pCmdMsg[0]);								\
	}

//---------------------------------------------------------------------
// macro	: MOVE_WINDOW
// function : 移动窗口
// author	: wangbin
// datetime : 2004-05-11
//---------------------------------------------------------------------
#define MOVE_WINDOW(pWnd, pCtl, x, y, nWidth, nHeight)			\
	{															\
		void *pCmdMsg[5] = {									\
			(void*)static_cast<CWnd*>(pCtl),					\
			(void*)(x),											\
			(void*)(y),											\
			(void*)(nWidth),									\
			(void*)(nHeight)};									\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_MoveWindow,			\
			(LPARAM)&pCmdMsg[0]);								\
	}

//---------------------------------------------------------------------
// macro	: MESSAGE_BOX
// function : 消息对话框
// author	: wangbin
// datetime : 2004-05-16
//---------------------------------------------------------------------
#define MESSAGE_BOX(pWnd, pCtl, pCaption, pMessage, nType)		\
	{															\
		void *pCmdMsg[4] = {									\
			(void*)static_cast<CWnd*>(pCtl),					\
			(void*)(pCaption),									\
			(void*)(pMessage),									\
			(void*)(nType)};									\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_MessageBox,			\
			(LPARAM)&pCmdMsg[0]);								\
	}

//---------------------------------------------------------------------
// macro	: CMD_HANDLER
// function : 执行指定命令
// author	: wangbin
// datetime : 2004-05-16
// comment	: 参数pCmd必须是CWndCommand::fnWndCommand类型
//---------------------------------------------------------------------
#define CMD_HANDLER(pWnd, pCmd, pParam)							\
	{															\
		void *pCmdMsg[2] = {									\
			(void*)static_cast<CWndCommand::fnWndCommand>(pCmd),\
			(void*)(pParam)};									\
		(pWnd)->SendMessage(									\
			CWndCommand::WM_USERCOMMAND,						\
			(WPARAM)CWndCommand::COMMAND_CmdHandler,			\
			(LPARAM)&pCmdMsg[0]);								\
	}

#endif // !defined(AFX_WNDCOMMAND_H__573C859F_53D6_4482_AB34_8F867B8BF321__INCLUDED_)
