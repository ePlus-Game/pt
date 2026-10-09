// UiEntrustComputer.cpp: implementation of the KUiEntrustComputer class.
//
//////////////////////////////////////////////////////////////////////

#include "UiEntrustComputer.h"
#include "CoreShell.h"
#include "AutoTrust.h"
#include "KTabFile.h"
#include "Ui/UiCase/UiAutoConnect.h"
#include "ui/KMessageCentre.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern iCoreShell*	g_pCoreShell;

template<> 
KUiEntrustComputer* KUiWndSingleton<KUiEntrustComputer>::ms_Singleton	= NULL;

KUiEntrustComputer::KUiEntrustComputer( const CEGUI::String& id_name )
: KUiWndSingleton<KUiEntrustComputer>( id_name )
{
	//自动打怪相关控件
	m_cbAutoAttack = NULL;
	m_cbAutoAttack_level_higher = NULL;
	m_cbAutoAttack_level_lower = NULL;
	m_cbAutoAttack_go_home = NULL;
	m_cbAutoAttack_only_normal_set = NULL;
	
	//自动拾取相关控件
	m_cbAutoPickup = NULL;
/*	m_cbAutoPickup_dear_first = NULL;
	m_cbAutoPickup_blue = NULL;
	m_cbAutoPickup_green = NULL;
	m_cbAutoPickup_medicine = NULL;*/
	
	//自动喝药相关控件
	m_cbAutoUseItem = NULL;
/*	m_cbAutoUseItem_hp_60 = NULL;
	m_cbAutoUseItem_hp_40 = NULL;
	m_cbAutoUseItem_mp_20 = NULL;*/

	m_lockEvent = false;

	m_pBaseSettingsPanel = NULL;
	m_pSkillSettingsPanel = NULL;

	m_pBaseSettingsPanel = NULL;
	m_pSkillSettingsPanel = NULL;
	
	m_AutoAttackBlast = NULL;
	m_AutoAttackRetour = NULL;
	m_AutoAttackRange = NULL;
	
	m_AutoUseItem_hp_60 = NULL;
	m_AutoUseItem_hp_30 = NULL;
	m_AutoUseItem_mp_60 = NULL;
	m_AutoUseItem_mp_30 = NULL;
	
	m_AutoPickupTypeListClipper = NULL;
	m_AutoPickupTypeList = NULL;
	m_AutoPickupTypeListVScrollbar = NULL;

	m_AutoPickupSelTypeListClipper = NULL;
	m_AutoPickupSelTypeList = NULL;
	m_AutoPickupSelTypeListVScrollbar = NULL;

	m_iAutoAttackState = 0;
	m_iAutoAttackHigherState = 0;
	m_iAutoAttackLowerState = 0;

	m_iTypeListNum = 0;
	m_TypeListItem = NULL;
	m_bSelectState = NULL;
	m_bIsLoadBaseSettings = false;
	m_bIsLoadSkillSettings = false;

	for (int i = 0; i < SkillCount; i++)
	{
		m_SkillObj[i] = NULL;
		m_IsCastCheckBox[i] = NULL;
		if (i > 0)
		{
			m_IntervalTime[i - 1] = NULL;
		}
	}
	
	m_BeginAutoAttack = NULL;
	m_StopAutoAttack = NULL;

	m_bIsSettingsChanged = false;
}

KUiEntrustComputer::~KUiEntrustComputer()
{
	if (m_bSelectState != NULL)
	{
		delete[] m_bSelectState;
	}

	if (m_TypeListItem != NULL)
	{
		for (int i = 0; i < m_iTypeListNum; i++)
		{
			delete m_TypeListItem[i];
			m_TypeListItem[i] = NULL;
		}
		delete[] m_TypeListItem;
	}
}

void KUiEntrustComputer::toggle()
{
	if( NULL != m_pThisWnd)
	{
		if( m_pThisWnd->isVisible() )
		{
			m_bIsSettingsChanged = false;
			const EventArgs args;
			btnClose_MouseClick(args);
		}
		else
		{
			Show();
		}
	}
}

void KUiEntrustComputer::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild( "TaharezLook/EntrustComputer/Close" )->subscribeEvent(
			PushButton::EventClicked, 
			Event::Subscriber( &KUiEntrustComputer::btnClose_MouseClick, ms_Singleton ) );

		m_pThisWnd->getChild("TaharezLook/EntrustComputer/BaseSettings")->subscribeEvent(
			PushButton::EventClicked,
			Event::Subscriber(&KUiEntrustComputer::OnBaseSettings, ms_Singleton));

		m_pThisWnd->getChild("TaharezLook/EntrustComputer/SkillSettings")->subscribeEvent(
			PushButton::EventClicked,
			Event::Subscriber(&KUiEntrustComputer::OnSkillSettings, ms_Singleton));

		m_pThisWnd->getChild("TaharezLook/EntrustComputer/SaveSettings")->subscribeEvent(
			PushButton::EventClicked,
			Event::Subscriber(&KUiEntrustComputer::OnSaveSettings, this));

		m_pThisWnd->getChild("TaharezLook/EntrustComputer/ClearAllSettings")->subscribeEvent(
			PushButton::EventClicked,
			Event::Subscriber(&KUiEntrustComputer::OnClearAllSettings, this));

		m_BeginAutoAttack	= static_cast<TLButton *>(m_pThisWnd->getChild( "TaharezLook/EntrustComputer/btnOK" ));
		m_StopAutoAttack	= static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/EntrustComputer/StopAutoAttack"));

		if (NULL == m_BeginAutoAttack
			|| NULL == m_StopAutoAttack)
		{
			return;
		}

		m_BeginAutoAttack->subscribeEvent(PushButton::EventClicked,	Event::Subscriber(&KUiEntrustComputer::OnBegin, this));
		m_StopAutoAttack->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiEntrustComputer::OnStop, this));

		m_pBaseSettingsPanel = static_cast<StaticImage *>(m_pThisWnd->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel"));
		m_pSkillSettingsPanel = static_cast<StaticImage *>(m_pThisWnd->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel"));

		m_lockEvent = true;
		initAutoAttackControls();
		initAutoPickupControls();
		initAutoUseItemControls();
		m_lockEvent = false;

		InitSkillPanel();

		m_pSkillSettingsPanel->hide();

		m_AutoRepair = static_cast<Checkbox *>(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoRepair"));

		if (NULL == m_AutoRepair)
		{
			return;
		}

		m_AutoRepair->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoRepair, this));
		m_AutoRepair->setSelected(false);
		for (int i = 0; i < ButtonCount; i++)
		{
			m_ButtonStates[i] = false;
		}
	}
}

void KUiEntrustComputer::initAutoAttackControls()
{
	//自动打怪相关控件
	m_cbAutoAttack					=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttack" ) );
	m_cbAutoAttack_level_higher		=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttack-level-higher" ) );
	m_cbAutoAttack_level_lower		=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttack-level-lower" ) );
	m_cbAutoAttack_go_home			=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttack-go-home" ) );
	m_cbAutoAttack_only_normal_set	=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttack-only-normal-set" ) );
	m_AutoAttackBlast				=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttackBlast" ) );
	m_AutoAttackRetour				=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoAttackRetour" ) );
	m_AutoAttackRange				=	static_cast< Checkbox* >	( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAtuoAttackRange" ) );

	if (NULL == m_cbAutoAttack
		|| NULL == m_cbAutoAttack_level_higher
		|| NULL == m_cbAutoAttack_level_lower
		|| NULL == m_cbAutoAttack_go_home
		|| NULL == m_cbAutoAttack_only_normal_set
		|| NULL == m_AutoAttackBlast
		|| NULL == m_AutoAttackRetour
		|| NULL == m_AutoAttackRange)
	{
		return;
	}

	m_cbAutoAttack->setSelected( false );
	m_AutoAttackRetour->setSelected(true);
	m_AutoAttackRange->setSelected(false);

	setAutoAttack( false );
	
	//自动打怪相关控件事件注册
	m_cbAutoAttack->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::OnAutoAttackGroup, ms_Singleton ) );

	m_cbAutoAttack_level_higher->subscribeEvent( 
		Checkbox::EventCheckStateChanged,
		Event::Subscriber( &KUiEntrustComputer::OnAutoAttackGroup, ms_Singleton ) );

	m_cbAutoAttack_level_lower->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::OnAutoAttackGroup, ms_Singleton ) );

	m_cbAutoAttack_go_home->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::OnAutoAttackGroup, ms_Singleton ) );

	m_cbAutoAttack_only_normal_set->subscribeEvent( 
		Checkbox::EventCheckStateChanged,
		Event::Subscriber( &KUiEntrustComputer::OnAutoAttackGroup, ms_Singleton ) );

	m_AutoAttackBlast->subscribeEvent(Checkbox::EventCheckStateChanged, Event::Subscriber(&KUiEntrustComputer::OnAutoAttackGroup, this));
	m_AutoAttackRetour->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoAttackGroup, this));
	m_AutoAttackRange->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoAttackGroup, this));
}

