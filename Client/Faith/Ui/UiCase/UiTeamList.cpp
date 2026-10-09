//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/18/2006 10:20
//      File_base        : UiTeamList
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"

#include "Ui/UiCase/UiTeamList.h"
#include "Coreshell.h"
#include "UiChatWindow.h"
#include "UiRaid.h"
#include "../KMessageCentre.h"
#include "Ui/UiCase/UiItemTip.h"
#include "Ui/UiCase/UiDragItem.h"
#include "Ui/UiCase/UiTradeBox.h"

extern iCoreShell*		g_pCoreShell;

bool KUiTeamList::m_bCanShow = true;

using namespace CEGUI;

#define DEFUALT_SPACE 10
#define MSG_TEAMMATE_NOT_NEARBY 6

template<> 
KUiTeamList* KUiWndSingleton<KUiTeamList>::ms_Singleton	= NULL;

KUiTeamList::KUiTeamList( const CEGUI::String& id_name ):
KUiWndSingleton<KUiTeamList>( id_name ),m_bHideDisapear(true)
{
	m_pRolePopmenu = NULL;
	m_FrontWidth   = 0.0f;
	m_nMemberNo    = 0;
}

KUiTeamList::~KUiTeamList()
{

}

void KUiTeamList::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setZLevel(Window::SuperBottom);
		Size  absSize;
		Point absPoint;
		for ( int nIdx = 0; nIdx < MAX_TEAMMEMBER_COUNT; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_32];
			sprintf( szName, "%d_", nIdx );
			Window* pTeamMember = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_TEAMMEMBER, szName, "", NULL, NULL, true  );
			if ( pTeamMember )
			{
				absPoint.d_x = 0;
				absPoint.d_y = 0;
				//absPoint.d_y = 20;
				absSize	= pTeamMember->getAbsoluteSize(); 
				ms_Singleton->m_pThisWnd->addChildWindow( pTeamMember );
				absPoint.d_y = absPoint.d_y + nIdx * (absSize.d_height + DEFUALT_SPACE);
				pTeamMember->setPosition( Absolute, absPoint );
				String str(pTeamMember->getName());
				
				pTeamMember->subscribeEvent(StaticImage::EventMouseButtonDown, Event::Subscriber(& KUiTeamList::MouseButtonDown, ms_Singleton));
					

				float width        = pTeamMember->getChild(str + "/SmallPanel")->getWidth(Absolute);
				Point smallInfoPos ;
				smallInfoPos.d_x   = -width -10.0f;
				smallInfoPos.d_y   = 0;

				TLStaticImage * pSmallPanel = (TLStaticImage *)pTeamMember->getChild(str+"/SmallPanel");
				pSmallPanel->setPosition(Absolute,smallInfoPos);
				pSmallPanel->enable();
			//	pSmallPanel->subscribeEvent(StaticImage::EventMouseEnters, Event::Subscriber(& KUiTeamList::MouseButtonDown, ms_Singleton));

                m_FrontWidth = pTeamMember->getChild(str+"/TeamMemberFront")->getWidth(Absolute);
			}//endif
		}
		/*Window *pRoleFace = NULL;
		try
		{
			pRoleFace = ms_Singleton->m_pWindowManager->getWindow( "TaharezLook/RoleFace" );
		}
		catch (...)
		{
			pRoleFace = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_ROLEFACE );	
		}
		
		if ( pRoleFace )
		{
			Size sizeRole;
			sizeRole = pRoleFace->getSize( Absolute );
			absPoint.d_x = 0;
			absPoint.d_y = sizeRole.d_height + DEFUALT_SPACE;
		}//*/
		absSize.d_height = MAX_TEAMMEMBER_COUNT * (absSize.d_height + DEFUALT_SPACE);
		ms_Singleton->m_pThisWnd->setSize( Absolute, absSize );
		//ms_Singleton->m_pThisWnd->setPosition( Absolute, absPoint );

		ms_Singleton->m_pRolePopmenu = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_TEAMMEMBERPOPMENU, "", "", NULL, NULL, true  );
		if ( ms_Singleton->m_pRolePopmenu )
		{
			ms_Singleton->m_pRootSheet->addChildWindow( ms_Singleton->m_pRolePopmenu );
			ms_Singleton->m_pChat		= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/Chat");
			ms_Singleton->m_pCaption	= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/Captain");
			ms_Singleton->m_pKick	= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/Kick");
			ms_Singleton->m_pLeave	= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/Leave");
			//ms_Singleton->m_pExp		= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/Exp");
			ms_Singleton->m_pFriend	= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/Friend");
//			ms_Singleton->m_pOpenBT	= (PushButton*)ms_Singleton->m_pRolePopmenu->getChild("TaharezLook/TeamMemberPopMenu/OpenBT");

			ms_Singleton->m_pChat->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::Chat, ms_Singleton));
			ms_Singleton->m_pCaption->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::Captain, ms_Singleton));
			ms_Singleton->m_pKick->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::Kick, ms_Singleton));
			ms_Singleton->m_pLeave->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::Leave, ms_Singleton));
			//ms_Singleton->m_pExp->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::Exp, ms_Singleton));
			ms_Singleton->m_pFriend->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::Friend, ms_Singleton));
