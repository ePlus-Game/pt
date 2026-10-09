//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/09/2008 
//      File_base        : UiPointListChatrs
//      File_ext         : h
//      Author           : marryme (Chen Lin)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIPOINTLISTCHARTS_H
#define UIPOINTLISTCHARTS_H

// #if _MSC_VER > 1000
// #pragma once
// #endif // _MSC_VER > 1000
#include "../uicommon.h"
#include "CEGUI.h"
#include <string>
#include <vector>

#include "TLVertScrollbar.h"
#include "GameDataDef.h"
#include "TLTree.h"
#include "TLTreeItem.h"
#include "TLRadioButton.h"
#include <algorithm>
using std::vector;
using std::string;

using namespace CEGUI;
const int LIST_BOX_ITEM_COUNT = 100;
const int POINT_LIST_COUNT	  = 100;
const int SHIZU_BUFFER_COUNT  = 20;
const int ZHUHOU_BUFFER_COUNT = 20;
enum LISTTYPE
{
	STRENGTH=1,
	FUNCTION
};
struct RankingCfg;
struct ParamForStrength 
{
	unsigned int	dataPoint;
	int				dataCount;
};
struct PLUS_POINT_TOPN_ELEM;
class KUiPointListTitle
{
public:
	KUiPointListTitle( Window* title, int x = 0, int y = 0 ): m_title( title ), m_xPosition( x ), m_yPosition( y )
	{
		assert( title );
	}
	virtual ~KUiPointListTitle( void )
	{
		m_title = NULL;
	}
	inline void SetContent( const String& content )
	{
		if( m_title )
			m_title->setText( content );
		else
			assert( FALSE );
	}
	inline void SetPosition( int x, int y )
	{
		m_xPosition = x;
		m_yPosition = y;
		if( m_title )
			m_title->setPosition( Absolute, Point( x, y ) );
		else
			assert( FALSE );
	}
	inline BOOL IsVisible( void )
	{
		if( m_title )
			return m_title->isVisible();
		else
			assert( FALSE );
		
	}
	inline void SetVisible( bool visible = true )
	{
		if( m_title )
			m_title->setVisible( visible );
		else
			assert( FALSE );
	}
private:
	Window*			m_title;
	int				m_xPosition;
	int				m_yPosition;
	
};

class KUiPointListBar  
{
public:
	KUiPointListBar( Window* bar ): m_bar( bar )
	{}
	virtual ~KUiPointListBar(void);
	void Init( void );
	void UpdateBar( const RankingCfg& cfg, const PLUS_POINT_TOPN_ELEM* data );
	bool IsVisible( void )
	{
		if( m_bar )
			return m_bar->isVisible();
		else
			assert( FALSE );
	}
	void SetVisible( bool visible = true )
	{
		if( m_bar )
			m_bar->setVisible( visible ); 
		else
			assert( FALSE );
	}
	void Clean( void );
	Window* GetBarPoint( void )
	{
		return m_bar;
	}
	
private:
	Window* m_bar;
	
};
class KUiPointListDataTable
{
public:
	KUiPointListDataTable( TLVertScrollbar* pScrollbar );
	virtual ~KUiPointListDataTable( void );
	void UpdateData( const RankingCfg& cfg, int dataCount = 0, const PLUS_POINT_TOPN_ELEM* data = NULL );
	inline void Init( void );
	inline void CleanBar();
	void InsertTitle( KUiPointListTitle* title )
	{
		if( title )
			m_titleContainer.push_back( title );
		else
			assert( FALSE );
	}
	void SetTitle( const RankingCfg& config );
private:
	//处理滚动条事件
	bool handleScroll( const CEGUI::EventArgs& e );
	bool Panel_MouseWheel( const CEGUI::EventArgs& e );
	
	
private:
	vector< KUiPointListTitle* >	m_titleContainer;
	vector< KUiPointListBar* >		m_barContainer;
	TLVertScrollbar*				m_pScrollbar;
	const PLUS_POINT_TOPN_ELEM*		m_pointList;
	int								m_dataCount;
	int								m_currentDisplay;
	const RankingCfg*				m_currentCfg;
	Window*							m_pPage;
	int								m_barHeight;
	int								m_exceedHeight;
	int								m_pageHeight;
};

class KUiPointListDataList 
{
public:
	KUiPointListDataList( TLTree* listBox, TLVertScrollbar* pScrollbar, TLStaticImage* dataListPannal ): m_dataList( listBox ), m_pScrollbar( pScrollbar ), m_dataListPannal( dataListPannal )
	{}
	virtual ~KUiPointListDataList(void)
	{}
	void Init( void );
	
    void SwapList(LISTTYPE listType);
	
	
private:
	//处理item被选定的事件
	bool handleSlectedItem( const CEGUI::EventArgs& e );
	class InnerTLTreeItem : public TLTreeItem
	{
	public:
		InnerTLTreeItem(): TLTreeItem( "" , 0, NULL, false, false)
		{}
	};
	bool handleScroll( const CEGUI::EventArgs& e );
	bool OnListWhell(const EventArgs & args);
private:
	TLTree*					m_dataList;
	InnerTLTreeItem			m_itemArrayForStrength[LIST_BOX_ITEM_COUNT];
	InnerTLTreeItem			m_itemArrayForFunction[LIST_BOX_ITEM_COUNT];
	TLVertScrollbar*		m_pScrollbar;
	TLStaticImage*			m_dataListPannal;

};

class KUiPointListCharts   : public KUiWndSingleton< KUiPointListCharts >
{
	friend KUiPointListDataTable;
public:
	KUiPointListCharts( const String& id_name );
	virtual ~KUiPointListCharts( void );
	
	void Init( void );
	void DataChangedNotiFy( void );
	void RequestData( unsigned int count );
	void SwapList( void );
	void Refresh( unsigned int type );
	LISTTYPE GetRankingStatu( void )
	{
		return m_swaper;
	}
private:
	inline bool handleButtonClick( const CEGUI::EventArgs& e )
	{
		Hide();
		return TRUE;
	}
	inline bool RankingStateChange( const CEGUI::EventArgs& e );
	void handleStrengeRanking( unsigned int count );
	void handleFunctionRanking( unsigned int count );
	int GetDataFromCore( const RankingCfg& config, PLUS_POINT_TOPN_ELEM* data );
private:
	KUiPointListDataList*				m_dataList;
	KUiPointListDataTable*				m_dataTable;
	TLButton*							m_cancelButton;
	TLButton*							m_okButton;
	PLUS_POINT_TOPN_ELEM				m_pointList[POINT_LIST_COUNT];
	UiShizuPopularityInfo				m_shiZuDataBuf[SHIZU_BUFFER_COUNT];
	UiZhuhouPopularityInfo				m_ZhuHouDataBuf[ZHUHOU_BUFFER_COUNT];
	UiCombatKillRankInfo				m_combatRankInfo[ZHUHOU_BUFFER_COUNT];
	LISTTYPE							m_swaper;
	TLButton*							m_buttonForStrength;
	TLButton*							m_buttonForFunction;
	const vector< RankingCfg >&			m_functionCfg;
	const vector< RankingCfg >&			m_strengthCfg;
	const RankingCfg*					m_currentCfg;
};


#endif // ifndef UIPOINTLISTCHARTS_H
