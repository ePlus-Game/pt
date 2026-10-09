#include "KWin32.h"
#include "UiSystemMessage.h"
#include "../KMessageCentre.h"
#include "UiTradeBox.h"
#include "UiDragItem.h"
#include "UiMailCentre.h"
#include "SocialComDef.h"
#include "UiGMCommunication.h"

using namespace std;
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"

#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "UiTongRecruitCentre.h"
#include "UiChatWindow.h" 
#include "UiLevelUp.h"


#define UI_SYSTEM_MESSAGE_IMAGESET		"tishixinxi"

#define UI_SYSTEM_MESSAGE_IMAGESET_ZUDUI			"tishixinxi1"
#define UI_SYSTEM_MESSAGE_IMAGESET_JIAOYI			"tishixinxi2"
#define UI_SYSTEM_MESSAGE_IMAGESET_SHIZU			"tishixinxi3"
#define UI_SYSTEM_MESSAGE_IMAGESET_ZHUHUOJOIN		"tishixinxi4"
#define UI_SYSTEM_MESSAGE_IMAGESET_LEVELUP			"tishixinxi4"
#define UI_SYSTEM_MESSAGE_IMAGESET_GM_FEEDBACK		"tishixinxi6"

#define UI_SYSTEM_MESSAGE_IMAGESET_ZUDUI_MOVING				"tishixinxi1_moving"
#define UI_SYSTEM_MESSAGE_IMAGESET_JIAOYI_MOVING			"tishixinxi2_moving"
#define UI_SYSTEM_MESSAGE_IMAGESET_SHIZU_MOVING				"tishixinxi3_moving"
#define UI_SYSTEM_MESSAGE_IMAGESET_ZHUHUOJOIN_MOVING		"tishixinxi4_moving"
#define UI_SYSTEM_MESSAGE_IMAGESET_LEVELUP_MOVING			"tishixinxi4_moving"
#define UI_SYSTEM_MESSAGE_IMAGESET_GM_FEEDBACK_MOVING		"tishixinxi6_moving"

#define UI_SYSTEM_MESSAGE_TEAMREQUEST	"tishixinxi_zudui"
#define UI_SYSTEM_MESSAGE_TRADEREQUEST	"tishixinxi_jiaoyi"
#define UI_SYSTEM_MESSAGE_SHIZUJOIN		"tishixinxi_shizu"
#define UI_SYSTEM_MESSAGE_ZHUHOUJOIN	"tishixinxi_shizu"
#define UI_SYSTEM_MESSAGE_GM_FEEDBACK	"tishixinxi_gm_feedback"
#define UI_SYSTEM_MESSAGE_FULLIMAGE		"full_image"

#define	UI_TEAMREQUEST_SOUND			"set:TaharezLook sound:Yaoqingxinxi"
#define	UI_TRADEREQUEST_SOUND			"set:TaharezLook sound:Yaoqingxinxi"
#define	UI_SHIZUJOIN_SOUND				"set:TaharezLook sound:Yaoqingxinxi"
#define	UI_ZHUHOUJOIN_SOUND				"set:TaharezLook sound:Yaoqingxinxi"
#define	UI_GM_FEEDBACK_SOUND			"set:TaharezLook sound:GmFeedback"

template<> 
KUiSystemMessage* KUiWndSingleton<KUiSystemMessage>::ms_Singleton	= NULL;

extern iCoreShell*		g_pCoreShell;

KUiSystemMessage::KUiSystemMessage(const CEGUI::String& id_name)
: KUiWndSingleton<KUiSystemMessage>( id_name )
, d_selectMsg(-1)
, d_AutoRefuse(false)
{
	for (int i = 0; i < SystemMessage_Ball_Count; ++i)
	{
		d_bMsgBallHasPlayed[i] = false;
	}
}

