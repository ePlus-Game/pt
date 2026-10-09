//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/21/2007 10::46
//      File_base        : UiGameSetting
//      File_ext         : h
//      Author           : likun
//      Description      : 游戏设置界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "../../Faith.h"
#include "UiGameSetting.h"
#include <assert.h>
#include "KIniFile.h"
#include "TLRadioButton.h"
#include "TLMiniHorzScrollbar.h"
#include "TLButton.h"
#include "iRepresentshell.h"
#include "KColors.h"

extern iRepresentShell * g_pRepresentShell;
#include "..\UiAdapter.h"
#include "UiChatWindow.h"
#include "UiMapCentre.h"
#include "UiComMsgBox.h"
#include "..\KMessageCentre.h"
//#include "CoreShell.h"

using namespace CEGUI;

const float	MUSIC_MAX_VALUE		= 100.0;
const float VOICE_MAX_VALUE		= 100.0;
const int	DEFAULT_MUSIC_VALUE = 28;
const float CATON_QULITY_VALUE	= 5.0;
const float RESELOVE_RADIO		= 3.0;

extern iCoreShell *g_pCoreShell;


template<>
KUiGameSetting *KUiWndSingleton<KUiGameSetting>::ms_Singleton = NULL;

KUiGameSetting::KUiGameSetting( const CEGUI::String &id_name )
: KUiWndSingleton<KUiGameSetting>( id_name )
, d_pMusicSlider(NULL)
, d_pVoiceSlider(NULL)
, d_pFullScrnRadio(NULL)
, d_pWindowRadio(NULL)
, d_pCloseButton(NULL)
, d_pFontShadow(NULL)
, d_pShowPlayer(NULL)
, d_pShowNpc(NULL)
, d_pShowShadow(NULL)
, d_pDrawGround(NULL)
, d_pDrawSmallObj(NULL)
, d_pDrawLargeObj(NULL)
, d_pLockShortcut(NULL)
, d_iMusicValue(DEFAULT_MUSIC_VALUE)
, d_iSoundValue(DEFAULT_MUSIC_VALUE)
, d_bWinOrFull(false)
, d_bShowPlayer(true)
, d_bShowNpc(true)
, d_bShowShadow(true)
, d_bDrawGround(true)
, d_bDrawSmallObj(true)
, d_bDrawLargeObj(true)
, d_bFontShadow(true)
, d_screenWidth(800)
, d_screenHeight(600)
, d_bLockShortcut(false)
, d_bShowSearchHelp(false)
, d_bShowSimpleHelp(false)
, d_bShowFirstHelp(false)
, d_hasNotifiedScreenChange(true)
{


}

KUiGameSetting::~KUiGameSetting()
{
	
}

void	KUiGameSetting::Show( void )
{
	if ( ms_Singleton != NULL )
	{
		KUiWndSingleton<KUiGameSetting>::Show();
		/*
		if ( ms_Singleton && ms_Singleton->m_pThisWnd &&
			ms_Singleton->d_pHeightRadio && ms_Singleton->d_pLowRadio )
		{
			ms_Singleton->d_pHeightRadio->disable();
			ms_Singleton->d_pHeightRadio->hide();
			ms_Singleton->d_pLowRadio->disable();
		}//*/
		ms_Singleton->SetCurrentGameSet();
		///
	}
}

void	KUiGameSetting::Hide( void )
{
	ms_Singleton->SaveAllGameSetValue();
	KUiWndSingleton<KUiGameSetting>::Hide();
}

void	KUiGameSetting::Init( void )
{
	if ( ms_Singleton != NULL && ms_Singleton->m_pThisWnd != NULL )
	{
		ms_Singleton->GetContrlRegister();
		if ( d_pMusicSlider && d_pVoiceSlider )
		{
			d_pMusicSlider->setScrollPosition(d_iMusicValue / MUSIC_MAX_VALUE);
			d_pVoiceSlider->setScrollPosition(d_iSoundValue / VOICE_MAX_VALUE);

			if ( d_bWinOrFull )
			{
				d_pFullScrnRadio->setSelected( true );
			}
			else
			{
				d_pWindowRadio->setSelected( true );
			}

			if ( d_screenWidth == 1024 && d_screenHeight == 768 )
			{
				d_pHeightRadio->setSelected(true);
			}

			if ( d_screenWidth == 800 && d_screenHeight == 600 )
			{
				d_pLowRadio->setSelected(true);
			}
		}
	}
}

