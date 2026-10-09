#include "KCore.h"
#include "CoreRelated.h"
#include "client_playerrealinfo_mgr.h"

BOOL ClientPlayerRealInfoManager::SendRequestToServer(void* pData, enPlayerRealInfoOper enOper)
{
	BOOL nRet = FALSE;

	//1. Check Param
	if ( !pData )
		return nRet;

	if ( !IsValidPRIOper(enOper) )
		return nRet;

	//2. Shunt Request
	switch ( enOper )
	{
	case enPlayerRealInfoOper_GetInfo:
		{
			nRet = GetPlayerRealInfoReq((UIPlayerRealInfoGet* )pData);		
		}
		break;
	case enPlayerRealInfoOper_SetInfo:
		{
			nRet = SetPlayerRealInfoReq((UIPlayerRealInfo* )pData);
		}
		break;
	default:
		{
			//nothing 
		}
		break;
	}

	return nRet;
}

void ClientPlayerRealInfoManager::ProtocolProcess(BYTE* pMsg)
{
	//1. Check Param
	if ( !pMsg )
		return ;

	PVARLEN_PROTOCOL_HEADER pProtoc = (PVARLEN_PROTOCOL_HEADER )pMsg;

	//2. Msg Shunt
	switch ( (enPlayerRealInfoSubProtocol)pProtoc->subProtocol )
	{
	case enPlayerRealInfo_GetInfo:
		{
			GetPlayerRealInfoRet(pMsg);
		}
		break;
	case enPlayerRealInfo_SetInfo:
		{
			//nothing
		}
		break;
	default:
		{
			//nothing
		}
		break;
	}

	return ;
}


BOOL ClientPlayerRealInfoManager::GetPlayerRealInfoReq(UIPlayerRealInfoGet* pPlayerInfoGet)
{
	//1. Check Param
	if ( !pPlayerInfoGet )
		return FALSE;

	//2. Fill ProtocolData
	PLAYER_REAL_INFO_GET_REQ_PROTOC c2sPlayerInfoProtoc;

	c2sPlayerInfoProtoc.protocHeader.protocol      = c2s_player_real_info_sync;
	c2sPlayerInfoProtoc.protocHeader.subProtocol   = enPlayerRealInfo_GetInfo;
	memcpy(&c2sPlayerInfoProtoc.PlayerInfoGet.PlayerGUID, &pPlayerInfoGet->PlayerGUID, sizeof(c2sPlayerInfoProtoc.PlayerInfoGet.PlayerGUID));
	c2sPlayerInfoProtoc.PlayerInfoGet.PlayerGUID.data[sizeof(c2sPlayerInfoProtoc.PlayerInfoGet.PlayerGUID) - 1] = 0;
	c2sPlayerInfoProtoc.protocHeader.len           = sizeof(c2sPlayerInfoProtoc) - PROTOCOL_SIZE;

	//3. Send Data
	SendDataToServer(&c2sPlayerInfoProtoc, c2sPlayerInfoProtoc.protocHeader.len + PROTOCOL_SIZE);
	return TRUE;
}