void KUiSystemMessage::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setRenderMode( true );
		ms_Singleton->getChild();
		ms_Singleton->refreshUi();
		d_msgBallSpeed = KUiCfgLoader::getSingleton().getCommonCfg().messageBallSpeed;
		d_msgBallPlayCycCount = KUiCfgLoader::getSingleton().getCommonCfg().messageBallPlayCycCount;
	}
}

void KUiSystemMessage::Show()
{
	KUiWndSingleton<KUiSystemMessage>::Show();
}

void KUiSystemMessage::clear()
{
	//在打开的时候清空列表
	d_msgList.clear();
	refreshUi();
}

void KUiSystemMessage::getChild()
{
	String parentWndName = "TaharezLook/SystemMessage";
	for(int i = 0; i < SystemMessage_Ball_Count; i++)
	{
		d_msgBall[i] = (TLStaticImage*)m_pThisWnd->getChild(parentWndName + "/Message" + iToString(i + 1));
		d_msgBallOriPos[i] = d_msgBall[i]->getPosition(Absolute);

		d_msgBall[i]->subscribeEvent(TLStaticImage::EventMouseClick, 
			Event::Subscriber(&KUiSystemMessage::onBDown, this));
		d_msgBall[i]->subscribeEvent(TLStaticImage::EventMouseMove, 
			Event::Subscriber(&KUiSystemMessage::onMMove, this));
		d_msgBall[i]->subscribeEvent(TLStaticImage::EventMouseLeaves, 
			Event::Subscriber(&KUiSystemMessage::onMLeave, this));
		d_msgBall[i]->setWindowShownSoundEnable(true);
	}

	d_msgComfirmBox = (TLStaticImage*)m_pThisWnd->getChild(parentWndName + "/MessageComfirmBox");
	d_msgbox_text	= (TLStaticText*)d_msgComfirmBox->getChild("TaharezLook/SystemMessage/MessageComfirmBox/MessageText");
	d_msgbox_yes	= (TLButton*)d_msgComfirmBox->getChild("TaharezLook/SystemMessage/MessageComfirmBox/Yes");
	d_msgbox_no		= (TLButton*)d_msgComfirmBox->getChild("TaharezLook/SystemMessage/MessageComfirmBox/No");
	d_msgbox_cancel	= (TLButton*)d_msgComfirmBox->getChild("TaharezLook/SystemMessage/MessageComfirmBox/Cancel");
	d_msgbox_yes->subscribeEvent(TLButton::EventMouseClick, 
			Event::Subscriber(&KUiSystemMessage::onYes, this));
	d_msgbox_no->subscribeEvent(TLButton::EventMouseClick, 
			Event::Subscriber(&KUiSystemMessage::onNo, this));
	d_msgbox_cancel->subscribeEvent(TLButton::EventMouseClick, 
			Event::Subscriber(&KUiSystemMessage::onCancel, this));
/*	d_msgbox_text->subscribeEvent(TLButton::EventMouseClick, 
			Event::Subscriber(&KUiSystemMessage::onMouseClickText, this));  
	d_msgbox_text->subscribeEvent(TLButton::EventMouseMove, 
			Event::Subscriber(&KUiSystemMessage::onMouseOnText, this));
*/
	ZeroMemory(d_layoutTextHead, sizeof(d_layoutTextHead));
	sprintf(d_layoutTextHead, "<Layout width=%d>", (int)(d_msgbox_text->getAbsoluteWidth()) );

}

KUiSystemMessage::~KUiSystemMessage()
{
	d_msgList.clear();
}

