// Robot script functions
// by Cooler liuyujun@263.net
// 2004-07-07

#include "KWin32.h"
#include "RobotScript.h"
#include "RobotControl.h"
#include "CoreShell.h"
#include <map>
#include <string>


typedef std::string				KString;
typedef std::map<KString, int>	MAPVARLIST;

extern iCoreShell	*g_pCoreShell;
static MAPVARLIST	g_listVar;


// Add player level to 120, add all skill to max level
// No call parameters
int LuaRobotPowerUp(Lua_State * L)
{
	CRobotControl::SendGmInstruction("GM_AddMaxLevel()");
	CRobotControl::SendGmInstruction("GM_ActiveAllSkill()");
	CRobotControl::SendGmInstruction("GM_AddAllSkillMaxLevel()");

	return 0;
}

// Add an item for player
// Call parameters: nItemClass, nDetailType, nParticularType, nItemAttribute, nLevel
int LuaRobotAddItem(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 5)
	{
		return 0;
	}
	
	int nItemClass		= (int)Lua_ValueToNumber(L, 1);
	int nDetailType		= (int)Lua_ValueToNumber(L, 2);
	int nParticularType	= (int)Lua_ValueToNumber(L, 3);
	int nItemAttribute	= (int)Lua_ValueToNumber(L, 4);
	int nLevel			= (int)Lua_ValueToNumber(L, 5);

	char szSendCmd[64] = {0};
	sprintf(szSendCmd, "GM_AddItem(%d,%d,%d,%d,%d)", 
		nItemClass, nDetailType, nParticularType, nItemAttribute, nLevel);
	CRobotControl::SendGmInstruction(szSendCmd);

	return 0;
}

// Use an item
int LuaRobotEatItem(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 1)
	{
		return 0;
	}
	
	int nEatItemType = (int)Lua_ValueToNumber(L, 1);
	// Note: eat item type. 0 = add blood, 1 = add mana
	if(nEatItemType < 0 || nEatItemType > 1)
	{
		return 0;
	}

	int nItemClass, nDetailType, nParticularType;
	int nItemAttribute, nLevel;
	int nItemIndex, nPosX, nPosY;
	INVENTORY_ROOM enBagType;

	if(nEatItemType == 0)
	{
		// Eat red potion
		nItemClass = 1;
		nDetailType = 0;
		nParticularType = 3;
		nItemAttribute = -1;
		nLevel = 3;
	}
	else if(nEatItemType == 1)
	{
		// Eat blue potion
		nItemClass = 1;
		nDetailType = 1;
		nParticularType = 3;
		nItemAttribute = -1;
		nLevel = 3;
	}

	// Find eat item
	if(!g_pCoreShell->FindTakeItem(nItemClass, nDetailType, 
		nParticularType, nItemAttribute, nLevel, enBagType, 
		nItemIndex, nPosX, nPosY))
	{
		return 0;
	}
	
	// Eat it
	KUiObjAtRegion tagItemInfo;
	tagItemInfo.Obj.uGenre = CGOG_ITEM;
	tagItemInfo.Obj.uId = nItemIndex;
	tagItemInfo.Region.h = nPosX;
	tagItemInfo.Region.v = nPosY;
	tagItemInfo.Region.Width = 1;
	tagItemInfo.Region.Height = 1;
	g_pCoreShell->OperationRequest(GOI_USE_ITEM, 
		(unsigned int)&tagItemInfo, UOC_ITEM_TAKE_WITH);

	return 0;
}

// Item list whether locked
int LuaIsItemListLocked(Lua_State * L)
{
	if(g_pCoreShell->IsItemListLocked())
	{
		Lua_PushNumber(L, 1);
	}
	else
	{
		Lua_PushNumber(L, 0);
	}
	
	return 1;
}

// Judge whether is the empty hand
int LuaIsEmptyHand(Lua_State * L)
{
	if(g_pCoreShell->GetHandItemIndex() > 0)
	{
		Lua_PushNumber(L, 1);
	}
	else
	{
		Lua_PushNumber(L, 0);
	}

	return 1;
}

