//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/22/2006 15:47
//      File_base        : NetConnectAgent
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
// 		Engine里的KNetClient模块包装实现了网络联接与传送包，
// 	此模块为KNetClient具体应用时的代理，	主要用于汇集具
// 	体应用中需要发送的网络包，以及把抵达的网络包派送到各相
// 	关处理接受模块。
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef IFNETCONNECTAGENT_H
#define IFNETCONNECTAGENT_H

#include "KProtocol.h"
#include "networkinterface.h"
#include "../Login/Login.h"

struct iKNetMsgTargetObject;

class KNetConnectAgent
{
	friend void  KLogin::ClientCallBack( void *lpParam, const unsigned long uInID, const unsigned long ulnEventType );
public:
	KNetConnectAgent();
	~KNetConnectAgent();
public:
	int						Initialize				(																		);
	void					Breathe					( void																	);
	void					Destroy					(																		);
	
public:
	int						ConnectToGameSvr		( const unsigned char* pIpAddress, unsigned short uPort, GUID* pGuid	);
	void					DisconnectGameSvr		( void																	);
	int						SendMsg					( const void *pBuffer, int nSize										);
	void					UpdateClientRequestTime	( bool bCancel, unsigned int uTimeLimit = DEF_TIMEOUT_LIMIT				);
	void					RegisterMsgTargetObject	( PROTOCOL_MSG_TYPE Msg, iKNetMsgTargetObject* pObject					);
	bool					IsConnecting			( void																	);
	void					Updata					();
private:
	void					TobeDisconnect			( void																	);
	void					ProcessGameServerData	( const char *pBuffer, unsigned int nSize								);

private:
	bool					m_bIsGameServConnecting;			//!< Is connected to game server.
	bool					m_bTobeDisconnect;					//!< Is to be disconnect.
	UINT					m_uClientRequestTime;				//!< Send request time.
	UINT					m_uClientTimeoutLimit;				//!< Current operation time limit.
	IClient*				m_pGameSvrClient;					//!< Client interface
	iKNetMsgTargetObject*	m_MsgTargetObjs[MAX_PROTOCOL_NUM];	//!< Message target array.
	int						m_nConnectID;						//!< Connect ID.
};

extern KNetConnectAgent g_NetConnectAgent;

#endif
