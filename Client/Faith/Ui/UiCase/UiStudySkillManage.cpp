#include "KWin32.h"
#include "UiStudySkillManage.h"
#include "UiLogin.h"
#include "KWin32Wnd.h"
#include "../../Login/Login.h"
#include "CoreShell.h"
#include "TLGameObject.h"
#include "SkillDef.h"
#include "TLGameObject.h"
#include "UiHelpCentre.h"
#include "UiDragItem.h"
#include "TLStatic.h"
#include "ui/UiCase/UiComMsgBox.h"
#include "Ui/KMessageCentre.h"
#include "ui/UiConfigManager.h"

#define MAX_STUDY_KIND     6
extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiStudySkillManage* KUiWndSingleton<KUiStudySkillManage>::ms_Singleton	= NULL;

KUiStudySkillManage::KUiStudySkillManage( const CEGUI::String& id_name ):
KUiWndSingleton<KUiStudySkillManage>( id_name )
{
	m_nSkillKindCount	= 0;
	m_nSkillCount		= 0;
	m_OldSkillKindIdx	= 0;
	m_nCurrentSelectIdx = 0;
	m_bDragFlag			= false;
	m_SkillListCliper	= NULL;
	m_SkillList			= NULL;
	d_pSkillListScrol	= NULL;
	ZeroMemory( m_SkillKindArray, MAX_SKILL_COUNT*sizeof(int) );
	ZeroMemory( m_SkillArray, MAX_SKILL_COUNT*sizeof(int));
	ZeroMemory( m_SkillCanDrag,MAX_SKILL_COUNT*sizeof(bool));
	ZeroMemory( m_isSkillNumUpdatedForEachKinfSkill, MAX_SKILL_COUNT * sizeof( bool ) );
	PlayTime	 = 1000;
	m_isOpenAnimation = false;
}


KUiStudySkillManage::~KUiStudySkillManage()
{
}

void KUiStudySkillManage::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{

		ms_Singleton->d_pSkillListScrol = (CEGUI::TLVertScrollbar*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillList/SkillListScrollbar");
		d_pSkillListScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiStudySkillManage::handleTreeScroll, this));


		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/Close")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind0")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleSkillKind0, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind1")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleSkillKind1, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind2")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleSkillKind2, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind3")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleSkillKind3, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind4")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleSkillKind4, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind5")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleSkillKind5, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind0")->getChild("TaharezLook/SkillStudy/SkillKind0/Animation")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleStaticImageButton, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind1")->getChild("TaharezLook/SkillStudy/SkillKind1/Animation")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleStaticImageButton, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind2")->getChild("TaharezLook/SkillStudy/SkillKind2/Animation")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleStaticImageButton, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind3")->getChild("TaharezLook/SkillStudy/SkillKind3/Animation")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleStaticImageButton, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind4")->getChild("TaharezLook/SkillStudy/SkillKind4/Animation")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleStaticImageButton, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillKind5")->getChild("TaharezLook/SkillStudy/SkillKind5/Animation")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiStudySkillManage::handleStaticImageButton, ms_Singleton));
		m_SkillListCliper	= ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillListClipper");
		m_SkillList			= ms_Singleton->m_pThisWnd->getChild("TaharezLook/SkillStudy/SkillListClipper")->getChild("TaharezLook/SkillStudy/SkillList");

		m_SkillList->getChild("TaharezLook/SkillStudy/Skill0")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill1")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill2")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill3")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill4")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill5")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill6")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill7")->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));


		m_SkillList->getChild("TaharezLook/SkillStudy/Skill0")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill1")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill2")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill3")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill4")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill5")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill6")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill7")->subscribeEvent(PushButton::EventMouseButtonUp, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));
		ms_Singleton->m_pThisWnd->subscribeEvent(StaticImage::EventMouseLeaves, Event::Subscriber(&KUiStudySkillManage::ClearPickUpSkill, ms_Singleton));

		m_SkillList->getChild("TaharezLook/SkillStudy/Skill0")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill1")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill2")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill3")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill4")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill5")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill6")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill7")->subscribeEvent(PushButton::EventMouseMove, Event::Subscriber(&KUiStudySkillManage::SkillMouseMove, ms_Singleton));

		m_SkillList->getChild("TaharezLook/SkillStudy/Skill0")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill0, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill1")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill1, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill2")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill2, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill3")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill3, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill4")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill4, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill5")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill5, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill6")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill6, ms_Singleton));
		m_SkillList->getChild("TaharezLook/SkillStudy/Skill7")->subscribeEvent(PushButton::EventMouseButtonDown, Event::Subscriber(&KUiStudySkillManage::handleSkill7, ms_Singleton));

		for ( int nIdx = 0; nIdx < MAX_SKILL_COUNT; ++nIdx )
		{
			char szBuf[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szBuf, "TaharezLook/SkillStudy/Levelup%d", nIdx );
			ms_Singleton->m_LevelUpBtn[nIdx] = (TLButton*)m_SkillList->getChild(szBuf);
		}

		ms_Singleton->m_LevelUpBtn[0]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill0, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[1]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill1, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[2]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill2, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[3]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill3, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[4]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill4, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[5]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill5, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[6]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill6, ms_Singleton));
		ms_Singleton->m_LevelUpBtn[7]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiStudySkillManage::handleLevelupSkill7, ms_Singleton));
		PlayTime = KUiCfgLoader::getSingleton().getPlayTime();
		m_isOpenAnimation = KUiCfgLoader::getSingleton().getAnimationOpenStatus();
		m_uncheckItem  = KUiCfgLoader::getSingleton().getUnCheckSkill();
	}
}

