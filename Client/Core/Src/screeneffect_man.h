//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 14:51
//      File_base        : screeneffect_man
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _screeneffect_man_h

#define  _screeneffect_man_h

#ifndef _SERVER

#include "screeneffect_def.h"
#include "screeneffect.h"
#include <map>

#define SCREENEFFECT_COUNT 15

class ScreenEffectMgr
{
public:
	ScreenEffectMgr( void );
	~ScreenEffectMgr( void );

public:
	static ScreenEffectMgr& Singleton( void );	

	void	Player( int nEffectID, int eStyle );
	void    SetEffectPos(int nEffectID, int nX, int nY);

	void	Load( void );
	void	PaintBeforeUi( void );
	void	PaintBehindUi( void );

	void	EffectStop( void ) { m_nActiveEffect--; }

private:
	typedef std::map<int, ScreenEffect*>	ScreenEffectMap;
	ScreenEffectMap		m_ScreenEffect;

	int					m_nActiveEffect;
};


#endif

#endif