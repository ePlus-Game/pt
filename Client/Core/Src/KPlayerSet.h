#ifndef KPlayerSetH
#define	KPlayerSetH

#include "KLinkArray.h"
#include "KLevelUp.h"
#define		MAX_LEAD_LEVEL				100		// 最大统率力等级

typedef struct
{
	int		m_nPKValueScale;
	int		m_dwExp;
	int		m_nMoney;
	int		m_nItem;
	int		m_nEquip;
	int		m_nBagLoseCount;
	int		m_nBagLoseRate;
	int		m_nEquipLoseCount;
	int		m_nEquipLoseRate;
}KPK_DEATH_PUNISH_PARAM;

typedef struct
{
	int		m_nLevel;
	int		m_nLeadLevel;
} STONG_CREATE_PARAM;

typedef struct 
{
	int		m_nConstantValue1;
	int		m_nConstantValue2;
	int		m_nWeakStartLevel;
	int		m_nScalePercent;
} EXP_DEATH_SUB_PARAM;

class KPlayerSet
{
private:	// 用于优化查找速度
	KLinkArray		m_FreeIdx;				//	可用表
	KLinkArray		m_UseIdx;				//	已用表
	int				m_nListCurIdx;			// 用于 GetFirstPlayer 和 GetNextPlayer
#ifdef _SERVER
	unsigned long	m_ulNextSaveTime;
	unsigned long	m_ulDelayTimePerSave;	//1mins
	unsigned long	m_ulMaxSaveTimePerPlayer; //30mins
#endif
public:
	STONG_CREATE_PARAM		m_sTongParam;

#ifdef _SERVER
	KPK_DEATH_PUNISH_PARAM	m_sPKPunishParam[MAX_DEATH_PUNISH_PK_VALUE + 1];	// PK惩罚参数
	EXP_DEATH_SUB_PARAM	m_DeathWeakParam;
	int						m_nNormalPKTimeLong;
#endif

public:
	KPlayerSet();
	BOOL	Init();
	int		FindSame(DWORD dwID);
	int     FindPlayerByNpcID(DWORD dwID);  // 根据NPC ID查找与这个NPC对应的Player
	int		GetFirstPlayer();				// 遍历所有玩家第一步
	int		GetNextPlayer();				// 遍历所有玩家下一步(这支函数必须在上一支调用之后才能调用)
	int		GetOnlinePlayerCount() { return m_UseIdx.GetCount(); }

#ifdef	_SERVER
	void	Activate();
	int		Add(LPSTR szPlayerID, void* pGuid);
	void	PrepareRemove(int nClientIdx);
	void	RemoveQuiting(int nIndex);
	void	ProcessClientMessage(int nClient, const char* pChar, int nSize);
	void	ProcessPaysysMessage(int nClient, const char* pChar, int nSize);
	int		GetPlayerNumber() { return m_nNumPlayer; }
	BOOL	GetPlayerName(int nClient, char* szName);
	int		Broadcasting(char* pMessage, int nLen);
	void	SetSaveBufferPtr(void* pData);
	int		AttachPlayer(const unsigned long lnID, GUID* pGuid);
	int		GetPlayerIndexByGuid(GUID* pGuid);

	int		GetPlayerIndexByGuidOL(GUID* pGuid);
	int		GetPlayerIndexByName(const char *pAccName); // By account name
	int		GetPlayerIndexByPlayerName(const char *pPlayerName);
	void    SetSocialSaveSwitch(bool bValue);
	bool	GetSocialSaveSwitch();
#endif

private:
	int		FindFree();

#ifdef	_SERVER

public:
	void	ReloadWelcomeMsg();
	void	InitAutoSave();//自动存盘初始化
	void	ProcessAutoSave();//处理自动存盘

	void	AddMoney(DWORD money);//统计产出金钱
	void	DelMoney(DWORD money);//统计消耗金钱
	void	SaveMoneyStatistic();//金钱统计存盘
	bool	IsNeedSaveMoneyStatistic();//金钱统计是否需要存盘

private:
	void	InitMoneyStatistic();
	void*	m_pWelcomeMsg;
	int		m_nNumPlayer;

	bool	m_AutoSaveProcessing;//自动存盘是否正在进行
	int		m_AutoSaveNextTime;//下次自动存盘开始时间
	int		m_AutoSaveCurrentPlayerIndex;//当前自动存盘PlayerIndex
	int		m_AutoSaveStepInterval;//步间隔
	bool	m_SocialSaveSwitch;		

	DWORD	m_TotalRecentAddMoney;//最近所有玩家总计产出金钱
	DWORD	m_TotalRecentDelMoney;//最近所有玩家总结消耗金钱
	DWORD	m_NextSaveMoneyStatisticTime;//下次需要金钱统计存盘的时刻
#endif
};

extern KPlayerSet PlayerSet;


#ifdef _SERVER
inline void KPlayerSet::SetSocialSaveSwitch(bool bValue)
{
	m_SocialSaveSwitch = bValue;
}

inline bool KPlayerSet::GetSocialSaveSwitch()
{
	return m_SocialSaveSwitch;
}
#endif

#endif //KPlayerSetH
