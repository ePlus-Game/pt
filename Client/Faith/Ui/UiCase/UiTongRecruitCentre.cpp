#include "UiTongRecruitCentre.h"
#include "CoreShell.h"
#include "SocialComDef.h"
#include "Ui/UiConfigManager.h"
#include "ChatDataDef.h"
#include "../KMessageCentre.h"
#include "CEGUICoordConverter.h"
#include "UiChatWindow.h"

extern iCoreShell* g_pCoreShell;

template<> 
KUiTongRecruitCentre* KUiWndSingleton<KUiTongRecruitCentre>::ms_Singleton	= NULL;

template<>
KUiSocialInfo       * KUiWndSingleton<KUiSocialInfo>::ms_Singleton          = NULL;

KUiTongRecruitCentre::KUiTongRecruitCentre(const CEGUI::String& id_name )
:KUiWndSingleton<KUiTongRecruitCentre>(id_name),d_CurLayerID(0),d_CurPageNo(0),d_CurSelectItem(0)
{
	
}

KUiTongRecruitCentre::~KUiTongRecruitCentre()
{

}

void KUiTongRecruitCentre::Show()
{
	KUiWndSingleton<KUiTongRecruitCentre>::Show();
	if (ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		if (ms_Singleton->d_CurLayerID == 0)
		{
			ms_Singleton->ShizuClicked();
		}
		else
		{
			switch (ms_Singleton->d_CurLayerID)
			{
			case enSULayer_Gens:
				{
					ms_Singleton->ShizuClicked();
				}
				break;

			case enSULayer_Tong:
				{
					ms_Singleton->ZhuhouClicked();
				}
				break;
			}
		}
	}//endif
	
}

