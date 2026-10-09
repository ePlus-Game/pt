//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 01/08/2007 14:39
//      File_base        : UiTongManager
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UiTongManager.h"
#include "../KMessageCentre.h"
#include "CoreShell.h"
#include "SocialComDef.h"
#include "UiErrorMessageBox.h"
#include "UiMessageBox.h"
#include "Ui/UiConfigManager.h"
#include "Ui/UiElem/TLVertScrollbar.h"
#include "Ui/UiElem/TLListbox.h"
#include "Ui/UiElem/TLMultiLineEditbox.h"
#include "Ui/UiCase/UiChatWindow.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "ChatDataDef.h"
#include "CEGUICoordConverter.h"
#include <sstream>
#include "Ui/UiCase/UiGenPersonalInfo.h"

using namespace std;

#define PAGE	"Page"
#define PAGEBTN "PageBtn"

extern iCoreShell* g_pCoreShell;

template<> 
KUiTongManager* KUiWndSingleton<KUiTongManager>::ms_Singleton	= NULL;

static ListboxTextGUIDItem tagShizuItem[TONGMEMBER_COUNT_PER_PAGE];
static ListboxTextGUIDItem tagZhuhouItem[TONGMEMBER_COUNT_PER_PAGE];

KUiTongManager::KUiTongManager( const CEGUI::String& id_name )
: KUiWndSingleton<KUiTongManager>( id_name )
, d_bShowOnline(true)
, d_CurLayerId(enSULayer_Gens)
, d_CurSubPageNo(0)
, d_IsTempOperValid(false)
, d_IsAllPageEventSubscribed(false)
, d_elementHight(0)
, d_CurCanShowNo(0)
, d_CurSelect(0)
,d_EditorFrame(NULL)
,d_Manu(0)
{
	ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );

	Window* pPage = NULL;
	pPage = ms_Singleton->m_pWindowManager->loadWindowLayout( "uisettings/layouts/ShizuPage.ls", "", "", NULL, NULL, true );
	ms_Singleton->m_pThisWnd->addChildWindow( pPage );
	pPage->hide();
	pPage->getChild( "TaharezLook/ShizuPage/ShowOnline")->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiTongManager::handleShowOnline, ms_Singleton));

	pPage = ms_Singleton->m_pWindowManager->loadWindowLayout( "uisettings/layouts/ZhuhouPage.ls", "", "", NULL, NULL, true  );
	ms_Singleton->m_pThisWnd->addChildWindow( pPage );
	pPage->hide();
	pPage->getChild( "TaharezLook/ZhuhouPage/ShowOnline")->subscribeEvent(Checkbox::EventMouseClick, Event::Subscriber(&KUiTongManager::handleShowOnline, ms_Singleton));

	pPage = ms_Singleton->m_pWindowManager->loadWindowLayout( "uisettings/layouts/LeaguePage.ls", "", "", NULL, NULL, true  );
	ms_Singleton->m_pThisWnd->addChildWindow( pPage );
	pPage->hide();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongManager/CloseBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongManager::handleExit, ms_Singleton));
	ms_Singleton->d_bProcessDataing = false;
	if ( success_errorcode == ms_Singleton->m_pUiMDLManager->createDataSet( tong_operation ) )
	{
		return;
	}

	TongOperParam tagTongParam;
	IUIMDLDataset*  pDataset = NULL;
	ms_Singleton->m_pUiMDLManager->queryDataSet( tong_operation, &pDataset );
	if ( pDataset )
	{
		for ( int nIdx = 0; nIdx < enSUO_Num; ++nIdx )
		{
			tagTongParam.nOperationID = (int)nIdx;
			tagTongParam.szName[0] = 0;
			pDataset->addDataRecord( &tagTongParam, sizeof(TongOperParam) );
		}
	}
	
	IUIMDLDataset* pOperDataset = NULL; 
	if ( success_errorcode != ms_Singleton->m_pUiMDLManager->queryDataSet( tong_dataset, &pOperDataset ) )
	{
		return;
	}
	if ( pOperDataset )
	{
		pOperDataset->setEventHandle( ms_Singleton );
	}

}

KUiTongManager::~KUiTongManager()
{

}

void	KUiTongManager::onCreate( UIMDLEvent& rEvent	)
{
}

void	KUiTongManager::onRelease( UIMDLEvent& rEvent	)
{
}

void    KUiTongManager::RefreshPage()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		switch (ms_Singleton->d_CurLayerId)
		{
		case enSULayer_Gens:
			{
				ms_Singleton->d_LayerIdx[ms_Singleton->d_CurLayerId].pPageWnd->hide();
				ms_Singleton->d_LayerIdx[ms_Singleton->d_CurLayerId].pPageWnd->show();
			}
			break;
			
		case enSULayer_Tong:
			{
				char szBuff[256];
				sprintf(szBuff,"%s%s",Utf8ToAnsi(ms_Singleton->d_LayerIdx[ms_Singleton->d_CurLayerId].pPageWnd->getName()),"/CurSelect");
				String pShizuName = ms_Singleton->d_LayerIdx[ms_Singleton->d_CurLayerId].pPageWnd->getChild(szBuff)->getText();
				
				FSGUID guid;

				for ( int nIdx=0; nIdx < TONGMEMBER_COUNT_PER_PAGE; ++nIdx )
				{				
					if (tagShizuItem[nIdx].getText() == pShizuName)
					{
						guid = tagShizuItem[nIdx].d_id;
					}//endif
				}//end for niDx

				if (guid.data[0] == 0)
					return ;
				
				TongOperParam tagTongOper;
				ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
				tagTongOper.nTemplateID		= enSUTplId_Tong;
				tagTongOper.nLayerID		= enSULayer_Tong;
				tagTongOper.nOperationID	= enSUO_GetSubList;
				tagTongOper.id				= guid;

				IUIMDLDataset* pTongOper = NULL;
				if ( success_errorcode != ms_Singleton->m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
				{
					Hide();
					return;
				}

				pTongOper->updateRecord( tagTongOper.nOperationID, &tagTongOper, sizeof( TongOperParam ) );

			}
			break;
		}//end for switch
		
	}//endif
}	

void	KUiTongManager::onChange( UIMDLEvent& rEvent	)
{
	int nIdx = rEvent.nRecordIndex;
	IUIMDLDataset* pTongDataset = rEvent.pDataSet;
	if ( pTongDataset	)
	{
		UIMDLDatasetRecord rRecord = pTongDataset->getDataRecord( nIdx );
		if ( rRecord.pRecordData )
		{
			TongData* pData = (TongData*)rRecord.pRecordData;
			d_bProcessDataing = false;
			if ( pData )
			{
				Window* pPage = d_LayerIdx[pData->nLayerID].pPageWnd;
				if ( pPage == NULL || pData->nLayerID != d_CurLayerId)
				{
					return;
				}//endif

				char     szOnline[128];
				sprintf( szOnline, "%s/ShowOnline",Utf8ToAnsi(pPage->getName()));
				Checkbox* pOnline = (Checkbox*)pPage->getChild( szOnline );
				
				if (pOnline)
				{
					d_bShowOnline = pOnline->isSelected();
				}//endif

				SocietyInfoIndex tagSocietyIdx;
				ZeroMemory( &tagSocietyIdx, sizeof(SocietyInfoIndex) );
				tagSocietyIdx.TemplateId	=	enSUTplId_Tong;
				tagSocietyIdx.Layer			=	pData->nLayerID;
				tagSocietyIdx.Operation		=	pData->nOperationServerID;
				SocietyLayerOperationController* pControl = getOperReceive(tagSocietyIdx);
				if ( pControl )
				{
					 for ( int nIdx = 0; nIdx < MAX_OPERATION_CONTROLLER_COUNT; ++nIdx )
					 {
						 if ( pControl[nIdx].ReturnController != NULL && pControl[nIdx].Controller != NULL
							 && strlen(pControl[nIdx].ReturnController)
							 )
						 {
							memcpy( pData->szReceiveName, pControl[nIdx].ReturnController, COMMON_CLIENT_MSG_LEN_128 );
							pData->szReceiveName[COMMON_CLIENT_MSG_LEN_128-1] = 0;
							if ( pData->eOperationClientID == get_society_baseinfo_name )
							{
								String strName = pPage->getName() + "/Name";
								pPage->getChild(strName)->setText( AnsiToUtf8( pData->szName ) );
								char szBuf[COMMON_CLIENT_MSG_LEN_64];
								sprintf( szBuf, "%d/%d", pData->tagCount.nOnlinePlayerCount, pData->tagCount.nMaxPlayerCount );
								strName = pPage->getName() + "/MemberCount";
								pPage->getChild(strName)->setText( AnsiToUtf8( szBuf ) );
								if (d_CurLayerId == enSULayer_Tong)
								{
									strName = pPage->getName() + "/City";
									pPage->getChild(strName)->setText(AnsiToUtf8(pData->szCityName));	

									strName = pPage->getName() + "/Pool";
									pPage->getChild(strName)->setText(AnsiToUtf8(pData->szPoolName));
								}//endif

								//特例，联盟页面需要显示盟主名字
								if ( enSULayer_League == d_CurLayerId )
								{
									strName = pPage->getName() + "/Leader";
									pPage->getChild( strName )->setText( AnsiToUtf8( pData->szOwnerName ) );
								}
							}
							else if ( pData->eOperationClientID == get_society_baseinfo_info )
							{
								String strName = pPage->getName() + "/Info";
								pPage->getChild(strName)->setText( AnsiToUtf8( pData->szTip ) );				
							}
							else if ( pData->eOperationClientID == get_society_baseinfo_count )
							{
								char szBuf[COMMON_CLIENT_MSG_LEN_64];
								sprintf( szBuf, "%d/%d", pData->tagCount.nOnlinePlayerCount, pData->tagCount.nMaxPlayerCount );
								String strName = pPage->getName() + "/MemberCount";
								pPage->getChild(strName)->setText( AnsiToUtf8( szBuf ) );
	
							}
							else if ( pData->eOperationClientID == get_society_memberlist_operation )
							{
								RedrawMemberList(d_LayerIdx[d_CurLayerId].pPageWnd,pData->memberList);
								return;
							}
							else if ( pData->eOperationClientID == get_society_zhuhoulist_operation )
							{
								RedrawLeagueMemberList( 
									d_LayerIdx[d_CurLayerId].pPageWnd,
									pData->memberList );

								return;
							}
							else if ( pData->eOperationClientID == get_society_shizulist_operation )
							{
								Listbox* pShizuList = NULL;
								if ( strcmp( "TaharezLook/ZhuhouPage/ShizuList", pData->szReceiveName) == 0 )
								{
									try
									{
										pShizuList = (Listbox*)(pPage->getChild(pData->szReceiveName));
										pShizuList->resetList();
									}
									catch (...)
									{
										continue;
									}
									
									if ( pShizuList == NULL )
									{
										return;
									}

									int nSubNum=0;
									
									for ( int nITestdx=0; nITestdx < TONGMEMBER_MAX_NUM; ++nITestdx )
									{
										if ( pData->memberList[nITestdx].szName[0] == NULL )
										{
											continue;
										}
										else
										{
											nSubNum++;
										}//endif
										
									}//end for nIdx

									if (nSubNum==0)
										return ;

									int   nIdxStart   =  d_CurSubPageNo * TONGMEMBER_COUNT_PER_PAGE;
									int   nIdStartMax = 0;
									nIdStartMax=nSubNum-1;

									if (nIdxStart>nIdStartMax)
									{
										d_CurSubPageNo = nIdStartMax / TONGMEMBER_COUNT_PER_PAGE;
										nIdxStart      = d_CurSubPageNo * TONGMEMBER_COUNT_PER_PAGE;
									}

									for ( int nIdx=0; nIdx < TONGMEMBER_COUNT_PER_PAGE; ++nIdx )
									{
										if ( pData->memberList[nIdx+nIdxStart].szName[0] == NULL )
										{
											continue;
										}

										tagShizuItem[nIdx].setText( AnsiToUtf8( pData->memberList[nIdx+nIdxStart].szName ) );
										tagShizuItem[nIdx].d_id = pData->memberList[nIdx+nIdxStart].guid;
										if ( tagShizuItem[nIdx].getSelectionBrushImage() == NULL )
										{
											tagShizuItem[nIdx].setSelectionBrushImage( String("txtbg"), String("txtbg10") );
										}
										pShizuList->addItem( &tagShizuItem[nIdx] );
									}
                                    
									((TLListbox *)pShizuList)->reCheckSize();
									
									((TLListbox *)pShizuList)->setShowVertScrollbar(false);
                                    ((TLListbox *)pShizuList)->setShowHorzScrollbar(false);
									

									pShizuList->setItemSelectState((size_t)0,true);
									Window* pItem  =  pShizuList;
									WindowEventArgs args(pItem);
									args.handled = false;

									handleListboxMouseClicked(args);    //选中Item0
				
								}
							}
							
						 }
					 }
				}
			}
		}
	}
}

