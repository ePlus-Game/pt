//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/21/2006 15:25
//      File_base        : Login
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include <crtdbg.h>
#include "KEngine.h"
#include "CoreUseNameDef.h"
#include "Login.h"
#include "PinCrypt.h"
#include "CoreShell.h"
#include "Shlwapi.h"
#include <direct.h>
#include "Faith.h"
#include "..\Ui\UiAdapter.h"
#include "..\Ui\UiCase\UiLoginBG.h"
#include "..\Ui\UiCase\UiUpdateTip.h"
#include "..\Ui\UiCase\UiLogin.h"
#include "..\Ui\UiCase\UiSelPlayer.h"
#include "..\Ui\UiCase\UiNewPlayer.h"
#include "..\Ui\UiCase\UiToolsControlBar.h"
#include "..\Ui\UiCase\UiChatWindow.h"
#include "..\Ui\UiCase\UiAutoConnect.h"
#include "..\Ui\UiCase\UiWaitingMsg.h"
#include "..\Ui\UiCase\UiChatWindow.h"
#include "..\Ui\UiCase\UiDelayQuit.h"
#include "..\Ui\UiCase\UiTrafficLight.h"
#include "..\Ui\UiCase\UiTaisuiWnd.h"
#include "..\Ui\UiCase\UiGameSetting.h"
#include "..\Ui\UiCase\UiChatCentre.h"
#include "..\Ui\UiCase\UiTopMessage.h"
#include "..\Ui\UiCase\UiSystemMessage.h"
#include "..\Ui\UiCase\UiRoleFace.h"
#include "..\Ui\UiCase\UiTeamList.h"
#include "..\Ui\UiCase\UiVendueWnd.h"
#include "..\Ui\UiCase\UiDragItem.h"
#include "..\Ui\UiCase\UiShortcutWnd.h"
#include "..\Ui\UiCase\UiShortcutPlusWnd.h"
#include "..\Ui\UiCase\UiChangeMapWnd.h"
#include "chatWindow/ChatMainDlg.h"
#include "../NetConnect/NetConnectAgent.h"
#include "ui/UiCase/UiFuryBox.h"
#include "ui/UiCase/UiFSBible.h"
#include "../Ui/UiCase/UiWaitingMsg.h"
#include "ui/UiCase/UiComMsgBox.h"
#include "ui/KMessageCentre.h"
#include "Ui/UiCase/UiWorldCombatInfo.h"
#include "Ui/UiCase/UiQuestTrack.h"
#include "minilzo.h"
#include "Ui/UiCase/UiServerList.h"
#include "Ui/UiCase/UiIBShop.h"
#include "Ui/UiCase/UiCreditShop.h"
#include "Ui/UiCase/UiQuestionWindow.h"

extern iCoreShell*		g_pCoreShell;
extern bool				g_useServerList;

KLogin		g_LoginLogic;

static unsigned gs_holdrand = ::time(NULL);

static inline unsigned _Rand()
{
    gs_holdrand = gs_holdrand * 244213L + 1541021L;
     
    return gs_holdrand;
}

static void RandMemSet(int nSize, unsigned char *pbyBuffer)
{
    _ASSERT(nSize);
    _ASSERT(pbyBuffer);

    while (nSize--)
    {
        *pbyBuffer++ = (unsigned char)_Rand();
    }
}


/************************************************************************/
/*					Construction KLogin object                          */
/************************************************************************/
KLogin::KLogin()
{
	m_loginStatus		= LL_S_IDLE;
	m_nNumRole			= 0;
	m_uLeftTime			= 0;
	m_uLeftTimeOfPoint	= 0;
	m_uDynamicKey		= 0;
	memset( &m_Choices, 0, sizeof(LOGIN_CHOICE) );
	m_bAccountValidate	= true;
	m_bSelectRole		= true;
	m_dwPort			= 6666;
	m_IndexRoleLoginGame = 0;
	m_bCanCreate		 = false;
	m_needAnswer		 = false;
}


/************************************************************************/
/*					Destruction KLogin object                           */
/************************************************************************/
KLogin::~KLogin()
{
	for ( int n = 0; n < m_RoleList.size(); ++n )
	{
		if ( m_RoleList[n] )
		{
			delete m_RoleList[n];
			m_RoleList[n] = NULL;
		}
	}
}

/************************************************************************/
/*				 Net msg filter callback function                       */
/************************************************************************/
void  KLogin::ClientCallBack( void *lpParam, const unsigned long uInID, const unsigned long ulnEventType )
{
	switch(ulnEventType)
	{
	case enumServerConnectCreate:
		break;
	case enumServerConnectClose:
		{
			if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME || KUiAutoConnect::IsVisible() )
			{
				g_LoginLogic.NotifyDisconnect(true);
				if ( KUiChangeMapWnd::IsVisible() )
				{
					KUiChangeMapWnd::GetSingleton().EndLoading();
				}
				KUiAutoConnect::Show();				
			}
			else
			{	
				g_LoginLogic.NotifyDisconnect();
				if ( KUiChangeMapWnd::IsVisible() )
				{
					KUiChangeMapWnd::GetSingleton().EndLoading();
				}
				if ( !KUiWaitingMsg::GetSingleton().IsConnectting() &&
					!KUiWaitingMsg::GetSingleton().IsQuiting() )
				{
					KUiWaitingMsg::GetSingleton().BeginLogin();
				}
			}
		}
		break;
	}
}

void KLogin::SetStatus( LOGIN_LOGIC_STATUS lls )
{
	KAutoCriticalSection autoLock(m_loginLock);
	m_loginStatus = lls; 
};

