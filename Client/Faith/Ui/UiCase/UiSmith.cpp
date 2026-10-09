
#include "crtdbg.h"
#include "UiSmith.h"
#include "CoreShell.h"
#include "layoutinterface.h"
#include "ItemCommonDef.h"
#include "UiDragItem.h"
#include "UiChatWindow.h"
#include "..\UiConfigManager.h"
#include "UiLinkedItemTip.h"
#include "UiTipGenerator.h"
#include "UiItemLockMgr.h"

using namespace CEGUI;

extern iCoreShell* g_pCoreShell;

KUiSmith::KUiSmith()
{
	loadUi();
}

KUiSmith::~KUiSmith()
{

}

KUiSmith& KUiSmith::getSingleton()
{
	static KUiSmith singleton;
	return singleton;
}

bool KUiSmith::isVisible()
{
	return _thisWindow->isVisible();
}

void KUiSmith::hide()
{
	_thisWindow->hide();
}

void KUiSmith::show()
{
	_thisWindow->show();
}

void KUiSmith::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SMITH_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SMITH_WINDOW_PATH);
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
	
	for(int j = 0; j < UI_SMITH_TYPE1_COUNT; ++j)
	{
		d_type1SelectBtn[j] = (TLRadioButton*)_thisWindow->getChild("TaharezLook/Smith/SmithBtn" + iToString(j + 1));
		d_type1SelectBtn[j]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiSmith::selectType1, this));
		d_type1SelectBtn[j]->hide();
	}
	d_skillListPannel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Smith/ItemListPanel");

	d_smithItems = (TLTree*)d_skillListPannel->getChild("TaharezLook/Smith/ItemListPanel/SmithItems");
	d_smithItems->setHeight(Absolute, 10000);
	d_smithItems->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiSmith::selectType23, this));
	d_smithItems->subscribeEvent(TLStaticImage::TR_EventBranchOpened, Event::Subscriber(&KUiSmith::onType2OpenClose, this));
	d_smithItems->subscribeEvent(TLStaticImage::TR_EventBranchClosed, Event::Subscriber(&KUiSmith::onType2OpenClose, this));
	d_smithItems->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiSmith::onWheel, this));
	d_smithItems->setSortingEnabled(false);
	
	d_type4Btn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/Type4Btn");
	d_type4Btn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSmith::openType4List, this));
	
	d_type4Pannel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Smith/Type4Pannel");
	d_type4List = (TLTree*)d_type4Pannel->getChild("TaharezLook/Smith/Type4Pannel/List");
	d_type4List->subscribeEvent(Window::TR_EventSelectionChanged, Event::Subscriber(&KUiSmith::selectType4, this));

	d_type4Btn->setZLevel(Window::Top);
	d_type4Pannel->setZLevel(Window::Top);
	d_type4List->setZLevel(Window::Top);

	d_openAllBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/OpenAll");
	d_foldAllBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/FoldAll");
	d_openAllBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiSmith::fold, this));
	d_foldAllBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiSmith::fold, this));
	d_openAllBtn->show();
	d_foldAllBtn->hide();
	
	d_scrollBar = (TLVertScrollbar*)_thisWindow->getChild("TaharezLook/Smith/Scrollbar");
	d_scrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiSmith::scroll, this));
	
	d_icon = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Smith/Icon");
	d_icon->subscribeEvent(TLStaticText::EventMouseMove, Event::Subscriber(&KUiSmith::onHoverIcon, this));
	d_icon->subscribeEvent(TLStaticText::EventMouseLeaves, Event::Subscriber(&KUiSmith::onLevaeIcon, this));
	
	d_moneyRequireText = (TLStaticText*)_thisWindow->getChild("TaharezLook/Smith/MoneyRequire");
	d_moneyRequireText->useLayout();

	d_smithRate = (TLStaticText*)_thisWindow->getChild("TaharezLook/Smith/SmithRate");
	d_smithRate->subscribeEvent(TLStaticText::EventMouseMove, Event::Subscriber(&KUiSmith::onHoverRate, this));
	d_smithRate->subscribeEvent(TLStaticText::EventMouseLeaves, Event::Subscriber(&KUiSmith::onLevaeRate, this));
	d_smithRate->useLayout();

	d_materialRequireText = (TLStaticText*)_thisWindow->getChild("TaharezLook/Smith/MaterialRequire");
	d_materialRequireText->useLayout();
	

	for(int i = 0; i < UI_SMITH_MAX_REQ_ITEM_COUNT; ++i)
	{
		d_reqItem[i] = (TLGameObject*)_thisWindow->getChild("TaharezLook/Smith/Item" + iToString(i + 1));
		d_reqItem[i]->subscribeEvent(TLButton::EventMouseButtonDown, 
			Event::Subscriber(&KUiSmith::onClickReqItem, this));
		
		KObjAtContRegion* itemInfo = &d_reqItemData[i];
		itemInfo->Obj.uId = -1;
		itemInfo->Obj.uGenre = CGOG_NOTHING;
		itemInfo->Region.h = -1;
		itemInfo->Region.v = i;
		itemInfo->Region.Height = 0;
		itemInfo->Region.Width = 0;	
		
		d_reqItem[i]->setUserData(itemInfo);
		d_reqItemGrid[i].setCtrl(d_reqItem[i]);
		d_reqItemGrid[i].addTip();
	}

	d_smithOneBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/SmithOne");
	d_smithOneBtn->subscribeEvent(TLButton::EventMouseClick, 
		Event::Subscriber(&KUiSmith::onClickSmithOne, this));

	d_smithAllBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/SmithAll");
	d_smithAllBtn->subscribeEvent(TLButton::EventMouseClick, 
		Event::Subscriber(&KUiSmith::onClickSmithAll, this));

	d_cancelBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/Cancel");
	d_cancelBtn->subscribeEvent(TLButton::EventMouseClick, 
		Event::Subscriber(&KUiSmith::onClickCancel, this));

	d_closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Smith/Close");
	d_closeBtn->subscribeEvent(TLButton::EventMouseClick, 
		Event::Subscriber(&KUiSmith::onClickClose, this));

	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiSmith::onHide, this));
	_thisWindow->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiSmith::onShow, this));
}