//			ms_Singleton->m_pOpenBT->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiTeamList::OpenBT, ms_Singleton));
			ms_Singleton->m_pRolePopmenu->hide();
		}
		ms_Singleton->m_nCurMember	= 0;
		ms_Singleton->m_bListShow		= false;

		ms_Singleton->UpdateData();
	}
}

void KUiTeamList::Show( void )
{
	if (m_bCanShow)
	{
		if ( ms_Singleton && ms_Singleton->m_pThisWnd)
		{
			Point pCurrent=ms_Singleton->m_pThisWnd->getPosition(Absolute);

			if (!ms_Singleton->m_pThisWnd->isVisible() || pCurrent.d_x<0 )
			{
				KUiWndSingleton<KUiTeamList>::Show();
				
				if (pCurrent.d_x<-ms_Singleton->m_pThisWnd->getWidth(Absolute))
				{
					pCurrent.d_x = -ms_Singleton->m_pThisWnd->getWidth(Absolute);
					ms_Singleton->m_pThisWnd->setPosition(Absolute,pCurrent);
				}//endif
				
				((TLStaticImage *)ms_Singleton->m_pThisWnd)->MoveTo(0,pCurrent.d_y,20,0,true);
				ms_Singleton->m_pRolePopmenu->hide();
			}//endif
		}//endif
        else
		{
		    KUiWndSingleton<KUiTeamList>::Show();
			if ( ms_Singleton && ms_Singleton->m_pThisWnd)
			{
			  Point pCur=ms_Singleton->m_pThisWnd->getPosition(Absolute);
			  pCur.d_x=-ms_Singleton->m_pThisWnd->getWidth(Absolute);

			  ms_Singleton->m_pThisWnd->setPosition(Absolute,pCur);
              ((TLStaticImage *)ms_Singleton->m_pThisWnd)->MoveTo(0,pCur.d_y,20,0,true);
			  ms_Singleton->m_pRolePopmenu->hide();
			}//endif

		}//endif

		for ( int nIdx = 0; nIdx < ms_Singleton->m_nMemberNo; ++nIdx )
		{
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/TeamMember", nIdx );
			String str(szName);
			
			float fSmallWidth     = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getWidth(Absolute);
			Point SmallInfoPos    = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getPosition(Absolute);
			Point newSmallInfoPos = SmallInfoPos;
			newSmallInfoPos.d_x   = - fSmallWidth;
			((TLStaticImage *)ms_Singleton->m_pThisWnd->getChild(str)->getChild(str+ "/SmallPanel"))->MoveTo(newSmallInfoPos.d_x,newSmallInfoPos.d_y,-32,0,true);
	   }//end for nIdx

	}//endif

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		    ms_Singleton->m_bHideDisapear = false;
			ms_Singleton->UpdateData();
	//		ms_Singleton->m_pRolePopmenu->hide();
	}
}

