//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/18/2008 17:53
//      File_base        : IBMoney
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _IBMoney_H
#define _IBMoney_H


#include "IMoney.h"
#include "cfs_db_interface.h"

#ifdef _SERVER

struct _IBDSHOPBHeader : _DBProcHeader
{
	int itemHashID;
	int itemLevel;
	int nPrice;
};

#endif



class MoneyMgr;

class JinshanBi : public IMoney<DWORD>
{
	friend class MoneyMgr;
public:
	JinshanBi( void );
	~JinshanBi( void );
public:
	virtual DWORD DecReq( void* money );
	virtual void DecRet( void* money );
	virtual DWORD AddReq( void* money );
	virtual void AddRet( void* money );
private:
	virtual DWORD SetMoney( DWORD money );
	void	SetMax( DWORD money );
	void	SetMin( DWORD money );
};

class CreditPoint : public IMoney<int>
{
public:
	CreditPoint( void );
	~CreditPoint( void );
public:
	virtual int AddReq( void* money );
	virtual int DecReq( void* money );
	virtual void AddRet( void* money );
	virtual void DecRet( void* money );

private:
	int	SetMoney( int money );
	void	SetMax( int money );
	void	SetMin( int money );
};

class Point : public IMoney<int>
{
public:
	Point( void );
	~Point( void );
public:	
	virtual int AddReq( void* money );
	virtual void AddRet( void* money );
	virtual int DecReq( void* money );
	virtual void DecRet( void* money );
private:
	int	SetMoney( int money );    
   	void	SetMax( int money );
	void	SetMin( int money ); 
	int	SetMoneyPlus( int moneyPlus);
	int	GetMoneyPlus( void);
private:
	void         JinShanbiAddedToPoint(const int dwJinShanBi,int & nInterger,int & nPlus);
private:
	int    d_MoneyPlus;
};

class MoneyMgr
{
public:
	MoneyMgr( void );
	~MoneyMgr( void );

public:
	void SetBelongPlayer( KPlayer* player );

	void SetMoneySize( MoneyType type,  int min, int max );
	void GetMoneySize( MoneyType type,  int& min, int& max );

	int AddReq( MoneyType type, void* money );   //Notice 对与Point 传入的是消费的金山币
	int DecReq(  MoneyType type, void* money );

	int SetMoney( MoneyType type, int money );
	int GetMoney( MoneyType type );

	int SetMoneyPlus( MoneyType type, int money );
	int GetMoneyPlus( MoneyType type);

	DWORD SetJinshanbi( DWORD money );
	DWORD GetJinshanbi(  );

	void AddRet( MoneyType type, void* money );
	void DecRet(  MoneyType type, void* money );

private:
	bool IsOkMoneyType( MoneyType type );

private:
	IMoney<int>*		d_money[money_type_count];
	JinshanBi			d_jinshabi;
	CreditPoint			d_creditPoint;
	Point				d_point;
};

#endif