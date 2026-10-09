//¼ÆÊ±Æ÷ xiehong 2007-11-13

#include "TLStatic.h"

#define UI_TIMER_COUNT 5
#define UI_TIMER_WINDOW_NAME_1024 "uisettings/layouts1024/Timer.ls"
#define UI_TIMER_WINDOW_NAME "uisettings/layouts/Timer.ls"
#define UI_TIMER_WINDOW_NAME_MAX_LEN 256
#define UI_TIMER_INVALID_INDEX -1

class KUiTimer
{
	TLStaticImage*	_thisWindow;
	TLStaticText*	_description[UI_TIMER_COUNT];
	TLStaticText*	_counter[UI_TIMER_COUNT];

	int				_timer[UI_TIMER_COUNT];

private:
	
	bool onTimer(const EventArgs& args);

public:	
	KUiTimer();
	~KUiTimer();

	static KUiTimer& getSingleton();

	void load();
	void openTimer(int time, char* description, int type);
	void show();
	void hide();
};