void KUiTongRecruitCentre::CheckButton()
{
	if (ms_Singleton && d_PubBtn && d_DelBtn && d_JoinBtn)
	{
		SocietyInfoIndex tagSocietyIdx;
	    tagSocietyIdx.TemplateId	= enSUTplId_Tong;
		tagSocietyIdx.Layer         = d_CurLayerID;
		tagSocietyIdx.Operation     = enSUO_AddRecruitInfo; 

		if ( g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
		{
			d_PubBtn->setEnabled( true  );
		}//endif
		else
			d_PubBtn->setEnabled( false );

		tagSocietyIdx.Operation    =  enSUO_DelRecruitInfo;

		if ( g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
		{
			d_DelBtn->setEnabled( true  );
		}//endif
		else
			d_DelBtn->setEnabled( false );

		if (g_pCoreShell->GetGameData( GDI_CAN_JOIN_SOCIAL_LAYER, d_CurLayerID, NULL ) && d_CurSelectItem)
		{
			d_JoinBtn->setEnabled( true  );
		}//endif
		else
			d_JoinBtn->setEnabled( false );

	}//endif

}

void KUiTongRecruitCentre::Hide()
{
	KUiWndSingleton<KUiTongRecruitCentre>::Hide();

	if (ms_Singleton && ms_Singleton->m_pThisWnd && ms_Singleton->d_OperNotice )
	{
		ms_Singleton->d_OperNotice->hide();
	}//endif

	if (ms_Singleton && ms_Singleton->m_pThisWnd && ms_Singleton->d_Manu)
	{
		ms_Singleton->d_Manu->hide();
	}//end if

}

void KUiTongRecruitCentre::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		d_Close      = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/CloseBtn");
		d_ShizuBtn   = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/ShizuPageBtn");
		d_ZhuhouBtn  = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/ZhuhouPageBtn");
		d_PubBtn     = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/Pub");
        d_DelBtn     = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/Del");
	    d_JoinBtn    = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/ReqJoin"); 	
		d_PrePage    = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/PrePage");
		d_NextPage   = (TLButton *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/NextPage");	
		d_CurPage    = (TLStaticText * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/CurPage");
		d_CurPage    ->show();

		d_OperNotice =              ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongCentre/TongOperMgr");
		d_OperNotice ->hide();

		d_OperTip    = (TLStaticText * ) d_OperNotice->getChild("TaharezLook/TongCentre/TongOperMgr/Tip");
		d_OperOK     = (TLButton     * ) d_OperNotice->getChild("TaharezLook/TongCentre/TongOperMgr/OkBtn");
		d_OperCancel = (TLButton     * ) d_OperNotice->getChild("TaharezLook/TongCentre/TongOperMgr/CancelBtn");

		d_Manu       = ms_Singleton->m_pThisWnd ->getChild("TaharezLook/TongCentre/Menu");
		d_ManuChat   = (TLButton     * ) d_Manu ->getChild("TaharezLook/TongCentre/Menu/Chat");
		d_ManuJoin   = (TLButton     * ) d_Manu ->getChild("TaharezLook/TongCentre/Menu/Join");	
		d_Manu       ->hide();
	
		Window * pRoot = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
		if (pRoot)
		{
			m_pThisWnd->removeChildWindow(d_Manu);
			pRoot->addChildWindow(d_Manu);
		}//endif

		
		d_Close->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnClose, ms_Singleton));
		d_ShizuBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnShizuClicked, ms_Singleton));
		d_ZhuhouBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnZhuhouClicked, ms_Singleton));
		d_PubBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnPubClicked, ms_Singleton));
		d_DelBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnDelClicked, ms_Singleton));
		d_JoinBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnReqJoinClicked, ms_Singleton));
		d_OperOK->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnOperaOk, ms_Singleton));
        d_OperCancel->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnOperaCancel, ms_Singleton));
		d_PrePage->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnPrePage, ms_Singleton));
		d_NextPage->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnNextPage, ms_Singleton));

		d_PrePage->setZLevel(Window::Top);
		d_NextPage->setZLevel(Window::Top);
		d_CurPage->setZLevel(Window::Top);
		
		d_ShizuPage = ms_Singleton->m_pWindowManager->loadWindowLayout( "uisettings/layouts/ShizuRecruitPage.ls", "", "", NULL, NULL, true );
		ms_Singleton->m_pThisWnd->addChildWindow( d_ShizuPage );
		d_ShizuPage->hide();
		
		d_ZhuhouPage = ms_Singleton->m_pWindowManager->loadWindowLayout( "uisettings/layouts/ZhuhouRecruitPage.ls", "", "", NULL, NULL, true  );
	    ms_Singleton->m_pThisWnd->addChildWindow( d_ZhuhouPage );
		d_ZhuhouPage->hide();

		d_ManuChat ->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnManuChat, ms_Singleton));
		d_ManuJoin ->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnManuJoin, ms_Singleton));

		for (int i = 0;i< MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE;i++)
		{
			char     szBuff[256];
			sprintf(szBuff,"TaharezLook/ShizuRecruitPage/MemberList/Item%d",i);
			Window * pItem = d_ShizuPage->getChild(d_ShizuPage->getName() + "/MemberList")->getChild(szBuff);
			pItem->subscribeEvent(Static::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnItemClicked, ms_Singleton));
		
			sprintf(szBuff,"TaharezLook/ZhuhouRecruitPage/MemberList/Item%d",i);
			pItem          = d_ZhuhouPage->getChild(d_ZhuhouPage->getName() + "/MemberList")->getChild(szBuff);
			pItem->subscribeEvent(Static::EventMouseClick, Event::Subscriber(&KUiTongRecruitCentre::OnItemClicked, ms_Singleton));
		}//end for i

		if ( success_errorcode == ms_Singleton->m_pUiMDLManager->createDataSet( tong_operation ) )
		{
			return;
		}//endif
		
		TongOperParam   tagTongParam;
		IUIMDLDataset*  pDataset = NULL;
		ms_Singleton->m_pUiMDLManager->queryDataSet( tong_operation, &pDataset );

		if ( pDataset )
		{
			for ( int nIdx = enSUO_CreateUnit; nIdx < enSUO_Num; ++nIdx )
			{
				tagTongParam.nOperationID = (int)nIdx;
				tagTongParam.szName[0] = 0;
				pDataset->addDataRecord( &tagTongParam, sizeof(TongOperParam) );
			}//end for nIdx

		}//endif

	}//endif

}

bool KUiTongRecruitCentre::OnPrePage(const CEGUI::EventArgs & args)
{
	if (d_CurPageNo > 0)
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= d_CurLayerID;
		tagTongOper.nOperationID	= enSUO_GetRecruitInfo;
		tagTongOper.nPage           = d_CurPageNo - 1;
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return true;
		}//endif
		
		pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	
	}//endif
	
	return true;
}

bool KUiTongRecruitCentre::OnNextPage(const CEGUI::EventArgs & args)
{
	TongOperParam tagTongOper;
	ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
	tagTongOper.nTemplateID		= enSUTplId_Tong;
	tagTongOper.nLayerID		= d_CurLayerID;
	tagTongOper.nOperationID	= enSUO_GetRecruitInfo;
	tagTongOper.nPage           = d_CurPageNo + 1;
	
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return true;
	}//endif
	
	pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	
	
	return true;
}

