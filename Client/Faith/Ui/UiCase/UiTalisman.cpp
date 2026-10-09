#include "UiTalisman.h"
#include "CoreShell.h"
#include "../KMessageCentre.h"
#include "UiComMsgBox.h"
#include "UiDragItem.h"
#include "UiChatWindow.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

void doInsertBall()
{
	KUiTalisman::getSingleton().insertBall();
}

KUiTalisman::KUiTalisman()
{
	load();
	_thisWindow->hide();
	d_talismanId = COMMON_ITEM_INVALID_ID;
}

KUiTalisman::~KUiTalisman()
{

}

KUiTalisman& KUiTalisman::getSingleton()
{
	static KUiTalisman talisman;
	return talisman;
}

void KUiTalisman::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_TALISMAN_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_TALISMAN_WINDOW_PATH);
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
	
	//法宝信息
	d_tm_nameText			= (TLStaticText*)_thisWindow->getChild("TaharezLook/Talisman/Name");
	d_tm_levelText			= (TLStaticText*)_thisWindow->getChild("TaharezLook/Talisman/Level");
	d_tm_curyunhunText		= (TLStaticText*)_thisWindow->getChild("TaharezLook/Talisman/CurYunHun");
	d_tm_maxyunhunText		= (TLStaticText*)_thisWindow->getChild("TaharezLook/Talisman/MaxYunHun");
	d_tm_descriptionText	= (TLEditbox*)_thisWindow->getChild("TaharezLook/Talisman/Description");

	d_tm_iconImage			= (TLGameObject*)_thisWindow->getChild("TaharezLook/Talisman/Icon");
	d_tm_iconImage->subscribeEvent(TLGameObject::EventMouseButtonUp, 
		Event::Subscriber(&KUiTalisman::onClickIcon, this));
	static KObjAtContRegion talismanInfo;
	d_tm_iconImage->setUserData(&talismanInfo);
	d_tm_iconImageGrid.setCtrl(d_tm_iconImage);
	d_tm_iconImageGrid.addTip();

	//升级按钮
	d_tm_uplevelButton		= (TLButton*)_thisWindow->getChild("TaharezLook/Talisman/Uplevel");
	d_tm_uplevelButton->subscribeEvent(TLButton::EventClicked, 
		Event::Subscriber(&KUiTalisman::onClickUpgrade, this));

	//孔和内丹
	static KObjAtContRegion ballInfo[TM_HoleRowCount][TM_MaxHoleCountPerRow];

	char ctrlName[CtrlNameLen] = "";
	for(int i = 0; i < TM_HoleRowCount; ++i)
	{
		for(int j = 0; j < TM_MaxHoleCountPerRow; ++j)
		{
			sprintf(ctrlName, "TaharezLook/Talisman/Hole%d-%d", i + 1, j + 1);
			d_tm_holeImage[i][j] = (TLButton*)_thisWindow->getChild(ctrlName);
			d_tm_holeImage[i][j]->subscribeEvent(TLGameObject::EventMouseButtonDown, 
				Event::Subscriber(&KUiTalisman::onClickHole, this));
			d_tm_holeImage[i][j]->setZLevel(Window::Top);

			sprintf(ctrlName, "TaharezLook/Talisman/Ball%d-%d", i + 1, j + 1);
			d_tm_ballImage[i][j] = (TLGameObject*)_thisWindow->getChild(ctrlName);

			d_tm_ballImageGrid[i][j].setCtrl(d_tm_ballImage[i][j]);
			d_tm_ballImageGrid[i][j].addTip();
			
			ballInfo[i][j].Obj.uId = -1;
			ballInfo[i][j].Obj.uGenre = CGOG_ITEM;
			ballInfo[i][j].Region.h = -1;
			ballInfo[i][j].Region.v = i * TM_MaxHoleCountPerRow + j;
			ballInfo[i][j].Region.Height = 1;
			ballInfo[i][j].Region.Width = 4;		//4表示已经镶嵌到法宝上的内丹
			d_tm_ballImage[i][j]->setUserData(&ballInfo[i][j]);
			d_tm_ballImage[i][j]->setZLevel(Window::SuperTop);
		}
	}

	//层
	for(int layerIndex = 0; layerIndex < TM_HoleRowCount; ++layerIndex)
	{
		for(int layerStateIndex = 0; layerStateIndex < TM_LAYER_STATE_COUNT; ++layerStateIndex)
		{
			sprintf(ctrlName, "TaharezLook/Talisman/Layer%d-%d", layerIndex + 1, layerStateIndex + 1);
			d_layer[layerIndex][layerStateIndex] = (TLStaticImage*)_thisWindow->getChild(ctrlName);
			TLStaticImage* thisCtrl = d_layer[layerIndex][layerStateIndex];
			thisCtrl->setZLevel(Window::Bottom);
			thisCtrl->hide();
		}
	}

	//关闭按钮
	d_tm_closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Talisman/Close");
	d_tm_closeBtn->subscribeEvent(TLGameObject::EventMouseClick, 
		Event::Subscriber(&KUiTalisman::onClickClose, this));

}

