///////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 11:20
//      File_base        : UiMessageBox
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)--小雨
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "kwin32.h"
#include "UiNpcMsgBox.h"
#include "CoreShell.h"
#include "TLGameObject.h"
#include "layoutinterface.h"
#include "UiChatWindow.h"
#include "UiQuestManage.h"
#include "UiDragItem.h"
#include "UiNpcNavigation.h"
#include "UiFSBible.h"
#include "../UiConfigManager.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiNpcMsgBox* KUiWndSingleton<KUiNpcMsgBox>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiNpcMsgBox::KUiNpcMsgBox( const CEGUI::String& id_name ):
KUiWndSingleton<KUiNpcMsgBox>( id_name )
{
	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		d_itemImage[i] = NULL;
		d_selectItemImage[i] = NULL;
	}
	for(int j = 0; j < MAX_QUEST_REQUIRED_ITEM; ++j)
	{
		d_requireItem[j] = NULL;
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiNpcMsgBox::~KUiNpcMsgBox()
{

}

void KUiNpcMsgBox::getChild()
{
	char ctrlName[COMMON_CLIENT_MSG_LEN_128];

	//滚动条
	d_scrollBar = (TLVertScrollbar*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/Scrollbar");
	d_scrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiNpcMsgBox::scroll, this));
	
	//裁减窗口
	d_clipper = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/ClipperWindow");
	d_clipperHeightAtRequirPanelShow = d_clipper->getHeight(Absolute);
	d_clipper->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiNpcMsgBox::handleWheelChanged, this));

	d_ctrlPanel = (TLStaticImage*)d_clipper->getChild("TaharezLook/NpcMsgBox/ClipperWindow/CtrlPannel");

	//任务内容（使用排版）
	d_contentText = (TLStaticText*)d_ctrlPanel->getChild("TaharezLook/NpcMsgBox/ClipperWindow/CtrlPannel/MsgContent");
	
	d_contentText->setMetricsMode(Absolute);
	d_contentText->useLayout();
	//设置排版相对于控件的偏移（主要处理有边框的情况）
	if(d_contentText->isFrameEnabled())
	{
		d_contentLayoutWidth = d_contentText->getUnclippedInnerRect().getWidth()
					- d_contentText->getLeftFrameWidth() - d_contentText->getRightFrameWidth();
		d_contentText->setLayoutOffset(d_contentText->getLeftFrameWidth(), d_contentText->getTopFrameHeight());
	}
	else
	{
		d_contentLayoutWidth = d_contentText->getUnclippedInnerRect().getWidth();
		d_contentText->setLayoutOffset(0, 0);
	}
	
	//可对话按钮
	d_questPanel = (TLStaticImage*)d_ctrlPanel->getChild("TaharezLook/NpcMsgBox/ClipperWindow/CtrlPannel/QuestList");
	for(int j = 0; j < UI_NPCBOX_MAX_QUEST_COUNT; ++j)
	{
		sprintf( ctrlName, "TaharezLook/NpcMsgBox/ClipperWindow/CtrlPannel/QuestList/Quest%d", j + 1);
		d_questBtn[j] = (TLButton*)d_questPanel->getChild(ctrlName);
		d_questBtn[j]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::handleSelectQuest, this));
		d_questBtn[j]->hide();
		d_questBtn[j]->setTextHFormatting(LeftAligned);

		sprintf( ctrlName, "TaharezLook/NpcMsgBox/ClipperWindow/CtrlPannel/QuestList/Title%d", j + 1);
		d_questTitleImage[j] = (TLStaticImage*)d_questPanel->getChild(ctrlName);
		d_questTitleImage[j]->hide();
	}
	
	//经验和金钱
	String rewardPannelPath = "TaharezLook/NpcMsgBox/ClipperWindow/CtrlPannel/RewardPanel";
	d_rewardPanel = (TLStaticImage*)d_ctrlPanel->getChild(rewardPannelPath);
	d_moneyText	= (TLStaticText*)d_rewardPanel->getChild(rewardPannelPath + "/Money");

	d_itemFrameTemplate = (TLStaticImage*)d_rewardPanel->getChild(rewardPannelPath + "/ItemFrame");
	d_itemFrameTemplate->hide();
	//任务奖励物品（使用TLGameObject）
	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		//固定奖励
		sprintf( ctrlName, "/Item%d", i);
		d_itemImage[i] = (TLGameObject*)d_rewardPanel->getChild(rewardPannelPath + ctrlName);
		
		KObjAtContRegion* newObjInfo = &d_rewardObjInfo[i];
		newObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_itemImage[i]->setUserData(newObjInfo);
		
		d_itemImageGrid[i].setCtrl(d_itemImage[i]);
		d_itemImageGrid[i].addTip();
		
		//可选奖励
		sprintf( ctrlName, "/SelectItem%d", i);
		d_selectItemImage[i] = (TLGameObject*)d_rewardPanel->getChild(rewardPannelPath + ctrlName);
		
		KObjAtContRegion* newSelectObjInfo = &d_selectRewardObjInfo[i];
		newSelectObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_selectItemImage[i]->setUserData(newSelectObjInfo);

		d_selectItemImage[i]->setUserString("N", iToString(i + 1));
		d_selectItemImage[i]->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::handleSelectItem, this));
		d_selectItemImageGrid[i].setCtrl(d_selectItemImage[i]);
		d_selectItemImageGrid[i].addTip();

		d_itemFrame[i] = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
		useTemplate(d_itemFrame[i], d_itemFrameTemplate);
		d_rewardPanel->addChildWindow(d_itemFrame[i]);
		d_itemFrame[i]->setXPosition(Absolute, d_itemImage[i]->getXPosition(Absolute) + d_itemFrameTemplate->getXPosition(Absolute));
		
		d_selectItemFrame[i] = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
		useTemplate(d_selectItemFrame[i], d_itemFrameTemplate);
		d_rewardPanel->addChildWindow(d_selectItemFrame[i]);
		d_selectItemFrame[i]->setXPosition(Absolute, d_selectItemImage[i]->getXPosition(Absolute) + d_itemFrameTemplate->getXPosition(Absolute));
	}
	d_rewardMsgText
		= (TLStaticText*)d_rewardPanel->getChild(rewardPannelPath + "/RewardMsg");
	d_rewardSelectMsgText
		= (TLStaticText*)d_rewardPanel->getChild(rewardPannelPath + "/RewardSelectMsg");
	
	//需求物品

	d_requiredPanel = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/RequiredPanel");
	for(int k = 0; k < MAX_QUEST_REQUIRED_ITEM; ++k)
	{
		sprintf(ctrlName, "TaharezLook/NpcMsgBox/RequiredPanel/RequiredItem%d", k);
		d_requireItem[k] = (TLGameObject*)d_requiredPanel->getChild(ctrlName);
		d_requireItem[k]->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::dropRequiredItem, this));

		KObjAtContRegion* newObjInfo = &d_requireItemObjInfo[k];
		newObjInfo->Obj.uGenre = CGOG_NOTHING;
		d_requireItem[k]->setUserData(newObjInfo);
		
		d_requireItemGrid[k].setCtrl(d_requireItem[k]);
		d_requireItemGrid[k].addTip();
	}

	//最下面的三个按钮
	d_submitBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/Submit");
	d_acceptBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/Accept");
	d_cancelBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/Cancel");
	d_closeBtn	= (TLButton*)m_pThisWnd->getChild("TaharezLook/NpcMsgBox/Close");

	d_submitBtn->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::handleOk, this));
	d_acceptBtn->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::handleOk, this));
	d_cancelBtn->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::handleCannel, this));
	d_closeBtn->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiNpcMsgBox::handleExit, this));
	
	//窗口打开的时候
	m_pThisWnd->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiNpcMsgBox::onWndShow, this));

	Window* helpWindow = m_pThisWnd->getChild("TaharezLook/NpcMsgBox_HelpBtn");
	helpWindow->hide();
	helpWindow->disable();
}

