//xiehong 2008-4-18 滑动面板管理器，用来给一些窗口加入滑动条以及事件处理，不用每个窗口去单写这些东西了

#ifndef UI_SCROLL_PANEL_MGR
#define UI_SCROLL_PANEL_MGR

#include "TLStatic.h"
#include "TLVertScrollbar.h"

class KUiScrollPanelMgr
{
	TLStaticImage*		_clipper;
	TLStaticImage*		_panel;
	TLVertScrollbar*	_scrollbar;

public:
	KUiScrollPanelMgr(TLStaticImage* clipper, TLStaticImage* panel, TLVertScrollbar* scrollbar);
	~KUiScrollPanelMgr();

protected:
	bool onScroll(const EventArgs& args);
	bool onWheelChanged(const EventArgs& args);
	bool onSized(const EventArgs& args);
	void resized();

private:
	bool checkValid();
};

#endif
