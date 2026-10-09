// TLListViewItem.cpp: implementation of the TLListViewItem class.
//
//////////////////////////////////////////////////////////////////////

#include "TLListViewItem.h"

namespace CEGUI
{

/********************************************************************
/*						class: ListViewItem
*********************************************************************/
ListViewItem::ListViewItem( Window* pBar, /*void* pData,*/ int barIndex )
{
	m_pData		= NULL;
	m_pBar		= pBar;
	m_barIndex	= barIndex;
	m_dataSize	= 0;
}

ListViewItem::ListViewItem()
{
	m_pData		= NULL;
	m_pBar		= NULL;
	m_barIndex	= -1;
	m_dataSize	=  0;
}

ListViewItem::~ListViewItem()
{
	destroyData();
}

void ListViewItem::SetData( const void* pData, const int dataSize )
{
	destroyData();
	if ( ( NULL == m_pData ) && ( NULL != pData ) && ( dataSize > 0 ) )
	{
		m_pData = new byte[ dataSize ];
		ZeroMemory( m_pData, dataSize );
		memcpy( m_pData, pData, dataSize  );
	}
	else
	{
		m_pData = NULL;
	}
}

const void* ListViewItem::GetData() const
{
	return m_pData;
}

void ListViewItem::SetBar( Window* pBar )
{
	m_pBar = pBar;
}

Window* ListViewItem::GetBar() const
{
	return m_pBar;
}

void ListViewItem::SetIndex( int index )
{
	m_barIndex = index;	
}

int ListViewItem::GetIndex() const
{
	return m_barIndex;
}

void ListViewItem::destroyData()
{
	if ( NULL != m_pData )
	{
		delete[] m_pData;
		m_pData = NULL;
	}
}

ListViewItem::ListViewItem( const ListViewItem& item )
{
	m_pData		= NULL;
	m_pBar		= NULL;
	m_barIndex	= -1;
	m_dataSize	=  0;

	SetData( item.m_pData, item.m_dataSize );
	m_dataSize	= item.m_dataSize;
	m_barIndex	= item.m_barIndex;
	m_pBar		= item.m_pBar;
}

/********************************************************************
/*						class: ListViewEventArgs
*********************************************************************/
ListViewEventArgs::ListViewEventArgs()
{
	m_pItem	= NULL;
}

ListViewEventArgs::ListViewEventArgs( const ListViewItem* pItem )
{
	m_pItem = pItem;
}

void ListViewEventArgs::SetListViewItem( const ListViewItem* pItem )
{
	m_pItem = pItem;
}

const ListViewItem* ListViewEventArgs::GetListViewItem() const
{
	return m_pItem;	
}

//end of namespace CEGUI
}