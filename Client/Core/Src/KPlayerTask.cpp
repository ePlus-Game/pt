//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.cpp
// Date:	2002.10.05
// Code:	边城浪子
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KPlayer.h"
#include	"KSubWorld.h"
#include	"KNpcTemplate.h"
#include	"KItemGenerator.h"
#include	"KPlayerTask.h"

#ifndef _SERVER
#include	"CoreShell.h"
#endif
//---------------------------------------------------------------------------
//	功能：
//---------------------------------------------------------------------------
#ifndef _SERVER
QuestLog* QuestLog::self = NULL;

// static const char* GetNpcName(unsigned int uNpcTempliteID)
// {
// 	if (g_pNpcTemplate[uNpcTempliteID][0] == NULL)
// 	{
// 		g_pNpcTemplate[uNpcTempliteID][0] = new KNpcTemplate;
// 		g_pNpcTemplate[uNpcTempliteID][0]->InitNpcBaseData(uNpcTempliteID);
// 		g_pNpcTemplate[uNpcTempliteID][0]->m_NpcSettingIdx = uNpcTempliteID;
// 		g_pNpcTemplate[uNpcTempliteID][0]->m_bHaveLoadedFromTemplate = TRUE;
// 	}
// 	return g_pNpcTemplate[uNpcTempliteID][0]->Name;
// }
// 
// static const char* GetItemName(unsigned int uItemTempliteID)
// {
// 	static const char* unkName = "unknownitem";
// 	FIND_ITEMINDEX_PARAM itemidx;
// 	SpliteHashId(
// 		uItemTempliteID,
// 		itemidx.nGenre,	itemidx.nDetail, itemidx.nParticular );
// 	const KBASICPROP_ITEM* pTemplate = g_ItemGen.GetItemTemplate( itemidx.nGenre, itemidx.nDetail, itemidx.nParticular, 0 );
// 	if (pTemplate != NULL)
// 	{
// 		return pTemplate->szName;
// 	}
// 	else
// 	{
// 		return unkName;
// 	}
// }


bool KTaskTable::Load(const char *pFileName)
{
	if ( m_TaskTableFile.Load(pFileName) )
	{
		return true;
	}
	return false;
}

bool KTaskTable::GetTaskTitle(int nTaskId, char* szTaskGroup, char* szTaskTitle)
{
	char szBuf[64];
	char szTmp[32];
	int nLen = 0;
	sprintf(szBuf, "Task_%d", nTaskId);
	if ( !m_TaskTableFile.IsSectionExist(szBuf) )
	{
		return false;
	}

	if ( szTaskTitle != NULL )
	{
		if ( m_TaskTableFile.GetString(szBuf, "Title", "", szTmp, sizeof(szBuf)) )
		{
			nLen = strlen(szTmp);
			nLen = min(nLen, sizeof(szBuf) - 1);
			memcpy(szTaskTitle, szTmp, nLen);
			szTaskTitle[nLen] = '\0';
		}
		else
			szTaskTitle[0] = '\0';
	}

	if ( szTaskGroup != NULL )
	{
		if ( m_TaskTableFile.GetString(szBuf, "Group", "", szTmp, sizeof(szBuf)) )
		{
			nLen = strlen(szTmp);
			nLen = min(nLen, sizeof(szBuf) - 1);
			memcpy(szTaskGroup, szTmp, nLen);
			szTaskGroup[nLen] = '\0';
		}
		else
			szTaskGroup[0] = '\0';
	}
	
	return true;
}

