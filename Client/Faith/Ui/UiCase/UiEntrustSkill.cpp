/////////////////////////////////////////////////////////////
//
//		Kingsoft Blaze Game Studio. Copyright (C) 2008
//		Create time	:	07/17/2008
//		File base	:	UiEntrustController
//		File ext	:	h
//		Create by	:	DarkMagic(DuanMu)
//		Description	:	内置遥控器
//
/////////////////////////////////////////////////////////////

#include "Ui/UiCase/UiEntrustSkill.h"
#include "Ui/UiCase/UiEntrustComputer.h"
#include "CoreShell.h"

extern iCoreShell * g_pCoreShell;
using namespace CEGUI;

template<> 
KUiEntrustSkill* KUiWndSingleton<KUiEntrustSkill>::ms_Singleton	= NULL;

KUiEntrustSkill::KUiEntrustSkill( const CEGUI::String& id_name )
: KUiWndSingleton<KUiEntrustSkill>( id_name )
{

}

KUiEntrustSkill::~KUiEntrustSkill( void )
{

}

void KUiEntrustSkill::Init( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		char skillGO[COMMON_CLIENT_MSG_LEN_64];
		int nChildCnt = m_pThisWnd->getChildCount();
		for ( int skillGOIdx = 0; skillGOIdx < nChildCnt; ++skillGOIdx )
		{		
			sprintf( skillGO, "TaharezLook/EntrustSkillwnd/Skill%d", skillGOIdx );
			try
			{
				TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
				if ( pGO )
				{
					pGO->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiEntrustSkill::OnClickSkill, this) );
				}
			}
			catch (...)
			{
				// do nothings,just ignore;
			}
		}
	}
}

bool KUiEntrustSkill::ShowSkills(int nIdx)
{
	KUiEntrustSkill::Hide();
	ClearAllLRSkill();
	m_SkillIdx = nIdx;
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
			if (nIdx == ShouSkill)
			{
				if (m_SkillArray[nSkillIdx] > 685 || m_SkillArray[nSkillIdx] < 651)
				{
					continue;
				}
			}
/*			else
			{
				if (m_SkillArray[nSkillIdx] <= 685 && m_SkillArray[nSkillIdx] >= 651)
				{
					continue;
				}
			}*/

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
				continue;
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
			if (nIdx == AttackSkill)
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

			}
			else
			{
				// to do nothing.
			}

			char skillGO[COMMON_CLIENT_MSG_LEN_64];
			int nChildCnt = m_pThisWnd->getChildCount();
			if ( skillGOIdx >= 0 && skillGOIdx < nChildCnt )
			{		
				sprintf( skillGO, "TaharezLook/EntrustSkillwnd/Skill%d", skillGOIdx++ );
				try
				{
					TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
					if ( pGO )
					{
					/*	sprintf( skillGOKey, "%s/Key", skillGO );
						Window* pKey = pGO->getChild( skillGOKey );
						if ( pKey )
						{
							pKey->setText("");
						}*/
						pGO->setObject( tagGO );
						pGO->setTooltipText( AnsiToUtf8( tagSkillInfo.szDesc ) );
						if (m_SkillIdx == AttackSkill)
						{
							_shortcutkey::iterator it = m_lKeyList.find( skillGO );
							if ( it != m_lKeyList.end() )
							{
							/*	if ( pKey && it->second != "" && it->second != "--" )
								{
									pKey->setText( it->second.c_str() );
								}*/
							}
						}
						else
						{
							_shortcutkey::iterator it = m_rKeyList.find( skillGO );
							if ( it != m_rKeyList.end() )
							{
							/*	if ( pKey && it->second != "" && it->second != "--" )
								{
									pKey->setText( it->second.c_str() );
								}*/
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
	KUiEntrustSkill::Show();

	return m_nSkillKindCount > 0 ? true : false;
}

bool KUiEntrustSkill::OnClickSkill( const CEGUI::EventArgs& args )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	if(arg->button == LeftButton && arg->window )
	{
		TLGameObject* pGO = (TLGameObject*)arg->window; 
		if ( !pGO->isEmpty() )
		{
			SelectSkill(pGO, m_SkillIdx);
		}
	}

	KUiEntrustSkill::Hide();
	return true;
}

void KUiEntrustSkill::SelectSkill(TLGameObject * SkillObj, int nIdx)
{
	try
	{
		if (SkillObj != NULL)
		{
			KUiEntrustComputer::GetSingleton().SetSelectedSkill(SkillObj, nIdx);
		}
	}
	catch ( ...) 
	{
		// do nothings just ignore;
	}
}

void KUiEntrustSkill::ClearAllShortcutKey( void )
{
	m_lKeyList.clear();
	m_rKeyList.clear();
}

void KUiEntrustSkill::ClearAllLRSkill( void )
{
	int nChildCnt = m_pThisWnd->getChildCount();
	for ( int skillGOIdx = 0; skillGOIdx < nChildCnt; ++skillGOIdx )
	{
		char skillGO[COMMON_CLIENT_MSG_LEN_64];
		sprintf( skillGO, "TaharezLook/EntrustSkillwnd/Skill%d", skillGOIdx );
		try
		{
			TLGameObject*pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( skillGO ));
		/*	sprintf( skillGOKey, "TaharezLook/LRSkillWnd/LRSkill%d/Key", skillGOIdx );
			Window* pKey = pGO->getChild( skillGOKey );*/
			if (pGO)
			{
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