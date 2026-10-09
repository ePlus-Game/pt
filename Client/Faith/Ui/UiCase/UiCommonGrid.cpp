#include "UiCommonGrid.h"
#include "GameDataDef.h"
#include "UiItemTip.h"
#include "UiTipGenerator.h"

KUiCommonGrid::KUiCommonGrid()
{
	d_goCtrl = NULL;
	d_showCompare = true;
}

KUiCommonGrid::~KUiCommonGrid()
{

}

void KUiCommonGrid::addTip()
{
	if(NULL == d_goCtrl)
		return;
	
	d_goCtrl->subscribeEvent(TLGameObject::EventMouseEnters, Event::Subscriber(&KUiCommonGrid::onMouseEnters, this));
	d_goCtrl->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiCommonGrid::onMouseLeaves, this));
	d_goCtrl->subscribeEvent(TLGameObject::EventMouseMove, Event::Subscriber(&KUiCommonGrid::onMouseMove, this));
	d_goCtrl->subscribeEvent(TLGameObject::EventObjectChanged, Event::Subscriber(&KUiCommonGrid::onObjectChanged, this));
}

void KUiCommonGrid::showTip()
{
	if(NULL == d_goCtrl)
		return;

	KObjAtContRegion* region = (KObjAtContRegion*)d_goCtrl->getUserData();
	if(NULL == region)
		return;
	
	if(CGOG_NOTHING == region->Obj.uGenre)
	{
		KUiItemTip::Hide();
		return;
	}
	
	KUiTipGenerator::TipObject tipObj;
	switch(region->Obj.uGenre)
	{
	case CGOG_ITEM:
		{
			if(region->Region.Width == 0)//物品
			{
				tipObj.count = d_goCtrl->getCount();
				tipObj.type = KUiTipGenerator::MyItem;
				tipObj.ids[0] = region->Obj.uId;
			}
			else if(region->Region.Width == 1)//卦位
			{
				tipObj.type = KUiTipGenerator::MyGua;
				tipObj.ids[0] = region->Region.h;
			}
			else if(region->Region.Width == 2)//查看对方的装备
			{
				tipObj.type = KUiTipGenerator::OppositeItem;
				tipObj.ids[0] = region->Obj.uId;
			}
			else if(region->Region.Width == 3)//查看对方的卦位
			{
				tipObj.type = KUiTipGenerator::OppositeGua;
				tipObj.ids[0] = region->Region.h;
			}
			else if(region->Region.Width == 4)//查看法宝上的内丹
			{
				tipObj.type = KUiTipGenerator::InsideBall;
				tipObj.ids[0] = region->Region.h;	
				tipObj.ids[1] = region->Region.v;											
			}
		}
		break;
	case CGOG_NPCSELLITEM:
		{
			if (region->bPlusShop)
			{
				tipObj.type = KUiTipGenerator::PlusPointShopItem;
			}
			else
			{
				tipObj.type = KUiTipGenerator::ShopItem;
			}
			
			tipObj.ids[0] = region->Obj.uId;	
		}
		break;
	case CGOG_ICON:
		{
			tipObj.type = KUiTipGenerator::LinkedItem;
			tipObj.ids[0] = region->Region.h;
			tipObj.ids[1] = region->Region.v;
			tipObj.ids[2] = region->Region.Width;
			tipObj.ids[3] = region->Region.Height;
			tipObj.ids[4] = -1;
		}
		break;
	case CGOG_QUEST:
		{
			tipObj.type = KUiTipGenerator::QuestIcon;
			tipObj.ids[0] = region->Region.h;
		}
		break;
	}

	char* layoutText = KUiTipGenerator::getSinglton().genLayoutDes(tipObj);
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show(layoutText, d_goCtrl->getUnclippedInnerRect());

	if(d_showCompare)
	{
		char* compareLayoutText = KUiTipGenerator::getSinglton().genCompareLayoutDes(tipObj);
		KUiItemTip::GetSingleton().showCompare(compareLayoutText);
	}
}

bool KUiCommonGrid::onMouseEnters(const CEGUI::EventArgs& e)
{
	showTip();
	return true;
}

bool KUiCommonGrid::onMouseLeaves(const CEGUI::EventArgs& e)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiCommonGrid::onMouseMove(const CEGUI::EventArgs& e)
{
	if(KUiItemTip::IsVisible() == false)
		showTip();
// 	if(d_goCtrl->isActive() == false)
// 		d_goCtrl->activate();
	return true;
}

bool KUiCommonGrid::onObjectChanged(const CEGUI::EventArgs& e)
{
	//如果鼠标还在这个icon上，则更新tip
	Point mousePos = MouseCursor::getSingleton().getPosition();
	if(d_goCtrl->isVisible() && d_goCtrl->getInnerRect().isPointInRect(mousePos))
	{		
		showTip();
	}
	return true;
}