bool KUiStudySkillManage::handleTreeScroll( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	float sparef = m_SkillList->getHeight(Absolute);
	float clipper = m_SkillListCliper->getHeight(Absolute);
	float yPos = 0;
	if (scrollCtrl->window == d_pSkillListScrol)
	{	
		float scrollPos = d_pSkillListScrol->getScrollPosition();

		if ( (sparef > clipper ) )
		{
			yPos = (sparef - clipper) * scrollPos;
			Point pos;
			pos.d_x = m_SkillList->getPosition(Absolute).d_x;
			pos.d_y = 0 - yPos;
			m_SkillList->setPosition( Absolute, pos );
		}
		else
		{
			m_SkillList->setPosition( Absolute, Point(0, 0));
		}
	}
	return true;
}

void KUiStudySkillManage::handleSkillSelect(const int nIdx)
{
	char szBuf[128];
	
    if (m_SkillArray[m_nCurrentSelectIdx]>0)
	{
		sprintf( szBuf, "TaharezLook/SkillStudy/Skill%d", m_nCurrentSelectIdx );
		TLGameObject* pGO = static_cast<TLGameObject*>(m_SkillList->getChild( szBuf ));
	}//endif
	
    m_nCurrentSelectIdx = nIdx;
	
    if (m_SkillArray[m_nCurrentSelectIdx]>0)
	{
		sprintf( szBuf, "TaharezLook/SkillStudy/Skill%d", m_nCurrentSelectIdx );
		TLGameObject* pGO = static_cast<TLGameObject*>(m_SkillList->getChild( szBuf ));
	}//endif
}

void KUiStudySkillManage::Show( bool bEnable )
{

	KUiWndSingleton<KUiStudySkillManage>::Show();

	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->ShowSkillKindList();
	}

}

void KUiStudySkillManage::Hide()
{
    if (KUiComMsgBox::GetSingleton().IsVisible())
	{
        KUiComMsgBox::GetSingleton().Hide();
	}//endif
	else 
		KUiWndSingleton<KUiStudySkillManage>::Hide();

}

