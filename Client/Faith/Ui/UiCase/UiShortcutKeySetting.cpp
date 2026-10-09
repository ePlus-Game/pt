

#include <fstream>
#include <strstream>

#include "UiShortcutkeySetting.h"
#include "UiChatWindow.h"
#include "CoreUseNameDef.h"
#include "../shortcutKey.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"
#include "UiAutoConnect.h"
#include "UiShortcutWnd.h"
#include "../KMessageCentre.h"

KUiSKSettingItem::KUiSKSettingItem()
{
	getChild();
}

KUiSKSettingItem::~KUiSKSettingItem()
{
	
}

void KUiSKSettingItem::getChild()
{
#ifndef _DEBUG
	try
	{
#endif
		static int i = 0;
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SKSETTING_ITEM_WINDOW_NAME, iToString(i++));
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

//	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	
	_cmdName = (TLStaticImage*)_thisWindow->getChild(_thisWindow->getName() + "/CmdName");

	_keyName = (TLRadioButton*)_thisWindow->getChild(_thisWindow->getName() + "/KeyName");
	_keyName->subscribeEvent(TLStaticImage::EventMouseClick, Event::Subscriber(&KUiSKSettingItem::onKeyBtnDown, this));
}

Point KUiSKSettingItem::getPos()
{
	if(!_thisWindow)
	{
		return Point(0, 0);
	}
	
	return _thisWindow->getPosition(Absolute);
}

void KUiSKSettingItem::setPos(const Point& pos)
{	
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->setPosition(Absolute, pos);
}

int KUiSKSettingItem::getHeight()
{
	if(!_thisWindow)
	{
		return 0;
	}

	return _thisWindow->getHeight(Absolute);
}

int KUiSKSettingItem::getWidth()
{
	if(!_thisWindow)
	{
		return 0;
	}

	return _thisWindow->getWidth(Absolute);
}

void KUiSKSettingItem::setType(bool isTitle)
{
	if(!_thisWindow)
	{
		return;
	}

	_isTitle = isTitle;
	static int oldOffset = _cmdName->getXPosition(Absolute);

	if(_isTitle)
	{
		_cmdName->setXPosition(Absolute, 0);
		_keyName->hide();
	}
	else
	{
		_cmdName->setXPosition(Absolute, oldOffset);
		_keyName->show();
	}
}

bool KUiSKSettingItem::isTitle()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _isTitle;
}

bool KUiSKSettingItem::isSelected()
{
	if(!_thisWindow)
	{
		return false;
	}

	if(_isTitle || !_thisWindow->isVisible())
	{
		return false;
	}

	return _keyName->isSelected();
}

bool KUiSKSettingItem::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}

std::string KUiSKSettingItem::getCmdName()
{
	return _cmdNameText;
}

void KUiSKSettingItem::setParent(Window* parent)
{
	if(!_thisWindow)
	{
		return;
	}

	if(NULL == parent)
	{
		return;
	}
	
	parent->addChildWindow(_thisWindow);
}

void KUiSKSettingItem::show(const char* cmdName, const char* keyName)
{
	if(!_thisWindow)
	{
		return;
	}

	_cmdNameText = cmdName;
	
	_cmdName->setText(AnsiToUtf8(cmdName));
	_keyName->setText(AnsiToUtf8(keyName));

	_thisWindow->show();

	if(!strcmp(keyName, "Title"))
	{
		setType(true);
	}
	else
	{
		setType(false);
	}
}

void KUiSKSettingItem::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

void KUiSKSettingItem::setSelected(bool selectState)
{
	if(!_thisWindow)
	{
		return;
	}

	_keyName->setSelected(selectState);
}

bool KUiSKSettingItem::onKeyBtnDown(const EventArgs& e)
{
	KUiSKSetting::getSinglton().clearOtherItem(Utf8ToAnsi(_cmdName->getText()));
	return true;
}

/*============================================================*/
/*============================================================*/
/*============================================================*/
/*============================================================*/

KUiSKSetting::KUiSKSetting()
{
	getChild();
}

KUiSKSetting::~KUiSKSetting()
{
	
}

