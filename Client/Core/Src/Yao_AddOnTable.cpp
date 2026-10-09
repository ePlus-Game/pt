//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-9
//      File_base        : Yao_AddOnTable
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 爻装附加属性表
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "Yao_AddOnTable.h"

YaoAddOnTable::YaoAddOnTable()
{
	m_bLoaded = false;
}

YaoAddOnTable::~YaoAddOnTable()
{
	m_YaoAddOns.clear();
}

YaoAddOnTable& YaoAddOnTable::Singleton()
{
	static YaoAddOnTable yaoAddOnTable;
	return yaoAddOnTable;
}

bool YaoAddOnTable::Load()
{
	bool sucess = true;	
	m_YaoAddOns.clear();

	KTabFile yaoAddOnTabFile;
	if (TRUE == yaoAddOnTabFile.Load(YAO_ADD_ON_TABLE_FILE))
	{
		int row = 0;
		int recordCount = yaoAddOnTabFile.GetHeight() - 1;
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;

			YaoAddOn yad = {0};
			
			int field = 1;			

			if (FALSE == yaoAddOnTabFile.GetInteger(row, field, 0, (int*)&(yad.GroupID)))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == yaoAddOnTabFile.GetInteger(row, field, 0, &(yad.Probability)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == yaoAddOnTabFile.GetInteger(row, field, 0, &(yad.BuffID)))
			{
				sucess = false;
			}
			++field;
			
			m_YaoAddOns[yad.GroupID].push_back( yad );

			if (!sucess)
			{
				break;
			}
		}
	}

	return sucess;
}

const YaoAddOn* YaoAddOnTable::GetYaoAddOn(int group,int index)
{
	if ((index >= 0) && (index < GetYaoAddOnCount(group)))
	{
		YaoAddOnMap::iterator it = m_YaoAddOns.find( group );
		if ( it != m_YaoAddOns.end() )
		{
			return &(it->second)[index];
		}		
	}
	return NULL;
	
}