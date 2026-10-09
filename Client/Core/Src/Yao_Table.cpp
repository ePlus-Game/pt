//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10-31
//      File_base        : Yao_Table
//      File_ext         : .cpp
//      Author           : –Ïœ˛∏’
//      Description      : ÿ≥ Ù–‘≈‰÷√±Ì∂¡»°
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "Yao_Table.h"

YaoTable::YaoTable()
{
	m_bLoaded = false;
}

YaoTable::~YaoTable()
{
	m_yaoDatas.clear();
	m_yaoSets.clear();
}

YaoTable& YaoTable::Singleton()
{
	static YaoTable yaoTable;
	return yaoTable;
}

bool YaoTable::Load()
{
	if (LoadYao() && LoadYaoSet())
	{
		m_bLoaded = true;
		return true;
	}
	else
	{
		m_bLoaded = false;
		return false;
	}
}

bool YaoTable::LoadYao()
{	
	bool sucess = true;	
	m_yaoDatas.clear();

	KTabFile yaoTabFile;
	if (TRUE == yaoTabFile.Load(YAO_TABLE_FILE))
	{
		int row = 0;
		int recordCount = yaoTabFile.GetHeight() - 1;
		
		YaoData tempYD = {0};
		m_yaoDatas.resize(recordCount, tempYD);
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			YaoData& as = m_yaoDatas[record];
			
			int field = 1;			
			
			if (FALSE == yaoTabFile.GetInteger(row, field, 0, &(as.GuaID)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == yaoTabFile.GetInteger(row, field, 0, &(as.Level)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == yaoTabFile.GetString(row, field, "", as.Name, MAX_GUA_NAME_LENGTH))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == yaoTabFile.GetString(row, field, "", as.Desc, MAX_GUA_DESC_LENGTH))
			{
				sucess = false;
			}
			++field;

			if (FALSE == yaoTabFile.GetInteger(row, field, 0, &(as.StandaloneEffectID)))
			{
				sucess = false;
			}
			++field;

			for (int i = 0; i < MAX_GUA_TYPE_COUNT; ++i)
			{
				if (FALSE == yaoTabFile.GetInteger(row, field, 0, &(as.EffectIDs[i])))
				{
					sucess = false;
				}
				++field;
			}

			for (int imageLoopCount = 0; imageLoopCount < 2; imageLoopCount++)
			{
				yaoTabFile.GetString(row, field, "", as.Image[imageLoopCount], MAX_GUA_IMG_FILE_NAME_LENGTH);
				++field;
			}			
			
			if (!sucess)
			{
				break;
			}
		}
	}

	return sucess;
}

bool YaoTable::LoadYaoSet()
{	
	bool sucess = true;	
	m_yaoSets.clear();

	KTabFile yaoSetTabFile;
	if (TRUE == yaoSetTabFile.Load(YAO_SET_TABLE_FILE))
	{
		int row = 0;
		int recordCount = yaoSetTabFile.GetHeight() - 1;

		YaoSet tempYS = {0};
		m_yaoSets.resize(recordCount, tempYS);
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			YaoSet& ys = m_yaoSets[record];
			
			int field = 1;			
			
			if (FALSE == yaoSetTabFile.GetInteger(row, field, 0, &(ys.YaoSetID)))
			{
				sucess = false;
			}
			++field;

			for (int i = 0; i < MAX_GUA_POS_COUNT; ++i)
			{
				if (FALSE == yaoSetTabFile.GetInteger(row, field, 0, &(ys.GuaIDs[i])))
				{
					sucess = false;
				}
				++field;
			}

			if (FALSE == yaoSetTabFile.GetInteger(row, field, 0, &(ys.Level)))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == yaoSetTabFile.GetString(row, field, "", ys.Name, MAX_YAO_SET_NAME_LENGTH))
			{
				sucess = false;
			}
			++field;

			if (FALSE == yaoSetTabFile.GetString(row, field, "", ys.Comment, MAX_YAO_SET_COMMENT_LENGTH))
			{
				sucess = false;
			}
			++field;

			if (FALSE == yaoSetTabFile.GetString(row, field, "", ys.Desc, MAX_YAO_SET_DESC_LENGTH))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == yaoSetTabFile.GetInteger(row, field, 0, &(ys.EffectID)))
			{
				sucess = false;
			}
			++field;			
			
			if (!sucess)
			{
				break;
			}
		}
	}

	return sucess;
}

const YaoData* YaoTable::GetYao(int nGuaID, int nLevel)
{
	YaoDataArray::iterator iterCurr = m_yaoDatas.begin();
	YaoDataArray::iterator iterEnd = m_yaoDatas.end();

	while (iterCurr != iterEnd)
	{
		if ((*iterCurr).GuaID == nGuaID
			&& (*iterCurr).Level == nLevel)
		{
			return &(*iterCurr);
		}

		++iterCurr;
	}
	
	return NULL;
}

const YaoSet* YaoTable::GetYaoSet(ParamYaoSet* paramYaoSet)
{
	YaoSetArray::iterator iterCurr = m_yaoSets.begin();
	YaoSetArray::iterator iterEnd = m_yaoSets.end();

	while (iterCurr != iterEnd)
	{
		YaoSet* pys = &(*iterCurr);
		bool match = true;
		if (pys->Level != paramYaoSet->Level)
		{
			match = false;
		}
		else
		{
			for(int i=0; i<MAX_GUA_POS_COUNT; ++i)
			{
				if (pys->GuaIDs[i] != paramYaoSet->GuaIDs[i])
				{
					match = false;
					break;
				}
			}
		}

		if (match)
		{
			return pys;
		}

		++iterCurr;
	}
	
	return NULL;
}

const YaoSet* YaoTable::GetYaoSet(int yaoSetID)
{
	YaoSetArray::iterator iterCurr = m_yaoSets.begin();
	YaoSetArray::iterator iterEnd = m_yaoSets.end();

	while (iterCurr != iterEnd)
	{
		YaoSet* pys = &(*iterCurr);
		bool match = true;
		if (pys->YaoSetID == yaoSetID)
		{
			return pys;
		}

		++iterCurr;
	}
	
	return NULL;
}