//获取控件并注册
void	KUiGameSetting::GetContrlRegister( void )
{
	if( ms_Singleton != NULL && m_pThisWnd != NULL )
	{
		d_pMusicSlider		= (TLMiniHorzScrollbar *)(m_pThisWnd->getChild("TaharezLook/GameSetting/MusicSlider"));
		d_pVoiceSlider		= (TLMiniHorzScrollbar *)(m_pThisWnd->getChild("TaharezLook/GameSetting/VoiceSlider"));
		d_pFullScrnRadio	= (TLRadioButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/FullScreen"));
		d_pWindowRadio		= (TLRadioButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/WindowScreen"));
		d_pShowPlayer		= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/ShowPlayer"));
		d_pShowNpc			= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/ShowNpc"));
		d_pShowShadow		= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/ShowShadow"));
		d_pCloseButton		= (TLButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/Close"));
		d_pCancelButton		= (TLButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/CancelBtn"));
		d_pOKButton			= (TLButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/OKBtn"));
		d_pHeightRadio		= (TLRadioButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/HeightRadio"));
		d_pLowRadio			= (TLRadioButton *)(m_pThisWnd->getChild("TaharezLook/GameSetting/LowRadio"));
		d_pDrawGround		= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/DrawGround"));
		d_pDrawSmallObj		= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/DrawSmallObj"));
		d_pDrawLargeObj		= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/DrawLargeObj"));
		d_pFontShadow		= (Checkbox *)(m_pThisWnd->getChild("TaharezLook/GameSetting/fontShadow"));
		d_pLockShortcut		= (Checkbox	*)(m_pThisWnd->getChild("TaharezLook/GameSetting/lockShortcut"));

		d_pMusicSlider->subscribeEvent( TLMiniHorzScrollbar::EventScrollPositionChanged, Event::Subscriber( &KUiGameSetting::handleSlider, ms_Singleton));
		d_pVoiceSlider->subscribeEvent( TLMiniHorzScrollbar::EventScrollPositionChanged, Event::Subscriber( &KUiGameSetting::handleSlider, ms_Singleton));
		d_pFullScrnRadio->subscribeEvent( TLRadioButton::EventMouseClick, Event::Subscriber( &KUiGameSetting::handleScreen, ms_Singleton));
		d_pWindowRadio->subscribeEvent( TLRadioButton::EventMouseClick, Event::Subscriber( &KUiGameSetting::handleScreen, ms_Singleton));
		d_pShowPlayer->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::ShowPlayer, ms_Singleton ));
		d_pShowNpc->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::ShowNpc, ms_Singleton ));
		d_pShowShadow->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::ShowShadow, ms_Singleton ));
		d_pCloseButton->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiGameSetting::handleClose, this ));
		d_pCancelButton->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiGameSetting::handleCancel, this ));
		d_pOKButton->subscribeEvent( TLButton::EventClicked, Event::Subscriber( &KUiGameSetting::handleOK, this ));
		d_pHeightRadio->subscribeEvent( TLRadioButton::EventMouseClick, Event::Subscriber( &KUiGameSetting::handleRadio, ms_Singleton ));
		d_pLowRadio->subscribeEvent( TLRadioButton::EventMouseClick, Event::Subscriber( &KUiGameSetting::handleRadio, ms_Singleton ));

		d_pDrawGround->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::DrawGround, ms_Singleton ));
		d_pDrawSmallObj->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::DrawSmallObj, ms_Singleton ));
		d_pDrawLargeObj->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::DrawLargeObj, ms_Singleton ));
		d_pFontShadow->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::handleFontShadow, ms_Singleton ));
		d_pLockShortcut->subscribeEvent( Checkbox::EventMouseClick, Event::Subscriber( &KUiGameSetting::handleLockShortcut, ms_Singleton ));

		/////////////render add ////取消按钮恢复以前设置///////////////
		SetCurrentGameSet();
		///end 
	}
}
//////////////////////////////////////////////////////////
void KUiGameSetting::SetCurrentGameSet()
{
	if(ms_Singleton != NULL && m_pThisWnd != NULL)
	{
		if(d_pShowPlayer)
			currentGameSet[SHOWPLAYER_FLAG_ID] = d_pShowPlayer->isSelected();
		if(d_pShowNpc)
			currentGameSet[SHOWNPC_FLAG_ID] = d_pShowNpc->isSelected();
		if(d_pShowShadow)
			currentGameSet[SHOWSHADOW_FLAG_ID] = d_pShowShadow->isSelected();
		if(d_pDrawGround)
			currentGameSet[DRAWGROUND_FLAG_ID] = d_pDrawGround->isSelected();
		if(d_pDrawLargeObj)
			currentGameSet[DRAWLARGEOBJ_FLAG_ID] = d_pDrawLargeObj->isSelected();
		if(d_pDrawSmallObj)
			currentGameSet[DRAWSMALLOBJ_FLAG_ID] = d_pDrawSmallObj->isSelected();
		if(d_pFontShadow)
			currentGameSet[FONTSHADOW_FLAG_ID] = d_pFontShadow->isSelected();
		if(d_pLockShortcut)
			currentGameSet[LOCKSHORTCUT_FLAG_ID] = d_pLockShortcut->isSelected();
	}
}
//////////////////////////////////////////////////////
void KUiGameSetting::ResumeGameSet()
{
	if(d_pShowPlayer&&
		d_pShowPlayer->isSelected()!=currentGameSet[SHOWPLAYER_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_PLAYER, currentGameSet[SHOWPLAYER_FLAG_ID]);
		d_pShowPlayer->setSelected(currentGameSet[SHOWPLAYER_FLAG_ID]);
		d_bShowPlayer = currentGameSet[SHOWPLAYER_FLAG_ID];
	}
	if(d_pShowNpc&&
		d_pShowNpc->isSelected()!=currentGameSet[SHOWNPC_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_NPC, currentGameSet[SHOWNPC_FLAG_ID]);
		d_pShowNpc->setSelected(currentGameSet[SHOWNPC_FLAG_ID]);
		d_bShowNpc = currentGameSet[SHOWNPC_FLAG_ID];
	}
	if(d_pShowShadow&&
		d_pShowShadow->isSelected()!=currentGameSet[SHOWSHADOW_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_SHADOW, currentGameSet[SHOWSHADOW_FLAG_ID]);
		d_pShowShadow->setSelected(currentGameSet[SHOWSHADOW_FLAG_ID]);
		d_bShowShadow = currentGameSet[SHOWSHADOW_FLAG_ID];
	}

	if(d_pDrawGround&&
		d_pDrawGround->isSelected()!=currentGameSet[DRAWGROUND_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_GROUND, currentGameSet[DRAWGROUND_FLAG_ID]);
		d_pDrawGround->setSelected(currentGameSet[DRAWGROUND_FLAG_ID]);
		d_bDrawGround = currentGameSet[DRAWGROUND_FLAG_ID];
	}
	if(d_pDrawLargeObj&&
		d_pDrawLargeObj->isSelected()!=currentGameSet[DRAWLARGEOBJ_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_LARGEOBJ, currentGameSet[DRAWLARGEOBJ_FLAG_ID]);
		d_pDrawLargeObj->setSelected(currentGameSet[DRAWLARGEOBJ_FLAG_ID]);
		d_bDrawLargeObj = currentGameSet[DRAWLARGEOBJ_FLAG_ID];
	}
	if(d_pDrawSmallObj&&
		d_pDrawSmallObj->isSelected()!=currentGameSet[DRAWSMALLOBJ_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_SMALLOBJ, currentGameSet[DRAWSMALLOBJ_FLAG_ID]);
		d_pDrawSmallObj->setSelected(currentGameSet[DRAWSMALLOBJ_FLAG_ID]);
		d_bDrawSmallObj = currentGameSet[DRAWSMALLOBJ_FLAG_ID];
	}
	if(d_pFontShadow&&
		d_pFontShadow->isSelected()!=currentGameSet[FONTSHADOW_FLAG_ID])
	{
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_FONT_SHADOW, currentGameSet[FONTSHADOW_FLAG_ID]);
		d_pFontShadow->setSelected(currentGameSet[FONTSHADOW_FLAG_ID]);
		d_bFontShadow = currentGameSet[FONTSHADOW_FLAG_ID];
	}
	if(d_pLockShortcut&&
		d_pLockShortcut->isSelected()!=currentGameSet[LOCKSHORTCUT_FLAG_ID])
	{
		d_pLockShortcut->setSelected(currentGameSet[LOCKSHORTCUT_FLAG_ID]);
		d_bLockShortcut = currentGameSet[LOCKSHORTCUT_FLAG_ID];
	}
}
///////////////////////////////////////////
/************************************************************************/
/*                                                                      */
/************************************************************************/
bool	KUiGameSetting::handleSlider( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider->window == d_pMusicSlider )
	{
		d_iMusicValue = d_pMusicSlider->getScrollPosition() * MUSIC_MAX_VALUE;
		g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, static_cast<int>(d_iMusicValue)  );
		SaveAllGameSetValue();
		return true;
	}

	if ( slider->window == d_pVoiceSlider )
	{
		d_iSoundValue = d_pVoiceSlider->getScrollPosition() * VOICE_MAX_VALUE;
		g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_SOUND_VALUE, static_cast<int>(d_iSoundValue) );
		System::getSingleton().getSound()->SetVolume(d_iSoundValue);
		SaveAllGameSetValue();
		return true;
	}

	return false;
}

