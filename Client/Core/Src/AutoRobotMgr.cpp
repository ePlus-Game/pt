//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:9   10:48
//      File_base        : AutoRobotMgr
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"

#ifdef	_AUTO_ROBOT

#include "AutoRobotMgr.h"
#include "KPlayer.h"
#include "KSkills.h"
#include "CoreShell.h"
#include "KSubWorldSet.h"
#include "MapObstacleMgr.h"
#include "ai_player_controller.h"
#include "Scene/KScenePlaceC.h"
#include "KItemGenerator.h"
#include "AutoDialogNpc.h"
#include "KNpcTemplate.h"
#include "KWin32Wnd.h"
#include "cfs_filelogs.h"

extern iCoreShell*							g_pCoreShell;
extern KScenePlaceC	g_ScenePlace;
//#include <list>
//using std::list;

#define		AUTO_ATTACK_TIME_INTERVAL	GAME_FPS
#define		AUTO_RUN_TIME_INTERVAL		(GAME_FPS / 6)
#define		FINDPATH_TIME_INTERVAL		1500		// ms

////////////////////////////
AutoMapInfo::AutoMapInfo()
{
	mapId = 0;//地图ID
	carryNumbers = 0;//传送点数目
	memset(mapName,0,AUTO_ITEM_NAME_LENGTH);
	levelLimited  = 0;//
}
AutoMapInfo::~AutoMapInfo()
{

}
CarryInfo::CarryInfo()
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
CarryInfo::~CarryInfo()
{

}
int AutoMapInfo::isHaveTypeNpc(MapNpcType type)
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
////////////////////////////

AutoRobotMgr&	AutoRobotMgr::Singleton()
{
	static AutoRobotMgr	mgr;
	return mgr;
}

