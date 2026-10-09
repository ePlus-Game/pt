//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:20   10:10
//      File_base        : IBShopUtil
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "IBShopUtil.h"
#include "KItemGenerator.h"

void IBShopUtil::ClearGoodsList(IBGoods_ListEntry *&pGoodsList)
{
	IBGoods_ListEntry *pListEntry = pGoodsList;
	IBGoods_ListEntry *pNextEntry;

	while(NULL != pListEntry)
	{
		pNextEntry = pListEntry->pNextEntry;
		delete pListEntry;
		pListEntry = pNextEntry;
	}

	pGoodsList = NULL;
}

IBGoods_ListEntry* IBShopUtil::CreateGoodsListEntry(const IBGoods &goods)
{
	if (!IsGoodsValid(goods.Id))
		return NULL;

	IBGoods_ListEntry *pListEntry = new IBGoods_ListEntry;
	if(NULL == pListEntry)
		return NULL;

	pListEntry->goods = goods;
	pListEntry->pNextEntry = NULL;

	return pListEntry;
}

bool IBShopUtil::IsGoodsValid(const IBGoods_Id &goods)
{
	KItem pItem;
	BOOL bGenItem = g_ItemGen.Gen_Item(goods.Genera,
		goods.Detail,
		goods.Particular,
		goods.Level,
		1,
		&pItem);
	
	if (!bGenItem)
		return false;
	return true;
}