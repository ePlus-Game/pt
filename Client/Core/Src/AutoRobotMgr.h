//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:9   10:48
//      File_base        : AutoRobotMgr
//      File_ext         : h
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

#ifndef _AutoRobotMgr_h
#define _AutoRobotMgr_h
#ifdef	_AUTO_ROBOT

#include "RobotSkillPolicy.h"
#include "AutoPathFinder.h"
#include "AutoRobotComDef.h"
#include "AutoTrust.h"
#include <list>
#include <deque>
#include "KObj.h"
using std::list;

class KRegion;

#define TIME_ATTACK_TIME 10

struct AutoItemInfo
{
	char autoItemName[AUTO_ITEM_NAME_LENGTH];
	int  autoItemTyp;
	int  autoItemSort;
	int  autoItemParticular;
	int  autoItemLowTyp;
	bool autoItemIsPickup;

	AutoItemInfo()
	{
		memset(autoItemName,0,AUTO_ITEM_NAME_LENGTH);
		autoItemTyp = 0;
		autoItemSort = 0;
		autoItemParticular = 0;
		autoItemLowTyp = 0;
		autoItemIsPickup = false;
	}
	AutoItemInfo(const AutoItemInfo& itemInfo)
	{
		strcpy(autoItemName,itemInfo.autoItemName);
		autoItemTyp = itemInfo.autoItemTyp;
		autoItemSort = itemInfo.autoItemSort;
		autoItemParticular = itemInfo.autoItemParticular;
		autoItemLowTyp = itemInfo.autoItemLowTyp;
		autoItemIsPickup = itemInfo.autoItemIsPickup;
	}
	AutoItemInfo& operator = ( const AutoItemInfo& itemInfo)
	{
		strcpy(autoItemName,itemInfo.autoItemName);
		autoItemTyp = itemInfo.autoItemTyp;
		autoItemSort = itemInfo.autoItemSort;
		autoItemParticular = itemInfo.autoItemParticular;
		autoItemLowTyp = itemInfo.autoItemLowTyp;
		autoItemIsPickup = itemInfo.autoItemIsPickup;

		return *this;
	}
};

struct AutoItemInfoGroup
{
	deque<AutoItemInfo> m_ItemList;
	char m_GroupName[COMMON_CLIENT_MSG_LEN_32];
	size_t m_NameLength;

	AutoItemInfoGroup()
	{
		memset(m_GroupName, 0, sizeof(m_GroupName));
		m_NameLength = sizeof(m_GroupName);
	}

	void AddItem(AutoItemInfo & itemInfo)
	{
		m_ItemList.push_back(itemInfo);
	}

	void SetPickup(int idx, bool isPickup)
	{
		m_ItemList[idx].autoItemIsPickup = isPickup;
	}

	void SetAllItemPickup(bool isPickup)
	{
		for (int i = 0; i < m_ItemList.size(); i++)
		{
			m_ItemList[i].autoItemIsPickup = isPickup;
		}
	}

	bool IsEmpty()
	{
		return m_ItemList.empty();
	}

	void SetGroupName(char * name, int nameLength)
	{
		size_t length = 0;

		if (m_NameLength < nameLength)
		{
			length = m_NameLength;
		}
		else
		{
			length = nameLength;
		}

		strncpy(m_GroupName, name, length);
		m_GroupName[m_NameLength - 1] = 0;
	}

	bool IsInPickup(char * name, bool &isPickup)
	{
		size_t itemNameLength = 0;
		for (int i = 0; i < m_ItemList.size(); i++)
		{
			itemNameLength = strlen(m_ItemList[i].autoItemName);
			if (strncmp(m_ItemList[i].autoItemName, name, itemNameLength) == 0)
			{
				isPickup = m_ItemList[i].autoItemIsPickup;
				return true;
			}
		}
		isPickup = false;
		return false;
	}

