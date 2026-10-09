
#include "UiGMCommunication.h"
#include "UiAutoConnect.h"
#include "Coreshell.h"
#include "../UiSheetMgr.h"
#include "../UiCommon.h"
#include "../KMessageCentre.h"
#include "UiErrorMessageBox.h"

extern iCoreShell* g_pCoreShell;

KUiGMCommunication::KUiGMCommunication()
{
	load();
}

KUiGMCommunication::~KUiGMCommunication()
{

}

KUiGMCommunication& KUiGMCommunication::getSingleton()
{
	static KUiGMCommunication singleton;
	return singleton;
}

void KUiGMCommunication::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_GM_COMMUNICATION_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_GM_COMMUNICATION);
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
	_thisWindow->hide();

	_yourMsgBox = (TLMultiLineEditbox*)_thisWindow->getChild("TaharezLook/GMCommunication/Msg");
	_gmMsgBox = (TLMultiLineEditbox*)_thisWindow->getChild("TaharezLook/GMCommunication/GMAnswer");
	_gmMsgBox->disable();
	
	_account = (TLStaticText*)_thisWindow->getChild("TaharezLook/GMCommunication/Account");
	_role = (TLStaticText*)_thisWindow->getChild("TaharezLook/GMCommunication/Role");
	_profession = (TLStaticText*)_thisWindow->getChild("TaharezLook/GMCommunication/Profession");
	_level = (TLStaticText*)_thisWindow->getChild("TaharezLook/GMCommunication/Level");
	_map = (TLStaticText*)_thisWindow->getChild("TaharezLook/GMCommunication/Map");
	
	_commitBtn = (TLButton*)_thisWindow->getChild("TaharezLook/GMCommunication/Commit");
	_commitBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiGMCommunication::commit, this));
	
	_newQuestionBtn = (TLButton*)_thisWindow->getChild("TaharezLook/GMCommunication/NewQuestion");
	_newQuestionBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiGMCommunication::newQuestion, this));

	TLButton* closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/GMCommunication/Close");
	closeBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiGMCommunication::close, this));

	TLButton* cancelBtn = (TLButton*)_thisWindow->getChild("TaharezLook/GMCommunication/Cancel");
	cancelBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiGMCommunication::close, this));

	showCommit();
}

void KUiGMCommunication::loadPlayerInfo()
{
	if(!_thisWindow)
	{
		return;
	}
	
	char* acountName = KUiAutoConnect::GetSingleton().GetUserName();
	_account->setText(AnsiToUtf8(acountName));

	KUiPlayerBaseInfo baseInfo;
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
	_role->setText(AnsiToUtf8(baseInfo.Name));

	KUiPlayerAttribute playerAttr;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&playerAttr, NULL );
	char professionType[COMMON_CLIENT_MSG_LEN_64];
	switch(playerAttr.nSeries)
	{
	case 0:
		strcpy( professionType, ROLE_CAREER_JS);
		break;
	case 1:
		strcpy( professionType, ROLE_CAREER_DS);
		break;
	case 2:
		strcpy( professionType, ROLE_CAREER_YR);
		break;
	default :
		strcpy( professionType, ROLE_CAREER_JS);
		break;
	}
	_profession->setText(AnsiToUtf8(professionType));

	_level->setText(CEGUI::PropertyHelper::intToString(playerAttr.nLevel));
	

	KUiSceneTimeInfo tagSceneMapTime = { 0 };
	
	g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&tagSceneMapTime, NULL );
	tagSceneMapTime.szSceneName[sizeof(tagSceneMapTime.szSceneName) - 1] = 0;
	
	_map->setText(AnsiToUtf8(tagSceneMapTime.szSceneName));
}

void KUiGMCommunication::show()
{
	if(_thisWindow)
	{
		_thisWindow->show();
		loadPlayerInfo();

		_thisWindow->activate();
		_yourMsgBox->activate();
		
		int maxEndIndex = 1000000;
		_yourMsgBox->setSelection(maxEndIndex, maxEndIndex);
	}
}

void KUiGMCommunication::hide()
{
	if(_thisWindow)
	{
		_thisWindow->hide();
		_yourMsgBox->deactivate();
	}
}

bool KUiGMCommunication::isVisible()
{
	if(_thisWindow)
	{
		return _thisWindow->isVisible();
	}

	return false;
}

void KUiGMCommunication::toggle()
{
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
		show();
	}
}

void KUiGMCommunication::setYourMsg(const char* text, bool canEdit /* = true */)
{
	if(!_thisWindow || NULL == text || _yourMsgBox == NULL)
	{
		return;
	}
	_yourMsgBox->setText(AnsiToUtf8(text));

	if (canEdit)
	{
		_yourMsgBox->setReadOnly(false);
	}
	else
	{
		_yourMsgBox->setReadOnly(true);
	}
}

void KUiGMCommunication::setGMMsg(const char* text)
{
	if(!_thisWindow)
	{
		return;
	}

	_gmMsgBox->setText(AnsiToUtf8(text));
}

void KUiGMCommunication::doCommit()
{
	if(!_thisWindow)
	{
		return;
	}

	String text = _yourMsgBox->getText();
	char msg[COMMON_CLIENT_MSG_LEN_256];
	strncpy(msg, Utf8ToAnsi(text), sizeof(msg));
	msg[sizeof(msg) - 1] = 0;
	if(strlen(msg) < UI_GM_COMMUNICATION_MIN_TEXT_LEN)
	{	
		char* message = KMessageCentre::GetMessage(common_message, UI_GM_ERROR_MESSAGE_ID);
		KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
		return;
	}
	g_pCoreShell->OperationRequest(GOI_SEND_GM_QUESTION, (UINT)msg, NULL);
}

bool KUiGMCommunication::commit(const EventArgs& args)
{
	doCommit();
	
	hide();
	return true;
}

void KUiGMCommunication::showCommit()
{
	if(!_thisWindow)
	{
		return;
	}

	_yourMsgBox->enable();
	_commitBtn->show();
	_newQuestionBtn->hide();
}

void KUiGMCommunication::showNewQuestion()
{
	if(!_thisWindow)
	{
		return;
	}
	
	_yourMsgBox->disable();
	_newQuestionBtn->show();
	_commitBtn->hide();
}

bool KUiGMCommunication::newQuestion(const EventArgs& args)
{
	showCommit();
	_yourMsgBox->setText("");
	_yourMsgBox->activate();
	return true;
}

bool KUiGMCommunication::close(const EventArgs& args)
{
	hide();
	return true;
}