void KUiEntrustComputer::initAutoPickupControls()
{
	//自动拾取相关控件
/*	m_cbAutoPickup = static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoPickup" ) );
	m_cbAutoPickup_dear_first	=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoPickup-dear_first" ) );
	m_cbAutoPickup_blue			=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoPickup-blue" ) );
	m_cbAutoPickup_green		=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoPickup-green" ) );
	m_cbAutoPickup_medicine		=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoPickup-medicine" ) );
	
	m_cbAutoPickup->setSelected( false );
	setAutoPickup( false );

	//自动拾取相关控件
	m_cbAutoPickup->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoPickup_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoPickup_dear_first->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoPickup_dear_first_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoPickup_blue->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoPickup_blue_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoPickup_green->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoPickup_green_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoPickup_medicine->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoPickup_medicine_CheckStateChanged, ms_Singleton ) );

	updateAutoPickup_medicine();*/

	m_cbAutoPickup = static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoPickup"));
	if (NULL == m_cbAutoPickup)
	{
		return;
	}

	m_cbAutoPickup->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoPickup, this));
	m_cbAutoPickup->setSelected(false);

	m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/SelectItemType")->subscribeEvent(
		PushButton::EventClicked,
		Event::Subscriber(&KUiEntrustComputer::OnSelectItemType, this));
	
	m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/ClearSelected")->subscribeEvent(
		PushButton::EventClicked,
		Event::Subscriber(&KUiEntrustComputer::OnClearSelected, this));
	
	m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/OnClearAllType")->subscribeEvent(
		PushButton::EventClicked,
		Event::Subscriber(&KUiEntrustComputer::OnClearAllType, this));

	m_AutoPickupTypeListClipper		= static_cast<TLStaticImage *>	(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/TypeClipper"));
	m_AutoPickupTypeList			= static_cast<TLTree *>			(m_AutoPickupTypeListClipper->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/TypeClipper/cbAutoPickupItemList"));
	m_AutoPickupTypeListVScrollbar	= static_cast<TLVertScrollbar *>(m_AutoPickupTypeListClipper->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/TypeClipper/cbAutoPickupItemList/VScorllbar"));

	if (NULL == m_AutoPickupTypeListClipper
		|| NULL == m_AutoPickupTypeList
		|| NULL == m_AutoPickupTypeListVScrollbar)
	{
		return;
	}

	m_AutoPickupTypeList->subscribeEvent(Tree::EventMouseWheel, Event::Subscriber(&KUiEntrustComputer::OnTypeMouseWheel, this));
	m_AutoPickupTypeListVScrollbar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiEntrustComputer::OnTypeScrollChanged, this));

	m_AutoPickupSelTypeListClipper		= static_cast<TLStaticImage *>	(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/SelListClipper"));
	m_AutoPickupSelTypeList				= static_cast<TLTree *>			(m_AutoPickupSelTypeListClipper->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/SelListClipper/cbAtuoPickupSelItemList"));
	m_AutoPickupSelTypeListVScrollbar	= static_cast<TLVertScrollbar *>(m_AutoPickupSelTypeListClipper->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/SelListClipper/cbAtuoPickupSelItemList/VScorllbar"));

	if (NULL == m_AutoPickupSelTypeListClipper
		|| NULL == m_AutoPickupSelTypeList
		|| NULL == m_AutoPickupSelTypeListVScrollbar)
	{
		return;
	}

	m_AutoPickupSelTypeList->subscribeEvent(Tree::EventMouseWheel, Event::Subscriber(&KUiEntrustComputer::OnSelListMouseWheel, this));
	m_AutoPickupSelTypeListVScrollbar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiEntrustComputer::OnSelScrollChanged, this));

	KTabFile listFile;
	if (!listFile.Load(AUTO_PICKUP_LIST_PATH))
	{
		return;
	}
	else
	{
		m_AutoPickupTypeList->setSortingEnabled(false);
		m_AutoPickupSelTypeList->setSortingEnabled(false);
		
		m_iTypeListNum = listFile.GetHeight() - 1;
		
		m_TypeListItem = new TLTreeItem * [m_iTypeListNum];
		m_bSelectState = new bool[m_iTypeListNum];

		char TypeName[COMMON_CLIENT_MSG_LEN_32];
		for (int i = 0; i < m_iTypeListNum; i++)
		{
			memset(TypeName, 0, sizeof(TypeName));
			int TypeId = -1;

			listFile.GetInteger(i + 2, 1, -1, &TypeId);
			listFile.GetString(i + 2, 2, "", TypeName, COMMON_CLIENT_MSG_LEN_32 - 1);

			m_TypeListItem[i] = new TLTreeItem("");
			m_TypeListItem[i]->setText(AnsiToUtf8(TypeName));
			m_TypeListItem[i]->setID(TypeId);
			m_TypeListItem[i]->setAutoDeleted(false);
			m_AutoPickupTypeList->addItem(m_TypeListItem[i]);
			m_bSelectState[i] = false;

			ItemClassInfo classInfo;
			int tag = 0;
			bool isGroup = false;
			listFile.GetInteger(i + 2, 3, 0, &tag);
			if (tag == 0)
			{
				isGroup = false;
			}
			else
			{
				isGroup = true;
			}

			bool isColor = false;
			listFile.GetInteger(i + 2, 4, 0, &tag);
			if (tag == 0)
			{
				isColor = false;
			}
			else
			{
				isColor = true;
			}

			memset(classInfo.m_className, 0, sizeof(classInfo.m_className));
			listFile.GetString(i + 2, 5, "", classInfo.m_className, sizeof(classInfo.m_className));
			if (strcmp(classInfo.m_className, "") != 0)
			{
				classInfo.SetItemClassInfo(isGroup, isColor);
				m_ClassInfoList.insert(pair<int, ItemClassInfo>(TypeId, classInfo));
			}
		}

		int totalItemHeight = m_AutoPickupTypeList->getTreeTotalItemsHeigh();
		int clipperHeight = m_AutoPickupTypeListClipper->getHeight(Absolute);
		m_AutoPickupSelTypeListVScrollbar->hide();

		if (totalItemHeight <= clipperHeight)
		{
			m_AutoPickupTypeListVScrollbar->hide();
		}
		else
		{
			m_AutoPickupTypeListVScrollbar->show();
			int itemCount = m_AutoPickupTypeList->getItemCount();
			float stepSize = ((float)totalItemHeight / itemCount) / (totalItemHeight - clipperHeight);
			m_AutoPickupTypeListVScrollbar->setStepSize(stepSize);
		}
	}
}

void KUiEntrustComputer::initAutoUseItemControls()
{
	//自动喝药相关控件
/*	m_cbAutoUseItem = static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem" ) );
	m_cbAutoUseItem_hp_60	=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-hp-60" ) );
	m_cbAutoUseItem_hp_40	=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-hp-40" ) );
	m_cbAutoUseItem_mp_20	=	static_cast< Checkbox* >( m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-mp-20" ) );

	m_cbAutoUseItem->setSelected( false );
	setAutoUseItem( false );

	//自动喝药相关控件
	m_cbAutoUseItem->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoUseItem_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoUseItem_hp_60->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoUseItem_hp_60_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoUseItem_hp_40->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoUseItem_hp_40_CheckStateChanged, ms_Singleton ) );
	
	m_cbAutoUseItem_mp_20->subscribeEvent( 
		Checkbox::EventCheckStateChanged, 
		Event::Subscriber( &KUiEntrustComputer::cbAutoUseItem_mp_20_CheckStateChanged, ms_Singleton ) );*/

	m_cbAutoUseItem		= static_cast<Checkbox*>	(m_pBaseSettingsPanel->getChild( "TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem"));
	m_AutoUseItem_hp_30 = static_cast<Checkbox *>	(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-hp-30"));
	m_AutoUseItem_hp_60 = static_cast<Checkbox *>	(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-hp-60"));
	m_AutoUseItem_mp_30 = static_cast<Checkbox *>	(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-mp-30"));
	m_AutoUseItem_mp_60 = static_cast<Checkbox *>	(m_pBaseSettingsPanel->getChild("TaharezLook/EntrustComputer/BaseSettingsPanel/cbAutoUseItem-mp-60"));

	m_cbAutoUseItem->setSelected(false);
	m_AutoUseItem_hp_30->setSelected(true);
	m_AutoUseItem_hp_60->setSelected(false);
	m_AutoUseItem_mp_30->setSelected(true);
	m_AutoUseItem_mp_60->setSelected(false);

	setAutoUseItem(false);

	m_cbAutoUseItem->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoUseItem, this));
	m_AutoUseItem_hp_30->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoUseItem, this));
	m_AutoUseItem_hp_60->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoUseItem, this));
	m_AutoUseItem_mp_30->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoUseItem, this));
	m_AutoUseItem_mp_60->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnAutoUseItem, this));
}

bool KUiEntrustComputer::btnClose_MouseClick( const EventArgs& e )
{
	Hide();

	if (!m_bIsSettingsChanged)
	{
		return true;
	}

	if (!CheckPtravailable())
	{
		return false;
	}

	if (m_AutoPickupTypeList == NULL || m_AutoPickupSelTypeList == NULL)
	{
		return false;
	}

	m_cbAutoAttack->setSelected(m_ButtonStates[ButtonAutoAttack]);
	setAutoAttack(m_ButtonStates[ButtonAutoAttack]);
	m_cbAutoAttack_level_higher->setSelected(m_ButtonStates[ButtonAutoAttackHeigher]);
	m_cbAutoAttack_level_lower->setSelected(m_ButtonStates[ButtonAutoAttackLower]);
	m_AutoAttackBlast->setSelected(m_ButtonStates[ButtonAutoAttackBlast]);
	m_AutoAttackRetour->setSelected(m_ButtonStates[ButtonAutoAttackRetour]);
	m_AutoAttackRange->setSelected(m_ButtonStates[ButtonAutoAttackRange]);
	m_cbAutoUseItem->setSelected(m_ButtonStates[ButtonAutoUseItem]);
	setAutoUseItem(m_ButtonStates[ButtonAutoUseItem]);
	m_AutoUseItem_hp_30->setSelected(m_ButtonStates[ButtonAutoUseHP30]);
	m_AutoUseItem_hp_60->setSelected(m_ButtonStates[ButtonAutoUseHP60]);
	m_AutoUseItem_mp_30->setSelected(m_ButtonStates[ButtonAutoUseMP30]);
	m_AutoUseItem_mp_60->setSelected(m_ButtonStates[ButtonAutoUseMP60]);
	m_AutoRepair->setSelected(m_ButtonStates[ButtonAutoRepair]);
	m_cbAutoPickup->setSelected(m_ButtonStates[ButtonAutoPickup]);

	m_AutoPickupTypeList->removeAllItem();
	m_AutoPickupSelTypeList->removeAllItem();

	bool isSelected = false;
	for (int i = 0; i < m_iTypeListNum; i++)
	{
		isSelected =false;
		for (int j = 0; j < m_SelectedItemID.size(); j++)
		{
			if (!m_SelectedItemID.empty())
			{
				if (m_TypeListItem[i]->getID() == m_SelectedItemID.front())
				{
					m_AutoPickupSelTypeList->addItem(m_TypeListItem[i]);
					m_SelectedItemID.pop_front();
					isSelected = true;
					break;
				}
			}
		}
		if (isSelected)
		{
			continue;
		}
		m_AutoPickupTypeList->addItem(m_TypeListItem[i]);
	}

	for (i = 0; i < SkillCount; i++)
	{
		if (m_SkillObj[i] == NULL)
		{
			continue;
		}

		m_SkillObj[i]->clear();
		m_SkillObj[i]->setObject(m_BackupSkillObj[i].m_GameObject);
		m_SkillObj[i]->setTooltipText(m_BackupSkillObj[i].m_Tooltip);

		if (m_IsCastCheckBox[i] == NULL)
		{
			continue;
		}
		
		m_IsCastCheckBox[i]->setSelected(m_CanCastButtonState[i]);

		if (i > 0)
		{
			if (m_IntervalTime[i - 1] == NULL)
			{
				continue;
			}
			m_IntervalTime[i - 1]->resetText(iToString(m_BackupIntervalTime[i - 1]));
		}
	}

	m_bIsSettingsChanged = false;
	return true;
}

bool KUiEntrustComputer::setAutoAttack( bool flag )
{
	if (!CheckPtravailable())
	{
		return false;
	}

	m_cbAutoAttack_level_higher->setSelected( false );
	m_cbAutoAttack_level_lower->setSelected( false );
	m_cbAutoAttack_go_home->setSelected( false );
	m_cbAutoAttack_only_normal_set->setSelected( false );
	m_AutoAttackBlast->setSelected(false);
	m_AutoAttackRange->setSelected(flag);
	m_AutoAttackRetour->setSelected(false);

/*	m_ButtonStates[ButtonAutoAttackHeigher] = false;
	m_ButtonStates[ButtonAutoAttackLower] = false;
	m_ButtonStates[ButtonAutoAttackBlast] = false;
	m_ButtonStates[ButtonAutoAttackRange] = flag;
	m_ButtonStates[ButtonAutoAttackRetour] = false;*/

	m_cbAutoAttack_level_higher->setEnabled( flag );
	m_cbAutoAttack_level_lower->setEnabled( flag );
	m_cbAutoAttack_go_home->setEnabled( flag );
	m_cbAutoAttack_only_normal_set->setEnabled( flag );
	m_AutoAttackBlast->setEnabled(flag);
	m_AutoAttackRetour->setEnabled(flag);
	m_AutoAttackRange->setEnabled(flag);
	
	return true;
}

bool KUiEntrustComputer::setAutoPickup( bool flag )
{
/*	m_cbAutoPickup_dear_first->setSelected( false );
	m_cbAutoPickup_blue->setSelected( false );
	m_cbAutoPickup_green->setSelected( false );
	m_cbAutoPickup_medicine->setSelected( false );

	m_cbAutoPickup_dear_first->setEnabled( flag );
	m_cbAutoPickup_blue->setEnabled( flag );
	m_cbAutoPickup_green->setEnabled( flag );
	//m_cbAutoPickup_medicine->setEnabled( flag );

	updateAutoPickup_medicine();*/
	return true;
}

