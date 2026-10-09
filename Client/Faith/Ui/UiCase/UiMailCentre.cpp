//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/27/2006 19:16
//      File_base        : UiMailCentre
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "UiMailCentre.h"
#include "CoreShell.h"
#include "ChatDataDef.h"
#include "cfs_fs2_savedef.h"
#include "UiItemTip.h"
#include "TLGameObject.h"
#include "UiDragItem.h"
#include "UiSystemMessage.h"
#include "UiChatWindow.h"
#include "TLTree.h"
#include "../UiElem/TLTreeEx.h"

#include "UiErrorMessageBox.h"
#include "../KMessageCentre.h"

#include "UiComMsgBox.h"
#include "../UiConfigManager.h"
#include <sstream>
#include <algorithm>

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;
using namespace CHAT;
using namespace std;

#define MAIL_IMAGESET_NAME				"dh_youjian_tu"
#define MAIL_IMAGE_NEW_WITH_PLUS		"new_plus"
#define MAIL_IMAGE_NEW_WITHOUT_PLUS		"new_null"
//#define MAIL_IMAGE_NEW_NEEDPAY			"new_needpay"
#define MAIL_IMAGE_READ_WITH_PLUS		"read_plus"
#define MAIL_IMAGE_READ_WITHOUT_PLUS	"read_null"


#define MAIL_NEEDPAY_IMAGESET_NAME				"fufeiyoujian"
#define MAIL_IMAGE_NEEDPAY						"full_image"


int KUiMailCentre::d_recvMoney = 0;
int KUiMailCentre::d_payMoney  = 0;
MAIL_PARAM KUiMailCentre::d_tempMail;
//ItemType KUiMailCentre::d_curItem;

const int MAX_MAIL_NUM = 16;
const string EmptyStr = "";

const colour RED_COLOUR = colour(1.0f, 0, 0);
const colour GREEN_COLOUR = colour(0, 1.0f, 0);
const colour BLUE_COLOUR = colour(0, 0, 1.0f);
const colour WHITE_COLOUR = colour(1.0f, 1.0f, 1.0f);

const colour TreeItemMail::DefaultReadColour = 0xFF888888;
/************************************************************************/
/*                                                                      */
/************************************************************************/
TreeItemMail::TreeItemMail( const String& text, uint item_id, void* item_data , bool disabled, bool auto_delete):
	TLTreeItem( text, item_id, item_data, disabled, auto_delete )
{

}

TreeItemMail::~TreeItemMail()
{	

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiMailCentre* KUiWndSingleton<KUiMailCentre>::ms_Singleton	= NULL;

KUiMailCentre::KUiMailCentre( const CEGUI::String& id_name ):
KUiWndSingleton<KUiMailCentre>( id_name )
, d_recvWnd(NULL)
, d_sendWnd(NULL)
, d_pMailTreeScrol(NULL)
, d_pMoney(NULL)
, d_pPayTax(NULL)
, d_sendTextTax(0)
, d_sendItemTax(0)
, d_maxFiendsNum(10)
, d_pSelFriend(NULL)
, d_PopMenu(NULL)
, d_pFriendScrol(NULL)
, d_PopMenuBG(NULL)
, m_btnSelRecent(NULL)
, m_capability(NULL)
, m_btnNextPage(NULL)
, m_btnPrevPage(NULL)
, m_totalPage(1)
, m_curPage(0)
, m_pageShow(NULL)
, m_pRecverNameEditbox(NULL)
{
	d_bCanDrag = false;
	d_bSend = false;
	d_PopupItemList.clear();
	m_hasMoney = false;
	m_hasItem = false;
	d_oriPosMenuYPosition = 0;
	m_lockReceiverNameText = false;
}

KUiMailCentre::~KUiMailCentre()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		TLGameObject* pGO = (TLGameObject*)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
		if( pGO )
		{
			delete pGO->getUserData();
		}
	}

	MailTreeItemList::iterator it = d_vFreeList.begin();
	while ( it != d_vFreeList.end()	)
	{
		if ( *it )
		{
			delete *it;
			*it = NULL;
		}
		++it;
	}
	
	TreeItemList::iterator iter = d_PopupItemList.begin();
	while (iter != d_PopupItemList.end())
	{
		if (*iter)
		{
			delete *iter;
			*iter = NULL;
		}
		iter++;
	}
}

void KUiMailCentre::Init()
{
	if ( ms_Singleton->m_pThisWnd ) 
	{
		ms_Singleton->d_bPayMoney = false;
		TLGameObject* pGO = (TLGameObject*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
		if( pGO )
		{
			KObjAtContRegion* region = new KObjAtContRegion();
			region->eContainer = UOC_EQUIPTMENT;
			region->Region.v = 0;
			pGO->setUserData(region);
		}
		ms_Singleton->d_curMailId = -1;
		ms_Singleton->d_sendWnd = ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail");
		ms_Singleton->d_recvWnd = ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/RecvMail");
		( static_cast<Tree*>(ms_Singleton->d_recvWnd) )->setSortingEnabled(false);

		ms_Singleton->d_pMoney = static_cast<StaticImage*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/moneyicontxt/moneyimg"));
		ms_Singleton->d_pPayTax = static_cast<StaticImage*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/Tax"));
		ms_Singleton->d_pPayTax->hide();

		ms_Singleton->m_capability = static_cast<StaticText*>( ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Capability") );
		
		//翻页按钮
		ms_Singleton->m_btnNextPage = static_cast<TLButton*>( ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/NextPage") );
		ms_Singleton->m_btnNextPage->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::btnNextPage_MouseClick, ms_Singleton));
		ms_Singleton->m_btnPrevPage = static_cast<TLButton*>( ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/PrevPage") );
		ms_Singleton->m_btnPrevPage->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::btnPrevPage_MouseClick, ms_Singleton));

		ms_Singleton->m_pageShow = static_cast<StaticText*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/PageShow"));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiMailCentre::onHandlePayMoney, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiMailCentre::onHandlePostMoney, ms_Singleton));
			
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ClearText")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleClearText, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleCallBackMail, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleSendMail, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleDelMail, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/BackRecvList")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleShowRecv, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendPage")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleShowSend, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/RecvMail")->subscribeEvent(Tree::TR_EventDoubleClick, Event::Subscriber(&KUiMailCentre::handleOpenMail, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Open")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleOpenMail, ms_Singleton));
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon")->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiMailCentre::onLBDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon")->subscribeEvent(TLGameObject::EventMouseMove, Event::Subscriber(&KUiMailCentre::onMouseMove, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon")->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiMailCentre::onMouseLeave, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon")->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiMailCentre::onMouseClickItem, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money")->subscribeEvent(TLGameObject::EventMouseClick, Event::Subscriber(&KUiMailCentre::onMouseClickMoney, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money")->subscribeEvent(TLGameObject::EventMouseMove, Event::Subscriber(&KUiMailCentre::onMouseCheckMoney, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money")->subscribeEvent(TLGameObject::EventMouseLeaves, Event::Subscriber(&KUiMailCentre::onMouseLeaveMoney, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CloseBtn")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiMailCentre::handleClose, ms_Singleton));

		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));

		RadioButton* pRadio0 = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0");
		RadioButton* pRadio1 = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1");
		if ( pRadio0 && pRadio1 )
		{
			pRadio0->hide();
			pRadio1->hide();
		}

		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));
		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));
		
		m_pRecverNameEditbox = static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName"));
		m_pRecverNameEditbox->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiMailCentre::edtTitle_TextChanged, ms_Singleton));
		
		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));
		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/bodyt")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));
		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/bodyy")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));
		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/bodyj")->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiMailCentre::handleKeyDown, ms_Singleton));

		ms_Singleton->d_sendBack = (TLButton*)m_pThisWnd->getChild("TaharezLook/MailCentre/SendBack");
		ms_Singleton->d_sendBack->subscribeEvent(Window::EventClicked, 
			Event::Subscriber(&KUiMailCentre::handleSendBack, ms_Singleton));
		ms_Singleton->d_pMailTreeScrol = static_cast<TLVertScrollbar *>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/TreeScrollbar"));
		ms_Singleton->d_pMailTreeScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiMailCentre::handleTreeScroll, this));

		// 好友下拉列表 
		KIniFile	ini;
		if (ini.Load(MAP_SETTING_FILE))
			ini.GetInteger("Mail", "FriendListNum", 10, &d_maxFiendsNum);
		d_pSelFriend	= static_cast<TLButton*>(d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/SelFriend"));
		m_btnSelRecent = static_cast<TLButton*>(d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/btnSelRecent"));
		d_PopMenuBG		= static_cast<StaticImage*>(d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Popupback"));
		d_PopMenu		= static_cast<TLTreeEx*>(d_PopMenuBG->getChild("TaharezLook/MailCentre/SendReadMail/Popupback/Popup1"));
		d_pFriendScrol	= static_cast<TLVertScrollbar*>(d_PopMenuBG->getChild("TaharezLook/MailCentre/SendReadMail/Popupback/ScrollBar"));
		
		d_pSelFriend->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMailCentre::handleOpenFriendList, this));		
		m_btnSelRecent->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMailCentre::btnSelRecent_MouseClick, this));
		d_PopMenu->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMailCentre::handleSelFriend, this));
		d_pFriendScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiMailCentre::handleFriendScroll, this));
		d_oriPosMenuYPosition = d_PopMenu->getAbsoluteYPosition();
		
		d_PopMenuBG->getChild("TaharezLook/MailCentre/SendReadMail/Popupback/Popup1")->subscribeEvent(
			TLVertScrollbar::EventMouseWheel, 
			Event::Subscriber(&KUiMailCentre::PopMenu_MouseWheel, this));

		d_PopMenuBG->subscribeEvent(
			TLVertScrollbar::EventMouseWheel, 
			Event::Subscriber(&KUiMailCentre::PopMenu_MouseWheel, this));

		
		m_maxMailCount = g_pCoreShell->GetGameData(GDI_GET_MAX_MAILCOUNT, 0, 0);
		g_pCoreShell->GetGameData(GDI_GET_SEND_MAIL_TAX, (unsigned int)&d_sendTextTax, (int)&d_sendItemTax);
	}
}