AutoGoBack::AutoGoBack()
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
AutoGoBack::~AutoGoBack()
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
AutoMapInfo* AutoGoBack::GetCurrentMapInfo()
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
const AutoMapInfo* 
AutoGoBack::GetMapInfo( const char* mapName ) const 
{
	int numberMaps = mapInfoList.size();
	for(int i = 0; i < numberMaps;i++)
	{
		AutoMapInfo* pInfo = mapInfoList[i];
		if(strcmp( mapName,pInfo->mapName) == NULL)
			return pInfo;
	}
	return 0;
}
AutoMapInfo* AutoGoBack::GetMapInfo(int id)
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
const CarryInfo* AutoGoBack::GetCarryInfo(int id)
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
void AutoGoBack::SetCurrentMapInfo(const char* name)
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
bool AutoGoBack::IsInCloseList(int id)
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
bool AutoGoBack::IsInPenList(int id)
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
bool AutoGoBack::FindPathInCloseList(int id,pathInfo& info)
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
void AutoGoBack::CreateCarryPath()
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
const vector<MapNpcInfo>* AutoGoBack::GetMapNpcListByName(const char* mapName)
{

	int numberMaps = mapInfoList.size();
	for(int i = 0; i < numberMaps;i++)
	{
		AutoMapInfo* pInfo = mapInfoList[i];
		if(strcmp(pInfo->mapName,mapName) == 0)

			return &pInfo->npcList;
	}
	return  0;
}
void AutoGoBack::CreateComeBackPath(list<int>* pathLista)
{
	pathLista->clear();
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
				pathLista->push_front(pCarry->carryID);
		}
		
	}
}
bool AutoGoBack::FindComeBackPath(list<int>* pathLista,int mapId)
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
		if(pMap->mapId == mapId)
		{
			CreateComeBackPath(pathLista);
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
	return false;
}
bool AutoGoBack::GotoSellItem()
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
		if(pMap->isHaveTypeNpc(npcTyp_Sell_Medicne) != -1)
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
	return false;
}
bool AutoGoBack::GotoTargetMap(int mapId)
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
			findOpenList.push_back(path);
		}
		
	}
	return false;
}
AutoGoBack& AutoGoBack::Singleton()
{
	static AutoGoBack autoBack;
	return autoBack;
}
void AutoRobotMgr::Activate()
{
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive
		||Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_death
		)

	{
		SetMode(enRobotMode_None);
		return;
	}
	KUiPlayerRuntimeInfo info;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_INFO, (unsigned int)&info, NULL );
	if(info.nLife == 0)
	{
		SetMode(enRobotMode_None);
		return;
	}
	AutoUseMedicine();
	
	if(enRobotMode_AutoAttack == m_RobotMode)
	{
		if( g_SubWorldSet.GetGameTime() % 10 == 0 )
		{
			AutoAttack();
			AutoBlast();
		}
	}
	else if(enRobotMode_AutoRun == m_RobotMode)
	{
		if( g_SubWorldSet.GetGameTime() % AUTO_RUN_TIME_INTERVAL == 0 )
		{
		//	canNotPickIndex.clear();
			canNotAttackNpcList.clear();
			if(!m_AutoPathFinder.Active())
			{
				StopAutoWalk();
			}
			else
			{
				if(autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
				{
					FindTargetNpcWhenRun( );
					if(!(autoAttackFlags&AUTO_ATTACK_FLAG_GO_BACK))
					{
						if((autoGoHomeFlags&AUTO_GO_HOME)&&(autoGoHomeFlags&AUTO_GO_ENABLE))
						{
							RemoveAutoGoHomeFlags(AUTO_GO_HOME);
							RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
							SetMode(enRobotMode_None);
						}
					}
				}
			}
		}
	}
	else
	if(enRobotMode_AutoPickUpItem == m_RobotMode)
	{
		if( g_SubWorldSet.GetGameTime() % AUTO_RUN_TIME_INTERVAL == 0 )
			AutoPickObj(false);
	}
	else
	if( enRobotMode_ReadyForSell == m_RobotMode)
	{
		if( g_SubWorldSet.GetGameTime() % AUTO_RUN_TIME_INTERVAL == 0 )
			OnReadyForSell();
	}
	else
	if(enRobotMode_Selling == m_RobotMode)
	{
		if( g_SubWorldSet.GetGameTime() % AUTO_RUN_TIME_INTERVAL == 0 )
			OnSellItems();
	}
	else
	if(enRobotMode_WaitingForChangeMap == m_RobotMode)
	{

	}
	else
	if(enRobotMode_GotoOtherMap == m_RobotMode )
	{
		DoGoToOtherMap();
	}
	else
	{
		AutoGoActive();
	}
	if (autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
	{
		TestAutoCastSkill();
	}
}

void AutoRobotMgr::ResetTargetNpc()
{
	if(m_RobotMode != enRobotMode_AutoAttack )
		return;
	int nSelfIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nTargetIdx = Npc[nSelfIdx].GetTargetNpc();
	if((nTargetIdx != nSelfIdx && IsValidNpc( nTargetIdx ) ) )
	{
		canNotAttackNpcList.push_back(Npc[nTargetIdx].m_dwID);
	}
	Npc[nSelfIdx].SetTarget( type_npc,0 );
	if(AutoPlayerRandomGo())
	{
		
		SetMode(enRobotMode_AutoRun);
		AddAutoAttackFlags(AUTO_ATTACK_FLAG_RANDOM_GO);
		canNotAttackNpcList.clear();
	}
}
void AutoRobotMgr::OnReadyForSell()
{
	OnSelectNpcDlg();
//	SetMode(enRobotMode_Selling);
}
void AutoRobotMgr::OnSellItems()
{
	if (m_bIsAutoRepair)
	{
		CoreDataChanged(GDCNI_AUTO_REPAIR, 0, 0);
	}

	if(runForSell == false)
	{
		SetMode(enRobotMode_None);
		return;
	}
	KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
	for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
	{
		PlayerItem& item = itemList.m_Items[i];
		if(item.nPlace == pos_equip )
		{
			continue;
		}
		int itemIndex = item.nIdx;
		
		if(itemIndex <= 0)
		{
			continue;
		}
		if(Item[itemIndex].GetGenre() != item_equip)
		{
			continue;
		}
		if (Item[itemIndex].GetGenre() == item_task && Item[itemIndex].GetLevel() != QK_Bag)
		{
			continue;
		}
		else if ( Item[itemIndex].GetGenre() == item_magicorscript && Item[itemIndex].GetParticular() == 145 )
		{
			continue;
		}
		if(autoAttackFlags& AUTO_ATTACK_FLAG_SELL_CHEAP)
		{
			ITEM_QUALITY_LABEL quelity = Item[itemIndex].GetQualityLabel();
			if(quelity >= 2)
				continue;
		}
		SendClientCmdSell(Item[itemIndex].GetID());
		return;
	}
	//在这里东西已经买完了
	SetMode(enRobotMode_None);
	//如果要添加买东西的选项可以在这里添加
	//在这里返回
	//
	comebackList.clear();
	comeback = false;
	runForSell = false;
	if(AutoFindBackPath())
	{
		comeback = true;
	}
	RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
	RemoveAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
	RemoveAutoGoHomeFlags(AUTO_GO_HOME);
	

}
void AutoRobotMgr::OnSelectNpcDlg(KUiQuestionAndAnswer* answer)
{
	if ( runForSell)
	{
		m_AutoPathFinder.StopAutoWalk();
		PLAYER_SELECTUI_COMMAND command;
		command.wLength = sizeof(PLAYER_SELECTUI_COMMAND) - sizeof(DWORD);
		command.nSelectIndex = 0;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		SetMode(enRobotMode_None);
	}
}
void AutoRobotMgr::StopAutoWalk()
{
	if( gotoOtherMap == true)
	{
		SetMode(enRobotMode_WaitingForChangeMap);
		return;
	}
	if(autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
	{
		if(autoAttackFlags&AUTO_ATTACK_FLAG_RANDOM_GO)
		{
			RemoveAutoAttackFlags(AUTO_ATTACK_FLAG_RANDOM_GO);
			SetMode( enRobotMode_AutoAttack );
			canNotAttackNpcList.clear();
			return;
		}
		if(autoGoHomeFlags&AUTO_GO_ENABLE)
		{
			RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
			RemoveAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
			RemoveAutoGoHomeFlags(AUTO_GO_HOME);
			SetMode(enRobotMode_None);
			if(lastGo)
			{
				int playerIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
				int mapPixelX = 0,mapPixelY = 0;
				Npc[playerIndex].GetMpsPos(&mapPixelX,&mapPixelY);
				mapPixelX = (mapPixelX>>5);
				mapPixelY= (mapPixelY>>5);
				mapPixelY>>=1;
				AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetCurrentMapInfo();
				if(pMapInfo == 0)
				{
					lastGo = false;
					m_AutoPathFinder.StopAutoWalk();
					RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
					RemoveAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
					return;
				}
				int npcIdx = pMapInfo->isHaveTypeNpc(npcTyp_Sell_Medicne);
				MapNpcInfo& npc = pMapInfo->npcList[npcIdx];
				if(mapPixelX>=npc.npcPosX-1&&mapPixelX<=npc.npcPosX+1&&
					mapPixelY>=npc.npcPosY-1&&mapPixelY<=npc.npcPosY+1)
				{
/*					lastGo = false;
					RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
					RemoveAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
					RemoveAutoGoHomeFlags(AUTO_GO_HOME);
					SetMode(enRobotMode_None);
					return;*/
					lastGo = false;
					//在这里添加买卖东西
					m_AutoPathFinder.StopAutoWalk();
					int npcIdx = NpcSet.GetNpcByName(npc.npcName);
					if( npcIdx != -1 )
					{
						int mapX = 0,mapY = 0;
						AutoDialogNpc::getSingleton().setTargetNpc(Npc[npcIdx].GetTemplate()->m_NpcSettingIdx);
						Npc[npcIdx].GetMpsPos(&mapX,&mapY);
						mapX>>=5;
						mapY>>=6;
						AutoRunTo(mapX,mapY*2);
					}
				}
			}
			else
			{
				SetMode(enRobotMode_WaitingForChangeMap);
			}
//			AddAutoGoHomeFlags(AUTO_GO_PREPAER);
//			SetMode(enRobotMode_None);
			return;
		}
		if(comeback == true )
		{
			//停下来了，并且comeback为真，则等待切换地图
			SetMode(enRobotMode_WaitingForChangeMap);
		}
	}
	SetMode(enRobotMode_None);
}
bool AutoRobotMgr::AutoSimpleCanTravTo(int subWordIndex,int destX,int destY,int srcX,int srcY)
{

/*	// 防止客户端传入一个很大或很小的非法目标点，导致服务器计算过多
	if( abs(nStartX - nEndX) >= 1024 )
	{
		_ASSERT(false);
		return FALSE;
	}

	if( abs(nStartY - nEndY) >= 1024 )
	{
		_ASSERT(false);
		return FALSE;
	}*/

	const int  nStepLen = 16;

	int    nLenX = destX - srcX;
	int    nLenY = destY - srcY ;
	float  fLen = qsqrt(nLenX * nLenX + nLenY * nLenY);
	int	   nStepX = int(nStepLen * nLenX / fLen);
	int	   nStepY = int(nStepLen * nLenY / fLen);

	if(0 == nLenX && 0 == nLenY)
		return TRUE;

	while(true)
	{
		int nRet = SubWorld[subWordIndex].TestBarrier(srcX, srcY);
		nRet &= 0xf;
	
		switch(nRet)
		{
		case Obstacle_Normal:
		case Obstacle_Fly:

		case Obstacle_Jump:

		case Obstacle_JumpFly:
			return false;
		}

		srcX += nStepX;
		srcY += nStepY;

		if(nLenX > 0 && srcX >= destX)
			break;

		if(nLenX < 0 && srcX <= destX)
			break;

		if(nLenY > 0 && srcY >= destY)
			break;

		if(nLenY < 0 && srcY <= destY)
			break;
	}

	return TRUE;	
}
void AutoRobotMgr::AutoAttack()
{
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive
		||Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_death)
	{
		return;
	}
	/*if ( GotoSartPos( ))
	{
		return ;
	}*/
//	canNotPickIndex.clear();
	int nSelfIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nTargetIdx = Npc[nSelfIdx].GetTargetNpc();
	if(nTargetIdx == nSelfIdx || IsValidNpc( nTargetIdx ) == false )
	{
		if ( GotoSartPos( ))
		{
			return ;
		}
		if(!AutoSetPlayerTarget())
		{
			ResetTargetNpc();
			return;
		}//endif
		//暂时堵一下由于兽使技能而导致中断的bug。。
		SetMode(enRobotMode_AutoAttack);
		return;

	}
	else
	{
		if( resetCentrePos )
		{
			resetCentrePos = false;
			SetAutoAttackPos( );
		}
		if(CanAttackTarget(nSelfIdx,nTargetIdx) == false)
		{
			ResetTargetNpc();
			return;

		}
	}
	int nRelation = NpcSet.GetRelation( nSelfIdx, nTargetIdx );
	/////////////////////////////////////////////////////
	if(autoAttackFlags&AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER)
	{
		if(Npc[nTargetIdx].m_Level - Npc[nSelfIdx].m_Level>= (int)(autoAttackOwnerLevel>>16))
		{
				ResetTargetNpc();
				return;
		}
	}
	if(autoAttackFlags&AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER)
	{
		if(Npc[nSelfIdx].m_Level - Npc[nTargetIdx].m_Level>= (int)(autoAttackOwnerLevel&0x0000ffff))
		{
			ResetTargetNpc();
			return;
		}
	}
	int destX = 0,destY = 0;
	int srcX = 0,srcY = 0;
	Npc[nTargetIdx].GetMpsPos(&destX,&destY);
	Npc[nSelfIdx].GetMpsPos(&srcX,&srcY);
	if(!AutoSimpleCanTravTo(Npc[nTargetIdx].m_SubWorldIndex,destX,destY,srcX,srcY))
	{
		ResetTargetNpc();
		return;
	}
	//////////////最后一次判断NPC是否可攻击//
	if( !CanAttackNpc( nTargetIdx) )
	{
		ResetTargetNpc();
		return;
	}
	if ( Npc[nSelfIdx].GetTargetType() == type_npc &&
		 !Npc[nTargetIdx].IsPlayer() &&
		 (nRelation & relation_enemy) &&
		 nTargetIdx > 0 )
	{
		PlayerController::Singleton().FollowAttack(nTargetIdx);
		return;
	}
}


void AutoRobotMgr::AutoUseMedicine()
{
	if(autoEntrustComputerMode&ENSTRUST_AUTO_USEITEM_MODE)
	{
		if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive )
		{
			return;
		}
		int HPPersent = 0;
		int MPPersent = 0;
		if(autoUseMedicineFlags&AUTO_USE_HP_MEDICINE_NORMAL)
		{
			HPPersent = 20;
			MPPersent = 10;
		}
		else
		{
			HPPersent = (autoHPMPPersent>>16);
			MPPersent = (autoHPMPPersent&0xffff);
			if(HPPersent == 0) HPPersent = 20;
			if(MPPersent == 0) MPPersent = 10;
		}
		int nSelfIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	    int currentHPPersent = Npc[nSelfIdx].GetCurrentLifePercentage();
		if(currentHPPersent < HPPersent)
		{
			int nIdx = 0,nX = 0,nY = 0;
			bool isPlayerHave = false;

			ItemInfoList::iterator it = medicineForHPList.begin();
			ItemInfoList::iterator endIt = medicineForHPList.end();
			for (; it != endIt; ++it)
			{
				AutoItemInfo itemInfo = *it;
				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSameParticularInEquipment(
					itemInfo.autoItemTyp,
					itemInfo.autoItemSort,
					itemInfo.autoItemParticular,
					&nIdx, &nX, &nY))
				{
					isPlayerHave = true;
					break;
				}
			}
	
			if(isPlayerHave)
			{
			    static unsigned int lastUseTime = 0;//UNIX_TMIE_STAMP;
				if(lastUseTime == 0 ||
					time(0) - lastUseTime>4)
				{
					ItemPos tmpPos, tmpTargetPos;
					tmpPos.nPlace = pos_equiproom;
					tmpPos.nX = nX;
					tmpPos.nY = nY;
					ZeroMemory(&tmpTargetPos, sizeof(ItemPos) );
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(nIdx, tmpPos, 0, tmpTargetPos);
				
					lastUseTime = time(0);
				}
			}

		}
		int currentMPPersent = Npc[nSelfIdx].GetCurrentManaPercentage();
		if(currentMPPersent < MPPersent)
		{
			int nIdx = 0,nX = 0,nY = 0;
			bool isPlayerHave = false;

			ItemInfoList::iterator it = medicineForMPList.begin();
			ItemInfoList::iterator endIt = medicineForMPList.end();
			
			for (; it != endIt; ++it)
			{
				AutoItemInfo itemInfo = *it;
				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSameParticularInEquipment( 
					itemInfo.autoItemTyp,
					itemInfo.autoItemSort,
					itemInfo.autoItemParticular,
					&nIdx, &nX, &nY ))
				{
					isPlayerHave = true;
					break;
				}
			}

			if(isPlayerHave)
			{
				static unsigned int lastUseTime = 0;//UNIX_TMIE_STAMP;
				if(lastUseTime == 0 ||
					time(0) - lastUseTime>4)
				{
					ItemPos tmpPos, tmpTargetPos;
					tmpPos.nPlace = pos_equiproom;
					tmpPos.nX = nX;
					tmpPos.nY = nY;
					ZeroMemory(&tmpTargetPos, sizeof(ItemPos) );
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(nIdx, tmpPos, 0, tmpTargetPos);
					lastUseTime = time(0);
				}
			}
		}
	}

}
bool AutoRobotMgr::AutoPlayerRandomGo()
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if( IsValidNpc( nPlayerNpcIdx ) == false )
	{
		return false;
	}
	int mapX = Npc[nPlayerNpcIdx].GetMapX();
	int mapY = Npc[nPlayerNpcIdx].GetMapY();

	if( Npc[nPlayerNpcIdx].m_RegionIndex  == -1 ||
		Npc[nPlayerNpcIdx].m_SubWorldIndex == -1 )
	{
		return  false;
	}
	if( m_bWorldChanged == true )
	{
		m_bWorldChanged = false;
		SetAutoAttackPos( );
	}
	
	int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
	int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
	int currentX = (reginPixelX>>5)+mapX;
	int currentY = (reginPixelY>>5)+mapY;
	currentY>>=1;
	int destPosX = 0;
	int destPosY = 0;
	if( abs( currentX - centreX ) >= attackRadius ||
		abs( currentY - centreY ) >= attackRadius )
	{
		destPosX = centreX;
		destPosY = centreY;
		lastX = currentX;
		lastY = currentY;
		destPosY<<=1;
		if( m_AutoPathFinder.SafeGoto( destPosX,destPosY))
		{
			if( destPosX == currentX && destPosY == currentY*2)
			{
				m_AutoPathFinder.GoTo( centreX,centreY*2);
			}
			autoGotoFindNpc = true;
			return true;
		}
		autoGotoFindNpc = false;
		return false;

	}
	else
	{
		int dir = 0;
		if( lastX ==0 ||
			lastY == 0)
			dir = rand()%8;
		else
		{
			if( currentX - lastX >0)
			{
				if( currentY - lastY == 0 )
				{
					dir = 2;
				}
				else
				if(currentY - lastY > 0 )
				{
					dir = 3;
				}
				else
				{
					dir = 1;
				}
			}
			else
			if( currentX - lastX < 0)
			{
				if( currentY - lastY == 0)
				{
					dir = 6;
				}
				else
				if( currentY - lastY > 0)
				{
					dir = 5;
				}
				else
				{
					dir = 7;
				}
			}
			else
			{
				if( currentY - lastY> 0)
				{
					dir = 4;
				}
				else
				if( currentY - lastY < 0)
				{
					dir = 0;
				}
				else
				{
					dir = rand( )%8;
				}
			}
		}
		switch(dir)
		{
		case 0:
			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					//
					destPosX = currentX;
					destPosY = currentY + randRunDistance;
				}
				else
				{
					destPosX = currentX;
					destPosY = currentY - randRunDistance;
				}	

			}
			break;
		case 1:
			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX - randRunDistance;
					destPosY = currentY + randRunDistance;
				}
				else
				{
					destPosX = currentX + randRunDistance;
					destPosY = currentY - randRunDistance;
				}
			}
			break;
		case 2:
			{
				
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX - randRunDistance;
					destPosY = currentY ;
				}
				else
				{
					destPosX = currentX + randRunDistance;
					destPosY = currentY ;
				}
			}
			break;
		case 3:
			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX - randRunDistance;
					destPosY = currentY  - randRunDistance;
				}
				else
				{
					destPosX = currentX + randRunDistance;
					destPosY = currentY + randRunDistance;
				}
			}
			break;
		case 4:
			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX;
					destPosY = currentY  - randRunDistance;
				}
				else
				{
					destPosX = currentX ;
					destPosY = currentY + randRunDistance;
				}
			}
			break;
		case 5:
			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX + randRunDistance;
					destPosY = currentY - randRunDistance;
				}
				else
				{
					destPosX = currentX - randRunDistance;
					destPosY = currentY + randRunDistance;
				}
			}
			break;
		case 6:

			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX + randRunDistance;
					destPosY = currentY ;
				}
				else
				{
					destPosX = currentX - randRunDistance;
					destPosY = currentY ;
				}
			}
			break;
		case 7:
			{
				if( lastX == currentX &&
					lastY == currentY )
				{
					destPosX = currentX + randRunDistance;
					destPosY = currentY + randRunDistance;
				}
				else
				{
					destPosX = currentX - randRunDistance;
					destPosY = currentY - randRunDistance;
				}
			}
			break;
		}
		lastX = currentX;
		lastY = currentY;
		if( abs( destPosX - centreX )>attackRadius ||
			abs( destPosY - currentY ) > attackRadius )
		{
			//如果超出范围了截断 
			if( destPosX > centreX )
				destPosX = centreX + attackRadius;
			else
				destPosX = centreX - attackRadius;
			if( destPosY > centreY )
				destPosY = centreY + attackRadius;
			else
				destPosY = centreY - attackRadius;
		}
		destPosY<<=1;

		if( m_AutoPathFinder.SafeGoto(destPosX,destPosY) )
		{
			if( destPosX == currentX && destPosY == currentY*2)
			{
				if( destPosX == centreX && destPosY == centreY)
				{
					autoGotoFindNpc = false;
					SetMode( enRobotMode_AutoAttack );
					lastX = 0;
					lastY = 0;
					return false;
				}
				m_AutoPathFinder.GoTo(currentX,currentY*2);
			}
			autoGotoFindNpc = true;
			return true;
		}
		autoGotoFindNpc = false;
		return false;
	}
	/*
	int dir = rand()%8;
	int playerIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int mapX = 0,mapY = 0;
	Npc[playerIndex].GetMpsPos(&mapX,&mapY);
	mapX>>=5;
	mapY>>=6;
	int dx = 0,dy=0;
	int l = 8;
	l+=rand()%8;
	int dt = (int)((float)l*0.707);
	switch(dir)
	{
	case 0:
		{
			dx = mapX;
			dy = mapY-l;
		}
		break;
	case 1:
		{
			dx = mapX + dt;
			dy = mapY - dt;
		}
		break;
	case 2:
		{
			dx = mapX +l;
			dy = mapY;
		}
		break;
	case 3:
		{
			dx = mapX + dt;
			dy = mapY + dt;
		}
		break;
	case 4:
		{
			dx = mapX;
			dy = mapY +l;
		}
		break;
	case 5:
		{
			dx = mapX - dt;
			dy = mapY + dt;
		}
		break;
	case 6:
		{
			dx = mapX -l;
			dy = mapY;
		}
		break;
	case 7:
		{
			dx = mapX -dt;
			dy = mapY -dt;
		}
		break;
	}
	return m_AutoPathFinder.GoTo(dx,dy*2);
	*/
}



