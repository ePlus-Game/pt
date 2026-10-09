//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2/21/2007 14:32
//      File_base        : UiPlayerMenu
//      File_ext         : cpp
//      Author           : 谢鉷
//      Description      : 把小雨的玩家菜单功能从targetface中拆出来
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiPlayerMenu.h"
#include "CoreShell.h"
#include "SocialComDef.h"
#include "UiChatWindow.h"
#include "../KMessageCentre.h"
#include "UiErrorMessageBox.h"
#include "UiTeamList.h"
#include "Ui/UiCase/UiDragItem.h"
#include "UiComMsgBox.h"
#include "chatWindow/PlayerShowInfo.h"
#include "Ui/UiCase/UiGMCommunication.h"
#include "Ui/UiCase/UiTargetEquipment.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiPlayerMenu* KUiWndSingleton<KUiPlayerMenu>::ms_Singleton	= NULL;

KUiPlayerMenu::KUiPlayerMenu(String wndType):
KUiWndSingleton<KUiPlayerMenu>(wndType),d_PrivateState(false)
{
	memset(d_reportText, 0, sizeof(d_reportText));

	for (int i = 0; i < PlayerMenuButtonCount; ++i)
	{
		m_ButtonList[i] = NULL;
	}
}

KUiPlayerMenu::~KUiPlayerMenu()
{

}

void  KUiPlayerMenu::ChuanSongByName()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && g_pCoreShell)
	{
		const String& test = KUiComMsgBox::GetSingleton().getEditText();
		GM_ChuanSong chuanSong;
		sscanf( Utf8ToAnsi(test),"%d,%d,%d", &chuanSong.nMapID, &chuanSong.nPosX, &chuanSong.nPosY );
		g_pCoreShell->OperationRequest(GOI_CHUANSONG, (unsigned int)&chuanSong, NULL);	
	}
}

void KUiPlayerMenu::DongjiePlayerByName()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && g_pCoreShell)
	{
		const String& test = KUiComMsgBox::GetSingleton().getEditText();
		int nSec = atoi(Utf8ToAnsi(test));
		if ( nSec >= 0 )
		{
			g_pCoreShell->OperationRequest(GOI_DONGJIE_PLAYER, (unsigned int)ms_Singleton->d_playerName, nSec * 60);	
		}	
		else
		{
			g_pCoreShell->OperationRequest(GOI_DONGJIE_PLAYER, (unsigned int)ms_Singleton->d_playerName, 60);	
		}
	}	
}

void KUiPlayerMenu::JinyanPlayerByName()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd && g_pCoreShell)
	{
		const String& test = KUiComMsgBox::GetSingleton().getEditText();
		int nSec = atoi(Utf8ToAnsi(test));
		if ( nSec >= 0 )
		{
			g_pCoreShell->OperationRequest(GOI_JINYAN_PLAYER, (unsigned int)ms_Singleton->d_playerName, nSec * 60);	
		}
		else
		{
			g_pCoreShell->OperationRequest(GOI_JINYAN_PLAYER, (unsigned int)ms_Singleton->d_playerName, 60);	
		}	
	}
}

void KUiPlayerMenu::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );
		ms_Singleton->getChild();		
	}
	m_iTargetTeamID = INVALID_TEAM_ID;
	d_playerName[0] = 0;
	d_playerNpcId = 0;
}

