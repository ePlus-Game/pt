//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-16
//      File_base        : Abrade_Table
//      File_ext         : .cpp
//      Author           : ÐìÏþ¸Õ
//      Description      : ×°±¸Ä¥ËðÅäÖÃ±í¶ÁÈ¡
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "ItemCommonDef.h"
#include "Abrade_Table.h"

AbradeTable::AbradeTable()
{
}

AbradeTable::~AbradeTable()
{
}

AbradeTable& AbradeTable::Singleton()
{
	static AbradeTable abradeTable;
	return abradeTable;
}

bool AbradeTable::Load()
{
	bool success = false;
	memset(m_Abrade, 0, sizeof(m_Abrade));	
	
	KIniFile abradeIniFile;
	if (TRUE == abradeIniFile.Load(ITEM_ABRADE_FILE))
	{
		abradeIniFile.GetInteger("Repair", "NormalRepairFactor", 0, &m_NormalRepairFactor);
		abradeIniFile.GetInteger("Repair", "NormalRepairMaxDurDropFactor", 0, &m_NormalRepairMaxDurDropFactor);
		abradeIniFile.GetInteger("Repair", "SpecialRepairFactor", 0, &m_SpecialRepairFactor);
		abradeIniFile.GetInteger("Repair", "SpecialRepairMaxDurDropFactor", 0, &m_SpecialRepairMaxDurDropFactor);
		abradeIniFile.GetInteger("Alert", "WeakAlert", 0, &m_WeakAlertValue);
		abradeIniFile.GetInteger("Alert", "BrokenAlert", 0, &m_BrokenAlertValue);
		abradeIniFile.GetInteger("Display", "DisplayRatio", 1, &m_DisplayRatio);

		const char* abradeMode[] = { "Attack", "Defend", "Death" };	
		
		for(int i = 0; i < abrade_count; i++)
		{
			const char* mode = abradeMode[i];
			AbradeInfo& abrade = m_Abrade[i];
			
			abradeIniFile.GetInteger(mode, "ValueType", 0, (int*)&abrade.ValueType);
			abradeIniFile.GetInteger(mode, "Amulet", 0, &abrade.Value[itempart_amulet]);
			abradeIniFile.GetInteger(mode, "Helm", 0, &abrade.Value[itempart_helm]);
			abradeIniFile.GetInteger(mode, "Pendant", 0, &abrade.Value[itempart_pendant]);
			abradeIniFile.GetInteger(mode, "Weapon", 0, &abrade.Value[itempart_weapon]);
			abradeIniFile.GetInteger(mode, "Armor", 0, &abrade.Value[itempart_armor]);
			abradeIniFile.GetInteger(mode, "Shoulder", 0, &abrade.Value[itempart_shoulder]);
			abradeIniFile.GetInteger(mode, "Ring", 0, &abrade.Value[itempart_ring]);
			abradeIniFile.GetInteger(mode, "Boots", 0, &abrade.Value[itempart_boots]);
			abradeIniFile.GetInteger(mode, "Cuff", 0, &abrade.Value[itempart_cuff]);
		}
		
		success = true;
	}

	return success;
}