/************************************************************************/
/*				Process login net message                               */
/************************************************************************/
void KLogin::AcceptNetMsg( void* pMsgData, int nSize )
{
	if ( pMsgData == NULL )
	{
		return;
	}

	switch( GetStatus() )
	{
	case LL_S_ACCOUNT_CONFIRMING:
		ProcessAccountLoginResponse( (KLoginStructHead*)pMsgData );
		break;
	case LL_S_WAIT_ROLE_LIST:
		ProcessRoleListResponse( (ROLE_LIST_SYNC*)( pMsgData ), nSize );
		break;
	case LL_S_CREATING_ROLE:
		ProcessCreateRoleResponse( static_cast<tagNewDelRoleResponse*>( pMsgData ) );
		break;
	case LL_S_DELETING_ROLE:
		ProcessDeleteRoleResponse( static_cast<tagNewDelRoleResponse*>( pMsgData) );
		break;
	case LL_S_WAITQUESTION:
		ProcessQuestResponse( pMsgData );
		break;
	case LL_S_ENTERING_GAME:
		break;
	}
}

/************************************************************************/
/*				Begin to login game server			                    */
/************************************************************************/
void KLogin::LoginStart( void )
{
	KUiLoginBackGround::Show();

	if ( g_useServerList )
	{
		KUiServerList::Show();
	}
	else
	{
		KUiLogin::Show();
	}
	
	if ( g_showtip )
	{
		KUiUpdateTip::Show();
	}
}

/************************************************************************/
/*				Connect to the account server request                   */
/************************************************************************/
bool KLogin::CreateGameServerConnection( const BYTE* pAddress /*= NULL*/ )
{
	if ( GetStatus() != LL_S_IDLE )
	{
		return false;
	}

	bool bRet = false;

	if ( pAddress )
	{
		memcpy( m_Choices.ServerAddress, pAddress, sizeof( m_Choices.ServerAddress ) );
	}
	
	if (  m_Choices.ServerAddress && ConnectServer(  m_Choices.ServerAddress ) )
	{	
		RegistNetAgent();	
		SetStatus( LL_S_WAIT_INPUT_ACCOUNT );
		bRet = true;
	}
	return bRet;
}

/************************************************************************/
/*			  Validate user password request                            */
/************************************************************************/
bool KLogin::LoginAccountValidate( const char* pAccount, const KSG_PASSWORD& crPassword,  const char* pActiveKey, bool bOrignPassword )
{
	CreateGameServerConnection();
	if ( GetStatus() != LL_S_WAIT_INPUT_ACCOUNT )
	{
		return false;
	}

	bool bRet = false;

	KSG_PASSWORD pass = crPassword;

	if ( pAccount && SendRequest( pAccount, &pass, LOGIN_A_LOGIN, pActiveKey ) )
	{	
        if ( bOrignPassword )
        {
    		SetAccountPassword( pAccount, &crPassword );
        }
		SetStatus( LL_S_ACCOUNT_CONFIRMING );
		bRet = true;
	}
	else
	{
		bRet = false;
	}

	return bRet;
}

void KLogin::NoEmploy( void )
{
	if (g_LoginLogic.m_IndexRoleLoginGame < 0 || g_LoginLogic.m_IndexRoleLoginGame >= g_LoginLogic.m_RoleList.size() )
	{
		return;
	}
		 
	tagDBSelPlayer	NetCommand;
	NetCommand.cProtocol = c2s_no_employ;
	strcpy( NetCommand.szRoleName, g_LoginLogic.m_RoleList[g_LoginLogic.m_IndexRoleLoginGame]->szName );
	g_NetConnectAgent.SendMsg( &NetCommand, sizeof(NetCommand) );
	g_NetConnectAgent.UpdateClientRequestTime( false );
}

/************************************************************************/
/*			Select a new role to login game                             */
/************************************************************************/
bool KLogin::SelectRoleLoginGame( int nIndex )
{
	if ( GetStatus() == LL_S_ENTERING_GAME )
	{
		SetStatus( LL_S_ROLE_LIST_READY );
	}

	if ( m_RoleList[nIndex]->byFreeze > 0 )
	{
		CallConnectInfoBox( CI_MI_ROLE_FREEZEN_BY_PLUGINS );
		return true;
	}

	DWORD EmployLeftTime = m_RoleList[nIndex]->dwEmployLeftTime;
	if ( EmployLeftTime > 0 && EmployLeftTime < 0xfffffffd )
	{
		m_IndexRoleLoginGame = nIndex;
		const char* szTip = KMessageCentre::GetMessage( login_error_message, CI_MI_ENGAGE_TIP_3 );
		if ( szTip )
		{
			KUiComMsgBox::GetSingleton().Show();
			KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(szTip));
			KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(ACCEPT_BIG5), AnsiToUtf8(CANNEL_BIG5));
			KUiComMsgBox::GetSingleton().setFristBtnCallback(NoEmploy);
		
		}
		return true;
	}


	bool bRet = false;
	if (GetStatus() == LL_S_ROLE_LIST_READY && nIndex >= 0 && nIndex < m_nNumRole)
	{
		KUiComMsgBox::GetSingleton().init();
		KUiComMsgBox::GetSingleton().Hide();
		tagDBSelPlayer	NetCommand;
		NetCommand.cProtocol = c2s_dbplayerselect;
		strcpy( NetCommand.szRoleName, m_RoleList[nIndex]->szName );
		g_NetConnectAgent.SendMsg( &NetCommand, sizeof(NetCommand) );
		g_NetConnectAgent.UpdateClientRequestTime( false );
		strcpy( m_Choices.szProcessingRoleName, NetCommand.szRoleName );
		SetStatus( LL_S_ENTERING_GAME );
		bRet = true;
		KUiChangeMapWnd::GetSingleton().show(false, defaultMap);
		KUiAutoConnect::GetSingletonPtr()->SetRoleName(AnsiToUtf8(NetCommand.szRoleName));
		KUiLRSkillWnd::GetSingleton().setRoleName(NetCommand.szRoleName);
		KUiFSBible::getSingleton().showSpecialPanelIfHave();
	}
	else
	{
		CallConnectInfoBox( CI_MI_CONNECT_FAILED );
	}
	return bRet;
}