void KUiMailCentre::Show()
{
	KUiWndSingleton<KUiMailCentre>::Show();

	ms_Singleton->d_recvWnd->show();
	ms_Singleton->d_sendWnd->hide();
	
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->hide();		
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->hide();	
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ClearText")->hide();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ListBoxImg")->show();
	
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/BackRecvList")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendPage")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Open")->show();
	//ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->hide();

	ms_Singleton->d_sendBack->hide();
	ms_Singleton->ShowCapability(0);
	ms_Singleton->GotoPage( 0 );
	//SetCurrentPage( 1 );
	//g_pCoreShell->OperationRequest( GO  I_MAIL_LIST_REQ, NULL, NULL );


	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->hide();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/fujiantxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/moneyicontxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/tubiaokuang1")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicontxt")->hide();
	TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
	pGameObj->clear();
	pGameObj->hide();
	pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
	pGameObj->clear();
	pGameObj->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->hide();
	ms_Singleton->ShowPageItems();
	ms_Singleton->d_pMailTreeScrol->show();
}

void KUiMailCentre::OnRecvMailListRet( BYTE* byParam )
{
	PDBTASK_GETROLEMAILLIST_RET	pDBMailList = (PDBTASK_GETROLEMAILLIST_RET)byParam;
	PDBTASK_MAILLISTITEM		pMailListItem = (PDBTASK_MAILLISTITEM)pDBMailList->pMailList;

	Tree* pTree = static_cast<Tree*>(ms_Singleton->d_recvWnd);
	if ( pTree )
	{
		pTree->removeAllItem();
		int nMailCount = pDBMailList->nMailListCount;

		if ( nMailCount >= MAX_MAIL_NUM )
		{
			nMailCount = MAX_MAIL_NUM;
			//KUiChannelCentre::GetSingleton().toSysMsg(KMessageCentre::GetMessage(mail_message, 13));
		}

		//显示邮箱容量
		ShowCapability(pDBMailList->nMailTotalCount);

		RefreshPageShow();

		for ( int nIdx = 0; nIdx < nMailCount; ++nIdx )
		{
			char str[COMMON_CLIENT_MSG_LEN_64];
			ZeroMemory(str, COMMON_CLIENT_MSG_LEN_64);
			sprintf( str, "%s  (%s)", pMailListItem[nIdx].tagMailInfo.szTile, pMailListItem[nIdx].tagMailInfo.szSenderName );
			TreeItemMail* pItem = new TreeItemMail( AnsiToUtf8(str) );
			//pItem->setAutoDeleted(true);
			pItem->setMailID( pMailListItem[nIdx].unID );
			int dayLeft = pMailListItem[nIdx].unDeadSeconds / (3600 * 24);
			string dayLeftShow;
			//设置剩余天数的文字
			if ( dayLeft < 1 )
			{
				dayLeftShow =  dayLeftShow + "<1" + KMessageCentre::GetMessage(login_error_message, 26);
			}
			else
			{
				dayLeftShow = iToStr(dayLeft) + KMessageCentre::GetMessage(login_error_message, 26);
			}
			
			//设置剩余天数的颜色
			if ( dayLeft <=  KUiCfgLoader::getSingleton().getMailCfg().Red_Day )
			{
				pItem->setTailTextColor(RED_COLOUR);
			}
			else if ( dayLeft <= KUiCfgLoader::getSingleton().getMailCfg().Green_Day )
			{
				pItem->setTailTextColor(GREEN_COLOUR);
			}
			else
			{
				pItem->setTailTextColor(WHITE_COLOUR);
			}	
			
			pTree->setItemTooltipsEnabled(true);
			pItem->setTailText( AnsiToUtf8( dayLeftShow.c_str() ) );
			string toolTip = KMessageCentre::GetMessage(mail_message, 15);
			pItem->setTooltipText( AnsiToUtf8( toolTip.c_str() ) );
			pTree->addItem( pItem );
			//pTree->setTooltipText(AnsiToUtf8("abc"));
			

			const Image* pImage = NULL;
			if (pMailListItem[nIdx].enState == enMailState_Read)
			{
				if ( pMailListItem[nIdx].tagMailInfo.nMailCost > 0 )
				{
					pImage = &ImagesetManager::getSingleton().getImageset(MAIL_NEEDPAY_IMAGESET_NAME)->getImage(MAIL_IMAGE_NEEDPAY);
				}
				else if (pMailListItem[nIdx].tagMailInfo.bHasApp || 
					pMailListItem[nIdx].tagMailInfo.nPostMoney > 0)
				{
					pImage = &ImagesetManager::getSingleton().getImageset(MAIL_IMAGESET_NAME)->getImage(MAIL_IMAGE_READ_WITH_PLUS);
				}
				else
				{
					pImage = &ImagesetManager::getSingleton().getImageset(MAIL_IMAGESET_NAME)->getImage(MAIL_IMAGE_READ_WITHOUT_PLUS);
				}
				pItem->setNormalColor(TreeItemMail::DefaultReadColour);
			}
			else
			{	
				if ( pMailListItem[nIdx].tagMailInfo.nMailCost > 0 )
				{
					pImage = &ImagesetManager::getSingleton().getImageset(MAIL_NEEDPAY_IMAGESET_NAME)->getImage(MAIL_IMAGE_NEEDPAY);
				}
				else if (pMailListItem[nIdx].tagMailInfo.bHasApp || 
					pMailListItem[nIdx].tagMailInfo.nPostMoney > 0)
				{
					pImage = &ImagesetManager::getSingleton().getImageset(MAIL_IMAGESET_NAME)->getImage(MAIL_IMAGE_NEW_WITH_PLUS);
				}
				else
				{
					pImage = &ImagesetManager::getSingleton().getImageset(MAIL_IMAGESET_NAME)->getImage(MAIL_IMAGE_NEW_WITHOUT_PLUS);
				}
			}
			if (pImage)
			{
				pItem->setHeadImage(pImage);
			}
			
			d_vFreeList.push_back(pItem);
		}
	}
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->hide();
}

