//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-26
//      File_base        : ai_threat
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 威胁监视器（仇恨列表）
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include <memory.h>
#include <string.h>
#include "ai_threat.h"

ThreatMonitor::ThreatMonitor()
{
	Reset();
}

ThreatMonitor::~ThreatMonitor()
{
	Reset();
}

void ThreatMonitor::Reset()
{
	m_EnemyCount = 0;
	memset(m_ThreatList, 0, sizeof(m_ThreatList));
}

void ThreatMonitor::AddEnemy(DWORD npcId, int npcIndex, int initThreat /* = 0 */)
{
	if (!IsInEnemyList(npcId))
	{
		m_ThreatList[m_EnemyCount].NpcIndex = npcIndex;
		m_ThreatList[m_EnemyCount].NpcId = npcId;
		m_ThreatList[m_EnemyCount].Threat = 0;
		m_ThreatList[m_EnemyCount].IsHidden = false;
		m_EnemyCount++;

		ChangeEnemyThreat(npcId, npcIndex, initThreat);

		//如果列表已经满了，则需要移除最后一个，保证整个列表的长度不超过MAX_THREAT_LIST_LENGTH
		if (m_EnemyCount > MAX_THREAT_LIST_LENGTH)
		{
			m_EnemyCount--;
			memset(m_ThreatList + MAX_THREAT_LIST_LENGTH, 0, sizeof(NpcThreatInfo));
		}
	}
}

void ThreatMonitor::RemoveEnemy(DWORD npcId)
{
	int enemyListIndex = GetEnemyListIndex(npcId);
	if (enemyListIndex >= 0)
	{
		memmove(m_ThreatList + enemyListIndex , m_ThreatList + enemyListIndex + 1, sizeof(NpcThreatInfo) * (m_EnemyCount - enemyListIndex - 1));
		m_EnemyCount--;
		memset(m_ThreatList + m_EnemyCount, 0, sizeof(NpcThreatInfo));
	}
}

int ThreatMonitor::GetEnemyListIndex(DWORD npcId) const
{
	for (int threatListLoopCount = 0; threatListLoopCount < m_EnemyCount; threatListLoopCount++)
	{
		if (m_ThreatList[threatListLoopCount].NpcId == npcId)
		{
			return threatListLoopCount;
		}
	}

	return -1;
}

const PNpcThreatInfo ThreatMonitor::GetEnemyThreatInfo(int enemyListIndex)
{
	if (enemyListIndex >= 0 && enemyListIndex < m_EnemyCount)
	{
		return &(m_ThreatList[enemyListIndex]);
	}
	else
	{
		return NULL;
	}
}

void ThreatMonitor::ChangeEnemyThreat(DWORD npcId, int npcIndex, int changeValue)
{
	int enemyListIndex = GetEnemyListIndex(npcId);
	if (enemyListIndex >= 0)
	{
		if (changeValue == 0)
			return;

		int tempValue = m_ThreatList[enemyListIndex].Threat + changeValue;
		if (tempValue < 0)
		{
			tempValue = 0;
		}

		NpcThreatInfo info;
		memcpy(&info, m_ThreatList + enemyListIndex, sizeof(NpcThreatInfo));
		info.Threat = tempValue;

		int newPos = enemyListIndex;
		if (changeValue > 0)
		{
			while (newPos > 0 && tempValue > m_ThreatList[newPos - 1].Threat)
			{
				newPos--;
			}

			memmove(m_ThreatList + newPos + 1, m_ThreatList + newPos, sizeof(NpcThreatInfo) * (enemyListIndex - newPos));
			memcpy(m_ThreatList + newPos, &info, sizeof(NpcThreatInfo));
		}
		else
		{
			while (newPos + 1 < m_EnemyCount && tempValue < m_ThreatList[newPos + 1].Threat)
			{
				newPos++;
			}

			memmove(m_ThreatList + enemyListIndex, m_ThreatList + enemyListIndex + 1, sizeof(NpcThreatInfo) * (newPos - enemyListIndex));
			memcpy(m_ThreatList + newPos, &info, sizeof(NpcThreatInfo));
		}		
	}
	else
	{
		AddEnemy(npcId, npcIndex, (changeValue > 0 ? changeValue : 0));
	}
}

void ThreatMonitor::HideEnemyThreat(DWORD npcId)
{
	const PNpcThreatInfo pThreatInfo = GetEnemyThreatInfo(GetEnemyListIndex(npcId));
	if (pThreatInfo != NULL)
	{
		pThreatInfo->IsHidden = true;
	}
}

void ThreatMonitor::ShowEnemyThreat(DWORD npcId)
{
	const PNpcThreatInfo pThreatInfo = GetEnemyThreatInfo(GetEnemyListIndex(npcId));
	if (pThreatInfo != NULL)
	{
		pThreatInfo->IsHidden = false;
	}
}

