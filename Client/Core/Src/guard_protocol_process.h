//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-20
//      File_base        : guard_protocol_process
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : Guard相关协议处理
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUARD_PROTOCOL_PROCESS_H_
#define	_GUARD_PROTOCOL_PROCESS_H_

#include "fseye_protocol.h"

extern long g_GameStartTime;

class GuardProtocolProcess
{
public:
	GuardProtocolProcess();
	void ProcessMsg(BYTE* pMsg, int size);

private:
	void (GuardProtocolProcess::*ProcessFunc[fseye_protocol_count])(BYTE* pMsg, int size);

	void SayToWorld(BYTE* pMsg, int size);			//世界消息	
	void GetBasicInfo(BYTE* pMsg, int size);		//得到基本信息
	void ExeGMCmd(BYTE* pMsg, int size);			//执行GM指令
	void PlayerCount(BYTE* pMsg, int size);			//玩家数量
	void Who(BYTE* pMsg, int size);					//列出游戏中的玩家
	void GetGlobalVariable(BYTE* pMsg, int size);	//得到全局变量
	void SetGlobalVariable(BYTE* pMsg, int size);	//设置全局变量
	void GetGameStartTime(BYTE* pMsg, int size);	//得到游戏启动时间
};

extern GuardProtocolProcess g_GuardProtocolProcess;

#endif// _GUARD_PROTOCOL_PROCESS_H_