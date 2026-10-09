#include "ui/UiCase/UiPathHelp.h"
#include "ui/UiConfigManager.h"
#include "CoreShell.h"
#include "ui/UiCase/UiErrorMessageBox.h"
#include "chatWindow/ChatMainDlg.h"
#include "ui/UiCase/UiChatWindow.h"


extern iCoreShell*		g_pCoreShell;

using namespace std;
using namespace ClientMapInfo;

int ClientMapInfo::AutoMapInfo::isHaveTypeNpc(MapNpcType type)
{
	if(npcList.empty())
		return -1;
	for(int i = 0; i < npcList.size();i++)
	{
		MapNpcInfo& npc = npcList[i];
		if(npc.type  == type )
			return i;
	}
	return -1;
}
ClientMapInfo::AutoMapInfo::AutoMapInfo()
{
	mapId = 0;//地图ID
	carryNumbers = 0;//传送点数目
	memset(mapName,0,AUTO_ITEM_NAME_LENGTH);
	levelLimited  = 0;//
}
ClientMapInfo::AutoMapInfo::~AutoMapInfo()
{

}
ClientMapInfo::CarryInfo::CarryInfo()
{
	carryID = 0;
	memset(carryName,0,AUTO_ITEM_NAME_LENGTH);
	levelLimited = 0;
	targetCarry = 0;
	currentMapIndex = 0;
	carryType = 0;
	carryPosX = 0;
	carryPosY = 0;
}
ClientMapInfo::CarryInfo::~CarryInfo()
{

}
///////////////////////////

