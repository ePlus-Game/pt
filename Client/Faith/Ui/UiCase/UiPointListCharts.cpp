///////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/09/2008 
//      File_base        : UiPointListChatrs
//      File_ext         : cpp
//      Author           : marryme (Chen Lin)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiPointListCharts.h"
#include <sstream>
#include <iostream>
#include "CoreShell.h"
#include "EmplomentDataDef.h"
#include "../UiConfigManager.h"
#include "GameDataDef.h"
#include "DBWrap\cfs_filelogs.h"

/////////////////////////////////////////////////////////////////////
extern iCoreShell*	g_pCoreShell;
const string BarLayoutsFileName = "uisettings/layouts/ChartsBar.ls";  
const int ROWNUM = 10;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Implement for class KUiCharts
//////////////////////////////////////////////////////////////////////////

template<> 
KUiPointListCharts* KUiWndSingleton<KUiPointListCharts>::ms_Singleton	= NULL;

KUiPointListCharts::KUiPointListCharts( const CEGUI::String& id_name )
: KUiWndSingleton<KUiPointListCharts>( id_name ),
m_strengthCfg( KUiCfgLoader::getSingleton().getStrengItemContainer() ),
m_functionCfg( KUiCfgLoader::getSingleton().getFunctionItemContainer() )
{
	m_swaper			= FUNCTION;
	m_cancelButton		= NULL;
	m_okButton			= NULL;
	m_dataList			= NULL;
	m_dataTable			= NULL;
	m_buttonForStrength = NULL;
	m_buttonForFunction = NULL;
}

KUiPointListCharts::~KUiPointListCharts( void )
{
	if( m_dataList )
		delete m_dataList;
	if( m_dataTable )
		delete m_dataTable;
	m_cancelButton		= NULL;
	m_okButton			= NULL;
	m_buttonForStrength = NULL;
	m_buttonForFunction = NULL;
	m_dataList			= NULL;
	m_dataTable			= NULL;
	
}

