//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/12/2006 10:01
//      File_base        : UiQuestManage
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "kwin32.h"
#include "UiQuestManage.h"
#include "CoreShell.h"
#include "layoutinterface.h"
#include "../KMessageCentre.h"
#include "UiComMsgBox.h"
#include "UiNpcMsgBox.h"
#include "UiChatWindow.h"
#include "../UiConfigManager.h"
#include "../UiAdapter.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiQuestManage* KUiWndSingleton<KUiQuestManage>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiQuestManage::KUiQuestManage( const CEGUI::String& id_name ):
KUiWndSingleton<KUiQuestManage>( id_name )
{
	d_lomsg[0] = 0;
	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		d_itemImage[i] = NULL;
		d_selectItemImage[i] = NULL;
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiQuestManage::~KUiQuestManage( void )
{
	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		if(d_itemImage[i] && d_itemImage[i]->getUserData() != NULL)
		{
			KObjAtContRegion* region = (KObjAtContRegion*)d_itemImage[i]->getUserData();
			delete region;
			region = NULL;
		}
		if(d_selectItemImage[i] && d_selectItemImage[i]->getUserData() != NULL)
		{
			KObjAtContRegion* region = (KObjAtContRegion*)d_selectItemImage[i]->getUserData();
			delete region;
			region = NULL;
		}
	}
}

