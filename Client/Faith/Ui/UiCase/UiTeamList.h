//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/18/2006 10:20
//      File_base        : UiTeamList
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UITEAMLIST_H
#define UITEAMLIST_H

#include "../uicommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"


class KUiTeamHideShow: public KUiWndSingleton<KUiTeamHideShow>
{
public:
	 KUiTeamHideShow(  const CEGUI::String& id_name );
	~KUiTeamHideShow(								);
public:
	void	    Init( void );
public:
	bool        TeamHandleClick(const CEGUI::EventArgs& args);
	bool		GroupSwitch(const CEGUI::EventArgs& args);
	bool		OpenGroupWnd(const CEGUI::EventArgs& args);
	void		SetTGBBtnState( bool bEnable );
	void		SetOpenTGBBtnState( bool bEnable );
private:
	TLButton* d_TGButton;
	TLButton* d_OpenTGButton;
};

#define MAX_TEAMMEMBER_COUNT 5

class KUiTeamList : public KUiWndSingleton<KUiTeamList>
{
public:
	KUiTeamList(  const CEGUI::String& id_name );
	~KUiTeamList(								);

public:
	static void SetCanShow(const bool bShow);
	static bool GetCanShow(void);
	static void SetDisapearFlag(void);
	static void Show( void );
	static void Hide( void );
	static unsigned int	UpdateData( void );
	void		Init( void );
	void		HideTeamMenu();
	void		QuitGame();

private:
	
	bool			Captain(const CEGUI::EventArgs& args);
	bool			Kick(const CEGUI::EventArgs& args);
	bool			Leave(const CEGUI::EventArgs& args);
	//bool			Exp(const CEGUI::EventArgs& args);
	bool			Friend(const CEGUI::EventArgs& args);
	bool			OpenBT(const CEGUI::EventArgs& args);
	bool			Chat(const CEGUI::EventArgs& args);
	void			HideAllMember( void );
	void			ShowAllMember( void );
	bool			MouseButtonDown( const CEGUI::EventArgs& args );
private:
	KUiTeamMemberItem	m_pMembersList[MAX_TEAMMEMBER_COUNT];
	int					m_nMemberNo;
	CEGUI::Window*	m_pRolePopmenu;
	CEGUI::PushButton		*m_pCaption;
	CEGUI::PushButton		*m_pKick;
	CEGUI::PushButton		*m_pLeave;
	CEGUI::PushButton		*m_pExp;
	CEGUI::PushButton		*m_pFriend;
	CEGUI::PushButton		*m_pChat;
//	CEGUI::PushButton		*m_pOpenBT;
	int				  	     m_nCurMember;
	bool				     m_bListShow;
	bool                     m_bHideDisapear;
	float                    m_FrontWidth;
private:
	static bool     m_bCanShow;
};

#endif