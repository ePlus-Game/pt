//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-17
//      File_base        : Abrade_Monitor
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 装备磨损监视器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"

#ifndef _SERVER//客户端代码

#include "KItem.h"
#include "Abrade_Table.h"
#include "Abrade_Monitor.h"
#include "CoreShell.h"

AbradeMonitor::AbradeMonitor()
{
	Init();
}

AbradeMonitor::~AbradeMonitor()
{
}

void AbradeMonitor::Init()
{
	memset(m_AbradeState, estate_normal, sizeof(m_AbradeState));
}

void AbradeMonitor::Update(const KEquipState* equipments)
{
	if (equipments == NULL)
		return;

	bool needToAlert = false;
	int weakValue = AbradeTable::Singleton().GetWeakAlertValue();
	int brokenValue = AbradeTable::Singleton().GetBrokenAlertValue();
	for(int i = 0; i < itempart_num; i++)
	{
		if (equipments[i].nEquipIdx > 0)
		{
			KItem& item = Item[equipments[i].nEquipIdx];
			int condition = item.GetDurability() * 100 / ((item.GetMaxDurability() > 0) ? item.GetMaxDurability() : 1);
			EQUIP_DUR_STATE& state = m_AbradeState[i];
			
			if (condition > weakValue)
			{
				if (state != estate_normal)
				{
					state = estate_normal;
					needToAlert = true;					
				}				
			}
			else if (condition > brokenValue)
			{
				if (state != estate_yellow)
				{
					state = estate_yellow;
					needToAlert = true;
				}
			}
			else
			{
				if (state != estate_red)
				{
					state = estate_red;
					needToAlert = true;
				}
			}
		}
		else
		{
			if (m_AbradeState[i] != estate_normal)
			{
				m_AbradeState[i] = estate_normal;
				needToAlert = true;					
			}	
		}
	}

	if (needToAlert)//需要通知装备破损面板
	{
		CoreDataChanged(GDCNI_ALERT_DURA, (unsigned int)m_AbradeState, NULL);
	}
}

#endif