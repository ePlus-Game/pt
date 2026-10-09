#include "KCore.h"
#include "ConfigManager.h"
#include "KNpcTemplate.h"
#include "npc_statistic.h"

#define DEFAULT_NPC_STATISTIC_AUTO_SAVE_INTERVAL (3600)//默认NPC统计自动存盘间隔（秒）
#define SAVING_PROGRESS_TIMEOUT (2)//保存进度超时，即当有存盘正在进行时需要等待的时间（秒）

DWORD NpcStatistic::s_SavingProgressTimeout = 0;

void NpcStatistic::Init(int npcTemplateIndex)
{
	m_NpcTemplateIndex = npcTemplateIndex;
	m_LastSaveTimeout = ConfigManager::Singleton().GetGlobalVariable(global_var_log_npc_statistic_auto_save_period);	
	Clear();
}

void NpcStatistic::Active()
{	
	if (m_LastSaveTimeout <= 0)
	{
		//s_SavingProgressTimeout是为了分散统计信息存盘压力，
		//保证一段时间之内只有一个NpcStatistic在存盘
		if (s_SavingProgressTimeout <= 0)
		{
			s_SavingProgressTimeout = SAVING_PROGRESS_TIMEOUT;
			Save();
		}
		else
		{
			s_SavingProgressTimeout--;
		}
	}
	else
	{
		m_LastSaveTimeout--;
	}
}

void NpcStatistic::Save()
{
	ConfigManager& cm = ConfigManager::Singleton();

	m_LastSaveTimeout = cm.GetGlobalVariable(global_var_log_npc_statistic_auto_save_period);
	if (m_LastSaveTimeout <= 0)
		m_LastSaveTimeout = DEFAULT_NPC_STATISTIC_AUTO_SAVE_INTERVAL;

	if (TRUE == cm.GetGlobalVariable(global_var_log_npc_statistic))
	{
		if (m_KillByPlayerCount > 0)
		{
			LogEventParam npcStatisticEvent;
			npcStatisticEvent.event = log_event_npc_statistic_kill_by_player;
			snprintf(npcStatisticEvent.param1.data, sizeof(npcStatisticEvent.param1.data), "%d", m_NpcTemplateIndex);
			npcStatisticEvent.param4 = m_KillByPlayerCount;
			g_pLogSystem->Log(npcStatisticEvent);
		}
		
		if (m_KillPlayerCount > 0)
		{
			LogEventParam npcStatisticEvent;
			npcStatisticEvent.event = log_event_npc_statistic_kill_player;
			snprintf(npcStatisticEvent.param1.data, sizeof(npcStatisticEvent.param1.data), "%d", m_NpcTemplateIndex);
			npcStatisticEvent.param4 = m_KillPlayerCount;
			g_pLogSystem->Log(npcStatisticEvent);
		}

		ItemTemplateId invalidTemplateId;
		for (int i = 0; i < MAX_NPC_DROP_ITEM_TYPE_COUNT; i++)
		{
			NpcDropItemInfo& info = m_DropItemInfo[i];
			if (info.Id != invalidTemplateId)
			{
				LogEventParam npcStatisticEvent;
				npcStatisticEvent.event = log_event_npc_statistic_drop_item;
				snprintf(npcStatisticEvent.param1.data, sizeof(npcStatisticEvent.param1.data), "%d", m_NpcTemplateIndex);
				snprintf(npcStatisticEvent.param2.data, sizeof(npcStatisticEvent.param2.data), "%d,%d,%d,%d", info.Id.IDArray[0], info.Id.IDArray[1], info.Id.IDArray[2], info.Id.IDArray[3]);
				npcStatisticEvent.param4 = info.Count;
				g_pLogSystem->Log(npcStatisticEvent);
			}
			else
			{
				break;
			}
		}
	}

	Clear();
}

void NpcStatistic::Clear()
{
	m_KillByPlayerCount = 0;
	m_KillPlayerCount = 0;
	memset(m_DropItemInfo, 0, sizeof(m_DropItemInfo));
}

void NpcStatisticActive()
{
	//一秒活动一次
	static int s_count = 0;
	if (s_count / GAME_FPS >= 1)
	{
		s_count = 0;
	}
	else
	{
		s_count++;
		return;
	}

	for (int style = 0 ; style < MAX_NPCSTYLE; style++)
	{
		for (int level = 0; level < MAX_NPC_LEVEL; level++)
		{
			KNpcTemplate* pTemplate = g_pNpcTemplate[style][level];
			if (pTemplate)
			{
				pTemplate->GetStatisticInfo().Active();
			}
		}
	}
}

void NpcStatisticForceSave()
{
	for (int style = 0 ; style < MAX_NPCSTYLE; style++)
	{
		for (int level = 0; level < MAX_NPC_LEVEL; level++)
		{
			KNpcTemplate* pTemplate = g_pNpcTemplate[style][level];
			if (pTemplate)
			{
				pTemplate->GetStatisticInfo().Save();
			}
		}
	}
}