void KUiTeamList::Hide( void )
{
	//KUiWndSingleton<KUiTeamList>::Hide();
	
	if ( ms_Singleton && ms_Singleton->m_pThisWnd)
	{
	   int   iHideX=(int)(-ms_Singleton->m_FrontWidth);	

	   if (ms_Singleton->m_bHideDisapear)
	   {
           iHideX=(-(int)ms_Singleton->m_pThisWnd->getWidth(Absolute));	
		   ms_Singleton->m_bHideDisapear = false;
	   }//endif

       Point pCurrent=ms_Singleton->m_pThisWnd->getPosition(Absolute);
	   ((TLStaticImage *)ms_Singleton->m_pThisWnd)->MoveTo(iHideX,pCurrent.d_y,-20,0,true);
	   
	   for ( int nIdx = 0; nIdx < ms_Singleton->m_nMemberNo; ++nIdx )
	   {
		   char szName[COMMON_CLIENT_MSG_LEN_64];
		   sprintf( szName, "%d_TaharezLook/TeamMember", nIdx );
		   String str(szName);

		   float fFrontWidth     = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/TeamMemberFront")->getWidth(Absolute);
           Point SmallInfoPos    = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getPosition(Absolute);
		   Point newSmallInfoPos = SmallInfoPos;
		   newSmallInfoPos.d_x   = fFrontWidth;
		   ((TLStaticImage *)ms_Singleton->m_pThisWnd->getChild(str)->getChild(str+ "/SmallPanel"))->MoveTo(newSmallInfoPos.d_x,newSmallInfoPos.d_y,16,0,true);
	   }//end for nIdx

	}//endif

	if ( ms_Singleton && ms_Singleton->m_pRolePopmenu )
	{
		ms_Singleton->m_pRolePopmenu->hide();
	}//endif
	
}

void KUiTeamList::SetDisapearFlag()
{
	if (ms_Singleton)
	{
      ms_Singleton->m_bHideDisapear = true;
	}
}

