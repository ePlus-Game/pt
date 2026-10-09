#ifndef _KLINKITEM_H
#define _KLINKITEM_H

#ifndef _SERVER

#define LINK_ITEM_MAX_COUNT 200

#include "KItem.h"

class KLinkItem
{
	struct KLinkItemType 
	{
		int id;
		int refCount;
	};

	KLinkItemType _itemIdList[LINK_ITEM_MAX_COUNT];
	KItem		_itemInfoList[LINK_ITEM_MAX_COUNT];
public:
	void addItem(ITEM_SYNC* newItem);
	KItem* get(FIND_ITEMINDEX_PARAM& item, int id);
	KLinkItem();
	~KLinkItem();
};

extern KLinkItem g_cLinkItem;
#endif

#endif