void	KUiTongManager::Hide( void )
{
	if ( IsVisible() && ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		KUiWndSingleton<KUiTongManager>::Hide();
		LayerIndex::iterator it = ms_Singleton->d_LayerIdx.begin();
		for ( ; it != ms_Singleton->d_LayerIdx.end(); it++)
		{
			(*it).second.pPageWnd->setVisible(false);
		}

		if (KUiTongOperMgr::IsVisible())
			KUiTongOperMgr::Hide();

		if (ms_Singleton->d_EditorFrame && ms_Singleton->d_EditorFrame->isVisible(true))
			ms_Singleton->d_EditorFrame->hide();

		if (ms_Singleton->d_Manu && ms_Singleton->d_Manu->isVisible(true))
			ms_Singleton->d_Manu->hide();
	}
}

void	KUiTongManager::UpdateData( void )
{
	if ( ms_Singleton == NULL || ms_Singleton->m_pThisWnd == NULL )
	{
		return;
	}//endif
	
	if (!ms_Singleton->d_IsAllPageEventSubscribed)
	{
       ms_Singleton->ComInit();
	}//endif

	ms_Singleton->clearAllPage();
	ms_Singleton->d_LayerIdx.clear();

	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId	= enSUTplId_Tong;
	g_pCoreShell->GetGameData( GDI_GET_SOCIETY_TEMPLATE_INFO, (unsigned int)&tagSocietyIdx, (int)&ms_Singleton->d_TemplateInfo );
	
	int nTopLayer = 0;
	if (!g_pCoreShell->GetGameData( GDI_GET_SOCIETY_PLAYER, (unsigned int)&tagSocietyIdx, (int)&nTopLayer ) || nTopLayer == 0)
	{
		Hide();
		return ;
	}//endif

	//控制所有范围内的控件状态 
	for ( int nLayerIdx = 1; nLayerIdx <enSUTong_LayerNum; ++nLayerIdx )
	{
		tagSocietyIdx.Layer = nLayerIdx;
		
		g_pCoreShell->GetGameData( GDI_GET_SOCIETY_LAYER_INFO, (unsigned int)&tagSocietyIdx, (int)&ms_Singleton->d_LayerInfo[nLayerIdx] );
		
		if ( ms_Singleton->d_LayerInfo[nLayerIdx].Controller[0] == NULL )
		{
			continue;
		}//endif

		//ms_Singleton->d_LayerInfo[nLayerIdx].nLayerID = nLayerIdx;

		//1. 换页按钮
		char szBuff[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szBuff, "TaharezLook/TongManager/%s%s", ms_Singleton->d_LayerInfo[nLayerIdx].Controller, PAGEBTN );
		
		ms_Singleton->m_pThisWnd->getChild( szBuff )->setText( AnsiToUtf8( ms_Singleton->d_LayerInfo[nLayerIdx].Name ) );
		ms_Singleton->m_pThisWnd->getChild( szBuff )->setTooltipText( AnsiToUtf8( ms_Singleton->d_LayerInfo[nLayerIdx].Desc ));
		
		if (!ms_Singleton->d_IsAllPageEventSubscribed )	
			ms_Singleton->m_pThisWnd->getChild( szBuff )->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handleShowPage, ms_Singleton));
		
		ms_Singleton->m_pThisWnd->getChild( szBuff )->setUserData( &(ms_Singleton->d_LayerInfo[nLayerIdx])); //临时存储对应层的信息
		
		if (nLayerIdx <= nTopLayer)
			ms_Singleton->m_pThisWnd->getChild( szBuff )->setEnabled( true );
		else
			ms_Singleton->m_pThisWnd->getChild( szBuff )->setEnabled( false);

		//2.每一页
		sprintf( szBuff, "TaharezLook/%s%s", ms_Singleton->d_LayerInfo[nLayerIdx].Controller, PAGE );
		Window* pPage =	ms_Singleton->m_pThisWnd->getChild( szBuff );
		if ( pPage )
		{
			ms_Singleton->d_LayerIdx[nLayerIdx].pPageWnd = pPage;
			ms_Singleton->m_pThisWnd->addChildWindow( pPage );

			if ( nLayerIdx > nTopLayer )
			{
                pPage->setEnabled(false);
			}//endif
			else
			{
				pPage->setEnabled( true );
			}//end else

			//2.1 滑动条
			char szScroll[COMMON_CLIENT_MSG_LEN_128];
            sprintf( szScroll, "TaharezLook/%s%s%s",ms_Singleton->d_LayerInfo[nLayerIdx].Controller, PAGE ,"/Scrollbar" );
            TLVertScrollbar * pBar=(TLVertScrollbar *)pPage->getChild(szScroll);

			if (!ms_Singleton->d_IsAllPageEventSubscribed)
				pBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiTongManager::handleScrollBar, ms_Singleton));
            
			pBar->setScrollPosition(0.0f);
			
		}//endif
		else
		{
			return;
		}//end else

		//2.2 特例:暂时只能这样
		if (!ms_Singleton->d_IsAllPageEventSubscribed)
		{
			if (nLayerIdx == enSULayer_Tong)
			{
				 char szBuff[256];
				 sprintf(szBuff,"%s%s",Utf8ToAnsi(pPage->getName()),"/PopupZhuhou");
                 pPage->getChild(szBuff)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongManager::handlePopup, ms_Singleton));
			}//endif
	
			//2.3 成员列表操作
			for ( int nIdx = 0; nIdx < TONGMEMBER_COUNT_PER_PAGE; ++nIdx )
			{
				char szMem[COMMON_CLIENT_MSG_LEN_128];
				char szBuff[COMMON_CLIENT_MSG_LEN_128];
				char szBuffSecond[COMMON_CLIENT_MSG_LEN_128];
				sprintf( szMem,"%s/MemberList",Utf8ToAnsi(pPage->getName()));
				sprintf( szBuff,"%s/MemberList/Clipper",Utf8ToAnsi(pPage->getName()));
				sprintf( szBuffSecond, "%s/MemberList/Clipper/Item%d", Utf8ToAnsi(pPage->getName()), nIdx );
				pPage->getChild( szMem )->getChild( szBuff )->getChild(szBuffSecond)->subscribeEvent(Static::EventMouseClick, Event::Subscriber(&KUiTongManager::handleStaticImageMouseClicked, ms_Singleton));
				if ( pPage->getChild( szMem ) && pPage->getChild( szMem )->getChild( szBuff ) && pPage->getChild( szMem )->getChild( szBuff )->getChild( szBuffSecond ) )
				{
					pPage->getChild( szMem )->getChild( szBuff )->getChild( szBuffSecond )->subscribeEvent( 
						Static::EventMouseDoubleClick, 
						Event::Subscriber( &KUiTongManager::staticImage_MouseDoubleClicked, ms_Singleton ) );
				}
				
				if (ms_Singleton->d_elementHight == 0.0f)
					ms_Singleton->d_elementHight = pPage->getChild( szMem )->getChild( szBuff )->getChild(szBuffSecond)->getHeight(Absolute);
			}//end for nIdx


			if (nLayerIdx ==enSULayer_Tong )
			{
				ms_Singleton->d_HideList.push_back(pPage->getChild("TaharezLook/ZhuhouPage/ShizuList"));	
			}

		}//endif

		//3.每页各个控件的事件与状态
		for ( int nLayerOperIdx = 0; nLayerOperIdx < ms_Singleton->d_LayerInfo[nLayerIdx].OperationCount; ++nLayerOperIdx )
		{
			for ( int nLayerOperUiIdx = 0; nLayerOperUiIdx < MAX_OPERATION_CONTROLLER_COUNT; ++nLayerOperUiIdx )
			{
				if ( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller[0] != NULL )
				{
					ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].LayerID = nLayerIdx;
					ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Id = ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Id;
					pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->setText( "" );
					pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->setTooltipText( AnsiToUtf8( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Desc ) );
					pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->setText( AnsiToUtf8( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Name ) );
					tagSocietyIdx.Operation = ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Id;
					
					//状态
					if ( g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
					{
						pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->setEnabled( true );
					}
					else
					{
						pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->setEnabled( false );
					}

					//事件
					if (!ms_Singleton->d_IsAllPageEventSubscribed)
					{
						
						if ( strcmp( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Event, "ButtonClicked" )  == NULL )
						{
							pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTongManager::handleOperation, ms_Singleton));
						}//endif
						else if ( strcmp( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Event, "ListboxDoubleClicked" )  == NULL )
						{
							pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->subscribeEvent(Listbox::EventDoubleClickListItem, Event::Subscriber(&KUiTongManager::handleListboxMouseDBClicked, ms_Singleton));
						}
						else if ( strcmp( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Event, "ListboxClicked" )  == NULL )
						{
							pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->subscribeEvent(Listbox::EventMouseClick, Event::Subscriber(&KUiTongManager::handleListboxMouseClicked, ms_Singleton));
						}
						else if ( strcmp( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Event, "StaticImageClicked" )  == NULL )
						{
							//Done before
						}
						else if (  strcmp( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Event, "OnShown" )  == NULL  )
						{
							if (!ms_Singleton->d_IsAllPageEventSubscribed )
								pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiTongManager::handleRequestMemberList, ms_Singleton));
						}
						
						pPage->getChild( ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx].Controller )->setUserData( &ms_Singleton->d_LayerInfo[nLayerIdx].Operations[nLayerOperIdx].Controllers[nLayerOperUiIdx] );

					}
				}
			}
		}
	}

	ms_Singleton->d_IsAllPageEventSubscribed = true;
}