ClientMapInfo::AutoGoBack::AutoGoBack()
{
	currentMapID= 0;
	KIniFile iniFile;
	iniFile.Load(AUTO_INI_FILE_PATH);
	int numberMaps = 0;
	int numberCarries = 0;
	iniFile.GetInteger("autoGoBack","mapNumbers",0,&numberMaps);
	char itemName[AUTO_ITEM_NAME_LENGTH] = {0};
	char itemNameValue[AUTO_ITEM_NAME_LENGTH] = {0};
	if(numberMaps>0)
	{
		mapInfoList.reserve(numberMaps);
		for(int mapIndex = 0; mapIndex < numberMaps; mapIndex++)
		{
			AutoMapInfo* pMapInfo = new AutoMapInfo;
			const char name[] = "mapItemName";
			memset(itemName,0,AUTO_ITEM_NAME_LENGTH);
			sprintf(itemName,"%s%d",name,mapIndex);
			iniFile.GetString("autoGoBack",itemName,"",itemNameValue,AUTO_ITEM_NAME_LENGTH);
			iniFile.GetInteger(itemNameValue,"id",0,&pMapInfo->mapId);
			iniFile.GetString(itemNameValue,"name","",pMapInfo->mapName,AUTO_ITEM_NAME_LENGTH);
			iniFile.GetInteger(itemNameValue,"numberCarried",0,&pMapInfo->carryNumbers);
			if(pMapInfo->carryNumbers>0)
			{
				for(int carryIndex = 0; carryIndex < pMapInfo->carryNumbers; carryIndex++)
				{
					const char carryName[] = "carryIndex";
					char   carryNameValue[AUTO_ITEM_NAME_LENGTH] = {0};
					sprintf(carryNameValue,"%s%d",carryName,carryIndex);
					int carryIndexValue  = 0;
					iniFile.GetInteger(itemNameValue,carryNameValue,0,&carryIndexValue);
					pMapInfo->carryIndexList.push_back(carryIndexValue);

				}
			}
			iniFile.GetInteger(itemNameValue,"levelLimited",0,&pMapInfo->levelLimited);
			int numNpc = 0;
			iniFile.GetInteger(itemNameValue,"numberNpc",0,&numNpc);
			if(numNpc>0)
			{
				for(int j = 0; j < numNpc;j++)
				{
					MapNpcInfo npc;
					char buffer[256] = {0};
					char valueBuffer[COMMON_CLIENT_MSG_LEN_512] = {0};
					sprintf(buffer,"npcIdx%d",j);
					iniFile.GetString(itemNameValue,buffer,"",valueBuffer,COMMON_CLIENT_MSG_LEN_512);
					int npcTyp = 0;
					sscanf(valueBuffer,"%d,%d,%d,%d,%s",&npcTyp,&npc.npcMapId,
						  &npc.npcPosX,&npc.npcPosY,npc.npcName);
					npc.type = (MapNpcType)npcTyp;
					pMapInfo->npcList.push_back(npc);
				}
			}
//			iniFile.GetInteger(itemNameValue,"canSell",0,&pMapInfo->canSellItem);
//			iniFile.GetInteger(itemNameValue,"sellNpcX",0,&pMapInfo->SellNpcPosX);
//			iniFile.GetInteger(itemNameValue,"sellNpcY",0,&pMapInfo->SellNpcPosY);
//			if(!pMapInfo->canSellItem)
//			{
/*				int goBackNumberCarries = 0;
				iniFile.GetInteger(itemNameValue,"carrynumbers",0,&goBackNumberCarries);
				for(int index = 0; index < goBackNumberCarries; index ++)
				{
					const char name1[] = "carry";
					char nameValue[AUTO_ITEM_NAME_LENGTH] = {0};
					sprintf(nameValue,"%s%d",name1,index);
					int carry = 0;
					iniFile.GetInteger(itemNameValue,nameValue,0,&carry);
					pMapInfo->backCarryList.push_back(carry);
				}
//			}*/
			mapInfoList.push_back(pMapInfo);
		}
	}

	iniFile.GetInteger("autoGoBack","carriedNumbers",0,&numberCarries);
	if(numberCarries>0)
	{
		carryInfoList.reserve(numberCarries);
		for(int carryIndex = 0; carryIndex < numberCarries; carryIndex ++)
		{
			CarryInfo* pCarryInfo = new CarryInfo;
			const  char name[] = "carryItemName";
			memset(itemName,0,AUTO_ITEM_NAME_LENGTH);
			sprintf(itemName,"%s%d",name,carryIndex);
			iniFile.GetString("autoGoBack",itemName,"",itemNameValue,AUTO_ITEM_NAME_LENGTH);
			iniFile.GetInteger(itemNameValue,"id",0,&pCarryInfo->carryID);
			iniFile.GetString(itemNameValue,"name","",pCarryInfo->carryName,AUTO_ITEM_NAME_LENGTH);
			iniFile.GetInteger(itemNameValue,"levelLimited",0,&pCarryInfo->levelLimited);
			iniFile.GetInteger(itemNameValue,"carryTarget",0,&pCarryInfo->targetCarry);
			iniFile.GetInteger(itemNameValue,"currentMapIndex",0,&pCarryInfo->currentMapIndex);
			iniFile.GetInteger(itemNameValue,"carryType",0,&pCarryInfo->carryType);
			iniFile.GetInteger(itemNameValue,"carryPosX",0,&pCarryInfo->carryPosX);
			iniFile.GetInteger(itemNameValue,"carryPosY",0,&pCarryInfo->carryPosY);
			carryInfoList.push_back(pCarryInfo);
		}
	}

}
ClientMapInfo::AutoGoBack::~AutoGoBack()
{
	while(!mapInfoList.empty())
	{
		AutoMapInfo* pInfo = mapInfoList.back();
		mapInfoList.pop_back();
		delete pInfo;
	}
	while(!carryInfoList.empty())
	{
		CarryInfo* pInfo = carryInfoList.back();
		carryInfoList.pop_back();
		delete pInfo;
	}
}
ClientMapInfo::AutoMapInfo* AutoGoBack::GetCurrentMapInfo()
{
	int numberMaps = mapInfoList.size();
	for(int i = 0; i < numberMaps;i++)
	{
		AutoMapInfo* pInfo = mapInfoList[i];
		if(pInfo->mapId == currentMapID)
			return pInfo;
	}
	return 0;
}
ClientMapInfo::AutoMapInfo* AutoGoBack::GetMapInfo(int id)
{
	int numberMaps = mapInfoList.size();
	for(int i = 0; i < numberMaps;i++)
	{
		AutoMapInfo* pInfo = mapInfoList[i];
		if(pInfo->mapId == id)
			return pInfo;
	}
	return 0;
}
const CarryInfo* ClientMapInfo::AutoGoBack::GetCarryInfo(int id)
{
	int numberCarries = carryInfoList.size();
	for(int i = 0; i < numberCarries; i++)
	{
		CarryInfo* pInfo = carryInfoList[i];
		if(pInfo->carryID == id)
			return pInfo;
	}
	return 0;
}
void AutoGoBack::SetCurrentMapInfo(int id)
{
	int numbermaps = mapInfoList.size();
	if(id<0|| id >=numbermaps)
		return;
	currentMapID = id;
}
void ClientMapInfo::AutoGoBack::SetCurrentMapInfo(const char* name)
{
	if(name == 0)
		return;
	int numberMaps = mapInfoList.size();
	for(int i = 0; i < numberMaps;i++)
	{
		AutoMapInfo* pInfo = mapInfoList[i];
		if(strcmp(pInfo->mapName,name) == 0)
		{
			currentMapID = pInfo->mapId;
			return;
		}
	}
	return ;

}
bool ClientMapInfo::AutoGoBack::IsInCloseList(int id)
{
	if(findCloseList.empty())
		return false;
	list<pathInfo>::iterator iter = findCloseList.begin();
	for(;iter!=findCloseList.end();iter++)
	{
		pathInfo& path = *iter;
		if(path.mapID == id)
			return true;
	}
	return false;
}
bool ClientMapInfo::AutoGoBack::IsInPenList(int id)
{
	if(findOpenList.empty())
		return false;
	list<pathInfo>::iterator iter = findOpenList.begin();
	for(;iter!=findOpenList.end();iter++)
	{
		pathInfo& path = *iter;
		if(path.mapID == id)
			return true;
	}
	return false;
}
bool ClientMapInfo::AutoGoBack::FindPathInCloseList(int id,pathInfo& info)
{
	if(findCloseList.empty())
		return false;
	list<pathInfo>::iterator iter = findCloseList.begin();
	for(;iter!=findCloseList.end();iter++)
	{
		pathInfo& path = *iter;
		if(path.mapID == id)
		{
			info = path;
			return true;
		}
	}
	return false;
}
void ClientMapInfo::AutoGoBack::CreateCarryPath()
{
	pathList.clear();
	mapPathList.clear();
	pathInfo& path = findCloseList.front();
	while(true)
	{
		mapPathList.push_front(path.mapID);
		if(!FindPathInCloseList(path.parentMapID,path))
			break;
		int currentPath = mapPathList.front();
		const AutoMapInfo* pInfo1 = GetMapInfo(currentPath);
		const AutoMapInfo* pInfo = GetMapInfo(path.mapID);
		if(pInfo1==0||pInfo == 0)
			return;
		for(int j = 0; j < pInfo->carryNumbers;j++)
		{
			const CarryInfo* pCarry = GetCarryInfo(pInfo->carryIndexList[j]);
			if(pCarry==0)
				return ;
			const CarryInfo* pTarget = GetCarryInfo(pCarry->targetCarry);
			if(pTarget == 0)
				return;
			if(pTarget->currentMapIndex == pInfo1->mapId)
				pathList.push_front(pCarry->carryID);
		}

	}
}
bool ClientMapInfo::AutoGoBack::GotoSellItem()
{
	findOpenList.clear();
	findCloseList.clear();
	pathInfo path;
	const AutoMapInfo* pCurrentMapInfo = GetCurrentMapInfo();
	path.mapID = pCurrentMapInfo->mapId;
	path.parentMapID = -1;
	findOpenList.push_front(path);
	while(!findOpenList.empty())
	{
		pathInfo firstPath = findOpenList.front();
		findOpenList.pop_front();
		findCloseList.push_front(firstPath);
		AutoMapInfo* pMap = GetMapInfo(firstPath.mapID);
		if(pMap == 0)
			return false;
		if(pMap->isHaveTypeNpc(npcTyp_Sell_Medicne))
		{
			CreateCarryPath();
			return true;
		}
		const AutoMapInfo*  pMapInfo = GetMapInfo(firstPath.mapID);
		if(pMapInfo == 0)
			return false;
		for(int i = 0; i < pMapInfo->carryNumbers; i++)
		{
			const CarryInfo* pCurrentCarry = GetCarryInfo(pMapInfo->carryIndexList[i]);
			if(pCurrentCarry == 0)
				return false;
			const CarryInfo* pTargetCarry = GetCarryInfo(pCurrentCarry->targetCarry);
			if(pTargetCarry == 0)
				return false;
			if(IsInCloseList(pTargetCarry->currentMapIndex))
				continue;
			if(IsInPenList(pTargetCarry->currentMapIndex))
				continue;
			pathInfo path; 
			path.mapID = pTargetCarry->currentMapIndex;
			path.parentMapID = firstPath.mapID;
			findOpenList.push_front(path);
		}
		
	}
	return false;
}
int ClientMapInfo::AutoGoBack::GetMapIndex(int mapId)
{
	for(int i = 0; i < mapInfoList.size();i++)
	{
		AutoMapInfo* pInfo = mapInfoList[i];
		if(mapId == pInfo->mapId)
			return i;
	}
	return -1;
}
bool ClientMapInfo::AutoGoBack::FindPath(int srcMapID,int targetMapId)
{
	findOpenList.clear();
	findCloseList.clear();
	if(srcMapID == targetMapId)
	{
		pathList.clear();
	    mapPathList.clear();
		return true;
	}
	if(srcMapID<0||srcMapID>=mapInfoList.size()||
		targetMapId<0||targetMapId>=mapInfoList.size())
	{
		pathList.clear();
	    mapPathList.clear();
		return false;
	}
	pathInfo path;
	const AutoMapInfo* pCurrentMapInfo = mapInfoList[srcMapID];
	path.mapID = pCurrentMapInfo->mapId;
	path.parentMapID = -1;
	findOpenList.push_front(path);
	while(!findOpenList.empty())
	{
		pathInfo firstPath = findOpenList.front();
		findOpenList.pop_front();
		findCloseList.push_front(firstPath);
		if(GetMapIndex(firstPath.mapID) == targetMapId)
		{
			CreateCarryPath();
			return true;
		}
		const AutoMapInfo*  pMapInfo = GetMapInfo(firstPath.mapID);
		if(pMapInfo == 0)
			return false;
		for(int i = 0; i < pMapInfo->carryNumbers; i++)
		{
			const CarryInfo* pCurrentCarry = GetCarryInfo(pMapInfo->carryIndexList[i]);
			if(pCurrentCarry == 0)
				return false;
			const CarryInfo* pTargetCarry = GetCarryInfo(pCurrentCarry->targetCarry);
			if(pTargetCarry == 0)
				return false;
			if(IsInCloseList(pTargetCarry->currentMapIndex))
				continue;
			if(IsInPenList(pTargetCarry->currentMapIndex))
				continue;
			pathInfo path; 
			path.mapID = pTargetCarry->currentMapIndex;
			path.parentMapID = firstPath.mapID;
			findOpenList.push_back(path);
		}
		
	}
	findCloseList.clear();
	return false;
}
bool ClientMapInfo::AutoGoBack::GotoTargetMap(int mapId)
{
	findOpenList.clear();
	findCloseList.clear();
	pathInfo path;
	const AutoMapInfo* pCurrentMapInfo = GetCurrentMapInfo();
	path.mapID = pCurrentMapInfo->mapId;
	path.parentMapID = -1;
	findOpenList.push_front(path);
	while(!findOpenList.empty())
	{
		pathInfo firstPath = findOpenList.front();
		findOpenList.pop_front();
		findCloseList.push_front(path);
		if(firstPath.mapID == mapId)
		{
			CreateCarryPath();
			return true;
		}
		const AutoMapInfo*  pMapInfo = GetMapInfo(firstPath.mapID);
		if(pMapInfo == 0)
			return false;
		for(int i = 0; i < pMapInfo->carryNumbers; i++)
		{
			const CarryInfo* pCurrentCarry = GetCarryInfo(pMapInfo->carryIndexList[i]);
			if(pCurrentCarry == 0)
				return false;
			const CarryInfo* pTargetCarry = GetCarryInfo(pCurrentCarry->targetCarry);
			if(pTargetCarry == 0)
				return false;
			if(IsInCloseList(pTargetCarry->currentMapIndex))
				continue;
			if(IsInPenList(pTargetCarry->currentMapIndex))
				continue;
			pathInfo path; 
			path.mapID = pTargetCarry->currentMapIndex;
			path.parentMapID = firstPath.mapID;
			findOpenList.push_front(path);
		}
		
	}
	return false;
}
AutoGoBack& ClientMapInfo::AutoGoBack::Singleton()
{
	static AutoGoBack autoBack;
	return autoBack;
}