void KUiSystemMessage::refreshUi()
{
	if(d_msgList.empty())
		Hide();
	else
		Show();

	for(int i = 0; i < SystemMessage_Ball_Count; ++i)
	{
		MsgList::iterator it = d_msgList.find(i); 
		if ( NULL != d_msgBall[i] )
		{
			if ( it != d_msgList.end() )
			{
				if ( !d_msgBall[i]->isPlaying() && !d_bMsgBallHasPlayed[i] )
				{
					d_msgBall[i]->setFrameEnabled(false);
 					d_msgBall[i]->setCyc(true);
					d_msgBall[i]->show();
					d_msgBall[i]->play();

					Point tempPoint;
					tempPoint.d_x = 0;
					tempPoint.d_y = d_msgBall[i]->getPosition(Absolute).d_y;
					d_msgBall[i]->setPosition(Absolute, tempPoint);
 					d_msgBall[i]->MoveTo(d_msgBallOriPos[i].d_x, d_msgBallOriPos[i].d_y, d_msgBallSpeed, 0, false);
					d_bMsgBallHasPlayed[i] = false;
				}
			}
			else
			{
				d_msgBall[i]->hide();
				d_msgBall[i]->stop();
				d_bMsgBallHasPlayed[i] = false;
			}
		}
	}

	//KUiChatInputWnd::GetSingleton().SetActive(true);
	d_msgComfirmBox->hide();
	d_selectMsg = -1;
}


void KUiSystemMessage::showTip(int msgIndex)
{
	if(d_msgComfirmBox->isVisible() && d_msgbox_cancel->isVisible())
	{
		return;
	}
	showMsg(msgIndex);
	d_msgbox_yes->hide();
	d_msgbox_no->hide();
	d_msgbox_cancel->hide();
	static int xpos = d_msgComfirmBox->getXPosition(Absolute);
	d_msgComfirmBox->setPosition(Absolute, Point(xpos, -d_msgComfirmBox->getHeight(Absolute)));
	
	d_msgComfirmBox->show();
}
void KUiSystemMessage::showComfirmBox(int msgIndex)
{
	d_msgbox_yes->show();
	d_msgbox_no->show();
	d_msgbox_cancel->show();

	showMsg(msgIndex);

	int height = d_msgbox_yes->getHeight(Absolute) + d_msgbox_text->getHeight(Absolute) + 12;
	d_msgComfirmBox->setYPosition(Absolute, -d_msgComfirmBox->getHeight(Absolute));
	
	d_msgComfirmBox->show();
}

void KUiSystemMessage::genMsg(const SystemMessage& msg, char* msgText)
{
	switch(msg._msgType)
	{
	case SystemMessage::TeamRequest:
		{
			char* message = KMessageCentre::GetMessage(team_message, 3);
			sprintf(msgText, message, msg._name);
		}
		break;
	case SystemMessage::TradeRequest:
		{
			char* message = KMessageCentre::GetMessage(trade_box_message, KUiTradeBox::ui_trade_request);
			sprintf(msgText, message, msg._name);
		}
		break;
	/*case SystemMessage::TradeRefuse:
		{
			char* message = KMessageCentre::GetMessage(trade_box_message, KUiTradeBox::ui_trade_refuse);
			sprintf(msgText, message, msg._name);
			d_msgbox_yes->hide();
			d_msgbox_no->hide();
		}
		break;//*/
	case SystemMessage::SocialComfirm:
		{
			sprintf(msgText,"<Seg text-align=left float=wrap>%s</Obj></Seg></Layout>", msg._name);
		}
		break;

	case SystemMessage::TeamApplyJoin:
		{
			char* message = KMessageCentre::GetMessage(team_message, 8);
			sprintf(msgText, message, msg._name);
		}
		break;
	case SystemMessage::GMFeedBack:
		{
			char* message = KMessageCentre::GetMessage(common_message, CE_GM_Give_You_Msg);
			sprintf(msgText, message, msg._name);
		}
		break;
	}
}

void KUiSystemMessage::showMsg(int msgIndex)
{
	if(msgIndex < 0 || msgIndex >= SystemMessage_Max_Count)
	{
		return;
	}

	MsgList::iterator it = d_msgList.find(msgIndex);
	
	char tempText[COMMON_CLIENT_MSG_LEN_512];
	char msgText[COMMON_CLIENT_MSG_LEN_1024];
	ZeroMemory(tempText, sizeof(tempText));
	ZeroMemory(msgText, sizeof(msgText));
	genMsg(it->second, tempText);
	sprintf(msgText, "%s%s", d_layoutTextHead, tempText);

	d_msgbox_text->useLayout();
	d_msgbox_text->getLayout()->SetText(msgText);
	d_msgbox_text->getLayout()->flashLayout();
}

