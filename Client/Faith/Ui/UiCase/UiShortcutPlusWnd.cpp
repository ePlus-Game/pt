 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/15/2006 2:10
//      File_base        : KUiShortcutPlusWnd
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiShortcutPlusWnd.h"
#include "CoreUseNameDef.h"
#include "GameDataDef.h"
#include "Coreshell.h"
#include "UiDragItem.h"
#include "../../Login/Login.h"

#include "../ShortcutKey.h"
#include "UiGameSetting.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

/************************************************************************
 *                      KUiShortCutPlusWndShowHide
 ***********************************************************************/

template<> 
KUiShortcutPlusWndShowHide* KUiWndSingleton<KUiShortcutPlusWndShowHide>::ms_Singleton	= NULL;

KUiShortcutPlusWndShowHide::KUiShortcutPlusWndShowHide(const CEGUI::String & id_name)
:KUiWndSingleton<KUiShortcutPlusWndShowHide>( id_name )
{

}

KUiShortcutPlusWndShowHide::~KUiShortcutPlusWndShowHide()
{

}

void KUiShortcutPlusWndShowHide::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
        ms_Singleton->m_pThisWnd->getChild("TaharezLook/ShortcutPlusSH/SHButton")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiShortcutPlusWndShowHide::HandleClick, this) ) ;
	}//endif
}

bool KUiShortcutPlusWndShowHide::HandleClick(const CEGUI::EventArgs& args )
{

	if (KUiShortcutPlusWnd::IsShowing())
		KUiShortcutPlusWnd::Hide();
	else
		KUiShortcutPlusWnd::Show();
	
	return true;
}

/************************************************************************/
/*						KUiShortcutPlusWnd system                       */
/************************************************************************/

template<> 
KUiShortcutPlusWnd* KUiWndSingleton<KUiShortcutPlusWnd>::ms_Singleton	= NULL;

KUiShortcutPlusWnd::KUiShortcutPlusWnd( const CEGUI::String& id_name ):
KUiWndSingleton<KUiShortcutPlusWnd>( id_name )
{
	m_bCast = false;
	m_bDragFlag = false;
	ms_Singleton->m_nCurrentBarIdx = 0;
	m_bShowing = false;

	IUIMDLDataset* pDataset = NULL;
	int nRet = m_pUiMDLManager->queryDataSet( itemgroupcd_dataset, &pDataset );
	if ( success_errorcode != nRet && dataset_areadycreated_errorcode != nRet )
	{
		m_pUiMDLManager->createDataSet( itemgroupcd_dataset );
		m_pUiMDLManager->queryDataSet( itemgroupcd_dataset, &pDataset );
	}
	pDataset->setEventHandle( ms_Singleton );

	ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );
	m_pThisWnd->setZLevel(Window::SuperBottom);

	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_nCurrentBarIdx = 0;
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_32];
			sprintf( szName, "%d_", nIdx );
			Window* pGO = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_SHORTCUTPLUSBAR, szName,"", NULL, NULL, true  );
			if ( pGO )
			{
				Point absPoint;
				absPoint.d_x = 0;
				absPoint.d_y = 0;
				Size  absSize	= ms_Singleton->m_pThisWnd->getAbsoluteSize(); 
				ms_Singleton->m_pThisWnd->addChildWindow( pGO );
				pGO->setSize( Absolute, absSize );
				pGO->setPosition( Absolute, absPoint );
			}
		}
		ms_Singleton->m_funList[0] =	handleShortcut0;
		ms_Singleton->m_funList[1] =	handleShortcut1;
		ms_Singleton->m_funList[2] =	handleShortcut2;
		ms_Singleton->m_funList[3] =	handleShortcut3;
		ms_Singleton->m_funList[4] =	handleShortcut4;
		ms_Singleton->m_funList[5] =	handleShortcut5;
		ms_Singleton->m_funList[6] =	handleShortcut6;
		ms_Singleton->m_funList[7] =	handleShortcut7;
		ms_Singleton->m_funList[8] =	handleShortcut8;
		ms_Singleton->m_funList[9] =	handleShortcut9;

		ms_Singleton->m_funListPlus[0] =	handleMouseMove0;
		ms_Singleton->m_funListPlus[1] =	handleMouseMove1;
		ms_Singleton->m_funListPlus[2] =	handleMouseMove2;
		ms_Singleton->m_funListPlus[3] =	handleMouseMove3;
		ms_Singleton->m_funListPlus[4] =	handleMouseMove4;
		ms_Singleton->m_funListPlus[5] =	handleMouseMove5;
		ms_Singleton->m_funListPlus[6] =	handleMouseMove6;
		ms_Singleton->m_funListPlus[7] =	handleMouseMove7;
		ms_Singleton->m_funListPlus[8] =	handleMouseMove8;
		ms_Singleton->m_funListPlus[9] =	handleMouseMove9;

		ms_Singleton->m_funListCast[0] =	handleMouseUp0;
		ms_Singleton->m_funListCast[1] =	handleMouseUp1;
		ms_Singleton->m_funListCast[2] =	handleMouseUp2;
		ms_Singleton->m_funListCast[3] =	handleMouseUp3;
		ms_Singleton->m_funListCast[4] =	handleMouseUp4;
		ms_Singleton->m_funListCast[5] =	handleMouseUp5;
		ms_Singleton->m_funListCast[6] =	handleMouseUp6;
		ms_Singleton->m_funListCast[7] =	handleMouseUp7;
		ms_Singleton->m_funListCast[8] =	handleMouseUp8;
		ms_Singleton->m_funListCast[9] =	handleMouseUp9;
		
		ms_Singleton->SetCurrentBar( ms_Singleton->m_nCurrentBarIdx );

		for ( nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
			Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
				if ( pBar )
				{
					TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
					if ( pGO )
					{
						KObjAtContRegion* region = new KObjAtContRegion();
						pGO->setUserData(region);
						
					}
				}
			}
		}
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiShortcutPlusWnd::handleMouseLeave, this) ) ;
	}
}