unsigned int KUiTeamList::UpdateData( void )
{
		
	static KUiTeamMemberItem	lastList[MAX_TEAMMEMBER_COUNT];

	int nMemberWndIdx = 0;

	KUiPlayerTeam teamInfo;
	memset(&teamInfo, 0, sizeof(KUiPlayerTeam));
	memcpy(&lastList,ms_Singleton->m_pMembersList,MAX_TEAMMEMBER_COUNT*sizeof(KUiTeamMemberItem));
	memset(ms_Singleton->m_pMembersList, 0, MAX_TEAMMEMBER_COUNT*sizeof(KUiTeamMemberItem));

	int nRet= g_pCoreShell->TeamOperation( TEAM_OI_GD_INFO, (unsigned int)&teamInfo, 0);

	KUiTeamHideShow::GetSingleton().SetOpenTGBBtnState( teamInfo.bIsBigTeam  );
	KUiTeamHideShow::GetSingleton().SetTGBBtnState( teamInfo.bCanOpenBigTeam );

	if (nRet!=0 && teamInfo.cNumMember > 0 ) 
	{
		ms_Singleton->m_nMemberNo = g_pCoreShell->TeamOperation( TEAM_OI_MEMBER_INFO, (unsigned int)ms_Singleton->m_pMembersList, 0);

		if ( ms_Singleton->m_nMemberNo > 0 )
		{
			ms_Singleton->m_bListShow = true;
			for ( int nIdx = 0; nIdx < ms_Singleton->m_nMemberNo; ++nIdx )
			{
				if ( ms_Singleton->m_pMembersList[nIdx].m_LifePercent >= 0 )
				{
					char szName[COMMON_CLIENT_MSG_LEN_64];
					sprintf( szName, "%d_TaharezLook/TeamMember", nMemberWndIdx );
					String str(szName);
					Window* pTeamMember = ms_Singleton->m_pThisWnd->getChild( szName );
					if ( pTeamMember )
					{
						KUiTeamMemberItem& teamMemberInfo = ms_Singleton->m_pMembersList[nIdx];

//TODO 暂时停用，需要相应的边框资源
// 						//根据队友是否在附近显示不同的边框
// 						if (ms_Singleton->m_pMembersList[nIdx].m_bIsNearBy)
// 							static_cast<StaticImage*>(pTeamMember)->setImage( "frame", "NearBy" );
// 						else
// 							static_cast<StaticImage*>(pTeamMember)->setImage( "frame", "FarAway" );

						//BUFF						
						/*char teamateBuffTooltip[COMMON_CLIENT_MSG_LEN_128 * MAX_SYNC_TEAMATE_BUFF_COUNT] = { 0 };
						strcat(teamateBuffTooltip, "<Layout margin-top=5 margin-left=5 margin-right=5 margin-bottom=5 width=100><Seg text-align=center float=wrap>");
						for (int buffLoopCount = 0; buffLoopCount < teamMemberInfo.m_nBuffCount; buffLoopCount++)
						{
							unsigned int teammateBuffTemplateId = teamMemberInfo.m_BuffTemplateId[buffLoopCount];
							KBufferInfo buffInfo;
							g_pCoreShell->GetGameData(GDI_GET_BUFFER_INFO, (unsigned int)&buffInfo, teammateBuffTemplateId);
							char buffImage[128] = { 0 };
							sprintf(buffImage, "<Obj type=pic>set:MiddleGameObject image:%s_normal</Obj><Obj> </Obj>", buffInfo.szImage, buffInfo.nDesc);
							strcat(teamateBuffTooltip, buffImage);
						}
						strcat(teamateBuffTooltip, "</Seg></Layout>");*/
						/*if (teamMemberInfo.m_nBuffCount > 0)
							pTeamMember->setTooltipText(AnsiToUtf8(teamateBuffTooltip));
						else
							pTeamMember->setTooltipText("");//*/

						//职业
						StaticImage* pProfessionImage = static_cast<StaticImage*>(pTeamMember->getChild(str+"/SmallPanel")->getChild(str + "/SmallPanel"+"/Profession"));
						if (pProfessionImage != NULL)
						{
							char professionImageSet[COMMON_CLIENT_MSG_LEN_32] = { 0 };
							char professionImageName[COMMON_CLIENT_MSG_LEN_32] = { 0 };
							char professionName[COMMON_CLIENT_MSG_LEN_32] = { 0 };
							char professionColor[COMMON_CLIENT_MSG_LEN_32] = { 0 };
							KUiCfgLoader::getSingleton().getProfessionCfg().GetImage(teamMemberInfo.m_nSeries, teamMemberInfo.m_nSkillSeries, professionImageSet, COMMON_CLIENT_MSG_LEN_32, professionImageName, COMMON_CLIENT_MSG_LEN_32);
							KUiCfgLoader::getSingleton().getProfessionCfg().GetName(teamMemberInfo.m_nSeries, teamMemberInfo.m_nSkillSeries, professionName, COMMON_CLIENT_MSG_LEN_32);
							KUiCfgLoader::getSingleton().getProfessionCfg().GetColor(teamMemberInfo.m_nSeries, teamMemberInfo.m_nSkillSeries, professionColor, COMMON_CLIENT_MSG_LEN_32);	
							pProfessionImage->setImage( AnsiToUtf8( professionImageSet ), AnsiToUtf8( professionImageName ) );
						}

						pTeamMember->getChild(str + "/Name")->setText( AnsiToUtf8( ms_Singleton->m_pMembersList[nIdx].m_szName ) );
				 	    char szBuf[COMMON_CLIENT_MSG_LEN_64];
						//sprintf( szBuf, "%d/%d", ms_Singleton->m_pMembersList[nIdx].m_nCurLife, ms_Singleton->m_pMembersList[nIdx].m_nMaxLife );
						//pTeamMember->getChild(str + "/TeamMemberImage")->getChild(str + "/TeamMemberImage/RoleBlood")->setText( AnsiToUtf8( szBuf ) );
						pTeamMember->getChild(str + "/TeamMemberImage")->getChild(str + "/TeamMemberImage/RoleBlood")->hide();
						//sprintf( szBuf, "%d/%d", ms_Singleton->m_pMembersList[nIdx].m_nCurMana, ms_Singleton->m_pMembersList[nIdx].m_nMaxMana );
						//pTeamMember->getChild(str + "/TeamMemberImage")->getChild(str + "/TeamMemberImage/RoleMagic")->setText( AnsiToUtf8( szBuf ) );
						pTeamMember->getChild(str + "/TeamMemberImage")->getChild(str + "/TeamMemberImage/RoleMagic")->hide();
						
						sprintf( szBuf, "%d", ms_Singleton->m_pMembersList[nIdx].m_nLevel);
					    pTeamMember->getChild(str + "/TeamMemberFront")->getChild(str + "/TeamMemberFront/RoleLevel")->setText( AnsiToUtf8( szBuf ) );	

						Window* pLeader = pTeamMember->getChild(str + "/TeamMemberImage")->getChild(str+"/TeamMemberImage/Leader");
						if ( pLeader )
						{
							if ( ms_Singleton->m_pMembersList[nIdx].m_bLeader )
							{
								if (!pLeader->isVisible())
									pLeader->show();
							}
							else
							{
								pLeader->hide();
							}
						}

						ProgressBar *pTargetBloodBar = (ProgressBar*)pTeamMember->getChild( str + "/BloodProgBar" );
						ProgressBar *pTargetMagicBar = (ProgressBar*)pTeamMember->getChild( str + "/MagicProgBar" );
						ProgressBar *pSmallBloodBar  = (ProgressBar*)pTeamMember->getChild(str + "/SmallPanel")->getChild( str +"/SmallPanel"+ "/SmallBloodProgBar" );

				    	if ( pTargetBloodBar && pTargetMagicBar && pSmallBloodBar)
						{
							char detailNumberTooltip[COMMON_CLIENT_MSG_LEN_256] = { 0 };
							float fCurentLife  = pTargetBloodBar->getProgress();
							float fCurentMana  = pTargetMagicBar->getProgress();

							float lifePercent = (float)teamMemberInfo.m_LifePercent / 100;
							float manaPercent = (float)teamMemberInfo.m_ManaPercent / 100;

							if (fCurentLife != lifePercent)
							{	
								pTargetBloodBar->setProgress(lifePercent);
								pSmallBloodBar->setProgress(lifePercent);
							}//endif
							
							if (fCurentMana != manaPercent)
							{	
								pTargetMagicBar->setProgress(manaPercent);
							}
						}//endif
					
						
						TLStaticImage * pSmallPanel = ((TLStaticImage *)ms_Singleton->m_pThisWnd->getChild(str)->getChild(str+ "/SmallPanel"));
						if (pSmallPanel && 
							( lastList[nIdx].m_nLevel != teamMemberInfo.m_nLevel || lastList[nIdx].m_uId != teamMemberInfo.m_uId || lastList[nIdx].m_nSkillSeries != teamMemberInfo.m_nSkillSeries || lastList[nIdx].m_nPortrait != teamMemberInfo.m_nPortrait )
						)
						{
							char szToolTip[512];
							szToolTip[0] = 0;

							//Begin...
							char * szBegin = KMessageCentre::GetMessage(41,0);
							if (szBegin)
							{
								strcat(szToolTip,szBegin);
							}//endif
							
							//Name
							char * szNameTemplate = KMessageCentre::GetMessage(41,1);
							if (szNameTemplate)
							{
								char szName[128];
								szName[0] = 0;
								sprintf(szName,szNameTemplate,ms_Singleton->m_pMembersList[nIdx].m_szName);
								strcat(szToolTip,szName);
							}//endif
							
							//Profession
							char * szProfessionTemplate = KMessageCentre::GetMessage(41,2);
							if (szProfessionTemplate)
							{
								char szZhiye[16];
							    szZhiye[0] = 0;
							
								if ( ms_Singleton->m_pMembersList[nIdx].m_nSkillSeries < 0 )
								{
									switch(ms_Singleton->m_pMembersList[nIdx].m_nSeries)
									{
									case 0:
										strcpy( szZhiye, ROLE_CAREER_JS);
										break;
									case 1:
										strcpy( szZhiye, ROLE_CAREER_DS);
										break;
									case 2:
										strcpy( szZhiye, ROLE_CAREER_YR);
										break;
									default :
										strcpy( szZhiye, ROLE_CAREER_JS);
										break;
									}//end switch
								}
								else
								{
									
									switch(ms_Singleton->m_pMembersList[nIdx].m_nSeries)
									{
									case 0:
										if ( ms_Singleton->m_pMembersList[nIdx].m_nSkillSeries )
										{
											strcpy( szZhiye, ROLE_CAREER_JS_0);
										}
										else
										{
											strcpy( szZhiye, ROLE_CAREER_JS_1);
										}
										break;
									case 1:
										if ( ms_Singleton->m_pMembersList[nIdx].m_nSkillSeries )
										{
											strcpy( szZhiye, ROLE_CAREER_DS_0);
										}
										else
										{
											strcpy( szZhiye, ROLE_CAREER_DS_1);
										}
										break;
									case 2:
										if ( ms_Singleton->m_pMembersList[nIdx].m_nSkillSeries )
										{
											strcpy( szZhiye, ROLE_CAREER_YR_1);
										}
										else
										{
											strcpy( szZhiye, ROLE_CAREER_YR_0);
										}
										break;
									default :
										if ( ms_Singleton->m_pMembersList[nIdx].m_nSkillSeries )
										{
											strcpy( szZhiye, ROLE_CAREER_JS_1);
										}
										else
										{
											strcpy( szZhiye, ROLE_CAREER_JS_0);
										}
										break;
									}
								}

								char szProfession[128];
								szProfession[0] = 0;

								sprintf(szProfession,szProfessionTemplate,szZhiye);
								strcat(szToolTip,szProfession);

							}//endif

							//Level...
							char * szLevelTemplate = KMessageCentre::GetMessage(41,3);
							if (szLevelTemplate)
							{
							    char szLevel[128];
								szLevel[0] = 0;
								
								sprintf(szLevel,szLevelTemplate,ms_Singleton->m_pMembersList[nIdx].m_nLevel);
								strcat(szToolTip,szLevel);
							}//endif
							
							//End.....
							char * szEnd = KMessageCentre::GetMessage(41,4);
							if (szEnd)
							{
								strcat(szToolTip,szEnd);
							}//endif


							Tooltip * pToolTip  = pSmallPanel->getTooltip();
							char    * szCurrTip = Utf8ToAnsi(pSmallPanel->getTooltipText()); 
							if ( szCurrTip == NULL || strcmp(szCurrTip,szToolTip) != 0 )
							{
								pToolTip->setTargetWindow(pSmallPanel);
								pSmallPanel->setTooltipText(AnsiToUtf8(szToolTip));
								TLStaticImage * _pMemberPanel = NULL;
								_pMemberPanel = static_cast<TLStaticImage *>(ms_Singleton->m_pThisWnd->getChild(str));
								_pMemberPanel->setTooltipText(AnsiToUtf8(szToolTip));
							}//endif

						}//endif

						if (!pTeamMember->isVisible())
							pTeamMember->show();

						if (m_bCanShow)
						{
							if ( ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getAbsolutePosition().d_x >=0 )
							{
								String str(pTeamMember->getName());
								float fSmallWidth     = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getWidth(Absolute);
								Point SmallInfoPos    = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getPosition(Absolute);
								Point newSmallInfoPos = SmallInfoPos;
							    newSmallInfoPos.d_x   = - fSmallWidth;
								((TLStaticImage *)ms_Singleton->m_pThisWnd->getChild(str)->getChild(str+ "/SmallPanel"))->MoveTo(newSmallInfoPos.d_x,newSmallInfoPos.d_y,-32,0,true);
							}//endif

						}//endif
						else
						{
							float fFrontWidth     = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/TeamMemberFront")->getWidth(Absolute);

							if ( ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getAbsolutePosition().d_x < fFrontWidth )
							{
								String str(pTeamMember->getName());
								
								Point SmallInfoPos    = ms_Singleton->m_pThisWnd->getChild(str)->getChild(str + "/SmallPanel")->getPosition(Absolute);
								Point newSmallInfoPos = SmallInfoPos;
								newSmallInfoPos.d_x   = fFrontWidth;
								((TLStaticImage *)ms_Singleton->m_pThisWnd->getChild(str)->getChild(str+ "/SmallPanel"))->MoveTo(newSmallInfoPos.d_x,newSmallInfoPos.d_y,16,0,true);
							}//endif
							
						}
							

						++nMemberWndIdx;
					}
				}
			}
		}
	}
	else
	{
        memset(&ms_Singleton->m_pMembersList,0,sizeof(ms_Singleton->m_pMembersList));
	}//end else

	for (int i=0;i<MAX_TEAMMEMBER_COUNT;i++)
	{
		if (ms_Singleton->m_pMembersList[i].m_uId==0)
        {
			char szName[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szName, "%d_TaharezLook/TeamMember", i );
			Window* pTeamMember = ms_Singleton->m_pThisWnd->getChild( szName );
			if ( pTeamMember )
			{
				pTeamMember->hide();
			}//endif
			
		}//endif
	}//end for i

	return 0;
}