bool    KUiTongManager::handleScrollBar(const CEGUI::EventArgs& args )
{
	if (d_CurCanShowNo<=1)
		return true;

	Window *       pPage  =  d_LayerIdx[d_CurLayerId].pPageWnd;
	String strOriginName  =  pPage->getName() + "/MemberList";
	String strName        =  pPage->getName() + "/MemberList"+"/Clipper";		
	String strScroll      =  pPage->getName() + "/Scrollbar";

	Window* pList        =  (Window*)pPage->getChild(strOriginName)->getChild(strName);
	Window * pClipper     =  pPage->getChild(strOriginName);
    TLVertScrollbar * pBar=  (TLVertScrollbar *)pPage->getChild(strScroll);

	float  totalShowHight = (d_CurCanShowNo) * d_elementHight;
	float scrollPos = pBar->getScrollPosition();
	if ( totalShowHight > pClipper->getHeight(Absolute) )
	{
			float ypos = (totalShowHight - pClipper->getHeight(Absolute)) * scrollPos;
			Point pos;
			pos.d_x = pList->getPosition(Absolute).d_x;
			pos.d_y = 0 - ypos;
			pList->setPosition(Absolute, pos);
	}
	else
	{
			pList->setPosition(Absolute,Point(0, 0));
	}

	return true;
}

bool    KUiTongManager::handlePopup(const CEGUI::EventArgs& args )
{
	CEGUI::WindowEventArgs  &  winArgs =  (CEGUI::WindowEventArgs  &)args;
	Window * pWindow = winArgs.window;

	const char   *  szPageName = Utf8ToAnsi(d_LayerIdx[d_CurLayerId].pPageWnd->getName());
	char            szShizuPopup[256];
	sprintf(szShizuPopup,"%s%s",szPageName,"/PopupZhuhou");

	if (strcmp(Utf8ToAnsi(pWindow->getName()),szShizuPopup)==0)
	{
		char        szListName[256];
		sprintf(szListName,"%s/%s",Utf8ToAnsi(d_LayerIdx[d_CurLayerId].pPageWnd->getName()),"ShizuList");
        
		((TLListbox *)d_LayerIdx[d_CurLayerId].pPageWnd->getChild(szListName))->setStaticBackGroudImage("ty_tip1_tu","full_image");
		d_LayerIdx[d_CurLayerId].pPageWnd->getChild(szListName)->setZLevel(Window::Top);
        d_LayerIdx[d_CurLayerId].pPageWnd->getChild(szListName)->show();
	}//endif
	
	return true;
}

void	KUiTongManager::Show( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		SocietyInfoIndex tagSocietyIdx;
		tagSocietyIdx.TemplateId	= enSUTplId_Tong;
		
		int nTopLayer = 0;
		g_pCoreShell->GetGameData( GDI_GET_SOCIETY_PLAYER, (unsigned int)&tagSocietyIdx, (int)&nTopLayer );
		
		if ( nTopLayer > 0 )
		{
			KUiWndSingleton<KUiTongManager>::Show();
			ms_Singleton->hideAllPage();
		
			LayerIndex::iterator it = ms_Singleton->d_LayerIdx.begin();
			if (it!=ms_Singleton->d_LayerIdx.end() && (*it).second.pPageWnd )
			{
				(*it).second.pPageWnd->show();
				ms_Singleton->d_CurLayerId = it->first;
			}//endif
			
		}//endif

	}
}

void    KUiTongManager::ComInit()
{
    if (ms_Singleton)
	{
       ms_Singleton->d_EditorFrame = m_pThisWnd->getChild("TaharezLook/TongManager/InfoEditor");
       ms_Singleton->d_EditorFrame->getChild("TaharezLook/TongManager/InfoEditor/Ok")->subscribeEvent(TLButton::EventMouseClick,Event::Subscriber(&KUiTongManager::handleEditOk, ms_Singleton));
	   ms_Singleton->d_EditorFrame->getChild("TaharezLook/TongManager/InfoEditor/Cancel")->subscribeEvent(TLButton::EventMouseClick,Event::Subscriber(&KUiTongManager::handleCancel, ms_Singleton));
	   ms_Singleton->d_EditorFrame->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiTongManager::handleKeyDown, this));
	   ms_Singleton->d_EditorFrame->setZLevel(Window::Top);
	   ms_Singleton->d_EditorFrame->hide();
	  ((TLMultiLineEditbox *) ms_Singleton->d_EditorFrame->getChild("TaharezLook/TongManager/InfoEditor/Edit"))->setMaxTextLength(255);

       ms_Singleton->d_Manu        = m_pThisWnd->getChild("TaharezLook/TongManager/TongMenu");
       ms_Singleton->d_AddFriend   = d_Manu->getChild("TaharezLook/TongManager/TongMenu/AddFriend");
       ms_Singleton->d_Invite      = d_Manu->getChild("TaharezLook/TongManager/TongMenu/Invite" );
	   ms_Singleton->d_Chat        = d_Manu->getChild("TaharezLook/TongManager/TongMenu/Chat");
       ms_Singleton->d_Detail      = d_Manu->getChild("TaharezLook/TongManager/TongMenu/Detail");
       ms_Singleton->d_ChangeOwner = d_Manu->getChild("TaharezLook/TongManager/TongMenu/ChangeOwner");
	   ms_Singleton->d_Kick        = d_Manu->getChild("TaharezLook/TongManager/TongMenu/Kick");
       ms_Singleton->d_PreventChat = d_Manu->getChild("TaharezLook/TongManager/TongMenu/PreventChat");
 
       ms_Singleton->d_AddFriend      ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handleAddFriend, ms_Singleton));
       ms_Singleton->d_Invite         ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handleInvide, ms_Singleton));
	   ms_Singleton->d_Chat           ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handleChat, ms_Singleton));
       ms_Singleton->d_Detail         ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handleDetail, ms_Singleton));
       ms_Singleton->d_ChangeOwner    ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handledChangeOwner, ms_Singleton));
	   ms_Singleton->d_Kick           ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handledKick, ms_Singleton));
       ms_Singleton->d_PreventChat    ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongManager::handlePreventChat, ms_Singleton));

	   d_Manu->hide();
	   
	   Window * pRoot = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
	   if (pRoot)
	   {
		   m_pThisWnd->removeChildWindow(d_Manu);
		   pRoot->addChildWindow(d_Manu);
	   }//endif

	   d_Manu->setZLevel(Window::Top);

	   GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseClick,     Event::Subscriber(&KUiTongManager::hideListCheck, this));
       d_HideList.push_back(d_Manu);
	}//endif
}

bool    KUiTongManager::handleKeyDown(const CEGUI::EventArgs& args )
{
	return true;
}

bool    KUiTongManager::handleCancel(const CEGUI::EventArgs& args )
{
    ms_Singleton->d_EditorFrame->hide();
	return true;
}

bool KUiTongManager::handleAddFriend 				( const CEGUI::EventArgs& args			)
{
	if ( d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0])
	{
		g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_ADD, (unsigned int)d_memberList[d_CurSelect].szName, CHAT::GROUPID_NONE  );
	}//endif
	
	ms_Singleton->d_Manu->hide();
	
    return true;
}

bool KUiTongManager::handleInvide					( const CEGUI::EventArgs& args			)
{
	
	if ( d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0]
		)
	{
		KUiPlayerItem tagPlayer;
		ZeroMemory(&tagPlayer, sizeof(KUiPlayerItem));
		
		strncpy( tagPlayer.Name, d_memberList[d_CurSelect].szName, CLIENT_NAME_AND_TITLE_MAX + 1);
		
		KUiPlayerTeam	TeamInfo;
		TeamInfo.cNumMember = 0;
		g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
		if ( ((int)(TeamInfo.cNumMember)) <= 5 )
		{
			if (TeamInfo.cNumMember == 0)
			{
				g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
			}//endif
			
			g_pCoreShell->TeamOperation( TEAM_OI_INVITE_BY_NAME, (unsigned int)&tagPlayer, NULL );
		}
		else
		{
			char *msg = KMessageCentre::GetMessage(team_message, 1);
			KUiChannelCentre::GetSingleton().toSysMsg(msg);
		}//end else
		
	}//endif
	ms_Singleton->d_Manu->hide();
    return true;
}

