/************************************************************************/
/* 此文件实现封神宝典窗口功能，包括今日焦点、任务记录和操作帮助                                                                     */
/************************************************************************/
#include "UiFSBible.h"
#include "UiFSBible_SpecialQuestData.h"
#include "UiFSBible_QuestData.h"
#include "Coreshell.h"
#include "../UiSheetMgr.h"
#include "../UiConfigManager.h"
#include "../UiAdapter.h"
#include "../KMessageCentre.h"
#include "UiChatWindow.h"
#include "UiComMsgBox.h"
#include "UiQuestTrack.h"
#include "CoreUseNameDef.h"
#include "UiTipGenerator.h"
#include "UiLinkedItemTip.h"
#include <algorithm>    
using namespace std;
using namespace CEGUI;
#include "UiErrorMessageBox.h"
typedef UpdateInfoMap::iterator CIT; 
typedef pair<CIT, CIT> Range;
const int YearAndMonthStringLen = 20;
const int BeginNum				= -1;
extern iCoreShell* g_pCoreShell;
const string	TREE_LAY			= "Lay";
const string	TREE_LEAF			= "Leaf";
const string	TREE_LEAF_COUNT		= "LeafCount";
const int		TREE_FLAG			= 1;
const int		TREE_PARENT_INFO	= 0;
const int		TREE_WORD_INFO		= 1;
const int		TREE_DESC_INFO		= 2;
const int		TREE_STEP_NUM		= 10;

KUiFSBible& KUiFSBible::getSingleton()
{
	static KUiFSBible singleton;
	return singleton;
}

KUiFSBible::KUiFSBible()
{
	d_selQuestId = QUEST_INVALID_ID;
	_specialQuestCurPage = 0;
	m_QuestNum = 0;
	m_IsLoadList = false;
	m_isUsed = true;

	d_pHelp = NULL;
	d_pHelpMultiEdit = NULL;
	d_pHelpTreeScrol = NULL;
	d_pHelpMulEdScrol = NULL;
	d_pHelpTreeParent = NULL;
	d_bItem = false;
	d_bHelpTip = false;

	m_UpdateInfoFrame = NULL;
	m_UpdateInfoText = NULL;
	m_UpdateInfoScroll = NULL;
	m_updateTreeSroll		= NULL;
	m_updateTree			= NULL;
	m_FileSize = 0;
	m_isUpdated				= false;
	loadUi();

	for (int i = 0; i < UI_FSBIBLE_MAX_QUEST_COUNT; i++)
	{
		m_oldQuestList[i].questId = QUEST_INVALID_ID;
		m_oldQuestList[i].questName[0] = 0;
		m_oldQuestList[i].questTypeName[0] = 0;
	}


}

KUiFSBible::~KUiFSBible()
{
	//DestryHelpTree();
}

void KUiFSBible::LoadUpdateInfoText()
{
	KIniFile ini;
	if(ini.Load( CONFIG_INI ) == false)
		return;
	int lastInfoCount = 0;
	ini.GetInteger("UpdateInfoCount", "Count", 0, &lastInfoCount);

	int index		= BeginNum;
	int currentMessage = ( atoi ( KMessageCentre::GetMessageSafe( UI_FSBIBLE_UPDATE_INFO_MSG_TYPE, index++ ) ) - 2 ) / 2;
	
	ini.WriteInteger( "UpdateInfoCount", "Count", currentMessage );
	ini.Save( CONFIG_INI );

	if( currentMessage > lastInfoCount )
		m_isUpdated = true;

	m_yearString	= ( KMessageCentre::GetMessageSafe( UI_FSBIBLE_UPDATE_INFO_MSG_TYPE, index++ ) );
	m_monthString	= ( KMessageCentre::GetMessageSafe( UI_FSBIBLE_UPDATE_INFO_MSG_TYPE, index++ ) );
	m_dayString		= ( KMessageCentre::GetMessageSafe( UI_FSBIBLE_UPDATE_INFO_MSG_TYPE, index++ ) );
	for ( int i = 0; i < currentMessage; ++i )
	{
		string updateInfo = ( KMessageCentre::GetMessageSafe( UI_FSBIBLE_UPDATE_INFO_MSG_TYPE, index++ ) );
		if ( updateInfo.empty() )
			break;
		int year	= 0;
		int month	= 0;
		int day		= 0;
		sscanf( updateInfo.c_str() , "%d,%d,%d", &year, &month, &day );
		if( year == 0 || month == 0 || day == 0)
			throw( "Incorrect string format" );
		m_mapKeyArray.push_back( make_pair< int , int >( year, month ) );
		updateInfo = ( KMessageCentre::GetMessageSafe( UI_FSBIBLE_UPDATE_INFO_MSG_TYPE, index++ ) );
		if( updateInfo.empty() )
			continue;
		m_updateInfoMap.insert( UpdateInfoMap::value_type( make_pair< int , int >( year, month ), make_pair< int, string >( day, updateInfo ) ) );
	}
	sort( m_mapKeyArray.begin(), m_mapKeyArray.end() );
	m_mapKeyArray.erase( unique( m_mapKeyArray.begin(), m_mapKeyArray.end() ), m_mapKeyArray.end() );
}

bool KUiFSBible::onUpdateTreeItemMouseDown(const EventArgs& args)
{
		
	if( !m_UpdateInfoScroll || !m_UpdateInfoText || !m_updateTree || !m_UpdateInfoFrame )
		return false;
	TLTreeItem* pUpdateItem = (TLTreeItem *)( m_updateTree->getFirstSelectedItem() );

	if ( pUpdateItem == NULL )
	{
		if ( !d_bItem )
		{
			m_UpdateInfoText->getLayout()->clearLayout();
		}
		d_bItem = false;
		return false;
	}
	string* text = ( string* ) pUpdateItem->getUserData();
	m_UpdateInfoScroll->setScrollPosition(0.0f);
	m_UpdateInfoText->setLayoutOffset( 0, 0 );
	Point pos = m_UpdateInfoText->getPosition(Absolute);
	pos.d_y	 = m_UpdateInfoFrame->getPosition(Absolute).d_y;
 	LORect pannelclipper;
 	pannelclipper.setWidth( m_UpdateInfoFrame->getWidth( Absolute ) );
 	pannelclipper.setHeight( m_UpdateInfoText->getHeight( Absolute ) );
	pannelclipper.setPos( pos.d_x, pos.d_y  );
 	m_UpdateInfoText->getLayout()->setClipper( pannelclipper );
	if( text )
		m_UpdateInfoText->getLayout()->SetText( const_cast<char*>(text->c_str()) );	
	m_UpdateInfoText->getLayout()->flashLayout();
	d_bItem = true;
	m_UpdateInfoText->enable();
	return false;
}

bool KUiFSBible::onUpdateTreeBrance(const EventArgs& args)
{
	if( !m_updateTree || !m_UpdateInfoFrame || !m_updateTreeSroll )
		return false;
	float fTree_height    =  m_updateTree->getTreeTotalItemsHeigh();
	float fClipper_height = m_UpdateInfoFrame->getHeight(Absolute);
	
	if (fTree_height > fClipper_height) 
	{
		m_updateTreeSroll->setScrollPosition(0);
	}
	else 
	{
		m_updateTreeSroll->setScrollPosition(1);
	}
	return true;
}

bool KUiFSBible::onUpdateTreeWheelChanged(const EventArgs& args)
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if( m_updateTreeSroll && m_updateTreeSroll->isVisible() && d_pHelpTreeScrol )
	{ 
		float currPos = d_pHelpTreeScrol->getScrollPosition();
		float step =	( d_pHelpTreeScrol->getStepSize() );
		float wheel = eventArgs->wheelChange;
		step *=  wheel;
		m_updateTreeSroll->setScrollPosition( currPos - step );
	}
	return true;
}

bool	KUiFSBible::onUpdateContentScrol( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	if ( m_UpdateInfoScroll && scrollCtrl->window == m_UpdateInfoScroll && m_UpdateInfoText ) 
	{
		int layoutHeight = m_UpdateInfoText->getLayout()->getRenderArea().getHeight();
		int clipperHeight = m_UpdateInfoText->getHeight(Absolute);
		
		if( layoutHeight <= clipperHeight )
			return false;
		
		float scrollPos = m_UpdateInfoScroll->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;
		
		m_UpdateInfoText->setLayoutOffset( 0, -yPos );
	}
	return true;
}

bool KUiFSBible::onUpdateTreeScroll(const EventArgs& args)
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	if( !m_updateTree || !m_UpdateInfoFrame || !m_updateTreeSroll || !scrollCtrl )
		return false;
	float sparef = m_updateTree->getTreeTotalItemsHeigh();
	float clipper = m_UpdateInfoFrame->getHeight(Absolute);
	float yPos = 0;
	
	if ( scrollCtrl->window == m_updateTreeSroll )
	{	
		float scrollPos = m_updateTreeSroll->getScrollPosition();
		
		if ( (sparef > clipper ) )
		{
			yPos = (sparef - clipper) * scrollPos;
			Point pos;
			pos.d_x = m_updateTree->getPosition(Absolute).d_x;
			pos.d_y = 0 - yPos;
			m_updateTree->setPosition( Absolute, pos );
		}
		else
		{
			m_updateTree->setPosition( Absolute, m_pos );
		}
	}
	return false;
	
}

CreateTreeFromMultiMap::CreateTreeFromMultiMap( TLTree* treeRoot,  UpdateInfoMap* pMap, string& year, string& month, string& day)
{
	m_map			= pMap;
	m_treeRoot		= treeRoot;
	m_parentItem	= NULL;
	m_yearString	= year;
	m_monthString	= month;
	m_dayString		= day;
}
void CreateTreeFromMultiMap::operator()( pair< int,int >& data)
{
	if( !m_map || !m_treeRoot )
		return;
	char yearAndMonth[YearAndMonthStringLen] = { 0 };
	sprintf( yearAndMonth, "%d%s%d%s", data.first, m_yearString.c_str(), data.second, m_monthString.c_str() );
	m_parentItem =  new TLTreeItem( AnsiToUtf8( yearAndMonth ) ); 
	m_treeRoot->addItem( m_parentItem );
	Range range = m_map->equal_range( data );
	vector<CIT> itArray;
	for ( CIT i = range.first; i != range.second; ++i)
		itArray.push_back( i );		
	std::sort( itArray.begin(), itArray.end(), SortRange());
	for( vector<CIT>::iterator it = itArray.begin(); it != itArray.end(); ++it)
	{
		string day( iToString( ( *it)->second.first ).c_str() );
		day += m_dayString;
		string* content = &( ( *it)->second.second );
		TLTreeItem* item = new TLTreeItem( AnsiToUtf8( day.c_str() ) ); 
		m_parentItem->addItem( item );
		if( content )
			item->setUserData( ( void* )content );
	}
}

