//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 04/22/2008
//      File_base        : UiBattleResult
//      File_ext         : cpp
//      Author           : DarkMagic(DuanMu)
//      Description      : 战场统计
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _UIBATTLERESULT_H_
#define _UIBATTLERESULT_H_

#define UI_BATTLE_RESULT_MAX_LIST_NUM 10

#include "CEGUI.h"
#include "TLButton.h"
#include "TLTree.h"
#include "TLTreeItem.h"
#include "Ui/UiCommon.h"

class KUiBattleResult : public KUiWndSingleton<KUiBattleResult>
{
	TLStaticImage *			m_pListPanel;
	TLButton *				m_pCloseBnt;
	int						m_iInfoNum;
	UICombatTopMemberInfo	m_TopPlayerInfo[UI_BATTLE_RESULT_MAX_LIST_NUM];

public:
	void	onCreate			(UIMDLEvent& rEvent);
	void	onRelease			(UIMDLEvent& rEvent);
	void	onChange			(UIMDLEvent& rEvent);
	void	Init				(void);
	bool	OnCloseBnt			(const CEGUI::EventArgs & args);
	bool	OnNameListClick		(const CEGUI::EventArgs & args);
	bool	OnScoreListClick	(const CEGUI::EventArgs & args);
	bool	OnMouseHover		(const CEGUI::EventArgs & args);
	void	RefreshList			(UICombatTopMemberInfo * playerInfo, int num);

public:
	static void Show ();
	static void Hide();	

public:
	KUiBattleResult(const CEGUI::String & strPath);
	~KUiBattleResult();
};


//////////////////////////////////////////////////////////////////////////
///					KUiSmallBattleFieldResult
///					Caolei+ 2008.7.22  小战场结果显示
//////////////////////////////////////////////////////////////////////////
class KUiSmallBattleFieldResult : public KUiWndSingleton<KUiSmallBattleFieldResult>
{
public:
	KUiSmallBattleFieldResult( const CEGUI::String& strPath );
	~KUiSmallBattleFieldResult();

	void	ShowResult( SMALL_BATTLE_FIELD_RESULT* pResult );

	void	Init();
	static void Show();
	
private:
	bool	btnClose_MouseClick( const CEGUI::EventArgs& args );
	string&  getResultDes( int selfScore, int emenyScore );

	TLStaticText* m_pTime;
	TLStaticText* m_pScore;
	TLStaticText* m_pRepute;

	string scoreDes_win;
	string scoreDes_lose;
	string scoreDes_draw;
};

#endif