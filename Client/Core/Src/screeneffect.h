//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/08/2007 14:51
//      File_base        : screeneffect
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _screeneffect_h
#define  _screeneffect_h

#ifndef _SERVER

#include "screeneffect_def.h"

enum ScreenEffectStyle
{
	BeforeUi,
	BehindUi,
};

enum ScreenEffectState
{
	Playing,
	Stoped,
};

class ScreenEffect
{
public:
	ScreenEffect( void );
	~ScreenEffect( void );

public:
	void				Load( int nEffectID );
	void                SetEffectPos(int nX,int nY);
	void				PlayEffect();
	void				PlayMusic();

	int					GetID() { return m_EffectID; }

	void				SetState(ScreenEffectState state) { m_State = state; }
	ScreenEffectState	GetState()	{ return m_State; }

	void				SetStyle(ScreenEffectStyle style) { m_Style = style; }
	ScreenEffectStyle	GetStyle() { return m_Style; }
	
private:
	KUiImageRef			m_Image;
	ScreenEffectStyle	m_Style;
	ScreenEffectState	m_State;
	KCacheNode*			m_pBuffSoundNode;
	KWavSound*			m_pBuffWave;

	int					m_EffectID;
};


#endif

#endif