void KUiMailCentre::OnRecvMailRet( BYTE* byParam )
{
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ClearText")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/NameTxt")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 11)));
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt_img")->hide();
	PCHAT_MAILDATA_CLIENT	pMailData = (PCHAT_MAILDATA_CLIENT)byParam;
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName"))->setText("");
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setText(AnsiToUtf8(pMailData->header.tagMailInfo.szTile));
	static_cast<MultiLineEditbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content"))->setText("");
	
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName"))->setReadOnly(false);
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setReadOnly(false);
	static_cast<MultiLineEditbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content"))->setReadOnly(false);
	//ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText(AnsiToUtf8( pMailData->header.tagMailInfo.szSenderName ));
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText("");
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName"))->setReadOnly(true);
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->setText(AnsiToUtf8( pMailData->header.tagMailInfo.szTile));
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setReadOnly(true);
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->setText(AnsiToUtf8( pMailData->szContent ));
	static_cast<MultiLineEditbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content"))->setReadOnly(true);
	ms_Singleton->d_recvWnd->hide();
	ms_Singleton->d_sendWnd->show();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ListBoxImg")->hide();

	int titlelong = MAXLEN_MAILTITLE+COMMON_CLIENT_MSG_LEN_16;
	char Title[MAXLEN_MAILTITLE+COMMON_CLIENT_MSG_LEN_16];
	ZeroMemory(Title, titlelong);
	if (pMailData->header.tagMailInfo.enSenderType == enMailSenderType_Player)
	{
		ms_Singleton->d_sendBack->show();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->show();
		sprintf(Title, "%s%s", pMailData->header.tagMailInfo.szSenderName, KUiCfgLoader::getSingleton().getChannelData().personalText);
		ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText(AnsiToUtf8(Title));
		static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setNormalTextColour( WHITE_COLOUR );
	}
	else
	{
		ms_Singleton->d_sendBack->hide();
		static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setNormalTextColour( RED_COLOUR );
	}
	

/*
	if ( strcmp( KMessageCentre::GetMessage(mail_message, 4), pMailData->header.tagMailInfo.szSenderName ) != 0
	   && strcmp( KMessageCentre::GetMessage(mail_message, 5), pMailData->header.tagMailInfo.szSenderName ) != 0 )
	{
		ms_Singleton->d_sendBack->show();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->show();
	}
//*/
	int money = 0;	

	//RadioButton* pRadio0 = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0");
	//RadioButton* pRadio1 = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1");
	
	if ( pMailData->header.tagMailInfo.nMailCost > 0 )
	{
		money = pMailData->header.tagMailInfo.nMailCost;
		d_payMoney = money;
		Window* pJin = ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj");
		Window* pYin = ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy");
		Window* pTong = ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt");
		int jin, yin, tong;
		sysMoneyToUiMoney(money, jin, yin, tong);
		pJin->setText(iToString(jin));
		pYin->setText(iToString(yin));
		pTong->setText(iToString(tong));
		ms_Singleton->d_bPayMoney = true;
/*
		if ( pRadio0 && pRadio1 )
		{
			pRadio0->setSelected( true );
			pRadio1->setSelected( false );
			pRadio0->setEnabled( false );
			pRadio1->setEnabled( false );
		}
//*/
		TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
		pGameObj->hide();
		ms_Singleton->d_pMoney->hide();
	}
	else
	{
		ms_Singleton->d_bPayMoney = false;
		d_payMoney = 0;
/*
		if ( pRadio0 && pRadio1 )
		{
			pRadio0->setSelected( false );
			pRadio1->setSelected( true );
			pRadio0->setEnabled( false );
			pRadio1->setEnabled( false );
		}
//*/
		if ( pMailData->header.tagMailInfo.nPostMoney > 0 )
		{
			d_recvMoney = pMailData->header.tagMailInfo.nPostMoney;
			TLGameObject::GameObject tagGO;
			tagGO.d_type = TLGameObject::item;		
			tagGO.d_gameobjectSet = AnsiToUtf8( "GameObject" );
			tagGO.d_gameobject	= AnsiToUtf8( "money" );
			//tagGO.d_count		= pMailData->header.tagMailInfo.nPostMoney;

			TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
			pGameObj->setObject( tagGO );
			pGameObj->show();
			ms_Singleton->d_pMoney->show();
			ms_Singleton->m_hasMoney = true;
		}
		else
		{
			ms_Singleton->d_pMoney->hide();
			ms_Singleton->m_hasMoney = false;
		}
	}
/*
	if ( pRadio0 && pRadio1 )
	{
		pRadio0->show();
		pRadio1->show();
	}
//*/
	KItemInfo tagItemInfo;
	ZeroMemory( &tagItemInfo, sizeof( KItemInfo ) );
	FIND_ITEMINDEX_PARAM tagItemIdx;
	ZeroMemory( &tagItemIdx, sizeof(FIND_ITEMINDEX_PARAM) );
	tagItemIdx.nGenre = pMailData->plusData[0].data.igenre;
	tagItemIdx.nDetail	= pMailData->plusData[0].data.idetailtype;
	tagItemIdx.nParticular	= pMailData->plusData[0].data.iparticulartype;
	tagItemIdx.nLevel	= pMailData->plusData[0].data.ilevel;

	TLGameObject* pGameObj = NULL;
	if ( tagItemIdx.nGenre == 0 && tagItemIdx.nDetail == 0 && tagItemIdx.nParticular == 0 && tagItemIdx.nLevel == 0 )
	{
		pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
		pGameObj->clear();
		pGameObj->hide();
		ms_Singleton->m_hasItem = false;
	}
	else
	{		
		char layoutText[LAYOUT_TEXT_MAX_LEN];
		ZeroMemory(layoutText, LAYOUT_TEXT_MAX_LEN);
		sprintf(layoutText, "<Layout width=%d margin-top=%d margin-left=%d margin-right=%d margin-bottom=%d>", 
			KUiCfgLoader::getSingleton().getTipData().windowWidth,
			KUiCfgLoader::getSingleton().getTipData().topMargin,
			KUiCfgLoader::getSingleton().getTipData().leftMargin,
			KUiCfgLoader::getSingleton().getTipData().RightMargin,
			KUiCfgLoader::getSingleton().getTipData().bottomMargin);

		int nRet = g_pCoreShell->GetGameData(GDI_VENDUE_ITEM_LAYOUT_DESC, 
			(unsigned int)layoutText, (int)&pMailData->plusData[0].data);
		strcat(layoutText, "</Layout>");
		nRet &= g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&tagItemIdx, (int)&tagItemInfo);
		
		pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild(
				"TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
		if ( nRet && tagItemInfo.szImageSet[0] && tagItemInfo.szImage[0]  )
		{
			TLGameObject::GameObject tagGO;
			tagGO.d_type = TLGameObject::item;		
			tagGO.d_gameobjectSet = AnsiToUtf8( tagItemInfo.szImageSet );
			tagGO.d_gameobject	= AnsiToUtf8( tagItemInfo.szImage );
			tagGO.d_count		= pMailData->plusData[0].data.nItemCount;
			tagGO.d_EdgeframeIdx = tagItemInfo.colour;

			pGameObj->setObject( tagGO );
			pGameObj->show();
			pGameObj->setTooltipText(AnsiToUtf8(layoutText));
			ms_Singleton->m_hasItem = true;
		}
		else
		{
			pGameObj->clear();
			pGameObj->hide();
			ms_Singleton->m_hasItem = false;
		}

	}
/*
	Window* moneyTitle = ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/bodytxt");
	if ( ms_Singleton->d_bPayMoney )
	{
		moneyTitle->getChild("TaharezLook/MailCentre/bodytxt/money")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 6)));
	}
	else
	{	
		moneyTitle->getChild("TaharezLook/MailCentre/bodytxt/money")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 7)));
	}
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->hide();
//*/
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/fujiantxt")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/moneyicontxt")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/tubiaokuang1")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicontxt")->show();
}

