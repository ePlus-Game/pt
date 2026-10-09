//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-22
//      File_base        : talisman_monitor
//      File_ext         : .cpp
//      Author           : ÐìÏþ¸Õ
//      Description      : ·¨±¦¼àÊÓÆ÷
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KPlayer.h"
#include "KItem.h"
#include "ConfigManager.h"
#include "talisman_manager.h"
#include "talisman_monitor.h"

TalismanMonitor::TalismanMonitor()
{
}

TalismanMonitor::~TalismanMonitor()
{
}

void TalismanMonitor::Init()
{	
}

void TalismanMonitor::Update(int npcIndex, const KEquipState* equipments)
{
	TalismanManager& tm = TalismanManager::Singleton();
	memset(&m_ActiveInfo, false, sizeof(m_ActiveInfo));
	
	int talismanIndex = equipments[itempart_talisman].nEquipIdx;
	if (tm.IsValidTalisman(talismanIndex))
	{
		tm.GetEnchaseActiveInfo(talismanIndex, m_ActiveInfo);
	}	

#ifdef _SERVER
	if (npcIndex <= 0 || npcIndex >= MAX_PLAYER)
	{
		return;
	}

	KNpc& npc = Npc[npcIndex];
	int originalTalismanNpcId = npc.GetEquipTalismanNpcId();
	int newTalismanNpcId = 0;

	BeginAddBuff();
	
	if (tm.IsValidTalisman(talismanIndex))
	{
		KItem& talisman = Item[talismanIndex];	
		
		bool isMasked = equipments[itempart_talisman].IsMasked;
		if (!isMasked)
		{
			newTalismanNpcId = talisman.GetTalismanId();

			int talismanBuffId = talisman.GetTalismanBuff();
			if (talismanBuffId > 0)
			{
				AddBuff(talismanBuffId);
			}
			
			for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
			{
				int count = m_ActiveInfo.CountInfo[enchaseLoopCount].Count;
				if (count > 0 && count <= TM_HOLE_NUM)
				{
					const PEnchaseData pEnchaseData = TalismanManager::Singleton().GetEnchaseData(m_ActiveInfo.CountInfo[enchaseLoopCount].EnchaseId);
					for (int enchaseBuffLoopCount = 0; enchaseBuffLoopCount < MAX_ENCHASE_BUFF_COUNT; enchaseBuffLoopCount++)
					{
						int enchaseBuffId = pEnchaseData->BuffSet[count - 1][enchaseBuffLoopCount];
						if (enchaseBuffId > 0)
						{
							AddBuff(enchaseBuffId);
						}
					}
				}
			}
		}
	}	

	EndAddBuff();
	SetupBuff(npcIndex);

	if (originalTalismanNpcId != newTalismanNpcId)
	{
		Npc[npcIndex].SetEquipTalismanNpcId(newTalismanNpcId);
		Npc[npcIndex].SyncEquipTalismanNpcId();
	}	
#endif
}