void KUiSmith::clearGameObj(TLGameObject* goCtrl)
{
	TLGameObject::GameObject go;
	go.d_gameobjectSet = BACKGROUND_IMAGE;
	go.d_gameobject = BACKGROUND_IMAGE;
	go.d_type = TLGameObject::idle;
	goCtrl->setObject(go);

	KObjAtContRegion* itemInfo = (KObjAtContRegion*)goCtrl->getUserData();
	itemInfo->Obj.uId = COMMON_ITEM_INVALID_ID;
	itemInfo->Obj.uGenre = CGOG_NOTHING;
}

void KUiSmith::clear()
{
	if(!_thisWindow)
	{
		return;
	}
	
	d_curList.clear();
	d_selItemIndex = UI_SMITH_INVALID_LIST_INDEX;
	d_scrollBar->setScrollPosition(0);
	
	d_icon->hide();

	for(int j = 0; j < UI_SMITH_MAX_REQ_ITEM_COUNT; ++j)
	{
		clearGameObj(d_reqItem[j]);
	}

	d_ruleMsg[0] = 0;
	d_materialRequireText->getLayout()->clearLayout();
	d_moneyRequireText->getLayout()->clearLayout();
	d_smithRate->getLayout()->clearLayout();

	d_continueSmith = false;

	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
}

void KUiSmith::open(int shopId)
{
	if(!_thisWindow)
	{
		return;
	}
	
	_thisWindow->show();

	clear();
	g_pCoreShell->GetGameData( GDI_GET_SMITH_RULE_LIST, (UINT)&d_curList, (int)shopId);
	showType1();
	
	for(int i = 0; i < d_curList.size(); ++i)
	{
		if(d_curList[i].childCount == 1)
		{
			showType23(d_curList[i].name);
		}
		else if(d_curList[i].childCount == 3)
		{
			showType4(d_curList[i].name);
		}
		else if(d_curList[i].childCount == 4)
		{
			d_selItemIndex = i;
			showSmithRule(d_curList[i].id);
			break;
		}
	}
	d_type4Pannel->hide();
}

void KUiSmith::showType1()
{
	if(!_thisWindow)
	{
		return;
	}
	
	int type1Index = 0;
	for(int i = 0; i < d_curList.size(); ++i)
	{
		CommonTreeItem& curItem = d_curList[i];
		if(curItem.childCount == 1)
		{
			d_type1SelectBtn[type1Index]->setText(AnsiToUtf8(curItem.name));
			d_type1SelectBtn[type1Index]->show();
			++type1Index;
			if(type1Index >= UI_SMITH_TYPE1_COUNT)
			{
				break;
			}
		}
	}
}

