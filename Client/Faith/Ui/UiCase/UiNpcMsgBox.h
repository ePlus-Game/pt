//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 11:20
//      File_base        : UiMessageBox
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUINPCMSGBOX_H
#define KUINPCMSGBOX_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLStatic.h"
#include "TLButton.h"
#include "TLGameObject.h"
#include "TLVertScrollbar.h"
#include "UiCommonGrid.h"

#define UI_NPCBOX_MAX_QUEST_COUNT 10
#define UI_NPCBOX_QUESTCTRL_TOP_OFFSET 10
#define	UI_NPCBOX_REQUIREITEM 5

using namespace CEGUI;
class KUiNpcMsgBox : public KUiWndSingleton<KUiNpcMsgBox>
{
	TLStaticImage*	d_ctrlPanel;
	TLStaticImage*	d_clipper;
	int				d_clipperHeightAtRequirPanelShow;
	
	TLVertScrollbar*d_scrollBar;

	char			d_lomsg[LAYOUT_TEXT_MAX_LEN];
	int				d_contentLayoutWidth;
	TLStaticText*	d_contentText;
	
	//任务奖励面版
	TLStaticImage*	d_rewardPanel;
	TLStaticText*	d_moneyText;

	TLStaticImage*	d_itemFrameTemplate;
	TLStaticText*	d_rewardMsgText;
	TLGameObject*	d_itemImage[MAX_QUEST_REWARD_ITEM];
	KUiCommonGrid	d_itemImageGrid[MAX_QUEST_REWARD_ITEM];
	StaticImage*	d_itemFrame[MAX_QUEST_REWARD_ITEM];
	KObjAtContRegion d_rewardObjInfo[MAX_QUEST_REWARD_ITEM];

	TLStaticText*	d_rewardSelectMsgText;
	TLGameObject*	d_selectItemImage[MAX_QUEST_REWARD_ITEM];
	KUiCommonGrid	d_selectItemImageGrid[MAX_QUEST_REWARD_ITEM];
	StaticImage*	d_selectItemFrame[MAX_QUEST_REWARD_ITEM];
	KObjAtContRegion d_selectRewardObjInfo[MAX_QUEST_REWARD_ITEM];

	//需求物品放置格子
	TLStaticImage*	d_requiredPanel;
	TLGameObject*	d_requireItem[MAX_QUEST_REQUIRED_ITEM];
	KUiCommonGrid	d_requireItemGrid[MAX_QUEST_REWARD_ITEM];
	KObjAtContRegion d_requireItemObjInfo[MAX_QUEST_REWARD_ITEM];

	//任务按钮面版
	StaticImage*	d_questPanel;
	TLButton*		d_questBtn[UI_NPCBOX_MAX_QUEST_COUNT];
	TLStaticImage*	d_questTitleImage[UI_NPCBOX_MAX_QUEST_COUNT];

	TLButton*		d_submitBtn;
	TLButton*		d_acceptBtn;
	TLButton*		d_cancelBtn;
	TLButton*		d_closeBtn;

	TLGameObject*	d_curSelItem;
	KQuestState		d_questState;
	char*			d_notSelItemErrMsg;
private:
	void getChild();
	void hideAllCtrl();
	void layoutQuest();
	void layoutNpcTalk();
	void showMoneyExp(int money, int exp);
	void useTemplate(StaticImage* wnd, StaticImage* templateWnd);

	const Image*	getTitleImage(const char* imagePath);
public:
	KUiNpcMsgBox( const CEGUI::String& id_name );
	~KUiNpcMsgBox();

	void OpenNpcTalk( const KUiQuestionAndAnswer* pQuest);
	void OpenNpcMission( const KQuestInfo* pMissionParam ); 
	void Init();

protected:
    bool onWndShow			( const EventArgs& args );
	bool scroll				( const EventArgs& args	);
    bool handleOk			( const EventArgs& args );
	bool handleCannel		( const EventArgs& args );
	bool handleExit			( const EventArgs& args	);
	bool handleSelectQuest	( const EventArgs& args );
	bool handleSelectItem	( const EventArgs& args );	
	bool dropRequiredItem	( const EventArgs& args );
    bool handleWheelChanged	( const EventArgs& args );
};

#endif	