void KUiQuestManage::getChild()
{
	char ctrlName[COMMON_CLIENT_MSG_LEN_128];
		
	//任务列表
	TLStaticImage* questListClipper = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/QuestManage/QuestListClipper");
	questListClipper->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiQuestManage::onListWheelChanged, this));
	d_questListMaxHeight = questListClipper->getAbsoluteHeight();
	d_questListPanel = (TLStaticImage*)questListClipper->getChild("TaharezLook/QuestManage/QuestListClipper/QuestListPanel");

	d_questDeleteBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/QuestManage/Delete");
	d_questDeleteBtn->subscribeEvent(TLButton::EventMouseClick,	Event::Subscriber(&KUiQuestManage::onDeleteQuest, this));
	d_trackBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/QuestManage/Track");
	d_trackBtn->subscribeEvent(TLButton::EventMouseClick,	Event::Subscriber(&KUiQuestManage::onTrackQuest, this));

	char tempText[COMMON_CLIENT_MSG_LEN_256];
	for(int k = 0; k < UI_QUESTMANAGE_TEMPLATE_NUM; ++k)
	{
		sprintf(tempText, "TaharezLook/QuestManage/QuestListClipper/QuestListPanel/QuestTitleTemplate%d", k + 1);
		d_questTitleTemplate[k] = (TLButton*)d_questListPanel->getChild(String(AnsiToUtf8(tempText)));
		d_questTitleTemplate[k]->hide();
	}
	
	d_questListPanelScrollBar = (TLVertScrollbar*)questListClipper->getChild("TaharezLook/QuestManage/QuestListClipper/Scrollbar");
	d_questListPanelScrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiQuestManage::onQuestListPanelScroll, this));
	
	//任务内容
	d_questInfoClipper = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/QuestManage/QuestInfoClipper");
	d_questInfoClipper->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiQuestManage::onQuestInfoWheelChanged, this));

	d_questInfoMaxHeight = d_questInfoClipper->getAbsoluteHeight();
	//任务内容面版
	d_questInfoPanel = (TLStaticImage*)d_questInfoClipper->getChild("TaharezLook/QuestManage/QuestInfoClipper/QuestInfoPanel");
	//任务内容文字描述
	d_questText = (TLStaticText*)d_questInfoPanel->getChild("TaharezLook/QuestManage/QuestInfoClipper/QuestInfoPanel/QuestText");
	d_questText->subscribeEvent(TLStaticImage::EventMouseClick, Event::Subscriber(&KUiQuestManage::onClickQuestInfo, this));
	d_questText->subscribeEvent(TLStaticImage::EventMouseDoubleClick, Event::Subscriber(&KUiQuestManage::onClickQuestInfo, this));
	d_questText->subscribeEvent(TLStaticImage::EventMouseMove, Event::Subscriber(&KUiQuestManage::onHoverText, this));
	d_questText->subscribeEvent(TLStaticImage::EventMouseLeaves, Event::Subscriber(&KUiQuestManage::onLeaveText, this));
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

	//任务奖励
	String rewardCtrlPath = "TaharezLook/QuestManage/QuestInfoClipper/QuestInfoPanel/QuestRewardPanel";
	d_questRewardPanel = (TLStaticImage*)d_questInfoPanel->getChild(rewardCtrlPath);
	
	d_questRewardMoneyText = (TLStaticText*)d_questRewardPanel->getChild(rewardCtrlPath + "/Money");
	
	d_itemFrameTemplate = (TLStaticImage*)d_questRewardPanel->getChild(rewardCtrlPath + "/ItemFrame");
	d_itemFrameTemplate->hide();
	for(int j = 0; j < MAX_QUEST_REWARD_ITEM; ++j)
	{
		//固定奖励
		sprintf(ctrlName, "/Item%d", j + 1);
		d_itemImage[j] = (TLGameObject*)d_questRewardPanel->getChild(rewardCtrlPath + ctrlName);
		
		KObjAtContRegion* newObjInfo = new KObjAtContRegion();
		newObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_itemImage[j]->setUserData(newObjInfo);

		d_itemImageGrid[j].setCtrl(d_itemImage[j]);
		d_itemImageGrid[j].addTip();

		//可选奖励
		sprintf(ctrlName, "/ChoiceItem%d", j + 1);
		d_selectItemImage[j] = (TLGameObject*)d_questRewardPanel->getChild(rewardCtrlPath + ctrlName);
		
		KObjAtContRegion* newSelectObjInfo = new KObjAtContRegion();
		newSelectObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_selectItemImage[j]->setUserData(newSelectObjInfo);

		d_selectItemImageGrid[j].setCtrl(d_selectItemImage[j]);
		d_selectItemImageGrid[j].addTip();

		d_itemFrame[j] = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
		useTemplate(d_itemFrame[j], d_itemFrameTemplate);
		d_questRewardPanel->addChildWindow(d_itemFrame[j]);
		d_itemFrame[j]->setXPosition(Absolute, d_itemImage[j]->getXPosition(Absolute) + d_itemFrameTemplate->getXPosition(Absolute));
		
		d_selectItemFrame[j] = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
		useTemplate(d_selectItemFrame[j], d_itemFrameTemplate);
		d_questRewardPanel->addChildWindow(d_selectItemFrame[j]);
		d_selectItemFrame[j]->setXPosition(Absolute, d_selectItemImage[j]->getXPosition(Absolute) + d_itemFrameTemplate->getXPosition(Absolute));
	}

	d_rewardMsgText
		= (TLStaticText*)d_questRewardPanel->getChild(rewardCtrlPath + "/RewardMsg");
	d_rewardSelectMsgText
		= (TLStaticText*)d_questRewardPanel->getChild(rewardCtrlPath + "/RewardSelectMsg");

	//滑动条
	d_questInfoPanelScrollBar = (TLVertScrollbar*)m_pThisWnd->getChild("TaharezLook/QuestManage/Scrollbar");
	d_questInfoPanelScrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiQuestManage::onQuestInfoPanelScroll, this));

	//其他按钮
	d_cancelBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/QuestManage/Cancel");
	d_cancelBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQuestManage::onCancel, this));
	d_closeBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/QuestManage/Close");
	d_closeBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQuestManage::onCancel, this));

	//窗口打开的时候
	m_pThisWnd->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiQuestManage::onWndShow, this));

	d_questListBtnCount = 0;
}

void KUiQuestManage::useTemplate(TLButton* wnd, TLButton* templateWnd)
{
	wnd->setHeight(Absolute, templateWnd->getHeight(Absolute));
	wnd->setWidth(Absolute, templateWnd->getWidth(Absolute));
	wnd->setXPosition(Absolute, templateWnd->getXPosition(Absolute));

	wnd->setNormalImage(templateWnd->getNormalImage());
	wnd->setHoverImage(templateWnd->getHoverImage());
	wnd->setPushedImage(templateWnd->getPushedImage());
	wnd->setDisabledImage(templateWnd->getDisabledImage());

	wnd->setNormalTextColour(templateWnd->getNormalTextColour());
	wnd->setHoverTextColour(templateWnd->getHoverTextColour());
	wnd->setPushedTextColour(templateWnd->getPushedTextColour());
	wnd->setDisabledTextColour(templateWnd->getDisabledTextColour());

	wnd->setTextHFormatting(LeftAligned);
	wnd->setTextXOffset(templateWnd->getTextXOffset());
	wnd->setHoverYOff(0);
	wnd->setPushedYOff(1);
}