void KUiPlayerMenu::getChild()
{
#ifndef _DEBUG
	try
#endif
	{
		d_chatBtn			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Chat");
		d_groupInviteBtn	= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Invite");
		d_tradeBtn			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Trade");
		d_viewBtn			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/View"); 
		d_detailBtn			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Detail");
		d_followBtn			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Follow");
		d_screenBtn			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Screen");
		d_pInviteShizu  	= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Shizhu");
		d_pInviteZhuhou  	= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Zhuhou");
		d_addFriend			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/AddFriend");
		d_appTeam			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/ApplicateTeam");
		d_pKick				= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Kick");
		d_pJinyan			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Jinyan");
		d_pDongjie			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Dongjie");
		d_pDongjieAccount   = (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/DongjieAccount");
		d_pChuanSong		= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/ChuanSong");
		d_pIp				= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Ip");
		d_report			= (PushButton*)m_pThisWnd->getChild("TaharezLook/RolePopMenu/Report");

		d_chatBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onChatClick, this));
		d_groupInviteBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onGroupInviteClick, this));
		d_tradeBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onTradeClick, this));
		d_viewBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onViewClick, this));
		d_detailBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onDetailClick, this));
		d_followBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onFollowClick, this));
		d_screenBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onScreenClick, this));
		d_pInviteShizu->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onShizuInviteClick, this));
		d_pInviteZhuhou->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onZhuhouInviteClick, this));
		d_addFriend->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onAddFriend, this));
		d_appTeam->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onAppTeam, this));
		d_pKick->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onKick, this));	
		d_pJinyan->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onJinyan, this));
		d_pDongjie->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onDongjie, this));
		d_pDongjieAccount->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onDongjieAccount, this));
		d_pChuanSong->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onChuanSong, this));
		d_pIp->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onIp, this));
		d_report->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiPlayerMenu::onReport, this));

		m_ButtonList[PlayerMenuPrivateChat]		= d_chatBtn;
		m_ButtonList[PlayerMenuInvite]			= d_groupInviteBtn;
		m_ButtonList[PlayerMenuTrade]			= d_tradeBtn;
		m_ButtonList[PlayerMenuView]			= d_viewBtn;
		m_ButtonList[PlayerMenuFollow]			= d_followBtn;
		m_ButtonList[PlayerMenuScreen]			= d_screenBtn;
		m_ButtonList[PlayerMenuShizu]			= d_pInviteShizu;
		m_ButtonList[PlayerMenuZhuhou]			= d_pInviteZhuhou;
		m_ButtonList[PlayerMenuAddFriend]		= d_addFriend;
		m_ButtonList[PlayerMenuApplicateTeam]	= d_appTeam;
		m_ButtonList[PlayerMenuKick]			= d_pKick;
		m_ButtonList[PlayerMenuJinYan]			= d_pJinyan;
		m_ButtonList[PlayerMenuDongjie]			= d_pDongjie;
		m_ButtonList[PlayerMenuDongjieAccount]  = d_pDongjieAccount;
		m_ButtonList[PlayerMenuChuansong]		= d_pChuanSong;
		m_ButtonList[PlayerMenuIP]				= d_pIp;
		m_ButtonList[PlayerMenuReport]			= d_report;
	}
#ifndef _DEBUG
	catch (...)
	{
		d_chatBtn			= NULL;
		d_groupInviteBtn	= NULL;
		d_tradeBtn			= NULL;
		d_viewBtn			= NULL;
		d_detailBtn			= NULL;
		d_followBtn			= NULL;
		d_screenBtn			= NULL;
		d_pInviteShizu  	= NULL;
		d_pInviteZhuhou  	= NULL;
		d_addFriend			= NULL;
		d_appTeam			= NULL;
		d_pKick				= NULL;
		d_pJinyan			= NULL;
		d_pDongjie			= NULL;
		d_pDongjieAccount   = NULL;
		d_pChuanSong		= NULL;
		d_pIp				= NULL;
		d_report			= NULL;

		for (int i = 0; i < PlayerMenuButtonCount; ++i)
		{
			m_ButtonList[i] = NULL;
		}
	}
#endif
}

bool KUiPlayerMenu::onChatClick(const EventArgs& args)
{
	if (!d_PrivateState)
	{
		KUiChatInputWnd::GetSingleton().clearText();
		
		KUiChatInputWnd::GetSingleton().write("/");
		
		KUiChatInputWnd::GetSingleton().write(d_playerName);
		KUiChatInputWnd::GetSingleton().write(" ");
		
		KUiChatInputWnd::GetSingleton().show();
		
		m_pThisWnd->hide();
	}//endif
	
	return true;
}

