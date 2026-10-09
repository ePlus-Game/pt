//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/15/2006 14:49
//      File_base        : UiSelPlayer
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UISELPLAYER_H
#define  UISELPLAYER_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "../../Login/Login.h"
#include "TLVertScrollbar.h"
#include "TLListbox.h"

class KUiSelPlayer;

class KUiRoleItem
{
public:
	KUiRoleItem();
	~KUiRoleItem() {};
public:
	void	loadWnd( int nIdx, KUiSelPlayer* pWnd );
	void	setRole( KRoleChiefInfo* pRole	);
	void	clearRole( void );
	bool	handleRole	 ( const CEGUI::EventArgs& args		);	
	bool	handleDCRole( const CEGUI::EventArgs& args		);
	Window* getFront( void ) { return d_roleFaceFront; }
	int		getWidth( void ) { return d_window != NULL ? d_window->getWidth(Absolute) : 0; }
	int		getHeight( void ) { return d_window != NULL ? d_window->getHeight(Absolute) : 0; }
private:
	Window* d_window;
	Window* d_roleFace; //程序中无用
	Window* d_roleFaceBg;//职业头像
	RadioButton* d_roleFaceFront;//选中高亮
	Window* d_roleInfo;
	Window* d_roleName;
	Window* d_roleLeve;
	Window* d_roleLastMap;
	Window* d_roleDestoryTime;
	Checkbox* d_roleFreeze;
	Checkbox* d_roleForbid;
	KUiSelPlayer* d_SelPlayerWnd;
	int d_index;
};

class KUiSelPlayer : public KUiWndSingleton<KUiSelPlayer>
{
	friend class KUiRoleItem;
public:
	KUiSelPlayer(  const CEGUI::String& id_name	 );
	~KUiSelPlayer(								 );

public:
	static void					Show		 ( void								);
	static void					Hide		 ( void								);		
	void						Init		 ( void								);
public:
	void						setRoleList	 (  KRoleList* pRoleList			);
	void						setFileName	 ( char *iniFileName				);
	void						setNewRoleName( char *newName					);
	inline int					GetRoleIndex( void ) { return m_nSelRoleIdx; };
	bool						handleFirstRole	 ( void );	
private:
	void						setRole		 ( int nIdx, KRoleChiefInfo* pRole	);
	void						clearRole	 ( int nIdx							);
	void						selectRole	 ( int nRoleKind					);
	void						clearRadioBtn( void								);
private:
	bool						handleListBoxClick( const CEGUI::EventArgs& args );
    bool						handleDListBoxClick( const CEGUI::EventArgs& args );
	bool						handleTreeScroll( const CEGUI::EventArgs& args );
    bool						handleCreate ( const CEGUI::EventArgs& args		);
	bool						handleDelete ( const CEGUI::EventArgs& args		);
	bool						handleEntry	 ( const CEGUI::EventArgs& args		);
	bool						handleBack	 ( const CEGUI::EventArgs& args		);	
	bool						handleKeyDown( const CEGUI::EventArgs& args		);
	bool						handleShowList( const CEGUI::EventArgs& args	);
	void						IsFirstLogin( const char *roleName );
private:
	static const unsigned int	CreateButtonID;
	static const unsigned int	EnterButtonID;
	static const unsigned int	DeleteButtonID;
	static const unsigned int	ExitButtonID;
	//KRoleList*					m_pRoleList;	
	int							m_nSelRoleIdx;
	CEGUI::Window*				m_pRoleListWnd;
	CEGUI::Window*				m_pRoleListWndCliper;
	char						m_filename[COMMON_CLIENT_MSG_LEN_128];
	char						m_newRoleName[COMMON_CLIENT_MSG_LEN_32];
	KUiRoleItem					m_roleItem[MAX_PLAYER_IN_ACCOUNT];
	int							m_roleCurCount;
	CEGUI::TLVertScrollbar*		m_proleListScrol;
	CEGUI::TLListbox*			m_roleListbox;
	CEGUI::Window*				m_roleListboxBG;
};

#endif 
