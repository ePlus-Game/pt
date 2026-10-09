// TLListViewItem.h: interface for the TLListViewItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLLISTVIEWITEM_H__9271DD68_AF63_4162_BF78_99E6F905CC89__INCLUDED_)
#define AFX_TLLISTVIEWITEM_H__9271DD68_AF63_4162_BF78_99E6F905CC89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <TLStatic.h>


namespace CEGUI
{

/********************************************************************
/*						class: TLListViewItem
*********************************************************************/
class ListViewItem
{
public:
	ListViewItem();
	ListViewItem( Window* pBar, /*void* pData, */int barIndex );
	ListViewItem( const ListViewItem& item );

	virtual ~ListViewItem();

	void
	SetData( const void* pData, const int dataSize );
	
	const void*
	GetData() const;
	
	void
	SetBar( Window* pBar );
	
	Window*
	GetBar() const;

	void 
	SetIndex( int index );

	int
	GetIndex() const;

private:
	void
	destroyData();

private:
	Window*		m_pBar;
	int			m_barIndex;
	byte*		m_pData;
	int			m_dataSize;
};


/********************************************************************
/*						interface: IBarHandler
*********************************************************************/
class IBarHandler
{
public:
	virtual void 
	setBar( const ListViewItem* pItem ) = 0;
};


/********************************************************************
/*						interface: ICallBack
*********************************************************************/
class ICallBack
{
public:
	virtual void
	DoCallBack( const ListViewItem* pItem ) = 0;
};

/********************************************************************
/*						class: KCallBackBinder
*********************************************************************/
template < typename CallerType >
class KCallBackBinder : public ICallBack
{
	typedef void ( CallerType::* HandleBarCallBack )( const ListViewItem* pItem );
public:
	KCallBackBinder( HandleBarCallBack func, CallerType* pCaller )
	{ m_pCaller = pCaller; m_pFunc = func; };

	void
	DoCallBack( const ListViewItem* pItem )
	{ if ( NULL != m_pCaller ) { ( m_pCaller->*m_pFunc )( pItem ); } }
		
private:
	CallerType*			m_pCaller;
	HandleBarCallBack	m_pFunc;
};

/********************************************************************
/*						class: ListViewEventArgs
*********************************************************************/
class ListViewEventArgs: public EventArgs
{
public:
	ListViewEventArgs();
	ListViewEventArgs( const ListViewItem* pItem );
	
	void
	SetListViewItem( const ListViewItem* pItem );
	
	const ListViewItem* 
	GetListViewItem() const;
	
private:
	const ListViewItem*	m_pItem;
};

class NoneSelectedItemException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	NoneSelectedItemException() : Exception( "There is no selected item!" ) {}
	NoneSelectedItemException(const String& message) : Exception(message) {}
};

class ItemIndexOutOfBoundException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	ItemIndexOutOfBoundException() : Exception( "Item Index is out of bound!" ) {}
	ItemIndexOutOfBoundException(const String& message) : Exception(message) {}
};

class EmptyFileNameException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	EmptyFileNameException() : Exception( "The file name is empty!" ) {}
	EmptyFileNameException(const String& message) : Exception(message) {}
};

//end of namespace CEGUI 
}

#endif // !defined(AFX_TLLISTVIEWITEM_H__9271DD68_AF63_4162_BF78_99E6F905CC89__INCLUDED_)
