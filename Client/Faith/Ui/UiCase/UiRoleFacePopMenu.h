//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/09/2007 19:05
//      File_base        : KUiHeadToolBar
//      File_ext         : h
//      Author           : likun
//      Description      : rolofaceµ¯³ö²Ëµ¥
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIROLEFACEPOPMENU_H
#define UIROLEFACEPOPMENU_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreShell.h"
#include "TLButton.h"
#include "GameDataDef.h"

class KUiRoleFacePopMenu : public KUiWndSingleton<KUiRoleFacePopMenu>
{
public:
	KUiRoleFacePopMenu( const CEGUI::String& id_name );
	~KUiRoleFacePopMenu();
	static void Show();
	static void SetPosition(Point pos);
	void		Init();
	bool		handleLeaveTeamBtn( const CEGUI::EventArgs &args );
	bool		handleExpSet( const CEGUI::EventArgs &args );
	bool		handleCreateRaid(const EventArgs & args);
	bool		handleOpenAutoAccept(const EventArgs & args);
	bool		handleCloseAutoAccept(const EventArgs & args);
	
private:
	TLButton * m_pLeaveTeam;
	TLButton * m_pExpSet;
	TLButton * m_pCreateRaid;
	TLButton * m_pOpenAutoAccept;
	TLButton * m_pCloseAutoAccept;
};

#endif