bool KUiSmith::selectType1(const EventArgs& args)
{
	MouseEventArgs* mouseEvent = (MouseEventArgs*)&args;
	Window* ctrl = mouseEvent->window;
	((TLRadioButton*)ctrl)->setSelected(true);
		
	const char* type1 = Utf8ToAnsi(ctrl->getText());
	showType23(type1);

	return true;
}

void KUiSmith::showType23(const char* type1)
{
	if(!_thisWindow)
	{
		return;
	}
	
	d_smithItems->removeAllItem();

	bool find = false;
	TreeItem* curTreeItem = NULL;
	for(int i = 0; i < d_curList.size(); ++i)
	{
		CommonTreeItem& curItem = d_curList[i];
		if(curItem.childCount == 1)
		{
			find = false;
			if(!strcmp(type1, curItem.name))
			{
				find = true;
			}
		}
		else if(find)
		{
			if(curItem.childCount == 2)
			{
				curTreeItem = new TLTreeItem(AnsiToUtf8(curItem.name), i);
				d_smithItems->addItem(curTreeItem);
			}
			else if(curItem.childCount == 3)
			{
				if(curTreeItem == NULL)
				{
					continue;
					_ASSERT(0);
				}
				curTreeItem->addItem(new TLTreeItem(AnsiToUtf8(curItem.name), i));		
			}
		}
	}

	int itemListHeight = d_smithItems->getTreeTotalItemsHeigh();
	int clipperHeight = d_skillListPannel->getHeight(Absolute);
	float stepHeight = 17.0f;
	if(clipperHeight >= itemListHeight)
	{
		d_scrollBar->setScrollPosition(0.0f);
		d_scrollBar->hide();
	}
	else
	{
		d_scrollBar->setScrollPosition(0.0f);
		d_scrollBar->show();
		d_scrollBar->setStepSize(stepHeight / (itemListHeight - clipperHeight));
	}
}

bool KUiSmith::selectType23(const EventArgs& args)
{
	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* treeItem = treeEvent->treeItem;

	if(treeItem == NULL)
	{
		return false;
	}

	int smithListIndex = treeItem->getID();
	if(smithListIndex < 0 || smithListIndex >= d_curList.size())
	{
		_ASSERT(0);
		return true;
	}

	if(d_curList[smithListIndex].childCount == 2)
	{
		return true;
	}

	const char* type3 = Utf8ToAnsi(treeItem->getText());

	showType4(type3);

	return true;
}

void KUiSmith::freshType23Scroll()
{
	if(!_thisWindow)
	{
		return;
	}
	
	int itemListHeight = d_smithItems->getTreeTotalItemsHeigh();
	int clipperHeight = d_skillListPannel->getHeight(Absolute);
	float stepHeight = 17.0f;
	if(clipperHeight >= itemListHeight)
	{
		d_scrollBar->setScrollPosition(0.0f);
		d_scrollBar->hide();
	}
	else
	{
		d_scrollBar->show();
		d_scrollBar->setStepSize(stepHeight / (itemListHeight - clipperHeight));
		float off = -d_smithItems->getYPosition(Absolute);
		d_scrollBar->setScrollPosition(off / (itemListHeight - clipperHeight));
	}
}

bool KUiSmith::onType2OpenClose( const EventArgs& args )
{
	freshType23Scroll();
	return true;
}

bool KUiSmith::fold(const EventArgs& args)
{
	static foldFlag = true;

	for(int i = 0; i < d_smithItems->getItemCount(); ++i)
	{
		TreeItem* item = d_smithItems->getItemAtIndex(i);
		if(item == NULL)
		{
			continue;
		}
		if(foldFlag)
		{
			item->setIsOpen(true);
		}
		else
		{
			item->setIsOpen(false);
		}
	}

	foldFlag = !foldFlag;
	if(foldFlag)
	{		
		d_foldAllBtn->hide();
		d_openAllBtn->show();
	}
	else
	{		
		d_foldAllBtn->show();
		d_openAllBtn->hide();
	}

	freshType23Scroll();
	return true;
}

bool KUiSmith::scroll(const EventArgs& args)
{
	int itemListHeight = d_smithItems->getTreeTotalItemsHeigh();
	int clipperHeight = d_skillListPannel->getHeight(Absolute);

	if(itemListHeight < clipperHeight)
	{
		d_smithItems->setYPosition(Absolute, 0);
		return true;
	}

	float pos = d_scrollBar->getScrollPosition();
	int listYOff = (itemListHeight - clipperHeight) * pos;
	d_smithItems->setYPosition(Absolute, -listYOff);

	return true;
}