bool KUiSystemMessage::onBDown(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLStaticImage* destBall = (TLStaticImage*)arg->window;

	int msgIndex = -1;
	for(int i = 0; i < SystemMessage_Ball_Count; ++i)
	{
		if(d_msgBall[i] == destBall)
		{
			msgIndex = i;
			break;
		}
	}

	if(-1 == msgIndex)
		return true;
	
	d_selectMsg = msgIndex;

	MsgList::iterator it = d_msgList.find(msgIndex);
	if ( d_msgList.end() != it )
	{
		if ( SystemMessage::LevelUp == d_msgList[msgIndex]._msgType )
		{
			KUiLevelUp::GetSingleton().handleMouseClicks(e);
			CancelIt();
			return true;
		}
	}

	if(LeftButton == arg->button)
	{
		showComfirmBox(msgIndex);
	}
	else if(RightButton == arg->button)
	{
	    //直接删除消息，Core里面没有得到回复、社会关系的请求机制会出问题
		MsgList::iterator it = d_msgList.find(d_selectMsg);
		if (it==d_msgList.end())
			return true;

		CancelIt();
	}
	return true;
}

bool KUiSystemMessage::onMMove(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLStaticImage* destBall = (TLStaticImage*)arg->window;

	int msgIndex = -1;
	for(int i = 0; i < SystemMessage_Ball_Count; ++i)
	{
		if(d_msgBall[i] == destBall)
		{
			msgIndex = i;
			break;
		}
	}
	if(-1 == msgIndex)
		return true;
	
	MsgList::iterator it = d_msgList.find(msgIndex);
	if ( d_msgList.end() != it )
	{
		if ( SystemMessage::LevelUp == d_msgList[msgIndex]._msgType )
		{
			KUiLevelUp::GetSingleton().handleMouseEntres(e);
			return true;
		}
	}

	
	showTip(msgIndex);
	
	return true;
}