bool KUiTongRecruitCentre::OnItemClicked(const CEGUI::EventArgs & args)
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		Window* pImageWnd = (Window*)pWinEvent->window;
		
		if ( pImageWnd )
		{
			CEGUI::colour col;
		    col.setARGB(0xff808080);

			//判断是否为该item为显示item
			String itemName = pImageWnd->getName() + "/name";

			TLStaticText * pName = (TLStaticText *)pImageWnd->getChild(itemName);
			if (pName && pName->getText()!="" && pName->getTextColours() != col)
			{
				Window * pMemberList = 0;
				
				switch (d_CurLayerID)
				{
				case enSULayer_Gens:
					{
						pMemberList = d_ShizuPage->getChild("TaharezLook/ShizuRecruitPage/MemberList");	
					}//end for case 
					break;
					
				case enSULayer_Tong:
					{
						pMemberList = d_ZhuhouPage->getChild("TaharezLook/ZhuhouRecruitPage/MemberList");
					}//end for case
					break;
					
				default:return true;
					}//endif

				for (int i = 0; i < MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE ; i++)
				{	
					char     szBuff[256];
					sprintf(szBuff,"/Item%d",i);

					Window * pItem = pMemberList->getChild(pMemberList->getName() + szBuff);
					
					if (pItem)
					{
						Window *       pHilight = pItem->getChild( pItem->getName() + "/HeightLight");
						
						if (pItem == pImageWnd)
						{
							d_CurSelectItem = pImageWnd;
							pHilight -> show();
							CheckButton();
						}//endif
						else
						{
							pHilight -> hide();	
						}//end else

					}//endif
					
				}//end for i

			}//endif

		}//endif
			
	}//endif	

	if (d_CurSelectItem)
	{
		MouseEventArgs & mouse = (MouseEventArgs &) args;
		switch (mouse.button)
		{
		case LeftButton:
			{
                if (d_Manu)
					d_Manu ->hide();
			}//end for case 
			break;

		case RightButton:
			{
				if (d_Manu)
				{
					
					Point newP    = MouseCursor::getSingleton().getPosition();
		
					d_Manu->setZLevel(Window::Top);
					d_Manu->setPosition(Absolute,newP);

					ShowManu();
				}//endif

			}//end for case
			break;	
		}//end for switch

	}//endif

	return true;
}

bool KUiTongRecruitCentre::OnClose(const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

bool KUiTongRecruitCentre::OnOperaOk(const CEGUI::EventArgs & args)
{
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return true;
	}//endif
	
	pTongOper->updateRecord( d_TongOper.nOperationID, &d_TongOper, sizeof( TongOperParam ) );
	
	//Refresh page
	TongOperParam tagTongOper;
	ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
	tagTongOper.nTemplateID		= enSUTplId_Tong;
	tagTongOper.nLayerID		= d_CurLayerID;
	tagTongOper.nOperationID	= enSUO_GetRecruitInfo;
	tagTongOper.nPage           = 0;
	
	pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	

	if (d_OperNotice)
	{
		d_OperNotice->hide();
	}//endif
	
	if (d_TongOper.nOperationID == enSUO_DelRecruitInfo )
	{
		if (d_CurLayerID == enSULayer_Gens)
			ClearShizuPage();
		else
			ClearZhuhouPage();
	}//endif

	return true;
}

bool KUiTongRecruitCentre::OnManuChat(const CEGUI::EventArgs & args )
{
	if ( d_CurSelectItem )
	{
		TLStaticText       * pOwerName =(TLStaticText       *)( d_CurSelectItem->getChild(d_CurSelectItem->getName() + "/owner"));	
		KUiChatInputWnd::GetSingleton().clearText();
		
		KUiChatInputWnd::GetSingleton().write("/");
		
		KUiChatInputWnd::GetSingleton().write(Utf8ToAnsi(pOwerName->getText()));
		KUiChatInputWnd::GetSingleton().write(" ");
		
		KUiChatInputWnd::GetSingleton().show();
	}//endif
	
	ms_Singleton->d_Manu->hide();

	return true;
}

