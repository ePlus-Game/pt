
#include "UiTimer.h"
#include "../UiSheetMgr.h"

KUiTimer& KUiTimer::getSingleton()
{
	static KUiTimer singleton;
	return singleton;
}

KUiTimer::KUiTimer()
{
	load();
}

KUiTimer::~KUiTimer()
{

};

void KUiTimer::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_TIMER_WINDOW_NAME_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_TIMER_WINDOW_NAME);
			
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif
	
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->show();
	_thisWindow->setRenderMode(true);
	_thisWindow->disable();

	char windowName[UI_TIMER_WINDOW_NAME_MAX_LEN];
	for(int i = 0; i < UI_TIMER_COUNT; ++i)
	{
		sprintf(windowName, "TaharezLook/Timer/Description%d", i + 1);
		_description[i]		= (TLStaticText*)_thisWindow->getChild(windowName);
		_description[i]->hide();

		sprintf(windowName, "TaharezLook/Timer/Counter%d", i + 1);
		_counter[i]			= (TLStaticText*)_thisWindow->getChild(windowName);
		_counter[i]->subscribeEvent(Window::EventNewFrame, Event::Subscriber(&KUiTimer::onTimer, this));
		_counter[i]->hide();

		_timer[i] = 0;
	}
	_thisWindow->setZLevel(Window::SuperBottom);
	_thisWindow->SetBottomWindow();
		
}

void KUiTimer::openTimer(int time, char* description, int type)
{
	if(!_thisWindow)
	{
		return;
	}
	
	time *= 1000;				//×ª»»³ÉºÁÃë
	if(!description)
	{
		return;
	}

	if(type < 1 || type > UI_TIMER_COUNT)
	{
		return;
	}
	
	int index = type - 1;
	_description[index]->setText(AnsiToUtf8(description));

	_timer[index] = time;
	if(time > 0)
	{
		_counter[index]->beginUpdate();
		_counter[index]->show();
		_description[index]->show();
	}
	else
	{
		_counter[index]->stopUpdate();
		_counter[index]->hide();
		_description[index]->hide();
	}
}

bool KUiTimer::onTimer(const EventArgs& args)
{
	FrameEventArgs* timerEvent = (FrameEventArgs*)&args;

	int index = UI_TIMER_INVALID_INDEX;
	for(int i = 0; i < UI_TIMER_COUNT; ++i)
	{
		if(_counter[i] == timerEvent->window)
		{
			index = i;
			break;
		}
	}

	if(UI_TIMER_INVALID_INDEX == index)
	{
		return true;
	}

	_timer[index] -= timerEvent->elapse;

	if(_timer[index] <= 0)
	{
		_counter[index]->hide();
		_counter[index]->stopUpdate();
		_description[index]->hide();
	}

	char leftTimeText[UI_TIMER_WINDOW_NAME_MAX_LEN] = "";

	int secTime = _timer[index] / 1000;

	int hour = secTime / 3600;
	int min = (secTime % 3600) / 60;
	int sec = (secTime % 60);
	if(hour > 0)
	{
		sprintf(leftTimeText, "%d:%02d:%02d", hour, min, sec);
	}
	else if(min > 0)
	{
		sprintf(leftTimeText, "%d:%02d", min, sec);
	}
	else if(sec > 0)
	{
		sprintf(leftTimeText, "%d", sec);
	}
	else
	{
		sprintf(leftTimeText, "0.%d", _timer[index] / 100);
	}

	_counter[index]->setText(leftTimeText);

	return true;
}

void KUiTimer::show()
{
	if(!_thisWindow)
	{
		return;
	}
	
	_thisWindow->show();
}

void KUiTimer::hide()
{
	if(!_thisWindow)
	{
		return;
	}
	
	_thisWindow->hide();
}