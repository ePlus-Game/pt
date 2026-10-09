//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-06-25
//      File_base        : charm_monitor
//      File_ext         : .h
//      Author           : ÐìÏþ¸Õ
//      Description      : »¤Éí·û¼àÊÓÆ÷
//
//////////////////////////////////////////////////////////////////////

#ifndef _CHARM_MONITOR_H_
#define _CHARM_MONITOR_H_

#include "KItemList.h"
#include "BaseMonitor.h"

class KItemList;

class CharmMonitor : public BaseMonitor
{
public:

	CharmMonitor();
	~CharmMonitor();

	void Init();

	void Update(int npcIndex, KItemList& itemList);
};

#endif// _CHARM_MONITOR_H_