void KUiSKSetting::getChild()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SKSETTING_WINDOW_NAME_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SKSETTING_WINDOW_NAME);
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
	
	//保存按钮和默认设置按钮
	TLButton* saveBtn = (TLButton*)_thisWindow->getChild("TaharezLook/SKSetting/Save");
	saveBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSKSetting::onSaveBtnDown, this));

	TLButton* defaultSetting = (TLButton*)_thisWindow->getChild("TaharezLook/SKSetting/DefautSetting");
	defaultSetting->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSKSetting::onDefaultSetting, this));
	
	//全局键盘消息
	GlobalEventSet::getSingleton().subscribeEvent(Window::EventKeyDown, 
		Event::Subscriber(&KUiSKSetting::globalKeyDown, this));

	//快捷键面板
	_skInfo = (TLStaticImage*)_thisWindow->getChild("TaharezLook/SKSetting/SKInfo");	
	_skInfo->subscribeEvent(TLVertScrollbar::EventMouseWheel, Event::Subscriber(&KUiSKSetting::onWheelChanged, this));
	_skPanel = (TLStaticImage*)_skInfo->getChild("TaharezLook/SKSetting/SKInfo/SKPanel");
	for(int i = 0; i < UI_SKSETTING_MAX_SETTING_COUNT; ++i)
	{
		_cmdCtrl[i].setParent(_skPanel);
	}

	//滑动条
	_scrollBar = (TLVertScrollbar*)_skInfo->getChild("TaharezLook/SKSetting/SKInfo/Scroll");
	_scrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiSKSetting::onScroll, this));
	
	//关闭按钮
	_closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/SKSetting/Close");
	_closeBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSKSetting::onCloseBtnDown, this));
}

void KUiSKSetting::freshCmdKey()
{
	if(!_thisWindow)
	{
		return;
	}

	const std::vector<KUiSKSettingMgr::KeyCmd>& cmdList = KUiSKSettingMgr::getSinglton().getSettings();
	if(cmdList.size() > UI_SKSETTING_MAX_SETTING_COUNT)
	{
		return;
	}

	for(int i = 0; i < cmdList.size(); ++i)
	{
		if(cmdList[i].CmdName != "" && cmdList[i].Sys == "false")
		{
			_cmdCtrl[i].show(cmdList[i].CmdName.c_str(), cmdList[i].Key.c_str());
		}
		else
		{
			_cmdCtrl[i].hide();
		}
		_cmdCtrl[i].setSelected(false);
	}

	for(int j = cmdList.size(); j < UI_SKSETTING_MAX_SETTING_COUNT; ++j)
	{
		_cmdCtrl[j].hide();
	}
}

void KUiSKSetting::layoutCmdKeyPanel()
{
	if(!_thisWindow)
	{
		return;
	}

	int itemHeight = _cmdCtrl[0].getHeight();

	const std::vector<KUiSKSettingMgr::KeyCmd>& cmdList = KUiSKSettingMgr::getSinglton().getSettings();
	if(cmdList.size() > UI_SKSETTING_MAX_SETTING_COUNT)
	{
		return;
	}

	Point pos(0, 0);
	int panelHeight = 0;
	for(int i = 0; i < cmdList.size(); ++i)
	{
		if(!_cmdCtrl[i].isVisible())
		{
			continue;
		}
		_cmdCtrl[i].setPos(pos);
		pos.d_y += itemHeight;
		if(i < cmdList.size())
		{
			panelHeight += _cmdCtrl[i].getHeight();
		}
	}

	_skPanel->setHeight(Absolute, panelHeight);
	
	if(_skInfo->getHeight(Absolute) < panelHeight)
	{
		_scrollBar->setStepSize((float)itemHeight / (panelHeight - _skInfo->getHeight(Absolute)));
		_scrollBar->show();
	}
	else
	{
		_scrollBar->hide();
	}
}

int KUiSKSetting::getSelectCtrlIndex()
{
	if(!_thisWindow)
	{
		return 0;
	}

	for(int i = 0; i < UI_SKSETTING_MAX_SETTING_COUNT; ++i)
	{
		if(_cmdCtrl[i].isSelected())
		{
			return i;
		}
	}
	return UI_SKSETTING_INVALID_INDEX_ID;
}
/*CEGUI消息处理********************begin***************************************/


