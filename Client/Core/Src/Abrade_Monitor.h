//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-17
//      File_base        : Abrade_Monitor
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 装备磨损监视器
//
//////////////////////////////////////////////////////////////////////

#ifndef _ABRADE_MONITOR_H_
#define _ABRADE_MONITOR_H_

#ifndef _SERVER//客户端代码

#include "GameDataDef.h"

class AbradeMonitor
{
public:
	
	AbradeMonitor();
	~AbradeMonitor();

	void Init();

	void Update(const KEquipState* equipments);

private:

	EQUIP_DUR_STATE m_AbradeState[itempart_num];	

};

#endif //_SERVER

#endif //_ABRADE_MONITOR_H_