template<> 
KUiPathHelp* KUiWndSingleton<KUiPathHelp>::ms_Singleton	= NULL;

KUiPathHelp::KUiPathHelp(const CEGUI::String& string_id_name):
KUiWndSingleton<KUiPathHelp>(string_id_name)
{
	
}
KUiPathHelp::~KUiPathHelp()
{
	
}
void KUiPathHelp::Init(void)
{
	if(ms_Singleton->m_pThisWnd)
	{
		///////////////////////
		KIniFile iniFile;
		iniFile.Load(UI_PATH_HELP_INI);
		iniFile.GetString("string","pathResult0","",pathResultGotEnd,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","pathResult1","",pathResultNoStart,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","pathResult2","",pathResultNoEnd,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","pathResult3","",pathResultNotFind,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","pathResult4","",pathResultTheSame,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","pathResultDft","",pathResultNone,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","showTextFormat","",showTextFormat,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","showText","",showText,COMMON_CLIENT_MSG_LEN_256);
		iniFile.GetString("string","showText1","",showText1,COMMON_CLIENT_MSG_LEN_256);
		////////////////////////
		pStartPlacListBox = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/pathHelp/StartListBox");
		pEndPlaceListBox = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/pathHelp/EndListBox");
		pResultBox        = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/pathHelp/result");
		pStartScrollbar   = (TLVertScrollbar*)pStartPlacListBox->getChild("TaharezLook/pathHelp/StartListBox/scrollbar");
		pStartScrollbar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged,Event::Subscriber(&KUiPathHelp::HandleStartScrollBarPos, ms_Singleton));
		pEndScrollbar     = (TLVertScrollbar*)pEndPlaceListBox->getChild("TaharezLook/pathHelp/End/scrollbar");
		pEndScrollbar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged,Event::Subscriber(&KUiPathHelp::HandleEndScrollBarPos, ms_Singleton));
		pResultScrollBar = (TLVertScrollbar*)pResultBox->getChild("TaharezLook/pathHelp/result/scrollbar");
		pResultScrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged,Event::Subscriber(&KUiPathHelp::handleResultScrollBarPos, ms_Singleton));
		pResultScrollBar->setStepSize(0.1f);
		pResultPathText   = (TLStaticText*)pResultBox->getChild("TaharezLook/pathHelp/result/resultText");
		pResultPathText->useLayout();
		ILayout* pLayout = pResultPathText->getLayout();
		LORect rc;
		Rect rcClip = pResultPathText->getUnclippedPixelRect();
		Rect rcClip1 = m_pThisWnd->getUnclippedPixelRect();
		int x = rcClip.d_left - rcClip1.d_left;
		int y = rcClip.d_top  - rcClip1.d_top;
		int width = pResultPathText->getWidth(Absolute);
		int height = pResultPathText->getHeight(Absolute);
		rc.setPos(x,y);
		rc.setWidth(width);
		rc.setHeight(height);
		char buffer[COMMON_CLIENT_MSG_LEN_256] = {0};
		sprintf(buffer,"<Layout width=200><Seg text-align=left ><Obj type=text>%s</Obj></Seg></Layout>",pathResultNone);
		pLayout->setClipper(rc);
		pLayout->SetText(buffer);
		pOkButton = (PushButton*)m_pThisWnd->getChild("TaharezLook/pathHelp/OkBtn");
		pOkButton->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&KUiPathHelp::HandleOkButton, ms_Singleton));
		pResultPathText->subscribeEvent(StaticText::EventMouseButtonDown,Event::Subscriber(&KUiPathHelp::HandleClickResultText, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/pathHelp/Close")->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&KUiPathHelp::HandleCloseButton, ms_Singleton));
		AddChildToListBox();
	}

}
bool KUiPathHelp::HandleCloseButton(const CEGUI::EventArgs& args)
{
	KUiPathHelp::Hide();
	return true;
}
void KUiPathHelp::SetItemFunEnd(Checkbox* pWindow,unsigned int id)
{
	pWindow->setID(id);
	pWindow->subscribeEvent(Checkbox::EventCheckStateChanged,Event::Subscriber(&KUiPathHelp::HandleSelectedEndList, ms_Singleton));
}
void KUiPathHelp::SetItemFunStart(Checkbox* pWindow,unsigned int id)
{
	pWindow->setID(id);
	pWindow->subscribeEvent(Checkbox::EventCheckStateChanged,Event::Subscriber(&KUiPathHelp::HandleSelectedStartList, ms_Singleton));
}
void KUiPathHelp::AddChildToListBox()
{
	KIniFile iniFile;
	iniFile.Load(UI_PATH_HELP_INI);
	//id的顺序与AutoGoBack中的一致//
	AutoGoBack& autoMap = AutoGoBack::Singleton();
	int numberMaps = autoMap.mapInfoList.size();
	int width = 0,height = 0,px= 0,py = 0;
	char normalImage[COMMON_CLIENT_MSG_LEN_512] = {0};
	char hoverImage[COMMON_CLIENT_MSG_LEN_512] = {0};
	char markImage[COMMON_CLIENT_MSG_LEN_512] = {0};
	iniFile.GetInteger("string","checkItemWidth",0,&width);
	iniFile.GetInteger("string","checkItemHeight",0,&height);
	iniFile.GetInteger("string","startPosX",0,&px);
	iniFile.GetInteger("string","startPosY",0,&py);
	itemWidth = width;
	itemHeight = height;
	startPx = px;
	startPy = py;
	iniFile.GetString("string","normalImage","",normalImage,COMMON_CLIENT_MSG_LEN_512);
	iniFile.GetString("string","hoverImage","",hoverImage,COMMON_CLIENT_MSG_LEN_512);
	iniFile.GetString("string","markImage","",markImage,COMMON_CLIENT_MSG_LEN_512);
	startListLength = 0;
	numberCheckedBoxes = numberMaps;
	for(int i = 0 ; i < numberMaps;i++)
	{
		char name[COMMON_CLIENT_MSG_LEN_512] = {0};
		sprintf(name,"%s%d",UI_START_PLACE_LIST_BOX_ITEM_NAME,i);
		TLCheckbox* pCheckBox = (TLCheckbox*)ms_Singleton->m_pWindowManager->createWindow("TaharezLook/Checkbox",name);
		if(pCheckBox)
		{
			pStartPlacListBox->addChildWindow(pCheckBox);
			pCheckBox->setWidth(Absolute,width);
			pCheckBox->setHeight(Absolute,height);
			Point absPoint;
			absPoint.d_x = px;
			absPoint.d_y = py+i*(height+2);
			pCheckBox->setPosition(Absolute,absPoint);
			const AutoMapInfo* pMapInfo = autoMap.mapInfoList[i];
			pCheckBox->setText(AnsiToUtf8(pMapInfo->mapName));
			pCheckBox->setCheckboxNormal(CEGUI::PropertyHelper::stringToImage(normalImage));
			pCheckBox->setCheckboxHover(CEGUI::PropertyHelper::stringToImage(hoverImage));
			pCheckBox->setCheckboxMark(CEGUI::PropertyHelper::stringToImage(markImage));
			pCheckBox->setVisible(true);
			startPlaceItemList.push_back(pCheckBox);
			SetItemFunStart(pCheckBox,FIND_PATH_UI_START + (unsigned int)i);
			
		}
	}
	startListLength = height*numberMaps;
	endListLength = 0;
	for(i = 0 ; i < numberMaps;i++)
	{
		char name[COMMON_CLIENT_MSG_LEN_512] = {0};
		sprintf(name,"%s%d",UI_END_PLACE_LIST_BOX_ITEM_NAME,i);
		TLCheckbox* pCheckBox = (TLCheckbox*)ms_Singleton->m_pWindowManager->createWindow("TaharezLook/Checkbox",name);
		if(pCheckBox)
		{
			pEndPlaceListBox->addChildWindow(pCheckBox);
			pCheckBox->setWidth(Absolute,width);
			pCheckBox->setHeight(Absolute,height);
			Point absPoint;
			absPoint.d_x = px;
			absPoint.d_y = py+i*(height+2);
			pCheckBox->setPosition(Absolute,absPoint);
			const AutoMapInfo* pMapInfo = autoMap.mapInfoList[i];
			pCheckBox->setText(AnsiToUtf8(pMapInfo->mapName));
			pCheckBox->setCheckboxNormal(CEGUI::PropertyHelper::stringToImage(normalImage));
			pCheckBox->setCheckboxHover(CEGUI::PropertyHelper::stringToImage(hoverImage));
			pCheckBox->setCheckboxMark(CEGUI::PropertyHelper::stringToImage(markImage));
			pCheckBox->setVisible(true);
			endPlaceItemList.push_back(pCheckBox);
			SetItemFunEnd(pCheckBox,(unsigned int)i+FIND_PATH_UI_END);
//			pCheckBox->subscribeEvent(Checkbox::EventClicked,Event::Subscriber(&KUiPathHelp::HandleSelectedEndListBox, ms_Singleton));
		}
	}
	endListLength=height*numberMaps;
}
void KUiPathHelp::SetCheckedOnlyOneStart(unsigned int  currentID)
{
	int id = currentID - FIND_PATH_UI_START;
	TLCheckbox* pCheckBox = (TLCheckbox*)startPlaceItemList[id];
	if(pCheckBox->isSelected())
	{
		for(int i = 0; i < numberCheckedBoxes; i ++)
		{
			if(i!=id)
			{
				TLCheckbox* pTemp = (TLCheckbox*)startPlaceItemList[i];
				if(pTemp->isSelected())
					pTemp->setSelected(false);
			}
		}
	}
}
void KUiPathHelp::SetCheckedOnlyOneEnd(unsigned int currentID)
{
	int id = currentID - FIND_PATH_UI_END;
	TLCheckbox* pCheckBox = (TLCheckbox*)endPlaceItemList[id];
	if(pCheckBox->isSelected())
	{
		for(int i = 0; i < numberCheckedBoxes; i ++)
		{
			if(i!=id)
			{
				TLCheckbox* pTemp = (TLCheckbox*)endPlaceItemList[i];
				if(pTemp->isSelected())
					pTemp->setSelected(false);
			}
		}
	}
}
bool KUiPathHelp::HandleSelectedStartList(const CEGUI::EventArgs& args)
{
	WindowEventArgs& checkedArgs = (WindowEventArgs&)args;
	Window* pWindow = checkedArgs.window;
	SetCheckedOnlyOneStart(pWindow->getID());
	return  true;

}
bool KUiPathHelp::HandleSelectedEndList(const CEGUI::EventArgs& args)
{
	WindowEventArgs& checkedArgs = (WindowEventArgs&)args;
	Window* pWindow = checkedArgs.window;
	SetCheckedOnlyOneEnd(pWindow->getID());
	return  true;
}
/*
bool KUiPathHelp::HandleSelectedStartListChaoge(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneStart(UI_LIST_BOX_ITEM_CHAOGE);
	return true;
}
bool KUiPathHelp::HandleSelectedStartListBeijiang(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneStart(UI_LIST_BOX_ITEM_BEIJIANG);
	return true;
}
bool KUiPathHelp::HandleSelectedStartListBeihai(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneStart(UI_LIST_BOX_ITEM_BEIHAI);
	return true;
}
bool KUiPathHelp::HandleSelectedStartListNanman(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneStart(UI_LIST_BOX_ITEM_NANMAN);
	return true;
}
bool KUiPathHelp::HandleSelectedStartListXizhou(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneStart(UI_LIST_BOX_ITEM_XIZHOU);
	return true;

}
bool KUiPathHelp::HandleSelectedStartListDonglu(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneStart(UI_LIST_BOX_ITEM_DONGLU);
	return true;
}

bool KUiPathHelp::HandleSelectedEndListBeihai(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneEnd(UI_LIST_BOX_ITEM_BEIHAI);
	return true;
}
bool KUiPathHelp::HandleSelectedEndListBeijiang(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneEnd(UI_LIST_BOX_ITEM_BEIJIANG);
	return true;
}
bool KUiPathHelp::HandleSelectedEndListChaoge(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneEnd(UI_LIST_BOX_ITEM_CHAOGE);
	return true;
}
bool KUiPathHelp::HandleSelectedEndListDonglu(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneEnd(UI_LIST_BOX_ITEM_DONGLU);
	return true;
}
bool KUiPathHelp::HandleSelectedEndListNanman(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneEnd(UI_LIST_BOX_ITEM_NANMAN);
	return true;
}
bool KUiPathHelp::HandleSelectedEndListXizhou(const CEGUI::EventArgs& args)
{
	SetCheckedOnlyOneEnd(UI_LIST_BOX_ITEM_XIZHOU);
	return true;
}
*/
bool KUiPathHelp::HandleOkButton(const CEGUI::EventArgs& args)
{
	FindPath();
	return true;
}
void KUiPathHelp::FindPath()
{
	int i = 0;
	int srcMapIdx = -1;
	int targetMapIdx = -1;
	for(i = 0; i < startPlaceItemList.size();i++)
	{
		TLCheckbox* pCheckBox = (TLCheckbox*)startPlaceItemList[i];
		if(pCheckBox->isSelected())
		{
			srcMapIdx = i;
			break;
		}
	}
	if(srcMapIdx == -1)
	{
		SetResultText(ResultError_not_select_start_place);
		return;
	}
	for(i = 0; i < startPlaceItemList.size();i++)
	{
		TLCheckbox* pCheckBox = (TLCheckbox*)endPlaceItemList[i];
		if(pCheckBox->isSelected())
		{
			targetMapIdx = i;
			break;
		}
	}
	if(targetMapIdx == -1)
	{
		SetResultText(ResultError_not_select_end_place);
		return;
	}
	if(targetMapIdx == srcMapIdx)
	{
		SetResultText(ResultError_START_END_THE_SAME);
		return;
	}
	if(ClientMapInfo::AutoGoBack::Singleton().FindPath(srcMapIdx,targetMapIdx))
	{
		SetResultText(ResultError_Success);
	}
	else
	{
		SetResultText(ResultError_not_find);
	}
}
void KUiPathHelp::SetResultText(FindResultError errorMode)
{
	char textForMainChat[COMMON_CLIENT_MSG_LEN_512*10+1] = {0};
	DWORD color = ChatString::ChatStringGetString().pathHelpTextColor;
	float red = ((float)(color>>16))/255.0f;
	float green = ((float)((color>>8)&0xff))/255.0f;
	float blue = ((float)(color&0xff))/255.0f;
	DWORD posColor = ChatString::ChatStringGetString().carriedTextColor;
	float carried_r = ((float)(posColor>>16))/255.0f;
	float carried_g = ((float)((posColor>>8)&0xff))/255.0f;
	float carried_b = ((float)(posColor&0xff))/255.0f;
	sprintf(textForMainChat,"<Seg text-align=left color=%d,%d,%d><Obj c=%x>%s</Obj>",color>>16,(color>>8)&0xff,color&0xff,color,ChatString::ChatStringGetString().pathHelpText);
	if(errorMode == ResultError_Success)
	{
		ILayout* pLayout = pResultPathText->getLayout();
		char buffer[COMMON_CLIENT_MSG_LEN_512] = {0};
		int width  = (int)pResultPathText->getWidth(Absolute);
		sprintf(buffer,"<Layout width = %d><Seg text-align=left color=%d,%d,%d></Seg></Layout>",width,color>>16,(color>>8)&0xff,color&0xff);
		pLayout->clearLayout();
		pLayout->SetText(buffer);
		list<int>& pathList =AutoGoBack::Singleton().pathList;
		int numberCarry  = pathList.size();
		if(numberCarry == 0)
		{
			wchar_t* pTemp = 0;
			sprintf(buffer,"%s",pathResultGotEnd);
			ansiToUnicode(buffer,pTemp);
			LOElemInfo elemInfo;
			elemInfo.elemType = LO_TEXT;
			elemInfo.color.red = red;
			elemInfo.color.green = green;
			elemInfo.color.blue = blue;
			elemInfo.content.set(pTemp);
			pLayout->insertElem(elemInfo);
			ChatMainDlg::InsertSystemMsg(pLayout);
			char bufferForMain[1024] = {0};
			sprintf(bufferForMain,"<Obj c=%x>%s</Obj></Seg>",color,buffer);
			strcat(textForMainChat,bufferForMain);
			KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,textForMainChat);
			return;
		}
		char  textTempForMain[1024]={0};
		list<int>::iterator iter = pathList.begin();
		for(; iter!=pathList.end();iter++)
		{
			
			wchar_t* pTemp = 0;
			int carryId = *iter;
			const CarryInfo* pCarryInfo = AutoGoBack::Singleton().GetCarryInfo(carryId);
			const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetMapInfo(pCarryInfo->currentMapIndex);
			sprintf(buffer,"%s%s",pMapInfo->mapName,showText);
			ansiToUnicode(buffer,pTemp);
			LOElemInfo elemInfo;
			elemInfo.color.red = red;
			elemInfo.color.green = green;
			elemInfo.color.blue = blue;
			elemInfo.elemType = LO_TEXT;
			elemInfo.content = pTemp;
			pLayout->insertElem(elemInfo);
			sprintf(textTempForMain,"<Obj color=%d,%d,%d>%s</Obj>",color>>16,(color>>8)&0xff,color&0xff,buffer);
			strcat(textForMainChat,textTempForMain);
			delete [] pTemp;
			pTemp = 0;
			sprintf(textTempForMain,"%s%s",pMapInfo->mapName,showText1);
			sprintf(buffer,showTextFormat,textTempForMain,pCarryInfo->carryPosX,pCarryInfo->carryPosY);
			ansiToUnicode(buffer,pTemp);
			elemInfo.elemType = LO_TEXT;

			wchar_t itemDescription[COMMON_CLIENT_MSG_LEN_512] = {0};
			char    description[256]={0};
			sprintf(description,"%s",buffer);
			itemDescription[0] = 0;
			wcscat(itemDescription, pTemp);
			elemInfo.isShowDes = true;
			elemInfo.gameObj._objType = LO_GO_POSITION;
			elemInfo.gameObj._objId[0] = pMapInfo->mapId;
			elemInfo.gameObj._objId[1] = pCarryInfo->carryPosX;
			elemInfo.gameObj._objId[2] = pCarryInfo->carryPosY;
			elemInfo.gameObj._objId[3] = 1;
			elemInfo.content = pTemp;
			elemInfo.description = itemDescription;
			elemInfo.color.red = carried_r;
			elemInfo.color.green = carried_g;
			elemInfo.color.blue = carried_b;
			pLayout->insertElem(elemInfo);
			delete [] pTemp ;
			////////////
			sprintf(textTempForMain,"<Obj gotype=pos s-d=false id=%d id1=%d id2=%d id3=%d des=%s c=%x>%s</Obj>",
				pMapInfo->mapId, pCarryInfo->carryPosX, pCarryInfo->carryPosY, 1,description,posColor,buffer);
			strcat(textForMainChat,textTempForMain);
			///////////////////
			pTemp = 0;
			sprintf(buffer,"%s",showText);
			ansiToUnicode(buffer,pTemp);
			LOElemInfo elem;
			elem.color.red = red;
			elem.color.green = green;
			elem.color.blue = blue;
			elem.elemType = LO_TEXT;
			elem.content.set(pTemp);

			pLayout->insertElem(elem);
			sprintf(textTempForMain,"<Obj c=%x>%s</Obj>",color,buffer);
			strcat(textForMainChat,textTempForMain);
			delete[] pTemp;

		}
		const CarryInfo* pLastCarry =  AutoGoBack::Singleton().GetCarryInfo(pathList.back());
		const CarryInfo* pTargetCarry = AutoGoBack::Singleton().GetCarryInfo(pLastCarry->targetCarry);
		const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetMapInfo(pTargetCarry->currentMapIndex);
		wchar_t* pTemp = 0;
		ansiToUnicode(pMapInfo->mapName,pTemp);
			
		LOElemInfo elem ;
		elem.elemType=LO_TEXT;
		elem.color.red = red;
		elem.color.green = green;
		elem.color.blue = blue;
		elem.content.set(pTemp);
		pLayout->insertElem(elem);
		delete [] pTemp;
		sprintf(textTempForMain,"<Obj c=%x>%s</Obj></Seg>",color,pMapInfo->mapName);
		strcat(textForMainChat,textTempForMain);
		pLayout->flashLayout();
		ChatMainDlg::InsertSystemMsg(pLayout);
		KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,textForMainChat);
	}
	else
	if(errorMode == ResultError_not_select_start_place)
	{
		ILayout* pLayout = pResultPathText->getLayout();
		char buffer[COMMON_CLIENT_MSG_LEN_512] = {0};
		int width  = (int)pResultPathText->getWidth(Absolute);
		sprintf(buffer,"<Layout width = %d><Seg text-align=left></Seg></Layout>",width);
		pLayout->clearLayout();
		pLayout->SetText(buffer);
		sprintf(buffer,"%s",pathResultNoStart);
		wchar_t* pTemp = 0;
		ansiToUnicode(buffer,pTemp);
		LOElemInfo elemInfo;
		elemInfo.elemType = LO_TEXT;
		elemInfo.color.red = red;
		elemInfo.color.green = green;
		elemInfo.color.blue = blue;
		elemInfo.content.set(pTemp);
		pLayout->insertElem(elemInfo);
		delete [] pTemp;
		ChatMainDlg::InsertSystemMsg(pLayout);
		char bufferForMain[1024]={0};
		sprintf(bufferForMain,"<Obj c=%x>%s</Obj></Seg>",color,buffer);
		strcat(textForMainChat,bufferForMain);
		KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,textForMainChat);
		return;
	}
	else
	if(errorMode == ResultError_not_select_end_place)
	{
		ILayout* pLayout = pResultPathText->getLayout();
		char buffer[COMMON_CLIENT_MSG_LEN_512] = {0};
		int width  = (int)pResultPathText->getWidth(Absolute);
		sprintf(buffer,"<Layout width = %d><Seg text-align=left></Seg></Layout>",width);
		pLayout->clearLayout();
		pLayout->SetText(buffer);
		wchar_t* pTemp = 0;
		sprintf(buffer,"%s",pathResultNoEnd);
		ansiToUnicode(buffer,pTemp);
		LOElemInfo elemInfo;
		elemInfo.color.red = red;
		elemInfo.color.green = green;
		elemInfo.color.blue = blue;
		elemInfo.elemType = LO_TEXT;
		elemInfo.content.set(pTemp);
		pLayout->insertElem(elemInfo);
		ChatMainDlg::InsertSystemMsg(pLayout);
		delete [] pTemp;
		char bufferForMain[1024]={0};
		sprintf(bufferForMain,"<Obj c=%x>%s</Obj></Seg>",color,buffer);
		strcat(textForMainChat,bufferForMain);
		KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,textForMainChat);
		return;
	}
	else
	if(errorMode == ResultError_not_find)
	{
		ILayout* pLayout = pResultPathText->getLayout();
		char buffer[COMMON_CLIENT_MSG_LEN_512] = {0};
		int width  = (int)pResultPathText->getWidth(Absolute);
		sprintf(buffer,"<Layout width = %d><Seg text-align=left></Seg></Layout>",width);
		pLayout->clearLayout();
		pLayout->SetText(buffer);
		wchar_t* pTemp = 0;
		sprintf(buffer,"%s",pathResultNotFind);
		ansiToUnicode(buffer,pTemp);
		LOElemInfo elemInfo;
		elemInfo.color.red = red;
		elemInfo.color.green = green;
		elemInfo.color.blue = blue;
		elemInfo.elemType = LO_TEXT;
		elemInfo.content.set(pTemp);
		pLayout->insertElem(elemInfo);
		ChatMainDlg::InsertSystemMsg(pLayout);
		delete [] pTemp;
		char bufferForMain[1024]={0};
		sprintf(bufferForMain,"<Obj c=%x>%s</Obj></Seg>",color,buffer);
		strcat(textForMainChat,bufferForMain);
		KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,textForMainChat);
		return;
	}
	else
	{
		ILayout* pLayout = pResultPathText->getLayout();
		char buffer[COMMON_CLIENT_MSG_LEN_512] = {0};
		int width  = (int)pResultPathText->getWidth(Absolute);
		sprintf(buffer,"<Layout width = %d><Seg text-align=left></Seg></Layout>",width);
		pLayout->clearLayout();
		pLayout->SetText(buffer);
		wchar_t* pTemp = 0;
		sprintf(buffer,"%s",pathResultTheSame);
		ansiToUnicode(buffer,pTemp);
		LOElemInfo elemInfo;
		elemInfo.color.red = red;
		elemInfo.color.green = green;
		elemInfo.color.blue = blue;
		elemInfo.elemType = LO_TEXT;
		elemInfo.content.set(pTemp);
		pLayout->insertElem(elemInfo);
		ChatMainDlg::InsertSystemMsg(pLayout);
		delete [] pTemp;
		char bufferForMain[1024]={0};
		sprintf(bufferForMain,"<Obj c=%x>%s</Obj></Seg>",color,buffer);
		strcat(textForMainChat,bufferForMain);
		KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,textForMainChat);
		return;
	}
}
bool KUiPathHelp::HandleClickResultText(const CEGUI::EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	Point pos = pResultPathText->getUnclippedPixelRect().getPosition();
	Point off = pResultPathText->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	LOElemInfo elemInfo;
	ILayout* pLayout = pResultPathText->getLayout();
	if(pLayout->pickupElem(xPos,yPos,elemInfo)==false)
	{
		return true;
	}
	if(elemInfo.gameObj._objType == LO_GO_POSITION)
	{
		const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetMapInfo(elemInfo.gameObj._objId[0]);
		if(pMapInfo == 0)
			return true;
		//检查是不是当前地图///////////////
		MapPosInfo mapInfo;
		g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_SCENE_POS, (unsigned)&mapInfo, (int)&Position());
		if(strcmp(pMapInfo->mapName,mapInfo.mapName)!=0)
		{
			KUiChannelCentre::GetSingleton().toSysMsg(MSG_CANT_AUTORUN);
			return true;
		}
		g_pCoreShell->OperationRequest( GOI_GOTO_POS, (unsigned)elemInfo.gameObj._objId[1], (int)elemInfo.gameObj._objId[2]*2);
		
	}
	return true;
}
void KUiPathHelp::Hide()
{
	KUiWndSingleton<KUiPathHelp>::Hide();
}
void KUiPathHelp::Show()
{
	KUiWndSingleton<KUiPathHelp>::Show();
}