bool KUiPlayerMenu::onGroupInviteClick( const EventArgs& args)
{
	KUiPlayerItem tagPlayer;

	strncpy( tagPlayer.Name, d_playerName, CLIENT_NAME_AND_TITLE_MAX + 1);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	tagPlayer.uId = d_playerNpcId;


	KUiPlayerTeam	TeamInfo;
	TeamInfo.cNumMember = 0;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
	if ( ((int)TeamInfo.cNumMember) <= MAX_TEAMMEMBER_COUNT )
	{
		if (TeamInfo.cNumMember == 0)
		{
			g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
		}

		g_pCoreShell->TeamOperation( TEAM_OI_INVITE, (unsigned int)&tagPlayer, NULL );
	}
	else
	{
		char *msg = KMessageCentre::GetMessage(team_message, 1);
		KUiChannelCentre::GetSingleton().toSysMsg(msg);
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onTradeClick(const EventArgs& args)
{	
	if (g_pCoreShell)
	{
		g_pCoreShell->TradeApplyStart(d_playerNpcId);
	}
	m_pThisWnd->hide();
	m_pThisWnd->deactivate();
	return true;
}

bool KUiPlayerMenu::onViewClick(const EventArgs& args)
{
	if (!d_PrivateState)
	{
		if (g_pCoreShell)
		{
			KUiTargetEquipment::GetSingleton().SetSelectedPlayerID( d_playerNpcId );
			g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM, (UINT)d_playerNpcId, NULL);
		}
		m_pThisWnd->hide();
	}//endif
	return true;
}

bool KUiPlayerMenu::onDetailClick(const EventArgs& args)
{
	PlayerInfo info;
	memset(&info,0,sizeof(PlayerInfo));
	strcpy(info.szName, d_playerName);
	PlayerShowInfo::GetSingle().AddItem(info);
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onFollowClick(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)d_playerNpcId, 0);
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onScreenClick(const EventArgs& args)
{
	if (!d_PrivateState)
	{
		if ( g_pCoreShell )
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_ADD_BLACK_LIST, (unsigned int)d_playerName, NULL );
		}

		m_pThisWnd->hide();

	}//endif
	
	return true;
}

bool KUiPlayerMenu::onShizuInviteClick(const EventArgs& args)
{
	if (d_playerName[0]!=0 && !d_PrivateState)
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= enSULayer_Gens;
		tagTongOper.nOperationID	= enSUO_AddSubUnit;
		strcpy(tagTongOper.szName,d_playerName);
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}//endif
		
		pTongOper->updateRecord( enSULayer_Gens, &tagTongOper, sizeof( TongOperParam ) );	
		
	}//endif
	
	Hide();

	return true;
}

bool KUiPlayerMenu::onZhuhouInviteClick(const EventArgs& args)
{
	if (d_playerName[0]!=0 && !d_PrivateState)
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= enSULayer_Tong;
		tagTongOper.nOperationID	= enSUO_AddSubUnit;
		strcpy(tagTongOper.szName,d_playerName);
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
		{
			Hide();
			return false;
		}//endif
		
		pTongOper->updateRecord( enSULayer_Tong, &tagTongOper, sizeof( TongOperParam ) );	
		
	}//endif
	
	Hide();

	return true;
}

bool KUiPlayerMenu::onAddFriend(const EventArgs& args)
{
	if (!d_PrivateState)
	{
		g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(d_playerName), CHAT::GROUPID_NONE );
		m_pThisWnd->hide();
	}//endif

	return true;
}

