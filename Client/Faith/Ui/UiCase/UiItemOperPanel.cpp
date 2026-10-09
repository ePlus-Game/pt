#include "UiItemOperPanel.h"
#include "UiItemLockMgr.h"
#include "UiDragItem.h"
#include "UiCompound.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"
#include "CoreShell.h"
#include "../KMessageCentre.h"

extern iCoreShell* g_pCoreShell;

KUiItemOperPanel::KUiItemOperPanel()
{
	_ruleId = COMMON_ITEM_INVALID_ID;
	_autoCompoundCommit = NULL;
	load();
	DisableAutoCommit();
}

KUiItemOperPanel::~KUiItemOperPanel()
{
	
}

KUiItemOperPanel& KUiItemOperPanel::getSingleton()
{
	static KUiItemOperPanel singleton;
	return singleton;
}

void KUiItemOperPanel::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_ITEM_OPER_PANEL_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_ITEM_OPER_PANEL_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	if(!_thisWindow)
	{
		return;
	}

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->hide();

	_liftBackImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/jejin");
	_liftBackImage->setZLevel(Window::Bottom);
	_compoundBackImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/hecheng");
	_compoundBackImage->setZLevel(Window::Bottom);

	_lift = (TLRadioButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/Lift");
	_compound = (TLRadioButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/Compound");
	_levelUp = (TLRadioButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/Levelup");

	_lift->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::selectPanel, this));
	_compound->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::selectPanel, this));
	_levelUp->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::selectPanel, this));

	char ctrlName[COMMON_CLIENT_MSG_LEN_256];
	for(int i = 0; i < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++i)
	{
		sprintf(ctrlName, "TaharezLook/ItemOperPanel/Item%d", i + 1);
		_item[i] = (TLGameObject*)_thisWindow->getChild(ctrlName);
		
		_item[i]->subscribeEvent(TLButton::EventMouseButtonDown, 
			Event::Subscriber(&KUiItemOperPanel::clickGrid, this));
		
		KObjAtContRegion* itemInfo = &_itemInfo[i];
		itemInfo->Obj.uId = -1;
		itemInfo->Obj.uGenre = CGOG_NOTHING;
		itemInfo->Region.h = -1;
		itemInfo->Region.v = i;
		itemInfo->Region.Height = 0;
		itemInfo->Region.Width = 0;	
		
		_item[i]->setUserData(itemInfo);
		if(i != 0)
		{
			_itemGrid[i].setCtrl(_item[i]);
			_itemGrid[i].addTip();
		}
	}

	_generateItemImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/GenerateItem");
	_generateItemImage->subscribeEvent(TLStaticText::EventMouseMove, Event::Subscriber(&KUiItemOperPanel::onHoverIcon, this));
	_generateItemImage->subscribeEvent(TLStaticText::EventMouseLeaves, Event::Subscriber(&KUiItemOperPanel::onLevaeIcon, this));
	_item[0]->subscribeEvent(TLStaticText::EventMouseMove, Event::Subscriber(&KUiItemOperPanel::onHoverIcon, this));
	_item[0]->subscribeEvent(TLStaticText::EventMouseLeaves, Event::Subscriber(&KUiItemOperPanel::onLevaeIcon, this));

	_liftRequirePanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/RulePanel");
	_moneyRequireText = (TLStaticText*)_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/Money");
	_moneyRequireText->useLayout();

	_rateText = (TLStaticText*)_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/Rate");
	_rateText->subscribeEvent(TLStaticText::EventMouseMove, Event::Subscriber(&KUiItemOperPanel::onHoverRate, this));
	_rateText->subscribeEvent(TLStaticText::EventMouseLeaves, Event::Subscriber(&KUiItemOperPanel::onLevaeRate, this));
	_rateText->useLayout();

	_materialRequireText = (TLStaticText*)_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/Material");
	_materialRequireText->useLayout();

	_compoundRequirePanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/TipPanel");
	_compoundMoneyRequireText = (TLStaticText*)_compoundRequirePanel->getChild("TaharezLook/ItemOperPanel/TipPanel/Money");
	_compoundMoneyRequireText->useLayout();

	_liftCommit = (TLButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/jiejinanniu");
	_liftCommit->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::onCommit, this));
	_compoundCommit = (TLButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/Commit");
	_compoundCommit->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::onCommit, this));
	_autoCompoundCommit = (TLButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/AutoCommit");
	_autoCompoundCommit->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::onAutoCommit, this));
	m_autoCommitBtnText = _autoCompoundCommit->getText();

	TLButton* closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/ItemOperPanel/Close");
	closeBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiItemOperPanel::onClose, this));
	
	_thisWindow->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiItemOperPanel::onShow, this));
	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiItemOperPanel::onHide, this));

	//动画
	_liftBeginAnimation = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/LiftBeginAnimation");
	_liftBeginAnimation->setZLevel(Window::Top);
	_liftBeginAnimation->setDummyWnd(true);

	_compBeginAnimation = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/CompBeginAnimation");
	_compBeginAnimation->setZLevel(Window::Top);
	_compBeginAnimation->setDummyWnd(true);
	
	_endImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/EndOk");
	_endImage->setZLevel(Window::Top);
	_endImage->setDummyWnd(true);

	_endFailImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/ItemOperPanel/EndFail");
	_endFailImage->setZLevel(Window::Top);
	_endFailImage->setDummyWnd(true);
}