bool KUiPathHelp::HandleStartScrollBarPos(const CEGUI::EventArgs& args)
{
	float height = pStartPlacListBox->getHeight(Absolute);
	float allHeight = (float)startListLength;
	if(allHeight<=height)
	{
		pStartScrollbar->setScrollPosition(0.0f);
		return true;
	}
	float curPersent = pStartScrollbar->getScrollPosition();
	float curlen = (allHeight-height)* curPersent;
	int size = startPlaceItemList.size();
	TLCheckbox* pCheckBox = (TLCheckbox*)startPlaceItemList[0];
	Point absPoint;
	curlen += 0.5f;
	if(curPersent == 0.0f)
	{
		absPoint.d_x = (float)startPx;
		absPoint.d_y = (float)startPy;
		pCheckBox->setPosition(Absolute,absPoint);
	}
	else
	{
		absPoint.d_x = (float)startPx;
		absPoint.d_y = -curlen;
		pCheckBox->setPosition(Absolute,absPoint);
	}
	for(int i = 1; i < size; i++)
	{
		TLCheckbox* pCheckBox = (TLCheckbox*)startPlaceItemList[i];
		Point pt;
		pt.d_x = (float)startPx;
		pt.d_y = absPoint.d_y+i*(itemHeight+2);
		pCheckBox->setPosition(Absolute,pt);

	}
	return true;
}