bool KUiTongRecruitCentre::OnManuJoin(const CEGUI::EventArgs & args )
{
	if (d_CurSelectItem)
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= d_CurLayerID - 1;
		tagTongOper.nOperationID	= enSUO_ReqJoinHigherLevel;
		tagTongOper.nPage           = 0;
		
		TLStaticText       * pOwerName =(TLStaticText       *)( d_CurSelectItem->getChild(d_CurSelectItem->getName() + "/owner"));	
		strcpy(tagTongOper.szReceiveName,Utf8ToAnsi(pOwerName->getText()));
		
		IUIMDLDataset      * pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return true;
		}//endif
		
		pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	
	}//endif
	
	ms_Singleton->d_Manu->hide();

	return true;
}

void KUiTongRecruitCentre::ShowManu()
{
    if (d_Manu)
	{	
		if (d_CurSelectItem)
		{
			TLStaticText       * pOwerName =(TLStaticText       *)( d_CurSelectItem->getChild(d_CurSelectItem->getName() + "/owner"));	
			KUiPlayerBaseInfo pInfo;
			memset(&pInfo,0,sizeof(pInfo));
			
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(int)&pInfo,0);
			if (strcmp(pInfo.Name,Utf8ToAnsi(pOwerName->getText())) == 0)
			{
				d_Manu->disable();
			}//endif
			else
			{
				d_Manu->enable();
				if (d_JoinBtn)
					d_ManuJoin ->setEnabled(!d_JoinBtn->isDisabled(true));

			}//end else

			d_Manu     ->show();

		}//endif
		
		
	}//endif
	
}

bool KUiTongRecruitCentre::OnOperaCancel(const CEGUI::EventArgs & args)
{
	if (d_OperNotice)
	{
		d_OperNotice->hide();
	}//endif

	return true;
}

bool KUiTongRecruitCentre::OnZhuhouClicked(const CEGUI::EventArgs & args)
{
    ZhuhouClicked();
	return true;
}

bool KUiTongRecruitCentre::OnReqJoinClicked(const CEGUI::EventArgs & args )
{
	if (d_CurSelectItem)
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= d_CurLayerID - 1;
		tagTongOper.nOperationID	= enSUO_ReqJoinHigherLevel;
		tagTongOper.nPage           = 0;
		
		TLStaticText       * pOwerName =(TLStaticText       *)( d_CurSelectItem->getChild(d_CurSelectItem->getName() + "/owner"));	
		strcpy(tagTongOper.szReceiveName,Utf8ToAnsi(pOwerName->getText()));

		IUIMDLDataset      * pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return true;
		}//endif
		
		pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	
	}//endif
	
	return true;
}

bool KUiTongRecruitCentre::OnPubClicked(const CEGUI::EventArgs & args )
{
	ZeroMemory( &d_TongOper, sizeof(d_TongOper) );
	d_TongOper.nTemplateID	= enSUTplId_Tong;
	d_TongOper.nLayerID		= d_CurLayerID;
	d_TongOper.nOperationID	= enSUO_AddRecruitInfo;

	char *msg = KMessageCentre::GetMessage(tong_operation_message, 30 + d_CurLayerID - enSULayer_Gens );
	if (msg)
	{
        d_OperTip->setText(AnsiToUtf8(msg));
	}//endif

	if (d_OperNotice)
	{
		d_OperNotice->setZLevel(Window::Top);
		d_OperNotice->show();
	}//endif

	return true;
}

bool KUiTongRecruitCentre::OnDelClicked(const CEGUI::EventArgs & args )
{
	ZeroMemory( &d_TongOper, sizeof(d_TongOper) );
	d_TongOper.nTemplateID	= enSUTplId_Tong;
	d_TongOper.nLayerID		= d_CurLayerID;
	d_TongOper.nOperationID	= enSUO_DelRecruitInfo;
	
	char *msg = KMessageCentre::GetMessage(tong_operation_message, 32 + d_CurLayerID - enSULayer_Gens );
	if (msg)
	{
        d_OperTip->setText(AnsiToUtf8(msg));
	}//endif
	
	if (d_OperNotice)
	{
		d_OperNotice->setZLevel(Window::Top);
		d_OperNotice->show();
	}//endif

	return true;
}

bool KUiTongRecruitCentre::OnShizuClicked(const CEGUI::EventArgs & args)
{
    ShizuClicked();
	return true;
}

void KUiTongRecruitCentre::ShizuClicked()
{
	TongRecruitData data;
	
	ZeroMemory(&data,sizeof(data));
	data.nCurPageNum = 0;
	data.nLayer      = enSULayer_Gens;
	data.nPageNo     = 0;
	
	ms_Singleton->ShowShizuPage(data);

	TongOperParam tagTongOper;
	ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
	tagTongOper.nTemplateID		= enSUTplId_Tong;
	tagTongOper.nLayerID		= enSULayer_Gens;
	tagTongOper.nOperationID	= enSUO_GetRecruitInfo;
	tagTongOper.nPage           = 0;
	
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return ;
	}//endif

	pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	

}

