//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/09/2007 12:34
//      File_base        : UiChangeMapWnd
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UICHANGEMAPWND_H
#define UICHANGEMAPWND_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "../UiAdapter.h"
#include "TLProgressBar.h"
#include "TLStatic.h"
#include "GameDataDef.h"

class KUiChangeMapWnd : public KUiWndSingleton<KUiChangeMapWnd>
{
public:
	KUiChangeMapWnd( const CEGUI::String& id_name  );
	~KUiChangeMapWnd();
public:
	void	show( bool showElf, ChangeMapParam param );
	void	Init( void );
	static void	Breathe( void );
	bool	isMapLoading( void ) { return d_loading; }
	void	EndLoading( void );

private:
	bool	handleKeyDown( const CEGUI::EventArgs& args	);
	bool	handleHide( const CEGUI::EventArgs& args );

private:
	bool	d_loading;
	DWORD	d_loadingTime;
	DWORD	d_loadingBegin;
	int		d_tipCount;
	CEGUI::TLStaticImage*	d_loadingImage;
	CEGUI::TLProgressBar*	d_loadingProgress;
	CEGUI::TLStaticText*	d_tipText;
//	bool	d_showElf;
};

#endif