void KUiNpcMsgBox::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		getChild();
		d_curSelItem = NULL;
	}
}

void KUiNpcMsgBox::useTemplate(StaticImage* wnd, StaticImage* templateWnd)
{
	wnd->setHeight(Absolute, templateWnd->getHeight(Absolute));
	wnd->setWidth(Absolute, templateWnd->getWidth(Absolute));
	wnd->setXPosition(Absolute, templateWnd->getXPosition(Absolute));

	wnd->setBackgroundEnabled(false);
	wnd->setFrameEnabled(false);
	wnd->disable();
	wnd->setImage(templateWnd->getImage());	
}

bool KUiNpcMsgBox::onWndShow( const CEGUI::EventArgs& args )
{
	if(KUiFSBible::getSingleton().isVisible())
	{
		KUiFSBible::getSingleton().hide();
	}
	if(KUiNpcNavigation::getSingleton().isVisible())
	{
		KUiNpcNavigation::getSingleton().hide();
	}
	return true;
}

void KUiNpcMsgBox::hideAllCtrl()
{
	d_scrollBar->hide();

	//任务描述或NPC说的话
	d_contentText->hide();

	//任务面版
	d_questPanel->hide();
	for(int j = 0; j < UI_NPCBOX_MAX_QUEST_COUNT; ++j)
	{
		d_questBtn[j]->hide();
		d_questTitleImage[j]->hide();
	}

	//任务奖励物品
	//奖励面版
	d_rewardPanel->hide();

	d_rewardMsgText->hide();
	d_rewardSelectMsgText->hide();
	
	d_moneyText->hide();

	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		//固定奖励和可选奖励
		d_itemImage[i]->hide();
		d_selectItemImage[i]->hide();
		d_itemFrame[i]->hide();
		d_selectItemFrame[i]->hide();
	}
	if(d_curSelItem != NULL)
	{
		d_curSelItem->setState(TLGameObject::normalState);
		d_curSelItem = NULL;
	}

	d_requiredPanel->hide();
	for(int k = 0; k < MAX_QUEST_REQUIRED_ITEM; ++k)
	{
		TLGameObject::GameObject objInfo;
		objInfo.d_gameobject = BACKGROUND_IMAGE;
		objInfo.d_type = TLGameObject::idle;
		d_requireItem[k]->setObject(objInfo);
	}

	//最下面的三个按钮
	d_submitBtn->hide();
	d_acceptBtn->hide();
	d_cancelBtn->hide();
	d_closeBtn->hide();
}

