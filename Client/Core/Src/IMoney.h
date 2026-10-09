//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/18/2008 17:39
//      File_base        : IMoney
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _IMoney_H
#define _IMoney_H

#include "AccountLoginDef.h"

class KPlayer;

enum MoneyType
{
	jinshanbi,
	creditpoint,
	point,
	money_type_count,
};

enum MoneyError
{
	money_ok,
	error_param,
	not_allow_oper,
	not_allow_money_type,
	not_enough_money,
	wait_add_ret,
	wait_dec_ret,
	over_max,
	below_min,
};

#define DWORD_MAX_LIMIT 4000000000
#define DWORD_MIN_LIMIT 0

#define LONG_MAX_LIMIT 2000000000
#define LONG_MIN_LIMIT -2000000000


class MoneyMgr;

template<typename Type>
class IMoney 
{
	friend class MoneyMgr;
public:
	IMoney( void );
	~IMoney( void );
public:

	virtual Type	AddReq( void* money ) = 0;     
	virtual Type	DecReq( void* money ) = 0;
	virtual void	AddRet( void* money ) = 0;
	virtual void	DecRet( void* money ) = 0;

protected:
	virtual void	SetBelongPlayer( KPlayer* player );
	virtual Type	SetMoney( Type money );        //设置可交易值
	virtual Type    SetMoneyPlus(Type money);      //设置不可交易值
	virtual Type    GetMoneyPlus( void );
	virtual Type	GetMoney( void );	
	virtual	bool	IsGoodParam( void * money );
	virtual bool	IsEnough( Type money );
	virtual Type	Add( Type money );
	virtual Type	Dec( Type money );
	virtual void	SetMax( Type money );
	virtual void	SetMin( Type money );
	virtual Type	GetMax( void );
	virtual Type	GetMin( void );
	virtual bool	OverMax( Type money );
	virtual bool	OverMin( Type money );

protected:
	Type		d_money;
	Type		d_min;
	Type		d_max;
	KPlayer*	d_player;
};

template<typename Type>
IMoney<Type>::IMoney( void )
{
	d_money		= 0;
	d_min		= 0;
	d_max		= 0;
	d_player	= NULL;
}

template<typename Type>
IMoney<Type>::~IMoney( void )
{

}

template<typename Type>
void	IMoney<Type>::SetBelongPlayer( KPlayer* player )
{
	d_player = player;
}

template<typename Type>
bool	IMoney<Type>::IsEnough( Type money )
{
	if ( money <= d_money )
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<typename Type>
Type    IMoney<Type>::GetMoneyPlus()
{
	return 0;
}

template<typename Type>
Type    IMoney<Type>::SetMoneyPlus(Type money)
{
	return 0;
};

template<typename Type>
Type	IMoney<Type>::SetMoney( Type money )
{
	if ( OverMax( money ) )
	{
		money = GetMax();
		return over_max;
	}

	if ( OverMin( money ) )
	{
		d_money = GetMin();
		return below_min;
	}

	d_money = money;
	return money_ok;
}

template<typename Type>
Type	IMoney<Type>::GetMoney( void )
{
	return d_money;
}

template<typename Type>
bool	IMoney<Type>::IsGoodParam( void * money )
{
	if ( d_player == NULL || money == NULL )
	{
		return false;
	}
	return true;
}

template<typename Type>
Type	IMoney<Type>::Add( Type money )
{
	return SetMoney( d_money + money );
}

template<typename Type>
Type	IMoney<Type>::Dec( Type money )
{
	return SetMoney( d_money - money );
}

template<typename Type>
void	IMoney<Type>::SetMax( Type money )
{
	d_max = money;
}

template<typename Type>
void	IMoney<Type>::SetMin( Type money )
{
	d_min = money;
}

template<typename Type>
Type	IMoney<Type>::GetMax( void )
{
	return d_max;
}

template<typename Type>
Type	IMoney<Type>::GetMin( void )
{
	return d_min;
}

template<typename Type>
bool	IMoney<Type>::OverMax( Type money )
{
	return money > GetMax();
}

template<typename Type>
bool	IMoney<Type>::OverMin( Type money )
{
	return money < GetMin();
}

#endif
