//xiehong 统一管理界面上的2级窗口

#include "ScriptDataDef.h"

class KUiWndMgr
{
public:
	KUiWndMgr();
	static KUiWndMgr& getSingleton();

	void openAWindow(WindowName wndName);
};