void KUiTongRecruitCentre::ZhuhouClicked()
{
	TongRecruitData data;
	
	ZeroMemory(&data,sizeof(data));
	data.nCurPageNum = 0;
	data.nLayer      = enSULayer_Tong;
	data.nPageNo     = 0;
	
	ms_Singleton->ShowZhuhouPage(data);

	TongOperParam tagTongOper;
	ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
	tagTongOper.nTemplateID		= enSUTplId_Tong;
	tagTongOper.nLayerID		= enSULayer_Tong;
	tagTongOper.nOperationID	= enSUO_GetRecruitInfo;
	tagTongOper.nPage           = 0;
	
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return ;
	}//endif
	
	pTongOper->updateRecord( enSUO_GetRecruitInfo, &tagTongOper, sizeof( TongOperParam ) );	
}

void KUiTongRecruitCentre::UpdateData(const TongRecruitData & data )
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		ms_Singleton->d_CurPageNo     = data.nPageNo;
		if (ms_Singleton->d_CurPage)
		{
			int nCurPageNumber = ms_Singleton->d_CurPageNo + 1;
			char szPageNumber[32];
			szPageNumber[0] = 0;

			sprintf(szPageNumber,"%d",nCurPageNumber);

			ms_Singleton->d_CurPage->setText(AnsiToUtf8(szPageNumber));
		}//endif

		ms_Singleton->d_CurLayerID    = data.nLayer;
		ms_Singleton->d_CurSelectItem = NULL;

		switch (data.nLayer)
		{
		case enSULayer_Gens:
			{
                ms_Singleton->ShowShizuPage(data);
			}//end for case 
			break;
			
		case enSULayer_Tong:
			{
				ms_Singleton->ShowZhuhouPage(data);
			}//end for case 
			break;
		}//end for switch

	}//endif
}

void KUiTongRecruitCentre::ClearShizuPage()
{
	if (ms_Singleton && m_pThisWnd && d_ShizuPage)
	{
		char szBuff[256] = "";
		Window * pMemberList = d_ShizuPage->getChild("TaharezLook/ShizuRecruitPage/MemberList");
		if (!pMemberList)
			return ;

		for (int i = 0; i < MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE ; i++)
		{	
			sprintf(szBuff,"TaharezLook/ShizuRecruitPage/MemberList/Item%d",i);
			Window * pItem = pMemberList->getChild(szBuff);
			
			if (pItem)
			{
				Window * pHilight = pItem->getChild( pItem->getName() + "/HeightLight");
				Window * pName    = pItem->getChild( pItem->getName() + "/name");
				Window * pOwner   = pItem->getChild( pItem->getName() + "/owner");
				Window * pMemCount= pItem->getChild( pItem->getName() + "/membercount");
				Window * pTime    = pItem->getChild( pItem->getName() + "/time");

				pHilight ->hide();
				pName    ->setText("");
				pOwner   ->setText("");
				pMemCount->setText("");
				pTime    ->setText("");

			}//endif

		}//end for i;

	}//endif
}

void KUiTongRecruitCentre::ClearZhuhouPage()
{
	if (ms_Singleton && m_pThisWnd && d_ZhuhouPage)
	{
		char szBuff[256] = "";
		Window * pMemberList = d_ZhuhouPage->getChild("TaharezLook/ZhuhouRecruitPage/MemberList");
		if (!pMemberList)
			return ;
		
		for (int i = 0; i < MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE ; i++)
		{	
			sprintf(szBuff,"TaharezLook/ZhuhouRecruitPage/MemberList/Item%d",i);
			Window * pItem = pMemberList->getChild(szBuff);
			
			if (pItem)
			{
				Window * pHilight = pItem->getChild( pItem->getName() + "/HeightLight");
				Window * pName    = pItem->getChild( pItem->getName() + "/name");
				Window * pOwner   = pItem->getChild( pItem->getName() + "/owner");
				Window * pMemCount= pItem->getChild( pItem->getName() + "/membercount");
				Window * pCity    = pItem->getChild( pItem->getName() + "/city");
				Window * pPool    = pItem->getChild( pItem->getName() + "/pool");	
				Window * pTime    = pItem->getChild( pItem->getName() + "/time");
				
				pHilight ->hide();
				pName    ->setText("");
				pOwner   ->setText("");
				pMemCount->setText("");
				pCity    ->setText("");
				pPool    ->setText("");
				pTime    ->setText("");
				
			}//endif
			
		}//end for i;
		
	}//endif
}