KUiShortcutPlusWnd::~KUiShortcutPlusWnd()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
			Window *pBar = m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
			{
				
				if ( pBar )
				{
					sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
					if ( pBar )
					{
						TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
						if ( pGO )
						{
							if ( pGO->getUserData() )
							{
								delete pGO->getUserData();
							}
							
						}
					}
				}
			}
		}
	}
}
void KUiShortcutPlusWnd::AddImmediacy( KImmediacyParam* pImm )
{
	if ( pImm == NULL || (pImm && pImm->nPos >= MAX_IMMEDIACY_ITEM) )
	{
		return;	
	}

	//修复bug：减少ShortCutPlus的物品栏数量后，历史数据会被显示在被删除的快捷栏中
	if ( pImm->nPos > 6 )
	{
		return;
	}

	TLGameObject *pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( pImm->nPos ));
	if ( pGO )
	{
		TLGameObject::GameObject tagGO;
		if ( pImm->nImmediacyType == item_immediacy_type )
		{
			KItemInfo tagItemInfo;
			g_pCoreShell->GetGameData( GDI_ITEM_INFO_ID, (unsigned int)&tagItemInfo, pImm->nID );
			tagGO.d_gameobjectSet = AnsiToUtf8( tagItemInfo.szImageSet ); 
			tagGO.d_gameobject	= AnsiToUtf8( tagItemInfo.szImage );
			tagGO.d_genre		= tagItemInfo.itemIdx.nGenre;
			tagGO.d_detail		= tagItemInfo.itemIdx.nDetail;
			tagGO.d_particular	= tagItemInfo.itemIdx.nParticular;
			tagGO.d_level		= tagItemInfo.itemIdx.nLevel;
			tagGO.d_group		= tagItemInfo.nGroup;
			tagGO.d_type		= TLGameObject::shortcut;
			tagGO.d_count		= g_pCoreShell->GetGameData( GDI_ITEM_COUNT, (unsigned int)&tagItemInfo.itemIdx, NULL );
			KObjAtContRegion *destPos = (KObjAtContRegion*)pGO->getUserData();
			destPos->Obj.uId = g_pCoreShell->GetGameData( GDI_GET_ITEM_INDEX_BY_ID, pImm->nID, NULL );
			if ( tagGO.d_count > 0 )
			{
				tagGO.d_state = TLGameObject::normalState;
			}
			else
			{
				tagGO.d_state = TLGameObject::disableState;
			}
			tagGO.d_EdgeframeIdx = tagItemInfo.colour;
			pGO->setObject( tagGO );
			pGO->setTooltipText( AnsiToUtf8( tagItemInfo.szToolTip ) );
		}
		else if ( pImm->nImmediacyType == skill_immediacy_type )
		{
			if ( pImm->nID <= 0 )
			{
				pGO->show();
				return;
			}
			pImm->nID = g_pCoreShell->GetGameData( GDI_GET_CUR_SKILL_ID, pImm->nID, NULL );
			KSkillInfo tagSkillInfo;
			tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
			tagSkillInfo.dwDescStyle=0;
			g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, pImm->nID );
			tagGO.d_gameobject	= AnsiToUtf8( tagSkillInfo.szIconName );
			tagGO.d_coolingTime = tagSkillInfo.nCoolingTime / 1000;
			tagGO.d_passivity	= tagSkillInfo.bPassivity;
			tagGO.d_skillID		= pImm->nID;
			tagGO.d_type		= TLGameObject::shortcut;
			pGO->setObject( tagGO );
			pGO->setTooltipText( AnsiToUtf8( tagSkillInfo.szDesc ) );
			if ( tagSkillInfo.bActive )
			{
				pGO->disable( false );
			}
			else
			{
				pGO->disable( true );
			}
		}
		else
		{
			return;// not suppost
		}
		pGO->show();
	}
	return;
}