/************************************************************************/
/*			Auto Select a new role to login game                             */
/************************************************************************/
bool KLogin::AutoSelectRoleLoginGame()
{
	bool bRet = false;

	int nIndex = -1;
	for ( int i = 0; i < m_nNumRole; i++ )
	{
		if ( m_RoleList[i] && m_RoleList[i]->szName && strcmp(KUiAutoConnect::GetSingleton().GetRoleName(), m_RoleList[i]->szName) == 0 )
		{
			nIndex = i;
			break;
		}
	}

	// 没有找到相同人物名退出游戏
	if ( -1 == nIndex )
	{
		NotifyDisconnect();
	}

	bRet = SelectRoleLoginGame( nIndex );
	
	KUiAutoConnect::Hide();
	if ( bRet )
	{
		g_LoginLogic.SetSelectRole(true);
	}
	else
	{
		KUiAutoConnect::GetSingletonPtr()->ReStart();
	}

	return bRet;
}

/************************************************************************/
/*					Create a new role                                   */
/************************************************************************/
bool KLogin::CreateRoleAtServer( KRoleChiefInfo* pCreateInfo )
{
	if ( g_LoginLogic.IsCanCreateRole() )
	{
		CallConnectInfoBox( CI_MI_ANSWER_OVERTIME );
		return false;
	}

	bool bRet	= false;
	if ( GetStatus() == LL_S_ROLE_LIST_READY && pCreateInfo && m_nNumRole < MAX_CREATE_PLAYER_IN_ACCOUNT && pCreateInfo->byAttribute >= 0 && pCreateInfo->byAttribute < series_num)
	{
		int nNameLen = strlen(pCreateInfo->szName);
		if (nNameLen >= 1 && nNameLen < sizeof(pCreateInfo->szName))
		{
			NEW_PLAYER_COMMAND NewRoleInfo;
			NewRoleInfo.m_btRoleNo							= pCreateInfo->byGender;
			NewRoleInfo.m_btSeries							= pCreateInfo->byAttribute;
			NewRoleInfo.m_NativePlaceId						= pCreateInfo->uNativePlaceId;
			memcpy( NewRoleInfo.m_szName, pCreateInfo->szName, nNameLen );
			NewRoleInfo.m_btPortrait						= pCreateInfo->byPortrait;
			NewRoleInfo.m_szName[nNameLen]					= '\0';
			NewRoleInfo.cProtocol						= c2s_newplayer;

			g_NetConnectAgent.SendMsg( &NewRoleInfo, sizeof(NewRoleInfo) );
			g_NetConnectAgent.UpdateClientRequestTime( false );//*/

			memcpy( m_Choices.szProcessingRoleName, pCreateInfo->szName, nNameLen );
			m_Choices.szProcessingRoleName[nNameLen]	= 0;

			SetStatus( LL_S_CREATING_ROLE );
			bRet										= true;
		}
	}
	if ( !bRet )
	{
		CallConnectInfoBox( CI_MI_CONNECT_FAILED );
	}
	return bRet;
}

/************************************************************************/
/*					Delete role from server                             */
/************************************************************************/
bool KLogin::DeleteRoleFromServer( int nIndex, const KSG_PASSWORD &crSupperPassword )
{
	bool bRet = false;

	if (GetStatus() == LL_S_ROLE_LIST_READY && nIndex >= 0 && nIndex < m_nNumRole)
	{
		tagDBDelPlayer	NetCommand;
		RandMemSet(sizeof(tagDBDelPlayer), (BYTE*)&NetCommand);	// random memory for make a cipher

		NetCommand.cProtocol = c2s_removeplayer;
		GetAccountPassword(NetCommand.szAccountName, NULL);
        NetCommand.Password = crSupperPassword;
		strncpy(NetCommand.szRoleName, m_RoleList[nIndex]->szName, sizeof(NetCommand.szRoleName));
        NetCommand.szRoleName[sizeof(NetCommand.szRoleName) - 1] = '\0';

		g_NetConnectAgent.SendMsg(&NetCommand, sizeof(NetCommand));
		memset(&NetCommand.Password, 0, sizeof(NetCommand.Password));
		g_NetConnectAgent.UpdateClientRequestTime(false);

		strcpy(m_Choices.szProcessingRoleName, m_RoleList[nIndex]->szName);

		SetStatus( LL_S_DELETING_ROLE );
		bRet = true;
	}
	else
	{
		bRet = false;
		CallConnectInfoBox( CI_MI_CONNECT_FAILED );
	}
	return bRet;
}

/************************************************************************/
/*				Notify to start paint game space                        */
/************************************************************************/
void KLogin::NotifyToStartGame( void )
{
	SetStatus( LL_S_IN_GAME );
	if ( GetStatus() == LL_S_IN_GAME )
	{
		g_NetConnectAgent.UpdateClientRequestTime(true);
	}

	//聊天
	KUiChanMgr::getSinglton().init();
	//ChatMainDlg::RegisterLocalChannel();
}