void KUiTongRecruitCentre::ShowZhuhouPage(const TongRecruitData & data )
{
	if (d_ShizuPage)
		d_ShizuPage->hide();

	ClearZhuhouPage();

	d_CurLayerID = data.nLayer;

	char     szBuff[256] = "";
	Window * pMemberList = d_ZhuhouPage->getChild("TaharezLook/ZhuhouRecruitPage/MemberList");
	if (!pMemberList)
		return ;
	
	for (int i = 0; i < data.nCurPageNum ; i++)
	{	
		sprintf(szBuff,"TaharezLook/ZhuhouRecruitPage/MemberList/Item%d",i);
		Window * pItem = pMemberList->getChild(szBuff);
		
		if (pItem)
		{
			Window *       pHilight = pItem->getChild( pItem->getName() + "/HeightLight");
			TLStaticText * pName    = (TLStaticText *)pItem->getChild( pItem->getName() + "/name");
			TLStaticText * pOwner   = (TLStaticText *)pItem->getChild( pItem->getName() + "/owner");
			TLStaticText * pMemCount= (TLStaticText *)pItem->getChild( pItem->getName() + "/membercount");
			TLStaticText * pCity    = (TLStaticText *)pItem->getChild( pItem->getName() + "/city");
			TLStaticText * pPool    = (TLStaticText *)pItem->getChild( pItem->getName() + "/pool");	
			TLStaticText * pTime    = (TLStaticText *)pItem->getChild( pItem->getName() + "/time");
			
			pHilight ->hide();
			
			bool     bOnline  = data.nPageData[i].bOwnerOnline;
			CEGUI::colour col;
			if (!bOnline)
				col.setARGB(0xff808080);
			else
				col.setARGB(0xffffffff);

			pName    ->setText(AnsiToUtf8(data.nPageData[i].szUnitName));
			pName    ->setTextColours(col);
			pOwner   ->setText(AnsiToUtf8(data.nPageData[i].szOwnerName));
			pOwner   ->setTextColours(col);
			
			char  szMemberCount[32];
			sprintf(szMemberCount,"%d",data.nPageData[i].nSubUnitCount);
			
			pMemCount->setText(AnsiToUtf8(szMemberCount));
			pMemCount->setTextColours(col);

		    pCity    ->setText(AnsiToUtf8(data.nPageData[i].szCityMapName));
			pCity    ->setTextColours(col);
			pPool    ->setText(AnsiToUtf8(data.nPageData[i].szPoolMapName));
			pPool    ->setTextColours(col);

			char *msg = KMessageCentre::GetMessage(social_info, 10);

			char szDueTime[64];
			sprintf(szDueTime,msg,data.nPageData[i].nDueTimeDay);
			pTime    ->setText(AnsiToUtf8(szDueTime));
			pTime    ->setTextColours(col);
			
		}//endif
		
	}//end for i;

	d_ZhuhouPage->show();	
	CheckButton();
}

void KUiTongRecruitCentre::ShowShizuPage(const TongRecruitData & data )
{
	if (d_ZhuhouPage)
		d_ZhuhouPage->hide();

	ClearShizuPage();
	
	d_CurLayerID = data.nLayer;
	
	char szBuff[256] = "";
	Window * pMemberList = d_ShizuPage->getChild("TaharezLook/ShizuRecruitPage/MemberList");
	if (!pMemberList)
		return ;
	
	for (int i = 0; i < data.nCurPageNum ; i++)
	{	
		sprintf(szBuff,"TaharezLook/ShizuRecruitPage/MemberList/Item%d",i);
		Window * pItem = pMemberList->getChild(szBuff);
		
		if (pItem)
		{
			Window *       pHilight = pItem->getChild( pItem->getName() + "/HeightLight");
			TLStaticText * pName    = (TLStaticText *)pItem->getChild( pItem->getName() + "/name");
			TLStaticText * pOwner   = (TLStaticText *)pItem->getChild( pItem->getName() + "/owner");
			TLStaticText * pMemCount= (TLStaticText *)pItem->getChild( pItem->getName() + "/membercount");
			TLStaticText * pTime    = (TLStaticText *)pItem->getChild( pItem->getName() + "/time");
			
			pHilight ->hide();
			
			bool     bOnline  = data.nPageData[i].bOwnerOnline;
			CEGUI::colour col;
			if (!bOnline)
				col.setARGB(0xff808080);
			else
				col.setARGB(0xffffffff);
			
			pName    ->setText(AnsiToUtf8(data.nPageData[i].szUnitName));
			pName    ->setTextColours(col);
			pOwner   ->setText(AnsiToUtf8(data.nPageData[i].szOwnerName));
			pOwner   ->setTextColours(col);

			char  szMemberCount[32];
			sprintf(szMemberCount,"%d",data.nPageData[i].nSubUnitCount);

			pMemCount->setText(AnsiToUtf8(szMemberCount));
			pMemCount->setTextColours(col);
			
			char *msg = KMessageCentre::GetMessage(social_info, 10);
			char szDueTime[64];
			sprintf(szDueTime,msg,data.nPageData[i].nDueTimeDay);
			pTime    ->setText(AnsiToUtf8(szDueTime));
			pTime    ->setTextColours(col);
			
		}//endif
		
	}//end for i;

	d_ShizuPage ->show();
	CheckButton();
}

