//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/21/2006 14:40
//      File_base        : Login
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 登陆过程中描述
//
// 			登陆过程中，所有客户端发给服务器端的消息都是c2s_login
//		捆绑着一个KLoginStructHead结构的数据。所有客户端发给服务器
//		端的消息都是s2c_login捆绑着一个KLoginStructHead结构的数据。
//		对于一些情况来说，捆绑的是一个以KLoginStructHead为第一个成
//		员的更大的结构。
// 
// 		一个完整的与账号服务器建立连接过程中消息往返如下：
// 		1.发送请求：
//			c2s_login & KLoginAccountInfo; 
//			KLoginInfo::Param = LOGIN_A_LOGIN | LOGIN_R_REQUEST;
// 		2.服务器返回登陆结果：
//			s2c_login & KLoginAccountInfo;
// 			KLoginInfo::Param = LOGIN_A_LOGIN | (LOGIN_R_SUCCESS |
//			LOGIN_R_FAILED | LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR);
// 		3.登陆过程结束。
// 
//		申请账号过程过程中消息往返如下：(注：调试版本才提供此操作)
// 		1.发送请求：
//			c2s_login & KLoginAccountInfo; 
//			KLoginInfo::Param = LOGIN_A_NEWACCOUNT | LOGIN_R_REQUEST;
// 		2.服务器返回的申请结果：
//			s2c_login & KLoginAccountInfo;
// 			KLoginInfo::Param = LOGIN_A_NEWACCOUNT | (LOGIN_R_SUCCESS |
//			LOGIN_R_FAILED or LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR | 
//			LOGIN_R_ACCOUNT_EXIST);
// 		3.申请过程结束。
// 
// 			登陆过程在实际的执行中相互间可能会有时段重叠，所以客户端
//		要依据 KLoginInfo中的 Account 与 Password 数据项来判断服务器
//		返回的消息是否为针对最后一次的登陆请求。如果不是则忽略之。
// 
// 			上述规则描述对于请求游戏服务器列表的操作没有作登陆或
//		连接前提限制，匿名暨可操作。
// 
// 			可以考虑把传送的数据的结构里的空间定长字符串都改为变
//		长（根据实例确定长度），以缩小需要网络传送的数据的长度。
//		定长的存储账号与密码的结构是否改为变长，将依据网络加密方
//		式特性单独确定。可以考虑把连接请求与申请账号过程中的多次
//		账号密码传送改为只在发送请求时传送一次，同时辅以特殊的标
//		识数值，一来减少账号密码的网络传送次数，增加一点安全性；
//		二来也可减小要网络传送数据的量。
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef LOGIN_H
#define LOGIN_H

#include <map>
#include "KCriticalSection.h"
#include "KProtocol.h"
#include "../NetConnect/NetMsgTargetObject.h"
#include <string>

using std::string;

#define KSG_EXT_LEFT_TIME_OF_POINT