/************************************************************************/
/*				Notify to start paint game space                        */
/************************************************************************/
void KLogin::NotifyToLoadMap( void )
{
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_byte_extend,		NULL				);
	SetStatus(LL_S_IN_GAME );
	KUiLoginBackGround::Hide();
	KUiUpdateTip::Hide();
	KUiLogin::Hide();
	KUiSelPlayer::Hide();
	KUiNewPlayer::Hide();
	KUiWaitingMsg::GetSingleton().QuitMsg();
}

/************************************************************************/
/*				Notify to disconnect game server                        */
/************************************************************************/
void KLogin::NotifyDisconnect( bool bToAutoConnect /* = false */, bool bShowActiveKey  )
{
	KUiShortcutWnd::GetSingleton().SaveAllItem();
	KUiShortcutPlusWnd::GetSingleton().SaveAllItem();
	g_pCoreShell->OperationRequest(GOI_FORCE_DELETE_NPC_HEADINFO,0,0);
#ifdef USING_CHAT_WINDOW
	ChatMainDlg::MainDlgCloseChannel();
	ChatMainDlg::MainDlgShowWndChat(FALSE);
	//好友列表需要更新
	ChatMainDlg::UpdateFriendList();
	KUiChannelCentre::Show();
	KUiChannelCentre::GetSingleton().showSystemFrame(true);	
#endif
	g_pCoreShell->OperationRequest( GOI_EXIT_GAME, 0, 0 );

	//聊天和频道相关
	KUiChanMgr::getSinglton().unregistAll();
	KUiChannelCentre::GetSingleton().closeAllFrame();
	KUiChatInputWnd::GetSingleton().clearLatestChatMenu();
	if ( KUiChatCentre::IsInit())
		KUiChatCentre::OfflineOperation();

	//消息相关
	KUiTopMessage::GetSingleton().resetText();
	KUiSystemMessage::GetSingleton().clear();

	//清空IB商店和信用商店的物品
	g_pCoreShell->OperationRequest( GOI_IBSHOP_CLEAR_ALL, 0, 0 );
	KUiIBShop::GetSingleton().ClearShop();
	KUiCreditShop::GetSingleton().ClearShop();
	
	//其他
	KUiTrafficLightManager::getSinglton().closeAll();
	KUiDragItem::GetSingleton().initItem();
	
	if (KUiWorldCombatInfo::IsVisible())
	{
		KUiWorldCombatInfo::Hide();
	}//endif
	
	if ( bToAutoConnect )
	{
		// 自动重连
		g_LoginLogic.ReturnToIdleStatus();
		KTaisuiWnd::EnableHide();
		KUiAutoConnect::GetSingletonPtr()->ReStart();
		closeUiWnd(SERVER_DISCONNECT);
	}
	else
	{				
		// 退出游戏
		if ( KUiRoleFace::IsVisible() )
			KUiRoleFace::SetData(pk_peace);
		if ( KUiTeamList::IsInit() )
			KUiTeamList::GetSingletonPtr()->QuitGame();
		if ( KUiChatInputWnd::IsInit() )
			KUiChatInputWnd::GetSingletonPtr()->clearAllCachedMessage();
		if ( KUiVendueWnd::IsInit() )
			KUiVendueWnd::GetSingletonPtr()->clearPageDate();

		ReturnToIdleStatus();
		KUiAdapter::UiEndGame();
		KUiLogin::Hide();
		KUiSelPlayer::Hide();
		KUiNewPlayer::Hide();
		KUiFuryBox::Hide();
		KUiWaitingMsg::GetSingleton().QuitMsg();
		KUiLoginBackGround::Show();
		KUiLogin::Show( bShowActiveKey );
		KTaisuiWnd::EnableHide();
		KUiFSBible::getSingleton().ClearQuestList();
		KUiQuestTrack::GetSingleton().ClearTrackList();
	}
}

/************************************************************************/
/*				Return to Login					                        */
/************************************************************************/
void KLogin::ReturnToLogin( void )
{
	if ( GetStatus() != LL_S_IDLE )
	{
		ReturnToIdleStatus();
		KUiAdapter::UiEndGame();
		KUiLogin::Hide();
		KUiSelPlayer::Hide();
		KUiNewPlayer::Hide();
		KUiWaitingMsg::GetSingleton().QuitMsg();
		KUiLoginBackGround::Show();
		KUiLogin::Show();
	}
}

/************************************************************************/
/*				Return to LL_S_IDLE status                              */
/************************************************************************/
void KLogin::ReturnToIdleStatus( void ) 
{
	KUiWaitingMsg::GetSingleton().EndLogin();
	if ( GetStatus() != LL_S_IDLE )
	{
		UnRegistNetAgent();
		g_NetConnectAgent.DisconnectGameSvr();
		SetStatus( LL_S_IDLE );
		m_bSelectRole = true;
		m_bAccountValidate = true;
		m_needAnswer = false;
	}
	m_Choices.bIsRoleNewCreated = false;
}

/************************************************************************/
/*           Set user account and password info                         */
/************************************************************************/
void KLogin::SetAccountPassword( const char* pszAccount, const KSG_PASSWORD* pcPassword )
{
	int i = 0;
	if ( pszAccount )
	{
		strncpy( m_Choices.szAccount, pszAccount, sizeof(m_Choices.szAccount) );
		for ( i = 0; i < COMMON_CLIENT_MSG_LEN_32; i++ )
		{
			m_Choices.szAccount[i] = ~m_Choices.szAccount[i];
		}
	}
	if ( pcPassword )
	{
		m_Choices.Password = *pcPassword;
		for ( i = 0; i < KSG_PASSWORD_MAX_SIZE; i++ )
		{
			m_Choices.Password.szPassword[i] = ~m_Choices.Password.szPassword[i];
		}
	}
}

