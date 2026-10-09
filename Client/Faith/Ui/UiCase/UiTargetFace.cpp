//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/14/2006 19:05
//      File_base        : KUiHeadToolBar
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiTargetFace.h"
#include "UiChatCentre.h"
#include "UiTargetbufferWnd.h"
#include "SocialComDef.h"
#include "UiPlayerMenu.h"
#include "UiChatWindow.h"
#include "Ui/UiCase/UiDragItem.h"
#include "Ui/UiCase/UiTradeBox.h"

extern iCoreShell* g_pCoreShell;

const unsigned short				s_dwPingPerSecond	= 0;

using namespace CEGUI;

template<> 
KUiTargetFace* KUiWndSingleton<KUiTargetFace>::ms_Singleton	= NULL;

KBufferSyncInfo KUiTargetFace::m_buffSyncInfo[MAX_TARGET_BUFFER];

KUiTargetFace::KUiTargetFace( const CEGUI::String& id_name ):
KUiWndSingleton<KUiTargetFace>( id_name )
{
}

KUiTargetFace::~KUiTargetFace()
{

}

void KUiTargetFace::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setZLevel(Window::SuperBottom);
		m_Blood				= ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild("TaharezLook/TargetFace/TargetFaceImage/RoleBlood");
		m_Magic				= ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild("TaharezLook/TargetFace/TargetFaceImage/RoleMagic");
		m_Level				= ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceFront")->getChild("TaharezLook/TargetFace/TargetFaceFront/RoleLevel");
		m_pRoleFace			= ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild("TaharezLook/TargetFace/TargetFaceImage/RoleFace");
	
		m_pRoleFace        ->setZLevel(Window::Top);
		m_pRoleFaceFrame    = ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild("TaharezLook/TargetFace/TargetFaceImage/RoleFaceFrame");
		m_pRoleFaceFrame   ->setZLevel(Window::Bottom);

		m_pDisplay			= ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild("TaharezLook/TargetFace/TargetFaceImage/RoleFace");
		m_pTargetBloodBar	= (ProgressBar*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild( "TaharezLook/TargetFace/TargetFaceImage/BloodProgBar" );
		m_ProgressBar		= (ProgressBar*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild( "TaharezLook/TargetFace/TargetFaceImage/MagicProgBar" );
		m_Name				= 	ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/Name");
		m_Kulou				= m_Level->getChild("TaharezLook/TargetFace/TargetFaceFront/RoleLevel/Kulou");

		m_TargetKind        = kind_num;
		ms_Singleton->m_pThisWnd->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(& KUiTargetFace::ShowRoleMenu, ms_Singleton));

	}
}

void KUiTargetFace::Show()
{
	KUiWndSingleton<KUiTargetFace>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && ms_Singleton->m_pThisWnd->isVisible() )
	{
		KUiTargetbufferCentre::Show();
		ms_Singleton->UpdateData();

		KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
		if ( pRoom && pRoom->IsVisible() )
		{
			KUiChatInputWnd::GetSingleton().Hide();
			pRoom->Show();
		}
		else
		{
			if(KUiChatInputWnd::GetSingleton().isCurInput())
			{
				KUiChatInputWnd::GetSingleton().show();
			}
		}

	}
}

void KUiTargetFace::Hide()
{
	KUiWndSingleton<KUiTargetFace>::Hide();
	KUiTargetbufferCentre::Hide();
}