void KUiQuestManage::useTemplate(StaticImage* wnd, StaticImage* templateWnd)
{
	wnd->setHeight(Absolute, templateWnd->getHeight(Absolute));
	wnd->setWidth(Absolute, templateWnd->getWidth(Absolute));
	wnd->setXPosition(Absolute, templateWnd->getXPosition(Absolute));

	wnd->setBackgroundEnabled(false);
	wnd->setFrameEnabled(false);
	wnd->disable();
	wnd->setImage(templateWnd->getImage());	
}

void KUiQuestManage::hideQuestList()
{
	d_questListPanel->hide();

	for(int i = 0; i < d_questListBtnCount; ++i)
	{
		d_questTitleBtn[i]->destroy();
	}

	d_questDeleteBtn->hide();
	d_questListPanelScrollBar->hide();
	
	d_cancelBtn->hide();
	d_closeBtn->hide();
}

void KUiQuestManage::hideQuestInfo()
{
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

void KUiQuestManage::show()
{
	hideQuestList();
	//printWnd(1, this->m_pThisWnd, "d:\\QuestManager.txt");
	hideQuestInfo();
	KUiWndSingleton<KUiQuestManage>::Show();

	flashQuestList();
	layoutQuestList();
	//打开窗口的时候显示一个任务
	if(!selQuestValidate())
	{
		d_selQuestId = d_questList[0].questId;
	}
	showSelQuest();

	d_cancelBtn->show();
	d_closeBtn->show();
}

void KUiQuestManage::showIfHave()
{
	show();
	if(QUEST_INVALID_ID == d_questList[0].questId)
	{
		hide();
	}
}

void KUiQuestManage::flashQuestList()
{
	static bool mute = false;
	if(mute == true)
		return;
	mute = true;
	
	//任务列表
	for(int i = 0; i < UI_QUESTMANAGE_MAX_QUEST_COUNT; ++i)
	{
		d_questList[i].questId = QUEST_INVALID_ID;
		d_questList[i].questName[0] = 0;
		d_questList[i].questTypeName[0] = 0;
	}

	g_pCoreShell->GetGameData( GDI_GET_QUEST_LIST, (UINT)d_questList, UI_QUESTMANAGE_MAX_QUEST_COUNT);
	
	d_questListBtnCount = 0;
	char initTypeName[COMMON_CLIENT_MSG_LEN_128];
	strcpy(initTypeName, "No Group");
	char tempText[COMMON_CLIENT_MSG_LEN_8];
	for(int j = 0; j < UI_QUESTMANAGE_MAX_QUEST_COUNT; ++j)
	{
		if(d_questList[j].questId != QUEST_INVALID_ID)
		{
			if(strcmp(initTypeName, d_questList[j].questTypeName) != 0)
			{
				strcpy(initTypeName, d_questList[j].questTypeName);

				d_questTitleBtn[d_questListBtnCount] = (TLButton*)WindowManager::getSingleton().createWindow("TaharezLook/Button");
				d_questListPanel->addChildWindow(d_questTitleBtn[d_questListBtnCount]);
				
				//printWnd(1, this->m_pThisWnd, "d:\\QuestManager.txt");
				useTemplate(d_questTitleBtn[d_questListBtnCount], d_questTitleTemplate[0]);
				
				d_questTitleBtn[d_questListBtnCount]->setText(AnsiToUtf8(d_questList[j].questTypeName));

				d_questTitleBtn[d_questListBtnCount]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQuestManage::onClickQuestList, this));
				
				d_questTitleBtn[d_questListBtnCount]->setUserString("T", "R");
				d_questTitleBtn[d_questListBtnCount]->setUserString("Name", d_questTitleBtn[d_questListBtnCount]->getText());
				d_questTitleBtn[d_questListBtnCount]->setUserString("F", "-");
				++d_questListBtnCount;
			}

			d_questTitleBtn[d_questListBtnCount] = (TLButton*)WindowManager::getSingleton().createWindow("TaharezLook/Button");
			d_questListPanel->addChildWindow(d_questTitleBtn[d_questListBtnCount]);
			
			//printWnd(1, this->m_pThisWnd, "d:\\QuestManager.txt");
			useTemplate(d_questTitleBtn[d_questListBtnCount], d_questTitleTemplate[1]);

			d_questTitleBtn[d_questListBtnCount]->setText(AnsiToUtf8(d_questList[j].questName));

			d_questTitleBtn[d_questListBtnCount]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiQuestManage::onClickQuestList, this));
			
			d_questTitleBtn[d_questListBtnCount]->setUserString("T", "N");

			sprintf(tempText, "%d", d_questList[j].questId);
			d_questTitleBtn[d_questListBtnCount]->setUserString("I", tempText);
			++d_questListBtnCount;
		}
	}
	
	mute = false;
	//printWnd(1, this->m_pThisWnd, "d:\\QuestManager.txt");
}

