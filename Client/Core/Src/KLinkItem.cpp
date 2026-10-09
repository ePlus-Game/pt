
#include "KCore.h"
#include "KLinkItem.h"
#include "KPlayer.h"
#include "KItemGenerator.h"

#ifndef _SERVER

KLinkItem g_cLinkItem;
KLinkItem::KLinkItem()
{
	for(int i = 0; i < LINK_ITEM_MAX_COUNT; ++i)
	{
		_itemIdList[i].refCount = 0;
		_itemIdList[i].id = COMMON_ITEM_INVALID_ID;
	}
}

KLinkItem::~KLinkItem()
{
	
}

void KLinkItem::addItem(ITEM_SYNC* newItem)
{
	int insertIndex = 0;
	int minCount = 1000000;
		
	//来了新物品的时候先把引用计数全部减一
	for(int i = 0; i < LINK_ITEM_MAX_COUNT; ++i)
	{
		if(_itemIdList[i].refCount > 1)
		{
			--_itemIdList[i].refCount;
		}
	}
	
	//找到最少引用的
	for(int j = 0; j < LINK_ITEM_MAX_COUNT; ++j)
	{
		if(_itemIdList[j].id == newItem->m_ID)
		{
			insertIndex = j;
			break;
		}

		if(minCount > _itemIdList[j].refCount)
		{
			minCount = _itemIdList[j].refCount;
			insertIndex = j;
		}
	}
	
	int genRet = g_ItemGen.Gen_Item(
		newItem->m_Genre, 
		newItem->m_Detail, 
		newItem->m_Particur,
		newItem->m_Level,
		1,
		&_itemInfoList[insertIndex]);

	if(!genRet)
	{
		return;
	}
	
	//按照服务器端发过来的配置信息更新该物品的属性
	_itemInfoList[insertIndex].SetID(newItem->m_ID);
	
	_itemInfoList[insertIndex].SetMaxDurability(newItem->m_MaxDurability);
	_itemInfoList[insertIndex].SetDurability(newItem->m_Durability);
	_itemInfoList[insertIndex].SetLevelupTimes(newItem->m_LevelupTimes);	
	_itemInfoList[insertIndex].SetLevelupType(newItem->m_nLevelupType);
	_itemInfoList[insertIndex].SetYaoID(newItem->m_YaoID);
	for (int yaoAddonBuffLoopCount = 0; yaoAddonBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddonBuffLoopCount++)
	{
		_itemInfoList[insertIndex].SetYaoAddOn(yaoAddonBuffLoopCount, newItem->m_YaoAddOnBuffSet[yaoAddonBuffLoopCount]);
	}	
	_itemInfoList[insertIndex].SetCompBuffTemplateSet( newItem->m_compBuffTemplateSet, sizeof( WORD ) * COMPOUND_COUNT );
	_itemInfoList[insertIndex].SetPlusInfo( (char*)newItem->m_szPlusInfo );
	_itemInfoList[insertIndex].SetTalismanPotential(newItem->m_TalismanPotential);
	for (int talismanEnchaseLoopCount = 0; talismanEnchaseLoopCount < TM_HOLE_NUM; talismanEnchaseLoopCount++)
	{
		_itemInfoList[insertIndex].SetTalismanEnchase(talismanEnchaseLoopCount, newItem->m_TalismanEnchaseSet[talismanEnchaseLoopCount]);
	}	
	_itemInfoList[insertIndex].SetBind( newItem->m_IsBind );
	_itemInfoList[insertIndex].SetLockCount( newItem->m_LockCount );

	_itemInfoList[insertIndex].ClearSocketSet();
	for ( int nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
	{
		InlayStuff stuff;
		if ( newItem->m_socketSet[nSocketIdx].nGenre == -1 &&
			newItem->m_socketSet[nSocketIdx].nDetail == -1 &&
			newItem->m_socketSet[nSocketIdx].nParticular == -1 &&
			newItem->m_socketSet[nSocketIdx].nLevel == -1 )
		{
			continue;
		}
		else
		{

			stuff.nGenre		= newItem->m_socketSet[nSocketIdx].nGenre;
			stuff.nDetail		= newItem->m_socketSet[nSocketIdx].nDetail;
			stuff.nParticular	= newItem->m_socketSet[nSocketIdx].nParticular;
			stuff.nLevel		= newItem->m_socketSet[nSocketIdx].nLevel;
			if ( stuff.nGenre == 0 && 
				stuff.nDetail == 0 &&
				stuff.nParticular == 0 && 
				stuff.nLevel == 0 )
			{
				_itemInfoList[insertIndex].CreateSocket();
			}
			else
			{
				_itemInfoList[insertIndex].SetSocketSet( stuff );
			}
			
		}
	}
	_itemInfoList[insertIndex].SetInlayBaseBuffSet((short *)newItem->m_InlayBaseBuffSet );
	_itemInfoList[insertIndex].SetInlayYaoBuffSet((short *)newItem->m_InlayYaoBuffSet );
	_itemInfoList[insertIndex].SetInlaySpecialBuffSet((short *)newItem->m_InlaySpecialBuffSet );

	_itemIdList[insertIndex].id = newItem->m_ID;
	_itemIdList[insertIndex].refCount = LINK_ITEM_MAX_COUNT;
}

KItem* KLinkItem::get(FIND_ITEMINDEX_PARAM& item, int id)
{
	if(id != COMMON_ITEM_INVALID_ID)
	{
		//先到itemlist中查
		int itemIndex = Player[CLIENT_PLAYER_INDEX].GetItemList().SearchID(id);
		if(itemIndex != 0)
		{
			return &Item[itemIndex];
		}	
		
		//如果itemlist中没有，则到LinkItemlist中查
		for(int i = 0; i < LINK_ITEM_MAX_COUNT; ++i)
		{
			if(_itemIdList[i].id == id)
			{
				_itemIdList[i].refCount++;
				return &_itemInfoList[i];
			}
		}
	}
	
	static KItem tempItem;
	//如果linkitemlist中没有，则临时生成一个
	int ret = g_ItemGen.Gen_Item(
		item.nGenre, 
		item.nDetail, 
		item.nParticular,
		item.nLevel,
		1,
		&tempItem);

	if(!ret)
	{
		_ASSERT(ret);
		return NULL;
	}

	return &tempItem;
}

#endif