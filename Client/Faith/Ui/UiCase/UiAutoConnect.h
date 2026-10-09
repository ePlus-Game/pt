//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/13/2007 9:50
//      File_base        : UiAutoConnect
//      File_ext         : h
//      Author           : Lucien (LIUSiliang)
//      Description      : 文件功能描述: 自动重连
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _UIAUTOCONNECT_H_
#define _UIAUTOCONNECT_H_

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "CoreUseNameDef.h"
#include "Login/Login.h"

class KUiAutoConnect : public KUiWndSingleton<KUiAutoConnect>
{
public:
	KUiAutoConnect( const CEGUI::String& id_name );
	~KUiAutoConnect();

public:
	void		QuitGame( void );
	static void Show				( void									);
	void		Init				( void									);
	
	bool		handleAutoConnect	( const CEGUI::EventArgs& args 			);
	bool		handleQuitGame		( const CEGUI::EventArgs& args			);
	
	void		Login				( void									);

	// 自动重连面版的显示
	void		DisplayAutoConnect	( void									);

	// 重新开始重连过程
	void		ReStart				( void									);

	// 记录用户名和密码
	void		SetPassParam		( const char* pUser, const char* pPass	);

	// 等待超作
	void		WaitAutoConnect		( void									);

	// 超作超时
	void		OverTime			( void									);

	// 选择角色索引
	inline void SetRoleIndex		( int roleIndex	)	{ m_nRoleIndex = roleIndex; };
	inline int	GetRoleIndex		( void			)	{ return m_nRoleIndex;		};
	// 选择角色名字
	inline void   SetRoleName		( String name	)	{ m_RoleName = name;		};
	inline char*  GetRoleName		( void			)	{ return Utf8ToAnsi(m_RoleName); };

	inline char* GetUserName		( void			)	{ return m_szUser;			};
	inline char* GetPassword		( void			)	{ return m_szPass;			};
	
	// 开始自动重连
	inline void SetStartConnect		( bool b		)   { m_bStartConnect = b;		};
	// 退出游戏
	inline void SetQuitGame			( bool b		)   { m_bQuitGame = b;			};

	// 重置自动重连次数
	inline void ResetConnectTimes	( void			)	{ m_nConnectTimes = m_nOldConnectTimes;	};
	// 设置自动重连次数
	inline void SetConnectTimes		( int times		)	{ m_nConnectTimes = times;	m_nOldConnectTimes = times; };

private:
	void		CombineTextPoint	( char* str, int i						);

private:
	CEGUI::Window*				m_StaticText;
	CEGUI::Window*				m_AutoConnect;
	CEGUI::Window*				m_QuitGame;

	CEGUI::String				m_RoleName;

	char						m_szUser[COMMON_CLIENT_MSG_LEN_32];		// 用户名
	char						m_szPass[COMMON_CLIENT_MSG_LEN_32];		// 用户密码
	int							m_nRoleIndex;							// 角色索引

	int							m_nTimeToConnect;						// 自动重连时间 （大于5，小于100）
	int							m_nOldConnectTimes;						// 自动重连次数 （-1 为无限次）
	int							m_nConnectTimes;						// 自动重连次数 （-1 为无限次）

	int							m_nDisplayTimeControl;
	int							m_nDisplayTime;

	int							m_nOverTimeControl;
	int							m_nOverTime;

	int							m_nOverTimeLimit;

	bool						m_bStartConnect;						// 是否开始连接
	bool						m_bQuitGame;							// 是否退出
};

extern const char*	g_szPoint[6];

#endif