void KUiShortcutPlusWnd::DelImmediacy( int nPos )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nPos ));
	if ( pGO )
	{
		pGO->clear();
		pGO->show();
	}
}

void	KUiShortcutPlusWnd::Init( void )
{

}

void KUiShortcutPlusWnd::Show( void )
{
	KUiWndSingleton<KUiShortcutPlusWnd>::Show();
	ms_Singleton->m_bShowing = true;
	
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{

	   float  fWidth = ms_Singleton->m_pThisWnd->getWidth(Absolute);
	   Point  pos    = ms_Singleton->m_pThisWnd->getPosition(Absolute);
	   pos.d_x       = g_GetScreenWidth() - fWidth;
	   
       ((TLStaticImage * )ms_Singleton->m_pThisWnd)->MoveTo(pos.d_x,pos.d_y,-16,0,true);
	}//endif

}

void KUiShortcutPlusWnd::Hide( void )
{
	ms_Singleton->m_bShowing = false;

	if ( g_LoginLogic.GetStatus() != LL_S_IN_GAME  )
	{
		ms_Singleton->ClearAllShortcut();
	}//endif

	Point  pos    = ms_Singleton->m_pThisWnd->getPosition(Absolute);
	((TLStaticImage * )ms_Singleton->m_pThisWnd)->MoveTo(g_GetScreenWidth(),pos.d_y,16,0,true);
	
	//KUiWndSingleton<KUiShortcutPlusWnd>::Hide();
}
unsigned int KUiShortcutPlusWnd::BeginGroupCD( KItemGroupCD_C* pGroupCD )
{
	if ( pGroupCD == NULL )
	{
		return 0;
	}
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				if ( pGO && pGO->getState() != TLGameObject::idleState && !pGO->isEmpty() )
				{
					TLGameObject::GameObject sourObjInfo;
					pGO->getObject( sourObjInfo );
					if ( sourObjInfo.d_skillID <= 0  )
					{
						if ( sourObjInfo.d_group == pGroupCD->nGroup )
						{
							if ( sourObjInfo.d_state == TLGameObject::normalState 
								|| sourObjInfo.d_state == TLGameObject::hoverState )
							{
								pGO->setCoolingTime(pGroupCD->ulCDTime);
								pGO->intonate();
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
unsigned int KUiShortcutPlusWnd::EndGroupCD( KItemGroupCD_C* pGroupCD )
{
	if ( pGroupCD == NULL )
	{
		return 0;
	}
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				if ( pGO && pGO->getState() != TLGameObject::idleState && !pGO->isEmpty() )
				{
					TLGameObject::GameObject sourObjInfo;
					pGO->getObject( sourObjInfo );
					if ( sourObjInfo.d_skillID <= 0  )
					{
						if ( sourObjInfo.d_group == pGroupCD->nGroup )
						{
							pGO->setState( TLGameObject::normalState );
							pGO->setEnabled(true);
						}
					}
				}
			}
		}
	}
	return 0;
}

bool         KUiShortcutPlusWnd::IsShowing()
{
	if (ms_Singleton)
	{
		return ms_Singleton->m_bShowing;
	}
    else 
		return false;

}

unsigned int KUiShortcutPlusWnd::UpdateData( void )
{
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				if ( pGO && pGO->getState() != TLGameObject::idleState )
				{
					TLGameObject::GameObject sourObjInfo;
					pGO->getObject( sourObjInfo );
					if ( sourObjInfo.d_skillID <= 0  )
					{
						FIND_ITEMINDEX_PARAM tagItemParam;
						tagItemParam.nGenre			= sourObjInfo.d_genre;
						tagItemParam.nDetail		= sourObjInfo.d_detail;
						tagItemParam.nParticular	= sourObjInfo.d_particular;
						tagItemParam.nLevel			= sourObjInfo.d_level;
						if ( tagItemParam.nGenre != item_horse )
						{
							int nItemCount = g_pCoreShell->GetGameData( GDI_ITEM_COUNT, (unsigned int)&tagItemParam, NULL );		
							pGO->setCount( nItemCount );
							if ( nItemCount <= 0 )
							{
								pGO->clear();
								pGO->show();
								g_pCoreShell->OperationRequest( GOI_DEL_IMMEDIACY, nIdy + 10, NULL );
							}
						}
					}
					else
					{
						KSkillInfo tagSkillInfo;
						tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
			            tagSkillInfo.dwDescStyle=0;
						g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, sourObjInfo.d_skillID );	
						
						// 升级更新，还可能要加设置，但更新GAMEOBJ的话会引起发技能时CD冷却闪烁一下
						//sourObjInfo.d_coolingTime = tagSkillInfo.nCoolingTime / 1000;
						//sourObjInfo.d_passivity = tagSkillInfo.bPassivity;
						//pGO->setObject( sourObjInfo );
						pGO->setTooltipText( AnsiToUtf8( tagSkillInfo.szDesc ) );
						
						if ( tagSkillInfo.bActive )
						{
							if ( pGO->getState() == TLGameObject::disableState )
							{
								pGO->disable( false );
							}

							if( pGO->getState( ) == TLGameObject::coolingState &&
								tagSkillInfo.bCDComplete )
							{
								pGO->setState( TLGameObject::normalState );
							}
						}
						else
						{
							pGO->disable( true );
						}
					}
				}
			}
		}
	}
	return 0;
}

