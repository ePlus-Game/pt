//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 19:55
//      File_base        : kiteminlayrule
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "kiteminlayrule.h"

KItemInlayRule&	KItemInlayRule::Singleton( void )
{
	static KItemInlayRule itemInlayRule;
	return itemInlayRule;
}

void KItemInlayRule::Load( void )
{
	m_inlayRuleMap.clear();
	KTabFile tabFile;
	BOOL bOk = tabFile.Load( INLAY_RULE_FILE );
	if ( bOk )
	{	
		int nHeight = tabFile.GetHeight();
		for ( int nIdy = 2; nIdy <= nHeight; ++nIdy )
		{
			std::string strKey;
			ItemInlayRule itemInlayRule;
			int nIdx = 0;
			for ( nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx)
			{
				char szBuff[256];
				sprintf( szBuff, "inlay%d", nIdx );
				int itemGroupID = 0;
				tabFile.GetInteger(nIdy, szBuff, 0, &itemGroupID );
				char szItemGroupID[256];
				sprintf( szItemGroupID, "%d", itemGroupID );
				strKey += szItemGroupID; 
				itemInlayRule.inlayGroupIDArray[nIdx] = itemGroupID;

			}

			for ( nIdx = 0; nIdx < MAX_SPECIALEFFECT_COUNT; ++nIdx )
			{
				char szBuff[256];
				sprintf( szBuff, "buffid%d", nIdx );
				tabFile.GetInteger( nIdy, szBuff, 0, &itemInlayRule.buffIDArray[nIdx] );
			}

			for ( nIdx = 0; nIdx < MAX_SPECIALEFFECT_FILTER; ++nIdx )
			{
				char szBuff[256];
				char sValue[32];
				sprintf( szBuff, "targetid%d", nIdx );
				tabFile.GetString( nIdy, szBuff, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32 );
				_getItemByString(
						itemInlayRule.DstItem[nIdx].nItemGenre, 
						itemInlayRule.DstItem[nIdx].nItemDetail, 
						itemInlayRule.DstItem[nIdx].nItemParticular,
						itemInlayRule.DstItem[nIdx].nItemLevel,
						itemInlayRule.DstItem[nIdx].nItemCount,
						itemInlayRule.DstItem[nIdx].nItemQuality,
						itemInlayRule.DstItem[nIdx].nItemYao,
						itemInlayRule.DstItem[nIdx].nItemColor,
						sValue, 32);
			}
			
			tabFile.GetString( nIdy, "info", "", itemInlayRule.szInfo, sizeof(itemInlayRule.szInfo) );

			m_inlayRuleMap[strKey] = itemInlayRule;
			
		}

	}
}

bool KItemInlayRule::GetInlayEffect( const char* szKey, ItemInlayRule& rRule )
{
	if ( szKey )
	{
		_ItemInlayRuleMap::iterator it = m_inlayRuleMap.find( szKey );
		if ( it != m_inlayRuleMap.end() )
		{
			rRule = it->second;
			return true;
		}
	}
	return false;
}

bool KItemInlayRule::IsRight( const char* szKey, const KItem* item )
{
	if ( szKey && item )
	{
		_ItemInlayRuleMap::iterator it = m_inlayRuleMap.find(szKey);
		if ( it != m_inlayRuleMap.end() )
		{
			for ( int nIdx = 0; nIdx < MAX_SPECIALEFFECT_FILTER; ++nIdx )
			{
				if ( g_CompoundRule.CheckItem( item, it->second.DstItem[nIdx] ) )
				{
					return true;
				}
			}
		}

	}

	return false;
}