unsigned KUiStudySkillManage::Updatedata( int nSkillKindID, int nSkillID )
{
	if(ms_Singleton == NULL || ms_Singleton->m_pThisWnd == NULL)
		return 0;
	
	ms_Singleton->ShowSkillKindList( false );
	
	if ( ms_Singleton->m_nCurrentSelSkillKind == nSkillKindID )
		ms_Singleton->ShowSkillList( nSkillKindID );

	return 0;
}

void KUiStudySkillManage::ShowSkillKindList( bool bShow/* = true */ )
{
	m_nSkillKindCount = g_pCoreShell->GetGameData( GDI_SKILL_KIND_LIST, (unsigned int)m_SkillKindArray, MAX_SKILL_COUNT );
	
	//Scan for unstudyable skills 
	int nIndex=0;
	while(nIndex<m_nSkillKindCount)
	{
        int nCanStudy=g_pCoreShell->GetGameData( GDI_GET_IS_STUDY_ABLE, m_SkillKindArray[nIndex], 0 );
		if (nCanStudy==0)
		{
			m_SkillKindArray[nIndex]=0;
			for (int i=nIndex;i<m_nSkillKindCount-1;i++)
			{
				m_SkillKindArray[i]=m_SkillKindArray[i+1];
				m_SkillKindArray[i+1]=0;
			}//end for i
            m_nSkillKindCount--;
		}//endif
		else
			nIndex++;
	}//end while
	
	char szBuf[COMMON_CLIENT_MSG_LEN_64];

	for ( int nIdx = 0; nIdx < m_nSkillKindCount; ++nIdx )
	{
		sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", nIdx );
		TLButton* pGO = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));
		pGO->show();
	
		if ( pGO )
		{
			// 先取前2个字做系名
			KSkillInfo tagSkillInfo;
			
			char name[5];
			g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, m_SkillKindArray[nIdx] );
			ZeroMemory(name, 5);
			strncpy(name, tagSkillInfo.szName, 4);
			pGO->setText( AnsiToUtf8( name ) );

		/*	char szBuf[COMMON_CLIENT_MSG_LEN_128];
			sprintf( szBuf, "%d/%d", tagSkillInfo.nSkillPoint, tagSkillInfo.nSkillMaxPoint);
			String str( AnsiToUtf8( szBuf ) );
			m_pThisWnd->getChild( "TaharezLook/SkillStudy/SprirtNum" )->setText( AnsiToUtf8( szBuf ) );
		*/
		}
	}
	
	for ( int nIdex = m_nSkillKindCount; nIdex < MAX_STUDY_KIND; ++nIdex )
	{
        sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", nIdex );
		TLButton* pGO = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));
		pGO->hide();
	}//end for nIdx

	if(nIdx)
		ShowSkillList( m_SkillKindArray[m_OldSkillKindIdx], bShow );	
	else
	{
		ClearSkillList();
		ShowSkillInfo(m_OldSkillKindIdx, false);
	//	m_pThisWnd->getChild( "TaharezLook/SkillStudy/SprirtNum" )->setText( AnsiToUtf8( "" ) );
	}
	if( !m_isOpenAnimation )
		return;
	for ( int i = 0; i < m_nSkillKindCount; ++i)
	{
		if( i == m_uncheckItem - 1 ) 
			continue;
		sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", i );
		TLButton* pGO = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));
		if( pGO )
		{
			bool isNew = ShowSkillList( m_SkillKindArray[i], true );
			BtnAnimationOperation( pGO, i, isNew );
		}
	}
}

void	KUiStudySkillManage::ClearSkillList( void )
{
	char szBuf[COMMON_CLIENT_MSG_LEN_64];	
	for ( int nIdx = 0; nIdx < MAX_SKILL_COUNT; ++nIdx )
	{
		sprintf( szBuf, "TaharezLook/SkillStudy/Skill%d", nIdx );
		TLGameObject* pGO = static_cast<TLGameObject*>(m_SkillList->getChild( szBuf ));
		if (pGO)
		{
			pGO->clear();
		}
	}
}