bool KUiQuestManage::onWndShow(const CEGUI::EventArgs& args)
{
	if(KUiNpcMsgBox::IsVisible())
	{
		KUiNpcMsgBox::Hide();
	}

	return true;
}

void KUiQuestManage::layoutQuestList()
{
	int curHideLayer = 1;

	int yPos = 0;

	char name[COMMON_CLIENT_MSG_LEN_32];

	bool showChild = false;

	d_questDeleteBtn->hide();

	for(int i = 0; i < d_questListBtnCount; ++i)
	{
		Window* child = d_questTitleBtn[i];
		child->show();

		if(child->getUserString("T") == String("R"))
		{
			if(child->getUserString("F") == String("+"))
			{
				strcpy(name, "+ ");
				showChild = false;
			}
			else
			{
				strcpy(name, "- ");
				showChild = true;
			}
			strcat(name, Utf8ToAnsi((utf8*)child->getUserString("Name").c_str()));
			child->setText(AnsiToUtf8(name));
		}
		else
		{
			if(false == showChild)
			{
				child->hide();
				continue;
			}
			if(atoi(child->getUserString("I").c_str()) == d_selQuestId)
			{
				d_questDeleteBtn->show();
			}
		}
		child->setYPosition(Absolute, yPos);
		yPos += child->getAbsoluteHeight();
	}

	d_questListPanel->setYPosition(Absolute, 0);
	d_questListPanel->setHeight(Absolute, yPos);
	d_questListPanel->show();

	if(yPos > d_questListMaxHeight)
	{
		float step = (float)d_questTitleTemplate[0]->getAbsoluteHeight() / (yPos - d_questListMaxHeight);
		d_questListPanelScrollBar->setStepSize(step);

		d_questListPanelScrollBar->setScrollPosition(0);
		d_questListPanelScrollBar->show();
	}
}

bool KUiQuestManage::onClickQuestList( const CEGUI::EventArgs& args )
{
	TLButton* clickWnd = (TLButton*)((MouseEventArgs*)&args)->window;

	if(clickWnd->getUserString("T") == String("R"))
	{
		if(clickWnd->getUserString("F") == String("+"))
		{
			clickWnd->setUserString("F", "-");
		}
		else
		{
			clickWnd->setUserString("F", "+");
		}
		layoutQuestList();
	}
	else
	{
		for(int i = 0; i < d_questListBtnCount; ++i)
		{
			d_questTitleBtn[i]->setNormalImage(d_questTitleTemplate[0]->getNormalImage());
		}

		d_selQuestId = atoi(clickWnd->getUserString("I").c_str());
		clickWnd->setNormalImage(d_questTitleTemplate[0]->getDisabledImage());
		
		if(System::getSingleton().isShiftDown())
		{
			KUiQuestTrack::GetSingleton().addTrack(d_selQuestId);
		}
		else
		{
			showSelQuest();
			d_questDeleteBtn->show();
		}
	}
	return true;	
}