void
AutoRobotMgr::FindTargetNpcWhenRun( )
{
#define MAX_TIME_CHECK 5000
	if( autoGotoFindNpc == false)
	{
		return;
	}
	static int nLastPosX = 0,nLastPosY = 0;
	static DWORD lastTime = 0;
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int mapX = Npc[nPlayerNpcIdx].GetMapX();
	int mapY = Npc[nPlayerNpcIdx].GetMapY();
	int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
	int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
	int currentX = (reginPixelX>>5)+mapX;
	int currentY = (reginPixelY>>5)+mapY;
	currentY >>=1;

	if( abs( currentX - centreX ) < attackRadius &&
		abs( currentY - centreY ) < attackRadius )
	{
		int	nNearestEnemyIdx = GetNearestEnemyIdx();
		
		if(-1 == nNearestEnemyIdx)
		{
			canNotAttackNpcList.clear();
			if( lastTime == 0 )
			{
				lastTime = GetTickCount( );
				nLastPosX = currentX;
				nLastPosY = currentY;
				return;
			}
			//检查位置
			if( nLastPosX  == currentX &&
				nLastPosY == currentY )
			{

				DWORD currentTime = GetTickCount( );

				if( currentTime - lastTime > MAX_TIME_CHECK )
				{
					//不动了
					StopAutoWalk( );
					return;
				}
			}
			else
			{
				lastTime = GetTickCount( );
				nLastPosX = currentX;
				nLastPosY = currentY;

			}
			return;
		
		}
		else
		{
			//
			StopAutoWalk();
			lastTime = 0;
			nLastPosX = 0;
			nLastPosY = 0;
		}
		
		// 选择目标 --刘思亮
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, nNearestEnemyIdx);
		KTargetInfo tagTargetInfo;
		strncpy( tagTargetInfo.strName, Npc[nNearestEnemyIdx].Name, sizeof(tagTargetInfo.strName) );
		tagTargetInfo.nLifePercentage = Npc[nNearestEnemyIdx].GetCurrentManaPercentage();
		tagTargetInfo.nMagicPercentage = Npc[nNearestEnemyIdx].GetCurrentManaPercentage();
		tagTargetInfo.nSex		= Npc[nNearestEnemyIdx].m_nSex;
		tagTargetInfo.nMetier	= Npc[nNearestEnemyIdx].m_Series;
		tagTargetInfo.nLevel	= Npc[nNearestEnemyIdx].m_Level;
		if ( Npc[nNearestEnemyIdx].m_bShowTargetFace )
		{
			CoreDataChanged( GDCNI_SEL_TARGET, (unsigned int)&tagTargetInfo, NULL );
		}
		else
		{
			CoreDataChanged( GDCNI_SEL_TARGET, FALSE, NULL );
		}
		SetMode( enRobotMode_AutoAttack );
		autoGotoFindNpc = false;
		
	}
}
bool AutoRobotMgr::AutoSetPlayerTarget()
{
	int	nNearestEnemyIdx = GetNearestEnemyIdx();

	if(-1 == nNearestEnemyIdx)
	{
		canNotAttackNpcList.clear();
		if(AutoPlayerRandomGo())
		{
			SetMode(enRobotMode_AutoRun);
			AddAutoAttackFlags(AUTO_ATTACK_FLAG_RANDOM_GO);
			canNotAttackNpcList.clear();
		}
		return false;
	}
	else
	{
		//
		lastX = 0;
		lastY = 0;
		StopAutoWalk();
	}

	// 选择目标 --刘思亮
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, nNearestEnemyIdx);
	KTargetInfo tagTargetInfo;
	strncpy( tagTargetInfo.strName, Npc[nNearestEnemyIdx].Name, sizeof(tagTargetInfo.strName) );
	tagTargetInfo.nLifePercentage = Npc[nNearestEnemyIdx].GetCurrentManaPercentage();
	tagTargetInfo.nMagicPercentage = Npc[nNearestEnemyIdx].GetCurrentManaPercentage();
	tagTargetInfo.nSex		= Npc[nNearestEnemyIdx].m_nSex;
	tagTargetInfo.nMetier	= Npc[nNearestEnemyIdx].m_Series;
	tagTargetInfo.nLevel	= Npc[nNearestEnemyIdx].m_Level;
	if ( Npc[nNearestEnemyIdx].m_bShowTargetFace )
	{
		CoreDataChanged( GDCNI_SEL_TARGET, (unsigned int)&tagTargetInfo, NULL );
	}
	else
	{
		CoreDataChanged( GDCNI_SEL_TARGET, FALSE, NULL );
	}
	return true;
}
void AutoRobotMgr::AutoPickObj(bool normalPicked)
{
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive )
	{
		SetMode(enRobotMode_None);
		return;
	}

	int nNearestObjIdx = GetNearestObjIdx(normalPicked);

	if(-1 == nNearestObjIdx)
	{
		RemoveAutoPickupItemFlags(AUTO_PICKUP_ITEM_DEATH);
		if(autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
		{
			SetMode(enRobotMode_AutoAttack);
		}
			
		else
		{
			SetMode(enRobotMode_None);
		}
	//	canNotPickIndex.clear();
		return;
	}	
	if(autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
	{
		if(autoAttackFlags&AUTO_ATTACK_FLAG_GO_BACK)
		{
			int weight= 0, weightMax = 0;
			Player[CLIENT_PLAYER_INDEX].GetWeightCurrent(&weight,&weightMax);
			if(weight>=weightMax)
			{
				if(AutoUseItemGoToSell())
				{
					AddAutoGoHomeFlags(AUTO_GO_PREPAER);
					AddAutoGoHomeFlags(AUTO_GO_ENABLE);
					SetMode(enRobotMode_None);
					return;
				}
				else
				if(!AutoFindHomePath())
				{
					SetMode(enRobotMode_AutoAttack);
		//			canNotPickIndex.clear();
				}
				return;
			}
			if(g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BAG_IS_FULL,0,0))
			{
				if(AutoUseItemGoToSell())
				{
					AddAutoGoHomeFlags(AUTO_GO_PREPAER);
					AddAutoGoHomeFlags(AUTO_GO_ENABLE);
					SetMode(enRobotMode_None);
					return;
				}
				else
				if(!AutoFindHomePath())
				{
					SetMode(enRobotMode_AutoAttack);
	//				canNotPickIndex.clear();
				}
				return;
			}
		}
		else
		{
			int weight= 0, weightMax = 0;
			Player[CLIENT_PLAYER_INDEX].GetWeightCurrent(&weight,&weightMax);
			if(weight>=weightMax)
			{
				SetMode(enRobotMode_AutoAttack);
	//			canNotPickIndex.clear();
				return;
			}
			if(g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BAG_IS_FULL,0,0))
			{
				SetMode(enRobotMode_AutoAttack);
		//		canNotPickIndex.clear();
				return;
			}
		}
	}
	PlayerController::Singleton().PickupObject(nNearestObjIdx);
}

void AutoRobotMgr::SetAutoAttackEnemyHigherLevel(unsigned int level)
{
	autoAttackOwnerLevel&=0x0000ffff;
	autoAttackOwnerLevel|=(level<<16);
}
void AutoRobotMgr::SetAutoAttackEnemyLowerLevel(unsigned int level)
{
	autoAttackOwnerLevel&=0xffff0000;
	autoAttackOwnerLevel|=level;
}
int AutoRobotMgr::GetNearestObjIdx(bool bNormalPicked)
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if(IsValidNpc(nPlayerNpcIdx) == false)
		return -1;
	KNpc &playerNpc = Npc[nPlayerNpcIdx];
	int	nSubWorldIdx = playerNpc.m_SubWorldIndex;

	if(playerNpc.m_RegionIndex == -1 || nSubWorldIdx == -1)
		return  -1;
	KRegion &CurRegion = SubWorld[nSubWorldIdx].m_Region[playerNpc.m_RegionIndex];

	int	nNearestObjIdx = -1;

	list<int> objIndexList;
	if ( CurRegion.m_nIndex >= 0 && playerNpc.m_RegionIndex >= 0 )
	{		
		int nMinDistance = GetNearestObjInRegion(CurRegion, nPlayerNpcIdx, nNearestObjIdx,bNormalPicked);
		if(nNearestObjIdx !=-1)
			objIndexList.push_front(nNearestObjIdx);
		for(int i = 0; i < 8; ++i)
		{
			int nRegionIdx = CurRegion.m_nConnectRegion[i];

			if(-1 != nRegionIdx)
			{
				int nObjIdx = 0;
				int nDistance = GetNearestObjInRegion(SubWorld[nSubWorldIdx].m_Region[nRegionIdx], nPlayerNpcIdx, nObjIdx ,bNormalPicked);
				if ((autoEntrustComputerMode&ENSTRUST_AUTO_PICK_UP_MODE)&&
					(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEATH))
		       //    (autoPickupItemFlags&AUTO_PICKUP_ITEM_DEAR_SET_FIRST)&&
				{
					if ( nObjIdx != -1 )
					{
						if(objIndexList.empty())
							objIndexList.push_front(nObjIdx);
						else
						{
							int firstIndex = objIndexList.front();
							if(Object[nObjIdx].m_nColorID> Object[firstIndex].m_nColorID)
							{
								objIndexList.push_front(nObjIdx);
							}
							else
							if(Object[nObjIdx].m_nColorID==Object[firstIndex].m_nColorID)
							{
								int firstObjDist = Object[firstIndex].GetDistanceSquare(nPlayerNpcIdx);
								if(nDistance<firstObjDist)
									objIndexList.push_front(nObjIdx);
							}
						}
					}
				}
				else
				{
					if ( nObjIdx != -1 && (nMinDistance == 0 || nDistance < nMinDistance) )
					{
						nMinDistance = nDistance;
						nNearestObjIdx = nObjIdx;

					}
				}
			}
		}
	}
	if ((autoEntrustComputerMode&ENSTRUST_AUTO_PICK_UP_MODE)&&
		(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEATH))
