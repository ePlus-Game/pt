//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-25
//      File_base        : BaseMonitor
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 监视器基类
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "BaseMonitor.h"
#include "buff_man.h"
#include "KPlayer.h"

BaseMonitor::BaseMonitor()
{	
	CleanUp();
}

BaseMonitor::~BaseMonitor()
{
	CleanUp();
}

void BaseMonitor::CleanUp()
{
	m_BuffToBeAddCount = 0;	
	memset(m_BuffIDToBeAdd, 0, sizeof(m_BuffIDToBeAdd));
	m_CurrentBuffIndexCount = 0;
	memset(m_CurrentBuff, 0, sizeof(m_CurrentBuff));
}

void BaseMonitor::BeginAddBuff()
{
	m_BuffToBeAddCount = 0;
	memset(m_BuffIDToBeAdd, 0, sizeof(m_BuffIDToBeAdd));
}

bool BaseMonitor::AddBuff(int buffID)
{
	if (buffID > 0 && m_BuffToBeAddCount < MAX_BUFF_TO_BE_SETUP - 1)
	{
		m_BuffIDToBeAdd[m_BuffToBeAddCount++] = buffID;
		return true;
	}
	else
	{
		_ASSERT(false);
		return false;
	}
}

void BaseMonitor::EndAddBuff()
{
	//TODO可能需要对BUFF进行排序
}

void BaseMonitor::SetupBuff(int npcIndex, bool clearAll/* = false*/)
{
	if (!IsValidNpc(npcIndex))
		return;

	int playerIndex = Npc[npcIndex].GetPlayerIdx();
	if (!IsValidPlayer(playerIndex))
		return;

	//这里使用static变量，所以必须保证单线程访问这个函数
	static BuffSetuped buffIndex[MAX_BUFF_TO_BE_SETUP];
	static int buffIndexCount;
	
	memset(buffIndex, 0, sizeof(buffIndex));	
	buffIndexCount = 0;

	BuffMgr& bm = BuffMgr::Singleton();

	int creatureNpcIndex = 0;

	if (Player[playerIndex].m_Creature.IsALive())
	{
		creatureNpcIndex = Player[playerIndex].m_Creature.GetCreatureNpc()->m_Index;
	}

	if (clearAll)//先清除所有的BUFF，然后重新添加新的BUFF
	{
		//清除所有的BUFF
		for(int currentBuffLoopCount = 0; currentBuffLoopCount < m_CurrentBuffIndexCount; currentBuffLoopCount++)
		{
			if (m_CurrentBuff[currentBuffLoopCount].BuffID > 0)
			{
				bm.DecBuffPile(npcIndex, m_CurrentBuff[currentBuffLoopCount].BuffIndex);
				if (creatureNpcIndex > 0)
				{
					bm.DecBuffPile(creatureNpcIndex, m_CurrentBuff[currentBuffLoopCount].CreatureBuffIndex);
				}
			}
		}

		//添加BUFF
		for(int buffToBeAddLoopCount = 0; buffToBeAddLoopCount < m_BuffToBeAddCount; buffToBeAddLoopCount++)
		{
			int buffID = m_BuffIDToBeAdd[buffToBeAddLoopCount];			
			unsigned long index = bm.AddNpcBuff( npcIndex, npcIndex, buffID );
			unsigned long creatureBuffIndex = 0;
			if (creatureNpcIndex > 0)
			{
				creatureBuffIndex= bm.AddNpcBuff( creatureNpcIndex, creatureNpcIndex, buffID );
			}

			_ASSERT(index);
			if (index)
			{
				buffIndex[buffIndexCount].BuffIndex = index;
				buffIndex[buffIndexCount].CreatureBuffIndex = creatureBuffIndex;
				buffIndex[buffIndexCount].BuffID = buffID;
				buffIndexCount++;
			}
		}
	}
	else//利用现有的BUFF，然后重新新的BUFF，最后清除多余的BUFF
	{
		//添加需要的BUFF
		for(int buffToBeAddLoopCount = 0; buffToBeAddLoopCount < m_BuffToBeAddCount; buffToBeAddLoopCount++)
		{
			int buffID = m_BuffIDToBeAdd[buffToBeAddLoopCount];
			bool found = false;
			for(int currentBuffLoopCount = 0; currentBuffLoopCount < m_CurrentBuffIndexCount; currentBuffLoopCount++)
			{
				if (m_CurrentBuff[currentBuffLoopCount].BuffID == buffID)//可以利用现有BUFF
				{				
					buffIndex[buffIndexCount].BuffIndex = m_CurrentBuff[currentBuffLoopCount].BuffIndex;
					buffIndex[buffIndexCount].CreatureBuffIndex = m_CurrentBuff[currentBuffLoopCount].CreatureBuffIndex;
					buffIndex[buffIndexCount].BuffID = buffID;
					buffIndexCount++;
					
					found = true;
					m_CurrentBuff[currentBuffLoopCount].BuffID = 0;//把这个标志为非法
					break;
				}
			}
			
			if (!found)//没有现有的BUFF可以利用，需要添加
			{
				unsigned long index = 0;
				index = bm.AddNpcBuff(npcIndex, npcIndex, buffID );
				unsigned long creatureBuffIndex = 0;
				if (creatureNpcIndex > 0)
				{
					creatureBuffIndex= bm.AddNpcBuff( creatureNpcIndex, creatureNpcIndex, buffID );
				}
				_ASSERT(index);
				if (index)
				{
					buffIndex[buffIndexCount].BuffIndex = index;
					buffIndex[buffIndexCount].CreatureBuffIndex = creatureBuffIndex;
					buffIndex[buffIndexCount].BuffID = buffID;
					buffIndexCount++;
				}
			}		
		}
		
		//清除多余的BUFF
		for(int remainCurrentBuffLoopCount = 0; remainCurrentBuffLoopCount < m_CurrentBuffIndexCount; remainCurrentBuffLoopCount++)
		{
			if (m_CurrentBuff[remainCurrentBuffLoopCount].BuffID > 0)
			{
				bm.DecBuffPile(npcIndex, m_CurrentBuff[remainCurrentBuffLoopCount].BuffIndex);
				bm.DecBuffPile(creatureNpcIndex, m_CurrentBuff[remainCurrentBuffLoopCount].CreatureBuffIndex);
			}
		}
	}

	//记录刚才添加的BUFF
	m_CurrentBuffIndexCount = buffIndexCount;
	memcpy(m_CurrentBuff, buffIndex, sizeof(m_CurrentBuff));
}

void BaseMonitor::SetupCurrentBuff(int npcIndex)
{
	BuffMgr& bm = BuffMgr::Singleton();

	for(int currentBuffLoopCount = 0; currentBuffLoopCount < m_CurrentBuffIndexCount; currentBuffLoopCount++)
	{
		int buffID = m_CurrentBuff[currentBuffLoopCount].BuffID;			
		unsigned long index = bm.AddNpcBuff( npcIndex, npcIndex, buffID );
		m_CurrentBuff[currentBuffLoopCount].CreatureBuffIndex = index;
	}
}