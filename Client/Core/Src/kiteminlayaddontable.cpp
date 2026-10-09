//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 19:48
//      File_base        : kiteminlayaddontable
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "kiteminlayaddontable.h"

KItemInlayAddOnTable::KItemInlayAddOnTable( void )
{
}

KItemInlayAddOnTable::~KItemInlayAddOnTable( void )
{
}

KItemInlayAddOnTable& KItemInlayAddOnTable::Singleton( void )
{
	static KItemInlayAddOnTable itemInlayAddOnTable;
	return itemInlayAddOnTable;
}

bool KItemInlayAddOnTable::Load( void )
{
	bool sucess = true;	
	m_InlayAddOns.clear();

	KTabFile InlayAddOnTabFile;
	if (TRUE == InlayAddOnTabFile.Load(INLAY_ADD_ON_TABLE_FILE))
	{
		int row = 0;
		int recordCount = InlayAddOnTabFile.GetHeight() - 1;
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;

			InlayAddOn yad;
			
			int field = 1;			

			if (FALSE == InlayAddOnTabFile.GetInteger(row, field, 0, (int*)&(yad.GroupID)))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == InlayAddOnTabFile.GetInteger(row, field, 0, &(yad.Probability)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == InlayAddOnTabFile.GetInteger(row, field, 0, &(yad.BuffID)))
			{
				sucess = false;
			}
			++field;
		
			if (FALSE == InlayAddOnTabFile.GetInteger(row, field, 0, (int*)&(yad.YangRate)))
			{
				sucess = false;
			}
			++field;
			
			if (FALSE == InlayAddOnTabFile.GetInteger(row, field, 0, (int*)&(yad.YinRate)))
			{
				sucess = false;
			}
			++field;

			m_InlayAddOns[yad.GroupID].push_back( yad );

			char szBuff[COMMON_CLIENT_MSG_LEN_16];
			sprintf( szBuff, "%d", yad.GroupID );
			std::string key = szBuff;
			sprintf( szBuff, "%d", yad.BuffID );
			key +=  szBuff;

			m_InlayAddOnsPlus[key].push_back( yad );

			if (!sucess)
			{
				break;
			}
		}
	}

	return sucess;
}

const InlayAddOn* KItemInlayAddOnTable::GetInlayAddOnByGroup( int group,int index )
{
	if ( ( index >= 0 ) && ( index < GetInlayAddOnCount( group ) ) )
	{
		InlayAddOnMap::iterator it = m_InlayAddOns.find( group );
		if ( it != m_InlayAddOns.end() )
		{
			return &(it->second)[index];
		}		
	}
	return NULL;	
}

const InlayAddOn*	KItemInlayAddOnTable::GetInlayAddOnByGroupAndBuffID( 
							int group,
							short buffID,
							int index )
{
	if ( ( index >= 0 ) && ( index < GetInlayAddOnCount( group ) ) )
	{
		char szBuff[COMMON_CLIENT_MSG_LEN_16];
		sprintf( szBuff, "%d", group );
		std::string key = szBuff;
		sprintf( szBuff, "%d", buffID );
		key +=  szBuff;
		InlayAddOnMapPlus::iterator it = m_InlayAddOnsPlus.find( key );
		if ( it != m_InlayAddOnsPlus.end() )
		{
			return &(it->second)[index];
		}		
	}
	return NULL;
}