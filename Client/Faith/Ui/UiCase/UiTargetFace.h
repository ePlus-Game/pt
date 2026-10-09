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
#ifndef UITARGETFACE_H
#define UITARGETFACE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#define MAX_TARGET_BUFFER 5

class KUiTargetFace : public KUiWndSingleton<KUiTargetFace>
{
public:
	KUiTargetFace(  const CEGUI::String& id_name );
	~KUiTargetFace(								);
public:
	void	Init();
	static void Show();
	static void Hide();
	static unsigned int UpdateData( void ); 
private:
	bool	ShowRoleMenu( const CEGUI::EventArgs& args );

private:
	KTargetInfo				m_TargetInfo;
	int                     m_TargetKind;
	static	KBufferSyncInfo m_buffSyncInfo[MAX_TARGET_BUFFER];
	CEGUI::Window*				m_Blood;
	CEGUI::Window*	m_Magic;
	CEGUI::Window*	m_Level;
	CEGUI::Window*	m_pRoleFace;
	CEGUI::Window*  m_pRoleFaceFrame;
	CEGUI::Window*	m_pDisplay;
	CEGUI::Window*	m_Kulou;

	CEGUI::ProgressBar*	m_pTargetBloodBar;
	CEGUI::ProgressBar*	m_ProgressBar;
	CEGUI::Window*		m_Name;
};

#endif