bool KUiTalisman::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}

void KUiTalisman::show()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
	getTalismanInfo();
}

void KUiTalisman::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

void KUiTalisman::toggle()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_thisWindow->isVisible())
	{
		_thisWindow->hide();
	}
	else
	{
		_thisWindow->show();
	}
}


void KUiTalisman::getTalismanInfo()
{
	if(!_thisWindow)
	{
		return;
	}

	clear();
	if(COMMON_ITEM_INVALID_ID == d_talismanId)
	{
		return;
	}

	TALISMAN_INFO tmInfo;
	tmInfo.canUplevel		= -1;
	tmInfo.curYunHun		= -1;
	tmInfo.id				= -1;
	tmInfo.level			= -1;
	tmInfo.maxYunHun		= -1;
	tmInfo.description[0]	= '\0';
	tmInfo.name[0]			= '\0';
	for(int k = 0; k < TM_HOLE_NUM; ++k)
	{
		tmInfo.holeBuffId[k] = -1;
	}
	g_pCoreShell->GetGameData(GDI_TALISMAN_SELF, (unsigned int)&tmInfo, d_talismanId);

// 	tmInfo.holeBuffId[0] = 1;
// 	tmInfo.holeBuffId[1] = 0;
// 	tmInfo.holeBuffId[2] = -1;
// 	tmInfo.holeBuffId[3] = 1;
// 	tmInfo.holeBuffId[4] = 1;
// 	tmInfo.holeBuffId[5] = 1;
// 	tmInfo.holeBuffId[6] = 1;
// 	tmInfo.holeBuffId[7] = 0;
// 	tmInfo.holeBuffId[8] = 1;
	if('\0' == tmInfo.name[0])
	{
		return;
	}
	//先清空所有已显示的信息
	
	KObjAtContRegion* objInfo = (KObjAtContRegion*)d_tm_iconImage->getUserData();
	objInfo->Obj.uGenre = CGOG_ITEM;
	objInfo->Region.Width = 0;
	objInfo->Obj.uId = g_pCoreShell->GetGameData(GDI_GET_ITEM_INDEX_BY_ID, d_talismanId, NULL);;

	//法宝名
	d_tm_nameText->setText(AnsiToUtf8(tmInfo.name));

	//等级
	char levelText[CtrlNameLen] = "";
	sprintf(levelText, "%d", tmInfo.level);
	d_tm_levelText->setText(levelText);

	//升级按钮
	d_tm_canUplevel = tmInfo.canUplevel;
	if(d_tm_canUplevel)
	{
		d_tm_uplevelButton->enable();
	}
	else
	{
		d_tm_uplevelButton->disable();
	}

	//蕴魂
	char curYunhun[CtrlNameLen];
	char maxYunhun[CtrlNameLen];
	sprintf(curYunhun, "%d", tmInfo.curYunHun);
	sprintf(maxYunhun, "%d", tmInfo.maxYunHun);
	d_tm_curyunhunText->setText(curYunhun);
	d_tm_maxyunhunText->setText(maxYunhun);

	//描述
	d_tm_descriptionText->setText(AnsiToUtf8(tmInfo.description));

	//法宝图标
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData(GDI_ITEM_INFO_ID, (unsigned int)&tagItemInfo, tmInfo.id);
	TLGameObject::GameObject go;
	go.d_gameobjectSet = tagItemInfo.szImageSet;
	go.d_gameobject = tagItemInfo.szImage;
	go.d_count = 1;
	go.d_type = TLGameObject::item;
	d_tm_iconImage->setObject(go);

	//显示内丹状态
// 	const char* activeHoleImageset = KUiCfgLoader::getSingleton().getTalismanCfg().activeHoleIms;
// 	const char* activeHoleImage = KUiCfgLoader::getSingleton().getTalismanCfg().activeHoleImage;
// 	const char* deactiveHoleImageset = KUiCfgLoader::getSingleton().getTalismanCfg().deactiveHoleIms;
// 	const char* deactiveHoleImage = KUiCfgLoader::getSingleton().getTalismanCfg().deactiveHoleImage;
				
	for(int row = 0; row < TM_HoleRowCount; ++row)
	{
		int curLayerState = 0;
		for(int col = 0; col < TM_MaxHoleCountPerRow; ++col)
		{
			TLButton* hole = d_tm_holeImage[row][col];
			TLGameObject* ball = d_tm_ballImage[row][col];

			int insideballIndex = row * TM_MaxHoleCountPerRow + col;
			
			d_tm_holeState[row][col] = tmInfo.holeBuffId[insideballIndex];
			
			KObjAtContRegion* ballInfo = (KObjAtContRegion*)d_tm_ballImage[row][col]->getUserData();
			ballInfo->Region.h = g_pCoreShell->GetGameData(GDI_GET_ITEM_INDEX_BY_ID, d_talismanId, NULL);
			ballInfo->Region.v = insideballIndex;
			
			TLGameObject::GameObject go;
			go.d_count = ballInfo->Region.Height;
			if(tmInfo.holeBuffId[insideballIndex] > 0)
			{
				//如果有内丹则显示内丹所对应的Buff的图片
				//先显示边框
				hole->hide();

				//再显示内丹图片
				KItemInfo ballInfo;
				if(g_pCoreShell->GetGameData(GDI_INSIDE_BALL_IMAGE_INFO, 
					(unsigned int)&ballInfo, tmInfo.holeBuffId[insideballIndex]) == false)			
				{
					go.d_gameobjectSet = ballInfo.szImageSet;
					go.d_gameobject = ballInfo.szImage;
					go.d_type = TLGameObject::item;
					go.d_count = 1;
					ball->setObject(go);
					ball->setVisible(true);
				}			
				else
				{
					ball->setVisible(false);
				}

				//本处孔开启
				++curLayerState;
			}
			else if(tmInfo.holeBuffId[insideballIndex] == 0)
			{
				//如果有孔，但是没有内丹，则显示孔的图片
				hole->show();
				hole->enable();

				//内丹不显示
				ball->hide();

				//本处孔开启
				++curLayerState;
			}
			else
			{
				//孔显示未开启状态
				hole->hide();
				
				//内丹不显示
				ball->hide();
			}
		}

		if(curLayerState > 0 && curLayerState <= TM_LAYER_STATE_COUNT)
		{
			d_layer[row][curLayerState - 1]->show();
		}
	}
}