bool KUiItemOperPanel::onClose(const EventArgs& args)
{
	hide();
	return true;
}

bool KUiItemOperPanel::isVisible()
{
	if(_thisWindow)
	{
		return _thisWindow->isVisible();
	}

	return false;
}

void KUiItemOperPanel::show()
{
	if(_thisWindow)
	{
		_thisWindow->show();
		_curState = INVALID_STATE;
		showLift();
		_lift->setSelected(true);
	}
}

void KUiItemOperPanel::hide()
{
	if(_thisWindow)
		_thisWindow->hide();	
}

void KUiItemOperPanel::toggle()
{
	if(!_thisWindow)
	{
		return;
	}
	
	if(_thisWindow->isVisible())
		hide();
	else
		show();
}

bool KUiItemOperPanel::selectPanel(const EventArgs& args)
{
	WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
	if(eventArgs->window == _lift)
	{
		showLift();
	}
	else if(eventArgs->window == _compound)
	{
		showCompound();
	}
	else if(eventArgs->window == _levelUp)
	{
		showLevelUp();
	}
	return true;
}

void KUiItemOperPanel::showLift()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_curState != LIFT)
	{
		clear();
		_curState = LIFT;
		_item[0]->show();
		_liftRequirePanel->show();

		_liftBackImage->show();
		
		_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/NoMainMaterialTip")->show();
		_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/MainMaterialWrongTip")->hide();

		_liftCommit->show();
		_lift->setSelected(true);
	}
}

void KUiItemOperPanel::showCompound()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_curState != COMPOUND)
	{
		clear();
		_curState = COMPOUND;
		_generateItemImage->show();
		_compoundRequirePanel->show();

		_compoundBackImage->show();
		_compound->setSelected(true);

		_compoundCommit->show();
		_autoCompoundCommit->show();
	}
}

void KUiItemOperPanel::showLevelUp()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_curState != LEVELUP)
	{
		clear();
		_curState = LEVELUP;
		_item[0]->show();
		KUiCompound::GetSingleton().Show(COMPOUND_LEVELUP);
		KUiCompound::GetSingleton().setPos(_thisWindow->getPosition(Absolute));
		hide();
	}
}

void KUiItemOperPanel::clear()
{
	if(!_thisWindow)
	{
		return;
	}

	_ruleId = COMMON_ITEM_INVALID_ID;

	for(int j = 0; j < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++j)
	{
		clearGameObj(_item[j]);
	}
	_item[0]->hide();

	_generateItemImage->setImage(NULL);
	_generateItemImage->hide();

	clearRequireInfo();

	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
		
	_liftCommit->disable();
	_compoundCommit->disable();
	_autoCompoundCommit->disable();
	DisableAutoCommit();
	_liftCommit->hide();
	_compoundCommit->hide();
	_autoCompoundCommit->hide();

	_liftBackImage->hide();
	_compoundBackImage->hide();

	_liftRequirePanel->hide();
	_compoundRequirePanel->hide();

	_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/NoMainMaterialTip")->hide();

	
	_liftBeginAnimation->hide();
	_compBeginAnimation->hide();
	_endFailImage->hide();
	_endImage->hide();

	_liftCommit->hide();
	_compoundCommit->hide();
}

