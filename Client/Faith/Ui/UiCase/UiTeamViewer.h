//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/29/2007 10:20
//      File_base        : UiTeamViewer
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UITEAMVIEWER_H
#define UITEAMVIEWER_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLVertScrollbar.h"

#define TEAMVIEWER_INVALID_TEAMLEADER_ID -1
#define TEAMVIEWERMEMBER_FILENAME "uisettings/layouts/TeamViewerMember.ls"

class KUiTeamViewer: public KUiWndSingleton<KUiTeamViewer>
{
public:
	 KUiTeamViewer(const CEGUI::String& id_name);
	~KUiTeamViewer();

	static void		Show(void);
	void			Init(void);

	bool			handleTeamClicked(const CEGUI::EventArgs& args);
	bool			handleRequestJoinTeam(const CEGUI::EventArgs& args);
	bool			handleScroll(const CEGUI::EventArgs& args);
	bool			handleWheelChanged(const CEGUI::EventArgs& args);
	bool			handleRefresh(const CEGUI::EventArgs& args);
	bool			handleClose(const CEGUI::EventArgs& args);

	void			Update(TeamBasicInfo* pTeamInfo, int nTeamCount);

private:
	void			Clear();
	void			ClearSelectedTeam();

private:
	TLButton*			d_pJoinTeamBtn;
	TLButton*			d_pRefreshBtn;
	TLVertScrollbar*	d_pScroll;
	CEGUI::Window*		d_pTeamsList;
	CEGUI::Window*		d_pTeamMembers;
	CEGUI::Window*		d_pTeam[MAX_TEAM_BASIC_INFO_LIST_SIZE];

	int					d_teamLeaderId[MAX_TEAM_BASIC_INFO_LIST_SIZE];
	int					d_curLeaderId;
};

#endif