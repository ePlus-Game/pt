//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/26/2006 20:02
//      File_base        : UiTongCreate
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UITONGCREATE_H
#define UITONGCREATE_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLGameObject.h"
#include "SocialComDef.h"

class KUiTongCreate : public KUiWndSingleton<KUiTongCreate>
{
public:
    KUiTongCreate( const CEGUI::String& id_name );
    ~KUiTongCreate(								);
public:
	static void Show( void ) {};
	static void Show( int eCompoundType  );
	void	Init();
private:
	
	bool	handleExit( const CEGUI::EventArgs& args	);
	bool	handleOK( const CEGUI::EventArgs& args		);
	bool	handleEditKeyDown( const CEGUI::EventArgs& args );
    bool    handleCreate( const CEGUI::EventArgs& args		);
	static void    ShowActually(int eCompoundType);
private:
	int d_type;
};

#endif