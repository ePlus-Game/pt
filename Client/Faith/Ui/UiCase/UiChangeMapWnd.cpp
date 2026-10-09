//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/09/2007 12:34
//      File_base        : UiChangeMapWnd
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiChangeMapWnd.h"
#include "Coreshell.h"
#include "../UiConfigManager.h"
#include "../KMessageCentre.h"
#include "UiSearchHelpWnd.h"
#include "UiElf.h"
#include "UiGameSetting.h"
#include "../UiSheetMgr.h"
#include "UiLinkedItemTip.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

template<> 
KUiChangeMapWnd* KUiWndSingleton<KUiChangeMapWnd>::ms_Singleton	= NULL;

KUiChangeMapWnd::KUiChangeMapWnd( const CEGUI::String& id_name ):
KUiWndSingleton<KUiChangeMapWnd>( id_name )
{
	d_loading			= false;
	d_loadingTime		= 0;
	d_loadingBegin		= 0;
	d_tipCount			= 0;
	d_loadingImage		= NULL;
	d_loadingProgress	= NULL;
	d_tipText			= NULL;
}

KUiChangeMapWnd::~KUiChangeMapWnd()
{
	
}

void KUiChangeMapWnd::Init( void )
{
	if ( ms_Singleton && m_pThisWnd )
	{

		const KUiCfgLoader::ChangeMapParam& changeMapParam = KUiCfgLoader::getSingleton().getChangeMapParam();
		d_loadingTime = (g_Random( changeMapParam.loadingTime ) + 1) * 1000;
		m_pThisWnd->setRenderMode(false,2);
		m_pThisWnd->setZLevel(Window::SuperTop);
		d_tipCount = changeMapParam.tipCount;
		d_loadingImage = (TLStaticImage*)m_pThisWnd;
		d_loadingProgress = (TLProgressBar*)m_pThisWnd->getChild("TaharezLook/ChangeMapWnd/LoadingBar");
		//d_loadingProgress->subscribeEvent(Window::EventProgressDone, Event::Subscriber(&KUiChangeMapWnd::handleHide, ms_Singleton));

		d_tipText = (TLStaticText*)m_pThisWnd->getChild("TaharezLook/ChangeMapWnd/TipText");
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiChangeMapWnd::handleKeyDown, ms_Singleton));
		KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT_2)->addChildWindow(m_pThisWnd);
	}
}

void KUiChangeMapWnd::show( bool showElf, ChangeMapParam param  )
{
//	KUiWndSingleton<KUiChangeMapWnd>::Show();
	if ( ms_Singleton->m_pThisWnd->isVisible() )
		return;

	const KUiCfgLoader::ChangeMapParam& changeMapParam = KUiCfgLoader::getSingleton().getChangeMapParam();
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT_2);
	ms_Singleton->m_pThisWnd->show();
	ms_Singleton->d_loading = true;
	ms_Singleton->d_loadingBegin = ::GetTickCount();
	if ( ms_Singleton->d_tipText )
	{
		ms_Singleton->d_tipText->setText( AnsiToUtf8(KMessageCentre::GetMessage( changemap_message, g_Random( ms_Singleton->d_tipCount ) ) ) );
	}

	param = randomMap;

	if ( changeMapParam.mapCount == 0 )
	{
		return;
	}
	if ( param == randomMap )
	{
		int nMapId = g_Random( changeMapParam.mapCount );
		if ( g_GetScreenWidth() == 800 )
		{			
			((TLStaticImage*)ms_Singleton->m_pThisWnd)->setImage(changeMapParam.mapList800[nMapId].imageSet, changeMapParam.mapList800[nMapId].image );
		}
		else
		{
			((TLStaticImage*)ms_Singleton->m_pThisWnd)->setImage(changeMapParam.mapList1024[nMapId].imageSet, changeMapParam.mapList1024[nMapId].image );
		}		
	}
	else
	{
		if ( param != defaultMap )
		{
			int nMapId = param;
			if ( nMapId >= 0 && nMapId < changeMapParam.mapCount)
			{
				if ( g_GetScreenWidth() == 800 )
				{			
					((TLStaticImage*)ms_Singleton->m_pThisWnd)->setImage(changeMapParam.mapList800[nMapId].imageSet, changeMapParam.mapList800[nMapId].image );
				}
				else
				{
					((TLStaticImage*)ms_Singleton->m_pThisWnd)->setImage(changeMapParam.mapList1024[nMapId].imageSet, changeMapParam.mapList1024[nMapId].image );
				}		
			}
		}
	}

	//把此界面放在另外一个root上，以避免和其他界面的遮挡关系错乱
}

void	KUiChangeMapWnd::Breathe( void )
{
	if ( ms_Singleton == NULL ||
		 ms_Singleton->m_pThisWnd == NULL )		 
	{
		return;
	}

	DWORD passTime = ::GetTickCount() - ms_Singleton->d_loadingBegin;
	float percent = (float)passTime/(float)ms_Singleton->d_loadingTime;
	if ( percent >= 1.0f )
	{
		if (!ms_Singleton->d_loading)
		{
			ms_Singleton->d_loadingProgress->setProgress(1.0f);
			
			// EventProgressDone事件响应有点问题，先移到这里来
			ms_Singleton->m_pThisWnd->hide();
			KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);
		}
		else
		{
			ms_Singleton->d_loadingProgress->setProgress(0.999f);
		}
	}
	else
	{
		ms_Singleton->d_loadingProgress->setProgress(percent);
	}
	
	KUiItemTip::GetSingleton().Hide();
	KUiLinkedItemTip::GetSingleton().Hide();
}

void	KUiChangeMapWnd::EndLoading( void )
{
	if ( !ms_Singleton->m_pThisWnd->isVisible() )
		return;

	ms_Singleton->d_loading = false;
}

bool	KUiChangeMapWnd::handleKeyDown( const CEGUI::EventArgs& args )
{
	return false;
}

bool	KUiChangeMapWnd::handleHide( const CEGUI::EventArgs& args )
{
	ms_Singleton->d_loading = false;
	ms_Singleton->m_pThisWnd->hide();
	
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);

	KUiItemTip::GetSingleton().Hide();
	KUiLinkedItemTip::GetSingleton().Hide();
	return true;
}