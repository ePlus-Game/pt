// kItemdateparser.cpp: implementation of the KItemDateParser class.
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "kItemdateparser.h"
#include "GameDataDef.h"
#include "KItemGenerator.h"

/************************************************************************/
/*							ITEM_VERSION 1                              */
/************************************************************************/

KItemDateParser_Version1::KItemDateParser_Version1( int version )
{
	d_point = NULL;
	d_itemIndex = 0;
	d_version = version;
}

KItemDateParser_Version1::~KItemDateParser_Version1( void )
{

}

int KItemDateParser_Version1::Parse( KItemList& itemList )
{
	int size = 0;

	_TDBItemData_Version_1* dataPoint = ( _TDBItemData_Version_1*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	size = GetSizeOf_TDBItemData();

	d_itemIndex = ItemSet.Add( 
		dataPoint->iequipclasscode, 
		dataPoint->idetailtype, 
		dataPoint->iparticulartype, 
		dataPoint->ilevel,  
		dataPoint->wItemCount);
	if (d_itemIndex > 0)
	{
		Item[d_itemIndex].SetGUID( dataPoint->guid );
	
		for (int yaoAddonBuffLoopCount = 0; yaoAddonBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddonBuffLoopCount++)
		{
			Item[d_itemIndex].SetYaoAddOn( yaoAddonBuffLoopCount, dataPoint->iyaoAddOnBuffIDSet[yaoAddonBuffLoopCount] );
		}					
		Item[d_itemIndex].SetMaxDurability( dataPoint->imaxdurability );
		Item[d_itemIndex].SetDurability( dataPoint->idurability );
		Item[d_itemIndex].SetCompBuffTemplateSet( dataPoint->icompBuffTemplateSet, sizeof(WORD) * COMPOUND_COUNT );
		Item[d_itemIndex].SetLevelupTimes( dataPoint->uLevelupTimes );
		Item[d_itemIndex].SetLevelupType( dataPoint->nLevelupType );						
		Item[d_itemIndex].SetTalismanPotential( dataPoint->TalismanPotential );
		for(int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
		{
			Item[d_itemIndex].SetTalismanEnchase(enchaseLoopCount, dataPoint->TalismanEnchaseSet[enchaseLoopCount]);
		}
		if ( dataPoint->IsBind )
		{
			Item[d_itemIndex].SetBind( true );
		}
		else
		{
			Item[d_itemIndex].SetBind( false );
		}
		if ( dataPoint->szPlusInfo[0] )
		{
			Item[d_itemIndex].SetPlusInfo( dataPoint->szPlusInfo );
		}
		
		//镶嵌相关
		Item[d_itemIndex].ClearSocketSet();
		int nSocketIdx = 0;
		for ( nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
		{
			if ( dataPoint->inlayItem[nSocketIdx].nGenre == -1 &&
				dataPoint->inlayItem[nSocketIdx].nDetail == -1 &&
				dataPoint->inlayItem[nSocketIdx].nParticular == -1 &&
				dataPoint->inlayItem[nSocketIdx].nLevel == -1 )
			{
				continue;
			}
			else
			{
				Item[d_itemIndex].CreateSocket();
			}
		}

		for ( nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
		{
			InlayStuff stuff;
			if ( dataPoint->inlayItem[nSocketIdx].nGenre == -1 &&
				dataPoint->inlayItem[nSocketIdx].nDetail == -1 &&
				dataPoint->inlayItem[nSocketIdx].nParticular == -1 &&
				dataPoint->inlayItem[nSocketIdx].nLevel == -1 )
			{
				continue;
			}
			else
			{
				stuff.nGenre		= dataPoint->inlayItem[nSocketIdx].nGenre;
				stuff.nDetail		= dataPoint->inlayItem[nSocketIdx].nDetail;
				stuff.nParticular	= dataPoint->inlayItem[nSocketIdx].nParticular;
				stuff.nLevel		= dataPoint->inlayItem[nSocketIdx].nLevel;
				if ( (stuff.nGenre == 0 && 
					stuff.nDetail == 0 &&
					stuff.nParticular == 0 && 
					stuff.nLevel == 0) == false )
				{
					const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate( 
						stuff.nGenre,
						stuff.nDetail,
						stuff.nParticular,
						stuff.nLevel );
					if ( pItemTemplate )
					{
						Item[d_itemIndex].InlaySocket( pItemTemplate, dataPoint->inlayYaoBuffSet[nSocketIdx] );
					}									
				}
			}
		}

		if ( (dataPoint->iequipclasscode == item_target || (dataPoint->iequipclasscode == item_ib && !Item->IsNoTarget())) &&
			dataPoint->iparticulartype == inlay_targetitem )
		{
			Item[d_itemIndex].SetInlayYaoBuffSet( dataPoint->inlayYaoBuffSet );
		}

		Item[d_itemIndex].SetYaoID( dataPoint->iyaoid );
		Item[d_itemIndex].SetIBGuid( dataPoint->IBGuid );
		Item[d_itemIndex].SetIBBuyDate( dataPoint->IBProductionData );
		Item[d_itemIndex].SetCreditFlag( dataPoint->IBCreaditFlag ) ;

		Item[d_itemIndex].SetIBUseCount( dataPoint->wItemCount );

		if ( 1 == d_version )
		{
			if (!itemList.Add(d_itemIndex, dataPoint->ilocal, dataPoint->ix, dataPoint->iy, NULL, item_sync_type_init))
			{
				ItemSet.Remove(d_itemIndex);
			}
		}
	}

	return size;
}

int KItemDateParser_Version1::Pack( const KItemList& itemList, const int itemListIdx, KItem& item )
{
	return Pack( itemList.m_Items[itemListIdx].nPlace, itemList.m_Items[itemListIdx].nX, itemList.m_Items[itemListIdx].nY, item );
}

int	KItemDateParser_Version1::Pack( int pos, int x, int y, KItem& item )
{
	int size = 0;

	_TDBItemData_Version_1* dataPoint = ( _TDBItemData_Version_1*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	size = GetSizeOf_TDBItemData();

	dataPoint->iequipclasscode	= item.GetGenre();
	dataPoint->idetailtype		= item.GetDetailType();
	dataPoint->iparticulartype	= item.GetParticular();
	dataPoint->ilevel			= item.GetLevel();
	dataPoint->ilocal			= pos;
	dataPoint->ix				= x;
	dataPoint->iy				= y;
	dataPoint->iyaoid			= item.GetYaoID();
	for (int yaoAddonBuffLoopCount = 0; yaoAddonBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddonBuffLoopCount++)
	{
		dataPoint->iyaoAddOnBuffIDSet[yaoAddonBuffLoopCount] = item.GetYaoAddOn(yaoAddonBuffLoopCount);
	}
	dataPoint->idurability		= item.GetDurability();
	dataPoint->imaxdurability	= item.GetMaxDurability();
	
	dataPoint->uLevelupTimes	= item.GetLevelupTimes();
	dataPoint->nLevelupType		= item.GetLevelupType();
	dataPoint->wItemCount		= item.GetItemCount();
	memcpy(dataPoint->icompBuffTemplateSet, item.GetCompBuffTemplateSet(), sizeof(WORD) * COMPOUND_COUNT);
	memcpy(&(dataPoint->guid), &(item.GetGUID()), sizeof(FSGUID));
	dataPoint->TalismanPotential = item.GetTalismanPotential();
	for(int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
	{
		dataPoint->TalismanEnchaseSet[enchaseLoopCount] = item.GetTalismanEnchase(enchaseLoopCount);
	}
	dataPoint->IsBind			 = item.IsBind();

	//镶嵌相关
	int useMaxCount = item.GetMaxSocketCount();
	for ( int nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
	{
		InlayStuff stuff;
		
		item.GetInlayStuffBySocketIdx( nSocketIdx, stuff );

		if ( nSocketIdx < useMaxCount )
		{
			dataPoint->inlayItem[nSocketIdx].nGenre			= stuff.nGenre;
			dataPoint->inlayItem[nSocketIdx].nDetail		= stuff.nDetail;
			dataPoint->inlayItem[nSocketIdx].nParticular	= stuff.nParticular;
			dataPoint->inlayItem[nSocketIdx].nLevel			= stuff.nLevel;
		}
		else
		{
			dataPoint->inlayItem[nSocketIdx].nGenre			= -1;
			dataPoint->inlayItem[nSocketIdx].nDetail		= -1;
			dataPoint->inlayItem[nSocketIdx].nParticular	= -1;
			dataPoint->inlayItem[nSocketIdx].nLevel			= -1;
		}
	}
	memcpy( dataPoint->inlayYaoBuffSet, item.GetInlayYaoBuffSet(), sizeof(dataPoint->inlayYaoBuffSet) );

	// 个性化描述相关
	char* szPlusInfo = item.getPlusInfo();					
	if ( szPlusInfo[0] )
	{
		strcpy( dataPoint->szPlusInfo, szPlusInfo );
	}
	else
	{
		dataPoint->szPlusInfo[0] = 0;
	}//*/

	dataPoint->IBGuid =item.GetIBGuid();
	dataPoint->IBProductionData = item.GetIBBuyData();
	dataPoint->IBCreaditFlag    = item.GetCreditFlag();
	return size;
}

void KItemDateParser_Version1::SetBegin( TDBItemData_Base* pItemDate )
{
	if ( pItemDate == NULL )
	{
		return;
	}

	_TDBItemData_Version_1* _pTDBItemV1 = ((TDBItemData_Version_1*)pItemDate)->ItemData;
	if ( _pTDBItemV1 )
	{
		d_point = (char*)_pTDBItemV1;
	}
}

void KItemDateParser_Version1::Next( void )
{
	if ( d_point == NULL )
	{
		return;
	}

	d_point += GetSizeOf_TDBItemData();
}

bool KItemDateParser_Version1::IsPackage( void )
{
	_TDBItemData_Version_1* dataPoint = ( _TDBItemData_Version_1*)d_point;

	if ( dataPoint == NULL )
	{
		return false;
	}

	if ( dataPoint->iequipclasscode == item_task && dataPoint->ilevel == QK_Bag 
	&& (dataPoint->ilocal == pos_itembox_extend || dataPoint->ilocal == pos_store_extend))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int		KItemDateParser_Version1::GetSizeOfTDBItemData( void )
{
	return sizeof(TDBItemData_Version_1);
}

int		KItemDateParser_Version1::GetSizeOf_TDBItemData( void )
{
	return sizeof(_TDBItemData_Version_1);
}

/************************************************************************/
/*								ITEM_VERSION 2                          */
/************************************************************************/

KItemDateParser_Version2::KItemDateParser_Version2( int version )
{
	d_point = NULL;
	d_itemIndex = 0;
	d_version = version;
}

KItemDateParser_Version2::~KItemDateParser_Version2( void )
{

}

int		KItemDateParser_Version2::Parse( KItemList& itemList )
{
	if ( KItemDateParser_Version1::Parse( itemList ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_2* dataPoint = ( _TDBItemData_Version_2*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	if (d_itemIndex <= 0 || d_itemIndex >= MAX_ITEM )
	{
		return 0;
	}

	Item[d_itemIndex].SetPosInfo(dataPoint->mapInfo.m_MapID, dataPoint->mapInfo.m_MapX, dataPoint->mapInfo.m_MapY);
	Item[d_itemIndex].SetStep(dataPoint->mapInfo.m_Step );

	if ( 2 == d_version )
	{
		if ( Item[d_itemIndex].GetGenre() == item_ib && Item[d_itemIndex].GetIBBuyData() != 0 )
		{
			const DWORD dwExtendTime  = 3600 * 24 * 3;    //Delay 3 dayse
			DWORD       dwLastBuyTime = Item[d_itemIndex].GetIBBuyData();
			
			if (dwLastBuyTime + dwExtendTime <= UNIX_TMIE_STAMP)
				Item[d_itemIndex].SetIBBuyDate(dwLastBuyTime + dwExtendTime);
			else
				Item[d_itemIndex].SetIBBuyDate(UNIX_TMIE_STAMP);

		}//endif

		if (!itemList.Add(d_itemIndex, dataPoint->ilocal, dataPoint->ix, dataPoint->iy, NULL, item_sync_type_init))
		{
			ItemSet.Remove(d_itemIndex);
		}//endif
		
	}//endif

	size = GetSizeOf_TDBItemData();	

	return size;
}

int		KItemDateParser_Version2::Pack( const KItemList& itemList, const int itemListIdx, KItem& item )
{
	return Pack( itemList.m_Items[itemListIdx].nPlace, itemList.m_Items[itemListIdx].nX, itemList.m_Items[itemListIdx].nY, item );
}

int		KItemDateParser_Version2::Pack( int pos, int x, int y, KItem& item )
{
	if ( KItemDateParser_Version1::Pack( pos, x, y, item ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_2* dataPoint = ( _TDBItemData_Version_2*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	item.GetPosInfo( dataPoint->mapInfo.m_MapID, dataPoint->mapInfo.m_MapX, dataPoint->mapInfo.m_MapY );
	dataPoint->mapInfo.m_Step = item.GetStep();

	size = GetSizeOf_TDBItemData();

	return size;
}

void	KItemDateParser_Version2::SetBegin( TDBItemData_Base* pItemDate )
{
	if ( pItemDate == NULL )
	{
		return;
	}

	_TDBItemData_Version_2* _pTDBItemV1 = ((TDBItemData_Version_2*)pItemDate)->ItemData;
	if ( _pTDBItemV1 )
	{
		
		d_point = (char*)_pTDBItemV1;
	}
}

void	KItemDateParser_Version2::Next( void )
{
	if ( d_point == NULL )
	{
		return;
	}

	d_point += GetSizeOf_TDBItemData();
}

bool	KItemDateParser_Version2::IsPackage( void )
{
	_TDBItemData_Version_2* dataPoint = ( _TDBItemData_Version_2*)d_point;
	if ( dataPoint == NULL )
	{
		return false;
	}

	if ( dataPoint->iequipclasscode == item_task && dataPoint->ilevel == QK_Bag 
	&& (dataPoint->ilocal == pos_itembox_extend || dataPoint->ilocal == pos_store_extend))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int		KItemDateParser_Version2::GetSizeOfTDBItemData( void )
{
	return sizeof(TDBItemData_Version_2);
}

int		KItemDateParser_Version2::GetSizeOf_TDBItemData( void )
{
	return sizeof(_TDBItemData_Version_2);
}

/************************************************************************/
/*								ITEM_VERSION 3                          */
/************************************************************************/

KItemDateParser_Version3::KItemDateParser_Version3( int version )
{
	d_point = NULL;
	d_itemIndex = 0;
	d_version = version;
}

KItemDateParser_Version3::~KItemDateParser_Version3( void )
{

}

int		KItemDateParser_Version3::Parse( KItemList& itemList )
{
	if ( KItemDateParser_Version2::Parse( itemList ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_3* dataPoint = ( _TDBItemData_Version_3*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	if (d_itemIndex <= 0 || d_itemIndex >= MAX_ITEM)
	{
		return 0;
	}

	Item[d_itemIndex].SetTaskGiven( dataPoint->IsTaskGiven ? TRUE:FALSE);

	if (d_version == 3)
	{
		if (!itemList.Add(d_itemIndex, dataPoint->ilocal, dataPoint->ix, dataPoint->iy, NULL, item_sync_type_init))
		{
			ItemSet.Remove(d_itemIndex);
		}//endif

	}//endif

	size = GetSizeOf_TDBItemData();	

	return size;
}

int		KItemDateParser_Version3::Pack( const KItemList& itemList, const int itemListIdx, KItem& item )
{
	return Pack( itemList.m_Items[itemListIdx].nPlace, itemList.m_Items[itemListIdx].nX, itemList.m_Items[itemListIdx].nY, item );
}

int		KItemDateParser_Version3::Pack( int pos, int x, int y, KItem& item )
{
	if ( KItemDateParser_Version2::Pack( pos, x, y, item ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_3* dataPoint = ( _TDBItemData_Version_3*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	dataPoint->IsTaskGiven = item.IsTaskGiven() ? 1 : 0 ;

	size = GetSizeOf_TDBItemData();

	return size;
}

void	KItemDateParser_Version3::SetBegin( TDBItemData_Base* pItemDate )
{
	if ( pItemDate == NULL )
	{
		return;
	}

	_TDBItemData_Version_3* _pTDBItemV1 = ((TDBItemData_Version_3*)pItemDate)->ItemData;
	if ( _pTDBItemV1 )
	{
		
		d_point = (char*)_pTDBItemV1;
	}
}

void	KItemDateParser_Version3::Next( void )
{
	if ( d_point == NULL )
	{
		return;
	}

	d_point += GetSizeOf_TDBItemData();
}

bool	KItemDateParser_Version3::IsPackage( void )
{
	_TDBItemData_Version_3* dataPoint = ( _TDBItemData_Version_3*)d_point;
	if ( dataPoint == NULL )
	{
		return false;
	}

	if ( dataPoint->iequipclasscode == item_task && dataPoint->ilevel == QK_Bag 
	&& (dataPoint->ilocal == pos_itembox_extend || dataPoint->ilocal == pos_store_extend))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int		KItemDateParser_Version3::GetSizeOfTDBItemData( void )
{
	return sizeof(TDBItemData_Version_3);
}

int		KItemDateParser_Version3::GetSizeOf_TDBItemData( void )
{
	return sizeof(_TDBItemData_Version_3);
}

/************************************************************************/
/*								ITEM_VERSION 4                          */
/************************************************************************/

KItemDateParser_Version4::KItemDateParser_Version4( int version )
{
	d_point = NULL;
	d_itemIndex = 0;
	d_version = version;
}

KItemDateParser_Version4::~KItemDateParser_Version4( void )
{

}

int		KItemDateParser_Version4::Parse( KItemList& itemList )
{
	if ( KItemDateParser_Version3::Parse( itemList ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_4* dataPoint = ( _TDBItemData_Version_4*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	if (d_itemIndex <= 0 || d_itemIndex >= MAX_ITEM)
	{
		return 0;
	}

	Item[d_itemIndex].SetLockDate( dataPoint->dwLockLeftTime );
	

	if (d_version == 4 )
	{
		if (!itemList.Add(d_itemIndex, dataPoint->ilocal, dataPoint->ix, dataPoint->iy, NULL, item_sync_type_init))
		{
			ItemSet.Remove(d_itemIndex);
		}
	}

	size = GetSizeOf_TDBItemData();	

	return size;
}

int		KItemDateParser_Version4::Pack( const KItemList& itemList, const int itemListIdx, KItem& item )
{
	return Pack( itemList.m_Items[itemListIdx].nPlace, itemList.m_Items[itemListIdx].nX, itemList.m_Items[itemListIdx].nY, item );
}

int		KItemDateParser_Version4::Pack( int pos, int x, int y, KItem& item )
{
	if ( KItemDateParser_Version3::Pack( pos, x, y, item ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_4* dataPoint = ( _TDBItemData_Version_4*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	dataPoint->dwLockLeftTime = item.GetLockDate();

	size = GetSizeOf_TDBItemData();

	return size;
}

void	KItemDateParser_Version4::SetBegin( TDBItemData_Base* pItemDate )
{
	if ( pItemDate == NULL )
	{
		return;
	}

	_TDBItemData_Version_4* _pTDBItemV1 = ((TDBItemData_Version_4*)pItemDate)->ItemData;
	if ( _pTDBItemV1 )
	{
		
		d_point = (char*)_pTDBItemV1;
	}
}

void	KItemDateParser_Version4::Next( void )
{
	if ( d_point == NULL )
	{
		return;
	}

	d_point += GetSizeOf_TDBItemData();
}

bool	KItemDateParser_Version4::IsPackage( void )
{
	_TDBItemData_Version_4* dataPoint = ( _TDBItemData_Version_4*)d_point;
	if ( dataPoint == NULL )
	{
		return false;
	}

	if ( dataPoint->iequipclasscode == item_task && dataPoint->ilevel == QK_Bag 
	&& (dataPoint->ilocal == pos_itembox_extend || dataPoint->ilocal == pos_store_extend))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int		KItemDateParser_Version4::GetSizeOfTDBItemData( void )
{
	return sizeof(TDBItemData_Version_4);
}

int		KItemDateParser_Version4::GetSizeOf_TDBItemData( void )
{
	return sizeof(_TDBItemData_Version_4);
}

/************************************************************************/
/*								ITEM_VERSION 5                          */
/************************************************************************/

KItemDateParser_Version5::KItemDateParser_Version5( int version )
{
	d_point = NULL;
	d_itemIndex = 0;
	d_version = version;
}

KItemDateParser_Version5::~KItemDateParser_Version5( void )
{

}

int		KItemDateParser_Version5::Parse( KItemList& itemList )
{
	if ( KItemDateParser_Version4::Parse( itemList ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_5* dataPoint = ( _TDBItemData_Version_5*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	if (d_itemIndex <= 0 || d_itemIndex >= MAX_ITEM)
	{
		return 0;
	}

	Item[d_itemIndex].SetFlushTimes( (int)dataPoint->nFlushTimes );
	

	if (d_version == 5 )
	{
		if (!itemList.Add(d_itemIndex, dataPoint->ilocal, dataPoint->ix, dataPoint->iy, NULL, item_sync_type_init))
		{
			ItemSet.Remove(d_itemIndex);
		}
	}

	size = GetSizeOf_TDBItemData();	

	return size;
}

int		KItemDateParser_Version5::Pack( const KItemList& itemList, const int itemListIdx, KItem& item )
{
	return Pack( itemList.m_Items[itemListIdx].nPlace, itemList.m_Items[itemListIdx].nX, itemList.m_Items[itemListIdx].nY, item );
}

int		KItemDateParser_Version5::Pack( int pos, int x, int y, KItem& item )
{
	if ( KItemDateParser_Version4::Pack( pos, x, y, item ) <= 0 )
	{
		return 0;
	}

	int size = 0;

	_TDBItemData_Version_5* dataPoint = ( _TDBItemData_Version_5*)d_point;

	if ( dataPoint == NULL )
	{
		return 0;
	}

	dataPoint->nFlushTimes = (int)item.GetFlushTimes();

	size = GetSizeOf_TDBItemData();

	return size;
}

void	KItemDateParser_Version5::SetBegin( TDBItemData_Base* pItemDate )
{
	if ( pItemDate == NULL )
	{
		return;
	}

	_TDBItemData_Version_5* _pTDBItemV1 = ((TDBItemData_Version_5*)pItemDate)->ItemData;
	if ( _pTDBItemV1 )
	{
		
		d_point = (char*)_pTDBItemV1;
	}
}

void	KItemDateParser_Version5::Next( void )
{
	if ( d_point == NULL )
	{
		return;
	}

	d_point += GetSizeOf_TDBItemData();
}

bool	KItemDateParser_Version5::IsPackage( void )
{
	_TDBItemData_Version_5* dataPoint = ( _TDBItemData_Version_5*)d_point;
	if ( dataPoint == NULL )
	{
		return false;
	}

	if ( dataPoint->iequipclasscode == item_task && dataPoint->ilevel == QK_Bag 
	&& (dataPoint->ilocal == pos_itembox_extend || dataPoint->ilocal == pos_store_extend))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int		KItemDateParser_Version5::GetSizeOfTDBItemData( void )
{
	return sizeof(TDBItemData_Version_5);
}

int		KItemDateParser_Version5::GetSizeOf_TDBItemData( void )
{
	return sizeof(_TDBItemData_Version_5);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


KItemDateMgr::KItemDateMgr( int itemVersion, TDBItemData_Base* pItemDate )
{
	d_dateMgr		= NULL;
	d_itemVersion	= itemVersion;

	switch( d_itemVersion )
	{
	case 1:
		d_dateMgr = new KItemDateParser_Version1( itemVersion );
		break;
	case 2:
		d_dateMgr = new KItemDateParser_Version2( itemVersion );
		break;
	case 3:
		d_dateMgr = new KItemDateParser_Version3( itemVersion );
		break;
	case 4:
		d_dateMgr = new KItemDateParser_Version4( itemVersion );
		break;
	case 5:
		d_dateMgr = new KItemDateParser_Version5( itemVersion );
		break;
	default:
	    break;
	}
	if ( d_dateMgr )
	{
		d_dateMgr->SetBegin( pItemDate );
	}

}

KItemDateMgr::~KItemDateMgr( void )
{
	if ( d_dateMgr )
	{
		delete d_dateMgr;
		d_dateMgr = NULL;
	}
}

int KItemDateMgr::Parse(  KItemList& itemList )
{
	int size = 0;
	
	if ( d_dateMgr )
	{
		size = d_dateMgr->Parse( itemList );
	}

	return size;
}

int KItemDateMgr::Pack( const KItemList& itemList, const int itemListIdx, KItem& item )
{
	int size = 0;

	if ( d_dateMgr )
	{
		size = d_dateMgr->Pack( itemList, itemListIdx, item );
	}

	return size;
}

int	KItemDateMgr::Pack( int pos, int x, int y, KItem& item )
{
	int size = 0;

	if ( d_dateMgr )
	{
		size = d_dateMgr->Pack( pos, x, y, item );
	}

	return size;
}

void KItemDateMgr::Next( void )
{
	if ( d_dateMgr )
	{
		d_dateMgr->Next();
	}
}
//*/

int KItemDateMgr::GetSizeOf_TDBItemData( void ) const
{
	if ( d_dateMgr )
	{
		return d_dateMgr->GetSizeOf_TDBItemData();
	}
	return 0;
}

int KItemDateMgr::GetSizeOfTDBItemData( void ) const
{
	if ( d_dateMgr )
	{
		return d_dateMgr->GetSizeOfTDBItemData();
	}
	return 0;
}

bool KItemDateMgr::IsPackage( void )
{
	if ( d_dateMgr )
	{
		return d_dateMgr->IsPackage();
	}
	return false;
}