bool KUiSystemMessage::onMLeave(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	TLStaticImage* destBall = (TLStaticImage*)arg->window;

	int msgIndex = -1;
	for(int i = 0; i < SystemMessage_Ball_Count; ++i)
	{
		if(d_msgBall[i] == destBall)
		{
			msgIndex = i;
			break;
		}
	}
	if(-1 == msgIndex)
		return true;
	
	MsgList::iterator it = d_msgList.find(msgIndex);
	if ( d_msgList.end() != it )
	{
		if ( SystemMessage::LevelUp == d_msgList[msgIndex]._msgType )
		{
			KUiLevelUp::GetSingleton().handleMouseLeaves(e);
			return true;
		}
	}

	if(d_msgComfirmBox->isVisible() && d_msgbox_cancel->isVisible())
	{
		return true;
	}

	d_msgComfirmBox->hide();
	return true;
}
/*
bool KUiSystemMessage::onMouseOnText(const CEGUI::EventArgs & e)
{
	MouseEventArgs* mouse     = (MouseEventArgs*)&e;
	TLStaticText  * frameCtrl = (TLStaticText*)mouse->window;
	ILayout       * lay       = frameCtrl->getLayout();

	frameCtrl                 ->setTooltipText("");
	if(lay == NULL)
		return false;

	Point pos  = frameCtrl->getUnclippedPixelRect().getPosition();
	Point off  = frameCtrl->getLayoutOffset();
	int   xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int   yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return false;
	}//endif

	if (d_selectMsg >=0 )
	{
		MsgList::iterator it = d_msgList.find(d_selectMsg);
		if (it != d_msgList.end())
		{
			switch (it->second._msgType)
			{
			case SystemMessage::SocialComfirm :
				{
					if (elemInfo.gameObj._objType == LO_GO_SOCIAL_OWNER)
					{
						char* message = KMessageCentre::GetMessage(social_info, 0);
						if (message)
							frameCtrl->setTooltipText(AnsiToUtf8(message));
					}//endif

				}//end for case
				break;
				
			default:break;  
			}//end for switch
			
		}//endif
		
	}//endif
	
	return true;
}
*/
/*
bool KUiSystemMessage::onMouseClickText(const CEGUI::EventArgs & e)
{
	MouseEventArgs* mouse     = (MouseEventArgs*)&e;
	TLStaticText  * frameCtrl = (TLStaticText*)mouse->window;
	ILayout       * lay       = frameCtrl->getLayout();
	
	if(lay == NULL)
		return false;
	
	Point pos  = frameCtrl->getUnclippedPixelRect().getPosition();
	Point off  = frameCtrl->getLayoutOffset();
	int   xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int   yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return false;
	}//endif
	
	if (d_selectMsg >=0 )
	{
		MsgList::iterator it = d_msgList.find(d_selectMsg);
		if (it != d_msgList.end())
		{
			switch (it->second._msgType)
			{
			case SystemMessage::SocialComfirm :
				{
					if (elemInfo.gameObj._objType == LO_GO_SOCIAL_OWNER)
					{
						char* name = NULL;
						unicodeToAnsi(elemInfo.content.get(), name);
						if (elemInfo.gameObj._objId[0] == enSULayer_Player)
								ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;

					    TongInfoRequestParam            pParam ;
						pParam.code.m_IdentiyFlag    = TIUI_SOCIAL_INFO;
						pParam.code.m_ExtraFlag      = 0;
						strcpy(pParam.szOwnerName,name);
						pParam.nLayer                = elemInfo.gameObj._objId[0];

						g_pCoreShell->OperationRequest(GOI_SOCIAL_OWNER_INFO,(unsigned int)&pParam,0);

						delete [] name;
					}//endif
					
				}//end for case
				break;
				
			default:break;  
			}//end for switch
			
		}//endif
		
	}//endif

	return true;
}
*/

bool KUiSystemMessage::onYes(const CEGUI::EventArgs& e)
{
	MsgList::iterator it = d_msgList.find(d_selectMsg);

	if( it->second._msgType == SystemMessage::TradeRequest )
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_SEND_INVITE_RESPONSE, (unsigned int)true, (int)it->second._id);
	}
		
	if ( it->second._msgType == SystemMessage::TeamRequest )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_INVITE_RESPONSE, (unsigned int)it->second._id, (int)TRUE);
	}

	if ( it->second._msgType == SystemMessage::SocialComfirm )
	{
		g_pCoreShell->OperationRequest(GOI_SOCIETY_INVITE_RESPONSE, (unsigned int)it->second._id, (int)1);
	}

	if ( it->second._msgType == SystemMessage::TeamApplyJoin )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE, (unsigned int)it->second._id, (int)1);
	}	
	
	if ( it->second._msgType == SystemMessage::GMFeedBack )
	{
		KUiGMCommunication::getSingleton().show();
	}

	removeAMessage(d_selectMsg);
	refreshUi();
	return true;
}

bool KUiSystemMessage::onNo(const CEGUI::EventArgs& e)
{
	MsgList::iterator it = d_msgList.find(d_selectMsg);

	if(it->second._msgType == SystemMessage::TradeRequest)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_SEND_INVITE_RESPONSE, (unsigned int)false, (int)it->second._id);
	}

	if ( it->second._msgType == SystemMessage::TeamRequest )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_INVITE_RESPONSE, (unsigned int)it->second._id, (int)FALSE);
	}

	if ( it->second._msgType == SystemMessage::SocialComfirm )
	{
		g_pCoreShell->OperationRequest(GOI_SOCIETY_INVITE_RESPONSE, (unsigned int)it->second._id, (int)0);
	}

	if ( it->second._msgType == SystemMessage::TeamApplyJoin )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE, (unsigned int)it->second._id, (int)0);
	}

	removeAMessage(d_selectMsg);
	refreshUi();
	return true;
}

