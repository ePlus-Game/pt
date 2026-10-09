#ifndef KItemSetH
#define	KItemSetH

#include "KLinkArray.h"
#include "KItem.h"

#define		IN
#define		OUT

// enum enumAbrade
// {
// 	enumAbradeAttack = 0,
// 	enumAbradeDefend,
// 	enumAbradeMove,
// 	enumAbradeProduce,
// 	enumAbradeNum,
// };
// 玩家之间交易进行时，用于判断玩家物品栏能否放下买进的物品
typedef struct
{
	int		m_nIdx;
	int		m_nX;
	int		m_nY;
} TRADE_ITEM_INFO;

typedef struct
{
	int		nPriceScale;
	int		nMagicScale;
} REPAIR_ITEM_PARAM;

class KItemSet
{
public:
	KItemSet();
	~KItemSet();
public:
	void				Init				( void							);
	int					GetItemCount		( IN int nItemGenre = -1		);
	int					SearchID			( IN DWORD dwID					);
	KItem* GetItem(DWORD itemID)
	{
		int itemIndex = SearchID(itemID);
		if (itemIndex > 0)
		{
			return &Item[itemIndex];
		}
		else
		{
			return NULL;
		}
	}

	int					Add					( KItem* pItem					);
	int					Add(
							IN int nItemGenre, 
							IN int nDetail,
							IN int nParticular, 
							IN int nLevel, 
							IN int nItemCount);

	int					Add(IN int nGenHashID, 
							IN int nLevel,
							IN int nItemCount );

	void				Remove				( IN int nIdx					);
//	int					GetAbradeRange		( IN int nType, IN int nPart	);
#ifdef  _SERVER
	void				BackItemInfo		( void							);
#endif
private:
	void				SetID				( IN int nIdx					);
	int					FindFree			( void							);
private:
	DWORD				m_dwIDCreator;																//	ID生成器，用于客户端与服务器端的交流
	KLinkArray			m_FreeIdx;																	//	可用表
	KLinkArray			m_UseIdx;																	//	已用表
#ifdef _SERVER
	INDEXIDMAP			m_ItemUseIndex;
#endif
	int					m_nNumItems;
public:
	REPAIR_ITEM_PARAM	m_sRepairParam;

	//等待删除（xiehong－2006年11月23日）
#ifdef _SERVER
	TRADE_ITEM_INFO*	m_psItemInfo;																// 玩家之间交易进行时，用于判断玩家物品栏能否放下买进的物品
	TRADE_ITEM_INFO*	m_psBackItemInfo;															// 玩家之间交易进行时，用于判断玩家物品栏能否放下买进的物品
	PlayerItem			m_sLoseItemFromEquipmentRoom[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];	// 用于玩家被PK死亡后的惩罚计算，掉落随身物品
	PlayerItem			m_sLoseEquipItem[itempart_num];												// 用于玩家被PK死亡后的惩罚计算，掉落穿在身上的装备
#endif

};

extern KItemSet	ItemSet;
#endif
