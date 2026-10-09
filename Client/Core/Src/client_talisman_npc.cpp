//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-7
//      File_base        : client_talisman_npc
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 法宝NPC（客户端）
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSkills.h"
#include "KSubWorld.h"
#include "CoreUseNameDef.h"
#include "client_talisman_npc.h"

ClientTalismanNpcTable::ClientTalismanNpcTable()
{
}

ClientTalismanNpcTable& ClientTalismanNpcTable::Singleton()
{
	static ClientTalismanNpcTable table;
	return table;
}

bool ClientTalismanNpcTable::Load()
{
	bool sucess = true;

	memset(m_TalismanNpc, 0, sizeof(m_TalismanNpc));

	KTabFile tableFile;
	if (TRUE == tableFile.Load(CLIENT_TALISMAN_NPC_TABLE_FILE))
	{
		int row = 0;
		int recordCount = tableFile.GetHeight() - 1;
		if (recordCount > MAX_CLIENT_TALISMAN_NPC)
		{
			_ASSERT(0);
			recordCount = MAX_CLIENT_TALISMAN_NPC;
		}
				
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			ClientTalismanNpc& talismanNpc = m_TalismanNpc[record];
			
			int field = 1;			

			if (FALSE == tableFile.GetInteger(row, field, 0, &(talismanNpc.Id)))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == tableFile.GetInteger(row, field, 0, &(talismanNpc.NpcId_Normal)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == tableFile.GetInteger(row, field, 0, &(talismanNpc.NpcId_UseSkill)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == tableFile.GetString(row, field, "", talismanNpc.Spr_UseSkill, sizeof(talismanNpc.Spr_UseSkill)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == tableFile.GetInteger(row, field, 0, &(talismanNpc.UseSkillTime)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == tableFile.GetInteger(row, field, 0, &(talismanNpc.FollowDistance)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == tableFile.GetInteger(row, field, 0, &(talismanNpc.FollowSpeed)))
			{
				sucess = false;
			}
			++field;
			
			if (!sucess)
			{
				break;
			}
		}
	}	

	return sucess;
}

ClientTalismanNpc* ClientTalismanNpcTable::GetClientTalismanNpc(int id)
{
	for (int loopCount = 0; loopCount < MAX_CLIENT_TALISMAN_NPC; loopCount++)
	{
		if (m_TalismanNpc[loopCount].Id == id)
		{
			return &(m_TalismanNpc[loopCount]);
		}
	}

	return NULL;
}

ClientTalismanNpcController::ClientTalismanNpcController()
{
	m_NpcIndex = 0;
	m_PolymorphTime = 0;
	m_TalismanNpcIndex = 0;
	m_pTemplate = NULL;
}

void ClientTalismanNpcController::Init(int npcIndex)
{
	m_NpcIndex = npcIndex;
	m_PolymorphTime = 0;
	if (m_TalismanNpcIndex != 0)
	{
		KNpc& talismanNpc = Npc[m_TalismanNpcIndex];
		if (talismanNpc.m_RegionIndex >= 0)
		{
			int nSubWorld = talismanNpc.m_SubWorldIndex;
			int nRegion = talismanNpc.m_RegionIndex;
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(m_TalismanNpcIndex);
		}
		NpcSet.Remove(m_TalismanNpcIndex, false);

		m_TalismanNpcIndex = 0;
	}
	m_pTemplate = NULL;
}

void ClientTalismanNpcController::Active()
{	
	if (m_TalismanNpcIndex > 0 && m_NpcIndex > 0)
	{
		unsigned long currentTime = time(NULL);
		
		KNpc& talismanNpc = Npc[m_TalismanNpcIndex];
		KNpc& ownerNpc = Npc[m_NpcIndex];
		
		int distance = NpcSet.GetDistance(m_TalismanNpcIndex, m_NpcIndex);
		int ownerPosX, ownerPosY, talismanPosX, talismanPosY;			
		talismanNpc.GetMpsPos(&talismanPosX, &talismanPosY);
		ownerNpc.GetMpsPos(&ownerPosX, &ownerPosY);
		
		int ownerNpcSpeed = ownerNpc.m_CompAttrMgr[ncai_runspeed];
				
		int basicDistance = 100;
		int followSpeed = ownerNpcSpeed;

		if (m_pTemplate != NULL)
		{
			if (m_pTemplate->FollowDistance > 0)
			{
				basicDistance = m_pTemplate->FollowDistance;
			}
			if (m_pTemplate->FollowSpeed > 0)
			{
				followSpeed = m_pTemplate->FollowSpeed;
			}
		}

		static int offsetXArray[8] = {
				0,
				-basicDistance * 7 / 10,
				-basicDistance,
				-basicDistance * 7 / 10,
				0,
				basicDistance * 7 / 10,
				basicDistance,
				basicDistance * 7 / 10,
		};
		static int offsetYArray[8] = {
				basicDistance,
				basicDistance * 7 / 10,
				0,
				-basicDistance * 7 / 10,
				-basicDistance,
				-basicDistance * 7 / 10,
				0,
				basicDistance * 7 / 10
		};
		
		//确定跟随点的坐标
		int targetPosDir = ((ownerNpc.m_UnaryAttrMgr[nuai_dir] + 40 ) % 64) / 8;
		int targetPosX = ownerPosX + offsetXArray[targetPosDir];
		int targetPosY = ownerPosY + offsetYArray[targetPosDir];
		
		talismanNpc.m_CompAttrMgr.Set(ncai_runspeed, idx_current_value, followSpeed);

		if (distance > 500)//离开太远了
		{
			//直接拽到跟随点
			talismanNpc.SetPos(targetPosX, targetPosY);
		}
		else
		{
			if (abs(talismanPosX - targetPosX) > followSpeed || abs(talismanPosY - targetPosY) > followSpeed)
			{
				//向跟随点移动
				talismanNpc.SendCommand(do_run, targetPosX, targetPosY);
			}				
		}
		
		//到达跟随点
		if (talismanNpc.m_Doing == do_stand)
		{
			//调整朝向和主人一致
			talismanNpc.m_UnaryAttrMgr.Set(nuai_dir, ownerNpc.m_UnaryAttrMgr[nuai_dir]);
		}
		
		if (m_PolymorphTime > 0 && m_PolymorphTime <= currentTime)//变身时间到了
		{				
			talismanNpc.PolyMorph( -1, 0, 0, 0, 0 );				
			m_PolymorphTime = 0;
		}
		
		talismanNpc.m_SyncSignal = SubWorld[0].m_dwCurrentTime;
	}
}

void ClientTalismanNpcController::UseTalismanSkill(int skillId, int skillLevel)
{
	if (m_PolymorphTime == 0)
	{
		ClientTalismanNpc* pTalismanNpcInfo = ClientTalismanNpcTable::Singleton().GetClientTalismanNpc(Npc[m_NpcIndex].GetEquipTalismanNpcId());
		if (pTalismanNpcInfo != NULL)
		{			
			KNpc& talismanNpc = Npc[m_TalismanNpcIndex];
			talismanNpc.PolyMorph(pTalismanNpcInfo->NpcId_UseSkill, 0, 0, 0, 0);
			talismanNpc.m_DataRes.SetSpecialSpr(pTalismanNpcInfo->Spr_UseSkill);			
			m_PolymorphTime = time(NULL) + pTalismanNpcInfo->UseSkillTime;
		}		
	}
}