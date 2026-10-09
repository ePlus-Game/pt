//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 11:41
//      File_base        : KItemSocket
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _kitemsocket_h_
#define _kitemsocket_h_


#include "kitemsocketset.h"

class KItemSocket
{
	friend class KItemSocketSet;
public:
	KItemSocket( void );
	virtual ~KItemSocket( void );
private:
	bool					Set( const InlayStuff& stuff );
	void					Reset( void );
	bool					IsEmpty( void );
	int						GetInlayGroupID( void );
	bool					GetInlayStuff( InlayStuff& stuff );
	bool					GetInlayEffect( InlayEffect* effect );
	bool					GetInlayYaoByBuffID( short buffID, int* yaoID );
private:
	InlayStuff				m_inlaySruff;
	const KBASICPROP_ITEM*	m_itemTemplate;
};

#endif
