//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 12:17
//      File_base        : IItemInlay
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _iiteminlay_h_
#define _iiteminlay_h_

#include "KMath.h"
#include "ItemCommonDef.h"

struct InlayStuff
{
	InlayStuff( void )
	{
		nGenre		= 0;
		nDetail		= 0;
		nParticular = 0;
		nLevel		= 0;
	}
	short nGenre;
	short nDetail;
	short nParticular;
	short nLevel;
};

struct InlayEffect
{
	InlayEffect( void )
	{
		nBuffID = 0;
	}
	int nBuffID;
};

#endif