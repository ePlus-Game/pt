//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-5-31
//      File_base        : npc_statistic
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : NPC信息统计
//
//////////////////////////////////////////////////////////////////////

#ifndef _NPC_STATISTIC_H_
#define _NPC_STATISTIC_H_

#include "KNpc.h"
#define MAX_NPC_STATISTIC_INFO_COUNT MAX_NPCSTYLE
#define MAX_NPC_DROP_ITEM_TYPE_COUNT 20	//NPC掉落物品种类数量

//NPC统计信息
struct NpcStatisticInfo
{
	DWORD KillByPlayerCount;
	DWORD KillPlayerCount;
};

//NPC掉落物品信息
struct NpcDropItemInfo
{
	ItemTemplateId Id;
	DWORD Count;
};

//NPC信息统计
class NpcStatistic
{
public:
	void Init(int npcTemplateIndex);//初始化
	void Active();//活动一次
	void Save();//保存并清空数据

	void KillByPlayer();//被玩家杀死
	void KillPlayer();//杀死玩家
	void DropItem(ItemTemplateId& id);//掉落物品

private:
	void Clear();

	DWORD m_LastSaveTimeout;
	static DWORD s_SavingProgressTimeout;

	int m_NpcTemplateIndex;
	DWORD m_KillByPlayerCount;
	DWORD m_KillPlayerCount;
	NpcDropItemInfo m_DropItemInfo[MAX_NPC_DROP_ITEM_TYPE_COUNT];
};

inline void NpcStatistic::KillByPlayer()
{
	m_KillByPlayerCount++;
}

inline void NpcStatistic::KillPlayer()
{
	m_KillPlayerCount++;
}

inline void NpcStatistic::DropItem(ItemTemplateId& id)
{
	ItemTemplateId invalidId;
	for (int i = 0 ; i < MAX_NPC_DROP_ITEM_TYPE_COUNT; i++)
	{
		NpcDropItemInfo& info = m_DropItemInfo[i];
		if (info.Id == id)
		{
			info.Count++;
			break;
		}
		else if (info.Id == invalidId)
		{
			info.Id = id;
			info.Count = 1;
			break;
		}
	}
}

void NpcStatisticActive();//NPC统计活动一次
void NpcStatisticForceSave();//NPC统计信息强制存盘

#endif// _NPC_STATISTIC_H_