unsigned int KUiShortcutPlusWnd::RefreshSelectedSkill( void )
{
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				if ( pGO )
				{
					TLGameObject::GameObject sourObjInfo;
					pGO->getObject( sourObjInfo );
					if ( sourObjInfo.d_skillID > 0  )					
					{
						KSkillInfo tagSkillInfo;
						g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, sourObjInfo.d_skillID );

						if (tagSkillInfo.bIsSelected)
								pGO->lock();								
							else
								pGO->unlock();
					}
				}
			}
		}
	}
	return 0;
}

void KUiShortcutPlusWnd::Intonate( int nIdx )
{
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nIdx )); 
	if ( pGO && !pGO->isEmpty() )
	{
		TLGameObject::GameObject tmpGO;
		pGO->getObject( tmpGO );
		if ( tmpGO.d_state == TLGameObject::normalState ||
			tmpGO.d_state == TLGameObject::hoverState ||
			tmpGO.d_state == TLGameObject::pushedState )
		{
			if ( tmpGO.d_skillID > 0 )
			{
				g_pCoreShell->NextSkill(tmpGO.d_skillID, KShortcutKeyCentre::ms_bAltPressed);
			}
			else
			{
				FIND_ITEMINDEX_PARAM tagItemInfo;
				tagItemInfo.nGenre = pGO->getGenre();
				tagItemInfo.nDetail = pGO->getDetail();
				tagItemInfo.nParticular = pGO->getParticular();
				tagItemInfo.nLevel = pGO->getLevel();

				int nCount = g_pCoreShell->GetGameData( GDI_ITEM_COUNT, (unsigned int)&tagItemInfo, NULL );

				if ( nCount > 0 )
				{
					g_pCoreShell->OperationRequest(GOI_USE_ITEM, NULL, (int)&tagItemInfo );
					pGO->intonate();
					/*if ( nCount == 1 && tagItemInfo.nGenre != item_horse )
					{
						pGO->clear();
						pGO->show();
						g_pCoreShell->OperationRequest( GOI_DEL_IMMEDIACY, nIdx + 10, NULL );
					}//*/
				}
			}
		}
	}
}

void KUiShortcutPlusWnd::SelectSkill( int nIdx )
{
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nIdx )); 
	if ( pGO && !pGO->isEmpty() )
	{
		TLGameObject::GameObject tmpGO;
		pGO->getObject( tmpGO );

		if ( tmpGO.d_skillID > 0 )
		{
			g_pCoreShell->SelectSkill(tmpGO.d_skillID);
			pGO->requestRedraw();
		}
	}
}