void KUiItemOperPanel::clearGameObj(TLGameObject* goCtrl)
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

void KUiItemOperPanel::clearSameObj( int itemIndex )
{
	if(!_thisWindow)
	{
		return;
	}

	for ( int i = 0; i < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++i )
	{
		if( _itemInfo[i].Obj.uId == itemIndex )
		{
			clearGameObj( _item[i] );
			return;
		}
	}
}

bool KUiItemOperPanel::clickGrid(const EventArgs& args)
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

	if(_curState == COMPOUND && _item[0] == item)
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

	freshOpInfo();

	//更新锁定物品状态
	freshLockedItem();
	return true;
}

void KUiItemOperPanel::freshOpInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	_ruleMsg[0] = 0;
	switch(_curState)
	{
	case LIFT:
		{
			freshLiftInfo();
		}
		break;
	case COMPOUND:
		{
			freshCompoundInfo();
		}
		break;
	case LEVELUP:
		{
			freshLevelUpInfo();
		}
		break;
	} 
	freshItemCount();
}

void KUiItemOperPanel::freshLiftInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	char tempText[COMMON_CLIENT_MSG_LEN_256];

	_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/NoMainMaterialTip")->hide();
	_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/MainMaterialWrongTip")->hide();
	int liftItemIndex = _itemInfo[0].Obj.uId;
	ItemType type;
	if(!g_pCoreShell->GetGameData(GDI_GET_ITEM_TYPE_BY_INDEX, (UINT)&type, liftItemIndex))
	{
		_generateItemValidate = false;
		_liftCommit->disable();
		_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/NoMainMaterialTip")->show();
		clearRequireInfo();
		return;
	}

	SmithRule rule;
	_ruleId = g_pCoreShell->GetGameData(GDI_GET_SMITH_RULE_BY_LIFT_ITEM, (UINT)&rule, (int)&type);
	if(!_ruleId)
	{
		_generateItemValidate = false;
		_liftCommit->disable();
		_liftRequirePanel->getChild("TaharezLook/ItemOperPanel/RulePanel/MainMaterialWrongTip")->show();
		clearRequireInfo();
		return;
	}

	_generateItemValidate = true;
	_generateItem = rule.smithItem.type;
	
	//显示icon
// 	const KBASICPROP_ITEM* itemBaseInfo;
// 	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemBaseInfo, (int)&rule.smithItem.type);
// 	if(itemBaseInfo)
// 	{
// 		sprintf(tempText, "%s%s_normal", itemBaseInfo->szImageName, EQUIPMENT_MAN_POSTFIX_SMALL);
// 		d_icon->setImage(itemBaseInfo->szImageSetName, tempText);
// 		d_icon->show();
// 	}

	const KUiCfgLoader::SmithData& smithCfg = KUiCfgLoader::getSingleton().getSmithData();
	//金钱和蕴魂
	//开始设置排版内容
	int wndWidth = _moneyRequireText->getWidth(Absolute);
	sprintf(tempText, "<Layout width=%d>", wndWidth);
	strcpy(_ruleMsg, tempText);

// 	//打印物品名称
// 	strcat(_ruleMsg, "<Seg float=wrap>");
// 	strcat(_ruleMsg, "<Obj type=text color=255,255,150>");
// 	strcat(_ruleMsg, itemBaseInfo->szName);
// 	strcat(_ruleMsg, "</Obj></Seg>");

	//金钱
	char colorText[COMMON_CLIENT_MSG_LEN_128];
	strcat(_ruleMsg, "<Seg float=none>");
	sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, smithCfg.moneyText);
	strcat(_ruleMsg, tempText);

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
		strcat(_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getJinImagePath());
		strcat(_ruleMsg, tempText);
	}
	
	if(rule.reqMoney % 10000 / 100 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, rule.reqMoney % 10000 / 100);
		strcat(_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getYinImagePath());
		strcat(_ruleMsg, tempText);
	}
	
	if(rule.reqMoney % 100 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, rule.reqMoney % 100);
		strcat(_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
		strcat(_ruleMsg, tempText);
	}
	strcat(_ruleMsg, "</Obj></Seg>");

	//蕴魂
	strcat(_ruleMsg, "<Seg float=wrap t-a=right>");
	sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, smithCfg.yunhunText);
	strcat(_ruleMsg, tempText);

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
	strcat(_ruleMsg, tempText);
	strcat(_ruleMsg, "</Seg>");

	//排版结束
	strcat(_ruleMsg, "</Layout>");
	_moneyRequireText->getLayout()->SetText(_ruleMsg);
	_moneyRequireText->getLayout()->flashLayout();