void KUiTalisman::onTalismanPropChange()
{
	if(!_thisWindow)
	{
		return;
	}

	getTalismanInfo();
}

void  KUiTalisman::onTalismanPotentialChange(int curProtential)
{
	if(!_thisWindow)
	{
		return;
	}

	char protentialText[CtrlNameLen];
	sprintf(protentialText, "%d", curProtential);
	d_tm_curyunhunText->setText(protentialText);
}

bool KUiTalisman::onClickUpgrade(const EventArgs& e)
{
	//判断师是否已经到了法宝经验上线
	TALISMAN_INFO tmInfo;
	tmInfo.canUplevel		= -1;
	tmInfo.curYunHun		= -1;
	tmInfo.id				= -1;
	tmInfo.level			= -1;
	tmInfo.maxYunHun		= -1;
	tmInfo.description[0]	= '\0';
	tmInfo.name[0]			= '\0';
	for(int k = 0; k < TM_HOLE_NUM; ++k)
	{
		tmInfo.holeBuffId[k] = -1;
	}
	g_pCoreShell->GetGameData(GDI_TALISMAN_SELF, (unsigned int)&tmInfo, d_talismanId);

	if (tmInfo.name[0]=='\0')
		return true;

	if(d_tm_canUplevel > 0 && d_talismanId != COMMON_ITEM_INVALID_ID)
	{
		g_pCoreShell->OperationRequest(GOI_TALISMAN_UPGRADE, (unsigned int)d_talismanId, NULL);
	}
	
	return true;
}