bool KUiEntrustComputer::setAutoUseItem( bool flag )
{
/*	m_cbAutoUseItem_hp_60->setSelected( false );
	m_cbAutoUseItem_hp_40->setSelected( false );
	m_cbAutoUseItem_mp_20->setSelected( false );

	m_cbAutoUseItem_hp_60->setEnabled( flag );
	m_cbAutoUseItem_hp_40->setEnabled( flag );
	m_cbAutoUseItem_mp_20->setEnabled( flag );*/

	if (!CheckPtravailable())
	{
		return false;
	}

	m_AutoUseItem_hp_30->setSelected(flag);
	m_AutoUseItem_hp_60->setSelected(false);
	m_AutoUseItem_mp_30->setSelected(flag);
	m_AutoUseItem_mp_60->setSelected(false);

/*	m_ButtonStates[ButtonAutoUseHP30] = flag;
	m_ButtonStates[ButtonAutoUseHP60] = false;
	m_ButtonStates[ButtonAutoUseMP30] = flag;
	m_ButtonStates[ButtonAutoUseMP60] = false;*/

	m_AutoUseItem_hp_30->setEnabled(flag);
	m_AutoUseItem_hp_60->setEnabled(flag);
	m_AutoUseItem_mp_30->setEnabled(flag);
	m_AutoUseItem_mp_60->setEnabled(flag);

	return true;
}

bool KUiEntrustComputer::cbAutoAttack_CheckStateChanged( const EventArgs& e )
{
	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );
	
	if ( handleStateChanged( e ) )
	{
		m_lockEvent = true;
		setAutoAttack( curCheckBox->isSelected() );
		m_lockEvent = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool KUiEntrustComputer::cbAutoPickup_CheckStateChanged( const EventArgs& e )
{
	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );
	
	if ( handleStateChanged( e ) )
	{
		m_lockEvent = true;
		setAutoPickup( curCheckBox->isSelected() );
		m_lockEvent = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool KUiEntrustComputer::cbAutoUseItem_CheckStateChanged( const EventArgs& e )
{
/*	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );
	if ( handleStateChanged( e ) )
	{
		m_lockEvent = true;
		setAutoUseItem( curCheckBox->isSelected() );
		m_lockEvent = false;
		return true;
	}
	else
	{
		return false;
	}*/

	if (g_pCoreShell == NULL)
	{
		return false;
	}

	if (m_cbAutoUseItem != NULL)
	{
		if (m_cbAutoUseItem->isSelected())
		{
			g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_SWITCH, 0, true);
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_SWITCH, 0, false);
		}
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
//自动打怪相关
//////////////////////////////////////////////////////////////////////////
bool KUiEntrustComputer::cbAutoAttack_level_higher_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

bool KUiEntrustComputer::cbAutoAttack_level_lower_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

bool KUiEntrustComputer::cbAutoAttack_go_home_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

bool KUiEntrustComputer::cbAutoAttack_only_normal_set_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

//////////////////////////////////////////////////////////////////////////
//自动拾取相关
//////////////////////////////////////////////////////////////////////////
bool KUiEntrustComputer::cbAutoPickup_dear_first_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

bool KUiEntrustComputer::cbAutoPickup_blue_CheckStateChanged( const EventArgs& e )
{
/*	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );
	
	if ( handleStateChanged( e ) )
	{
		if ( m_cbAutoPickup_green->isSelected() && m_cbAutoPickup_blue->isSelected() )
		{
			m_lockEvent = true;
			m_cbAutoPickup_green->setSelected( false );
			m_lockEvent = false;
		}
		m_lockEvent = true;
		updateAutoPickup_medicine();
		m_lockEvent = false;

		return true;
	}
	else
	{
		return false;
	}*/
	return true;
}

bool KUiEntrustComputer::cbAutoPickup_green_CheckStateChanged( const EventArgs& e )
{
/*	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );

	if ( handleStateChanged( e ) )
	{
		if ( m_cbAutoPickup_green->isSelected() && m_cbAutoPickup_blue->isSelected() )
		{
			m_lockEvent = true;
			m_cbAutoPickup_blue->setSelected( false );
			m_lockEvent = false;
		}
		m_lockEvent = true;
		updateAutoPickup_medicine();
		m_lockEvent = false;
		
		return true;
	}
	else
	{
		return false;
	}*/
	return true;
}

bool KUiEntrustComputer::cbAutoPickup_medicine_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

//////////////////////////////////////////////////////////////////////////
//自动喝药相关
//////////////////////////////////////////////////////////////////////////
bool KUiEntrustComputer::cbAutoUseItem_hp_60_CheckStateChanged( const EventArgs& e )
{
/*	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );

	if ( handleStateChanged( e ) )
	{
		if ( m_cbAutoUseItem_hp_60->isSelected() && m_cbAutoUseItem_hp_40->isSelected() )
		{
			m_lockEvent = true;
			m_cbAutoUseItem_hp_40->setSelected( false );
			m_lockEvent = false;
		}
		
		return true;
	}
	else
	{
		return false;
	}*/
	return true;
}

bool KUiEntrustComputer::cbAutoUseItem_hp_40_CheckStateChanged( const EventArgs& e )
{
/*	const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
	Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );

	if ( handleStateChanged( e ) )
	{
		if ( m_cbAutoUseItem_hp_60->isSelected() && m_cbAutoUseItem_hp_40->isSelected() )
		{
			m_lockEvent = true;
			m_cbAutoUseItem_hp_60->setSelected( false );
			m_lockEvent = false;
		}
		
		return true;
	}
	else
	{
		return false;
	}*/
	return true;
}

bool KUiEntrustComputer::cbAutoUseItem_mp_20_CheckStateChanged( const EventArgs& e )
{
	return handleStateChanged( e );
}

void KUiEntrustComputer::updateAutoPickup_medicine()
{
/*	if ( m_cbAutoPickup_green->isSelected() || m_cbAutoPickup_blue->isSelected() )
	{
		m_cbAutoPickup_medicine->setEnabled( true );
	}
	else
	{
		m_cbAutoPickup_medicine->setEnabled( false );
		m_cbAutoPickup_medicine->setSelected( false );
	}*/
}

// void KUiEntrustComputer::syncExtChatWnd( CheckedButtonGroup& buttonGroup, WORD buttonID )
// {	
// 	WPARAM wParam = MAKELONG( buttonID, BN_CLICKED );
// 	
// 	buttonGroup.CheckedButtonProcessLButtonDown( wParam, 0 );
// }

void KUiEntrustComputer::RefreshUI( WORD buttonID )
{
	m_lockEvent = true;

	Checkbox* curCheckBox = getCheckBoxByID( buttonID );

	if ( NULL != curCheckBox )
	{
		if ( !curCheckBox->isDisabled() )
		{
			curCheckBox->setSelected( !curCheckBox->isSelected() );
		}
	}

	if ( curCheckBox == m_cbAutoAttack )
	{
		setAutoAttack( curCheckBox->isSelected() );
	}

	if ( curCheckBox == m_cbAutoPickup )
	{
		setAutoPickup( curCheckBox->isSelected() );
	}

	if ( curCheckBox == m_cbAutoUseItem )
	{
		setAutoUseItem( curCheckBox->isSelected() );
	}

/*	if ( curCheckBox == m_cbAutoPickup_green )
	{
		if ( m_cbAutoPickup_green->isSelected() && m_cbAutoPickup_blue->isSelected() )
		{
			m_cbAutoPickup_blue->setSelected( false );
		}
		updateAutoPickup_medicine();
	}

	if ( curCheckBox == m_cbAutoPickup_blue )
	{
		if ( m_cbAutoPickup_green->isSelected() && m_cbAutoPickup_blue->isSelected() )
		{
			m_cbAutoPickup_green->setSelected( false );
		}
		updateAutoPickup_medicine();
	}

	if ( curCheckBox == m_cbAutoUseItem_hp_60 )
	{
		if ( m_cbAutoUseItem_hp_60->isSelected() && m_cbAutoUseItem_hp_40->isSelected() )
		{
			m_cbAutoUseItem_hp_40->setSelected( false );
		}
	}

	if ( curCheckBox == m_cbAutoUseItem_hp_40 )
	{
		if ( m_cbAutoUseItem_hp_60->isSelected() && m_cbAutoUseItem_hp_40->isSelected() )
		{
			m_cbAutoUseItem_hp_60->setSelected( false );
		}
	}*/

	m_lockEvent = false;
}

bool KUiEntrustComputer::handleStateChanged( const EventArgs& e )
{
	if ( !m_lockEvent )
	{
		const WindowEventArgs args = static_cast< const WindowEventArgs& >( e );
		Checkbox* curCheckBox = static_cast< Checkbox* >( args.window );
		
		DWORD buttonID = getIDByCheckBox( curCheckBox );

		WPARAM wParam = MAKELONG( buttonID, BN_CLICKED );
		EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgProcessCommand( wParam , 0 );
		
		return true;
	}
	else
	{
		return false;
	}	
}

Checkbox* KUiEntrustComputer::getCheckBoxByID( WORD buttonID )
{
	Checkbox* curCheckBox = NULL;

	switch ( buttonID )
	{
		//自动打怪相关	
	case _ENTRUST_AUTOATTACK_MAIN_BUTTON_ID:
		curCheckBox = m_cbAutoAttack;
		break;
	case _ENTRUST_AUTOATTACK_ENEMY_LEVEL_HIGHER_BUTTON_ID:
		curCheckBox = m_cbAutoAttack_level_higher;
		break;
	case _ENTRUST_AUTOATTACK_ENEMY_LEVEL_LOWER_BUTTON_ID:
		curCheckBox = m_cbAutoAttack_level_lower;
		break;
	case _ENTRUST_AUTOATTACK_GO_HOME_BUTTON_ID:
		curCheckBox = m_cbAutoAttack_go_home;
		break;
	case _ENTRUST_AUTOATTACK_ONLY_NORMAL_SET_BUTTON_ID:
		curCheckBox = m_cbAutoAttack_only_normal_set;
		break;
		
		//自动拾取相关
	case _ENTRUST_AUTOPICKUP_MAIN_BUTTON_ID:
		curCheckBox = m_cbAutoPickup;
		break;
/*	case _ENTRUST_AUTOPICKUP_DEAR_FIRST_BUTTON_ID:
		curCheckBox = m_cbAutoPickup_dear_first;
		break;
	case _ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_ID:
		curCheckBox = m_cbAutoPickup_blue;
		break;
	case _ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_ID:
		curCheckBox = m_cbAutoPickup_green;
		break;
	case _ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_ID:
		curCheckBox = m_cbAutoPickup_medicine;
		break;*/
		
		//自动喝药
	case _ENTRUST_AUTOUSEITEM_MAIN_BUTTON_ID:
		curCheckBox = m_cbAutoUseItem;
		break;
/*	case _ENTRUST_AUTOUSEITEM_HP_60_BUTTON_ID:
		curCheckBox = m_cbAutoUseItem_hp_60;
		break;
	case _ENTRUST_AUTOUSEITEM_HP_40_BUTTON_ID:
		curCheckBox = m_cbAutoUseItem_hp_40;
		break;
	case _ENTRUST_AUTOUSEITEM_MP_20_BUTTON_ID:
		curCheckBox = m_cbAutoUseItem_mp_20;
		break;*/
	}

	return curCheckBox;
}

WORD KUiEntrustComputer::getIDByCheckBox( Checkbox* cb )
{
	WORD cbID = 0;
	//自动打怪	
	if ( cb == m_cbAutoAttack )
	{
		cbID = _ENTRUST_AUTOATTACK_MAIN_BUTTON_ID;
	}
	else if ( cb == m_cbAutoAttack_level_higher )
	{
		cbID = _ENTRUST_AUTOATTACK_ENEMY_LEVEL_HIGHER_BUTTON_ID;
	}
	else if ( cb == m_cbAutoAttack_level_lower )
	{
		cbID = _ENTRUST_AUTOATTACK_ENEMY_LEVEL_LOWER_BUTTON_ID;
	}
	else if ( cb == m_cbAutoAttack_go_home )
	{
		cbID = _ENTRUST_AUTOATTACK_GO_HOME_BUTTON_ID;
	}
	else if ( cb == m_cbAutoAttack_only_normal_set )
	{
		cbID = _ENTRUST_AUTOATTACK_ONLY_NORMAL_SET_BUTTON_ID;
	}
	//自动拾取
	else if ( cb == m_cbAutoPickup )
	{
		cbID = _ENTRUST_AUTOPICKUP_MAIN_BUTTON_ID;
	}
/*
	else if ( cb == m_cbAutoPickup_dear_first )
	{
		cbID = _ENTRUST_AUTOPICKUP_DEAR_FIRST_BUTTON_ID;
	}
	else if ( cb == m_cbAutoPickup_blue )
	{
		cbID = _ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_ID;
	}
	else if ( cb == m_cbAutoPickup_green )
	{
		cbID = _ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_ID;
	}
	else if ( cb == m_cbAutoPickup_medicine )
	{
		cbID = _ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_ID;
	}*/

	//自动喝药
/*
	else if ( cb == m_cbAutoUseItem )
	{
		cbID = _ENTRUST_AUTOUSEITEM_MAIN_BUTTON_ID;
	}
	else if ( cb == m_cbAutoUseItem_hp_60 )
	{
		cbID = _ENTRUST_AUTOUSEITEM_HP_60_BUTTON_ID;
	}
	else if ( cb == m_cbAutoUseItem_hp_40 )
	{
		cbID = _ENTRUST_AUTOUSEITEM_HP_40_BUTTON_ID;
	}
	else if ( cb == m_cbAutoUseItem_mp_20 )
	{
		cbID = _ENTRUST_AUTOUSEITEM_MP_20_BUTTON_ID;
	}*/


	return cbID;
}

void KUiEntrustComputer::setAutoAttackButtonState( bool state )
{
	m_lockEvent = true;
	
	m_cbAutoAttack->setSelected( state );
	setAutoAttack( state );

	m_lockEvent = false;
}

bool KUiEntrustComputer::OnBaseSettings(const CEGUI::EventArgs & args)
{
	if (m_pBaseSettingsPanel == NULL || m_pSkillSettingsPanel == NULL)
	{
		return false;
	}

	m_pBaseSettingsPanel->show();
	m_pSkillSettingsPanel->hide();
	return true;
}

bool KUiEntrustComputer::OnSkillSettings(const CEGUI::EventArgs & args)
{
	if (m_pBaseSettingsPanel == NULL || m_pSkillSettingsPanel == NULL)
	{
		return false;
	}

	m_pSkillSettingsPanel->show();
	m_pBaseSettingsPanel->hide();
	return true;
}

void KUiEntrustComputer::InitSkillPanel()
{
	m_IsCastCheckBox[AttackSkill]	= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseAttack"));
	m_IsCastCheckBox[Skill0]		= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseSkill0"));
	m_IsCastCheckBox[Skill1]		= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseSkill1"));
	m_IsCastCheckBox[Skill2]		= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseSkill2"));
	m_IsCastCheckBox[Skill3]		= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseSkill3"));
	m_IsCastCheckBox[Skill4]		= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseSkill4"));
	m_IsCastCheckBox[ShouSkill]		= static_cast<Checkbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/UseShouSkill"));

	m_SkillObj[AttackSkill] = static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/AttackSkillObj"));
	m_SkillObj[Skill0]		= static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/SkillObj0"));
	m_SkillObj[Skill1]		= static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/SkillObj1"));
	m_SkillObj[Skill2]		= static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/SkillObj2"));
	m_SkillObj[Skill3]		= static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/SkillObj3"));
	m_SkillObj[Skill4]		= static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/SkillObj4"));
	m_SkillObj[ShouSkill]	= static_cast<TLGameObject *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/ShouSkillObj"));

#define EDITBOX_NAME "TaharezLook/EntrustComputer/SkillSettingsPanel/IntervalTime"
	size_t nameLength = strlen(EDITBOX_NAME);
	nameLength += 10;

	char * editboxName = new char[nameLength];
	for (int i = 0; i < (SkillCount - 2); i++)
	{
		memset(editboxName, 0, sizeof(editboxName));
		sprintf(editboxName, "%s%d", EDITBOX_NAME, i);
		m_IntervalTime[i] = static_cast<TLEditbox *>(m_pSkillSettingsPanel->getChild(editboxName));
		if (NULL == m_IntervalTime[i])
		{
			continue;
		}
		m_IntervalTime[i]->resetText(CEGUI::String("0"));
	}
	delete[] editboxName;

	m_IntervalTime[ShouSkill - 1] = static_cast<TLEditbox *>(m_pSkillSettingsPanel->getChild("TaharezLook/EntrustComputer/SkillSettingsPanel/ShouIntervalTime"));
	if (m_IntervalTime[ShouSkill - 1] == NULL)
	{
		return;
	}
	m_IntervalTime[ShouSkill - 1]->resetText(CEGUI::String("0"));

	for (i = 0; i < SkillCount; i++)
	{
		m_IsCastCheckBox[i]->subscribeEvent(Checkbox::EventCheckStateChanged, Event::Subscriber(&KUiEntrustComputer::OnUseSkillBnt, ms_Singleton));
		if (NULL == m_IsCastCheckBox[i])
		{
			continue;
		}
		m_IsCastCheckBox[i]->setID(i);

		m_SkillObj[i]->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiEntrustComputer::OnSelectSkillBnt, this));
		if (NULL == m_SkillObj[i])
		{
			continue;
		}
		m_SkillObj[i]->setID(i);

		if (i > 0)
		{
			if (NULL == m_IntervalTime[i - 1])
			{
				continue;
			}
			m_IntervalTime[i - 1]->subscribeEvent(TLEditbox::EventTextChanged, Event::Subscriber(&KUiEntrustComputer::OnTimeTextChanged, this));
		}
	}
}

