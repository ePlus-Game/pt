//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-23
//      File_base        : BasicProperty_Monitor
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 装备基本属性（基本、升级、加持属性）监视器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "BasicProperty_Monitor.h"
#include "KItem.h"
#include "buff_man.h"
#include "KPlayer.h"

BasicPropertyMonitor::BasicPropertyMonitor()
{
	Init();
}

BasicPropertyMonitor::~BasicPropertyMonitor()
{
}

void BasicPropertyMonitor::Init()
{
	CleanUp();
}

void BasicPropertyMonitor::Update(int npcIndex, const KEquipState* equipments)
{
	if (!IsValidNpc(npcIndex))
		return;

	int playerIndex = Npc[npcIndex].GetPlayerIdx();
	if (!IsValidPlayer(playerIndex))
		return;

	BeginAddBuff();

	//添加相关BUFF
	for(int itemPartLoopCount = 0; itemPartLoopCount < itempart_num; ++itemPartLoopCount)
	{
		int itemIndex = equipments[itemPartLoopCount].nEquipIdx;
		if (itemIndex > 0)
		{
			KItem& item = Item[itemIndex];
			
			if (!item.IsBroken() && !item.IsOverDate())
			{
#ifdef _SERVER
				//堵漏：防止物品Index错误导致添加非装备的BUFF
				if ((item.GetBelong() != playerIndex)
					|| (item.GetGenre() != item_equip))
				{
					//记录系统日志
					char invalidEquipmentInfo[256] = { 0 };
					snprintf(invalidEquipmentInfo, sizeof(invalidEquipmentInfo), 
						"InvalidEquipment: PlayerName=\"%s\", ItemIndex=%d, EquipPart=%d, ItemName=\"%s\"", 
						Player[playerIndex].GetPlayerName(), itemIndex, itemPartLoopCount, item.GetName());
					invalidEquipmentInfo[sizeof(invalidEquipmentInfo) - 1] = 0;
					g_pLogSystem->SysDbgLog(invalidEquipmentInfo, strlen(invalidEquipmentInfo), sys_dbg_log_event_invalid_equipment);

					continue;
				}
#endif

				//物品基本属性效果
				const ItemBasicBuff& basicBuff = item.GetBasicBuffID();
				for(int basicBuffLoopCount = 0; basicBuffLoopCount < ITEM_BUFF_COUNT; basicBuffLoopCount++)
				{
					int buffID = basicBuff.BuffID[basicBuffLoopCount];
					if (buffID > 0)
					{
						AddBuff(buffID);
					}
				}
				
				//物品附加属性效果
				const WORD* pCompBuffSet = item.GetCompBuffTemplateSet();
				for(int compBuffLoopCount = 0; compBuffLoopCount < COMPOUND_COUNT; compBuffLoopCount++)
				{
					int buffID = pCompBuffSet[compBuffLoopCount];
					if (buffID > 0)
					{
						AddBuff(buffID);
					}
				}

				//镶嵌属性效果
				short* pBaseInlayBuffSet = item.GetInlayBaseBuffSet();
				for(int baseInlayCount = 0; baseInlayCount < MAX_INLAY_COUNT; baseInlayCount++)
				{
					for (int y = 0; y < ITEM_BUFF_COUNT; ++y )
					{
						int buffID = *(pBaseInlayBuffSet + ( baseInlayCount* ITEM_BUFF_COUNT + y));
						if (buffID > 0)
						{
							AddBuff(buffID);
						}
					}
				}
				const short* pYaoInlayBuffSet = item.GetInlayYaoBuffSet();
				for(int yaoInlayCount = 0; yaoInlayCount < MAX_ITEM_INLAY_YAO_EFFECT_COUNT; yaoInlayCount++)
				{
					int buffID = pYaoInlayBuffSet[yaoInlayCount];
					if (buffID > 0)
					{
						AddBuff(buffID);
					}
				}
				const short* pSpecialInlayBuffSet = item.GetInlaySpecialBuffSet();
				for(int SpecialInlayCount = 0; SpecialInlayCount < MAX_SPECIALEFFECT_COUNT; SpecialInlayCount++)
				{
					int buffID = pSpecialInlayBuffSet[SpecialInlayCount];
					if (buffID > 0)
					{
						AddBuff(buffID);
					}
				}
			}
		}
	}

	EndAddBuff();

	SetupBuff(npcIndex);
}