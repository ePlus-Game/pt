//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-9-8
//      File_base        : title
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 称号系统
//
//////////////////////////////////////////////////////////////////////

#ifndef _TITLE_H_
#define _TITLE_H_

#define MAX_TITLE_COUNT 64
#define MAX_UPGRADABLE_TITLE_LEVEL_COUNT 10
#define MAX_TITLE_NAME_LENGTH 17
#define MAX_TITLE_SHOW_NAME_LENGTH 256
#define MAX_TITLE_TIP_LENGTH 256
#define DEFAULT_SELECT_TITLE_INTERVAL 3
#define RANDOM_SELECT_TITLE_FALG -1

//称号状态
struct TitleState
{
	BYTE IsActive;
	union
	{
		DWORD ExpireTime;
		struct
		{
			WORD Level;
			WORD Value;
		} LevelInfo;
	};
};

//成长型称号信息
struct UpgradableTitleInfo
{
#ifndef _SERVER
	char Name[MAX_TITLE_NAME_LENGTH];
	char ShowName[MAX_TITLE_SHOW_NAME_LENGTH];
	char Tip[MAX_TITLE_TIP_LENGTH];
#endif
	int MaxValue;
	int BuffTemplateId;
};

//称号信息
struct TitleInfo
{
#ifndef _SERVER
	char Name[MAX_TITLE_NAME_LENGTH];
	char ShowName[MAX_TITLE_SHOW_NAME_LENGTH];
	char Tip[MAX_TITLE_TIP_LENGTH];
#endif
	bool Valid;
	TitleType Type;
	bool Deprecated;
	int LevelCount;
	int BuffTemplateId;
	UpgradableTitleInfo UpgradInfo[MAX_UPGRADABLE_TITLE_LEVEL_COUNT];
};

//称号管理器
class TitleManager
{
public:
	TitleManager();
	~TitleManager();

	void Init(int playerIndex);//初始化
	bool HasTitle(int titleIndex) const;//是否有某个称号
	bool GetTitleExpireTime(int titleIndex, DWORD& expireTime) const;//得到称号到期时间
	bool GetTitleLevelInfo(int titleIndex, WORD& currentLevel, WORD& currentValue) const;//得到称号级别信息
	void GetSelectedTitle(BYTE& selectedTitle, BYTE& titleLevel) const;//得到选中的称号

	void SelectTitle(int titleIndex);//选择显示称号
	
	static bool LoadSettings();//载入配置
	static TitleInfo* GetTitleInfo(int titleIndex);//得到称号信息
	static bool IsValidTitleIndex(int titleIndex);//是否是合法的称号序号

#ifdef _SERVER
	void Active();//活动一次
	void GrantTitle(int titleIndex);//授予称号
	void GrantTitle(int titleIndex, DWORD expireTime);//授予称号
	void GrantTitle(int titleIndex, WORD startLevel, WORD startValue);//授予称号
	void RevokeTitle(int titleIndex);//取消称号
	void SetTitleExpireTime(int titleIndex, DWORD newExpireTime);//设置称号到期时间
	void ModifyTitleExpireTime(int titleIndex, int changeExpireTime);//修改称号到期时间
	void ModifyTitleLevelInfo(int titleIndex, int changeValue);//修改称号活跃度
	void SetRandomSelectTitle(bool on);//设置为随机选择称号
	void OnLaunchPlayer();//玩家上线

	void SyncSelfTitle() const;//发送所有称号数据
	void UpdateSelfTitle(int titleIndex) const;//更新称号数据
	void SyncSelectTitleResult() const;//同步选中称号

	bool LoadState(const BYTE* pData, int dataSize);//载入称号状态
	bool SaveState(BYTE* pSaveBuff, int& buffSize);//保存称号状态
#else
	int GetSelfTitleInfo(UiTitleInfo* uiTitleInfoArray, int maxCount) const;//得到自己称号信息
	int GetDetailSelfTitleInfo(UiTitleInfo* uiTitleInfoArray, int maxCount) const;//得到自己称号信息
	int GetCurrentSelectedTitle() const;//得到选择的称号
	void ReceiveSyncData(BYTE* pMsg);//收到同步数据
	void ReceiveUpdateTitle(BYTE* pMsg);//收到更新数据
	void ReceiveSelectTitleResult(BYTE* pMsg);//收到选择称号
#endif

private:
	void UpdateSelectedTitle() const;//更新选中的称号

#ifdef _SERVER
	void RandomSelectTitle();//随机选择称号
	void CheckExpireTitle();//检查过期称号
	void CheckTitleBuff();//检查称号BUFF
#else
#endif

	TitleState m_State[MAX_TITLE_COUNT + 1];
	int m_SelectedTitle;
	int m_PlayerIndex;

#ifdef _SERVER
	bool m_RandomSelectTitle;
	DWORD m_NextRandomChangeTitleTime;
	DWORD m_NextCheckExpireTitleTime;
	int m_CurrenTitleBuffTemplateId;
#else
	int m_CurrentSelectTitleResult;
#endif

	static TitleInfo m_TitleInfo[MAX_TITLE_COUNT + 1];
};

inline bool TitleManager::IsValidTitleIndex(int titleIndex)
{
	return (titleIndex > 0 && titleIndex <= MAX_TITLE_COUNT);
}

inline bool TitleManager::HasTitle(int titleIndex) const
{
	if (!IsValidTitleIndex(titleIndex))
		return false;
	
	return (TRUE == m_State[titleIndex].IsActive);
}

inline bool TitleManager::GetTitleExpireTime(int titleIndex, DWORD& expireTime) const
{
	if (!IsValidTitleIndex(titleIndex))
		return false;
	
	if (TRUE == m_State[titleIndex].IsActive)
	{
		expireTime = m_State[titleIndex].ExpireTime;
		return true;
	}
	
	return false;
}

inline bool TitleManager::GetTitleLevelInfo(int titleIndex, WORD& currentLevel, WORD& currentValue) const
{
	if (!IsValidTitleIndex(titleIndex))
		return false;
	
	if (TRUE == m_State[titleIndex].IsActive)
	{
		currentLevel = m_State[titleIndex].LevelInfo.Level;
		currentValue = m_State[titleIndex].LevelInfo.Value;
		return true;
	}
	
	return false;
}

#ifdef _SERVER

inline void TitleManager::SetRandomSelectTitle(bool on)
{
	m_RandomSelectTitle = on;
	m_NextRandomChangeTitleTime = 0;
}

#else

#endif

inline TitleInfo* TitleManager::GetTitleInfo(int titleIndex)
{
	if (!IsValidTitleIndex(titleIndex))
		return NULL;
	
	if (m_TitleInfo[titleIndex].Valid)
		return &(m_TitleInfo[titleIndex]);
	else
		return NULL;
}

#endif// _TITLE_H_