bool KUiEntrustComputer::OnUseSkillBnt(const CEGUI::EventArgs & args)
{
	if (NULL == m_BeginAutoAttack || NULL == m_StopAutoAttack)
	{
		return false;
	}

	StopAutoAttack(1);
	m_BeginAutoAttack->show();
	m_StopAutoAttack->hide();

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnSelectSkillBnt(const CEGUI::EventArgs & args)
{
	MouseEventArgs & eventArg = (MouseEventArgs &)args;
	TLGameObject * pWindow = (TLGameObject *)eventArg.window;

	if (pWindow == NULL)
	{
		return false;
	}

	if (!KUiEntrustSkill::GetSingleton().IsVisible())
	{
		KUiEntrustSkill::GetSingleton().ShowSkills(pWindow->getID());
	}
	else
	{
		KUiEntrustSkill::Hide();
	}
	return true;
}

bool KUiEntrustComputer::OnSelectItemType(const CEGUI::EventArgs & args)
{
	TLTreeItem * pSelItem = static_cast<TLTreeItem *>(m_AutoPickupTypeList->getLastSelectedItem());
	if (pSelItem == NULL || m_bSelectState == NULL || m_AutoPickupTypeList == NULL || m_AutoPickupSelTypeList == NULL)
	{
		return false;
	}

	m_bSelectState[pSelItem->getID()] = true;;
	pSelItem->setSelected(false);
	m_AutoPickupTypeList->removeItem(pSelItem);

	if (m_AutoPickupSelTypeList->getItemCount() == 0)
	{
		m_AutoPickupSelTypeList->addItem(pSelItem);
	}
	else
	{
		TLTreeItem * pExistItem = NULL;
		bool isInsert = false;
		for (int i = pSelItem->getID(); i >= 0; --i)
		{
			pExistItem = static_cast<TLTreeItem *>(m_AutoPickupSelTypeList->findFirstItemWithID(i));
			if (pExistItem != NULL)
			{
				m_AutoPickupSelTypeList->insertItem(pSelItem, pExistItem);
				m_AutoPickupSelTypeList->removeItem(pExistItem);
				m_AutoPickupSelTypeList->insertItem(pExistItem, pSelItem);
				isInsert = true;
				break;
			}
			else
			{
				continue;
			}
		}
		if (!isInsert)
		{
			for (int i = pSelItem->getID();i <= m_iTypeListNum; i++)
			{
				pExistItem = static_cast<TLTreeItem *>(m_AutoPickupSelTypeList->findFirstItemWithID(i));
				if (pExistItem != NULL)
				{
					m_AutoPickupSelTypeList->insertItem(pSelItem, pExistItem);
					isInsert = true;
					break;
				}
				else
				{
					continue;
				}
			}
		}
	}

	RecalculateScroll();

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnClearSelected(const CEGUI::EventArgs & args)
{
	TLTreeItem * pSelItem = static_cast<TLTreeItem *>(m_AutoPickupSelTypeList->getLastSelectedItem());
	if (pSelItem == NULL || m_bSelectState == NULL || m_AutoPickupTypeList == NULL || m_AutoPickupSelTypeList == NULL)
	{
		return false;
	}

	m_bSelectState[pSelItem->getID()] = false;
	pSelItem->setSelected(false);
	m_AutoPickupSelTypeList->removeItem(pSelItem);

	if (m_AutoPickupTypeList->getItemCount() == 0)
	{
		m_AutoPickupTypeList->addItem(pSelItem);
	}
	else
	{
		TLTreeItem * pExistItem = NULL;
		bool isInsert = false;
		for (int i = pSelItem->getID(); i >= 0; i--)
		{
			pExistItem = static_cast<TLTreeItem *>(m_AutoPickupTypeList->findFirstItemWithID(i));
			if (pExistItem != NULL)
			{
				m_AutoPickupTypeList->insertItem(pSelItem, pExistItem);
				m_AutoPickupTypeList->removeItem(pExistItem);
				m_AutoPickupTypeList->insertItem(pExistItem, pSelItem);
				isInsert = true;
				break;
			}
			else
			{
				continue;
			}
		}
		if (!isInsert)
		{
			for (int i = pSelItem->getID();i <= m_iTypeListNum; i++)
			{
				pExistItem = static_cast<TLTreeItem *>(m_AutoPickupTypeList->findFirstItemWithID(i));
				if (pExistItem != NULL)
				{
					m_AutoPickupTypeList->insertItem(pSelItem, pExistItem);
					isInsert = true;
					break;
				}
				else
				{
					continue;
				}
			}
		}
	}

	RecalculateScroll();

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnClearAllType(const CEGUI::EventArgs & args)
{
	if (m_bSelectState == NULL || m_AutoPickupTypeList == NULL || m_AutoPickupSelTypeList == NULL)
	{
		return false;
	}

	m_AutoPickupSelTypeList->removeAllItem();
	m_AutoPickupTypeList->removeAllItem();

	for (int i = 0; i < m_iTypeListNum; i++)
	{
		m_AutoPickupTypeList->addItem(m_TypeListItem[i]);
		m_bSelectState[i] = false;
	}

	RecalculateScroll();

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnTypeMouseWheel(const CEGUI::EventArgs & args)
{
	if (m_AutoPickupTypeListVScrollbar == NULL)
	{
		return false;
	}

	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(m_AutoPickupTypeListVScrollbar->isVisible())
	{
		m_AutoPickupTypeListVScrollbar->setScrollPosition(m_AutoPickupTypeListVScrollbar->getScrollPosition()
			- m_AutoPickupTypeListVScrollbar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiEntrustComputer::OnSelListMouseWheel(const CEGUI::EventArgs & args)
{
	if (m_AutoPickupSelTypeListVScrollbar == NULL)
	{
		return false;
	}

	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(m_AutoPickupSelTypeListVScrollbar->isVisible())
	{
		m_AutoPickupSelTypeListVScrollbar->setScrollPosition(m_AutoPickupSelTypeListVScrollbar->getScrollPosition()
			- m_AutoPickupSelTypeListVScrollbar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiEntrustComputer::OnBegin(const CEGUI::EventArgs & args)
{
	KUiWndSingleton<KUiEntrustComputer>::Hide();

	if (m_bIsSettingsChanged)
	{
		StartAutoCast();
		StartAutoAttack();
		OnSaveSettings(args);
	}
	else
	{
		StartAutoCast();
		StartAutoAttack();
		StartAutoPickup();
		StartAutoUseItem();
		StartAutoRepair();
	}

	m_bIsSettingsChanged = false;
	return true;
}

bool KUiEntrustComputer::OnStop(const CEGUI::EventArgs & args)
{
	if (NULL == m_BeginAutoAttack || NULL == m_StopAutoAttack)
	{
		return false;
	}

	StopAutoAttack(0);
//	StopAutoPickup();

	m_BeginAutoAttack->show();
	m_StopAutoAttack->hide();

	m_bIsSettingsChanged = false;
	return true;
}

bool KUiEntrustComputer::OnClearAllSettings(const CEGUI::EventArgs & args)
{
	ResetAllNotSave();
	m_bIsLoadBaseSettings = true;
	m_bIsLoadSkillSettings = true;
	m_bIsSettingsChanged = true;

	return true;
}

bool KUiEntrustComputer::OnSaveSettings(const CEGUI::EventArgs & args)
{
	RefreshSettings();
	SaveSettingsToFile();

	StartAutoUseItem();
	StartAutoPickup();
	StartAutoRepair();

	m_bIsSettingsChanged = false;
	return true;
}

bool KUiEntrustComputer::OnAutoAttackGroup(const CEGUI::EventArgs & args)
{
	WindowEventArgs eArgs = static_cast<const WindowEventArgs &>(args);
	Checkbox * curButton = static_cast<Checkbox *>(eArgs.window);

	if (curButton == NULL || m_cbAutoAttack == NULL || m_AutoAttackRange == NULL || m_AutoAttackRetour == NULL)
	{
		return false;
	}

	if (curButton == m_cbAutoAttack)
	{
		setAutoAttack(curButton->isSelected());
	}
	if (curButton == m_AutoAttackRange)
	{
		m_AutoAttackRange->setSelected(true);
		m_AutoAttackRetour->setSelected(false);
	}
	if (curButton == m_AutoAttackRetour)
	{
		m_AutoAttackRange->setSelected(false);
		m_AutoAttackRetour->setSelected(true);
	}

	if (NULL == m_BeginAutoAttack || NULL == m_StopAutoAttack)
	{
		return false;
	}
	
	StopAutoAttack(1);
	m_BeginAutoAttack->show();
	m_StopAutoAttack->hide();

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnAutoUseItem(const CEGUI::EventArgs & args)
{
	WindowEventArgs eArgs = static_cast<const WindowEventArgs &>(args);
	Checkbox * curButton = static_cast<Checkbox *>(eArgs.window);

	if (curButton == NULL 
		|| m_cbAutoUseItem == NULL 
		|| m_AutoUseItem_hp_30 == NULL 
		|| m_AutoUseItem_hp_60 == NULL
		|| m_AutoUseItem_mp_30 == NULL
		|| m_AutoUseItem_mp_60 == NULL)
	{
		return false;
	}

	if (curButton == m_cbAutoUseItem)
	{
		setAutoUseItem(m_cbAutoUseItem->isSelected());
	}
	else if (curButton == m_AutoUseItem_hp_30)
	{
		m_AutoUseItem_hp_30->setSelected(true);
		m_AutoUseItem_hp_60->setSelected(false);
	}
	else if (curButton == m_AutoUseItem_hp_60)
	{
		m_AutoUseItem_hp_30->setSelected(false);
		m_AutoUseItem_hp_60->setSelected(true);
	}
	else if (curButton == m_AutoUseItem_mp_30)
	{
		m_AutoUseItem_mp_30->setSelected(true);
		m_AutoUseItem_mp_60->setSelected(false);
	}
	else if (curButton == m_AutoUseItem_mp_60)
	{
		m_AutoUseItem_mp_30->setSelected(false);
		m_AutoUseItem_mp_60->setSelected(true);
	}
	
/*	if (m_cbAutoUseItem->isSelected())
	{
		StartAutoUseItem();
	}
	else
	{
		StopAutoUseItem();
	}

	char fileName[COMMON_CLIENT_MSG_LEN_256];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s%s%s", UI_ACCOUT_SET, KUiAutoConnect::GetSingleton().GetUserName(), ".ini");
	
	KIniFile iniFile;
	if (!iniFile.Load(fileName))
	{
		FILE *ifile = fopen(fileName, "w");
		if( ifile != NULL)
		{
			fclose(ifile);
		}
		else
		{
			return false;
		}
		iniFile.Load(fileName);
	}

	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoUseItem", m_cbAutoUseItem->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoUseHP30", m_AutoUseItem_hp_30->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoUseMP30", m_AutoUseItem_mp_30->isSelected() ? 1 : 0);

	iniFile.Save(fileName);*/

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnTypeScrollChanged(const CEGUI::EventArgs & args)
{
	if (m_AutoPickupTypeListClipper == NULL || m_AutoPickupTypeList == NULL || m_AutoPickupTypeListVScrollbar == NULL)
	{
		return false;
	}

	int clipperHeight = m_AutoPickupTypeListClipper->getHeight(Absolute);
	int totalItemHeight = m_AutoPickupTypeList->getTreeTotalItemsHeigh();
	float scrollPos = m_AutoPickupTypeListVScrollbar->getScrollPosition();
	int exceedSize = (totalItemHeight - clipperHeight) * scrollPos;
	m_AutoPickupTypeList->setYPosition(Absolute, -exceedSize);
	return true;
}

bool KUiEntrustComputer::OnSelScrollChanged(const CEGUI::EventArgs & args)
{
	if (m_AutoPickupSelTypeListClipper == NULL || m_AutoPickupSelTypeList == NULL || m_AutoPickupSelTypeListVScrollbar == NULL)
	{
		return false;
	}

	int clipperHeight = m_AutoPickupSelTypeListClipper->getHeight(Absolute);
	int totalItemHeight = m_AutoPickupSelTypeList->getTreeTotalItemsHeigh();
	float scrollPos = m_AutoPickupSelTypeListVScrollbar->getScrollPosition();
	int exceedSize = (totalItemHeight - clipperHeight) * scrollPos;
	m_AutoPickupSelTypeList->setYPosition(Absolute, -exceedSize);
	return true;
}

void KUiEntrustComputer::RecalculateScroll()
{
	if (m_AutoPickupTypeListClipper == NULL || m_AutoPickupTypeList == NULL || m_AutoPickupTypeListVScrollbar == NULL)
	{
		return;
	}

	int clipperHeight = m_AutoPickupTypeListClipper->getHeight(Absolute);
	int totalItemHeight = m_AutoPickupTypeList->getTreeTotalItemsHeigh();
	float scrollPos = m_AutoPickupTypeListVScrollbar->getScrollPosition();

	if (clipperHeight >= totalItemHeight)
	{
		m_AutoPickupTypeListVScrollbar->hide();
		m_AutoPickupTypeList->setYPosition(Absolute, 0);
	}
	else
	{
		m_AutoPickupTypeListVScrollbar->show();

		int itemCount = m_AutoPickupTypeList->getItemCount();
		float nStep = scrollPos / m_AutoPickupTypeListVScrollbar->getStepSize();
		float stepSize = ((float)totalItemHeight / itemCount) / (totalItemHeight - clipperHeight);
		m_AutoPickupTypeListVScrollbar->setStepSize(stepSize);

		scrollPos = stepSize * nStep;
		m_AutoPickupTypeListVScrollbar->setScrollPosition(scrollPos);
	}

	clipperHeight = m_AutoPickupSelTypeListClipper->getHeight(Absolute);
	totalItemHeight = m_AutoPickupSelTypeList->getTreeTotalItemsHeigh();
	scrollPos = m_AutoPickupSelTypeListVScrollbar->getScrollPosition();

	if (clipperHeight >= totalItemHeight)
	{
		m_AutoPickupSelTypeListVScrollbar->hide();
		m_AutoPickupSelTypeList->setYPosition(Absolute, 0);
	}
	else
	{
		m_AutoPickupSelTypeListVScrollbar->show();
		
		int itemCount = m_AutoPickupSelTypeList->getItemCount();
		float nStep = scrollPos / m_AutoPickupSelTypeListVScrollbar->getStepSize();
		float stepSize = ((float)totalItemHeight / itemCount) / (totalItemHeight - clipperHeight);
		m_AutoPickupSelTypeListVScrollbar->setStepSize(stepSize);

		scrollPos = stepSize * nStep;
		m_AutoPickupSelTypeListVScrollbar->setScrollPosition(scrollPos);
	}
}

void KUiEntrustComputer::SetSelectedSkill(TLGameObject * skillObj, int nIdx)
{
	if (skillObj == NULL || g_pCoreShell == NULL)
	{
		return;
	}

	if (nIdx >= SkillCount || nIdx < 0)
	{
		return;
	}

	if (m_SkillObj[nIdx] == NULL)
	{
		return;
	}

	m_SkillObj[nIdx]->setObject(skillObj->getObject());
	m_SkillObj[nIdx]->setTooltipText(skillObj->getTooltipText());
	TLGameObject::GameObject GO;
	m_SkillObj[nIdx]->getObject(GO);
	if (GO.d_skillID > 0)
	{
		m_SkillObj[nIdx]->requestRedraw();
	}

	if (nIdx > 0)
	{
		if (m_IntervalTime[nIdx - 1] == NULL)
		{
			return;
		}
		KSkillInfo info;
		g_pCoreShell->GetGameData(GDI_SKILL_INFO, (unsigned int)&info, GO.d_skillID);
		int intervalTime = 0;
		if (info.nCoolingTime <= 0)
		{
			intervalTime = 2;
		}
		else
		{
			intervalTime = info.nCoolingTime / 1000;
		}
		m_IntervalTime[nIdx - 1]->resetText(iToString(intervalTime));
	}

	if (NULL == m_BeginAutoAttack || NULL == m_StopAutoAttack)
	{
		return;
	}

	StopAutoAttack(1);
	m_BeginAutoAttack->show();
	m_StopAutoAttack->hide();

	m_bIsSettingsChanged = true;
}

void KUiEntrustComputer::StartAutoAttack()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	if (m_AutoAttackRange == NULL || m_AutoAttackRetour == NULL)
	{
		return;
	}

	m_iAutoAttackState = g_pCoreShell->OperationRequest(GOI_AUTOATTACK_SWITCH, 0, m_cbAutoAttack->isSelected());
	m_iAutoAttackLowerState	= g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS, AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER, m_cbAutoAttack_level_lower->isSelected());
	m_iAutoAttackHigherState = g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS, AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER, m_cbAutoAttack_level_higher->isSelected());
	g_pCoreShell->OperationRequest(GOI_AUTO_ATTACK_BLAST, 0, m_AutoAttackBlast->isSelected());

	if (m_AutoAttackRange->isSelected())
	{
		KIniFile iniFile;
		if (iniFile.Load(AUTO_ATTACK_SETTINGS_PATH))
		{
			int attackRadius = 0;
			iniFile.GetInteger("autoAttackDistance", "RangeDistance", 80, &attackRadius);
			g_pCoreShell->OperationRequest(GOI_AUTO_ATTACK_RADIUS, 0, attackRadius);
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_AUTO_ATTACK_RADIUS, 0, 80);
		}
	}
	else if (m_AutoAttackRetour->isSelected())
	{
		KIniFile iniFile;
		if (iniFile.Load(AUTO_ATTACK_SETTINGS_PATH))
		{
			int attackRadius = 0;
			iniFile.GetInteger("autoAttackDistance", "RetourDistance", 10, &attackRadius);
			g_pCoreShell->OperationRequest(GOI_AUTO_ATTACK_RADIUS, 0, attackRadius);
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_AUTO_ATTACK_RADIUS, 0, 10);
		}
	}
}

void KUiEntrustComputer::StartAutoPickup()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	if (m_cbAutoPickup == NULL
		|| m_AutoPickupTypeList == NULL)
	{
		return;
	}

	if (m_cbAutoPickup->isSelected())
	{
		static deque<ItemClassInfo> infoList;
		infoList.clear();
		for (int i = 0; i < m_AutoPickupTypeList->getItemCount(); i++)
		{
			TLTreeItem * item = static_cast<TLTreeItem *>(m_AutoPickupTypeList->getItemFromIndex(i));
			map<int, ItemClassInfo>::iterator iter = NULL;
			iter = m_ClassInfoList.find(item->getID());
			if (iter != NULL && iter != m_ClassInfoList.end())
			{
				ItemClassInfo & info = iter->second;
				infoList.push_back(iter->second);
			}
		}
		g_pCoreShell->OperationRequest(GOI_AUTOPICKUP_ITEM_SWITCH, (unsigned int)(&infoList), true);
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_AUTOPICKUP_ITEM_SWITCH, 0, false);
	}
}

void KUiEntrustComputer::StartAutoCast()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	TLGameObject::GameObject obj;
	AutoCastSkillInfo skillInfo;
	for (int i = 0; i < SkillCount; i++)
	{
		if (m_SkillObj[i] == NULL || m_IsCastCheckBox[i] == NULL)
		{
			continue;
		}
		m_SkillObj[i]->getObject(obj);
		skillInfo.m_SkillID = obj.d_skillID;
		skillInfo.m_IsSelected = m_IsCastCheckBox[i]->isSelected();
		if (i > 0)
		{
			if (m_IntervalTime[i - 1] == NULL)
			{
				continue;
			}

			char * timeStr = Utf8ToAnsi(m_IntervalTime[i - 1]->getText().c_str());
			skillInfo.m_CastInterval = atoi(timeStr) * 1000;
			KSkillInfo info;
			g_pCoreShell->GetGameData(GDI_SKILL_INFO, (unsigned int)&info, obj.d_skillID);
			if (skillInfo.m_CastInterval < info.nCoolingTime)
			{
				skillInfo.m_CastInterval = info.nCoolingTime;
				m_IntervalTime[i - 1]->resetText(iToString(info.nCoolingTime / 1000));
			}
			if (skillInfo.m_CastInterval > (3600 * 1000))
			{
				skillInfo.m_CastInterval = 3600 * 1000;
				m_IntervalTime[i - 1]->resetText(iToString(3600));
			}
			else if (skillInfo.m_CastInterval <= 0)
			{
				skillInfo.m_CastInterval = 2 * 1000;
				m_IntervalTime[i - 1]->resetText(iToString(2));
			}
		}
		if (i == SkillCount - 1)
		{
			KUiPlayerBaseInfo baseInfo;
			KUiPlayerAttribute playerAttr;
			g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
			g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&playerAttr, NULL);
			if (!(playerAttr.nSeries == 2 && baseInfo.nSkillType == 0))
			{
				skillInfo.m_IsSelected = false;
				skillInfo.m_SkillID = INVALID_SKILL_ID;
			}
		}
		g_pCoreShell->OperationRequest(GOI_AUTO_CAST_SKILL, (unsigned int)&skillInfo, i);
	}
}

void KUiEntrustComputer::StartAutoUseItem()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	if (m_AutoUseItem_hp_30 == NULL
		|| m_AutoUseItem_hp_60 == NULL
		|| m_AutoUseItem_mp_30 == NULL
		|| m_AutoUseItem_mp_60 == NULL)
	{
		return;
	}

	g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_SWITCH, 0, m_cbAutoUseItem->isSelected());
	if (m_AutoUseItem_hp_30->isSelected())
	{
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS, AUTO_USE_HP_MEDICINE_LIFE_1, true);
	}
	else if (m_AutoUseItem_hp_60->isSelected())
	{
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS, AUTO_USE_HP_MEDICINE_LIFE_2, true);
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS, AUTO_USE_HP_MEDICINE_LIFE_1, true);
	}

	if (m_AutoUseItem_mp_30->isSelected())
	{
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS, AUTO_USE_MP_MEDICINE_1, true);
	}
	else if (m_AutoUseItem_mp_60->isSelected())
	{
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS, AUTO_USE_MP_MEDICINE_2, true);
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS, AUTO_USE_MP_MEDICINE_1, true);
	}
}