//	   (autoPickupItemFlags&AUTO_PICKUP_ITEM_DEAR_SET_FIRST)&&
	{
		if(objIndexList.empty())
			return -1;
		return objIndexList.front();
	}
	return nNearestObjIdx;
}

int	AutoRobotMgr::GetNearestObjInRegion(KRegion &curRegion, int nSrcNpcIdx, int &nObjIdx,bool bNormalPicked)
{
#define WHITE_COLOR_ID     0
#define GREEN_COLOR_ID     2
	int	nMinDistance = 0;
	int	nNearestObjIdx = -1;
	KIndexNode *pNode = (KIndexNode*)curRegion.m_ObjList.GetHead();
	list<int> objIndexList;
	unsigned int currentTime = time(0);
	while(NULL != pNode)
	{
		int nTargetIdx = pNode->m_nIndex;
		pNode = (KIndexNode*)pNode->GetNext();
		if((autoEntrustComputerMode&ENSTRUST_AUTO_PICK_UP_MODE)&&
			(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEATH)&&bNormalPicked == false)
		{
			int targetX = 0,targetY = 0;
			Object[nTargetIdx].GetMpsPos(&targetX,&targetY);
			if(targetX<targetItemPosX - AUTO_PICKUP_POS_WIDTH/2||
				targetX>targetItemPosX + AUTO_PICKUP_POS_WIDTH/2||
				targetY<targetItemPosY - AUTO_PICKUP_POS_HEIGHT/2||
				targetY>targetItemPosY + AUTO_PICKUP_POS_HEIGHT/2)
				continue;
		}

		if(Object[nTargetIdx].m_bCanPickForClient == false&&
			bNormalPicked == false)
			continue;
		if (bNormalPicked == false)
		{
			bool isInNotPickupColor = false;
			bool isInList = false;
			bool isPickup = false;

			isInNotPickupColor = IsInNotPickupColorList(Object[nTargetIdx]);
			isInList = IsInPickupList(Object[nTargetIdx], isPickup);

			if (isInList)
			{
				if (!isPickup)
				{
					continue;
				}
			}
			else
			{
				if (isInNotPickupColor)
				{
					continue;
				}
			}
		}
		else
		{
			if (Object[nTargetIdx].m_nKind == Obj_Kind_Money
				|| Object[nTargetIdx].m_nKind == Obj_Kind_Item)
			{
				int nDistance = Object[nTargetIdx].GetDistanceSquare(nSrcNpcIdx);
				if ( nMinDistance == 0 || nDistance < nMinDistance )
				{
					nMinDistance = nDistance;
					nObjIdx = nTargetIdx;
					return nMinDistance;
				}
			}
		}

		if (Object[nTargetIdx].m_nKind == Obj_Kind_Money
			|| Object[nTargetIdx].m_nKind == Obj_Kind_Item)
		{
			if(objIndexList.empty())
			{
				objIndexList.push_back(nTargetIdx);
			}
			else
			{
				int temObjIndex = objIndexList.front();
				if(Object[temObjIndex].m_nColorID < Object[nTargetIdx].m_nColorID)
				{
					objIndexList.push_front(nTargetIdx);
				}
				else if(Object[temObjIndex].m_nColorID == Object[nTargetIdx].m_nColorID)
				{
					int temDist = Object[temObjIndex].GetDistanceSquare(nSrcNpcIdx);
					int targetDis = Object[nTargetIdx].GetDistanceSquare(nSrcNpcIdx);
					if(targetDis > temDist)
					{
						objIndexList.push_front(temObjIndex);
					}
					else
					{
						objIndexList.push_back(nTargetIdx);
					}
				}
			}
/*			int nDistance = Object[nTargetIdx].GetDistanceSquare(nSrcNpcIdx);
			if ( nMinDistance == 0 || nDistance < nMinDistance )
			{
				nMinDistance = nDistance;
				nObjIdx = nTargetIdx;
				return nMinDistance;
			}*/
		}

	/*	 if(m_pickTypeFlags == AUTO_PICKUP_ITEM_NOT_WHITE&&
			 bNormalPicked == false)
		 {
			 //不失去白色装备。但是金钱排除再外//
			 if(Object[nTargetIdx].m_nColorID <= WHITE_COLOR_ID&&
				 Object[nTargetIdx].m_nKind != Obj_Kind_Money)
			 {
				 if(pickMedicine)
				 {
					 if(isObjMedicine(Object[nTargetIdx].m_nDataID) == false)
					 {
						 continue;
					 }
				 }
				 else
				 {
					 continue;
				 }
			 }
		 }
		 else
		 if(m_pickTypeFlags == AUTO_PICKUP_ITEM_ONLY_GREEN&&
			 bNormalPicked == false)
		 {
			 if(Object[nTargetIdx].m_nColorID < GREEN_COLOR_ID&&
				 Object[nTargetIdx].m_nKind != Obj_Kind_Money)
			 {
				 if(pickMedicine)
				 {
					 if(isObjMedicine(Object[nTargetIdx].m_nDataID) == false)
					 {
						 continue;
					 }
				 }
				 else
				 {
					 continue;
				 }
			 }
		 }
			 

		 //药水//
		if ( Obj_Kind_Money == Object[nTargetIdx].m_nKind || 
			Obj_Kind_Item == Object[nTargetIdx].m_nKind )
		{
			if((autoEntrustComputerMode&ENSTRUST_AUTO_PICK_UP_MODE)&&
				(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEAR_SET_FIRST)&&
				(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEATH))
			{//优先拾取高级物品开启
					if(objIndexList.empty())
					{
						objIndexList.push_back(nTargetIdx);
					}
					else
					{
						int temObjIndex = objIndexList.front();
						if(Object[temObjIndex].m_nColorID>Object[nTargetIdx].m_nColorID)
						{
								objIndexList.push_front(nTargetIdx);
						}
						else
						if(Object[temObjIndex].m_nColorID == Object[nTargetIdx].m_nColorID)
						{
							int temDist = Object[temObjIndex].GetDistanceSquare(nSrcNpcIdx);
							int targetDis = Object[nTargetIdx].GetDistanceSquare(nSrcNpcIdx);
							if(targetDis<temDist)
							{
									objIndexList.push_back(nTargetIdx);
							}
						}
					}
			}	
			else
			{
				int nDistance = Object[nTargetIdx].GetDistanceSquare(nSrcNpcIdx);
				if ( nMinDistance == 0 || nDistance < nMinDistance )
				{
				
					nMinDistance = nDistance;
					 nNearestObjIdx = nTargetIdx;

				}
			}
		}*/
	}
	if((autoEntrustComputerMode&ENSTRUST_AUTO_PICK_UP_MODE)&&
		(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEATH))
//		(autoPickupItemFlags&AUTO_PICKUP_ITEM_DEAR_SET_FIRST)&&
	{
		if(objIndexList.empty())
		{
			nObjIdx = -1;
			return 0;
		}
		nObjIdx = objIndexList.front();
		return Object[nObjIdx].GetDistanceSquare(nSrcNpcIdx);
	}

	nObjIdx = nNearestObjIdx;
	return nMinDistance;
}


bool AutoRobotMgr::AutoRunToForSell(int dx,int dy)
{
#define FINDPATH_TIME_FOR_SELL_TIME 5000
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive )
	{
		return true;
	}
	static DWORD dwPreCalcTime = 0;

	DWORD dwCurTime = timeGetTime();

	if(dwCurTime - dwPreCalcTime > 0)
	{
		if ( m_AutoPathFinder.GoTo(dx, dy) )
		{
			m_RobotMode = enRobotMode_AutoRun;
			dwPreCalcTime = timeGetTime();
			CoreDataChanged(GDCNI_BEGIN_AUTO_PATH, 0, 0);
			return true;
		}
		else
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_AUTORUN_NOWAY, 0);
	}
	return false;
}
bool AutoRobotMgr::canRunTo(int nDstCellX, int nDstCellY, int nSearchUnit)
{
	return m_AutoPathFinder.canGoto(nDstCellX, nDstCellY, nSearchUnit);
}

void AutoRobotMgr::AutoRunTo(int nDstCellX, int nDstCellY, int nSearchUnit /* = enSearchUnit_1by1Cell */)
{
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive )
	{
		return;
	}

	static DWORD dwPreCalcTime = 0;

	DWORD dwCurTime = timeGetTime();

	if(dwCurTime - dwPreCalcTime > FINDPATH_TIME_INTERVAL)
	{
		dwPreCalcTime = timeGetTime();//记录上次自动寻路的时间
		int nOldDestX = nDstCellX;
		int nOldDesty = nDstCellY;

		if ( m_AutoPathFinder.SafeGoto(nDstCellX, nDstCellY, nSearchUnit) )
		{
			m_RobotMode = enRobotMode_AutoRun;
			CoreDataChanged(GDCNI_BEGIN_AUTO_PATH, 0, 0);
			
			if (nOldDestX != nDstCellX || nOldDesty != nDstCellY)
			{
				char szErrorMsg[128] = "";
				g_GetStringRes(sid_walk_adjust , szErrorMsg, 128);
				szErrorMsg[127] = 0;

				if (szErrorMsg[0]!= 0)
					CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)szErrorMsg, 0);
			}//end else

		}
		else
		{
			CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_AUTORUN_NOWAY, 0);
		}
	}
	else
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_CLICK_TOO_FREQUENTLY, 0);
	}

	
}

void AutoRobotMgr::AutoRunTo(float fXPosPercent, float fYPosPercent, int nSearchUnit /* = enSearchUnit_1by1Cell */)
{
	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_revive )
	{
		return;
	}

	int	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int	nMapId = SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID;
	
	int	nCellX, nCellY;
	MapObstacleMgr	&mapObsMgr = MapObstacleMgr::Singleton();
	
	if( mapObsMgr.GetCellPos(nMapId, fXPosPercent, fYPosPercent, nCellX, nCellY) )
		AutoRunTo(nCellX, nCellY, nSearchUnit);
}

void AutoRobotMgr::SetMode(enRobotMode enMode)
{
	if(enMode > enRobotMode_Begin && enMode < enRobotMode_End)
	{
		//开始自动打怪
		if(enRobotMode_AutoAttack == enMode)
		{
			CoreDataChanged(GDCNI_BEGIN_AUTO_ATTACK, 0, 0);
			isUsingGoHomeItem = false;
		}

		//停止自动打怪
		if(enRobotMode_AutoAttack == m_RobotMode && enMode != enRobotMode_AutoAttack)
		{
			CoreDataChanged(GDCNI_STOP_AUTO_ATTACK, 0, 0);
		}

		//停止自动寻路
		if(enRobotMode_AutoRun == m_RobotMode && enMode != enRobotMode_AutoRun)
		{
			CoreDataChanged(GDCNI_STOP_AUTO_PATH, 0, 0);
		}

		if(enRobotMode_AutoRun == m_RobotMode)
			m_AutoPathFinder.StopAutoWalk();

		m_RobotMode = enMode;
	}
}