void KUiFSBible::InitializeUpdateTree()
{
	if( !m_updateTree )
		return ;
	m_updateTree->setSortingEnabled( false ); //树内容不排序
	CreateTreeFromMultiMap functor( m_updateTree, &m_updateInfoMap, m_yearString, m_monthString, m_dayString);
	for_each( m_mapKeyArray.rbegin(), m_mapKeyArray.rend(), functor);
}

void KUiFSBible::LoadUpdateInfoFrame()
{
#ifndef _DEBUG
	try
#endif
	{
		TLButton * updateInfo	=	static_cast<TLButton *>(_thisWindow->getChild("TaharezLook/FSBible/UpdateInfo"));
		updateInfo->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiFSBible::btnUpdateInfo_MouseClick, this));
		m_UpdateInfoFrame		=	static_cast<TLStaticImage *>(_thisWindow->getChild("TaharezLook/FSBible/UpdateInfoFrame"));
		m_UpdateInfoScroll		=	static_cast<TLVertScrollbar*>(m_UpdateInfoFrame->getChild( "TaharezLook/FSBible/UpdateInfoFrame/Scrollbar" ));
		m_UpdateInfoScroll->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiFSBible::onUpdateContentScrol, this));
		m_UpdateInfoText		=	static_cast<TLStaticText*>(m_UpdateInfoFrame->getChild( "TaharezLook/FSBible/UpdateInfoFrame/Text" ) );
		m_UpdateInfoText->useLayout();
		m_updateTree			=	static_cast<TLTree *>(m_UpdateInfoFrame->getChild("TaharezLook/FSBible/UpdateInfoFrame/TextPanel")->getChild("TaharezLook/FSBible/UpdateInfoFrame/TextPanel/Tree"));;
		m_updateTree->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiFSBible::onUpdateTreeItemMouseDown, this));	
		m_updateTree->subscribeEvent(Tree::TR_EventBranchOpened, Event::Subscriber(&KUiFSBible::onUpdateTreeBrance, this));
		m_updateTree->subscribeEvent(Tree::TR_EventBranchClosed, Event::Subscriber(&KUiFSBible::onUpdateTreeBrance, this));
		m_updateTree->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiFSBible::onUpdateTreeWheelChanged, this));
		m_updateTreeSroll		=	static_cast<TLVertScrollbar *>(m_UpdateInfoFrame->getChild( "TaharezLook/FSBible/UpdateInfoFrame/TextPanel" )->getChild("TaharezLook/FSBible/UpdateInfoFrame/TextPanel/TreeScrollbar"));
		m_updateTreeSroll->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiFSBible::onUpdateTreeScroll, this));
		m_pos = m_updateTree->getPosition( Absolute );
		LoadUpdateInfoText();
		InitializeUpdateTree();
	}
#ifndef _DEBUG
	catch (...)
	{
		m_UpdateInfoFrame	= NULL;
		m_UpdateInfoScroll	= NULL;
		m_updateTree		= NULL;
		m_UpdateInfoText	= NULL;
		m_updateTreeSroll	= NULL;
	}
#endif
}

void KUiFSBible::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_FSBIBLE_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_FSBIBLE_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->hide();

	TLButton* closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/FSBible/Close");
	closeBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiFSBible::clickClose, this));
	
	todayMsgBtn = (TLRadioButton*)_thisWindow->getChild("TaharezLook/FSBible/TodayMsg");
	todayMsgBtn->subscribeEvent(TLRadioButton::EventMouseClick, Event::Subscriber(&KUiFSBible::clickTodayMsgPanelBtn, this));

	questBtn = (TLRadioButton*)_thisWindow->getChild("TaharezLook/FSBible/Quest");
	questBtn->subscribeEvent(TLRadioButton::EventMouseClick, Event::Subscriber(&KUiFSBible::clickQuestPanelBtn, this));
	
	//////操作帮助 zhangxin
	InitHelp();
	///////


	//今日信息面板
	_todayPanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/FSBible/TodayPanel");
	_todayPanel->show();

	TLStaticImage* qTodayPanel = (TLStaticImage*)_todayPanel->getChild("TaharezLook/FSBible/TodayPanel/QuestListPanel");	
	TodayMsgTip= static_cast<TLCheckbox *>(qTodayPanel->getChild("TaharezLook/FSBible/TodayPanel/QuestListPanel/TodayMsgTip"));
	TodayMsgTip->subscribeEvent(TLCheckbox::EventMouseClick, Event::Subscriber(&KUiFSBible::onLoginAutoShow, this));
	TodayMsgTip->setSelected(true);	


//	TLButton* specialQuestBtn = (TLButton*)questListPanel->getChild("TaharezLook/FSBible/TodayPanel/QuestListPanel/SpecialQuest");
//	specialQuestBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiFSBible::showSpecialQuest, this));

	//普通任务面板
	_questPanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/FSBible/QuestPanel");
	_questPanel->hide();
	//任务列表
	TLStaticImage* questListPanel = (TLStaticImage*)_questPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestListPanel");	
	TLStaticImage* questListClipper = (TLStaticImage*)questListPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestListPanel/Clipper");
	_questList = (TLTree*)questListClipper->getChild("TaharezLook/FSBible/QuestPanel/QuestListPanel/Clipper/QuestList");
	_questList->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiFSBible::selectQuest, this));
	_questList->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiFSBible::onListWheelChanged, this));
	d_questListMaxHeight = questListClipper->getAbsoluteHeight();
	
	d_questListPanelScrollBar = (TLVertScrollbar*)questListPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestListPanel/Scrollbar");
	d_questListPanelScrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiFSBible::onQuestListPanelScroll, this));


	//普通任务详细显示面板
	_normalQuestPanel = (TLStaticImage*)_questPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel");
	_normalQuestPanel->show();

	d_questInfoPanelScrollBar = (TLVertScrollbar*)_normalQuestPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Scrollbar");
	d_questInfoPanelScrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiFSBible::onQuestInfoPanelScroll, this));

	d_questInfoClipper = (TLStaticImage*)_normalQuestPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper");
	d_questInfoClipper->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiFSBible::onQuestInfoWheelChanged, this));
	d_questInfoMaxHeight = d_questInfoClipper->getAbsoluteHeight();

	d_questInfoPanel = (TLStaticImage*)d_questInfoClipper->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel");
	
	d_questText = (TLStaticText*)d_questInfoPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestText");
	d_questText->subscribeEvent(TLStaticImage::EventMouseClick, Event::Subscriber(&KUiFSBible::onClickQuestInfo, this));
	d_questText->subscribeEvent(TLStaticImage::EventMouseDoubleClick, Event::Subscriber(&KUiFSBible::onClickQuestInfo, this));
	d_questText->subscribeEvent(TLStaticImage::EventMouseMove, Event::Subscriber(&KUiFSBible::onHoverText, this));
	d_questText->subscribeEvent(TLStaticImage::EventMouseLeaves, Event::Subscriber(&KUiFSBible::onLeaveText, this));
	d_questText->setMetricsMode(Absolute);
	d_questText->useLayout();
	//设置排版相对于控件的偏移（主要处理有边框的情况）
	if(d_questText->isFrameEnabled())
	{
		d_questTextLayoutWidth = d_questText->getUnclippedInnerRect().getWidth()
			- d_questText->getLeftFrameWidth() - d_questText->getRightFrameWidth();
		d_questText->setLayoutOffset(d_questText->getLeftFrameWidth(), d_questText->getTopFrameHeight());
	}
	else
	{		
		d_questTextLayoutWidth = d_questText->getUnclippedInnerRect().getWidth();
		d_questText->setLayoutOffset(0, 0);
	}

	d_questRewardPanel = (TLStaticImage*)d_questInfoPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestRewardPanel");
	d_questRewardMoneyText = (TLStaticText*)d_questRewardPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestRewardPanel/Money");
	d_rewardMsgText = (TLStaticText*)d_questRewardPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestRewardPanel/RewardMsg");
	d_itemFrameTemplate = (TLStaticImage*)d_questRewardPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestRewardPanel/ItemFrame");
	d_itemFrameTemplate->hide();
	d_questRewardMoneyText->useLayout();

	d_rewardSelectMsgText = (TLStaticText*)d_questRewardPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestRewardPanel/RewardSelectMsg");
	String rewardCtrlPath = "TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Clipper/Panel/QuestRewardPanel";
	char ctrlName[COMMON_CLIENT_MSG_LEN_128];
	for(int j = 0; j < MAX_QUEST_REWARD_ITEM; ++j)
	{
		//固定奖励
		sprintf(ctrlName, "/Item%d", j + 1);
		d_itemImage[j] = (TLGameObject*)d_questRewardPanel->getChild(rewardCtrlPath + ctrlName);
		
		KObjAtContRegion* newObjInfo = &d_questCtrlUserData[j];
		newObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_itemImage[j]->setUserData(newObjInfo);

		d_itemImageGrid[j].setCtrl(d_itemImage[j]);
		d_itemImageGrid[j].addTip();

		//可选奖励
		sprintf(ctrlName, "/ChoiceItem%d", j + 1);
		d_selectItemImage[j] = (TLGameObject*)d_questRewardPanel->getChild(rewardCtrlPath + ctrlName);
		
		KObjAtContRegion* newSelectObjInfo = &d_selectCtrlUserData[j];
		newSelectObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_selectItemImage[j]->setUserData(newSelectObjInfo);

		d_selectItemImageGrid[j].setCtrl(d_selectItemImage[j]);
		d_selectItemImageGrid[j].addTip();

		d_itemFrame[j] = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
		useTemplate(d_itemFrame[j], d_itemFrameTemplate);
		d_questRewardPanel->addChildWindow(d_itemFrame[j]);
		d_itemFrame[j]->setXPosition(Absolute, d_itemImage[j]->getXPosition(Absolute) 
			- (d_itemFrame[j]->getWidth(Absolute) - d_itemImage[j]->getWidth(Absolute)) / 2);
		d_itemFrame[j]->setZLevel(Window::Bottom);
		
		d_selectItemFrame[j] = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
		useTemplate(d_selectItemFrame[j], d_itemFrameTemplate);
		d_questRewardPanel->addChildWindow(d_selectItemFrame[j]);
		d_selectItemFrame[j]->setXPosition(Absolute, d_selectItemImage[j]->getXPosition(Absolute)
			- (d_selectItemFrame[j]->getWidth(Absolute) - d_selectItemFrame[j]->getWidth(Absolute)) / 2);
		d_selectItemFrame[j]->setZLevel(Window::Bottom);
	}

	TLButton* trackQuestBtn = (TLButton*)_normalQuestPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Track");
	trackQuestBtn->subscribeEvent(TLButton::EventMouseClick,	Event::Subscriber(&KUiFSBible::onTrackQuest, this));
	trackQuestBtn->setZLevel(Window::Top);
	TLButton* delQuestBtn = (TLButton*)_normalQuestPanel->getChild("TaharezLook/FSBible/QuestPanel/QuestInfoPanel/Del");
	delQuestBtn->subscribeEvent(TLButton::EventMouseClick,	Event::Subscriber(&KUiFSBible::onDeleteQuest, this));
	delQuestBtn->setZLevel(Window::Top);

	//特殊任务面板
	_specialQuestPanel = (TLStaticImage*)_todayPanel->getChild("TaharezLook/FSBible/TodayPanel/SpecilaQuestInfoPanel");
	_specialQuestPanel->show();
	TLStaticImage* specialQuestList = (TLStaticImage*)_specialQuestPanel->getChild("TaharezLook/FSBible/TodayPanel/SpecilaQuestInfoPanel/QuestList");

	Point pos(0, 0);
	for(int i = 0; i < UI_FSBIBLE_MAX_SPECIAL_QUEST_COUNT_PER_PAGE; ++i)
	{
		TLStaticImage* questItem = _specialQuestItem[i]._thisWindow;
		specialQuestList->addChildWindow(questItem);

		questItem->setPosition(Absolute, pos);
		pos.d_y += questItem->getHeight(Absolute);
		
		questItem->setRenderMode(true);
	}
	
	_prevBtn = (TLButton*)_specialQuestPanel->getChild("TaharezLook/FSBible/TodayPanel/SpecilaQuestInfoPanel/Prev");
	_prevBtn->subscribeEvent(TLButton::EventMouseClick, 
		Event::Subscriber(&KUiFSBible::onPrevPage, this));
	_prevBtn->setZLevel(Window::Top);
	_nextBtn = (TLButton*)_specialQuestPanel->getChild("TaharezLook/FSBible/TodayPanel/SpecilaQuestInfoPanel/Next");
	_nextBtn->subscribeEvent(TLVertScrollbar::EventMouseClick, 
		Event::Subscriber(&KUiFSBible::onNextPage, this));
	_nextBtn->setZLevel(Window::Top);

	LoadUpdateInfoFrame();
}