void KUiEntrustComputer::SetSkillPanel()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	if (m_IsCastCheckBox[ShouSkill] == NULL
		|| m_SkillObj[ShouSkill] == NULL
		|| m_IntervalTime[ShouSkill - 1] == NULL)
	{
		return;
	}

	KUiPlayerBaseInfo baseInfo;
	KUiPlayerAttribute playerAttr;
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&playerAttr, NULL);
	if (playerAttr.nSeries == 2 && baseInfo.nSkillType == 0)
	{	
		m_IsCastCheckBox[ShouSkill]->setEnabled(true);
		m_SkillObj[ShouSkill]->setEnabled(true);
		m_IntervalTime[ShouSkill - 1]->setEnabled(true);
	}
	else
	{
		m_IsCastCheckBox[ShouSkill]->setEnabled(false);
		m_SkillObj[ShouSkill]->setEnabled(false);
		m_IntervalTime[ShouSkill - 1]->setEnabled(false);
	}
}

void KUiEntrustComputer::Show(void)
{
	KUiWndSingleton<KUiEntrustComputer>::Show();
	if (ms_Singleton == NULL)
	{
		return;
	}

	if (!ms_Singleton->m_bIsLoadBaseSettings)
	{
		ms_Singleton->m_bIsLoadBaseSettings = ms_Singleton->LoadBaseSettings();
	}

	if (!ms_Singleton->m_bIsLoadBaseSettings)
	{
		ms_Singleton->m_bIsLoadBaseSettings = ms_Singleton->LoadDefaultSettings();
	}

	if (!ms_Singleton->m_bIsLoadSkillSettings)
	{
		ms_Singleton->m_bIsLoadSkillSettings = ms_Singleton->LoadSkillSettings();
	}

	if (ms_Singleton->m_BeginAutoAttack == NULL || ms_Singleton->m_StopAutoAttack == NULL)
	{
		return;
	}

	if (ms_Singleton->m_iAutoAttackState == 1)
	{
		ms_Singleton->m_BeginAutoAttack->hide();
		ms_Singleton->m_StopAutoAttack->show();
	}
	else
	{
		ms_Singleton->m_BeginAutoAttack->show();
		ms_Singleton->m_StopAutoAttack->hide();
	}

	ms_Singleton->RefreshSettings();
	ms_Singleton->SetSkillPanel();
}