bool KUiQuestManage::onListWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(d_questListPanelScrollBar->isVisible())
	{
		d_questListPanelScrollBar->setScrollPosition(d_questListPanelScrollBar->getScrollPosition()
			- d_questListPanelScrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiQuestManage::onQuestInfoWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(d_questInfoPanelScrollBar->isVisible())
	{
		d_questInfoPanelScrollBar->setScrollPosition(d_questInfoPanelScrollBar->getScrollPosition()
			- d_questInfoPanelScrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool tipIsQuest = false;
bool KUiQuestManage::onHoverText(const EventArgs& args)
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
		if(tipIsQuest)
		{
			KUiItemTip::Hide();
			tipIsQuest = false;
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
		tipIsQuest = true;
	}
	else
	{
		if(tipIsQuest)
		{
			KUiItemTip::Hide();
			tipIsQuest = false;
		}
		//KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
	}

	KUiAdapter::SetMouseRes( MOUSE_SUPER_LINK_PLAYER + elemInfo.gameObj._objType - 1 );
	
	return true;
}

bool KUiQuestManage::onLeaveText(const EventArgs& args)
{
	if(tipIsQuest)
	{
		KUiItemTip::Hide();
	}
	KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
	tipIsQuest = false;
	return true;
}

bool KUiQuestManage::onClickQuestInfo( const CEGUI::EventArgs& args )
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
			
			if(elemInfo.gameObj._objId[0] == mapInfo.nSceneId)
			{
				NpcMapPos pos;
				g_pCoreShell->GetGameData(GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&pos, elemInfo.gameObj._objId[1]);
				g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)pos.x, (int)pos.y * 2);
			}
			else
			{
				char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
				KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
			}
		}
		break;
	default:
		break;
	}

	return handled;
}

bool KUiQuestManage::selQuestValidate()
{
	for(int j = 0; j < UI_QUESTMANAGE_MAX_QUEST_COUNT; ++j)
	{
		if(d_questList[j].questId == d_selQuestId && d_selQuestId != QUEST_INVALID_ID)
		{
			return true;
		}
	}
	return false;
}