bool KUiStudySkillManage::ShowSkillList( int nSkillKind, bool bShow/* = true */ )
{
	if ( !bShow)
	{
		return false;
	}

	ClearSkillList();

	int nParam = 0;
	nParam |= MAX_SKILL_COUNT << 16;
	nParam |= nSkillKind;  
	m_nSkillCount = g_pCoreShell->GetGameData( GDI_SKILL_LIST, (unsigned int)m_SkillArray, nParam );
	char szBuf[COMMON_CLIENT_MSG_LEN_64];
	char szBufName[COMMON_CLIENT_MSG_LEN_64];
	char szBufLevel[COMMON_CLIENT_MSG_LEN_64];
	char szBufFrame[COMMON_CLIENT_MSG_LEN_64]; 
	String	str;

	for ( int i = 0; i < MAX_SKILL_COUNT; i++ )
	{
		// 先清空所有技能名称等级
		sprintf( szBufName, "TaharezLook/SkillStudy/SkillName%d", i );
		sprintf( szBufLevel, "TaharezLook/SkillStudy/SkillLevel%d", i );
		sprintf( szBufFrame, "TaharezLook/SkillStudy/Frame%d", i );
		m_SkillList->getChild( szBufName )->setText("");
		m_SkillList->getChild( szBufLevel )->setText("");
		if ( m_LevelUpBtn[i] )
		{
			m_LevelUpBtn[i]->hide();
		}
		
		
		//m_pThisWnd->getChild( szBufName )->setTooltipText("");
	}
	bool isSkillUpdate = false;
	for ( int nIdx = 0; nIdx < m_nSkillCount; ++nIdx )
	{
		// 显示此系所有技能
		sprintf( szBuf, "TaharezLook/SkillStudy/Skill%d", nIdx );
		sprintf( szBufName, "TaharezLook/SkillStudy/SkillName%d", nIdx );
		sprintf( szBufLevel, "TaharezLook/SkillStudy/SkillLevel%d", nIdx );
		TLGameObject* pGO = static_cast<TLGameObject*>(m_SkillList->getChild( szBuf ));
		char szWinName[64]="";
	//	sprintf(szWinName,"TaharezLook/SkillStudy/Skill%dbg",nIdx);
	//	m_pThisWnd->getChild(szWinName)->show();
		
		if ( pGO )
		{
			pGO->unlock();
			TLGameObject::GameObject tagGO;
			
			KSkillInfo tagSkillInfo;
			tagSkillInfo.bDescAvailable = true;
            tagSkillInfo.bNextDescAvailable=true;
			tagSkillInfo.dwNextDescStyle=0;
			
			char szStudyTip[MAX_STUDY_TIP_SIZE];
			tagSkillInfo.nStudyTipBuffSize=MAX_STUDY_TIP_SIZE;
			tagSkillInfo.szStudyTipBuff=(char *)szStudyTip;

			g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, m_SkillArray[nIdx] );
			tagGO.d_gameobject = AnsiToUtf8( tagSkillInfo.szIconName );
			tagGO.d_type = TLGameObject::skill;
			tagGO.d_coolingTime = tagSkillInfo.nCoolingTime / 1000;
			tagGO.d_passivity = tagSkillInfo.bPassivity;
			tagGO.d_skillID = m_SkillArray[nIdx];
			m_SkillCanDrag[nIdx]=tagSkillInfo.bCanHumanUse;
			
			pGO->setObject( tagGO );
            pGO->setTooltipText( AnsiToUtf8( szStudyTip ));

			m_SkillList->getChild( szBufName )->setText( AnsiToUtf8( tagSkillInfo.szName ) );
			
			//if (tagSkillInfo.pSkillCond!=NULL)  //Max Level
			{
				str = AnsiToUtf8( KMessageCentre::GetMessage(29,3) );
				m_SkillList->getChild( szBufLevel )->setText( str += AnsiToUtf8( itoa(tagSkillInfo.nLevel, szBuf, 10) ) );
            }//endif
			/*
			else
			{
                str = AnsiToUtf8( KMessageCentre::GetMessage(29,4)  );
				m_pThisWnd->getChild( szBufLevel )->setText( str );
			}//*/

			KUiPlayerAttribute playerInfo;
			g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE,(unsigned int)&playerInfo,0);

			if (  tagSkillInfo.nLevel == 0 )
			{
				pGO->disable( true );
			}
			else
			{
				pGO->disable( false );
			}		

			if ( tagSkillInfo.pSkillCond && playerInfo.nLevel>=tagSkillInfo.pSkillCond->nPlayerLvl )
			{
				if ( m_LevelUpBtn[nIdx] )
				{
					m_LevelUpBtn[nIdx]->show();
					m_LevelUpBtn[nIdx]->setEnabled( true );
					sprintf( szBufFrame, "TaharezLook/SkillStudy/Frame%d", nIdx );
					m_SkillList->getChild( szBufFrame )->show();
					isSkillUpdate = true;
				
				}

			}
			else
			{
				if ( m_LevelUpBtn[nIdx] )
				{
					m_LevelUpBtn[nIdx]->hide();
					m_LevelUpBtn[nIdx]->setEnabled( false );
					sprintf( szBufFrame, "TaharezLook/SkillStudy/Frame%d", nIdx );
					//m_SkillList->getChild( szBufFrame )->hide();
				}			
			}
		}
	}

	ShowSkillInfo( nSkillKind, bShow );
	return isSkillUpdate;
}