void KUiEntrustComputer::RefreshSettings()
{
	if (!CheckPtravailable())
	{
		return;
	}

	m_ButtonStates[ButtonAutoAttack]		= m_cbAutoAttack->isSelected();
	m_ButtonStates[ButtonAutoAttackHeigher]	= m_cbAutoAttack_level_higher->isSelected();
	m_ButtonStates[ButtonAutoAttackLower]	= m_cbAutoAttack_level_lower->isSelected();
	m_ButtonStates[ButtonAutoAttackBlast]	= m_AutoAttackBlast->isSelected();
	m_ButtonStates[ButtonAutoAttackRetour]	= m_AutoAttackRetour->isSelected();
	m_ButtonStates[ButtonAutoAttackRange]	= m_AutoAttackRange->isSelected();
	m_ButtonStates[ButtonAutoUseItem]		= m_cbAutoUseItem->isSelected();
	m_ButtonStates[ButtonAutoUseHP30]		= m_AutoUseItem_hp_30->isSelected();
	m_ButtonStates[ButtonAutoUseHP60]		= m_AutoUseItem_hp_60->isSelected();
	m_ButtonStates[ButtonAutoUseMP30]		= m_AutoUseItem_mp_30->isSelected();
	m_ButtonStates[ButtonAutoUseMP60]		= m_AutoUseItem_mp_60->isSelected();
	m_ButtonStates[ButtonAutoRepair]		= m_AutoRepair->isSelected();
	m_ButtonStates[ButtonAutoPickup]		= m_cbAutoPickup->isSelected();

	m_SelectedItemID.clear();
	for (int i = 0; i < m_AutoPickupSelTypeList->getItemCount(); i++)
	{
		TLTreeItem * pItem = static_cast<TLTreeItem *>(m_AutoPickupSelTypeList->getItemFromIndex(i));
		if (pItem == NULL)
		{
			continue;
		}
		m_SelectedItemID.push_back(pItem->getID());
	}

	m_BackupIntervalTime.clear();
	for (i = 0; i < SkillCount; i++)
	{	
		if (m_SkillObj[i] == NULL)
		{
			continue;
		}
		m_SkillObj[i]->getObject(m_BackupSkillObj[i].m_GameObject);	
		m_BackupSkillObj[i].m_Tooltip = m_SkillObj[i]->getTooltipText();

		if (m_IsCastCheckBox[i] == NULL)
		{
			continue;
		}
		m_CanCastButtonState[i] = m_IsCastCheckBox[i]->isSelected();

		if (i > 0)
		{
			if (m_IntervalTime[i - 1] == NULL)
			{
				continue;
			}
			char * timeStr = Utf8ToAnsi(const_cast<char *>(m_IntervalTime[i - 1]->getText().c_str()));
			if (timeStr == NULL)
			{
				continue;
			}
			m_BackupIntervalTime.push_back(atoi(timeStr));
		}
	}
}

bool KUiEntrustComputer::LoadDefaultSettings()
{
	KIniFile iniFile;
	if (!iniFile.Load(AUTO_ATTACK_DEFAULT_SETTINGS))
	{
		return false;
	}

	m_bIsLoadBaseSettings = LoadBaseSettings(iniFile, "EntrustCfg");
	m_bIsLoadSkillSettings = LoadDefaultSkillSettings(iniFile, "EntrustCfg");

	if (!m_bIsLoadBaseSettings)
	{
		return false;
	}

	StartAutoUseItem();
	StartAutoPickup();
	StartAutoRepair();

	return true;
}

