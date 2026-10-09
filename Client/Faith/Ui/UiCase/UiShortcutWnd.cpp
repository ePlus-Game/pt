 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/15/2006 2:10
//      File_base        : KUiShortcutWnd
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiShortcutWnd.h"
#include "CoreUseNameDef.h"
#include "GameDataDef.h"
#include "Coreshell.h"
#include "UiDragItem.h"
#include "../../Login/Login.h"
#include "../ShortcutKey.h"
#include "UiShortcutKeySetting.h"
#include "../UiConfigManager.h"
#include "UiGameSetting.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiLRSkillWnd* KUiWndSingleton<KUiLRSkillWnd>::ms_Singleton	= NULL;

KUiLRSkillWnd::KUiLRSkillWnd( const CEGUI::String& id_name ):
KUiWndSingleton<KUiLRSkillWnd>( id_name )
{
}

KUiLRSkillWnd::~KUiLRSkillWnd( void )
{
}

void	KUiLRSkillWnd::Init( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		char skillGO[COMMON_CLIENT_MSG_LEN_64];
		int nChildCnt = m_pThisWnd->getChildCount();
		for ( int skillGOIdx = 0; skillGOIdx < nChildCnt; ++skillGOIdx )
		{		
			sprintf( skillGO, "TaharezLook/LRSkillWnd/LRSkill%d", skillGOIdx );
			try
			{
				TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
				if ( pGO )
				{
					pGO->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiLRSkillWnd::HandleSelectSkill, this) );
				}
			}
			catch (...)
			{
				// do nothings,just ignore;
			}
		}
	}
	try
	{
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiLRSkillWnd::HandleKeyDown, this) );
	}
	catch (...)
	{
		// do nothings,just ignore;
	}
}