bool KTaskTable::GetTaskInfo(int nTaskId, int nStep ,char* szIntro, char* szTaskStep, char* szTrace, unsigned int uParma[MAX_PARAM])
{
	char szSection[32];
	char szKey[32];
	char szTmp[1024];
	int nLen = 0;
	sprintf(szSection, "Task_%d", nTaskId);
	if ( !m_TaskTableFile.IsSectionExist(szSection) )
	{
		return false;
	}
	if ( szIntro != NULL )
	{
		if ( m_TaskTableFile.GetString(szSection, "Intro", "", szTmp, sizeof(szTmp)) )
		{
			nLen = strlen(szTmp);
			nLen = min(nLen, sizeof(szTmp) - 1);
			memcpy(szIntro, szTmp, nLen);
			szIntro[nLen] = '\0';
		}
		else
			szIntro[0] = '\0';
	}

	sprintf(szKey, "Step_%d", nStep);
	if ( m_TaskTableFile.GetString(szSection, szKey, "", szTmp, sizeof(szTmp)) )
	{
		sprintf(szTaskStep, szTmp,uParma[0], uParma[1], uParma[2], uParma[3], uParma[4]);
	}
	else
	{
		szIntro[0] = '\0';
	}

	
	sprintf(szKey, "Trace_%d", nStep);
	char szTmp32[256];
	if ( m_TaskTableFile.GetString(szSection, szKey, "", szTmp32, sizeof(szTmp32)) )
	{
		snprintf(szTrace, sizeof(szTmp32), szTmp32,uParma[0], uParma[1], uParma[2], uParma[3], uParma[4]);
	}
	else
	{
		szIntro[0] = '\0';
	}
//		if ( szTaskStep != NULL )
//		{
//			TranslateTaskNote(szTaskStep, szTmp, szArgument);
//		}
//		return true;

	return true;
}

bool GetNextString(const char * arg, int& argpos, char * result)
{
	char size = arg[argpos];
	if ( size > 0 )
	{
		argpos++;
		strncpy(result, arg+argpos, size);
		result[size] = 0;
		argpos += size;
		return true;
	}
	return false;
}
bool GetNextNumber(const char * arg, int& argpos, int& result)
{
	char size = arg[argpos];
	if ( size == -1 )
	{
		argpos ++;
		result = *((int*)(arg+argpos));
		argpos += 4;
		return true;
	}
	return false;
}

void KTaskTable::TranslateTaskNote(char * result, const char * buf, const char * arg)
{
	int i = 0;
	int writepos = 0;
	int argpos = 0;
	while ( buf[i] != 0 )  
	{
		if ( buf[i] == '%' )
		{
			i++;
			if ( buf[i] == 's' )
			{
				char tmp[256];
				if (GetNextString(arg, argpos, tmp))
				{
					int j = 0;
					
					while ( tmp[j] != 0 )
					{
						result[writepos] = tmp[j];
						j ++;
						writepos ++;
					}
				}
				i++;
				
			}
			else if ( buf[i] == 'd' )
			{
				int nresult = 0;
				if (GetNextNumber(arg, argpos, nresult))
				{
					char tmp[256];
					itoa(nresult, tmp, 10);
					int j = 0;
					
					while ( tmp[j] != 0 )
					{
						result[writepos] = tmp[j];
						j ++;
						writepos ++;
					}
				}
				i++;
			}
			else
			{
				result[writepos] = '%';
				writepos ++;
				result[writepos] = buf[i];
			}

		}
		else
		{
			result[writepos] = buf[i];
			i++;
			writepos ++;
		}
		
		
	}
	result[writepos] = 0;
}

//////////////////////////////////////////////////////////////////////////

QuestLog::QuestLog()
{
	bool r  = questTable.Load("\\settings\\taskinfo.ini");
	_ASSERT(r);
	Release();
}

QuestLog::~QuestLog()
{
	Release();
}

void QuestLog::Release()
{
	questInfoList.clear();
	uLastTime = 0;
}

void QuestLog::GetUIInfo(KQuestInfo& info)
{
	int questId = info.id;
	memset(&info, 0, sizeof(info));
	QuestList::iterator it = questInfoList.find(questId);
	if (it != questInfoList.end())
	{
		questTable.GetTaskTitle(questId, info.category, info.name);
		info.requirement[0][0].uID = 1;
		questTable.GetTaskInfo(questId, it->second.uStep, info.description, info.aim, info.requirement[0][0].name, it->second.uParam);
		info.id = questId;
	}
}

// 任务列表排序用
int comp(const void* a,const void* b)
{
    const KSimpleQuestInfo* pQuestA = (const KSimpleQuestInfo*)a;
    const KSimpleQuestInfo* pQuestB = (const KSimpleQuestInfo*)b;
    return strncmp(pQuestA->questTypeName, pQuestB->questTypeName, sizeof(pQuestA->questTypeName));
}