void KUiMailCentre::OnDelMailRet( unsigned int uParam )
{
	if ( uParam > 0 )
	{
		ms_Singleton->GotoPage( 0 );
		//g_pCoreShell->OperationRequest( G  OI_MAIL_LIST_REQ, NULL, NULL );
		ms_Singleton->d_recvWnd->show();
		ms_Singleton->d_sendWnd->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->hide();
		//ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->show();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj_img")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy_img")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt_img")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money")->show();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1")->hide();
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/BackRecvList")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendPage")->show();
		ms_Singleton->ShowPageItems();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Open")->show();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->hide();
		ms_Singleton->d_sendBack->hide();
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ListBoxImg")->show();
		
		TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
		pGameObj->clear();
		pGameObj->show();
		pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
		pGameObj->clear();
		pGameObj->show();
		
		Tree* pTree = static_cast<Tree*>(ms_Singleton->d_recvWnd);
		if ( pTree )
		{
			pTree->removeAllItem();
		}//endif
	}
}

void KUiMailCentre::OnSendMailRet( BYTE* byParam )
{
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->setText("0");
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->setText("0");
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->setText("0");
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText("");
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->setText("");
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->setText("");
	TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
	pGameObj->clear();
	pGameObj->show();
	pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
	pGameObj->clear();
	pGameObj->show();

	ms_Singleton->UpdateSendTax();
}

bool	KUiMailCentre::handleShowRecv( const CEGUI::EventArgs& args )
{
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/BackRecvList")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendPage")->show();
	ShowPageItems();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Open")->show();
	//ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->hide();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/RecvMail")->show();
	d_pMailTreeScrol->show();
	d_bSend = false;
	d_bCanDrag = false;
	d_recvWnd->show();
	d_sendWnd->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ClearText")->hide();
	d_sendBack->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->hide();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/NameTxt")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 11)));
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt_img")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/fujiantxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/moneyicontxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/tubiaokuang1")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicontxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ListBoxImg")->show();
	TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
	pGameObj->clear();
	pGameObj->show();
	pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
	pGameObj->clear();
	pGameObj->show();

	GotoPage( 0 );
	//g_pCoreShell->OperationRequest( G  OI_MAIL_LIST_REQ, NULL, NULL );

	d_pPayTax->hide();
	d_pSelFriend->hide();
	m_btnSelRecent->hide();
	HideFriendsList();

	return true;
}

bool	KUiMailCentre::handleShowSend( const CEGUI::EventArgs& args )
{
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/BackRecvList")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendPage")->hide();
	ShowPageItems(false);
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Open")->hide();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/RecvMail")->hide();
	d_pMailTreeScrol->hide();
	d_bSend = true;
	d_payMoney = 0;
	d_bCanDrag = true;
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/NameTxt")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 12)));
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/fujiantxt")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/moneyicontxt")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/SendReadMail/tubiaokuang1")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicontxt")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/CallBack")->hide();	
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ClearText")->show();
	ms_Singleton->d_pMoney->hide();
	
	RadioButton* pRadioButtom = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0");
	d_bPayMoney = pRadioButtom->isSelected();

	ChangeMoneyTile();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->setText("");
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->setText("");
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->setText("");
	RadioButton* pRadio0 = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0");
	RadioButton* pRadio1 = (RadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1");
	pRadio0->setEnabled( true );
	pRadio1->setEnabled( true );
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText("");
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->setText("");
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->setText("");
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName"))->setReadOnly(false);
	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setReadOnly(false);
	static_cast<MultiLineEditbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content"))->setReadOnly(false);
	d_recvWnd->hide();
	d_sendWnd->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Send")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->hide();
	d_sendBack->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj_img")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy_img")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt_img")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/ListBoxImg")->hide();

	if ( pRadio0 && pRadio1 )
	{
		pRadio0->show();
		pRadio1->show();
	}

	static_cast<Editbox*>(ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title"))->setNormalTextColour( WHITE_COLOUR );

	TLGameObject* pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
	pGameObj->clear();
	pGameObj->hide();
	pGameObj = (TLGameObject *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
	pGameObj->clear();
	pGameObj->show();

	UpdateSendTax();
	d_pPayTax->show();
	d_pSelFriend->show();
	m_btnSelRecent->show();
	HideFriendsList();

	return true;
}

bool KUiMailCentre::handleOpenMail(const CEGUI::EventArgs& args )
{
	Tree* pTree = static_cast<Tree*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));

	if(!pTree)
	{
		return true;
	}
	
	TreeItemMail* pItem = static_cast<TreeItemMail*>(pTree->getFirstSelectedItem());
	if(!pItem)
	{
		return true;
	}

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendPage")->hide();
	ShowPageItems(false);
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/BackRecvList")->show();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Open")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/Del")->show();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/RecvMail")->hide();
	d_pMailTreeScrol->hide();
	d_bSend = false;
	
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose0")->hide();
	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/Choose1")->hide();
	
	g_pCoreShell->OperationRequest( GOI_MAIL_REQ, pItem->getMailID(), NULL );
	d_curMailId = pItem->getMailID();
	
	d_pPayTax->hide();
	d_pSelFriend->hide();
	m_btnSelRecent->hide();
	HideFriendsList();

	return true;
}