bool KUiSmith::onWheel( const EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(d_scrollBar->isVisible())
	{
		d_scrollBar->setScrollPosition(d_scrollBar->getScrollPosition()
			- d_scrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

void KUiSmith::showType4(const char* type3)
{
	if(!_thisWindow)
	{
		return;
	}
	
	d_type4List->resetList();

	bool find = false;
	for(int i = 0; i < d_curList.size(); ++i)
	{
		CommonTreeItem& curItem = d_curList[i];
		if(curItem.childCount == 3 && !strcmp(type3, curItem.name))
		{
			find = true;
			continue;
		}
		
		if(find)
		{
			if(curItem.childCount == 4)
			{
				d_type4List->addItem(new TLTreeItem(AnsiToUtf8(curItem.name), i));
			}
			else
			{
				break;
			}
		}
	}

	if(d_type4List->getItemCount())
	{
		int typeListHeight = d_type4List->getTreeTotalItemsHeigh();
		d_type4Pannel->setHeight(Absolute, typeListHeight + 10);
		TreeItem* item = d_type4List->getItemAtIndex(0);
		d_type4Btn->setText(item->getText());
		d_type4Btn->enable();
		
		d_selItemIndex = item->getID();
		showSmithRule(d_curList[d_selItemIndex].id);
	}
	else
	{
		d_type4Btn->setText("--");
		d_type4Btn->disable();
	}
}

bool KUiSmith::openType4List(const EventArgs& args)
{
	if(d_type4Pannel->isVisible())
	{
		d_type4Pannel->hide();
	}
	else
	{
		d_type4Pannel->show();
	}
	return true;
}

bool KUiSmith::selectType4(const EventArgs& args)
{
	TreeEventArgs* treeEvent = (TreeEventArgs*)&args;
	TreeItem* item = treeEvent->treeItem;

	if(item == NULL)
	{
		return false;
	}

	int smithListIndex = item->getID();
	if(smithListIndex < 0 || smithListIndex >= d_curList.size())
	{
		return true;
	}

	d_selItemIndex = smithListIndex;
	int ruleId = d_curList[smithListIndex].id;
	d_type4Btn->setText(item->getText());
	showSmithRule(ruleId);
	d_type4Pannel->hide();

	return true;
}

void KUiSmith::showSmithRule(int ruleId)
{
	if(!_thisWindow)
	{
		return;
	}
	
	char tempText[COMMON_CLIENT_MSG_LEN_256];

	SmithRule rule;
	if(g_pCoreShell->GetGameData( GDI_GET_SMITH_RULE_BY_ID, (UINT)&rule, (int)ruleId) == 0)
	{
		d_smithOneBtn->disable();
		d_smithAllBtn->disable();
		return;
	}
	
	//显示icon
	const KBASICPROP_ITEM* itemBaseInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemBaseInfo, (int)&rule.smithItem.type);
	if(itemBaseInfo)
	{
		sprintf(tempText, "%s%s_normal", itemBaseInfo->szImageName, EQUIPMENT_MAN_POSTFIX_SMALL);
		d_icon->setImage(itemBaseInfo->szImageSetName, tempText);
		d_icon->show();
	}

	const KUiCfgLoader::SmithData& smithCfg = KUiCfgLoader::getSingleton().getSmithData();
	//金钱和蕴魂
	//开始设置排版内容
	int wndWidth = d_moneyRequireText->getWidth(Absolute);
	sprintf(tempText, "<Layout width=%d>", wndWidth);
	strcpy(d_ruleMsg, tempText);

// 	//打印物品名称
// 	strcat(d_ruleMsg, "<Seg float=wrap>");
// 	strcat(d_ruleMsg, "<Obj type=text color=255,255,150>");
// 	strcat(d_ruleMsg, itemBaseInfo->szName);
// 	strcat(d_ruleMsg, "</Obj></Seg>");

	//金钱
	char colorText[COMMON_CLIENT_MSG_LEN_128];
	strcat(d_ruleMsg, "<Seg float=wrap>");
	sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, smithCfg.moneyText);
	strcat(d_ruleMsg, tempText);

	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, NULL, NULL);
	if(money < rule.reqMoney)
	{
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionUnfillColor, smithCfg.textFont);
	}
	else
	{
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionFillColor, smithCfg.textFont);
	}
	
	if(rule.reqMoney / 10000 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, rule.reqMoney / 10000);
		strcat(d_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getJinImagePath());
		strcat(d_ruleMsg, tempText);
	}
	
	if(rule.reqMoney % 10000 / 100 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, rule.reqMoney % 10000 / 100);
		strcat(d_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getYinImagePath());
		strcat(d_ruleMsg, tempText);
	}
	
	if(rule.reqMoney % 100 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, rule.reqMoney % 100);
		strcat(d_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
		strcat(d_ruleMsg, tempText);
	}
	strcat(d_ruleMsg, "</Obj></Seg>");

	//蕴魂
	strcat(d_ruleMsg, "<Seg float=wrap>");
	sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, smithCfg.yunhunText);
	strcat(d_ruleMsg, tempText);

	int yunhun = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_POINT, NULL, NULL);
	if(yunhun < rule.reqYunHun)
	{
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionUnfillColor, smithCfg.textFont);
	}
	else
	{
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionFillColor, smithCfg.textFont);
	}
	sprintf(tempText, "<Obj %s>%d</Obj>", colorText, rule.reqYunHun);
	strcat(d_ruleMsg, tempText);
	strcat(d_ruleMsg, "</Seg>");

	//排版结束
	strcat(d_ruleMsg, "</Layout>");
	d_moneyRequireText->getLayout()->SetText(d_ruleMsg);
	d_moneyRequireText->getLayout()->flashLayout();