bool KUiLRSkillWnd::ShowSkills( bool bLeft )
{
	KUiLRSkillWnd::Hide();
	ClearAllLRSkill();
	m_bLeft = bLeft;
	int skillGOIdx = 0;
	m_nSkillKindCount = g_pCoreShell->GetGameData( GDI_SKILL_KIND_LIST, (unsigned int)m_SkillKindArray, MAX_SKILL_COUNT );
	for ( int nSkillKindIdx = 0; nSkillKindIdx < m_nSkillKindCount; ++nSkillKindIdx )
	{
		int nParam = 0;
		nParam |= MAX_SKILL_COUNT << 16;
		nParam |= m_SkillKindArray[nSkillKindIdx];  
		m_nSkillCount = g_pCoreShell->GetGameData( GDI_SKILL_LIST, (unsigned int)m_SkillArray, nParam ) + 2;
		for ( int nSkillIdx = 0; nSkillIdx < m_nSkillCount; ++nSkillIdx )
		{


			KSkillInfo tagSkillInfo;
			tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
			tagSkillInfo.dwDescStyle=0;
            tagSkillInfo.bNextDescAvailable=true;
			tagSkillInfo.dwNextDescStyle=1;
			
			char szStudyTip[MAX_STUDY_TIP_SIZE];
			tagSkillInfo.nStudyTipBuffSize = MAX_STUDY_TIP_SIZE;
			tagSkillInfo.szStudyTipBuff=(char *)szStudyTip;
			if ( nSkillIdx == m_nSkillCount - 1 && nSkillKindIdx == m_nSkillKindCount -1 )
			{
				KUiPlayerAttribute tagPlayerAttribute;
				int nNormalSkill = KNIGHT_NORMALSKILL_ID;
				g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&tagPlayerAttribute, NULL );
				switch(tagPlayerAttribute.nSeries)
				{
				case enRoleType_Knight:
					nNormalSkill = KNIGHT_NORMALSKILL_ID;
					break;
				case enRoleType_Enchanter:
					nNormalSkill = ENCHANTER_NORMALSKILL_ID;
					break;
				case enRoleType_Monstrous:
					nNormalSkill = MONSTROUS_NORMAILSKILL_ID;
					break;
				}
				g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, nNormalSkill );	
				if ( tagSkillInfo.szName[0] == 0 )
				{
					continue;
				}					 
			}
			else if ( nSkillIdx == m_nSkillCount - 2 && nSkillKindIdx == m_nSkillKindCount -1 )
			{
				g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, RUN_SKILL_ID );	
				if ( tagSkillInfo.szName[0] == 0 )
				{
					continue;
				}					 
			}
			else
			{
				if ( nSkillIdx == m_nSkillCount - 1 || nSkillIdx == m_nSkillCount - 2 )
				{
					continue;
				}
				g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, m_SkillArray[nSkillIdx] );	
			}			

			if ( !tagSkillInfo.bActive || 
				!tagSkillInfo.bCanHumanUse ||
				tagSkillInfo.bPassivity ||
				tagSkillInfo.nLevel == 0 )
			{
				continue;
			}
			if ( m_bLeft )
			{
				if ( tagSkillInfo.eSkillLR == NoneSkill || tagSkillInfo.eSkillLR == RightOnlySkill )
				{
					continue;
				}
			}
			else
			{
				if ( tagSkillInfo.eSkillLR == NoneSkill || tagSkillInfo.eSkillLR == leftOnlySkill )
				{
					continue;
				}
			}

			TLGameObject::GameObject tagGO;
			tagGO.d_gameobject = AnsiToUtf8( tagSkillInfo.szIconName );
			tagGO.d_type = TLGameObject::shortcut;
			tagGO.d_coolingTime = tagSkillInfo.nCoolingTime / 1000;
			tagGO.d_passivity = tagSkillInfo.bPassivity;
			tagGO.d_skillID = m_SkillArray[nSkillIdx];
			if ( nSkillIdx == m_nSkillCount - 1 && nSkillKindIdx == m_nSkillKindCount -1 )
			{
				KUiPlayerAttribute tagPlayerAttribute;
				int nNormalSkill = KNIGHT_NORMALSKILL_ID;
				g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&tagPlayerAttribute, NULL );
				switch(tagPlayerAttribute.nSeries)
				{
				case enRoleType_Knight:
					nNormalSkill = KNIGHT_NORMALSKILL_ID;
					break;
				case enRoleType_Enchanter:
					nNormalSkill = ENCHANTER_NORMALSKILL_ID;
					break;
				case enRoleType_Monstrous:
					nNormalSkill = MONSTROUS_NORMAILSKILL_ID;
					break;
				}
				tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
				tagSkillInfo.dwDescStyle=0;
				tagSkillInfo.bNextDescAvailable=true;
				tagSkillInfo.dwNextDescStyle=1;
				
				g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, nNormalSkill );	
				if ( tagSkillInfo.szName[0] != 0 )
				{
					tagGO.d_skillID = nNormalSkill;
				}					 
			}
			else if ( nSkillIdx == m_nSkillCount - 2 && nSkillKindIdx == m_nSkillKindCount -1 )
			{
				tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
				tagSkillInfo.dwDescStyle=0;
				tagSkillInfo.bNextDescAvailable=true;
				tagSkillInfo.dwNextDescStyle=1;

				g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, RUN_SKILL_ID );	
				if ( tagSkillInfo.szName[0] != 0 )
				{
					tagGO.d_skillID = RUN_SKILL_ID;
				}
			}
			else
			{
				// to do nothing.
			}			

			char skillGO[COMMON_CLIENT_MSG_LEN_64];
			int nChildCnt = m_pThisWnd->getChildCount();
			if ( skillGOIdx >= 0 && skillGOIdx < nChildCnt )
			{		
				sprintf( skillGO, "TaharezLook/LRSkillWnd/LRSkill%d", skillGOIdx++ );
				try
				{
					TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
					if ( pGO )
					{
						char skillGOKey[COMMON_CLIENT_MSG_LEN_64];
						sprintf( skillGOKey, "%s/Key", skillGO );
						Window* pKey = pGO->getChild( skillGOKey );
						if ( pKey )
						{
							pKey->setText("");
						}
						pGO->setObject( tagGO );			
						pGO->setTooltipText( AnsiToUtf8( tagSkillInfo.szDesc ) );
						if (m_bLeft)
						{
							_shortcutkey::iterator it = m_lKeyList.find( skillGO );
							if ( it != m_lKeyList.end() )
							{
								if ( pKey && it->second != "" && it->second != "--" )
								{
									pKey->setText( it->second.c_str() );
								}
							}
						}
						else
						{
							_shortcutkey::iterator it = m_rKeyList.find( skillGO );
							if ( it != m_rKeyList.end() )
							{
								if ( pKey && it->second != "" && it->second != "--" )
								{
									pKey->setText( it->second.c_str() );
								}
							}
						}
					}
				}
				catch (...)
				{
					// do nothings just ignore;				
				}
			}
		}
	}
	KUiLRSkillWnd::Show();

	return m_nSkillKindCount > 0 ? true : false;
}