bool KUiTongManager::handleChat  					( const CEGUI::EventArgs& args			)
{
	if ( d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0])
	{
		
		KUiChatInputWnd::GetSingleton().clearText();
		
		KUiChatInputWnd::GetSingleton().write("/");
		
		KUiChatInputWnd::GetSingleton().write(d_memberList[d_CurSelect].szName);
		KUiChatInputWnd::GetSingleton().write(" ");
		
		KUiChatInputWnd::GetSingleton().show();
	}
	
	ms_Singleton->d_Manu->hide();
	return true;
}

bool KUiTongManager::handleDetail        			( const CEGUI::EventArgs& args			)
{
	if (d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0])
	{
		ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;
		g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)d_memberList[d_CurSelect].szName, NULL );
	}
	
	ms_Singleton->d_Manu->hide();
	return true;
}

bool KUiTongManager::handledChangeOwner	            ( const CEGUI::EventArgs& args			)
{	
	if ( d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0])
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= d_TemplateInfo.Id;
		tagTongOper.nLayerID		= d_CurLayerId;
		tagTongOper.nOperationID	= enSUO_ChangeOwner;
		tagTongOper.szName[0]		= '\0';
		tagTongOper.id				= d_SigleTarget;
		tagTongOper.szTip[COMMON_CLIENT_MSG_LEN_256-1] = 0;
		tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128-1] = '\0';
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}
		
		pTongOper->updateRecord( enSUO_ChangeOwner, &tagTongOper, sizeof( TongOperParam ) );	
		
		RefreshPage();
	}//endif

	ms_Singleton->d_Manu->hide();
	return true;
}

bool KUiTongManager::handledKick		                ( const CEGUI::EventArgs& args			)
{
	if ( d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0])
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= d_TemplateInfo.Id;
		tagTongOper.nLayerID		= d_CurLayerId;
		tagTongOper.nOperationID	= enSUO_RemoveSubUnit;
		tagTongOper.szName[0]		= '\0';
		tagTongOper.id				= d_id;
		tagTongOper.szTip[COMMON_CLIENT_MSG_LEN_256-1] = 0;
		tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128-1] = '\0';
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}
		
		bool bNotice=true;
		
		KUiTongOperMgr::Show( tagTongOper ,bNotice);
		
	}//endif
	
	ms_Singleton->d_Manu->hide();
	return true;
}

bool KUiTongManager::handlePreventChat           	( const CEGUI::EventArgs& args			)
{
	if ( ( d_CurSelect >= 0 && d_CurSelect < TONGMEMBER_MAX_NUM && d_memberList[d_CurSelect].szName[0]) &&
		 (d_PreventChat->getID()==enSUO_ForbidChat || d_PreventChat->getID()==enSUO_UnForbidChat)
		 )
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= d_TemplateInfo.Id;
		tagTongOper.nLayerID		= d_CurLayerId;
		tagTongOper.nOperationID	= d_PreventChat->getID();
		tagTongOper.szName[0]		= '\0';
		tagTongOper.id				= d_SigleTarget;
		tagTongOper.szTip[COMMON_CLIENT_MSG_LEN_256-1] = 0;
		tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128-1] = '\0';
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}

		pTongOper->updateRecord( d_PreventChat->getID(), &tagTongOper, sizeof( TongOperParam ) );	
		
		RefreshPage();	
	}
	ms_Singleton->d_Manu->hide();
	return true;
}
 
bool KUiTongManager::hideListCheck                   ( const CEGUI::EventArgs& args          )
{
	std::list<Window *>::iterator it= d_HideList.begin();
    while (it!=d_HideList.end())
	{
        Window * pWindow = *it;
		
		const MouseEventArgs & mouseArgs = (const MouseEventArgs &)  args;	
		Point localPos(CEGUI::CoordConverter::screenToWindow(*pWindow, mouseArgs.position));
		
		if (localPos.d_x < 0 - 40.0f || localPos.d_x > pWindow->getSize(Absolute).d_width + 40.0f|| localPos.d_y < 0 - 40.0f || localPos.d_y >pWindow->getSize(Absolute).d_height + 40.0f )
		{
			if (pWindow->isVisible(true))
				pWindow->hide();
		}//endif
		
		++it;
	}//end for while

	return false;
}

bool    KUiTongManager::handleEditOk(const CEGUI::EventArgs& args )
{
	IUIMDLDataset* pTongOper = NULL;

	if (strlen(Utf8ToAnsi( ((TLMultiLineEditbox *)d_EditorFrame->getChild("TaharezLook/TongManager/InfoEditor/Edit"))->getText() )) >= MAXSIZE_ANNOUNCEMENT )
	{
		KUiChannelCentre::GetSingleton().toSysMsg(PUB_ANUCMENT_TOO_LONG);
		return true;
	}//endif

	strncpy( d_TempEditOper.szTip, Utf8ToAnsi( ((TLMultiLineEditbox *)d_EditorFrame->getChild("TaharezLook/TongManager/InfoEditor/Edit"))->getText() ), COMMON_CLIENT_MSG_LEN_512 );

	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return false;
	}//endif

	char  szBuff[256];
	sprintf(szBuff,"%s%s",Utf8ToAnsi(d_LayerIdx[d_CurLayerId].pPageWnd->getName()),"/Info");
	((TLMultiLineEditbox *)d_LayerIdx[d_CurLayerId].pPageWnd->getChild(szBuff))->setText(AnsiToUtf8(d_TempEditOper.szTip));

	pTongOper->updateRecord(d_TempEditOper.nOperationID, &d_TempEditOper, sizeof( TongOperParam ) );	

    ms_Singleton->d_EditorFrame->hide();
	return true;
}

bool	KUiTongManager::handleOperation( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		SocietyLayerOperationController* pOper = (SocietyLayerOperationController*)(pWinEvent->window->getUserData());
	
		if ( pOper == NULL )
		{
			Hide();
			return false;
		}//endif

		if (pOper->Id == enSUO_PubAnnouncement )
		{
			char  szBuff[256];
			sprintf(szBuff,"%s%s",Utf8ToAnsi(d_LayerIdx[d_CurLayerId].pPageWnd->getName()),"/Info");
			
			TLMultiLineEditbox * pSourEditBox = (TLMultiLineEditbox *) d_LayerIdx[d_CurLayerId].pPageWnd->getChild(szBuff);
             ((TLMultiLineEditbox *)d_EditorFrame->getChild("TaharezLook/TongManager/InfoEditor/Edit"))->setText(pSourEditBox->getText());

            d_EditorFrame->show();
			ZeroMemory( &d_TempEditOper, sizeof(d_TempEditOper) );
			d_TempEditOper.nTemplateID  = d_TemplateInfo.Id;
            d_TempEditOper.nLayerID     = pOper->LayerID;
            d_TempEditOper.nOperationID = pOper->Id;
            d_TempEditOper.szName[0]    = '\0';
            d_TempEditOper.id           = d_id;
			memcpy( d_TempEditOper.szReceiveName, pOper->ReturnController, COMMON_CLIENT_MSG_LEN_128 );
		    d_TempEditOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128-1] = '\0';

			return true;
		}//endif

		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= d_TemplateInfo.Id;
		tagTongOper.nLayerID		= pOper->LayerID;
		tagTongOper.nOperationID	= pOper->Id;
		tagTongOper.szName[0]		= '\0';
	
		tagTongOper.id				= d_id;
	
		if ( tagTongOper.nOperationID == enSUO_GetPrePageSubList )
		{
			String   strOriginName = d_LayerIdx[d_CurLayerId].pPageWnd->getName() + "/MemberList";
			String   strName       = d_LayerIdx[d_CurLayerId].pPageWnd->getName() + "/MemberList"+"/Clipper";		
			Window * pList         = (Window*)d_LayerIdx[d_CurLayerId].pPageWnd->getChild(strOriginName)->getChild(strName);
			Point    pos           = pList->getPosition(Absolute);
			pos.d_y                -= 20;
			pList->setPosition(Absolute,pos);

			return true;
		}//endif

		if ( tagTongOper.nOperationID == enSUO_GetNextPageSubList )
		{
			String   strOriginName = d_LayerIdx[d_CurLayerId].pPageWnd->getName() + "/MemberList";
			String   strName       = d_LayerIdx[d_CurLayerId].pPageWnd->getName() + "/MemberList"+"/Clipper";		
			Window * pList         = (Window*)d_LayerIdx[d_CurLayerId].pPageWnd->getChild(strOriginName)->getChild(strName);
			Point    pos           = pList->getPosition(Absolute);
			pos.d_y                += 20;

			pList->setPosition(Absolute,pos);
		    return true;
		}//endif

		if ( pOper->ReturnController[0] != NULL )
		{
			memcpy( tagTongOper.szTip, Utf8ToAnsi( pWinEvent->window->getParent()->getChild(pOper->ReturnController)->getText() ), COMMON_CLIENT_MSG_LEN_256 );
		}//endif

		tagTongOper.szTip[COMMON_CLIENT_MSG_LEN_256-1] = 0;
		memcpy( tagTongOper.szReceiveName, pOper->ReturnController, COMMON_CLIENT_MSG_LEN_128 );
		tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128-1] = '\0';
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}
		if ( !pOper->NeedConfirm )
		{
			pTongOper->updateRecord( pOper->Id, &tagTongOper, sizeof( TongOperParam ) );	
		}
		else
		{
			bool bNotice=false;
			if (/*d_ToltalSubCount==2 &&*/ (pOper->Id==enSUO_RemoveSubUnit || pOper->Id==enSUO_LeaveUnit))
			{
               bNotice=true;
			}//endif

			KUiTongOperMgr::Show( tagTongOper , bNotice);
		}
		
	}
	return true;
}