/************************************************************************/
/*           Set user account and password info                         */
/************************************************************************/
void KLogin::GetAccountPassword(char* pszAccount, KSG_PASSWORD* pPassword)
{
	int i = 0;
	if ( pszAccount )
	{
		memcpy( pszAccount, m_Choices.szAccount, sizeof(m_Choices.szAccount) );
		for ( i = 0; i < COMMON_CLIENT_MSG_LEN_32; i++ )
		{
			pszAccount[i] = ~pszAccount[i];
		}
	}
	if ( pPassword )
	{
        *pPassword = m_Choices.Password;
		for ( i = 0; i < KSG_PASSWORD_MAX_SIZE; i++ )
		{
			pPassword->szPassword[i] = ~pPassword->szPassword[i];
		}
	}
}

/************************************************************************/
/*					Process account login net message.					*/
/************************************************************************/
void KLogin::ProcessAccountLoginResponse( KLoginStructHead* pResponse )
{
	KUiWaitingMsg::GetSingleton().EndLogin();
	KUiWaitingMsg::GetSingleton().SetShowActiveKey( false );
	KIniFile iniFile;
	if ( ((pResponse->Param & LOGIN_ACTION_FILTER) == LOGIN_A_LOGIN) )				
	{
		KLoginAccountInfo* pInfo = (KLoginAccountInfo*)pResponse;
		char	szAccount[32];
        KSG_PASSWORD Password;
		GetAccountPassword(szAccount, &Password);
		
		bool bRet = false;
		bRet = ( stricmp( pInfo->Account,  szAccount )  == 0) && (strcmp( pInfo->Password.szPassword, Password.szPassword ) == 0);
		if ( bRet )
		{	
			//如果用户配置文件不存在创建一个
			char pathName[COMMON_CLIENT_MSG_LEN_128];
			getcwd(pathName, COMMON_CLIENT_MSG_LEN_128);
			strcat(pathName, USERDATA_FOLDER);
			if ( !PathFileExists( pathName ) )
			{
				if ( 0 != mkdir(pathName) )
				{
					return;
				}
			}

			strcat(pathName, USER_SCHEME_FOLDER);
			if ( !PathFileExists( pathName) )
			{
				if ( 0 != mkdir(pathName) )
				{
					return;
				}
			}

			char fileName[COMMON_CLIENT_MSG_LEN_128] = UI_ACCOUT_SET;
			strcat(fileName, szAccount);
			strcat(fileName, ".ini");
			strcat(fileName, "\0");

			strncpy( g_Accounts, fileName, 64);
			KUiGameSetting& gs = KUiGameSetting::GetSingleton();
			if ( !gs.LoadAllGameSetValue() )
			{
				KUiAdapter::InitGameSet();
				gs.SaveAllGameSetValue();
			}

			if ( iniFile.Load(fileName) == false )
			{
				FILE *ifile = fopen(fileName, "w");
				if( ifile != NULL)
				{
					fclose(ifile);
				}
			}
			//
			KUiNewPlayerInfo::GetSingleton().setFileName(fileName);
			KUiSelPlayer::GetSingleton().setFileName(fileName);
			KUiLRSkillWnd::GetSingleton().setFileName(fileName);
			strncpy( g_Accounts, fileName, 64);

			int nResult = ((pResponse->Param) & ~LOGIN_ACTION_FILTER);
			if ( nResult == LOGIN_R_SUCCESS )
			{
				g_NetConnectAgent.UpdateClientRequestTime( false );
				
				SetStatus( LL_S_WAIT_ROLE_LIST );

				m_uLeftTime			= pInfo->nLeftTime;
#ifdef KSG_EXT_LEFT_TIME_OF_POINT
                m_uLeftTimeOfPoint	= pInfo->nLeftTimeOfPoint;
#else
                m_uLeftTimeOfPoint	= 0;
#endif
				if ( m_uLeftTime < m_uLeftTimeOfPoint )
				{
					m_uLeftTime = m_uLeftTimeOfPoint;
				}
				if ( m_bSelectRole && m_bAccountValidate )
				{
					KUiLoginBackGround::Show();
					KUiLogin::Show();
				}
				/*else if ( !m_bSelectRole && !m_bAccountValidate )
				{
					// 如果等待角色列表
					KUiSelPlayer::Show();
				}//*/
			}
			else if ( (m_bSelectRole && m_bAccountValidate) || nResult== LOGIN_R_INVALID_PROTOCOLVERSION )
			{
				switch( nResult )
				{
				case LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR:
					CallConnectInfoBox( CI_MI_ACCOUNT_PWD_ERROR );
					break;
				case LOGIN_R_ACCOUNT_EXIST:
					/*
					{
						if ( !KUiWaitingMsg::GetSingleton().IsConnectting() )
						{
							ReturnToIdleStatus();
							KUiWaitingMsg::GetSingleton().BeginLogin();
						}
					}
					return;
					//*/
					CallConnectInfoBox( CI_MI_ACCOUNT_LOCKED );
					break;
				case LOGIN_R_ACCOUNT_WAIT:
					break;
				case LOGIN_R_ACCOUNT_LOGINING:
					CallConnectInfoBox( CI_MI_ACCOUNT_LOGINING );
					break;
				case LOGIN_R_FREEZE:
					CallConnectInfoBox( CI_MI_ACCOUNT_FREEZE );
					break;
				case LOGIN_R_INVALID_PROTOCOLVERSION:
					CallConnectInfoBox( CI_MI_INVALID_PROTOCOLVERSION );
					break;
				case LOGIN_R_TIMEOUT:
					CallConnectInfoBox( CI_MI_NOT_ENOUGH_ACCOUNT_POINT );
					break;
				case LOGIN_R_ACCOUNT_GUESTFULL:	
					CallConnectInfoBox( CI_MI_ACCOUNT_GUEST_FULL );
					break;
				case LOGIN_R_ACCOUNT_FREEZE_BY_MOBILE:
					CallConnectInfoBox( CI_MI_ACCOUNT_FREEZEN_BY_MOBLIE );
					break;
				case LOGIN_R_FAILED:
					CallConnectInfoBox( CI_MI_CONNECT_SERV_BUSY );
					break;
				case LOGIN_R_NEED_ACTIVE:
					{
						KUiLogin::Show( true );
						CallConnectInfoBox( CI_MI_NEED_ACTIVEKEY );
						
						KUiWaitingMsg::GetSingleton().SetShowActiveKey( true );
					}
					break;
				case LOGIN_R_ACTIVEKEY_ERROR:
					{
						KUiLogin::Show( true );
						CallConnectInfoBox( CI_MI_ACTIVEKEY_ERROR );						
						KUiWaitingMsg::GetSingleton().SetShowActiveKey( true );
					}
					break;
				default:
					CallConnectInfoBox( CI_MI_CONNECT_FAILED );
					break;
				}
				ReturnToIdleStatus();
			}
		}
		memset(szAccount, 0, sizeof(szAccount));
		memset(&Password, 0, sizeof(Password));
	}
	else if( (pResponse->Param & LOGIN_ACTION_FILTER) == LOGIN_A_LOGINSIGN )
	{
		KLoginAccountInfo* pInfo	= (KLoginAccountInfo*)pResponse;
		m_uDynamicKey				= pInfo->nLeftTime;
		SetStatus( LL_S_ACCOUNT_CONFIRMING );
		KUiUpdateTip::Hide();
		KUiLogin::Show();
	}
	else
	{
		//Error login response type.
	}
}