KUiSocialInfo::KUiSocialInfo(const CEGUI::String& id_name )
:KUiWndSingleton<KUiSocialInfo>(id_name)
{
	
}

void KUiSocialInfo::Init()
{
	if (ms_Singleton && m_pThisWnd)
	{
		d_NameTitle  = (TLStaticText   *)m_pThisWnd->getChild("TaharezLook/SocialInfo/NameTxt");
		d_Name       = m_pThisWnd->getChild("TaharezLook/SocialInfo/Name");

		d_OwnerTitle = (TLStaticText   *)m_pThisWnd->getChild("TaharezLook/SocialInfo/OwnerNameTxT");
		d_Owner      = m_pThisWnd->getChild("TaharezLook/SocialInfo/OwnerName");

		d_SubNumTitle= (TLStaticText   *)m_pThisWnd->getChild("TaharezLook/SocialInfo/SubUnitCountTxt");
		d_SubNum     = m_pThisWnd->getChild("TaharezLook/SocialInfo/SubUnitCount");

		d_AddtionTitle1 = (TLStaticText   *)m_pThisWnd->getChild("TaharezLook/SocialInfo/InfoAdition1Txt");
		d_Addtion1      = m_pThisWnd->getChild("TaharezLook/SocialInfo/InfoAdition1");

		d_AddtionTitle2 = (TLStaticText   *)m_pThisWnd->getChild("TaharezLook/SocialInfo/InfoAdition2Txt");
        d_Addtion2      = m_pThisWnd->getChild("TaharezLook/SocialInfo/InfoAdition2");

		d_AddtionTitle3 = (TLStaticText   *)m_pThisWnd->getChild("TaharezLook/SocialInfo/InfoAdition3Txt");
        d_Addtion3      = m_pThisWnd->getChild("TaharezLook/SocialInfo/InfoAdition3");

		d_CloseBtn      = (TLButton *)m_pThisWnd->getChild("TaharezLook/SocialInfo/Close");
		d_CloseBtn      ->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiSocialInfo::OnClose, ms_Singleton));
		
	}//endif

}

bool KUiSocialInfo::OnClose(const CEGUI::EventArgs & args)
{
	Hide();
	return true;
}