/*!
\brief
	Login process tip message index enum.	
*/
enum LOGIN_BG_INFO_MSG_INDEX
{
	CI_MI_CONNECTING						= 1,		//!< Connecting.
	CI_MI_CONNECT_FAILED,								//!< Connect failed.
	CI_MI_CONNECT_SERV_BUSY,							//!< Game server is busy.
	CI_MI_CONNECT_TIMEOUT,								//!< Time out.
	CI_MI_ACCOUNT_PWD_ERROR,							//!< Account or password error.
	CI_MI_ACCOUNT_LOCKED,								//!< Account be locked.
	CI_MI_ERROR_ROLE_NAME,								//!< Error role name.
	CI_MI_CREATING_ROLE,								//!< Creating new role.
	CI_MI_DELETING_ROLE,								//!< Deleting role.
	CI_MI_GETTING_ROLE_DATA,							//!< Get role info.
	CI_MI_ENTERING_GAME,								//!< Is in game.
	CI_MI_SVRDOWN,										//!< Game server is full or is maintenance.
	CI_MI_TO_DEL_ROLE,									//!< Will delete role.
    CI_MI_INVALID_PROTOCOLVERSION,						//!< Error protocol version
	CI_MI_ERROR_LOGIN_INPUT,							//!< Account or password error, please input again.
	CI_MI_ERROR_CONFIRM_INPUT,							//!< Delete role password error,please input again.
	CI_MI_INVALID_LOGIN_INPUT1,							//!< Role name can't include space tab and so on.
	CI_MI_INVALID_LOGIN_INPUT2,							//!< Role name length must between 2 and 18 char.
	CI_MI_NOT_ENOUGH_ACCOUNT_POINT,						//!< Not enough point.
	CI_MI_INVALID_PASSWORD,								//!< Error password.
	CI_MI_ACCOUNT_FREEZE					= 30,		//!< Account is freeze.
	CI_MI_DEL_ROLE_NOT_ALLOW				= 31,		//!< The role can't be delete.
	CI_MI_ACCOUNT_RELOGIN					= 34,		//!< Need re-login.
	CI_MI_ACCOUNT_LOGINING					= 35,		//!< Account is loginning.
	CI_MI_DEL_ROLE_ISTONGMEMBER				= 36,		//!< Is tong member.
	CI_MI_ACCOUNT_GUEST_FULL				= 37,		//!< Guest is full.
	CI_MI_ROLE_LIST_ERROR					= 38,		//!< Error role list.
	CI_MI_ACCOUNT_FREEZEN_BY_MOBLIE			= 39,		//!< Account freezen by moblie.
	CI_MI_ROLE_FREEZEN_BY_PLUGINS			= 40,		//!< Account freezen by plugins.
	CI_MI_COMBINE_SEVER_NOTCREATE			= 41,		//!< Combine can't create role.
	CI_MI_NOTDEL_OPER_ERROR					= 42,		//!< Combine can't delete role.
	CI_MI_DROPLINE,										//!< Disconnect by 
	CI_MI_FULLROLE,										//!< Full role.
	CI_MI_NO18,											//!< 18岁判定
	CI_MI_NEED_ACTIVEKEY,								//!< need input active key.
	CI_MI_ACTIVEKEY_ERROR,								//!< error active key.
	CI_MI_TONGMEMBER,									//!< error active key.
	CI_MI_WAITING_ROLEDELETE,									//!< error active key.
	CI_MI_ENGAGE_TIP_1,									//!< error active key.
	CI_MI_ENGAGE_TIP_2,									//!< error active key.
	CI_MI_ENGAGE_TIP_3,									//!< error active key.
	CI_MI_ENGAGE_TIP_4,									//!< error active key.
	CI_MI_ENGAGE_CANNT_DEL_ROLE,									//!< error active key.
	CI_MI_DELING_CANNT_DEL_ROLE,									//!< error active key.
	CI_MI_NO_AGREEMENT,									//不同意登陆协议
	CI_MI_NO_UPDATASERVERLIST,
	CI_MI_NO_UPDATASERVERLIST_ERROR,
	CI_MI_ANSWER_OVERTIME,										//!< Full role.
};

/*!
\brief
	Enum for define Login state.
*/
enum LOGIN_LOGIC_STATUS
{
	LL_S_IDLE												= 0,//!< Free time.
	LL_S_WAIT_INPUT_ACCOUNT,									//!< Wait for user input account.
	LL_S_ACCOUNT_CONFIRMING,									//!< Wait for server validate account.
	LL_S_WAIT_ROLE_LIST,										//!< Wait for  server send role list.
	LL_S_ROLE_LIST_READY,										//!< Role list arrive.
	LL_S_CREATING_ROLE,											//!< Server creating role.
	LL_S_DELETING_ROLE,											//!< Server Delete role.
	LL_S_ENTERING_GAME,											//!< Entering game.
	LL_S_IN_GAME,												//!< In game.
	LL_S_DROPLINE,												//!< Disconnect by uncertainty reason.
	LL_S_WAITQUESTION,
};