void AutoRobotMgr::InitSkillPolicy()
{
	static	bool	bInit = false;

	if(!bInit)
	{
		m_SkillPolicy.Initialize();
		bInit = true;
	}
}

int AutoRobotMgr::GetSellNpcPosByName(const char* name)
{
	if(name == NULL)
		return -1;
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if(IsValidNpc(nPlayerNpcIdx) == false)
		return -1;
	KNpc &playerNpc = Npc[nPlayerNpcIdx];
	int	nSubWorldIdx = playerNpc.m_SubWorldIndex;
	
	if(nSubWorldIdx == -1||
		playerNpc.m_RegionIndex == -1)
		return -1;
	
	KRegion &CurRegion = SubWorld[nSubWorldIdx].m_Region[playerNpc.m_RegionIndex];

	int npcIdx = -1;
	GetSellNpcPosByNameInRegion(name,CurRegion,npcIdx);
	for(int i = 0; i < 8; ++i)
	{
		int nRegionIdx = CurRegion.m_nConnectRegion[i];
		if(nRegionIdx != -1)
		{
			GetSellNpcPosByNameInRegion(name,SubWorld[nSubWorldIdx].m_Region[nRegionIdx],npcIdx);
			if(npcIdx!=-1)
				break;
		}
	}
	return npcIdx;


}

int AutoRobotMgr::GetSellNpcPosByNameInRegion(const char* name,KRegion& curRegion,int &nNpcIdx)
{
	//OK l 

	KIndexNode *pNode = (KIndexNode*)curRegion.m_NpcList.GetHead();

	while( pNode != NULL )
	{
		int nTargetIdx = pNode->m_nIndex;
		if(IsValidNpc(nTargetIdx) == false)
		{
			pNode = (KIndexNode*)pNode->GetNext();
			continue;
		}
		pNode = (KIndexNode*)pNode->GetNext();

		if(strcmp( name,Npc[nTargetIdx].Name) == NULL )
		{
			//找到了
			if(Npc[nTargetIdx].m_Kind != kind_player )
			{
				nNpcIdx = nTargetIdx;
				return nTargetIdx;
			}
		}
	}
	return -1;

}
int	AutoRobotMgr::GetNearestEnemyIdx()
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if(IsValidNpc(nPlayerNpcIdx) == false)
		return -1;
	KNpc &playerNpc = Npc[nPlayerNpcIdx];
	int	nSubWorldIdx = playerNpc.m_SubWorldIndex;

	if(nSubWorldIdx == -1||
		playerNpc.m_RegionIndex == -1)
		return -1;
	
	KRegion &CurRegion = SubWorld[nSubWorldIdx].m_Region[playerNpc.m_RegionIndex];

	int	nMinDistance;
	int	nNearestEnemyIdx;

	nMinDistance = GetNearestNpcInRegion(CurRegion, nPlayerNpcIdx, nNearestEnemyIdx);

	for(int i = 0; i < 8; ++i)
	{
		int nRegionIdx = CurRegion.m_nConnectRegion[i];
		int	nDistance;
		int	nEnemyIdx;

		if(-1 != nRegionIdx)
		{
			nDistance = GetNearestNpcInRegion(SubWorld[nSubWorldIdx].m_Region[nRegionIdx], 
				nPlayerNpcIdx, nEnemyIdx);

			if(nDistance < nMinDistance)		
			{
				nMinDistance = nDistance;
				nNearestEnemyIdx = nEnemyIdx;
			}
		}
	}

	return nNearestEnemyIdx;
}

int	AutoRobotMgr::GetNearestNpcInRegion(KRegion &curRegion, int nSrcNpcIdx, int &nNpcIdx)
{
	int	nMinDistance = 1 << 30;
	int	nNearestNpcIdx = -1;
	KIndexNode *pNode = (KIndexNode*)curRegion.m_NpcList.GetHead();
	
	while(NULL != pNode)
	{
		int nTargetIdx = pNode->m_nIndex;

		if(IsValidNpc(nTargetIdx) == false)
		{
			pNode = (KIndexNode*)pNode->GetNext();
			continue;
		}
		if ( ( Npc[nTargetIdx].GetCurrentLifePercentage() == 0 && 
			Npc[nTargetIdx].m_Kind == kind_normal) )
		{
			pNode = (KIndexNode*)pNode->GetNext();
			continue;
		}//*/


		pNode = (KIndexNode*)pNode->GetNext();
		if ( Npc[nTargetIdx].m_Level - Npc[nSrcNpcIdx].m_Level >= 20 )
		{
			nTargetIdx = -1;
			continue;
		}
		if(autoAttackFlags&AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER)
		{
			if(Npc[nTargetIdx].m_Level - Npc[nSrcNpcIdx].m_Level>= ((int)(autoAttackOwnerLevel>>16)))
			{
				nTargetIdx = -1;
				continue;
			}
		}
		if(autoAttackFlags&AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER)
		{
			if(Npc[nSrcNpcIdx].m_Level - Npc[nTargetIdx].m_Level>= ((int)(autoAttackOwnerLevel&0x0000ffff)))
			{
				nTargetIdx = -1;
				continue;
			}
		}
		int destX = 0,destY = 0;
		int srcX = 0,srcY = 0;
		Npc[nTargetIdx].GetMpsPos(&destX,&destY);
		Npc[nSrcNpcIdx].GetMpsPos(&srcX,&srcY);
		if(!AutoSimpleCanTravTo(Npc[nTargetIdx].m_SubWorldIndex,destX,destY,srcX,srcY))
		{
			nTargetIdx = -1;
			continue;
		}
		if(CanAttackTarget( nSrcNpcIdx,nTargetIdx) == false)
		{
			nTargetIdx = -1;	
			continue;
		}
		if( (nTargetIdx > 0 && nTargetIdx < MAX_NPC) && !Npc[nTargetIdx].IsPlayer() )
		{
			int	nDistance = NpcSet.GetDistanceSquare(nSrcNpcIdx, nTargetIdx);
			int	nRelation = NpcSet.GetRelation(nSrcNpcIdx, nTargetIdx);

			if( (nDistance <= nMinDistance) && (nRelation & relation_enemy))
			{
				bool canAttack = true;
				for(unsigned int idx = 0; idx < canNotAttackNpcList.size(); idx ++)
				{
					if(canNotAttackNpcList[idx] == Npc[nTargetIdx].m_dwID)
					{
						canAttack = false;
						break;
					}
				}
				if( canAttack )
				{
					nMinDistance   = nDistance;
					nNearestNpcIdx = nTargetIdx;
				}
				else
				{
					nTargetIdx = -1;
					continue;
				}
			}
		}
	}

	nNpcIdx = nNearestNpcIdx;
	if(nNpcIdx != -1)
	{
		//被选择上了
		canNotAttackNpcList.clear();
		canNotAttackNpcList.push_back(Npc[nNpcIdx].m_dwID);
	}
	return nMinDistance;
}
void AutoRobotMgr::LoadAutoUseItemInfo()
{
	if(!medicineForHPList.empty())
	{
		medicineForHPList.clear();
	}

	if (!medicineForMPList.empty())
	{
		medicineForMPList.clear();
	}

	KIniFile  iniFile;
	iniFile.Load(AUTO_INI_FILE_PATH);
	int numbers = 0;
	iniFile.GetInteger("medicine","numbers",0,&numbers);
	char str[256] = {0};
	for(int i = 0; i < numbers; i++)

	{
		sprintf(str,"medicine%d",i);
		AutoItemInfo itemInfo;
		iniFile.GetString(str, "name", "", itemInfo.autoItemName, AUTO_ITEM_NAME_LENGTH);
		iniFile.GetInteger(str, "type", 0, &itemInfo.autoItemTyp);
		iniFile.GetInteger(str, "sort", 0, &itemInfo.autoItemSort);
		iniFile.GetInteger(str, "particularType", 0, &itemInfo.autoItemParticular);
		iniFile.GetInteger(str, "lowType", 0, &itemInfo.autoItemLowTyp);
		int isUseForHP = 1;
		iniFile.GetInteger(str, "isUseForHP", 1, &isUseForHP);
		if (1 == isUseForHP)
		{
			medicineForHPList.push_back(itemInfo);
		}
		else
		{
			medicineForMPList.push_back(itemInfo);
		}
	}
}

void  AutoRobotMgr::AddAutoUseMedicineFlags(unsigned int flags)
{
	if(autoUseMedicineFlags&flags)
		return;
	autoUseMedicineFlags |= flags;
}
	//删除标志//
void  AutoRobotMgr::RemoveAutoUseMedicineFlags(unsigned int flags)
{
	if(!(autoUseMedicineFlags&flags))
		return;
	autoUseMedicineFlags &=(~flags);
}
void  AutoRobotMgr::SetAutoUseMedicineHPPersent(int persent)
{
	autoHPMPPersent&= 0x0000ffff;
	autoHPMPPersent |= ((persent&0xffff)<<16);
}
void  AutoRobotMgr::SetAutoUseMedicineMPPersent(int persent)
{
	autoHPMPPersent &= 0xffff0000;
	autoHPMPPersent |= (persent&0xffff);
}

