//xiehong 2008-5-13 ∆µµ¿≈‰÷√√Ê∞Â

#ifndef UI_CHAT_CONFIG_H
#define UI_CHAT_CONFIG_H

#define UI_CHAT_CONFIG_WINDOW_PATH_1024 "uisettings/layouts1024/ChanConfig.ls"
#define UI_CHAT_CONFIG_WINDOW_PATH "uisettings/layouts/ChanConfig.ls"

#include "CEGUI.h"
#include <vector>
#include "GameDataDef.h"
#include "TLStatic.h"

using namespace CEGUI;
using namespace std;

class KUiChanConfig
{
	enum Channel
	{
		World = 0,
		Team,
		Local,
		Chat,
		Shizu,
		GM,
		Zhuhou,
		Battle,
		ChannelCount,
	};

	Window*		_thisWindow;
	Checkbox*	_checkFlag[ChannelCount];

	PushButton*		_ok;
	PushButton*		_cancel;
	PushButton*		_close;

	Editbox*		_input;

	bool			_loaded;

	int				_panelIndex;

	vector<int>		_chanIds;

	char			m_LocalChannelName[COMMON_CLIENT_MSG_LEN_32];
	TLStaticText *	m_LocalText;
	colour			m_LocalTextColor;

private:
	
	void	clearAll();

protected:
	bool	onClickOk(const EventArgs& args);
	bool	onClickCancel(const EventArgs& args);

public:
	bool	loadUi();

	void	hide();
	void	show(int panelIndex);
	void	toggle();
	bool	isVisible();

	KUiChanConfig();
    ~KUiChanConfig();
	static KUiChanConfig& getSingleton();
};


#endif //UI_CHAT_CONFIG_H