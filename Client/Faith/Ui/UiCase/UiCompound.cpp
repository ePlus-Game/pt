//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/26/2006 20:02
//      File_base        : UiCompound
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "UiCompound.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "UiDragItem.h"
#include "../KMessageCentre.h"
#include "UiItemTip.h"
#include "../UiConfigManager.h"
#include "UiItemLockMgr.h"
#include "UiComMsgBox.h"
#include "UiItemOperPanel.h"
#include "UiErrorMessageBox.h"


extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiCompound* KUiWndSingleton<KUiCompound>::ms_Singleton	= NULL;

KUiCompound::KUiCompound( const CEGUI::String& id_name ):
KUiWndSingleton<KUiCompound>( id_name )
{
	d_money = 0;
	d_isBusy = false;
	d_lastCommitTime = 0;
}


KUiCompound::~KUiCompound()
{

}

void KUiCompound::Init()
{
	if(!ms_Singleton || !m_pThisWnd )
		return;
	
	d_eCompoundType = COMPOUND_INVALID;
	
	char childName[COMMON_CLIENT_MSG_LEN_64];
	for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i)
	{
		sprintf(childName, "%s%d", "TaharezLook/Compound/Item", i);
		d_item[i] = (TLGameObject*)m_pThisWnd->getChild(childName);

		KObjAtContRegion* region = &d_itemInfo[i];

		region->eContainer = UOC_EQUIPTMENT;
		region->Region.v = i;
		d_item[i]->setUserData(region);
		
		d_itemGrid[i].setCtrl(d_item[i]);
		d_itemGrid[i].addTip();
		
		d_item[i]->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiCompound::onBDown, this));
	}

	sprintf(childName, "TaharezLook/Compound/%s", UI_COMPOUND_SHENGJI_TEXT);
	d_shengjiBtn = (TLButton*)m_pThisWnd->getChild(childName);
	d_shengjiBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiCompound::handleOK, this));

	sprintf(childName, "TaharezLook/Compound/%s", UI_COMPOUND_JIACHI_TEXT);
	d_jiachiBtn = (TLButton*)m_pThisWnd->getChild(childName);
	d_jiachiBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiCompound::handleOK, this));

	sprintf(childName, "TaharezLook/Compound/%s", UI_COMPOUND_CHAIYAO_TEXT);
	d_chaiyaoBtn = (TLButton*)m_pThisWnd->getChild(childName);
	d_chaiyaoBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiCompound::handleOK, this));

	sprintf(childName, "TaharezLook/Compound/%s", UI_COMPOUND_FUYAO_TEXT);
	d_fuyaoBtn = (TLButton*)m_pThisWnd->getChild(childName);
	d_fuyaoBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiCompound::handleOK, this));
	
	d_costWindow	= (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/Compound/Cost");
	d_jin			= (TLStaticText*)d_costWindow->getChild("TaharezLook/Compound/Cost/MoneyG");
	d_yin			= (TLStaticText*)d_costWindow->getChild("TaharezLook/Compound/Cost/MoneyY");
	d_tong			= (TLStaticText*)d_costWindow->getChild("TaharezLook/Compound/Cost/MoneyT");

	d_result			= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/Compound/Result");
	d_ruleFitCondition	= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/Compound/RuleFitCondition");
	d_ruleFitCondition->useLayout();

	d_yunhunWindow	= (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/Compound/Yunhun");
	d_curYunhun		= (TLStaticText*)d_yunhunWindow->getChild("TaharezLook/Compound/Yunhun/Cur");
	d_requireYunhun	= (TLStaticText*)d_yunhunWindow->getChild("TaharezLook/Compound/Yunhun/Require");

	Window* close = m_pThisWnd->getChild("TaharezLook/Compound/Close");
	close->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiCompound::handleExit, this));

	m_pThisWnd->getChild("TaharezLook/Compound/Bg")->setZLevel(Window::SuperBottom);
	//动画
	d_beginImage = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/Compound/Begin");
	d_beginImage->setZLevel(Window::Bottom);
	
	d_endImage = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/Compound/EndOk");
	d_endImage->setZLevel(Window::Bottom);

	d_endFailImage = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/Compound/EndFail");
	d_endFailImage->setZLevel(Window::Bottom);
	
	m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiCompound::onHide, this));
	m_pThisWnd->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiCompound::onShow, this));

	_lift = (TLRadioButton*)m_pThisWnd->getChild("TaharezLook/Compound/Lift");
	_compound = (TLRadioButton*)m_pThisWnd->getChild("TaharezLook/Compound/Compound");
	_levelUp = (TLRadioButton*)m_pThisWnd->getChild("TaharezLook/Compound/LevelUp");

	_lift->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiCompound::selectPanel, this));
	_compound->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiCompound::selectPanel, this));
	_levelUp->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiCompound::selectPanel, this));
}