bool	KUiTongManager::handleListboxMouseDBClicked( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		ListboxItem* lItem = static_cast<Listbox*>(pWinEvent->window)->getFirstSelectedItem();
		ListboxTextGUIDItem *Item = static_cast<ListboxTextGUIDItem*>(lItem);
		if ( Item )
		{
			SocietyLayerOperationController* pOper = (SocietyLayerOperationController*)(pWinEvent->window->getUserData());
			if ( pOper == NULL )
			{
				Hide();
				return false;
			}
			TongOperParam tagTongOper;
			ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
			tagTongOper.nTemplateID		= d_TemplateInfo.Id;
			tagTongOper.nLayerID		= pOper->LayerID;
			tagTongOper.nOperationID	= pOper->Id;
			tagTongOper.id				= Item->d_id;
			memcpy( tagTongOper.szReceiveName, pOper->ReturnController, COMMON_CLIENT_MSG_LEN_128 );
			tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
			IUIMDLDataset* pTongOper = NULL;
			if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
			{
				Hide();
				return false;
			}
			pTongOper->updateRecord( pOper->Id, &tagTongOper, sizeof( TongOperParam ) );

			FSGUID  invalid;
		}
	}
	return true;
}

bool	KUiTongManager::handleListboxMouseClicked( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		//clearAllPage();
		ListboxItem* lItem = static_cast<Listbox*>(pWinEvent->window)->getFirstSelectedItem();
		
		if (lItem == NULL && static_cast<Listbox*>(pWinEvent->window)->getItemCount())
		{
			static_cast<Listbox*>(pWinEvent->window)->setItemSelectState((int)0,true);
			lItem  = static_cast<Listbox*>(pWinEvent->window)->getListboxItemFromIndex(0);
		}//endif

		pWinEvent->window->hide();

		ListboxTextGUIDItem *Item = static_cast<ListboxTextGUIDItem*>(lItem);
		if ( Item )
		{
			if (d_CurLayerId == enSULayer_Tong )
			{
				d_id = Item->d_id;
				d_id.data[32] = 0;

				char szBuff[256];
				sprintf(szBuff,"%s%s",Utf8ToAnsi(d_LayerIdx[d_CurLayerId].pPageWnd->getName()),"/CurSelect");
				d_LayerIdx[d_CurLayerId].pPageWnd->getChild(szBuff)->setText(Item->getText());
			}//endif

			SocietyLayerOperationController* pOper = (SocietyLayerOperationController*)(pWinEvent->window->getUserData());
			if ( pOper == NULL )
			{
				Hide();
				return false;
			}//endif

			TongOperParam tagTongOper;
			ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
			tagTongOper.nTemplateID		= d_TemplateInfo.Id;
			tagTongOper.nLayerID		= pOper->LayerID;
			tagTongOper.nOperationID	= pOper->Id;
			tagTongOper.id				= Item->d_id;
			memcpy( tagTongOper.szReceiveName, pOper->ReturnController, COMMON_CLIENT_MSG_LEN_128 );
			tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
			IUIMDLDataset* pTongOper = NULL;
			if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
			{
				Hide();
				return false;
			}
			pTongOper->updateRecord( pOper->Id, &tagTongOper, sizeof( TongOperParam ) );

		}//endif
	}
	return true;
}

void KUiTongManager::RedrawLeagueMemberList( Window * pPage, TongPageData *  pMemberList )
{
	ms_Singleton->clearAllPage();

	Window* pList = pPage->getChild( pPage->getName() + "/MemberList" );
	if ( NULL == pList )
	{
		return;
	}

	Window* pClipper = pList->getChild( pList->getName() + "/Clipper" );
	if ( NULL == pClipper )
	{
		return;
	}

	ZeroMemory( d_memberList, sizeof( d_memberList ) );
	memcpy( d_memberList, pMemberList, sizeof( d_memberList ) );
	
	for ( int i = 0; i < TONGMEMBER_MAX_NUM ; i++ )
	{
		if ( 0 == pMemberList[i].szName[0] )
		{
			break;
		}

		StaticText* pName			= NULL;
		StaticText* pMemberCount	= NULL;
		StaticText* pCity			= NULL;
		StaticText* pFenxingchi		= NULL;

		String itemName = pClipper->getName() + "/Item" + iToString( i );

		Window* pItem =NULL;

#ifndef _DEBUG
	try
	{
#endif
		pItem	= pClipper->getChild( itemName );
		pItem->setID( i );
		pItem->setUserString( "guid", Utf8ToAnsi( pMemberList[i].guid ) );

		pName			= static_cast< StaticText * >( pItem->getChild( pItem->getName() + "/Name" ) );
		pMemberCount	= static_cast< StaticText * >( pItem->getChild( pItem->getName() + "/MemberCount" ) );
		pCity			= static_cast< StaticText * >( pItem->getChild( pItem->getName() + "/City" ) );
		pFenxingchi		= static_cast< StaticText * >( pItem->getChild( pItem->getName() + "/Fenxingchi" ) );
#ifndef _DEBUG
	}
	catch (...)
	{
		return;
	}
#endif
		pName->setText( AnsiToUtf8( pMemberList[i].szName ) );
		pMemberCount->setText( iToString( pMemberList[i].nSubUnitNum ) );
		//pCity->setText( AnsiToUtf8( pMemberList[i].szCityName ) );
		pCity->setText( AnsiToUtf8( "-" ) );
		pFenxingchi->setText( AnsiToUtf8( pMemberList[i].szPoolName ) );
	}

	//选中Item0
	Window* pItem	= pClipper->getChild( pClipper->getName() + "/Item0" );
	WindowEventArgs args( pItem );
	args.handled = false;
	handleStaticImageMouseClicked( args );    
}

