//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/12/2006 10:01
//      File_base        : UiQuestManage
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUIQUESTMANAGE_H
#define KUIQUESTMANAGE_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLStatic.h"
#include "TLButton.h"
#include "TLGameObject.h"
#include "TLVertScrollbar.h"
#include "UiCommonGrid.h"
#include "UiQuestTrack.h"

#define UI_QUESTMANAGE_MAX_QUEST_COUNT 40
#define UI_QUESTMANAGE_COMMON_CTRL_OFFSET 10
#define UI_QUESTMANAGE_TEMPLATE_NUM 3

using namespace CEGUI;

class KUiQuestManage : public KUiWndSingleton<KUiQuestManage>
{
	enum StringCode
	{
		QUEST_DELETE_NOTIFY = 0,
		QUEST_OK,
		QUEST_CANCEL
	};

	TLStaticImage*	d_questListPanel;
	TLButton*		d_questTitleL;
	TLButton*		d_questTitleTemplate[UI_QUESTMANAGE_TEMPLATE_NUM];
	TLButton*		d_questTitleBtn[UI_QUESTMANAGE_MAX_QUEST_COUNT];
	TLVertScrollbar*d_questListPanelScrollBar;
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
	TLGameObject*	d_itemImage[MAX_QUEST_REWARD_ITEM];
	KUiCommonGrid	d_itemImageGrid[MAX_QUEST_REWARD_ITEM];
	StaticImage*	d_itemFrame[MAX_QUEST_REWARD_ITEM];

	TLStaticText*	d_rewardSelectMsgText;
	TLGameObject*	d_selectItemImage[MAX_QUEST_REWARD_ITEM];
	KUiCommonGrid	d_selectItemImageGrid[MAX_QUEST_REWARD_ITEM];
	StaticImage*	d_selectItemFrame[MAX_QUEST_REWARD_ITEM];

	TLButton*		d_trackBtn;
	TLButton*		d_questDeleteBtn;	
	TLButton*		d_cancelBtn;
	TLButton*		d_closeBtn;
	
	//任务列表和当前选中的任务信息
	int					d_selQuestId;
	KSimpleQuestInfo	d_questList[UI_QUESTMANAGE_MAX_QUEST_COUNT];
	int					d_questListBtnCount;

public:
	KUiQuestManage( const String& id_name );
	~KUiQuestManage( void );

	void	show();
	void	hide();
	void	Init();
	void	onQuestListChange();
	void	onQuestChange(int questId);

	void	showIfHave();
	
	static void	doDeleteQuest();
protected:
    bool	onWndShow( const EventArgs& args );
    bool	onListWheelChanged( const EventArgs& args );
    bool	onQuestInfoWheelChanged( const EventArgs& args );
    bool	onDeleteQuest( const EventArgs& args );
    bool	onTrackQuest( const EventArgs& args );
	bool	onCancel ( const EventArgs& args );
	bool	onClickQuestList( const EventArgs& args );
	bool	onQuestListPanelScroll(const EventArgs& args);
	bool	onQuestInfoPanelScroll(const EventArgs& args);
	bool	onClickQuestInfo(const EventArgs& args);
	bool	onHoverText(const EventArgs& args);
	bool	onLeaveText(const EventArgs& args);
private:
	void	getChild();
	void	useTemplate(TLButton* wnd, TLButton* templateWnd);
	void	useTemplate(StaticImage* wnd, StaticImage* templateWnd);

	void	showMoneyExp(int money, int exp);
	void	flashQuestList();
	void	layoutQuestList();
	void	hideQuestList();
	void	hideQuestInfo();
	void	layoutQuestInfo();
	bool	selQuestValidate();
	void	showSelQuest();
};

#endif	