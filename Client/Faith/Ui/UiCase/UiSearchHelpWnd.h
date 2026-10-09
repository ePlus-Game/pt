//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiSearchHelpWnd
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 查询帮助Tip
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _KUISEARCHHELPWND_H_
#define _KUISEARCHHELPWND_H_

#include "../uicommon.h"
#include "IQueryInfo.h"
#include "TLEditbox.h"
#include "TLVertScrollbar.h"
#include "UiQueryWnd.h"
#include <vector>

using namespace CEGUI;

#define	MAX_CACHE_NUM 10			// 记录上次查询的最大个数
#define	TIP_INTERVAL 20				// 自动隐藏TIP提示时间
//#define	RANDOMHELP_INTERVAL 5		// 随机提示间隔时间
#define INVALID_CONTENT_ID -1

class KUiSearchHelpWnd : public KUiWndSingleton<KUiSearchHelpWnd>
{
public:
    KUiSearchHelpWnd( const CEGUI::String& id_name	);
    ~KUiSearchHelpWnd(								);
	
	static	void	Show();
	static	void	Hide();
	void			Init();
	void			Breathe();
	
	bool			IsSearchVisible();
	void			ShowSimpleHelp(char* layoutText);
	void			ShowSearchHelp();

	void			QueryRequest( SearchContent content, bool cache = false );
	void			ShowWhatICanDo();

private:
	void			getChild(void);

	bool			handleCloseSimpleHelp( const EventArgs& e );
	bool			handleCloseSearchHelp( const EventArgs& e );
	bool			handleLastSearch( const EventArgs& e );
	bool			handleNextSearch( const EventArgs& e );
	bool			handleScrollbar( const EventArgs& e );
	bool			handleClickText( const EventArgs& e );
	bool			handleMouseWheel( const EventArgs& e );
	bool			handleHoverText(const EventArgs& args);

	void			UpdateButton();
	void			SaveQuery( SearchContent content );
	void			ShowSearchResult( int count, char* result, QueryResultType queryResultType );

private:
	typedef	std::vector<SearchContent>	SearchVector;
	SearchVector		d_LastSearchContent;
	SearchVector		d_NextSearchContent;

	Window*				d_pSimpleHelpTip;
	Window*				d_pSearchHelp;
	TLStaticText*		d_pSimpleHelpText;
	TLStaticText*		d_pSearchResultText;

	TLButton*			d_pNextSearch;
	TLButton*			d_pLastSearch;

	TLVertScrollbar*	d_pScroll;
	
	unsigned long		d_OperationHelpTime;
	unsigned long		d_RandomHelpTime;

	int					d_MaxHelpCount;
	bool				d_bInnerRequest;
};


#endif