BOOL ClientPlayerRealInfoManager::SetPlayerRealInfoReq(UIPlayerRealInfo* pUIPlayerInfo)
{
	//1. Check Param
	if ( !pUIPlayerInfo )
		return FALSE;

	//2. Fill ProtocolData And Check Text
	PLAYER_REAL_INFO_SET_REQ_PROTOC c2sPlayerInfoProtoc;

	c2sPlayerInfoProtoc.protocHeader.protocol	 = c2s_player_real_info_sync;
	c2sPlayerInfoProtoc.protocHeader.subProtocol = enPlayerRealInfo_SetInfo;
	
	char cReplace = '*';
	c2sPlayerInfoProtoc.playerInfo.Age			 = pUIPlayerInfo->Age;
	c2sPlayerInfoProtoc.playerInfo.Sex			 = pUIPlayerInfo->Sex;

	strncpy(c2sPlayerInfoProtoc.playerInfo.Name, pUIPlayerInfo->Name, sizeof(c2sPlayerInfoProtoc.playerInfo.Name));
	c2sPlayerInfoProtoc.playerInfo.Name[sizeof(c2sPlayerInfoProtoc.playerInfo.Name) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.Name[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.Name, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.Address, pUIPlayerInfo->Address, sizeof(c2sPlayerInfoProtoc.playerInfo.Address));
	c2sPlayerInfoProtoc.playerInfo.Address[sizeof(c2sPlayerInfoProtoc.playerInfo.Address) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.Address[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.Address, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.QQNumber, pUIPlayerInfo->QQNumber, sizeof(c2sPlayerInfoProtoc.playerInfo.QQNumber));
	c2sPlayerInfoProtoc.playerInfo.QQNumber[sizeof(c2sPlayerInfoProtoc.playerInfo.QQNumber) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.QQNumber[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.QQNumber, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.MSNNumber, pUIPlayerInfo->MSNNumber, sizeof(c2sPlayerInfoProtoc.playerInfo.MSNNumber));
	c2sPlayerInfoProtoc.playerInfo.MSNNumber[sizeof(c2sPlayerInfoProtoc.playerInfo.MSNNumber) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.MSNNumber[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.MSNNumber, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.ISNumber, pUIPlayerInfo->ISNumber, sizeof(c2sPlayerInfoProtoc.playerInfo.ISNumber));
	c2sPlayerInfoProtoc.playerInfo.ISNumber[sizeof(c2sPlayerInfoProtoc.playerInfo.ISNumber) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.ISNumber[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.ISNumber, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.UTNumber, pUIPlayerInfo->UTNumber, sizeof(c2sPlayerInfoProtoc.playerInfo.UTNumber));
	c2sPlayerInfoProtoc.playerInfo.UTNumber[sizeof(c2sPlayerInfoProtoc.playerInfo.UTNumber) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.UTNumber[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.UTNumber, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.TeleNumber, pUIPlayerInfo->TeleNumber, sizeof(c2sPlayerInfoProtoc.playerInfo.TeleNumber));
	c2sPlayerInfoProtoc.playerInfo.TeleNumber[sizeof(c2sPlayerInfoProtoc.playerInfo.TeleNumber) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.TeleNumber[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.TeleNumber, cReplace) )
		return FALSE;

	strncpy(c2sPlayerInfoProtoc.playerInfo.MobleNumber, pUIPlayerInfo->MobleNumber, sizeof(c2sPlayerInfoProtoc.playerInfo.MobleNumber));
	c2sPlayerInfoProtoc.playerInfo.MobleNumber[sizeof(c2sPlayerInfoProtoc.playerInfo.MobleNumber) - 1] = 0;
	if ( c2sPlayerInfoProtoc.playerInfo.MobleNumber[0] && !g_FilterChatText(c2sPlayerInfoProtoc.playerInfo.MobleNumber, cReplace) )
		return FALSE;
	

	c2sPlayerInfoProtoc.protocHeader.len         = sizeof(c2sPlayerInfoProtoc) - PROTOCOL_SIZE;

	//3. Send Data
	SendDataToServer(&c2sPlayerInfoProtoc, c2sPlayerInfoProtoc.protocHeader.len + PROTOCOL_SIZE);

	return TRUE;
}

void ClientPlayerRealInfoManager::GetPlayerRealInfoRet(BYTE* pMsg)
{
	//1. Check Param
	if ( !pMsg )
		return ;

	//2.Decompress Data
	char Buff[sizeof(PLAYER_REAL_INFO_GET_RET_PROTOC) + 256];
	if ( !DecompressProtocol(pMsg, (BYTE* )Buff, sizeof(Buff), sizeof(VARLEN_PROTOCOL_HEADER)) )
		return ;

	//3. fill UI used Data
	PPLAYER_REAL_INFO_GET_RET_PROTOC pProtoc = (PPLAYER_REAL_INFO_GET_RET_PROTOC )Buff;
	PLAYER_REAL_INFO_DOWN& pPlayerInfo    = pProtoc->playerInfo;
	char tBuff[sizeof(UIPlayerRealInfoEx)];
	ZeroMemory(tBuff, sizeof(UIPlayerRealInfoEx));
	UIPlayerRealInfoEx* TempUIPlayerRealInfo = (UIPlayerRealInfoEx* )tBuff;

	TempUIPlayerRealInfo->Age			 = pPlayerInfo.Age;
	TempUIPlayerRealInfo->Sex			 = pPlayerInfo.Sex;
	strncpy(TempUIPlayerRealInfo->Name, pPlayerInfo.Name, sizeof(TempUIPlayerRealInfo->Name));
	TempUIPlayerRealInfo->Name[sizeof(TempUIPlayerRealInfo->Name) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->Consort, pPlayerInfo.Consort, sizeof(TempUIPlayerRealInfo->Consort));
	TempUIPlayerRealInfo->Consort[sizeof(TempUIPlayerRealInfo->Consort) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->Address, pPlayerInfo.Address, sizeof(TempUIPlayerRealInfo->Address));
	TempUIPlayerRealInfo->Address[sizeof(TempUIPlayerRealInfo->Address) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->QQNumber, pPlayerInfo.QQNumber, sizeof(TempUIPlayerRealInfo->QQNumber));
	TempUIPlayerRealInfo->QQNumber[sizeof(TempUIPlayerRealInfo->QQNumber) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->MSNNumber, pPlayerInfo.MSNNumber, sizeof(TempUIPlayerRealInfo->MSNNumber));
	TempUIPlayerRealInfo->MSNNumber[sizeof(TempUIPlayerRealInfo->MSNNumber) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->ISNumber, pPlayerInfo.ISNumber, sizeof(TempUIPlayerRealInfo->ISNumber));
	TempUIPlayerRealInfo->ISNumber[sizeof(TempUIPlayerRealInfo->ISNumber) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->UTNumber, pPlayerInfo.UTNumber, sizeof(TempUIPlayerRealInfo->UTNumber));
	TempUIPlayerRealInfo->UTNumber[sizeof(TempUIPlayerRealInfo->UTNumber) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->TeleNumber, pPlayerInfo.TeleNumber, sizeof(TempUIPlayerRealInfo->TeleNumber));
	TempUIPlayerRealInfo->TeleNumber[sizeof(TempUIPlayerRealInfo->TeleNumber) - 1] = 0;
	strncpy(TempUIPlayerRealInfo->MobleNumber, pPlayerInfo.MobleNumber, sizeof(TempUIPlayerRealInfo->MobleNumber));
	TempUIPlayerRealInfo->MobleNumber[sizeof(TempUIPlayerRealInfo->MobleNumber) - 1] = 0;

	//4. Notify UI
	CoreDataChanged(GDCNI_PLAYER_REAL_INFO, (unsigned int)(tBuff), 0);
}