//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-8
//      File_base        : ArmorSet_Monitor
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 套装监视器
//
//////////////////////////////////////////////////////////////////////

#ifndef _ARMORSET_MONITOR_H_
#define _ARMORSET_MONITOR_H_

#include "KItem.h"
#ifdef _SERVER
#include "BaseMonitor.h"
#endif

//////////////////////////////////////////////////////////////////////
//名称：ArmorSetMonitor（套装监视器）
//描述：用来检测已经装备的套装，如果是服务器端，还需要管理相应的套装效
//      果（激活/取消）。
//使用：首先使用Init()初始化；角色装备改变后使用Update()函数更新套装信
//      息，并取消原有的和重新激活新的套装效果；使用GetArmorSetDesc取得
//      套装描述。
//////////////////////////////////////////////////////////////////////
class ArmorSetMonitor
#ifdef _SERVER
 : public BaseMonitor
#endif
{
public:

	ArmorSetMonitor();
	~ArmorSetMonitor();

	//初始化
	void Init();

	//更新套装
	void Update(int npcIndex, const KEquipState* equipments);

#ifndef _SERVER

	//得到装备的套装描述
	//itemIndex: 装备在Item中的index
	//descBuff: 描述字符串缓存
	void GetArmorSetDesc(KItem* pItem, char* descBuff) const;
	
#endif

private:
	
	//套装数量
	int m_ArmorSetCount;

	//套装情况
	ArmorSetState m_ArmorSetState[itempart_num];

};

#endif//_ARMORSET_MONITOR_H_