// TLListView.cpp: implementation of the TLListView class.
//
//////////////////////////////////////////////////////////////////////

#include "TLListView.h"
#include <sstream>
#include <utility>

using std::ostringstream;
using std::make_pair;

namespace CEGUI
{

const utf8	TLListView::WidgetTypeName[]		= "TaharezLook/ListView";
const utf8* TLListView::ScrollBarWidgetType		= TLVertScrollbar::WidgetTypeName;
const utf8* TLListView::ContainerWidgetType		= TLStaticImage::WidgetTypeName;
const utf8* TLListView::ListWidgetType			= TLStaticImage::WidgetTypeName;
const int	TLListView::s_DefaultScrollBarWidth = 15;

void cerectToLorect(CEGUI::Rect* cerect, void* lorect)
{
	LORect* rect = (LORect*)lorect;
	rect->setPos(cerect->d_left, cerect->d_top);
	rect->setWidth(cerect->getWidth());
	rect->setHeight(cerect->getHeight());
}

/********************************************************************
/*						class: TLListView
*********************************************************************/
TLListView::TLListView( const String& type, const String& name )
: TLStaticImage(type, name)
{
	m_pScrollbar				= NULL;
	m_pContainer				= NULL;
	m_pList						= NULL;
	m_pBarHandler				= NULL;
	m_pBinder					= NULL;
	m_barCallBack				= NULL;
	

	m_barHeight					= -1;
	m_pCurrentSelectedItemIndex	= -1;
	m_exceedHeight				=  0;
	m_pageHeight				=  0;
	m_titleSpaceHeight			=  0;
	m_bIsBarUseLayout				= false;

	addEvent( EventListViewUpdateItem		);
	addEvent( EventListViewItemClick		);
	addEvent( EventListViewItemDoubleClick	);
	addEvent( EventListViewItemSelected		);
}

TLListView::~TLListView()
{
	destroyBinder();
}

void TLListView::initialise()
{
	m_pScrollbar = createScrollbar( getName() + "__auto___scrollbar__" );
	addChildWindow( m_pScrollbar );

	m_pContainer = createContainer( getName() + "__auto___container__" );
	addChildWindow( m_pContainer );

	m_pList = createList( getName() + "__auto___list__" );
	m_pContainer->addChildWindow( m_pList );
	
	// do initial layout
	performChildWindowLayout();
}

TLVertScrollbar* TLListView::createScrollbar( const String& name ) const
{
	TLVertScrollbar* scrollbar = static_cast< TLVertScrollbar* >( 
		WindowManager::getSingleton().createWindow( ScrollBarWidgetType, name ) ) ;

	scrollbar->setMLClickedSoundEnable( true );
	scrollbar->setMouseLClickedSound( 
		const_cast< Sound* >( &SoundSetManager::getSingletonPtr()->GetSoundSet( SoundSet )->GetSound( "Dianjihuadongtiao" ) ) );

	setScrollbar_ContainerImage( scrollbar );
	setScrollbar_UpImage( scrollbar );
	setScrollbar_DownImage( scrollbar );
	setScrollbar_ThumbImage( scrollbar );

	scrollbar->setWidth( Absolute, s_DefaultScrollBarWidth );
	int parentWidth = this->getWidth( Absolute );
	scrollbar->setVisible( true );
	scrollbar->setZLevel( Window::Top );

	scrollbar->subscribeEvent( 
		TLVertScrollbar::EventScrollPositionChanged,
		Event::Subscriber( &TLListView::scrollBar_ScrollPositionChanged, const_cast< TLListView* >( this ) ) );
	
	return scrollbar;	
}


TLStaticImage* TLListView::createContainer( const String& name ) const
{
	TLStaticImage* container = static_cast< TLStaticImage* >( 
		WindowManager::getSingleton().createWindow( ContainerWidgetType, name ) );
	container->setXPosition( Absolute, 0 );
	container->setYPosition( Absolute, 0 );
	return container;
}

TLStaticImage* TLListView::createList( const String& name ) const
{
	TLStaticImage* list = static_cast< TLStaticImage* >( 
		WindowManager::getSingleton().createWindow( ContainerWidgetType, name ) ) ;	
	list->setXPosition( Absolute, 0 );
	list->setYPosition( Absolute, 0 );
	return list;	
}

void TLListView::setScrollbar_ContainerImage( TLVertScrollbar* pScrollbar ) const
{
	if ( NULL != pScrollbar )
	{
		pScrollbar->setContainerTopImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangshang_normal" ) ) );
		
		pScrollbar->setContainerMiddleImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodongtiao" )->getImage( "chang" ) ) );
		
		pScrollbar->setContainerBottomImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangxia_normal" ) ) );	
	}
}

void TLListView::setScrollbar_UpImage( TLVertScrollbar* pScrollbar ) const
{
	if ( NULL != pScrollbar )
	{
		pScrollbar->setUpNormalImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangshang_hover" ) ) );
		
		pScrollbar->setUpHoverImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangshang_hover" ) ) );
		
		pScrollbar->setUpPushedImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangshang_push" ) ) );
	}
}

void TLListView::setScrollbar_DownImage( TLVertScrollbar* pScrollbar ) const
{
	if ( NULL != pScrollbar )
	{
		pScrollbar->setDownNormalImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangxia_normal" ) ) );
		
		pScrollbar->setDownHoverImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangxia_hover" ) ) );
		
		pScrollbar->setDownPushedImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "xiangxia_push" ) ) );
	}	
}