void KUiSystemMessage::CancelIt()
{
	MsgList::iterator it = d_msgList.find(d_selectMsg);
	
	if (it==d_msgList.end())
		return ;

	if ( SystemMessage::LevelUp == d_msgList[d_selectMsg]._msgType )
	{
		/*KUiLevelUp::GetSingleton().handleMouseEntres(e);*/
		removeAMessage(d_selectMsg);
		refreshUi();
		return;
	}

	//社会关系比较特殊，因为Core里面缓存了最近的消息，如果这个消息不被响应，则其他新的邀请请求将被忽略
	if ( it->second._msgType == SystemMessage::SocialComfirm )
	{
		g_pCoreShell->OperationRequest(GOI_SOCIETY_INVITE_RESPONSE, (unsigned int)it->second._id, (int)2);
	}
	
	if(it->second._msgType == SystemMessage::TradeRequest)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_SEND_INVITE_RESPONSE, (unsigned int)false, (int)it->second._id);
	}
	
	if ( it->second._msgType == SystemMessage::TeamRequest )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_INVITE_RESPONSE, (unsigned int)it->second._id, (int)FALSE);
	}
	
	if ( it->second._msgType == SystemMessage::TeamApplyJoin )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE, (unsigned int)it->second._id, (int)2);
	}
	
	removeAMessage(d_selectMsg);
	refreshUi();

}

bool KUiSystemMessage::onCancel(const CEGUI::EventArgs& e)
{
	/*
	MsgList::iterator it = d_msgList.find(d_selectMsg);

	if (it==d_msgList.end())
		return true;

	//社会关系比较特殊，因为Core里面缓存了最近的消息，如果这个消息不被响应，则其他新的邀请请求将被忽略
	if ( it->second._msgType == SystemMessage::SocialComfirm )
	{
		g_pCoreShell->OperationRequest(GOI_SOCIETY_INVITE_RESPONSE, (unsigned int)it->second._id, (int)2);
	}
	
	if(it->second._msgType == SystemMessage::TradeRequest)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_SEND_INVITE_RESPONSE, (unsigned int)false, (int)it->second._id);
	}
	
	if ( it->second._msgType == SystemMessage::TeamRequest )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_INVITE_RESPONSE, (unsigned int)it->second._id, (int)FALSE);
	}

	if ( it->second._msgType == SystemMessage::TeamApplyJoin )
	{
		g_pCoreShell->TeamOperation(TEAM_OI_APPLY_RESPONSE, (unsigned int)it->second._id, (int)2);
	}
	
	removeAMessage(d_selectMsg);
	refreshUi();
	*/

	MsgList::iterator it = d_msgList.find(d_selectMsg);
	
	if (it==d_msgList.end())
		return true;


	//修改Cancel 流程为查看详细信息.
	switch(it->second._msgType)
	{	
	case SystemMessage::SocialComfirm:
		{
			char szBuff[256] = "";
			int  nIndex     = 0;
			bool bWriting   = false;
			for (int i = 0;i<strlen(it->second._name);i++)
			{
				if (bWriting)
				{
					if (it->second._name[i] == '<')
					{
						szBuff[nIndex] = 0;
						break;
					}//endif
					else
					{
						szBuff[nIndex] = it->second._name[i];
						nIndex ++ ;
					}//end else

				}//endif
				else
				{
					if (it->second._name[i] == '>')
						bWriting = true;
				}//end else

			}//end for i;

			if (strlen(szBuff))
			{
				char* name   = szBuff;
				int   nLayer = it->second._id;

				TongInfoRequestParam            pParam ;
				pParam.code.m_IdentiyFlag    = TIUI_SOCIAL_INFO;
				pParam.code.m_ExtraFlag      = 0;
				strcpy(pParam.szOwnerName,name);
				pParam.nLayer                = nLayer;
				
				g_pCoreShell->OperationRequest(GOI_SOCIAL_OWNER_INFO,(unsigned int)&pParam,0);

				if (nLayer == enSULayer_Player)
						ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;

			}//

		}//end for socialcomfirm
		break;	
		
	case SystemMessage::TeamRequest:
		{
			g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)it->second._name , NULL );
			ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;
		}//end for socialcomfirm
		break;	
		
	case SystemMessage::TradeRequest:
		{
			g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)it->second._name, NULL );
			ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;
		}//end for socialcomfirm
		break;	
		
	case SystemMessage::TeamApplyJoin:
		{
			g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)it->second._name, NULL );
			ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;
		}//end for socialcomfirm
		break;	
		
	default: break;	
	}
	return true;
}

