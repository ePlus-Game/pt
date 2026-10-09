//Õ®”√Õ∑œÒ xiehong 2007-11-26

#include "TLStatic.h"
#include "TLProgressBar.h"

#define UI_UNIT_FRAME_LEVEL_FLAG		0x01
#define UI_UNIT_FRAME_PROFESSION_FLAG	0x02
#define UI_UNIT_FRAME_HEAD_FLAG			0x04
#define UI_UNIT_FRAME_NAME_FLAG			0x08
#define UI_UNIT_FRAME_BLOOD_FLAG		0x10
#define UI_UNIT_FRAME_MANA_FLAG			0x20
#define UI_UNIT_FRAME_EXP_FLAG			0x40

#define UI_UNIT_FRAME_WINDOW_PATH		"uisettings/layouts/UnitFrame.ls"
#define UI_UF_MGR_MAX_UNIT_COUNT		5
#define UI_UF_MGR_INVALID_ID			-1

class KUiUnitFrame
{
	friend class KUiUnitFrameMgr;
	
	TLStaticImage* _thisWindow;

	TLStaticImage*	_headImg;

	TLStaticText*	_levelText;

	TLStaticImage*	_professionImg;

	TLStaticText*	_nameText;

	TLProgressBar*	_bloodImg;
	TLStaticText*	_bloodText;
	TLStaticText*	_bloodPercentText;

	TLProgressBar*	_manaImg;
	TLStaticText*	_manaText;
	TLStaticText*	_manaPercentText;

	TLProgressBar*	_expImage;

	int				_style;
private:
	KUiUnitFrame();
	~KUiUnitFrame();

	void	create(int id);
public:
	void	setStyle(int style);
	int		getStyle();

	void	setHeadImg(const Image* image);
	void	setLevel(int level);
	void	setProfessionImage(const Image* image);
	void	setName(const char* name);
	void	setBlood(int cur, int max);
	void	setMana(int cur, int max);
	void	setExp(int cur, int max);

	void	setPos(Point pos);
	void	setSize(int flag, Size size);

	void	show();
	void	hide();
	bool	isVisible();

	Window* getWindow(){	return _thisWindow;	}
};


class KUiUnitFrameMgr
{
	KUiUnitFrame _frames[UI_UF_MGR_MAX_UNIT_COUNT];
	
	int _curIndex;
public:
	KUiUnitFrameMgr();
	~KUiUnitFrameMgr();
	static KUiUnitFrameMgr& getSingleton();
	
	int				createAUnit();
	KUiUnitFrame*	find(int index);
};