
#include "UiFSBible_SpecialQuestItem.h"
#include "UiItemTip.h"
#include "Coreshell.h"
#include "UiChatWindow.h"
#include "UiErrorMessageBox.h"
#include "../UiConfigManager.h"
#include "../KMessageCentre.h"

extern iCoreShell* g_pCoreShell;

KUiFSBibleSpecialQuestItem::KUiFSBibleSpecialQuestItem()
{
	loadUi();
}

KUiFSBibleSpecialQuestItem::~KUiFSBibleSpecialQuestItem()
{
	
}

void KUiFSBibleSpecialQuestItem::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		static int namePrefix = 0;
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_FSBIBLE_SPECIAL_QUEST_ITEM_PATH,
			iToString(namePrefix++));
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif
	
	_doubleExpTag	= (TLStaticImage*)_thisWindow->getChild(_thisWindow->getName() + "/DoubleExp");
	_questType		= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/QuestType");
	_questName		= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/QuestName");
	_count			= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Count");
	_time			= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Time");
	_place			= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Place");
	_npc			= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Npc");
	_level			= (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Level");
	
	_doubleExpTag->enable();
	_questType->disable();
	_questName->disable();
	_count->disable();
	_time->disable();
	_place->disable();
	_npc->enable();
	_level->disable();

	_npc->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiFSBibleSpecialQuestItem::onNpcClick, this));
	_thisWindow->subscribeEvent(TLStaticImage::EventMouseEnters, Event::Subscriber(&KUiFSBibleSpecialQuestItem::onMouseEnters, this));
	_thisWindow->subscribeEvent(TLStaticImage::EventMouseLeaves, Event::Subscriber(&KUiFSBibleSpecialQuestItem::onMouseLeaves, this));
	_thisWindow->subscribeEvent(TLStaticImage::EventMouseMove, Event::Subscriber(&KUiFSBibleSpecialQuestItem::onMouseMove, this));
}

void KUiFSBibleSpecialQuestItem::setContent(SpecialQuestData& data)
{
	if(!_thisWindow)
	{
		return;
	}

	const map<int, string>& imageCfg = KUiCfgLoader::getSingleton().getCommImage();
	map<int, string>::const_iterator curIt = imageCfg.find(data.changedData.doubleExpTag);
	if(curIt != imageCfg.end())
	{
		const Image* image = getImage(curIt->second.c_str());
		_doubleExpTag->setImage(image);
	}

	_questType->setText(AnsiToUtf8(data.questType));
	_questName->setText(AnsiToUtf8(data.questName));
	char countText[COMMON_CLIENT_MSG_LEN_128];
	sprintf(countText, data.count, data.changedData.count);
	_count->setText(AnsiToUtf8(countText));
	_time->setText(AnsiToUtf8(data.time));
	_place->setText(AnsiToUtf8(data.place));
	_npc->setText(AnsiToUtf8(data.npc));
	_level->setText(AnsiToUtf8(data.level));

	_questType->setTextColours(data.showColor);
	_questName->setTextColours(data.showColor);
	_count->setTextColours(data.showColor);
	_time->setTextColours(data.showColor);
	_place->setTextColours(data.showColor);
	_npc->setTextColours(data.showColor);
	_level->setTextColours(data.showColor);

	_tipText = data.tip;

	_npc->setUserData(&data);
}

void KUiFSBibleSpecialQuestItem::clear()
{
	if(!_thisWindow)
	{
		return;
	}

	_doubleExpTag->setImage(NULL);
	_questType->setText("");
	_questName->setText("");
	_count->setText("");
	_time->setText("");
	_place->setText("");
	_npc->setText("");
	_level->setText("");
	_tipText = "";
}

bool KUiFSBibleSpecialQuestItem::onMouseEnters(const EventArgs& e)
{
	showTip();
	return true;
}

bool KUiFSBibleSpecialQuestItem::onMouseLeaves(const EventArgs& e)
{	
	KUiItemTip::Hide();
	return true;
}

bool KUiFSBibleSpecialQuestItem::onMouseMove(const EventArgs& e)
{	
	if(KUiItemTip::IsVisible() == false)
		showTip();
	return true;
}

bool KUiFSBibleSpecialQuestItem::onNpcClick(const EventArgs& e)
{	
	WindowEventArgs* arg = (WindowEventArgs*)&e;
	SpecialQuestData* data = (SpecialQuestData*)arg->window->getUserData();
	if(!data)
	{
		return true;
	}

	KUiSceneTimeInfo mapInfo = { 0 };
	
	g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
	mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
	
	if(data->npcPos.mapId == mapInfo.nSceneId)
	{
		g_pCoreShell->OperationRequest(GOI_SET_AUTO_DIALOG_NPC, data->npcId, NULL);
		g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)data->npcPos.x, (int)data->npcPos.y * 2);
	}
	else if(data->npcId != -1)
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
			sprintf(mapId, "%d", data->npcPos.mapId);
			mapFile.GetString( "List", mapId, "", mapName, sizeof(mapName));
		}

		sprintf(errorMsg, szMsg, mapName, data->npcPos.x, data->npcPos.y);
		KUiChannelCentre::GetSingleton().toSysMsg(errorMsg);
		
		static DWORD lastErrorMsgTime = 0;
		if(GetTickCount() - lastErrorMsgTime > 2000)
		{
			KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(errorMsg));
			lastErrorMsgTime = GetTickCount();
		}
	}
	return true;
}

void KUiFSBibleSpecialQuestItem::showTip()
{
	if(!_thisWindow)
	{
		return;
	}
	
	if(_tipText == "")
	{
		return;
	}

	KUiItemTip::GetSingleton();
	char* tip = const_cast<char*>(_tipText.c_str());
	KUiItemTip::GetSingleton().show(tip, _thisWindow->getUnclippedPixelRect(), KUiItemTip::BottomRight);
}