/************************************************************************/
/*					Process role net message                            */
/************************************************************************/
void KLogin::ProcessRoleListResponse( ROLE_LIST_SYNC* pResponse, int nSize )
{	
	ClearRoleList();
	if ( pResponse->ProtocolType == s2c_rolelist )
	{
		m_bCanCreate	= pResponse->bPermitCreate != 0 ? true : false;
		m_nNumRole      = pResponse->RoleCount;
		
		if ( m_nNumRole == 0xff )
		{
			SetStatus( LL_S_IDLE );
			CallConnectInfoBox( CI_MI_ROLE_LIST_ERROR );
		}
		else
		{
			unsigned char szBuff[SAVETEMPBUFLEN];
			unsigned int  nLen = SAVETEMPBUFLEN;
			
			int                     nNewSize = SAVETEMPBUFLEN;
			int                     nOldSize = nSize - sizeof(ROLE_LIST_SYNC) - 1;
			
			if ( pResponse->RoleCount != 0)
			{
				lzo1x_decompress(
					(const unsigned char *)pResponse->RoleList,
					nOldSize,
					(unsigned char *)szBuff,
					&nLen,
					NULL);
			}//endif

			KUiLogin::GetSingleton().SetPassWord("");
			RoleBaseInfo* pList = (RoleBaseInfo*)szBuff;
			for ( int i = 0; i < m_nNumRole; ++i )
			{
				m_RoleList[i] = new KRoleChiefInfo;
				if ( pList->szName[0] && m_RoleList[i] )
				{
					strcpy( m_RoleList[i]->szName, pList->szName );
					m_RoleList[i]->byAttribute		= pList->Series;
					m_RoleList[i]->byGender			= pList->Sex;
					m_RoleList[i]->uLevel			= pList->Level;
					m_RoleList[i]->byPortrait		= pList->btPortrait;
					m_RoleList[i]->dwLastLogoutTime	= pList->dwLastLoginTime;
					m_RoleList[i]->dwLastIP			= pList->dwLastLoginIP;
					m_RoleList[i]->dwLastMapID		= pList->dwLastMapID;
					m_RoleList[i]->nWeapon			= 0;
					m_RoleList[i]->nHelm			= 0;
					m_RoleList[i]->nArmor			= 0;			
					m_RoleList[i]->nShoulder		= 0;
					m_RoleList[i]->nCuff			= 0;
					m_RoleList[i]->nBoot			= 0;
					m_RoleList[i]->nHorse			= 0;
					m_RoleList[i]->bRideHorse		= 0;
					m_RoleList[i]->dwWillDestoryTime	= pList->dwWillDestoryTime;
					m_RoleList[i]->dwEmployLeftTime = pList->dwEmployLeftTime;
					m_RoleList[i]->byForbid			= pList->byForbid;
					m_RoleList[i]->byFreeze			= pList->byFreeze;
					m_RoleList[i]->bTongMember		= pList->bIsTongMember > 0 ? true : false;

					/*
					m_RoleList[i]->nWeapon			= pList->nWeapon;
					m_RoleList[i]->nHelm			= pList->nHelm;
					m_RoleList[i]->nArmor			= pList->nArmor;			
					m_RoleList[i]->nShoulder		= pList->nShoulder;
					m_RoleList[i]->nCuff			= pList->nCuff;
					m_RoleList[i]->nBoot			= pList->nBoot;
					m_RoleList[i]->nHorse			= pList->nHorse;
					m_RoleList[i]->bRideHorse		= pList->bRideHorse;//*/
					++pList;
				}
				else
				{
					m_nNumRole = i;
					break;
				}
			}
			
			g_NetConnectAgent.UpdateClientRequestTime(true);
			SetStatus( LL_S_ROLE_LIST_READY );
			
			if ( !m_bSelectRole && m_bAccountValidate )
			{
				if ( g_LoginLogic.AutoSelectRoleLoginGame() )
				{
					KUiAutoConnect::GetSingletonPtr()->ResetConnectTimes();
					g_LoginLogic.SetSelectRole( true );
				}
				else
				{
					KUiAutoConnect::GetSingletonPtr()->ReStart();
				}
			}
			else if ( m_bSelectRole && m_bAccountValidate )
			{
				KUiLogin::Hide();
				KUiLoginBackGround::Hide();

				if ( m_nNumRole > 0 )
				{
					KUiSelPlayer::Show();
					KUiSelPlayer::GetSingletonPtr()->setRoleList( &m_RoleList );
				}
				else
				{
					
					if ( g_LoginLogic.IsAnswerRight() )
					{
						KUiNewPlayer::Show();
						g_NetConnectAgent.RegisterMsgTargetObject( s2c_byte_extend,		NULL				);
					}		
					else if ( g_LoginLogic.IsCanCreateRole() )
					{
						KUiNewPlayer::Show();
						CallConnectInfoBox( CI_MI_ANSWER_OVERTIME );
					}
					else
					{
						KUiNewPlayer::Show();
						g_LoginLogic.RequestQuestion();
					}
				}			
			}

		}
	}
}