/*!
\brief
	const for create role limit
*/
#define	MAX_PLAYER_PER_ACCOUNT_CREATE	2

/*!
\brief
	Struct for login role info
*/
struct KRoleChiefInfo
{
	char				szName[COMMON_CLIENT_MSG_LEN_32];		//!< Role name.
	BYTE				byGender;								//!< Role gender.
	BYTE				byAttribute;							//!< Role metier.
	union
	{
		unsigned short	uNativePlaceId;							//!< Role native place id.
		short			uLevel;									//!< Role level.
	};
	BYTE				byPortrait;								//!< Role face.
	unsigned int		uCurExp;								//!< Role level.
	DWORD				dwLastMapID;							//!< Last login map id.
	DWORD				dwLastIP;								//!< Last login ip.
	DWORD				dwLastLogoutTime;						//!< last login time.
	BYTE                byChangeName;		
	BYTE				byFreeze;								//!< 冻结时间
	BYTE				byForbid;								//!< 是否禁言
	DWORD				dwWillDestoryTime;
	DWORD				dwEmployLeftTime;
	bool				tagChiefInfo;
	bool				bTongMember;

	int					nWeapon;
	int					nHelm;
	int					nArmor;			
	int					nShoulder;
	int					nCuff;
	int					nBoot;
	int					nHorse;
	bool				bRideHorse;
};

/*!
\brief
	Typedef for role list
*/
typedef std::map<int,KRoleChiefInfo*> KRoleList; 

/*!
\brief
	Class for all the login process
*/
class KLogin : public iKNetMsgTargetObject
{
	/*!
	\brief
		Struct for all the login process
	*/	
	struct	LOGIN_CHOICE
	{
		BYTE					ServerAddress[BYTE_IP_ADDRESS_LEN];								//!< Server IP.
		char					szAccount[COMMON_CLIENT_MSG_LEN_32];			//!< Current account.
		KSG_PASSWORD    		Password;										//!< Current role password.
		char					szProcessingRoleName[COMMON_CLIENT_MSG_LEN_32];	//!< Current role name.
		bool					bRememberAccount;								//!< If remember Account.
		bool					bRememberAll;									//!< If remember all login operation.
		bool					bAutoLoginEnable;								//!< If it can auto-connect.
		bool					bIsRoleNewCreated;								//!< If new role.
	};	
public:

	static void NoEmploy( void );
	/************************************************************************/
	/*					Construction and Destruction		                */
	/************************************************************************/
	KLogin();								
	~KLogin();		

public:
	static void  ClientCallBack( void *lpParam, const unsigned long uInID, const unsigned long ulnEventType );
	/************************************************************************/
	/*					Overrride from iKNetMsgTargetObject                 */
	/************************************************************************/
	/*!
	\brief
		Call the UiConnectInfo wnd to tell use the login status.

	\param eIdx
		Login  error information index.	

	\return
		Nothing
	*/
	void	CallConnectInfoBox( LOGIN_BG_INFO_MSG_INDEX eIdx );
	/*!
	\brief
		Process login net message.			
	
	\param pMsgData 
		pMsgData is the net message, need constraint transform to out need type.
		
	\return
		Nothing.			
	*/
	void	AcceptNetMsg( void* pMsgData, int nSize );

	/************************************************************************/
	/*					Operation function					                */
	/************************************************************************/
	bool	IsCanCreateRole( void ) { return !m_bCanCreate; }
	bool	IsAnswerRight(void) { return m_needAnswer; }
	/*!
	\brief
		Start to the login game server.
	
	\return
		Nothing.
	*/
	void	LoginStart( );

	/*!
	\brief
		Connect to the account server. If connect succeed m_Status change from 
	LL_S_IDLE to LL_S_WAIT_INPUT_ACCOUNT and return true, or m_Status do't change 
	and return false.		
	
	\param pAddress
		Account IP.
	
	\return
		If connect succeed return true,	or return false.
	*/
	bool	CreateGameServerConnection( const BYTE* pAddress = NULL );