void KUiStudySkillManage::ShowSkillInfo( int nSkillID, bool bShow/* = true */ )
{
	m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" )->setText("");

	if ( !bShow )
	{
		return;
	}//endif
	
	
	KSkillInfo tagSkillInfo;
	tagSkillInfo.bDescAvailable=true;  //标志Desc有效 程序自动生成排版
	tagSkillInfo.dwDescStyle=0;
	g_pCoreShell->GetGameData( GDI_SKILL_INFO, (unsigned int)&tagSkillInfo, nSkillID );
	
	if ( !strcmp (tagSkillInfo.szName, "") )
		return;
	
	static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))->useLayout();
	static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))
		->getLayout()->SetText( tagSkillInfo.szDesc );
	static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))
		->setLayoutOffset(static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))->getLeftFrameWidth(),
	static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))->getTopFrameHeight());
	return;
	
}

bool	KUiStudySkillManage::handleSkillKind0( const CEGUI::EventArgs& args )
{	

	if (m_SkillKindArray[0]>0 && !KUiComMsgBox::IsVisible())
	{
		SelectSkillKind(0);
		m_nCurrentSelSkillKind = m_SkillKindArray[0];
		ShowSkillList( m_nCurrentSelSkillKind );
		WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
		if( eventArgs && eventArgs->window )
			BtnAnimationOperation( eventArgs->window, 0, false );
	}
	return 0;
}

bool	KUiStudySkillManage::handleSkillKind1( const CEGUI::EventArgs& args )
{
	if (m_SkillKindArray[1]>0 && !KUiComMsgBox::IsVisible())
	{
		SelectSkillKind(1);
		
		m_nCurrentSelSkillKind = m_SkillKindArray[1];
		ShowSkillList( m_nCurrentSelSkillKind );
		WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
		if( eventArgs && eventArgs->window )
			BtnAnimationOperation( eventArgs->window, 1, false );
	}
	return 0;
}


bool	KUiStudySkillManage::handleSkillKind2( const CEGUI::EventArgs& args )
{
	if (m_SkillKindArray[2]>0 && !KUiComMsgBox::IsVisible())
	{
		
		SelectSkillKind(2);
		
		m_nCurrentSelSkillKind = m_SkillKindArray[2];
		ShowSkillList( m_nCurrentSelSkillKind );
		WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
		if( eventArgs && eventArgs->window )
			BtnAnimationOperation( eventArgs->window, 2, false );
	}
	return 0;
}

