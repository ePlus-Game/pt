//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTask.h
// Date:	2002.10.05
// Code:	边城浪子
// Desc:	PlayerTask Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTASK_H
#define KPLAYERTASK_H

enum
{
	c2s_query_npc_quest_state,
	c2s_query_quest_title,
	c2s_query_quest_detail_log,
	c2s_query_quest_detail_log_and_show,
	c2s_query_quest_detail_cache,
    c2s_abandon_quest,
};

enum
{
	s2c_sync_npc_quest_state,
	s2c_sync_quest_title,
	s2c_sync_quest_detail_log,
	s2c_sync_quest_detail_log_and_show,
	s2c_sync_quest_detail_cache,
	s2c_sync_quest_process,
    s2c_sync_quest_state,
	s2c_remove_quest,
};

#define		MAX_TASK_ID			65535
#define		MAX_TASK			1024
#define		MAX_PROCESS_TASK	20
#define		MAX_PARAM			5

//////////////////////////////////////////////////////////////////////////
// 任务详细信息的客户端缓存
struct TaskProcessInfo
{
	unsigned int uID;
	unsigned int uStep;
	unsigned int uParam[MAX_PARAM];
};


#ifndef _SERVER
class KTaskTable
{
public:
	KTaskTable(){};
	~KTaskTable(){};
		
	bool			Load(const char *pFileName);
	bool			GetTaskTitle(int nTaskId, char* szTaskGroup, char* szTaskTitle);
	bool			GetTaskInfo(int nTaskId, int nStep ,char* szIntro, char* szTaskStep, char* szTrace, unsigned int uParma[MAX_PARAM]);
protected:
private:
	void			TranslateTaskNote(char * result, const char * buf, const char * arg);
	KIniFile		m_TaskTableFile;
	
};

enum query_state {query_quest_removed, query_quest_new, query_quest_pending, query_quest_ready };


class QuestLog
{
	typedef std::map<unsigned int, TaskProcessInfo>	QuestList;
public:
	QuestLog();
	~QuestLog();
	static QuestLog * GetInstance()
	{
		if(self == NULL)
		{
	        self = new QuestLog;
		}
        return self;
	}
	void Release();
	void AbandonQuest(unsigned int nTaskID);
	void UpDateQuest(unsigned int nTaskID, int nStep, unsigned int uParma[MAX_PARAM]);
	void RemoveQuest(unsigned int nTaskID);
	void GetUIInfo(KQuestInfo& info);
	void GetQuestList(KSimpleQuestInfo* questListInfo,int maxCount);
	unsigned int GetQuestProcessLastUpDateTime() const {return uLastTime;}
	void UpDateQuestProcessLastUpDateTime();
protected:
	static QuestLog* self;
private:
	KTaskTable questTable;
	QuestList questInfoList;
	unsigned int uLastTime;
};

#define INVALID_QUEST_IDX	0xFFFFFFFF
#define	QUESTCACHE_SIZE 20 
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _SERVER
#define TASKGLOBALVALUENUM 5000
extern int		g_TaskGlobalValue[TASKGLOBALVALUENUM];


class KPlayerTask
{
	friend class KPlayer;
public:
	KPlayerTask();
	unsigned int& operator[](unsigned int uIdx)
	{
		static unsigned int uDummy;
		if (uIdx < MAX_TASK_ID)
		{
			unsigned int uRet = KPlayerTask::ID2Idx(uIdx);
			if (uRet > 0)
			{
				return nSave[uRet];
			}
		}
		uDummy = 0xFFFFFFFF;
		return uDummy;
	}

	void SetTaskNote(unsigned int uQuestID, unsigned int uIndex, int uValue, bool bLoad = false);
	void SendTaskInfo(unsigned int nPlayer);
	void		Release();							// 清空


	int			Save(BYTE * pRoleBuffer);
	int			Load(BYTE * pRoleBuffer, int nSize );
private:

#pragma pack(push, 1)

	struct TaskState 
	{
		unsigned int	uID;
		unsigned int	uValue;
	};

	struct TaskProcessSave
	{
		unsigned int	uQuestID;
		unsigned int	uProcessCode;
		unsigned int	uValue;
	};

#pragma pack(pop)

	static unsigned int ID2Idx(unsigned int uID)
	{
		unsigned int uIdx = TaskID2Idx[uID];
		if (uIdx == 0)
		{
			if (uLastFreeIdx < MAX_TASK - 1)
			{
				uIdx = ++uLastFreeIdx;
				TaskID2Idx[uID]		= uIdx;
				TaskIdx2ID[uIdx]	= uID;
			}
		}
		return uIdx;
	}
	int				m_nPlayerIdx;
	unsigned int	nSave[MAX_TASK];					// 用于记录任务是否完成，须保存到数据库
	TaskProcessInfo	Info[MAX_PROCESS_TASK];

	static		unsigned int TaskID2Idx[MAX_TASK_ID];
	static		unsigned int TaskIdx2ID[MAX_TASK];
	static		unsigned int uLastFreeIdx;
};

#endif // #ifdef _SERVER

#endif
