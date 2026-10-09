//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/22/2006 15:57
//      File_base        : NetConnectAgent
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include <crtdbg.h>
#include "KEngine.h"
#include "NetConnectAgent.h"
#include "NetMsgTargetObject.h"
#include "coreshell.h"
#include "KProtocolDef.h"
#include "cfs_filelogs.h"

int			g_bDisconnect;
extern iCoreShell*	g_pCoreShell;
KNetConnectAgent	g_NetConnectAgent;

#define DEFAULT_MAXCLIENT 5

/************************************************************************/
/*					Construction function                               */
/************************************************************************/
KNetConnectAgent::KNetConnectAgent( void )
{
	memset(&m_MsgTargetObjs, 0, sizeof(m_MsgTargetObjs));
	m_pGameSvrClient		= NULL;
	m_bTobeDisconnect		= false;
	m_uClientRequestTime	= 0;
}

/************************************************************************/
/*				  Destruction function                                  */
/************************************************************************/
KNetConnectAgent::~KNetConnectAgent( void )
{
}

/************************************************************************/
/*				Initialize KNetConnectAgent object                      */
/************************************************************************/
int KNetConnectAgent::Initialize( void )
{
	DisconnectGameSvr();

	CreateClient( m_pGameSvrClient, DEFAULT_MAXCLIENT );
    
    if ( !m_pGameSvrClient )
	{
        return false;
	}

	if ( FAILED( m_pGameSvrClient->Startup() ) )
	{
		return false;
	}

	m_pGameSvrClient->RegisterMsgFilter( (void*)true, KLogin::ClientCallBack );
	return true;
}

/************************************************************************/
/* Keep KNetConnectAgent obj Breathe, so that it can process net msg.	*/
/************************************************************************/
void KNetConnectAgent::Breathe( void )
{
	unsigned int nSize	= 0;
	const char* pBuffer = NULL;

	// if time out
	if ( m_uClientRequestTime && ((::GetTickCount() - m_uClientRequestTime) >= m_uClientTimeoutLimit) )
	{
		g_bDisconnect		= true;
		m_bTobeDisconnect	= true;
	}

	// if drop from server
	if ( m_bTobeDisconnect )
	{
		m_bTobeDisconnect = false;
		DisconnectGameSvr();
		return;
	}

	// process net message
	if ( m_bIsGameServConnecting && m_pGameSvrClient )
	{
		while ( true )
		{          
			if ( !m_pGameSvrClient )
			{
				break;
			}
			pBuffer = (const char*)m_pGameSvrClient->GetPackFromServer( m_nConnectID, nSize);
			if ( !(pBuffer && nSize) )
			{
				break;
			}
			ProcessGameServerData(pBuffer, nSize);
		}// end while

	}// end if ( m_bIsGameServConnecting && m_pGameSvrClient )
}

/************************************************************************/
/*					Stop KNetConnectAgent obj                           */
/************************************************************************/
void KNetConnectAgent::Destroy( void )
{
	memset(&m_MsgTargetObjs, 0, sizeof(m_MsgTargetObjs));
	
	DisconnectGameSvr();

    if (m_pGameSvrClient)
    {
		m_pGameSvrClient->Cleanup();
        m_pGameSvrClient->Release();
        m_pGameSvrClient = NULL;
    }
}



/************************************************************************/
/*				Connect to game server                                  */
/************************************************************************/
int KNetConnectAgent::ConnectToGameSvr( const unsigned char* pIpAddress, unsigned short uPort, GUID* pGuid )
{
	if ( !pIpAddress || !uPort || m_pGameSvrClient == NULL )
	{
        return false;
	}

	char	Address[128];
	sprintf( Address, "%d.%d.%d.%d", pIpAddress[0], pIpAddress[1], pIpAddress[2], pIpAddress[3] );
	
	m_nConnectID = m_pGameSvrClient->ConnectTo( Address, uPort );
	if ( m_nConnectID == -1 )
	{
		return false;
	}

	m_bIsGameServConnecting = true;

	if ( g_pCoreShell )
	{
		g_pCoreShell->SetClient(m_pGameSvrClient,m_nConnectID);
	}

	return true;
}

