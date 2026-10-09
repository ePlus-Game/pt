/**********************************************************************
** Description : 升级界面回调接口
** FileName    : WndCallBack.h
** Author      : wangbin
** Datetime    : 2004-05-18 15:40
** Comment     : 
**********************************************************************/
#ifndef __WNDCALLBACK_H__
#define __WNDCALLBACK_H__

/*
// 按钮命令
enum ButtonCmd
{
BTNCMD_CANCEL,	// 取消
BTNCMD_ENTER,	// 进入游戏
BTNCMD_RETRY,	// 重试
BTNCMD_EOF
};
*/
class CWndCallBack
{
public:
	virtual void NotifyProcessStatus(LPCSTR pcszMessage) = 0;	// 状态消息
	virtual void NotifyCurrentRate(int nRate) = 0;				// 当前进度
	virtual void NotifyOverallRate(int nRate) = 0;				// 整体进度
	virtual void NotifyClose(int nResult) = 0;					// 关闭
	virtual void NotifyResult(BOOL bSuccess, BOOL bCanPlay) = 0;				// 结果通知，TRUE-成功;FALSE-失败
	virtual void NotifyVersion(int nMajor, int nMinor) = 0;		// 版本号通知
//	virtual void NotifyServerList(void)=0;                      //服务器列表下载完毕
	virtual void NotifyImportant()=0;							//服务器不可用的通知
};

class CWndShell
{
public:
	virtual int Refresh(int nStatus, long lParam) = 0;
};

#endif //__WNDCALLBACK_H__
