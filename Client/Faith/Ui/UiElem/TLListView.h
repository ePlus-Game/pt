// TLListView.h: interface for the TLListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLLISTVIEW_H__648E9181_C0F3_4149_BBFC_BC0D34DD346D__INCLUDED_)
#define AFX_TLLISTVIEW_H__648E9181_C0F3_4149_BBFC_BC0D34DD346D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <TLVertScrollbar.h>
#include <TLStatic.h>
#include "TLListViewItem.h"

#include <vector>
#include <map>

using std::vector;
using std::map;

namespace CEGUI
{
	typedef vector< ListViewItem > ListViewItemList;
	typedef map< Window*, int > Bar_ListViewItem_Map;
	typedef void (* SetBarCallBackFunc )( const ListViewItem* );

/********************************************************************
/*						class: TLListView
*********************************************************************/
class TLListView : public TLStaticImage
{
public:
	TLListView( const String& type, const String& name );
	virtual ~TLListView();

	virtual void 
	initialise();

	void
	SetBarLayoutsFileName( const String& barLayoutFileName );

	void
	SetBarHandler( IBarHandler* pHandler );

	void
	SetHoverName( String& name );

	ListViewItem&
	GetSelectedItem();

	template< typename CallerType > void
	SetBarHandler( void ( CallerType::*func )( const ListViewItem* ), CallerType* caller )
	{ destroyBinder(); if ( NULL == m_pBinder ) { m_pBinder = new KCallBackBinder< CallerType >( func, caller ); } }

	void
	SetBarHandler( SetBarCallBackFunc func );

	void
	SetTitleSpaceHeight( int height );

	void
	SetIsBarUseLayout( bool flag );

	bool
	GetIsBarUseLayout() const;

	template< typename DataType > void
	RefreshBars( const DataType* pData,  const int dataCount )
	{ RefreshBarsImp( pData, dataCount, sizeof( DataType ) ); };

	template< typename DataType > void
	RefreshBars( const vector< DataType >& vec )
	{ if ( !vec.empty() ) { RefreshBarsImp( &vec[0], vec.size(), sizeof( DataType ) ); } };

protected:
	virtual void	
	onMoved( WindowEventArgs& e );

	virtual void	
	onSized( WindowEventArgs& e );

	virtual TLVertScrollbar* 
	createScrollbar( const String& name ) const;

	virtual TLStaticImage* 
	createContainer( const String& name ) const;

	virtual TLStaticImage* 
	createList( const String& name ) const;

	virtual void
	setScrollbar_ContainerImage( TLVertScrollbar* pScrollbar ) const;
	
	virtual void
	setScrollbar_UpImage( TLVertScrollbar* pScrollbar ) const;

	virtual void
	setScrollbar_DownImage( TLVertScrollbar* pScrollbar ) const;

	virtual void
	setScrollbar_ThumbImage( TLVertScrollbar* pScrollbar ) const;

	virtual void
	clearBar( Window* pBar );

	virtual void
	clearAllBar();
	
	virtual void
	handleBar( const ListViewItem* pItem );

	virtual void
	createNewBar( int barIndex );

	virtual void
	resetScrollbar();

	virtual void 
	RefreshBarsImp( const void* pData, const int dataCount, const int dataSize );

	bool
	panel_MouseWheel( const EventArgs& e );

	bool
	bar_MouseClick( const EventArgs& e );

	bool
	bar_MouseDoubleClick( const EventArgs& e );

	bool
	scrollBar_ScrollPositionChanged( const EventArgs& e );

	void
	setItemHover( ListViewItem& item, bool flag );

	void
	setWindowBarSelected( Window& wnd );

private:
	void
	destroyBinder();

	void
	setLayoutClipper( const Window& bar );

public:
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget
	static const utf8*	ScrollBarWidgetType;
	static const utf8*	ContainerWidgetType;
	static const utf8*	ListWidgetType;

protected:
	TLVertScrollbar*	m_pScrollbar;
	TLStaticImage*		m_pContainer;
	TLStaticImage*		m_pList;
	IBarHandler*		m_pBarHandler;
	ICallBack*			m_pBinder;

	String				m_barLayoutsFileName;
	int					m_barHeight;
	int					m_exceedHeight;
	int					m_pageHeight;
	int					m_titleSpaceHeight;
	int					m_pCurrentSelectedItemIndex;
	String				m_hoverName;
	bool				m_bIsBarUseLayout;

	ListViewItemList	m_barList;
	SetBarCallBackFunc	m_barCallBack;
	static const int	s_DefaultScrollBarWidth;
	Bar_ListViewItem_Map m_bar_ListViewItem_Map;
};	


/********************************************************************
/*						class: TLQuestionWindowFactory
*********************************************************************/
class TAHAREZLOOK_API TLListViewFactory : public WindowFactory
{
public:
	TLListViewFactory() : WindowFactory( TLListView::WidgetTypeName ){}
	~TLListViewFactory(){}
public:
	Window*	
	createWindow( const String& name );
	
	virtual void	
	destroyWindow( Window* window )	 { if ( window->getType() == d_type ) delete window; }
};

}


#endif // !defined(AFX_TLLISTVIEW_H__648E9181_C0F3_4149_BBFC_BC0D34DD346D__INCLUDED_)