bool KUiPlayerMenu::onKick(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_KICK_PLAYER, (unsigned int)d_playerName, 0);
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onJinyan(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		ZeroMemory(yesString, COMMON_CLIENT_MSG_LEN_8);
		ZeroMemory(noString, COMMON_CLIENT_MSG_LEN_8);
		
		strncpy(yesString, KUiCfgLoader::getSingleton().getCommonCfg().yesString, COMMON_CLIENT_MSG_LEN_8);
		strncpy(noString, KUiCfgLoader::getSingleton().getCommonCfg().noString, COMMON_CLIENT_MSG_LEN_8);
		KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(yesString), AnsiToUtf8(noString));
		KUiComMsgBox::GetSingleton().setStyle(KUiComMsgBox::DongJie);
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(KMessageCentre::GetMessage(common_message, 2001)), true);
		KUiComMsgBox::GetSingleton().setFristBtnCallback(JinyanPlayerByName);			
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onDongjie(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		ZeroMemory(yesString, COMMON_CLIENT_MSG_LEN_8);
		ZeroMemory(noString, COMMON_CLIENT_MSG_LEN_8);
		
		strncpy(yesString, KUiCfgLoader::getSingleton().getCommonCfg().yesString, COMMON_CLIENT_MSG_LEN_8);
		strncpy(noString, KUiCfgLoader::getSingleton().getCommonCfg().noString, COMMON_CLIENT_MSG_LEN_8);
		KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(yesString), AnsiToUtf8(noString));
		KUiComMsgBox::GetSingleton().setStyle(KUiComMsgBox::DongJie);
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(KMessageCentre::GetMessage(common_message, 2000)), true);
		KUiComMsgBox::GetSingleton().setFristBtnCallback(DongjiePlayerByName);		
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onDongjieAccount(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_DONGJIEACCOUNT_PLAYER, (unsigned int)d_playerName, 0);
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onReport(const EventArgs& args)
{
	if (d_report == NULL || m_pThisWnd == NULL)
	{
		return false;	
	}

	char reportStr[COMMON_CLIENT_MSG_LEN_128 + COMMON_CLIENT_MSG_LEN_256];
	memset(reportStr, 0, sizeof(reportStr));

	char * message = KMessageCentre::GetMessage(report_message, 0);
	if (message != NULL && message[0] != 0)
	{
		_snprintf(reportStr, sizeof(reportStr), message, d_playerName);
		reportStr[sizeof(reportStr) - 1] = 0;
	}

	strcat(reportStr, "\n");
	if ((strlen(reportStr) + strlen(d_reportText)) < (COMMON_CLIENT_MSG_LEN_128 + COMMON_CLIENT_MSG_LEN_256 - 1))
	{
		strcat(reportStr, d_reportText);
	}
	else
	{
		strncat(reportStr, d_reportText, (COMMON_CLIENT_MSG_LEN_128 + COMMON_CLIENT_MSG_LEN_256 -1 - strlen(reportStr)));
	}

	d_reportText[COMMON_CLIENT_MSG_LEN_128 + COMMON_CLIENT_MSG_LEN_256 - 1] = 0;
	KUiGMCommunication::getSingleton().setYourMsg(reportStr, false);
 	KUiGMCommunication::getSingleton().show();
 	KUiGMCommunication::getSingleton().showCommit();

	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onAppTeam(const EventArgs& args)
{
	KUiPlayerItem tagPlayer;

	strncpy( tagPlayer.Name, d_playerName, CLIENT_NAME_AND_TITLE_MAX + 1);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	tagPlayer.uId = d_playerNpcId;

	g_pCoreShell->TeamOperation(TEAM_OI_APPLY_JOIN, (unsigned int)&tagPlayer, NULL );
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onChuanSong(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		ZeroMemory(yesString, COMMON_CLIENT_MSG_LEN_8);
		ZeroMemory(noString, COMMON_CLIENT_MSG_LEN_8);
		
		strncpy(yesString, KUiCfgLoader::getSingleton().getCommonCfg().yesString, COMMON_CLIENT_MSG_LEN_8);
		strncpy(noString, KUiCfgLoader::getSingleton().getCommonCfg().noString, COMMON_CLIENT_MSG_LEN_8);
		KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(yesString), AnsiToUtf8(noString));
		KUiComMsgBox::GetSingleton().setStyle(KUiComMsgBox::DongJie);
		KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(KMessageCentre::GetMessage(common_message, 2002)), true);
		KUiComMsgBox::GetSingleton().setFristBtnCallback(ChuanSongByName);	
		
	}
	m_pThisWnd->hide();
	return true;
}