bool AutoRobotMgr::AutoUseItemGoToSell()
{
	int nIdx = 0,nX = 0,nY = 0;
	MapPosInfo mapInfo;
	Position pos;
	g_ScenePlace.getMapInfoAtPos(&mapInfo,&pos);
	const AutoMapInfo* pCurrentInfo  =  AutoGoBack::Singleton().GetCurrentMapInfo();
	if(pCurrentInfo == 0)
		return false;
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int mapX = Npc[nPlayerNpcIdx].GetMapX();
	int mapY = Npc[nPlayerNpcIdx].GetMapY();
	int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
	int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
	int worldMapX = (reginPixelX>>5)+mapX;
	int worldMapY = (reginPixelY>>5)+mapY;
	const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetCurrentMapInfo();
	if(pMapInfo)
		lastMapIndex = pMapInfo->mapId;
	goBackPosX = worldMapX;
	goBackPosY = worldMapY;
	if(AutoGoBack::Singleton().GotoSellItem()&&AutoGoBack::Singleton().pathList.empty())
	{
		
		AutoRobotMgr::Singleton().AddAutoGoHomeFlags(AUTO_GO_PREPAER);
		AddAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
		return true;
	}
	AutoItemInfo itemInfo = medicineForHPList[gobackItemIdx];
	if(Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSameParticularInEquipment( 
		itemInfo.autoItemTyp, 
		itemInfo.autoItemSort, 
		itemInfo.autoItemParticular,
				&nIdx, &nX, &nY )&&isUsingGoHomeItem == false )
	{

		ItemPos tmpPos, tmpTargetPos;
		tmpPos.nPlace = pos_equiproom;
		tmpPos.nX = nX;
		tmpPos.nY = nY;
		ZeroMemory(&tmpTargetPos, sizeof(ItemPos) );
//		Player[CLIENT_PLAYER_INDEX].ApplyUseItem( nIdx,tmpPos,0,tmpTargetPos);
		isUsingGoHomeItem = true;
		return true;
	}
	return isUsingGoHomeItem;
}
bool AutoRobotMgr::AutoFindBackPath()
{
	MapPosInfo mapInfo;
	Position pos;
	g_ScenePlace.getMapInfoAtPos(&mapInfo,&pos);
	AutoGoBack::Singleton().SetCurrentMapInfo(mapInfo.mapName);
	const AutoMapInfo* pCurrentInfo  =  AutoGoBack::Singleton().GetCurrentMapInfo();
	if(pCurrentInfo == 0)
		return false;
	if(strcmp(pCurrentInfo->mapName,mapInfo.mapName)!=0)
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_ROBOT_AUTOGOBACK_ERROR, 0);
		return false;
	}
	if(AutoGoBack::Singleton().FindComeBackPath(&comebackList,lastMapIndex))
	{
		return true;
	}
	return false;

}
bool AutoRobotMgr::AutoFindHomePath()
{
	AutoGoBack& goBack = AutoGoBack::Singleton();
	if(!(autoAttackFlags&AUTO_ATTACK_FLAG_GO_BACK))
		return false;
	MapPosInfo mapInfo;
	Position pos;
	g_ScenePlace.getMapInfoAtPos(&mapInfo,&pos);
	AutoGoBack::Singleton().SetCurrentMapInfo(mapInfo.mapName);
	const AutoMapInfo* pCurrentInfo  =  AutoGoBack::Singleton().GetCurrentMapInfo();
	if(pCurrentInfo == 0)
		return false;
	if(strcmp(pCurrentInfo->mapName,mapInfo.mapName)!=0)
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_ROBOT_AUTOGOBACK_ERROR, 0);
		return false;
	}
	if(goBack.GotoSellItem())
	{
		int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		int mapX = Npc[nPlayerNpcIdx].GetMapX();
		int mapY = Npc[nPlayerNpcIdx].GetMapY();
		int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
		int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
		int worldMapX = (reginPixelX>>5)+mapX;
		int worldMapY = (reginPixelY>>5)+mapY;
		const AutoMapInfo* pMapInfo = goBack.GetCurrentMapInfo();
		if(pMapInfo)
			lastMapIndex = pMapInfo->mapId;
		goBackPosX = worldMapX;
		goBackPosY = worldMapY;
		AddAutoGoHomeFlags(AUTO_GO_ENABLE);
		AddAutoGoHomeFlags(AUTO_GO_PREPAER);
		AddAutoGoHomeFlags(AUTO_GO_HOME);
		SetMode(enRobotMode_None);
		if(goBack.pathList.empty())
		{
			AddAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
			return true;
		}
		return true;
	}
	return false;
	
}
void AutoRobotMgr::ProcessChangeWorld()
{
	m_AutoPathFinder.StopAutoWalk();
	SetMode(enRobotMode_None);
	if(comeback == true)
	{
		return;
	}
	if( gotoOtherMap == true )
	{
		SetMode( enRobotMode_GotoOtherMap );
		return;
	}
	if(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_ATTACK_MODE)
	{
		SetAutoAttackPos( );
		resetCentrePos = true;
		m_bWorldChanged = true;
		if(AutoRobotMgr::Singleton().GetGoHomeFlags()&AUTO_GO_ENABLE)
		{
			if(isUsingGoHomeItem == true ) 
			{
				AutoRobotMgr::Singleton().AddAutoGoHomeFlags(AUTO_GO_PREPAER);
				AddAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
				isUsingGoHomeItem = false;
				return;
				
			}
			if(!(AutoRobotMgr::Singleton().GetGoHomeFlags()&AUTO_GO_HOME_AT_CAN_SELL_MAP))
			{
				int current = AutoGoBack::Singleton().pathList.front();
				const AutoMapInfo* pCurrentMap = AutoGoBack::Singleton().GetCurrentMapInfo();
				const CarryInfo* pCarryInfo    = AutoGoBack::Singleton().GetCarryInfo(current);
				if(pCarryInfo == 0)
				{
					RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
					return;
				}
				const CarryInfo* pCurrenMapCarry = AutoGoBack::Singleton().GetCarryInfo(pCarryInfo->targetCarry);
				if(pCurrenMapCarry == 0||pCurrentMap==0)
				{
					RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
					return;
				}
				if(pCurrentMap->mapId!=pCurrenMapCarry->currentMapIndex)
				{
					RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
					return;
				}
				AutoGoBack::Singleton().pathList.pop_front();
				AutoRobotMgr::Singleton().AddAutoGoHomeFlags(AUTO_GO_PREPAER);
				if(AutoGoBack::Singleton().pathList.empty())
				{
					AddAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
					return;
				}
			}
			else
			{
				RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
				RemoveAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
				SetMode(enRobotMode_AutoAttack);
				return;
			}
		}
		else
		{
			SetMode( enRobotMode_AutoAttack );
		}
	}
}

void AutoRobotMgr::SetNotPickupList(deque<ItemClassInfo> * infoList)
{
	if (infoList == NULL)
	{
		return;
	}

	if (infoList->empty())
	{
		m_noPickupColorList.clear();
		for (int i = 0; i < m_noPickupList.size(); ++i)
		{
			m_noPickupList[i].SetAllItemPickup(true);
		}
		return;
	}

	KIniFile iniFile;
	if (!iniFile.Load(AUTO_INI_FILE_PATH))
	{
		return;
	}

	for (int i = 0; i < m_noPickupList.size(); i++)
	{
		m_noPickupList[i].SetAllItemPickup(true);
	}

	m_noPickupColorList.clear();

	for (i = 0; i < infoList->size(); i++)
	{
		ItemClassInfo & info = infoList[0][i];
		if (infoList[0][i].m_isColor)
		{
			int colorID = -1;
			iniFile.GetInteger(infoList[0][i].m_className, "colorID", -1, &colorID);
			if (colorID != -1)
			{
				m_noPickupColorList.push_back(colorID);
			}
			continue;
		}

		for (int j = 0; j < m_noPickupList.size(); j++)
		{
			AutoItemInfoGroup &itemGroup = m_noPickupList[j];
			if (itemGroup == infoList[0][i].m_className)
			{
				itemGroup.SetAllItemPickup(false);
			}
		}
	}
}

void AutoRobotMgr::AutoGoActive()
{
//	AddAutoGoHomeFlags(AUTO_GO_PREPAER);
//	canNotPickIndex.clear();
	if(runForSell == true)
		return;
	if(comeback == true)
	{
		DoComeBack();
		return;
	}
	ProcessPrepareGoToSell();
}
void AutoRobotMgr::ProcessAutoSellItems()
{
	KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
	bool isOver= true;
	for(int i = 0; i < MAX_PLAYER_ITEM ; i++)
	{
		PlayerItem& item = itemList.m_Items[i];
		if(item.nPlace == pos_equiproom)
		{
			int itemIdx = item.nIdx;
			if(Item[itemIdx].GetGenre() == item_equip)
			{
				ITEM_QUALITY_LABEL quelity = Item[itemIdx].GetQualityLabel();
				if(quelity < 2)
				{
					isOver = false;
					SendClientCmdSell(Item[itemIdx].GetID());
				}
			}

		}
	}
	if(isOver)
	{
		lastGo = false;
		RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
		RemoveAutoGoHomeFlags(AUTO_GO_HOME);
	}
}

void AutoRobotMgr::DoComeBack()
{
	//自动返回//
	if(comeback == false)
		return;
	if( comebackList.empty() )
	{
		AutoMapInfo* pInfo = AutoGoBack::Singleton().GetCurrentMapInfo();
		if( pInfo == NULL )
		{
			RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
			RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
			SetMode(enRobotMode_None);
			comeback = false;
			return;
		}
		if(pInfo->mapId != lastMapIndex )
		{
			RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
			RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
			SetMode(enRobotMode_None);
			comeback = false;
			return;
		}
		else
		{
			int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			int mapX = Npc[nPlayerNpcIdx].GetMapX();
			int mapY = Npc[nPlayerNpcIdx].GetMapY();
			int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
			int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
			int worldMapX = (reginPixelX>>5)+mapX;
			int worldMapY = (reginPixelY>>5)+mapY;
			if(
				(worldMapX>=goBackPosX-2&&worldMapX<=goBackPosX+2)&&
				(worldMapY>=goBackPosY - 2&&worldMapY<=goBackPosY+2)
			)
			{
				//已经到达目的。
				RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
			    RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
				SetMode(enRobotMode_None);
				comeback = false;
				return;
			}
		}
		if(!AutoRunToForSell(goBackPosX,goBackPosY))
		{
			comeback = false;
			RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
			RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
			SetMode(enRobotMode_None);
			return;
		}

	}
	else
	{
		int carryIndex = comebackList.front();
		const CarryInfo* pCarryInfo = AutoGoBack::Singleton().GetCarryInfo(carryIndex);
		if( pCarryInfo == NULL )
		{
			RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
			RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
			SetMode(enRobotMode_None);
			comeback = false;
			return;
		}
		comebackList.pop_front( );
		if( !AutoRunToForSell( pCarryInfo->carryPosX,(pCarryInfo->carryPosY)<<1 ) )
		{
			RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
			RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
			SetMode(enRobotMode_None);
			comeback = false;
			return;
		}
			
		
	}

}
void AutoRobotMgr::ProcessPrepareGoToSell()
{
	if(autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
	{
		if(autoAttackFlags&AUTO_ATTACK_FLAG_GO_BACK)
		{
			if(autoGoHomeFlags&AUTO_GO_ENABLE)
			{
				if(autoGoHomeFlags&AUTO_GO_PREPAER)
				{
					if(isUsingGoHomeItem == true)
						return;
					RemoveAutoGoHomeFlags(AUTO_GO_PREPAER);
					if(autoGoHomeFlags&AUTO_GO_HOME_AT_CAN_SELL_MAP)
					{
						AutoMapInfo* pInfo = AutoGoBack::Singleton().GetCurrentMapInfo();
						if(pInfo==0)
							return;
						int idx = pInfo->isHaveTypeNpc(npcTyp_Sell_Medicne);
						if(idx == -1)
						{
							SetMode(enRobotMode_None);
							CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_ROBOT_AUTOGOBACK_ERROR, 0);
							return;
						}
						/*
						回去卖装备肯定要打开对话框
						*/
						/////////////////////////
						MapNpcInfo& npc = pInfo->npcList[idx];
/*						int npcIdx = GetSellNpcPosByName(npc.npcName);
						if(npcIdx == -1)
						{
								SetMode(enRobotMode_None);
								return;
						}
						//寻
						AutoDialogNpc::getSingleton().setTargetNpc(Npc[npcIdx].GetTemplate()->m_NpcSettingIdx);*/
						if(!AutoRunToForSell(npc.npcPosX,npc.npcPosY<<1))
							AddAutoGoHomeFlags(AUTO_GO_PREPAER);
						else
						{
							runForSell = true;
						}
						lastGo = TRUE;
					}
					else
					{
						if(AutoGoBack::Singleton().pathList.empty())
						{
							RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
							return;
						}
						int carryIndex = AutoGoBack::Singleton().pathList.front();
						const CarryInfo* pCarryInfo = AutoGoBack::Singleton().GetCarryInfo(carryIndex);
						if(pCarryInfo==0)
						{
							RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
							return;
						}
						if(!AutoRunToForSell(pCarryInfo->carryPosX,pCarryInfo->carryPosY<<1))
							AddAutoGoHomeFlags(AUTO_GO_PREPAER);
					}
				}
				else
				{
					RemoveAutoGoHomeFlags(AUTO_GO_ENABLE);
					RemoveAutoGoHomeFlags(AUTO_GO_HOME_AT_CAN_SELL_MAP);
					RemoveAutoGoHomeFlags(AUTO_GO_HOME);
					return ;
				}
			}
			else
			{
				if((autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE)
					)
				{
					if(StandHere())
						return;
					else
						SetMode(enRobotMode_AutoAttack);
				}
			}
		}
		else
		{
			if((autoEntrustComputerMode&ENSTRUST_AUTO_ATTACK_MODE))
			{
				if(StandHere())
					return;
				else
					SetMode(enRobotMode_AutoAttack);
				return;
			}
		}
	}
	else
	{
		SetMode(enRobotMode_None);
	}
}