bool KUiSKSetting::globalKeyDown(const EventArgs& e)
{	
	if(!_thisWindow->isVisible())
	{
		return false;
	}

	const KeyEventArgs& key = static_cast<const KeyEventArgs&>(e);
	
	int curSelectCtrlIndex = getSelectCtrlIndex();
	if(UI_SKSETTING_INVALID_INDEX_ID == curSelectCtrlIndex)
	{
		return true;
	}

	std::string keyName = getKeyName(key.scancode);
	if(keyName == "")
	{
		return true;
	}

	std::string sysKeyName = "";
	if(key.sysKeys & Shift)
	{
		sysKeyName = "Shift+";
	}
	else if(key.sysKeys & Control)
	{
		sysKeyName = "Ctrl+";
	}
	else if(key.sysKeys & Alt)
	{
		sysKeyName = "Alt+";
	}
	else
	{
		sysKeyName = "";
	}

	keyName = sysKeyName + keyName;

	const char* curSelectCmdName = _cmdCtrl[curSelectCtrlIndex].getCmdName().c_str();
	if(KUiSKSettingMgr::getSinglton().bindAKey(curSelectCmdName, keyName.c_str()))
	{
		freshCmdKey();
		KUiChannelCentre::GetSingleton().toSysMsg(KUiCfgLoader::getSingleton().getSkCfg().successMsg);
	}
	else
	{
		static char failMsg[COMMON_CLIENT_MSG_LEN_128];
		sprintf(failMsg, KUiCfgLoader::getSingleton().getSkCfg().failMsg, curSelectCmdName, keyName.c_str());
		KUiChannelCentre::GetSingleton().toSysMsg(failMsg);
	}

	return true;
}

bool KUiSKSetting::onScroll(const EventArgs& arg)
{
	float scrollPos = _scrollBar->getScrollPosition();
	int panelHeight = _skPanel->getWindowArea().getHeight().asAbsolute(0);
	
	if(panelHeight < _skInfo->getHeight(Absolute))
	{
		return false;
	}

	int exceedSize = (panelHeight - _skInfo->getHeight(Absolute)) * scrollPos;
	_skPanel->setYPosition(Absolute, -exceedSize);
	return true;	
}

