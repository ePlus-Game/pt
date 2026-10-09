
#include "UiErrorMessageBox.h"
#include "../UiSheetMgr.h"
#include "UiChatWindow.h"
#include "chatWindow/ChatMainDlg.h"

using namespace CHAT;

KUiErrorMessageBox::KUiErrorMessageBox()
{
	loadUi();
}

KUiErrorMessageBox::~KUiErrorMessageBox()
{
}

KUiErrorMessageBox& KUiErrorMessageBox::GetSingleton()
{
	static KUiErrorMessageBox singleton;
	return singleton;
}

void KUiErrorMessageBox::toSystemMessage(String message)
{
	if(!_thisWindow)
	{
		return;
	}

	const char* tempText = Utf8ToAnsi(message);
	string msgstr = tempText;
	
	ChatMainDlg::InsertSystemMsg(msgstr.c_str());

	char buf[sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG];
	PCHATROOMMSG_TO_SOMEONE	msgData = (PCHATROOMMSG_TO_SOMEONE)buf;

	msgData->senderName[0] = 0;
	msgData->msgLen = strlen(msgstr.c_str());
	
	memcpy(msgData->msg, msgstr.c_str(), msgData->msgLen);
	msgData->msg[msgData->msgLen] = '\0';
	KUiChannelCentre::GetSingleton().recvMessage(SYSTEM_ROOM_ID, (BYTE*)buf);
}

void KUiErrorMessageBox::AddMessage(String message)
{
	if(!_thisWindow)
	{
		return;
	}

	for(int i = _messageCount - 1; i > 0; i--)
	{
		if(d_messages[i - 1]->isVisible() == false)
		{
			continue;
		}
		else
		{
			float leftClostTime = d_messages[i - 1]->getAutoCloseTime();
			String newString = d_messages[i - 1]->getText();
			d_messages[i]->setText(newString);
			d_messages[i]->StopFiding();
			float curAlpha = d_messages[i - 1]->getAlpha();
			if(leftClostTime <= 0.0f)
			{
				d_messages[i]->show();
				d_messages[i]->setAlpha(curAlpha);
				d_messages[i]->CloseBox();
			}
			else
			{
				d_messages[i]->show();
				d_messages[i]->setAlpha(1.0f);
				d_messages[i]->setAutoCloseTime(leftClostTime);
			}
		}
	}
	d_messages[0]->setText(message);
	d_messages[0]->setAlpha(1.0f);
	d_messages[0]->show();
	d_messages[0]->StopFiding();
	d_messages[0]->setAutoCloseTime(d_autoCloseTime);

	_thisWindow->getRoot()->addChildWindow(_thisWindow);
	show();
}

void KUiErrorMessageBox::show()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
}

void KUiErrorMessageBox::hide()
{
	if(!_thisWindow)
	{
		return;
	}
	
	_thisWindow->hide();
}

void KUiErrorMessageBox::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_ERROR_MSG_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_ERROR_MSG_WINDOW_PATH);
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
	
	_thisWindow->setRenderMode(true);
	_thisWindow->setZLevel(Window::SuperTop);
	
	_messageCount = _thisWindow->getChildCount();
	_messageCount = _messageCount > UI_ERROR_MSG_MAX_MESSAGE_COUNT ? UI_ERROR_MSG_MAX_MESSAGE_COUNT : _messageCount;
	
	for(int i = 0; i < _messageCount; i++)
	{
		d_messages[i] = (TLStaticText*)_thisWindow->getChild(
			"TaharezLook/ErrorMessageBox/Message" + iToString(i + 1));
		
		StaticText* msgCtrl = d_messages[i];
		msgCtrl->hide();
		msgCtrl->subscribeEvent(StaticText::EventHidden, 
			Event::Subscriber(&KUiErrorMessageBox::onHide, this));
		msgCtrl->subscribeEvent(StaticText::EventShown, 
			Event::Subscriber(&KUiErrorMessageBox::onShow, this));
	}
	d_autoCloseTime = d_messages[0]->getAutoCloseTime();
	
	_thisWindow->disable();
	System::getSingleton().subscribeEvent(Window::EventGUISheetChanged,
		Event::Subscriber(&KUiErrorMessageBox::onSheetChanged, this));
}

bool KUiErrorMessageBox::onHide(const CEGUI::EventArgs& e)
{
	((WindowEventArgs*)&e)->window->stopUpdate();
	return true;
}

bool KUiErrorMessageBox::onShow(const CEGUI::EventArgs& e)
{
	((WindowEventArgs*)&e)->window->beginUpdate();
	return true;
}

bool KUiErrorMessageBox::onSheetChanged(const EventArgs& e)
{
	System::getSingleton().getGUISheet()->addChildWindow(_thisWindow);
	for(int i = 0; i < _messageCount; ++i)
	{
		if(d_messages[i]->isNeddUpdate())
		{
			d_messages[i]->beginUpdate();
		}
	}
	return true;
}