	/*!
	\brief
		Sent account and password message to request validate if validate succeed 
	m_Status change from LL_S_WAIT_INPUT_ACCOUNT to LL_S_ACCOUNT_CONFIRMING, or   
	m_Status don't change.
	
	\param pAccount
		User account.
	
	\param crPassword
		User password struct define by KOL.
	
	\param bOrignPassword
		If need password.
	
	\return
		If connect succeed return true,	or return false.
	*/
 	bool	LoginAccountValidate( const char* pAccount, const KSG_PASSWORD& crPassword, const char* pActiveKey, bool bOrignPassword = true );

	/*!
	\brief
		Select role by index to enter game if  succeed	m_Status change from 
	LL_S_ROLE_LIST_READY to LL_S_WAIT_TO_LOGIN_GAMESERVER, or m_Status don't change.
	
	\param nIndex
		Role Index.
	
	\return
		If succeed return true,	or return false.
	*/
	bool	SelectRoleLoginGame( int nIndex );

	/*!
	\brief
		Auto Select role to enter game if  succeed	m_Status change from 
	LL_S_ROLE_LIST_READY to LL_S_WAIT_TO_LOGIN_GAMESERVER, or m_Status don't change.
	
	\param nIndex
		Role Index.
	
	\return
		If succeed return true,	or return false.
	*/
	bool	AutoSelectRoleLoginGame();

	/*!
	\brief
		Create a new role by pCreateInfo param. if create  succeed	m_Status change from 
	LL_S_ROLE_LIST_READY to LL_S_CREATING_ROLE, or m_Status don't change.
	
	\param pCreateInfo
		New role info.
	
	\return
		If succeed return true,	or return false.
	*/	
	bool	CreateRoleAtServer( KRoleChiefInfo* pCreateInfo );

	/*!
	\brief
		Delete a new role by nIndex param. if delete succeed m_Status change from 
	LL_S_DELETING_ROLE -> LL_S_ROLE_LIST_READY, or m_Status don't change.
	
	\param nIndex
		Delete role index.

	\param crSupperPassword
		Delete password struct define by KOL.
	
	\return
		If succeed return true,	or return false.
	*/		
	bool	DeleteRoleFromServer( int nIndex, const KSG_PASSWORD &crSupperPassword );

	/*!
	\brief
		Notify to start paint gamespace.if call this function m_Status change from 
	LL_S_ENTERING_GAME to LL_S_IN_GAME, or m_Status don't change.	

	\return
		Nothing.
	*/
	void	NotifyToStartGame( void );

	/*!
	\brief
		Notify to start paint gamespace.if call this function m_Status change from 
	LL_S_ENTERING_GAME to LL_S_IN_GAME, or m_Status don't change.	

	\return
		Nothing.
	*/
	void	NotifyToLoadMap( void );

	/*!
	\brief
		Notify to start paint gamespace.if call this function m_Status change to
	LL_R_CONNECT_FAILED, or m_Status don't change.	

	\return
		Nothing.
	*/
 	void	NotifyDisconnect( bool bToAutoConnect = false, bool bShowActiveKey = false );

	void	ReturnToLogin( void );

// 	void						NotifyTimeout					(																										);//!< 通知等待返回结果超时了

	/************************************************************************/
	/*				Public data process function			                */
	/************************************************************************/
	/*!
	\brief
		Set user select Game server IP for next step connect.
	
	\param pServerIP
		User select Game server IP.
	
	\return
		Nothing.
	*/
    inline void	SetGameServerIP( const char* szAccountServer, DWORD dwPort );

	/*!																																										  /*!
	\brief
		Get user select Game server IP.
	
	\return
		Return user select Game server IP.
	*/
	inline BYTE*	GetGameServerIP( void );

