
#include "UiShizuBanner.h"
#include "../UiSheetMgr.h"
#include "../UiConfigManager.h"
#include "CEGUI.h"
#include "KIniFile.h"

KUiShizuBanner::KUiShizuBanner()
{
	_thisWindow = NULL;

	m_jiugongPosX		= 0;
	m_jiugongPosY		= 0;
	m_shizuBannerPosX	= 0;
	m_shizuBannerPosY	= 0;

	m_Pronunciamento = NULL;

	loadUi();
}

KUiShizuBanner::~KUiShizuBanner()
{

}
	
KUiShizuBanner& KUiShizuBanner::getSingleton()
{
	static KUiShizuBanner singleton;
	return singleton;
}

void KUiShizuBanner::loadUi()
{	
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SHIZU_BANNER_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SHIZU_BANNER_WINDOW_PATH);
		}

		m_Pronunciamento = static_cast<TLStaticText *>(_thisWindow->getChild("TaharezLook/ShizuBanner/Pronunciamento"));
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		m_Pronunciamento = NULL;
		return;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);

	_thisWindow->setZLevel(Window::SuperBottom);
	_thisWindow->SetBottomWindow();

	for(int i = 0; i < UI_SHIZU_BANNER_MAX_COUNT; ++i)
	{
		_msg[i] = (TLStaticText*)_thisWindow->getChild(_thisWindow->getName() + "/Text" + PropertyHelper::intToString(i + 1));
		_msg[i]->useLayout();
		_msg[i]->hide();
	}

	_thisWindow->disable();
	_thisWindow->setRenderMode(true);

	if (m_Pronunciamento == NULL)
	{
		return;
	}

	m_Pronunciamento->useLayout();
	m_Pronunciamento->hide();

	KIniFile iniFile;
	if (!iniFile.Load(UI_SHIZU_BANNER_INI_FILE_PATH))
	{
		Point pos;
		pos = _thisWindow->getPosition(Absolute);
		m_jiugongPosX		= pos.d_x;
		m_jiugongPosY		= pos.d_y;
		m_shizuBannerPosX	= pos.d_x;
		m_shizuBannerPosY	= pos.d_y;
	}
	else
	{
		Point pos;
		pos = _thisWindow->getPosition(Absolute);
		iniFile.GetInteger("ShizuBanner", "shizuPoxX",  pos.d_x, &m_shizuBannerPosX);
		iniFile.GetInteger("ShizuBanner", "shizuPoxY",  pos.d_y, &m_shizuBannerPosY);
		iniFile.GetInteger("ShizuBanner", "jiugongPosX", pos.d_x, &m_jiugongPosX);
		iniFile.GetInteger("ShizuBanner", "jiugongPosY", pos.d_y, &m_jiugongPosY);
	}
}

void KUiShizuBanner::clean()
{
	if(!_thisWindow)
	{
		return;
	}

	for(int index = 0; index < UI_SHIZU_BANNER_MAX_COUNT; ++index)
	{
		showBanner(index, "");
	}
}

void KUiShizuBanner::showBanner(int index, char* bannerText)
{
	if(!_thisWindow)
	{
		return;
	}

	if(bannerText == NULL)
	{
		return;
	}

	if(index < 0 || index >= UI_SHIZU_BANNER_MAX_COUNT)
	{
		return;
	}

	SetBannerPos(m_shizuBannerPosX, m_shizuBannerPosY);
	_msg[index]->show();

	char layoutText[LAYOUT_TEXT_MAX_LEN];

	if(bannerText[0] == '<')
	{
		sprintf(layoutText, 
			"<Layout width=200 m-l=15 m-r=15 m-t=15 m-b=15>"
				"<Seg text-align=left >"
					"%s"
				"</Seg>"
			"</Layout>", bannerText);
		_msg[index]->getLayout()->SetText(layoutText);
	}
	else if(!strcmp(bannerText, ""))
	{
		_msg[index]->hide();
	}
	else
	{
		_msg[index]->setText(AnsiToUtf8(bannerText) + String(" (bad layout)"));
	}

	//看是否需要显示
	_thisWindow->hide();
	for(int i = 0; i < UI_SHIZU_BANNER_MAX_COUNT; ++i)
	{
		if(_msg[i]->isVisible(true))
		{
			_thisWindow->show();
			break;
		}
	}
	m_Pronunciamento->hide();
}

void KUiShizuBanner::ShowPronunciamento(char * msg, int msgLen)
{
	if (_thisWindow == NULL)
	{
		return;
	}

	if (m_Pronunciamento == NULL)
	{
		return;
	}


	if (msg == NULL || msg[0] == 0)
	{
		return;
	}

	for(int i = 0; i < UI_SHIZU_BANNER_MAX_COUNT; i++)
	{
		if (_msg[i] == NULL)
			continue;

		_msg[i]->hide();
	}

	SetBannerPos(m_jiugongPosX, m_jiugongPosY);
	_thisWindow->show();

	m_Pronunciamento->show();

	if (msgLen > 0)
	{
		msg[msgLen] = 0;
	}

	if (msg[0] == '<')
	{
		m_Pronunciamento->getLayout()->SetText(msg);
	}
	else
	{
		m_Pronunciamento->setText(AnsiToUtf8("Bad layout"));
	}
}

void KUiShizuBanner::Hide()
{
	if (_thisWindow != NULL)
	{
		if (_thisWindow->isVisible())
		{
			_thisWindow->hide();
		}
	}
}

void KUiShizuBanner::SetBannerPos(int & x, int & y)
{
	if (_thisWindow == NULL)
	{
		return;
	}
	_thisWindow->setPosition(Absolute, Point(x, y));
}