void TLListView::setScrollbar_ThumbImage( TLVertScrollbar* pScrollbar ) const
{
	if ( NULL != pScrollbar )
	{
		pScrollbar->setThumbNormalImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "huadong_normal" ) ) );
		
		pScrollbar->setThumbHoverImage( 
			&( ImagesetManager::getSingleton().getImageset( "tuodonganniu" )->getImage( "huadong_hover" ) ) );
	}
}

void TLListView::onMoved( WindowEventArgs& e )
{
	TLStaticImage::onMoved( e );
	if ( NULL != m_pScrollbar )
	{
		m_pScrollbar->setXPosition( Absolute, getWidth( Absolute ) - s_DefaultScrollBarWidth );
		m_pScrollbar->setYPosition( Absolute, 0 );
	}
}

void TLListView::onSized( WindowEventArgs& e )
{
	TLStaticImage::onSized( e );
	if ( NULL != m_pScrollbar )
	{
		m_pScrollbar->setHeight( Absolute, getHeight( Absolute ) );
	}
	if ( NULL != m_pContainer )
	{
		m_pContainer->setWidth( Absolute, getWidth( Absolute ) - s_DefaultScrollBarWidth );
		m_pContainer->setHeight( Absolute, getHeight( Absolute ) );
		m_pageHeight = m_pContainer->getHeight( Absolute );
	}
	if ( NULL != m_pList )
	{
		if ( NULL != m_pContainer )
		{
			m_pList->setWidth( Absolute, m_pContainer->getWidth( Absolute ) );
			m_pList->setHeight( Absolute, m_pContainer->getHeight( Absolute ) );
		}
	}
}

void TLListView::clearBar( Window* pBar )
{
	if ( NULL != pBar )
	{
		pBar->hide();
	}
}

bool TLListView::panel_MouseWheel( const EventArgs& e )
{
	MouseEventArgs* eventArgs = ( MouseEventArgs* )( &e );
	
	if( ( NULL != m_pScrollbar ) && m_pScrollbar->isVisible() )
	{
		float newPos = m_pScrollbar->getScrollPosition() - m_pScrollbar->getStepSize() * eventArgs->wheelChange;
		m_pScrollbar->setScrollPosition( newPos );
	}
	return true;
}

bool TLListView::scrollBar_ScrollPositionChanged( const EventArgs& e )
{
	if ( ( m_exceedHeight > 0 ) && ( NULL != m_pScrollbar ) )
	{
		float scrollPos = m_pScrollbar->getScrollPosition();
		int scrollSize = - scrollPos * m_exceedHeight;
		m_pList->setYPosition( Absolute, scrollSize );
	}
	return true;
}

void TLListView::SetBarLayoutsFileName( const String& barLayoutFileName )
{
	m_barLayoutsFileName = barLayoutFileName;
	if ( ! m_barLayoutsFileName.empty() )
	{
		Window* pCurrentBar = WindowManager::getSingleton().loadWindowLayout( m_barLayoutsFileName, getName() + "__tempBar_forCaluHeight_" );
		m_barHeight = pCurrentBar->getHeight( Absolute );
		WindowManager::getSingleton().destroyWindow( pCurrentBar );
	}
	else
	{
		throw EmptyFileNameException();
	}
}

void TLListView::SetBarHandler( IBarHandler* pHandler )
{
	m_pBarHandler = pHandler;
}

void TLListView::SetBarHandler( SetBarCallBackFunc func )
{
	m_barCallBack = func;
}