void KUiCompound::clear()
{
	for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i)
	{
		clearIdx(i);
		/*KObjAtContRegion* region = (KObjAtContRegion*)d_item[i]->getUserData();
		region->Obj.uId = COMMON_ITEM_INVALID_ID;
		region->Obj.uGenre = CGOG_NOTHING;
		TLGameObject::GameObject object;
		object.d_gameobject = BACKGROUND_IMAGE;
		object.d_type = TLGameObject::idle;
		d_item[i]->setObject(object);//*/
	}

	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
}

void KUiCompound::clearIdx(int idx)
{
	KObjAtContRegion* region = (KObjAtContRegion*)d_item[idx]->getUserData();
	region->Obj.uId = COMMON_ITEM_INVALID_ID;
	region->Obj.uGenre = CGOG_NOTHING;
	TLGameObject::GameObject object;
	object.d_gameobject = BACKGROUND_IMAGE;
	object.d_type = TLGameObject::idle;
	d_item[idx]->setObject(object);
}

void KUiCompound::clearSameObj(int id)
{
	for ( int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i )
	{
		KObjAtContRegion* region = (KObjAtContRegion*)d_item[i]->getUserData();
		if ( region->Obj.uId == id )
		{
			clearIdx(i);
			return;
		}
	}
}

void KUiCompound::hideAllBtn( void )
{
	d_shengjiBtn->hide();
	d_jiachiBtn->hide();
	d_chaiyaoBtn->hide();
	d_fuyaoBtn->hide();

	d_beginImage->hide();
	d_endFailImage->hide();
	d_endImage->hide();
}

std::string	KUiCompound::getCompoundTypeStr(COMPOUNDTYPE type)
{
	switch(type)
	{
	case COMPOUND_LEVELUP:
		{
			return UI_COMPOUND_SHENGJI_TEXT;
		}
		break;
	case COMPOUND_ADDMAGIC:
		{
			return UI_COMPOUND_JIACHI_TEXT;
		}
		break;
	case COMPOUND_CLEAR:
		{
			return UI_COMPOUND_INVALID_TEXT;
		}
		break;
	case COMPOUND_ADDYAO:
		{
			return UI_COMPOUND_FUYAO_TEXT;
		}
		break;
	case COMPOUND_GETYAO:
		{
			return UI_COMPOUND_CHAIYAO_TEXT;
		}
		break;
	case COMPOUND_MAKE:
		{
			return UI_COMPOUND_INVALID_TEXT;
		}
		break;
	}
	return UI_COMPOUND_INVALID_TEXT;
}

void KUiCompound::Show(COMPOUNDTYPE eCompoundType)
{
	KUiWndSingleton<KUiCompound>::Show();

	ms_Singleton->d_eCompoundType = eCompoundType;
	ms_Singleton->hideAllBtn();
	ms_Singleton->clear();
	ms_Singleton->freshInfo();

	std::string btnName = ms_Singleton->getCompoundTypeStr(eCompoundType);
	char tempText[COMMON_CLIENT_MSG_LEN_128];
	sprintf(tempText, "TaharezLook/Compound/%s", btnName.c_str());
	ms_Singleton->m_pThisWnd->getChild(tempText)->show();
	if ( eCompoundType == COMPOUND_GETYAO )
	{
		for(int j = 1; j < MAX_LEVELUP_ITEMS_COUNT; ++j)
		{
			if ( ms_Singleton->d_item[j] )
			{
				ms_Singleton->d_item[j]->setSize( Absolute,Size(0,0));
			}
		}
	}
	else
	{
		for(int j = 1; j < MAX_LEVELUP_ITEMS_COUNT; ++j)
		{
			if ( ms_Singleton->d_item[j] )
			{
				ms_Singleton->d_item[j]->setSize( Absolute,Size(GAMEOBJECT_WIDTH_MID,GAMEOBJECT_WIDTH_MID));
			}
		}
	}
}

