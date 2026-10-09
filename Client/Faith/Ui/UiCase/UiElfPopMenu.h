//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiElfPopMenu
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : °ïÖúÐ¡¾«Áé²Ëµ¥
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _KUiElfPopMenuPOPMENU_H_
#define _KUiElfPopMenuPOPMENU_H_

#include "../uicommon.h"

using namespace CEGUI;

class KUiElfPopMenu : public KUiWndSingleton<KUiElfPopMenu>
{
public:
    KUiElfPopMenu( const CEGUI::String& id_name	);
    ~KUiElfPopMenu(								);
	
	static	void	Show();
	static	void	Hide();
	void			Init();

	void			SetPosition(Point pos);

private:
	bool			handleCloseQuery( const EventArgs& e );
	bool			handleCloseSearchHelp( const EventArgs& e );
	bool			handleCloseAll( const EventArgs& e );

};

#endif