void KUiSystemMessage::addMessage(const SystemMessage& newMsg)
{
	if ( d_msgList.size() >= SystemMessage_Max_Count )
	{
		return;
	}

	//如果已经有升级信息了就不做任何处理
	for ( int i = 0; i < d_msgList.size(); i++ )
	{
		if ( (SystemMessage::LevelUp ==  d_msgList[i]._msgType) && (SystemMessage::LevelUp == newMsg._msgType) )
		{
			return;
		}
	}

	int iPos = 0;
	for ( iPos; iPos < SystemMessage_Max_Count; iPos++ )
	{
		MsgList::iterator it = d_msgList.find(iPos);
		if ( it == d_msgList.end() )
		{
			d_msgList.insert(MsgList::value_type(iPos, newMsg));
			d_msgDuring[iPos] = ::GetTickCount();
			const Sound* sound = NULL;

			switch(newMsg._msgType)
			{
			case SystemMessage::TeamRequest:
				{
					//d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_TEAMREQUEST);
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET_ZUDUI_MOVING, UI_SYSTEM_MESSAGE_FULLIMAGE);
					d_msgBall[iPos]->setText(UI_SYSTEM_MESSAGE_IMAGESET_ZUDUI);
					sound = PropertyHelper::stringToSound(UI_TEAMREQUEST_SOUND);
				}
				break;
			case SystemMessage::TradeRequest:
				{
					//d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_TRADEREQUEST);
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET_JIAOYI_MOVING, UI_SYSTEM_MESSAGE_FULLIMAGE);
					d_msgBall[iPos]->setText(UI_SYSTEM_MESSAGE_IMAGESET_JIAOYI);
					sound = PropertyHelper::stringToSound(UI_TRADEREQUEST_SOUND);
				}
				break;
			/*case SystemMessage::TradeRefuse:
				{
					d_msgBall[iPos]->setImage(UI_TRADEREFUSE, UI_FULL_IMAGESET);
				}
				break;//*/
			case SystemMessage::SocialComfirm:
				{
					//d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_SHIZUJOIN);
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET_SHIZU_MOVING, UI_SYSTEM_MESSAGE_FULLIMAGE);
					d_msgBall[iPos]->setText(UI_SYSTEM_MESSAGE_IMAGESET_SHIZU);
					sound = PropertyHelper::stringToSound(UI_SHIZUJOIN_SOUND);
				}
				break;
		/*	case SystemMessage::ZhuhouJoin:
				{
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_ZHUHOUJOIN);
					sound = PropertyHelper::stringToSound(UI_ZHUHOUJOIN_SOUND);
				}
				break;
		*/		
			case SystemMessage::TeamApplyJoin:
				{
					//d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_TEAMREQUEST);
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET_ZUDUI_MOVING, UI_SYSTEM_MESSAGE_FULLIMAGE);
					d_msgBall[iPos]->setText(UI_SYSTEM_MESSAGE_IMAGESET_ZUDUI);
					sound = PropertyHelper::stringToSound(UI_TEAMREQUEST_SOUND);
				}
				break;
			case SystemMessage::GMFeedBack:
				{
					//d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_TEAMREQUEST);
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET_GM_FEEDBACK_MOVING, UI_SYSTEM_MESSAGE_FULLIMAGE);
					d_msgBall[iPos]->setText(UI_SYSTEM_MESSAGE_IMAGESET_GM_FEEDBACK);
					sound = PropertyHelper::stringToSound(UI_GM_FEEDBACK_SOUND);
				}
			case SystemMessage::LevelUp :
				{
					//d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET, UI_SYSTEM_MESSAGE_TEAMREQUEST);
					d_msgBall[iPos]->setImage(UI_SYSTEM_MESSAGE_IMAGESET_LEVELUP_MOVING, UI_SYSTEM_MESSAGE_FULLIMAGE);
					d_msgBall[iPos]->setText(UI_SYSTEM_MESSAGE_IMAGESET_LEVELUP);
					//sound = PropertyHelper::stringToSound(UI_TEAMREQUEST_SOUND);
				}
				break;
			}

			if ( NULL != sound )
			{
				d_msgBall[iPos]->setWindowShownSound(sound);
				
			}
			else
			{
				d_msgBall[iPos]->setWindowShownSoundEnable(false);
			}
			
			refreshUi();

			return;
		}
	}
}

