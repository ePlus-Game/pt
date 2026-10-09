
#include "UiInsurance.h"
#include "../UiSheetMgr.h"

KUiInsurance::KUiInsurance()
{
	_thisWindow = NULL;
}

KUiInsurance::~KUiInsurance()
{
	
}

KUiInsurance& KUiInsurance::getSingleton()
{
	static KUiInsurance singleton;
	return singleton;
}

bool KUiInsurance::loadUi()
{
	#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_INSURANCE_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_INSURANCE_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return false;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);

	return true;
}

void KUiInsurance::show()
{
	if(!_loaded)
	{
		bool _loaded = loadUi();
	}

	if(_thisWindow)
	{
		_thisWindow->show();
	}
}

void KUiInsurance::hide()
{
	if(_loaded && _thisWindow)
	{
		_thisWindow->hide();
	}
}

void KUiInsurance::toggle()
{
	if(isVisible())
	{
		hide();
	}
	else
	{
		show();
	}
}

bool KUiInsurance::isVisible()
{
	if(_loaded && _thisWindow)
	{
		return _thisWindow->isVisible();
	}

	return false;
}