bool	KUiStudySkillManage::handleSkillKind3( const CEGUI::EventArgs& args )
{
	
	if (m_SkillKindArray[3]>0 && !KUiComMsgBox::IsVisible())
	{
		SelectSkillKind(3);
		
		m_nCurrentSelSkillKind = m_SkillKindArray[3];
		ShowSkillList( m_nCurrentSelSkillKind );
		WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
		if( eventArgs && eventArgs->window )
			BtnAnimationOperation( eventArgs->window, 3, false );
	}
	return 0;
}

bool	KUiStudySkillManage::handleSkillKind4( const CEGUI::EventArgs& args )
{
	if (m_SkillKindArray[4]>0 && !KUiComMsgBox::IsVisible())
	{
		SelectSkillKind(4);
		
		m_nCurrentSelSkillKind = m_SkillKindArray[4];
		ShowSkillList( m_nCurrentSelSkillKind );
		WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
		if( eventArgs && eventArgs->window )
			BtnAnimationOperation( eventArgs->window, 4, false );
	}//endif

	return 0;
}

bool	KUiStudySkillManage::handleSkillKind5( const CEGUI::EventArgs& args )
{
	if (m_SkillKindArray[5]>0 && !KUiComMsgBox::IsVisible())
	{
		SelectSkillKind(5);
		
		m_nCurrentSelSkillKind = m_SkillKindArray[5];
		ShowSkillList( m_nCurrentSelSkillKind );
		WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
		if( eventArgs && eventArgs->window )
			BtnAnimationOperation( eventArgs->window, 5, false );
	}//endif

	return 0;
}

bool	KUiStudySkillManage::handleLevelupSkill0( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[0] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(0);
		handleOK(args);
	}	
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill1( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[1] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(1);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill2( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[2] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(2);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill3( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[3] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(3);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill4( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[4] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(4);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill5( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[5] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(5);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill6( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[6] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(6);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleLevelupSkill7( const CEGUI::EventArgs& args )
{
	if ( m_SkillArray[7] > 0  /*&& !KUiComMsgBox::IsVisible()*/)
	{
	    handleSkillSelect(7);
		handleOK(args);
	}
	return true;
}

bool	KUiStudySkillManage::handleSkill0( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[0])
		m_bDragFlag = true;
	return true;
}

bool	KUiStudySkillManage::handleSkill1( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[1])
		m_bDragFlag = true;	
	return true;
}

bool	KUiStudySkillManage::handleSkill2( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[2])
		m_bDragFlag = true;		
	return true;
}

bool	KUiStudySkillManage::handleSkill3( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[3])
		m_bDragFlag = true;		
	return true;
}

bool	KUiStudySkillManage::handleSkill4( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[4])
		m_bDragFlag = true;		
	return true;
}

bool	KUiStudySkillManage::handleSkill5( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[5])
		m_bDragFlag = true;		
	return true;
}

bool	KUiStudySkillManage::handleSkill6( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[6])
		m_bDragFlag = true;		
	return true;
}

bool	KUiStudySkillManage::handleSkill7( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pEvent = (MouseEventArgs*)&args;
	if ( pEvent && pEvent->button == RightButton )
	{
		if (m_SkillCanDrag[0])
			m_bDragFlag = false;
		return true;
	}
	if (m_SkillCanDrag[7])
		m_bDragFlag = true;		
	return true;
}