void KUiCompound::Update(int nRusult)
{
	if ( NULL == ms_Singleton )
	{
		return;
	}

//	ms_Singleton->clear();
	ms_Singleton->refreshItemInfo();
	
//	ms_Singleton->d_beginImage->stop();
//	ms_Singleton->d_beginImage->disable();
	
//	ms_Singleton->d_beginImage->hide();
	if ( nRusult == enchaser_error_ratesuccess )
	{
		ms_Singleton->d_endImage->enable();
		ms_Singleton->d_endImage->play(true);
	}
	else
	{
		ms_Singleton->d_endFailImage->enable();
		ms_Singleton->d_endFailImage->play(true);
	}
	
 	std::string tipText = ms_Singleton->getCompoundRstTip(nRusult);
 	if(tipText == "")
 	{
 		tipText = KMessageCentre::GetMessage(compound_error_message, nRusult);
 	}
 	ms_Singleton->d_result->setText(AnsiToUtf8(tipText.c_str()));
	KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(tipText.c_str()));
// 	KUiWndSingleton<KUiCompound>::Show();
	ms_Singleton->freshInfo();
	ms_Singleton->d_result->show();
}

bool KUiCompound::checkIsBusy()
{
	if ( d_isBusy )
	{
		if ( GetTickCount() - d_lastCommitTime < 1000 )
		{
			char* tipMsg = KMessageCentre::GetMessage( compound_error_message, 8 );
			if ( tipMsg )
			{
				KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( tipMsg ) );
			}
			return true;
		}
		else
		{
			d_isBusy = false;
			return false;
		}
	}
	else
	{
		return false;
	}
}

void KUiCompound::commit()
{
	if ( checkIsBusy() )
	{
		return;
	}

	d_lastCommitTime = GetTickCount();
	d_isBusy = true;

	KUiCompoundParam tagCompoundList;
	ZeroMemory(&tagCompoundList, sizeof(KUiCompoundParam));
	//类型
	tagCompoundList.nCompoundType = d_eCompoundType;

	//原材料
	int itemCount = 0;
	for ( int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i )
	{
		TLGameObject* item = d_item[i];
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)item->getUserData();
		if(CGOG_NOTHING == itemRegion->Obj.uGenre)
		{
			continue;
		}
		tagCompoundList.nItemIndex[i] = itemRegion->Obj.uId;
		++itemCount;
	}

	if(itemCount <= 0)
	{
		return;
	}

	memset(tagCompoundList.szPlusInfo, 0, COMMON_CLIENT_MSG_LEN_64);

	tagCompoundList.szPlusInfo[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	
	tagCompoundList.nMoney = 0;
	g_pCoreShell->OperationRequest( GOI_COMPOUND_BEGIN, (unsigned int)&tagCompoundList, MAX_LEVELUP_ITEMS_COUNT );
		
	d_beginImage->enable();
	d_beginImage->show();
	d_beginImage->play(true);
	
	d_result->setText("");
	d_ruleFitCondition->setText("");
	d_ruleFitCondition->getLayout()->clearLayout();
}

void Commit()
{
	KUiCompound::GetSingleton().commit();
}

bool KUiCompound::handleOK( const CEGUI::EventArgs& args )
{
	this->commit();
	/*
	KUiComMsgBox::Show();

	KUiComMsgBox& msgBox = KUiComMsgBox::GetSingleton();
	msgBox.setFristBtnCallback(Commit);
	msgBox.setMsg(AnsiToUtf8(KMessageCentre::GetMessage(compound_error_message, 1000)));

	char okBtnName[COMMON_CLIENT_MSG_LEN_32];
	char cancelBtnName[COMMON_CLIENT_MSG_LEN_32];
	strcpy(okBtnName, KMessageCentre::GetMessage(quest_message, 1));
	strcpy(cancelBtnName, KMessageCentre::GetMessage(quest_message, 2));
	msgBox.setBtnName(AnsiToUtf8(okBtnName), AnsiToUtf8(cancelBtnName));
	*/
	return true;
}