bool KUiFSBible::clickClose(const EventArgs& args)
{
	hide();
	return false;
}

void KUiFSBible::showSpecialPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	KUiFSBibleSpecialQuestData::getSingleton().freshUi();
	
	loadSpecialQuestInfo();
	_thisWindow->show();
	hideAllPanel();
	_todayPanel->show();
}

void KUiFSBible::showSpecialPanelIfHave()
{
	if(!_thisWindow)
	{
		return;
	}

	//Add By DarkMagic
	KIniFile configIni;
	configIni.Load(CONFIG_INI);

	int isShow = 1;
	int textSize = 0;
	unsigned long updateInfoTextSize = 0;
	configIni.GetInteger("GameSetting", "LoginBibleAutoShow", 1, &isShow);
	configIni.GetInteger("GameSetting", "UpdateInfoSize", 0, &textSize);

	updateInfoTextSize = textSize;

	if (isShow == 0)
	{
		TodayMsgTip->setSelected(false);
	}
	//End Add
	
	int count = KUiFSBibleSpecialQuestData::getSingleton().freshUi();
	if(count && TodayMsgTip && TodayMsgTip->isSelected() )
	{
		loadSpecialQuestInfo();
		_thisWindow->show();
		hideAllPanel();
		_todayPanel->show();
	}
	else
	{
		hide();
	}

	if (!m_IsLoadList)
	{
		FirstGetQuestList();
		m_IsLoadList = true;
	}

	const time_t curTime_T = time(NULL);
	tm * curTime = localtime(&curTime_T);
	if (curTime->tm_wday == 2 && m_UpdateInfoFrame != NULL ||
		m_FileSize != updateInfoTextSize)
	{
		ShowUpdateInfoPanel();

		if (m_FileSize != updateInfoTextSize)
		{
			configIni.WriteInteger("GameSetting", "UpdateInfoSize", updateInfoTextSize);
			configIni.Save(CONFIG_INI);
		}
	}
	if ( m_isUpdated )
	{
		ShowUpdateInfoPanel();
	}
}

void KUiFSBible::show()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
	loadSpecialQuestInfo();
	KUiFSBibleSpecialQuestData::getSingleton().freshUi();
	
	if(_questPanel->isVisible())
	{
		showQuestPanel();
	}

}



void KUiFSBible::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
/*	if (m_isUsed)
	{
		if (!m_IsLoadList)
		{
			FirstGetQuestList();
			m_IsLoadList = true;
		}
	}*/
}

void KUiFSBible::toggle()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_thisWindow->isVisible())
	{
		hide();
	}
	else
	{
		show();
	}
}

void KUiFSBible::toggleToday()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_thisWindow->isVisible() && _todayPanel->isVisible())
	{
		hide();
	}
	else
	{		
		todayMsgBtn->setSelected(true);
		showSpecialPanel();
	}
}

void KUiFSBible::toggleQuest()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_thisWindow->isVisible() && _questPanel->isVisible())
	{		
		hide();
	}
	else
	{
		questBtn->setSelected(true);
		showQuestPanel();
	}
}

void KUiFSBible::loadSpecialQuestInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	KUiFSBibleSpecialQuestData::getSingleton().sendDataReq();
}

void KUiFSBible::updateSpeicalQuestData(vector<SpecialQuestData>& data)
{
	if(!_thisWindow)
	{
		return;
	}

	_specialQuestPageCount = (data.size() - 1) / UI_FSBIBLE_MAX_SPECIAL_QUEST_COUNT_PER_PAGE + 1;
	for(int i = 0; i < UI_FSBIBLE_MAX_SPECIAL_QUEST_COUNT_PER_PAGE; ++i)
	{
		int index = _specialQuestCurPage * UI_FSBIBLE_MAX_SPECIAL_QUEST_COUNT_PER_PAGE + i ;
		if(index >= data.size())
		{
			_specialQuestItem[i].clear();
		}
		else
		{
			_specialQuestItem[i].setContent(data[index]);
		}
	}

	if(_specialQuestCurPage <= 0)
	{
		_prevBtn->disable();
	}
	else
	{
		_prevBtn->enable();
	}

	if(_specialQuestCurPage >= _specialQuestPageCount - 1)
	{
		_nextBtn->disable();
	}
	else
	{
		_nextBtn->enable();
	}
}

void KUiFSBible::hideAllPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	if (m_UpdateInfoFrame != NULL &&
		_todayPanel != NULL &&
		_questPanel != NULL &&
		_helpPanel != NULL)
	{
		m_UpdateInfoFrame->hide();
		_todayPanel->hide();
		_questPanel->hide();
		_helpPanel->hide();//zhangxin
	}
}

bool KUiFSBible::clickTodayMsgPanelBtn(const EventArgs& args)
{
	hideAllPanel();
	_todayPanel->show();
	return true;
}

bool KUiFSBible::clickQuestPanelBtn(const EventArgs& args)
{
	showQuestPanel();

	return true;
}


/*
说明：当点击操作帮助单选钮时，隐藏其他窗口，只显示操作帮助窗口 zhangxin     
*/
bool KUiFSBible::onHelpClickPanelBtn(const EventArgs& args)
{
	hideAllPanel();
	if( !d_pHelpMulEdScrol || !d_pHelpTreeParent || !d_pHelpTreeScrol || !_helpPanel || !d_pHelpMultiEdit )
		return false;
	d_pHelpMulEdScrol->show();
	d_pHelpTreeParent->show();
	d_pHelpTreeScrol->show();
	d_pHelpMultiEdit->show();
	_helpPanel->enable();
	_helpPanel->show();
	return true;
}

void KUiFSBible::showQuestPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
	hideAllPanel();
	_questPanel->show();
	flashQuestList();
	//第一次打开窗口的时候显示一个任务
	if(!selQuestValidate())
	{
		d_selQuestId = _questListData[0].questId;
	}
	
	showSelQuest();
}

void KUiFSBible::useTemplate(StaticImage* wnd, StaticImage* templateWnd)
{
	wnd->setHeight(Absolute, templateWnd->getHeight(Absolute));
	wnd->setWidth(Absolute, templateWnd->getWidth(Absolute));
	wnd->setXPosition(Absolute, templateWnd->getXPosition(Absolute));

	wnd->setBackgroundEnabled(false);
	wnd->setFrameEnabled(false);
	wnd->disable();
	wnd->setImage(templateWnd->getImage());	
}


bool KUiFSBible::onPrevPage( const EventArgs& args )
{
	--_specialQuestCurPage;
	KUiFSBibleSpecialQuestData::getSingleton().freshUi();
	return true;
}

bool KUiFSBible::onNextPage( const EventArgs& args )
{
	++_specialQuestCurPage;
	KUiFSBibleSpecialQuestData::getSingleton().freshUi();
	return true;
}

