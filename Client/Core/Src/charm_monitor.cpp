//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-06-25
//      File_base        : charm_monitor
//      File_ext         : .cpp
//      Author           : ÐìÏþ¸Õ
//      Description      : »¤Éí·û¼àÊÓÆ÷
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KItem.h"
#include "buff_man.h"
#include "KPlayer.h"
#include "charm_monitor.h"

CharmMonitor::CharmMonitor()
{
	Init();
}

CharmMonitor::~CharmMonitor()
{
}

void CharmMonitor::Init()
{
	CleanUp();
}

void CharmMonitor::Update(int npcIndex, KItemList& itemList)
{
	if (!IsValidNpc(npcIndex))
		return;

	int playerIndex = Npc[npcIndex].GetPlayerIdx();
	if (!IsValidPlayer(playerIndex))
		return;

	BeginAddBuff();

	PlayerItem* pItemInfo = itemList.GetFirstItem();
	while (pItemInfo)
	{
		int itemIndex = pItemInfo->nIdx;
		if (itemIndex > 0 && itemIndex < MAX_ITEM && pos_equiproom == pItemInfo->nPlace && Item[itemIndex].GetGenre() == item_charm)
		{
			const ItemBasicBuff& charmBuff = Item[itemIndex].GetBasicBuffID();
			for(int charmBuffLoopCount = 0; charmBuffLoopCount < ITEM_BUFF_COUNT; charmBuffLoopCount++)
			{
				int buffID = charmBuff.BuffID[charmBuffLoopCount];
				if (buffID > 0)
				{
					AddBuff(buffID);
				}
			}
		}

		pItemInfo = itemList.GetNextItem();
	}

	EndAddBuff();

	SetupBuff(npcIndex);
}