//判断一个NPC是否能被自动打怪攻击//
bool 
AutoRobotMgr::CanAttackTarget( int nIdx,int targetIdx )
{
	if(!IsValidNpc(nIdx)||
		!IsValidNpc(targetIdx))
		return false;

	if (Npc[targetIdx].IsEmployee())
		return false;

	if(NpcSet.GetRelation(nIdx,targetIdx) != relation_enemy)
	{
		return false;
	}
	unsigned int targetIdx_ID = g_FileName2Id( Npc[targetIdx].Name );
	unsigned int  size = canNotAutoAttackNpcList.size();
	///在不能攻击列表里面查找npc//
	for( unsigned int idx = 0; idx < size; idx++ )
	{
		if(canNotAutoAttackNpcList[idx] == targetIdx_ID&&
			Npc[targetIdx].m_Kind != kind_player )
		{
			//这个表记录的是怪物而非npc所以玩家可以随便攻击，只要能攻击
			return false;
		}
	}

	return true;

}

bool AutoRobotMgr::GenNotPickupList(KIniFile &iniFile)
{
	KTabFile tabIniFile;
	if (!tabIniFile.Load(AUTO_TAB_INI_FILE_PATH))
	{
		return false;
	}
	
	int height = tabIniFile.GetHeight() - 1;
	char section[COMMON_CLIENT_MSG_LEN_32];
	size_t sectionLength = sizeof(section);

	char itemInfoSection[COMMON_CLIENT_MSG_LEN_64];
	size_t infoSectionLen = sizeof(itemInfoSection);

	for (int i = 0; i < height; i++)
	{
		memset(section, 0, sectionLength);
		
		tabIniFile.GetString(i + 2, 5, "", section, sectionLength);
		if (section[0] == 0)
		{
			continue;
		}
		
		int isColor = -1;
		int isGroup = -1;
		tabIniFile.GetInteger(i + 2, 3, -1, &isGroup);
		tabIniFile.GetInteger(i + 2, 4, -1, &isColor);
		
		if (isColor == -1
			|| isColor == 1
			|| isGroup == -1)
		{
			continue;
		}

		if (isGroup == 1)
		{
			int itemNum = 0;
			iniFile.GetInteger(section, "numbers", 0, &itemNum);
			if (itemNum == 0)
			{
				continue;
			}

			AutoItemInfoGroup itemInfoGroup;
			itemInfoGroup.SetGroupName(section, sectionLength);
	
			for (int j = 0; j < itemNum; j++)
			{
				memset(itemInfoSection, 0, infoSectionLen);
				sprintf(itemInfoSection, "%s%d", section, j);

				AutoItemInfo itemInfo;
				if (!GetItemInfo(iniFile, itemInfoSection, itemInfo))
				{
					continue;
				}
				itemInfoGroup.AddItem(itemInfo);
			}

			if (itemInfoGroup.IsEmpty())
			{
				continue;
			}

			m_noPickupList.push_back(itemInfoGroup);
		}
		else if (isGroup == 0)
		{
			AutoItemInfoGroup itemInfoGroup;
			itemInfoGroup.SetGroupName(section, sectionLength);

			AutoItemInfo itemInfo;
			if (!GetItemInfo(iniFile, section, itemInfo))
			{
				continue;
			}

			itemInfoGroup.AddItem(itemInfo);
			m_noPickupList.push_back(itemInfoGroup);
		}
	}
	return true;
}

////
AutoRobotMgr::AutoRobotMgr()
{
	m_RobotMode = enRobotMode_None;
	autoAttackFlags = AUTO_ATTACK_FLAG_NORNAL;
	autoAttackOwnerLevel  = 0;
	autoEntrustComputerMode = ENSTRUST_NORMAL_MODE;
	autoPickupItemFlags = AUTO_PICKUP_ITEM_ALL;
	LoadAutoUseItemInfo();
	autoUseMedicineFlags = AUTO_USE_HP_MEDICINE_NORMAL;
	autoHPMPPersent = 20;
	autoGoHomeFlags = 0;
	lastMapIndex = 0;
	goBackPosX = 0;
	goBackPosY = 0;
	lastGo = false;
	m_bWorldChanged = false;
	isUsingGoHomeItem = false;
	KIniFile iniFile;
	iniFile.Load(AUTO_INI_FILE_PATH);
	int numbers = 0;
	iniFile.GetInteger("canNotAttackNpc","numbers",0,&numbers);
	char npcName[256] = {0};
	char keyName[32] = {0};
	for(int idx = 0; idx < numbers; idx++)
	{
		sprintf(keyName,"npc%d",idx);
		iniFile.GetString("canNotAttackNpc",keyName,"",npcName,256);
		unsigned int id = g_FileName2Id(npcName);
		canNotAutoAttackNpcList.push_back(id);
	}
	iniFile.GetInteger("autoAttackDistance","runDistance",32,&randRunDistance);
	iniFile.GetInteger( "autoAttackDistance","AttackDistance",80,&attackRadius );
	pickMedicine = true;

	runForSell = false;
	comeback   =  false;
	iniFile.GetInteger("goback","idx",0,&gobackItemIdx);
	gotoOtherMap = false;
	autoGotoFindNpc = false;
	lastX = 0;
	lastY = 0;
//	randRunDistance = 32;
//	attackRadios = 80;

	m_iBlastExp = 0;

	m_bIsAutoBlast = false;

	GenNotPickupList(iniFile);

	m_bIsAutoRepair = false;
	////////获取不能攻击的Npc名字，再把名字转换成ID///////////
}

int AutoRobotMgr::GoToOTher(MapPosInfo* pMapInfo)
{
	if( pMapInfo == NULL)
	{
		return 0;
	}
	const AutoMapInfo* pDestMap = AutoGoBack::Singleton().GetMapInfo(pMapInfo->mapName);
	const AutoMapInfo* pCurrentMap = AutoGoBack::Singleton().GetCurrentMapInfo();
	if( pDestMap == NULL ||
		pCurrentMap == NULL )
		return 0;
	findMapWay.clear();
	if( !AutoGoBack::Singleton().FindComeBackPath( &findMapWay, pDestMap->mapId ) )
		return 0;
	gotoOtherMap = true;

	destMapPos.x = pMapInfo->pos.x;
	destMapPos.y = pMapInfo->pos.y;
	
	SetMode( enRobotMode_GotoOtherMap );
	return 1;
	
}
void AutoRobotMgr::DoGoToOtherMap()
{
	if( findMapWay.empty( ) )
	{
		//表示已经到达当前地图了
		gotoOtherMap = false;
		if( !AutoRunToForSell( destMapPos.x, destMapPos.y << 1 ) )
		{
			SetMode( enRobotMode_None );
		}	
		return;
	}

	int carryIndex = findMapWay.front();
	const CarryInfo* pCarryInfo = AutoGoBack::Singleton().GetCarryInfo( carryIndex );
	findMapWay.pop_front();
	if( pCarryInfo == NULL )
	{
		SetMode( enRobotMode_None );
		return;
	}

	if( !AutoRunToForSell( pCarryInfo->carryPosX, pCarryInfo->carryPosY <<1 ) )
	{
		SetMode( enRobotMode_None );
		return ;
	}



}

//判断一个时间段内NPC的血量是不是在减少
#define MAX_CHECK_NPC_TIME   5000
bool AutoRobotMgr::CanAttackNpc(int npcIdx )
{
	static DWORD lastTime = GetTickCount();
	static DWORD lastNpcId = 0;
	static int lastNpcLifePersent = -1;
	if(IsValidNpc( npcIdx ) == false|| npcIdx == -1 )
	{
		lastNpcId = 0;
		lastNpcLifePersent = -1;
		return false;
	}
	if( lastNpcId == 0 ||
		lastNpcId != Npc[npcIdx].GetId() )
	{
		lastNpcLifePersent = Npc[npcIdx].GetCurrentLifePercentage();
		lastNpcId = Npc[npcIdx].GetId();
		lastTime = GetTickCount(); 
		return true;
	}
	DWORD nowTime = GetTickCount();
	DWORD dTime = nowTime - lastTime;
	if( dTime < MAX_CHECK_NPC_TIME )
		return true;
	int currentNpcLife = Npc[npcIdx].GetCurrentLifePercentage();
	if(lastNpcLifePersent != currentNpcLife )//血量在减少
	{
		lastNpcLifePersent = currentNpcLife;
		lastTime = GetTickCount();
		return true;
	}
//	canNotAttackNpcList.push_back(Npc[npcIdx].GetId());
	lastTime = GetTickCount();
	lastNpcId = 0;
	lastNpcLifePersent = -1;
	return false;

}
bool    AutoRobotMgr::StandHere()
{
	int nPlayerIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if(IsValidNpc( nPlayerIdx ) == false)
		return true;
	if( Npc[nPlayerIdx].IsInSafeArea())
		return true;
	int targetNpc = Npc[nPlayerIdx].GetTargetNpc();
	if(IsValidNpc(targetNpc) == false)
	{
		return false;
	}
	if( NpcSet.GetRelation(nPlayerIdx,targetNpc)&relation_enemy)
	{
		//敌人
		return false;
	}
	return true;
}

void 
AutoRobotMgr::SetAutoAttackPos( )
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if( IsValidNpc( nPlayerNpcIdx )  == false )
	{
		return;
	}
	int mapX = Npc[nPlayerNpcIdx].GetMapX();
	int mapY = Npc[nPlayerNpcIdx].GetMapY();
	if( Npc[nPlayerNpcIdx].m_RegionIndex == -1 || Npc[nPlayerNpcIdx].m_SubWorldIndex == -1 )
	{
		return ;
	}
	int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
	int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
	centreX = (reginPixelX>>5)+mapX;
	centreY = (reginPixelY>>5)+mapY;
	centreY>>=1;
}

bool   
AutoRobotMgr:: GotoSartPos( )
{
	int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if( IsValidNpc( nPlayerNpcIdx ) == false )
	{
		return false;
	}
	int mapX = Npc[nPlayerNpcIdx].GetMapX();
	int mapY = Npc[nPlayerNpcIdx].GetMapY();
	
	if( Npc[nPlayerNpcIdx].m_RegionIndex  == -1 ||
		Npc[nPlayerNpcIdx].m_SubWorldIndex == -1 )
	{
		return  false;
	}
	if( m_bWorldChanged == true )
	{
		m_bWorldChanged = false;
		SetAutoAttackPos( );
	}

	
	int reginPixelX = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionX;
	int reginPixelY = SubWorld[Npc[nPlayerNpcIdx].m_SubWorldIndex].m_Region[Npc[nPlayerNpcIdx].m_RegionIndex].m_nRegionY;
	int currentX = (reginPixelX>>5)+mapX;
	int currentY = (reginPixelY>>5)+mapY;
	currentY >>=1;
	if( abs( currentY - centreY ) > attackRadius ||
		abs( currentX - centreX ) > attackRadius )
	{
		Npc[nPlayerNpcIdx].SetTarget( type_npc,0 );
		if( m_AutoPathFinder.GoTo( centreX,centreY<<1))
		{
			SetMode( enRobotMode_AutoRun ) ;
			AddAutoAttackFlags(AUTO_ATTACK_FLAG_RANDOM_GO);
			autoGotoFindNpc = true;
			return true;
		}
		else
		{
			autoGotoFindNpc = false;
			return false;
		}
	}
	return false;
}


void AutoRobotMgr::SetBlastExp(int nExp)
{
	m_iBlastExp = nExp;
}

void AutoRobotMgr::AutoBlast()
{
	if (m_iBlastExp >= 100 && m_bIsAutoBlast &&
		(autoEntrustComputerMode & ENSTRUST_AUTO_ATTACK_MODE))
	{
		CoreDataChanged(GDCNI_AUTO_ATTACK_BLAST, 0, 0);
	}
}

void AutoRobotMgr::SetAutoBlastFlag(bool bFlag)
{
	m_bIsAutoBlast = bFlag;
}

