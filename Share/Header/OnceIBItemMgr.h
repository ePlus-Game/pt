//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/23/2008 21:35
//      File_base        : OnceIBItemMgr
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KTabFile.h"
#include "IBShopComDef.h"

#define MAX_ONCE_ITEM 4000

class KOnceIBItemMgr 
{
private:
	KOnceIBItemMgr( void );
public:
	~KOnceIBItemMgr( void );
public:
	static KOnceIBItemMgr& GetSingleten( void );
	void LoadConfig( void );
	void GetOnceItemParam( int index, ClientBuyGoods& buyGoods );
	bool IsOkOnceItemParam( int genre, int deital,int particular, int level, int price );
private:
	ClientBuyGoods d_OnceIBItemList[MAX_ONCE_ITEM];
	KTabFile d_TabFile;
};