bool	KUiMailCentre::handleSendMail( const CEGUI::EventArgs& args )
{
	MAIL_PARAM tagMail;
	ZeroMemory( &tagMail, sizeof(MAIL_PARAM) );
	String strRecvName		= d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->getText();
	String strTitle			= d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->getText();
	String strSendername	= d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->getText();
	String strContent		= d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->getText();
	strncpy( tagMail.strReceiver, Utf8ToAnsi( strRecvName ), CLIENT_NAME_AND_TITLE_MAX );
	strncpy( tagMail.szTitle, Utf8ToAnsi( strTitle ), MAXLEN_MAILTITLE );
	strncpy( tagMail.szContent, Utf8ToAnsi( strContent ), MAXSIZE_MAILTEXT );
	tagMail.nContentLen		= strlen( tagMail.szContent ); 

	if ( strRecvName == "" || strTitle == "" )
	{
		KUiChannelCentre::GetSingleton().toSysMsg(KMessageCentre::GetMessage(mail_message, 1));
		return false;
	}

	int j, y, t;
	j = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->getText() ) );
	y = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->getText() ) );
	t = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->getText() ) );
	int nMoney = uiMoneyToSysMoney( j, y, t );

	TLGameObject* pGO = (TLGameObject*)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/itemicon" );
	if ( pGO && pGO->getGameObjectType() == TLGameObject::item )
	{
 		KObjAtContRegion* itemRegion = (KObjAtContRegion*)pGO->getUserData();
//		tagMail.pPlusData		= d_pPlusData;
		tagMail.pPlusData[0]	= itemRegion->Obj.uId;
		tagMail.nItemCount		= 1;
	}

	if ( d_bPayMoney )
	{
		if ( !pGO || pGO->getGameObjectType() != TLGameObject::item || nMoney <= 0 )
		{
			KUiChannelCentre::GetSingleton().toSysMsg(KMessageCentre::GetMessage(mail_message, 2));
			return false;
		}

		tagMail.postMoney = 0;
		tagMail.costMoney = nMoney;
		if ( tagMail.nItemCount > 0 && tagMail.pPlusData[0] > 0 )
		{
			g_pCoreShell->OperationRequest( GOI_SEND_MAIL, (unsigned int)&tagMail, NULL );
			UpdateSendTax();
		}
	}
	else
	{
		tagMail.postMoney = nMoney;
		tagMail.costMoney = 0;
		if ( tagMail.postMoney > 0 )
		{
			ZeroMemory( &d_tempMail, sizeof(MAIL_PARAM) );
			memcpy(&d_tempMail, &tagMail, sizeof(MAIL_PARAM));

			char postMoneyMsg[COMMON_CLIENT_MSG_LEN_512];
			ZeroMemory(postMoneyMsg, COMMON_CLIENT_MSG_LEN_512);
			sprintf(postMoneyMsg, KUiCfgLoader::getSingleton().getCommonCfg().postMoneyMsg, j, y, t, tagMail.strReceiver);
			
			KUiComMsgBox::GetSingleton().setModalStatus(true);
			KUiComMsgBox::GetSingleton().setComMsgPosition();
			KUiComMsgBox::Show();
			//KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(postMoneyMsg));
			KUiComMsgBox::GetSingleton().setLayoutMsg( postMoneyMsg );
			KUiComMsgBox::GetSingleton().setFristBtnCallback(processPostMoney);
			char yesString[COMMON_CLIENT_MSG_LEN_8];
			char noString[COMMON_CLIENT_MSG_LEN_8];
			strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
			strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
			KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);

			return true;
		}
		else
		{
			g_pCoreShell->OperationRequest( GOI_SEND_MAIL, (unsigned int)&tagMail, NULL );
			UpdateSendTax();
		}
	}
	Editbox *pRecverName	= (Editbox *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/RecverName" );
	Editbox *pTitle			= (Editbox *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/Title" );
	Editbox *pContent		= (Editbox *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/Content" );
	
	if ( pRecverName && pTitle && pTitle )
	{	 
		//pRecverName->setText("");
		pTitle->setText("");		
		pContent->setText("");	
	}

	m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->setText("0");
	m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->setText("0");
	m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->setText("0");

	TLGameObject* pGOItem = (TLGameObject*)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/itemicon" );
	if ( pGOItem )
	{
		pGOItem->clear();
		pGOItem->show();
	}
	TLGameObject* pGOMoney = (TLGameObject*)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/money" );
	if ( pGOMoney )
	{
		pGOMoney->clear();
		pGOMoney->show();
	}

	return true;
}

bool	KUiMailCentre::handleDelMail( const CEGUI::EventArgs& args )
{
	Tree* pTree = static_cast<Tree*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));
	if ( pTree )
	{
		TreeItemMail* pItem = static_cast<TreeItemMail*>(pTree->getFirstSelectedItem());
		if ( pItem )
		{
			//首先确认邮件中是否有附件和金钱
			if ( m_hasMoney  || m_hasItem )
			{
				KUiErrorMessageBox::GetSingleton().AddMessage(
					AnsiToUtf8( KMessageCentre::GetMessage( mail_message, 14 ) ) );
				return false;
			}

			// 确认删除面板
			char deleteMailMsg[COMMON_CLIENT_MSG_LEN_64];
			sprintf(deleteMailMsg, KUiCfgLoader::getSingleton().getCommonCfg().deleteMailMsg);

			KUiComMsgBox::GetSingleton().setModalStatus(true);
			KUiComMsgBox::GetSingleton().setComMsgPosition();
			KUiComMsgBox::Show();
			KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(deleteMailMsg));
			KUiComMsgBox::GetSingleton().setFristBtnCallback(processDeleteMail);
			char yesString[COMMON_CLIENT_MSG_LEN_8];
			char noString[COMMON_CLIENT_MSG_LEN_8];
			strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
			strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
			KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
			
		}
	}
	
	return true;
}


bool	KUiMailCentre::handleCallBackMail( const CEGUI::EventArgs& args )
{
	//去掉回复时名字后面的“非官方”字样
	std::string postFix_PersonalText = KUiCfgLoader::getSingleton().getChannelData().personalText;
	std::string oriName = Utf8ToAnsi( ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->getText() );
	string::size_type findPos = oriName.rfind( postFix_PersonalText );
	if ( string::npos != findPos )
	{
		string::size_type eraseCount = postFix_PersonalText.length();
		oriName = oriName.erase( findPos, eraseCount );
	}

	//String name = ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->getText();
	String name = AnsiToUtf8( oriName.c_str() );
	String title = ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->getText();
	title = "Re:" + title;
	handleShowSend( args );
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText(name);
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Title")->setText(title);
	return true;
}

bool	KUiMailCentre::handleClearText( const CEGUI::EventArgs& args )
{
	ms_Singleton->d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/Content")->setText("");
	return true;
}

bool	KUiMailCentre::onMouseMove(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLGameObject* destObj = (TLGameObject*)arg->window;
	Window* pBar = destObj->getParent();
	if ( pBar )
	{
		TItemtransfersData* pData = NULL;
		if ( pData == NULL )
		{
			return false;
		}
		Point pos = m_pThisWnd->getPosition(Absolute) + Point(destObj->getWidth(Absolute), 0) + destObj->getPosition(Absolute);
//		KUiItemTip::Show(pData, pos, destObj->getWidth(Absolute));
	}

	return true;
}

bool	KUiMailCentre::onMouseLeave(const CEGUI::EventArgs& e)
{
//	KUiItemTip::Hide();
	return true;
}

bool	KUiMailCentre::onLBDown(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton || d_bSend == false)
		return false;

	TLGameObject *pGO = static_cast<TLGameObject*>(arg->window);
	if ( pGO )
	{
		TLGameObject* destObj,* sourObj;
		TLGameObject::GameObject destObjInfo, sourObjInfo;
		destObj = pGO;
		destObj->getObject(destObjInfo);

		sourObj = KUiDragItem::GetSingleton().getObj();
		KObjAtContRegion* pItem = (KObjAtContRegion*)sourObj->getUserData();
		sourObj->getObject(sourObjInfo);
		sourObj->clear();

		if(sourObjInfo.d_type != TLGameObject::idle)
		{
			// 邮件中附件的TIP
			KItemInfo tagItemInfo;
			g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, pItem->Obj.uId );
			if ( tagItemInfo.bIsBind )
			{
				char layoutText[COMMON_CLIENT_MSG_LEN_1024];
				sprintf(layoutText, "<Seg float=wrap><Obj type=text c=ffff0000>[%s]%s</Obj></Seg>", CHAT_CHANNEL_NAME_SYSTEM, ITEM_CANNT_MAIL);
				KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID, layoutText);
				KUiDragItem::GetSingleton().initItem();
			}
			else
			{
				pGO->clear();
				sourObjInfo.d_EdgeframeIdx = tagItemInfo.colour;
				pGO->setObject( sourObjInfo );
				pGO->setTooltipText( AnsiToUtf8( tagItemInfo.szToolTip ) );
				*((KObjAtContRegion*)(pGO->getUserData())) = *((KObjAtContRegion*)(sourObj->getUserData()));
				pGO->setType( TLGameObject::item );

				UpdateSendTax(true);
			}

		}
		else
		{
			if(destObjInfo.d_type == TLGameObject::item && d_bCanDrag)
			{
				sourObj->setObject(destObjInfo);
				sourObj->setCanDrag(true);
			}
			if( d_payMoney == 0 )
			{
				pGO->clear();
				pGO->show();
			}
			UpdateSendTax();
		}	
		if ( KUiItemTip::IsVisible() )
		KUiItemTip::Hide();
	}
	return true;
}

bool	KUiMailCentre::onMouseClickMoney(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton)
		return false;
	g_pCoreShell->OperationRequest( GOI_GET_MONEY, d_curMailId, NULL );
	
	if ( ms_Singleton->d_pMoney->isVisible() )
		ms_Singleton->d_pMoney->hide();
	static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money"))->clear();

	m_hasMoney = false;

	return true;
}

bool	KUiMailCentre::onMouseCheckMoney(const CEGUI::EventArgs& e)
{
	MouseEventArgs* event = (MouseEventArgs*)(&e);

	TLGameObject* Obj = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money"));
	if ( !Obj->isEmpty() )
	{	
		char szMoney[COMMON_CLIENT_MSG_LEN_512];
		ZeroMemory(szMoney, COMMON_CLIENT_MSG_LEN_512);
		
		int jin, yin, tong;
		sysMoneyToUiMoney(d_recvMoney, jin, yin, tong);

		sprintf( szMoney, KMessageCentre::GetMessage(mail_message, 10), jin, yin, tong );

		KUiItemTip::GetSingleton();
		KUiItemTip::GetSingleton().show( szMoney, event->window->getUnclippedInnerRect(), KUiItemTip::Top);
	}

	return true;
}

