//xiehong 2008-4-18 推荐人领取奖励界面

#ifndef UI_RECOMMOND_H
#define UI_RECOMMOND_H

#define UI_RECOMMEND_WINDOW_PATH_1024 "uisettings/layouts1024/Recommend.ls"
#define UI_RECOMMEND_WINDOW_PATH "uisettings/layouts/Recommend.ls"
#define UI_RECOMMEND_ITEM_WINDOW_PATH_1024 "uisettings/layouts1024/RecommendItem.ls"
#define UI_RECOMMEND_ITEM_WINDOW_PATH "uisettings/layouts/RecommendItem.ls"

#include "TLButton.h"
#include "TLEditbox.h"
#include "TLStatic.h"
#include "TLVertScrollbar.h"
#include "GameDataDef.h"

class KUiRecommend
{
	TLStaticImage*	_thisWindow;

	TLButton*		_getBtn;
	TLButton*		_cancelBtn;
	TLButton*		_closeBtn;
	
	TLStaticImage*	_panel;
	TLVertScrollbar* _scrollbar;

	TLStaticText*	_money;
	TLStaticText*	_totalMoney;
	TLStaticText*	_studentCount;

	TLStaticImage*	_items[MAX_STUDENT_COUNT];
private:
	void	loadUi();
	void	hideAllItems();

protected:
	bool	onGet(const EventArgs& e);

	bool	onClose(const EventArgs& e);
public:
	void	show(RecommedList& studentList, int rewardToAdd, int totalRewardTicketAdded);
	void	showReport(const char* masterName, int lastLevel, int curLevel);
	void	hide();
	bool	isVisible();
	
	static KUiRecommend& getSingleton();
	static char* getMoneyLayout(int money, int color);
	static char* getMoneyLayoutObj(int money, int color);

	KUiRecommend();
	~KUiRecommend();
};

#endif