void QuestLog::GetQuestList(KSimpleQuestInfo* questListInfo,int maxCount)
{
	//if (uLastTime == 0)
	{
		_QUERY_QUEST_TITLE SYN;
		SYN.Protocol = c2s_quest_family;
		SYN.wProtocolSize = sizeof(SYN) - 1;
		SYN.ProtocolExtend = c2s_query_quest_title;
		SYN.dwQuestID = 0;
		if (g_pClient)
		{
			g_pClient->SendPackToServer(g_ConnectID, &SYN, sizeof(SYN));
		}
		uLastTime = 1;
	}

	int listidx = 0;
	QuestList::iterator it = questInfoList.begin();
	while (it !=  questInfoList.end())
	{
		questListInfo[listidx].questId = it->first;
		questTable.GetTaskTitle(it->first, questListInfo[listidx].questTypeName, questListInfo[listidx].questName);
		listidx++;
		if (listidx > maxCount)
		{
			return;
		}
		++it;
	}
	qsort(questListInfo,listidx,sizeof(questListInfo[0]),comp); 

}

void QuestLog::UpDateQuestProcessLastUpDateTime()
{
	uLastTime = SubWorld[0].m_dwCurrentTime;
}

void QuestLog::AbandonQuest(unsigned int nTaskID)
{
	QuestList::iterator it = questInfoList.find(nTaskID);
	if (it != questInfoList.end())
	{
		_ABANDON_QUEST SYN;
		SYN.Protocol = c2s_quest_family;
		SYN.wProtocolSize = sizeof(SYN) - 1;
		SYN.ProtocolExtend = c2s_abandon_quest;
		SYN.dwQuestID = nTaskID;
		if (g_pClient != NULL)
			g_pClient->SendPackToServer(g_ConnectID, &SYN, sizeof(SYN));
	}	
}

void QuestLog::RemoveQuest(unsigned int nTaskID)
{
	QuestList::iterator it = questInfoList.find(nTaskID);
	if (it != questInfoList.end())
	{
		questInfoList.erase(it);
		CoreDataChanged(GDCUI_QUEST_LIST_CHANGE, nTaskID, 0);	
	}
}


void QuestLog::UpDateQuest(unsigned int nTaskID, int nStep, unsigned int uParma[MAX_PARAM])
{
	UpDateQuestProcessLastUpDateTime();
	QuestList::iterator it = questInfoList.find(nTaskID);
	if (it != questInfoList.end())
	{
		it->second.uStep = nStep;
		memcpy(&(it->second.uParam) , (&uParma[0]), sizeof(int)*MAX_PARAM);
	    CoreDataChanged(GDCNI_QUEST_INFO_CHANGED, nTaskID, 0);
	}
	else
	{
		TaskProcessInfo info = {0};
		info.uID = nTaskID;
		info.uStep = nStep;
		memcpy(&(info.uParam) , (&uParma[0]),  sizeof(int)*MAX_PARAM);
		questInfoList[nTaskID] = info;
		CoreDataChanged(GDCUI_QUEST_LIST_CHANGE, nTaskID, 0);	
	}
}

#endif

#ifdef _SERVER
int			g_TaskGlobalValue[TASKGLOBALVALUENUM]; //全局的变量，用于服务器脚本系统


unsigned int KPlayerTask::TaskID2Idx[MAX_TASK_ID] = {0};
unsigned int KPlayerTask::TaskIdx2ID[MAX_TASK] = {0};
unsigned int KPlayerTask::uLastFreeIdx = 0;


//---------------------------------------------------------------------------
//	功能：构造函数
//---------------------------------------------------------------------------
KPlayerTask::KPlayerTask()
{
	Release();
}

//---------------------------------------------------------------------------
//	功能：清空
//---------------------------------------------------------------------------
void	KPlayerTask::Release()
{
	memset(nSave, 0, sizeof(nSave));
	memset(Info, 0 ,sizeof(Info));
}