bool KUiLRSkillWnd::HandleSelectSkill( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton && arg->window )
	{
		TLGameObject* pGO = (TLGameObject*)arg->window; 
		if ( !pGO->isEmpty() )
		{
			SelectSkill( Utf8ToAnsi( arg->window->getName() ), m_bLeft );
		}
		
	}

	KUiLRSkillWnd::Hide();
	return true;
}

bool KUiLRSkillWnd::HandleKeyDown( const CEGUI::EventArgs& args )
{
	KeyEventArgs* arg = (KeyEventArgs*)(&args);
	if( arg->window )
	{
		if ( ms_Singleton && ms_Singleton->m_pThisWnd )
		{
			char skillGO[COMMON_CLIENT_MSG_LEN_64];
			int nChildCnt = m_pThisWnd->getChildCount();
			for ( int skillGOIdx = 0; skillGOIdx < nChildCnt; ++skillGOIdx )
			{		
				sprintf( skillGO, "TaharezLook/LRSkillWnd/LRSkill%d", skillGOIdx );
				try
				{
					TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
					if ( pGO )
					{
						Point mouesPt = MouseCursor::getSingleton().getPosition();
						if ( pGO->isHit( mouesPt ) )
						{							
							char szBuff[COMMON_CLIENT_MSG_LEN_256];
							if ( m_bLeft )
							{
								sprintf( szBuff, "SetLSkill('%s')", Utf8ToAnsi( pGO->getName() ));
							}
							else
							{
								sprintf( szBuff, "SetRSkill('%s')", Utf8ToAnsi( pGO->getName() ));
							}
							KUiSKSettingMgr::getSinglton().bindAKeyByCmd( szBuff, KUiSKSetting::getSinglton().getKeyName(arg->scancode).c_str() );
							KUiSKSettingMgr::getSinglton().save();
							std::string scriptPath = KUiSKSettingMgr::getSinglton().getCurValidSKPath();
							KShortcutKeyCentre::LoadScript(const_cast<char*>(scriptPath.c_str()));
							KUiSKSettingMgr::getSinglton().loadUserSettings();
							KUiSKSetting::getSinglton().hide();

							ShowSkills( m_bLeft );
							return true;
						}
					}
				}
				catch (...)
				{
					// do nothings,just ignore;
				}
			}
		}



	}
	return true;

}

void KUiLRSkillWnd::SelectSkill( const char* szName, bool bLeft )
{
	try
	{
		TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( szName ));
		if ( pGO )
		{
			std::string str = szName;
			str += "/Key";
			Window* pKey = pGO->getChild( str.c_str() );
			if ( pKey )
			{
				KIniFile iniFile;
				iniFile.Load(m_filename);				
				if ( bLeft )
				{
					KUiShortcutWnd::GetSingleton().SelectLSkill( pGO, pKey->getText() );
					iniFile.WriteString(m_rolename, "LeftSkill", szName);
				}
				else
				{					
					KUiShortcutWnd::GetSingleton().SelectRSkill( pGO, pKey->getText() );
					iniFile.WriteString(m_rolename, "RightSkill", szName);
				}		 		
				iniFile.Save(m_filename);
			}
		}
	}
	catch ( ...) 
	{
		// do nothings just ignore;	
	}

}

void KUiLRSkillWnd::ClearAllShortcutKey( void )
{
	m_lKeyList.clear();
	m_rKeyList.clear();
}

void KUiLRSkillWnd::ClearAllLRSkill( void )
{
	int nChildCnt = m_pThisWnd->getChildCount();
	for ( int skillGOIdx = 0; skillGOIdx < nChildCnt; ++skillGOIdx )
	{
		char skillGO[COMMON_CLIENT_MSG_LEN_64];
		char skillGOKey[COMMON_CLIENT_MSG_LEN_64];
		sprintf( skillGO, "TaharezLook/LRSkillWnd/LRSkill%d", skillGOIdx );
		try
		{
			TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
			sprintf( skillGOKey, "TaharezLook/LRSkillWnd/LRSkill%d/Key", skillGOIdx );
			Window* pKey = pGO->getChild( skillGOKey );
			if ( pGO && pKey )
			{
				pKey->setText("");
				pGO->clear();
				pGO->show();
			}
		}
		catch (...)
		{
			// do nothings just ignore;				
		}	
	}
}