#define MAX_LEN_COND_MSG 1024
bool KUiStudySkillManage::handleOK( const CEGUI::EventArgs& args )
{
	if (m_SkillArray[m_nCurrentSelectIdx]>0 /*&&!KUiComMsgBox::IsVisible()*/)
	{
		char          szCondMsg[MAX_LEN_COND_MSG];
		int uRet      = g_pCoreShell->GetGameData(GDI_STUDY_SKILL_COND,m_SkillArray[m_nCurrentSelectIdx],(int)szCondMsg);
		KUiComMsgBox::GetSingleton().setLayoutMsg(szCondMsg);
		KUiComMsgBox::GetSingleton().setModalStatus(true);
		KUiComMsgBox::GetSingleton().Show();
		
		if (uRet==1) //Can update
		{
             KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(KMessageCentre::GetMessage(29,1) ),AnsiToUtf8(KMessageCentre::GetMessage(29,2)));
			 KUiComMsgBox::GetSingleton().setFristBtnCallback(ComMsgBoxHandler);
		}
		else
		{
			 KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(KMessageCentre::GetMessage(29,2)));
		}

    }//endif

	return true;
}

#define SKILL_EFFECT_DX 10
#define SKILL_EFFECT_DY 10

void ComMsgBoxHandler()
{
	g_pCoreShell->OperationRequest( GOI_LEVELUP_SKILL, KUiStudySkillManage::GetSingleton().m_SkillArray[KUiStudySkillManage::GetSingleton().m_nCurrentSelectIdx], NULL );
    ((TLStaticImage *) KUiStudySkillManage::GetSingleton().m_pThisWnd)->Shake(false,true,6);
	KUiComMsgBox::Hide();
	int Param[2];
	char          szTableName[32];
	sprintf(szTableName,"TaharezLook/SkillStudy/Skill%d",KUiStudySkillManage::GetSingleton().m_nCurrentSelectIdx);
    Point ParentPos=KUiStudySkillManage::GetSingleton().m_pThisWnd->getPosition(Absolute);
	Point ChildPos=KUiStudySkillManage::GetSingleton().m_SkillList->getChild(szTableName)->getPosition(Absolute);
    
	Param[0]=ChildPos.d_x+ParentPos.d_x +
		KUiStudySkillManage::GetSingleton().getSkillListCliper()->getPosition(Absolute).d_x + 
		KUiStudySkillManage::GetSingleton().getSkillList()->getPosition(Absolute).d_x + SKILL_EFFECT_DX;
	Param[1]=ChildPos.d_y+ParentPos.d_y +
				KUiStudySkillManage::GetSingleton().getSkillListCliper()->getPosition(Absolute).d_y + 
		KUiStudySkillManage::GetSingleton().getSkillList()->getPosition(Absolute).d_y + 
		SKILL_EFFECT_DY;
	g_pCoreShell->OperationRequest(GOI_SET_EFFECT_POS,4,(int)Param);
	g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT,4,0);
}

bool KUiStudySkillManage::handleExit( const CEGUI::EventArgs& args )
{
	//if (!KUiComMsgBox::IsVisible())
	//{
		Hide();
	//}//endif
    return true;
}

void KUiStudySkillManage::SelectSkillKind( int SkillKindIdx )
{
	char szBuf[COMMON_CLIENT_MSG_LEN_64];

	sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", m_OldSkillKindIdx );
	TLButton* OldGO = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));
	
	sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", SkillKindIdx );
	TLButton* pGO = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));
	pGO->setNormalImage( OldGO->getNormalImage() );

	for ( int nIdx = 0; nIdx < m_nSkillKindCount; ++nIdx )
	{
		sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", nIdx );
		pGO = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));

		if ( nIdx != SkillKindIdx )
			pGO->setNormalImage(pGO->getDisabledImage());
	}

	m_OldSkillKindIdx = SkillKindIdx;
}


