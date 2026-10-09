// UiEntrustComputer.h: interface for the KUiEntrustComputer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIENTRUSTCOMPUTER_H__A7B0C132_9D49_4529_9F9A_DBE5C1707E68__INCLUDED_)
#define AFX_UIENTRUSTCOMPUTER_H__A7B0C132_9D49_4529_9F9A_DBE5C1707E68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define AUTO_ATTACK_SETTINGS_PATH		"settings\\entrustcpu.ini"
#define AUTO_PICKUP_LIST_PATH			"UiSettings\\AutoPickupList.ini"
#define AUTO_ATTACK_DEFAULT_SETTINGS	"uisettings\\uicfg.ini"
#define AUTO_PICKUP_LIST_SECTION		"TypeList"
#define AUTO_PICKUP_LIST_NUM_KEYNAME	"ListNum"

#define DEFAULT_SKILL_STR_JIASHI		"JiaShiSkill"
#define DEFAULT_SKILL_STR_DAOSHI		"DaoShiSkill"
#define DEFAULT_SKILL_STR_YIREN			"YiRenSkill"
#define DEFAULT_SKILL_STR_XUANFENG		"XuanFengSkill"
#define DEFAULT_SKILL_STR_XINGTIAN		"XingTianSkill"
#define DEFAULT_SKILL_STR_ZHENREN		"ZhenRenSkill"
#define DEFAULT_SKILL_STR_TIANSHI		"TianShiSkill"
#define DEFAULT_SKILL_STR_YISHI			"YiShiSkill"
#define DEFAULT_SKILL_STR_SHOUSHI		"ShouShiSkill"

#include "../UiCommon.h"
#include "../../chatWindow/chatWnd.h"
#include "../../chatWindow/EntrustComputerDlg.h"
#include "Ui/UiCase/UiEntrustSkill.h"
#include "TLTree.h"
#include "TLMiniHorzScrollbar.h"
#include "TLEditbox.h"
#include <map>
#include <deque>

enum ButtonName
{
	ButtonAutoAttack = 0,
	ButtonAutoAttackHeigher,
	ButtonAutoAttackLower,
	ButtonAutoAttackBlast,
	ButtonAutoAttackRetour,
	ButtonAutoAttackRange,
	ButtonAutoUseItem,
	ButtonAutoUseHP30,
	ButtonAutoUseHP60,
	ButtonAutoUseMP30,
	ButtonAutoUseMP60,
	ButtonAutoPickup,
	ButtonAutoRepair,
	ButtonCount,
};

struct GameObjectInfo
{
	TLGameObject::GameObject	m_GameObject;
	CEGUI::String				m_Tooltip;
};

using namespace CEGUI;

class KUiEntrustComputer : public KUiWndSingleton<KUiEntrustComputer>
{
public:
	KUiEntrustComputer( const CEGUI::String& id_name );
	virtual ~KUiEntrustComputer();

	void	Init();
	void	toggle();

	void	RefreshUI( WORD buttonID );
	void    setAutoAttackButtonState( bool state );
	void    ClosePanel();

private:
	bool	btnClose_MouseClick( const EventArgs& e );

	//自动打怪checkBox事件
	bool	cbAutoAttack_CheckStateChanged( const EventArgs& e );
	bool	cbAutoAttack_level_higher_CheckStateChanged( const EventArgs& e );
	bool	cbAutoAttack_level_lower_CheckStateChanged( const EventArgs& e );
	bool	cbAutoAttack_go_home_CheckStateChanged( const EventArgs& e );
	bool	cbAutoAttack_only_normal_set_CheckStateChanged( const EventArgs& e );

	//自动拾取checkBox事件
	bool	cbAutoPickup_CheckStateChanged( const EventArgs& e );
	bool	cbAutoPickup_dear_first_CheckStateChanged( const EventArgs& e );
	bool	cbAutoPickup_blue_CheckStateChanged( const EventArgs& e );
	bool	cbAutoPickup_green_CheckStateChanged( const EventArgs& e );
	bool	cbAutoPickup_medicine_CheckStateChanged( const EventArgs& e );
	
	//自动打怪checkBox事件
	bool	cbAutoUseItem_CheckStateChanged( const EventArgs& e );
	bool	cbAutoUseItem_hp_60_CheckStateChanged( const EventArgs& e );
	bool	cbAutoUseItem_hp_40_CheckStateChanged( const EventArgs& e );
	bool	cbAutoUseItem_mp_20_CheckStateChanged( const EventArgs& e );
	
	bool	setAutoAttack	( bool flag );
	bool	setAutoPickup	( bool flag );
	bool	setAutoUseItem	( bool flag );

	void	initAutoAttackControls();
	void	initAutoPickupControls();
	void	initAutoUseItemControls();

	void	updateAutoPickup_medicine();

	bool	handleStateChanged( const EventArgs& e );

	Checkbox* getCheckBoxByID( WORD buttonID );
	WORD	  getIDByCheckBox( Checkbox* cb );

	bool	LoadBaseSettings(KIniFile & iniFile, char * section);
	bool	LoadDefaultSkillSettings(KIniFile & iniFile, char * section);
	void	GetMetierKeyName(char * buffer, int bufferLen, DWORD baseMetier);
	void	SetSkillObject(int index, int skillId);

private:
	//自动打怪相关控件
	Checkbox*	m_cbAutoAttack;
	Checkbox*	m_cbAutoAttack_level_higher;
	Checkbox*	m_cbAutoAttack_level_lower;
	Checkbox*	m_cbAutoAttack_go_home;
	Checkbox*	m_cbAutoAttack_only_normal_set;

