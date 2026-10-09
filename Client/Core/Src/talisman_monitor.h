//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-22
//      File_base        : talisman_monitor
//      File_ext         : .h
//      Author           : ÐìÏþ¸Õ
//      Description      : ·¨±¦¼àÊÓÆ÷
//
//////////////////////////////////////////////////////////////////////

#ifndef _TALISMAN_MONITOR_H_
#define _TALISMAN_MONITOR_H_

#include "ItemCommonDef.h"
#include "talisman_manager.h"
#ifdef _SERVER
#include "BaseMonitor.h"
#endif

//·¨±¦¼àÊÓÆ÷
class TalismanMonitor
#ifdef _SERVER
 : public BaseMonitor
#endif
{
public:
	TalismanMonitor();
	~TalismanMonitor();

	void Init();
	void Update(int npcIndex, const KEquipState* equipments);
	bool IsEnchaseEnabled(int enchasePos);

private:
	EnchaseActiveInfo m_ActiveInfo;
};

inline bool TalismanMonitor::IsEnchaseEnabled(int enchasePos)
{
	if (enchasePos >= 0 && enchasePos < TM_HOLE_NUM)
	{
		return m_ActiveInfo.IsActive[enchasePos];
	}
	else
	{
		return false;
	}
}

#endif// _TALISMAN_MONITOR_H_