
//xiehong 2007-8-13 电影场景效果
#ifndef UI_MOVIE_FRAME
#define UI_MOVIE_FRAME

#include "../UiCommon.h"
#include "GameDataDef.h"

#define UI_MOVIE_FRAME_WINDOW_PATH "uisettings/layouts/MovieFrame.ls"
#define UI_MOVIE_QUEST_BOTTOM_COUNT 8

class KUiMovieFrame
{
	TLStaticImage*	_thisWindow;

	TLStaticImage*	_topFrame;
	TLStaticImage*	_bottomFrame;

	TLStaticImage*	_headImg;
	TLStaticText*	_msgText;

	TLStaticImage*	_questPanel;
	TLStaticImage*	_questTitleImage[UI_MOVIE_QUEST_BOTTOM_COUNT];
	TLButton*		_questBtn[UI_MOVIE_QUEST_BOTTOM_COUNT];

	char			_msg[LAYOUT_TEXT_MAX_LEN];
public:
	KUiMovieFrame();
	~KUiMovieFrame();
	
	void	npcWantTalk(const KUiMovieScene* npcTalkMessage);
	static KUiMovieFrame& getSinglton()
	{
		static KUiMovieFrame singlton;
		return singlton;
	}
protected:
	bool	onFadingInEnd(const EventArgs& arg);
	bool	onHide(const EventArgs& arg);
	bool	onClick(const EventArgs& arg);
private:
	void	load();
	void	hideCtrls();
	void	beginScene();
	void	endScene();
};

#endif UI_MOVIE_FRAME