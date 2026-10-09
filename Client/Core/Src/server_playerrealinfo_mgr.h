//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2008
//
//      Created_datetime : 2008-11-29   
//      File_base        : Server_PlayerRealInfo_Mgr
//      File_ext         : h
//      Author           : Wu Shaohui
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#ifndef _SERVER_PLAYER_REAL_INFO_MGR_H_
#define _SERVER_PLAYER_REAL_INFO_MGR_H_
#include "playerrealinfocomdef.h"

#define DB_OP_INTERVAL_TIME 3

class ServerPlayerRealInfoManager
{
	public:
		ServerPlayerRealInfoManager();
		~ServerPlayerRealInfoManager();

		void ProtocolProcess(int nPlayerIndex, BYTE* nMsg, int nDataSize);
		void ProcessDBRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);

	private:
		//Request
		void SetPlayerRealInfoReq(int nPlayerIndex, BYTE* nMsg, int nDataSize);
		void GetPlayerRealInfoReq(int nPlayerIndex, BYTE* nMsg, int nDataSize);

		//Ret
		void GetPlayerRealInfoRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);
		void SetPlayerRealInfoRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet);

		//func
		bool CheckDBOperInterval();
		void BindFunction();
		bool HasPrivilage(int nPlayerIndex, FSGUID& guid, char* Name);
		bool IsInfoValid(PPLAYER_REAL_INFO_UP pPlayerInfo);
		void NotifyClientInfoInValid(int nPlayerIndex);

	private:
		typedef void (ServerPlayerRealInfoManager::*ReqFuncType)(int nPlayerIndex, BYTE* nMsg, int nDataSize);
		typedef void (ServerPlayerRealInfoManager::*RetFuncType)(int nDbOpeRst, int nPlayerIndex, IProcRet* pRet);

		ReqFuncType m_ClientReqProcess[enPlayerRealInfo_Num];
		RetFuncType m_DBRetProcess[enPlayerRealInfoDBOper_Num];

	private:
		DWORD m_PreDBOperTime;
		enPlayerRealInfoDBOper m_curDBOper;
};

class PlayerRealInfoDBCenter
{
public:
	static PlayerRealInfoDBCenter& Singleton();
	
	//DB Operation
	void SetPlayerRealInfo(int nPlayerIndex, PPLAYER_REAL_INFO_UP pPlayerInfo);
	void GetPlayerRealInfo(int nPlayerIndex, char* PlayerName);
	void DeletePlayerRealInfo(char* PlayerName);
	
private:
	PlayerRealInfoDBCenter(){};
	PlayerRealInfoDBCenter(const PlayerRealInfoDBCenter& PDB){};
	PlayerRealInfoDBCenter& operator= (const PlayerRealInfoDBCenter &rhs){};
};

inline void ServerPlayerRealInfoManager::ProcessDBRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	if ( m_curDBOper > enPlayerRealInfoDBOper_None && m_curDBOper < enPlayerRealInfoDBOper_Num && m_DBRetProcess[m_curDBOper] )
		(this->*m_DBRetProcess[m_curDBOper])(nDbOpeRst, nPlayerIdx, pRet);
}

inline void ServerPlayerRealInfoManager::SetPlayerRealInfoRet(int nDbOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_curDBOper = enPlayerRealInfoDBOper_None;
}

inline bool ServerPlayerRealInfoManager::CheckDBOperInterval()
{
	if ( m_PreDBOperTime - UNIX_TMIE_STAMP > DB_OP_INTERVAL_TIME )
	{
		m_PreDBOperTime = UNIX_TMIE_STAMP;
		return true;
	}
	else
	{
		return false;
	}
}

#endif