bool KUiPathHelp::HandleEndScrollBarPos(const CEGUI::EventArgs& args)
{
	float height = pEndPlaceListBox->getHeight(Absolute);
	float allHeight = (float)endListLength;
	if(allHeight<=height)
	{
		pEndScrollbar->setScrollPosition(0.0f);
		return true;
	}
	float curPersent = pEndScrollbar->getScrollPosition();
	float curlen = (allHeight-height)* curPersent;
	int size = endPlaceItemList.size();
	TLCheckbox* pCheckBox = (TLCheckbox*)endPlaceItemList[0];
	Point absPoint;
	curlen += 0.5f;
	if(curPersent == 0.0f)
	{
		absPoint.d_x = (float)startPx;
		absPoint.d_y = (float)startPy;
		pCheckBox->setPosition(Absolute,absPoint);
	}
	else
	{
		absPoint.d_x = (float)startPx;
		absPoint.d_y = -curlen;
		pCheckBox->setPosition(Absolute,absPoint);
	}
	for(int i = 1; i < size; i++)
	{
		TLCheckbox* pCheckBox = (TLCheckbox*)endPlaceItemList[i];
		Point pt;
		pt.d_x = (float)startPx;
		pt.d_y = absPoint.d_y+i*(itemHeight+2);
		pCheckBox->setPosition(Absolute,pt);

	}
	return true;
}
bool KUiPathHelp::handleResultScrollBarPos(const CEGUI::EventArgs& args)
{
	LORect rc = pResultPathText->getLayout()->getRenderArea();
	float height = pResultPathText->getHeight(Absolute);
	if((float)rc.getHeight()<=height)
	{
		pResultScrollBar->setScrollPosition(0.0f);
		return true;
	}
	float curPersent = pResultScrollBar->getScrollPosition();
	float curlen = ((float)rc.getHeight()-height)* curPersent;
	Point pt = pResultPathText->getLayoutOffset();
	pResultPathText->setLayoutOffset(pt.d_x,(int)-curlen);
	pResultPathText->update(0);
	return true;
}