void KUiFSBible::showSelQuest()
{
	if(!_thisWindow)
	{
		return;
	}

	hideQuestInfo();

	if(QUEST_INVALID_ID == d_selQuestId)
	{
		return;
	}

	KQuestInfo questInfo;
	g_pCoreShell->GetGameData(GDI_GET_QUEST_INFO, (UINT)&questInfo, d_selQuestId);

//	char loTempStr[COMMON_CLIENT_MSG_LEN_128];

//	strcpy(questInfo.aim, "<Obj type=pic des=1234567 gotype=npc>set:face image:wunai</Obj><Obj type=text vertical-align=bottom >杀掉赵兄\n</Obj>");
	sprintf(d_lomsg, 
		"<Layout width=%d>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"<Obj>%s</Obj>"
			"</Seg>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"%s"
			"</Seg>"
			"<Seg f=wrap>"
				"<Obj t=text> </Obj>"
			"</Seg>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"<Obj>%s</Obj>"
			"</Seg>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"%s"
			"</Seg>"
			"<Seg float=wrap>"
				"<Obj v-a=bottom> </Obj>"
			"</Seg>",
			d_questTextLayoutWidth, 

			KUiCfgLoader::getSingleton().getQuestData().descriptionTitleFont,
			KUiCfgLoader::getSingleton().getQuestData().descriptionTitleColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().descriptionTitleText,

			KUiCfgLoader::getSingleton().getQuestData().descriptionFont,
			KUiCfgLoader::getSingleton().getQuestData().descriptionColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,

			questInfo.description,

			KUiCfgLoader::getSingleton().getQuestData().aimTitleFont,
			KUiCfgLoader::getSingleton().getQuestData().aimTitleColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().aimTitleText,

			KUiCfgLoader::getSingleton().getQuestData().aimFont,
			KUiCfgLoader::getSingleton().getQuestData().aimColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,

			
			questInfo.aim);
	
	//max_objective_type=任务目标类别数，MAX_OBJECTIVE=每个类别的最大需求数
	//一个任务可能有最多max_objective_type种任务目标，例如：打怪和NPC对话等，
	//每种任务目标可能有MAX_OBJECTIVE种不同的需求，例如：杀死A xx个再杀死B yy个，然后再找C对话
	for(int i = 0; i < max_objective_type; ++i)
	{
		for(int j = 0; j < MAX_OBJECTIVE; ++j)
		{
			KQuestInfo::ObjectiveInfo& npcRequire = questInfo.requirement[i][j];
			
			if(npcRequire.uID == QUEST_INVALID_ID)
			{
				continue;
			}
//去掉
// 			strcat(d_lomsg, "<Seg f=wrap><Obj f-f=");
// 			strcat(d_lomsg, KUiCfgLoader::getSingleton().getQuestData().requestFont);
// 			strcat(d_lomsg, " wc=");
// 			
// 			if(npcRequire.uCount > npcRequire.uProcess)
// 			{
// 				strcat(d_lomsg, KUiCfgLoader::getSingleton().getQuestData().requestNormalColor);
// 			}
// 			else
// 				
// 			{
// 				strcat(d_lomsg, KUiCfgLoader::getSingleton().getQuestData().requestCompleteColor);
// 			}
// 
// 			sprintf(loTempStr, ">%s", npcRequire.name);
// 			strcat(d_lomsg, loTempStr);
// 			strcat(d_lomsg, "</Obj></Seg>");
		}
	}
	strcat(d_lomsg, "</Layout>");

	d_questText->getLayout()->formatText(d_lomsg);
	d_questText->getLayout()->SetText(d_lomsg);
	d_questText->getLayout()->flashLayout();
	d_questText->fitLayoutSize();
	
	Rect textArea = d_questText->getUnclippedInnerRect();

	//裁剪区域必须是相对底板的位置
	Vector2 posOff = textArea.getPosition() - _thisWindow->getUnclippedPixelRect().getPosition();
 	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	clipper.setHeight(d_questInfoMaxHeight);
	d_questText->getLayout()->setClipper(clipper);

	d_questText->show();

	//任务奖励
	d_questRewardPanel->show();
	bool haveReward = false;
	//金钱和经验
// 	if(0 != questInfo.rewardExp)
// 	{
// 		haveReward = true;
// 	}
// 	if(0 !=questInfo.rewardMoney)
// 	{
// 		haveReward = true;
// 	}
// 	if(haveReward)
// 	{
// 		showMoneyExp(questInfo.rewardMoney, questInfo.rewardExp);
// 	}
	//奖励
	KReward* reward = KUiFSBibleQuestData::getSingleton().getQuestRewardById(d_selQuestId);
	int rewardImageIndex = 0;
	if(reward)
	{
		for(int k = 0; k < MAX_REWARD_ITEM; ++k)
		{
			if(reward->item[k].genre == 0
				&& reward->item[k].detail == 0
				&& reward->item[k].particular == 0
				&& reward->item[k].level == 0)
			{
				continue;
			}
			
			FIND_ITEMINDEX_PARAM itemidx;
			itemidx.nGenre = reward->item[k].genre;
			itemidx.nDetail = reward->item[k].detail;
			itemidx.nParticular = reward->item[k].particular;
			itemidx.nLevel = reward->item[k].level;
			
			KItemInfo itemInfo;	
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&itemidx, (int)&itemInfo);
			
			TLGameObject::GameObject goInfo;
			goInfo.d_type			= TLGameObject::item;
			goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
			goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
			goInfo.d_count			= reward->itemCount[k];
			d_itemImage[rewardImageIndex]->setObject( goInfo );
			d_itemImage[rewardImageIndex]->show();
			
			KObjAtContRegion* objInfo = (KObjAtContRegion*)d_itemImage[rewardImageIndex]->getUserData();
			objInfo->Obj.uGenre = CGOG_ICON;
			objInfo->Region.h = itemidx.nGenre;
			objInfo->Region.v = itemidx.nDetail;
			objInfo->Region.Width = itemidx.nParticular;
			objInfo->Region.Height = itemidx.nLevel;
			haveReward = true;
			
			++rewardImageIndex;
		}

		for(int l = 0; l < MAX_OTHER_REWARD; ++l)
		{
			if(rewardImageIndex > MAX_QUEST_REWARD_ITEM)
			{
				break;
			}
			
			int rewardTypeId = reward->otherRewardIds[l];
			KRewardType* rewardType = KUiFSBibleQuestData::getSingleton().getRewardTypeById(rewardTypeId);
			if(rewardType)
			{
				char imageSet[COMMON_CLIENT_MSG_LEN_64];
				char image[COMMON_CLIENT_MSG_LEN_64];
				if(sscanf(rewardType->imagePath, "set:%s image:%s", imageSet, image) != 2)
				{
					continue;
				}
				imageSet[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
				image[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
				
				TLGameObject::GameObject goInfo;
				goInfo.d_type			= TLGameObject::item;
				goInfo.d_gameobjectSet	= AnsiToUtf8(imageSet);
				goInfo.d_gameobject		= AnsiToUtf8(image);
				goInfo.d_count			= 1;
				d_itemImage[rewardImageIndex]->setObject( goInfo );
				d_itemImage[rewardImageIndex]->show();
				
				KObjAtContRegion* objInfo = (KObjAtContRegion*)d_itemImage[rewardImageIndex]->getUserData();
				objInfo->Obj.uGenre = CGOG_QUEST;
				objInfo->Region.h = rewardTypeId;
				haveReward = true;
				
				++rewardImageIndex;
			}
		}
	}
	//如果金钱、经验或者固定奖励三者有一个显示的话，就显示提示信息
	if(haveReward)
	{
		d_rewardMsgText->show();
	}

	if(false == haveReward)
	{
		d_questRewardPanel->hide();
	}
	d_questInfoPanel->show();
	
	layoutQuestInfo();
}

void KUiFSBible::hideQuestInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	//任务信息面版
	d_questInfoPanel->hide();
	d_questInfoPanel->setYPosition(Absolute, 0);
	//任务描述
	d_questText->hide();
	d_questInfoPanelScrollBar->hide();
	
	//任务奖励面版
	d_questRewardPanel->hide();
	
	d_rewardMsgText->hide();
	d_rewardSelectMsgText->hide();

	d_questRewardMoneyText->hide();

	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		d_itemImage[i]->hide();
		d_selectItemImage[i]->hide();
		d_itemFrame[i]->hide();
		d_selectItemFrame[i]->hide();
	}
}

void KUiFSBible::layoutQuestInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	//根据任务描述框高度来调整奖励面版的位置
	d_questRewardPanel->setYPosition(Absolute, 
		d_questText->getAbsoluteHeight() + d_questText->getAbsolutePosition().d_y
		+ UI_FSBIBLE_COMMON_CTRL_OFFSET);

	int rewardPannelHeight = UI_FSBIBLE_COMMON_CTRL_OFFSET;
	if(d_rewardMsgText->isVisible())
	{
		d_rewardMsgText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_rewardMsgText->getAbsoluteHeight() + UI_FSBIBLE_COMMON_CTRL_OFFSET;
	}
	
	//经验和金钱
	if(d_questRewardMoneyText->isVisible())
	{
		d_questRewardMoneyText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_questRewardMoneyText->getAbsoluteHeight() + UI_FSBIBLE_COMMON_CTRL_OFFSET;
	}

	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		if(d_itemImage[i]->isVisible())
		{
			if(i == 0)
			{
				d_itemImage[i]->setYPosition(Absolute, rewardPannelHeight);
				rewardPannelHeight = d_itemImage[i]->getYPosition(Absolute) + d_itemImage[i]->getAbsoluteHeight()
					 + UI_FSBIBLE_COMMON_CTRL_OFFSET;
			}
			else
			{
				d_itemImage[i]->setYPosition(Absolute, d_itemImage[0]->getYPosition(Absolute));
			}
			d_itemFrame[i]->setYPosition(Absolute, d_itemImage[i]->getYPosition(Absolute)
				- (d_itemFrame[i]->getHeight(Absolute) - d_itemImage[i]->getHeight(Absolute)) / 2);
			d_itemFrame[i]->show();
		}
	}
	
	if(d_rewardSelectMsgText->isVisible())
	{
		d_rewardSelectMsgText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_rewardSelectMsgText->getAbsoluteHeight() + UI_FSBIBLE_COMMON_CTRL_OFFSET;
	}

	for(int j = 0; j < MAX_QUEST_REWARD_ITEM; ++j)
	{
		if(d_selectItemImage[j]->isVisible())
		{
			if(j == 0)
			{
				d_selectItemImage[j]->setYPosition(Absolute, rewardPannelHeight + UI_FSBIBLE_COMMON_CTRL_OFFSET);
				rewardPannelHeight = d_selectItemImage[j]->getYPosition(Absolute) + d_selectItemImage[j]->getAbsoluteHeight();
			}
			else
			{
				d_selectItemImage[j]->setYPosition(Absolute, d_selectItemImage[0]->getYPosition(Absolute));
			}
			d_selectItemImage[j]->setYPosition(Absolute, d_selectItemImage[j]->getYPosition(Absolute)
				- (d_selectItemImage[j]->getHeight(Absolute) - d_selectItemImage[j]->getHeight(Absolute)) / 2);
			d_selectItemFrame[j]->show();
		}
	}

	d_questRewardPanel->setHeight(Absolute, rewardPannelHeight + UI_FSBIBLE_COMMON_CTRL_OFFSET);

	int ctrlHeight = d_questRewardPanel->getAbsoluteHeight() + d_questRewardPanel->getAbsolutePosition().d_y;
	
	//所有内容的高度
	d_questInfoPanel->setHeight(Absolute, ctrlHeight);
	d_questInfoPanel->setYPosition(Absolute, 0);

	//根据所有内容高度和控件高度来决定是否显示滑动条
	if(d_questInfoMaxHeight < ctrlHeight)
	{
		d_questInfoPanelScrollBar->show();
		d_questInfoPanelScrollBar->setScrollPosition(0);

		float step = (float)d_questInfoMaxHeight / (2 * (ctrlHeight - d_questInfoMaxHeight));
		d_questInfoPanelScrollBar->setStepSize(step);
	}
}

