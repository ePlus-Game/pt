//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-16
//      File_base        : Abrade_Table
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 装备磨损配置表读取
//
//////////////////////////////////////////////////////////////////////

#ifndef _ABRADE_TABLE_H_
#define _ABRADE_TABLE_H_

//装备磨损值的类型
enum AbradeValueType
{
	abrade_value_by_count = 0,
	abrade_value_by_percentage
};

//装备磨损信息
typedef struct tagAbradeInfo
{
	AbradeValueType ValueType;
	int Value[itempart_num];
} AbradeInfo;

//装备磨损表
class AbradeTable
{
public:
	
	AbradeTable();
	~AbradeTable();

	static AbradeTable& Singleton();

	bool Load();

	//得到磨损信息
	const AbradeInfo* GetAbrade(AbradeMode mode) const
	{
		if (mode >= 0 && mode < abrade_count)
			return &m_Abrade[mode];
		else
			return NULL;
	}

	//得到普通修理因子
	int GetNormalRepairFactor() const { return m_NormalRepairFactor; }

	//得到普通修理耐久上限下降因子
	int GetNormalRepairMaxDurDropFactor() const { return m_NormalRepairMaxDurDropFactor; }

	//得到特殊修理因子
	int GetSpecialRepairFactor() const { return m_SpecialRepairFactor; }

	//得到特殊修理耐久上限下降因子
	int GetSpecialRepairMaxDurDropFactor() const { return m_SpecialRepairMaxDurDropFactor; }

	//得到装备虚弱警戒值
	int GetWeakAlertValue() const { return m_WeakAlertValue; }

	//得到装备损毁警戒值
	int GetBrokenAlertValue() const { return m_BrokenAlertValue; }

	//得到耐久显示比例
	int GetDisplayRatio() const { return m_DisplayRatio; }

private:

	AbradeInfo m_Abrade[abrade_count];

	int m_NormalRepairFactor;

	int m_NormalRepairMaxDurDropFactor;

	int m_SpecialRepairFactor;

	int m_SpecialRepairMaxDurDropFactor;

	int m_WeakAlertValue;

	int m_BrokenAlertValue;

	int m_DisplayRatio;
};

#endif _ABRADE_TABLE_H_