void KUiSystemMessage::removeAMessage(int index)
{
	MsgList::iterator it = d_msgList.find(index);
	if ( it != d_msgList.end() )
	{
		d_msgList.erase(it);
	}
	refreshUi();
}

void KUiSystemMessage::Breathe()
{
	if ( d_AutoRefuse )
	{
		for( int i = 0; i < SystemMessage_Max_Count; i++ )
		{
			MsgList::iterator it = d_msgList.find(i);
			if ( it != d_msgList.end() )
			{
				//直接删除消息，Core里面没有得到回复、社会关系的请求机制会出问题题
				CEGUI::EventArgs e;
				d_selectMsg    = i;
				onNo(e);
			}
		}
	}
	else
	{
		for( int i = 0; i < SystemMessage_Max_Count; i++ )
		{
			MsgList::iterator it = d_msgList.find(i);
			if ( it != d_msgList.end() )
			{
				int interval = ((::GetTickCount()-d_msgDuring[i])/1000);
				if ( SystemMessage_Cancel_Time <  interval )
				{
					//直接删除消息，Core里面没有得到回复、社会关系的请求机制会出问题
					d_selectMsg    = i;

//					MsgList::iterator it = d_msgList.find(d_selectMsg);
					
// 					if ( d_msgList.end() != it )
// 					{
// 						if ( (SystemMessage::LevelUp == d_msgList[d_selectMsg]._msgType) && (SystemMessage_CancelLevelUp_Time > interval) )
// 						{
// 							continue;
// 						}
// 					}

			    	CancelIt();
				}
			}
		}
	}

	UpdateMsgBall();
}

void KUiSystemMessage::UpdateMsgBall()
{
	for ( int j = 0; j < SystemMessage_Ball_Count; ++j )
	{
		if ( NULL != d_msgBall[j] )
		{
			if ( !d_bMsgBallHasPlayed[j] && d_msgBall[j]->isVisible() && d_msgBall[j]->getPosition(Absolute) == d_msgBallOriPos[j] )
			{
				
				d_msgBall[j]->stop();
				d_msgBall[j]->setImage(d_msgBall[j]->getText(), UI_SYSTEM_MESSAGE_FULLIMAGE);
				d_msgBall[j]->show();

				//当 0 == d_msgBallPlayCycCount 的时候不play
				if ( d_msgBallPlayCycCount > 0 )
				{
					d_msgBall[j]->setCycCount(d_msgBallPlayCycCount);	
					d_msgBall[j]->setCyc(false);
					d_msgBall[j]->play();
				}
				else if ( -1 == d_msgBallPlayCycCount)
				{
					d_msgBall[j]->setCyc(true);
					d_msgBall[j]->play();
				}
				
				d_bMsgBallHasPlayed[j] = true;
			}
		}
	}
}