bool	KUiMailCentre::onMouseLeaveMoney(const CEGUI::EventArgs& e)
{
	MouseEventArgs* event = (MouseEventArgs*)(&e);
	KUiItemTip::Hide();
	return true;
}

bool	KUiMailCentre::onMouseClickItem(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton || d_bSend == true)
		return false;
	
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon"));
	if ( pGO->isEmpty() )
		return false;

	//判断是否背包格子够（服务器有判断，现在只做简单检测）
	//if ( g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BAG_IS_FULL, (unsigned int)&d_curItem, pGO->getCount()) )
	if ( g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BAG_IS_FULL, NULL, NULL) )
	{
		char *szMsg = KMessageCentre::GetMessage( common_message, CE_Bag_Full );
		if ( szMsg )
		{
			KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
			return false;
		}
	}

	// 添加确认(金钱数)
	Tree* pTree = static_cast<Tree*>(ms_Singleton->m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));
	if ( pTree && d_payMoney == 0 )
	{
		g_pCoreShell->OperationRequest( GOI_GET_ITEM, d_curMailId, NULL );
		RecvObj();
	}
	else if ( pTree )
	{
		// 确认付款面板
		char payMoneyMsg[COMMON_CLIENT_MSG_LEN_512];	
		String name = d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->getText();
		int money = KUiMailCentre::GetSingleton().GetPayMoney();
		int jin, yin, tong;
		sysMoneyToUiMoney(money, jin, yin, tong);
		sprintf(payMoneyMsg, KUiCfgLoader::getSingleton().getCommonCfg().payMoneyMsg, jin, yin, tong );

		KUiComMsgBox::GetSingleton().setModalStatus(true);
		KUiComMsgBox::GetSingleton().setComMsgPosition();
		KUiComMsgBox::Show();
		//KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(payMoneyMsg));
		KUiComMsgBox::GetSingleton().setLayoutMsg(payMoneyMsg);
		KUiComMsgBox::GetSingleton().setFristBtnCallback(processPayMoney);
		char yesString[COMMON_CLIENT_MSG_LEN_8];
		char noString[COMMON_CLIENT_MSG_LEN_8];
		strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
		strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
		KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
	}
	if ( KUiItemTip::IsVisible() )
		KUiItemTip::Hide();
	KUiDragItem::GetSingleton().initItem();

	return true;
}

void	KUiMailCentre::OnGetPlusItem( int nRet )
{
	if ( nRet )
	{
		TLGameObject* pGameObj = (TLGameObject *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/money");
		pGameObj->clear();
		pGameObj->show();
	}
	else
	{
		TLGameObject* pGameObj = (TLGameObject *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon");
		pGameObj->clear();
		pGameObj->show();
	}
}

bool	KUiMailCentre::onHandlePayMoney(const CEGUI::EventArgs& e)
{
	d_bPayMoney = true;
	ChangeMoneyTile();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->show();
	return true;		 
}

bool	KUiMailCentre::onHandlePostMoney(const CEGUI::EventArgs& e)
{
	d_bPayMoney = false;
	ChangeMoneyTile();

	ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodytxt")->show();

	return true;
}

bool	KUiMailCentre::handleClose( const CEGUI::EventArgs& args )
{
	KUiMailCentre::Hide();
	
	Tree* pTree = static_cast<Tree*>(ms_Singleton->d_recvWnd);
	if ( pTree )
	{
		pTree->removeAllItem();
	}
	
	return true;
}

bool	KUiMailCentre::handleKeyDown( const CEGUI::EventArgs& args	)
{
   using namespace CEGUI;

    switch (static_cast<const KeyEventArgs&>(args).scancode)
    {
	case Key::P:
	case Key::F1:
	case Key::F2:
	case Key::F3:	
	case Key::F4:
	case Key::F5:
	case Key::F6:	
	case Key::F7:
	case Key::F8:
	case Key::F9:	
	case Key::F10:
	case Key::F11:
	case Key::F12:
	case Key::Escape:
		return false;
    case Key::Tab:
		{
			Editbox *pRecverName	= (Editbox *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/RecverName" );
			Editbox *pTitle	= (Editbox *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/Title" );
			Editbox *pContent	= (Editbox *)m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/Content" );
			
			if ( pRecverName && pTitle && pContent )
			{
				if ( pRecverName->hasInputFocus() )
				{
					pTitle->activate();
					pTitle->captureInput();
				}
				else if ( pTitle->hasInputFocus() )
				{
					pContent->activate();
					pContent->captureInput();
				}
				else
				{
					pRecverName->activate();
					pRecverName->captureInput();
				}
			}
			return true;
		}
        break;
     default:
        return true;
    }
	return true;
}

bool	KUiMailCentre::handleSendBack( const CEGUI::EventArgs& args	)
{	
	/*Tree* pTree = static_cast<Tree*>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));
	if ( pTree )
	{
		TreeItemMail* pItem = static_cast<TreeItemMail*>(pTree->getFirstSelectedItem());
		if ( pItem )
		{
			g_pCoreShell->OperationRequest( GOI_SEND_BACK_MAIL, (unsigned int)pItem->getMailID(), NULL );
		}
	}//*/
	char SendBackMailMsg[COMMON_CLIENT_MSG_LEN_64];
	ZeroMemory(SendBackMailMsg, COMMON_CLIENT_MSG_LEN_64);
	sprintf(SendBackMailMsg, KUiCfgLoader::getSingleton().getCommonCfg().sendBackMailMsg);
	
	KUiComMsgBox::GetSingleton().setModalStatus(true);
	KUiComMsgBox::GetSingleton().setComMsgPosition();
	KUiComMsgBox::Show();
	KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(SendBackMailMsg));
	KUiComMsgBox::GetSingleton().setFristBtnCallback(processSendBackMail);
	char yesString[COMMON_CLIENT_MSG_LEN_8];
	char noString[COMMON_CLIENT_MSG_LEN_8];
	strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
	strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
	KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
	
	return true;
}

void	KUiMailCentre::HideFriendsList()
{
	d_PopMenuBG->hide();
}

bool	KUiMailCentre::handleOpenFriendList( const CEGUI::EventArgs& args )
{
	RefreshFriendList(EmptyStr);
	return true;
}

bool KUiMailCentre::btnSelRecent_MouseClick( const CEGUI::EventArgs& args )
{
	RefreshRecentList(EmptyStr);
	return true;
}

bool	KUiMailCentre::handleSelFriend( const CEGUI::EventArgs& args )
{
	TreeItemEx* pItem = static_cast<TreeItemEx*>(d_PopMenu->getFirstSelectedItem());

	if (pItem)
	{
		HideFriendsList();
		m_lockReceiverNameText = true;
		d_sendWnd->getChild("TaharezLook/MailCentre/SendReadMail/RecverName")->setText(pItem->getText());
		m_lockReceiverNameText = false;
	}

	return true;
}

bool	KUiMailCentre::handleFriendScroll(const CEGUI::EventArgs& args)
{
	if (d_PopMenu)
	{
		WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
		float sparef = d_PopMenu->getTreeTotalItemsHeigh();
		float clipper = d_PopMenuBG->getHeight(Absolute);
		float yPos = 0;
		if (scrollCtrl->window == d_pFriendScrol)
		{
			float scrollPos = d_pFriendScrol->getScrollPosition();

			if ((sparef > clipper))
			{
				yPos = (sparef - clipper) * scrollPos;
				Point pos;
				pos.d_x = d_PopMenu->getPosition(Absolute).d_x;
				pos.d_y = 0 - yPos;
				d_PopMenu->setPosition( Absolute, pos );
			}
			else
			{
				d_PopMenu->setPosition( Absolute, Point(d_PopMenu->getPosition(Absolute).d_x, d_PopMenu->getPosition(Absolute).d_y) );
			}
		}
	}

	return true;
}

void	KUiMailCentre::RecvObj()
{
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/itemicon"));
	int money = g_pCoreShell->GetGameData( GDI_PLAYER_HOLD_MONEY, 0, 0 );
	
	if ( !pGO->isEmpty() && (d_payMoney <= money) )
	{
		pGO->clear();
		pGO->show();
		d_payMoney = 0;
		m_hasItem = false;
	}
}
/*
bool	KUiMailCentre::handleShown( const CEGUI::EventArgs& args )
{
	((WindowEventArgs*)&args)->window->beginUpdate();
	return true;
}

bool	KUiMailCentre::handleHidden( const CEGUI::EventArgs& args )
{
	((WindowEventArgs*)&args)->window->stopUpdate();
	return true;
}
//*/
bool	KUiMailCentre::handleTreeScroll( const CEGUI::EventArgs& args )
{
	/*TLTree* pTree = static_cast<TLTree*>(ms_Singleton->d_recvWnd);
	if ( pTree )
	{
		WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
		float sparef = pTree->getTreeTotalItemsHeigh();
		float clipper = pTree->getHeight(Absolute);
		float yPos = 0;
		if (scrollCtrl->window == d_pMailTreeScrol)
		{	
			float scrollPos = d_pMailTreeScrol->getScrollPosition();

			if ( ( sparef > clipper ) )
			{
				yPos = (sparef - clipper) * scrollPos;
				Point pos;
				pos.d_x = pTree->getPosition(Absolute).d_x;
				pos.d_y = 0 - yPos;
				pTree->setPosition( Absolute, pos );
			}
			else
			{
				pTree->setPosition( Absolute, Point(pTree->getPosition(Absolute).d_x, pTree->getPosition(Absolute).d_y) );
			}
		}

		return false;
	}//*/

	return true;
}

void	KUiMailCentre::ChangeMoneyTile()
{
	Window* moneyTitle = d_sendWnd->getChild("TaharezLook/MailCentre/bodytxt");
	if ( d_bPayMoney )
	{
		moneyTitle->getChild("TaharezLook/MailCentre/bodytxt/money")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 8)));
	}
	else
	{	
		moneyTitle->getChild("TaharezLook/MailCentre/bodytxt/money")->setText(AnsiToUtf8(KMessageCentre::GetMessage(mail_message, 9)));
	}
}