//	d_moneyRequireText->fitLayoutSize();
	

	//爻装概率
	//开始设置排版内容
	if(rule.rate.size() == 3)
	{
		float sum = rule.rate[0] + rule.rate[1] + rule.rate[2];
		int normalRate = (float)rule.rate[0] / sum * 100;
		int yaoRate = (float)rule.rate[1] / sum * 100;
		int randRate = (float)rule.rate[2] / sum * 100;

		wndWidth = _rateText->getWidth(Absolute);
		sprintf(tempText, "<Layout width=%d>", wndWidth);
		strcpy(_ruleMsg, tempText);
		strcat(_ruleMsg, "<Seg float=wrap>");
		//爻概率图片
// 		sprintf(tempText, "<Obj type=pic gotype=item goid=0>%s</Obj>", smithCfg.yaoRateImage);
// 		strcat(_ruleMsg, tempText);
		strcat(_ruleMsg, KUiCfgLoader::getSingleton().getSmithData().yaoRateText);
		//爻概率
		sprintf(tempText, "<Obj color=%s font-family=%s> %d%%</Obj>", smithCfg.normalTextColor, smithCfg.textFont, yaoRate);
		strcat(_ruleMsg, tempText);
		//非爻概率图片
// 		sprintf(tempText, "<Obj type=pic gotype=item goid=1>%s</Obj>", smithCfg.normalRateImage);
// 		strcat(_ruleMsg, tempText);
		strcat(_ruleMsg, KUiCfgLoader::getSingleton().getSmithData().normalRateText);
		//非爻概率
		sprintf(tempText, "<Obj color=%s font-family=%s> %d%%</Obj>", smithCfg.normalTextColor, smithCfg.textFont, normalRate);
		strcat(_ruleMsg, tempText);
		//随机装备图片
// 		sprintf(tempText, "<Obj type=pic gotype=item goid=2>%s</Obj>", smithCfg.randRateImage);
// 		strcat(_ruleMsg, tempText);
		strcat(_ruleMsg, KUiCfgLoader::getSingleton().getSmithData().randRateText);
		//随机装备概率
 		sprintf(tempText, "<Obj color=%s font-family=%s> %d%%</Obj>", smithCfg.normalTextColor, smithCfg.textFont, randRate);
 		strcat(_ruleMsg, tempText);
		
		strcat(_ruleMsg, "</Seg></Layout>");
		_rateText->getLayout()->SetText(_ruleMsg);
		_rateText->getLayout()->flashLayout();
	}
//	_rateText->fitLayoutSize();

	//打印需求物品
	wndWidth = _materialRequireText->getWidth(Absolute);
	sprintf(tempText, "<Layout width=%d>", wndWidth);
	strcpy(_ruleMsg, tempText);
	for(int i = 0; i < rule.reqItemTypeCount; ++i)
	{
		const KBASICPROP_ITEM* itemInfo;
		g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemInfo, (int)&rule.reqItem[i].type);

		if(i == 0 || i % 2 == 0)
		{
			strcat(_ruleMsg, "<Seg float=wrap>");
		}
		else
		{
			strcat(_ruleMsg, "<Seg float=none text-align=right>");
		}
		sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, itemInfo->szName);
		strcat(_ruleMsg, tempText);

		int count = itemCountOfAType(rule.reqItem[i].type);
		if(count < rule.reqItem[i].itemCount)
		{
			sprintf(tempText, "<Obj color=%s font-family=%s>", smithCfg.conditionUnfillColor, smithCfg.textFont);
		}
		else
		{
			sprintf(tempText, "<Obj color=%s font-family=%s>", smithCfg.conditionFillColor, smithCfg.textFont);
		}
		strcat(_ruleMsg, tempText);

		sprintf(tempText, "(%d/%d)", count, rule.reqItem[i].itemCount);
		strcat(_ruleMsg, tempText);
		strcat(_ruleMsg, "</Obj></Seg>");
	}
	strcat(_ruleMsg, "</Layout>");
	_materialRequireText->getLayout()->SetText(_ruleMsg);
	_materialRequireText->getLayout()->flashLayout();
