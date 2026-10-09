//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-23
//      File_base        : BasicProperty_Monitor
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 装备基本属性（基本、升级、加持属性）监视器
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASICPROPERTY_MONITOR_H_
#define _BASICPROPERTY_MONITOR_H_

#include "ItemCommonDef.h"
#include "BaseMonitor.h"

class BasicPropertyMonitor : public BaseMonitor
{
public:

	BasicPropertyMonitor();
	~BasicPropertyMonitor();

	void Init();

	void Update(int npcIndex, const KEquipState* equipments);

};

#endif// _BASICPROPERTY_MONITOR_H_