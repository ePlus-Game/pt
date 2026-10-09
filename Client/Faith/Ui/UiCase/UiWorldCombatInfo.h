//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/25/2008 10:13
//      File_base        : UiWorldCombatInfo
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : WorldCombatInfo
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UI_WORLD_COMBAT_INFO_H
#define UI_WORLD_COMBAT_INFO_H

#include "..\UiCommon.h"
#include "CEGUI.h"

class KUiWorldCombatInfo : public KUiWndSingleton<KUiWorldCombatInfo>
{
public:
	KUiWorldCombatInfo( const CEGUI::String& id_name );
	~KUiWorldCombatInfo();
public:
	static void Show( void );
	static void Update(const WorldCombatUIParam  * pInfo);
	void	    Init();
};

#endif	