//	d_moneyRequireText->fitLayoutSize();
	

	//爻装概率
	//开始设置排版内容
	if(rule.rate.size() == 3)
	{
		float sum = rule.rate[0] + rule.rate[1] + rule.rate[2];
		int normalRate = (float)rule.rate[0] / sum * 100;
		int yaoRate = (float)rule.rate[1] / sum * 100;
		int randRate = (float)rule.rate[2] / sum * 100;

		wndWidth = d_smithRate->getWidth(Absolute);
		sprintf(tempText, "<Layout width=%d>", wndWidth);
		strcpy(d_ruleMsg, tempText);
		strcat(d_ruleMsg, "<Seg float=wrap>");
		//爻概率图片
		sprintf(tempText, "<Obj type=pic gotype=item goid=0>%s</Obj>", smithCfg.yaoRateImage);
		strcat(d_ruleMsg, tempText);
		//爻概率
		sprintf(tempText, "<Obj color=%s font-family=%s> %d%%  </Obj>", smithCfg.normalTextColor, smithCfg.textFont, yaoRate);
		strcat(d_ruleMsg, tempText);
		//非爻概率图片
		sprintf(tempText, "<Obj type=pic gotype=item goid=1>%s</Obj>", smithCfg.normalRateImage);
		strcat(d_ruleMsg, tempText);
		//非爻概率
		sprintf(tempText, "<Obj color=%s font-family=%s> %d%%</Obj>", smithCfg.normalTextColor, smithCfg.textFont, normalRate);
		strcat(d_ruleMsg, tempText);
		//随机装备图片
		sprintf(tempText, "<Obj type=pic gotype=item goid=2>%s</Obj>", smithCfg.randRateImage);
		strcat(d_ruleMsg, tempText);
		//随机装备概率
		sprintf(tempText, "<Obj color=%s font-family=%s> %d%%</Obj>", smithCfg.normalTextColor, smithCfg.textFont, randRate);
		strcat(d_ruleMsg, tempText);
		
		strcat(d_ruleMsg, "</Seg></Layout>");
		d_smithRate->getLayout()->SetText(d_ruleMsg);
		d_smithRate->getLayout()->flashLayout();
	}
//	d_smithRate->fitLayoutSize();

	//打印需求物品
	wndWidth = d_materialRequireText->getWidth(Absolute);
	sprintf(tempText, "<Layout width=%d>", wndWidth);
	strcpy(d_ruleMsg, tempText);
	for(int i = 0; i < rule.reqItemTypeCount; ++i)
	{
		const KBASICPROP_ITEM* itemInfo;
		g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemInfo, (int)&rule.reqItem[i].type);

		strcat(d_ruleMsg, "<Seg float=wrap>");
		sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, itemInfo->szName);
		strcat(d_ruleMsg, tempText);

		int count = itemCountOfAType(rule.reqItem[i].type);
		if(count < rule.reqItem[i].itemCount)
		{
			sprintf(tempText, "<Obj color=%s font-family=%s>", smithCfg.conditionUnfillColor, smithCfg.textFont);
		}
		else
		{
			sprintf(tempText, "<Obj color=%s font-family=%s>", smithCfg.conditionFillColor, smithCfg.textFont);
		}
		strcat(d_ruleMsg, tempText);

		sprintf(tempText, "(%d/%d)", count, rule.reqItem[i].itemCount);
		strcat(d_ruleMsg, tempText);
		strcat(d_ruleMsg, "</Obj></Seg>");
	}
	strcat(d_ruleMsg, "</Layout>");
	d_materialRequireText->getLayout()->SetText(d_ruleMsg);
	d_materialRequireText->getLayout()->flashLayout();