bool	KUiTeamList::Captain(const CEGUI::EventArgs& args)
{
	KUiPlayerItem tagPlayer;
	strncpy( tagPlayer.Name, m_pMembersList[m_nCurMember].m_szName, CLIENT_NAME_AND_TITLE_MAX + 1);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = m_pMembersList[m_nCurMember].m_nIndex;
	tagPlayer.nParam = 0;
	tagPlayer.uId = m_pMembersList[m_nCurMember].m_uId;
	g_pCoreShell->TeamOperation( TEAM_OI_APPOINT, (unsigned int)&tagPlayer, NULL );

	ms_Singleton->m_pRolePopmenu->hide();
	return 0;
}

bool	KUiTeamList::Kick(const CEGUI::EventArgs& args)
{
	KUiPlayerItem tagPlayer;
	strncpy( tagPlayer.Name, m_pMembersList[m_nCurMember].m_szName, CLIENT_NAME_AND_TITLE_MAX + 1);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = m_pMembersList[m_nCurMember].m_nIndex;
	tagPlayer.nParam = 0;
	tagPlayer.uId = m_pMembersList[m_nCurMember].m_uId;
	g_pCoreShell->TeamOperation( TEAM_OI_KICK, (unsigned int)&tagPlayer, NULL );
	ms_Singleton->m_pRolePopmenu->hide();
	return 0;
}

