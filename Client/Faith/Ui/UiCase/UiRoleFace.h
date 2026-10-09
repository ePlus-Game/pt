//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/14/2006 19:05
//      File_base        : KUiHeadToolBar
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIROLEFACE_H
#define UIROLEFACE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreShell.h"
#include "TLButton.h"
#include "GameDataDef.h"

class KUiRoleFace : public KUiWndSingleton<KUiRoleFace>
{
public:
	KUiRoleFace(  const CEGUI::String& id_name );
	~KUiRoleFace(								);
public:
	static void Show();
	static void Hide();
	void Init();
	static unsigned int UpdateData( void ); 
	static void	SetData( PK_MODE pkFlag );
	static PK_MODE	GetData() { return s_Flag; };

	Rect	getArea();

	void	PlayerAttackNotify();	// 被玩家攻击通知

private:
	bool	handlePkButton( const CEGUI::EventArgs &args );
	bool	handlePopMenu( const CEGUI::EventArgs &args );
private:
	KUiPlayerBaseInfo		m_BaseInfo;
	KUiPlayerRuntimeInfo	m_RuntimeInfo;
	KUiPlayerAttribute		m_RuntimeAttribute;
	CEGUI::Window*			m_pTeamLeader;	
	CEGUI::Window*			m_pRoleName;			
	CEGUI::Window*			m_pRoleBlood;			
	CEGUI::Window*			m_pRoleMagic;		
	CEGUI::ProgressBar*		m_pRoleBloodBar;	
	CEGUI::ProgressBar*		m_pRoleMagicBar;
	CEGUI::Window*			m_pDisplay;
	CEGUI::Window*			m_pDisplayLevel;
	CEGUI::TLButton*		m_pPkButton;
	static	PK_MODE		    s_Flag;
	char *					m_sPkName;
	KUiPlayerTeam			m_PlayerTeam;

	//PK状态菜单是否关闭的状态
	bool					m_bPKStatus;
	//是否被人PK
	TLStaticImage*			m_pPKNotifyImg;
};

#endif