bool KUiCompound::handleExit( const CEGUI::EventArgs& args )
{
	Hide();
    return true;
}

bool KUiCompound::onBDown(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton)
		return false;

	TLGameObject* item = (TLGameObject*)arg->window;
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


	sourObj->clear();
	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		if(destObjInfo.d_type == TLGameObject::item)//拿起
		{
			//更新手上的物品
			sourObj->setObject(destObjInfo);
			sourObj->setCanDrag(true);
			*sourRegion = *destRegion;

			//清空被拿起的物品
			destRegion->Obj.uId = COMMON_ITEM_INVALID_ID;
			destRegion->Obj.uGenre = CGOG_NOTHING;
			TLGameObject::GameObject object;
			object.d_gameobject = BACKGROUND_IMAGE;
			object.d_type = TLGameObject::idle;
			destObj->setObject(object);
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
				sourRegion->Obj.uId = COMMON_ITEM_INVALID_ID;
				sourRegion->Obj.uGenre = CGOG_NOTHING;
				TLGameObject::GameObject object;
				object.d_gameobject = BACKGROUND_IMAGE;
				object.d_type = TLGameObject::idle;
				sourObj->setObject(object);

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
			sourRegion->Obj.uId = COMMON_ITEM_INVALID_ID;
			sourRegion->Obj.uGenre = CGOG_NOTHING;
			TLGameObject::GameObject object;
			object.d_gameobject = BACKGROUND_IMAGE;
			object.d_type = TLGameObject::idle;
			sourObj->setObject(object);

			sourObj->setCanDrag(false);
		}
	}

	freshInfo();

	//挪动物品后更新锁定状态
	freshLockedItem();

	return true;
}

