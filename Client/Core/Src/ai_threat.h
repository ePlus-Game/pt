//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-26
//      File_base        : ai_threat
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 威胁监视器（仇恨列表）
//
//////////////////////////////////////////////////////////////////////

#ifndef _AI_THREAT_H_
#define _AI_THREAT_H_

#define MAX_THREAT_LIST_LENGTH 50//最大威胁列表长度

typedef struct tagNpcThreatInfo
{
	int NpcIndex;
	DWORD NpcId;
	int Threat;
	bool IsHidden;
} NpcThreatInfo, *PNpcThreatInfo;

class ThreatMonitor
{
public:

	ThreatMonitor();
	~ThreatMonitor();

	void Reset();//重置
	
	void RemoveEnemy(DWORD npcId);//删除敌人
	void AddEnemy(DWORD npcId, int npcIndex, int initThreat = 0);//添加敌人
	int GetEnemyCount() const;//得到敌人计数

	int GetEnemyListIndex(DWORD npcId) const;//得到NPC在敌人列表中的Index，若不在列表中，则返回-1
	bool IsInEnemyList(DWORD npcId) const;//判断是否在敌人列表里	
	const PNpcThreatInfo GetEnemyThreatInfo(int enemyListIndex);//得到指定位置的敌人的信息	

	void ChangeEnemyThreat(DWORD npcId, int npcIndex, int changeValue);//改变威胁值
	void HideEnemyThreat(DWORD npcId);//隐藏威胁值
	void ShowEnemyThreat(DWORD npcId);//显示威胁值

private:

	int m_EnemyCount;
	NpcThreatInfo m_ThreatList[MAX_THREAT_LIST_LENGTH + 1];
};

inline int ThreatMonitor::GetEnemyCount() const
{
	return m_EnemyCount;
}

inline bool ThreatMonitor::IsInEnemyList(DWORD npcId) const
{
	return (GetEnemyListIndex(npcId) >= 0);
}

#endif// _AI_THREAT_H_