void KUiShortcutPlusWnd::Disable(  int nIdx  )
{
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nIdx ));
	if ( pGO )
	{
		pGO->disable( true );
	}	
}

void	KUiShortcutPlusWnd::ClearAllShortcut( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
			Window *pBar = m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
				if ( pBar )
				{
					TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
					if ( pGO )
					{
						pGO->clear();
						pGO->show();
					}
				}
			}
		}
	}
}

void	KUiShortcutPlusWnd::SaveAllItem( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
			Window *pBar = m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
				if ( pBar )
				{
					TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
					if ( pGO && !pGO->isEmpty() && !pGO->isSkill() )
					{
						TLGameObject::GameObject sourObjInfo;
						pGO->getObject( sourObjInfo );
						KImmediacyParam tagImmInfo;
						tagImmInfo.nImmediacyType	= item_immediacy_type;
						tagImmInfo.nPos				= nIdy + 10;
						tagImmInfo.nID				= GenerateItemHashId(sourObjInfo.d_genre, sourObjInfo.d_detail, sourObjInfo.d_particular );
						g_pCoreShell->OperationRequest( GOI_ADD_IMMEDIACY, (unsigned int)&tagImmInfo, true );

					}
				}
			}
		}
	}
}

Window* KUiShortcutPlusWnd::GetSelShortcutKey( int nSel )
{
	char szName[COMMON_CLIENT_MSG_LEN_64];
	sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", m_nCurrentBarIdx, nSel );	
	Window *pBar = ms_Singleton->GetCurrentBar();
	if ( pBar )
	{
		return pBar->getChild( szName );
	}
	return NULL;
}

Window* KUiShortcutPlusWnd::GetCurrentBar( void )
{
	char szName[COMMON_CLIENT_MSG_LEN_32];
	sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", m_nCurrentBarIdx );
	return m_pThisWnd->getChild( szName );
}

void KUiShortcutPlusWnd::SetCurrentBar( int nSel )
{
	m_nCurrentBarIdx = nSel;
	char szName[COMMON_CLIENT_MSG_LEN_64];
	sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", m_nCurrentBarIdx );
	Window *pBar = m_pThisWnd->getChild( szName );
	if ( pBar )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
			Window *pBarInFor = m_pThisWnd->getChild( szName );
			if ( pBarInFor )
			{
				pBarInFor->hide();
			}
		}
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			Window *pShortcut = GetSelShortcutKey( nIdy );
			if ( pShortcut )
			{
				pShortcut->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(m_funList[nIdy], this) ) ;
				pShortcut->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(m_funListCast[nIdy], this) ) ;
				pShortcut->subscribeEvent(TLGameObject::EventMouseMove, Event::Subscriber(m_funListPlus[nIdy], this) ) ;
			}
		}
		pBar->show();
	}

}

bool KUiShortcutPlusWnd::handleShortcut1( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 1 ));
		if ( pGO )
		{
			return onLBDown( pGO, 1 );		
		}
	}
	else
	{
		Intonate( 1 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut2( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 2 ));
		if ( pGO )
		{
			return onLBDown( pGO, 2 );		
		}
	}
	else
	{
		Intonate( 2 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut3( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 3 ));
		if ( pGO )
		{
			return onLBDown( pGO, 3 );		
		}
	}
	else
	{
		Intonate( 3 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut4( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 4 ));
		if ( pGO )
		{
			return onLBDown( pGO, 4 );		
		}
	}
	else
	{
		Intonate( 4 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut5( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 5 ));
		if ( pGO )
		{
			return onLBDown( pGO, 5 );		
		}
	}
	else
	{
		Intonate( 5 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut6( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 6 ));
		if ( pGO )
		{
			return onLBDown( pGO, 6 );		
		}
	}
	else
	{
		Intonate( 6 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut7( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 7 ));
		if ( pGO )
		{
			return onLBDown( pGO, 7 );		
		}
	}
	else
	{
		Intonate( 7 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut8( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 8 ));
		if ( pGO )
		{
			return onLBDown( pGO, 8 );		
		}
	}
	else
	{
		Intonate( 8 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut9( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 9 ));
		if ( pGO )
		{
			return onLBDown( pGO, 9 );		
		}
	}
	else
	{
		Intonate( 9 );
	}
	return true;
}

bool KUiShortcutPlusWnd::handleShortcut0( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton)
	{
		TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 0 ));
		if ( pGO )
		{
			return onLBDown( pGO, 0 );		
		}
	}
	else
	{
		Intonate( 0 );
	}

	return true;
}