/************************************************************************/
/*			Process delete role net message                             */
/************************************************************************/
void KLogin::ProcessDeleteRoleResponse( tagNewDelRoleResponse* pResponse )
{
	if (pResponse->cProtocol == s2c_rolenewdelresponse)
	{
		g_NetConnectAgent.UpdateClientRequestTime( true );
		SetStatus( LL_S_ROLE_LIST_READY );

		if ( pResponse->bSucceeded == DEL_ROLE_SUCESS )
		{
			char	szAccount[32];
			GetAccountPassword( szAccount, NULL );

			for ( int i = 0; i < m_nNumRole; ++i )
			{
				if ( strcmp(m_RoleList[i]->szName, m_Choices.szProcessingRoleName) == 0 )
				{
					m_nNumRole--;
					delete m_RoleList[i];
					m_RoleList[i] = NULL;
					m_RoleList.erase( i );
					KUiSelPlayer::Hide();
					
					SetStatus( LL_S_WAIT_ROLE_LIST );
					
					break;
				}
			}
		}
		else if ( pResponse->bSucceeded == DEL_ROLE_NOT_ALLOW )
		{
			CallConnectInfoBox( CI_MI_NOTDEL_OPER_ERROR );
		}
		else if(pResponse->bSucceeded == DEL_ROLE_FAIL_SOCIETY )
		{
			CallConnectInfoBox( CI_MI_DEL_ROLE_ISTONGMEMBER );
		}
		else if ( pResponse->bSucceeded == DEL_ROLE_FAIL_ERROR_PASSWORD)
		{
			CallConnectInfoBox( CI_MI_ERROR_CONFIRM_INPUT );
		}
		else 
		{
			CallConnectInfoBox( CI_MI_CONNECT_FAILED );
		}
	}
}

void	KLogin::ProcessQuestResponse( void* pResponse )
{
	BYTE* pByte = (BYTE*)pResponse;
	if ( pByte[0] != s2c_rolenewdelresponse )
	{
		UIQuestionData uiQuestionData;
		if (g_pCoreShell && g_pCoreShell->CoreParseQuestionProtocol((BYTE*)pResponse, uiQuestionData))
		{
			KUiQuestionWindow::GetSingleton().ShowQuestion( (unsigned int)&uiQuestionData, NULL );
			SetStatus( LL_S_CREATING_ROLE );
		}
	}
	else
	{
		ProcessCreateRoleResponse(static_cast<tagNewDelRoleResponse*>(pResponse));
	}
}

/************************************************************************/
/*				Process new create role net message                     */
/************************************************************************/
void KLogin::ProcessCreateRoleResponse(tagNewDelRoleResponse* pResponse)
{
	if (pResponse->cProtocol == s2c_rolenewdelresponse)
	{
		switch( pResponse->bSucceeded )
		{
		case CREATE_ROLE_SUCESS:
			g_NetConnectAgent.UpdateClientRequestTime(false);
			m_Choices.bIsRoleNewCreated = true;
			SetStatus( LL_S_WAIT_ROLE_LIST );
			KUiNewPlayer::Hide();
			KUiLoginBackGround::Hide();
			break;
		case CREATE_ROLE_FAIL_NOT_ALLOW:
			g_NetConnectAgent.UpdateClientRequestTime(true);
			SetStatus( LL_S_ROLE_LIST_READY );			
			CallConnectInfoBox( CI_MI_COMBINE_SEVER_NOTCREATE );
			break;
		case CREATE_ROLE_ERROR_NAME:
			g_NetConnectAgent.UpdateClientRequestTime(true);
			SetStatus( LL_S_ROLE_LIST_READY );			
			CallConnectInfoBox( CI_MI_ERROR_ROLE_NAME );
			break;
		case DEL_ROLE_SUCESS_ANSWER:
		case DEL_ROLE_FAIL_ERROR_QUESTION:
			KUiLoginBackGround::Hide();
			KUiLogin::Hide();
			KUiSelPlayer::Hide();	
			KUiNewPlayer::Show();
			g_NetConnectAgent.RegisterMsgTargetObject( s2c_byte_extend,		NULL				);
			SetStatus(LL_S_ROLE_LIST_READY);
			m_needAnswer = true;
			break;
		case DEL_ROLE_FAIL_ERROR_OVERANSWER:
			CallConnectInfoBox( CI_MI_ANSWER_OVERTIME );
			break;
		case DEL_ROLE_FAIL_ERROR_ANSWER:
			RequestQuestion();
			break;
		default:
			g_NetConnectAgent.UpdateClientRequestTime(true);
			SetStatus( LL_S_ROLE_LIST_READY );			
			CallConnectInfoBox( CI_MI_CONNECT_FAILED );
			break;
		}
	}
}

