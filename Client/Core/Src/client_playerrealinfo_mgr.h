//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2008
//
//      Created_datetime : 2008-11-29   
//      File_base        : Client_PlayerRealInfo_Mgr
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
#ifndef _CLIENT_PALYER_REAL_INFO_MGR_H_
#define _CLIENT_PALYER_REAL_INFO_MGR_H_
#include "playerrealinfocomdef.h"

class ClientPlayerRealInfoManager
{
	public:
		BOOL SendRequestToServer(void* pData, enPlayerRealInfoOper enOper);
		void ProtocolProcess(BYTE* pMsg);

	private:
		//Request for Server
		BOOL GetPlayerRealInfoReq(UIPlayerRealInfoGet* pPlayerInfoGet);	//
		BOOL SetPlayerRealInfoReq(UIPlayerRealInfo* pUIPlayerInfo    );	//

		//Ret from Server
		void GetPlayerRealInfoRet(BYTE* pMsg);
};
#endif