void KUiFSBible::showMoneyExp(int money, int exp)
{
	if(!_thisWindow)
	{
		return;
	}

	const KUiCfgLoader::QuestCfgData& questCfg = KUiCfgLoader::getSingleton().getQuestData();
	
	char tempText[COMMON_CLIENT_MSG_LEN_512] = "\0";
	char loText[MAX_TEXT_LEN] = "\0";

	sprintf(tempText, "<Layout width=%d>", d_questTextLayoutWidth);
	strcat(loText, tempText);
	
	if(exp > 0)
	{
		sprintf(tempText, "<Seg text-align=left float=wrap><Obj type=text color=%s font-family=%s vertical-align=center>%s%d</Obj></Seg>",
			questCfg.expColor, questCfg.expFont, questCfg.expText, exp);
		strcat(loText, tempText);
	}
	if(money > 0)
	{
		sprintf(tempText, "<Seg text-align=left><Obj type=text color=%s font-family=%s vertical-align=center>%s </Obj>",
			questCfg.moneyColor, questCfg.moneyFont, questCfg.moneyText);
		strcat(loText, tempText);
		
		if(money / 10000 > 0)
		{
			sprintf(tempText, "<Obj type=text color=%s font-family=%s vertical-align=center>%d </Obj>",
				questCfg.moneyColor,
				questCfg.moneyFont,
				money / 10000);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getJinImagePath());
			strcat(loText, tempText);
		}
		
		if(money % 10000 / 100 > 0)
		{
			sprintf(tempText, "<Obj type=text %s font-family=%s vertical-align=center>%d </Obj>", 
				questCfg.moneyColor,
				questCfg.moneyFont, 
				money % 10000 / 100);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getYinImagePath());
			strcat(loText, tempText);
		}
		
		if(money % 100 > 0)
		{
			sprintf(tempText, "<Obj type=text %s font-family=%s vertical-align=center>%d </Obj>", 
				questCfg.moneyColor,
				questCfg.moneyFont, 
				money % 100);
			strcat(loText, tempText);
			sprintf(tempText, "<Obj type=pic vertical-align=center>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
			strcat(loText, tempText);
		}
		strcat(loText, "</Seg>");
	}
	strcat(loText, "</Layout>");

	d_questRewardMoneyText->useLayout();
	d_questRewardMoneyText->getLayout()->SetText(loText);
	d_questRewardMoneyText->getLayout()->flashLayout();
	d_questRewardMoneyText->fitLayoutSize();

	Rect textArea = d_questInfoClipper->getUnclippedInnerRect();

	//裁剪区域必须是相对底板的位置
	Vector2 posOff = textArea.getPosition() - _thisWindow->getUnclippedPixelRect().getPosition();
 	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	clipper.setHeight(d_questInfoMaxHeight);

	d_questRewardMoneyText->getLayout()->setClipper(clipper);
	d_questRewardMoneyText->show();
}

bool KUiFSBible::onQuestInfoWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(d_questInfoPanelScrollBar->isVisible())
	{
		d_questInfoPanelScrollBar->setScrollPosition(d_questInfoPanelScrollBar->getScrollPosition()
			- d_questInfoPanelScrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiFSBible::onQuestListPanelScroll(const CEGUI::EventArgs& args)
{
	float scrollPos = d_questListPanelScrollBar->getScrollPosition();
	int ctrlHeight = _questList->getTreeTotalItemsHeigh();
	int exceedSize = (ctrlHeight - d_questListMaxHeight) * scrollPos;
	_questList->setYPosition(Absolute, -exceedSize);
	return true;
}

void KUiFSBible::ClearQuestList()
{
	if(!_thisWindow)
	{
		return;
	}

	for (int i = 0; i < UI_FSBIBLE_MAX_QUEST_COUNT; i++)
	{
		_questListData[i].questId = QUEST_INVALID_ID;
		_questListData[i].questName[0] = 0;
		_questListData[i].questTypeName[0] = 0;
	}

	for (i = 0; i < UI_FSBIBLE_MAX_QUEST_COUNT; i++)
	{
		m_oldQuestList[i].questId = QUEST_INVALID_ID;
		m_oldQuestList[i].questName[0] = 0;
		m_oldQuestList[i].questTypeName[0] = 0;
	}
	m_QuestNum = 0;
	m_IsLoadList = false;
}

void KUiFSBible::flashQuestList()
{
	if(!_thisWindow)
	{
		return;
	}

	//任务列表
	for(int i = 0; i < UI_FSBIBLE_MAX_QUEST_COUNT; ++i)
	{
		_questListData[i].questId = QUEST_INVALID_ID;
		_questListData[i].questName[0] = 0;
		_questListData[i].questTypeName[0] = 0;
	}

	g_pCoreShell->GetGameData( GDI_GET_QUEST_LIST, (UINT)_questListData, UI_FSBIBLE_MAX_QUEST_COUNT);

	_questList->removeAllItem();

// 	TLTreeItem* treeHead = new TLTreeItem(AnsiToUtf8(ACCEPTED_QUEST), QUEST_INVALID_ID);
// 	_questList->addItem(treeHead);

	char typeName[COMMON_CLIENT_MSG_LEN_128];
	TLTreeItem* curTypeItem = NULL;
	for(int j = 0; j < UI_FSBIBLE_MAX_QUEST_COUNT; ++j)
	{
// 		_questListData[j].questId = 1;
// 		sprintf(_questListData[j].questName, "test%d", j);
		if(_questListData[j].questId != QUEST_INVALID_ID)
		{
			if(strcmp(typeName, _questListData[j].questTypeName))
			{
				strcpy(typeName, _questListData[j].questTypeName);
				curTypeItem = new TLTreeItem(AnsiToUtf8(typeName), QUEST_INVALID_ID);
				_questList->addItem(curTypeItem);
				curTypeItem->setIsOpen(true);
				curTypeItem->setAutoDeleted(true);
			}

			if(curTypeItem)
			{
				TLTreeItem* questItem = new TLTreeItem(AnsiToUtf8(_questListData[j].questName), _questListData[j].questId);
				if(_questListData[j].questId == d_selQuestId)
				{
					questItem->setSelected(true);
				}
				questItem->setAutoDeleted(true);
				curTypeItem->addItem(questItem);
			}
		}
	}

	if(_questList->getTreeTotalItemsHeigh() <= d_questListMaxHeight)
	{
		d_questListPanelScrollBar->hide();
		_questList->setPosition(Absolute, Point(0, 0));
	}
	else
	{
		d_questListPanelScrollBar->show();
	}
}

bool KUiFSBible::selectQuest(const EventArgs& args)
{
	if(_questList->getTreeTotalItemsHeigh() <= d_questListMaxHeight)
	{
		d_questListPanelScrollBar->hide();
		_questList->setPosition(Absolute, Point(0, 0));
	}
	else
	{
		d_questListPanelScrollBar->show();
	}

	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* treeItem = treeEvent->treeItem;

	if(treeItem == NULL)
	{
		return false;
	}

	int questId = treeItem->getID();
	if(questId == QUEST_INVALID_ID)
	{
		return true;
	}

	d_selQuestId = questId;

	//如果按住shift就跟踪该任务
	if(System::getSingleton().isShiftDown())
	{
		KUiQuestTrack::GetSingleton().addTrack(questId);
	}

	showSelQuest();

	return true;
}

bool KUiFSBible::onQuestInfoPanelScroll(const CEGUI::EventArgs& args)
{
	float scrollPos = d_questInfoPanelScrollBar->getScrollPosition();
	int ctrlHeight = d_questInfoPanel->getHeight(Absolute);
	int exceedSize = (ctrlHeight - d_questInfoMaxHeight) * scrollPos;
	d_questInfoPanel->setYPosition(Absolute, -exceedSize);
	return true;
}

bool fb_tipIsQuest = false;
bool KUiFSBible::onHoverText(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* frameCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = frameCtrl->getLayout();

	if(lay == NULL)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}

	Point pos = frameCtrl->getUnclippedPixelRect().getPosition();
	Point off = frameCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;

	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		if(fb_tipIsQuest)
		{
			KUiItemTip::Hide();
			fb_tipIsQuest = false;
		}

		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}

	if(elemInfo.gameObj._objType == LO_GO_NPC)
	{
		const wchar_t* desUnicode = elemInfo.description.get();
		char* desAnsi = NULL;
		unicodeToAnsi(desUnicode, desAnsi);	

		char tipText[COMMON_CLIENT_MSG_LEN_1024 * 2];
		sprintf(tipText, KUiCfgLoader::getSingleton().getQuestData().aimTip, desAnsi);

		CEGUI::Rect loarea;
		lorectToCerect(&elemInfo.area, &loarea);
		loarea = loarea.offset(off + pos);
		
		KUiItemTip::GetSingleton().show(tipText, loarea, KUiItemTip::BottomRight);

		delete[] desAnsi;
		fb_tipIsQuest = true;
	}
	else
	{
		if(fb_tipIsQuest)
		{
			KUiItemTip::Hide();
			fb_tipIsQuest = false;
		}
		//KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
	}

	KUiAdapter::SetMouseRes( MOUSE_SUPER_LINK_PLAYER + elemInfo.gameObj._objType - 1 );
	
	return true;
}

bool KUiFSBible::onLeaveText(const EventArgs& args)
{
	if(fb_tipIsQuest)
	{
		KUiItemTip::Hide();
	}
	KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
	fb_tipIsQuest = false;
	return true;
}

bool KUiFSBible::vslUpdateInfoScroll_ScrollPositionChanged(const EventArgs & args)
{
	bool ret = false;
	if (m_UpdateInfoScroll != NULL && m_UpdateInfoText != NULL)
	{
		int layoutHeight = m_UpdateInfoText->getLayout()->getRenderArea().getHeight();
		int clipperHeight = m_UpdateInfoText->getHeight(Absolute) 
			- m_UpdateInfoText->getTopFrameHeight() - m_UpdateInfoText->getBottomFrameHeight();
		
		
		float scrollPos = m_UpdateInfoScroll->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;
		
		m_UpdateInfoText->setLayoutOffset(0, -yPos + m_UpdateInfoText->getTopFrameHeight());
		ret = true;
	}
	return ret;
}

bool KUiFSBible::btnUpdateInfo_MouseClick(const EventArgs & args)
{
	if(!_thisWindow)
	{
		return false;
	}
	_thisWindow->setZLevel(Window::SuperTop);
	_thisWindow->show();

	bool ret = false;
	ShowUpdateInfoPanel();
	return ret;
}
void KUiFSBible::ShowUpdateInfoPanel()
{
	if (m_UpdateInfoFrame && d_pHelpMulEdScrol && _helpPanel && d_pHelpTreeParent)
	{
		hideAllPanel();
		m_UpdateInfoFrame->show();
		_helpPanel->show();
		_helpPanel->disable();
		d_pHelpMulEdScrol->hide();
		d_pHelpTreeParent->hide();
		d_pHelpTreeScrol->hide();
		d_pHelpMultiEdit->hide();
	}
}
bool KUiFSBible::onClickQuestInfo( const CEGUI::EventArgs& args )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* chanCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = chanCtrl->getLayout();

	if(lay == NULL)
		return false;
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;

	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return false;
	}
	
	bool handled = false;
	switch(elemInfo.gameObj._objType)
	{
	case LO_GO_POSITION:
		{
			KUiSceneTimeInfo mapInfo = { 0 };
			
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
			mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
			
			NpcMapPos pos;
			g_pCoreShell->GetGameData(GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&pos, elemInfo.gameObj._objId[1]);
			if(pos.mapId == mapInfo.nSceneId)
			{
				g_pCoreShell->OperationRequest(GOI_SET_AUTO_DIALOG_NPC, elemInfo.gameObj._objId[1], NULL);
				g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)pos.x, (int)pos.y * 2);
			}
			else
			{
				char errorMsg[COMMON_CLIENT_MSG_LEN_1024];
				char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
				char mapName[COMMON_CLIENT_MSG_LEN_64];
				memset(mapName, 0, sizeof(mapName));
				
				//获得地图名
				KIniFile mapFile;
				if(mapFile.Load( "\\settings\\maplist.ini" ))
				{
					char mapId[COMMON_CLIENT_MSG_LEN_32];
					sprintf(mapId, "%d", pos.mapId);
					mapFile.GetString( "List", mapId, "", mapName, sizeof(mapName));
				}
				sprintf(errorMsg, szMsg, mapName, pos.x, pos.y);
				KUiChannelCentre::GetSingleton().toSysMsg(errorMsg);
				static DWORD lastErrorMsgTime = 0;
				if(GetTickCount() - lastErrorMsgTime > 2000)
				{
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(errorMsg));
					lastErrorMsgTime = GetTickCount();
				}
			}
		}
		break;
	default:
		break;
	}

	return handled;
}

