// kItemdateparser.h: interface for the KItemDateParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KITEMDATEPARSER_H__CB00D2B4_50F7_41E5_9E01_4416DF2FF818__INCLUDED_)
#define AFX_KITEMDATEPARSER_H__CB00D2B4_50F7_41E5_9E01_4416DF2FF818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KItemList.h"

class KItemDateParser  
{
public:
	KItemDateParser( void ) {}
	KItemDateParser( int version ) {};
	virtual ~KItemDateParser() {};
public:
	virtual int		Parse( KItemList& itemList ) = 0;
	virtual int		Pack( const KItemList& itemList, const int itemListIdx, KItem& item ) = 0;
	virtual int		Pack( int pos, int x, int y, KItem& item ) = 0;
	virtual	void	SetBegin( TDBItemData_Base* pItemDate ) = 0;
	virtual void	Next( void ) = 0;
	virtual bool	IsPackage( void ) = 0;
	virtual int		GetSizeOfTDBItemData( void ) = 0;
	virtual int		GetSizeOf_TDBItemData( void ) = 0;
protected:
	char*			d_point;
	int				d_itemIndex;
	int				d_version;
};

class KItemDateParser_Version1 : public KItemDateParser
{
public:
	KItemDateParser_Version1( void ) {}
	KItemDateParser_Version1( int version );
	~KItemDateParser_Version1( void );
public:
	virtual int		Parse( KItemList& itemList );
	virtual int		Pack( const KItemList& itemList, const int itemListIdx, KItem& item );
	virtual int		Pack( int pos, int x, int y, KItem& item );
	virtual	void	SetBegin( TDBItemData_Base* pItemDate );
	virtual void	Next( void );
	virtual bool	IsPackage( void );
	virtual int		GetSizeOfTDBItemData( void );
	virtual int		GetSizeOf_TDBItemData( void );
};

class KItemDateParser_Version2 : public KItemDateParser_Version1
{
public:
	KItemDateParser_Version2( void ) {}
	KItemDateParser_Version2( int version );
	~KItemDateParser_Version2( void );
public:
	virtual int		Parse( KItemList& itemList );
	virtual int		Pack( const KItemList& itemList, const int itemListIdx, KItem& item );
	virtual int		Pack( int pos, int x, int y, KItem& item );
	virtual	void	SetBegin( TDBItemData_Base* pItemDate );
	virtual void	Next( void );
	virtual bool	IsPackage( void );
	virtual int		GetSizeOfTDBItemData( void );
	virtual int		GetSizeOf_TDBItemData( void );
};

class KItemDateParser_Version3 : public KItemDateParser_Version2
{
public:
	KItemDateParser_Version3( void ) {}
	KItemDateParser_Version3( int version );
	~KItemDateParser_Version3( void );
public:
	virtual int		Parse( KItemList& itemList );
	virtual int		Pack( const KItemList& itemList, const int itemListIdx, KItem& item );
	virtual int		Pack( int pos, int x, int y, KItem& item );
	virtual	void	SetBegin( TDBItemData_Base* pItemDate );
	virtual void	Next( void );
	virtual bool	IsPackage( void );
	virtual int		GetSizeOfTDBItemData( void );
	virtual int		GetSizeOf_TDBItemData( void );
};

class KItemDateParser_Version4 : public KItemDateParser_Version3
{
public:
	KItemDateParser_Version4( void ){}
	KItemDateParser_Version4( int version );
	~KItemDateParser_Version4( void );
public:
	virtual int		Parse( KItemList& itemList );
	virtual int		Pack( const KItemList& itemList, const int itemListIdx, KItem& item );
	virtual int		Pack( int pos, int x, int y, KItem& item );
	virtual	void	SetBegin( TDBItemData_Base* pItemDate );
	virtual void	Next( void );
	virtual bool	IsPackage( void );
	virtual int		GetSizeOfTDBItemData( void );
	virtual int		GetSizeOf_TDBItemData( void );
};

class KItemDateParser_Version5 : public KItemDateParser_Version4
{
public:
	KItemDateParser_Version5( void ){}
	KItemDateParser_Version5( int version );
	~KItemDateParser_Version5( void );
public:
	virtual int		Parse( KItemList& itemList );
	virtual int		Pack( const KItemList& itemList, const int itemListIdx, KItem& item );
	virtual int		Pack( int pos, int x, int y, KItem& item );
	virtual	void	SetBegin( TDBItemData_Base* pItemDate );
	virtual void	Next( void );
	virtual bool	IsPackage( void );
	virtual int		GetSizeOfTDBItemData( void );
	virtual int		GetSizeOf_TDBItemData( void );
};

class KItemDateMgr
{
public:
	KItemDateMgr( int itemVersion, TDBItemData_Base* pItemDate );
	~KItemDateMgr( void );
public:
	int							GetSizeOfTDBItemData( void ) const;
	int							GetSizeOf_TDBItemData( void ) const;	
	 bool						IsPackage( void );					
	int							Parse(
									KItemList& itemList );
	int							Pack( 
									const KItemList& itemList,
									const int itemListIdx,
									KItem& item );
	int							Pack( 
									int pos,
									int x,
									int y,
									KItem& item );
	void						Next( void );
private:
	KItemDateParser*			d_dateMgr;
	int							d_itemVersion;
};



#endif // !defined(AFX_KITEMDATEPARSER_H__CB00D2B4_50F7_41E5_9E01_4416DF2FF818__INCLUDED_)