bool KUiShortcutPlusWnd::onLBDown( CEGUI::TLGameObject* pGO,int nSel )
{
	m_bCast = true;
	m_bDragFlag = true;
	TLGameObject* destObj,* sourObj;
	TLGameObject::GameObject destObjInfo, sourObjInfo;
	destObj = pGO;
	destObj->getObject(destObjInfo);
	KObjAtContRegion *destPos = (KObjAtContRegion*)destObj->getUserData();

	sourObj = KUiDragItem::GetSingleton().getObj();
	sourObj->getObject(sourObjInfo);
	KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();	
	int nItemId = sourPos->Obj.uId;

	KObjAtContRegion* temPos = new KObjAtContRegion(*sourPos);

	
	// 技能替换
	if ( !sourObj->isEmpty() && !destObj->isEmpty() )
	{
		sourObj->setObject( destObjInfo );
		*sourPos = *destPos;
	}
	else
		sourObj->clear();

	if(sourObjInfo.d_type != TLGameObject::idle)
	{
		m_bDragFlag = false;
		pGO->clear();
		//*destPos = *sourPos;
		KImmediacyParam tagImmInfo;

		if ( sourObjInfo.d_skillID )
		{
			KSkillInfo tagSkillInfo;
			tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
			tagSkillInfo.dwDescStyle=0;
			g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, sourObjInfo.d_skillID );
			pGO->setObject( sourObjInfo );
			*destPos = *temPos;
			pGO->setType( TLGameObject::shortcut );
			tagImmInfo.nImmediacyType	= skill_immediacy_type;
			tagImmInfo.nPos				= nSel + 10;
			tagImmInfo.nID				= sourObjInfo.d_skillID;
			pGO->setTooltipText( AnsiToUtf8( tagSkillInfo.szDesc ) );
			if ( tagSkillInfo.bActive )
			{
				if ( pGO->getState() == TLGameObject::disableState )
				{
					pGO->disable( false );
				}				
			}
			else
			{
				pGO->disable( true );
			}
			g_pCoreShell->OperationRequest( GOI_ADD_IMMEDIACY, (unsigned int)&tagImmInfo, false );			
		}
		else
		{

			//KObjAtContRegion* pItem =	(KObjAtContRegion*)sourObj->getUserData();
			//if ( pItem )
			if ( nItemId > 0 )
			{
				FIND_ITEMINDEX_PARAM tagItemInfo;
				g_pCoreShell->GetGameData( GDI_GET_ITEM_PARTICULAR, (unsigned int)&tagItemInfo, nItemId ); //pItem->Obj.uId );
				if ( tagItemInfo.nGenre == item_target || (tagItemInfo.nGenre == item_ib && !tagItemInfo.bIBNoTarget ) )
				{
					KUiDragItem::GetSingleton().initItem();
					pGO->clear();
					pGO->show();
					if ( temPos )
					{
						delete temPos;
						temPos = NULL;
					}
					return false;
				}
				tagImmInfo.nImmediacyType	= item_immediacy_type;
				tagImmInfo.nPos				= nSel + 10;
				tagImmInfo.nID				= GenerateItemHashId(tagItemInfo.nGenre, tagItemInfo.nDetail, tagItemInfo.nParticular );
				int nItemCount = 0;
				nItemCount = g_pCoreShell->GetGameData( GDI_ITEM_COUNT, (unsigned int)&tagItemInfo, NULL );
				if ( nItemCount > 0 )
				{
					if ( pGO->getState() == TLGameObject::disableState )
					{
						sourObjInfo.d_state = TLGameObject::normalState;
					}
				}
				else
				{
					sourObjInfo.d_state = TLGameObject::disableState;
				}

				sourObjInfo.d_genre			=	tagItemInfo.nGenre;
				sourObjInfo.d_detail		=	tagItemInfo.nDetail;
				sourObjInfo.d_particular	=	tagItemInfo.nParticular;
				sourObjInfo.d_level			=	tagItemInfo.nLevel;
				sourObjInfo.d_group			=	tagItemInfo.nGroup;
				sourObjInfo.d_type			=	TLGameObject::shortcut;
				sourObjInfo.d_count			=	nItemCount;
				KItemInfo ItemInfo;
				g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&ItemInfo, nItemId ); //pItem->Obj.uId );
				sourObjInfo.d_EdgeframeIdx	=	ItemInfo.colour;
				pGO->setObject( sourObjInfo );
				*destPos = *temPos;
				pGO->setTooltipText( AnsiToUtf8( ItemInfo.szToolTip ) );
				g_pCoreShell->OperationRequest( GOI_ADD_IMMEDIACY, (unsigned int)&tagImmInfo, true );
			}
		}
	}
	
	if ( temPos )
	{
		delete temPos;
		temPos = NULL;
	}

	RefreshSelectedSkill();

	KUiDragItem::GetSingleton().initItem();
	return true;
}