//嵌入内丹
bool KUiTalisman::onClickHole(const EventArgs& e)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&e;
	
	//判定手上是否拿着其他物品
	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
	KObjAtContRegion* sourRegion = (KObjAtContRegion*)sourObj->getUserData();
	TLGameObject::GameObject& sourObjInfo = sourObj->getObject();
	
	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		return true;
	}
	
	//如果手上有物品，则得到手上拿着物品的index
	d_curInsideBallId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, (unsigned int)sourRegion->Obj.uId, NULL);
	if(isInsideBall(d_curInsideBallId) == false)
	{
		d_curInsideBallId = -1;
		char msg[COMMON_DESC_LENGTH];
		strcpy(msg, KMessageCentre::GetMessage(talisman_message, TM_PLACE_INSIDE_BALL_FAILURE));
		KUiChannelCentre::GetSingleton().toSysMsg(msg);
		return true;	
	}

	//得到孔的index
	d_curClickHole.talismanId = d_talismanId;
	for(int i = 0; i < TM_HOLE_NUM; ++i)
	{
		TLButton* hole = d_tm_holeImage[i / TM_MaxHoleCountPerRow][i % TM_MaxHoleCountPerRow];	
		if(hole == mouse->window)
		{
			d_curClickHole.holeIndex = i;
			break;
		}
	}
	
	KUiComMsgBox::Show();
	KUiComMsgBox::GetSingleton().setFristBtnCallback(doInsertBall);
	
	char ctrlNameOk[CtrlNameLen];
	char ctrlNameCancel[CtrlNameLen];
	char msg[CtrlNameLen];
	strcpy(ctrlNameOk, KMessageCentre::GetMessage(talisman_message, TM_LEVELUP_BTN_OK));
	strcpy(ctrlNameCancel, KMessageCentre::GetMessage(talisman_message, TM_LEVELUP_BTN_CANCEL));
	strcpy(msg, KMessageCentre::GetMessage(talisman_message, TM_PLACE_INSIDE_BALL_COMFIRM));
	KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(msg));
	KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(ctrlNameOk), AnsiToUtf8(ctrlNameCancel));

	KUiDragItem::GetSingleton().initItem();
	
	return true;
}

void KUiTalisman::insertBall()
{
	int nRet = g_pCoreShell->OperationRequest(GOI_TALISMAN_INLAY, (UINT)&d_curClickHole, d_curInsideBallId);
}