//	d_materialRequireText->fitLayoutSize();

 	for (int j = 0; j < UI_SMITH_MAX_REQ_ITEM_COUNT; ++j)
 	{		
 		int index = ((KObjAtContRegion*)d_reqItem[j]->getUserData())->Obj.uId;
 
 		if(COMMON_ITEM_INVALID_ID == index)
 		{
 			continue;
 		}
 
 		int itemCount = g_pCoreShell->GetGameData(GDI_ITEM_COUNT_QUERY, index, NULL);
 		if(itemCount >0)
 		{
 			TLGameObject::GameObject& objInfo = d_reqItem[j]->getObject();
 			if(objInfo.d_count != itemCount)
 			{
 				d_reqItem[j]->setCount(itemCount);
 			}
 		}
 		else
 		{
 			clearGameObj(d_reqItem[j]);
 		}
 	}

	int smithCount = canSmithCount();
	if(smithCount)
	{
		d_smithOneBtn->enable();
		d_smithAllBtn->enable();
	}
	else
	{
		d_smithOneBtn->disable();
		d_smithAllBtn->disable();
	}
	
	delete[] rule.reqItem;
}

void KUiSmith::showTip(int ruleId)
{
	if(!_thisWindow)
	{
		return;
	}
	
	SmithRule rule;
	if(g_pCoreShell->GetGameData(GDI_GET_SMITH_RULE_BY_ID, (UINT)&rule, (int)ruleId) == 0)
	{
		return;
	}
	
	//显示icon
	const KBASICPROP_ITEM* itemBaseInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemBaseInfo, (int)&rule.smithItem.type);

	FIND_ITEMINDEX_PARAM itemType;
	itemType.nGenre			= itemBaseInfo->nItemGenre;
	itemType.nDetail		= itemBaseInfo->nDetailType;
	itemType.nParticular	= itemBaseInfo->nParticularType;
	itemType.nLevel			= itemBaseInfo->nLevel;
	itemType.nGroup			= -1;

	char tipHeadText[COMMON_CLIENT_MSG_LEN_256];
		sprintf(tipHeadText, "<Layout width=%d margin-top=%d margin-left=%d margin-right=%d margin-bottom=%d>", 
		KUiCfgLoader::getSingleton().getTipData().windowWidth,
		KUiCfgLoader::getSingleton().getTipData().topMargin,
		KUiCfgLoader::getSingleton().getTipData().leftMargin,
		KUiCfgLoader::getSingleton().getTipData().RightMargin,
		KUiCfgLoader::getSingleton().getTipData().bottomMargin);

	d_layoutText[0] = 0;
	strcat(d_layoutText, tipHeadText);
	g_pCoreShell->GetGameData(GDI_LINKED_ITEM_LAYOUT_DESC, (unsigned int)d_layoutText, (int)&itemType);
	
	strcat(d_layoutText, "</Layout>");
	d_layoutText[LAYOUT_TEXT_MAX_LEN - 1] = 0;

	Rect area = d_icon->getUnclippedInnerRect();
	KUiItemTip::GetSingleton().show(d_layoutText, area, KUiItemTip::Right);
}

int KUiSmith::itemCountOfAType(ItemType& type)
{
	if(!_thisWindow)
	{
		return 0;
	}
	
	int count = 0;
	for(int i = 0; i < UI_SMITH_MAX_REQ_ITEM_COUNT; ++i)
	{
		int index = ((KObjAtContRegion*)d_reqItem[i]->getUserData())->Obj.uId;

		if(COMMON_ITEM_INVALID_ID == index)
		{
			continue;
		}

		ItemType curType;
		g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (UINT)&curType, index);
		
		if(type == curType)
		{
			count += g_pCoreShell->GetGameData(GDI_ITEM_COUNT_QUERY, index, NULL);
		}
	}
	return count;
}