void KUiLRSkillWnd::AddShortcutKey( bool bLeft, const char* szName, const char* szKey )
{
	if ( bLeft )
	{
		m_lKeyList[szName] = szKey;
	}
	else
	{
		m_rKeyList[szName] = szKey;
	}
	/*
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		try
		{
			TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( szName ));
			if ( pGO )
			{
				char skillGOKey[COMMON_CLIENT_MSG_LEN_64];
				sprintf( skillGOKey, "%s/Key", szName );
				Window* pKey = pGO->getChild( skillGOKey );
				if ( pKey )
				{
					pKey->setText( szKey );
				}
			}

		}
		catch (...)
		{
			// do nothings,just ignore;
		}
	}	//*/
}

/************************************************************************/
/*						KUiShortcutWnd system                           */
/************************************************************************/

template<> 
KUiShortcutWnd* KUiWndSingleton<KUiShortcutWnd>::ms_Singleton	= NULL;

KUiShortcutWnd::KUiShortcutWnd( const CEGUI::String& id_name ):
KUiWndSingleton<KUiShortcutWnd>( id_name )
{
	m_bLoading = false;
	m_bCast = false;
	m_bDragFlag = false;
	ms_Singleton->m_nCurrentBarIdx = 0;
	IUIMDLDataset* pDataset = NULL;
	int nRet = m_pUiMDLManager->queryDataSet( itemgroupcd_dataset, &pDataset );
	if ( success_errorcode != nRet && dataset_areadycreated_errorcode != nRet )
	{
		m_pUiMDLManager->createDataSet( itemgroupcd_dataset );
		m_pUiMDLManager->queryDataSet( itemgroupcd_dataset, &pDataset );
		pDataset->setEventHandle( ms_Singleton );
	} 
	ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );

	m_pThisWnd->setZLevel(Window::Bottom);
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_nCurrentBarIdx = 0;
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_32];
			sprintf( szName, "%d_", nIdx );
			Window* pGO = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_SHORTCUTBAR, szName, "", NULL, NULL, true  );
			if ( pGO )
			{
				Point absPoint;
				absPoint.d_x = 0;
				absPoint.d_y = 0;
				Size  absSize	= ms_Singleton->m_pThisWnd->getAbsoluteSize(); 
				absSize.d_width -= 2 * GAMEOBJECT_WIDTH_MID;
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

		for ( nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
			Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
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
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiShortcutWnd::handleMouseLeave, this) ) ;
		m_left = (TLGameObject *)(m_pThisWnd->getChild("TaharezLook/ShortcutWnd/ShortcutL"));
		m_right = (TLGameObject *)(m_pThisWnd->getChild("TaharezLook/ShortcutWnd/ShortcutR"));
		if ( m_left && m_right )
		{
			m_left->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiShortcutWnd::handleL, this) ) ;
			m_right->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiShortcutWnd::handleR, this) ) ;
		}
	}
}

KUiShortcutWnd::~KUiShortcutWnd()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
			Window *pBar = m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
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

void KUiShortcutWnd::Init( void	)
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		KUiCfgLoader& cm = KUiCfgLoader::getSingleton();
		const KUiCfgLoader::DefaultLRSkill& defaultLRSkill = cm.getDefaultLRSkill();
		KUiPlayerAttribute tagPlayerAttribute;
		g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&tagPlayerAttribute, NULL );
		int nSeries = tagPlayerAttribute.nSeries;
		if ( nSeries >= 0 && nSeries <= 2)
		{
			if ( defaultLRSkill.lrSkill[nSeries].lSkillID )
			{
				LoadDefaultLSkill( defaultLRSkill.lrSkill[nSeries].lSkillID, true );	
			}
			if ( defaultLRSkill.lrSkill[nSeries].rSkillID )
			{
				LoadDefaultLSkill( defaultLRSkill.lrSkill[nSeries].rSkillID, false);	
			}
			
			
		}

	}	
}