void KPlayerTask::SetTaskNote(unsigned int uQuestID, unsigned int uIndex, int uValue,  bool bLoad)
{
	if ((uIndex == 0) && (uValue == -1 || uValue == -2))
	{
		for(unsigned int i=0 ; i< MAX_PROCESS_TASK; i++)
		{
			TaskProcessInfo& aTaskInfo = Info[i];
			if (aTaskInfo.uID == uQuestID)
			{
				if (IsValidPlayer(m_nPlayerIdx) && g_pLogSystem)
				{
					LogEventParam questEvent;
					questEvent.event = ((uValue == -1) ? log_event_quest_complete : log_event_quest_abort);
					questEvent.param1 = Player[m_nPlayerIdx].GetGUID();
					snprintf(questEvent.param2.data, sizeof(questEvent.param2.data), "%u %d", aTaskInfo.uID, aTaskInfo.uStep);
					questEvent.param2.data[sizeof(questEvent.param2.data) - 1] = 0;
					questEvent.param4 = Player[m_nPlayerIdx].GetOnlineTime();
					g_pLogSystem->Log(questEvent);
				}

				memset(&aTaskInfo, 0, sizeof(aTaskInfo));
				return;
			}
		}
	}
	else
	{
		if ( uIndex > MAX_PARAM)
			return;

		for(unsigned int i=0 ; i< MAX_PROCESS_TASK; i++)
		{
			TaskProcessInfo& aTaskInfo = Info[i];
			if (aTaskInfo.uID == uQuestID)
			{
				aTaskInfo.uID = uQuestID;
				if (uIndex == 0)
				{
					aTaskInfo.uStep = uValue;
				}
				else
				{
					aTaskInfo.uParam[uIndex - 1] = uValue;
				}
				return;
			}
		}

		for(unsigned int ii=0 ; ii< MAX_PROCESS_TASK; ii++)
		{
			TaskProcessInfo& aTaskInfo = Info[ii];
			if (aTaskInfo.uID == 0)
			{
				aTaskInfo.uID = uQuestID;
				if (uIndex == 0)
				{
					aTaskInfo.uStep = uValue;
				}
				else
				{
					aTaskInfo.uParam[uIndex - 1] = uValue;
				}

				if (!bLoad && IsValidPlayer(m_nPlayerIdx) && g_pLogSystem)
				{
					LogEventParam questEvent;
					questEvent.event = log_event_quest_accept;
					questEvent.param1 = Player[m_nPlayerIdx].GetGUID();
					snprintf(questEvent.param2.data, sizeof(questEvent.param2.data), "%u %d", aTaskInfo.uID, aTaskInfo.uStep);
					questEvent.param2.data[sizeof(questEvent.param2.data) - 1] = 0;
					questEvent.param4 = Player[m_nPlayerIdx].GetOnlineTime();
					g_pLogSystem->Log(questEvent);
				}

				return;
			}
		}
	}
}

void KPlayerTask::SendTaskInfo(unsigned int nPlayer)
{
	for(unsigned int i=0 ; i< MAX_PROCESS_TASK; i++)
	{
		const TaskProcessInfo& aTaskInfo = Info[i];
		if ( aTaskInfo.uID!= 0)
		{
			PLAYER_SCRIPTACTION_SYNC UiInfo;
			UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
			UiInfo.m_bUIId = UI_NOTEINFO;
			UiInfo.m_bParam2 = 1;
			UiInfo.m_bOptionNum = 0;
			UiInfo.m_nParam =  aTaskInfo.uID; // task id
			UiInfo.m_bOptionNum = aTaskInfo.uStep & 0xFF;
			UiInfo.m_bParam1 = aTaskInfo.uStep >> 8; // task step
			UiInfo.m_nBufferLen = sizeof(aTaskInfo.uParam);
			memcpy(UiInfo.m_pContent, aTaskInfo.uParam, UiInfo.m_nBufferLen);
			Player[nPlayer].DoScriptAction(&UiInfo);
		}
	}
}