	bool operator == (char * name)
	{
		if (strcmp(m_GroupName, name) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

enum SkillIndex
{
	AttackSkill = 0,
	Skill0,
	Skill1,
	Skill2,
	Skill3,
	Skill4,
	ShouSkill,
	SkillCount,
};

enum MapNpcType
{
		npcTyp_None=0,
		npcTyp_Sell_Medicne,
		npcTyp_Sell_Eq,
		npcTyp_Sell_Weapon,
};
struct MapNpcInfo
{
	MapNpcType type;
	int npcMapId;
	char npcName[AUTO_ITEM_NAME_LENGTH];
	int  npcPosX;
	int npcPosY;
	MapNpcInfo::MapNpcInfo()
	{
		type = npcTyp_None;
		npcMapId = 0;
		memset(npcName,0,AUTO_ITEM_NAME_LENGTH);
		npcPosX = 0;
		npcPosY = 0;
	}
	MapNpcInfo::MapNpcInfo(const MapNpcInfo& npc)
	{
		type = npc.type;
		npcMapId = npc.npcMapId;
		strcpy(npcName,npc.npcName);
		npcPosX = npc.npcPosX;
		npcPosY = npc.npcPosY;
	}
	const MapNpcInfo& operator =(const MapNpcInfo& npc)
	{
		type = npc.type;
		npcMapId = npc.npcMapId;
		strcpy(npcName,npc.npcName);
		npcPosX = npc.npcPosX;
		npcPosY = npc.npcPosY;
		return *this;
	}
};
struct AutoMapInfo
{
	int isHaveTypeNpc(MapNpcType type);
	AutoMapInfo();
	~AutoMapInfo();
	int mapId;//地图ID
	int carryNumbers;//传送点数目
	char mapName[AUTO_ITEM_NAME_LENGTH];//传送点名字
	vector<int> carryIndexList;//传送点索引
	int levelLimited ;//
	///////////////
	vector<MapNpcInfo> npcList;
};
struct CarryInfo
{
	CarryInfo();
	~CarryInfo();
	int carryID;
	char carryName[AUTO_ITEM_NAME_LENGTH];
	int levelLimited;
	int targetCarry;
	int currentMapIndex;
	int carryType;
	int carryPosX;
	int carryPosY;
};

class AutoGoBack
{
public:
	AutoGoBack();
	~AutoGoBack();
    AutoMapInfo* GetCurrentMapInfo();
    AutoMapInfo*       GetMapInfo(int id);
	const CarryInfo*   GetCarryInfo(int id);
	const AutoMapInfo* GetMapInfo( const char* mapName) const;
	void               SetCurrentMapInfo(int id);
	void               SetCurrentMapInfo(const char* name);
	int                GetCurrentMapIndex() const { return currentMapID;}
	static AutoGoBack& Singleton();
	bool               GotoTargetMap(int mapId);
	bool               GotoSellItem();
	bool               FindComeBackPath(list<int>* pathList,int mapId);
	void               CreateComeBackPath(list<int>* pathList);
	const vector<MapNpcInfo>* GetMapNpcListByName(const char* mapName);
private:
	struct pathInfo{
		int mapID;
		int parentMapID;
	};
	bool IsInPenList(int id);
	bool IsInCloseList(int id);
	bool FindPathInCloseList(int id,pathInfo& info);
	void CreateCarryPath();
public:
	vector<AutoMapInfo*> mapInfoList;
	vector<CarryInfo*>   carryInfoList;
	int currentMapID;
	list<pathInfo>               findOpenList;
	list<pathInfo>          findCloseList;
	list<int>               pathList;
	list<int>               mapPathList;
	
};

typedef std::vector<AutoItemInfo> ItemInfoList;

class AutoRobotMgr
{
public:
	static AutoRobotMgr&	Singleton();

	void	Activate();
	void	SetMode(enRobotMode enMode);
	enRobotMode	GetMode();
	void	AutoRunTo(int nDstCellX, int nDstCellY, int nSearchUnit = enSearchUnit_1by1Cell);
	void	AutoRunTo(float fXPosPercent, float fYPosPercent, int nSearchUnit = enSearchUnit_1by1Cell);
	bool    AutoRunToForSell(int dx,int dy);
	bool	canRunTo(int nDstCellX, int nDstCellY, int nSearchUnit = enSearchUnit_1by1Cell);
	void	AutoPickObj(bool normalPicked = true);
	void	GetDestCellPos(int &nDestCellX, int &nDestCellY);
	//////////////////////////////////////////
	void    AddAutoAttackFlags(unsigned int flags);
	void    RemoveAutoAttackFlags(unsigned int flags);
	unsigned int GetAutoAttackFlags() const { return autoAttackFlags;}
	void    SetAutoAttackFlags(unsigned int flags);
	void    SetAutoAttackEnemyHigherLevel(unsigned int level);
	void    SetAutoAttackEnemyLowerLevel(unsigned int level);
	void    SetAutoAttackEnemyLevel(unsigned int level) { autoAttackOwnerLevel = level;}
	void    SetAutoEnstrustMode(unsigned int flags);
	void    RemoveAutoEnstrustMode(unsigned int flags);
	unsigned int GetAutoEnstrustMode() const { return autoEntrustComputerMode;}

	bool    AutoSimpleCanTravTo(int subWordIndex,int destX,int destY,int srcX,int srcY);
	bool    AutoSetPlayerTarget();
	bool    AutoPlayerRandomGo();
	void    OnReadyForSell();
//	void    AutoPickupItem();
	/////////////////////////////////////////////////////////
	//自动拾取//
	void    AddAutoPickupItemFlags(unsigned int flags);
	void    ResetPickupItemFlags(){autoPickupItemFlags = AUTO_PICKUP_ITEM_ALL ;}
	void    RemoveAutoPickupItemFlags(unsigned int flags);
	unsigned int GetAutoPickupItemFlags() const { return autoPickupItemFlags;}
	void    SetPickUpTargetPosX(int itemPosX) { targetItemPosX = itemPosX;}
	void    SetPickUpTargetPosY(int itemPosY) { targetItemPosY = itemPosY;}
	void	SetNotPickupList(deque<ItemClassInfo> * classInfo);

	////////////////////////////////
	void LoadAutoUseItemInfo();
	////////////////////////////////
	//添加标志//
	void  SetAutoUseMedicineFlags(unsigned int flags){ autoPickupItemFlags = flags;}
	void  AddAutoUseMedicineFlags(unsigned int flags);
	//删除标志//
	void  RemoveAutoUseMedicineFlags(unsigned int flags);
	void  SetAutoUseMedicineHPPersent(int persent);
	void  SetAutoUseMedicineMPPersent(int persent);
	unsigned int GetAutoUseMedicineFlags() const {return autoUseMedicineFlags;}
	////////////////////////////////
	//自动回城//
	bool  AutoFindHomePath();
	bool  AutoFindBackPath();
	bool  AutoUseItemGoToSell();
	void  AddAutoGoHomeFlags(int flags);
	void  RemoveAutoGoHomeFlags(int flags);
	unsigned int GetGoHomeFlags() const {return autoGoHomeFlags;}
	void  ProcessAutoGoHome();
	int&   GetLastMapIndex() {return lastMapIndex;}
	AutoPathFinder& GetPathFinder() { return m_AutoPathFinder;}
	void  ProcessPrepareGoToSell();
	void  AutoGoActive();
	void  ProcessChangeWorld();
	void  ProcessAutoSellItems();
	void  StopAutoWalk();
	void  ResetTargetNpc();
	void  OnSelectNpcDlg(KUiQuestionAndAnswer* answer = NULL);
	void  OnSellItems();
	void  SetAutoSellState( bool sell)
	{
		runForSell = sell;
		comeback = sell;
		autoGotoFindNpc = false;
		resetCentrePos = false;
	}
	void StopGoToOtherMap( bool state )
	{
		gotoOtherMap = false;
	}
	int GoToOTher(MapPosInfo* pMapInfo);
	void DoGoToOtherMap();
	bool    CanAttackNpc( int npcIdx ) ;
	void    SetAutoAttackPos( );
	bool    GotoSartPos( );

	//自动爆魂
	void    SetBlastExp(int nExp);
	void	SetAutoBlastFlag(bool bFlag);
	void    AutoBlast();

	//自动修理
	void	SetAutoRepairFlag(bool bFlag);

	//自动施放
	void	SetAutoCastSkill(int index, AutoCastSkillInfo & skillInfo);
	void	AutoCastSkill();
	void	TestAutoCastSkill();
	void	CastSkill(int skillId, int posX, int posY,  bool targetSelf = false);
	bool	SelectPet();
	bool	SelectPlayerself();
	void	BackupAttackSkill();

private:
	AutoRobotMgr();

	void	InitSkillPolicy();
	void	AutoAttack();

	void    AutoUseMedicine();

	int		GetNearestObjIdx(bool bNormalPicked);
	int		GetNearestObjInRegion(KRegion &curRegion, int nSrcNpcIdx, int &nObjIdx,bool bNormalPicked);
	int		GetNearestEnemyIdx();
	int		GetNearestNpcInRegion(KRegion &curRegion, int nSrcNpcIdx, int &nNpcIdx);

	int     GetSellNpcPosByName(const char* name);
	int     GetSellNpcPosByNameInRegion(const char* name ,KRegion& curRegion,int &nNpcIdx);

	bool    CanAttackTarget(int nIdx,int targetIdx);
	void    DoComeBack();
	//判断玩家是不是可以站在原地
	bool    StandHere();

	void    FindTargetNpcWhenRun( );

	//自动拾取，获取物品信息
	bool	GetItemInfo				(KIniFile &iniFile, char * section, AutoItemInfo & itemInfo);
	bool	GenNotPickupList		(KIniFile &iniFile);
	bool	IsInNotPickupColorList	(KObj & object);
	bool	IsInPickupList			(KObj & object, bool & isPickup);

	int		FindNextAvailableSkill	(int &skillIndex);

private:
	enRobotMode			m_RobotMode;
	RobotSkillPolicy	m_SkillPolicy;
	AutoPathFinder		m_AutoPathFinder;


	////render 添加////////
	unsigned int        autoEntrustComputerMode; //0没有设置任何状态,以便托机处理时候状态切换//
	unsigned int        autoAttackFlags;
	unsigned int        autoAttackOwnerLevel;//敌对怪物等级,高字段为等级上限,底字段为等级下限

	//自动拾取//
	unsigned int				autoPickupItemFlags;//自动拾取标志
	int							targetItemPosX;
	int							targetItemPosY;
	deque<AutoItemInfoGroup>	m_noPickupList;
	deque<int>					m_noPickupColorList;

	//自动打怪标志
	int                 centreX;
	int                 centreY;
	bool                autoGotoFindNpc;
	bool				resetCentrePos;
	int lastX ;
	int lastY ;
	
	///////自动喝药//////
	ItemInfoList		medicineForHPList;
	ItemInfoList		medicineForMPList;
	unsigned int		autoUseMedicineFlags;
	int					autoHPMPPersent;//高字段为生命百分比,低字段为魔法百分比
	vector<int>			autoGoHomeCarryList;//记录传送门点。

	///自动修理标志/////
	bool	m_bIsAutoRepair;

	///自动回城//////////
	unsigned int         autoGoHomeFlags;
	bool                 isUsingGoHomeItem;
	int                  lastMapIndex;
	int                  goBackPosX;
	int                  goBackPosY;
	bool                 lastGo;
	bool                 runForSell;
	int                  gobackItemIdx;
//	unsigned int         attackTime;
//	unsigned int         pickTime;

	//////不能自动攻击的NPC////
	vector<unsigned int> canNotAutoAttackNpcList;
	//返回的路上不能受到骚扰，用户以骚扰则变为false;
	bool                 comeback;
	list<int>          comebackList;
	//跨地图寻路
	list<int>          findMapWay;
	bool               gotoOtherMap;
	POINT              destMapPos;
	bool               m_bWorldChanged;

	////Add by DarkMagic(DuanMu)////
	//自动爆魂标志
	int				m_iBlastExp;
	bool			m_bIsAutoBlast;

	//自动释放技能
	AutoCastSkillInfo	m_AutoCastSkillList[SkillCount];

public:
	vector<int>          goBackList;
//	vector<int>          canNotAtackNpcIdx;
//	vector<int>          canNotPickIndex ;
 	unsigned int        m_pickTypeFlags;//拾取类型标志//
	vector<DWORD> canNotAttackNpcList;
	bool                pickMedicine; 


	int randRunDistance;
	int attackRadius;
};




inline enRobotMode AutoRobotMgr::GetMode()
{
	return m_RobotMode;
	
}
inline void AutoRobotMgr::SetAutoEnstrustMode(unsigned int flags)
{
	if(autoEntrustComputerMode&flags)
		return;
	autoEntrustComputerMode|=flags;
}
inline void AutoRobotMgr::RemoveAutoEnstrustMode(unsigned int flags)
{
	if(!(autoEntrustComputerMode&flags))
		return;
	autoEntrustComputerMode&=(~flags);
}

inline void AutoRobotMgr::AddAutoPickupItemFlags(unsigned int flags)
{
	if(autoPickupItemFlags&flags)
		return ;
	autoPickupItemFlags|=flags;
}
inline void AutoRobotMgr::RemoveAutoPickupItemFlags(unsigned int flags)
{
	if(!(autoPickupItemFlags&flags))
		return;
	autoPickupItemFlags&=(~flags);
}
inline void	AutoRobotMgr::GetDestCellPos(int &nDestCellX, int &nDestCellY)
{
	if(enRobotMode_AutoRun == m_RobotMode)
		m_AutoPathFinder.GetDestCellPos(nDestCellX, nDestCellY);
}
inline void AutoRobotMgr::AddAutoAttackFlags(unsigned int flags)
{
	if(autoAttackFlags&flags)
		return;
	autoAttackFlags|=flags;
}
inline void AutoRobotMgr::RemoveAutoAttackFlags(unsigned int flags)
{
	if(!(autoAttackFlags&flags))
		return;
	autoAttackFlags &=(~flags);
}
inline void AutoRobotMgr::SetAutoAttackFlags(unsigned int flags)
{
	autoAttackFlags = flags;
}
inline void AutoRobotMgr::AddAutoGoHomeFlags(int flags)
{
	if(autoGoHomeFlags&flags)
		return;
	autoGoHomeFlags|=flags;
}
inline void AutoRobotMgr::RemoveAutoGoHomeFlags(int flags)
{
	if(!(autoGoHomeFlags&flags))
		return;
	autoGoHomeFlags&=(~flags);
}

inline bool AutoRobotMgr::GetItemInfo(KIniFile &iniFile, char * section, AutoItemInfo & itemInfo)
{
	if (section == NULL || section[0] == 0)
	{
		return false;
	}

	iniFile.GetString(section, "name", "", itemInfo.autoItemName, sizeof(itemInfo.autoItemName));
	if (section[0] == 0)
	{
		return false;
	}

	iniFile.GetInteger(section, "type", -1, &itemInfo.autoItemTyp);
	iniFile.GetInteger(section, "sort", -1, &itemInfo.autoItemSort);
	iniFile.GetInteger(section, "particularType", -1, &itemInfo.autoItemParticular);

	if (itemInfo.autoItemParticular == -1
		|| itemInfo.autoItemSort == -1
		|| itemInfo.autoItemParticular == -1)
	{
		return false;
	}
	itemInfo.autoItemIsPickup = false;

	return true;
}

#endif	// #ifdef _AUTO_ROBOT
#endif	// #ifndef _AutoRobotMgr_h