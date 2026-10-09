#ifndef UI_FURY_BOX_H
#define UI_FURY_BOX_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/06/2007 16:30
//      File_base        : UiFuryBox
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ±¬»ê
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreShell.h"
#include "TLButton.h"
#include "TLStatic.h"

class KUiFuryBox : public KUiWndSingleton<KUiFuryBox>
{
	int              d_CurExp;
	int              d_TotalFuryAniFrame;
	int              d_TotalFuryHoverFrame;
	
	TLStaticImage *  d_FuryAnimation;
	TLStaticImage *  d_FuryHover;
	TLButton      *  d_ClickRect;

public:
	KUiFuryBox(  const CEGUI::String& id_name );
	~KUiFuryBox(								);
public:
	static void Update(const int nExp);
	static void Warnning(const int nNum);
	static void AutoFury();
	void Init(void);
private:
    void RefreshAnimation(void);
	bool HandleMouseClick(const EventArgs& args);
	void HideAllWarningNum(void);
	void ShowWarningNum(int nNum);
};

#endif