const Image* KUiNpcMsgBox::getTitleImage(const char* imagePath)
{
	if (imagePath[0] == 0)
	{
		return NULL;
	}
	char imageSet[COMMON_CLIENT_MSG_LEN_128] = {0};
	char imageName[COMMON_CLIENT_MSG_LEN_128] = {0};
				
	sscanf(imagePath, "set:%127s image:%127s", imageSet, imageName);

	const Image* image;

	try
	{
		if(0 != imageSet[0])
		{
			image = &ImagesetManager::getSingleton().getImageset(imageSet)->getImage(imageName);
		}
		else
		{
			if(ImagesetManager::getSingleton().isImagesetPresent(imagePath) == true)
			{
				image = &ImagesetManager::getSingleton().getImageset(imagePath)->getImage("full_image");
			}
			else
			{
				image = &ImagesetManager::getSingleton().createImagesetFromImageFile(imagePath, imagePath)->getImage("full_image");
			}			
		}
	}
	catch (UnknownObjectException)
	{
		image = NULL;
	}
	
	return image;
}

void KUiNpcMsgBox::OpenNpcTalk(const KUiQuestionAndAnswer* npcTalkMessage)
{
	if(NULL == npcTalkMessage)
		return;

	KUiWndSingleton<KUiNpcMsgBox>::Show();

	hideAllCtrl();
	d_cancelBtn->show();
	
	//显示NPC要说的话
	if(npcTalkMessage->Question[0] == '<')
	{
		sprintf(d_lomsg, 
		"<Layout width=%d>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"%s"
			"</Seg>"
		"</Layout>", 
		d_contentLayoutWidth, 
		KUiCfgLoader::getSingleton().getNpcMsgData().npcMsgFont,
		KUiCfgLoader::getSingleton().getNpcMsgData().npcMsgColor,
		KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
		KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
		npcTalkMessage->Question);
	}
	else
	{
		sprintf(d_lomsg, 
		"<Layout width=%d>"
			"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
				"<Obj>%s</Obj>"
			"</Seg>"
		"</Layout>", 
		d_contentLayoutWidth, 
		KUiCfgLoader::getSingleton().getNpcMsgData().npcMsgFont,
		KUiCfgLoader::getSingleton().getNpcMsgData().npcMsgColor,
		KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
		KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
		npcTalkMessage->Question);
	}

	d_contentText->getLayout()->SetText(d_lomsg);
	d_contentText->getLayout()->flashLayout();

	d_contentText->show();

	for(int j = 0; j < UI_NPCBOX_MAX_QUEST_COUNT; ++j)
	{
		if(j < npcTalkMessage->AnswerCount)
		{
			int imageId = npcTalkMessage->Answer[j].imageId;
			const map<int, string>& commImageCfg = KUiCfgLoader::getSingleton().getCommImage();
			map<int, string>::const_iterator curIt = commImageCfg.find(imageId);
			if(curIt != commImageCfg.end())
			{
				const Image* image = getTitleImage(curIt->second.c_str());
				d_questTitleImage[j]->setImage(image);
				d_questTitleImage[j]->show();
			}
			else
			{
				d_questTitleImage[j]->hide();
			}
			d_questBtn[j]->setText(AnsiToUtf8(npcTalkMessage->Answer[j].AnswerText));
			d_questBtn[j]->show();
		}
		else
		{
			d_questBtn[j]->hide();
			d_questTitleImage[j]->hide();
		}
	}
	d_questPanel->show();

	layoutNpcTalk();
	
	d_closeBtn->show();
}