//	_materialRequireText->fitLayoutSize();

 	for (int j = 0; j < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++j)
 	{		
 		int index = _itemInfo[j].Obj.uId;
 
 		if(COMMON_ITEM_INVALID_ID == index)
 		{
 			continue;
 		}
 
 		int itemCount = g_pCoreShell->GetGameData(GDI_ITEM_COUNT_QUERY, index, NULL);
 		if(itemCount >0)
 		{
 			TLGameObject::GameObject& objInfo = _item[j]->getObject();
 			if(objInfo.d_count != itemCount)
 			{
 				_item[j]->setCount(itemCount);
 			}
 		}
 		else
 		{
 			clearGameObj(_item[j]);
 		}
 	}



	if(canMake(rule))
	{
		_liftCommit->enable();
	}
	else
	{
		_liftCommit->disable();
	}

	delete[] rule.reqItem;
}

bool KUiItemOperPanel::canMake(SmithRule& rule)
{
	if(!_thisWindow)
	{
		return false;
	}

	for(int i = 0; i < rule.reqItemTypeCount; ++i)
	{
		int itemCount = itemCountOfAType(rule.reqItem[i].type);

		if(itemCount < rule.reqItem[i].itemCount)
		{
			return false;
		}
	}
	
	int yunhun = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_POINT, NULL, NULL);
	if(yunhun < rule.reqYunHun)
	{
		return false;
	}
	
	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, NULL, NULL);
	if(money < rule.reqMoney)
	{
		return false;
	}
	
	return true;
}

void KUiItemOperPanel::freshCompoundInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	KUiCompoundParam tagCompoundList;
	ZeroMemory(&tagCompoundList, sizeof(KUiCompoundParam));
	tagCompoundList.nCompoundType = COMPOUND_MAKE;

	for(int i = 0; i < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++i)
	{
		tagCompoundList.nItemIndex[i] = _itemInfo[i].Obj.uId;
	}

	KUiCompoundRuleInfo tagBesetInfo;
	int nResult = g_pCoreShell->GetGameData(GDI_GET_COMPOUND_INFO, (unsigned int)&tagCompoundList, (int)&tagBesetInfo);
	if(nResult != enchaser_error_no)
	{
		clearRequireInfo();
		_compoundCommit->disable();
		_autoCompoundCommit->disable();
		DisableAutoCommit();
		_generateItemValidate = false;
		
		_generateItemImage->hide();
		return;
	}
	_generateItemImage->show();

	_ruleId = tagBesetInfo.ruleId;
	char tempText[COMMON_CLIENT_MSG_LEN_64];
	const KBASICPROP_ITEM* itemBaseInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemBaseInfo, (int)&tagBesetInfo.generateItem);
	if(itemBaseInfo)
	{
		sprintf(tempText, "%s%s_normal", itemBaseInfo->szImageName, EQUIPMENT_MAN_POSTFIX_SMALL);
		_generateItemImage->setImage(itemBaseInfo->szImageSetName, tempText);
		_generateItemImage->show();
	}

	_generateItem = tagBesetInfo.generateItem;
	_generateItemValidate = true;

	bool moenyYunhunFit = true;
	//金钱和蕴魂
	//开始设置排版内容
	int wndWidth = _compoundMoneyRequireText->getWidth(Absolute);
	sprintf(tempText, "<Layout width=%d>", wndWidth);
	strcpy(_ruleMsg, tempText);

	const KUiCfgLoader::SmithData& smithCfg = KUiCfgLoader::getSingleton().getSmithData();

	//金钱
	char colorText[COMMON_CLIENT_MSG_LEN_128];
	strcat(_ruleMsg, "<Seg float=none>");
	sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, smithCfg.moneyText);
	strcat(_ruleMsg, tempText);

	int money = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, NULL, NULL);
	if(money < tagBesetInfo.money)
	{
		moenyYunhunFit = false;
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionUnfillColor, smithCfg.textFont);
	}
	else
	{
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionFillColor, smithCfg.textFont);
	}
	
	if(tagBesetInfo.money / 10000 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, tagBesetInfo.money / 10000);
		strcat(_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getJinImagePath());
		strcat(_ruleMsg, tempText);
	}
	
	if(tagBesetInfo.money % 10000 / 100 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, tagBesetInfo.money % 10000 / 100);
		strcat(_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getYinImagePath());
		strcat(_ruleMsg, tempText);
	}
	
	if(tagBesetInfo.money % 100 > 0)
	{
		sprintf(tempText, "<Obj %s>%d </Obj>", colorText, tagBesetInfo.money % 100);
		strcat(_ruleMsg, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
		strcat(_ruleMsg, tempText);
	}
	strcat(_ruleMsg, "</Obj></Seg>");

	//蕴魂
	strcat(_ruleMsg, "<Seg float=wrap t-a=right>");
	sprintf(tempText, "<Obj color=%s font-family=%s>%s</Obj>", smithCfg.normalTextColor, smithCfg.textFont, smithCfg.yunhunText);
	strcat(_ruleMsg, tempText);

	int yunhun = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_POINT, NULL, NULL);
	if(yunhun < tagBesetInfo.yunhun)
	{
		moenyYunhunFit = false;
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionUnfillColor, smithCfg.textFont);
	}
	else
	{
		sprintf(colorText, "color=%s font-family=%s", smithCfg.conditionFillColor, smithCfg.textFont);
	}
	sprintf(tempText, "<Obj %s>%d</Obj>", colorText, tagBesetInfo.yunhun);
	strcat(_ruleMsg, tempText);
	strcat(_ruleMsg, "</Seg>");

	//排版结束
	strcat(_ruleMsg, "</Layout>");
	_compoundMoneyRequireText->getLayout()->SetText(_ruleMsg);
	_compoundMoneyRequireText->getLayout()->flashLayout();

	if(moenyYunhunFit)
	{
		_compoundCommit->enable();
		_autoCompoundCommit->enable();
	}
}

