#ifndef UI_FSBIBLE_H
#define UI_FSBIBLE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "TLStatic.h"
#include "TLRadioButton.h"
#include "TLButton.h"
#include "TLTree.h"
#include "TLCheckbox.h"
#include "TLGameObject.h"
#include "UiCommonGrid.h"
#include "TLVertScrollbar.h"
#include "UiFSBible_SpecialQuestItem.h"
#include <map>
#include <string>
using std::multimap;
using std::string;
#define UI_FSBIBLE_PATH_1024 "uisettings/layouts1024/FSBible.ls"
#define UI_FSBIBLE_PATH "uisettings/layouts/FSBible.ls"

#define UI_FSBIBLE_UPDATE_INFO_MSG_TYPE		62
#define UI_FSBIBLE_UPDATE_INFO_MSG_CODE		0

#define UI_FSBIBLE_MAX_SPECIAL_QUEST_COUNT_PER_PAGE 10
#define UI_FSBIBLE_COMMON_CTRL_OFFSET 10
#define UI_FSBIBLE_MAX_QUEST_COUNT 40
typedef multimap< pair< int, int >, pair< int, string >, less< pair< int, int > > > UpdateInfoMap;

class KUiFSBible
{
	enum StringCode
	{
		QUEST_DELETE_NOTIFY = 0,
		QUEST_OK,
		QUEST_CANCEL
	};

	TLStaticImage*	_thisWindow;

	TLStaticImage*	_todayPanel;
	TLStaticImage*	_questPanel;
	
		
	TLTree*			_questList;
	TLStaticImage*	_specialQuestPanel;
	TLStaticImage*	_normalQuestPanel;
	
	TLStaticImage*		m_UpdateInfoFrame;
	TLStaticText*		m_UpdateInfoText;
	TLVertScrollbar*	m_UpdateInfoScroll;
	TLVertScrollbar*	m_updateTreeSroll;
	TLTree*				m_updateTree;
	string				m_yearString;
	string				m_monthString;
	string				m_dayString;
	bool				m_isUpdated;
	Point				m_pos;
	vector< pair< int, int > >    m_mapKeyArray;
	UpdateInfoMap		m_updateInfoMap;
	TLButton* _prevBtn;
	TLButton* _nextBtn;
	KUiFSBibleSpecialQuestItem _specialQuestItem[UI_FSBIBLE_MAX_SPECIAL_QUEST_COUNT_PER_PAGE];
	TLRadioButton* todayMsgBtn;
	TLRadioButton* questBtn;

	//普通任务——begin
	TLVertScrollbar*d_questListPanelScrollBar;
	KSimpleQuestInfo	_questListData[UI_FSBIBLE_MAX_QUEST_COUNT];
	KSimpleQuestInfo	m_oldQuestList[UI_FSBIBLE_MAX_QUEST_COUNT];
	int				d_questListMaxHeight;

	TLStaticImage*	d_questInfoClipper;
	TLStaticImage*	d_questInfoPanel;
	TLStaticText*	d_questText;
	TLVertScrollbar*d_questInfoPanelScrollBar;
	char			d_lomsg[LAYOUT_TEXT_MAX_LEN];
	int				d_questTextLayoutWidth;
	int				d_questInfoMaxHeight;

	TLStaticImage*	d_questRewardPanel;
	TLStaticText*	d_questRewardMoneyText;
	
	TLStaticImage*	d_itemFrameTemplate;
	TLStaticText*	d_rewardMsgText;
	KObjAtContRegion d_questCtrlUserData[MAX_QUEST_REWARD_ITEM];
	TLGameObject*	d_itemImage[MAX_QUEST_REWARD_ITEM];
	KUiCommonGrid	d_itemImageGrid[MAX_QUEST_REWARD_ITEM];
	StaticImage*	d_itemFrame[MAX_QUEST_REWARD_ITEM];

	TLStaticText*	d_rewardSelectMsgText;
	KObjAtContRegion d_selectCtrlUserData[MAX_QUEST_REWARD_ITEM];
	TLGameObject*	d_selectItemImage[MAX_QUEST_REWARD_ITEM];
	KUiCommonGrid	d_selectItemImageGrid[MAX_QUEST_REWARD_ITEM];
	StaticImage*	d_selectItemFrame[MAX_QUEST_REWARD_ITEM];
	int				d_selQuestId;
	//普通任务——end

	unsigned long	m_FileSize;
	int				m_QuestNum;
	int				_specialQuestCurPage;
	int				_specialQuestPageCount;
	bool			m_IsLoadList;
private:
	//更新提示版--begin
	bool	onUpdateTreeItemMouseDown(const EventArgs& args);
	bool	onUpdateTreeBrance(const EventArgs& args);
	bool	onUpdateTreeWheelChanged(const EventArgs& args);
	bool	onUpdateTreeScroll(const EventArgs& args);
	void	InitializeUpdateTree();
	bool	onUpdateContentScrol( const CEGUI::EventArgs& args );
	//更新提示版--end
	void	loadUi();
	bool	clickClose(const EventArgs& args);
	
	void	useTemplate(StaticImage* wnd, StaticImage* templateWnd);

	void	hideAllPanel();
	//今日提示面板——begin
	bool	clickTodayMsgPanelBtn(const EventArgs& args);
	bool	clickQuestPanelBtn(const EventArgs& args);
	

	void	loadSpecialQuestInfo();

