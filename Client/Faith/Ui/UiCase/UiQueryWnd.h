//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiQueryWnd
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 查询框
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _KUiQUERY_H_
#define _KUiQUERY_H_

#include "../uicommon.h"
#include "IQueryInfo.h"
#include "TLEditbox.h"
#include "TLRadioButton.h"
#include "GameDataDef.h"
#include <vector>

using namespace CEGUI;

struct SearchContent 
{
	String text;
	int		id;
	QueryType queryType;
	QueryResultType	queryResultType;
};

class KUiQueryWnd : public KUiWndSingleton<KUiQueryWnd>
{
public:
    KUiQueryWnd( const CEGUI::String& id_name	);
    ~KUiQueryWnd(								);
	
	static	void	Show();
	static	void	Hide();
	void			Init();

private:
	void			getChild(void);

	bool			handleSearch( const EventArgs& e );
	bool			handleClose( const EventArgs& e );
	bool			handleSelectType( const EventArgs& e );
	bool			handleKeyDown( const EventArgs& e );

	bool			btnClose_MouseClick( const EventArgs& e );

private:
	CEGUI::TLEditbox		*d_pSearchText;		// 查询内容
	QueryType				d_queryType;		// 查询类别

	// 查询类别控件指针
	CEGUI::TLRadioButton	*d_pAll;
	CEGUI::TLRadioButton	*d_pItem;
	CEGUI::TLRadioButton	*d_pSkill;
	CEGUI::TLRadioButton	*d_pQuest;
	CEGUI::TLRadioButton	*d_pLevel;
	CEGUI::TLRadioButton	*d_pNpc;
};

#endif