void    KUiTongManager::RedrawMemberList(Window * pPage,TongPageData * pMemberList )
{
	int  nSubNum=0;
	int  nOnlineNum=0;
	
	for ( int nITestdx=0; nITestdx < TONGMEMBER_MAX_NUM; ++nITestdx )
	{
		if ( pMemberList[nITestdx].szName[0] == NULL )
		{
			continue;
		}
		else
		{
			if (pMemberList[nITestdx].bOnline)
				nOnlineNum++;
			
			nSubNum++;
		}//endelse
		
	}//end for nIdx
	
	ms_Singleton->clearAllPage();
	String strOriginName = pPage->getName() + "/MemberList";
	String strName       = pPage->getName() + "/MemberList"+"/Clipper";		
	Window * pList       = (Window*)pPage->getChild(strOriginName)->getChild(strName);
	Window * pCliper     = pPage->getChild(strOriginName);
	if ( pList == NULL )
	{
		return;
	}//endif

	if (d_bShowOnline)
		d_CurCanShowNo = nOnlineNum;
	else
		d_CurCanShowNo = nSubNum;
	
	if ((!d_bShowOnline && nSubNum>0) || (d_bShowOnline && nOnlineNum>0))
	{
		SortMenberList(pMemberList);
		memcpy(d_memberList,pMemberList,sizeof(d_memberList));
		
		int   nIdxStart   =  d_CurSubPageNo * TONGMEMBER_COUNT_PER_PAGE;
		int   nIdStartMax =  0;
		if (d_bShowOnline)
		{
			nIdStartMax = nOnlineNum-1;
		}
		else
		{
			nIdStartMax=nSubNum-1;
		}//endif
		
		if (nIdxStart>nIdStartMax)
		{
			d_CurSubPageNo = nIdStartMax / TONGMEMBER_COUNT_PER_PAGE;
			nIdxStart      = d_CurSubPageNo * TONGMEMBER_COUNT_PER_PAGE;
		}
		
		for ( int nIdx = 0; nIdx < TONGMEMBER_COUNT_PER_PAGE; ++nIdx )
		{
			char szBuf[COMMON_CLIENT_MSG_LEN_256];
			sprintf( szBuf, "%s/Item%d",Utf8ToAnsi(strName),  nIdx);
			Window* pItem = pList->getChild( szBuf );
			if ( pItem )
			{
				pItem->setID(nIdx);
				pItem->setUserString( "guid", Utf8ToAnsi( pMemberList[nIdx +nIdxStart].guid ) );
				char szBuff[COMMON_CLIENT_MSG_LEN_256];
				sprintf( szBuff, "%s%s", szBuf, "/name" );
				StaticText *pName = (StaticText *)pItem->getChild( szBuff );
				sprintf( szBuff, "%s%s", szBuf, "/level" );
				StaticText *pLevel = (StaticText *)pItem->getChild( szBuff );
				sprintf( szBuff, "%s%s", szBuf, "/metier" );
				StaticText *pMetier = (StaticText *)pItem->getChild( szBuff );
				sprintf( szBuff, "%s%s", szBuf, "/HeightLight" );
				Window *pitemHeightLight = NULL;
				pitemHeightLight =pItem->getChild( szBuff );
				if (pitemHeightLight)
					pitemHeightLight->setVisible(false);
				
				if ( pName && pLevel && pMetier )
				{
					Rect   winRect = ms_Singleton->m_pThisWnd->getPixelRect();
					Rect   oldrect = pCliper->getPixelRect();	
					LORect rect(oldrect.d_left-winRect.d_left,oldrect.d_top-winRect.d_top,oldrect.d_right-oldrect.d_left,oldrect.d_bottom-oldrect.d_top);
                    ((TLStaticText *)pName)->useLayout();
		     		((TLStaticText *)pName)->getLayout()->setClipper(rect);

					KUiCfgLoader& cfgMgr=KUiCfgLoader::getSingleton();
					
					if ( pMemberList[nIdx +nIdxStart].szName[0] )
					{
						char szNameImageAndText[512];
						szNameImageAndText[0] =0;
						
						sprintf(szNameImageAndText,"<Layout width=%d><Seg text-align=center float=wrap f-f=%s>",(int)pName->getWidth(Absolute),pName->getFont()->getName().c_str());	
						
						bool bShowPrevent = false;
						switch (d_CurLayerId)
						{
						case enSULayer_Gens:
							if (pMemberList[nIdx +nIdxStart].bPreventChatState[0])
								bShowPrevent = true;	
							break;
						case enSULayer_Tong:
							if (pMemberList[nIdx +nIdxStart].bPreventChatState[1])
								bShowPrevent = true;
							break;
					
						default:break;
						}
						
						if ( pMemberList[nIdx +nIdxStart].bOnline )
						{
							if (bShowPrevent)
								strcat(szNameImageAndText,cfgMgr.getTongSmallImageCfg().ForbidChatImagePathOnline);
							
							switch (pMemberList[nIdx +nIdxStart].nTopOwnerLayer)
							{
							case enSULayer_Gens:
								strcat(szNameImageAndText,cfgMgr.getTongSmallImageCfg().GensImagePathOnline);
								break;
							case enSULayer_Tong:
								strcat(szNameImageAndText,cfgMgr.getTongSmallImageCfg().TongImagePathOnline);
								break;

							default:break;
							}
							
							CEGUI::colour col;
							col.setARGB(0xffffffff);
							
							strcat(szNameImageAndText,"<Obj color=255,255,255>");
							strcat(szNameImageAndText,pMemberList[nIdx +nIdxStart].szName);
							strcat(szNameImageAndText,"</Obj></Seg></Layout>");	
							
							pLevel->setTextColours(col);
							pMetier->setTextColours(col);
						}
						else
						{
							
							if (bShowPrevent)
								strcat(szNameImageAndText,cfgMgr.getTongSmallImageCfg().ForbidChatPathOffline);
							
							switch (pMemberList[nIdx +nIdxStart].nTopOwnerLayer )
							{
							case enSULayer_Gens:
								strcat(szNameImageAndText,cfgMgr.getTongSmallImageCfg().GensImagePathOffline);
								break;
							case enSULayer_Tong:
								strcat(szNameImageAndText,cfgMgr.getTongSmallImageCfg().TongImagePathOffline);
								break;
			
							default:break;
							}
							
							CEGUI::colour col;
							col.setARGB(0xff808080);
							
							strcat(szNameImageAndText,"<Obj color=128,128,128>");
							strcat(szNameImageAndText,pMemberList[nIdx +nIdxStart].szName);
							strcat(szNameImageAndText,"</Obj></Seg></Layout>");
							
							pLevel->setTextColours(col);
							pMetier->setTextColours(col);
						}//end else
						
						((TLStaticText *)pName)->useLayout();
						((TLStaticText *)pName)->getLayout()->SetText(szNameImageAndText);
						
						sprintf( szBuff, "%d", pMemberList[nIdx +nIdxStart].nLevel );
						pLevel->setText( AnsiToUtf8( szBuff ) );
						if ( !pMemberList[nIdx +nIdxStart].bOnline )
						{
							if (!d_bShowOnline)
								pMetier->setText( AnsiToUtf8( KMessageCentre::GetMessage(7,26) ) );
							else
							{
								((TLStaticText *)pName)->useLayout();
						        ((TLStaticText *)pName)->getLayout()->clearLayout();
								pLevel->setText( "" );
								pMetier->setText( "" );
							}
						}
						else
						{
							DWORD  nProfession       = pMemberList[nIdx +nIdxStart].nMetier;
							DWORD  nBaseProfession   = (nProfession & 0x000000000f);
							DWORD  nHiwordProfession = ((nProfession & 0x000000f0)>>4);
							
							switch( nBaseProfession )
							{
							case 0:
								if (nHiwordProfession==0x0000000f)
									pMetier->setText( AnsiToUtf8( ROLE_CAREER_JS ) );
								else
								{
									if (nHiwordProfession==0)
									{
										pMetier->setText( AnsiToUtf8( ROLE_CAREER_JS_1 ) );
									}
									else
									{
										pMetier->setText( AnsiToUtf8( ROLE_CAREER_JS_0 ) );
									}
								}
								break;
							case 1:
								if (nHiwordProfession==0x0000000f)
								{
									pMetier->setText( AnsiToUtf8( ROLE_CAREER_DS ) );
								}
								else
								{
									if (nHiwordProfession==0)
									{
										pMetier->setText( AnsiToUtf8( ROLE_CAREER_DS_1 ) );
									}
									else
									{
										pMetier->setText( AnsiToUtf8( ROLE_CAREER_DS_0 ) );
									}
								}
								break;
							case 2:
								if (nHiwordProfession==0x0000000f)
								{
									pMetier->setText( AnsiToUtf8( ROLE_CAREER_YR) );
								}
								else
								{
									if (nHiwordProfession==0)
									{
										pMetier->setText( AnsiToUtf8( ROLE_CAREER_YR_0) );
									}//end for if
									else
									{
										pMetier->setText( AnsiToUtf8( ROLE_CAREER_YR_1) );
									}//end for else
									
								}//end for else
								break;
							}//end else
						}//end else
					}//endelse
					else
					{
						((TLStaticText *)pName)->getLayout()->clearLayout();
						// ((TLStaticText *)pName)->getLayout()->SetText(_CHAT_EIDT_DEFAULT_STRING);
						pLevel->setText( "" );
						pMetier->setText( "" );
					}//end else
				}
			}
		}//end for 
		
		char szBuf[COMMON_CLIENT_MSG_LEN_256];
		sprintf( szBuf, "%s/Item%d",Utf8ToAnsi(strName), 0);
		Window* pItem  =  pList->getChild( szBuf );
		WindowEventArgs args(pItem);
		args.handled = false;
		
		handleStaticImageMouseClicked(args);    //选中Item0

	}//endif
	else
	{
		//Clear it 
		for ( int nIdx = 0; nIdx < TONGMEMBER_COUNT_PER_PAGE; ++nIdx )
		{
			char szBuf[COMMON_CLIENT_MSG_LEN_256];
			sprintf( szBuf, "%s/Item%d",Utf8ToAnsi(strName),  nIdx);
			Window* pItem = pList->getChild( szBuf );

			if ( pItem )
			{
				pItem->setUserString( "guid", "" );
				char szBuff[COMMON_CLIENT_MSG_LEN_256];
				sprintf( szBuff, "%s%s", szBuf, "/name" );
				StaticText *pName = (StaticText *)pItem->getChild( szBuff );
				sprintf( szBuff, "%s%s", szBuf, "/level" );
				StaticText *pLevel = (StaticText *)pItem->getChild( szBuff );
				sprintf( szBuff, "%s%s", szBuf, "/metier" );
				StaticText *pMetier = (StaticText *)pItem->getChild( szBuff );
				sprintf( szBuff, "%s%s", szBuf, "/HeightLight" );

				Window *pitemHeightLight = NULL;
				pitemHeightLight =pItem->getChild( szBuff );
				if (pitemHeightLight)
					pitemHeightLight->setVisible(false);
				
				if (pName )
					((TLStaticText *)pName)->getLayout()->clearLayout();
				
				if (pLevel)
					pLevel->setText( "" );
				
				if (pMetier)
					pMetier->setText( "" );
			}//endif	
			
		}//end for 
		
	}//end else

	String       strScroll  =  pPage->getName() + "/Scrollbar";
    TLVertScrollbar * pBar  =  (TLVertScrollbar *)pPage->getChild(strScroll);
	pBar->setScrollPosition(0.0f);
	
}

void    KUiTongManager::SortMenberList(TongPageData * pMemberlist)
{
        for (int  nIdx=0;nIdx<TONGMEMBER_MAX_NUM;++nIdx)
		{ 
            if (pMemberlist[nIdx].bOnline)
			{
				int nRightIndex=nIdx;

				while (nRightIndex>0 && !pMemberlist[nRightIndex-1].bOnline)
				{
                       --nRightIndex;
				}

				if (nRightIndex!=nIdx)
				{
					TongPageData    temp;
					memcpy(&temp,&pMemberlist[nRightIndex],sizeof(TongPageData));
                    memcpy(&pMemberlist[nRightIndex],&pMemberlist[nIdx],sizeof(TongPageData));
					memcpy(&pMemberlist[nIdx],&temp,sizeof(TongPageData));
				}

			}//endif
		}//end for nIdx
}

bool 
KUiTongManager::staticImage_MouseDoubleClicked( const CEGUI::EventArgs& args )
{
	const CEGUI::MouseEventArgs& mouse = static_cast< const CEGUI::MouseEventArgs& >( args );
	const CEGUI::WindowEventArgs& winEvent = static_cast< const CEGUI::WindowEventArgs& >( args );
	if ( mouse.button == LeftButton )
	{
		handleStaticImageMouseClicked( args );

		Window* pBar = static_cast< Window* >( winEvent.window );
		if ( NULL != pBar && pBar->isUserStringDefined( "guid" ) )
		{
			d_CurSelect = pBar->getID();
			const String& strGuid = pBar->getUserString( "guid" );
			string selectedName = d_memberList[d_CurSelect].szName;
			if ( ! strGuid.empty() && ! selectedName.empty() )
			{
				if (d_CurLayerId == enSULayer_Gens)
				{
					memcpy( d_id.data, Utf8ToAnsi( strGuid ), 33 );
					d_id.data[32] = 0;
				}//endif
				KUiPlayerBaseInfo baseInfo;
				ZeroMemory( &baseInfo, sizeof( baseInfo ) );
				g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, reinterpret_cast< unsigned int >( &baseInfo ), NULL );
				string clientPlayerName = baseInfo.Name;
				bool isSelf = clientPlayerName == selectedName;
				KUiGenPersonalInfo::GetSingleton().SetEnableEdit( isSelf );

				UIPlayerRealInfoGet infoGet;
				infoGet.PlayerGUID = d_id;
				g_pCoreShell->OperationRequest( GOI_GET_PLAYER_REAL_INFO, reinterpret_cast< unsigned int >( &infoGet ), NULL );
			}
		}
	}
	return true;
}

