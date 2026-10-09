//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-8
//      File_base        : Yao_Monitor
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 爻装监视器
//
//////////////////////////////////////////////////////////////////////

#ifndef _YAO_MONITOR_H_
#define	_YAO_MONITOR_H_

#include "ItemCommonDef.h"
#include "BaseMonitor.h"

//////////////////////////////////////////////////////////////////////
//名称：YaoMonitor（爻装监视器）
//描述：用来检测已经装备的爻装，如果是服务器端，还需要管理相应的爻装效
//      果（激活/取消）。
//使用：首先使用Init()初始化；角色装备改变后使用Update()函数更新爻装信
//      息，并取消原有的和重新激活新的爻装效果；使用GetGuaStates取得所
//      有卦位的基本信息，使用GetGusDesc取得指定卦位的描述，使用GetMutiGuaDesc
//      取得多卦描述，使用GetYaoSetDesc取得爻装（卦位组合）描述。
//////////////////////////////////////////////////////////////////////
class YaoMonitor
#ifdef _SERVER
 : public BaseMonitor
#endif
{
public:

	YaoMonitor();
	~YaoMonitor();

	//初始化
	void Init();

	//更新爻装
	void Update(int npcIndex, const KEquipState* equipments);

	//得到所有卦位的状态（基本信息）
	const GuaState* GetGuaStates()
	{
		return m_GuaState;
	}

#ifndef _SERVER

public:

	//得到指定位置的卦的描述
	//guaPos: 卦的位置
	//descBuff: 描述字符串缓存
	void GetGuaDesc(GUA_POS guaPos, char* descBuff);

private:

	//得到多卦的描述
	//descBuff: 描述字符串缓存。
	void GetMutiGuaDesc(char* descBuff);

	//得到卦组合的描述
	//descBuff: 描述字符串缓存。
	void GetYaoSetDesc(char* descBuff);

#endif

private:
	
	//计算卦
	inline int CaculateGua(int yao1, int yao2, int yao3)
	{
		return (yao1 * 4 + yao2 * 2 + yao3);
	}
	
private:

	//卦的状态
	GuaState m_GuaState[gua_pos_count];

	//多个同种卦（卦编号和等级都相同）
	MutiGuaState m_MutiGuaState[gua_pos_count];
	
	//多个同种卦的数量
	int m_MutiGuaCount;

	//爻装编号（卦位组合编号）
	int	m_YaoSetID;
	
};

#endif//_YAO_MONITOR_H_