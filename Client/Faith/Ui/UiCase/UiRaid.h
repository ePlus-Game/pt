//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/2/2007 22£∫03
//      File_base        : UiRaid
//      File_ext         : head
//      Author           : xiehong
//      Description      : Õ≈∂”UI
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UI_RAID
#define UI_RAID

#include "CEGUI.h"
#include "TLStatic.h"
#include "TLButton.h"
#include "TLCheckbox.h"
#include "TLVertScrollbar.h"
#include "GameDataDef.h"

#define UI_RAID_WINDOW_NAME_1024 "uisettings/layouts1024/Raid.ls"
#define UI_RAID_WINDOW_NAME "uisettings/layouts/Raid.ls"
#define UI_RAID_PLAYER_WINDOW_NAME "uisettings/layouts/RaidPlayer.ls"
#define UI_RAID_MAX_NUM 50
#define UI_RAID_INVALID_PLAYER_INDEX -1

class KUiRaidPlayer
{
	friend class KUiRaid;

protected:
	TLStaticImage*	_thisWindow;
	TLStaticText*	_playerName;
	TLStaticImage*	_leadIcon;
	TLButton*		_assistIcon;
	TLStaticImage*	_headImg;
	TLStaticImage*	_hoverFrameImg;
	TLStaticImage*	_pushedFrameImg;

	bool			_lbDown;
	virtual bool onAssistBtnDown(const EventArgs& e);

	virtual bool onBDown(const EventArgs& e);
	virtual bool onBUp(const EventArgs& e);
	virtual bool onMouseOut(const EventArgs& e);
	virtual bool onMouseIn(const EventArgs& e);
	
	const char*	getHeadImg(int series, int skillSeries);
	
	void		setHoverState();
	void		setNormalState();
	void		setPushDownState();
public:
	KUiRaidPlayer();
	~KUiRaidPlayer();
	
	Point		getPos();
	void		setPos(const Point& pos);
	int			getHeight();
	int			getWidth();

	void		setParent(Window* parent);

	void		show(int playerIndex);
	void		hide();
	bool		isVisible();

	const char*	getPlayerName();
	int			getPlayerIndex();

	void		showLeadIcon(bool isLead);
	void		showAssistIcon(bool show, bool isAssist);
	
	void		disable();
private:
	int			_playerIndex;
};

class KUiRaid
{
	TLStaticImage*		_thisWindow;

	//child ctrl
	TLVertScrollbar*	_scrollBar;
	TLStaticImage*		_upArea;
	TLStaticImage*		_downArea;

	KUiTeamMemberItem	_teamInfo[UI_RAID_MAX_NUM];
	KUiRaidPlayer		_player[UI_RAID_MAX_NUM];

	bool				_isLeader;

	TLStaticImage*		_playerPanel;
	TLStaticImage*		_playerInfo;
	TLButton*			_closeBtn;
	
	int					_playerPanelMaxSize;
	int					_curMemberCount;

	bool				_mouseAtUpArea;
	bool				_mouseAtDownArea;
private:
	void			layoutPanel();
	bool			onScroll(const EventArgs& arg);
	bool			onClickCloseBtn(const EventArgs& arg);
	bool			onWheelChanged(const EventArgs& args);

	bool			onTimer(const EventArgs& args);

	bool			onUpAreaIn(const EventArgs& args);
	bool			onUpAreaOut(const EventArgs& args);
	bool			onDownAreaIn(const EventArgs& args);
	bool			onDownAreaOut(const EventArgs& args);
public:
	KUiRaid();
	~KUiRaid();

	const KUiTeamMemberItem&	getPlayerByIndex(int playerIndex);
	void			switchPlayer(int index1, int index2);
	
	bool			isLeader(){	return _isLeader;	};
	void			show();
	void			freshTeamInfo();
	void			toggle();
	void			hide();
	bool			isVisible();

	static KUiRaid& getSinglton()
	{
		static KUiRaid singlton;
		return singlton;
	};
};

class KUiRaidDragPlayer : public KUiRaidPlayer
{
	void	moveToMouse();
public:
	KUiRaidDragPlayer();
	~KUiRaidDragPlayer();

	void	show(int playerIndex);
	bool	onMouseMove(const EventArgs& e);
	bool	onMouseBDown(const EventArgs& e);

	static KUiRaidDragPlayer& getSinglton()
	{
		static KUiRaidDragPlayer singlton;
		return singlton;
	};
};
#endif