// Pickup an item from take bag
int LuaRobotPickupItem(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 5)
	{
		return 0;
	}
	
	int nItemClass		= (int)Lua_ValueToNumber(L, 1);
	int nDetailType		= (int)Lua_ValueToNumber(L, 2);
	int nParticularType	= (int)Lua_ValueToNumber(L, 3);
	int nItemAttribute	= (int)Lua_ValueToNumber(L, 4);
	int nLevel			= (int)Lua_ValueToNumber(L, 5);
	
	int nItemIndex, nPosX, nPosY;
	INVENTORY_ROOM enBagType;
	// Find item position
	if(!g_pCoreShell->FindTakeItem(nItemClass, nDetailType, 
		nParticularType, nItemAttribute, nLevel, enBagType, 
		nItemIndex, nPosX, nPosY))
	{
		return 0;
	}

	KUiObjAtContRegion tagPick;
	tagPick.eContainer = UOC_ITEM_TAKE_WITH;
	tagPick.Obj.uGenre = CGOG_ITEM;
	tagPick.Obj.uId = nItemIndex;
	tagPick.Region.Height = 1;
	tagPick.Region.Width = 1;
	tagPick.Region.h = nPosX;
	tagPick.Region.v = nPosY;
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, 
		(unsigned int)&tagPick, 0);

	return 0;
}

// Place an item to equipment
int LuaRobotPlaceItemEQ(Lua_State * L)
{
	int nHand = g_pCoreShell->GetHandItemIndex();

	if(nHand > 0)
	{
		KUiObjAtContRegion tagDrop;
		tagDrop.eContainer = UOC_EQUIPTMENT;
		tagDrop.Obj.uGenre = CGOG_ITEM;
		tagDrop.Obj.uId = nHand;
		tagDrop.Region.Height = 1;
		tagDrop.Region.Width = 1;
		tagDrop.Region.h = 0;
		tagDrop.Region.v = 5;
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, 
			0, (int)&tagDrop);
	}

	return 0;
}

// Place an item back to take bag
int LuaRobotPlaceItemTB(Lua_State * L)
{
	int nHand = g_pCoreShell->GetHandItemIndex();

	POINT ptEmpty;
	if(nHand > 0 && 
		g_pCoreShell->FindPlacePos(1, 1, &ptEmpty))
	{
		KUiObjAtContRegion tagDrop;
		tagDrop.eContainer = UOC_ITEM_TAKE_WITH;
		tagDrop.Obj.uGenre = CGOG_ITEM;
		tagDrop.Obj.uId = nHand;
		tagDrop.Region.Height = 1;
		tagDrop.Region.Width = 1;
		tagDrop.Region.h = ptEmpty.x;
		tagDrop.Region.v = ptEmpty.y;
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, 
			0, (int)&tagDrop);
	}

	return 0;
}

// Select a skill to use
int LuaRobotSwitchSkill(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 2)
	{
		return 0;
	}
	
	// Note: nSkillPos = 0 means left skill, nSkillPos = 1 means right skill;
	int nSkillPos		= (int)Lua_ValueToNumber(L, 1);
	int nSkillID		= (int)Lua_ValueToNumber(L, 2);

	if(nSkillPos < 0 || nSkillPos > 1 || nSkillID < 0)
	{
		return 0;
	}

	KUiGameObject tagObj;
	tagObj.uGenre = CGOG_ITEM;
	tagObj.uId = nSkillID;
	g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, 
							(unsigned int)&tagObj, nSkillPos);
	
	return 0;
}

// Post message
int LuaRobotPostMessage(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 2)
	{
		return 0;
	}
	
	// Note: nSkillPos = 0 means nearby message, 
	//       nSkillPos = 1 means global message.
	int nMessageType		= (int)Lua_ValueToNumber(L, 1);
	LPSTR pMsgText			= (LPSTR)Lua_ValueToString(L, 2);

	// TODO...

	return 0;
}

// Check whether item exist
int LuaRobotIsExistItem(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 5)
	{
		return 0;
	}
	
	int nItemClass		= (int)Lua_ValueToNumber(L, 1);
	int nDetailType		= (int)Lua_ValueToNumber(L, 2);
	int nParticularType	= (int)Lua_ValueToNumber(L, 3);
	int nItemAttribute	= (int)Lua_ValueToNumber(L, 4);
	int nLevel			= (int)Lua_ValueToNumber(L, 5);
	
	int nItemIndex, nPosX, nPosY;
	INVENTORY_ROOM enBagType;
	// Find item position
	if(g_pCoreShell->FindTakeItem(nItemClass, nDetailType, 
		nParticularType, nItemAttribute, nLevel, enBagType, 
		nItemIndex, nPosX, nPosY))
	{
		Lua_PushNumber(L, 1);
		return 1;
	}

	Lua_PushNumber(L, 0);
	return 1;
}

// Move to a new world
int LuaRobotGotoWorld(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 3)
	{
		return 0;
	}
	
	int nWorldID		= (int)Lua_ValueToNumber(L, 1);
	int nWorldPosX		= (int)Lua_ValueToNumber(L, 2);
	int nWorldPosY		= (int)Lua_ValueToNumber(L, 3);

	char szSendCmd[64] = {0};
	sprintf(szSendCmd, "GM_NewWorld(%d,%d,%d)", 
				nWorldID, nWorldPosX, nWorldPosY);
	CRobotControl::SendGmInstruction(szSendCmd);

	return 0;
}