bool	KUiTongManager::handleStaticImageMouseClicked( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent )
	{
		Window* pImageWnd = (Window*)pWinEvent->window;

		if ( pImageWnd && pImageWnd->isUserStringDefined("guid"))
		{
			//判断是否为该item为显示item
			String itemHeightLightName = pImageWnd->getName() + "/HeightLight";
			String itemName = pImageWnd->getName() + "/name";
			String itemLevel = pImageWnd->getName() + "/level";
			String itemMetier = pImageWnd->getName() +"/metier";

			FSGUID invalid;
			const String &strGuid = pImageWnd->getUserString( "guid" );

			static const String emptyStr = "";
			Window* pLeagueName	= tryGetChild( pImageWnd, pImageWnd->getName() + "/Name" );
	
			//避免操作没有Online显示时候的玩家 以及空GUID的
			bool checkResult = ( strGuid != "" )
				&& ( pLeagueName != NULL || ( static_cast< TLStaticText * >( pImageWnd->getChild( itemLevel ) )->getText() != emptyStr ) );

			if ( !checkResult )
			{
				return false;
			}

			memcpy(d_SigleTarget.data,Utf8ToAnsi( strGuid ),33);
            d_SigleTarget.data[32]=0;

			if ( ( enSULayer_Gens == d_CurLayerId ) || ( enSULayer_League == d_CurLayerId ) )
			{
				memcpy( d_id.data, Utf8ToAnsi( strGuid ), 33 );
				d_id.data[32] = 0;
			}
			
			bool shizuExists = false, leagueExists = false;
			Window* pName	= tryGetChild( pImageWnd, itemName );
			Window* pLevel	= tryGetChild( pImageWnd, itemLevel );
			Window* pMetier = tryGetChild( pImageWnd, itemMetier );
			if ( ( NULL != pName ) && ( NULL != pLevel ) && ( NULL != pMetier) )
			{
				shizuExists = ( pName->getText() != emptyStr ) 
					|| ( pLevel->getText() != emptyStr ) 
					|| ( pMetier->getText() != emptyStr );
			}

			Window* pLeagueMemberCount	= tryGetChild( pImageWnd, pImageWnd->getName() + "/MemberCount" );
			Window* pLeagueCity			= tryGetChild( pImageWnd, pImageWnd->getName() + "/City" );
			Window* pLeagueFenxingchi	= tryGetChild( pImageWnd, pImageWnd->getName() + "/Fenxingchi" );

			if ( ( NULL != pLeagueFenxingchi ) 
				&& ( NULL != pLeagueCity ) 
				&& ( NULL != pLeagueMemberCount ) 
				&& ( NULL != pLeagueName ) )
			{
				leagueExists = ( ( pLeagueName->getText() != emptyStr ) 
					|| ( pLeagueCity->getText() != emptyStr ) 
					|| ( pLeagueMemberCount->getText() != emptyStr )
					|| ( pLeagueFenxingchi->getText() != emptyStr ) );
			}

			if ( ( pImageWnd->getChild(itemHeightLightName) != NULL ) && ( shizuExists || leagueExists ) )
			{
				//为氏族成员item显示高亮
				for ( int i = 0; i < TONGMEMBER_COUNT_PER_PAGE; i++ )
				{
					char temp[128] ;
					sprintf(temp, "%d", i);
					String itemName = String(pImageWnd->getParent()->getName()) + String("/Item")+String(temp);

					Window *pItemDest = pImageWnd->getParent()->getChild(itemName);

					if ( pItemDest != NULL )
					{
						pItemDest->getChild( itemName + String("/HeightLight"))->setVisible(false);
					}
				}//end for i
				
				pImageWnd->getChild(itemHeightLightName)->setVisible(true);
			}
			
			CEGUI::MouseEventArgs & mouse = (CEGUI::MouseEventArgs &)args;

	//		Point parentP = m_pThisWnd->getPosition(Absolute);
			Point newP = MouseCursor::getSingleton().getPosition() /*- parentP*/;

			if (mouse.button == RightButton)
			{
//  				if ( enSULayer_League == d_CurLayerId )
//  				{
//  					return false;
//  				}
				if ( NULL == d_Manu )
				{
					return false;
				}

				if ( leagueExists || shizuExists )
				{
					d_CurSelect     = pImageWnd->getID();
					
					if (d_CurSelect< TONGMEMBER_COUNT_PER_PAGE)
					{
						d_Manu->enable();
						d_Manu->setPosition(Absolute,newP);

						
						//由于联盟的列表每个Item是一个诸侯而不是一个人，所以要做特例
 						if ( enSULayer_League == d_CurLayerId )
 						{
							if ( NULL != d_AddFriend )
							{
								d_AddFriend->disable();
							}
							
							if ( NULL != d_Invite )
							{
								d_Invite->disable();
							}
							
							if ( NULL != d_Chat )
							{
								d_Chat->disable();
							}
							
							if ( NULL != d_Detail )
							{
								d_Detail->disable();
							}
							
							if ( NULL != d_ChangeOwner )
							{
								d_ChangeOwner->enable();
							}
							
							if ( NULL != d_Kick )
							{
								d_Kick->disable();
							}
							
							if ( NULL != d_PreventChat )
							{
								d_PreventChat->disable();
							}
							
							d_Manu->show();

 							return true;
 						}


						
						if (d_CurLayerId == enSULayer_Gens)
						{
							d_Kick->show();
							d_PreventChat->hide();
						}//endif
						else
						{
							d_Kick->hide();
							d_PreventChat->show();
						}//end else
						
						if (d_memberList[d_CurSelect].bOnline)
						{
						    d_Invite->enable();
							d_Chat->enable();
							d_Detail->enable();

							if (d_memberList[d_CurSelect].nTopOwnerLayer==d_CurLayerId -1 )
                              d_ChangeOwner->enable();

						}//endif
						else
						{
                            d_Invite->disable();
							d_Chat->disable();
							d_Detail->disable();
							d_ChangeOwner->disable();
						}//end else

						char szBuff[32];

						if (d_memberList[d_CurSelect].bPreventChatState[d_CurLayerId-enSULayer_Gens])
						{
							sprintf (szBuff,"%s",CONFIRM_CHAT_IN_UNIT);
							((PushButton *)d_PreventChat)->setText(AnsiToUtf8(szBuff));
							d_PreventChat->setID(enSUO_UnForbidChat);
						}
						else
						{
							sprintf (szBuff,"%s",FORBID_CHAT_IN_UNIT);
							((PushButton *)d_PreventChat)->setText(AnsiToUtf8(szBuff));
							d_PreventChat->setID(enSUO_ForbidChat);
						}//end else

						//Privilage 
						SocietyInfoIndex tagSocietyIdx;
	                    tagSocietyIdx.TemplateId	= enSUTplId_Tong;
						tagSocietyIdx.Layer         = d_CurLayerId;
                        tagSocietyIdx.Operation     = enSUO_RemoveSubUnit;
							
						if (g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ))
						{
                            d_Kick ->enable();
							d_PreventChat->enable();
							
						}//endif
						else
						{
							d_ChangeOwner->disable();   //注意 同样不可以禅让
							d_Kick->disable();
							d_PreventChat->disable();     
						}//end else 

						KUiPlayerBaseInfo pInfo;
						memset(&pInfo,0,sizeof(pInfo));
						
						g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(int)&pInfo,0);
						if (strcmp(pInfo.Name,d_memberList[d_CurSelect].szName) == 0)
						{
							d_Manu->disable();
						}//endif

					}//endif

					d_Manu->show();
					
				}//endif

			}//endif
		}
	}
	return true;
}

bool	KUiTongManager::handleRequestMemberList( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	if ( pWinEvent && pWinEvent->window->isVisible() )
	{
		SocietyLayerOperationController* pOper = (SocietyLayerOperationController*)(pWinEvent->window->getUserData());
		if ( pOper == NULL )
		{
			Hide();
			return false;
		}
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= d_TemplateInfo.Id;
		tagTongOper.nLayerID		= pOper->LayerID;
		tagTongOper.nOperationID	= pOper->Id;
		memcpy( tagTongOper.szReceiveName, pOper->ReturnController, COMMON_CLIENT_MSG_LEN_128 );
		tagTongOper.szReceiveName[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;
		tagTongOper.nPage           = 0;  //Notice!!
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}
		//当氏族界面显示时才更新数据
		LayerIndex::iterator it = ms_Singleton->d_LayerIdx.begin();
		for ( ; it != ms_Singleton->d_LayerIdx.end(); it++)
		{
			if ( m_pThisWnd->isVisible() && (*it).second.pPageWnd && (*it).second.pPageWnd->isVisible())
			{
				pTongOper->updateRecord( pOper->Id, &tagTongOper, sizeof( TongOperParam ) );	
			}
		}
		d_bProcessDataing = true;

		d_CurSubPageNo = 0;

		memcpy(&d_TempRedrawTongOper,&tagTongOper,sizeof(d_TempRedrawTongOper));
		d_IsTempOperValid = true;
	}
	return true;
}

bool KUiTongManager::handleShowPage( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pWinEvent = (WindowEventArgs*)&args;
	SocietyLayerInfo* pOper = (SocietyLayerInfo*)(pWinEvent->window->getUserData());
	if ( pWinEvent )
	{
		hideAllPage();

		if ( pOper && d_LayerIdx[pOper->nLayerID].pPageWnd )
		{
			d_LayerIdx[pOper->nLayerID].pPageWnd->show();
			d_CurLayerId = pOper->nLayerID;
		}
	}
	return true;
}

void	KUiTongManager::hideAllPage( void )
{
	LayerIndex::iterator it = d_LayerIdx.begin();
	while ( it != d_LayerIdx.end() )
	{
		(*it).second.pPageWnd->hide();
		it++;
	}
}