bool KUiFSBible::onListWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(d_questListPanelScrollBar->isVisible())
	{
		d_questListPanelScrollBar->setScrollPosition(d_questListPanelScrollBar->getScrollPosition()
			- d_questListPanelScrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiFSBible::onDeleteQuest(const CEGUI::EventArgs& args)
{
	KUiComMsgBox::Show();

	KUiComMsgBox& msgBox = KUiComMsgBox::GetSingleton();
	msgBox.setFristBtnCallback(doDeleteQuest);
	msgBox.setMsg(AnsiToUtf8(KMessageCentre::GetMessage(quest_message, QUEST_DELETE_NOTIFY)));

	char okBtnName[COMMON_CLIENT_MSG_LEN_32];
	char cancelBtnName[COMMON_CLIENT_MSG_LEN_32];
	strcpy(okBtnName, KMessageCentre::GetMessage(quest_message, QUEST_OK));
	strcpy(cancelBtnName, KMessageCentre::GetMessage(quest_message, QUEST_CANCEL));
	msgBox.setBtnName(AnsiToUtf8(okBtnName), AnsiToUtf8(cancelBtnName));
	return true;
}

bool KUiFSBible::onTrackQuest(const CEGUI::EventArgs& args)
{
	KUiQuestTrack::GetSingleton().addTrack(d_selQuestId);
	return true;
}

void KUiFSBible::doDeleteQuest()
{
	g_pCoreShell->OperationRequest( GOI_DELETE_QUEST, KUiFSBible::getSingleton().d_selQuestId, NULL );
}

void KUiFSBible::onQuestListChange()
{
	if(!_thisWindow)
	{
		return;
	}

	if (m_isUsed)
	{
		if (m_IsLoadList)
		{
			KSimpleQuestInfo _newQuestList[UI_FSBIBLE_MAX_QUEST_COUNT];

			for (int i = 0; i < UI_FSBIBLE_MAX_QUEST_COUNT; i++)
			{
				_newQuestList[i].questId = QUEST_INVALID_ID;
				_newQuestList[i].questName[0] = 0;
				_newQuestList[i].questTypeName[0] = 0;
			}

			g_pCoreShell->GetGameData( GDI_GET_QUEST_LIST, (UINT)_newQuestList, UI_FSBIBLE_MAX_QUEST_COUNT);

			int _newQuestNum = 0;
			int _oldQuestNum = 0;
			while (_newQuestList[_newQuestNum].questId != QUEST_INVALID_ID || m_oldQuestList[_oldQuestNum].questId != QUEST_INVALID_ID)
			{
				if (_newQuestList[_newQuestNum].questId != QUEST_INVALID_ID)
				{
					_newQuestNum++;
				}
				if (m_oldQuestList[_oldQuestNum].questId != QUEST_INVALID_ID)
				{
					_oldQuestNum++;
				}
			}

			if (_newQuestNum > _oldQuestNum)
			{
				int _questID = QUEST_INVALID_ID;
				BOOL _isEqual = FALSE;
				for (i = 0; i < _newQuestNum; i++)
				{
					_isEqual = FALSE;
					for (int j = 0; j < _oldQuestNum; j++)
					{
						if (_newQuestList[i].questId == m_oldQuestList[j].questId)
						{
							_isEqual = TRUE;
							break;
						}
					}
					if (!_isEqual)
					{
						_questID = _newQuestList[i].questId;
						KUiQuestTrack::GetSingleton().addTrack(_questID);
						break;
					}
				}
			}
			else if (_oldQuestNum >= _newQuestNum)
			{
				int _questID = QUEST_INVALID_ID;
				BOOL _isEqual = FALSE;
				for (i = 0; i < _oldQuestNum; i++)
				{
					_isEqual = FALSE;
					for (int j = 0; j < _newQuestNum; j++)
					{
						if (_newQuestList[j].questId == m_oldQuestList[i].questId)
						{
							_isEqual = TRUE;
							break;
						}
					}
					if (!_isEqual)
					{
						_questID = m_oldQuestList[i].questId;
						KUiQuestTrack::GetSingleton().addTrack(_questID);
						break;
					}
				}
			}

			for (i = 0; QUEST_INVALID_ID != m_oldQuestList[i].questId; i++)
			{
				m_oldQuestList[i].questId = QUEST_INVALID_ID;
				m_oldQuestList[i].questName[0] = 0;
				m_oldQuestList[i].questTypeName[0] = 0;
			}
			g_pCoreShell->GetGameData( GDI_GET_QUEST_LIST, (UINT)m_oldQuestList, UI_FSBIBLE_MAX_QUEST_COUNT);
		}
		else
		{
			FirstGetQuestList();
			m_IsLoadList = true;
		}
	}

	if(!_thisWindow->isVisible())
		return;

	flashQuestList();
}

void KUiFSBible::onQuestChange(int questId)
{
	if(!_thisWindow)
	{
		return;
	}

	if(d_selQuestId == questId)
	{
		showSelQuest();
	}
}

bool KUiFSBible::selQuestValidate()
{
	if(!_thisWindow)
	{
		return false;
	}

	for(int j = 0; j < UI_FSBIBLE_MAX_QUEST_COUNT; ++j)
	{
		if(_questListData[j].questId == d_selQuestId && d_selQuestId != QUEST_INVALID_ID)
		{
			return true;
		}
	}
	return false;
}

void KUiFSBible::FirstGetQuestList()
{
	if(!_thisWindow)
	{
		return;
	}
	
	//任务列表
	for(int i = 0; QUEST_INVALID_ID != m_oldQuestList[i].questId; ++i)
	{
		m_oldQuestList[i].questId = QUEST_INVALID_ID;
		m_oldQuestList[i].questName[0] = 0;
		m_oldQuestList[i].questTypeName[0] = 0;
	}

	g_pCoreShell->GetGameData( GDI_GET_QUEST_LIST, (UINT)m_oldQuestList, UI_FSBIBLE_MAX_QUEST_COUNT);
}




/*************************************操作帮助窗口方法****************************************/


/**************************************************************************************/
/*	初始化操作帮助窗口控件，并从UiSettings\Setting.ini文件中
 *	读取帮助目录信息，并创建树；
/**************************************************************************************/
void	KUiFSBible::InitHelp()
{
	InitHelpCtrl();
	d_pHelpMultiEdit->useLayout();
	InitHelpLayOut();
	CreateHelpTree();
		
	ZeroMemory(d_layoutTextHead, sizeof(d_layoutTextHead));
	sprintf(d_layoutTextHead, "<Layout width=%d>", (int)(d_pHelpMultiEdit->getAbsoluteWidth()) );	
}


/**************************************************************************************/
/*	初始化操作帮助窗口控件,并注册控件的事件函数
/**************************************************************************************/
void	KUiFSBible::InitHelpCtrl()
{

	_helpPanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/HelpFrame");
	_helpPanel->hide();

	d_pHelpTreeParent= static_cast<TLStaticImage *>(_helpPanel->getChild("TaharezLook/HelpFrame/HelpTreeClipper"));		

	//帮助信息鼠标移动TIP事件		
	d_pHelpMultiEdit = static_cast<TLStaticText *>(_helpPanel->getChild("TaharezLook/HelpFrame/HelpEditbox"));	
	d_pHelpMultiEdit->setZLevel(CEGUI::Window::SuperTop);
	d_pHelpMultiEdit->subscribeEvent(TLStaticText::EventMouseMove, Event::Subscriber(&KUiFSBible::onHelpMultiEditMouseMove, this));	
	d_pHelpMultiEdit->subscribeEvent(TLStaticText::EventMouseClick, Event::Subscriber(&KUiFSBible::onHelpMultiEditMouseClick, this));
	d_pHelpMultiEdit->subscribeEvent(TLStaticText::EventMouseLeaves, Event::Subscriber(&KUiFSBible::onHelpMultiEditMouseLeaves, this));
	d_pHelpMultiEdit->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiFSBible::onHelpMultiEditMouseWheel, this));
	

	//帮助目录树事件	
	d_pHelp = static_cast<TLTree *>(d_pHelpTreeParent->getChild("TaharezLook/HelpFrame/HelpTreeClipper/HelpTree"));
	d_pHelp->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiFSBible::onHelpSelItemMouseDown, this));	
	d_pHelp->subscribeEvent(Tree::TR_EventBranchOpened, Event::Subscriber(&KUiFSBible::onHelpBranceOpened, this));		
	d_pHelp->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiFSBible::onHelpWheelChanged, this));

	//帮助目录树垂直滚动事件	
	d_pHelpTreeScrol = static_cast<TLVertScrollbar *>(_helpPanel->getChild("TaharezLook/HelpFrame/TreeScrollbar"));
	d_pHelpTreeScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiFSBible::onHelpTreeScroll, this));
	
	//帮助内容垂直滚动事件
	d_pHelpMulEdScrol= static_cast<TLVertScrollbar *>(_helpPanel->getChild("TaharezLook/HelpFrame/MulEditScrollbar"));
	d_pHelpMulEdScrol->setZLevel(CEGUI::Window::SuperTop);
	d_pHelpMulEdScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiFSBible::onHelpMultiScrol, this));
	

	//操作帮助单选钮点击事件
	TLRadioButton* helpBtn = (TLRadioButton*)_thisWindow->getChild("TaharezLook/FSBible/Help");
	helpBtn->subscribeEvent(TLRadioButton::EventMouseClick, Event::Subscriber(&KUiFSBible::onHelpClickPanelBtn, this));
	
	//窗口移动事件
	_thisWindow->subscribeEvent(Window::EventMoved, Event::Subscriber(&KUiFSBible::onHelpMoveWindow, this));					
}