// Add a global variant, only support positive int
int LuaRobotAddVar(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 2)
	{
		return 0;
	}
	
	LPSTR pVarName	= (LPSTR)Lua_ValueToString(L, 1);
	int nVarValue	= (int)Lua_ValueToNumber(L, 2);

	if(nVarValue < 0)
	{
		return 0;
	}

	KString strVarName = pVarName;
	g_listVar[strVarName] = nVarValue;

	return 0;
}

// Update a global variant's value
int LuaRobotUpdateVar(Lua_State * L)
{
	return LuaRobotAddVar(L);
}

// Get a global variant's value
// If var doesn't exist, return -1
int LuaRobotGetVar(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 1)
	{
		return 0;
	}
	
	LPSTR pVarName	= (LPSTR)Lua_ValueToString(L, 1);

	KString strVarName = pVarName;
	MAPVARLIST::iterator itFound = NULL;
	itFound = g_listVar.find(strVarName);
	if(itFound != g_listVar.end())
	{
		Lua_PushNumber(L, itFound->second);
	}
	else
	{
		Lua_PushNumber(L, -1);
	}

	return 1;
}

// Del a global variant
int LuaRobotDelVar(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 1)
	{
		return 0;
	}
	
	LPSTR pVarName	= (LPSTR)Lua_ValueToString(L, 1);

	KString strVarName = pVarName;
	MAPVARLIST::iterator itFound = NULL;
	itFound = g_listVar.find(strVarName);
	if(itFound != g_listVar.end())
	{
		g_listVar.erase(itFound);
	}

	return 0;
}

// Empty your bag
int LuaRobotEmptyBag(Lua_State * L)
{
	// TODO...

	return 0;
}

// Add money
int LuaRobotAddMoney(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 1)
	{
		return 0;
	}
	
	int nAddMoney		= (int)Lua_ValueToNumber(L, 1);

	char szSendCmd[64] = {0};
	sprintf(szSendCmd, "GM_AddMoney(%d,%d,%d)", nAddMoney);
	CRobotControl::SendGmInstruction(szSendCmd);
	
	return 0;
}

// Set robot current action
int LuaSetRobotAction(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 2)
	{
		return 0;
	}

	ROBOTACTIONINFO tagInfo;
	int nRobotAction		= (int)Lua_ValueToNumber(L, 1);
	if(nRobotAction <= enActionNone || 
			nRobotAction >= enActionEnd)
	{
		return 0;
	}
	tagInfo.enAction		= (enACTIONTYPE)nRobotAction;
	tagInfo.dwRunCircles	= (DWORD)Lua_ValueToNumber(L, 2);

	CRobotControl::SetAction(tagInfo);

	return 0;
}

// Get current role type
int LuaGetRoleType(Lua_State * L)
{
	int nRoleType = 0;
	g_pCoreShell->GetGameData(GDI_ROLE_TYPE, 0, (int)&nRoleType);

	Lua_PushNumber(L, nRoleType);

	return 1;
}

TLua_Funcs g_RobotScriptFuns[] = 
{
	{"RobotPowerUp", LuaRobotPowerUp},
	{"RobotAddItem", LuaRobotAddItem},
	{"RobotEatItem", LuaRobotEatItem},
	{"IsItemListLocked", LuaIsItemListLocked},
	{"IsEmptyHand", LuaIsEmptyHand},
	{"RobotPickupItem", LuaRobotPickupItem},
	{"RobotPlaceItemEQ", LuaRobotPlaceItemEQ},
	{"RobotPlaceItemTB", LuaRobotPlaceItemTB},
	{"RobotSwitchSkill", LuaRobotSwitchSkill},
	{"RobotPostMessage", LuaRobotPostMessage},
	{"RobotIsExistItem", LuaRobotIsExistItem},
	{"RobotGotoWorld", LuaRobotGotoWorld},
	{"RobotAddVar", LuaRobotAddVar},
	{"RobotUpdateVar", LuaRobotUpdateVar},
	{"RobotGetVar", LuaRobotGetVar},
	{"RobotDelVar", LuaRobotDelVar},
	{"RobotEmptyBag", LuaRobotEmptyBag},
	{"RobotAddMoney", LuaRobotAddMoney},
	{"SetRobotAction", LuaSetRobotAction},
	{"GetRoleType", LuaGetRoleType},
};

int GetRobotScriptNum()
{
	return sizeof(g_RobotScriptFuns) / sizeof(g_RobotScriptFuns[0]);
}