bool	KUiGameSetting::handleScreen( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	//当为全屏时,设置OPTION_FULL_SCREEN为真
	if ( slider->window == d_pFullScrnRadio && !g_IsFullScreen() )
	{
		g_SetFullScreen( true );
		CEGUI::System::getSingleton().releaseTexture();
		g_pRepresentShell->Reset( g_GetScreenWidth(), g_GetScreenHeight(), true );
		CEGUI::System::getSingleton().reCreateTexture();
	//	CEGUI::System::getSingleton().ReDrawAllDxSurfaceWindow();
		KUiSheetMgr::getSinglton().redrawAllWindow();
		d_bWinOrFull = true;
		SaveAllGameSetValue();
		KUiChannelCentre::GetSingleton().showSystemFrame(true);
		KUiMiniMap::Show();
		return true;
	}

	//当为窗口时,设置OPTION_FULL_SCREEN为假
	if ( slider->window == d_pWindowRadio && g_IsFullScreen() )
	{
		g_SetFullScreen( false );
		CEGUI::System::getSingleton().releaseTexture();
		g_pRepresentShell->Reset( g_GetScreenWidth(), g_GetScreenHeight(), false );
		CEGUI::System::getSingleton().reCreateTexture();
		KUiSheetMgr::getSinglton().redrawAllWindow();
//zz		CEGUI::System::getSingleton().ReDrawAllDxSurfaceWindow();
		d_bWinOrFull = false;
		SaveAllGameSetValue();
		return true;
	}
	return true;
}