void	KUiMailCentre::UpdateSendTax(bool bHasItem)
{
	StaticText* pTax = static_cast<StaticText*>(d_pPayTax->getChild("TaharezLook/MailCentre/Tax/TaxTaxYin"));
	pTax->setTextColours(colour(1.0f, 1.0f, 1.0f, 1.0f));
	int playerMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, NULL, NULL);

	int jing, yin, tong;
	if (bHasItem)
	{
		if (playerMoney < d_sendItemTax)
		{
			pTax->setTextColours(colour(1.0f, 0, 0, 1.0f));
		}
		sysMoneyToUiMoney(d_sendItemTax, jing, yin, tong);
	}
	else
	{
		if (playerMoney < d_sendTextTax)
		{
			pTax->setTextColours(colour(1.0f, 0, 0, 1.0f));
		}
		sysMoneyToUiMoney(d_sendTextTax, jing, yin, tong);
	}
	pTax->setText(iToString(yin));
}

void KUiMailCentre::CleanNameList()
{
	d_PopMenu->removeAllItem();
	TreeItemList::iterator it = d_PopupItemList.begin();
	while (it != d_PopupItemList.end())
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
		it++;
	}
}

void KUiMailCentre::ShowNameList( int itemHeight )
{
	d_PopMenu->scratchWindowDueItems();
    d_PopMenu->setZLevel(Window::SuperTop);
	d_PopMenu->show();
	
	if (d_PopMenu->getItemCount() == 0)
	{
		d_PopMenuBG->hide();
	}
	else
	{
		if (d_PopMenu->getItemCount() <= d_maxFiendsNum)
		{
			d_pFriendScrol->hide();
			d_PopMenuBG->setHeight(Absolute, d_PopMenu->getHeight(Absolute));
		}
		else
		{
			float height = d_maxFiendsNum * itemHeight;
			
			d_pFriendScrol->setPosition(Absolute, Point(d_PopMenuBG->getAbsoluteWidth() - \
				d_pFriendScrol->getAbsoluteWidth(), 2));
			d_pFriendScrol->setHeight(Absolute, height);
			d_pFriendScrol->setStepSize((float)(d_PopMenu->getTreeTotalItemsHeigh() - height) / height);
			d_pFriendScrol->show();
			
			d_PopMenuBG->setHeight(Absolute, height);
			// 		d_PopMenuBG->setZLevel(Window::Top);
			// 		d_PopMenuBG->show();
		}
		
		
		//d_PopMenu->setPosition( Absolute, Point(d_PopMenu->getPosition(Absolute).d_x, d_PopMenu->getPosition(Absolute).d_y) );
		d_PopMenu->setPosition( Absolute, Point( d_PopMenu->getPosition(Absolute).d_x, d_oriPosMenuYPosition ) );
		d_pFriendScrol->setScrollPosition( 0 );
		d_PopMenuBG->setZLevel(Window::Top);
		d_PopMenuBG->show();
	}
}

int KUiMailCentre::FillFriendList( const String& nameFilter )
{
	DWORD	groupIDList[MAX_FRIENDGROUP_COUNT];
	char	groupNameList[MAX_FRIENDGROUP_COUNT][CLIENT_NAME_AND_TITLE_MAX + 1];
	
	int nGroupCount = g_pCoreShell->GetGameData( GDI_CHAT_GROUP_INFO, (unsigned int)groupIDList, (int)groupNameList );
	int itemHeight = 0;
	for (int nGroupIdx = 0; nGroupIdx < nGroupCount; ++nGroupIdx)
	{
		if ( groupIDList[nGroupIdx] != (int)GROUPID_BLACK &&
			groupIDList[nGroupIdx] != (int)GROUPID_TEMP &&
			groupIDList[nGroupIdx] != (int)GROUPID_ENEMY )
		{
			UI_CHAT_OBJINFO  friendNameList[COMMON_CLIENT_MSG_LEN_256]; //
			int nFriendInGroupCount = g_pCoreShell->GetGameData( GDI_CHAT_FRIENDS_IN_A_GROUP, (unsigned int)groupIDList[nGroupIdx], (int)friendNameList );
			
			if ( nFriendInGroupCount > COMMON_CLIENT_MSG_LEN_256)
			{
				nFriendInGroupCount = COMMON_CLIENT_MSG_LEN_256;
			}//endif
			
			for ( int nFriendIdx = 0; nFriendIdx < nFriendInGroupCount; ++nFriendIdx )
			{
				String friendName = AnsiToUtf8(((UI_CHAT_OBJINFO)friendNameList[nFriendIdx]).szName);
				bool needFilter = false;

				//大小写不敏感
				String source = friendName;
				String dest = nameFilter;
				transform(source.begin(), source.end(), source.begin(), tolower);
				transform(dest.begin(), dest.end(), dest.begin(), tolower);
				//if ( 1 == nameFilter.size() )
				//{
					//filter如果包含一个字符串，择利用该字符串进行过滤
					if ( String::npos == source.find( dest ) )
					{
						needFilter = true;
					}
				//}

				if ( !needFilter )
				{
					TreeItemEx *Friend = new TreeItemEx( friendName );
					itemHeight = Friend->getPixelSize().d_height;
					d_PopMenu->addItem( Friend );
					d_PopupItemList.push_back(Friend);
				}
			}//end for nFriendIdx
		}
	}

	return itemHeight;
}

