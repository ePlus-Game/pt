//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/08/2006 11:12
//      File_base        : UiPKFilter
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIPKFILTER_H
#define UIPKFILTER_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "TLRadioButton.h"

class KUiPKFilter : public KUiWndSingleton<KUiPKFilter>
{
	typedef std::map<int, CEGUI::PushButton*> PKIndex;
public:
	KUiPKFilter( const CEGUI::String& id_name  );
	~KUiPKFilter();

public:
	static void	Show( void );
	void	Init( void );
	static void UpdatePKState( int nPK );
protected:
	bool	handlePK0( const CEGUI::EventArgs& args );
	bool	handlePK1( const CEGUI::EventArgs& args );
	bool	handlePK2( const CEGUI::EventArgs& args );
	bool	handlePK3( const CEGUI::EventArgs& args );
	bool	handlePK4( const CEGUI::EventArgs& args );
	bool	handlePK5( const CEGUI::EventArgs& args );
	bool	handlePK6( const CEGUI::EventArgs& args );
	bool	handlePK7( const CEGUI::EventArgs& args );
	bool	handlePK8( const CEGUI::EventArgs& args );

private:
	void	SetPKState( int nPK );

private:
	PKIndex	d_pkIndex;
};

#endif