void KUiItemOperPanel::showTip()
{
	if(!_thisWindow)
	{
		return;
	}

	if(!_generateItemValidate)
	{
		return;
	}
	
	//显示icon
	const KBASICPROP_ITEM* itemBaseInfo;
	g_pCoreShell->GetGameData(GDI_GET_ITEM_BASIC_INFO_BY_TYPE, (UINT)&itemBaseInfo, (int)&_generateItem);

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

	_ruleMsg[0] = 0;
	strcat(_ruleMsg, tipHeadText);
	g_pCoreShell->GetGameData(GDI_LINKED_ITEM_LAYOUT_DESC, (unsigned int)_ruleMsg, (int)&itemType);
	
	strcat(_ruleMsg, "</Layout>");
	_ruleMsg[LAYOUT_TEXT_MAX_LEN - 1] = 0;

	Rect area = _generateItemImage->getUnclippedInnerRect();
	KUiItemTip::GetSingleton().show(_ruleMsg, area, KUiItemTip::Right);
}

bool KUiItemOperPanel::onHoverIcon(const EventArgs& args)
{
	showTip();
	return true;
}

bool KUiItemOperPanel::onLevaeIcon(const EventArgs& args)
{
	KUiItemTip::Hide();
	return true;
}

void KUiItemOperPanel::freshLevelUpInfo()
{
	if(!_thisWindow)
	{
		return;
	}
}

void KUiItemOperPanel::freshItemCount()
{
	if(!_thisWindow)
	{
		return;
	}

	for (int j = 0; j < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++j)
	{		
		int index = _itemInfo[j].Obj.uId;
		
		if(COMMON_ITEM_INVALID_ID == index)
		{
			continue;
		}
		
		int itemCount = g_pCoreShell->GetGameData(GDI_ITEM_COUNT_QUERY, index, NULL);
		if(itemCount >0)
		{
			TLGameObject::GameObject& objInfo = _item[j]->getObject();
			if(objInfo.d_count != itemCount)
			{
				_item[j]->setCount(itemCount);
			}
		}
		else
		{
			clearGameObj(_item[j]);
		}
	}
}

void KUiItemOperPanel::freshLockedItem()
{
	if(!_thisWindow)
	{
		return;
	}

	KUiItemLockMgr::getSingleton().clear();

	for(int i = 0; i < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++i)
	{
		if(_itemInfo[i].Obj.uGenre == CGOG_NOTHING)
		{
			continue;
		}
		KUiItemLockMgr::getSingleton().lock(_itemInfo[i].Obj.uId);
	}

	KUiItemLockMgr::getSingleton().refreshItemBox();
}

