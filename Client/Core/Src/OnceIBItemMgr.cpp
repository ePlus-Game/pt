//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/23/2008 21:35
//      File_base        : OnceIBItemMgr
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "OnceIBItemMgr.h"

#define ONCE_IB_ITEM_PATH "\\settings\\onceibitempath.txt"

KOnceIBItemMgr::KOnceIBItemMgr( void )
{
	memset( &d_OnceIBItemList, 0, sizeof(d_OnceIBItemList) );
}

KOnceIBItemMgr::~KOnceIBItemMgr( void ) 
{

}

KOnceIBItemMgr& KOnceIBItemMgr::GetSingleten( void )
{
	static KOnceIBItemMgr s_OnceIBItem;
	return s_OnceIBItem;
}

void KOnceIBItemMgr::LoadConfig( void )
{
	if ( d_TabFile.Load(ONCE_IB_ITEM_PATH) )
	{
		int nHeight = d_TabFile.GetHeight();
		if ( nHeight > 0 )
		{
			int nPos = 0;
			for ( nPos = 0; nPos < nHeight; ++nPos )
			{
				d_OnceIBItemList[nPos].bOnecItem = true;
				
				d_TabFile.GetInteger( nPos + 2, "gener", 0, &d_OnceIBItemList[nPos].goods.Genera );
				d_TabFile.GetInteger( nPos + 2, "detail", 0, &d_OnceIBItemList[nPos].goods.Detail );
				d_TabFile.GetInteger( nPos + 2, "particular", 0, &d_OnceIBItemList[nPos].goods.Particular );
				d_TabFile.GetInteger( nPos + 2, "level", 0, &d_OnceIBItemList[nPos].goods.Level );
				d_OnceIBItemList[nPos].number = 1;
				d_TabFile.GetInteger( nPos + 2, "price", 0, &d_OnceIBItemList[nPos].price );
				d_OnceIBItemList[nPos].shelfIdx = 0;

				int nShopIdx = 0;
				d_TabFile.GetInteger( nPos + 2, "moneytype", 0, &nShopIdx );
				d_OnceIBItemList[nPos].shopIdx = nShopIdx;
				
				if ( d_OnceIBItemList[nPos].shopIdx >= 4 )
				{
					d_OnceIBItemList[nPos].shopIdx = 0;
					d_OnceIBItemList[nPos].useTicket = true;
				}
				else
				{
					d_OnceIBItemList[nPos].useTicket = true;
				}
				
			}
		}
	}
}

void KOnceIBItemMgr::GetOnceItemParam( int index, ClientBuyGoods& buyGoods )
{
	if ( index >= 0 && index < MAX_ONCE_ITEM )
	{
		buyGoods = d_OnceIBItemList[index];
	}
}

bool KOnceIBItemMgr::IsOkOnceItemParam( int genre, int deital,int particular, int level, int price )
{
	int nPos = 0;
	for ( nPos = 0; nPos < MAX_ONCE_ITEM; ++nPos )
	{
		if ( d_OnceIBItemList[nPos].goods.Genera == genre &&
			d_OnceIBItemList[nPos].goods.Detail == deital &&
			d_OnceIBItemList[nPos].goods.Particular == particular &&
			d_OnceIBItemList[nPos].goods.Level == level &&
			d_OnceIBItemList[nPos].price == price )
		{
			return true;
		}
	}
	return false;
}
