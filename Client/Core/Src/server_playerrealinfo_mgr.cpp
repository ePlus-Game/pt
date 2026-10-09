#include "KCore.h"
#include "server_playerrealinfo_mgr.h"
#include "KPlayer.h"
#include "CoreRelated.h"
#include "ServerSocialUnitMgr.h"
#include "ChatCenter_S.h"

//Constructor And Destructor
ServerPlayerRealInfoManager::ServerPlayerRealInfoManager()
{
	m_PreDBOperTime = 0;
	m_curDBOper     = enPlayerRealInfoDBOper_None;
	BindFunction();
}

ServerPlayerRealInfoManager::~ServerPlayerRealInfoManager()
{
	m_PreDBOperTime = 0;
	m_curDBOper     = enPlayerRealInfoDBOper_None;
}

void ServerPlayerRealInfoManager::BindFunction()
{
	ZeroMemory(m_ClientReqProcess, sizeof(m_ClientReqProcess));
	ZeroMemory(m_DBRetProcess,     sizeof(m_DBRetProcess)    );

	m_ClientReqProcess[enPlayerRealInfo_SetInfo]   = &ServerPlayerRealInfoManager::SetPlayerRealInfoReq;
	m_ClientReqProcess[enPlayerRealInfo_GetInfo]   = &ServerPlayerRealInfoManager::GetPlayerRealInfoReq;

	m_DBRetProcess[enPlayerRealInfoDBOper_GetInfo] = &ServerPlayerRealInfoManager::GetPlayerRealInfoRet;
	m_DBRetProcess[enPlayerRealInfoDBOper_SetInfo] = &ServerPlayerRealInfoManager::SetPlayerRealInfoRet;
}

//Process Shunt
void ServerPlayerRealInfoManager::ProtocolProcess(int nPlayerIndex, BYTE* nMsg, int nDataSize)
{
	//1. Check Param
	if ( !IsValidPlayer(nPlayerIndex) || !nMsg)
		return ;

	PVARLEN_PROTOCOL_HEADER pProtoc = (PVARLEN_PROTOCOL_HEADER )nMsg;
	if ( pProtoc->subProtocol <= enPlayerRealInfo_None || pProtoc->subProtocol >= enPlayerRealInfo_Num )
		return ;

	//2. Msg Shunt
	if ( m_ClientReqProcess[pProtoc->subProtocol] )
		(this->*m_ClientReqProcess[pProtoc->subProtocol])(nPlayerIndex, nMsg, nDataSize);
}

//Request Functions
void ServerPlayerRealInfoManager::SetPlayerRealInfoReq(int nPlayerIndex, BYTE* nMsg, int nDataSize)
{
	//1. Check Param
	if ( !IsValidPlayer(nPlayerIndex) || !nMsg)
		return ;

	if ( !CheckDBOperInterval() )
		return ;

	//2. 
	PPLAYER_REAL_INFO_SET_REQ_PROTOC pProtoc  = (PPLAYER_REAL_INFO_SET_REQ_PROTOC )nMsg;
	bool bIsValid = IsInfoValid(&pProtoc->playerInfo);

	if ( !bIsValid )
	{
		NotifyClientInfoInValid(nPlayerIndex);
		return ;
	}

	if ( bIsValid)
	{
		PlayerRealInfoDBCenter::Singleton().SetPlayerRealInfo(nPlayerIndex, &(pProtoc->playerInfo));
		m_curDBOper = enPlayerRealInfoDBOper_SetInfo;
	}
}

void ServerPlayerRealInfoManager::GetPlayerRealInfoReq(int nPlayerIndex, BYTE* nMsg, int nDataSize)
{
	//1.
	PPLAYER_REAL_INFO_GET_REQ_PROTOC pProtoc = (PPLAYER_REAL_INFO_GET_REQ_PROTOC )nMsg;
	
	//2.
	char NameBuff[MAXSIZE_ROLENAME];
	ZeroMemory(NameBuff, sizeof(NameBuff));
	bool bIsValid = HasPrivilage(nPlayerIndex, pProtoc->PlayerInfoGet.PlayerGUID, NameBuff);

	if ( CheckDBOperInterval() && bIsValid )
	{	
		PlayerRealInfoDBCenter::Singleton().GetPlayerRealInfo(nPlayerIndex, NameBuff);
		m_curDBOper = enPlayerRealInfoDBOper_GetInfo;
	}
}