	//自动拾取相关控件
	Checkbox*	m_cbAutoPickup;
/*	Checkbox*	m_cbAutoPickup_dear_first;
	Checkbox*	m_cbAutoPickup_blue;
	Checkbox*	m_cbAutoPickup_green;
	Checkbox*	m_cbAutoPickup_medicine;*/

	//自动喝药相关控件
	Checkbox*	m_cbAutoUseItem;
/*	Checkbox*	m_cbAutoUseItem_hp_60;
	Checkbox*	m_cbAutoUseItem_hp_40;
	Checkbox*	m_cbAutoUseItem_mp_20;*/

	bool		m_lockEvent;

public:
	void	LoadItemType		(void);
	void	InitSkillPanel		(void);
	void	RecalculateScroll	(void);

	bool	OnClearAllSettings	(const CEGUI::EventArgs & args);
	bool	OnSaveSettings		(const CEGUI::EventArgs & args);
	bool	OnBegin				(const CEGUI::EventArgs & args);
	bool	OnStop				(const CEGUI::EventArgs & args);

	bool	OnBaseSettings		(const CEGUI::EventArgs & args);
	bool	OnSkillSettings		(const CEGUI::EventArgs & args);
	void	RefreshSettings		();
	bool	SaveSettingsToFile	();
	bool	LoadBaseSettings	();
	bool	LoadDefaultSettings	();
	bool	LoadSkillSettings	();
	void	SetSkillPanel		();
	void	ResetAll			();
	void	ResetAllNotSave		();
	bool	CheckPtravailable	();

public:
	//自动拾取	
	bool	OnTypeMouseWheel	(const CEGUI::EventArgs & args);
	bool	OnSelListMouseWheel	(const CEGUI::EventArgs & args);

	bool	OnTypeScrollChanged	(const CEGUI::EventArgs & args);
	bool	OnSelScrollChanged	(const CEGUI::EventArgs & args);

	bool	OnSelectItemType	(const CEGUI::EventArgs & args);
	bool	OnClearSelected		(const CEGUI::EventArgs & args);
	bool	OnClearAllType		(const CEGUI::EventArgs & args);
	bool	OnAutoPickup		(const CEGUI::EventArgs & args);
	void	StartAutoPickup		();
	void	StopAutoPickup		();
	void	SavePickupSettings	();

	//自动打怪
	bool	OnAutoAttackGroup	(const CEGUI::EventArgs & args);
	void	StartAutoAttack		();
	void	StopAutoAttack		(int msgState);

	//自动喝药
	bool	OnAutoUseItem		(const CEGUI::EventArgs & args);
	void	StartAutoUseItem	();
	void	StopAutoUseItem		();

	//自动修理
	bool	OnAutoRepair		(const CEGUI::EventArgs & args);
	void	StartAutoRepair		();

	//技能设置界面
	bool	OnSelectSkillBnt	(const CEGUI::EventArgs & args);
	bool	OnUseSkillBnt		(const CEGUI::EventArgs & args);
	bool	OnTimeTextChanged	(const CEGUI::EventArgs & args);

	void	SetSelectedSkill	(TLGameObject * skillObj, int nIdx);
	void	StartAutoCast		();
	void	UpdateSkill			();
	void	CtrlAndA			();

	static void	Show	(void);

private:
	bool	m_bIsSettingsChanged;
	bool	m_bIsLoadBaseSettings;
	bool	m_bIsLoadSkillSettings;

	int		m_iAutoAttackState;
	int		m_iAutoAttackHigherState;
	int		m_iAutoAttackLowerState;

	StaticImage	*	m_pBaseSettingsPanel;
	StaticImage	*	m_pSkillSettingsPanel;

	bool				m_ButtonStates[ButtonCount];
	deque<unsigned int>	m_SelectedItemID;
	GameObjectInfo		m_BackupSkillObj[SkillCount];

	TLButton	*	m_BeginAutoAttack;
	TLButton	*	m_StopAutoAttack;

	//自动修理
	Checkbox	*	m_AutoRepair;

	//自动攻击
	Checkbox	*	m_AutoAttackBlast;
	Checkbox	*	m_AutoAttackRetour;
	Checkbox	*	m_AutoAttackRange;

	//自动喝药
	Checkbox	*	m_AutoUseItem_hp_60;
	Checkbox	*	m_AutoUseItem_hp_30;
	Checkbox	*	m_AutoUseItem_mp_60;
	Checkbox	*	m_AutoUseItem_mp_30;

	//自动拾取
	int						m_iTypeListNum;
	map<int, ItemClassInfo>	m_ClassInfoList;

	TLStaticImage	*	m_AutoPickupTypeListClipper;
	TLStaticImage	*	m_AutoPickupSelTypeListClipper;

	TLTree			*	m_AutoPickupTypeList;
	TLTree			*	m_AutoPickupSelTypeList;

	TLVertScrollbar	*	m_AutoPickupTypeListVScrollbar;
	TLVertScrollbar	*	m_AutoPickupSelTypeListVScrollbar;

	bool			*	m_bSelectState;
	TLTreeItem		**	m_TypeListItem;

	//技能设置
	deque<int>			m_BackupIntervalTime;
	bool				m_CanCastButtonState[SkillCount];
	Checkbox		*	m_IsCastCheckBox[SkillCount];
	TLGameObject	*	m_SkillObj[SkillCount];
	TLEditbox		*	m_IntervalTime[SkillCount - 1];
};

#endif // !defined(AFX_UIENTRUSTCOMPUTER_H__A7B0C132_9D49_4529_9F9A_DBE5C1707E68__INCLUDED_)