void	KUiTongManager::clearAllPage( void )
{
	LayerIndex::iterator it = d_LayerIdx.begin();
	while ( it != d_LayerIdx.end() )
	{
		Window* pPage = (*it).second.pPageWnd;		
		if ( pPage )
		{
			char   szFir[COMMON_CLIENT_MSG_LEN_128];
			char   szSec[COMMON_CLIENT_MSG_LEN_128];

			sprintf(szFir,"%s%s",Utf8ToAnsi(pPage->getName()),"/MemberList");	
			sprintf(szSec,"%s%s%s",Utf8ToAnsi(pPage->getName()),"/MemberList","/Clipper");	

			Window* pList = (Window*)(pPage->getChild(szFir)->getChild(szSec));
			if ( pList == NULL )
			{
				continue;
			}
			for ( int nIdx = 0; nIdx < TONGMEMBER_COUNT_PER_PAGE; ++nIdx )
			{
				char szSecond[COMMON_CLIENT_MSG_LEN_128];
				sprintf( szSecond, "%s/Clipper/Item%d",szFir,  nIdx );
				Window* pItem = pList->getChild(szSecond);
				if ( pItem )
				{
					FSGUID  invalid;
					pItem->setUserString( "guid", Utf8ToAnsi( invalid.data ) );
					
					char szBuff[COMMON_CLIENT_MSG_LEN_64];
					sprintf( szBuff, "%s%s", szSecond, "/name" );
					Window* pName	= tryGetChild( pItem, szBuff );
					sprintf( szBuff, "%s%s", szSecond, "/level" );
					Window* pLevel	= tryGetChild( pItem, szBuff );
					sprintf( szBuff, "%s%s", szSecond, "/metier" );
					Window* pMetier = tryGetChild( pItem, szBuff );
					if ( pName && pLevel && pMetier )
					{
						((TLStaticText *)pName)->useLayout();
						((TLStaticText *)pName)->getLayout()->clearLayout();
						pLevel->setText( "" );
						pMetier->setText( "" );
					}//endif

					Window* pLeagueName			= tryGetChild( pItem, pItem->getName() + "/Name" );
					Window* pLeagueMemberCount	= tryGetChild( pItem, pItem->getName() + "/MemberCount" );
					Window* pLeagueCity			= tryGetChild( pItem, pItem->getName() + "/City" );
					Window* pLeagueFenxingchi	= tryGetChild( pItem, pItem->getName() + "/Fenxingchi" );
					if ( pLeagueCity && pLeagueMemberCount&& pLeagueName && pLeagueFenxingchi )
					{
						pLeagueName->setText( "" );
						pLeagueCity->setText( "" );
						pLeagueFenxingchi->setText( "" );
						pLeagueMemberCount->setText( "" );
					}
					
					sprintf( szBuff, "%s%s", szSecond, "/HeightLight" );
					Window *pitemHeightLight = NULL;
					pitemHeightLight =pItem->getChild( szBuff );
					if (pitemHeightLight)
						pitemHeightLight->setVisible(false);

				}//endif
			}
		}
       ++it;
	}
}

bool	KUiTongManager::handleExit( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

bool	KUiTongManager::handleShowOnline( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pEvent = (WindowEventArgs*)&args;
	if ( pEvent->window )
	{
		Checkbox* checkbox = (Checkbox*)pEvent->window;
		if ( checkbox )
		{	
			d_bShowOnline  =	checkbox->isSelected();
			d_CurSubPageNo =    0;
			
			Window *       pPage  =  d_LayerIdx[d_CurLayerId].pPageWnd;
			String strOriginName  =  pPage->getName() + "/MemberList";
			String strName        =  pPage->getName() + "/MemberList"+"/Clipper";		
			String strScroll      =  pPage->getName() + "/Scrollbar";
			
			TLVertScrollbar * pBar=  (TLVertScrollbar *)pPage->getChild(strScroll);
	        pBar->setScrollPosition(0.0f);

			RedrawMemberList(d_LayerIdx[d_CurLayerId].pPageWnd,d_memberList);
		}//endif		
	}
	
	return true;
}

SocietyLayerOperationController* KUiTongManager::getOperReceive( const SocietyInfoIndex& tagSocietyIdx )
{
	for ( int nIdx = 0; nIdx < d_LayerInfo[tagSocietyIdx.Layer].OperationCount; ++nIdx )
	{
		if ( d_LayerInfo[tagSocietyIdx.Layer].Operations[nIdx].Id == tagSocietyIdx.Operation )
		{
			return d_LayerInfo[tagSocietyIdx.Layer].Operations[nIdx].Controllers;
		}
	}
	return NULL;
} 

Window* KUiTongManager::tryGetChild( const Window* pWindow, const String& childName )
{
	if ( pWindow && pWindow->isChild( childName ) )
	{
		return pWindow->getChild( childName );
	}
	
	return NULL;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

template<> 
KUiTongOperMgr* KUiWndSingleton<KUiTongOperMgr>::ms_Singleton	= NULL;

KUiTongOperMgr::KUiTongOperMgr( const CEGUI::String& id_name ):
KUiWndSingleton<KUiTongOperMgr>( id_name )
{
}


KUiTongOperMgr::~KUiTongOperMgr()
{
}

void KUiTongOperMgr::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongOperMgr/OkBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongOperMgr::handleOK, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongOperMgr/CancelBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongOperMgr::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongOperMgr/CloseBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiTongOperMgr::handleExit, ms_Singleton));
	}
}

void KUiTongOperMgr::Show( const TongOperParam& rOperType , const bool bShowNotice /*=false*/)
{
	KUiWndSingleton<KUiTongOperMgr>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->d_OperType = rOperType;
		
		char          szMsgAll[256];
		szMsgAll[0]=0;

		if (bShowNotice && KMessageCentre::GetMessage( tong_operation_message, 25+ms_Singleton->d_OperType.nLayerID))
		{ 
			strcpy(szMsgAll,KMessageCentre::GetMessage( tong_operation_message, 25+ms_Singleton->d_OperType.nLayerID));
		}//endif
	
        strcat(szMsgAll,KMessageCentre::GetMessage( tong_operation_message, ms_Singleton->d_OperType.nOperationID ));

		String strMsg =AnsiToUtf8( szMsgAll );
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/TongOperMgr/Tip")->setText( strMsg );

		if ( ms_Singleton->d_OperType.nOperationID == enSUO_AddSubUnit)
		{
			static_cast<Editbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Input" ))->show();
			static_cast<Editbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Image"))->show();
			static_cast<Editbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Input" ))->setReadOnly(false);
		}
		else
		{
			static_cast<Editbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Input" ))->hide();
			static_cast<Editbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Image"))->hide();
			static_cast<Editbox*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Input" ))->setReadOnly(true);
		}

	}

}

bool KUiTongOperMgr::handleOK( const CEGUI::EventArgs& args )
{
	memcpy( d_OperType.szName, Utf8ToAnsi( m_pThisWnd->getChild( "TaharezLook/TongOperMgr/Input" )->getText() ), CLIENT_NAME_AND_TITLE_MAX );
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return false;
	}
	
	pTongOper->updateRecord( d_OperType.nOperationID, &d_OperType, sizeof( TongOperParam ) );

	KUiTongManager::RefreshPage();

	Hide();
	return true;
}

bool KUiTongOperMgr::handleExit( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}



//////////////////////////////////////////////////////////////////////////
///					KUiTongStatueMsg
//////////////////////////////////////////////////////////////////////////

template<> 
KUiTongStatueMsg* KUiWndSingleton<KUiTongStatueMsg>::ms_Singleton	= NULL;

KUiTongStatueMsg::KUiTongStatueMsg( const CEGUI::String& id_name ):
KUiWndSingleton<KUiTongStatueMsg>( id_name )
{
	m_pCity = NULL;
	m_pLeader = NULL;
	m_pLeague = NULL;
	m_pCityIcon = NULL;
	m_pTime = NULL;
	m_pReward = NULL;
}

KUiTongStatueMsg::~KUiTongStatueMsg()
{
	
}

void KUiTongStatueMsg::Init()
{
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pThisWnd ) )
	{
		m_pCity		= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/City" ) );
		m_pLeader	= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/Leader" ) );
		m_pLeague	= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/League" ) );
		m_pTime		= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/Time" ) );
		m_pReward	= static_cast< TLStaticText* >( m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/Reward" ) );
		m_pCityIcon	= static_cast< TLStaticImage* >( m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/CityIcon" ) );

		m_pThisWnd->getChild( "TaharezLook/TongStatueMsg/btnClose" )->subscribeEvent( 
			PushButton::EventClicked, 
			Event::Subscriber( &KUiTongStatueMsg::btnClose_Clicked, ms_Singleton ) );
	}
}

void KUiTongStatueMsg::ShowStatueMsg( UIStatueInfo* uParam )
{
	if ( ( NULL != m_pCity ) && ( NULL != m_pLeader ) && ( NULL != m_pLeague ) && ( NULL != uParam ) && ( NULL != m_pCityIcon ) )
	{
		m_pCity->setText( AnsiToUtf8( uParam->cMapName ) );
		m_pLeader->setText( AnsiToUtf8( uParam->cName ) );
		m_pLeague->setText( AnsiToUtf8( uParam->cTongName ) );
		
		//显示地图图标
		const int mapId = uParam->nMapId;
		const KUiCfgLoader::CityImageCfg& cityImageCfg = KUiCfgLoader::getSingleton().getCityImageCfg();
		for ( int i = 0; i < 4; i++ )
		{
			if ( mapId == cityImageCfg.CityMapID[i] )
			{
				m_pCityIcon->setImage( cityImageCfg.CityIConSet, cityImageCfg.CityImage[i] );
			}
		}

		//显示时间
		showTime( uParam->dwtime );

		//显示是否有奖励
		if ( 0 == uParam->hasBuff )
		{
			m_pReward->setText( AnsiToUtf8( KMessageCentre::GetMessage( social_info, 16 ) ) );
		}
		else
		{
			m_pReward->setText( AnsiToUtf8( KMessageCentre::GetMessage( social_info, 17 ) ) );
		}

		Show();
	}
}

bool KUiTongStatueMsg::btnClose_Clicked( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

void KUiTongStatueMsg::showTime( DWORD dwSeconds )
{
	if ( NULL == m_pTime )
	{
		return;
	}

	const int secondsPerHour = 3600;
	const int secondsPerDay = secondsPerHour * 24;
	
	int day = 0, hour = 0;
	if ( secondsPerDay > 0 )
	{
		day = dwSeconds / secondsPerDay;
	}
	
	if ( secondsPerHour > 0 )
	{
		hour = ( dwSeconds % secondsPerDay ) / secondsPerHour;
	}

	string dayUnit = KMessageCentre::GetMessage( social_info, 14 );
	string hourUnit = KMessageCentre::GetMessage( social_info, 15 );
	ostringstream timeShowStream;
	if ( 0 < day )
	{
		timeShowStream << day << dayUnit;
	}
	timeShowStream << hour << hourUnit;

	m_pTime->setText( AnsiToUtf8( timeShowStream.str().c_str() ) );
}