void KUiNpcMsgBox::layoutNpcTalk()
{
	//NPC消息的高度
	d_contentText->fitLayoutSize();
	//设置NPC消息的layout裁减区域
	Rect textArea = d_clipper->getUnclippedInnerRect();
	//裁剪区域必须是相对底板的位置
	Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	d_contentText->getLayout()->setClipper(clipper);

	int ctrlHeight = d_contentText->getAbsoluteHeight() + d_contentText->getAbsolutePosition().d_y;
	
	//根据NPC消息的高度来得到对话按钮栏的位置
	d_questPanel->setYPosition(Absolute, ctrlHeight + UI_NPCBOX_QUESTCTRL_TOP_OFFSET);

	//对话按钮面版的高度
	d_questPanel->setHeight(Absolute, 0);
	for ( int j = 0; j < UI_NPCBOX_MAX_QUEST_COUNT; ++j )
	{
		if(d_questBtn[j]->isVisible())
		{
			d_questPanel->setHeight(Absolute, d_questPanel->getHeight(Absolute) + d_questBtn[j]->getHeight(Absolute));
		}
	}
	
	//所有内容高度
	ctrlHeight = d_questPanel->getAbsoluteHeight() + d_questPanel->getAbsolutePosition().d_y;
	d_ctrlPanel->setHeight(Absolute, ctrlHeight);
	d_ctrlPanel->setYPosition(Absolute, 0);

	//是否显示滚动条
	int clipperHeight = d_clipper->getAbsoluteHeight();
	if(clipperHeight < ctrlHeight)
	{
		d_scrollBar->show();
		d_scrollBar->setScrollPosition(0);

		float step = (float)clipperHeight / (2 * (ctrlHeight - clipperHeight));
		d_scrollBar->setStepSize(step);
	}
}