/************************************************************************/
/*						Transform char* ip to BYTE*                     */
/************************************************************************/
bool KLogin::GetIpAddress( const char* szAddress, BYTE* pcAddress )
{
	_ASSERT( pcAddress );
	int nValue[4];
	int nRet = sscanf( szAddress, "%d.%d.%d.%d", &nValue[0], &nValue[1], &nValue[2], &nValue[3] );
	if (nRet == 4 &&
		nValue[0] >= 0 && nValue[0] < 256 &&
		nValue[1] >= 0 && nValue[1] < 256 &&
		nValue[2] >= 0 && nValue[2] < 256 &&
		nValue[3] >= 0 && nValue[3] < 256)
	{
		pcAddress[0] = nValue[0];
		pcAddress[1] = nValue[1];
		pcAddress[2] = nValue[2];
		pcAddress[3] = nValue[3];
		return true;
	}
	return false;
}

/************************************************************************/
/*		Register net agent to g_NetConnectAgent global object.          */
/************************************************************************/
void KLogin::RegistNetAgent()
{
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_login, this							);
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_rolelist, this						);
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_rolenewdelresponse, this				);	
}

/************************************************************************/
/*    	Unregister net agent to g_NetConnectAgent global object.        */
/************************************************************************/
void KLogin::UnRegistNetAgent()
{
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_login, NULL							);
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_rolelist, NULL						);
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_rolenewdelresponse, NULL				);	
}

/************************************************************************/
/*						Connect game server                             */
/************************************************************************/
bool KLogin::ConnectServer( const BYTE* pIpAddress, GUID *pUID /*= NULL*/ )
{
	bool bRet = false;
//	KIniFile	IniFile;
//	if ( pIpAddress && IniFile.Load( CONFIG_INI ) )
//	{			
	//	int nPort;
	//	IniFile.GetInteger( "Server", "GameServPort", DEFAULT_GAMESERVER_PORT, &nPort );
		bRet = g_NetConnectAgent.ConnectToGameSvr( pIpAddress, m_dwPort, pUID ) > 0 ? true : false;
//	}
	return bRet;
}

/************************************************************************/
/*						Send request                                    */
/************************************************************************/

bool KLogin::SendRequest( const char* pszAccount, const KSG_PASSWORD* pcPassword, int nAction, const char* pActiveKey )
{
	KLoginAccountInfo AccountInfo;

	if (pszAccount && pcPassword)
	{
		AccountInfo.cProtocol = c2s_login;
		AccountInfo.Size  = sizeof(KLoginAccountInfo);
		AccountInfo.Param = nAction | LOGIN_R_REQUEST;
		strncpy(AccountInfo.Account,  pszAccount, sizeof(AccountInfo.Account));
		memset(AccountInfo.ActiveKey, 0, _ACTIVEKEY_LEN );
		strcpy((char*)AccountInfo.ActiveKey, (char*)pActiveKey );
        AccountInfo.Account[sizeof(AccountInfo.Account) - 1] = '\0';
		AccountInfo.Password = *pcPassword;
        AccountInfo.ProtocolVersion = KPROTOCOL_VERSION;    //  传输协议版本，以便校验是否兼容


		if ( g_NetConnectAgent.SendMsg( &AccountInfo, sizeof(AccountInfo)) )
		{
			g_NetConnectAgent.UpdateClientRequestTime( false );
			return true;
		}
	}
	return false;
}

bool	KLogin::RequestQuestion( void )
{
	tagProtoHeader requestQuestion;
	requestQuestion.cProtocol = c2s_get_question;
	if ( g_NetConnectAgent.SendMsg( &requestQuestion, sizeof(requestQuestion)) )
	{
		g_NetConnectAgent.UpdateClientRequestTime( false );
		SetStatus( LL_S_WAITQUESTION );
		g_NetConnectAgent.RegisterMsgTargetObject( s2c_byte_extend,		this				);
		return true;
	}	
	return false;
}


/************************************************************************/
/*						Send request                                    */
/************************************************************************/
void KLogin::CallConnectInfoBox( LOGIN_BG_INFO_MSG_INDEX eIdx )
{
	if ( eIdx == CI_MI_INVALID_PROTOCOLVERSION && KUiAutoConnect::GetSingleton().IsVisible() )
	{
		KUiAutoConnect::GetSingleton().QuitGame();
	}
	KUiWaitingMsg::GetSingleton().SetLoginStatus( eIdx );

}

void KLogin::ClearRoleList()
{
	for ( int n = 0; n < m_RoleList.size(); ++n )
	{
		if ( m_RoleList[n] )
		{
			delete m_RoleList[n];
			m_RoleList[n] = NULL;
		}
	}
	m_RoleList.clear();
	m_nNumRole = 0;
}

string KLogin::GetAccountName()
{
	string sResult;
	char szAccount[COMMON_CLIENT_MSG_LEN_32];
	memcpy( szAccount, m_Choices.szAccount, sizeof(m_Choices.szAccount) );
	for ( int i = 0; i < COMMON_CLIENT_MSG_LEN_32; i++ )
	{
		szAccount[i] = ~szAccount[i];
	}
	sResult = szAccount;
	return sResult;
}