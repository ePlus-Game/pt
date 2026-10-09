//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/26/2007 9:50
//      File_base        : UiDelComfirm
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 删除角色确认
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef  _DELCOMFIRM_H_
#define  _DELCOMFIRM_H_

#include "../UiCommon.h"
#include "CEGUI.h"

using namespace CEGUI;

class KUiDelComfirm : public KUiWndSingleton<KUiDelComfirm>
{
public:
	KUiDelComfirm( const CEGUI::String& id_name );
	~KUiDelComfirm();

public:
	static void Show( void );
	void		Init( void );

	bool		handleComfirm( const CEGUI::EventArgs& args );
	bool		handleCancel( const CEGUI::EventArgs& args );

	bool		handleForgetPwd2(const CEGUI::EventArgs& args);
	bool		handleClose(const CEGUI::EventArgs& args);
private:
	CEGUI::Window*	m_StaticText;	
	CEGUI::Window*	m_Password;

	CEGUI::Window*	m_StaticTextDetail;
	
};

#endif