void KUiShortcutWnd::LoadDefaultLSkill( int nSkillID, bool bLeft )
{


	char szStudyTip[MAX_STUDY_TIP_SIZE];
	KSkillInfo tagSkillInfo;
	tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
	tagSkillInfo.dwDescStyle=0;
	tagSkillInfo.bNextDescAvailable=true;
	tagSkillInfo.dwNextDescStyle=1;
	tagSkillInfo.nStudyTipBuffSize = MAX_STUDY_TIP_SIZE;
	tagSkillInfo.szStudyTipBuff=(char *)szStudyTip;
	g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, nSkillID );		


	TLGameObject::GameObject tagGO;
	tagGO.d_gameobject = AnsiToUtf8( tagSkillInfo.szIconName );
	tagGO.d_type = TLGameObject::shortcut;
	tagGO.d_coolingTime = tagSkillInfo.nCoolingTime / 1000;
	tagGO.d_passivity = tagSkillInfo.bPassivity;
	tagGO.d_skillID = nSkillID;

	int nNormalSkill = KNIGHT_NORMALSKILL_ID;


	if ( bLeft )
	{
		if ( m_left )
		{
			m_left->clear();
			m_left->setObject( tagGO );
			m_left->setTooltipText( szStudyTip );
			TLGameObject::GameObject GO;
			m_left->getObject( GO );
			if ( GO.d_skillID > 0 )
			{
				g_pCoreShell->SelectSkill( GO.d_skillID );
				g_pCoreShell->OperationRequest( GOI_SET_L_SKILL, GO.d_skillID, NULL );
				m_left->requestRedraw();
			}

		}
	}
	else
	{
		if ( m_right )
		{
			m_right->clear();
			m_right->setObject( tagGO );
			m_right->setTooltipText( szStudyTip );
			TLGameObject::GameObject GO;
			m_right->getObject( GO );		
			if ( GO.d_skillID > 0 )
			{
				g_pCoreShell->OperationRequest( GOI_SET_R_SKILL, GO.d_skillID, NULL );
				m_right->requestRedraw();
			}		
		}
	}
}

void KUiShortcutWnd::AddImmediacy( KImmediacyParam* pImm )
{
	if ( pImm == NULL || (pImm && pImm->nPos >= MAX_IMMEDIACY_ITEM) )
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
			tagGO.d_EdgeframeIdx = tagItemInfo.colour;
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

void KUiShortcutWnd::DelImmediacy( int nPos )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nPos ));
	if ( pGO )
	{
		pGO->clear();
		pGO->show();
	}
}

void KUiShortcutWnd::Show( void )
{
	KUiWndSingleton<KUiShortcutWnd>::Show();
}

