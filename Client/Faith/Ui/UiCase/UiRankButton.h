//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/09/2008 
//      File_base        : UICharts.h
//      File_ext         : h
//      Author           : marryme (Chen Lin)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "../UiCommon.h"
#include "TLListbox.h"
#ifndef UIRANKBUTTON_H
#define UIRANKBUTTON_H

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000
class KUiRankButton   : public KUiWndSingleton< KUiRankButton >
{
public:
	KUiRankButton( const String& id_name );

	virtual ~KUiRankButton();
	void Init( void );
	void MoveToRightEdge();
private:
	bool handleButtonEvent( const CEGUI::EventArgs& e );
private:
	TLButton*  m_Button;

};

#endif // ifndef UIRANKBUTTON_H