void KUiSocialInfo::Updata(const TongInfoData & data)
{
	if (ms_Singleton)
	{
		Show();
	}//endif

	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		switch (data.nLayer)
		{
		case enSULayer_Gens:
			{
				char* message = KMessageCentre::GetMessage(social_info, 1);
				ms_Singleton->d_NameTitle->setText(AnsiToUtf8(message));
				ms_Singleton->d_Name     ->setText(AnsiToUtf8(data.szUnitName));

				message       = KMessageCentre::GetMessage(social_info, 2);
				ms_Singleton->d_OwnerTitle  ->setText(AnsiToUtf8(message));
				ms_Singleton->d_Owner       ->setText(AnsiToUtf8(data.szOwnerName));

				message       = KMessageCentre::GetMessage(social_info, 3);
				ms_Singleton->d_SubNumTitle  ->setText(AnsiToUtf8(message));
				char          szBuff[128] = "";
				sprintf(szBuff,"%d",data.nSubUnitNum);
				ms_Singleton->d_SubNum       ->setText(AnsiToUtf8(szBuff));

				message       = KMessageCentre::GetMessage(social_info, 4);
				ms_Singleton->d_AddtionTitle1  ->setText(AnsiToUtf8(message));
				sprintf(szBuff,"%d",data.nPlayerAvgLevel);
				ms_Singleton->d_Addtion1       ->setText(AnsiToUtf8(szBuff));
				
				ms_Singleton->d_AddtionTitle2->hide();
				ms_Singleton->d_Addtion2->hide();
				
				ms_Singleton->d_AddtionTitle3->hide();
				ms_Singleton->d_Addtion3->hide();
			}//end for case
			break;

		case enSULayer_Tong:
			{
				char* message = KMessageCentre::GetMessage(social_info, 5);
				ms_Singleton->d_NameTitle->setText(AnsiToUtf8(message));
				ms_Singleton->d_Name     ->setText(AnsiToUtf8(data.szUnitName));
				
				message       = KMessageCentre::GetMessage(social_info, 6);
				ms_Singleton->d_OwnerTitle  ->setText(AnsiToUtf8(message));
				ms_Singleton->d_Owner       ->setText(AnsiToUtf8(data.szOwnerName));
				
				message       = KMessageCentre::GetMessage(social_info, 7);
				ms_Singleton->d_SubNumTitle  ->setText(AnsiToUtf8(message));
				char          szBuff[128] = "";
				sprintf(szBuff,"%d",data.nSubUnitNum);
				ms_Singleton->d_SubNum       ->setText(AnsiToUtf8(szBuff));
				
				message       = KMessageCentre::GetMessage(social_info, 8);
				ms_Singleton->d_AddtionTitle1  ->setText(AnsiToUtf8(message));
				ms_Singleton->d_Addtion1       ->setText(AnsiToUtf8(data.nCityMapName));
				
				message       = KMessageCentre::GetMessage(social_info, 9);
				ms_Singleton->d_AddtionTitle2  ->setText(AnsiToUtf8(message));
				ms_Singleton->d_Addtion2       ->setText(AnsiToUtf8(data.nPoolMapName));

				ms_Singleton->d_AddtionTitle2->show();
				ms_Singleton->d_Addtion2->show();

				ms_Singleton->d_AddtionTitle3->hide();
				ms_Singleton->d_Addtion3->hide();
			}//end for case
			break;
		case enSULayer_League:
			{
				ms_Singleton->updateLeagueInfo( data );
			}
			break;

		}//end for switch

	}//endif

}

void KUiSocialInfo::updateLeagueInfo( const TongInfoData& data )
{
	bool isControlValid = ( NULL != d_NameTitle ) && ( NULL != d_Name )
		&& ( NULL != d_OwnerTitle ) && ( NULL != d_Owner )
		&& ( NULL != d_SubNumTitle ) && ( NULL != d_SubNum )
		&& ( NULL != d_AddtionTitle1 ) && ( NULL != d_Addtion1 );

	if ( ! isControlValid )
	{
		return;
	}

	//联盟名称
	char* message = KMessageCentre::GetMessage( social_info, 11 );
	if ( NULL != message )
	{
		d_NameTitle->setText( AnsiToUtf8( message ) );
	}
	d_Name->setText( AnsiToUtf8( data.szUnitName ) );

	
	//盟主
	message = KMessageCentre::GetMessage(social_info, 12);
	if ( NULL != message )
	{
		d_OwnerTitle->setText( AnsiToUtf8( message ) );
	}
	d_Owner->setText( AnsiToUtf8( data.szOwnerName ) );
	
	//联盟所包含诸侯数量
	message = KMessageCentre::GetMessage( social_info, 13 );
	if ( NULL != message )
	{
		d_SubNumTitle->setText( AnsiToUtf8( message ) );
	}
	char szBuff[128] = "";
	sprintf( szBuff, "%d", data.nSubUnitNum );
	d_SubNum->setText( AnsiToUtf8 ( szBuff ) );
	
	//联盟的城市
	message = KMessageCentre::GetMessage( social_info, 8 );
	if ( NULL != message )
	{
		d_AddtionTitle1->setText( AnsiToUtf8( message ) );
	}
	d_Addtion1->setText( AnsiToUtf8( data.nCityMapName ) );

	//调整附加信息是否可见
	d_Addtion1->show();
	d_AddtionTitle1->show();

	if ( ( NULL != d_Addtion2 ) && ( NULL != d_AddtionTitle2 ) )
	{
		d_Addtion2->hide();
		d_AddtionTitle2->hide();
	}

	if ( ( NULL != d_Addtion3 ) && ( NULL != d_AddtionTitle3 ) )
	{
		d_Addtion3->hide();
		d_AddtionTitle3->hide();
	}
}