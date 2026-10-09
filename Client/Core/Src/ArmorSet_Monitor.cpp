//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-8
//      File_base        : ArmorSet_Monitor
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 套装监视器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "ArmorSet_Monitor.h"
#include "ArmorSet_Table.h"
#include "buff_man.h"
#include "buff_tab.h"
#include "KItemGenerator.h"
#include "ConfigManager.h"

ArmorSetMonitor::ArmorSetMonitor()
{
	Init();
}

ArmorSetMonitor::~ArmorSetMonitor()
{
}

void ArmorSetMonitor::Init()
{
	m_ArmorSetCount = 0;
	memset(m_ArmorSetState, 0, sizeof(m_ArmorSetState));

#ifdef _SERVER
	CleanUp();
#endif

}

void ArmorSetMonitor::Update(int npcIndex, const KEquipState* equipments)
{
	ArmorSetTable& asTable = ArmorSetTable::Singleton();

	//清空原有套装情况
	memset(m_ArmorSetState, 0, sizeof(ArmorSetState) * itempart_num);
	m_ArmorSetCount = 0;	

	//更新套装情况
	for(int equipLoopCount = 0; equipLoopCount < itempart_num; ++equipLoopCount)
	{
		int equipIndex = equipments[equipLoopCount].nEquipIdx;
		if (equipIndex > 0)
		{
			KItem& item = Item[equipIndex];

			if (!item.IsBroken() && !item.IsOverDate())//装备没有损坏且没有过期
			{
				int armorSetID = item.GetArmorSetID();
				if (armorSetID > 0)//这个装备是套装
				{
					const ArmorSet* pArmorSet = asTable.GetArmorSet(armorSetID);
					_ASSERT(pArmorSet != NULL);
					if (pArmorSet != NULL)//套装信息存在
					{
						//找到合适的位置放置套装信息
						int infoIndex = -1;
						for(int infoIndexLoopCount = 0; infoIndexLoopCount < itempart_num; infoIndexLoopCount++)
						{
							if (m_ArmorSetState[infoIndexLoopCount].ArmorSetID <= 0)//这个位置还没有使用
							{
								infoIndex = infoIndexLoopCount;
								m_ArmorSetCount++;						
								m_ArmorSetState[infoIndexLoopCount].ArmorSetID = armorSetID;
								m_ArmorSetState[infoIndexLoopCount].ArmorSetCount = 0;
								break;
							}
							else
							{
								if (m_ArmorSetState[infoIndexLoopCount].ArmorSetID == armorSetID)//这个套装已经出现过了
								{
									infoIndex = infoIndexLoopCount;
									break;
								}
							}
						}	
						
						if (infoIndex >= 0)//找到合适的位置了
						{
							bool foundMatchPart = false;//找到符合的部件
							
							//把这套套装已有的部件标志一下
							for(int asPartLoopCount = 0; asPartLoopCount < itempart_num; asPartLoopCount++)
							{
								//看这个装备是否是这个套装的某个部位的可选部件之一
								bool foundMatchAlernativePart = false;
								int alernativePartCount = pArmorSet->AlternativePartCount[asPartLoopCount];
								for(int alternativePartLoopCount = 0; alternativePartLoopCount < alernativePartCount; alternativePartLoopCount++)
								{
									const int* IDArray = pArmorSet->PartIDs[asPartLoopCount][alternativePartLoopCount].IDArray;
									if (item.IsSameParitcularItem(
										IDArray[0],
										IDArray[1],
										IDArray[2]
										))
									{
										foundMatchAlernativePart = true;
										break;
									}
								}
								
								if (foundMatchAlernativePart && !m_ArmorSetState[infoIndex].Exist[asPartLoopCount])
								{
									m_ArmorSetState[infoIndex].ArmorSetCount++;
									m_ArmorSetState[infoIndex].Exist[asPartLoopCount] = true;
									foundMatchPart = true;
									break;
								}
							}
							
							if (!foundMatchPart)//没有找到符合的部件
							{
								if (m_ArmorSetState[infoIndex].ArmorSetCount == 0)
								{
									m_ArmorSetCount--;
									m_ArmorSetState[infoIndex].ArmorSetID = 0;
								}
							}
							
							//装备如果是套装，则必须在套装表中找到符合的项
							_ASSERT(foundMatchPart);
						}			
					}
				}
			}
		}
	}

#ifdef _SERVER

	BeginAddBuff();

	//激活套装属性
	for (int asStateLoopCount = 0; asStateLoopCount < m_ArmorSetCount; asStateLoopCount++)
	{
		const ArmorSet* pArmorSet = asTable.GetArmorSet(m_ArmorSetState[asStateLoopCount].ArmorSetID);
		if (pArmorSet != NULL)
		{
			//激活的套装数量不可能大于这个套装所有的部件数量
			int activePartCount = m_ArmorSetState[asStateLoopCount].ArmorSetCount;			
			_ASSERT(activePartCount <= pArmorSet->PartCount);
			if (activePartCount <= pArmorSet->PartCount)
			{
				for(int asEffectLoopCount = 0; asEffectLoopCount < activePartCount; asEffectLoopCount++)
				{
					int buffID = pArmorSet->EffectIDs[asEffectLoopCount];
					if (buffID > 0)
					{
						AddBuff(buffID);
					}				
				}
			}			
		}
	}

	EndAddBuff();

	SetupBuff(npcIndex);

#endif

}

