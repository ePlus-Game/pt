 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 03/8/2007
//      File_base        : KUiSmith
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 打造界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef Ui_SMITH_H
#define Ui_SMITH_H

#include "TLButton.h"
#include "TLStatic.h"
#include "TLVertScrollbar.h"
#include "TLTree.h"
#include "TLRadioButton.h"
#include "TLGameObject.h"
#include "UiCommonGrid.h"

#include <vector>

#define UI_SMITH_MAX_LIST_ITEM_COUNT 400
#define UI_SMITH_MAX_REQ_ITEM_COUNT 9
#define UI_SMITH_INVALID_LIST_INDEX -1
#define UI_SMITH_TYPE1_COUNT 4

#define UI_SMITH_WINDOW_PATH_1024 "uisettings/layouts1024/Smith.ls"
#define UI_SMITH_WINDOW_PATH "uisettings/layouts/Smith.ls"
#define UI_SMITH_WINDOW_NAME "TaharezLook/Smith"

using namespace CEGUI;

class KUiSmith
{
public:
	enum SmithCode
	{
		smith_invalid_rule = 0,
		smith_less_material,
		smith_money_not_enough,
		smith_skill_exp_not_enough,
		smith_another_action_processing,
		smith_failure_destory,
		smith_failure_not_destory,
		smith_failure_not_enough_space,
		smith_params_error,
		smith_success,
	};
	typedef std::vector<CommonTreeItem> TreeItemList;

private:
	char		d_layoutText[LAYOUT_TEXT_MAX_LEN];

	TLStaticImage*	_thisWindow;

	TLVertScrollbar*	d_scrollBar;		//用于显示打造列表的滚动条
	TLStaticImage*	d_skillListPannel;		//打造列表底版
	TLTree*			d_smithItems;			//打造列表
	TLRadioButton*	d_type1SelectBtn[UI_SMITH_TYPE1_COUNT];		//选择打造类型按钮：材料、气炼、魂炼、神炼
	TreeItemList	d_curList;				//打造列表数据结构
	TLButton*		d_type4Btn;
	TLStaticImage*	d_type4Pannel;
	TLTree*			d_type4List;
	
	TLButton*		d_foldAllBtn;			//打造列表中全体合拢散开按钮
	TLButton*		d_openAllBtn;			//打造列表中全体合拢散开按钮

	int				d_selItemIndex;			//当前选定项

	TLStaticImage*	d_icon;					//当前打造规则中的生成品图标

	TLGameObject*	d_reqItem[UI_SMITH_MAX_REQ_ITEM_COUNT];//要求的物品图标
	KObjAtContRegion d_reqItemData[UI_SMITH_MAX_REQ_ITEM_COUNT];
	KUiCommonGrid	d_reqItemGrid[UI_SMITH_MAX_REQ_ITEM_COUNT];
	
	char			d_ruleMsg[LAYOUT_TEXT_MAX_LEN];//当前打造规则说明文字
	TLStaticText*	d_materialRequireText;		   //当前打造规则说明文字控件
	TLStaticText*	d_moneyRequireText;		   //当前打造规则说明文字控件
	TLStaticText*	d_smithRate;		   //当前打造规则说明文字控件
	
	TLButton*		d_smithOneBtn;				//确定打造按钮
	TLButton*		d_smithAllBtn;				//确定打造按钮
	TLButton*		d_cancelBtn;				//取消按钮
	TLButton*		d_closeBtn;					//右上方的关闭按钮

	bool			d_continueSmith;
	
private:
	void	showType1();
	bool	selectType1(const EventArgs& args);

	void	showType23(const char* type1);
	bool	selectType23(const EventArgs& args);
    bool	onType2OpenClose( const EventArgs& args );
	void	freshType23Scroll();

	void	showType4(const char* type3);
	bool	selectType4(const EventArgs& args);
	bool	openType4List(const EventArgs& args);

	void clearGameObj(TLGameObject* goCtrl);
	void loadUi();
	void clear();
	void useTemplate(TLButton* wnd, TLButton* templateWnd);
	int	 getItemListHeight();
	void layout();
	void offset();
	void onClickRoot(int index);
	void onClickNode(int index);
	void showSmithRule(int ruleId);
	int  itemCountOfAType(ItemType& type);
	void showErrorMsg(int errorCode);
	void doSmith();
	int  canSmithCount();
	
	void showTip(int ruleId);
	void clearSameObj(int itemIndex);

	void freshLockedItem();
protected:

	bool fold(const EventArgs& args);
	bool scroll(const EventArgs& args);
	bool onClickSmithOne(const EventArgs& args);
	bool onClickSmithAll(const EventArgs& args);
	bool onClickCancel(const EventArgs& args);
	bool onClickClose(const EventArgs& args);
	bool onClickReqItem(const EventArgs& args);
	bool onHoverRate(const EventArgs& args);
	bool onLevaeRate(const EventArgs& args);
	bool onHoverIcon(const EventArgs& args);
	bool onLevaeIcon(const EventArgs& args);
    bool onWheel( const EventArgs& args );
    bool onHide( const EventArgs& args );
    bool onShow( const EventArgs& args );

public:
	void	show();
	void	hide();
	bool	isVisible();
	void	onConditionChanged();
	void	open(int shopId);
	void	onEndSmith();

    KUiSmith();
    ~KUiSmith();
	static KUiSmith& getSingleton();
};


#endif