void	KUiShortcutPlusWnd::onCreate( UIMDLEvent& rEvent	)
{

}

void	KUiShortcutPlusWnd::onRelease( UIMDLEvent& rEvent	)
{

}

void	KUiShortcutPlusWnd::onChange( UIMDLEvent& rEvent	)
{
	IUIMDLDataset* pIDataset = rEvent.pDataSet;
	if ( pIDataset )
	{
		UIMDLDatasetRecord &rRecord = pIDataset->getDataRecord( rEvent.nRecordIndex );
		KItemGroupCD_C* pCD = (KItemGroupCD_C*)rRecord.pRecordData;
		if ( pCD )
		{
			if ( pCD->ulCDTime == 0 && pCD->dwStartCount == 0 )
			{
				if ( pCD->eType == skill_immediacy_type )
				{
					BeginSkillCD( pCD->Id, pCD->ulCDTime );
				}
				else if ( pCD->eType == skill_common_coolingdown )
				{
					BeginCommonSkillCD( pCD->ulCDTime );
				}
				else
				{
					EndGroupCD( pCD );
				}
			}
			else
			{
				if ( pCD->eType == skill_immediacy_type )
				{
					BeginSkillCD( pCD->Id, pCD->ulCDTime );
				}
				else if ( pCD->eType == skill_common_coolingdown )
				{
					BeginCommonSkillCD( pCD->ulCDTime );
				}
				else
				{
					BeginGroupCD( pCD );
				}
			}
		}
	}
}


void	KUiShortcutPlusWnd::BeginSkillCD( int nSkillID, int ulCDTime )
{
	KSkillInfo tagSkillInfo;
	//g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo,nSkillID );	
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				if ( pGO && pGO->getState() != TLGameObject::idleState && !pGO->isEmpty() )
				{
					TLGameObject::GameObject sourObjInfo;
					pGO->getObject( sourObjInfo );
					sourObjInfo.d_skillID = g_pCoreShell->GetGameData( GDI_GET_CUR_SKILL_ID, sourObjInfo.d_skillID, NULL );
					if ( sourObjInfo.d_skillID == nSkillID  )
					{
						g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo,nSkillID );
						if( pGO->getState( ) == TLGameObject::coolingState && tagSkillInfo.bCDComplete )
						{
							pGO->setState( TLGameObject::normalState );
						}
						else
						{
							if ( sourObjInfo.d_state == TLGameObject::normalState 
								|| sourObjInfo.d_state == TLGameObject::hoverState )
							{
								int nCDTime = tagSkillInfo.nCoolingTime/1000;
								if ( nCDTime == ulCDTime || 0 == ulCDTime )
								{
									pGO->setCoolingTime(ulCDTime);
								}
								else
								{
									pGO->setCoolingTime(nCDTime, nCDTime - ulCDTime);
								}
								//pGO->setCoolingTime(ulCDTime);
								//pGO->intonate();
							}
						}
					}
				}
			}
		}
	}
}

