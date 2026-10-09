//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:19   18:17
//      File_base        : IBShopUtil
//      File_ext         : h
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
#ifndef _IBShopUtil_h
#define _IBShopUtil_h

#include "CoreUseNameDef.h"
#include "IBShopComDef.h"

class IBShopUtil
{
public:
	static void ClearGoodsList(IBGoods_ListEntry *&pGoodsList);
	static IBGoods_ListEntry* CreateGoodsListEntry(const IBGoods &goods);
	static bool IsGoodsValid(const IBGoods_Id &goods);

	static inline bool IsCSProtValid(int nProtocol)
	{
		return nProtocol > enIB_CSProt_Begin && nProtocol < enIB_CSProt_End;
	}

	static inline bool IsShopIndexValid(int nShopIndex)
	{
		return ( nShopIndex>-1 && nShopIndex<enSHOP_NUM );
	}

	static inline bool IsShelfIdxValid(int nShelfIdx)
	{
		return ( nShelfIdx>-1 && nShelfIdx<MAX_SHELF_NUM );
	}

	static inline bool IsErrCodeInvalid(int nErrCode)
	{
		return nErrCode > enIBShopErr_None && nErrCode < enIBShopErr_End;
	}
	
	static inline const char* GetErrMsg(int nErrCode)
	{
		if( IsErrCodeInvalid(nErrCode) )
			return g_szIBShopErrMsg[nErrCode];
		else
			return NULL;
	}
	
	static void PushToGoodsListHead(IBGoods_ListEntry *pGoodsEntry, IBGoods_ListEntry *&pList)
	{
		if(NULL != pGoodsEntry)
		{
			pGoodsEntry->pNextEntry = pList;
			pList = pGoodsEntry;
		}
	}

};


#endif