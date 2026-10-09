 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/07/2006
//      File_base        : KUiItemTip
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 物品（包括装备）信息的tip显示
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiItemTip_H
#define KUiItemTip_H

#include "../uicommon.h"
#include "TLButton.h"
#include "TLStatic.h"

using namespace CEGUI;

class KUiItemTip : public KUiWndSingleton<KUiItemTip>
{
public:
	enum TipPos
	{
		Left,
		Right,
		Top,
		Bottom,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
	};
private:

	TLButton*		d_closeBtn;
	TLStaticText*	d_tipText;
	TLStaticText*	d_compareTipText;

private:
	void	getChild(void);
	void	adjustPos(CEGUI::Rect aroundArea, TipPos tipPos);

protected:
	bool close(const EventArgs& args);

public:
    KUiItemTip( const CEGUI::String& id_name	);
    ~KUiItemTip(								);
	
	void	Init();
	void	show(char* layoutText, CEGUI::Rect aroundArea, TipPos tipPos = TopRight);
	void	showCompare(char* layoutText);

	
	static TipPos textToTipPos(char* text);
};


#endif