void KUiQuestManage::showSelQuest()
{
	if(QUEST_INVALID_ID == d_selQuestId)
		return;
	
	hideQuestInfo();

	KQuestInfo questInfo;
	g_pCoreShell->GetGameData( GDI_GET_QUEST_INFO, (UINT)&questInfo, d_selQuestId);

	char loTempStr[COMMON_CLIENT_MSG_LEN_128];

//	strcpy(questInfo.aim, "<Obj type=pic des=1234567 gotype=npc>set:face image:wunai</Obj><Obj type=text vertical-align=bottom >杀掉赵兄\n</Obj>");
	sprintf(d_lomsg, 
		"<Layout width=%d>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"%s"
			"</Seg>"
			"<Seg f=wrap>"
				"<Obj t=text> </Obj>"
			"</Seg>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"%s"
			"</Seg>"
			"<Seg float=wrap>"
				"<Obj v-a=bottom> </Obj>"
			"</Seg>",
			d_questTextLayoutWidth, 
			KUiCfgLoader::getSingleton().getQuestData().descriptionFont,
			KUiCfgLoader::getSingleton().getQuestData().descriptionColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			questInfo.description,
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

			strcat(d_lomsg, "<Seg f=wrap><Obj f-f=");
			strcat(d_lomsg, KUiCfgLoader::getSingleton().getQuestData().requestFont);
			strcat(d_lomsg, " wc=");
			
			if(npcRequire.uCount > npcRequire.uProcess)
			{
				strcat(d_lomsg, KUiCfgLoader::getSingleton().getQuestData().requestNormalColor);
			}
			else
				
			{
				strcat(d_lomsg, KUiCfgLoader::getSingleton().getQuestData().requestCompleteColor);
			}

			sprintf(loTempStr, ">%s", npcRequire.name);
			strcat(d_lomsg, loTempStr);
			strcat(d_lomsg, "</Obj></Seg>");
		}
	}
	strcat(d_lomsg, "</Layout>");

	d_questText->getLayout()->formatText(d_lomsg);
	d_questText->getLayout()->SetText(d_lomsg);
	d_questText->getLayout()->flashLayout();
	d_questText->fitLayoutSize();
	
	Rect textArea = d_questText->getUnclippedInnerRect();

	//裁剪区域必须是相对底板的位置
	Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
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
	if(0 != questInfo.rewardExp)
	{
		haveReward = true;
	}
	if(0 !=questInfo.rewardMoney)
	{
		haveReward = true;
	}
	if(haveReward)
	{
		showMoneyExp(questInfo.rewardMoney, questInfo.rewardExp);
	}
	//奖励物品
  	FIND_ITEMINDEX_PARAM itemidx;
	for (int k = 0; k < questInfo.rewardItemCount; ++k )
	{
		SpliteHashId(questInfo.rewardItem[k].ItemID, itemidx.nGenre, itemidx.nDetail, itemidx.nParticular);

		if ( questInfo.rewardItem[k].ItemType == task_item )
		{
			KItemInfo itemInfo;			
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&itemidx, (int)&itemInfo);
			
			TLGameObject::GameObject goInfo;
			goInfo.d_type			= TLGameObject::item;
			goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
			goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
			goInfo.d_count			= questInfo.rewardItem[k].ItemNum;
			d_itemImage[k]->setObject( goInfo );
			d_itemImage[k]->show();

			KObjAtContRegion* objInfo = (KObjAtContRegion*)d_itemImage[k]->getUserData();
			objInfo->Obj.uGenre = CGOG_ICON;
			objInfo->Region.h = itemidx.nGenre;
			objInfo->Region.v = itemidx.nDetail;
			objInfo->Region.Width = itemidx.nParticular;
			objInfo->Region.Height = itemidx.nLevel;
			haveReward = true;
		}
	}
	//如果金钱、经验或者固定奖励三者有一个显示的话，就显示提示信息
	if(haveReward)
	{
		d_rewardMsgText->show();
	}
	//奖励的可选择物品
	for (int m = 0; m < questInfo.selectRewardItemCount; ++m )
	{
		SpliteHashId(questInfo.selectRewardItem[m].ItemID, itemidx.nGenre, itemidx.nDetail, itemidx.nParticular);

		if ( questInfo.selectRewardItem[m].ItemType == task_item )
		{
			KItemInfo itemInfo;			
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&itemidx, (int)&itemInfo);
			
			TLGameObject::GameObject goInfo;
			goInfo.d_type			= TLGameObject::item;
			goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
			goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
			goInfo.d_count			= questInfo.selectRewardItem[m].ItemNum;
			d_selectItemImage[m]->setObject( goInfo );
			d_selectItemImage[m]->show();

			KObjAtContRegion* objInfo = (KObjAtContRegion*)d_selectItemImage[m]->getUserData();
			objInfo->Obj.uGenre = CGOG_ICON;
			objInfo->Region.h = itemidx.nGenre;
			objInfo->Region.v = itemidx.nDetail;
			objInfo->Region.Width = itemidx.nParticular;
			objInfo->Region.Height = itemidx.nLevel;

			d_rewardSelectMsgText->show();
			haveReward = true;
		}
	}
	if(false == haveReward)
	{
		d_questRewardPanel->hide();
	}
	d_questInfoPanel->show();
	
	layoutQuestInfo();
}

void KUiQuestManage::showMoneyExp(int money, int exp)
{
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
	Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	clipper.setHeight(d_questInfoMaxHeight);

	d_questRewardMoneyText->getLayout()->setClipper(clipper);
	d_questRewardMoneyText->show();
}

