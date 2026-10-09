//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-20
//      File_base        : guard_protocol_process
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : Guard相关协议处理
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KPlayerSet.h"
#include "KSubWorld.h"
#include "ChatCenter_S.h"
#include "guard_protocol_process.h"
#include "ConfigManager.h"

GuardProtocolProcess g_GuardProtocolProcess;

GuardProtocolProcess::GuardProtocolProcess()
{
	memset(ProcessFunc, 0, sizeof(ProcessFunc));

	ProcessFunc[e2l_SayToWorld_def] = &GuardProtocolProcess::SayToWorld;
	ProcessFunc[e2l_GetBasicInfo_def] = &GuardProtocolProcess::GetBasicInfo;
	ProcessFunc[e2l_ExeGMCmd_def] = &GuardProtocolProcess::ExeGMCmd;	
	ProcessFunc[e2l_PlayerCount_def] = &GuardProtocolProcess::PlayerCount;
	ProcessFunc[e2l_Who_def] = &GuardProtocolProcess::Who;
	ProcessFunc[e2l_GetGlobalVariable_def] = &GuardProtocolProcess::GetGlobalVariable;
	ProcessFunc[e2l_SetGlobalVariable_def] = &GuardProtocolProcess::SetGlobalVariable;
	ProcessFunc[e2l_GetGameStartTime_def] = &GuardProtocolProcess::GetGameStartTime;
}

void GuardProtocolProcess::ProcessMsg(BYTE* pMsg, int size)
{
	_ASSERT(pMsg);
	
	if (size >= sizeof(e2l_header) + sizeof(unsigned short))
	{
		unsigned short* protocol = (unsigned short*)(pMsg + sizeof(e2l_header));
		if (ProcessFunc[*protocol])
		{
			(this->*ProcessFunc[*protocol])(pMsg, size);
		}
	}
	else
	{
		_ASSERT(false);
	}
}

void GuardProtocolProcess::SayToWorld(BYTE* pMsg, int size)
{
	e2l_SayToWorld* pSayToWorld = (e2l_SayToWorld*)pMsg;
	
	g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, (BYTE*)(pSayToWorld->Message), strlen(pSayToWorld->Message));
}

void GuardProtocolProcess::GetBasicInfo(BYTE* pMsg, int size)
{
	l2e_GetBasicInfo getBasicInfo;
	getBasicInfo.Header.Protocol = l2e_header_def;
	getBasicInfo.Protocol = l2e_GetBasicInfo_def;
	getBasicInfo.PlayerCount = PlayerSet.GetPlayerNumber();
	getBasicInfo.UpTime = SubWorld[0].m_dwCurrentTime / GAME_FPS;
	if (g_pController != NULL)
		g_pController->PushData(protocol_type_guard, NULL, &getBasicInfo, sizeof(getBasicInfo));
}

void GuardProtocolProcess::ExeGMCmd(BYTE* pMsg, int size)
{
	//执行GM指令
	const e2l_ExeGMCmd* pExeGMCmd = (e2l_ExeGMCmd*)pMsg;	
	
	char command[sizeof(pExeGMCmd->Command)];
	memcpy(command, pExeGMCmd->Command, sizeof(pExeGMCmd->Command));
	command[sizeof(command) - 1] = 0;
	int playerIndex = 0;
	if (pExeGMCmd->PlayerName[0])
	{
		playerIndex = PlayerSet.GetPlayerIndexByPlayerName(pExeGMCmd->PlayerName);
		if (0 == playerIndex)
		{
			playerIndex = -1;
		}
	}

	int returnCode = l2e_ExeGMCmd_err;
	if (playerIndex > -1)
	{
		BOOL result = TextGMFilter(playerIndex, command, strlen(command));
		int returnCode = (TRUE == result ? fseye_success : l2e_ExeGMCmd_err);
		
		//记录日志
		if (g_pLogSystem)
		{
			char exeGMInfo[sizeof(pExeGMCmd->Command)];
			memcpy(exeGMInfo, pExeGMCmd->Command, sizeof(pExeGMCmd->Command));
			exeGMInfo[sizeof(exeGMInfo) - 1] = 0;
			g_pLogSystem->SysDbgLog(exeGMInfo, strlen(exeGMInfo), sys_dbg_log_event_exe_guard_gm_cmd);
		}
	}
	
	//返回GM指令执行结果
	l2e_ExeGMCmd exeGMCmdResult;
	exeGMCmdResult.Header.Protocol = l2e_header_def;
	exeGMCmdResult.Protocol = l2e_ExeGMCmd_def;
	exeGMCmdResult.ReturnCode = returnCode;	
	if (g_pController != NULL)
		g_pController->PushData(protocol_type_guard, NULL, &exeGMCmdResult, sizeof(exeGMCmdResult));
}

