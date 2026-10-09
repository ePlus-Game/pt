//xiehong 2008-3-17 GMÃæ°å


#ifndef UI_GM_COMMUNICATION_H
#define UI_GM_COMMUNICATION_H

#define UI_GM_COMMUNICATION_1024	"uisettings/layouts1024/GMCommunication.ls"
#define UI_GM_COMMUNICATION			"uisettings/layouts/GMCommunication.ls"
#define UI_GM_COMMUNICATION_MIN_TEXT_LEN 32

#define UI_GM_ERROR_MESSAGE_ID 103

#include "TLStatic.h"
#include "TLButton.h"
#include "TLMultiLineEditbox.h"

class KUiGMCommunication
{
	TLStaticImage*		_thisWindow;
	TLMultiLineEditbox*	_yourMsgBox;
	TLMultiLineEditbox*	_gmMsgBox;

	TLStaticText*		_account;
	TLStaticText*		_role;
	TLStaticText*		_profession;
	TLStaticText*		_level;
	TLStaticText*		_map;

	TLButton*			_commitBtn;
	TLButton*			_newQuestionBtn;
private:
	void	load();
	void	loadPlayerInfo();

	void	doCommit();

	bool	commit(const EventArgs& args);
	bool	newQuestion(const EventArgs& args);
	bool	close(const EventArgs& args);
	bool	keyDown(const EventArgs& args);

public:
	KUiGMCommunication();
	~KUiGMCommunication();

	static KUiGMCommunication& getSingleton();

	void	setYourMsg(const char* text, bool canEdit = true);
	void	setGMMsg(const char* text);
	
	void	showCommit();
	void	showNewQuestion();

	void	show();
	void	hide();
	bool	isVisible();
	void	toggle();
};

#endif