bool	KUiGameSetting::ShowPlayer( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_PLAYER, pBtn->isSelected() );
			d_bShowPlayer = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::ShowNpc( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_NPC, pBtn->isSelected() );
			d_bShowNpc = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::ShowShadow( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_SHADOW, pBtn->isSelected() );
			d_bShowShadow = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::handleRadio( const CEGUI::EventArgs &args )
{
	//当用户重新设置了分辨率之后，要重新通知用户重启后才能生效
	d_hasNotifiedScreenChange = false;

	//当为高分辨率时, 设OPTION_RESOLVE_RADIO为真
	WindowEventArgs *radio = (WindowEventArgs *)(&args);
	if (radio->window == d_pHeightRadio)// && g_GetScreenWidth() == 1024 )
	{
		d_screenWidth = 1024;
		d_screenHeight = 768;
		SaveAllGameSetValue();
		return true;
	}	
	//当为低分辨率时, 设OPTION_RESOLVE_RADIO为假
	if (radio->window == d_pLowRadio)// && g_GetScreenWidth() == 800 )
	{
		d_screenWidth = 800;
		d_screenHeight = 600;
		SaveAllGameSetValue();
		return true;
	}

	return true;
}

bool	KUiGameSetting::DrawGround( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_GROUND, pBtn->isSelected() );
			d_bDrawGround = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::DrawSmallObj( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_SMALLOBJ, pBtn->isSelected() );
			d_bDrawSmallObj = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::DrawLargeObj( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_LARGEOBJ, pBtn->isSelected() );
			d_bDrawLargeObj = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::handleFontShadow( const CEGUI::EventArgs &args )
{
	WindowEventArgs *slider = (WindowEventArgs *)(&args);
	if ( slider )
	{
		Checkbox* pBtn = (Checkbox*)slider->window;
		if ( g_pCoreShell && pBtn )
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_FONT_SHADOW, pBtn->isSelected() );
			d_bFontShadow = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return false;
}