	/*!																																										  /*!
	\brief
		Get login status.
	
	\return
		Return login status.
	*/
 	inline LOGIN_LOGIC_STATUS	GetStatus( void );

	/*!																																										  /*!
	\brief
		Set login status.
	
	\return
		Return Nothing.
	*/
	void	SetStatus( LOGIN_LOGIC_STATUS lls ); 

	
	/*!
	\brief
		Set AccountValidate State.
	\return
		Nothing
	*/
	inline void	SetAccountValidate( bool b ) { m_bAccountValidate = b; };

	/*!
	\brief
		Get AccountValidate State.
	\return
		Return bool
	*/
	inline bool	GetAccountValidate( void ) { return m_bAccountValidate; };
	
	/*!
	\brief
		Set SelectRole State.
	\return
		Nothing
	*/
	inline void	SetSelectRole( bool b ) { m_bSelectRole = b; };

	/*!
	\brief
		Get SelectRole State.
	\return
		Return bool
	*/
	inline bool	GetSelectRole( void ) { return m_bSelectRole; };

	/*!																																										  /*!
	\brief
		Get login result.
	
	\return
		Return login result.
	*/
 	inline int	GetRoleCount( void );

	/*!																																										  /*!
	\brief
		Get login result.
	
	\return
		Return login result.
	*/
 	inline int	GetRoleInfo( int nIndex, KRoleChiefInfo* pInfo );
	/*!
	\brief
		Return to LL_S_IDLE status. 
	
	\return
		Nothing.		
	*/
	void	ReturnToIdleStatus( void );

	string	GetAccountName();

	bool	RequestQuestion( void );

private:
	/************************************************************************/
	/*				private data process function			                */
	/************************************************************************/

	/*!
	\brief
		Set user account and password info.
	
	\param pAccount
		User account.
	
	\param crPassword
		User password struct define by KOL.
	
	\return
		Nothing.
	*/
	void	SetAccountPassword( const char* pszAccount, const KSG_PASSWORD* pcPassword );	

	/*!
	\brief
		Set user account and password info.
	
	\param pAccount(out)
		User account.
	
	\param crPassword(out)
		User password struct define by KOL.
	
	\return
		Nothing.
	*/
	void	GetAccountPassword( char* pszAccount, KSG_PASSWORD* pPassword );



private:
	/************************************************************************/
	/*					Implement function					                */
	/************************************************************************/
	/*!
	\brief
		Process account login net message. If account login succeed m_Status change 
	from LL_S_ACCOUNT_CONFIRMING to LL_S_WAIT_ROLE_LIST,  or m_Status change from 
	LL_S_ACCOUNT_CONFIRMING to LL_S_IDLE.				
	
	\param pResponse 
		pResponse is the net response struct for account login.
		
	\return
		Nothing.			
	*/
	void	ProcessAccountLoginResponse( KLoginStructHead* pResponse );

	/*!
	\brief
		Process role list net message.If Process succeed m_Status change from 
	LL_S_WAIT_ROLE_LIST -> LL_S_ROLE_LIST_READY,  or m_Status don't change.	
	
	\param pResponse
		pResponse is the net response struct for role list.
	
	\return
		Nothing.		
	*/
	void	ProcessRoleListResponse( ROLE_LIST_SYNC* pResponse, int nSize );

	/*!
	\brief
		Process delete role response net message.If account login succeed m_Status change 
	from LL_S_ACCOUNT_CONFIRMING to LL_S_WAIT_ROLE_LIST,  or m_Status change from 
	LL_S_ACCOUNT_CONFIRMING to LL_S_IDLE.	
	
	\param pResponse
		pResponse is the net response struct for delete role response.
	
	\return
		Nothing.		
	*/
	void	ProcessDeleteRoleResponse( tagNewDelRoleResponse* pResponse );

	void	ProcessQuestResponse( void* pResponse );