void KUiShortcutWnd::Hide( void )
{
	if ( g_LoginLogic.GetStatus() != LL_S_IN_GAME  )
	{
		ms_Singleton->ClearAllShortcut();
		ms_Singleton->m_bLoading = false;
	}
	KUiWndSingleton<KUiShortcutWnd>::Hide();
}
unsigned int KUiShortcutWnd::BeginGroupCD( KItemGroupCD_C* pGroupCD )
{
	if ( pGroupCD == NULL )
	{
		return 0;
	}
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = NULL;
				if ( nIdy == SHORTCUT_PER_BAR_COUNT )
				{
					pGO = ms_Singleton->m_left;
				}
				else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
				{
					pGO = ms_Singleton->m_right;
				}
				else
				{
					pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				}
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
unsigned int KUiShortcutWnd::EndGroupCD( KItemGroupCD_C* pGroupCD )
{
	if ( pGroupCD == NULL )
	{
		return 0;
	}
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = NULL;
				if ( nIdy == SHORTCUT_PER_BAR_COUNT )
				{
					pGO = ms_Singleton->m_left;
				}
				else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
				{
					pGO = ms_Singleton->m_right;
				}
				else
				{
					pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				}
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

unsigned int KUiShortcutWnd::UpdateData( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && !ms_Singleton->m_bLoading)
	{
		ms_Singleton->Init();

		char szName[COMMON_CLIENT_MSG_LEN_256];
		KIniFile iniFile;
		iniFile.Load(KUiLRSkillWnd::GetSingleton().m_filename);				
		iniFile.GetString(KUiLRSkillWnd::GetSingleton().m_rolename, "LeftSkill", "", szName, COMMON_CLIENT_MSG_LEN_256);
		bool bOkL = KUiLRSkillWnd::GetSingleton().ShowSkills(true);
		KUiLRSkillWnd::Hide();
		KUiLRSkillWnd::GetSingleton().SelectSkill( szName, true );

		iniFile.GetString(KUiLRSkillWnd::GetSingleton().m_rolename, "RightSkill", "", szName, COMMON_CLIENT_MSG_LEN_256);
		bool bOkR = KUiLRSkillWnd::GetSingleton().ShowSkills(false);
		KUiLRSkillWnd::Hide();
		KUiLRSkillWnd::GetSingleton().SelectSkill( szName, false );
		if ( bOkL && bOkR )
		{
			ms_Singleton->m_bLoading = true;
		}		
	}

	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = NULL;
				if ( nIdy == SHORTCUT_PER_BAR_COUNT )
				{
					pGO = ms_Singleton->m_left;
				}
				else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
				{
					pGO = ms_Singleton->m_right;
				}
				else
				{
					pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
					if ( pGO )
					{
						Window* pKey = pGO->getChild( pGO->getName() + "/Key" );
						if ( pKey )
						{
							pKey->setText(ms_Singleton->m_KeyList[nIdy].c_str());
						}
					}
				}
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
								
								if ( nIdy < 10 )
								{
									g_pCoreShell->OperationRequest( GOI_DEL_IMMEDIACY, nIdy, NULL );
								}								
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

unsigned int KUiShortcutWnd::RefreshSelectedSkill( void )
{
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = NULL;
				if ( nIdy == SHORTCUT_PER_BAR_COUNT )
				{
					pGO = ms_Singleton->m_left;
				}
				else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
				{
					pGO = ms_Singleton->m_right;
				}
				else
				{
					pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				}
				
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

void KUiShortcutWnd::Intonate( int nIdx )//, bool bAlt )
{
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nIdx )); 
	if ( pGO && !pGO->isEmpty() )
	{
		TLGameObject::GameObject tmpGO;
		pGO->getObject( tmpGO );

		if ( tmpGO.d_skillID > 0 )
		{
			g_pCoreShell->NextSkill(tmpGO.d_skillID, KShortcutKeyCentre::ms_bAltPressed);
		}

		if ( tmpGO.d_state == TLGameObject::normalState ||
			tmpGO.d_state == TLGameObject::hoverState ||
			tmpGO.d_state == TLGameObject::pushedState )
		{
			if ( tmpGO.d_skillID <= 0 )
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
						g_pCoreShell->OperationRequest( GOI_DEL_IMMEDIACY, nIdx, NULL );
					}//*/
				}
			}
		}
	}
}

void KUiShortcutWnd::SelectSkill( int nIdx )
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

void KUiShortcutWnd::Disable(  int nIdx  )
{
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->GetSelShortcutKey( nIdx ));
	if ( pGO )
	{
		pGO->disable( true );
	}	
}

void	KUiShortcutWnd::ClearAllShortcut( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
			Window *pBar = m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
				if ( pBar )
				{
					TLGameObject *pGO = NULL;
					if ( nIdy == SHORTCUT_PER_BAR_COUNT )
					{
						pGO = ms_Singleton->m_left;
					}
					else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
					{
						pGO = ms_Singleton->m_right;
					}
					else
					{
						pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
					}
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

void	KUiShortcutWnd::SaveAllItem( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
			Window *pBar = m_pThisWnd->getChild( szName );
			for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT; ++nIdy )
			{
				sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
				if ( pBar )
				{
					TLGameObject *pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
					if ( pGO && !pGO->isEmpty() && !pGO->isSkill() )
					{
						TLGameObject::GameObject sourObjInfo;
						pGO->getObject( sourObjInfo );
						KImmediacyParam tagImmInfo;
						tagImmInfo.nImmediacyType	= item_immediacy_type;
						tagImmInfo.nPos				= nIdy;
						tagImmInfo.nID				= GenerateItemHashId(sourObjInfo.d_genre, sourObjInfo.d_detail, sourObjInfo.d_particular );
						g_pCoreShell->OperationRequest( GOI_ADD_IMMEDIACY, (unsigned int)&tagImmInfo, true );

					}
				}
			}
		}
	}
}

Window* KUiShortcutWnd::GetSelShortcutKey( int nSel )
{
	char szName[COMMON_CLIENT_MSG_LEN_64];
	sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", m_nCurrentBarIdx, nSel );	
	Window *pBar = ms_Singleton->GetCurrentBar();
	if ( pBar )
	{
		return pBar->getChild( szName );
	}
	return NULL;
}

Window* KUiShortcutWnd::GetCurrentBar( void )
{
	char szName[COMMON_CLIENT_MSG_LEN_32];
	sprintf( szName, "%d_TaharezLook/ShortcutBar", m_nCurrentBarIdx );
	return m_pThisWnd->getChild( szName );
}

