//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:4:19   9:46
//      File_base        : IBAllocator
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
#ifndef _IBAllocator_h
#define _IBAllocator_h

/*#include "buff_alloc.h"
#include "SocialAllocator.h"
#include "IBShopComDef.h"
#include "KItem.h"

enum
{
	__ALLOC_MAXCOUNT_GOODS = 1024,
	__ALLOC_GRANULARITY = 128,
};


class IBAllocator
{
public:
	static IBGoods_ListEntry* AllocListEntry()
	{
		return (IBGoods_ListEntry*)__GoodsListEntryAlloc._alloc();
	}

	static void FreeListEntry(IBGoods_ListEntry *pListEntry)
	{
		if(NULL != pListEntry)
			__GoodsListEntryAlloc._free(pListEntry);
	}

	static KItem* AllocItem()
	{
		return (KItem*)__ItemAlloc._alloc();
	}

	static void FreeItem(KItem *pItem)
	{
		if(NULL != pItem)
			__ItemAlloc._free(pItem);
	}	

private:
	static __allocator<KItem, 
		__ALLOC_GRANULARITY, 
		__ALLOC_MAXCOUNT_GOODS>	__ItemAlloc;

	static __simpleallocator<sizeof(IBGoods_ListEntry), 
		__ALLOC_GRANULARITY, 
		__ALLOC_MAXCOUNT_GOODS> __GoodsListEntryAlloc;		
};

__allocator<KItem, 
			__ALLOC_GRANULARITY, 
			__ALLOC_MAXCOUNT_GOODS>	IBAllocator::__ItemAlloc;

__simpleallocator<sizeof(IBGoods_ListEntry),
				  __ALLOC_GRANULARITY, 
				  __ALLOC_MAXCOUNT_GOODS> IBAllocator::__GoodsListEntryAlloc;	
//*/
#endif