
#include "UiDuraAlert.h"
#include "UiItemTip.h"
#include "..\UiSheetMgr.h"
#include "..\UiConfigManager.h"
#include "CoreShell.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

KUiDuraAlert::KUiDuraAlert()
{
	_thisWindow = NULL;
	load();
}

KUiDuraAlert::~KUiDuraAlert()
{
	
}

void KUiDuraAlert::hide()
{
	if(_thisWindow)
	{
		_thisWindow->hide();
	}
};

KUiDuraAlert& KUiDuraAlert::getSingleton()
{
	static KUiDuraAlert singleton;
	return singleton;
}

void KUiDuraAlert::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_DURA_ALERT_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_DURA_ALERT_WINDOW_PATH);
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

	_thisWindow->subscribeEvent(Window::EventNewFrame, Event::Subscriber(&KUiDuraAlert::slash, this));
	_thisWindow->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiDuraAlert::onWindowShow, this));
	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiDuraAlert::onWindowHide, this));

	for(int i = 0; i < itempart_num; ++i)
	{
		_equipRed[i] = NULL;
	}
	//显示全身装备（正常）
	_wholeBody						= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/Quanshen");
	_wholeBody->subscribeEvent(Window::EventNewFrame, Event::Subscriber(&KUiDuraAlert::onMouseHover, this));
	
	//装备红时的显示
	_equipRed[itempart_amulet]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/YupeiRed");
	_equipRed[itempart_helm]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/ToushiRed");
	_equipRed[itempart_pendant]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/PifengRed");
	_equipRed[itempart_weapon]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/WuqiRed");
	_equipRed[itempart_armor]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/YifuRed");
	_equipRed[itempart_shoulder]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/YaodaiRed");
	_equipRed[itempart_ring]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/JiezhiRed");
	_equipRed[itempart_boots]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/XieziRed");
	_equipRed[itempart_cuff]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/ShouzhuoRed");
	
	//装备黄时的显示
	_equipYellow[itempart_amulet]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/YupeiYellow");
	_equipYellow[itempart_helm]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/ToushiYellow");
	_equipYellow[itempart_pendant]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/PifengYellow");
	_equipYellow[itempart_weapon]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/WuqiYellow");
	_equipYellow[itempart_armor]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/YifuYellow");
	_equipYellow[itempart_shoulder]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/YaodaiYellow");
	_equipYellow[itempart_ring]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/JiezhiYellow");
	_equipYellow[itempart_boots]	= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/XieziYellow");
	_equipYellow[itempart_cuff]		= (TLStaticImage*)_thisWindow->getChild("TaharezLook/DuraAlert/ShouzhuoYellow");
}

void KUiDuraAlert::updateState(EQUIP_DUR_STATE equipStatus[])
{
	if(!_thisWindow)
	{
		return;
	}

	bool needShow = false;
	for (int i = 0; i < itempart_num; ++i)
	{
		_state[i] = equipStatus[i];

		if(NULL == _equipRed[i] || NULL == _equipYellow[i])
			continue;
		if(equipStatus[i] == estate_yellow)
		{
			_equipYellow[i]->show();
			_equipRed[i]->hide();
			needShow = true;
		}
		else if(equipStatus[i] == estate_red)
		{
			_equipYellow[i]->hide();
			_equipRed[i]->show();
			needShow = true;
		}
		else
		{
			_equipYellow[i]->hide();
			_equipRed[i]->hide();
		}
	}

	if(true == needShow)
	{
		_wholeBody->show();
		_thisWindow->show();
		_thisWindow->beginUpdate();
	}
	else
	{
		_thisWindow->hide();
		_thisWindow->stopUpdate();
	}
}

bool KUiDuraAlert::slash(const EventArgs& args)
{
	static int timeCount = 0;
	FrameEventArgs* frameEvent = (FrameEventArgs*)&args;
	timeCount += frameEvent->elapse;

	if(timeCount % 300 < 150)
	{
		_thisWindow->show();
	}
	else
	{
		_thisWindow->hide();
	}
	if(timeCount > 900)
	{
		timeCount = 0;
		_thisWindow->stopUpdate();
		_thisWindow->show();
	}
	return true;
}

bool KUiDuraAlert::onWindowShow(const EventArgs& args)
{
	_wholeBody->beginUpdate();
	return true;
}

bool KUiDuraAlert::onWindowHide(const EventArgs& args)
{
	_wholeBody->stopUpdate();
	return true;
}

bool KUiDuraAlert::onMouseHover(const EventArgs& args)
{
	Point mouse = MouseCursor::getSingleton().getPosition();
	_thisWindow->setMetricsMode(Absolute);
	Rect ctrlArea = _thisWindow->getUnclippedPixelRect();

	static bool tipShown = false;
	if(!ctrlArea.isPointInRect(mouse))
	{
		if(tipShown)
		{
			KUiItemTip::Hide();
			tipShown = false;
		}
		return true;
	}

	Point ctrlPos = ctrlArea.getPosition();
	int alpha = _thisWindow->getAlphaAtPixel(0, mouse.d_x - ctrlPos.d_x, mouse.d_y - ctrlPos.d_y);
	if(alpha > 0)
	{
		tipShown = true;
		showTip();
	}
	return true;
}

void KUiDuraAlert::showTip()
{
	if(!_thisWindow)
	{
		return;
	}

	KObjAtContRegion equipRegion[itempart_num];
	int nCount = g_pCoreShell->GetGameData(GDI_EQUIPMENT, (unsigned int)equipRegion, 0);

	char layoutText[LAYOUT_TEXT_MAX_LEN];

	layoutText[0] = 0;
	strcpy(layoutText, KUiCfgLoader::getSingleton().getDuraAlert().tipLayout);
	for(int i = 0; i < itempart_num; ++i)
	{
		if(equipRegion[i].Obj.uGenre != CGOG_ITEM)
		{
			continue;
		}
		int equipIndex = equipRegion[i].Obj.uId;
		char equipName[COMMON_CLIENT_MSG_LEN_64];
		int nCount = g_pCoreShell->GetGameData(GDI_GET_ITEM_NAME_BY_INDEX, (unsigned int)equipName, equipIndex);

		pair<int, int> durData;
		int ret = g_pCoreShell->GetGameData(GDI_GET_ITEM_DURA_BY_INDEX, (unsigned int)&durData, equipIndex);
		if(ret)
		{
			char durText[COMMON_CLIENT_MSG_LEN_16];
			sprintf(durText, "(%d/%d)", durData.first / 100, durData.second / 100);
			strcat(equipName, durText);
		}
		
		char segText[COMMON_CLIENT_MSG_LEN_512];
		switch(_state[i])
		{
		case estate_normal:
			{
				if(KUiCfgLoader::getSingleton().getDuraAlert().showNormal)
				{
					sprintf(segText, KUiCfgLoader::getSingleton().getDuraAlert().normalTipSeg, equipName);
				}
				else
				{
					strcat(segText, "");
				}
			}
			break;
		case estate_red:
			{
				sprintf(segText, KUiCfgLoader::getSingleton().getDuraAlert().redTipSeg, equipName);
			}
			break;
		case estate_yellow:
			{
				sprintf(segText, KUiCfgLoader::getSingleton().getDuraAlert().yellowTipSeg, equipName);
			}
			break;
		}
		strcat(layoutText, segText);
	}
	strcat(layoutText, "</Layout>");

	KUiItemTip::GetSingleton().show(layoutText, _thisWindow->getUnclippedPixelRect(), KUiItemTip::Bottom);
}