bool	KUiTeamList::Leave(const CEGUI::EventArgs& args)
{
	g_pCoreShell->TeamOperation( TEAM_OI_LEAVE, NULL, NULL );
	ms_Singleton->m_pRolePopmenu->hide();
	return 0;
}

/*bool	KUiTeamList::Exp(const CEGUI::EventArgs& args)
{
	ms_Singleton->m_pRolePopmenu->hide();
	return 0;
}*/

bool	KUiTeamList::Friend(const CEGUI::EventArgs& args)
{
	if ( g_pCoreShell )
	{
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(m_pMembersList[m_nCurMember].m_szName), CHAT::GROUPID_NONE );
		ms_Singleton->m_pRolePopmenu->hide();
	}
	return 0;
}

bool	KUiTeamList::OpenBT(const CEGUI::EventArgs& args)
{
	if ( g_pCoreShell )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_OPEN_BIG_TEAM_MODE, NULL, NULL );
		ms_Singleton->m_pRolePopmenu->hide();
	}
	return 0;
}

bool KUiTeamList::Chat(const CEGUI::EventArgs& args)
{
	KUiChatInputWnd::GetSingleton().clearText();
	
	KUiChatInputWnd::GetSingleton().write("/");
	
	KUiChatInputWnd::GetSingleton().write(m_pMembersList[m_nCurMember].m_szName);
	KUiChatInputWnd::GetSingleton().write(" ");
	
	KUiChatInputWnd::GetSingleton().show();

	ms_Singleton->m_pRolePopmenu->hide();
	return 0;
}