void KUiCompound::freshInfo()
{
	KUiCompoundParam tagCompoundList;
	ZeroMemory(&tagCompoundList, sizeof(KUiCompoundParam));
	tagCompoundList.nCompoundType = d_eCompoundType;

	for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i)
	{
		TLGameObject* item = d_item[i];
		
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)item->getUserData();
		tagCompoundList.nItemIndex[i] = itemRegion->Obj.uId;
	}

	KUiCompoundRuleInfo tagBesetInfo;
	int nResult = g_pCoreShell->GetGameData(GDI_GET_COMPOUND_INFO, (unsigned int)&tagCompoundList, (int)&tagBesetInfo);

	d_result->hide();
	d_costWindow->hide();
	d_yunhunWindow->hide();

	d_shengjiBtn->disable();
	d_jiachiBtn->disable();
	d_chaiyaoBtn->disable();
	d_fuyaoBtn->disable();

	if(nResult != enchaser_error_no)
	{
		std::string ruleFitText = getRuleFillTip(nResult);
		if(ruleFitText == "")
		{
			ruleFitText = KMessageCentre::GetMessage(compound_error_message, nResult);
		}	
		if(d_eCompoundType == COMPOUND_LEVELUP)
		{
			d_ruleFitCondition->setText("");
			d_ruleFitCondition->getLayout()->SetText(const_cast<char*>(ruleFitText.c_str()));
			d_ruleFitCondition->getLayout()->flashLayout();
		}
		else
		{
			d_ruleFitCondition->getLayout()->clearLayout();
			d_ruleFitCondition->setText(AnsiToUtf8(ruleFitText.c_str()));
		}
		return;
	}

	const char* colorText = KUiCfgLoader::getSingleton().getShopCfg().moneyNotEnoughTextColor;
	int r, g, b;
	sscanf(colorText, "%d,%d,%d", &r, &g, &b);
	colour notEnoughColor((float)r / 255, (float)g / 255, (float)b / 255);
	colorText = KUiCfgLoader::getSingleton().getShopCfg().moneyTextColor;
	sscanf(colorText, "%d,%d,%d", &r, &g, &b);
	colour enoughColor((float)r / 255, (float)g / 255, (float)b / 255);

	bool fitMoneyAndYunhun = true;
	
	int requireMoney = tagBesetInfo.money;
	int reqireYunhun = tagBesetInfo.yunhun;
	if(requireMoney > 0)
	{
		d_costWindow->show();
		
		char moneyText[COMMON_CLIENT_MSG_LEN_32];
		int nG = 0;
		int nY = 0;
		int nT = 0;
		sysMoneyToUiMoney( requireMoney, nG, nY, nT );
		sprintf( moneyText, "%d", nG );
		d_jin->setText( moneyText );
		sprintf( moneyText, "%d", nY );
		d_yin->setText( moneyText );
		sprintf( moneyText, "%d", nT );
		d_tong->setText( moneyText );

		int holdMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
		if(requireMoney > holdMoney)
		{
			fitMoneyAndYunhun = false;
			nResult = enchaser_error_money;
			d_jin->setTextColours(notEnoughColor);
			d_yin->setTextColours(notEnoughColor);
			d_tong->setTextColours(notEnoughColor);
		}
		else
		{
			d_jin->setTextColours(enoughColor);
			d_yin->setTextColours(enoughColor);
			d_tong->setTextColours(enoughColor);
		}
	}

	if(reqireYunhun)
	{
		d_yunhunWindow->show();

		int haveYunhun = g_pCoreShell->GetGameData(GDI_GET_CUR_SKILL_POINT, 0, 0);
		char yunhunText[COMMON_CLIENT_MSG_LEN_32];
		sprintf(yunhunText, "%d", haveYunhun);
		d_curYunhun->setText(yunhunText);

		sprintf(yunhunText, "%d", reqireYunhun);
		d_requireYunhun->setText(yunhunText);

		if(reqireYunhun > haveYunhun)
		{
			fitMoneyAndYunhun = false;
			nResult = enchaser_error_skillpoint;
			d_curYunhun->setTextColours(notEnoughColor);
			d_requireYunhun->setTextColours(notEnoughColor);
		}
		else
		{
			d_curYunhun->setTextColours(enoughColor);
			d_requireYunhun->setTextColours(enoughColor);
		}
	}

	std::string ruleFitText = getRuleFillTip(nResult);
	if(ruleFitText == "")
	{
		ruleFitText = KMessageCentre::GetMessage(compound_error_message, nResult);
	}

	if(d_eCompoundType == COMPOUND_LEVELUP)
	{
		d_ruleFitCondition->setText("");
		d_ruleFitCondition->getLayout()->SetText(const_cast<char*>(ruleFitText.c_str()));
		d_ruleFitCondition->getLayout()->flashLayout();
	}
	else
	{
		d_ruleFitCondition->getLayout()->clearLayout();
		d_ruleFitCondition->setText(AnsiToUtf8(ruleFitText.c_str()));
	}

	if(fitMoneyAndYunhun)
	{
		d_shengjiBtn->enable();
		d_jiachiBtn->enable();
		d_chaiyaoBtn->enable();
		d_fuyaoBtn->enable();

		d_result->show();
		std::string rstText = getResultTip(tagBesetInfo);
		d_result->setText(AnsiToUtf8(rstText.c_str()));
	}
}

