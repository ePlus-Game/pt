//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/16/2006 15:17
//      File_base        : UiGame
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIGAMESPACE_H
#define UIGAMESPACE_H

class KUiGameSpace
{
public:
	KUiGameSpace( void );
	~KUiGameSpace( void );

public:
	int		handleInput( unsigned int uMsg, unsigned int uParam, int nParam		);
	void	PaintWindow(														);
	void    PaintUiEffect(                                                      );
	void	BreatheWindow( void													);
private:
	DWORD	d_lButtonDownBeginTick;
	DWORD	d_lButtonDownTickCount;
};

extern KUiGameSpace	g_WndGameSpace;

#endif