void TLListView::RefreshBarsImp( const void* pData, const int dataCount, const int dataSize )
{
	clearAllBar();
	const byte* pDataList = static_cast< const byte* >( pData );
	
	for ( int i = 0; i < dataCount; ++i )
	{
		if ( i >= m_barList.size() )
		{
			createNewBar( i );
		}
		
		if ( i < m_barList.size() )
		{
			ListViewItem& lvItem = m_barList[i];
			lvItem.SetData( pDataList, dataSize );
			handleBar( &lvItem );
// 			if ( NULL != lvItem.GetBar() )
// 			{
// 				setLayoutClipper( *lvItem.GetBar() );
// 			}
		}
		
		pDataList += dataSize;
	}
	
	m_exceedHeight = m_barHeight * dataCount - m_pageHeight;
	m_pList->setHeight( Absolute, m_barHeight * dataCount );
	
	resetScrollbar();
	return;
}

//该函数在for循环中被调用，所以不要在这里注册bar以外的控件的事件
void TLListView::handleBar( const ListViewItem* pItem )
{
	if ( NULL != pItem )
	{
		if ( NULL != m_pBarHandler )
		{
			m_pBarHandler->setBar( pItem );
		}
		if ( NULL != m_pBinder )
		{
			m_pBinder->DoCallBack( pItem );
		}
		if ( NULL != m_barCallBack )
		{
			m_barCallBack( pItem );
		}

		ListViewEventArgs arg( pItem );
		fireEvent( EventListViewUpdateItem, arg );

		pItem->GetBar()->show();
	}
}

void TLListView::resetScrollbar()
{
	if ( NULL != m_pScrollbar )
	{
		if ( m_exceedHeight > 0 )
		{
			float stepSize = static_cast< float >( m_barHeight  ) / static_cast< float >( m_exceedHeight );
			m_pScrollbar->setStepSize( ( ( stepSize > 0.0 ) && ( stepSize < 1.0 ) ) ? stepSize : 1.0 );
			m_pScrollbar->setVisible( true );
		}
		else
		{
			m_pScrollbar->setVisible( false );
		}
		
		m_pScrollbar->setScrollPosition( 0.0f );
	}
}

void TLListView::clearAllBar()
{
	for ( int j = 0; j < m_barList.size(); ++j )
	{
		clearBar( m_barList[j].GetBar() );
	}	
}

//该函数在for循环中被调用，所以不要在这里注册bar以外的控件的事件
void TLListView::createNewBar( int barIndex )
{
	Window* pCurrentBar = NULL;
	ostringstream curBarName;
	curBarName << m_pList->getName() << '__' << barIndex << '__';
	if ( ! m_barLayoutsFileName.empty() )
	{
		pCurrentBar = WindowManager::getSingleton().loadWindowLayout( m_barLayoutsFileName, curBarName.str() );
	}
	if ( NULL != pCurrentBar )
	{
		ListViewItem listviewItem( pCurrentBar, barIndex );
		m_barList.push_back( listviewItem );
		m_bar_ListViewItem_Map.insert( make_pair( pCurrentBar, m_barList.size() - 1 ) );
	
		pCurrentBar->setXPosition( Absolute, 0 );
		pCurrentBar->setYPosition( Absolute, barIndex * m_barHeight );
		pCurrentBar->subscribeEvent(
			TLVertScrollbar::EventMouseWheel, 
			Event::Subscriber( &TLListView::panel_MouseWheel, this ) );

		pCurrentBar->subscribeEvent(
			StaticImage::EventMouseClick, 
			Event::Subscriber( &TLListView::bar_MouseClick, this ) );

		pCurrentBar->subscribeEvent(
			StaticImage::EventMouseDoubleClick, 
			Event::Subscriber( &TLListView::bar_MouseDoubleClick, this ) );
		
		if ( NULL != m_pList )
		{
			m_pList->addChildWindow( pCurrentBar );
		}
	}	
}

void TLListView::destroyBinder()
{
	if ( NULL != m_pBinder )
	{
		delete m_pBinder;
		m_pBinder = NULL;
	}
}

void TLListView::SetTitleSpaceHeight( int height )
{
	m_titleSpaceHeight = height;
	if ( ( NULL != m_pContainer ) && ( NULL != m_pList ) )
	{
		m_pContainer->setHeight( Absolute, getHeight( Absolute ) - m_titleSpaceHeight );
		m_pContainer->setYPosition( Absolute, m_titleSpaceHeight );
		m_pageHeight = m_pContainer->getHeight( Absolute );
		m_pList->setHeight( Absolute, m_pContainer->getHeight( Absolute ) );
	}
}

