//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10-30
//      File_base        : ArmorSet_Table
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 套装配置表读取
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "ArmorSet_Table.h"

#define itempart_num 9//TODO 测试用，将来需要去掉

ArmorSetTable::ArmorSetTable()
{
	m_bLoaded = false;
}

ArmorSetTable::~ArmorSetTable()
{
	m_ArmorSets.clear();		
}

ArmorSetTable& ArmorSetTable::Singleton()
{
	static ArmorSetTable asTable;
	return asTable;
}

bool ArmorSetTable::Load()
{	
	bool sucess = true;
	
	m_ArmorSets.clear();
	m_bLoaded = false;

	KTabFile armorSetTabFile;
	if (TRUE == armorSetTabFile.Load(ARMOR_SET_TABLE_FILE))
	{
		int row = 0;
		int recordCount = armorSetTabFile.GetHeight() - 1;
		char alternativeString[MAX_ITEM_ID_STR_LENGTH * ALTERNATIVE_PART_COUNT] = { 0 };

		ArmorSet tempAS = {0};
		m_ArmorSets.resize(recordCount, tempAS);
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			ArmorSet& as = m_ArmorSets[record];
			
			int field = 1;			
			
			if (FALSE == armorSetTabFile.GetInteger(row, field, 0, &(as.ArmorSetID)))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == armorSetTabFile.GetString(row, field, "", as.ArmorSetName, MAX_ARMOR_SET_NAME_LENGTH))
			{
				sucess = false;
			}
			++field;

			int partCount = 0;
			for (int i = 0; i < itempart_num; ++i)
			{
				as.AlternativePartCount[i] = 0;
				if (TRUE == armorSetTabFile.GetString(row, field, "", alternativeString, MAX_ITEM_ID_STR_LENGTH * ALTERNATIVE_PART_COUNT))
				{
					char* pToken = strtok(alternativeString, ITEM_ID_DELIMITER);
					while(pToken != NULL)
					{
						for(int idPart = 0; idPart < MAX_EQUIPMENT_ID_ARRAY_LENGTH; idPart++)
						{
							as.PartIDs[i][as.AlternativePartCount[i]].IDArray[idPart] = atoi(pToken);
							if (idPart == MAX_EQUIPMENT_ID_ARRAY_LENGTH - 2)
							{
								pToken = strtok(NULL, ALTERNATIVE_ITEM_DELIMITER);
							}
							else
							{								
								pToken = strtok(NULL, ITEM_ID_DELIMITER);
							}
						}
						as.AlternativePartCount[i]++;				
					}
					
					++partCount;
					++field;
				}
				else
				{
					field += itempart_num - i;
					break;
				}				
			}
			as.PartCount = partCount;
			
			for (int j = 0; j < partCount; ++j)
			{
				if (TRUE == armorSetTabFile.GetInteger(row, field, 0, &(as.EffectIDs[j])))
				{
					++field;					
				}
				else
				{
					sucess = false;
					field += partCount - j;
					break;
				}				
			}
			field += itempart_num - partCount;

			if (!sucess)
			{
				break;
			}
		}
	}

	//读取套装配置表必须成功
	_ASSERT(sucess);

	if (sucess)
	{
		m_bLoaded = true;
	}

	return sucess;
}

const ArmorSet* ArmorSetTable::GetArmorSet(int nArmorSetID)
{
	ArmorSetArray::iterator iterCurr = m_ArmorSets.begin();
	ArmorSetArray::iterator iterEnd = m_ArmorSets.end();

	while (iterCurr != iterEnd)
	{
		if ((*iterCurr).ArmorSetID == nArmorSetID)
		{
			return &(*iterCurr);
		}

		++iterCurr;
	}
	
	return NULL;
}

int ArmorSetTable::GetAllArmorSetID(int* IDArray, int max)
{
	if (!m_bLoaded || max <= 0)
	{
		return 0;
	}

	int size = m_ArmorSets.size();
	max = max > size ? size : max;

	for (int i = 0; i < max; ++i)
	{
		IDArray[i] = m_ArmorSets[i].ArmorSetID;
	}

	return max;
}

