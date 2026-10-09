//Õ®”√≤Àµ• xiehong 2007-11-26

#include "TLStatic.h"

#define	UI_UNIT_MENU_WINDOW_PATH	"UiSettings/layouts/UnitMenu.ls"

#define UI_UNIT_MENU_WISPER_FLAG		0x01
#define UI_UNIT_MENU_CAPTION_FLAG		0x02
#define UI_UNIT_MENU_KICK_FLAG			0x04
#define UI_UNIT_MENU_LEAVE_FLAG			0x08
#define UI_UNIT_MENU_ADDFRIEND_FLAG		0x10
#define UI_UNIT_MENU_EXP_FLAG			0x20

using namespace std;

class KUiUnitMenu
{
public:
	struct UMNpcInfo
	{
		string	NpcName;
		int		NpcId;
	};

	enum MENUITEM
	{
		UMI_WISPER = 0,
		UMI_CAPTION,
		UMI_KICK,
		UMI_LEAVE,
		UMI_ADDFRIEND,
		UMI_EXP,
		UMI_COUNT,
	};

private:
	UMNpcInfo		_npcInfo;
	
	TLStaticImage*	_thisWindow;
	TLButton*		_button[UMI_COUNT];

private:
	void	loadUi();

	bool	captain(const EventArgs& args);
	bool	kick(const EventArgs& args);
	bool	leave(const EventArgs& args);
	bool	addFriend(const EventArgs& args);
	bool	wisper(const EventArgs& args);
	bool	expSetting(const EventArgs& args);
	
public:
	void	show(UMNpcInfo npcInfo, int flag, Point pos);
	void	hide();

	KUiUnitMenu();
	~KUiUnitMenu();

	static KUiUnitMenu& getSingleton();
};
