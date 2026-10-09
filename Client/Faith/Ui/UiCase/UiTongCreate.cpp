//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/26/2006 20:02
//      File_base        : UiTongCreate
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "UiTongCreate.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "UiMDLInterface.h"
#include "../KMessageCentre.h"
#include "UiNpcMsgBox.h"
#include "Ui/UiConfigManager.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiTongCreate* KUiWndSingleton<KUiTongCreate>::ms_Singleton	= NULL;

KUiTongCreate::KUiTongCreate( const CEGUI::String& id_name ):
KUiWndSingleton<KUiTongCreate>( id_name )
{
	d_type = -1;
	if ( success_errorcode == m_pUiMDLManager->createDataSet( tong_operation ) )
	{
		TongOperParam tagCreateParam;
		ZeroMemory( &tagCreateParam, sizeof( TongOperParam ) );
		IUIMDLDataset*  pDataset = NULL;
		m_pUiMDLManager->queryDataSet( tong_operation, &pDataset );
		if ( pDataset )
		{
			for ( int nIdx = 0; nIdx < enSUO_Num; ++nIdx )
			{
				tagCreateParam.nTemplateID	= enSUTplId_Tong;
				tagCreateParam.nLayerID		= nIdx;
				tagCreateParam.nOperationID	= enSUO_CreateUnit;
				pDataset->addDataRecord( &tagCreateParam, sizeof(TongOperParam) );
			}
		}
	}

}


KUiTongCreate::~KUiTongCreate()
{
}

void KUiTongCreate::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/OkBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongCreate::handleOK, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CancelBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongCreate::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CloseBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongCreate::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/Name")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiTongCreate::handleEditKeyDown, ms_Singleton));
	/*	ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CondiTionTip")->getChild("TaharezLook/TongCreate/CondiTionTip/CancelBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongCreate::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CondiTionTip")->getChild("TaharezLook/TongCreate/CondiTionTip/OkBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongCreate::handleCreate, ms_Singleton));
	*/
	} 

}

bool KUiTongCreate::handleCreate(const CEGUI::EventArgs& args )
{
//	ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CondiTionTip")->hide();
	return true;
}

void KUiTongCreate::ShowActually(int eCompoundType)
{
	KUiWndSingleton<KUiTongCreate>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		Window* pTitle	= NULL;
		Window* pText	= NULL;
		Window* pInfo	= NULL;

#ifndef _DEBUG
	try
	{
#endif
		pTitle	= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongCreate/Title" );
		pText	= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongCreate/Txt" );
		pInfo	= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongCreate/Info" );
#ifndef _DEBUG
	}	
	catch ( ... )
	{
		return;
	}
#endif

		switch( eCompoundType )
		{
		case 0:
			pTitle->setText( AnsiToUtf8( SHIZU_CREATE ) );
			pText->setText( AnsiToUtf8( SHIZU_NAME ) ); 
			break;
		case 1:
			pTitle->setText( AnsiToUtf8( ZHUHOU_CREATE ) );
			pText->setText( AnsiToUtf8( ZHUHOU_NAME ) ); 
			break;
		case 2:
			pTitle->setText( AnsiToUtf8( LEAGUE_CREATE ) );
			pText->setText( AnsiToUtf8( LEAGUE_NAME ) ); 
			break;
		default:
			return;
			
		}
		pInfo->setText( AnsiToUtf8( KMessageCentre::GetMessage( createtong_error_message, eCompoundType ) ) );
		ms_Singleton->d_type = eCompoundType + 2;
	}
}

void KUiTongCreate::Show( int eCompoundType  )
{
	ShowActually(eCompoundType);

/*	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
      ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CondiTionTip")->show();
		TLStaticText * pText=(TLStaticText *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCreate/CondiTionTip")->getChild("TaharezLook/TongCreate/CondiTionTip/Text");
	    
		
		if (pText)
		{
			const char * szConditionText=0;
			const KUiCfgLoader::TongConditionCfg & tongCfg=KUiCfgLoader::getSingleton().getTongConditionCfg();

			switch( eCompoundType )
			{
			case 0:
				szConditionText=tongCfg.szShizuCond;
				break;
			case 1:
				szConditionText=tongCfg.szZhuhouCond;
				break;
			case 2:
				szConditionText=tongCfg.szGuoJiaCond;
				break;
			default:
				return;
				
			}

            pText->useLayout();
			pText->getLayout()->SetText((char *)szConditionText);
		}//endif

	}//endif
*/
	
}

bool KUiTongCreate::handleOK( const CEGUI::EventArgs& args )
{
	TongOperParam tagCreateParam;
	IUIMDLDataset*  pDataset = NULL;
	m_pUiMDLManager->queryDataSet( tong_operation, &pDataset );
	if ( pDataset )
	{
		tagCreateParam.nOperationID = d_type;
		String strName = m_pThisWnd->getChild("TaharezLook/TongCreate/Name")->getText();
		tagCreateParam.nTemplateID	= enSUTplId_Tong;
		tagCreateParam.nLayerID		= d_type - 1;
		tagCreateParam.nOperationID	= enSUO_CreateUnit;
		memcpy( tagCreateParam.szName, Utf8ToAnsi( strName ), CLIENT_NAME_AND_TITLE_MAX );
		tagCreateParam.szName[CLIENT_NAME_AND_TITLE_MAX-1] = 0;
		pDataset->updateRecord( d_type, &tagCreateParam, sizeof( TongOperParam ) );
	}
	Hide();
	KUiNpcMsgBox::GetSingleton().Hide();
	return true;
}

bool KUiTongCreate::handleExit( const CEGUI::EventArgs& args )
{
	Hide();
	KUiNpcMsgBox::GetSingleton().Hide();
    return true;
}

bool KUiTongCreate::handleEditKeyDown( const CEGUI::EventArgs& args )
{
	return true;
}