//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/27/2007 13:08
//      File_base        : UiBubble
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiBubble_H
#define KUiBubble_H

#include "../uicommon.h"
#include "TLButton.h"
#include "TLStatic.h"
#include <vector>
#include "GameDataDef.h"
#include "../UiConfigManager.h"

#define UI_BUBBLE_MAX_COUNT 30
#define UI_BUBBLE_INVALIDE_INDEX -1
#define UI_BUBBLE_WINDOW_Y_OFF -10
#define UI_BUBBLE_MAX_WORD_COUNT 10

#define UI_BUBBLE_INI_FILE_PATH "UiSettings\\setting.ini"

using namespace CEGUI;

class _Bubble
{
	friend class KUiBubbleManager;
public:
	enum BubbleType
	{
		NpcHeadPop,
		TeamPop,
	};

	enum BubblePos
	{
		BubblePos_Left,
		BubblePos_Right,
		BubblePos_Top,
		BubblePos_Bottom,
		BubblePos_TopLeft,
		BubblePos_TopRight,
		BubblePos_BottomLeft,
		BubblePos_BottomRight,
	};
private:
    _Bubble();
    ~_Bubble();

	void	create(int index);
	void	destory();

	void	show(char* layoutText, bool bGm);
	void	hide( void );

	bool	isIdle()		{	return d_thisWnd->getAutoCloseTime() <= 0 ? true : false;	};

	int		getId()			{	return d_id;									};
	void	setId(int id);

	BubbleType getType()			{	return d_type;							};
	void	setType(BubbleType type){	d_type = type;							};
	
	void	formatText(const char* transText, char* layoutText, bool bGM);

	void	updatePosition(int x, int y);
protected:
	bool	onHide(const EventArgs& e);
	bool	onShow(const EventArgs& e);
	bool	onNewFrame(const EventArgs& e);
	bool	onWndAlphaChanged(const EventArgs& e);
private:
	TLStaticImage*	d_thisWnd;
	TLStaticText*	_tipText;
	TLStaticImage*	_tail;
	int				d_id;
	BubbleType		d_type;
	int				m_CloseTime;
};

class KUiBubbleManager
{
public:
	static KUiBubbleManager& getSington()
	{
		static KUiBubbleManager sington;
		return sington;
	}

	KUiBubbleManager();
	~KUiBubbleManager();
public:
	void	showBubble(char* layoutText, _Bubble::BubbleType type, int id, bool bGm);
	void	updateBubble(_Bubble::BubbleType type, int id, Position pos);
	void	closeAll();
private:
	int		findIdle(_Bubble::BubbleType type, int id);
private:
	_Bubble	d_bubbleList[UI_BUBBLE_MAX_COUNT];
};

#endif