void KUiQuestManage::layoutQuestInfo()
{
	//根据任务描述框高度来调整奖励面版的位置
	d_questRewardPanel->setYPosition(Absolute, 
		d_questText->getAbsoluteHeight() + d_questText->getAbsolutePosition().d_y
		+ UI_QUESTMANAGE_COMMON_CTRL_OFFSET);

	int rewardPannelHeight = UI_QUESTMANAGE_COMMON_CTRL_OFFSET;
	if(d_rewardMsgText->isVisible())
	{
		d_rewardMsgText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_rewardMsgText->getAbsoluteHeight() + UI_QUESTMANAGE_COMMON_CTRL_OFFSET;
	}
	
	//经验和金钱
	if(d_questRewardMoneyText->isVisible())
	{
		d_questRewardMoneyText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_questRewardMoneyText->getAbsoluteHeight() + UI_QUESTMANAGE_COMMON_CTRL_OFFSET;
	}

	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		if(d_itemImage[i]->isVisible())
		{
			if(i == 0)
			{
				d_itemImage[i]->setYPosition(Absolute, rewardPannelHeight);
				rewardPannelHeight = d_itemImage[i]->getYPosition(Absolute) + d_itemImage[i]->getAbsoluteHeight()
					 + UI_QUESTMANAGE_COMMON_CTRL_OFFSET;
			}
			else
			{
				d_itemImage[i]->setYPosition(Absolute, d_itemImage[0]->getYPosition(Absolute));
			}
			d_itemFrame[i]->setYPosition(Absolute, d_itemImage[i]->getYPosition(Absolute) + d_itemFrameTemplate->getYPosition(Absolute));
			d_itemFrame[i]->show();
		}
	}
	
	if(d_rewardSelectMsgText->isVisible())
	{
		d_rewardSelectMsgText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_rewardSelectMsgText->getAbsoluteHeight() + UI_QUESTMANAGE_COMMON_CTRL_OFFSET;
	}

	for(int j = 0; j < MAX_QUEST_REWARD_ITEM; ++j)
	{
		if(d_selectItemImage[j]->isVisible())
		{
			if(j == 0)
			{
				d_selectItemImage[j]->setYPosition(Absolute, rewardPannelHeight + UI_QUESTMANAGE_COMMON_CTRL_OFFSET);
				rewardPannelHeight = d_selectItemImage[j]->getYPosition(Absolute) + d_selectItemImage[j]->getAbsoluteHeight();
			}
			else
			{
				d_selectItemImage[j]->setYPosition(Absolute, d_selectItemImage[0]->getYPosition(Absolute));
			}
			d_selectItemFrame[j]->setYPosition(Absolute, d_selectItemImage[j]->getYPosition(Absolute) + d_itemFrameTemplate->getYPosition(Absolute));
			d_selectItemFrame[j]->show();
		}
	}

	d_questRewardPanel->setHeight(Absolute, rewardPannelHeight + UI_QUESTMANAGE_COMMON_CTRL_OFFSET);

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

void KUiQuestManage::doDeleteQuest()
{
	g_pCoreShell->OperationRequest( GOI_DELETE_QUEST, ms_Singleton->d_selQuestId, NULL );
}

bool KUiQuestManage::onDeleteQuest(const CEGUI::EventArgs& args)
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

bool KUiQuestManage::onTrackQuest(const CEGUI::EventArgs& args)
{
	KUiQuestTrack::GetSingleton().addTrack(d_selQuestId);
	return true;
}

bool KUiQuestManage::onCancel(const CEGUI::EventArgs& args)
{
	m_pThisWnd->hide();
	return true;
}

bool KUiQuestManage::onQuestListPanelScroll(const CEGUI::EventArgs& args)
{
	float scrollPos = d_questListPanelScrollBar->getScrollPosition();
	int ctrlHeight = d_questListPanel->getHeight(Absolute);
	int exceedSize = (ctrlHeight - d_questListMaxHeight) * scrollPos;
	d_questListPanel->setYPosition(Absolute, -exceedSize);
	return true;
}

bool KUiQuestManage::onQuestInfoPanelScroll(const CEGUI::EventArgs& args)
{
	float scrollPos = d_questInfoPanelScrollBar->getScrollPosition();
	int ctrlHeight = d_questInfoPanel->getHeight(Absolute);
	int exceedSize = (ctrlHeight - d_questInfoMaxHeight) * scrollPos;
	d_questInfoPanel->setYPosition(Absolute, -exceedSize);
	return true;
}

void KUiQuestManage::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		getChild();
	}
}
void KUiQuestManage::hide()
{
	m_pThisWnd->hide();
}

void KUiQuestManage::onQuestListChange()
{
	if(!m_pThisWnd->isVisible())
		return;
	
	show();
}

void KUiQuestManage::onQuestChange( int questId )
{
	if(d_selQuestId == questId)
	{
		showSelQuest();
	}
}