	/*!
	\brief
		Process create role response net message.If create role succeed m_Status change 
	from LL_S_CREATING_ROLE -> LL_S_ROLE_LIST_READY,  or m_Status don't change.	
	
	\param pResponse
		pResponse is the net response struct for create role response.
	
	\return
		Nothing.		
	*/
	void	ProcessCreateRoleResponse( tagNewDelRoleResponse* pResponse );

	/*!
	\brief
		Transform char* ip to BYTE* .
	
	\param szAddress
		IP Address type of char*.
	
	\param pcAddress
		BYTE* IP address buffer.
	
	\return
		If transform succeed return true, or return false.		
	*/
	bool	GetIpAddress( const char* szAddress, BYTE* pcAddress );

	/*!
	\brief
		Register net agent to g_NetConnectAgent global object.
	
	\return
		Nothing.
	*/
	void	RegistNetAgent( void );	

	/*!
	\brief
		Unregister net agent from g_NetConnectAgent global object.
	
	\return
		Nothing.
	*/
	void	UnRegistNetAgent( void );

	/*!
	\brief
		Connect to the Game server.
	
	\param pcAddress
		BYTE* IP address buffer.
	
	\return
		If transform succeed return true, or return false.		
	*/
	bool	ConnectServer( const BYTE* pIpAddress, GUID *pUID = NULL );

	/*!
	\brief
		Send all kinds of login request to the game sever.
	
	\param pszAccount
		User account.
	
	\param pcPassword
		User password.
	
	\param nAction
		The kinds of request.
	
	\return
		If request send succeed return ture, or return false.
	*/
	bool	SendRequest( const char* pszAccount, const KSG_PASSWORD* pcPassword, int nAction, const char* pActiveKey );

	/*!
	\brief
		Close the UiConnectInfo wnd.
	\return
		Nothing
	*/
	void	ClearRoleList( void );

private:
	/************************************************************************/
	/*							Data member					                */
	/************************************************************************/
	bool						m_bCanCreate;
	LOGIN_LOGIC_STATUS			m_loginStatus;					//!< Login status.										
	short						m_nNumRole;						//!< Role count.
	KRoleList				    m_RoleList;						//!< Role list.
	unsigned long				m_uLeftTime;					//!< Account left time.
	unsigned long				m_uLeftTimeOfPoint;				//!< Account left time for left point.
	unsigned int				m_uDynamicKey;					//!< A dynamic random key for net gate to validate.(no use now)
	LOGIN_CHOICE				m_Choices;						//!< LOGIN_CHOICE object.
    KCriticalSection			m_loginLock;
	// LSL
	bool						m_bAccountValidate;
	bool						m_bSelectRole;
	DWORD						m_dwPort;
	int							m_IndexRoleLoginGame;
	bool						m_needAnswer;
};

/************************************************************************/
/*			Inline function implement                                   */
/************************************************************************/

inline void KLogin::SetGameServerIP( const char* szAccountServer, DWORD dwPort  )
{
	GetIpAddress( szAccountServer, m_Choices.ServerAddress );
	m_dwPort = dwPort;
}

inline BYTE* KLogin::GetGameServerIP( void )
{
	return m_Choices.ServerAddress;
}

inline LOGIN_LOGIC_STATUS KLogin::GetStatus( void ) 
{ 
	return m_loginStatus; 
}
 
int KLogin::GetRoleCount( void )
{
	return m_RoleList.size();
}

int	KLogin::GetRoleInfo( int nIndex, KRoleChiefInfo* pInfo )
{
	if ( nIndex >= 0 && nIndex < m_nNumRole )
	{
		memcpy( pInfo, m_RoleList[nIndex], sizeof(KRoleChiefInfo));
		return true;
	}
	return false;
}



// inline int KLogin::IsRoleNewCreated()
// { 
// 	return m_Choices.bIsRoleNewCreated; 
// }
// 
// inline unsigned int KLogin::GetDynamicKey() const
// {
// 	return m_uDynamicKey;
// }

extern	KLogin		g_LoginLogic;

#endif


