//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 15:16
//      File_base        : screeneffect_man
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"

#ifndef _SERVER

#include "screeneffect_man.h"
#include "screeneffect_tab.h"
#include "CoreShell.h"

ScreenEffectMgr::ScreenEffectMgr( void )
: m_nActiveEffect(0)
{
}

ScreenEffectMgr::~ScreenEffectMgr( void )
{
	ScreenEffectMap::iterator it = m_ScreenEffect.begin();
	for ( it; it != m_ScreenEffect.end(); it++ )
	{
		if ( it->second )
		{
			delete it->second;
			it->second = NULL;
		}
	}
}

ScreenEffectMgr& ScreenEffectMgr::Singleton( void )
{
	static ScreenEffectMgr sm;
	
	return sm;
}

void	ScreenEffectMgr::Load()
{
	m_nActiveEffect = 0;
	for ( int i = 1; i < SCREENEFFECT_COUNT+1; i++ )
	{
		m_ScreenEffect[i] = new ScreenEffect();
		m_ScreenEffect[i]->Load(i);
	}
}

void	ScreenEffectMgr::Player( int nEffectID, int eStyle )
{
    ScreenEffectStyle sRealtyle=(ScreenEffectStyle)eStyle;

    if (m_ScreenEffect.find(nEffectID)!=m_ScreenEffect.end())
	{
		m_ScreenEffect[nEffectID]->SetState(Playing);
		m_ScreenEffect[nEffectID]->SetStyle(sRealtyle);

		m_ScreenEffect[nEffectID]->PlayMusic();
		m_nActiveEffect++;
	}
}

void    ScreenEffectMgr::SetEffectPos(int nEffectID, int nX, int nY)
{
    if (m_ScreenEffect.find(nEffectID)!=m_ScreenEffect.end())
	{
        m_ScreenEffect[nEffectID]->SetEffectPos(nX,nY);
	}//endif
}

void	ScreenEffectMgr::PaintBeforeUi( void )
{
	if ( m_nActiveEffect <= 0 )
	{
		return;
	}

	for ( int i = 1; i < SCREENEFFECT_COUNT+1; i++ )
	{
		if ( m_ScreenEffect[i]->GetState() == Playing && 
			 m_ScreenEffect[i]->GetStyle() == BeforeUi &&
			 m_ScreenEffect[i]->GetID() > 0 && 
			 m_ScreenEffect[i]->GetID() < SCREENEFFECT_COUNT+1 )
		{
			m_ScreenEffect[i]->PlayEffect();
		}
	}
}

void	ScreenEffectMgr::PaintBehindUi( void )
{
	if ( m_nActiveEffect <= 0 )
	{
		return;
	}
	
	for ( int i = 1; i < SCREENEFFECT_COUNT+1; i++ )
	{
		if ( m_ScreenEffect[i]->GetState() == Playing && 
			 m_ScreenEffect[i]->GetStyle() == BehindUi &&
			 m_ScreenEffect[i]->GetID() > 0 && 
			 m_ScreenEffect[i]->GetID() < SCREENEFFECT_COUNT+1 )
		{
			m_ScreenEffect[i]->PlayEffect();
		}
	}
}


#endif