//Ret Functions
void ServerPlayerRealInfoManager::GetPlayerRealInfoRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_curDBOper = enPlayerRealInfoDBOper_None;

	//1. Check Param
	if ( !nDbOpeRst )
		return ;

	int nColCount = pRet->GetColCount();
	if ( nColCount != DB_PLAYER_REAL_INFO::GetElemt() )
	{
		_ASSERT(false);
		return ;
	}

	//2. 
	char Buff[sizeof(PLAYER_REAL_INFO_GET_RET_PROTOC) + 256];
	ZeroMemory(Buff, sizeof(Buff));
	PPLAYER_REAL_INFO_GET_RET_PROTOC pProtoc = (PPLAYER_REAL_INFO_GET_RET_PROTOC )Buff;

	pProtoc->protocHeader.protocol    = s2c_palyerinfo_sync;
	pProtoc->protocHeader.subProtocol = enPlayerRealInfo_GetInfo;
	
	//fill data
	PLAYER_REAL_INFO_DOWN& pPlayerData = pProtoc->playerInfo;
	int nCol = 0;
	pRet->GetData(0, nCol++, pPlayerData.Name, sizeof(pPlayerData.Name)				 );
	pRet->GetData(0, nCol++, pPlayerData.Sex										 );
	pRet->GetData(0, nCol++, pPlayerData.Consort, sizeof(pPlayerData.Consort)		 );
	pRet->GetData(0, nCol++, pPlayerData.Age										 );
	pRet->GetData(0, nCol++, pPlayerData.Address, sizeof(pPlayerData.Address)		 );
	pRet->GetData(0, nCol++, pPlayerData.QQNumber, sizeof(pPlayerData.QQNumber)		 );
	pRet->GetData(0, nCol++, pPlayerData.MSNNumber,sizeof(pPlayerData.MSNNumber)	 );
	pRet->GetData(0, nCol++, pPlayerData.ISNumber, sizeof(pPlayerData.ISNumber)		 );
	pRet->GetData(0, nCol++, pPlayerData.UTNumber, sizeof(pPlayerData.UTNumber)		 );
	pRet->GetData(0, nCol++, pPlayerData.TeleNumber, sizeof(pPlayerData.TeleNumber)  );
	pRet->GetData(0, nCol++, pPlayerData.MobleNumber, sizeof(pPlayerData.MobleNumber));	
	pRet->GetData(0, nCol++, pPlayerData.Version									 );
	//fill end

	pProtoc->protocHeader.len = sizeof(PLAYER_REAL_INFO_GET_RET_PROTOC) - PROTOCOL_SIZE;

	//3.
	if ( !CompressProtocol((BYTE* )Buff, pProtoc->protocHeader.len + 1, sizeof(Buff), sizeof(VARLEN_PROTOCOL_HEADER)) )
		return ;

	//4.
	SendDataToClient(nPlayerIdx, pProtoc, pProtoc->protocHeader.len + PROTOCOL_SIZE);
}


//Used Functions
bool ServerPlayerRealInfoManager::HasPrivilage(int nPlayerIndex, FSGUID& guid, char* Name)
{
	if ( !IsValidPlayer(nPlayerIndex) || guid.data[0] == 0)
		return false;

	SocialUnit* pRLeafUnit = GetLeafUnit(nPlayerIndex, enSUTplId_Tong);
	if ( !pRLeafUnit )
		return false;

	SocialUnit* pRUnit = GetUpNUnit(pRLeafUnit, enSULayer_Gens);
	if ( !pRUnit )
		return false;

	ServerSocialUnitMgr& ssum = ServerSocialUnitMgr::Singleton();
	SocialUnit* pLLeafUnit    = ssum.GetUnit(guid, enSUTplId_Tong);
	if ( !pLLeafUnit )
		return false;

	SocialUnit* pLUnit = GetUpNUnit(pLLeafUnit, enSULayer_Gens);
	if ( !pLUnit )
		return false;

	if ( pLUnit != pRUnit )
		return false;

	strncpy(Name, pLLeafUnit->GetOwnerName(), MAXSIZE_ROLENAME);
	Name[MAXSIZE_ROLENAME - 1] = 0;

	return true;
}

bool ServerPlayerRealInfoManager::IsInfoValid(PPLAYER_REAL_INFO_UP pPlayerInfo)
{
	if ( !pPlayerInfo )
		return false;

	//1. Name
	pPlayerInfo->Name[sizeof(pPlayerInfo->Name) - 1] = 0;
	if( !g_IsChatPass((const char* )pPlayerInfo->Name) )
		return false;

	//2. Sex
	if ( pPlayerInfo->Sex != enRoleSex_Male && pPlayerInfo->Sex != enRoleSex_Female)
		return false;
	
	//3. Age
	if ( pPlayerInfo->Age < 0 )
		return false;

	//4. Address
	pPlayerInfo->Address[sizeof(pPlayerInfo->Address) - 1] = 0;
	if ( !g_IsChatPass((const char* )pPlayerInfo->Address) )
		return false;
	
	//5. QQ
	pPlayerInfo->QQNumber[sizeof(pPlayerInfo->QQNumber) - 1] = 0;
	if ( !g_IsChatPass((const char* )pPlayerInfo->QQNumber) )
		return false;

	//6. Msn
	pPlayerInfo->MSNNumber[sizeof(pPlayerInfo->MSNNumber) - 1] = 0;
	if ( !g_IsChatPass((const char* )pPlayerInfo->MSNNumber) )
		return false;

	//7. IS
	pPlayerInfo->ISNumber[sizeof(pPlayerInfo->ISNumber) - 1] = 0;
	if ( !g_IsChatPass((const char* )pPlayerInfo->ISNumber) )
		return false;

	//8. UT
	pPlayerInfo->UTNumber[sizeof(pPlayerInfo->UTNumber) - 1] = 0;
	if ( !g_IsChatPass(pPlayerInfo->UTNumber) )
		return false;
	
	//9. Tele
	pPlayerInfo->TeleNumber[sizeof(pPlayerInfo->TeleNumber) - 1] = 0;
	if ( !g_IsChatPass((const char* )pPlayerInfo->TeleNumber) )
		return false;

	//10. Mobel
	pPlayerInfo->MobleNumber[sizeof(pPlayerInfo->MobleNumber) - 1] = 0;
	if ( !g_IsChatPass((const char* )pPlayerInfo->MobleNumber) )
		return false;

	return true;
}