void KUiTeamList::HideAllMember( void )
{
	for ( int nIdx = 0; nIdx < MAX_TEAMMEMBER_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/TeamMember", nIdx );
		Window* pTeamMember = m_pThisWnd->getChild( szName );
		if ( pTeamMember )
		{
			pTeamMember->hide();
		}
	}
}

void KUiTeamList::ShowAllMember( void )
{
	for ( int nIdx = 0; nIdx < MAX_TEAMMEMBER_COUNT; ++nIdx )
	{
		char szName[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szName, "%d_TaharezLook/TeamMember", nIdx );
		Window* pTeamMember = m_pThisWnd->getChild( szName );
		if ( pTeamMember )
		{
			pTeamMember->show();
		}
	}
}

void KUiTeamList::HideTeamMenu()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && m_pRolePopmenu->isVisible() )
	{
		m_pRolePopmenu->hide();
	}
}

void KUiTeamList::QuitGame()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		if ( ms_Singleton->m_pThisWnd->isVisible() )
		{
			CEGUI::EventArgs e;
			Leave(e);
		}
	}
}

bool KUiTeamList::MouseButtonDown( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	if (NULL == pArgs)
		return false;

	char szBuf[COMMON_CLIENT_MSG_LEN_64];
	String str(pArgs->window->getName());
	str = str.substr(0, str.find_first_of("_"));
	strcpy( szBuf, Utf8ToAnsi( str ));
	m_nCurMember = atoi( szBuf );

	DWORD memberNpcId = m_pMembersList[m_nCurMember].m_uId;

	if (LeftButton == pArgs->button)
	{
		//左键选中队友
		int npcIndex = g_pCoreShell->FindNpcIndexById(memberNpcId);
		if (npcIndex > 0)
		{
			g_pCoreShell->SelectNPC(npcIndex);
			if (!KUiDragItem::GetSingleton().getObj()->isEmpty())
			{
				if ( KUiDragItem::GetSingleton().getObj()->isSkill() || 
					KUiDragItem::GetSingleton().getObj()->getType() == TLGameObject::shortcut )
				{
					KUiDragItem::GetSingleton().getObj()->clear();
				}
				else
				{
					if (g_pCoreShell)
					{
						KUiTradeBox::GetSingleton().SetIsItem(true);
						if (!KUiTradeBox::GetSingleton().isVisible())
						{
							g_pCoreShell->TradeApplyStart(memberNpcId);
						}
						else
						{
							KUiTradeBox::GetSingleton().AddItemByClickPlayer();
						}
					}
				}
			}
		}
		else
		{
			KUiChannelCentre::GetSingleton().toSysMsg(KMessageCentre::GetMessage(team_message, MSG_TEAMMATE_NOT_NEARBY));
		}
	}
	else if (RightButton == pArgs->button)
	{		
		//右键弹出队友菜单
		if ( !m_pRolePopmenu->isVisible() )
		{
			KUiPlayerTeam teamInfo;
			g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&teamInfo, NULL );

			m_pRootSheet->removeChildWindow(m_pRolePopmenu);
			m_pRootSheet->addChildWindow(m_pRolePopmenu);

			Point parentP = m_pThisWnd->getPosition(Absolute);
			Point newP = MouseCursor::getSingleton().getPosition();
			m_pRolePopmenu->setPosition(Absolute, newP);

			m_pKick->setEnabled(teamInfo.bCanKick && (memberNpcId != teamInfo.dwCaptainNpcID));
			m_pCaption->setEnabled(teamInfo.bTeamLeader);			

			m_pRolePopmenu->show();
			//m_pRolePopmenu->activate();
		}
		else
		{
			m_pRolePopmenu->hide();
		}
	}

	pArgs->handled = true;

	return true;
}

