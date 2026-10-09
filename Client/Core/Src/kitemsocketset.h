//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 14:47
//      File_base        : KItemSocketSet
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _kitemsocketset_h_
#define _kitemsocketset_h_

#include "iiteminlay.h"
#include "kitemsocket.h"
#include <vector>
#include <string>

class KItem;

class KItemSocketSet
{
	typedef std::vector<KItemSocket> _ItemSocketSet;
public:
	KItemSocketSet( void ) {};
	~KItemSocketSet( void ) {};
public:
	bool			CreateSocket( void );
	void			DestroySocket( void );
	int				GetSocketCount( void );
	int				GetUseSocketCount( void );
	bool			SetInlayStuffBySocketIdx( 
						int socketIdx, 
						const InlayStuff& stuff );
	bool			GetInlayStuffBySocketIdx( 
						int socketIdx, 
						InlayStuff& stuff );
	bool			GetInlayEffectBySocketIdx( 
						int socketIdx, 
						InlayEffect* effect );
	bool			GetInlayYaoBySocketIdxAndBuffID(
						int socketIdx, 
						short buffID,
						int* yaoID );
	int				GetInlayGroupIDBySocketIdx( int socketIdx );
	bool			IsEmptyBySocketIdx( int socketIdx );
	bool			GetInlaySpecialEffect(
						const KItem* item, 
						InlayEffect* effect );
	bool			GetInlaySpecialEffectName(
						const KItem* item, 
						std::string& name );
private:
	bool			IsValidSocketIndex( int socketIdx );
	bool			GetSocket( int socketIdx, KItemSocket* socket );
private:
	_ItemSocketSet	m_itemSocketSet;
};

#endif