void KUiNpcMsgBox::OpenNpcMission(const KQuestInfo* questParam)
{
	if(NULL == questParam)
		return;
	
	KUiWndSingleton<KUiNpcMsgBox>::Show();

	//先隐藏所有控件
	hideAllCtrl();
		
	const char* npcSpeakmsg = NULL;
	if(questParam->state == accept_quest)
	{
		npcSpeakmsg = questParam->description;
		d_acceptBtn->show();
		d_cancelBtn->show();
		d_requiredPanel->hide();
	}
	else if(questParam->state == incomplete_quest)
	{
		npcSpeakmsg = questParam->incomplete;
		d_cancelBtn->show();
		d_requiredPanel->hide();
	}
	else if(questParam->state == complete_quest)
	{
		npcSpeakmsg = questParam->completeMsg;
		d_submitBtn->show();
        if (questParam->showItemBox)
        {
            d_requiredPanel->show();
        }
        else
        {
            d_requiredPanel->hide();
        }
	}
	d_questState = (KQuestState)questParam->state;

	const KUiCfgLoader::QuestCfgData& questCfg = KUiCfgLoader::getSingleton().getQuestData();

	//显示任务描述
	if(npcSpeakmsg[0] == '<' && questParam->aim[0] == '<')
	{
		sprintf(d_lomsg, 
			"<Layout width=%d>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"%s"
				"</Seg>"
				"<Seg f=wrap>"
					"<Obj> </Obj>"
				"</Seg>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"%s"
				"</Seg>"
			"</Layout>", 
			d_contentLayoutWidth, 
			questCfg.descriptionFont,
			questCfg.descriptionColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			npcSpeakmsg,
			questCfg.aimFont,
			questCfg.aimColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			questParam->aim);
	}
	else if(npcSpeakmsg[0] == '<' && questParam->aim[0] != '<')
	{
		sprintf(d_lomsg, 
			"<Layout width=%d>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"%s"
				"</Seg>"
				"<Seg f=wrap>"
					"<Obj> </Obj>"
				"</Seg>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"<Obj>%s</Obj>"
				"</Seg>"
			"</Layout>",
			d_contentLayoutWidth, 
			questCfg.descriptionFont,
			questCfg.descriptionColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			npcSpeakmsg,
			questCfg.aimFont,
			questCfg.aimColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			questParam->aim);
	}
	else if(npcSpeakmsg[0] != '<' && questParam->aim[0] == '<')
	{
		sprintf(d_lomsg, 
			"<Layout width=%d>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"<Obj>%s</Obj>"
				"</Seg>"
				"<Seg f=wrap>"
					"<Obj> </Obj>"
				"</Seg>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"%s"
				"</Seg>"
			"</Layout>", 
			d_contentLayoutWidth, 
			questCfg.descriptionFont,
			questCfg.descriptionColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			npcSpeakmsg,
			questCfg.aimFont,
			questCfg.aimColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			questParam->aim);
	}
	else
	{
		sprintf(d_lomsg, 
			"<Layout width=%d>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"<Obj>%s</Obj>"
				"</Seg>"
				"<Seg f=wrap>"
					"<Obj> </Obj>"
				"</Seg>"
				"<Seg f=wrap f-f=%s wc=%s l-e=%d w-e=%d>"
					"<Obj>%s</Obj>"
				"</Seg>"
			"</Layout>",  
			d_contentLayoutWidth, 
			questCfg.descriptionFont,
			questCfg.descriptionColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			npcSpeakmsg,
			questCfg.aimFont,
			questCfg.aimColor,
			KUiCfgLoader::getSingleton().getQuestData().lineExtSpace,
			KUiCfgLoader::getSingleton().getQuestData().wordExtSpace,
			questParam->aim);
	}
	
	d_contentText->getLayout()->formatText(d_lomsg);
	d_contentText->getLayout()->SetText(d_lomsg);
	d_contentText->getLayout()->flashLayout();
	d_contentText->fitLayoutSize();
	Rect textArea = d_clipper->getUnclippedInnerRect();

	//裁剪区域必须是相对底板的位置
	Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	d_contentText->getLayout()->setClipper(clipper);
	d_contentText->show();

	//任务奖励
	d_rewardPanel->show();
	bool haveReward = false;
	//金钱和经验
	if(0 != questParam->rewardExp)
	{
		haveReward = true;
	}
	if(0 != questParam->rewardMoney)
	{
		haveReward = true;
	}
	if(haveReward)
	{
		showMoneyExp(questParam->rewardMoney, questParam->rewardExp);
	}
	//奖励物品
  	FIND_ITEMINDEX_PARAM itemidx;
	for(int i = 0; i < questParam->rewardItemCount; ++i )
	{
		SpliteHashId(questParam->rewardItem[i].ItemID, itemidx.nGenre, itemidx.nDetail, itemidx.nParticular);

		if ( questParam->rewardItem[i].ItemType == task_item )
		{
			KItemInfo itemInfo;
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&itemidx, (int)&itemInfo);
			
			TLGameObject::GameObject goInfo;
			goInfo.d_type			= TLGameObject::item;
			goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
			goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
			goInfo.d_count			= questParam->rewardItem[i].ItemNum;
			d_itemImage[i]->setObject( goInfo );
			d_itemImage[i]->show();

			KObjAtContRegion* objInfo = (KObjAtContRegion*)d_itemImage[i]->getUserData();
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
	for (int j = 0; j < questParam->selectRewardItemCount; ++j )
	{
		SpliteHashId(questParam->selectRewardItem[j].ItemID, itemidx.nGenre, itemidx.nDetail, itemidx.nParticular);

		if ( questParam->selectRewardItem[j].ItemType == task_item )
		{
			KItemInfo itemInfo;			
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&itemidx, (int)&itemInfo);
			
			TLGameObject::GameObject goInfo;
			goInfo.d_type			= TLGameObject::item;
			goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
			goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
			goInfo.d_count			= questParam->selectRewardItem[j].ItemNum;
			d_selectItemImage[j]->setObject( goInfo );
			d_selectItemImage[j]->show();
			
			KObjAtContRegion* objInfo = (KObjAtContRegion*)d_selectItemImage[j]->getUserData();
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
		d_rewardPanel->hide();
	}

	//根据刚才的显示情况，调整控件之间的位置
	layoutQuest();

	d_closeBtn->show();
}

void KUiNpcMsgBox::showMoneyExp(int money, int exp)
{
	const KUiCfgLoader::QuestCfgData& questCfg = KUiCfgLoader::getSingleton().getQuestData();
	
	char tempText[COMMON_CLIENT_MSG_LEN_512] = "\0";
	char loText[MAX_TEXT_LEN] = "\0";
	sprintf(tempText, "<Layout width=%d>", d_contentLayoutWidth);
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
		
	d_moneyText->useLayout();
	d_moneyText->getLayout()->SetText(loText);
	d_moneyText->getLayout()->flashLayout();
	d_moneyText->fitLayoutSize();

	Rect clipArea = d_clipper->getUnclippedInnerRect();
	LORect clipper;
	cerectToLorect(&clipArea, &clipper);

//	d_moneyText->getLayout()->setClipper(clipper);
	d_moneyText->show();
}

void KUiNpcMsgBox::layoutQuest()
{
	//根据任务描述框高度来调整奖励面版的位置
	d_rewardPanel->setYPosition(Absolute, 
		d_contentText->getAbsoluteHeight() + d_contentText->getAbsolutePosition().d_y
		+ UI_NPCBOX_QUESTCTRL_TOP_OFFSET);

	int rewardPannelHeight = UI_NPCBOX_QUESTCTRL_TOP_OFFSET;
	if(d_rewardMsgText->isVisible())
	{
		d_rewardMsgText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_rewardMsgText->getAbsoluteHeight() + UI_NPCBOX_QUESTCTRL_TOP_OFFSET;
	}
	//经验和金钱
	if(d_moneyText->isVisible())
	{
		d_moneyText->setYPosition(Absolute, rewardPannelHeight);
		rewardPannelHeight += d_moneyText->getAbsoluteHeight() + UI_NPCBOX_QUESTCTRL_TOP_OFFSET;

	}
	
	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		if(d_itemImage[i]->isVisible())
		{
			if(i == 0)
			{
				d_itemImage[i]->setYPosition(Absolute, rewardPannelHeight + UI_NPCBOX_QUESTCTRL_TOP_OFFSET);
				rewardPannelHeight = d_itemImage[i]->getYPosition(Absolute) + d_itemImage[i]->getAbsoluteHeight();
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
		d_rewardSelectMsgText->setYPosition(Absolute, rewardPannelHeight + UI_NPCBOX_QUESTCTRL_TOP_OFFSET);
		rewardPannelHeight += d_rewardSelectMsgText->getAbsoluteHeight() + UI_NPCBOX_QUESTCTRL_TOP_OFFSET;
	}

	for(int j = 0; j < MAX_QUEST_REWARD_ITEM; ++j)
	{
		if(d_selectItemImage[j]->isVisible())
		{
			if(j == 0)
			{
				d_selectItemImage[j]->setYPosition(Absolute, rewardPannelHeight + UI_NPCBOX_QUESTCTRL_TOP_OFFSET);
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

	d_rewardPanel->setHeight(Absolute, rewardPannelHeight + UI_NPCBOX_QUESTCTRL_TOP_OFFSET);

	int ctrlHeight = d_rewardPanel->getAbsoluteHeight() + d_rewardPanel->getAbsolutePosition().d_y;
	
	//所有内容的高度
	d_ctrlPanel->setHeight(Absolute, ctrlHeight);
	d_ctrlPanel->setYPosition(Absolute, 0);

	//根据所有内容高度和控件高度来决定是否显示滑动条
	if(d_requiredPanel->isVisible())
	{
		d_clipper->setHeight(Absolute, d_clipperHeightAtRequirPanelShow);
	}
	else
	{
		d_clipper->setHeight(Absolute, d_clipperHeightAtRequirPanelShow + d_requiredPanel->getHeight(Absolute));
	}
	int clipperHeight = d_clipper->getAbsoluteHeight();
	if(clipperHeight < ctrlHeight)
	{
		d_scrollBar->show();
		d_scrollBar->setScrollPosition(0);

		float step = (float)clipperHeight / (2 * (ctrlHeight - clipperHeight));
		d_scrollBar->setStepSize(step);
	}
}

bool KUiNpcMsgBox::scroll(const EventArgs& args)
{
	float scrollPos = d_scrollBar->getScrollPosition();
	int ctrlHeight = d_ctrlPanel->getHeight(Absolute);
	int exceedSize = (ctrlHeight - d_clipper->getAbsoluteHeight()) * scrollPos;
	d_ctrlPanel->setYPosition(Absolute, -exceedSize);
	return true;
}

bool KUiNpcMsgBox::handleWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(d_scrollBar->isVisible())
	{
		d_scrollBar->setScrollPosition(d_scrollBar->getScrollPosition() - d_scrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiNpcMsgBox::handleOk( const CEGUI::EventArgs& args )
{
	KQuestRequest requestResult;
	requestResult.eOperType	= operation_ok;
	
	requestResult.uParam	= 0;
	requestResult.uParamEx	= 0;
	if(complete_quest == d_questState && d_selectItemImage[0]->isVisible())
	{
		if(NULL == d_curSelItem)
		{
			KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getNpcMsgData().commitQuestNotSelectErrMsg);
			return true;
		}
		else
		{
			requestResult.uParam = atoi(d_curSelItem->getUserString("N").c_str());
		}
	}

	for(int i = 0; i < MAX_QUEST_REQUIRED_ITEM; ++i)
	{
		TLGameObject::GameObject& objInfo = d_requireItem[i]->getObject();
		if(objInfo.d_type != TLGameObject::item)
		{
			continue;
		}
		KObjAtContRegion* pos = (KObjAtContRegion*)d_requireItem[i]->getUserData();
		int itemIndex = pos->Obj.uId;
		requestResult.requiredItemIndex.push_back(itemIndex);
	}
	g_pCoreShell->OperationRequest(GOI_OK_QUEST, (unsigned int)&requestResult, NULL);
	return true;
}

bool KUiNpcMsgBox::handleCannel( const CEGUI::EventArgs& args )
{
	KUiNpcMsgBox::Hide();
	return true;
}

bool KUiNpcMsgBox::handleExit( const CEGUI::EventArgs& args	)
{
	KUiNpcMsgBox::Hide();
	return true;
}	

bool KUiNpcMsgBox::handleSelectQuest( const CEGUI::EventArgs& args )
{	
	Window* clickWnd = ((MouseEventArgs*)&args)->window;
	for(int i = 0; i < UI_NPCBOX_MAX_QUEST_COUNT; ++i)
	{
		if(clickWnd == d_questBtn[i])
		{
			g_pCoreShell->OperationRequest( GOI_QUEST_REQUEST, NULL, i);
			break;
		}
	}
	return true;	
}


bool KUiNpcMsgBox::handleSelectItem( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWnd = (WindowEventArgs*)&args;
	TLGameObject* ctrl = (TLGameObject*)pWnd->window;
	
	if(d_curSelItem != NULL)
	{
		d_curSelItem->setState(TLGameObject::normalState);
		d_curSelItem = NULL;
	}
	if(complete_quest == d_questState)
	{
		ctrl->setState(TLGameObject::selectState);
		d_curSelItem = ctrl;
	}
	
	return true;
}

bool KUiNpcMsgBox::dropRequiredItem( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWnd = (WindowEventArgs*)&args;
	TLGameObject* destObj = (TLGameObject*)pWnd->window;
	TLGameObject::GameObject& destObjInfo = destObj->getObject();
	KObjAtContRegion* destPos = (KObjAtContRegion*)destObj->getUserData();
	
	TLGameObject*sourObj = KUiDragItem::GetSingleton().getObj();

	TLGameObject::GameObject& sourObjInfo = sourObj->getObject();
	KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();
		


	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		if(destObjInfo.d_type == TLGameObject::item)
		{
			sourObj->setObject(destObjInfo);
			*sourPos = *destPos;
			sourObj->setCanDrag(true);

			TLGameObject::GameObject objInfo;
			objInfo.d_gameobject = BACKGROUND_IMAGE;
			objInfo.d_type = TLGameObject::idle;
			destObj->setObject(objInfo);
			destPos->Obj.uGenre = CGOG_NOTHING;
		}
	}
	else//手上拿着的东西为item或者其他东西
	{
		//手上拿着的只能是物品栏中的物品
		if(UOC_ITEM_TAKE_WITH != sourPos->eContainer)
		{
			return false;
		}
		
		//手里拿的物品不能是已经放上去的
		for(int i = 0; i < MAX_QUEST_REQUIRED_ITEM; ++i)
		{
			TLGameObject::GameObject& objInfo = d_requireItem[i]->getObject();
			if(objInfo.d_type != TLGameObject::item)
			{
				continue;
			}
			KObjAtContRegion* pos = (KObjAtContRegion*)d_requireItem[i]->getUserData();
			if(sourPos->Obj.uId == pos->Obj.uId)
			{
				return false;
			}
		}

		if(destObjInfo.d_type == TLGameObject::item)
		{
			KObjAtContRegion tempPos;
			sourObj->setObject(destObjInfo);
			destObj->setObject(sourObjInfo);
			tempPos = *sourPos;
			*sourPos = *destPos;
			*destPos = tempPos;
		}
		else
		{
			destObj->setObject(sourObjInfo);
			*destPos = *sourPos;

			TLGameObject::GameObject objInfo;
			objInfo.d_gameobject = BACKGROUND_IMAGE;
			objInfo.d_type = TLGameObject::idle;
			sourObj->setObject(objInfo);
		}
	}

	return true;
}