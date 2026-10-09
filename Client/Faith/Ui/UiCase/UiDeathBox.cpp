///////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 11:20
//      File_base        : UiMessageBox
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "kwin32.h"
#include "UiDeathBox.h"
#include "CoreShell.h"
#include "IBShopComDef.h"
#include "OnceIBItemMgr.h"
#include "../KMessageCentre.h"
#include "Ui/UiCase/UiChatWindow.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiDeathBox* KUiWndSingleton<KUiDeathBox>::ms_Singleton	= NULL;

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiDeathBox::KUiDeathBox( const CEGUI::String& id_name ):
KUiWndSingleton<KUiDeathBox>( id_name ),m_LocalRevival(NULL)
{
	
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiDeathBox::~KUiDeathBox()
{

}

void KUiDeathBox::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/DeathBox/OK")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiDeathBox::handleRenascence, ms_Singleton));
		ms_Singleton->m_LocalRevival = (TLButton *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/DeathBox/LocalRevive");

		if (ms_Singleton->m_LocalRevival)
		{
			ms_Singleton->m_LocalRevival->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiDeathBox::handleLocalRevive, ms_Singleton));
		}//endif

	}
}

void	KUiDeathBox::Show( void )
{
	KUiWndSingleton<KUiDeathBox>::Show();
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		char * LocalRiviveStyle = KMessageCentre::GetMessage(ib_use,0);
		if (LocalRiviveStyle)
		{
			char szLocalRivival[128] = "";
			ClientBuyGoods ibItem;
			memset(&ibItem,0,sizeof(ibItem));
	        KOnceIBItemMgr::GetSingleten().GetOnceItemParam(local_revive,ibItem);
			sprintf(szLocalRivival,LocalRiviveStyle,ibItem.price);

			ms_Singleton->m_LocalRevival->setText(AnsiToUtf8(szLocalRivival));

		}//endif

	}//endif

}

bool KUiDeathBox::handleRenascence( const CEGUI::EventArgs& args )
{
	g_pCoreShell->OperationRequest( GOI_PLAYER_RENASCENCE, true, NULL );
	KUiDeathBox::Hide();
	return true;
}

bool KUiDeathBox::handleLocalRevive(const CEGUI::EventArgs & args)
{
	ClientBuyGoods ibItem;
	memset(&ibItem,0,sizeof(ibItem));
	KOnceIBItemMgr::GetSingleten().GetOnceItemParam(local_revive,ibItem);
	int jinshanBi = g_pCoreShell->GetGameData(GDI_PLAYER_JINSHANBI, NULL, NULL);
	
	if (jinshanBi >= ibItem.price)
	{
		g_pCoreShell->OperationRequest( GOI_IBSHOP_BUY,(int)&ibItem,0);
	}
	else
	{
		char * LocalNotEnough = KMessageCentre::GetMessage(ib_use,1);
		if (LocalNotEnough)
		{
			KUiChannelCentre::GetSingleton().toSysMsg(LocalNotEnough);
		}

	}//end else

	return true;
}