unsigned int KUiTargetFace::UpdateData( void ) 
{
	KUiPlayerAttribute runtimeAttribute;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&runtimeAttribute, NULL );
	int nCount = g_pCoreShell->GetGameData( GDI_GET_TARGET_BUFFER_INFO, (unsigned int)&m_buffSyncInfo, MAX_TARGET_BUFFER);

	KUiTargetbufferCentre::DelAllRoleBuffer();
	for ( int nIdx = 0; nIdx < nCount; ++nIdx )
	{
		KUiTargetbufferCentre::AddRoleBuffer( (unsigned int)&m_buffSyncInfo[nIdx], KUiTargetbufferCentre::wrap );
	}

	ms_Singleton->m_TargetInfo.szHeadImage = "";
	g_pCoreShell->GetGameData( GDI_PLAYER_TARGET_INFO, (unsigned int)&ms_Singleton->m_TargetInfo, NULL );
	ms_Singleton->m_TargetKind = g_pCoreShell->GetGameData(GDI_GET_NPC_KIND,(unsigned int)ms_Singleton->m_TargetInfo.nIndex,0);
	ms_Singleton->m_Name->setText( AnsiToUtf8( ms_Singleton->m_TargetInfo.strName ) );


	char szBuf[COMMON_CLIENT_MSG_LEN_32];

	int nLifePercentage = ms_Singleton->m_TargetInfo.nLifePercentage;
	if (ms_Singleton->m_TargetInfo.nMetier == -1 && nLifePercentage == 0)
		nLifePercentage = 1;

	sprintf( szBuf, "%d%s", nLifePercentage, "%" );
	ms_Singleton->m_Blood->setText( AnsiToUtf8( szBuf ) );
	sprintf( szBuf, "%d%s", ms_Singleton->m_TargetInfo.nMagicPercentage, "%" );
	ms_Singleton->m_Magic->setText( AnsiToUtf8( szBuf ) );

	int nRelation = g_pCoreShell->GetNPCRelation(ms_Singleton->m_TargetInfo.nIndex);
	
	/*
	if ( nRelation != relation_dialog )
		sprintf( szBuf, "%d", ms_Singleton->m_TargetInfo.nLevel);
	else//*/

    sprintf(szBuf,"%d",ms_Singleton->m_TargetInfo.nLevel);
	
	ms_Singleton->m_Level->setText( AnsiToUtf8( szBuf ) );

	if ( runtimeAttribute.nLevel >= ms_Singleton->m_TargetInfo.nLevel - 20 )
	{
		ms_Singleton->m_Kulou->hide();
	}
	else
	{
		ms_Singleton->m_Kulou->show();
	}

	//职业
	StaticImage* pProfessionImage = (StaticImage*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/TargetFace/TargetFaceImage")->getChild("TaharezLook/TargetFace/TargetFaceImage/Profession");
	if (ms_Singleton->m_TargetInfo.nMetier!=-1 )
	{
			char professionImageSet[COMMON_CLIENT_MSG_LEN_32] = { 0 };
			char professionImageName[COMMON_CLIENT_MSG_LEN_32] = { 0 };
			char professionName[COMMON_CLIENT_MSG_LEN_32] = { 0 };
			char professionColor[COMMON_CLIENT_MSG_LEN_32] = { 0 };

			KUiCfgLoader::getSingleton().getProfessionCfg().GetImage(ms_Singleton->m_TargetInfo.nMetier, ms_Singleton->m_TargetInfo.nSkillSeries, professionImageSet, COMMON_CLIENT_MSG_LEN_32, professionImageName, COMMON_CLIENT_MSG_LEN_32);
			KUiCfgLoader::getSingleton().getProfessionCfg().GetName(ms_Singleton->m_TargetInfo.nMetier, ms_Singleton->m_TargetInfo.nSkillSeries, professionName, COMMON_CLIENT_MSG_LEN_32);
			KUiCfgLoader::getSingleton().getProfessionCfg().GetColor(ms_Singleton->m_TargetInfo.nMetier, ms_Singleton->m_TargetInfo.nSkillSeries, professionColor, COMMON_CLIENT_MSG_LEN_32);
			pProfessionImage->setImage( AnsiToUtf8( professionImageSet ), AnsiToUtf8( professionImageName ) );
	        pProfessionImage->show();
	}//endif
	else
	{
		pProfessionImage->hide();
	}//end else

	//头像
	if ( ms_Singleton->m_TargetInfo.nMetier!=-1)
	{
		if ( (ms_Singleton->m_TargetInfo.szHeadImage != NULL && 
			strcmp(ms_Singleton->m_TargetInfo.szHeadImage,"") != 0 )&& 
			(ms_Singleton->m_TargetInfo.szHeadImageSet != NULL &&
			strcmp(ms_Singleton->m_TargetInfo.szHeadImageSet, "") != 0))
		{
			String imgsetname = AnsiToUtf8(ms_Singleton->m_TargetInfo.szHeadImageSet);
			String imgname    = AnsiToUtf8(ms_Singleton->m_TargetInfo.szHeadImage);
			Imageset *imgset = ImagesetManager::getSingleton().getImageset(imgsetname);
			try
			{
				Image img = imgset->getImage(imgname);
				static_cast<StaticImage*>(ms_Singleton->m_pRoleFace)->setImage( AnsiToUtf8(ms_Singleton->m_TargetInfo.szHeadImageSet), AnsiToUtf8(ms_Singleton->m_TargetInfo.szHeadImage) );	
			}
			catch (...)
			{
				try
				{
					Image img = imgset->getImage(String("zanwu"));
					static_cast<StaticImage*>(ms_Singleton->m_pRoleFace)->setImage( AnsiToUtf8(ms_Singleton->m_TargetInfo.szHeadImageSet), AnsiToUtf8(ms_Singleton->m_TargetInfo.szHeadImage) );	
				}
				catch (...)
				{
					return 0;
				}
			}
		}
		else
		{
			String strRoleFaceImageSetName;
			String strRoleFaceImageName;
			
			KUiCfgLoader& cfg = KUiCfgLoader::getSingleton();
			std::string imageset;
			std::string image;
			if ( ms_Singleton->m_TargetInfo.nSex )
			{
				cfg.getMinWomanPortraitPath(ms_Singleton->m_TargetInfo.nPortrait, imageset, image );
			}
			else
			{
				cfg.getMinManPortraitPath(ms_Singleton->m_TargetInfo.nPortrait, imageset, image );
			}
			
			strRoleFaceImageSetName = imageset.c_str();
			strRoleFaceImageName = image.c_str();
			
			if ( !strRoleFaceImageName.empty() )
			{
				static_cast<StaticImage*>(ms_Singleton->m_pRoleFace)->setImage( strRoleFaceImageSetName, strRoleFaceImageName );	
			}
			
		}

		ms_Singleton->m_pRoleFace->show();
		ms_Singleton->m_pRoleFaceFrame->show();
		
	}//endif
	else
	{
        ms_Singleton->m_pRoleFace->hide();
		ms_Singleton->m_pRoleFaceFrame->hide();
	}//end else

	if ( ms_Singleton->m_pTargetBloodBar && ms_Singleton->m_ProgressBar )
	{
		ms_Singleton->m_pTargetBloodBar->setProgress( nLifePercentage / 100.0f );
		ms_Singleton->m_ProgressBar->setProgress( ms_Singleton->m_TargetInfo.nMagicPercentage / 100.0f );
	}//*/
//	ms_Singleton->m_pThisWnd->requestRedraw();
	return 0;
}

