//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/15/2006 2:10
//      File_base        : KUiShortcutPlusWnd
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UISHORTCUTPLUS_H
#define UISHORTCUTPLUS_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLGameObject.h"
#include "ItemCommonDef.h"


#define SHORTCUTBAR_PLUS_COUNT 1
#define SHORTCU_PLUS_PER_BAR_COUNT 10

/*!
	屏幕右边的快捷栏
*/

class KUiShortcutPlusWndShowHide : public KUiWndSingleton<KUiShortcutPlusWndShowHide>
{
public:
	KUiShortcutPlusWndShowHide( const CEGUI::String & id_name);
	~KUiShortcutPlusWndShowHide(                             );
public:
   void Init(void);
public:
   bool HandleClick(const CEGUI::EventArgs& args );
};

class KUiShortcutPlusWnd : public KUiWndSingleton<KUiShortcutPlusWnd>
{
	typedef	bool (KUiShortcutPlusWnd:: * handleShortcutFun)( const CEGUI::EventArgs& args );
public:
	KUiShortcutPlusWnd(  const CEGUI::String& id_name );
	~KUiShortcutPlusWnd(								);
public:
	static bool         IsShowing(void);
	static unsigned int UpdateData( void );
	static unsigned int RefreshSelectedSkill( void );
	static unsigned int BeginGroupCD( KItemGroupCD_C* pGroupCD );
	static unsigned int EndGroupCD( KItemGroupCD_C* pGroupCD );
	static void AddImmediacy( KImmediacyParam* pImm );
	static void DelImmediacy( int nPos );
	static void Intonate(	int nIdx		);
	static void SelectSkill( int nIdx );
	static void	Disable	(	int nIdx 		);
	static void Show( void );
	static void Hide( void );
	static void	BeginSkillCD( int nSkillID, int ulCDTime );
	
	static void BeginCommonSkillCD( int ulCDTime );

	void	onCreate		( UIMDLEvent& rEvent	);
	void	onRelease		( UIMDLEvent& rEvent	);
	void	onChange		( UIMDLEvent& rEvent	);
	void	Init			( void					);

	void	RefreshImmediacy( FIND_ITEMINDEX_PARAM obj );
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

	bool	handleMouseLeave( const CEGUI::EventArgs& args );

	bool	IsObjExist		( FIND_ITEMINDEX_PARAM obj, int &pos );

private:
	int			m_nCurrentBarIdx;
	bool		m_bDragFlag;
	bool		m_bCast;
	bool        m_bShowing;
	handleShortcutFun	m_funList[SHORTCU_PLUS_PER_BAR_COUNT];
	handleShortcutFun	m_funListPlus[SHORTCU_PLUS_PER_BAR_COUNT];
	handleShortcutFun	m_funListCast[SHORTCU_PLUS_PER_BAR_COUNT];
};

#endif