/************************************************************************/
/*				Disconnect to game server                               */
/************************************************************************/
void KNetConnectAgent::DisconnectGameSvr()
{
	if ( m_pGameSvrClient && m_bIsGameServConnecting )
	{
		if ( g_pCoreShell )
		{
			g_pCoreShell->SetClient(NULL,-1);
		}

		m_bIsGameServConnecting = false;
		m_pGameSvrClient->Shutdown();
	}    
}

/************************************************************************/
/*						Send net msg to game server                     */
/************************************************************************/
int KNetConnectAgent::SendMsg( const void *pBuffer, int nSize )
{
	if ( m_pGameSvrClient && m_bIsGameServConnecting )
	{
		m_pGameSvrClient->SendPackToServer( m_nConnectID, (BYTE*)pBuffer, nSize );
		m_pGameSvrClient->FlushData();
		return true;
	}
	return false;
}


/************************************************************************/
/*		Register net msg callback function                              */
/************************************************************************/
void KNetConnectAgent::RegisterMsgTargetObject(PROTOCOL_MSG_TYPE Msg, iKNetMsgTargetObject* pObject)
{
	if ( Msg >= 0 && Msg < MAX_PROTOCOL_NUM )
		m_MsgTargetObjs[Msg] = pObject;
}

/************************************************************************/
/*		Nodify KNetConnectAgent obj to update current time              */
/************************************************************************/
void KNetConnectAgent::UpdateClientRequestTime(bool bCancel, unsigned int uTimeLimit)
{
	if ( m_bIsGameServConnecting )
	{
		if ( bCancel == false )
		{
			m_uClientRequestTime = GetTickCount();
			if ( m_uClientRequestTime == 0 )
			{
				m_uClientRequestTime = 1;
			}
			m_uClientTimeoutLimit = uTimeLimit;
		}
		else
		{
			m_uClientRequestTime = 0;
		}
	}
}

/************************************************************************/
/*		Is KNetConnectAgent obj connected					            */
/************************************************************************/
bool	KNetConnectAgent::IsConnecting( void )
{
	return m_bIsGameServConnecting;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
void KNetConnectAgent::TobeDisconnect()
{
	m_bTobeDisconnect = true;
}

/************************************************************************/
/*              Process net msg function to callback function			*/
/************************************************************************/
void KNetConnectAgent::ProcessGameServerData(const char *pBuffer, unsigned int nSize)
{
    PROTOCOL_MSG_TYPE*	pMsg = (PROTOCOL_MSG_TYPE*)pBuffer;
	while(pMsg < (PROTOCOL_MSG_TYPE*)(pBuffer + nSize))
	{
		PROTOCOL_MSG_TYPE	Msg = pMsg[0];		
		if ( m_MsgTargetObjs[Msg] )
		{
			m_MsgTargetObjs[Msg]->AcceptNetMsg( (void*)pBuffer, nSize );
			break;
		}
		else
		{
			if (g_pCoreShell)
			{
				_ASSERT(Msg > s2c_clientbegin && Msg < s2c_end);
				_ASSERT(g_pCoreShell->GetProtocolSize(Msg) != 0);
				g_pCoreShell->NetMsgCallbackFunc( pMsg );

				if ( g_pCoreShell->GetProtocolSize( Msg ) > 0 )
				{
					pMsg = (PROTOCOL_MSG_TYPE*)(((char*)pMsg) + g_pCoreShell->GetProtocolSize( Msg ));
				}
				else
				{
					pMsg = (PROTOCOL_MSG_TYPE*)(((char*)pMsg) + PROTOCOL_MSG_SIZE + (*(unsigned short*) (((char*)pMsg) + PROTOCOL_MSG_SIZE)));
				}
			}
		}
	}
}