void KUiEntrustComputer::SetSkillObject(int index, int skillId)
{
	if (0 > index || index >= SkillCount || INVALID_SKILL_ID == skillId)
	{
		return;
	}

	if (NULL == m_SkillObj[index])
	{
		return;
	}

	KSkillInfo tagSkillInfo;
	tagSkillInfo.bDescAvailable = true;
	tagSkillInfo.dwDescStyle = 0;
	tagSkillInfo.bNextDescAvailable = true;
	tagSkillInfo.dwNextDescStyle = 1;
	tagSkillInfo.nStudyTipBuffSize = MAX_STUDY_TIP_SIZE;
	skillId = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_ID, skillId, NULL);

	TLGameObject::GameObject tagGO;
	m_SkillObj[index]->clear();
	if (g_pCoreShell->GetGameData(GDI_GET_SKILL_LEVEL, skillId, NULL) > 0)
	{
		g_pCoreShell->GetGameData(GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, skillId);
		tagGO.d_gameobject = AnsiToUtf8( tagSkillInfo.szIconName );
		tagGO.d_type = TLGameObject::shortcut;
		tagGO.d_coolingTime = tagSkillInfo.nCoolingTime / 1000;
		tagGO.d_passivity = tagSkillInfo.bPassivity;
		tagGO.d_skillID = skillId;
		m_SkillObj[index]->setObject(tagGO);
		m_SkillObj[index]->setTooltipText(AnsiToUtf8(tagSkillInfo.szDesc));
	}
	else
	{
		char * tip = KMessageCentre::GetMessageSafe(entrustComputerTip_message, 0);
		m_SkillObj[index]->setObject(tagGO);
		m_SkillObj[index]->setTooltipText(AnsiToUtf8(tip));
	}
}

void KUiEntrustComputer::GetMetierKeyName(char * buffer, int bufferLen, DWORD baseMetier)
{
	if (NULL == buffer || 0 == bufferLen)
	{
		return;
	}

	DWORD dwMetire = baseMetier;
	DWORD dwBaseMetire = (dwMetire & 0x000000000f);
	DWORD dwHiwordMetire = ((dwMetire & 0x000000f0)>>4);

	switch(dwBaseMetire)
	{
	case 0:
		{
			if (dwHiwordMetire == 0x0000000f)
			{
				snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_JIASHI);
			}
			else
			{
				if (dwHiwordMetire == 0)
				{
					snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_XINGTIAN);
				}
				else
				{
					snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_XUANFENG);
				}
			}
		}
		break;
	case 1:
		{
			if (dwHiwordMetire == 0x0000000f)
			{
				snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_DAOSHI);
			}
			else
			{
				if (dwHiwordMetire == 0)
				{
					snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_ZHENREN);
				}
				else
				{
					snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_TIANSHI);
				}
			}
		}
		break;
	case 2:
		{
			if (dwHiwordMetire == 0x0000000f)
			{
				snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_YIREN);
			}
			else
			{
				if (dwHiwordMetire == 0)
				{
					snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_SHOUSHI);
				}
				else
				{
					snprintf(buffer, bufferLen, DEFAULT_SKILL_STR_YISHI);
				}
			}
		}
		break;
	}
}

bool KUiEntrustComputer::LoadDefaultSkillSettings(KIniFile & iniFile, char * section)
{
	if (NULL == section)
	{
		return false;
	}

	KUiPlayerAttribute playerAttr;

	g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&playerAttr, NULL);

	char keyName[COMMON_CLIENT_MSG_LEN_32] = {0};

	GetMetierKeyName(keyName, sizeof(keyName), playerAttr.nSeries);
	int nValue = 0;
	iniFile.GetInteger(section, keyName, 0, &nValue);

	if (NULL == m_IsCastCheckBox[AttackSkill])
	{
		return false;
	}

	m_IsCastCheckBox[AttackSkill]->setSelected(true);

	if (INVALID_SKILL_ID == nValue)
	{
		return false;
	}

	SetSkillObject(AttackSkill, nValue);

	return true;
}

bool KUiEntrustComputer::LoadBaseSettings()
{
	if (!CheckPtravailable())
	{
		return false;
	}

	char fileName[COMMON_CLIENT_MSG_LEN_256];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s%s%s", UI_ACCOUT_SET, KUiAutoConnect::GetSingleton().GetUserName(), ".ini");

	KIniFile iniFile;
	if (!iniFile.Load(fileName))
	{
		return false;
	}

	int isNeedDefault = 1;
	iniFile.GetInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "NeedDefault", 1, &isNeedDefault);
	if (1 == isNeedDefault)
	{
		iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "NeedDefault", 0);
		iniFile.Save(fileName);
		return false;
	}

	bool ret = LoadBaseSettings(iniFile, KUiAutoConnect::GetSingleton().GetRoleName());

	if (!ret)
	{
		return ret;
	}

	StartAutoUseItem();
	StartAutoPickup();
	StartAutoRepair();

	return ret;
}

bool KUiEntrustComputer::LoadBaseSettings(KIniFile & iniFile, char * section)
{
	if (NULL == section)
	{
		return false;
	}

	if (!CheckPtravailable())
	{
		return false;
	}

	int isSelected = 0;
	iniFile.GetInteger(section, "AutoAttack", 0, &isSelected);
	m_cbAutoAttack->setSelected((isSelected == 1));

	if (m_cbAutoAttack->isSelected())
	{
		setAutoAttack(true);
		isSelected = 0;
		iniFile.GetInteger(section, "AttackHeigher", 0, &isSelected);
		m_cbAutoAttack_level_higher->setSelected((isSelected == 1));

		isSelected = 0;
		iniFile.GetInteger(section, "AttackLower", 0, &isSelected);
		m_cbAutoAttack_level_lower->setSelected((isSelected == 1));

		isSelected = 0;
		iniFile.GetInteger(section, "AttackBlast", 0, &isSelected);
		m_AutoAttackBlast->setSelected((isSelected == 1));

		isSelected = 0;
		iniFile.GetInteger(section, "AttackRetour", 0, &isSelected);
		if (isSelected == 1)
		{
			m_AutoAttackRetour->setSelected((isSelected == 1));
			m_AutoAttackRange->setSelected(false);
		}
		else
		{
			m_AutoAttackRetour->setSelected((isSelected == 1));
			m_AutoAttackRange->setSelected(true);
		}
	}

	isSelected = 0;
	iniFile.GetInteger(section, "AutoUseItem", 0, &isSelected);
	m_cbAutoUseItem->setSelected((isSelected == 1));

	if (m_cbAutoUseItem->isSelected())
	{
		setAutoUseItem(true);
		isSelected = 0;
		iniFile.GetInteger(section, "AutoUseHP30", 0, &isSelected);
		if (isSelected == 1)
		{
			m_AutoUseItem_hp_30->setSelected(true);
			m_AutoUseItem_hp_60->setSelected(false);
		}
		else
		{
			m_AutoUseItem_hp_30->setSelected(false);
			m_AutoUseItem_hp_60->setSelected(true);
		}

		isSelected = 0;
		iniFile.GetInteger(section, "AutoUseMP30", 0, &isSelected);
		if (isSelected == 1)
		{
			m_AutoUseItem_mp_30->setSelected(true);
			m_AutoUseItem_mp_60->setSelected(false);
		}
		else
		{
			m_AutoUseItem_mp_30->setSelected(false);
			m_AutoUseItem_mp_60->setSelected(true);
		}
	}

	isSelected = 0;
	iniFile.GetInteger(section, "AutoPickup", 0, &isSelected);
	m_cbAutoPickup->setSelected((isSelected == 1));

	isSelected = false;
	iniFile.GetInteger(section, "AutoRepair", 0, &isSelected);
	m_AutoRepair->setSelected((isSelected == 1));

	char keyName[COMMON_CLIENT_MSG_LEN_32];
	int listNum = 0;
	iniFile.GetInteger(section, "PickupListNum", 0, &listNum);

	deque<int>	IDList;
	for (int i = 0; i < listNum; i++)
	{
		int listID = -1;
		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "PickupListID%d", i);
		iniFile.GetInteger(section, keyName, -1, &listID);

		IDList.push_back(listID);
	}

	m_AutoPickupTypeList->removeAllItem();
	m_AutoPickupSelTypeList->removeAllItem();

	bool bIsSelected = false;
	for (i = 0; i < m_iTypeListNum; i++)
	{
		bIsSelected =false;
		for (int j = 0; j < listNum; j++)
		{
			if (m_TypeListItem[i] == NULL || m_AutoPickupSelTypeList == NULL)
			{
				continue;
			}

			if (!IDList.empty())
			{
				if (m_TypeListItem[i]->getID() == IDList.front())
				{
					m_AutoPickupSelTypeList->addItem(m_TypeListItem[i]);
					IDList.pop_front();
					bIsSelected = true;
					break;
				}
			}
		}
		if (bIsSelected)
		{
			continue;
		}
		if (m_AutoPickupTypeList == NULL)
		{
			continue;
		}

		m_AutoPickupTypeList->addItem(m_TypeListItem[i]);
	}

	RecalculateScroll();
	return true;
}

bool KUiEntrustComputer::LoadSkillSettings()
{
	char fileName[COMMON_CLIENT_MSG_LEN_256];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s%s%s", UI_ACCOUT_SET, KUiAutoConnect::GetSingleton().GetUserName(), ".ini");

	KIniFile iniFile;
	if (!iniFile.Load(fileName))
	{
		return false;
	}

	char keyName[COMMON_CLIENT_MSG_LEN_32];
	for (int i = 0; i < SkillCount; i++)
	{
		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "IsCastSkill%d", i);
		int nValue = 0;
		iniFile.GetInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, 0, &nValue);
		bool isCast = false;
		if (nValue == 1)
		{
			isCast = true;
		}
		else if (nValue == 0)
		{
			isCast = false;
		}
		if (m_IsCastCheckBox[i] == NULL)
		{
			continue;
		}

		m_IsCastCheckBox[i]->setSelected(isCast);

		if (i > 0)
		{
			memset(keyName, 0, sizeof(keyName));
			sprintf(keyName, "IntervalTime%d", i - 1);
			iniFile.GetInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, 0, &nValue);
			if (m_IntervalTime[i - 1] == NULL)
			{
				continue;
			}
			m_IntervalTime[i - 1]->resetText(iToString(nValue));
		}

		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "SkillInfo%d", i);
		iniFile.GetInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, 0, &nValue);

		if (nValue == INVALID_SKILL_ID)
		{
			continue;
		}

		SetSkillObject(i, nValue);
	}

	return true;
}

void KUiEntrustComputer::UpdateSkill()
{
	if (ms_Singleton == NULL || g_pCoreShell == NULL)
	{
		return;
	}

	for (int i = 0; i < SkillCount; i++)
	{
		if (m_SkillObj[i] == NULL)
		{
			continue;
		}

		if (m_SkillObj[i]->getState() == TLGameObject::idleState)
		{
			continue;
		}
		TLGameObject::GameObject sourObjInfo;
		m_SkillObj[i]->getObject(sourObjInfo);
		if (sourObjInfo.d_skillID == INVALID_SKILL_ID)
		{
			continue;
		}
		KSkillInfo tagSkillInfo;
		tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
		tagSkillInfo.dwDescStyle=0;
		g_pCoreShell->GetGameData(GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, sourObjInfo.d_skillID);
		m_SkillObj[i]->setTooltipText( AnsiToUtf8(tagSkillInfo.szDesc));
		sourObjInfo.d_skillID = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_ID, sourObjInfo.d_skillID, NULL);
		m_SkillObj[i]->setObject(sourObjInfo);
	}
}