void ServerPlayerRealInfoManager::NotifyClientInfoInValid(int nPlayerIndex)
{
	char ErrorBuff[100] = { 0 };
	snprintf(ErrorBuff, sizeof(ErrorBuff), "%s", MSG_PLAYER_REAL_INFO_MSG);
	g_ChatCenterS.SysMsgToSomeone(nPlayerIndex, SYSMSG_TYPE_STR, (const BYTE*)ErrorBuff, strlen(ErrorBuff));
}

/*-----------------------PlayerRealInfoDBCenter-----------------------------*/

PlayerRealInfoDBCenter& PlayerRealInfoDBCenter::Singleton()
{
	static PlayerRealInfoDBCenter PRIDBCenter;
	return PRIDBCenter;
}

//DB Request Functions
void PlayerRealInfoDBCenter::GetPlayerRealInfo(int nPlayerIndex, char* PlayerName)
{
	if ( !PlayerName || !IsValidPlayer(nPlayerIndex))
		return ;

	_DBProcHeader DBHeader = { 0 };
	DBHeader.ulNetID  = GetNetConnectIdx(nPlayerIndex);
	DBHeader.ProcType = Proc_PlayerRealInfo;

	IProcParam* pParam = g_pController->GetProcParam();

	//begin push
	pParam->BeginPush( GET_PLAYER_REAL_INFO );

	pParam->Push( PlayerName );

	int nTargetPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(PlayerName);
	if ( IsValidPlayer(nTargetPlayerIndex) )
	{
		int nNpcIndex = Player[nTargetPlayerIndex].GetNpcIndex();
		if ( IsValidNpc(nNpcIndex) )
			pParam->Push( Npc[nNpcIndex].m_nSex );
		else
			return ;
	}
	else
	{
		pParam->Push( NullPair( ) );
	}
	

	//end push
	pParam->EndPush((char* )&DBHeader, sizeof(DBHeader));

	g_pController->CallProc(cfs_db_cnn_global_npcsave, pParam);
}

void PlayerRealInfoDBCenter::SetPlayerRealInfo(int nPlayerIndex, PPLAYER_REAL_INFO_UP pPlayerInfo)
{
	if ( !pPlayerInfo )
		return ;

	if ( !IsValidPlayer(nPlayerIndex) || !IsValidNpc(Player[nPlayerIndex].GetNpcIndex()) )
		return ;

	_DBProcHeader DBHeader = { 0 };
	DBHeader.ulNetID  = GetNetConnectIdx(nPlayerIndex);
	DBHeader.ProcType = Proc_PlayerRealInfo;

	IProcParam* pParam = g_pController->GetProcParam();

	//begin push
	pParam->BeginPush( SET_PLAYER_REAL_INFO );
	
	//Player Game Name
	pParam->Push( Player[nPlayerIndex].GetPlayerName() );

	//1. Player Real Name
	pParam->Push( pPlayerInfo->Name );

	//2. Sex
	pParam->Push( pPlayerInfo->Sex );

	//3. Age
	pParam->Push( pPlayerInfo->Age );

	//4. Address
	pParam->Push( pPlayerInfo->Address );

	//5. QQ
	pParam->Push( pPlayerInfo->QQNumber );

	//6. Msn
	pParam->Push( pPlayerInfo->MSNNumber );

	//7. IS
	pParam->Push( pPlayerInfo->ISNumber );

	//8. UT
	pParam->Push( pPlayerInfo->UTNumber );

	//9. Tele
	pParam->Push( pPlayerInfo->TeleNumber );

	//10. Moble
	pParam->Push( pPlayerInfo->MobleNumber );

	//end push
	pParam->EndPush((char* )&DBHeader, sizeof(DBHeader));

	g_pController->CallProc(cfs_db_cnn_global_npcsave, pParam);

}

void PlayerRealInfoDBCenter::DeletePlayerRealInfo(char* PlayerName)
{
	//1. Check Param
	if ( !PlayerName )
		return ;

	//2.
	_DBProcHeader DBHeader = { 0 };

	DBHeader.ulNetID  = -1;
	DBHeader.ProcType = Proc_PlayerRealInfo;

	IProcParam* pParam = g_pController->GetProcParam();

	//begin push
	pParam->BeginPush( DELETE_PLAYER_REAL_INFO );

	pParam->Push( PlayerName );

	//end push
	pParam->EndPush( (char* )&DBHeader, sizeof(DBHeader));

	g_pController->CallProc(cfs_db_cnn_global_npcsave, pParam);
}
