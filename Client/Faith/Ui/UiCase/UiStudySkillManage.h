//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/12/2006 14:08
//      File_base        : UiStudySkillManage
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu) Brianyao (yaojie) edited
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UISTUDYSKILLMANAGE_H
#define UISTUDYSKILLMANAGE_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "TLVertScrollbar.h"

#ifndef MAX_SKILL_COUNT
#define MAX_SKILL_COUNT 8
#define MAX_STUDY_TIP_SIZE 2048
#endif
void ComMsgBoxHandler(void);

class KUiStudySkillManage : public KUiWndSingleton<KUiStudySkillManage>
{	
    friend void ComMsgBoxHandler(void);
	
	typedef	bool (KUiStudySkillManage:: * handleSkillObjectFun)( const CEGUI::EventArgs& args );
public:
    KUiStudySkillManage( const CEGUI::String& id_name );
    ~KUiStudySkillManage(								);
public:
	static void Show( bool bEnable = false );
	static void Hide();
	void Init();
	static unsigned Updatedata( int nSkillKindID, int nSkillID );
	
private:
	void	ShowSkillKindList( bool bShow = true );
	bool	ShowSkillList( int nSkillKind,  bool bShow = true );
	void	ShowSkillInfo( int nSkillID,  bool bShow = true );
	void	ClearSkillList( void );

	bool	handleSkillKind0( const CEGUI::EventArgs& args );
	bool	handleSkillKind1( const CEGUI::EventArgs& args );
	bool	handleSkillKind2( const CEGUI::EventArgs& args );
	bool	handleSkillKind3( const CEGUI::EventArgs& args );
	bool	handleSkillKind4( const CEGUI::EventArgs& args );
	bool	handleSkillKind5( const CEGUI::EventArgs& args );

	bool	handleSkill0( const CEGUI::EventArgs& args );
	bool	handleSkill1( const CEGUI::EventArgs& args );
	bool	handleSkill2( const CEGUI::EventArgs& args );
	bool	handleSkill3( const CEGUI::EventArgs& args );
	bool	handleSkill4( const CEGUI::EventArgs& args );
	bool	handleSkill5( const CEGUI::EventArgs& args );
	bool	handleSkill6( const CEGUI::EventArgs& args );
	bool	handleSkill7( const CEGUI::EventArgs& args );
	
	bool	handleLevelupSkill0( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill1( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill2( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill3( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill4( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill5( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill6( const CEGUI::EventArgs& args );
	bool	handleLevelupSkill7( const CEGUI::EventArgs& args );

	bool	SkillMouseMove( const CEGUI::EventArgs& args );
	bool	ClearPickUpSkill( const CEGUI::EventArgs& args );

	void	SelectSkillKind( int SkillKindIdx );
	bool	handleTreeScroll( const CEGUI::EventArgs& args );

	Window* getSkillListCliper( void )
	{
		return m_SkillListCliper;
	}

	Window* getSkillList( void )
	{
		return m_SkillList;
	}
	
private:
	bool	handleExit( const CEGUI::EventArgs& args	);
	bool	handleOK( const CEGUI::EventArgs& args		);
	void    handleSkillSelect(const int nIdx);
	bool	handleStaticImageButton( const CEGUI::EventArgs& args );
	void	clearAll();
	void	BtnAnimationOperation( Window* button, int buttonNUm, bool isPlay = true )
	{
		BtnAnimationOperation( ( TLButton* ) button, buttonNUm, isPlay );
	};
	void	BtnAnimationOperation( TLButton* button, int buttonNUm, bool isPlay = true );
private:
	int		m_uncheckItem;
	int		m_nSkillKindCount;
	int		m_nSkillCount;
	int		m_nCurrentSelSkillKind;
	int		m_SkillKindArray[MAX_SKILL_COUNT];
	int		m_SkillArray[MAX_SKILL_COUNT];
	int		m_OldSkillKindIdx;
	int     m_nCurrentSelectIdx;
	bool		m_isSkillNumUpdatedForEachKinfSkill[MAX_SKILL_COUNT];
	int			PlayTime;
	TLButton* m_LevelUpBtn[MAX_SKILL_COUNT];
	bool m_bDragFlag;
	bool m_SkillCanDrag[MAX_SKILL_COUNT];
	Window*	m_SkillListCliper;
	Window* m_SkillList;
	bool		m_isOpenAnimation;
	CEGUI::TLVertScrollbar	*d_pSkillListScrol;
};

#endif 