//---------------------------------------------------------------------------
//	功能：保存
//---------------------------------------------------------------------------
int		KPlayerTask::Save(BYTE * pRoleBuffer )
{
	unsigned int uSaveCount = 0;
	unsigned int uSaveSize = 0;

	// 保存任务状态
	unsigned int *pSaveBuffer = (unsigned int *)pRoleBuffer;
	KPlayerTask::TaskState *pStates = (KPlayerTask::TaskState *)(pSaveBuffer + 1);
	for (unsigned int uTaskIdx = 0; uTaskIdx < MAX_TASK; uTaskIdx++)
	{
		if (nSave[uTaskIdx] != 0)
		{
			pStates[uSaveCount].uID = TaskIdx2ID[uTaskIdx];
			pStates[uSaveCount].uValue = nSave[uTaskIdx];
			uSaveCount++;
		}
	}
	*pSaveBuffer = uSaveCount;
	uSaveSize = (sizeof(KPlayerTask::TaskState) * uSaveCount + sizeof(unsigned int));
	uSaveCount = 0;

	// 保存任务进展
	pSaveBuffer = (unsigned int *)(pRoleBuffer + uSaveSize);
	KPlayerTask::TaskProcessSave *pProcess = (KPlayerTask::TaskProcessSave *)(pSaveBuffer + 1);
	for (unsigned int uIndex = 0; uIndex < MAX_PROCESS_TASK; uIndex++ )
	{
		for (unsigned int uKind = 0; uKind < max_objective_type; uKind++ )
		{
			for (unsigned int uObjective = 0; uObjective < MAX_OBJECTIVE; uObjective++ )
			{
				const unsigned int uQuestID = Info[uIndex].uID;
				if (uQuestID != 0)
				{
					pProcess[uSaveCount].uQuestID = uQuestID;
					pProcess[uSaveCount].uProcessCode = 0;
					pProcess[uSaveCount].uValue = Info[uIndex].uStep;
					uSaveCount++;
					for (unsigned int uParamIdx = 0; uParamIdx < MAX_PARAM; uParamIdx++)
					{
						const unsigned int uParam = Info[uIndex].uParam[uParamIdx];
						if (uParam != 0)
						{
							pProcess[uSaveCount].uQuestID = uQuestID;
							pProcess[uSaveCount].uProcessCode = uParamIdx + 1;
							pProcess[uSaveCount].uValue = uParam;
							uSaveCount++;
						}
					}
				}
			}
		}
	}
	*pSaveBuffer = uSaveCount;
	uSaveSize += (sizeof(KPlayerTask::TaskProcessSave) * uSaveCount + sizeof(unsigned int));
	
	return uSaveSize;
}
//---------------------------------------------------------------------------
//	功能：加载
//---------------------------------------------------------------------------
int		KPlayerTask::Load(BYTE * pRoleBuffer, int nSize )
{
	this->Release();
	if (nSize < sizeof(unsigned int) + sizeof(unsigned int)) //最少记录了2个类别的Count
	{
		return 0;
	}

	unsigned int uTaskStateCount = *(unsigned int *)pRoleBuffer;

	const KPlayerTask::TaskState *pStates = (const KPlayerTask::TaskState *)((unsigned int *)pRoleBuffer + 1);
	
	if (nSize < sizeof(unsigned int) + (uTaskStateCount * sizeof(KPlayerTask::TaskState)) )
		return 0;

	for (unsigned int uLoopState = 0; uLoopState < uTaskStateCount; uLoopState++)
	{
		this->operator [](pStates[uLoopState].uID) = pStates[uLoopState].uValue;
	}

	pRoleBuffer += (sizeof(KPlayerTask::TaskState) * uTaskStateCount + sizeof(unsigned int));

	if (nSize <   sizeof(unsigned int) + (uTaskStateCount * sizeof(KPlayerTask::TaskState)) 
		        + sizeof(unsigned int) )
		return 0;

	unsigned int uTaskProcessCount = *(unsigned int *)pRoleBuffer;
	
	if (nSize <   sizeof(unsigned int) + (uTaskStateCount * sizeof(KPlayerTask::TaskState))
		+ sizeof(unsigned int) + uTaskProcessCount * sizeof(KPlayerTask::TaskProcessSave))
		return 0;

	const KPlayerTask::TaskProcessSave *pProcess = (const KPlayerTask::TaskProcessSave *)((unsigned int *)pRoleBuffer + 1);
	for (unsigned int uLoopProcess = 0; uLoopProcess < uTaskProcessCount; uLoopProcess++)
	{
		const KPlayerTask::TaskProcessSave& ProcessRef = pProcess[uLoopProcess];
		SetTaskNote(ProcessRef.uQuestID, ProcessRef.uProcessCode, ProcessRef.uValue, true);
	}

	return 1;
}

#endif // #ifdef _SERVER