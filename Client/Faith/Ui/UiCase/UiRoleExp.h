//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/13/2007 22:08
//      File_base        : UiRoleExp
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 经验值
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIROLEEXP_H
#define UIROLEEXP_H

#include "CEGUI.h"
#include "../UiCommon.h"
#include "GameDataDef.h"

class KUiRoleExp : public KUiWndSingleton<KUiRoleExp>
{
public:
	enum MessageNum
	{
		CM_REWARDEXP_UNDER_LEVEL = 9,
		CM_REWARDTIME_UNDER_LEVEL = 10,
		CM_REWARDEXP_EXCEED_LEVEL = 11,
		CM_REWARDTIME_EXCEED_LEVEL = 12,
		CM_REWARDEXP_ENTER_LEVEL = 13,
		CM_REWARDTIME_ENTER_LEVEL = 14,
	};

public:
	KUiRoleExp( const CEGUI::String& id_name );
	~KUiRoleExp();
public:
	static void Show( void );
	static void Hide( void );
	void		Init();
	void		UpdateDataSkillExp(DWORD curSkillExp, DWORD fullSkillExp);
	void		UpdateDataRoleExp(DWORD curRoleExp, DWORD fullRoleExp);

	//caolei+ 升级保险和任务保险
	void		UpdateRewardExp( /*DWORD curExp, DWORD fullExp*/ );
	void		UpdateRewardTime( /*DWORD curExp, DWORD fullExp*/ );

	void		RefreshTip();
private:
	void		RefreshExpBar( DWORD curExp, DWORD fullExp, Window* expWnd, ProgressBar* expBar, MessageNum msgNum, bool useFloat = false );
	void		setTip( ProgressBar* expBar, MessageNum msgNum, int minLevel );
	void		InnerUpdateRewardExp( /*DWORD curExp, DWORD fullExp*/ );
	void		InnerUpdateRewardTime( /*DWORD curExp, DWORD fullExp*/ );

private:
	CEGUI::Window*			m_pSkillExp;
	CEGUI::ProgressBar*		m_pSkillExpBar;

	CEGUI::Window*			m_pRoleExp;
	CEGUI::ProgressBar*		m_pRoleExpBar;

	CEGUI::Window*			m_pRewardExp;
	CEGUI::ProgressBar*		m_pRewardExpBar;

	CEGUI::Window*			m_pRewardTime;
	CEGUI::ProgressBar*		m_pRewardTimeBar;
};

class KUiCastSkillExp : public KUiWndSingleton<KUiCastSkillExp>
{
public:
	KUiCastSkillExp( const CEGUI::String& id_name );
	~KUiCastSkillExp();
public:
	static void Show( void );
	void		Init();
	void		UpdateData(DWORD currentExp, DWORD fullExp);
	bool		handleCast(const CEGUI::EventArgs& args);
private:
	CEGUI::PushButton*		m_pCastExpSkillBtn;
	CEGUI::StaticImage*		m_pCastExpSkillCount;
	CEGUI::StaticImage*		m_pCastExpSkillDisplay;
};


/********************************************************************
/*						class: KUiRewardExpNotify
*********************************************************************/
class KUiRewardExpNotify : public KUiWndSingleton<KUiRewardExpNotify>
{
public:
	KUiRewardExpNotify( const CEGUI::String& id_name );
	virtual ~KUiRewardExpNotify();
public:
	void		Init();
	static void	Hide();
	void		Notify( DWORD rewardExp, DWORD rewardTime );
	bool		btnCancel_MouseClick( const CEGUI::EventArgs& e );
private:
	StaticText* m_stRewardExp;
	StaticText* m_stRewardTime;
};


#endif