void KUiPointListCharts::Init( void )
{
	
	if ( NULL == ms_Singleton->m_pThisWnd  )
	{
		assert( FALSE );
		return;
	}
	try
	{
		m_cancelButton	= static_cast< TLButton* >( ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Charts/Close" ) );
		m_okButton		= static_cast< TLButton* >( ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Charts/btnOK" ) );
		//初始化DataList控件
		TLTree*				tempTree	= static_cast< TLTree* >( m_pThisWnd->getChild( "TaharezLook/Charts/DataList" )->getChild( "TaharezLook/Charts/DataList/List" ) );
		TLVertScrollbar*	tempVert	= static_cast< TLVertScrollbar* >( m_pThisWnd->getChild( "TaharezLook/Charts/DataList" )->getChild( "TaharezLook/Charts/DataList/ScrollbarForList" ) );
		TLStaticImage*		tempImage	= static_cast< TLStaticImage* >( m_pThisWnd->getChild( "TaharezLook/Charts/DataList" ) );
		m_dataList = new KUiPointListDataList( tempTree, tempVert, tempImage );
		if( !m_dataList )
		{	
			assert( FALSE );
			return ;
		}
		m_dataList->Init();
		
		//初始化DataTable控件
		m_dataTable = new KUiPointListDataTable( static_cast< TLVertScrollbar* >( m_pThisWnd->getChild( "TaharezLook/Charts/DataTable" )->getChild( "TaharezLook/Charts/DataTable/ScrollbarForTable" ) ) );
		if( !m_dataTable )
		{
			assert( FALSE );
			return;
		}
		for ( int i = 0; i < 10; ++i )
		{
			String tempPath( "TaharezLook/Charts/DataTable/Title0" );
			tempPath+=iToString( i );
			Window* temp = static_cast< TLButton* >( m_pThisWnd->getChild( "TaharezLook/Charts/DataTable" )->getChild( tempPath ) );
			m_dataTable->InsertTitle(new KUiPointListTitle(temp));
			temp = NULL;
		}
		m_dataTable->Init();
		
		//初始化两个按钮
		m_buttonForStrength = static_cast< TLButton* >( m_pThisWnd->getChild( "TaharezLook/Charts/StrengthRanking" ) );
		m_buttonForFunction = static_cast< TLButton* >( m_pThisWnd->getChild( "TaharezLook/Charts/FunctionRanking" ) );
		//绑定按钮事件
		m_cancelButton->subscribeEvent( PushButton::EventClicked, Event::Subscriber( &KUiPointListCharts::handleButtonClick, ms_Singleton ) );
		m_okButton->subscribeEvent( PushButton::EventClicked, Event::Subscriber( &KUiPointListCharts::handleButtonClick, ms_Singleton ) );
		m_buttonForStrength->subscribeEvent( PushButton::EventClicked, Event::Subscriber( &KUiPointListCharts::RankingStateChange, ms_Singleton ) );
		m_buttonForFunction->subscribeEvent( PushButton::EventClicked, Event::Subscriber( &KUiPointListCharts::RankingStateChange, ms_Singleton ) );
		if( !m_functionCfg.empty() )
		{
			m_dataTable->SetTitle( m_functionCfg[0] );
			handleFunctionRanking( 0 );
		}
		
	}
	catch (...)
	{
		return ;
	}
	
}

bool KUiPointListCharts::RankingStateChange( const CEGUI::EventArgs& e )
{
	WindowEventArgs* wargs = ( WindowEventArgs* )( &e );
	if( m_functionCfg.empty() || m_strengthCfg.empty() || !m_dataList || !m_dataTable )
	{
		assert( FALSE );
		return FALSE;
	}
	if( ( Window* )m_buttonForStrength == wargs->window )
	{
		if ( STRENGTH == m_swaper )
		{
			return TRUE;
		} 
		else
		{
			m_swaper = STRENGTH;
			m_dataList->SwapList( m_swaper );
			m_dataTable->SetTitle( m_strengthCfg[ 0 ] );
		}
	}
	
	else if( ( Window* )m_buttonForFunction == wargs->window )
	{
		if (FUNCTION == m_swaper)
		{
			return TRUE;
		} 
		else
		{
			m_swaper = FUNCTION;
			m_dataList->SwapList( m_swaper );
			m_dataTable->SetTitle( m_functionCfg[ 0 ] );
		}
	}
	m_dataTable->CleanBar();
	RequestData( 0 );
	return TRUE;
}

void KUiPointListCharts::RequestData( unsigned int count )
{
	if( STRENGTH == m_swaper )
        handleStrengeRanking(count);
	else if(FUNCTION==m_swaper)
		handleFunctionRanking(count); 
}

void KUiPointListCharts::handleStrengeRanking( unsigned int count)
{
	if( count >= m_strengthCfg.size() || !g_pCoreShell || !m_dataTable )
	{
		assert( FALSE );
		return;
	}
	m_currentCfg = &( m_strengthCfg[count] );
	m_dataTable->CleanBar();
	//设置标题
	//SetTitle会自动更具当前的状态选择正确的配置进行更新
	m_dataTable->SetTitle( *m_currentCfg );
	
	//通过GDI取得数据
	int nret = 0;
	switch( m_currentCfg->param )
	{
	case 0:
		{
			//氏族排名
			nret = g_pCoreShell->GetGameData( GDI_GET_SHIZU_POPULARITY_TOP_N_INFO, ( unsigned int )m_shiZuDataBuf, SHIZU_BUFFER_COUNT );
			if( 0 == nret )
			{
				g_pCoreShell->OperationRequest( GOI_REFRESH_SHIZU_POPULARITY, m_currentCfg->param, NULL );
				return ;
			}
			m_dataTable->UpdateData( *( m_currentCfg ), nret, ( PLUS_POINT_TOPN_ELEM* )m_shiZuDataBuf );
		}
		break;
	case 1:
		{
			//诸侯排名
			nret = g_pCoreShell->GetGameData( GDI_GET_ZHUHOU_POPULARITY_TOP_N_INFO, ( unsigned int )m_ZhuHouDataBuf, ZHUHOU_BUFFER_COUNT );
			if( 0 == nret )
			{
				g_pCoreShell->OperationRequest( GOI_REFRESH_ZHUHOU_POPULARITY, m_currentCfg->param, NULL );
				return ;
			}
			if( m_currentCfg )
				m_dataTable->UpdateData( *( m_currentCfg ), nret, ( PLUS_POINT_TOPN_ELEM* )m_ZhuHouDataBuf );
		}
		break;
	case 2:
		{
			nret = g_pCoreShell->GetGameData( GDI_GET_COMBAT_KILL_TOP_N_INFO, ( unsigned int )m_combatRankInfo, ZHUHOU_BUFFER_COUNT );
			if( 0 == nret )
			{
				g_pCoreShell->OperationRequest( GOI_REFRESH_COMBAT_KILL_RANK, m_currentCfg->param, NULL );
				return ;
			}
			if( m_currentCfg )
			m_dataTable->UpdateData( *( m_currentCfg ), nret, ( PLUS_POINT_TOPN_ELEM* )m_combatRankInfo );
		}
		break;
	case 3:
		{

		}
		break;
	}
}

void KUiPointListCharts::handleFunctionRanking( unsigned int count )
{
	if( count >= m_functionCfg.size() || !m_dataTable || !g_pCoreShell )
	{
		assert( FALSE );
		return;
	}
	m_dataTable->CleanBar();
	m_currentCfg = ( &m_functionCfg[count] );
	if( !m_currentCfg )
	{
		assert( FALSE );
		return ;
	}
	
	//设置标题
	//SetTitle会自动更具当前的状态选择正确的配置进行更新
	m_dataTable->SetTitle( *m_currentCfg );
	
	//通过GDI取得数据
	int dataCount = GetDataFromCore( *m_currentCfg, m_pointList );
	if( dataCount == 0 )
	{	
		g_pCoreShell->OperationRequest( GOI_POINTLIST_REQ, m_currentCfg->param, NULL );
		return ;
	}
	m_dataTable->UpdateData( *m_currentCfg, dataCount, m_pointList );
}

void  KUiPointListCharts::Refresh( unsigned int type )
{
	if( STRENGTH == m_swaper )
	{
		if( !m_currentCfg )
		{
			assert( FALSE );
			return;
		}
		handleStrengeRanking( m_currentCfg->param );
	}
	else if( FUNCTION == m_swaper )
	{
		
		const vector< RankingCfg >& cfg = m_functionCfg;
		for(vector< RankingCfg >::const_iterator it = cfg.begin() ; it != cfg.end() ; ++it )
			if( type == it->param )
				break;
			if( it == cfg.end() )
			{
				assert( FALSE );
				return;
			}
			int dataCount = GetDataFromCore( ( *it ) , m_pointList );
			if( m_dataTable )
				m_dataTable->UpdateData( ( *it ), dataCount,  m_pointList );
			else
			{
				assert( FALSE );
				return ;
			}
	}
}

int KUiPointListCharts::GetDataFromCore( const RankingCfg& config, PLUS_POINT_TOPN_ELEM* data )
{
	if( !g_pCoreShell )
	{
		assert( FALSE );
		return 0;
	}
	return g_pCoreShell->GetGameData( GDI_GET_POINTLIST_INFO, config.param, reinterpret_cast< int >( data ) );
}

//////////////////////////////////////////////////////////////////////////
//Implement for class KUiDataList
//////////////////////////////////////////////////////////////////////////

void KUiPointListDataList::Init( void )
{
	if ( !m_dataList && !m_pScrollbar )
	{
		assert( FALSE );
		return;
	}
	try
	{
		m_dataList->setVertScrollBar( m_pScrollbar );
		m_pScrollbar->subscribeEvent( TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber( &KUiPointListDataList::handleScroll, this ) );
		m_dataList->subscribeEvent( Tree::EventMouseWheel, Event::Subscriber( &KUiPointListDataList::OnListWhell, this ) );
		m_dataList->subscribeEvent( Tree::TR_EventSelectionChanged, Event::Subscriber( &KUiPointListDataList::handleSlectedItem, this ) );
		{
			const vector< RankingCfg >& tempFunctionCfg = KUiCfgLoader::getSingleton().getFunctionItemContainer();
			int i = 0;
			for ( vector< RankingCfg >::const_iterator it = tempFunctionCfg.begin(); it != tempFunctionCfg.end(); ++it, ++i )
			{
				if( i >= LIST_BOX_ITEM_COUNT)
					break;
				( m_itemArrayForFunction + i)->setText( AnsiToUtf8( it->itemName.c_str() ) );
				( m_itemArrayForFunction + i)->setID( i );
			}
 			if ( i )
 				m_itemArrayForFunction[0].setSelected( true );
		}
		{
			int i = 0;
			const vector< RankingCfg >& tempstrength = KUiCfgLoader::getSingleton().getStrengItemContainer();
			for ( vector< RankingCfg >::const_iterator it = tempstrength.begin(); it != tempstrength.end(); ++it, ++i )
			{
				if( i >= LIST_BOX_ITEM_COUNT )
					break;
				( m_itemArrayForStrength + i )->setText( AnsiToUtf8( it->itemName.c_str() ) );
				( m_itemArrayForStrength + i)->setID( i );
			}
			if ( i )
 				m_itemArrayForStrength[0].setSelected( true );
		}
		
		for( int i = 0; i < KUiCfgLoader::getSingleton().getFunctionItemContainer().size(); ++i)
			m_dataList->addItem( ( m_itemArrayForFunction + i ) );
		
		if( m_dataList->getTreeTotalItemsHeigh() <= m_dataListPannal->getHeight( Absolute ) )
			m_pScrollbar->hide();
		else
			m_pScrollbar->show();
		
	}
	catch( ... )
	{
		assert( FALSE );
		return;
	}
}

bool KUiPointListDataList::handleSlectedItem(const CEGUI::EventArgs& e)
{
	
	TreeEventArgs* treeEvent = (TreeEventArgs*)&e;
	TreeItem* treeItem = treeEvent->treeItem;
	
	if(treeItem == NULL)
	{
		return false;
	}
	
	unsigned int count = treeItem->getID();
	
	KUiPointListCharts::GetSingleton().RequestData( count );
	return TRUE;
}

void KUiPointListDataList::SwapList( LISTTYPE listType )
{
	if( !m_dataList )
	{
		assert( FALSE );
		return ;
	}
	if ( m_dataList->getItemCount() )
		m_dataList->resetList();
	
	if ( STRENGTH == listType )
	{
		const vector< RankingCfg >& tempStrengeCfg = KUiCfgLoader::getSingleton().getStrengItemContainer();
		{
			for ( int i = 0; i < tempStrengeCfg.size(); ++i )
			{
				if ( i < LIST_BOX_ITEM_COUNT )
				{ 
					m_itemArrayForStrength[i].setSelected( false );
					m_dataList->addItem( ( m_itemArrayForStrength + i ) );
				}
			}
			if( i )
				m_itemArrayForStrength[0].setSelected( true );

		}
	} 
	else
	{
		const vector< RankingCfg >& tempFunctionCfg = KUiCfgLoader::getSingleton().getFunctionItemContainer();
		{
			for ( int i = 0; i < tempFunctionCfg.size() ; ++i )
			{
				if ( i < LIST_BOX_ITEM_COUNT )
				{
					m_itemArrayForFunction[i].setSelected( false );
					m_dataList->addItem( ( m_itemArrayForFunction + i ) );
				}
			}
			if( i )
				m_itemArrayForFunction[0].setSelected( true );
		}
	}
	if( m_dataList->getTreeTotalItemsHeigh() <= m_dataListPannal->getHeight( Absolute ) )
		m_pScrollbar->hide();
	else
		m_pScrollbar->show();
	m_dataList->setYPosition( Absolute, 7.0f );
	m_pScrollbar->setScrollPosition( 0.0f );
}

bool KUiPointListDataList::OnListWhell(const EventArgs & args)
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(m_pScrollbar ->isVisible())
	{
		m_pScrollbar ->setScrollPosition(m_pScrollbar ->getScrollPosition()
			- m_pScrollbar ->getStepSize() * eventArgs->wheelChange);		
	}
	return true;
}

bool KUiPointListDataList::handleScroll( const CEGUI::EventArgs& e )
{
	float scrollPos		= m_pScrollbar->getScrollPosition();
	int clipperHeight	= m_dataListPannal->getHeight( Absolute );
	int actHeight		= m_dataList->getTreeTotalItemsHeigh();
	int exceedSize		= ( actHeight - clipperHeight ) * scrollPos;
	m_dataList->setYPosition( Absolute, -exceedSize + 7);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//Implement for class KUiDataTable
//////////////////////////////////////////////////////////////////////////


KUiPointListDataTable::KUiPointListDataTable( TLVertScrollbar* pScrollbar): m_pScrollbar(pScrollbar), m_pointList(NULL)
{
	m_dataCount			= 0;
	m_currentDisplay	= 0;
	m_exceedHeight		= 0;
	m_pageHeight		= 0;
	m_barHeight			= -1;
	m_currentCfg		= NULL;
	m_pPage				= NULL;
}

KUiPointListDataTable::~KUiPointListDataTable( void )
{
	{
		for ( vector< KUiPointListTitle* >::iterator it = m_titleContainer.begin(); it != m_titleContainer.end(); ++it )
		{
			
			if( *it )
				delete *it;
			else
				assert( FALSE );
		}
	}
	
	{
		for( vector< KUiPointListBar* >::iterator it = m_barContainer.begin(); it != m_barContainer.end(); ++it )
		{
			if( *it )
				delete *it;
			else
				assert( FALSE );
		}
	}
	m_pScrollbar = NULL;
	m_currentCfg = NULL;
}

void KUiPointListDataTable::Init( void )
{
	try
	{
		//绑定滚动条事件
		m_pScrollbar->subscribeEvent( 
			TLVertScrollbar::EventScrollPositionChanged, 
			Event::Subscriber( &KUiPointListDataTable::handleScroll, this ) );
		m_pPage = KUiPointListCharts::GetSingleton().m_pThisWnd->getChild( "TaharezLook/Charts/DataTable" )->getChild( "TaharezLook/Charts/DataTable/BarList" );
		m_pageHeight = m_pPage->getHeight( Absolute );
		m_pScrollbar->setVisible( false );
	}
	catch( ... )
	{
		assert( FALSE );
		return;
	}
	
	//初始化所有title为unvisible
	for ( vector< KUiPointListTitle* >::iterator it = m_titleContainer.begin(); it != m_titleContainer.end(); ++it )
	{
		if( *it )
			( *it )->SetVisible( FALSE );
		else
			assert( FALSE );
	}
}

bool KUiPointListDataTable::handleScroll( const CEGUI::EventArgs& e )
{
	if ( ( m_exceedHeight > 0 ) && ( m_pScrollbar ) )
	{
		float scrollPos = m_pScrollbar->getScrollPosition();
		int scrollSize = - scrollPos * m_exceedHeight;
		for ( int i = 0; i < m_barContainer.size(); i++ )
		{
			if ( NULL != m_barContainer[i] )
			{
				Point newPos;
				newPos.d_x = m_barContainer[i]->GetBarPoint()->getAbsoluteXPosition();
				newPos.d_y = m_barContainer[i]->GetBarPoint()->getAbsoluteHeight() * i + scrollSize;
				m_barContainer[i]->GetBarPoint()->setPosition( Absolute, newPos );
			}
		}
	}
	else
		assert( FALSE );
	return TRUE;
	
}

void KUiPointListDataTable::UpdateData( const RankingCfg& cfg, int dataCount,  const PLUS_POINT_TOPN_ELEM* data )
{
	if( !m_pScrollbar || ( NULL == data && 0 != dataCount) )
	{
		assert( FALSE );
		return;
	}
	const UiShizuPopularityInfo* dataPointShizu = ( UiShizuPopularityInfo* )( data );
	const UiZhuhouPopularityInfo* dataPointZhuhou =( UiZhuhouPopularityInfo*)( data );
	const UiCombatKillRankInfo*   dataPointCombat = ( UiCombatKillRankInfo* )( data );
	m_pScrollbar -> setScrollPosition( ( float ) 0 );
	m_pScrollbar->setVisible( TRUE );
	m_pointList = ( data );
	m_dataCount = dataCount;
	m_currentCfg = ( & cfg );
	Window* pCurrentBar = NULL;
	ostringstream curBarName;	
	CleanBar();
	for ( int i = 0; i < m_dataCount; ++i )
	{
		if ( i >= m_barContainer.size() )
		{
			//积分信息的条数大于Bar控件数量，需要新建bar
			curBarName.str( "" );		
			curBarName << KUiPointListCharts::GetSingleton().m_pThisWnd->getName() << '_' << i << '_';
			string temp = curBarName.str();
			pCurrentBar = KUiPointListCharts::GetSingleton().m_pWindowManager->loadWindowLayout( BarLayoutsFileName, curBarName.str() );
			if ( ( !pCurrentBar ) || ( !m_pPage ) )
			{
				assert( FALSE );
				return;
			}
			if ( m_barHeight < 0 )
				m_barHeight = pCurrentBar->getHeight( Absolute );	
			pCurrentBar->setXPosition( Absolute, 0 );
			pCurrentBar->setYPosition( Absolute, i * m_barHeight );
			m_barContainer.push_back( new KUiPointListBar( pCurrentBar ) );
			pCurrentBar->subscribeEvent( 
				TLVertScrollbar::EventMouseWheel,
				Event::Subscriber( &KUiPointListDataTable::Panel_MouseWheel, this ) );
			m_pPage->addChildWindow( pCurrentBar );
		}
		
		//异常情况，创建bar控件失败，退出	
		if ( i >= m_barContainer.size() )
		{
			assert( FALSE );
			return;
		}
		
		if ( STRENGTH == KUiPointListCharts::GetSingleton().GetRankingStatu())
		{
			switch( cfg.param )
			{
			case 0:
				{	
					m_barContainer[i]->UpdateBar( cfg, ( PLUS_POINT_TOPN_ELEM* )dataPointShizu );
					++dataPointShizu;
				}
				break;
			case 1:
				{
					m_barContainer[i]->UpdateBar( cfg, ( PLUS_POINT_TOPN_ELEM* )dataPointZhuhou );
					++dataPointZhuhou;
				}
				break;
			case 2:
				{
					m_barContainer[i]->UpdateBar( cfg, ( PLUS_POINT_TOPN_ELEM* )dataPointCombat );
					++dataPointCombat;
				}
				break;
			}
		} 
		else
		{
			m_barContainer[i]->UpdateBar( cfg, data );
			++data;
		}
	}
	m_exceedHeight = m_barHeight * dataCount - m_pageHeight;
	if ( m_exceedHeight > 0 )
	{
		float stepSize = static_cast< float >( m_barHeight  ) / static_cast< float >( m_exceedHeight );
		m_pScrollbar->setStepSize( ( ( stepSize > 0.0 ) && ( stepSize < 1.0 ) ) ? stepSize : 1.0 );
		m_pScrollbar->setVisible( TRUE );
	}
	else
		m_pScrollbar->setVisible( FALSE );
	
	if( dataCount <= ROWNUM )	
		m_pScrollbar->setVisible( FALSE );
	dataPointShizu = NULL;
	dataPointZhuhou = NULL;
	data = NULL;
}

void KUiPointListDataTable::SetTitle( const RankingCfg& config )
{
	
	vector< KUiPointListTitle* >::iterator visibleTitle = m_titleContainer.begin() + config.titleNum;
	
	//获得title的名字
	vector< string >::const_iterator tempString = config.titleName.begin();
	for ( vector< KUiPointListTitle* >::iterator it = m_titleContainer.begin(); it != m_titleContainer.end(); ++it )
	{
		
		if( !( *it ) )
		{ 
			assert( FALSE );
			return;
		}
		
		if ( it < visibleTitle )
		{
			( *it )->SetVisible();
			if( config.titleName.end() != tempString )
			{
				const char * szTitleName = tempString->c_str();
				( *it )->SetContent(  AnsiToUtf8 ( szTitleName ) );
				++tempString;
			}
		} 
		else
			( *it )->SetVisible( FALSE );
	}
}

bool KUiPointListDataTable::Panel_MouseWheel( const CEGUI::EventArgs& e )
{
	MouseEventArgs* eventArgs = ( MouseEventArgs* )( &e );
	if( !m_pScrollbar )
		assert( FALSE );
	if( m_pScrollbar->isVisible() )
	{
		float newPos = m_pScrollbar->getScrollPosition() - m_pScrollbar->getStepSize() * eventArgs->wheelChange;
		m_pScrollbar->setScrollPosition( newPos );
	}
	return TRUE;	
}

void KUiPointListDataTable::CleanBar()
{
	for ( int j = 0; j < m_barContainer.size(); ++j )
		m_barContainer[j]->Clean();
}

//////////////////////////////////////////////////////////////////////////
//Implement for class KUiBar
//////////////////////////////////////////////////////////////////////////
KUiPointListBar:: ~KUiPointListBar(void)
{
}

void KUiPointListBar::Clean( void )
{
	if ( m_bar )
	{
		String barName = m_bar -> getName();
		barName += "/Title";
		try
		{
			for( int i = 1; i < 11; ++i )
			{
				m_bar->getChild( barName +iToString(i) )->setText( "" );
				m_bar->getChild( barName +iToString(i) )->setVisible( FALSE );
			}
		}
		catch ( ... )
		{
			return;
		}
	}
	else
		assert( FALSE );
}

void KUiPointListBar::UpdateBar( const RankingCfg& cfg, const PLUS_POINT_TOPN_ELEM* data )
{
	
	//对已不同的数据结构，需要在这里更改代码以显示
	if ( ( !m_bar ) || ( !data ) )
	{
		assert( FALSE );
		return;
	}
	try
	{
		switch(KUiPointListCharts::GetSingleton().GetRankingStatu())
		{
		case STRENGTH:
			{
				switch( cfg.param )
				{
				case 0:
					{
						const UiShizuPopularityInfo* dataPoint = ( UiShizuPopularityInfo* )( data );
						
						String currentBarName = m_bar->getName();
						
						Window* title1 = m_bar->getChild( currentBarName + "/Title1" );
						title1->setText( ( iToString( dataPoint->Rank ) ) );
						title1->setVisible( TRUE );
						
						Window* title2 = m_bar->getChild( currentBarName + "/Title2" );
						if(0 == strcmp( dataPoint->Name, "" ) )
							title2->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title2->setText( ( AnsiToUtf8 ( dataPoint->Name ) ) );
						title2->setVisible( TRUE );
						
						Window* title3 = m_bar->getChild( currentBarName + "/Title3" );
						if ( -1 == dataPoint->Level )
							title3->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title3->setText( ( iToString( dataPoint->Level ) ) );
						title3->setVisible( TRUE );
						
						Window* title4 = m_bar->getChild( currentBarName + "/Title4" );
						title4->setText( ( iToString( dataPoint->PlayerCount ) ) );
						title4->setVisible( TRUE );
						
						Window* title5 = m_bar->getChild( currentBarName + "/Title5" );
						title5->setText( ( iToString( dataPoint->Popularity ) ) );
						title5->setVisible( TRUE );
					}
					
					break;
				case 1:
					{
						const UiZhuhouPopularityInfo* dataPoint = ( UiZhuhouPopularityInfo* )( data );
						String currentBarName = m_bar->getName();
						
						Window* title1 = m_bar->getChild( currentBarName + "/Title1" );
						title1->setText( ( iToString( dataPoint->Rank ) ) );
						title1->setVisible( TRUE );
						
						Window* title2 = m_bar->getChild( currentBarName + "/Title2" );
						if(0 == strcmp( dataPoint->Name, "" ) )
							title2->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title2->setText( ( AnsiToUtf8 ( dataPoint->Name ) ) );
						title2->setVisible( TRUE );
						
						Window* title3 = m_bar->getChild( currentBarName + "/Title3" );
						if ( -1 == dataPoint->Level )
							title3->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title3->setText( ( iToString( dataPoint->Level ) ) );
						title3->setVisible( TRUE );
						
						Window* title4 = m_bar->getChild( currentBarName + "/Title4" );
						title4->setText( ( iToString( dataPoint->PlayerCount ) ) );
						title4->setVisible( TRUE );
						
						Window* title5 = m_bar->getChild( currentBarName + "/Title5" );
						title5->setText( ( iToString( dataPoint->Popularity ) ) );
						title5->setVisible( TRUE );
					}
					break;
				case 2:
					{
						const UiCombatKillRankInfo* dataPoint = ( UiCombatKillRankInfo* )( data );
						String currentBarName = m_bar->getName();
						
						Window* title1 = m_bar->getChild( currentBarName + "/Title1" );
						title1->setText( ( iToString( dataPoint->Rank ) ) );
						title1->setVisible( TRUE );
						
						Window* title2 = m_bar->getChild( currentBarName + "/Title2" );
						if(0 == strcmp( dataPoint->Name, "" ) )
							title2->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title2->setText( ( AnsiToUtf8 ( dataPoint->Name ) ) );
						title2->setVisible( TRUE );
						
						Window* title3 = m_bar->getChild( currentBarName + "/Title3" );
						if ( -1 == dataPoint->Level )
							title3->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title3->setText( ( iToString( dataPoint->Level ) ) );
						title3->setVisible( TRUE );
						
						Window* title4 = m_bar->getChild( currentBarName + "/Title4" );
						if(0 == strcmp( dataPoint->Shizu, "" ) )
							title4->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title4->setText( ( AnsiToUtf8 ( dataPoint->Shizu ) ) );
						title4->setVisible( TRUE );
						
						Window* title5 = m_bar->getChild( currentBarName + "/Title5" );
						if(0 == strcmp( dataPoint->Zhuhou, "" ) )
							title5->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title5->setText( ( AnsiToUtf8 ( dataPoint->Zhuhou ) ) );
						title5->setVisible( TRUE );

						Window* title6 = m_bar->getChild( currentBarName + "/Title6" );
						if ( -1 == dataPoint->CombatKill )
							title6->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title6->setText( ( iToString( dataPoint->CombatKill ) ) );
						title6->setVisible( TRUE );
					}

					break;
				case 3:
					break;
				}
			}	
			break;
		case FUNCTION:
			{
				switch( cfg.param )
				{
				case 0:
					{
						String currentBarName = m_bar->getName();
						
						Window* title1 = m_bar->getChild( currentBarName + "/Title1" );
						title1->setText( ( iToString( data->m_nNumber ) ) );
						title1->setVisible( TRUE );
						
						Window* title2 = m_bar->getChild( currentBarName + "/Title2" );
						if(0 == strcmp( data->m_szName, "" ) )
							title2->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title2->setText( ( AnsiToUtf8 ( data->m_szName ) ) );
						title2->setVisible( TRUE );
						
						Window* title3=m_bar->getChild( currentBarName + "/Title3" );
						title3->setText( ( iToString( data->m_Level ) ) );
						title3->setVisible( TRUE );
						
						Window* title4=m_bar->getChild( currentBarName + "/Title4" );
						if( 0 == strcmp ( data->m_GensName, "" ) )
							title4->setText( ( AnsiToUtf8( "---" ) ) );
						else
							title4->setText( ( AnsiToUtf8( data->m_GensName ) ) );
						title4->setVisible( TRUE );			
						
						Window* title5 = m_bar->getChild( currentBarName + "/Title5" );
						if( 0 == strcmp( data->m_TongName, "" ) )
							title5->setText( (AnsiToUtf8( "---" ) ) );
						else
							title5->setText( ( AnsiToUtf8( data->m_TongName ) ) );
						title5->setVisible( TRUE );
						
						Window* title6 = m_bar->getChild( currentBarName + "/Title6" );
						if( title6 )
							title6->setText( ( ( iToString( data->m_nScore ) ) ) );
						title6->setVisible( TRUE );
					}
					
					break;
				case 1:
					{
						String currentBarName = m_bar->getName();
						
						Window* title1 = m_bar->getChild( currentBarName + "/Title1" );
						title1->setText( ( iToString( data->m_nNumber ) ) );
						title1->setVisible( TRUE );
						
						Window* title2 = m_bar->getChild( currentBarName + "/Title2" );
						if(0 == strcmp( data->m_szName, "" ) )
							title2->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title2->setText( ( AnsiToUtf8 ( data->m_szName ) ) );
						title2->setVisible( TRUE );
						
						Window* title3=m_bar->getChild( currentBarName + "/Title3" );
						title3->setText( ( iToString( data->m_Level ) ) );
						title3->setVisible( TRUE );
						
						Window* title4=m_bar->getChild( currentBarName + "/Title4" );
						if( 0 == strcmp ( data->m_GensName, "" ) )
							title4->setText( ( AnsiToUtf8( "---" ) ) );
						else
							title4->setText( ( AnsiToUtf8( data->m_GensName ) ) );
						title4->setVisible( TRUE );			
						
						Window* title5 = m_bar->getChild( currentBarName + "/Title5" );
						if( 0 == strcmp( data->m_TongName, "" ) )
							title5->setText( (AnsiToUtf8( "---" ) ) );
						else
							title5->setText( ( AnsiToUtf8( data->m_TongName ) ) );
						title5->setVisible( TRUE );
						
						Window* title6 = m_bar->getChild( currentBarName + "/Title6" );
						if( title6 )
							title6->setText( ( ( iToString( data->m_nScore ) ) ) );
						title6->setVisible( TRUE );
					}
					break;
				case 2:
					{
						String currentBarName = m_bar->getName();
						
						Window* title1 = m_bar->getChild( currentBarName + "/Title1" );
						title1->setText( ( iToString( data->m_nNumber ) ) );
						title1->setVisible( TRUE );
						
						Window* title2 = m_bar->getChild( currentBarName + "/Title2" );
						if(0 == strcmp( data->m_szName, "" ) )
							title2->setText( ( AnsiToUtf8 ( "---" ) ) );
						else
							title2->setText( ( AnsiToUtf8 ( data->m_szName ) ) );
						title2->setVisible( TRUE );
						
						Window* title3=m_bar->getChild( currentBarName + "/Title3" );
						title3->setText( ( iToString( data->m_Level ) ) );
						title3->setVisible( TRUE );
						
						Window* title4=m_bar->getChild( currentBarName + "/Title4" );
						if( 0 == strcmp ( data->m_GensName, "" ) )
							title4->setText( ( AnsiToUtf8( "---" ) ) );
						else
							title4->setText( ( AnsiToUtf8( data->m_GensName ) ) );
						title4->setVisible( TRUE );			
						
						Window* title5 = m_bar->getChild( currentBarName + "/Title5" );
						if( 0 == strcmp( data->m_TongName, "" ) )
							title5->setText( (AnsiToUtf8( "---" ) ) );
						else
							title5->setText( ( AnsiToUtf8( data->m_TongName ) ) );
						title5->setVisible( TRUE );
						
						Window* title6 = m_bar->getChild( currentBarName + "/Title6" );
						if( title6 )
							title6->setText( ( ( iToString( data->m_nScore ) ) ) );
						title6->setVisible( TRUE );
					}
					break;
				case 3:
					{

					}
					break;
				}
			}
			break;
		}
	}
	catch ( ... )
	{
		return;
	}
}
