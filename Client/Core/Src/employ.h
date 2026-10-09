//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-03-19 12:13
//      File_base        : employ
//      File_ext         : h
//      Author           : 徐晓刚
//      Description      : 雇用系统
//
//////////////////////////////////////////////////////////////////////
#ifndef EMPLOY_H
#define EMPLOY_H

#define ROLE_TYPE_COUNT 3
#define SKILL_SERIES_COUNT 3
#define EMPLOY_UNIT_SECONDS 3600
#define MAX_EXP_SETTING_COUNT 5

class Employee
{
public:
	Employee();
	~Employee();

	void Init();
	void SetPlayerIndex(int playerIndex);
	void Active();
	int GetNpcIndex() const;
	bool IsExist() const;
	int Employ(int employeeNpcIndex, int type, DWORD employTime, DWORD salary);
	int Fire();
	DWORD GetSalary() const;
	int GetEmployType() const;
	DWORD GetUnpayedTime() const;

private:
	int SelectUsableSkill();
	void Pay();
	bool NeedPay() const;
	bool IsExpired() const;

	DWORD m_NextActiveTime;
	DWORD m_NextPayTime;
	int m_EmployType;
	DWORD m_EmploySalary;
	int m_PlayerIndex;
	int m_EmployeeNpcIndex;
	DWORD m_EmployExpireTime;
};

#define MAX_EMPLOYEE_SKILL_COUNT 2
#define MAX_EMPLOYEE_BUFF_COUNT 2

struct EmployeeSetting
{
	int Skills[MAX_EMPLOYEE_SKILL_COUNT];
	int Buffs[MAX_EMPLOYEE_BUFF_COUNT];
};

struct ExpSetting
{
	int ExpRewardPlus;
	int TimeElapseRate;
};

struct _SearchEmployeeDBHeader : _DBProcHeader 
{
	int employType;
	int offset;
};

class EmployCenter
{
public:
	EmployCenter();
	~EmployCenter();

	static EmployCenter& Singleton();

	bool Load();
	void Active();

	int SearchEmployee(int playerIndex, HireReqData& filter);//查找雇用列表
	int Post(int playerIndex, int type, int salary, int rateIndex);//发布
	int Cancel(int playerIndex);//取消（佣兵上线）
	int Employ(int playerIndex, const char* employeeName, int employTime = EMPLOY_UNIT_SECONDS);//雇用
	int Pay(int playerIndex, int employeeNpcIndex);//付费
	int Fire(int playerIndex, int employeeNpcIndex);//解雇

	void SearchEmployDbOpComplete(int playerIndex, IProcRet* pRet);
	void PostDbOpComplete(int playerIndex, IProcRet* pRet);
	void CancelDbOpComplete(int playerIndex, IProcRet* pRet);
	void EmployDbOpComplete(int playerIndex, IProcRet* pRet);
	void PayDbOpComplete(int playerIndex, IProcRet* pRet);
	void FireDbOpComplete(int playerIndex, IProcRet* pRet);

	DWORD GetPayInterval() const;

private:
	void InitEmployee(int employeeNpcIndex, int roleType, int skillSeries, int employType);
	DWORD CalcPayMoney(DWORD intervalSeconds, DWORD salary) const;
	DWORD CalcRewardExp(int employeeLevel, DWORD beEmployedTime, int employTimeElapseRate) const;
	DWORD CalcRewardMoney(DWORD beEmployedTime, DWORD salary) const;
	int GetEmployTimeElapseRate(int index) const;
	int GetRewardExpRate(int employTimeElapseRate) const;
	bool IsValidEmployType(int type) const;
	bool IsValidRateIndex(int index) const;
	bool IsValidSalary(int secondSalary) const;
	void SendResultToClient(int playerIndex, int returnCode);
	const ExpSetting* GetExpSetting(int index) const;
	const EmployeeSetting* GetEmployeeSetting(int roleType, int skillSeries, int employType) const;
	DWORD GetLevelExp(int level) const;
	void CheckNoTimeEmploy();

	DWORD m_PayInterval;
	int m_FightRequireLevelMin;
	int m_FightRequireLevelMax;
	DWORD m_LevelExp[MAX_LEVEL];
	ExpSetting m_ExpSettings[MAX_EXP_SETTING_COUNT];
	EmployeeSetting m_EmployeeSettings[ROLE_TYPE_COUNT][SKILL_SERIES_COUNT][HT_TYPE_COUNT];
	DWORD m_NextCheckTime;
};

inline int Employee::GetNpcIndex() const
{
	return m_EmployeeNpcIndex;
}

inline bool Employee::IsExist() const
{
	return (m_EmployeeNpcIndex > 0);
}

inline void Employee::SetPlayerIndex(int playerIndex)
{
	m_PlayerIndex = playerIndex;
}

inline DWORD Employee::GetSalary() const
{
	return m_EmploySalary;
}

inline int Employee::GetEmployType() const
{
	return m_EmployType;
}

inline DWORD EmployCenter::CalcPayMoney(DWORD intervalSeconds, DWORD salary) const
{
	return intervalSeconds * salary;
}

inline DWORD EmployCenter::CalcRewardMoney(DWORD beEmployedTime, DWORD salary) const
{
	return CalcPayMoney(beEmployedTime, salary);
}

inline const EmployeeSetting* EmployCenter::GetEmployeeSetting(int roleType, int skillSeries, int employType) const
{
	if (roleType >= 0 && roleType < 3 && skillSeries >= -1 && skillSeries < 2 && employType >=0 && employType < HT_TYPE_COUNT)
		return &(m_EmployeeSettings[roleType][skillSeries + 1][employType]);
	else
		return NULL;
}

inline const ExpSetting* EmployCenter::GetExpSetting(int index) const
{
	if (index >= 0 && index < MAX_EXP_SETTING_COUNT)
		return &(m_ExpSettings[index]);
	else
		return NULL;
}

inline DWORD EmployCenter::GetPayInterval() const
{
	return m_PayInterval;
}

inline DWORD EmployCenter::GetLevelExp(int level) const
{
	if (level > 0 && level <= MAX_LEVEL)
		return m_LevelExp[level - 1];
	else
		return 0;
}

#endif EMPLOY_H