void KUiMailCentre::RefreshFriendList( const String& filter, bool forceVisible )
{
	if ( ( d_PopMenu->isVisible() || d_PopMenuBG->isVisible() ) && ( filter.size() == 0 ) && !forceVisible )
	{
		HideFriendsList();
		return;
	}
	else
	{
		CleanNameList();

		int itemHeight = FillFriendList(filter);
		
		ShowNameList(itemHeight);
		
		return;
	}
}

void KUiMailCentre::RefreshRecentList( const String& filter )
{
	if (d_PopMenu->isVisible() || d_PopMenuBG->isVisible())
	{
		HideFriendsList();
		return;
	}
	else
	{
		CleanNameList();
		
		int itemHeight = 0;
		
		UI_RECENT_OBJ_NAME  recentNameList[COMMON_CLIENT_MSG_LEN_256];
		ZeroMemory(recentNameList, COMMON_CLIENT_MSG_LEN_256 * sizeof(UI_RECENT_OBJ_NAME));
		
		int nameCount = g_pCoreShell->GetGameData( GDI_GET_RECENTLIST, 0, reinterpret_cast<int>(recentNameList) );
		
		for ( int i = 0; i < nameCount; i++ )
		{
			String	friendName = AnsiToUtf8( ( ( UI_RECENT_OBJ_NAME )recentNameList[i] ).szName );
			bool	needFilter = false;
			
			if ( 1 == filter.size() )
			{
				//filter如果包含一个字符串，择利用该字符串进行过滤
				if ( -1 == friendName.find_first_of( filter ) )
				{
					needFilter = true;
				}
			}

			if ( !needFilter )
			{
				TreeItemEx *Friend = new TreeItemEx( friendName );
				itemHeight = Friend->getPixelSize().d_height;
				d_PopMenu->addItem( Friend );
				d_PopupItemList.push_back(Friend);
			}
		}
		
		ShowNameList(itemHeight);
		return;
	}	
}

bool KUiMailCentre::btnNextPage_MouseClick( const CEGUI::EventArgs& args )
{
	if ( m_curPage + 1 < m_totalPage  )
	{
		m_curPage++;
		GotoPage( m_curPage );
	}
	return true;
}

bool KUiMailCentre::btnPrevPage_MouseClick( const CEGUI::EventArgs& args )
{
	if ( m_curPage  > 0  )
	{
		m_curPage--;
		GotoPage( m_curPage );
	}
	return true;
}

void KUiMailCentre::RefreshPageShow()
{
	ostringstream pageShow;
	pageShow<<m_curPage + 1<<"/"<<m_totalPage;
	m_pageShow->setText( AnsiToUtf8( pageShow.str().c_str() ) );
}

void KUiMailCentre::ShowPageItems( bool show )
{
	if ( show )
	{
		m_pageShow->show();
		m_btnNextPage->show();
		m_btnPrevPage->show();
	}
	else
	{
		m_pageShow->hide();
		m_btnNextPage->hide();
		m_btnPrevPage->hide();
	}
}

void KUiMailCentre::ShowCapability( int mailCount )
{
	//显示邮箱容量
	ostringstream mailCountShow;
	mailCountShow<<mailCount<<"/"<<m_maxMailCount;
	
	m_totalPage = mailCount / MAX_MAIL_NUM + (0 != mailCount % MAX_MAIL_NUM);

	if ( 0 == m_totalPage )
	{
		m_totalPage = 1;
	}
	
	if ( mailCount >= KUiCfgLoader::getSingleton().getMailCfg().Red_MailCount )
	{
		m_capability->setTextColours(RED_COLOUR);
	}
	else
	{
		m_capability->setTextColours(GREEN_COLOUR);
	}
	
	m_capability->setText( AnsiToUtf8( mailCountShow.str().c_str() ) );
}

// void KUiMailCentre::SetCurrentPage( int curPage )
// {
// 	
// 	
// }

void KUiMailCentre::GotoPage( int toPage )
{
	if ( toPage >= 0/* && toPage < m_totalPage */)
	{
		m_curPage = toPage;
		RefreshPageShow();
		g_pCoreShell->OperationRequest( GOI_MAIL_LIST_REQ, toPage, NULL );	
	}
}

bool KUiMailCentre::edtTitle_TextChanged( const CEGUI::EventArgs& args )
{   
	if ( NULL != m_pRecverNameEditbox && NULL != d_pSelFriend  && !m_lockReceiverNameText )
	{
		if ( d_pSelFriend->isVisible() )
		{
			String ReceiverName = m_pRecverNameEditbox->getText();
// 			if ( ReceiverName.size() == 1 )
// 			{
//				RefreshFriendList(ReceiverName);
//			}
// 			else if ( ReceiverName.size() == 0 )
// 			{
				RefreshFriendList(ReceiverName, ReceiverName.size() == 0);
//			}
			
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool KUiMailCentre::PopMenu_MouseWheel( const CEGUI::EventArgs& e )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&e;
	
	if(d_pFriendScrol->isVisible())
	{
		float newPos = d_pFriendScrol->getScrollPosition() - d_pFriendScrol->getStepSize() * eventArgs->wheelChange;
		d_pFriendScrol->setScrollPosition(newPos);
	}
	return true;	
}

void	processDeleteMail()
{
	// 确认删除邮件
	Tree* pTree = static_cast<Tree*>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));
	if ( pTree )
	{
		TreeItemMail* pItem = static_cast<TreeItemMail*>(pTree->getFirstSelectedItem());
		if ( pItem )
		{
			g_pCoreShell->OperationRequest( GOI_DEL_MAIL, (unsigned int)pItem->getMailID(), NULL );
			KUiMailCentre::GetSingleton().ShowCapability( 0 );
			KUiMailCentre::GetSingleton().GotoPage( 0 );
		}
	}
}

void	processPayMoney()
{
	// 确认付款
	Tree* pTree = static_cast<Tree*>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));
	if ( pTree && KUiMailCentre::d_payMoney > 0 )	
	{			
		TreeItemMail* pItem = static_cast<TreeItemMail*>(pTree->getFirstSelectedItem());
		if ( pItem )
		{
			g_pCoreShell->OperationRequest( GOI_GET_ITEM, (unsigned int)pItem->getMailID(), NULL );
			KUiMailCentre::GetSingleton().RecvObj();
		}
	}
}

void	processPostMoney()
{
	// 添加确认发送金钱
	g_pCoreShell->OperationRequest( GOI_SEND_MAIL, (unsigned int)(&KUiMailCentre::d_tempMail), NULL );
	Editbox *pTitle			= static_cast<Editbox *>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/Title" ));
	Editbox *pContent		= static_cast<Editbox *>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/SendReadMail/Content" ));
	
	if ( pTitle && pContent )
	{	 
		pTitle->setText("");		
		pContent->setText("");	
	}

	KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyj")->setText("0");
	KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyy")->setText("0");
	KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild("TaharezLook/MailCentre/bodyt")->setText("0");

	TLGameObject* pGOItem = static_cast<TLGameObject *>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/itemicon" ));
	if ( pGOItem )
	{
		pGOItem->clear();
		pGOItem->show();
	}
	TLGameObject* pGOMoney = static_cast<TLGameObject *>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild("TaharezLook/MailCentre/SendReadMail")->getChild( "TaharezLook/MailCentre/money" ));
	if ( pGOMoney )
	{
		pGOMoney->clear();
		pGOMoney->show();
	}

	KUiMailCentre::GetSingleton().UpdateSendTax();
}


void	processSendBackMail()
{
	// 确认退回邮件
	Tree* pTree = static_cast<Tree*>(KUiMailCentre::GetSingleton().m_pThisWnd->getChild( "TaharezLook/MailCentre/RecvMail" ));
	if ( pTree )
	{
		TreeItemMail* pItem = static_cast<TreeItemMail*>(pTree->getFirstSelectedItem());
		if ( pItem )
		{
			g_pCoreShell->OperationRequest( GOI_SEND_BACK_MAIL, (unsigned int)pItem->getMailID(), NULL );
			KUiMailCentre::GetSingleton().ShowCapability( 0 );
			KUiMailCentre::GetSingleton().GotoPage( 0 );
		}
	}
}