void  KUiTeamList::SetCanShow(const bool bShow)
{
	m_bCanShow = bShow;
}

bool  KUiTeamList::GetCanShow()
{
    return m_bCanShow;
}

/**************************************
 * KUiTeamHideShow                    *
 **************************************/

template<> 
KUiTeamHideShow* KUiWndSingleton<KUiTeamHideShow>::ms_Singleton	= NULL;

KUiTeamHideShow::KUiTeamHideShow(const CEGUI::String& id_name )
:KUiWndSingleton<class KUiTeamHideShow>(id_name)
{

}

KUiTeamHideShow::~KUiTeamHideShow()
{

}

void KUiTeamHideShow::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		TLButton * pButton=(TLButton * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/TeamShowHide/SHButton");
		if (pButton)
		{
			pButton->setEnabled(true);
			pButton->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTeamHideShow::TeamHandleClick, this));
		}//endif

		d_TGButton=(TLButton * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/TeamShowHide/TGButton");
		if (d_TGButton)
		{
			d_TGButton->setEnabled(true);
			d_TGButton->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTeamHideShow::GroupSwitch, this));
		}//endif
		d_OpenTGButton=(TLButton * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/TeamShowHide/OpenTGButton");
		if (d_OpenTGButton)
		{
			d_OpenTGButton->setEnabled(true);
			d_OpenTGButton->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiTeamHideShow::OpenGroupWnd, this));
		}//endif

		ms_Singleton->m_pThisWnd->setZLevel(Window::Bottom);
	}//endif
}

bool KUiTeamHideShow::TeamHandleClick(const CEGUI::EventArgs& args)
{
	if (KUiTeamList::GetCanShow())
	{
		KUiTeamList::SetCanShow(false);
		KUiTeamList::Hide();
	}//endif
	else
	{
		KUiTeamList::SetCanShow(true);
		KUiTeamList::Show();
	}//end else

	return true;
}

bool KUiTeamHideShow::GroupSwitch(const CEGUI::EventArgs& args)
{
	g_pCoreShell->TeamOperation(TEAM_OI_OPEN_BIG_TEAM_MODE, NULL, NULL);
	return true;
}

bool KUiTeamHideShow::OpenGroupWnd(const CEGUI::EventArgs& args)
{
	KUiRaid::getSinglton().toggle();
	return true;
}

void KUiTeamHideShow::SetTGBBtnState( bool bEnable )
{
	if ( d_TGButton )
	{
		d_TGButton->setEnabled( bEnable );
	}
}

void KUiTeamHideShow::SetOpenTGBBtnState( bool bEnable )
{
	if ( d_OpenTGButton )
	{
		d_OpenTGButton->setEnabled( bEnable );
	}
}