void KUiSmith::doSmith()
{
	if(!_thisWindow)
	{
		return;
	}
	
	if(d_selItemIndex < 0 || d_selItemIndex >= d_curList.size())
		return;
	
	KUiCompoundParam tagCompoundList;
	ZeroMemory(&tagCompoundList, sizeof(KUiCompoundParam) );

	int itemCount = 0;
	for ( int i = 0; i < UI_SMITH_MAX_REQ_ITEM_COUNT; ++i )
	{
		int itemIndex = ((KObjAtContRegion*)d_reqItem[i]->getUserData())->Obj.uId;

		if(COMMON_ITEM_INVALID_ID == itemIndex)
		{
			continue;
		}

		tagCompoundList.nItemIndex[i] = itemIndex;
		++itemCount;
	}

	tagCompoundList.nCompoundType = COMPOUND_SMITH;
	tagCompoundList.ruleId = d_curList[d_selItemIndex].id;
	tagCompoundList.szPlusInfo[0] = 0;


	g_pCoreShell->OperationRequest(GOI_COMPOUND_BEGIN, (unsigned int)&tagCompoundList, itemCount);
}

bool KUiSmith::onClickSmithOne(const EventArgs& args)
{
	d_continueSmith = false;
	doSmith();
	return true;
}

bool KUiSmith::onClickSmithAll(const EventArgs& args)
{
	d_continueSmith = true;
	doSmith();
	return true;
}

void KUiSmith::clearSameObj(int itemIndex)
{
	if(!_thisWindow)
	{
		return;
	}
	
	for ( int i = 0; i < UI_SMITH_MAX_REQ_ITEM_COUNT; ++i )
	{
		KObjAtContRegion* region = (KObjAtContRegion*)d_reqItem[i]->getUserData();
		if ( region->Obj.uId == itemIndex )
		{
			clearGameObj(d_reqItem[i]);
			return;
		}
	}
}

bool KUiSmith::onHoverRate(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* rateCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = rateCtrl->getLayout();

	Point pos = rateCtrl->getUnclippedPixelRect().getPosition();
	Point off = rateCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;

	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return true;
	}

	if(elemInfo.gameObj._objType == LO_GO_ITEM)
	{
		Point tipPos = pos + off
			+ Point(elemInfo.area.getLeft(), elemInfo.area.getTop())
			+ Point(elemInfo.area.getWidth(), elemInfo.area.getHeight());
		if(elemInfo.gameObj._objId[0] == 0)
		{
			KUiItemTip::GetSingleton().show(const_cast<char*>(KUiCfgLoader::getSingleton().getSmithData().yaoRateText), 
				Rect(tipPos, Size(0, 0)), KUiItemTip::BottomRight);
		}
		else if(elemInfo.gameObj._objId[0] == 1)
		{
			KUiItemTip::GetSingleton().show(const_cast<char*>(KUiCfgLoader::getSingleton().getSmithData().normalRateText), 
				Rect(tipPos, Size(0, 0)), KUiItemTip::BottomRight);
		}
		else if(elemInfo.gameObj._objId[0] == 2)
		{
			KUiItemTip::GetSingleton().show(const_cast<char*>(KUiCfgLoader::getSingleton().getSmithData().randRateText), 
				Rect(tipPos, Size(0, 0)), KUiItemTip::BottomRight);
		}
	}
	else
	{
		KUiItemTip::Hide();
	}

	return true;
}

bool KUiSmith::onLevaeRate(const EventArgs& args)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiSmith::onHoverIcon(const EventArgs& args)
{
	showTip(d_curList[d_selItemIndex].id);
	return true;
}

