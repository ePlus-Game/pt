//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/15/2006 2:10
//      File_base        : KUiShortcutWnd
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UISHORTCUT_H
#define UISHORTCUT_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLGameObject.h"
#include "ItemCommonDef.h"
#include <map>
#include "UiStudySkillManage.h"


#define SHORTCUTBAR_COUNT 1
#define SHORTCUT_PER_BAR_COUNT 10

/************************************************************************/
/*                                                                      */
/************************************************************************/
class KUiLRSkillWnd  : public KUiWndSingleton<KUiLRSkillWnd>
{
	friend class KUiShortcutWnd;
	typedef std::map<std::string, std::string> _shortcutkey;
public:
	KUiLRSkillWnd( const CEGUI::String& id_name );
	~KUiLRSkillWnd( void );
public:
	virtual void	Init( void );
	bool ShowSkills( bool bLeft );
	bool HandleSelectSkill( const CEGUI::EventArgs& args );
	bool HandleKeyDown( const CEGUI::EventArgs& args );
	void SelectSkill( const char* szName, bool bLeft );
	void ClearAllShortcutKey( void );
	void ClearAllLRSkill( void );
	void AddShortcutKey( bool bLeft, const char* szName, const char* szKey );
	void	setFileName( const char *iniFileName )
	{
		strcpy(m_filename, iniFileName);
	}
	void	setRoleName( const char* szRoleName  )
	{
		strcpy(m_rolename, szRoleName);
	}
protected:
private:
	bool	m_bLeft;
	int		m_nSkillKindCount;
	int		m_nSkillCount;
	int		m_SkillKindArray[MAX_SKILL_COUNT];
	int		m_SkillArray[MAX_SKILL_COUNT];
	_shortcutkey m_lKeyList;
	_shortcutkey m_rKeyList;
	char				m_filename[COMMON_CLIENT_MSG_LEN_256];
	char				m_rolename[COMMON_CLIENT_MSG_LEN_64];
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
class KUiShortcutWnd : public KUiWndSingleton<KUiShortcutWnd>
{
	typedef std::map<int, std::string> _shortcutkey;
	typedef	bool (KUiShortcutWnd:: * handleShortcutFun)( const CEGUI::EventArgs& args );
public:
	KUiShortcutWnd(  const CEGUI::String& id_name );
	~KUiShortcutWnd(								);
public:
	void	Init( void	);
	static unsigned int UpdateData( void );
	static unsigned int RefreshSelectedSkill( void );
	static unsigned int BeginGroupCD( KItemGroupCD_C* pGroupCD );
	static unsigned int EndGroupCD( KItemGroupCD_C* pGroupCD );
	static void AddImmediacy( KImmediacyParam* pImm );
	static void DelImmediacy( int nPos );
	static void Intonate( int nIdx ); //, bool bAlt );
	static void SelectSkill( int nIdx );
	static void	Disable	(	int nIdx 		);
	static void	Show( void );
	static void Hide( void );
	static void BeginSkillCD( int nSkillID, int ulCDTime );
	
	static void BeginCommonSkillCD( int ulCDTime );
	void	ClearAllShortcutKey( void );
	void	AddShortcutKey( int nIdx, const char* szKey );
	void	onCreate		( UIMDLEvent& rEvent	);
	void	onRelease		( UIMDLEvent& rEvent	);
	void	onChange		( UIMDLEvent& rEvent	);

	void	RefreshImmediacy( FIND_ITEMINDEX_PARAM obj );

	void	SelectLSkill( TLGameObject* pGO, const String& key );
	void	SelectRSkill( TLGameObject* pGO, const String& key );

	void	LoadDefaultLSkill( int nSkillID, bool bLeft );
	void	SaveAllItem( void );

private:
	CEGUI::Window* GetSelShortcutKey( int nSel ); 
	CEGUI::Window* GetCurrentBar( void );
	void	SetCurrentBar( int nSel );
	void	ClearAllShortcut( void );
	bool	onLBDown( CEGUI::TLGameObject* pGO,int nSel );
	bool	onMouseMove( CEGUI::TLGameObject* pGO,int nSel );

	bool	handleMouseUp0( const CEGUI::EventArgs& args );	
	bool	handleMouseUp1( const CEGUI::EventArgs& args );
	bool	handleMouseUp2( const CEGUI::EventArgs& args );
	bool	handleMouseUp3( const CEGUI::EventArgs& args );
	bool	handleMouseUp4( const CEGUI::EventArgs& args );
	bool	handleMouseUp5( const CEGUI::EventArgs& args );
	bool	handleMouseUp6( const CEGUI::EventArgs& args );
	bool	handleMouseUp7( const CEGUI::EventArgs& args );
	bool	handleMouseUp8( const CEGUI::EventArgs& args );
	bool	handleMouseUp9( const CEGUI::EventArgs& args );


	bool	handleMouseMove0( const CEGUI::EventArgs& args );	
	bool	handleMouseMove1( const CEGUI::EventArgs& args );
	bool	handleMouseMove2( const CEGUI::EventArgs& args );
	bool	handleMouseMove3( const CEGUI::EventArgs& args );
	bool	handleMouseMove4( const CEGUI::EventArgs& args );
	bool	handleMouseMove5( const CEGUI::EventArgs& args );
	bool	handleMouseMove6( const CEGUI::EventArgs& args );
	bool	handleMouseMove7( const CEGUI::EventArgs& args );
	bool	handleMouseMove8( const CEGUI::EventArgs& args );
	bool	handleMouseMove9( const CEGUI::EventArgs& args );

	bool	handleShortcut0( const CEGUI::EventArgs& args );	
	bool	handleShortcut1( const CEGUI::EventArgs& args );
	bool	handleShortcut2( const CEGUI::EventArgs& args );
	bool	handleShortcut3( const CEGUI::EventArgs& args );
	bool	handleShortcut4( const CEGUI::EventArgs& args );
	bool	handleShortcut5( const CEGUI::EventArgs& args );
	bool	handleShortcut6( const CEGUI::EventArgs& args );
	bool	handleShortcut7( const CEGUI::EventArgs& args );
	bool	handleShortcut8( const CEGUI::EventArgs& args );
	bool	handleShortcut9( const CEGUI::EventArgs& args );

	bool	handleMouseLeave( const CEGUI::EventArgs& args );
	
	bool	IsObjExist		( FIND_ITEMINDEX_PARAM obj, int &pos );
	
	bool	handleL( const CEGUI::EventArgs& args );
	bool	handleR( const CEGUI::EventArgs& args );
private:
	int					m_nCurrentBarIdx;
	bool				m_bDragFlag;
	bool				m_bCast;
	handleShortcutFun	m_funList[SHORTCUT_PER_BAR_COUNT];
	handleShortcutFun	m_funListPlus[SHORTCUT_PER_BAR_COUNT];
	handleShortcutFun	m_funListCast[SHORTCUT_PER_BAR_COUNT];
	TLGameObject*		m_left;
	TLGameObject*		m_right;
	_shortcutkey		m_KeyList;
	bool				m_bLoading;
};

#endif
