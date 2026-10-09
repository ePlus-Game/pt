#include "UiChatConfig.h"
#include "UiChatWindow.h"
#include "../UiConfigManager.h"

KUiChanConfig::KUiChanConfig()
{
	_loaded = false;
	_thisWindow = NULL;

	for(int i = 0; i < ChannelCount; ++i)
	{
		_checkFlag[i] = NULL;
	}

	_ok		= NULL;
	_cancel = NULL;
	_close	= NULL;
	_panelIndex = -1;

	_input	= NULL;
	m_LocalText = NULL;

	_chanIds.clear();
}

KUiChanConfig::~KUiChanConfig()
{

}

KUiChanConfig& KUiChanConfig::getSingleton()
{
	static KUiChanConfig singleton;
	return singleton;
}

bool KUiChanConfig::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_CHAT_CONFIG_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_CHAT_CONFIG_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return false;
	}
#endif
	
	if(!_thisWindow)
	{
		return false;
	}
	
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->hide();

	_checkFlag[World]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/World");
	_checkFlag[Team]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Team");
	_checkFlag[Local]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Local");
	_checkFlag[Chat]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Chat");
	_checkFlag[Shizu]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Shizu");
	_checkFlag[GM]		= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/System");
	_checkFlag[Zhuhou]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Zhuhou");
	_checkFlag[Battle]	= (Checkbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Battle");

	_input				= (Editbox*)_thisWindow->getChild("TaharezLook/ChanConfig/Name");

	_ok					= (PushButton*)_thisWindow->getChild("TaharezLook/ChanConfig/OK");
	_cancel				= (PushButton*)_thisWindow->getChild("TaharezLook/ChanConfig/Cancel");
	_close				= (PushButton*)_thisWindow->getChild("TaharezLook/ChanConfig/Close");

	m_LocalText			= static_cast<TLStaticText *>(_thisWindow->getChild("TaharezLook/ChanConfig/LocalText"));

	_ok->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChanConfig::onClickOk, this));
	_cancel->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChanConfig::onClickCancel, this));
	_close->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChanConfig::onClickOk, this));

	memset(m_LocalChannelName, 0, sizeof(m_LocalChannelName));
	strcpy(m_LocalChannelName, Utf8ToAnsi(m_LocalText->getText()));
	m_LocalTextColor = m_LocalText->getTextColours();
	return true;
}

bool KUiChanConfig::onClickOk(const EventArgs& args)
{
	hide();

	KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();

	if(_panelIndex < 0 || _panelIndex >= chanCfg.framesCfg.size())
	{
		return false;
	}
	
	vector<string>& chanNames = chanCfg.framesCfg[_panelIndex].channelName;
	chanNames.clear();
	for(int i = 0; i < ChannelCount; ++i)
	{
		if(_checkFlag[i]->isSelected())
		{
			string name = Utf8ToAnsi(_checkFlag[i]->getText());

			chanNames.push_back(name);
		}
	}

	string frameNewName = Utf8ToAnsi(_input->getText());
	chanCfg.framesCfg[_panelIndex].frameName = frameNewName;

	KUiChannelCentre::GetSingleton().freshFrameChannel(_panelIndex);

	return true;
}

bool KUiChanConfig::onClickCancel(const EventArgs& args)
{
	hide();
	return true;
}

void KUiChanConfig::hide()
{
	if(!_loaded)
	{
		return;
	}

	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

void KUiChanConfig::clearAll()
{
	for(int i = 0; i < ChannelCount; ++i)
	{
		_checkFlag[i]->setSelected(false);
	}
}

void KUiChanConfig::show(int panelIndex)
{
	KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();
	if(panelIndex < 0 || panelIndex >= chanCfg.framesCfg.size())
	{
		return;
	}

	if(!_loaded)
	{
		_loaded = loadUi();
	}
	
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
	
	clearAll();

	_panelIndex = panelIndex;

	const vector<string> chanNames = chanCfg.framesCfg[_panelIndex].channelName;

	for(int i = 0; i < chanNames.size(); ++i)
	{
		const string& name = chanNames[i];
		for(int j = 0; j < ChannelCount; ++j)
		{
			string cfgName = Utf8ToAnsi(_checkFlag[j]->getText());
			if(cfgName == name)
			{
				_checkFlag[j]->setSelected(true);
			}
		}
	}
	
	const string& frameName = chanCfg.framesCfg[_panelIndex].frameName;
	_input->setText(AnsiToUtf8(frameName.c_str()));

	MapChannelInfo mapInfo;
	if (KUiChanMgr::getSinglton().getChanInfo(MAP_ROOM_ID, mapInfo))
	{
		m_LocalText->setText(AnsiToUtf8(mapInfo.szChannelFullName));

		int r = 255;
		int g = 255;
		int b = 255;

		sscanf(mapInfo.szColor, "%d,%d,%d", &r, &g, &b);
		m_LocalText->setTextColours(RGB(b, g, r));
	}
	else
	{
		m_LocalText->setText(AnsiToUtf8(m_LocalChannelName));
	}
}

void KUiChanConfig::toggle()
{
	if(!_loaded)
	{
		return;
	}
	
	if(!_thisWindow)
	{
		return;
	}
	
	if(isVisible())
	{
		hide();
	}
	else
	{
		show(_panelIndex);
	}
}

bool KUiChanConfig::isVisible()
{
	if(!_loaded)
	{
		return false;
	}
	
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}