/**************************************************************************************/
/*	初始化操作帮助窗口文字显示的排版系统
/**************************************************************************************/
void	KUiFSBible::InitHelpLayOut()
{
	Rect textArea;
	if ( d_pHelpMultiEdit != NULL )
	{
		textArea = d_pHelpMultiEdit->getUnclippedPixelRect();
		Vector2 posOff = textArea.getPosition() - _thisWindow->getUnclippedPixelRect().getPosition();
		textArea.setPosition(posOff);
	}

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	if (clipper.getTop() > 0)
	{
		d_pHelpMultiEdit->getLayout()->setClipper(clipper);
	}
}

/**************外部调用****************************************************************/

/**************************************************************************************/
/*	1.TLStatic.handleHelp调用，用于点击功能窗口的?链接到相应的操作帮助
 *	2.游戏功能窗口的?将导航到操作帮助窗口相对应的帮助信息，
 *	3.导航到操作帮助窗口之前，将调用此函数设置导航文本地址	
/**************************************************************************************/
void	KUiFSBible::SetHelpItemSelectStatus( String &pName )
{
	if ( getSingleton().d_pHelpTreeParent != NULL )
	{
		getSingleton().d_pHelp->clearAllSelections();
		float clipper = getSingleton().d_pHelpTreeParent->getHeight(Absolute);
		TreeItem *tempItem = getSingleton().d_pHelp->findLeafPathFromTree(pName);

		const EventArgs args;
		getSingleton().onHelpSelItemMouseDown(args);

		/*float treeRenderHigh =	getSingleton().d_pHelp->getTreeTotalItemsHeigh();
		float treeSomeHigh	 =  getSingleton().d_pHelp->getHeightItemToTop(tempItem);

		if ( (treeRenderHigh - treeSomeHigh) > clipper )
		{
			float pos = treeSomeHigh / (treeRenderHigh - clipper);
			getSingleton().d_pHelpTreeScrol->setScrollPosition(pos);
		}
		else
		{
			getSingleton().d_pHelpTreeScrol->setScrollPosition(1);
		}*/
	}
} 


/**************************************************************************************/
/*	TLStatic.handleHelp调用，用于点击功能窗口的?链接到相应的操作帮助;
 *	此方法为显示相应的封神宝典中的操作帮助窗口；
/**************************************************************************************/
void KUiFSBible::ShowHelp()
{
	if(!_thisWindow)
	{
		return;
	}
	_thisWindow->setZLevel(Window::SuperTop);
	_thisWindow->show();
	
	//loadSpecialQuestInfo();
	//KUiFSBibleSpecialQuestData::getSingleton().freshUi();
	
	hideAllPanel();

	TLRadioButton* helpBtn = (TLRadioButton*)_thisWindow->getChild("TaharezLook/FSBible/Help");
	helpBtn->setSelected(true);
	if( !d_pHelpMulEdScrol || !d_pHelpTreeParent || !d_pHelpTreeScrol || !_helpPanel )
		return;
	d_pHelpMulEdScrol->show();
	d_pHelpTreeParent->show();
	d_pHelpTreeScrol->show();
	_helpPanel->enable();
	_helpPanel->show();
	
}


/************操作帮助事件方法***********************************************************************/

/**************************************************************************************/
/*	1.当帮助窗口左边树被打开时触发；
 *	2.当树打开后，树高度超过面板容器高度时，设置合适的滚动位置
/**************************************************************************************/
bool	KUiFSBible::onHelpBranceOpened( const CEGUI::EventArgs& args )
{
	float fTree_height    =  d_pHelp->getTreeTotalItemsHeigh();
	float fClipper_height = d_pHelpTreeParent->getHeight(Absolute);

	if (fTree_height > fClipper_height) {
		d_pHelpTreeScrol->setScrollPosition(0);
	} else {
		d_pHelpTreeScrol->setScrollPosition(1);
	}
	return true;
}


/**************************************************************************************/
/*	1.当鼠标滑轮在帮助窗口左边树滑动时触发；
 *	2.实现目录树内容的滚动浏览	zhangxin
/**************************************************************************************/
bool	KUiFSBible::onHelpWheelChanged( const CEGUI::EventArgs& args ) 
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(d_pHelpTreeScrol->isVisible())
	{ 
		float currPos = d_pHelpTreeScrol->getScrollPosition();
		float step =	( d_pHelpTreeScrol->getStepSize() );
		float wheel = eventArgs->wheelChange;
		step *=  wheel;
		d_pHelpTreeScrol->setScrollPosition(d_pHelpTreeScrol->getScrollPosition() - \
											(d_pHelpTreeScrol->getStepSize() * eventArgs->wheelChange));
	}
	return true;	
}

/**************************************************************************************/
/*	1.鼠标点击帮助目录树子条目事件
 *	2.事件将读取string.ini文件文本并按排版格式显示
/**************************************************************************************/
bool	KUiFSBible::onHelpSelItemMouseDown( const CEGUI::EventArgs& args )
{		
	char text[COMMON_CLIENT_MSG_LEN_1024 * 10] = {0};
	TLTreeItem *pHelpItem = (TLTreeItem *)(d_pHelp->getFirstSelectedItem());

	if ( pHelpItem == NULL )
	{
		if ( !d_bItem )
		{
			d_pHelpMultiEdit->getLayout()->clearLayout();
		}
		d_bItem = false;
		return false;
	}

	d_pHelpMulEdScrol->setScrollPosition(0.0f);
	d_pHelpMultiEdit->setLayoutOffset( 0, 0 );

	uint itemIndex = pHelpItem->getItemAdIndex();
	const int lWidth = d_pHelpMultiEdit->getWidth(Absolute);
	sprintf( text, "%s%s", d_layoutTextHead, KMessageCentre::GetMessage(help_edit_message, itemIndex));

	d_pHelpMultiEdit->getLayout()->SetText(text);
	d_bItem = true;

	d_pHelpMultiEdit->enable();
	
	return true;
}

/**************************************************************************************/
/*	1.操作帮助目录树垂直滚动条滚动事件
 *	2.根据滚动条位置，设置文本上下位置来实现滚动
/**************************************************************************************/
bool	KUiFSBible::onHelpTreeScroll( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;

	float sparef = d_pHelp->getTreeTotalItemsHeigh();
	float clipper = d_pHelpTreeParent->getHeight(Absolute);
	float yPos = 0;
	
	if (scrollCtrl->window == d_pHelpTreeScrol)
	{	
		float scrollPos = d_pHelpTreeScrol->getScrollPosition();

		if ( (sparef > clipper ) )
		{
			yPos = (sparef - clipper) * scrollPos;
			Point pos;
			pos.d_x = d_pHelp->getPosition(Absolute).d_x;
			pos.d_y = 0 - yPos;
			d_pHelp->setPosition( Absolute, pos );
		}
		else
		{
			d_pHelp->setPosition( Absolute, Point(0, 0));
		}
	}

	return false;
}

/**************************************************************************************/
/* 1.当鼠标滑轮在帮助窗口右边帮助信息时，滑动时触发；
 *	2.实现帮助信息内容的滚动浏览
/**************************************************************************************/
bool	KUiFSBible::onHelpMultiEditMouseWheel( const CEGUI::EventArgs& args ) 
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(d_pHelpMulEdScrol && d_pHelpMulEdScrol->isVisible())
	{ 
		d_pHelpMulEdScrol->setScrollPosition(d_pHelpMulEdScrol->getScrollPosition() - \
											(d_pHelpMulEdScrol->getStepSize() * eventArgs->wheelChange));
	}
	return true;	
}

/**************************************************************************************/
/*	1.操作帮助内容窗口的垂直滚动条滚动事件
 *	2.根据滚动条位置，设置文本上下位置来实现滚动
/**************************************************************************************/
bool	KUiFSBible::onHelpMultiScrol( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	if ( scrollCtrl->window == d_pHelpMulEdScrol && d_pHelpMultiEdit != NULL ) 
	{
		int layoutHeight = d_pHelpMultiEdit->getLayout()->getRenderArea().getHeight();
		int clipperHeight = d_pHelpMultiEdit->getHeight(Absolute);
		
		if(layoutHeight <= clipperHeight)
			return false;
		
		float scrollPos = d_pHelpMulEdScrol->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;
		
		d_pHelpMultiEdit->setLayoutOffset( 0, -yPos );
	}
	return true;
}

bool	KUiFSBible::onHelpMoveWindow( const CEGUI::EventArgs& args )
{
	InitHelpLayOut();
	return true;
}


/**************************************************************************************/
/*	1.操作帮助信息，鼠标移动事件
 *	2.当鼠标滑动到NPC名字时，出现Tip提出窗口，显示NPC所在地图及坐标；
 *	  单击NPC名字，如果在本地图，人物自动寻路到指定NPC
 *	3.当鼠标滑动到Item名字时，出现Tip提出窗口，显示Item所有属性
/**************************************************************************************/
bool	KUiFSBible::onHelpMultiEditMouseMove( const CEGUI::EventArgs& args )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* chanCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = chanCtrl->getLayout();
	
	if(lay == NULL)
		return false;
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;	//鼠标相对于文本区域左上顶点的位置
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)	//取出文本中游戏对象（NPC,Item）的属性(npc模板ID)
	{
		return false;
	}
	

	if(d_bHelpTip) {
		KUiItemTip::GetSingleton().Hide();
		d_bHelpTip = false;
	}
	
	char _tipText[LAYOUT_TEXT_MAX_LEN];

	switch(elemInfo.gameObj._objType)
	{
	case LO_GO_NPC:
		{
			
			NpcMapPos pos;
			g_pCoreShell->GetGameData(GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&pos, elemInfo.gameObj._objId[0]);
			
			char mapName[COMMON_CLIENT_MSG_LEN_64];
			memset(mapName, 0, sizeof(mapName));
			
			//根据地图id获得地图名
			KIniFile mapFile;
			if(mapFile.Load( "\\settings\\maplist.ini" ))
			{
				char mapId[COMMON_CLIENT_MSG_LEN_32];
				sprintf(mapId, "%d", pos.mapId);
				mapFile.GetString( "List", mapId, "", mapName, sizeof(mapName));
			}	

			sprintf(_tipText, "<Layout width=150 margin-top=%d margin-left=%d margin-right=%d margin-bottom=%d>", 
				KUiCfgLoader::getSingleton().getSceneMapCfg().topMargin,
				KUiCfgLoader::getSingleton().getSceneMapCfg().leftMargin,
				KUiCfgLoader::getSingleton().getSceneMapCfg().RightMargin,
				KUiCfgLoader::getSingleton().getSceneMapCfg().bottomMargin);
			
			strcat(_tipText, "<Seg float=wrap><Obj color=");
			strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipColor);
			strcat(_tipText, " font-family=");
			strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipFont);
			strcat(_tipText, ">");

			strcat(_tipText, mapName);
			strcat(_tipText, "</Obj></Seg>");

			strcat(_tipText, "<Seg float=wrap><Obj color=");
			strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipColor);
			strcat(_tipText, " font-family=");
			strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipFont);
			strcat(_tipText, ">");

			char tempText[COMMON_CLIENT_MSG_LEN_32];
			sprintf(tempText, "(%d,%d)", pos.x, pos.y);
			strcat(_tipText, tempText);
			strcat(_tipText, "</Obj></Seg>");
			strcat(_tipText, "</Layout>");

			KUiItemTip::GetSingleton();
			KUiItemTip::GetSingleton().show(_tipText, Rect(mouse->position, Size(1, 1)));
			d_bHelpTip = true;
		}
		break;
	case LO_GO_ITEM:
		{
			KUiTipGenerator::TipObject tipObj;
			tipObj.type = KUiTipGenerator::LinkedItem;			
			tipObj.ids[0] = elemInfo.gameObj._objId[0];
			tipObj.ids[1] = elemInfo.gameObj._objId[1];
			tipObj.ids[2] = elemInfo.gameObj._objId[2];
			tipObj.ids[3] = elemInfo.gameObj._objId[3];		
			
			char* layoutDes = KUiTipGenerator::getSinglton().genLayoutDes(tipObj);
			KUiItemTip::GetSingleton().show(layoutDes, Rect(mouse->position, Size(1, 1)));

			d_bHelpTip = true;
		}
		break;
	default:
		break;
	}
	
	return true;
}