bool KUiPlayerMenu::onIp(const EventArgs& args)
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_IP, (unsigned int)d_playerName, 0);
	}
	m_pThisWnd->hide();
	return true;
}

void KUiPlayerMenu::show(char* name, int npcId, bool bPrivateState)
{

	if(strlen(name) > MAXSIZE_ROLENAME)
	{
		strncpy(d_playerName, name, MAXSIZE_ROLENAME - 1);
		d_playerName[MAXSIZE_ROLENAME - 1] = 0;
	}
	else
	{
		strcpy(d_playerName, name);
	}

	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId       = enSUTplId_Tong;
	tagSocietyIdx.Layer            = enSULayer_Gens;
	tagSocietyIdx.Operation        = enSUO_AddSubUnit;
	bool isShiZu = false;
	bool isZhuHou = false;

	if (ms_Singleton->d_pInviteShizu &&  g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
        ms_Singleton->d_pInviteShizu->show();
		m_pThisWnd->setSize(Absolute, Size(135, 182));
		isShiZu = true;
	}//endif
	else
	{
		ms_Singleton->d_pInviteShizu->hide();
		m_pThisWnd->setSize(Absolute, Size(135, 165));
		isShiZu = false;
	}
	
	tagSocietyIdx.Layer            = enSULayer_Tong;
	
	if (ms_Singleton->d_pInviteZhuhou && g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
		ms_Singleton->d_pInviteZhuhou->show();
		m_pThisWnd->setSize(Absolute, Size(135, 199));
		isZhuHou = true;
	}
	else
	{
		ms_Singleton->d_pInviteZhuhou->hide();
		if (isShiZu)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 182));
		}
		else
		{
			m_pThisWnd->setSize(Absolute, Size(135, 165));
		}
		isZhuHou = false;
	}

	d_playerNpcId  = npcId;
	d_PrivateState = bPrivateState;

	bool bGM = g_pCoreShell->GetGameData(GDI_IS_GM, NULL, NULL) > 0 ? true : false;
	if (bGM)
	{
		d_pKick->show();	
		d_pJinyan->show();
		d_pDongjie->show();
		d_pDongjieAccount->show();
		d_pChuanSong->show();
		d_pIp->show();
		m_pThisWnd->setSize(Absolute,Size(135,301));
	}
	else
	{
		d_pKick->hide();
		d_pJinyan->hide();
		d_pDongjie->hide();
		d_pDongjieAccount->hide();
		d_pChuanSong->hide();
		d_pIp->hide();
		if (isZhuHou)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 199));
		}
		else if (isShiZu)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 182));
		}
		else
		{
			m_pThisWnd->setSize(Absolute,Size(135,165));
		}
	}

	d_detailBtn->hide();
	d_viewBtn->show();
	d_report->hide();

	Show();
	RefreshPos(false);
}

