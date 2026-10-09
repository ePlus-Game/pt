//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 15:16
//      File_base        : screeneffect
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"

#ifndef _SERVER
#include "KWin32Wnd.h"
#include "screeneffect.h"
#include "screeneffect_tab.h"
#include "screeneffect_man.h"
#include "CoreShell.h"

ScreenEffect::ScreenEffect( void )
: m_Style(BeforeUi)
, m_State(Stoped)
, m_pBuffSoundNode(NULL)
, m_pBuffWave(NULL)
, m_EffectID(0)
{
}

ScreenEffect::~ScreenEffect( void )
{
}

void	ScreenEffect::Load( int nEffectID )
{
	ScreenEffectTab& st =  ScreenEffectTab::Singleton();
	PST pScreenEffect = st.GetScreenEffect( nEffectID );
	if ( pScreenEffect )
	{
		m_EffectID = nEffectID;

		IR_InitUiImageRef( m_Image );	
		strncpy( m_Image.szImage, pScreenEffect->szImage, sizeof(m_Image.szImage));
		m_Image.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_Image.nType = ISI_T_SPR;
		m_Image.Color.Color_dw = 0xFF000000;
		m_Image.nFrame = 0;	
		m_Image.oPosition.nX = (float)pScreenEffect->nX/100.0f * g_GetScreenWidth();
		m_Image.oPosition.nY = (float)pScreenEffect->nY/100.0f * g_GetScreenHeight();

		m_pBuffSoundNode = g_SoundCache.GetNode(pScreenEffect->szSound, (KCacheNode*)m_pBuffSoundNode);
		if ( m_pBuffSoundNode )
		{
			m_pBuffWave = (KWavSound*)m_pBuffSoundNode->m_lpData;
		}
	}
}

void    ScreenEffect::SetEffectPos(int nX,int nY)
{
	m_Image.oPosition.nX=nX;
	m_Image.oPosition.nY=nY;
}

void	ScreenEffect::PlayEffect()
{
	KUiImageRef *pImage = NULL;
	pImage = &m_Image;
	if ( pImage && m_State == Playing )
	{
		g_pRepresent->DrawPrimitives(1, pImage, RU_T_IMAGE, true);
		if ( IR_NextFrame(*pImage) )
		{
			m_State = Stoped;
			if ( m_EffectID == 1 || m_EffectID == 2 )
			{
				CoreDataChanged(GDCNI_PICKUP_OBJECT_TO_BAG, 0, 0);
				ScreenEffectMgr::Singleton().EffectStop();
			}
		}
	}
}

void	ScreenEffect::PlayMusic()
{
	if (m_pBuffWave)
	{
		int nVolume = Option.GetSndVolume();
		m_pBuffWave->Play(0, nVolume, false);		
	}
}

#endif