bool KUiItemOperPanel::onHoverRate(const EventArgs& args)
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

bool KUiItemOperPanel::onLevaeRate(const EventArgs& args)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiItemOperPanel::onAutoCommit(const EventArgs& args)
{
	if ( isAutoCommit() )
	{
		DisableAutoCommit();

		return false;
	}
	else
	{
		EnableAutoCommit();
		
		doCommit();
		
		return true;
	}

	return true;
}

bool KUiItemOperPanel::onCommit(const EventArgs& args)
{
	if ( isAutoCommit() )
	{
		return false;
	}

	doCommit();
	return true;
}

bool KUiItemOperPanel::onShow(const EventArgs& args)
{
	showLift();
	return true;
}

bool KUiItemOperPanel::onHide(const EventArgs& args)
{
	clear();
	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
	DisableAutoCommit();
	return true;
}

int KUiItemOperPanel::itemCountOfAType(ItemType& type)
{
	if(!_thisWindow)
	{
		return 0;
	}

	int count = 0;
	for(int i = 0; i < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++i)
	{
		int index = _itemInfo[i].Obj.uId;

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

void KUiItemOperPanel::clearRequireInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	_materialRequireText->getLayout()->clearLayout();
	_rateText->getLayout()->clearLayout();
	_moneyRequireText->getLayout()->clearLayout();
	_compoundMoneyRequireText->getLayout()->clearLayout();

	_ruleId = COMMON_ITEM_INVALID_ID;
}

void KUiItemOperPanel::onEndSmith(int code)
{
	if(!_thisWindow)
	{
		return;
	}
	
	if(!isVisible())
	{
		return;
	}
	_generateItemValidate = false;

	freshOpInfo();
	
	//打造成功后，更新物品锁定状态
	freshLockedItem();

	if(enchaser_error_no == code)
	{
		_endImage->enable();
		_endImage->play(true);

		if ( m_enableAutoCommit )
		{
			doCommit();
		}
	}
	else
	{
		_endFailImage->enable();
		_endFailImage->play(true);

		DisableAutoCommit();
	}
}

void KUiItemOperPanel::doCommit()
{
	if(_ruleId == COMMON_ITEM_INVALID_ID)
	{
		return;
	}
	
	KUiCompoundParam tagCompoundList;
	ZeroMemory(&tagCompoundList, sizeof(KUiCompoundParam) );
	
	int itemCount = 0;
	for ( int i = 0; i < UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT; ++i )
	{
		int itemIndex = _itemInfo[i].Obj.uId;
		
		if(COMMON_ITEM_INVALID_ID == itemIndex)
		{
			continue;
		}
		
		tagCompoundList.nItemIndex[i] = itemIndex;
		++itemCount;
	}
	
	tagCompoundList.nCompoundType = COMPOUND_SMITH;
	tagCompoundList.ruleId = _ruleId;
	tagCompoundList.szPlusInfo[0] = 0;
	
	
	g_pCoreShell->OperationRequest(GOI_COMPOUND_BEGIN, (unsigned int)&tagCompoundList, itemCount);
	
	if(LIFT == _curState)
	{
		_liftBeginAnimation->enable();
		_liftBeginAnimation->show();
		_liftBeginAnimation->play(true);
	}
	else if(COMPOUND == _curState)
	{
		_compBeginAnimation->enable();
		_compBeginAnimation->show();
		_compBeginAnimation->play(true);
	}
	return;	
}

#define autocompound_message 29

void KUiItemOperPanel::EnableAutoCommit()
{
	m_enableAutoCommit = true;
	char* btnMsg = KMessageCentre::GetMessage( autocompound_message, 2 );
	if ( ( NULL != btnMsg ) && ( NULL != _autoCompoundCommit ) )
	{
		_autoCompoundCommit->setText( AnsiToUtf8( btnMsg ) );
	}
}

void KUiItemOperPanel::DisableAutoCommit()
{
	m_enableAutoCommit = false;
	if ( NULL != _autoCompoundCommit )
	{
		_autoCompoundCommit->setText( m_autoCommitBtnText );
	}
}

bool KUiItemOperPanel::isAutoCommit()
{
	return m_enableAutoCommit;	
}