    bool	onPrevPage( const EventArgs& args );
    bool	onNextPage( const EventArgs& args );
	//普通任务
	void	flashQuestList();
	void	showMoneyExp(int money, int exp);
	void	showSelQuest();
	void	hideQuestInfo();
	void	layoutQuestInfo();
    bool	onQuestInfoWheelChanged( const EventArgs& args );
    bool	onListWheelChanged( const EventArgs& args );
    bool	selectQuest( const EventArgs& args );
	bool	onQuestListPanelScroll(const EventArgs& args);
	bool	onQuestInfoPanelScroll(const EventArgs& args);
    bool	onDeleteQuest( const EventArgs& args );
    bool	onTrackQuest( const EventArgs& args );

	bool	onHoverText(const EventArgs& args);
	bool	onLeaveText(const EventArgs& args);
	bool	onClickQuestInfo(const EventArgs& args);
	
	bool	selQuestValidate();
	static void	doDeleteQuest();
	//今日提示面板——end

	bool	vslUpdateInfoScroll_ScrollPositionChanged(const EventArgs & args);
	bool	btnUpdateInfo_MouseClick(const EventArgs & args);
	void	ShowUpdateInfoPanel();
	void	LoadUpdateInfoFrame();
	void	LoadUpdateInfoText();

	bool	m_isUsed;

public:
	KUiFSBible();
	~KUiFSBible();

	static KUiFSBible& getSingleton();

	void	ClearQuestList();
	void	showQuestPanel();
	void	showSpecialPanel();
	void	showSpecialPanelIfHave();
	void	show();
	void	hide();
	void	toggle();
	void	toggleToday();
	void	toggleQuest();
	bool	isVisible(){	return _thisWindow->isVisible();	};

	void	updateSpeicalQuestData(vector<SpecialQuestData>& data);

	void	onQuestListChange();
	void	onQuestChange(int questId);
	void	FirstGetQuestList();



/***************操作帮助(方法说明请参看cpp文件的函数说明)*******************************************************************/
public:
	void		ShowHelp( void );
	void		InitHelp();
	void		CreateHelpTree();										//根据配置文件创建树控件
	void		DestryHelpTree();										//销毁树	
	bool		onHelpSelItemMouseDown ( const CEGUI::EventArgs& args	);
	bool		onHelpTreeScroll( const CEGUI::EventArgs& args  );
	bool		onHelpMultiScrol( const CEGUI::EventArgs& args  );
	bool		onHelpMoveWindow( const CEGUI::EventArgs& args  );
	bool		onHelpClickPanelBtn(const EventArgs& args);
	bool		onHelpBranceOpened( const CEGUI::EventArgs& args );		//帮助树打开事件	
	bool		onHelpWheelChanged( const CEGUI::EventArgs& args );		//鼠标滑轮滚动事件	
	bool		onHelpMultiEditMouseMove( const CEGUI::EventArgs& args );
	bool		onHelpMultiEditMouseClick( const CEGUI::EventArgs& args );
	bool		onHelpMultiEditMouseLeaves( const CEGUI::EventArgs& args );
	bool		onHelpMultiEditMouseWheel( const CEGUI::EventArgs& args );
	static void	SetHelpItemSelectStatus( CEGUI::String &pName );	
private:
	void		InitHelpCtrl();											//初始化各个控件并注册事件
	void		InitHelpLayOut();										//初始化排版系统
	int			getHelpParentInfo( const char *leafInfo );				//解析配置文件中树的父节点
	std::string	getHelpItemWord( const char *leafInfo );			    //返回配置文件中树结点的字符信息
	int			getHelpItemDesInfo( const char *leafInfo );				//返回该树结点的描述信息的索引	
	std::string	getHelpLeafName( const std::string &lay, const int layIndex, 
	const std::string &treeInfo, const int leafIndex );					//组合得到配置文件中的key值	
	bool		onLoginAutoShow( const CEGUI::EventArgs& args );
	
private:	
	TLStaticImage*						_helpPanel;
	CEGUI::TLTree						*d_pHelp;						//操作帮助窗口目录树
	CEGUI::TLStaticText					*d_pHelpMultiEdit;				//操作帮助窗口帮助信息
	CEGUI::TLVertScrollbar				*d_pHelpTreeScrol;				//操作帮助窗口目录树垂直滚动条
	CEGUI::TLVertScrollbar				*d_pHelpMulEdScrol;				//操作帮助窗口帮助信息垂直滚动条
	CEGUI::TLStaticImage				*d_pHelpTreeParent;				//操作帮助窗口目录树父窗口，用于实现滚动
	std::vector<CEGUI::TLTreeItem *>	d_itemArray;					//操作帮助窗口目录树的条目数组
	bool								d_bItem;						//判断树的选中状态?
	char								d_layoutTextHead[COMMON_CLIENT_MSG_LEN_64];
	bool								d_bHelpTip;						//是否帮助信息中的tip窗口已经打开	
	CEGUI::TLCheckbox*					TodayMsgTip;					//今日焦点登陆后自动打开
};

class SortRange
{
public:
	bool operator()( const UpdateInfoMap::iterator A, const UpdateInfoMap::iterator B )
	{
		return A->second.first < B->second.first;
	}
};

class CreateTreeFromMultiMap
{
	UpdateInfoMap*		m_map;
	TLTree*				m_treeRoot;
	TLTreeItem*			m_parentItem;
	string				m_yearString;
	string				m_monthString;
	string				m_dayString;	
	
public:
	CreateTreeFromMultiMap( TLTree* treeRoot,  UpdateInfoMap* pMap, string& year, string& month, string& day);
	void operator()( pair< int,int >& data);
};
#endif