bool KUiSKSetting::onWheelChanged(const EventArgs& arg)
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&arg;
	
	if(_scrollBar->isVisible())
	{
		_scrollBar->setScrollPosition(_scrollBar->getScrollPosition()
			- _scrollBar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiSKSetting::onSaveBtnDown(const EventArgs& e)
{
	KUiSKSettingMgr::getSinglton().save();
	string scriptPath = KUiSKSettingMgr::getSinglton().getCurValidSKPath();
	KShortcutKeyCentre::LoadScript(const_cast<char*>(scriptPath.c_str()));
	hide();
	return true;
}

bool KUiSKSetting::onCloseBtnDown(const EventArgs& e)
{
	hide();
	return true;
}

bool KUiSKSetting::onWindowShow(const EventArgs& e)
{
	_thisWindow->setModalState(true);
	return true;
}

bool KUiSKSetting::onWindowHide(const EventArgs& e)
{
	_thisWindow->setModalState(false);
	return true;
}

bool KUiSKSetting::onDefaultSetting(const EventArgs& e)
{
	KUiSKSettingMgr::getSinglton().loadDefalutSettings();
	_scrollBar->setScrollPosition(0.0f);
	freshCmdKey();
	layoutCmdKeyPanel();
	return true;
}

void KUiSKSetting::clearOtherItem(const char* cmdName)
{
	if(!_thisWindow)
	{
		return;
	}

 	for(int i = 0; i < UI_SKSETTING_MAX_SETTING_COUNT; ++i)
 	{
 		if(strcmp(_cmdCtrl[i].getCmdName().c_str(), cmdName))
		{
			_cmdCtrl[i].setSelected(false);
		}
 	}
}

void KUiSKSetting::show()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
 	System::getSingleton().getGUISheet()->removeChildWindow(_thisWindow);
 	System::getSingleton().getGUISheet()->addChildWindow(_thisWindow);

	KUiSKSettingMgr::getSinglton().loadUserSettings();
	freshCmdKey();
	layoutCmdKeyPanel();
}

void KUiSKSetting::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

void KUiSKSetting::toggle()
{
	if(!_thisWindow)
	{
		return;
	}

	if(_thisWindow->isVisible())
	{
		hide();
	}
	else
	{
		show();
	}
}

std::string KUiSKSetting::getKeyName(Key::Scan key)
{
	std::string keyName = "";
	switch(key)
	{
	case Key::Escape:
		{
			keyName = "ESC";
		}
		break;
	case Key::One:
		{
			keyName = "1";
		}
		break;
	case Key::Two:
		{
			keyName = "2";
		}
		break;
	case Key::Three:
		{
			keyName = "3";
		}
		break;
	case Key::Four:
		{
			keyName = "4";
		}
		break;
	case Key::Five:
		{
			keyName = "5";
		}
		break;
	case Key::Six:
		{
			keyName = "6";
		}
		break;
	case Key::Seven:
		{
			keyName = "7";
		}
		break;
	case Key::Eight:
		{
			keyName = "8";
		}
		break;
	case Key::Nine:
		{
			keyName = "9";
		}
		break;
	case Key::Zero:
		{
			keyName = "0";
		}
		break;
	case Key::Minus:
		{
			keyName = "-";
		}
		break;
	case Key::Equals:
		{
			keyName = "=";
		}
		break;
	case Key::Backspace:
		{
			keyName = "Backspace";
		}
		break;
	case Key::Tab:
		{
			keyName = "Tab";
		}
		break;
	case Key::Q:
		{
			keyName = "Q";
		}
		break;
	case Key::W:
		{
			keyName = "W";
		}
		break;
	case Key::E:
		{
			keyName = "E";
		}
		break;
	case Key::R:
		{
			keyName = "R";
		}
		break;
	case Key::T:
		{
			keyName = "T";
		}
		break;
	case Key::Y:
		{
			keyName = "Y";
		}
		break;
	case Key::U:
		{
			keyName = "U";
		}
		break;
	case Key::I:
		{
			keyName = "I";
		}
		break;
	case Key::O:
		{
			keyName = "O";
		}
		break;
	case Key::P:
		{
			keyName = "P";
		}
		break;
	case Key::LeftBracket:
		{
			keyName = "[";
		}
		break;
	case Key::RightBracket:
		{
			keyName = "]";
		}
		break;
	case Key::Return:
		{
			keyName = "Enter";
		}
		break;
// 	case Key::LeftControl:
// 		{
// 			keyName = "Ctrl";
// 		}
// 		break;
	case Key::A:
		{
			keyName = "A";
		}
		break;
	case Key::S:
		{
			keyName = "S";
		}
		break;
	case Key::D:
		{
			keyName = "D";
		}
		break;
	case Key::F:
		{
			keyName = "F";
		}
		break;
	case Key::G:
		{
			keyName = "G";
		}
		break;
	case Key::H:
		{
			keyName = "H";
		}
		break;
	case Key::J:
		{
			keyName = "J";
		}
		break;
	case Key::K:
		{
			keyName = "K";
		}
		break;
	case Key::L:
		{
			keyName = "L";
		}
		break;
	case Key::Semicolon:
		{
			keyName = ";";
		}
		break;
	case Key::Apostrophe:
		{
			keyName = "'";
		}
		break;		
// 	case Key::Grave:
// 		{
// 			keyName = "Tab";
// 		}
// 		break;
// 	case Key::LeftShift:
// 		{
// 			keyName = "Shift";
// 		}
// 		break;
 	case Key::Backslash:
 		{
 			keyName = "\\";
 		}
 		break;
	case Key::Z:
		{
			keyName = "Z";
		}
		break;
	case Key::X:
		{
			keyName = "X";
		}
		break;
	case Key::C:
		{
			keyName = "C";
		}
		break;
	case Key::V:
		{
			keyName = "V";
		}
		break;
	case Key::B:
		{
			keyName = "B";
		}
		break;
	case Key::N:
		{
			keyName = "N";
		}
		break;
	case Key::M:
		{
			keyName = "M";
		}
		break;
	case Key::Comma:
		{
			keyName = ",";
		}
		break;
	case Key::Period:
		{
			keyName = ".";
		}
		break;
	case Key::Slash:
		{
			keyName = "/";
		}
		break;
// 	case Key::RightShift:
// 		{
// 			keyName = "RShift";
// 		}
// 		break;
// 	case Key::Multiply:
// 		{
// 			keyName = "Tab";
// 		}
// 		break;
// 	case Key::LeftAlt:
// 		{
// 			keyName = "Alt";
// 		}
// 		break;
	case Key::Space:
		{
			keyName = "Space";
		}
		break;
// 	case Key::Capital:
// 		{
// 			keyName = "Tab";
// 		}
// 		break;
	case Key::F1:
		{
			keyName = "F1";
		}
		break;
	case Key::F2:
		{
			keyName = "F2";
		}
		break;
	case Key::F3:
		{
			keyName = "F3";
		}
		break;
	case Key::F4:
		{
			keyName = "F4";
		}
		break;
	case Key::F5:
		{
			keyName = "F5";
		}
		break;
	case Key::F6:
		{
			keyName = "F6";
		}
		break;
	case Key::F7:
		{
			keyName = "F7";
		}
		break;
	case Key::F8:
		{
			keyName = "F8";
		}
		break;
	case Key::F9:
		{
			keyName = "F9";
		}
		break;
	case Key::F10: 
		{
			keyName = "F10";
		}
		break;
	case Key::NumLock:
		{
			keyName = "NumLock";
		}
		break;
	case Key::ScrollLock:
		{
			keyName = "ScrollLock";
		}
		break;
	case Key::Numpad7:
		{
			keyName = "Numpad7";
		}
		break;
	case Key::Numpad8:
		{
			keyName = "Numpad8";
		}
		break;
	case Key::Numpad9:
		{
			keyName = "Numpad9";
		}
		break;
	case Key::Subtract:
		{
			keyName = "-";
		}
		break;
	case Key::Numpad4:
		{
			keyName = "Numpad4";
		}
		break;
	case Key::Numpad5:
		{
			keyName = "Numpad5";
		}
		break;
	case Key::Numpad6:
		{
			keyName = "Numpad6";
		}
		break;
	case Key::Add:	
		{
			keyName = "+";
		}
		break;			
	case Key::Numpad1:
		{
			keyName = "Numpad1";
		}
		break;
	case Key::Numpad2:
		{
			keyName = "Numpad2";
		}
		break;
	 case Key::Numpad3:
		{
			keyName = "Numpad3";
		}
		break;
	 case Key::Numpad0:
		{
			keyName = "Numpad0";
		}
		break;
	 case Key::Decimal:
		{
			keyName = ".";
		}
		break;			
/*	 case Key::OEM_102 */
	 case Key::F11:
		{
			keyName = "F11";
		}
		break;
	 case Key::F12:
		{
			keyName = "F12";
		}
		break;
	 case Key::F13:
		{
			keyName = "F13";
		}
		break;
	 case Key::F14:
		{
			keyName = "F14";
		}
		break;
	 case Key::F15:
		{
			keyName = "F15";
		}
		break;
// 	 Kana
// 	 ABNT_C1 
// 	 Convert 
// 	 NoConvert 
// 	 Yen 
// 	 ABNT_C2 
// 	 NumpadEquals
// 	 PrevTrack 
// 	 At
// 	 Colon 
// 	 Underline 
// 	 Kanji 
// 	 Stop
// 	 AX
// 	 Unlabeled 
// 	 NextTrack 
// 	 NumpadEnter 
// 	 RightControl
// 	 Mute
// 	 Calculator
// 	 PlayPause 
// 	 MediaStop 
// 	 VolumeDown
// 	 VolumeUp
// 	 WebHome 
// NumpadComma 
// Divide
// SysRq 
// RightAlt
// Pause 
// Home
// ArrowUp 
// PageUp
// ArrowLeft 
// ArrowRight
// End 
// ArrowDown 
// PageDown		
// Insert
// Delete
// LeftWindows 
// RightWindow 
// RightWindows
// AppMenu 
// Power 
// Sleep 
// Wake			
// WebSearch		
// WebFavorites	
// WebRefresh		
// WebStop			
// WebForward		
// WebBack			
// MyComputer		
// Mail			
// MediaSelect
	}
	return keyName;
}
/*========================================================================================*/
/*========================================================================================*/
/*========================================================================================*/
/*========================================================================================*/
/*========================================================================================*/


KUiSKSettingMgr::KUiSKSettingMgr()
{
	loadUserSettings();
}

KUiSKSettingMgr::~KUiSKSettingMgr()
{

}

void KUiSKSettingMgr::loadSettings(const char* path)
{
	_cmdList.clear();
	
	KPakFile dataFile;
	BOOL bOk = dataFile.Open(path);
	if(bOk != TRUE)
	{
		return;
	}

	int size = dataFile.Size();
	if(size <= 0)
	{
		dataFile.Close();
		return;
	}

	char buffer[UI_SKSETTING_MAX_FILE_SIZE];
	memset(buffer, 0, UI_SKSETTING_MAX_FILE_SIZE);
	
	dataFile.Read(buffer, UI_SKSETTING_MAX_FILE_SIZE);
	dataFile.Close();
	buffer[UI_SKSETTING_MAX_FILE_SIZE - 1] = 0;

	std::strstream data;
	data<<buffer;

	char aLine[COMMON_CLIENT_MSG_LEN_512];
	while(data.getline(aLine, COMMON_CLIENT_MSG_LEN_512 - 1))
	{
		aLine[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
		std::string strLine = aLine;
		
		KUiSKSettingMgr::KeyCmd keyCmd;
		int startIndex = strLine.find('\"');
		int endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.Key = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.CmdName = strLine.substr(startIndex + 1, endIndex - startIndex - 1);
		
		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.Cmd = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.Sys = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		if ( !keyCmd.Sys.empty() && 
			( keyCmd.Sys == "true" || 
			keyCmd.Sys == "TRUE" || 
			keyCmd.Sys == "True" ) )
		{
			keyCmd.Sys = "true";
		}
		else
		{
			keyCmd.Sys = "false";
		}

		
		_cmdList.push_back(keyCmd);
	}
}

void KUiSKSettingMgr::loadKey(const char* path)
{
	KUiShortcutWnd::GetSingleton().ClearAllShortcutKey();
	KUiLRSkillWnd::GetSingleton().ClearAllShortcutKey();
	KPakFile dataFile;
	BOOL bOk = dataFile.Open(path);
	if(bOk != TRUE)
	{
		return;
	}

	int size = dataFile.Size();
	if(size <= 0)
	{
		dataFile.Close();
		return;
	}

	char buffer[UI_SKSETTING_MAX_FILE_SIZE];
	memset(buffer, 0, UI_SKSETTING_MAX_FILE_SIZE);

	dataFile.Read(buffer, UI_SKSETTING_MAX_FILE_SIZE);
	dataFile.Close();
	buffer[UI_SKSETTING_MAX_FILE_SIZE - 1] = 0;

	std::strstream data;
	data<<buffer;

	char aLine[COMMON_CLIENT_MSG_LEN_512];
	while(data.getline(aLine, COMMON_CLIENT_MSG_LEN_512 - 1))
	{
		aLine[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
		std::string strLine = aLine;
		
		KUiSKSettingMgr::KeyCmd keyCmd;
		int startIndex = strLine.find('\"');
		int endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.Key = strLine.substr(startIndex + 1, endIndex - startIndex - 1);

		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.CmdName = strLine.substr(startIndex + 1, endIndex - startIndex - 1);
		
		startIndex = strLine.find('\"', endIndex + 1);
		endIndex = strLine.find('\"', startIndex + 1);
		keyCmd.Cmd = strLine.substr(startIndex + 1, endIndex - startIndex - 1);
		
		for(int i = 0; i < _cmdList.size(); ++i)
		{
			if(_cmdList[i].Cmd == keyCmd.Cmd && _cmdList[i].Sys == "false")
			{
				_cmdList[i].Key = keyCmd.Key;
				if ( _cmdList[i].Cmd.find("SetLSkill") != string::npos )
				{
					char szBuff[COMMON_CLIENT_MSG_LEN_64];
					sscanf( _cmdList[i].Cmd.c_str(), "SetLSkill('%[^']')", szBuff );
					KUiLRSkillWnd::GetSingleton().AddShortcutKey( true, szBuff, _cmdList[i].Key.c_str() );
				}
				else if ( _cmdList[i].Cmd.find("SetRSkill") != string::npos ) 
				{
					char szBuff[COMMON_CLIENT_MSG_LEN_64];
					sscanf( _cmdList[i].Cmd.c_str(), "SetRSkill('%[^']')", szBuff );
					KUiLRSkillWnd::GetSingleton().AddShortcutKey( false, szBuff, _cmdList[i].Key.c_str() );
				}
				else if ( _cmdList[i].Cmd.find("ShortcutUse" ) != string::npos )
				{
					int nShortcutIdx = 0;
					sscanf( _cmdList[i].Cmd.c_str(), "ShortcutUse(%d)", &nShortcutIdx );
					KUiShortcutWnd::GetSingleton().AddShortcutKey( nShortcutIdx, _cmdList[i].Key.c_str() );
				}				
			}
		}
	}

	KUiShortcutWnd::GetSingleton().UpdateData();
}

void KUiSKSettingMgr::loadUserSettings()
{
	string loadPath = getSKPath();

	std::fstream file;
	file.open(loadPath.c_str(), ios::in);

	if(!file)
	{
		loadDefalutSettings();
		save();
	}
	else
	{
		loadDefalutSettings();
		loadKey(loadPath.c_str());
	}
	file.close();
}

void KUiSKSettingMgr::loadDefalutSettings()
{
	loadSettings(UI_SHORTCUT_KEY_DEFAULT_FILE_NAME);
}

bool KUiSKSettingMgr::bindAKey(const char* cmdName, const char* key)
{
	bool binded = false;

	for(int j = 0; j < _cmdList.size(); ++j)
	{
		if(!strcmp(cmdName, _cmdList[j].CmdName.c_str()))
		{
			if(_cmdList[j].Sys == "false")
			{
				_cmdList[j].Key = key;
				binded = true;
				break;
			}
			else//如果该命令绑定的是系统快捷键，就不能绑定了
			{
				return false;
			}
		}
	}

	if(!binded)
	{
		return false;
	}

	for(int i = 0; i < _cmdList.size(); ++i)
	{
		if(!strcmp(key, _cmdList[i].Key.c_str()) && strcmp(cmdName, _cmdList[i].CmdName.c_str()))
		{
			char tempText[COMMON_CLIENT_MSG_LEN_128];
			sprintf(tempText, KUiCfgLoader::getSingleton().getSkCfg().keyUnbindMsg, cmdName);
			KUiChannelCentre::GetSingleton().toSysMsg(tempText);
			_cmdList[i].Key = "--";
		}
	}

	return true;
}

bool KUiSKSettingMgr::bindAKeyByCmd(const char* cmd, const char* key)
{
	bool binded = false;

	for(int j = 0; j < _cmdList.size(); ++j)
	{
		if(!strcmp(cmd, _cmdList[j].Cmd.c_str()))
		{
			if(_cmdList[j].Sys == "false")
			{
				_cmdList[j].Key = key;
				binded = true;
				break;
			}
			else//如果该命令绑定的是系统快捷键，就不能绑定了
			{
				return false;
			}
		}
	}

	if(!binded)
	{
		return false;
	}

	for(int i = 0; i < _cmdList.size(); ++i)
	{
		if(!strcmp(key, _cmdList[i].Key.c_str()) && strcmp(cmd, _cmdList[i].Cmd.c_str()))
		{
			_cmdList[i].Key = "--";
		}
	}

	return true;
}

bool KUiSKSettingMgr::save(const char* path)
{
	fstream file;
	file.open(path, ios::binary|ios::out);
	
	for(int i = 0; i < _cmdList.size(); ++i)
	{
		if(!strcmp(_cmdList[i].Key.c_str(), "Title"))
		{
			file<<"--\"Title\",\""<<_cmdList[i].CmdName.c_str()<<"\"";
		}
		else
		{
			if ( _cmdList[i].Sys.empty())
			{
				file<<"AddCommand(\""<<_cmdList[i].Key.c_str()<<"\", \""<<_cmdList[i].CmdName.c_str()<<"\", \""<<_cmdList[i].Cmd.c_str()<<"\")";
			}
			else
			{
				file<<"AddCommand(\""<<_cmdList[i].Key.c_str()<<"\", \""<<_cmdList[i].CmdName.c_str()<<"\", \""<<_cmdList[i].Cmd.c_str()<<"\", \""<<_cmdList[i].Sys.c_str()<<"\")";
			}
			
		}
		if(i != _cmdList.size() - 1)
		{
			file<<endl;
		}
	}
	file.close();

	return true;
}

void KUiSKSettingMgr::save()
{
	std::string savePath = getSKPath();
	save(savePath.c_str());

	string loadPath = getSKPath();
	loadDefalutSettings();
	loadKey(loadPath.c_str());
}

std::string KUiSKSettingMgr::getSKPath()
{
	char path[COMMON_CLIENT_MSG_LEN_128];
	sprintf(path, UI_SHORTCUT_KEY_FILE_NAME, KUiAutoConnect::GetSingleton().GetUserName(), KUiAutoConnect::GetSingleton().GetRoleName());
	return path;
}

std::string KUiSKSettingMgr::getCurValidSKPath()
{	
	string userSKPath = getSKPath();

	std::fstream file;
	file.open(userSKPath.c_str(), ios::in);
	if(!file)
	{
		return UI_SHORTCUT_KEY_DEFAULT_FILE_NAME;
	}
	else
	{
		return userSKPath;
	}
}