#ifndef _SERVER

void ArmorSetMonitor::GetArmorSetDesc(KItem* pItem, char* descBuff) const
{
	if (pItem == NULL || descBuff == NULL)
		return;

	ConfigManager& cm = ConfigManager::Singleton();	
	
	//检测装备是否是套装
	int armorSetID = pItem->GetArmorSetID();
	if (armorSetID <= 0)
		return;
	
	//在已经激活的套装中查找这个装备所属的套装
	int armorSetStateIndex = -1;
	for(int asStateLoopCount=0; asStateLoopCount<m_ArmorSetCount; asStateLoopCount++)
	{
		if (m_ArmorSetState[asStateLoopCount].ArmorSetID == armorSetID)
		{
			armorSetStateIndex = asStateLoopCount;
			break;
		}
	}

	const ArmorSet* pAS = ArmorSetTable::Singleton().GetArmorSet(armorSetID);
	if (pAS != NULL)
	{
		//套装的名称
		char armorSetName[256] = { 0 };
		const char* armorSetNameTemplate = cm.GetConfigurableDisplayStyle(style_armorset_name);		
		if (armorSetNameTemplate != NULL)
		{
			sprintf(armorSetName, armorSetNameTemplate, pAS->ArmorSetName, ((armorSetStateIndex >= 0) ? m_ArmorSetState[armorSetStateIndex].ArmorSetCount : 0), pAS->PartCount);
		}
		strcat(descBuff, armorSetName);
		
		//套装的所有组成
		char armorSetPart[1536] = { 0 };		
		char armorSetPartList[1536] = { 0 };
		const char* armorSetPartTemplate = cm.GetConfigurableDisplayStyle(style_armorset_part);
		const char* newLineObj = cm.GetConfigurableDisplayStyle(style_new_line_obj);
		if (armorSetPartTemplate != NULL && newLineObj != NULL)
		{
			const int partPerRow = 3;
			int partRowCount = (pAS->PartCount - 1) / partPerRow + 1;
			for (int row = 0; row < partRowCount; row++)
			{				
				for(int i = 0; i < partPerRow; i++)
				{
					int asPartLoopCount = partPerRow * row + i;
					if (asPartLoopCount < pAS->PartCount)
					{
						const int* IDArray = pAS->PartIDs[asPartLoopCount][0].IDArray;//显示的时候取第一个可选部件
						const KBASICPROP_ITEM* itemTemplate = g_ItemGen.GetItemTemplate(
							IDArray[0],
							IDArray[1],
							IDArray[2],
							IDArray[3]);
						
						if (itemTemplate != NULL)
						{
							bool exist = false;
							if (armorSetStateIndex >= 0)
							{
								exist = m_ArmorSetState[armorSetStateIndex].Exist[asPartLoopCount];
							}				
							const char* partIndividalTemplate = cm.GetConfigurableDisplayStyle(style_armorset_part_individal, (exist ? TRUE : FALSE));
							if (partIndividalTemplate != NULL)
							{
								char armorSetPartIndividal[128] = { 0 };
								sprintf(armorSetPartIndividal, partIndividalTemplate, itemTemplate->szName);
								strcat(armorSetPartList, armorSetPartIndividal);
							}
						}
					}
				}
				
				strcat(armorSetPartList, newLineObj);
			}

			sprintf(armorSetPart, armorSetPartTemplate, armorSetPartList);
		}
		
		strcat(descBuff, armorSetPart);
		
		BuffTable& buffTable = BuffTable::Singleton();

		//套装的效果
		char armorSetEffectList[1024] = { 0 };
		for(int asEffectLoopCount=0; asEffectLoopCount<itempart_num; asEffectLoopCount++)
		{
			int effectBuffID = pAS->EffectIDs[asEffectLoopCount];
			if (effectBuffID > 0)//效果存在
			{
				PBAT buff = buffTable.GetBuff(effectBuffID);
				_ASSERT(buff != NULL);
				if (buff != NULL)
				{
					bool isEffectActive = ((armorSetStateIndex >= 0) ? m_ArmorSetState[armorSetStateIndex].ArmorSetCount : 0) > asEffectLoopCount;
					const char* armorSetEffectTemplate = cm.GetConfigurableDisplayStyle(style_armorset_effect, (isEffectActive ? TRUE : FALSE));
					if (armorSetEffectTemplate != NULL)
					{
						char armorSetEffect[128] = { 0 };
						if (isEffectActive)
						{
							sprintf(armorSetEffect, armorSetEffectTemplate, buff->szDesc);
						}
						else
						{
							sprintf(armorSetEffect, armorSetEffectTemplate, (asEffectLoopCount+1), buff->szDesc);
						}
						strcat(armorSetEffectList, armorSetEffect);
					}
				}						
			}				
		}
		strcat(descBuff, armorSetEffectList);
	}	
}

#endif