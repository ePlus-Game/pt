
#ifndef UI_FSBIBLE_SPECIAL_QUEST_ITEM_H
#define UI_FSBIBLE_SPECIAL_QUEST_ITEM_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "TLStatic.h"

#define UI_FSBIBLE_SPECIAL_QUEST_ITEM_PATH "uisettings/layouts/FSBible_SpecialQuestItem.ls"

using namespace std;

class KUiFSBibleSpecialQuestItem
{
	friend class KUiFSBible;

	TLStaticImage*	_thisWindow;

	TLStaticImage*	_doubleExpTag;
	TLStaticText*	_questType;
	TLStaticText*	_questName;
	TLStaticText*	_count;
	TLStaticText*	_time;
	TLStaticText*	_place;
	TLStaticText*	_npc;
	TLStaticText*	_level;

	string			_tipText;
private:
	void	loadUi();
	
	void	showTip();

	bool	onMouseEnters(const EventArgs& e);
	bool	onNpcClick(const EventArgs& e);
	bool	onMouseLeaves(const EventArgs& e);
	bool	onMouseMove(const EventArgs& e);
public:
	KUiFSBibleSpecialQuestItem();
	~KUiFSBibleSpecialQuestItem();

	void	setContent(SpecialQuestData& data);
	void	clear();
};

#endif