bool KUiTalisman::onClickIcon(const EventArgs& e)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&e;
	if(RightButton == mouse->button)
	{
		return true;
	}

	//手
	TLGameObject* sourObj = KUiDragItem::GetSingleton().getObj();
	KObjAtContRegion* sourRegion = (KObjAtContRegion*)sourObj->getUserData();
	TLGameObject::GameObject& sourObjInfo = sourObj->getObject();
	//法宝框
	TLGameObject* destObj = d_tm_iconImage;
	KObjAtContRegion* destRegion = (KObjAtContRegion*)destObj->getUserData();
	TLGameObject::GameObject& destObjInfo = destObj->getObject();

	if(destRegion->Obj.uGenre == CGOG_NOTHING && sourRegion->Obj.uGenre == CGOG_NOTHING)
	{
		return true;
	}

	//手上没有物品，拿起
	if(CGOG_NOTHING == sourRegion->Obj.uGenre)
	{
		*sourRegion = *destRegion;
		sourObj->setObject(destObjInfo);
		sourObj->setCanDrag(true);

		d_talismanId = COMMON_ITEM_INVALID_ID;
	}
	else
	{
		int itemId = g_pCoreShell->GetGameData(GDI_GET_ITEM_ID_BY_INDEX, (unsigned int)sourRegion->Obj.uId, NULL);
		if(isTalisman(itemId) == false)
		{
			char msg[COMMON_DESC_LENGTH];
			strcpy(msg, KMessageCentre::GetMessage(talisman_message, TM_PLACE_TALISMAN_FAILURE));
			KUiChannelCentre::GetSingleton().toSysMsg(msg);
			return true;
		}

		//法宝框上没有物品，放下
		if(CGOG_NOTHING == destRegion->Obj.uGenre)
		{
			*destRegion = *sourRegion;
			destObj->setObject(sourObjInfo);

			KUiDragItem::GetSingleton().initItem();
			sourObj->setCanDrag(false);
		}
		else//都有、交换
		{
			KObjAtContRegion tempRegion;
			tempRegion = *destRegion;
			*destRegion = *sourRegion;
			destObj->setObject(sourObjInfo);

			*sourRegion = tempRegion;
			sourObj->setObject(destObjInfo);
		}
		d_talismanId = itemId;
	}
	
	getTalismanInfo();

	return true;
}

bool KUiTalisman::onClickClose(const EventArgs& e)
{
	_thisWindow->hide();
	return true;
}

bool KUiTalisman::isTalisman(int itemId)
{
	return g_pCoreShell->GetGameData(GDI_IS_TALISMAN, (unsigned int)itemId, NULL) == TRUE ? true : false;
}

bool KUiTalisman::isInsideBall(int itemId)
{
	return g_pCoreShell->GetGameData(GDI_IS_INSIDE_BALL, (unsigned int)itemId, NULL)  == TRUE ? true : false;
}

void KUiTalisman::clear()
{
	if(!_thisWindow)
	{
		return;
	}

//	d_talismanId = COMMON_ITEM_INVALID_ID;

	//法宝名
	d_tm_nameText->setText("--");
	//等级
	d_tm_levelText->setText("-");
	//升级按钮
	d_tm_uplevelButton->disable();
	//蕴魂
	d_tm_curyunhunText->setText("-");
	d_tm_maxyunhunText->setText("-");
	//描述
	d_tm_descriptionText->setText("--");
	
	//设置法宝tip显示 会由之后的setObject触发tip更新
	KObjAtContRegion* objInfo = (KObjAtContRegion*)d_tm_iconImage->getUserData();
	objInfo->Obj.uGenre = CGOG_NOTHING;
	objInfo->Obj.uId = -1;
	
	//法宝图标
	TLGameObject::GameObject go;
	go.d_gameobjectSet = BACKGROUND_IMAGE;
	go.d_gameobject = BACKGROUND_IMAGE;
	go.d_type = TLGameObject::item;
	go.d_count = 1;
	d_tm_iconImage->setObject(go);

	const char* holeImageset = KUiCfgLoader::getSingleton().getTalismanCfg().deactiveHoleIms;
	const char* holeImage = KUiCfgLoader::getSingleton().getTalismanCfg().deactiveHoleImage;
	//内丹图标
	for(int i = 0; i < TM_HoleRowCount; ++i)
	{
		for(int j = 0; j < TM_MaxHoleCountPerRow; ++j)
		{
			TLButton* hole = d_tm_holeImage[i][j];
			hole->hide();
			
			TLGameObject* ball = d_tm_ballImage[i][j];
			ball->hide();
		}
	}

	//层
	for(int layerIndex = 0; layerIndex < TM_HoleRowCount; ++layerIndex)
	{
		for(int layerStateIndex = 0; layerStateIndex < TM_LAYER_STATE_COUNT; ++layerStateIndex)
		{
			d_layer[layerIndex][layerStateIndex]->hide();
		}
	}
}