void AutoRobotMgr::SetAutoRepairFlag(bool bFlag)
{
	m_bIsAutoRepair = bFlag;
}

inline bool AutoRobotMgr::IsInPickupList(KObj & object, bool & isPickup)
{
	if (object.m_nKind != Obj_Kind_Item)
	{
		isPickup = false;
		return false;
	}

	for (int i = 0; i < m_noPickupList.size(); i++)
	{
		if (m_noPickupList[i].IsInPickup(object.m_szName, isPickup))
		{
			return true;
		}
	}

	isPickup = false;
	return false;
}

inline bool AutoRobotMgr::IsInNotPickupColorList(KObj & object)
{
	//对某些物品做特例，如药品为白色，但玩家不拾取药品，要拾取白装
	for (int i = 0; i < m_noPickupColorList.size(); i++)
	{
		int colorID = m_noPickupColorList[i];
		if (object.m_nColorID == colorID)
		{
			return true;
		}
	}
	return false;
}

void AutoRobotMgr::SetAutoCastSkill(int index, AutoCastSkillInfo & skillInfo)
{
	if (index >= SkillCount || index < 0)
	{
		return;
	}

	if (AttackSkill == index)
	{
		if (g_pCoreShell != NULL && skillInfo.m_SkillID >= 0 && skillInfo.m_SkillID != RUN_SKILL_ID && skillInfo.m_IsSelected)
		{
			g_pCoreShell->SelectSkill(skillInfo.m_SkillID);
		}
	}

	memcpy(&m_AutoCastSkillList[index], &skillInfo, sizeof(AutoCastSkillInfo));
}

void AutoRobotMgr::AutoCastSkill()
{
	//每隔一秒才施放一个技能以限制向服务器发包的速率
	static int gameLoop = 0;
	if (gameLoop > 0)
	{
		gameLoop++;
		gameLoop = gameLoop % (GAME_FPS + GAME_FPS / 2);
		return;
	}

	static int skillIndex = 1;

	if (skillIndex == 0)
	{
		skillIndex++;
	}

	if (!m_AutoCastSkillList[skillIndex].m_IsSelected
		|| m_AutoCastSkillList[skillIndex].m_SkillID == INVALID_SKILL_ID)
	{
		gameLoop++;
		gameLoop = gameLoop % (GAME_FPS + GAME_FPS / 2);
		skillIndex++;
		skillIndex = skillIndex % SkillCount;

		return;
	}

	if (Npc == NULL)
	{
		gameLoop++;
		gameLoop = gameLoop % (GAME_FPS + GAME_FPS / 2);
		return;
	}

	KNpc & player = Npc[GetClientPlayer().GetNpcIndex()];
	NpcSkillList & skillList = player.GetSkillList();

	int availableSkillID = m_AutoCastSkillList[skillIndex].m_SkillID;
	if ( availableSkillID != KNIGHT_NORMALSKILL_ID && 
		availableSkillID != ENCHANTER_NORMALSKILL_ID &&
		availableSkillID != MONSTROUS_NORMAILSKILL_ID )
	{
		availableSkillID = skillList.GetCurSameSubSkillId(availableSkillID);
	}
	bool canCast = skillList.CanCast(availableSkillID);
	bool isSelectedTarget = true;

	if (m_AutoCastSkillList[skillIndex].m_lastCastTime + m_AutoCastSkillList[skillIndex].m_CastInterval < GetTickCount() && canCast)
	{
		//考虑到网络延迟，所以时间+0.5秒
		if (skillIndex == SkillCount - 1)
		{
			if (!SelectPet())
			{
				isSelectedTarget = false;
			}
			else
			{
				isSelectedTarget = true;
			}
		}
		else
		{
			if (m_AutoCastSkillList[skillIndex].m_SkillID <= 685 && m_AutoCastSkillList[skillIndex].m_SkillID >= 651)
			{
				if (!SelectPlayerself())
				{
					isSelectedTarget = false;
				}
				else
				{
					isSelectedTarget = true;
				}
			}
		}

		int targetIdx = player.GetTargetNpc();
		int targetPosX = 0;
		int targetPosY = 0;
		if (isSelectedTarget)
		{
			Npc[targetIdx].GetMpsPos(&targetPosX, &targetPosY);
			if (SkillCount - 1 != skillIndex && (m_AutoCastSkillList[skillIndex].m_SkillID <= 685 && m_AutoCastSkillList[skillIndex].m_SkillID >= 651))
			{
				CastSkill(m_AutoCastSkillList[skillIndex].m_SkillID, targetPosX, targetPosY, true);
			}
			else
			{
				CastSkill(m_AutoCastSkillList[skillIndex].m_SkillID, targetPosX, targetPosY);
			}
			m_AutoCastSkillList[skillIndex].m_lastCastTime = GetTickCount() + 500;
			if (skillIndex == SkillCount - 1 || (m_AutoCastSkillList[skillIndex].m_SkillID <= 685 && m_AutoCastSkillList[skillIndex].m_SkillID >= 651))
			{
				ResetTargetNpc();
			}
		}
	}
	gameLoop++;
	gameLoop = gameLoop % (GAME_FPS + GAME_FPS / 2);
	skillIndex++;
	skillIndex = skillIndex % SkillCount;
}

void AutoRobotMgr::CastSkill(int skillId, int posX, int posY, bool targetSelf /* = false */)
{
	if (Npc == NULL || g_pCoreShell == NULL)
	{
		return;
	}

	if ( skillId != KNIGHT_NORMALSKILL_ID && 
		skillId != ENCHANTER_NORMALSKILL_ID &&
		skillId != MONSTROUS_NORMAILSKILL_ID )
	{
		skillId = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(skillId);
	}
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	PlayerController& controller = PlayerController::Singleton();
	
	if(skillId > 0)
	{
		KSkill * pISkill =  g_SkillManager.GetSkill(skillId, 1);
		
		if (!pISkill) 
			return;
		
		int nTargetIdx = Npc[nIndex].GetTargetNpc();
		int nAttackTargetType = pISkill->GetAttackTargetType();
		
		if( (nAttackTargetType & att_target_only) && !(nAttackTargetType & att_target_self) && nTargetIdx <= 0 )
		{
			int nNPCKind = 0;
			KUiPlayerItem SelectPlayer;
			if ( g_pCoreShell->FindSelectNPC(posX, posY, relation_all, true, &SelectPlayer, nNPCKind, true) <= 0 )
			{
				return;
			}					
		}
/*		if(!(nAttackTargetType & att_target_self))
		{
			AutoRobotMgr::Singleton().SetMode(enRobotMode_None);
		}*/
		controller.SetNextSkill(skillId, posX, posY, targetSelf);
	}
	else
		controller.SetNextSkill(INVALID_SKILL_ID, 0, 0, false);
}

bool AutoRobotMgr::SelectPet()
{
	KPlayer & player = GetClientPlayer();
	KNpc * pet = player.m_Creature.GetCreatureNpc();
	if (pet != NULL)
	{
		if (pet->m_Index == 0)
		{
			return false;
		}	
		Npc[player.GetNpcIndex()].SetTarget(type_npc, pet->m_Index);
		return true;
	}
	return false;
}

bool AutoRobotMgr::SelectPlayerself()
{
	if (Npc == NULL)
	{
		return false;
	}

	KPlayer & player = GetClientPlayer();
	Npc[player.GetNpcIndex()].SetTarget(type_npc, player.GetNpcIndex());

	return true;
}

int AutoRobotMgr::FindNextAvailableSkill(int & skillIndex)
{
	static int lastIndex = 1;
	int curIndex = lastIndex;
	curIndex = curIndex % SkillCount;

	if (curIndex == 0)
	{
		++curIndex;
	}

	KNpc & player = Npc[GetClientPlayer().GetNpcIndex()];
	NpcSkillList & skillList = player.GetSkillList();

	do
	{
		if (!m_AutoCastSkillList[curIndex].m_IsSelected
			|| m_AutoCastSkillList[curIndex].m_SkillID == INVALID_SKILL_ID)
		{
			++curIndex;
			curIndex = curIndex % SkillCount;
			if (curIndex == 0)
			{
				++curIndex;
			}
			continue;
		}

		int availableSkillID = m_AutoCastSkillList[curIndex].m_SkillID;
		if ( availableSkillID != KNIGHT_NORMALSKILL_ID && 
			availableSkillID != ENCHANTER_NORMALSKILL_ID &&
			availableSkillID != MONSTROUS_NORMAILSKILL_ID )
		{
			availableSkillID = skillList.GetCurSameSubSkillId(availableSkillID);
		}
		bool canCast = skillList.CanCast(availableSkillID);

		if (m_AutoCastSkillList[curIndex].m_lastCastTime + m_AutoCastSkillList[curIndex].m_CastInterval < GetTickCount() && canCast)
		{
			lastIndex = curIndex;
			skillIndex = curIndex;
			return m_AutoCastSkillList[curIndex].m_SkillID;
		}

		++curIndex;
		curIndex = curIndex % SkillCount;
		if (curIndex == 0)
		{
			++curIndex;
		}
	}while (curIndex != lastIndex);

	++lastIndex;
	lastIndex = lastIndex % SkillCount;
	if (lastIndex == 0)
	{
		++lastIndex;
	}
	skillIndex = lastIndex;
	return INVALID_SKILL_ID;
}

void AutoRobotMgr::TestAutoCastSkill()
{
	static int findLoop = 0;
	static int castLoop = 0;

	static int skillId = INVALID_SKILL_ID;
	static int skillIndex = 0;

	if (findLoop == 0)
	{
		if (skillId == INVALID_SKILL_ID)
		{
			skillId = FindNextAvailableSkill(skillIndex);
		}
	}

	if (castLoop == 0)
	{
		if (skillId != INVALID_SKILL_ID)
		{
			bool isSelectedTarget = true;
			if (skillIndex == ShouSkill)
			{
				if (!SelectPet())
				{
					isSelectedTarget = false;
				}
				else
				{
					isSelectedTarget = true;
				}
			}
			else
			{
				if (m_AutoCastSkillList[skillIndex].m_SkillID <= 685 && m_AutoCastSkillList[skillIndex].m_SkillID >= 651)
				{
					if (!SelectPlayerself())
					{
						isSelectedTarget = false;
					}
					else
					{
						isSelectedTarget = true;
					}
				}
			}
			KNpc & player = Npc[GetClientPlayer().GetNpcIndex()];
			NpcSkillList & skillList = player.GetSkillList();

			int targetIdx = player.GetTargetNpc();
			int targetPosX = 0;
			int targetPosY = 0;
			if (isSelectedTarget)
			{
				Npc[targetIdx].GetMpsPos(&targetPosX, &targetPosY);
				if (SkillCount - 1 != skillIndex && (m_AutoCastSkillList[skillIndex].m_SkillID <= 685 && m_AutoCastSkillList[skillIndex].m_SkillID >= 651))
				{
					CastSkill(m_AutoCastSkillList[skillIndex].m_SkillID, targetPosX, targetPosY, true);
				}
				else
				{
					CastSkill(m_AutoCastSkillList[skillIndex].m_SkillID, targetPosX, targetPosY);
				}
			}
			m_AutoCastSkillList[skillIndex].m_lastCastTime = GetTickCount() + 500;
			skillId = INVALID_SKILL_ID;
		}
	}

	++findLoop;
	++castLoop;
	findLoop = findLoop % GAME_FPS;
	castLoop = castLoop % (GAME_FPS + GAME_FPS / 2);
}

void AutoRobotMgr::BackupAttackSkill()
{
	if (g_pCoreShell == NULL)
	{
		return;
	}

	g_pCoreShell->SelectSkill(g_pCoreShell->GetGameData(GDI_LEFT_ENABLE_SKILLS, 0, 0));
}

#endif	// #ifdef _AUTO_ROBOT