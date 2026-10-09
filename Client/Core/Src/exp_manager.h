//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-19
//      File_base        : exp_manager
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 经验管理器
//
//////////////////////////////////////////////////////////////////////

#ifndef _EXP_MANAGER_H_
#define _EXP_MANAGER_H_

class ExpManager
{
public:
	ExpManager();
	static ExpManager& Singleton();
	bool Init();

	int GetExpDistributeWeight(int level);
	void DistributeExp(int expNpcIndex, int expOwnerPlayerIndex);

private:
	int m_expDistributeFunction[MAX_LEVEL];
};

inline int ExpManager::GetExpDistributeWeight(int level)
{
	if (level > 0 && level <= MAX_LEVEL)
		return m_expDistributeFunction[level - 1];
	else
		return 1;
}

#endif// _EXP_MANAGER_H_