bool	KUiGameSetting::handleLockShortcut( const CEGUI::EventArgs &args )
{	
	WindowEventArgs *eventargs = (WindowEventArgs *)(&args);
	if ( eventargs )
	{
		Checkbox* pBtn = (Checkbox*)eventargs->window;
		if ( g_pCoreShell && pBtn )
		{
			d_bLockShortcut = pBtn->isSelected();
		}
		SaveAllGameSetValue();
	}
	return true;
}

bool	KUiGameSetting::handleClose( const CEGUI::EventArgs &args )
{
	ResumeGameSet();
	KUiGameSetting::Hide();
	return false;
}

bool	KUiGameSetting::handleCancel( const CEGUI::EventArgs &args )
{
	ResumeGameSet();
	KUiGameSetting::Hide();
	return true;
}

bool	KUiGameSetting::handleOK( const CEGUI::EventArgs &args )
{
	KUiGameSetting::Hide();
	if (!d_hasNotifiedScreenChange)
	{
		char comfirmString[COMMON_CLIENT_MSG_LEN_8];
		strcpy(comfirmString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().comfirmString));
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(KMessageCentre::GetMessage(common_message, 102)));
		KUiComMsgBox::GetSingleton().setBtnName((utf8*)comfirmString);
		d_hasNotifiedScreenChange = true;
	}
	return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/


//初始化系统设置值
bool KUiGameSetting::LoadAllGameSetValue( void )
{
	int temp = 0;
	KIniFile Ini;
	if ( g_Accounts[0] != 0 && Ini.Load(g_Accounts) ) 
	{
		InitGameSetScrolContrl( &Ini, "MusicSet", &d_iMusicValue, d_pMusicSlider, MUSIC_MAX_VALUE );
		g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, d_iMusicValue );
		InitGameSetScrolContrl( &Ini, "VoiceSet",  &d_iSoundValue, d_pVoiceSlider, VOICE_MAX_VALUE );
		g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_SOUND_VALUE, d_iSoundValue );
		System::getSingleton().getSound()->SetVolume(d_iSoundValue);
		InitGameRadioContrl( &Ini );
		InitGameCheckContrl( &Ini, "ShowPlayer", &temp, d_pShowPlayer, d_bShowPlayer, true );
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_PLAYER, d_bShowPlayer );
		InitGameCheckContrl( &Ini, "ShowNpc", &temp, d_pShowNpc, d_bShowNpc, true);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_NPC, d_bShowNpc );
		InitGameCheckContrl( &Ini, "ShowShadow", &temp, d_pShowShadow, d_bShowShadow, true);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_SHADOW, d_bShowShadow );
		InitGameCheckContrl( &Ini, "DrawGround", &temp, d_pDrawGround, d_bDrawGround, true );
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_GROUND, d_bShowPlayer );
		InitGameCheckContrl( &Ini, "DrawSmallObj", &temp, d_pDrawSmallObj, d_bDrawSmallObj, true);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_SMALLOBJ, d_bDrawSmallObj );
		InitGameCheckContrl( &Ini, "DrawLargeObj", &temp, d_pDrawLargeObj, d_bDrawLargeObj, true);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_LARGEOBJ, d_bDrawLargeObj );
		InitGameCheckContrl( &Ini, "FontShadow", &temp, d_pFontShadow, d_bFontShadow, true);
		InitGameCheckContrl( &Ini, "LockShortcut", &temp, d_pLockShortcut, d_bLockShortcut, false);
		return true;
	}
	else
	{
		return false;
	}

}


//初始化一个滚动条的值
void KUiGameSetting::InitGameSetScrolContrl( KIniFile *pIni, char *pKeyValue, int *pValue, TLMiniHorzScrollbar *pCtrl, float fMaxRange )
{
	float tempf = 0;
	if ( pIni != NULL && pCtrl != NULL )
	{
		pIni->GetInteger("GameSetting", pKeyValue, DEFAULT_MUSIC_VALUE, pValue);
		tempf = static_cast<float>(*pValue / fMaxRange);
		pCtrl->setScrollPosition(tempf);
	}
}