bool	KUiShortcutPlusWnd::handleMouseMove0( const CEGUI::EventArgs& args )	
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 0 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 0 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove1( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 1 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 1 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove2( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 2 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 2 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove3( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 3 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 3 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove4( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 4 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 4 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove5( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 5 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 5 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove6( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 6 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 6 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove7( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 7 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 7 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove8( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 8 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 8 );		
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseMove9( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 9 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 9 );		
	}
	return true;
}

bool KUiShortcutPlusWnd::onMouseMove(  CEGUI::TLGameObject* pGO,int nSel  )
{
	m_bCast = false;

	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		m_bCast = true;
		m_bDragFlag = false;
	}

	if ( m_bDragFlag )
	{
		TLGameObject* destObj,* sourObj;
		TLGameObject::GameObject destObjInfo, sourObjInfo;
		destObj = pGO;
		destObj->getObject(destObjInfo);
		KObjAtContRegion *destPos = (KObjAtContRegion*)destObj->getUserData();

		sourObj = KUiDragItem::GetSingleton().getObj();
		sourObj->getObject(sourObjInfo);
		KObjAtContRegion* sourPos = (KObjAtContRegion*)sourObj->getUserData();
		
		sourObj->clear();
		if(sourObjInfo.d_type == TLGameObject::idle)
		{
			m_bDragFlag = false;
			if(destObjInfo.d_type == TLGameObject::shortcut)
			{
				sourObj->setObject(destObjInfo);
				*sourPos = *destPos;
				sourObj->setCanDrag(true);
			}
			pGO->clear();
			pGO->show();
			g_pCoreShell->OperationRequest( GOI_DEL_IMMEDIACY, nSel + 10, NULL );
		}
	}
	return true;
}

bool	KUiShortcutPlusWnd::handleMouseLeave( const CEGUI::EventArgs& args )
{
	m_bDragFlag = false;
	return true;
}


bool	KUiShortcutPlusWnd::handleMouseUp0( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 0 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 0 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp1( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 1 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 1 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp2( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 2 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 2 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp3( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 3 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 3 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp4( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 4 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 4 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp5( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 5 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 5 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp6( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 6 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 6 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp7( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 7 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 7 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp8( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 8 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 8 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

bool	KUiShortcutPlusWnd::handleMouseUp9( const CEGUI::EventArgs& args )
{
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		if ( m_bCast )
		{
			Intonate( 9 );
		}
	}
	else
	{
		if ( m_bCast && m_bDragFlag )
		{
			Intonate( 9 );
		}
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);

	if(arg->button == LeftButton)
	{
		m_bDragFlag = false;
	}

	return true;
}

void KUiShortcutPlusWnd::BeginCommonSkillCD( int ulCDTime )
{	
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				
				if ( pGO && !pGO->isEmpty() && (pGO->getState() == TLGameObject::normalState || pGO->getState() == TLGameObject::hoverState) )
				{
					TLGameObject::GameObject sourObjInfo;
					KSkillInfo tagSkillInfo;
					pGO->getObject( sourObjInfo );
					sourObjInfo.d_skillID = g_pCoreShell->GetGameData( GDI_GET_CUR_SKILL_ID, sourObjInfo.d_skillID, NULL );
					g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, sourObjInfo.d_skillID );
						
					if ( tagSkillInfo.bIsCommonCoolingDown )
					{
						// 开始计时
						pGO->setCoolingTime(ulCDTime); 
						pGO->intonate();
					}
				}
			}
		}
	}
}

bool KUiShortcutPlusWnd::IsObjExist( FIND_ITEMINDEX_PARAM obj, int &pos )
{
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_PLUS_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutPlusBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCU_PLUS_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutPlusBar/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				if ( pGO && pGO->getState() != TLGameObject::idleState )
				{
					TLGameObject::GameObject sourObjInfo;
					pGO->getObject( sourObjInfo );
					if ( sourObjInfo.d_skillID <= 0  )
					{
						if ( 
							sourObjInfo.d_genre == obj.nGenre
							&& sourObjInfo.d_detail == obj.nDetail
							&& sourObjInfo.d_particular == obj.nParticular
							&& sourObjInfo.d_level == obj.nLevel
							)
						{
							pos = nIdy;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void KUiShortcutPlusWnd::RefreshImmediacy( FIND_ITEMINDEX_PARAM obj )
{
	int pos = 0;
	if ( IsObjExist( obj, pos ) )
	{
		assert( pos >= 0 && pos < SHORTCU_PLUS_PER_BAR_COUNT );

		int nItemCount = g_pCoreShell->GetGameData( GDI_ITEM_COUNT, (unsigned int)&obj, NULL );
		
		if ( nItemCount > 0 )
		{
			int nItemID = g_pCoreShell->GetGameData( GDI_FIRST_ITEM_ID, (unsigned int)&obj, NULL );
			if ( nItemID > 0 )
			{
				KImmediacyParam tagImmInfo;
				tagImmInfo.nImmediacyType	= item_immediacy_type;
				tagImmInfo.nPos				= pos + SHORTCU_PLUS_PER_BAR_COUNT;
				tagImmInfo.nID				= GenerateItemHashId(obj.nGenre, obj.nDetail, obj.nParticular );
				g_pCoreShell->OperationRequest( GOI_ADD_IMMEDIACY, (unsigned int)&tagImmInfo, true );
			}
		}
	}
}