bool TLListView::bar_MouseClick( const EventArgs& e )
{
	const MouseEventArgs& args = static_cast< const MouseEventArgs& >( e );
	if ( LeftButton == args.button )
	{
		setWindowBarSelected( *( args.window ) );
	}
	
	EventArgs tmpArgs = e;
	fireEvent( EventListViewItemClick, tmpArgs );
	return true;
}

bool TLListView::bar_MouseDoubleClick( const EventArgs& e )
{
	const MouseEventArgs& args = static_cast< const MouseEventArgs& >( e );
	if ( LeftButton == args.button )
	{
		setWindowBarSelected( *( args.window ) );
	}

	EventArgs tmpArgs = e;
	fireEvent( EventListViewItemDoubleClick, tmpArgs );
	return true;
}

void TLListView::setItemHover( ListViewItem& item, bool flag )
{
	Window* pHover= NULL;
	Window* pBar = item.GetBar();
	if ( NULL != pBar )
	{
		String barName = pBar->getName();

		if ( !m_hoverName.empty() )
		{
			if ( pBar->isChild( barName + "/" + m_hoverName ) )
			{
				pHover = pBar->getChild( barName + "/" + m_hoverName );
			}
		}
		else if ( pBar->isChild( barName + "/Hover" ) )
		{
			pHover = pBar->getChild( barName + "/Hover" );
		}
		else if ( pBar->isChild( barName + "/hover" ) )
		{
			pHover = pBar->getChild( barName + "/hover" );
		}

		if ( NULL != pHover )
		{
			pHover->setVisible( flag );
			pHover->setEnabled( false );
		}
	}
}

void TLListView::SetHoverName( String& name )
{
	m_hoverName = name;
}

void TLListView::setWindowBarSelected( Window& wnd )
{
	Bar_ListViewItem_Map::const_iterator it = m_bar_ListViewItem_Map.find( &wnd );
	if ( it != m_bar_ListViewItem_Map.end() )
	{
		const int clickIndex =  it->second;
		if ( ( clickIndex >= 0 ) && ( clickIndex < m_barList.size() ) )
		{
			ListViewItem& clickedItem = m_barList[ clickIndex ];
			if ( m_pCurrentSelectedItemIndex >= 0 )
			{
				ListViewItem& currentSelectedItem = m_barList[ m_pCurrentSelectedItemIndex ];
				setItemHover( currentSelectedItem, false );
			}
			
			setItemHover( clickedItem, true );
			m_pCurrentSelectedItemIndex = clickIndex;
			
			ListViewEventArgs lvArgs( &clickedItem );
			fireEvent( EventListViewItemSelected, lvArgs );
		}
	}	
}

ListViewItem& TLListView::GetSelectedItem()
{
	if ( m_pCurrentSelectedItemIndex < 0 )
	{
		throw NoneSelectedItemException();
	}

	if ( m_pCurrentSelectedItemIndex >= m_barList.size() )
	{
		throw ItemIndexOutOfBoundException();
	}

	return m_barList[ m_pCurrentSelectedItemIndex ];
}

void TLListView::setLayoutClipper( const Window& bar )
{
	Window* parent	= NULL;
	Window* root	= this;
	while ( NULL != root->getParent() )
	{
		parent = root;
		root = root->getParent();
	}

	int childCount = bar.getChildCount();
	for ( int i = 0; i < childCount; ++i )
	{
		Window* pChild = bar.getChildAtIdx( i );

		if ( ( NULL != pChild ) && ( pChild->getType() == TLStaticText::WidgetTypeName ) )
		{
			TLStaticText* pTextChild = static_cast< TLStaticText* >( pChild );
			if ( ( NULL != pTextChild->getLayout() ) && ( NULL != m_pList ) )
			{
				Rect textArea = m_pList->getUnclippedPixelRect();
				Point clipperPos;
				clipperPos.d_x = textArea.d_left - parent->getXPosition( Absolute );
				clipperPos.d_y = textArea.d_top - parent->getYPosition( Absolute );
				textArea.setPosition( clipperPos );
				LORect pannelclipper;
				cerectToLorect( &textArea, &pannelclipper );
				pTextChild->getLayout()->setClipper( pannelclipper );
			}
		}
	}
}

bool TLListView::GetIsBarUseLayout() const
{
	return m_bIsBarUseLayout;	
}

void TLListView::SetIsBarUseLayout( bool flag )
{
	m_bIsBarUseLayout = flag;
}

/********************************************************************
/*						class: TLQuestionWindowFactory
*********************************************************************/
Window* TLListViewFactory::createWindow( const String& name )
{
	return new TLListView( d_type, name );
}

//end of CEGUI namespace
}