void KUiPlayerMenu::show(char* name, int npcId, int teamID, bool bPrivateState)
{
	if(strlen(name) > MAXSIZE_ROLENAME)
	{
		strncpy(d_playerName, name, MAXSIZE_ROLENAME - 1);
		d_playerName[MAXSIZE_ROLENAME - 1] = 0;
	}
	else
	{
		strcpy(d_playerName, name);
	}

	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId       = enSUTplId_Tong;
	tagSocietyIdx.Layer            = enSULayer_Gens;
	tagSocietyIdx.Operation        = enSUO_AddSubUnit;
	bool isShiZu = false;
	bool isZhuHou = false;
	
	if (ms_Singleton->d_pInviteShizu &&  g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
        ms_Singleton->d_pInviteShizu->show();
		m_pThisWnd->setSize(Absolute, Size(135, 182));
		isShiZu = true;
	}//endif
	else
	{
		ms_Singleton->d_pInviteShizu->hide();
		m_pThisWnd->setSize(Absolute, Size(135, 165));
		isShiZu = false;
	}
	
	tagSocietyIdx.Layer            = enSULayer_Tong;
	
	if (ms_Singleton->d_pInviteZhuhou && g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
		ms_Singleton->d_pInviteZhuhou->show();
		m_pThisWnd->setSize(Absolute, Size(135, 199));
		isZhuHou = true;
	}
	else
	{
		ms_Singleton->d_pInviteZhuhou->hide();
		if (isShiZu)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 182));
		}
		else
		{
			m_pThisWnd->setSize(Absolute, Size(135, 165));
		}
		isZhuHou = false;
	}

	d_playerNpcId  = npcId;
	d_PrivateState = bPrivateState;
/*	m_iTargetTeamID = teamID;

	if (m_iTargetTeamID > 0)
	{
		KUiPlayerTeam	TeamInfo;
		TeamInfo.cNumMember = 0;
		g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
		if (TeamInfo.nTeamServerID > 0)
		{
			d_appTeam->disable();
		}
		else
		{
			d_appTeam->enable();
		}
		d_groupInviteBtn->hide();
		d_appTeam->show();
	}
	else
	{
		KUiPlayerTeam TeamInfo;
		TeamInfo.cNumMember = 0;
		g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
		if (TeamInfo.nTeamServerID > 0)
		{
			if (TeamInfo.bTeamLeader)
			{
				if (TeamInfo.cNumMember < 6)
				{
					d_groupInviteBtn->enable();
				}
				else
				{
					d_groupInviteBtn->disable();
				}
			}
			else
			{
				d_groupInviteBtn->disable();
			}
		}
		else
		{
			d_groupInviteBtn->enable();
		}

		d_groupInviteBtn->show();
		d_appTeam->hide();
	}
*/
	bool bGM = g_pCoreShell->GetGameData(GDI_IS_GM, NULL, NULL) > 0 ? true : false;
	if (bGM)
	{
		d_pKick->show();	
		d_pJinyan->show();
		d_pDongjie->show();
		d_pDongjieAccount->show();
		d_pChuanSong->show();
		d_pIp->show();
		m_pThisWnd->setSize(Absolute, Size(135,301));
	}
	else
	{
		d_pKick->hide();
		d_pJinyan->hide();
		d_pDongjie->hide();
		d_pDongjieAccount->hide();
		d_pChuanSong->hide();
		d_pIp->hide();
		if (isZhuHou)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 199));
		}
		else if (isShiZu)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 182));
		}
		else
		{
			m_pThisWnd->setSize(Absolute,Size(135, 165));
		}
	}

	d_detailBtn->hide();
	d_viewBtn->show();
	if (d_report != NULL)
	{
		d_report->hide();
	}

	Show();
	RefreshPos(false);
}

void KUiPlayerMenu::setPos(Point pos)
{
	m_pThisWnd->setPosition(Absolute, pos);
}

Rect KUiPlayerMenu::getArea()
{
	return m_pThisWnd->getUnclippedInnerRect();
}

int	KUiPlayerMenu::GetWinHeight()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getAbsoluteHeight();
	}
	return 0;
}

void KUiPlayerMenu::SetReportText(char * text, size_t textLength)
{
	if (text == NULL)
	{
		return;
	}

	memset(d_reportText, 0, sizeof(d_reportText));
	if (textLength <= COMMON_CLIENT_MSG_LEN_256 - 1)
	{
		strncpy(d_reportText, text, textLength);
		d_reportText[COMMON_CLIENT_MSG_LEN_256 - 1] = 0;
	}
	else
	{
		strncpy(d_reportText, text, COMMON_CLIENT_MSG_LEN_256 - 1);
		d_reportText[COMMON_CLIENT_MSG_LEN_256 - 1] = 0;
	}
}