void KUiStudySkillManage::clearAll()
{
	char szBuf[COMMON_CLIENT_MSG_LEN_64];
	char szBufLevel[COMMON_CLIENT_MSG_LEN_64];

	for ( int nIdx = 0; nIdx < m_nSkillKindCount; ++nIdx )
	{
		char szBuf[COMMON_CLIENT_MSG_LEN_64];
		ZeroMemory(szBuf, COMMON_CLIENT_MSG_LEN_64);
		sprintf( szBuf, "TaharezLook/SkillStudy/SkillKind%d", nIdx );
		TLButton* pBtn = static_cast<TLButton*>(m_pThisWnd->getChild( szBuf ));
		if (pBtn)
			pBtn->setText("");
	}
	
	TLGameObject* pGO = static_cast<TLGameObject*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkill" ));
	if (pGO)
		pGO->clear();
	
	for ( int i = 0; i < MAX_SKILL_COUNT; i++ )
	{
		// 清空所有技能名称等级
		ZeroMemory(szBuf, COMMON_CLIENT_MSG_LEN_64);
		ZeroMemory(szBufLevel, COMMON_CLIENT_MSG_LEN_64);
		sprintf( szBuf, "TaharezLook/SkillStudy/SkillName%d", i );
		sprintf( szBufLevel, "TaharezLook/SkillStudy/SkillLevel%d", i );
		m_pThisWnd->getChild( szBuf )->setText("");
		m_pThisWnd->getChild( szBufLevel )->setText("");
	}

	m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillName" )->setText("");
	m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillNextLV" )->setText("");
	m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillWaste" )->setText("");
	static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))->useLayout();
	static_cast<TLStaticText*>(m_pThisWnd->getChild( "TaharezLook/SkillStudy/SelectSkillIntro" ))->getLayout()->SetText("");

	ClearSkillList();
}

bool KUiStudySkillManage::ClearPickUpSkill( const CEGUI::EventArgs& args )
{
	if ( m_bDragFlag )
	{
		m_bDragFlag = false;
	}

	return true;
}

bool KUiStudySkillManage::SkillMouseMove( const CEGUI::EventArgs& args )
{
	if ( m_bDragFlag == false )
	{
		return false;
	}

	MouseEventArgs* arg = (MouseEventArgs*)(&args);
	TLGameObject* destObj,* sourObj;
	TLGameObject::GameObject destObjInfo, sourObjInfo;
	destObj = (TLGameObject*)arg->window;
	destObj->getObject(destObjInfo);
	if ( destObj->isEmpty() || destObj->isPassivity() )
	{
		return false;
	}
	sourObj = KUiDragItem::GetSingleton().getObj();
	if ( !sourObj->isEmpty() )
	{
		return false;
	}
	sourObj->getObject(sourObjInfo);
	sourObj->clear();
	if(sourObjInfo.d_type == TLGameObject::idle)
	{
		if(destObjInfo.d_type == TLGameObject::skill)
		{
			sourObj->setObject(destObjInfo);
			sourObj->setCanDrag(true);
		}
	}//*/
	return true;
}
void KUiStudySkillManage::BtnAnimationOperation( TLButton* button, int buttonNUm, bool isPlay /*= true*/ )
{
	if( !button )
		return ;
	TLStaticImage * animation;
	char animationName[COMMON_CLIENT_MSG_LEN_64];
	sprintf( animationName, "TaharezLook/SkillStudy/SkillKind%d/Animation", buttonNUm );
#ifndef _DEBUG
	try
	{
#endif
		animation = static_cast<TLStaticImage*> ( button->getChild( animationName ) );
#ifndef _DEBUG
	}
	catch ( ... )
	{
		return ;
	}
#endif
	if( isPlay )
	{
		animation->show();
		animation->setCycCount( PlayTime );
		animation->play();
	}
	else
	{
		animation->stop();
		animation->hide();
	}
	if( buttonNUm >= 0 && buttonNUm < MAX_SKILL_COUNT )
		m_isSkillNumUpdatedForEachKinfSkill[buttonNUm] = false;
}
bool KUiStudySkillManage::handleStaticImageButton( const CEGUI::EventArgs& args )
{
	WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
	if( !eventArgs )
		return false;
	TLStaticImage* image  = ( TLStaticImage* )eventArgs->window;
	if( !image )
		return false;
	WindowEventArgs eventObj( *eventArgs );

	eventObj.window = image->getParent();
	image->getParent()->fireEvent( RadioButton::EventMouseClick, eventObj );
	return true;
}