void KUiEntrustComputer::ResetAll()
{
	ResetAllNotSave();

	StopAutoPickup();
	StopAutoUseItem();
	StopAutoAttack(0);
	RefreshSettings();

	m_bIsLoadBaseSettings = false;
	m_bIsLoadSkillSettings = false;
	m_bIsSettingsChanged = false;
}

void KUiEntrustComputer::ResetAllNotSave()
{
	if (m_cbAutoAttack == NULL
		|| m_cbAutoUseItem == NULL
		|| m_cbAutoPickup == NULL
		|| m_AutoRepair == NULL)
	{
		return;
	}
	setAutoAttack(false);
	setAutoUseItem(false);
	m_cbAutoAttack->setSelected(false);
	m_cbAutoPickup->setSelected(false);
	m_cbAutoUseItem->setSelected(false);
	m_AutoRepair->setSelected(false);
	const CEGUI::EventArgs args;
	OnClearAllType(args);

	for (int i = 0; i < SkillCount; i++)
	{
		if (m_SkillObj[i] == NULL)
		{
			continue;
		}

		m_SkillObj[i]->clear();
		TLGameObject::GameObject obj;
		m_SkillObj[i]->setObject(obj);
		char * tip = KMessageCentre::GetMessageSafe(entrustComputerTip_message, 0);
		if (tip == NULL || tip[0] == 0)
		{
			m_SkillObj[i]->setTooltipText("");
			continue;
		}
		m_SkillObj[i]->setTooltipText(AnsiToUtf8(tip));

		if (m_IsCastCheckBox[i] == NULL)
		{
			continue;
		}
		m_IsCastCheckBox[i]->setSelected(false);

		if (i > 0)
		{
			if (m_IntervalTime[i - 1] == NULL)
			{
				continue;
			}
			m_IntervalTime[i - 1]->resetText(iToString(0));
		}
	}
}

bool KUiEntrustComputer::SaveSettingsToFile()
{
	if (!CheckPtravailable())
	{
		return false;
	}

	if (m_AutoPickupSelTypeList == NULL)
	{
		return false;
	}

	char fileName[COMMON_CLIENT_MSG_LEN_256];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s%s%s", UI_ACCOUT_SET, KUiAutoConnect::GetSingleton().GetUserName(), ".ini");

	KIniFile iniFile;
	if (!iniFile.Load(fileName))
	{
		FILE *ifile = fopen(fileName, "w");
		if( ifile != NULL)
		{
			fclose(ifile);
		}
		else
		{
			return false;
		}
		iniFile.Load(fileName);
	}

	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoAttack", m_cbAutoAttack->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AttackHeigher", m_cbAutoAttack_level_higher->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AttackLower", m_cbAutoAttack_level_lower->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AttackBlast", m_AutoAttackBlast->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AttackRetour", m_AutoAttackRetour->isSelected() ? 1 : 0);

	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoUseItem", m_cbAutoUseItem->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoUseHP30", m_AutoUseItem_hp_30->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoUseMP30", m_AutoUseItem_mp_30->isSelected() ? 1 : 0);

	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoPickup", m_cbAutoPickup->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoRepair", m_AutoRepair->isSelected() ? 1 : 0);

	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "PickupListNum", m_AutoPickupSelTypeList->getItemCount());
	char keyName[COMMON_CLIENT_MSG_LEN_32];
	for (int i = 0; i < m_AutoPickupSelTypeList->getItemCount(); i++)
	{
		TLTreeItem * pItem = static_cast<TLTreeItem *>(m_AutoPickupSelTypeList->getItemFromIndex(i));
		if (pItem == NULL)
		{
			continue;
		}
		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "PickupListID%d", i);
		iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, pItem->getID());
	}

	char * timeStr = NULL;
	for (i = 0; i < SkillCount; i++)
	{
		if (m_IsCastCheckBox[i] == NULL || m_SkillObj[i] == NULL)
		{
			continue;
		}

		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "IsCastSkill%d", i);
		iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, m_IsCastCheckBox[i]->isSelected() ? 1 : 0);

		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "SkillInfo%d", i);
		TLGameObject::GameObject obj;
		m_SkillObj[i]->getObject(obj);
		iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, obj.d_skillID);

		if (i > 0)
		{
			if (m_IntervalTime[i - 1] == NULL)
			{
				continue;
			}

			memset(keyName, 0, sizeof(keyName));
			sprintf(keyName, "IntervalTime%d", i - 1);
			timeStr = Utf8ToAnsi(m_IntervalTime[i - 1]->getText().c_str());
			if (timeStr == NULL)
			{
				continue;
			}
			int intervalTime = atoi(timeStr);
			//不知道为啥，这句把roleName同时置为m_IntervalTime[i]->getText().c_str()了。。难道越界了？
			//iniFile.WriteInteger(RoleName, keyName, intervalTime);
			iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, intervalTime);
		}
	}

	iniFile.Save(fileName);
	return true;
}

void KUiEntrustComputer::StopAutoAttack(int msgState)
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	m_iAutoAttackState = g_pCoreShell->OperationRequest(GOI_AUTOATTACK_SWITCH, msgState, false);
	m_iAutoAttackLowerState	= g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS, AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER, false);
	m_iAutoAttackHigherState = g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS, AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER, false);
	g_pCoreShell->OperationRequest(GOI_AUTO_ATTACK_BLAST, 0, false);
}

void KUiEntrustComputer::StopAutoPickup()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	g_pCoreShell->OperationRequest(GOI_AUTOPICKUP_ITEM_SWITCH, 0, false);
}

void KUiEntrustComputer::StopAutoUseItem()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_SWITCH, 0, false);
}

void KUiEntrustComputer::CtrlAndA()
{
	if (m_BeginAutoAttack == NULL || m_StopAutoAttack == NULL)
	{
		return;
	}

	if (!m_bIsLoadBaseSettings || !m_bIsLoadSkillSettings)
	{
		KUiEntrustComputer::Show();
		m_bIsSettingsChanged = false;
		KUiEntrustComputer::Hide();
	}

	if (m_iAutoAttackState == 0)
	{
/*		StartAutoAttack();
	//	StartAutoPickup();
	//	StartAutoUseItem();
		StartAutoCast();

		if (m_bIsSettingsChanged)
		{
			RefreshSettings();
			SaveSettingsToFile();
		}*/
		EventArgs e;
		OnBegin(e);
	}
	else
	{	
/*		StopAutoAttack(0);
	//	StopAutoPickup();
	//	StopAutoUseItem();*/
		EventArgs e;
		OnStop(e);	
	}

	if (1 == m_iAutoAttackState)
	{
		m_BeginAutoAttack->hide();
		m_StopAutoAttack->show();
	}
	else
	{
		m_BeginAutoAttack->show();
		m_StopAutoAttack->hide();
	}
}

bool KUiEntrustComputer::CheckPtravailable()
{
	if (m_cbAutoAttack == NULL
		||	m_cbAutoAttack_level_higher == NULL
		||	m_cbAutoAttack_level_lower == NULL
		||	m_AutoAttackBlast == NULL
		||	m_AutoAttackRetour == NULL
		||	m_AutoAttackRange == NULL
		||	m_cbAutoUseItem == NULL
		||	m_AutoUseItem_hp_30 == NULL
		||	m_AutoUseItem_hp_60 == NULL
		||	m_AutoUseItem_mp_30 == NULL
		||	m_AutoUseItem_mp_60 == NULL
		||	m_cbAutoPickup == NULL
		||	m_AutoRepair == NULL)
	{
		return false;
	}
	return true;
}

bool KUiEntrustComputer::OnAutoRepair(const CEGUI::EventArgs & args)
{
/*	if (m_AutoRepair == NULL || g_pCoreShell == NULL)
	{
		return false;
	}

	char fileName[COMMON_CLIENT_MSG_LEN_256];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s%s%s", UI_ACCOUT_SET, KUiAutoConnect::GetSingleton().GetUserName(), ".ini");

	KIniFile iniFile;
	if (!iniFile.Load(fileName))
	{
		FILE *ifile = fopen(fileName, "w");
		if( ifile != NULL)
		{
			fclose(ifile);
		}
		else
		{
			return false;
		}
		iniFile.Load(fileName);
	}

	g_pCoreShell->OperationRequest(GOI_AUTO_REPAIR, 0, m_AutoRepair->isSelected());
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoRepair", m_AutoRepair->isSelected() ? 1 : 0);

	iniFile.Save(fileName);*/

	m_bIsSettingsChanged = true;
	return true;
}

bool KUiEntrustComputer::OnAutoPickup(const CEGUI::EventArgs & args)
{
/*	if (NULL == m_cbAutoPickup)
	{
		return false;
	}

	SavePickupSettings();*/

	m_bIsSettingsChanged = true;
	return true;
}
/*
void KUiEntrustComputer::SavePickupSettings()
{
	if (m_AutoPickupSelTypeList == NULL || NULL == m_cbAutoPickup)
	{
		return;
	}
	
	char fileName[COMMON_CLIENT_MSG_LEN_256];
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s%s%s", UI_ACCOUT_SET, KUiAutoConnect::GetSingleton().GetUserName(), ".ini");
	
	KIniFile iniFile;
	if (!iniFile.Load(fileName))
	{
		FILE *ifile = fopen(fileName, "w");
		if( ifile != NULL)
		{
			fclose(ifile);
		}
		else
		{
			return;
		}
		iniFile.Load(fileName);
	}

	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "AutoPickup", m_cbAutoPickup->isSelected() ? 1 : 0);
	iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), "PickupListNum", m_AutoPickupSelTypeList->getItemCount());

	char keyName[COMMON_CLIENT_MSG_LEN_32];
	for (int i = 0; i < m_AutoPickupSelTypeList->getItemCount(); i++)
	{
		TLTreeItem * pItem = static_cast<TLTreeItem *>(m_AutoPickupSelTypeList->getItemFromIndex(i));
		if (pItem == NULL)
		{
			continue;
		}
		memset(keyName, 0, sizeof(keyName));
		sprintf(keyName, "PickupListID%d", i);
		iniFile.WriteInteger(KUiAutoConnect::GetSingleton().GetRoleName(), keyName, pItem->getID());
	}

	iniFile.Save(fileName);
}
*/
void KUiEntrustComputer::StartAutoRepair()
{
	if (g_pCoreShell == NULL || m_AutoRepair == NULL)
	{
		return;
	}

	g_pCoreShell->OperationRequest(GOI_AUTO_REPAIR, 0, m_AutoRepair->isSelected());
}

void KUiEntrustComputer::ClosePanel()
{
	const EventArgs args;
	btnClose_MouseClick(args);
}

bool KUiEntrustComputer::OnTimeTextChanged(const CEGUI::EventArgs & args)
{
	if (NULL == m_BeginAutoAttack || NULL == m_StopAutoAttack)
	{
		return false;
	}
	
	StopAutoAttack(1);
	m_BeginAutoAttack->show();
	m_StopAutoAttack->hide();

	m_bIsSettingsChanged = true;
	return true;
}