void KUiPlayerMenu::ShowByClickText(char * name, int npcId, bool bPrivateState /* = false */)
{
	if(strlen(name) > MAXSIZE_ROLENAME)
	{
		strncpy(d_playerName, name, MAXSIZE_ROLENAME - 1);
		d_playerName[MAXSIZE_ROLENAME - 1] = 0;
	}
	else
	{
		strcpy(d_playerName, name);
	}

	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId       = enSUTplId_Tong;
	tagSocietyIdx.Layer            = enSULayer_Gens;
	tagSocietyIdx.Operation        = enSUO_AddSubUnit;
	bool isShiZu = false;
	bool isZhuHou = false;

	if (ms_Singleton->d_pInviteShizu &&  g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
        ms_Singleton->d_pInviteShizu->show();
		m_pThisWnd->setSize(Absolute, Size(135, 182));
		isShiZu = true;
	}//endif
	else
	{
		ms_Singleton->d_pInviteShizu->hide();
		m_pThisWnd->setSize(Absolute, Size(135, 165));
		isShiZu = false;
	}

	tagSocietyIdx.Layer            = enSULayer_Tong;

	if (ms_Singleton->d_pInviteZhuhou && g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
		ms_Singleton->d_pInviteZhuhou->show();
		m_pThisWnd->setSize(Absolute, Size(135, 199));
		isZhuHou = true;
	}
	else
	{
		ms_Singleton->d_pInviteZhuhou->hide();
		if (isShiZu)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 182));
		}
		else
		{
			m_pThisWnd->setSize(Absolute, Size(135, 165));
		}
		isZhuHou = false;
	}

	d_playerNpcId  = npcId;
	d_PrivateState = bPrivateState;

	bool bGM = g_pCoreShell->GetGameData(GDI_IS_GM, NULL, NULL) > 0 ? true : false;
	if (bGM)
	{
		d_pKick->show();
		d_pJinyan->show();
		d_pDongjie->show();
		d_pDongjieAccount->show();
		d_pChuanSong->show();
		d_pIp->show();
		m_pThisWnd->setSize(Absolute, Size(135, 301));
	}
	else
	{
		d_pKick->hide();
		d_pJinyan->hide();
		d_pDongjie->hide();
		d_pDongjieAccount->hide();
		d_pChuanSong->hide();
		d_pIp->hide();
		if (isZhuHou)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 199));
		}
		else if (isShiZu)
		{
			m_pThisWnd->setSize(Absolute, Size(135, 182));
		}
		else
		{
			m_pThisWnd->setSize(Absolute, Size(135, 165));
		}
	}

	if (d_report	!= NULL)
	{
		d_report->show();
	};
	d_viewBtn->hide();
	d_detailBtn->show();

	Show();
	RefreshPos(true);
}

void KUiPlayerMenu::RefreshPos(bool isDetail)
{
	if (m_pThisWnd == NULL)
	{
		return;
	}
	int ShowButtonNum = 0;
	for (int i = 0; i < PlayerMenuButtonCount; ++i)
	{
		if (m_ButtonList[i] == NULL)
		{
			continue;
		}
		if (m_ButtonList[i]->isVisible())
		{
			Point pos(10, 6 + ShowButtonNum * 17);
			m_ButtonList[i]->setPosition(Absolute, pos);
			++ShowButtonNum;
		}
		if (isDetail)
		{
			if (i == PlayerMenuView)
			{
				if (d_detailBtn == NULL)
				{
					continue;
				}
				if (d_detailBtn->isVisible())
				{
					Point pos(10, 6 + ShowButtonNum * 17);
					d_detailBtn->setPosition(Absolute, pos);
					++ShowButtonNum;
				}
			}
		}
	}
	m_pThisWnd->setSize(Absolute, Size(135, ShowButtonNum * 17 + 12));
	m_pThisWnd->requestRedraw();
}