void KUiShortcutWnd::SetCurrentBar( int nSel )
{
	m_nCurrentBarIdx = nSel;
	char szName[COMMON_CLIENT_MSG_LEN_64];
	sprintf( szName, "%d_TaharezLook/ShortcutBar", m_nCurrentBarIdx );
	Window *pBar = m_pThisWnd->getChild( szName );
	if ( pBar )
	{
		for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
			Window *pBarInFor = m_pThisWnd->getChild( szName );
			if ( pBarInFor )
			{
				pBarInFor->hide();
			}
		}
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT; ++nIdy )
		{
			Window *pShortcut = GetSelShortcutKey( nIdy );
			if ( pShortcut )
			{
				pShortcut->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(m_funList[nIdy], this) ) ;
				pShortcut->subscribeEvent(TLGameObject::EventMouseMove, Event::Subscriber(m_funListPlus[nIdy], this) ) ;
				pShortcut->subscribeEvent(TLGameObject::EventMouseButtonUp, Event::Subscriber(m_funListCast[nIdy], this) ) ;
			}
		}
		pBar->show();
	}

}

bool KUiShortcutWnd::handleShortcut1( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut2( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut3( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut4( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut5( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut6( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut7( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut8( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut9( const CEGUI::EventArgs& args )
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

bool KUiShortcutWnd::handleShortcut0( const CEGUI::EventArgs& args )
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
		//Intonate( 0, KShortcutKeyCentre::ms_bAltPressed );
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseLeave( const CEGUI::EventArgs& args )
{
	m_bDragFlag = false;
	return true;
}

bool KUiShortcutWnd::onLBDown( CEGUI::TLGameObject* pGO,int nSel )
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
			tagImmInfo.nPos				= nSel;
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
				tagImmInfo.nPos				= nSel;
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


void	KUiShortcutWnd::onCreate( UIMDLEvent& rEvent	)
{

}

void	KUiShortcutWnd::onRelease( UIMDLEvent& rEvent	)
{

}

void	KUiShortcutWnd::onChange( UIMDLEvent& rEvent	)
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

void	KUiShortcutWnd::BeginSkillCD( int nSkillID, int ulCDTime )
{
	KSkillInfo tagSkillInfo;
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = NULL;
				if ( nIdy == SHORTCUT_PER_BAR_COUNT )
				{
					pGO = ms_Singleton->m_left;
				}
				else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
				{
					pGO = ms_Singleton->m_right;
				}
				else
				{
					pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				}
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
							// 恢复正常状态
							pGO->setState( TLGameObject::normalState );
						}
						else
						{
							if ( sourObjInfo.d_state == TLGameObject::normalState 
								|| sourObjInfo.d_state == TLGameObject::hoverState )
							{
								// 开始计时
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

bool KUiShortcutWnd::onMouseMove(  CEGUI::TLGameObject* pGO,int nSel  )
{
	m_bCast = false;
	
	if (KUiGameSetting::GetSingleton().GetLockShortcut())
	{
		m_bDragFlag = false;
		m_bCast = true;
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
			g_pCoreShell->OperationRequest( GOI_DEL_IMMEDIACY, nSel, NULL );
		}
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove0( const CEGUI::EventArgs& args )	
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 0 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 0 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove1( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 1 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 1 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove2( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 2 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 2 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove3( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 3 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 3 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove4( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 4 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 4 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove5( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 5 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 5 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove6( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 6 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 6 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove7( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 7 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 7 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove8( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 8 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 8 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseMove9( const CEGUI::EventArgs& args )
{
	TLGameObject *pGO = static_cast<TLGameObject*>(GetSelShortcutKey( 9 ));
	if ( pGO )
	{
		return onMouseMove( pGO, 9 );		
	}
	return true;
}

bool	KUiShortcutWnd::handleMouseUp0( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp1( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp2( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp3( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp4( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp5( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp6( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp7( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp8( const CEGUI::EventArgs& args )
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

bool	KUiShortcutWnd::handleMouseUp9( const CEGUI::EventArgs& args )
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

void KUiShortcutWnd::BeginCommonSkillCD( int ulCDTime )
{	
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT + 2; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
			if ( pBar )
			{
				TLGameObject *pGO = NULL;
				if ( nIdy == SHORTCUT_PER_BAR_COUNT )
				{
					pGO = ms_Singleton->m_left;
				}
				else if ( nIdy == SHORTCUT_PER_BAR_COUNT + 1 )
				{
					pGO = ms_Singleton->m_right;
				}
				else
				{
					pGO = static_cast<TLGameObject*>(pBar->getChild( szName ));
				}
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

void	KUiShortcutWnd::ClearAllShortcutKey( void )
{
	m_KeyList.clear();
}

void	KUiShortcutWnd::AddShortcutKey( int nIdx, const char* szKey )
{
	m_KeyList[nIdx] = szKey;
}

bool KUiShortcutWnd::IsObjExist( FIND_ITEMINDEX_PARAM obj, int &pos )
{
	for ( int nIdx = 0; nIdx < SHORTCUTBAR_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/ShortcutBar", nIdx );
		Window *pBar = ms_Singleton->m_pThisWnd->getChild( szName );
		for ( int nIdy = 0; nIdy < SHORTCUT_PER_BAR_COUNT; ++nIdy )
		{
			sprintf( szName, "%d_TaharezLook/ShortcutWnd/Shortcut%d", nIdx, nIdy );	
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

bool	KUiShortcutWnd::handleL( const CEGUI::EventArgs& args )
{
	if (  KUiLRSkillWnd::IsVisible() )
	{
		KUiLRSkillWnd::Hide();
	}
	else
	{
		KUiLRSkillWnd::GetSingleton().ShowSkills( true );
	}
	
	return true;
}

bool	KUiShortcutWnd::handleR( const CEGUI::EventArgs& args )
{
	if ( KUiLRSkillWnd::IsVisible() )
	{
		KUiLRSkillWnd::Hide();
	}
	else
	{
		KUiLRSkillWnd::GetSingleton().ShowSkills( false );
	}
	
	return true;
}

void KUiShortcutWnd::RefreshImmediacy( FIND_ITEMINDEX_PARAM obj )
{
	int pos = 0;
	if ( IsObjExist( obj, pos ) )
	{
		assert( pos >= 0 && pos < SHORTCUT_PER_BAR_COUNT );

		int nItemCount = g_pCoreShell->GetGameData( GDI_ITEM_COUNT, (unsigned int)&obj, NULL );
		
		if ( nItemCount > 0 )
		{
			int nItemID = g_pCoreShell->GetGameData( GDI_FIRST_ITEM_ID, (unsigned int)&obj, NULL );
			if ( nItemID > 0 )
			{
				KImmediacyParam tagImmInfo;
				tagImmInfo.nImmediacyType	= item_immediacy_type;
				tagImmInfo.nPos				= pos;
				tagImmInfo.nID				= GenerateItemHashId(obj.nGenre, obj.nDetail, obj.nParticular );
				g_pCoreShell->OperationRequest( GOI_ADD_IMMEDIACY, (unsigned int)&tagImmInfo, true );
			}
		}
	}
}

void	KUiShortcutWnd::SelectLSkill( TLGameObject* pGO, const String& key  )
{
	if ( m_left && pGO )
	{
		m_left->clear();
		m_left->setObject( pGO->getObject() );
		m_left->setTooltipText( pGO->getTooltipText() );
		TLGameObject::GameObject GO;
		m_left->getObject( GO );
		if ( GO.d_skillID > 0 )
		{
			Window* pKey = m_left->getChild( m_left->getName()+ "/Key" );
			if ( pKey )
			{
				pKey->setText( key );
			}
			g_pCoreShell->SelectSkill( GO.d_skillID );
			g_pCoreShell->OperationRequest( GOI_SET_L_SKILL, GO.d_skillID, NULL );
			m_left->requestRedraw();
		}
	}
}

void	KUiShortcutWnd::SelectRSkill( TLGameObject* pGO, const String& key  )
{
	if ( m_right && pGO )
	{
		m_right->clear();
		m_right->setObject( pGO->getObject() );
		m_right->setTooltipText( pGO->getTooltipText() );
		TLGameObject::GameObject GO;
		m_right->getObject( GO );		
		if ( GO.d_skillID > 0 )
		{
			Window* pKey = m_right->getChild( m_right->getName()+ "/Key" );
			if ( pKey )
			{
				pKey->setText( key );
			}
			g_pCoreShell->OperationRequest( GOI_SET_R_SKILL, GO.d_skillID, NULL );
			m_right->requestRedraw();
		}		
	}
}