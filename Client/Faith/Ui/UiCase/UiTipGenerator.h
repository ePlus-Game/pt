//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007年3月22日
//      File_base        : 
//      File_ext         : h
//      Author           : xiehong
//      Description      : tip描述生成
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UITIPGENERATOR_H
#define UITIPGENERATOR_H

#include "layoutinterface.h"

#define UI_ITEM_TIP_MAX_ITEM_ID_COUNT 5

class KUiTipGenerator
{
public:
	enum TipObjectType
	{
		MyItem,
		OppositeItem,
		MyGua,
		OppositeGua,
		InsideBall,
		LinkedItem,
		VendueItem,
		ShopItem,
		QuestIcon,
		PlusPointShopItem,
	};
	
	struct TipObject
	{
		TipObjectType	type;
		int				count;
		int				ids[UI_ITEM_TIP_MAX_ITEM_ID_COUNT];
		bool operator==(TipObject& other)
		{
			if(type != other.type)
			{
				return false;
			}
			
			for(int i = 0; i < UI_ITEM_TIP_MAX_ITEM_ID_COUNT; ++i)
			{
				if(ids[i] != other.ids[i])
				{
					return false;
				}
			}
			return true;
		}
	};
private:
	char		d_headText[256];
	char		d_layoutText[LAYOUT_TEXT_MAX_LEN];
	char		d_compareLayoutText[LAYOUT_TEXT_MAX_LEN];
	TipObject	d_lastObject;
	void addItemPrice(TipObject& tipObj);
	void addItemPlusPointPrice(TipObject& tipObj);
public:
	KUiTipGenerator();
	~KUiTipGenerator(){};
	
	char* genLayoutDes(TipObject& tipObj);
	char* genCompareLayoutDes(TipObject& tipObj);

	static KUiTipGenerator& getSinglton()
	{
		static KUiTipGenerator s_layoutGen;
		return s_layoutGen;
	}
};

#endif