//初始化一个checkbox的值
void KUiGameSetting::InitGameCheckContrl(KIniFile *pIni, char *pKeyValue, int *pValue, Checkbox *pCtrl, bool &bValue, int nDefault )
{
	if (pIni != NULL && pCtrl != NULL)
	{
		pIni->GetInteger("GameSetting", pKeyValue, nDefault, pValue);
		((*pValue) == 0) ? (bValue = false) : (bValue = true);
		pCtrl->setSelected(bValue);
	}
}

void	KUiGameSetting::InitGameRadioContrl( KIniFile *Ini)
{
	if ( Ini == NULL )
	{
		return;
	}
	int temp = 0;
	Ini->GetInteger( "GameSetting", "FullOrWin", 0, &temp);
	(temp == 0 ) ? (d_bWinOrFull = false) : (d_bWinOrFull = true);
	if ( d_bWinOrFull )
	{
		d_pFullScrnRadio->setSelected( true );
	}
	else
	{
		d_pWindowRadio->setSelected( true );
	}

	//Ini->GetInteger("GameSetting","ScreenWidth", 800, &d_screenWidth);
	//Ini->GetInteger("GameSetting","ScreenHeight", 600, &d_screenHeight);

	//if ( d_screenWidth == 1024 && d_screenHeight == 768 )
	if ( g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768 )
	{
		d_screenWidth = 1024;
		d_screenHeight = 768;		
		d_pHeightRadio->setSelected(true);
	}
	//if ( d_screenWidth == 800 && d_screenHeight == 600 )
	else if ( g_GetScreenWidth() == 800 && g_GetScreenHeight() == 600 )
	{
		d_screenWidth = 800;
		d_screenHeight = 600;
		d_pLowRadio->setSelected(true);
	}
}

void KUiGameSetting::SaveAllGameSetValue( void )
{
	KIniFile pSetting;
	if ( !pSetting.Load(g_Accounts))
	{
		return;
	}
	if ( ms_Singleton )
	{
		pSetting.WriteInteger( "GameSetting", "MusicSet", ms_Singleton->d_iMusicValue );
		pSetting.WriteInteger( "GameSetting", "VoiceSet", ms_Singleton->d_iSoundValue );
		pSetting.WriteInteger( "GameSetting", "FullOrWin", static_cast<int>(ms_Singleton->d_bWinOrFull));
		pSetting.WriteInteger( "GameSetting", "ScreenWidth", d_screenWidth);
		pSetting.WriteInteger( "GameSetting", "ScreenHeight", d_screenHeight);
		pSetting.WriteInteger( "GameSetting", "ShowPlayer", static_cast<int>(ms_Singleton->d_bShowPlayer));
		pSetting.WriteInteger( "GameSetting", "ShowNpc", static_cast<int>(ms_Singleton->d_bShowNpc));
		pSetting.WriteInteger( "GameSetting", "ShowShadow", static_cast<int>(ms_Singleton->d_bShowShadow));
		pSetting.WriteInteger( "GameSetting", "DrawGround", static_cast<int>(ms_Singleton->d_bDrawGround));
		pSetting.WriteInteger( "GameSetting", "DrawSmallObj", static_cast<int>(ms_Singleton->d_bDrawSmallObj));
		pSetting.WriteInteger( "GameSetting", "DrawLargeObj", static_cast<int>(ms_Singleton->d_bDrawLargeObj));
		pSetting.WriteInteger( "GameSetting", "FontShadow", static_cast<int>(ms_Singleton->d_bFontShadow));
		pSetting.WriteInteger( "GameSetting", "LockShortcut", static_cast<int>(ms_Singleton->d_bLockShortcut));
		pSetting.Save(g_Accounts);
	}
	KIniFile config;
	if ( !config.Load( CONFIG_INI ) )
	{
		return;
	}
	if ( ms_Singleton )
	{
		config.WriteInteger( "GameSetting", "MusicSet", ms_Singleton->d_iMusicValue );
		config.WriteInteger( "GameSetting", "VoiceSet", ms_Singleton->d_iSoundValue );
		config.WriteInteger( "GameSetting", "FullOrWin", static_cast<int>(ms_Singleton->d_bWinOrFull));
		config.WriteInteger( "GameSetting", "ScreenWidth", d_screenWidth);
		config.WriteInteger( "GameSetting", "ScreenHeight", d_screenHeight);
		config.WriteInteger( "GameSetting", "FontShadow", static_cast<int>(ms_Singleton->d_bFontShadow));
		config.Save(CONFIG_INI);
	}	
}