bool KUiSmith::onLevaeIcon(const EventArgs& args)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiSmith::onClickReqItem(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	
	if(LeftButton != mouse->button)
	{
		return true;
	}
	
	TLGameObject* item = (TLGameObject*)mouse->window;
	if(!item)
	{
		return false;
	}

	TLGameObject* destObj, * sourObj;
	TLGameObject::GameObject destObjInfo, sourObjInfo;
	KObjAtContRegion* destRegion, * sourRegion;

	destObj = item;
	destObj->getObject(destObjInfo);
	destRegion = (KObjAtContRegion*)destObj->getUserData();

	sourObj = KUiDragItem::GetSingleton().getObj();
	sourObj->getObject(sourObjInfo);
	sourRegion = (KObjAtContRegion*)sourObj->getUserData();
		
	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		if(destObjInfo.d_type == TLGameObject::item)//拿起
		{
			//更新手上的物品
			sourObj->setObject(destObjInfo);
			sourObj->setCanDrag(true);
			*sourRegion = *destRegion;

			//清空被拿起的物品
			clearGameObj(destObj);
		}
	}
	else if(sourObjInfo.d_type == TLGameObject::item)
	{
		if(destObjInfo.d_type == TLGameObject::item) 
		{
			// 点击目的有物品
			if ( destRegion->Obj.uId == sourRegion->Obj.uId )
			{
				//如果是同样的物品，清空手上的东西
				clearGameObj(sourObj);

				sourObj->setCanDrag(false);
			}
			else
			{
				// 交换
				clearSameObj( sourRegion->Obj.uId );

				destObj->setObject(sourObjInfo);
				sourObj->setObject(destObjInfo);
				sourObj->setCanDrag(true);

				KObjAtContRegion tempRegion;
				tempRegion = *sourRegion;
				*sourRegion = *destRegion;
				*destRegion = tempRegion;
			}
		}
		else if(destObjInfo.d_type == TLGameObject::idle)
		{
			// 点击目的为空
			clearSameObj( sourRegion->Obj.uId );

			destObj->setObject(sourObjInfo);
			*destRegion = *sourRegion;

			//清空手上的东西
			clearGameObj(sourObj);

			sourObj->setCanDrag(false);
		}
	}

	if(d_selItemIndex != UI_SMITH_INVALID_LIST_INDEX)
	{
		showSmithRule(d_curList[d_selItemIndex].id);
	}

	//更新锁定物品状态
	freshLockedItem();
	return true;
}

int KUiSmith::canSmithCount()
{
	if(!_thisWindow)
	{
		return 0;
	}
	
	SmithRule rule;
	if(UI_SMITH_INVALID_LIST_INDEX == d_selItemIndex)
	{
		return 0;
	}

	int ruleId = d_curList[d_selItemIndex].id;
	if(g_pCoreShell->GetGameData( GDI_GET_SMITH_RULE_BY_ID, (UINT)&rule, (int)ruleId) == 0)
	{
		return 0;
	}

	int smithCount = 100;
	for(int i = 0; i < rule.reqItemTypeCount; ++i)
	{
		int itemCount = itemCountOfAType(rule.reqItem[i].type);
		if(rule.reqItem[i].itemCount && itemCount / rule.reqItem[i].itemCount < smithCount)
			smithCount = itemCount / rule.reqItem[i].itemCount;
	}
	
	int yunhun = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_POINT, NULL, NULL);
	if(rule.reqYunHun && yunhun / rule.reqYunHun < smithCount)
	{
		smithCount = yunhun / rule.reqYunHun;
	}
	
	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, NULL, NULL);
	if(rule.reqMoney && money / rule.reqMoney < smithCount)
	{
		smithCount = money / rule.reqMoney;
	}

	delete[] rule.reqItem;
	return smithCount;
}


void KUiSmith::showErrorMsg(int errorCode)
{
	if(!_thisWindow)
	{
		return;
	}
	
	KUiChannelCentre::GetSingleton().toSysMsg("");
}

void KUiSmith::onEndSmith()
{
	if(!_thisWindow)
	{
		return;
	}
	
	if(!_thisWindow->isVisible())
	{
		return;
	}
	
	showSmithRule(d_curList[d_selItemIndex].id);
	
	//打造成功后，更新物品锁定状态
	freshLockedItem();

	if(d_continueSmith)
	{
		doSmith();
	}
}

void KUiSmith::onConditionChanged()
{
	if(!_thisWindow)
	{
		return;
	}
	
	showSmithRule(d_curList[d_selItemIndex].id);
}

bool KUiSmith::onClickCancel(const EventArgs& args)
{
	_thisWindow->hide();
	return true;
}

bool KUiSmith::onClickClose(const EventArgs& args)
{
	_thisWindow->hide();
	return true;
}

bool KUiSmith::onHide( const EventArgs& args )
{
	KUiLinkedItemTip::Hide();
	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
	return true;
}

bool KUiSmith::onShow( const EventArgs& args )
{
	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
	return true;
}

void KUiSmith::freshLockedItem()
{
	if(!_thisWindow)
	{
		return;
	}
	
	KUiItemLockMgr::getSingleton().clear();

	for(int i = 0; i < UI_SMITH_MAX_REQ_ITEM_COUNT; ++i)
	{
		if(d_reqItemData[i].Obj.uGenre == CGOG_NOTHING)
		{
			continue;
		}
		KUiItemLockMgr::getSingleton().lock(d_reqItemData[i].Obj.uId);
	}

	KUiItemLockMgr::getSingleton().refreshItemBox();
}