bool KUiTargetFace::ShowRoleMenu(const CEGUI::EventArgs& args)
{
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	if (pArgs->button != RightButton)
	{
		Point parentP = m_pThisWnd->getPosition(Absolute);
		Point newP = MouseCursor::getSingleton().getPosition();
		int nRelation = g_pCoreShell->GetNPCRelation(m_TargetInfo.nIndex);
		if (m_TargetKind != kind_player || !(nRelation == relation_ally || (nRelation == relation_enemy && (-1 != m_TargetInfo.nSex) )))
		{
			KUiPlayerMenu::Hide();
			return true;
		}

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
						g_pCoreShell->TradeApplyStart(m_TargetInfo.nId);
					}
					else
					{
						KUiTradeBox::GetSingleton().AddItemByClickPlayer();
					}
				}
			}
		}
		KUiPlayerMenu::Hide();
		return true;
	}
	
	//如果是佣兵就不弹出来
	if(g_pCoreShell->GetGameData(GDI_IS_EMPLOEE, 1, m_TargetInfo.nIndex))
	{
		return true;
	}

	//如果是召唤兽就不弹出来
	if(g_pCoreShell->GetGameData(GDI_IS_EMPLOEE, 0, m_TargetInfo.nIndex))
	{
		return true;
	}

	Point parentP = m_pThisWnd->getPosition(Absolute);
	Point newP = MouseCursor::getSingleton().getPosition();
	int nRelation = g_pCoreShell->GetNPCRelation(m_TargetInfo.nIndex);
	
	if (m_TargetKind == kind_player && (nRelation == relation_ally || (nRelation == relation_enemy && (-1 != m_TargetInfo.nSex) )))
	{
		KUiPlayerMenu::GetSingleton().setPos(newP);
		KUiPlayerMenu::GetSingleton().show(m_TargetInfo.strName, m_TargetInfo.nId, m_TargetInfo.nTeamId, m_TargetInfo.nPrivateState > 0);
	}
	return true;
}