std::string KUiCompound::getResultTip(const KUiCompoundRuleInfo& ruleInfo)
{
	int invalidRate = 100 - ruleInfo.successRate - ruleInfo.destoryRate - ruleInfo.levelDownRate;

	switch(d_eCompoundType)
	{
	case COMPOUND_LEVELUP:
		{
			//以下逻辑假设成功率(ruleInfo.successRate)不可能为0的情况
			if(invalidRate)
			{
				if(ruleInfo.levelDownRate)
				{
					if(ruleInfo.destoryRate)
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_destory_or_leveldown_or_not);
					}
					else
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_leveldown_or_not);
					}
				}
				else
				{
					if(ruleInfo.destoryRate)
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_destory_or_not);
					}
					else
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_not);
					}
				}
			}
			else
			{
				if(ruleInfo.levelDownRate)
				{
					if(ruleInfo.destoryRate)
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_destory_or_leveldown);
					}
					else
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_leveldown);
					}
				}
				else
				{
					if(ruleInfo.destoryRate)
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_success_or_destory);
					}
					else
					{
						return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_levelup_full_success);
					}
				}
			}
		}
		break;
	case COMPOUND_ADDMAGIC:
		{
			if(invalidRate)
			{
				if(ruleInfo.destoryRate)
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addmagic_success_or_not_or_destory);
				}
				else
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addmagic_success_or_not);
				}			
			}
			else
			{
				if(ruleInfo.destoryRate)
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addmagic_success_or_destory);
				}
				else
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addmagic_full_success);
				}
			}
		}
		break;
	case COMPOUND_GETYAO:
		{
			if(invalidRate)
			{
				return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_getyao_success_or_not);
			}
			else
			{
				return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_getyao_full_success);
			}
		}
		break;
	case COMPOUND_ADDYAO:
		{
			if(invalidRate)
			{
				if(ruleInfo.destoryRate)
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addyao_success_or_not_or_destory);
				}
				else
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addyao_success_or_not);
				}			
			}
			else
			{
				if(ruleInfo.destoryRate)
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addyao_success_or_destory);
				}
				else
				{
					return KMessageCentre::GetMessage(compound_tip_message, enchaser_tip_addyao_full_success);
				}
			}
		}
		break;
	default:
		{

		}
		break;
	}
	return "";
}

std::string KUiCompound::getRuleFillTip(int rstCode)
{
	switch(d_eCompoundType)
	{
	case COMPOUND_LEVELUP:
		{
			if(enchaser_error_no == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_levelup_valid);
			}
			else if(enchaser_error_less_material == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_levelup_less_material);
			}
			else if(enchaser_error_less_targetItem == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_levelup_less_targetItem);
			}
			else if(enchaser_error_targetItem_invalid == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_levelup_targetitem_invalid);
			}
		}
		break;
	case COMPOUND_ADDMAGIC:
		{
			if(enchaser_error_no == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_addmagic_valid);
			}
			else if(enchaser_error_less_material == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_addmagic_less_material);
			}
		}
		break;
	case COMPOUND_GETYAO:
		{
			if(enchaser_error_no == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_getyao_valid);
			}
			else if(enchaser_error_less_targetItem == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_getyao_less_targetItem);
			}
			else if(enchaser_error_targetItem_invalid == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_getyao_targetitem_invalid);
			}
		}
		break;
	case COMPOUND_ADDYAO:
		{
			if(enchaser_error_no == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_addyao_valid);
			}
			else if(enchaser_error_less_material == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_addyao_less_material);
			}
			else if(enchaser_error_less_targetItem == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_getyao_less_targetItem);
			}
			else if(enchaser_error_targetItem_invalid == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rule_fit_message, enchaser_rule_addyao_targetitem_invalid);
			}
		}
		break;
	default:
		{

		}
		break;
	}
	return "";
}

std::string KUiCompound::getCompoundRstTip(int rstCode)
{
	switch(d_eCompoundType)
	{
	case COMPOUND_LEVELUP:
		{
			if(enchaser_error_ratesuccess == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_levelup_success);
			}
			else if(enchaser_error_ratefailed == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_levelup_invalid);
			}
			else if(enchaser_error_ratedestroy == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_levelup_destory);
			}
			else if(enchaser_error_level_down == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_levelup_leveldown);
			}
		}
		break;
	case COMPOUND_ADDMAGIC:
		{
			if(enchaser_error_ratesuccess == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_addmagic_success);
			}
			else if(enchaser_error_ratefailed == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_addmagic_invalid);
			}
			else if(enchaser_error_ratedestroy == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_addmagic_destory);
			}
		}
		break;
	case COMPOUND_GETYAO:
		{
			if(enchaser_error_ratesuccess == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_getyao_success);
			}
			else if(enchaser_error_ratedestroy == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_getyao_fail);
			}
		}
		break;
	case COMPOUND_ADDYAO:
		{
			if(enchaser_error_ratesuccess == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_addyao_success);
			}
			else if(enchaser_error_ratefailed == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_addyao_invalid);
			}
			else if(enchaser_error_ratedestroy == rstCode)
			{
				return KMessageCentre::GetMessage(compound_rst_message, enchaser_rst_addyao_destory);
			}
		}
		break;
	default:
		{

		}
		break;
	}
	return "";
}