/************************************************************************/
/* 鼠标离开操作帮助信息事件；
 * 处理Tip窗口，当鼠标离开后隐藏
/************************************************************************/
bool	KUiFSBible::onHelpMultiEditMouseLeaves( const CEGUI::EventArgs& args )
{
	if(d_bHelpTip)
	{
		KUiItemTip::GetSingleton().Hide();
		d_bHelpTip = false;
	}
	return true;
}


/**************************************************************************************/
/*	1.操作帮助信息，鼠标单击事件
 *	2.单击NPC名字，如果在本地图，人物自动寻路到指定NPC；
 *	  如果不是当前地图，提示无法跨地图寻路
/**************************************************************************************/
bool	KUiFSBible::onHelpMultiEditMouseClick( const CEGUI::EventArgs& args )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* chanCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = chanCtrl->getLayout();
	
	if(lay == NULL)
		return false;
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;	//鼠标相对于文本区域左上顶点的位置
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)	//取出文本中游戏对象（NPC,Item）的属性(npc模板ID，itemid)
	{
		return false;
	}
	
	//char _tipText[LAYOUT_TEXT_MAX_LEN];
	
	switch(elemInfo.gameObj._objType)
	{
	case LO_GO_NPC:
		{
			
			KUiSceneTimeInfo mapInfo = { 0 };
			
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );//取得当前主角所处的地域时间环境
			mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
			
			NpcMapPos pos;
			g_pCoreShell->GetGameData(GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&pos, elemInfo.gameObj._objId[0]);//根据npc模板id，取得npc在地图的坐标
			if(pos.mapId == mapInfo.nSceneId)//如果是当前地图
			{
				g_pCoreShell->OperationRequest(GOI_SET_AUTO_DIALOG_NPC, elemInfo.gameObj._objId[0], NULL);//自动打开npc对话框
				g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)pos.x, (int)pos.y * 2);//自动寻路到指定位置
			}
			else//不能跨地图自动寻路
			{
				char errorMsg[COMMON_CLIENT_MSG_LEN_1024];
				char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
				char mapName[COMMON_CLIENT_MSG_LEN_64];
				memset(mapName, 0, sizeof(mapName));
				
				//获得地图名
				KIniFile mapFile;
				if(mapFile.Load( "\\settings\\maplist.ini" ))
				{
					char mapId[COMMON_CLIENT_MSG_LEN_32];
					sprintf(mapId, "%d", pos.mapId);
					mapFile.GetString( "List", mapId, "", mapName, sizeof(mapName));
				}
				sprintf(errorMsg, szMsg, mapName, pos.x, pos.y);
				KUiChannelCentre::GetSingleton().toSysMsg(errorMsg);
			}			
		}
		break;
	default:
		break;
	}	
	return true;
}

/************************************************************************/
/* 鼠标单击上线提示今日焦点checkbox事件；
 * 选中，登陆后自动打开封神宝典窗口
/************************************************************************/
bool	KUiFSBible::onLoginAutoShow( const CEGUI::EventArgs& args )
{
	if (!TodayMsgTip) {
		return false;
	}	

	KIniFile ini;
	ini.Load(CONFIG_INI);

	if (TodayMsgTip->isSelected()) 
	{		
		ini.WriteInteger("GameSetting", "LoginBibleAutoShow", 1);	
	} 
	else {	
		ini.WriteInteger("GameSetting", "LoginBibleAutoShow", 0);
	}
	ini.Save(CONFIG_INI);
	return true;
}


/**********操作帮助树控件相关操作***********************************************************/


/**************************************************************************************/
/*	从UiSettings\Setting.ini文件中
 *		读取帮助目录信息，并创建树；
/**************************************************************************************/
void	KUiFSBible::CreateHelpTree()
{	
	KIniFile ini;
	vector<TLTreeItem *> treeItemArray;
	vector<TLTreeItem *> treeItemCopy;
	if ( ini.Load( MAP_SETTING_FILE )) 
	{
		d_pHelp->setSortingEnabled(false); //树内容不排序

		int LayCount = 0;
		ini.GetInteger( "HelpTree","LayCount", 0, &LayCount);
		
		for( int i = 1; i <= LayCount; i++)
		{
			if ( i == TREE_FLAG )
			{
				int   leafCount = 0;
				string temp = getHelpLeafName( TREE_LAY, i, TREE_LEAF_COUNT, 0 );
				ini.GetInteger( "HelpTree", temp.c_str(), 0, &leafCount );
				for( int j = 1; j <= leafCount; j++)
				{
					char leafInfo[COMMON_CLIENT_MSG_LEN_128];
					string temp1 = getHelpLeafName( TREE_LAY, i, TREE_LEAF, j );
					ini.GetString( "HelpTree", temp1.c_str(), "", leafInfo, COMMON_CLIENT_MSG_LEN_128 );
					
					unsigned char *tempword = AnsiToUtf8(getHelpItemWord( leafInfo ).c_str());
					TLTreeItem *pHelpItem = new TLTreeItem(AnsiToUtf8(getHelpItemWord( leafInfo ).c_str()), 0, NULL, false, false );
					d_pHelp->addItem( pHelpItem );
					treeItemArray.push_back( pHelpItem );
					d_itemArray.push_back( pHelpItem );
				}
			}
			else
			{
				int	leafCount = 0;
				string temp = getHelpLeafName( TREE_LAY, i, TREE_LEAF_COUNT, 0 );
				ini.GetInteger( "HelpTree", temp.c_str(), 0, &leafCount );
				for( int j = 1; j <= leafCount; j++ )
				{
					char leafInfo[COMMON_CLIENT_MSG_LEN_128];
					string temp1 = getHelpLeafName( TREE_LAY, i, TREE_LEAF, j  );
					ini.GetString( "HelpTree", temp1.c_str(), "", leafInfo, COMMON_CLIENT_MSG_LEN_128 );
					int parentIndex = getHelpParentInfo( leafInfo );
					TLTreeItem *pHelpItem = new TLTreeItem( AnsiToUtf8(getHelpItemWord( leafInfo ).c_str()), 0, NULL, false, false );
					pHelpItem->setItemAdIndex( getHelpItemDesInfo( leafInfo ));
					TLTreeItem *pParentItem = treeItemArray[parentIndex - 1];
					pParentItem->addItem( pHelpItem );
					treeItemCopy.push_back( pHelpItem );
					d_itemArray.push_back( pHelpItem );
				}
				treeItemArray = treeItemCopy;
			}
		}// end if		
	}
}


/**************************************************************************************/
/*	类析构函数调用此销毁树方法
/**************************************************************************************/
void	KUiFSBible::DestryHelpTree()
{
	if ( d_pHelp ) 
	{
		d_pHelp->removeAllItem();
		int count = d_itemArray.size();
		for( int i=0; i < count; i++ )
		{
			delete d_itemArray[i];
			d_itemArray[i] = NULL;
		}
	}
}

/**************************************************************************************/
/*	取得配置文件中树的父节点对应的编号
/**************************************************************************************/
int		KUiFSBible::getHelpParentInfo( const char *leafInfo )
{
	int iResult = 0;
	vector<string> word;
	string leafInfoCpy = leafInfo;
	string flag = ",";
	string::size_type pos = 0;
	string::size_type pre_pos = 0;
	while( (pos = leafInfoCpy.find_first_of( flag, pos )) != string::npos )
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
		pre_pos = ++pos;
	}
	iResult = static_cast<int>(atoi(word[TREE_PARENT_INFO].c_str()));
	return iResult;
}

/**************************************************************************************/
/*	返回配置文件中树结点的目录名字符串
/**************************************************************************************/
string	KUiFSBible::getHelpItemWord( const char *leafInfo )
{
	vector<string> word;
	string leafInfoCpy = leafInfo;
	string flag = ",";
	string::size_type pos = 0;
	string::size_type pre_pos = 0;
	while( (pos = leafInfoCpy.find_first_of( flag, pos )) != string::npos )
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
		pre_pos = ++pos;
	}
	return word[TREE_WORD_INFO].c_str();
}


/**************************************************************************************/
/*	返回该树子结点的唯一索引，
 *	用该索引到string.ini文件中找到对应的帮助信息
/**************************************************************************************/
int		KUiFSBible::getHelpItemDesInfo( const char *leafInfo )
{
	int iResult = 0;
	vector<string> word;
	string leafInfoCpy = leafInfo;
	string flag = ",";
	string::size_type pos = 0;
	string::size_type pre_pos = 0;
	while( (pos = leafInfoCpy.find_first_of( flag, pos )) != string::npos )
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
		pre_pos = ++pos;
	}

	if(pos == string::npos)
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
	}
	
	iResult = static_cast<int>(atoi(word[TREE_DESC_INFO].c_str()));
	return iResult;
}


/**************************************************************************************/
/*	使用通用方法得到配置文件中的key值，
 *	便于当配置文件增减目录时，程序的通用性
/**************************************************************************************/
string	KUiFSBible::getHelpLeafName( const string &lay, const int layIndex, 
								const string &treeInfo, const int leafIndex)
{
	char buffer[COMMON_CLIENT_MSG_LEN_128];
	char buffert[COMMON_CLIENT_MSG_LEN_128];
	string cresult;
	sprintf( buffer, "%d", layIndex );
	sprintf( buffert, "%d", leafIndex );
	if ( leafIndex == 0 )
	{
		cresult = lay + buffer + treeInfo;
	}
	else
	{
		cresult = lay + buffer + treeInfo + buffert;
	}
	return cresult;
}