void GuardProtocolProcess::PlayerCount(BYTE* pMsg, int size)
{
	l2e_PlayerCount playerCount;
	playerCount.Header.Protocol = l2e_header_def;
	playerCount.Protocol = l2e_PlayerCount_def;
	playerCount.PlayerCount = PlayerSet.GetPlayerNumber();
	if (g_pController != NULL)
		g_pController->PushData(protocol_type_guard, NULL, &playerCount, sizeof(playerCount));
}

void GuardProtocolProcess::Who(BYTE* pMsg, int size)
{
	const e2l_Who* pWho = (e2l_Who*)pMsg;
	int startOffset = pWho->Offset;	

	l2e_Who who;
	memset(&who, 0, sizeof(who));
	who.Header.Protocol = l2e_header_def;
	who.Protocol = l2e_Who_def;

	int endOffset = startOffset + sizeof(who.PlayerList) / sizeof(l2e_Who_PlayerInfo);
	
	int playerCount = 0;
	int playerIndex = PlayerSet.GetFirstPlayer();
	for (int i = 0; i < endOffset; i++)
	{
		if (playerIndex > 0)
		{
			if (i >= startOffset)
			{
				KPlayer& player = Player[playerIndex];
				
				l2e_Who_PlayerInfo& info = who.PlayerList[playerCount++];
				strcpy(info.Name, Npc[player.GetNpcIndex()].Name);
			}
			
			playerIndex = PlayerSet.GetNextPlayer();
		}
		else
		{
			break;
		}
	}

	who.PlayerCount = playerCount;

	if (g_pController != NULL)
		g_pController->PushData(protocol_type_guard, NULL, &who, sizeof(who));
}

void GuardProtocolProcess::GetGlobalVariable(BYTE* pMsg, int size)
{
	const e2l_GetGlobalVariable* pProtocol = (e2l_GetGlobalVariable*)pMsg;	

	l2e_GetGlobalVariable returnProtocol;
	returnProtocol.Header.Protocol = l2e_header_def;
	returnProtocol.Protocol = l2e_GetGlobalVariable_def;
	returnProtocol.VariableIndex = pProtocol->VariableIndex;
	returnProtocol.VariableValue = ConfigManager::Singleton().GetGlobalVariable((enumGlobalVariable)pProtocol->VariableIndex);

	if (g_pController != NULL)
		g_pController->PushData(protocol_type_guard, NULL, &returnProtocol, sizeof(returnProtocol));
}

void GuardProtocolProcess::SetGlobalVariable(BYTE* pMsg, int size)
{
	const e2l_SetGlobalVariable* pProtocol = (e2l_SetGlobalVariable*)pMsg;	
	ConfigManager::Singleton().SetGlobalVariable((enumGlobalVariable)pProtocol->VariableIndex, pProtocol->VariableValue);
}

void GuardProtocolProcess::GetGameStartTime(BYTE* pMsg, int size)
{
	long runningTime = UNIX_TMIE_STAMP - g_GameStartTime;

	l2e_GetGameStartTime returnProtocol;
	returnProtocol.Header.Protocol = l2e_header_def;
	returnProtocol.Protocol = l2e_GetGameStartTime_def;
	memset(returnProtocol.GameStartTime, 0, sizeof(returnProtocol.GameStartTime));
	sprintf(returnProtocol.GameStartTime, "%d", runningTime);

	if (g_pController != NULL)
		g_pController->PushData(protocol_type_guard, NULL, &returnProtocol, sizeof(returnProtocol));
}