void KUiCompound::freshLockedItem()
{
	KUiItemLockMgr::getSingleton().clear();

	for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i)
	{
		if(d_itemInfo[i].Obj.uGenre == CGOG_NOTHING)
		{
			continue;
		}
		KUiItemLockMgr::getSingleton().lock(d_itemInfo[i].Obj.uId);
	}

	KUiItemLockMgr::getSingleton().refreshItemBox();
}

bool KUiCompound::onHide( const EventArgs& args )
{
	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
	return true;
}

bool KUiCompound::onShow( const EventArgs& args )
{
	_levelUp->setSelected(true);
	KUiItemLockMgr::getSingleton().clear();
	KUiItemLockMgr::getSingleton().refreshItemBox();
	return true;
}

bool KUiCompound::selectPanel(const EventArgs& args)
{
	WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
	if(eventArgs->window == _lift)
	{
		KUiItemOperPanel::getSingleton().show();
		KUiItemOperPanel::getSingleton().showLift();
		KUiItemOperPanel::getSingleton().setPos(m_pThisWnd->getPosition(Absolute));
		Hide();
	}
	else if(eventArgs->window == _compound)
	{
		KUiItemOperPanel::getSingleton().show();
		KUiItemOperPanel::getSingleton().showCompound();
		KUiItemOperPanel::getSingleton().setPos(m_pThisWnd->getPosition(Absolute));
		Hide();
	}
	else if(eventArgs->window == _levelUp)
	{
		//do nothing
	}

	KUiItemOperPanel::getSingleton().DisableAutoCommit();
	return true;
}

void KUiCompound::refreshItemInfo()
{
	//更新物品数量
	for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i)
	{
		int index = d_itemInfo[i].Obj.uId;
		
		if( COMMON_ITEM_INVALID_ID == index )
		{
			continue;
		}
		
		int itemCount = g_pCoreShell->GetGameData( GDI_ITEM_COUNT_QUERY, index, NULL );
		if( itemCount > 0 )
		{
			TLGameObject::GameObject& objInfo = d_item[i]->getObject();
			if( objInfo.d_count != itemCount )
			{
				d_item[i]->setCount( itemCount );
			}
		}
		else
		{
			clearIdx( i );
		}
	}
}

void KUiCompound::AutoAddNewItem( int index, ItemPos* iPos )
{
	clearIdx( 0 );

	//取得物品基本信息
	KItemInfo itemInfo;
	int ret = g_pCoreShell->GetGameData( 
		GDI_ITEM_INFO_INDEX, 
		reinterpret_cast< unsigned int >( &itemInfo ), 
		index );
	if ( ret == 0 )
	{
		return;
	}
	
	//取得tip
	char itemTip[GOD_MAX_OBJ_DESC_LEN] = { 0 };
	strcpy( itemTip, "<Layout width=200 margin-top=10 margin-left=10 margin-right=10 margin-bottom=10>" );
	g_pCoreShell->GetGameData( 
		GDI_MY_ITEM_LAYOUT_DESC, 
		reinterpret_cast< unsigned int >( itemTip ),
		index );
	
	TLGameObject::GameObject goInfo;
 	goInfo.d_type			= TLGameObject::item;
 	goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
 	goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
 	goInfo.d_count			= 1;
	goInfo.d_EdgeframeIdx	= itemInfo.colour;
 	d_item[0]->setObject( goInfo );
	d_item[0]->setTooltipText( AnsiToUtf8( itemTip ) );	

	KObjAtContRegion* objInfo = static_cast< KObjAtContRegion* >( d_item[0]->getUserData() );
	objInfo->Obj.uGenre = CGOG_ITEM;
	objInfo->Obj.uId = index;
  	objInfo->Region.h = iPos->nX;
  	objInfo->Region.v = iPos->nY;
  	objInfo->Region.Width = 0;
  	objInfo->Region.Height = 1;
	objInfo->eContainer = UOC_ITEM_TAKE_WITH;

	freshInfo();
	freshLockedItem();

	d_isBusy = false;
}
