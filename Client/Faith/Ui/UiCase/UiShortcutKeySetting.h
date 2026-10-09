
//xiehong 2007-8-8
//快捷键设置界面

#ifndef UI_SHORTCUT_KEY_SETTING
#define UI_SHORTCUT_KEY_SETTING

#define UI_SHORTCUT_KEY_FILE_NAME "UserData/Account/skcfg_%s_%s.lua"
#define UI_SHORTCUT_KEY_DEFAULT_FILE_NAME "UiSettings/autoexec_default.lua"
#define UI_SKSETTING_WINDOW_NAME_1024 "uisettings/layouts1024/SKSetting.ls"
#define UI_SKSETTING_WINDOW_NAME "uisettings/layouts/SKSetting.ls"
#define UI_SKSETTING_ITEM_WINDOW_NAME "uisettings/layouts/SKSettingItem.ls"
#define UI_SKSETTING_MAX_SETTING_COUNT 120
#define UI_SKSETTING_INVALID_INDEX_ID -1
#define UI_SKSETTING_MAX_FILE_SIZE 1024 * 30

#include <vector>
#include "TLStatic.h"
#include "TLButton.h"
#include "..\UiCommon.h"
#include "TLVertScrollbar.h"
#include "TLRadioButton.h"

using namespace CEGUI;

class KUiSKSettingItem
{
	bool			_isTitle;
protected:
	TLStaticImage*	_thisWindow;
	TLStaticImage*	_cmdName;
	TLRadioButton*	_keyName;
	std::string		_cmdNameText;
	
private:
	void			getChild();
protected:
	virtual bool	onKeyBtnDown(const EventArgs& e);
public:
	KUiSKSettingItem();
	~KUiSKSettingItem();
	
	Point			getPos();
	void			setPos(const Point& pos);
	int				getHeight();
	int				getWidth();

	void			setType(bool isTitle);
	bool			isTitle();
	bool			isSelected();
	bool			isVisible();
	std::string		getCmdName();


	void			setParent(Window* parent);
	void			show(const char* cmdName, const char* keyName);
	void			hide();
	void			setSelected(bool selectState);
};

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

class KUiSKSetting
{
	KUiSKSettingItem	_cmdCtrl[UI_SKSETTING_MAX_SETTING_COUNT];
	TLStaticImage*		_thisWindow;
	TLVertScrollbar*	_scrollBar;
	TLStaticImage*		_skPanel;
	TLStaticImage*		_skInfo;

	TLButton*			_closeBtn;

private:
	KUiSKSetting();

	void			getChild();
	void			freshCmdKey();
	void			layoutCmdKeyPanel();

	int				getSelectCtrlIndex();


protected:
	virtual bool	onScroll(const EventArgs& arg);
	virtual bool	onWheelChanged(const EventArgs& arg);
	virtual bool	globalKeyDown(const EventArgs& e);
	virtual bool	onSaveBtnDown(const EventArgs& e);
	virtual bool	onDefaultSetting(const EventArgs& e);
	virtual bool	onCloseBtnDown(const EventArgs& e);
	virtual bool	onWindowShow(const EventArgs& e);
	virtual bool	onWindowHide(const EventArgs& e);

public:
	void	show();
	void	hide();
	void	toggle();
	void	clearOtherItem(const char* cmdName);
	std::string		getKeyName(Key::Scan key);
	~KUiSKSetting();
	static KUiSKSetting& getSinglton()
	{
		static KUiSKSetting singlton;
		return singlton;
	}
};

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

class KUiSKSettingMgr
{
public:
	struct KeyCmd
	{
		std::string Key;
		std::string CmdName;
		std::string Cmd;
		std::string Sys;
		KeyCmd(char* key, char* cmdName, char* cmd, char* sys)
		{
			Key = key;
			CmdName = cmdName;
			Cmd = cmd;
			Sys = sys;
		}
		KeyCmd()
		{
		}
	};

private:		
	std::vector<KeyCmd> _cmdList;

	void	loadSettings(const char* path);
	void	loadKey(const char* path);
	bool	save(const char* path);
	std::string getSKPath();

public:
	KUiSKSettingMgr();
	~KUiSKSettingMgr();
	void	loadUserSettings();
	void	loadDefalutSettings();
	void	save();
	bool	bindAKey(const char* cmdName, const char* key);
	bool	bindAKeyByCmd(const char* cmd, const char* key);
	std::string getCurValidSKPath();

	static KUiSKSettingMgr& getSinglton()
	{
		static KUiSKSettingMgr singlton;
		return singlton;
	}

	const std::vector<KeyCmd>& getSettings()
	{
		return _cmdList;
	}
};

#endif