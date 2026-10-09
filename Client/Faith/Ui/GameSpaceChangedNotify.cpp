//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 21:31
//      File_base        : GameSpaceChangedNotify
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "coreshell.h"
#include "ChatDataDef.h"
#include "Faith.h"
#include "Login/Login.h"
#include "KMessageCentre.h"
#include "Ui/UiCase/UiChatWindow.h"
#include "Ui/UiCase/UiChatCentre.h"
#include "Ui/UiCase/UiMessageBox.h"
#include "Ui/UiCase/UiMailCentre.h"
#include "Ui/UiCase/UiBufferWnd.h"
#include "Ui/UiCase/UiDeathBox.h"
#include "Ui/UiCase/UiDebufferWnd.h"
#include "Ui/UiCase/UiTargetFace.h"
#include "Ui/UiCase/UiNpcMsgBox.h"
#include "Ui/UiCase/UiQuestManage.h"
#include "Ui/UiCase/UiTradeBox.h"
#include "Ui/UiCase/UiItemBox.h"
#include "Ui/UiCase/UiTopMessage.h"
#include "Ui/UiCase/UiPopMessage.h"
#include "Ui/UiCase/UiComMsgBox.h"
#include "Ui/UiCase/UiShop.h"
#include "Ui/UiCase/UiTeamList.h"
#include "Ui/UiCase/UiCompound.h"
#include "Ui/UiCase/UiErrorMessageBox.h"
#include "Ui/UiCase/UiEquipment.h"
#include "Ui/UiCase/UiStoreBox.h"
#include "Ui/UiCase/UiDuraAlert.h"
#include "Ui/UiCase/UiShortcutWnd.h"
#include "Ui/UiCase/UiSystemMessage.h"
#include "Ui/UiCase/UiTongCreate.h"
#include "Ui/UiCase/UiCityManager.h"
#include "Ui/UiCase/UiCastBar.h"
#include "Ui/UiCase/UiTalisman.h"
#include "Ui/UiCase/UiTongManager.h"
#include "Ui/UiCase/UiVendueWnd.h"
#include "Ui/UiCase/UiTargetEquipment.h"
#include "Ui/UiCase/UiRoleFace.h"
#include "Ui/UiCase/UiEquipment.h"
#include "Ui/UiCase/UiShortcutPlusWnd.h"
#include "Ui/UiCase/UiPKFilter.h"
#include "Ui/UiCase/UiSmith.h"
#include "Ui/UiCase/UiAutoConnect.h"
#include "Ui/UiCase/UiStudySkillManage.h"
#include "Ui/UiCase/UiLevelUp.h"
#include "Ui/UiCase/UiLevelUpInfo.h"
#include "Ui/UiCase/UiToolsControlBar.h"
#include "Ui/UiCase/UiBubble.h"
#include "Ui/UiCase/UiRoleHead.h"
#include "Ui/UiCase/UiChangeMapWnd.h"
#include "Ui/UiCase/UiBubble.h"
#include "Ui/UiCase/UiTrafficLight.h"
#include "Ui/UiCase/UiRaid.h"
#include "Ui/UiCase/UiTaisuiWnd.h"
#include "Ui/UiCase/UiMovieFrame.h"
#include "Ui/UiCase/UiQueryWnd.h"
#include "Ui/UiCase/UiSearchHelpWnd.h"
#include "Ui/UiCase/UiElf.h"
#include "Ui/UiCase/UiDragItem.h"
#include "Ui/UiCommon.h"
#include "Ui/UiCase/UiWorldCombatInfo.h"
#include "Ui/UiCase/UiGameSetting.h"
#include "Ui/UiCase/UiIBShop.h"
#include "Ui/UiCase/UiTimer.h"
#include "Ui/UiCase/UiPetFrame.h"
#include "Ui/UiCase/UiTeamViewer.h"
#include "Ui/UiWindowMgr.h"
#include "Ui/UiCase/UiShizuBanner.h"
#include "Ui/UiCase/UiFSBible.h"
#include "Ui/UiCase/UiFSBible_SpecialQuestData.h"
#include "Ui/UiCase/UiRandomCopyRewards.h"
#include "Ui/UiCase/UiGMCommunication.h"
#include "Ui/UiCase/UiCreditShop.h"
#include "Ui/UiCase/UiItemOperPanel.h"
#include "Ui/UiCase/UiHire.h"
#include "Ui/UiCase/UiDelayQuit.h"
#include "Ui/UiCase/UiIBShop.h"
#include "Ui/UiCase/UiCreditShop.h"
#include "Ui/UiCase/UiRecommend.h"
#include "Ui/UiCase/UiGenPersonalInfo.h"

///////////////////////
#include <commctrl.h>
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
#include "chatWindow/PlayerShowInfo.h"
#include "Ui/UiCase/UiTongRecruitCentre.h"

#include "ui/UiCase/UiFuryBox.h"
#include "chatWindow/PlayerShowInfo.h"
#include "chatWindow/ChatMiniMap.h"
#include "Ui/UiCase/UiBattleResult.h"
#include "Ui/UiCase/UiInfoBar.h"
#include "Ui/UiCase/UiQuestionWindow.h"
#include "Ui/UiCase/UiItemPassword.h"
#include "Ui/UiCase/UiRoleExp.h"
#include "Ui/UiCase/UiEntrustComputer.h"
#include "UiCase/UiPointListCharts.h"
//#define USING_CHAT_WINDOW

extern iCoreShell*		g_pCoreShell;
extern _UseItem 	g_UseItem;
extern int			g_uItemID;
extern _ItemPos		g_uItemPos;

// const int SCENEID_BEIHAI = 1;
// const int SCENEID_KUNLUN = 2;
// const int SCENEID_JIULI = 3;
// const int SCENEID_CHAOGE = 4;

void AcceptAddBuff()
{
	if (g_pCoreShell)
		g_pCoreShell->ReplyPrompt(prompt_event_add_buff, true);
}

void RefuseAddBuff()
{
	if (g_pCoreShell)
		g_pCoreShell->ReplyPrompt(prompt_event_add_buff, false);
}

/*!
\brief
	Declare function CoreDateChangedCallback(...).	
*/
void CoreDataChangedCallback( unsigned int uDataId, unsigned int uParam, int nParam );

/*!
\brief
	Implement KClientCallback interface function CoreDateChanged(...).	
*/
void KClientCallback::CoreDataChanged( unsigned int uDataId, unsigned int uParam, int nParam )
{
	CoreDataChangedCallback( uDataId, uParam, nParam );
}

/*!
\brief
	Implement function CoreDateChangedCallback(...).	
*/
void CoreDataChangedCallback( unsigned int uDataId, unsigned int uParam, int nParam )
{
	switch( uDataId )
	{
	case GDCNI_OPEN_USEITEM_DLG:
		{
			char* message = KMessageCentre::GetMessage( use_yibu_item, uParam );
			if ( message )
			{
				KUiComMsgBox::GetSingleton().setStyle( KUiComMsgBox::Style::UseItem );
				KUiComMsgBox::GetSingleton().setComMsgPosition();
				KUiComMsgBox::Show();
				KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(message), true);
			}
		}
		break;
	case GDCNI_OPEN_PLAYER_INFO:
		{
			FindResult* pResult = (FindResult*)uParam;
			if ( pResult )
			{
				PlayerInfo tagPlayer;
				ZeroMemory( &tagPlayer, sizeof(tagPlayer));
				strncpy( tagPlayer.szName, pResult->szName, 17 );
				strncpy( tagPlayer.szShizu, pResult->szShizu, 17 );
				strncpy( tagPlayer.szZhuhou, pResult->szZhuhou, 17 );
				tagPlayer.sLevel = pResult->sLevel;
				tagPlayer.sSkillType = pResult->sSkillType;
				tagPlayer.sMetier = pResult->sMetier;
				if(ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata)
				{
					KUiPlayerInfo::GetSingleton().Updatedata(tagPlayer );
					ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = false;
				}
				else
				////////////////窗口聊天获取信息///////////////////
				ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManegerGetPlayerInfo(tagPlayer);
				PlayerShowInfo::GetSingle().GetBaseInfo(tagPlayer);
			}
		}
		break;
/*
	case GDCNI_BUBBLE_WND:
		{
			BubbleParam* bubble = (BubbleParam*)uParam;
			if ( bubble )
			{
				CEGUI::Point point;
				point.d_x = bubble->x;
				point.d_y = bubble->y;

				CEGUI::Size size;
				size.d_width = bubble->width;
				size.d_height = bubble->height;

				CEGUI::Rect rect;
				rect.setPosition( point );
				rect.setSize( size );

				g_BubbleManager.ShowBubble( bubble->content, rect, bubble->ePos, bubble->breatheTime );
			}			
		}
		break;//*/
	case GDCNI_SEARCH_INFO:
		{
			SearchContentParam* pParam = (SearchContentParam*)uParam;
			if ( pParam )
			{
				SearchContent content;
				content.text			= pParam->text.c_str();
				content.id				= pParam->id;
				content.queryType		= pParam->queryType;
				content.queryResultType	= pParam->queryResultType;
				KUiSearchHelpWnd::GetSingleton().QueryRequest( content );
			}
		}
		break;
	case GDCNI_ROLEHEADINFO_BUBBLE_UPDATE:
		{
			Position* pos = (Position*)nParam;
			KUiBubbleManager::getSington().updateBubble( _Bubble::NpcHeadPop, uParam, *pos );
		}
		break;
	case GDCNI_ROLEHEADINFO_DEL:
		{
			RoleHeadInfo *pRoleHeadInfo = (RoleHeadInfo*)uParam;
			if ( pRoleHeadInfo )
			{
				KUiLayoutManager& lm = KUiLayoutManager::GetSingleton();
				lm.DelLayoutUnit( pRoleHeadInfo->dwID );
			}

		}
		break;
	case GDCNI_ROLEHEADINFO_UPDATA:
		{
			RoleHeadInfo *pRoleHeadInfo = (RoleHeadInfo*)uParam;
			if ( pRoleHeadInfo )
			{
				KUiLayoutManager& lm = KUiLayoutManager::GetSingleton();
				lm.AddLayoutUnit( pRoleHeadInfo->dwID, pRoleHeadInfo->szInfo );
			}
			
		}
		break;
	case GDCNI_ROLEHEADINFO_UPDATA_POS:
		{
			RoleHeadInfo *pRoleHeadInfo = (RoleHeadInfo*)uParam;
			if ( pRoleHeadInfo )
			{
				KUiLayoutManager& lm = KUiLayoutManager::GetSingleton();
				lm.SetLayoutUnitPosition( pRoleHeadInfo->dwID, pRoleHeadInfo->nX, pRoleHeadInfo->nY );
			}
			
		}
		break;
	case GDCNI_ROLEHEADINFO_VISIBLE:
		{
			if(uParam)
				KUiLayoutManager::GetSingleton().Render();
			else
				KUiLayoutManager::GetSingleton().Hide();
			break;
		}
		break;
	case GDCNI_ROLEHEADINFO_HIDE:
		{
			RoleHeadInfo *pRoleHeadInfo = (RoleHeadInfo*)uParam;
			if(pRoleHeadInfo)
			{
				KUiLayoutManager& lm = KUiLayoutManager::GetSingleton();
				lm.Hide(pRoleHeadInfo->dwID);
			}
		}
		break;
		
	case GDCNI_PING:
		{
			if ( KUiNaviation::IsVisible() )
			{
				KUiNaviation::GetSingletonPtr()->UpdateNetInfo(uParam);
			}

			if ( KUiInfoBarPing::IsVisible() )
			{
				KUiInfoBarPing::GetSingleton().UpdateNetInfo( uParam );
			}
		}
		break;
	case GDCNI_PK_SETTING:
		{
			KUiPKFilter::UpdatePKState( uParam );
		}
		break;

	case GDCNI_AUCTION_WND:
		{
			KUiVendueWnd::Show();
		}
		break;
	
	case GDCNI_CLOSE_ALLDIALOG:
		{
			KUiAdapter::UiCloseNoNpcDlg();
		}
		break;
	/************************************************************************/
	/*							Team system                                 */
	/************************************************************************/
	/*!
	\brief
		Notify team list change.
	*/
	case GDCNI_OPEN_CREATETONG:
		{
			KUiTongCreate::Show( uParam );
		}
		break;
	case GDCNI_OPEN_CITY:
		{
			KUiCityManager::Show();
		}
		break;
	case GDCNI_UPDATA_TONG_MANAGER:
		{
			KUiTongManager::UpdateData();
		}
		break;
	case GDCNI_OPEN_INVOTE_SOCIETY_REPLY_COMFIRM:
		{
			KUiPlayerItem *pPlayerItem = (KUiPlayerItem*)uParam;
			if ( pPlayerItem )
			{
				KUiSystemMessage::SystemMessage msg;
				msg._id = pPlayerItem->uId;

				char            szLayoutMsg[512]="";
				char            szRoleName[64]  ="";
				const char *    szHitMsg        = (const char *)pPlayerItem->nParam;

				int             nLen = strlen(szHitMsg);
				int             nIdx = 0;

				for ( int i=0; i < nLen ; i++ )
				{
					if (szHitMsg[i] != ' ')
						szRoleName[i] = szHitMsg[i];
					else
					{
						szRoleName[i] = 0;
						nIdx          = i+1;
						break;
					}//end else

				}//end for i

				if (strlen(szRoleName))
				{
					sprintf(msg._name,"<Obj type=text color=255,0,0 gotype=social goid=%d>%s</Obj><Obj type=text color=250,250,250 vertical-align=left>%s",pPlayerItem->uId,szRoleName,szHitMsg + nIdx );
				}//endif
				else
					strncpy(msg._name,(const char *)pPlayerItem->nParam,sizeof(msg._name));

				msg._msgType = KUiSystemMessage::SystemMessage::SocialComfirm;
				KUiSystemMessage::GetSingleton().addMessage(msg);
			}			
		}
		break;

	/************************************************************************/
	/*						Immediacy room                                  */
	/************************************************************************/
	/*!
	\brief
		Add immediacy room.

	\param uParam	
		
	\return
		
	*/	
	case GDCNI_ADD_IMMEDIACY:
		{
			KImmediacyParam* pImm = (KImmediacyParam*)uParam;
			if ( pImm )
			{
				if ( pImm->nPos < 10  )
				{
					KUiShortcutWnd::AddImmediacy( pImm );
				}
				else
				{
					pImm->nPos -= 10;
					KUiShortcutPlusWnd::AddImmediacy( pImm );
				}
			}
		}
		break;

	/*!
	\brief
		Del immediacy room.

	\param uParam	
		
	\return
		
	*/	
	case GDCNI_DEL_IMMEDIACY:
		{
			if ( uParam < 10  )
			{
				KUiShortcutWnd::DelImmediacy( uParam );
			}
			else
			{
				uParam -= 10;
				KUiShortcutPlusWnd::DelImmediacy( uParam );
			}
		}
		break;

	case GDCNI_UPDATA_SHORTCUT:
		{
			KUiShortcutWnd::UpdateData();
			KUiShortcutPlusWnd::UpdateData();
		}
		break;
	case GDCNI_REFRESH_SELECTED_SKILL:
		{
			KUiShortcutWnd::RefreshSelectedSkill();
			KUiShortcutPlusWnd::RefreshSelectedSkill();
		}
		break;
	/************************************************************************/
	/*                     Font system                                      */
	/************************************************************************/
	/*!
	\brief
		Draw text with cegui.

	\param uParam	
		
	\return
		
	*/	
	case GDCNI_DRAWTEXT:
		{
			KUiNewFont* pText = (KUiNewFont*)uParam;
			int leftAligned = nParam;
			
			CEGUI::FontManager* cefontMgr = CEGUI::FontManager::getSingletonPtr();
			if(!cefontMgr || !pText)
			{
				break;
			}
			
			CEGUI::String strContext(AnsiToUtf8(pText->szContext));
			CEGUI::Font* cefont = NULL;
			String fontName = AnsiToUtf8(pText->szName);

			if(pText->bDefaultFont || !cefontMgr->isFontPresent(fontName))
			{
				cefont = CEGUI::System::getSingleton().getDefaultFont();
			}
			else
			{
				cefont = cefontMgr->getFont(fontName);
			}

			if(!cefont)
			{
				break;
			}
			
			CEGUI::Point drawPos;
			int textPixelWidth = cefont->getTextExtent(strContext);
			if(leftAligned == NULL)
			{
				drawPos.d_x = pText->nX - textPixelWidth / 2;
			}
			else
			{
				drawPos.d_x = pText->nX;
			}
			drawPos.d_y = pText->nY;

			CEGUI::Size	size(textPixelWidth, cefont->getLineSpacing());
			CEGUI::Rect rect(drawPos, size);
			CEGUI::ColourRect color;
			color.setColours(CEGUI::colour(pText->uColor));
			cefont->drawText(strContext, rect, 0, CEGUI::Centred, color );
		}
		break;
	/************************************************************************/
	/*						Item system                                     */
	/************************************************************************/
	/*!
	\brief
		Open or Close the compound dialog.

	\param uParam	
		(bool) if uParam != 0 open, else close.
		
	\return
		
	*/
	case GDCNI_OPEN_COMPOUND_WND:
		{
			if ( uParam > 0 )
			{
				if ( uParam == 1 )
				{
					KUiCompound::Show( (COMPOUNDTYPE)nParam );
				}
				else
				{
					KUiCompound::Update( nParam );
				}
				
			}
			else
			{
				KUiCompound::Hide();
			}
			
		}
		break;

	/*!
	\brief
		Display Get Object Animation.

	\param uParam = NULL
	\param nParam = NULL		
	\return
		
	*/
	case GDCNI_PICKUP_OBJECT_TO_BAG:
		{
			KUiNaviation::GetSingletonPtr()->PickUpObject();
		}
		break;
		
	/*!
	\brief
		Begin the group cool down.

	\param uParam	
		(KItemGroupCD_C*)uParam.

	\return
		
	*/
	case GDCNI_BEGIN_GROUP_CD:
		{
			KItemGroupCD_C* pGroupCD = (KItemGroupCD_C*)uParam;
			if ( pGroupCD )
			{
				KUiShortcutWnd::BeginGroupCD( pGroupCD );
				KUiShortcutPlusWnd::BeginGroupCD( pGroupCD );
				KUiItemBox::getSingleton().beginGroupCD( pGroupCD );
			}
		}
		break;

	/*!
	\brief
		End the group cool down.

	\param uParam	
		(KItemGroupCD_C*)uParam.

	\return
		
	*/
	case GDCNI_END_GROUP_CD:
		{
			KItemGroupCD_C* pGroupCD = (KItemGroupCD_C*)uParam;
			if ( pGroupCD )
			{
				KUiShortcutWnd::EndGroupCD( pGroupCD );
				KUiShortcutPlusWnd::EndGroupCD( pGroupCD );
				KUiItemBox::getSingleton().endGroupCD( pGroupCD );
			}
		}
		break;
	/************************************************************************/
	/*				String for client from server                           */
	/************************************************************************/
	/*!
	\brief
		Show string on a framewindow in game space.
	*/
	case GDCNI_FRAME_MESSAGE:
		{
			KUiCommonMsgBox::OpenWindow( (const char *)uParam );
		}
		break;
	/************************************************************************/
	/*				Login and Exit                                          */
	/************************************************************************/
	/*!
	\brief
		Notify load map.
	*/
	case GDCNI_GAME_PRE_LOAD_MAP:
		{
		 	g_LoginLogic.NotifyToLoadMap();
			KUiChangeMapWnd::GetSingleton().show( uParam > 0 ? true : false, (ChangeMapParam)nParam );
			KUiShizuBanner::getSingleton().Hide();
			closeUiWnd(PLAYER_TRANSMISION);
		}
		break;
	/*!
	\brief
		Notify end load progress.
	*/
	case GDCNI_GAME_END_LOAD_PROGRESS:
		{
			KUiChangeMapWnd::GetSingleton().EndLoading();
			KUiChatInputWnd::GetSingleton().freshChanName();
			B2ChatDialog::SwapChannel();
		}
		break;
	
	/*!
	\brief
		Notify enter game space.
	*/
	case GDCNI_GAME_START:
		{
			KUiAdapter::UiStartGame();
		 	g_LoginLogic.NotifyToStartGame();
			KUiAutoConnect::Hide();
		}
		break;

	/*!
	\brief
		Notify exit game space.
	*/
	case GDCNI_END_GAME:
		{
		}
		break;
	/************************************************************************/
	/*                  Chat and Friend                                     */
	/************************************************************************/
	case GDCNI_CHAT_ROOM_CHANGEOWNER:
		{
			KUiChatCentre::ProcessChatNotify( ChangeRoomOwnerNotify, uParam, (BYTE*)nParam );
		}
		break;
	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The chat channel id made by server.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_CHAT_CHANNEL_CREATE:
	{
		KUiChanMgr::getSinglton().regist(*(Ui_Channel_Param*)uParam);
#ifdef USING_CHAT_WINDOW
		B2ChatDialog::chatManager.ChatManagerRegistChannel(*(Ui_Channel_Param*)uParam);
#endif
	}
	break;

	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The chat channel id made by server.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GDCNI_CHAT_CHANNEL_CLOSE:
		{
			KUiChanMgr::getSinglton().unregist(((Ui_Channel_Param*)uParam)->dwChannelID);
#ifdef USING_CHAT_WINDOW
			B2ChatDialog::chatManager.ChatManagerChannelClose(((Ui_Channel_Param*)uParam)->dwChannelID);
#endif
		}

		break;

	/*!
	\brief
		Notify chat channel create.
	
	\param uParam
		(DWORD)The chat channel id made by server.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GDCNI_CHAT_All_CHANNEL_CLOSE:
	{
		KUiChanMgr::getSinglton().unregistAll();
		KUiChannelCentre::GetSingleton().closeAllFrame();
	}
	break;
	
	/*!
	\brief
		Receive message from a person.
	
	\param uParam
		(const string &)The sender role name.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	case GDCNI_RECV_CHAT_DATE_P2P:	
		{
			KUiChatCentre::ProcessChatNotify( PrivateChatNotify, uParam, NULL );
		}
		break;	
	case GDCNI_RECV_CHAT_DATE_P2P_TO_CHAT_WINDOW:	
		{
			KUiChannelCentre::GetSingleton().recvCozeMessage((BYTE*)uParam);
#ifdef USING_CHAT_WINDOW
			B2ChatDialog::chatManager.ChatManagerReceiveCozeMsg((BYTE*)uParam);
#endif
		}
		break;	
	/*!
	\brief
		Receive message from a chat room.
	
	\param uParam
		(DWORD)The chat room id.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	case GDCNI_RECV_CHAT_DATE_R2P:
		{
			KUiChannelCentre::GetSingleton().recvMessage( uParam, (BYTE*)nParam );
			KUiChatCentre::ProcessChatNotify( RoomChatNotify, uParam, (BYTE*)nParam );
#ifdef USING_CHAT_WINDOW
			B2ChatDialog::chatManager.ChatManagerReceiveChatMsg(uParam,(BYTE*)nParam);
#endif
		}
		break;

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_CHAT_ROOM_CREATE:
		{
			KUiChatCentre::ProcessChatNotify( CreateChatRoomNotify, uParam, (BYTE*)nParam );
		}
		break;
	case GDCNI_CHAT_ROOM_JOIN:
		{
			KUiChatCentre::ProcessChatNotify( JoinRoomMemberNotify, uParam, (BYTE*)nParam );
		}
		break;
	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_CHAT_ROOM_ADD:
		{
			KUiChatCentre::ProcessChatNotify( AddRoomMemberNotify, uParam, (BYTE*)nParam );
		}
		break;
	case GDCNI_CHAT_ROOM_KICK:
		{
			CHAT::PCHATROOM_KICKMEMBER_NOTIFY	pNotify = (CHAT::PCHATROOM_KICKMEMBER_NOTIFY)nParam;
			char* myName = NULL;
         	g_pCoreShell->GetGameData(GDI_GET_MY_NAME, (unsigned int)&myName, NULL);

			if (strcmp(pNotify->name,myName)==0)
			{
                KUiChatCentre::ProcessChatNotify( KickMemberNotify, uParam, (BYTE*)nParam );
			}//endif
            else
				KUiChatCentre::ProcessChatNotify( LeaveMemberNotify, uParam, (BYTE*)nParam );
		}
		break;
	case GDCNI_CHAT_ROOM_LEAVE:
		{
			CHAT::PCHATROOM_KICKMEMBER_NOTIFY	pNotify = (CHAT::PCHATROOM_KICKMEMBER_NOTIFY)nParam;
			char* myName = NULL;
         	g_pCoreShell->GetGameData(GDI_GET_MY_NAME, (unsigned int)&myName, NULL);

			if (strcmp(pNotify->name,myName)==0)
			{
                KUiChatCentre::ProcessChatNotify( KickMemberNotify, uParam, (BYTE*)nParam );
			}//endif
            else
				KUiChatCentre::ProcessChatNotify( LeaveMemberNotify, uParam, (BYTE*)nParam );
		}
		break;
	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_FRIENDLIST_NOTIFY:
		{
			KUiChatCentre::ProcessFriendNotify( AddFriendReturnNotify, (BYTE*)uParam );
			ChatFriendPanelManager::ChatFriendManagerGet().ChatFriendManagerReceiveFriendList();
		}
		break;
	/************************************************************************/
	/*						Mail                                            */
	/************************************************************************/
	case GDCNI_NOTIFY_PLUSITEM_STATE:
		{
			KUiMailCentre::GetSingleton().OnGetPlusItem( uParam );
		}
		break;

	/*!
	\brief
		Open or close the mail tip dialog.

	\return
		
	*/
	case GDCNI_SWITCH_MAIL_TIP:
		{

		}
		break;

	/*!
	\brief
		Open or close the mail dialog.

	\return
		
	*/
	case GDCNI_SWITCH_MAIL:
		{
			if ( uParam > 0 )
			{
				KUiMailCentre::Show();
			}
			else
			{
				KUiMailCentre::Hide();
			}
			
		}
		break;

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_RECV_MAIL_LIST:
		{
			KUiMailCentre::GetSingletonPtr()->OnRecvMailListRet( (BYTE*)uParam );
		}
		break;

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_RECV_MAIL:
		{
			KUiMailCentre::OnRecvMailRet( (BYTE*)uParam );
		}
		break;

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_SEND_MAIL_RET:
		{
			KUiMailCentre::OnSendMailRet( NULL );
		}
		break;

	/*!
	\brief
		Notify chat room group change.
	
	\param uParam
		(DWORD)The change type.

  	\param nParam
		(void *)The change param.

	\return
		Nothing.
	*/
	case GDCNI_DEL_MAIL_RET:
		{
			KUiMailCentre::OnDelMailRet( true );
		}
		break;

	case GDCNI_NEW_MAIL_NOTIFY:
		{
			KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find("NewMail");
			if(NULL == light)
			{
				break;
			}
			light->lightup();

			//如果玩家在野外，则给出头顶提示
			KUiSceneTimeInfo mapInfo;
			ZeroMemory(&mapInfo, sizeof(KUiSceneTimeInfo));
			g_pCoreShell->SceneMapOperation( GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
			
// 			bool bShowRemind = 
// 				(SCENEID_BEIHAI != mapInfo.nSceneId)
// 				&& (SCENEID_KUNLUN != mapInfo.nSceneId)
// 				&& (SCENEID_JIULI != mapInfo.nSceneId)
// 				&& (SCENEID_CHAOGE != mapInfo.nSceneId);
			
// 			if ( bShowRemind )
// 			{
				KUiErrorMessageBox::GetSingleton().AddMessage(
					AnsiToUtf8( KMessageCentre::GetMessage( mail_message, 16 ) ) 
					);
//			}
	
		}
		break;

	case GDCNI_OPEN_MAIL_NOTIFY:
		{
			int newMailCount = g_pCoreShell->GetGameData(GDI_GET_NEW_MAIL_COUNT, NULL, NULL);
			if(newMailCount > 0)
			{
				break;
			}

			//如果没有新邮件了，就取消状态
			KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find("NewMail");
			if(NULL == light)
			{
				break;
			}
			light->terminate();
		}
		break;
	case GDCNI_OPEN_LIGHT:
		{
			if ( uParam )
			{
				KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find((char*)uParam);
				if(light)
				{
					light->lightup();
				}
			}
		}
		break;
	case GDCNI_CLOSE_LIGHT:
		{
			if ( uParam )
			{
				KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find((char*)uParam);
				if(light)
				{
					light->terminate();
				}			

			}
		}
		break;
		
	case GDCNI_PK_VALUE_CHANGE:
		{ 
			KUiChatCentre::PkValueChangeNotify((const char *)uParam,(int)nParam);
		}
		break;
		
	/************************************************************************/
	/*					Buffer system                                       */
	/************************************************************************/
	/*!
	\brief
		Notify client to open a buffer.
	
	\param uParam
		(DWORD)The buffer param struct.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	case GDCNI_BUFFER_OPEN:
		{
			KBufferSyncInfo* pBufSycnInfo = (KBufferSyncInfo*)uParam;
			if ( pBufSycnInfo )
			{
				KBufferInfo buffInfo;
				g_pCoreShell->GetGameData( GDI_GET_BUFFER_INFO, (unsigned int)&buffInfo, pBufSycnInfo->nTempBuffID );
				if ( buffInfo.nBuff )
				{
					KUiBufferCentre::AddRoleBuffer( uParam, (KUiBufferCentre::CoverType)nParam );
				}
				else
				{
					KUiDebufferCentre::AddRoleBuffer( uParam, (KUiDebufferCentre::CoverType)nParam );
				}
			}
		}
		break;

	/*!
	\brief
		Notify client to delete buffer.
	
	\param uParam
		(DWORD)The buffer param struct.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	case GDCNI_BUFFER_DEL:
		{
			KUiBufferCentre::DelRoleBuffer( uParam );
			KUiDebufferCentre::DelRoleBuffer( uParam );
		}
		break;

	/*!
	\brief
		Notify client to delete all buffers.
	
	\param uParam
		(DWORD)The buffer param struct.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	case GDCNI_BUFFER_DEL_ALL:
		{	
			
			KUiBufferCentre::DelAllRoleBuffer();
			KUiDebufferCentre::DelAllRoleBuffer();
		}
		break;

	/************************************************************************/
	/*							Death                                       */
	/************************************************************************/
	case GDCNI_DEATH:
		{
			KUiDeathBox::Show();
			closeUiWnd(PLAYER_DEATH);
		}
		break;
	case GDCNI_DEATH_CLOSE:
		{
			KUiDeathBox::Hide();
			closeUiWnd(PLAYER_DEATH);
		}
		break;

	case GDCNI_REVIVE:
		{
			KUiDeathBox::Hide();
		}
		break;
	/************************************************************************/
	/*							Select target                               */
	/************************************************************************/
	case GDCNI_SEL_TARGET:
		{
			if ( uParam )
			{
				KUiTargetFace::Show();
			}
			else
			{
				KUiTargetFace::Hide();
			}
		}
		break;
	case CDCNI_UPDATA_SEL_TARGET:
		{
			if ( KUiTargetFace::IsVisible() )
			{
				KUiTargetFace::UpdateData();
			}
		}
		break;

	case CDCNI_UPDATA_SELF_FACE:
		{
			if ( KUiRoleFace::IsVisible() )
			{
				KUiRoleFace::UpdateData();
				// 同步选择目标信息
				if(KUiTargetFace::IsVisible())
					KUiTargetFace::UpdateData();
			}
		}
		break;
	case CDCNI_UPDATA_ROLE_STATE:
		{
			if ( KUiEquipment::IsVisible() )
			{
				KUiEquipment::UpdateData();
			}
			
		}
		break;

	/************************************************************************/
	/*							Quest system                                */
	/************************************************************************/


	case GDCNI_OPEN_QUEST_NPC_DIALOG:
		{
			if ( nParam )
			{
				KUiNpcMsgBox::GetSingleton().OpenNpcTalk( (const KUiQuestionAndAnswer*)uParam);
				KUiFSBible::getSingleton().hide();
			} 
			else
			{
				KUiNpcMsgBox::GetSingleton().OpenNpcMission( (const KQuestInfo*)uParam );
				KUiFSBible::getSingleton().hide();
			}
		}
		break;
	case GDCNI_OPEN_MOVIE_SCENE:
		{
			const KUiMovieScene* movieSceneData = (KUiMovieScene*)uParam;
			if(movieSceneData)
			{
				KUiMovieFrame::getSinglton().npcWantTalk(movieSceneData);
			}
		}
		break;
	case GDCNI_NPC_DLG_CLOSE:
		{
			KUiNpcMsgBox::Hide();
		}
		break;
	case GDCUI_QUEST_LIST_CHANGE:
		{
			KUiFSBible::getSingleton().onQuestListChange();
			KUiQuestManage::GetSingleton().onQuestListChange();
			KUiQuestTrack::GetSingleton().freshTrackList();
		}
		break;
		
	/************************************************************************/
	/*							Skill system                                */
	/************************************************************************/
	/*!
	\brief
		Notify skill list change.

	\param 
		uParam(int) SkillID.		
	*/
	case GDCNI_SKILL_CD_BEGIN:
		{
//			KUiShortcutWnd::BeginSkillCD( uParam );
//			KUiShortcutPlusWnd::BeginSkillCD( uParam );
		}
		break;

	/*!
	\brief
		Notify skill list change.
	*/
	case GDCNI_SKILLLIST_CHANGE:
		{
			// 不为普通攻击
			if ( uParam != 0 )
			{
				KUiStudySkillManage::Updatedata( uParam, nParam );

				// 技能升级更新快捷栏
				KUiShortcutWnd::UpdateData();
				KUiShortcutPlusWnd::UpdateData();
				KUiEntrustComputer::GetSingleton().UpdateSkill();
			}
			if(KUiItemBox::getSingleton().isVisible())
			{
				KUiItemBox::getSingleton().updatePropertys();
			}
		}
		break;

	/*!
	\brief
		Notify skill list change.

	\param 
		uParam(int) show or hide.		

	\param 
		nParam(int) if study skill.		

	*/
	case GDCNI_SKILLLIST_OPEN:
		{
			if ( uParam > 0 )
			{
				KUiStudySkillManage::Show( nParam > 0 ? true : false );
			}
			else
			{
				KUiStudySkillManage::Hide();
			}
			
		}
		break;

	/************************************************************************/
	/*							Skill system                                */
	/************************************************************************/
	case CDCNI_UPDATE_TEAM_INFO:
		{
			KUiPlayerTeam * pInfo = (KUiPlayerTeam*)uParam;
			if ( pInfo ) 
			{
				if (nParam)
				{
                    KUiTeamList::SetCanShow(true);
				}

				KUiTeamList::Show();
				KUiTeamHideShow::Show();
				
				if(KUiRaid::getSinglton().isVisible())
				{
					KUiRaid::getSinglton().freshTeamInfo();
				}
			}
			else
			{
				KUiTeamList::SetDisapearFlag();
				KUiTeamHideShow::Hide();
				KUiTeamList::Hide();
			}
		}
		break;


	/*!
	\brief
		Notify open comfirm team request.

	\param 
		uParam(int) show or hide.		

	\param 
		nParam(int) if study skill.		

	*/
	case CDCNI_OPEN_INVOTE_TEAM_REPLY:
		{
			KUiPlayerItem *pPlayerItem = (KUiPlayerItem*)uParam;
			if ( pPlayerItem )
			{
				KUiSystemMessage::SystemMessage msg;
				msg._id = pPlayerItem->uId;
				strcpy(msg._name, pPlayerItem->Name);
				msg._msgType = KUiSystemMessage::SystemMessage::TeamRequest;
				KUiSystemMessage::GetSingleton().addMessage(msg);
			}
		}
		break;

	case CDCNI_OPEN_APPLY_JOIN_TEAM_REPLY:
		{
			KUiPlayerItem *pPlayerItem = (KUiPlayerItem*)uParam;
			if ( pPlayerItem )
			{
				KUiSystemMessage::SystemMessage msg;
				msg._id = pPlayerItem->uId;
				strcpy(msg._name, pPlayerItem->Name);
				msg._msgType = KUiSystemMessage::SystemMessage::TeamApplyJoin;
				KUiSystemMessage::GetSingleton().addMessage(msg);
			}
		}
		break;

	case GDCNI_UPDATE_LIST_TEAM:
		{
			if ( KUiTeamViewer::IsVisible() )
			{
				KUiTeamViewer::GetSingleton().Update((TeamBasicInfo*)uParam, nParam);
			}
		}
		break;

	/************************************************************************/
	/*                  Trade system                                        */
	/************************************************************************/
	case GDCNI_TRADE_START:
		{
			char* oppoName = (char*)nParam;
			KUiTradeBox::GetSingleton().show(oppoName);
			KUiItemBox::getSingleton().show();
		}
		break;
	case GDCNI_TRADE_OPPOSITE_BUSY:
		{	
			char* message = KMessageCentre::GetMessage(trade_box_message, KUiTradeBox::ui_trade_opposite_busy);
			KUiChannelCentre::GetSingleton().toSysMsg(message);
		}
		break;
	case GDCNI_TRADE_LOCK:		//自己或对方锁定状态同步
		if ( uParam > 0)
		{
			KUiTradeBox::GetSingleton().lock(true);
		}
		else
		{
			KUiTradeBox::GetSingleton().lock(false);
		}
		break;
	case GDCNI_TRADE_UNLOCK:		//自己或对方锁定状态同步
		KUiTradeBox::GetSingleton().unlock();
		break;
	case GDCNI_END_TRADE:			//交易结束
		if ( uParam > 0)
		{
			KUiTradeBox::GetSingleton().endTrade(true);
		}
		else
		{
			KUiTradeBox::GetSingleton().endTrade(false);
		}
		break;
	case GDCNI_TRADE_OK:
		{
			KUiTradeBox::GetSingleton().Hide();
			KUiTradeBox::GetSingleton().printSystemMessage(KUiCfgLoader::getSingleton().getTradeCfg().completeTradeMsg);
		}
		break;
	case GDCNI_TRADE_CANCEL:
		{
			KUiTradeBox::GetSingleton().Hide();
			KUiTradeBox::GetSingleton().printSystemMessage(KUiCfgLoader::getSingleton().getTradeCfg().cancelTradeMsg);
		}
		break;
	case GDCNI_OBJECT_CHANGED:	//物品刷新
		{
			if (!uParam)
				break;

			KObjAtContRegion* pObjRegion = (KObjAtContRegion*)uParam;
			
			if(!nParam && KUiDragItem::GetSingleton().getObj()->getObject().d_type != TLGameObject::idle)
			{
				TLGameObject* go = KUiDragItem::GetSingleton().getObj();
				int itemIndex = ((KObjAtContRegion*)go->getUserData())->Obj.uId;
				if(pObjRegion->Obj.uId == itemIndex)
				{
					KUiDragItem::GetSingleton().initItem();
				}
			}

			switch(pObjRegion->eContainer)
			{
			case UOC_ITEM_TAKE_WITH:
				KUiItemBox::getSingleton().onItemChanged(pObjRegion, nParam);
				break;
			case UOC_EQUIPTMENT:
				KUiEquipment::GetSingleton().onItemChanged(pObjRegion, nParam);
				break;
			case UOC_STORE_BOX:
				KUiStoreBox::getSingleton().onItemChanged(pObjRegion, nParam);
				break;
			case UOC_TO_BE_TRADE:
				KUiTradeBox::GetSingleton().onSelfItemChanged(pObjRegion, nParam);
				break;
			case UOC_OTHER_TO_BE_TRADE:
				KUiTradeBox::GetSingleton().onOppositeItemChanged(pObjRegion, nParam);
				break;
			case UOC_ITEMBOX_EXTEND:
				KUiItemBox::getSingleton().onItemExtendChanged(pObjRegion, nParam);
				break;
			case UOC_STORE_EXTEND:
				KUiStoreBox::getSingleton().onItemExtendChanged(pObjRegion, nParam);
				break;
			}
			break;
		}
		break;	
	case GDCNI_JINSHANBI_CHANGED:
		{
			KUiItemBox::getSingleton().onJinShanBiChanged(nParam);
			KUiIBShop::GetSingleton().SetJinShanBi(nParam);
		}
		break;
	case GDCNI_TRADE_OPPOSITE_MONEY_CHANGED:
		{
			int money = (int)uParam;
			KUiTradeBox::GetSingleton().onOppositeMoneyChanged(money);
		}
		break;
		/*!
		\brief
		顶部消息.
		*/
	case GDCNI_TOPMESSAGE:
		{
			char* message = (char*)uParam;
			CommonStyle* style = (CommonStyle*)nParam;
			KUiTopMessage::GetSingleton().setText(AnsiToUtf8(message));
			KUiTopMessage::GetSingleton().setStyle(*style);
		}
		break;
	case GDCNI_TOPMESSAGE_ID:
		{
			CommonStyle2* style = (CommonStyle2*)uParam;
			if(style)
			{
				KUiTopMessage::GetSingleton().setStyle(*style);
			}
		}
		break;
	case GDCNI_SHIZU_BANNER:
		{
			char* message = (char*)uParam;
			int bannerIndex = nParam;
			KUiShizuBanner::getSingleton().showBanner(bannerIndex, message);
		}
		break;
		/*!
		\brief
		IBSHOP消息.
		*/
	case GDCNI_IBSHOPMESSAGE:
		{
			char* message = (char*)uParam;
			CommonStyle* style = (CommonStyle*)nParam;
			KUiIBShop::GetSingleton().SetIBShopMessage(*style, AnsiToUtf8(message));
		}
		break;
	case GDCNI_RETURN_CREDIT_SUCCESS:
		{
			//信用还款成功返回
			KUiIBShopResultMessage::GetSingleton().SetReturnCreditMessage();
		}
		break;
	case GDCNI_IBITEM_BUY_SUCCESS:
		{
			//购买IB物品成功返回
			IBGoods_Id* msg = (IBGoods_Id*)uParam;
			if(msg)
			{
				KUiIBShopResultMessage::GetSingleton().SetBuySuccessMessage(msg);
			}
			KUiCreditShop::GetSingleton().RefreshVoucherCount();
		}
		break;
	case GDCNI_IBSHOP_SHELF:
		{
			KUiIBShop::GetSingleton().LoadShelf((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_CREDITSHOP_SHELF:
		{
			KUiCreditShop::GetSingleton().LoadShelf((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_POINTSHOP_SHELF:
		{
			KUiCreditShop::GetSingleton().LoadPointShopShelf((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_IBSHOP_PANEL:
		{
			KUiIBShop::GetSingleton().LoadPanel((BYTE*)uParam, (int)nParam);
			KUiCreditShop::GetSingleton().LoadPanel((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_IBSHOP_CONTENTSTYLE:
		{
			KUiIBShop::GetSingleton().LoadStyle((BYTE*)uParam, (int)nParam);
			KUiCreditShop::GetSingleton().LoadStyle((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_IBSHOP_GOODS:
		{
			KUiIBShop::GetSingleton().LoadGoodsInShelf((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_CREDITSHOP_GOODS:
		{
			KUiCreditShop::GetSingleton().LoadGoodsInShelf((BYTE*)uParam, (int)nParam);
		}
		break;
	case GDCNI_POINTSHOP_GOODS:
		{
			KUiCreditShop::GetSingleton().LoadGoodsInShelf((BYTE*)uParam, (int)nParam);
		}
		break;
		/************************************************************************/
		/*							弹出聊天窗口                                */
		/************************************************************************/

	case GDCNI_POPMESSAGE:
		{
			char* message = (char*)uParam;
			int len = strlen(message);
			KUiPopMessage* pop = new KUiPopMessage(UI_POPMESSAGE);
			Point* pos = (Point*)nParam;
			pop->show(message, *pos);
		}
		break;
		/************************************************************************/
		/*							弹出聊天窗口                                */
		/************************************************************************/
	case GDCNI_SET_MOUSECURSOR:
		{
			KUiAdapter::SetMouseRes( uParam );
		}
		break;
	case GDCNI_COMMSG:
		{
			char* message = (char*)uParam;
			KUiComMsgBox::Style style = (KUiComMsgBox::Style)nParam;
			KUiComMsgBox::GetSingleton().setStyle(style);
			KUiComMsgBox::GetSingleton().setMsg(message);
			KUiComMsgBox::GetSingleton().Show();
		}
		break;

	case GDCNI_MAKESURE_USEITEM:
		{
			char* message = (char*)uParam;
			
			KUiComMsgBox::GetSingleton().setModalStatus(true);
			KUiComMsgBox::GetSingleton().setComMsgPosition();
			KUiComMsgBox::Show();
			KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(message));
			KUiComMsgBox::GetSingleton().setFristBtnCallback(ConfirmUseItem);
			KUiComMsgBox::GetSingleton().setSecondBtnCallback(CannelUseItem);
			char yesString[COMMON_CLIENT_MSG_LEN_8];
			char noString[COMMON_CLIENT_MSG_LEN_8];
			strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
			strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
			KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
		}
		break;

	case GDCNI_NPC_TRADE:	
		{	
			ShopType type = (ShopType)uParam;
			if (type == ST_Item)
			{
				KUiAdapter::UiCloseNoNpcDlg();
				KUiShop::GetSingleton().show();
				////////////
				g_pCoreShell->OperationRequest(GOI_AUTO_SELL_ITEMS,0,0);
			}
			else
			{	
				KUiAdapter::UiCloseNoNpcDlg();
				KUiHire::Show();
			}
		}
		break;

	case GDCNI_UPDATA_PLUS_POINT:	
		{
			if (KUiItemBox::getSingleton().isVisible())
			{
				KUiItemBox::getSingleton().UpdateInsteadSpecie();
			}

			if ( KUiShop::IsVisible() )
			{
				KUiShop::GetSingleton().showCurPageByPlusPoint();
			}
		}
		break;

	case GDCNI_LEVELUPINFO:
		{
			//升级信息
 			KUiLevelUpInfo::Show();
 			KUiLevelUpInfo::Hide();
 			KUiLevelUpInfo::GetSingletonPtr()->GetLevelUpInfo( (const LevelUpAdd*)uParam );
// 
// 			KUiLevelUp::Show();
			
			KUiSystemMessage::SystemMessage msg;
			msg._id = -1;
			//msg._name = "";
			//strcpy(msg._name, (char*)nParam);
			msg._msgType = KUiSystemMessage::SystemMessage::LevelUp;
			KUiSystemMessage::GetSingleton().addMessage(msg);

		}
		break;
	case GDCNI_TOP_MESSAGE:
		{
			char* msg = (char*)uParam;
			if(msg)
			{
				KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(msg));
			}
		}
		break;
	case GDCNI_ERROR_MESSAGE:
		{
			char* msg = (char*)uParam;
			if(msg)
			{
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
			}
		}
		break;
	case GDCNI_IBCENTER_ERROR_MESSAGE:
		{
			char* msg = (char*)uParam;
			if(msg)
			{
				KUiIBShopResultMessage::GetSingleton().SetResultMessage(msg, nParam);
			}
		}
		break;
	case GDCNI_ERROR_MESSAGE_CODE:
		{
			if(uParam >= 0 && nParam >= 0 )
			{
				char *szMsg = KMessageCentre::GetMessage( uParam, nParam );
				if ( szMsg )
				{
					KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
				}
			}
		}
		break;
	case GDCNI_OPEN_STORE_BOX:
		{
			KUiAdapter::UiCloseNoNpcDlg();
			
			if ( uParam == 2 )
			{
				KUiItemPassword::Show();
			}
			else
			{
				KUiStoreBox::getSingleton().show();
			}
		}
		break;
	case GDCNI_STORE_BOX_CHANGE_PASSWORD_RESULT:
		{//等待删除2007-9-10
			char* message = NULL;
			if ( uParam != 0 )
			{
				message = KMessageCentre::GetMessage( storebox_error_message, 3 );
			}
			else
			{
				message = KMessageCentre::GetMessage( storebox_error_message, 4 );
			}
			if ( NULL != message )
			{
				KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( message ) );
			}

			KUiStoreBox::getSingleton().RefreshBtnModifyPasswordStates();
		}
		break;
	case GDCNI_STORE_BOX_UNLOCK_RESULT:
		{//等待删除2007-9-10
			char* message = NULL;
			if ( uParam != 0 )
			{
				message = KMessageCentre::GetMessage( storebox_error_message, 5 );
			}
			else
			{
				message = KMessageCentre::GetMessage( storebox_error_message, 6 );
			}
			if ( NULL != message )
			{
				KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( message ) );
			}
		}
		break;
	case GDCNI_ALERT_DURA:
		{
			KUiDuraAlert::getSingleton().updateState((EQUIP_DUR_STATE*)uParam);
		}
		break;
	case GDCNI_PLAYER_WEIGHT_CHANGED:
		{
			KUiItemBox::getSingleton().onWeighChanged(uParam, nParam);
		}
		break;
	case GDCNI_PLAYER_BAG_SIZED:
		{
			switch((INVENTORY_ROOM)uParam)
			{
			case room_equipment:
				KUiItemBox::getSingleton().onBagSized(nParam);
				break;
			case room_repository:
				KUiStoreBox::getSingleton().onBagSized(nParam);
				break;
			default:
				break;
			}
		}
		break;
	case GDCNI_RECIVE_TRADE_REQUEST:
		{
			KUiSystemMessage::SystemMessage msg;
			msg._id = uParam;
			strcpy(msg._name, (char*)nParam);
			msg._msgType = KUiSystemMessage::SystemMessage::TradeRequest;
			KUiSystemMessage::GetSingleton().addMessage(msg);
		}
		break;
	case GDCNI_RECIVE_TRADE_REFUSE:
		{
			char* name = (char*)nParam;
			char msg[COMMON_CLIENT_MSG_LEN_128];
			sprintf(msg, KUiCfgLoader::getSingleton().getTradeCfg().oppositeRefuseMsg, name);
			KUiTradeBox::GetSingleton().printSystemMessage(msg);
		}
		break;
	case GDCNI_TRADE_OPPOSITE_PICKUP_ITEM:
		{
			bool pickdrop = uParam == 0 ? false : true;
			int itemIndex = nParam;

			KUiTradeBox::GetSingleton().printOppoPickDropItem(pickdrop, itemIndex);
		}
		break;
		/************************************************************************/
		/*							显示吟唱条                                  */
		/************************************************************************/
	case CDCNI_CASTBAR_OPER:
		{
			CASTBAR_PARAM* param = (CASTBAR_PARAM*)uParam;
			if(param == NULL)
				break;

			switch(param->cmd)
			{
			case DA_New:
				{
					KUiCastBar::GetSingleton().cast(0, param->time, param->msgCode);
				}
				break;
			case DA_Delay:
				{
					KUiCastBar::GetSingleton().delay(param->time, param->msgCode);
				}
				break;
			case DA_Cancel:
				{
					KUiCastBar::GetSingleton().cancel();
					char *szMsg = KMessageCentre::GetMessage(cast_bar_message, KUiCastBar::CB_CODE_CANCEL);
					KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
				}
				break;
			case DA_Complete:
				{
					KUiCastBar::GetSingleton().complete();
				}
				break;
			}
		}
		break;
		/************************************************************************/
		/*							法宝属性变更                                */
		/************************************************************************/
	case CDCNI_TALISMAN_PROP_CHANGE:
		{
			int talismanOldId = (int)uParam;
			int talismanNewid = nParam;
			if(KUiTalisman::getSingleton().getEditTalismanId() == talismanOldId)
			{
				KUiTalisman::getSingleton().setEditTalismanId(talismanNewid);
				KUiTalisman::getSingleton().onTalismanPropChange();
			}
			break;
		}
		/************************************************************************/
		/*							法宝蕴魂变更                                */
		/************************************************************************/
	case CDCNI_TALISMAN_POTENTIAL_CHANGE:
		{
			int talismanId = (int)uParam;
			int curPotential = nParam;
			if(KUiTalisman::getSingleton().getEditTalismanId() == talismanId)
			{
				KUiTalisman::getSingleton().onTalismanPotentialChange(curPotential);
			}
			break;
		}
	
	case GDCNI_EQUIPMENT_VIEW_NOTIFY:
		{
			KUiTargetEquipment::GetSingleton().show();
			break;
		}

	case GDCNI_PLAYER_RUN:
		{
			closeUiWnd(PLAYER_RUN);
			break;
		}

	case GDCNI_APPEND_MESSAGE:
		{
			char* pMessage = (char*)uParam;
			int channelId = nParam;
			KUiChannelCentre::GetSingleton().recvCustomMessage(channelId, pMessage);
#ifdef		USING_CHAT_WINDOW
			B2ChatDialog::chatManager.ChatManagerReceiveCustomMsg(channelId,pMessage);
#endif
		}
		break;
	case GDCNI_PLAYER_ATTACK_NOTIFY:
		{
			KUiRoleFace::GetSingleton().PlayerAttackNotify();
		}
		break;
	case GDCNI_OPEN_SMITH_SHOP:
		{
			int shopId = (int)uParam;
			if(shopId == 4)
			{
				KUiItemOperPanel::getSingleton().show();
			}
			else
			{
				KUiSmith::getSingleton().open(shopId);
			}
		}
		break;
	case GDCNI_OPEN_NAVIGATION_WND:
		{
			if (KUiNaviation::IsVisible() == false)
			{
				KUiNaviation::Show();
			}
		}
		break;
	case GDCNI_OPEN_NAVIGATIONEX_WND:
		{
			if (KUiNaviationEx::IsVisible() == false)
			{
				KUiNaviationEx::Show();
			}
		}
		break;
	case GDCNI_OPEN_SHORTCUT_WND:
		{
			if (KUiShortcutWnd::IsVisible() == false)
			{
				KUiShortcutWnd::Show();
			}
		}
		break;
	case GDCNI_OPEN_SHORTCUTPLUS_WND:
		{
			if (KUiShortcutPlusWnd::IsVisible() == false)
			{
				KUiShortcutPlusWnd::Show();
			}
		}
		break;
	case GDCNI_ACTIVE_NAVIGATION_BUTTON:
		{
			KUiNaviation::GetSingleton().ActiveButton((int)uParam);
			KUiNaviationEx::GetSingleton().ActiveButton((int)uParam);
		}
		break;
	case GDCNI_END_SMITH:
		{
			int result = nParam;
			
			KUiItemOperPanel::getSingleton().onEndSmith(result);
			if(enchaser_error_no == result)
			{
				KUiSmith::getSingleton().onEndSmith();
			}
			else
			{
				KUiSmith::SmithCode msgCode;

				switch(result)
				{
				case enchaser_error_money:
					{
						msgCode = KUiSmith::smith_money_not_enough;
					}
					break;
				case enchaser_error_skillpoint:
					{
						msgCode = KUiSmith::smith_skill_exp_not_enough;
					}
					break;
				case enchaser_error_another_action:
					{
						msgCode = KUiSmith::smith_another_action_processing;
					}
					break;
				case enchaser_error_less_material:
					{
						msgCode = KUiSmith::smith_less_material;
					}
					break;
				case enchaser_error_conditionisinvalid:
					{
						msgCode = KUiSmith::smith_params_error;
					}
					break;
				case enchaser_error_ratedestroy:
					{
						msgCode = KUiSmith::smith_failure_destory;
					}
					break;
				case enchaser_error_not_enough_space:
					{
						msgCode = KUiSmith::smith_failure_not_enough_space;
					}
					break;
				default:
					{
						msgCode = KUiSmith::smith_failure_not_destory;
					}
					break;
				}
				char *szMsg = KMessageCentre::GetMessage(smith_message, msgCode);
				KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
			}
		}
		break;
	case GDCNI_QUEST_INFO_CHANGED:
		{
			int questId = (int)uParam;
			KUiFSBible::getSingleton().onQuestChange(questId);
			KUiQuestManage::GetSingleton().onQuestChange(questId);
			KUiQuestTrack::GetSingleton().freshTrackList();
		}
		break;
	case GDCNI_BEGIN_AUTO_PATH:
		{
			KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find("AutoPath");
			if(NULL == light)
			{
				break;
			}
			light->lightup();
		}
		break;
	case GDCNI_STOP_AUTO_PATH:
		{
			KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find("AutoPath");
			if(NULL == light)
			{
				break;
			}
			light->terminate();
		}
		break;
	case GDCNI_BEGIN_AUTO_ATTACK:
		{
			KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find("AutoAttack");
			if(NULL == light)
			{
				break;
			}
			light->lightup();
		}
		break;
	case GDCNI_STOP_AUTO_ATTACK:
		{
			KUiTrafficLight* light = KUiTrafficLightManager::getSinglton().find("AutoAttack");
			if(NULL == light)
			{
				break;
			}
			light->terminate();
		}
		break;

		
		//TaisuiSys
	case GDCNI_TIAN_XIANG_CHANGED:
		{
			KTaisuiWnd::UpdateTianXiang();
		}
		break;
		
	case GDCNI_TAISUI_WHEEL_TIANGAN_RES:
		{
			KTaisuiWnd::UpdateTianGanRes();
		}
		break;
		
	case GDCNI_TAISUI_WHEEL_DIZHI_RES:
		{
			KTaisuiWnd::UpdateDizhiRes();
			if (nParam)
			KTaisuiWnd::NotifyEventShow();
			if (uParam)
			KTaisuiWnd::NotifyGiftShow(uParam);
		}
		break;
		
	case GDCNI_JIAZI_EVENT_CHANGED:
		{
			KTaisuiWnd::UpdateJiaziEvent();
		}
		break;
		
	case GDCNI_WHEEL_TIMES_CHANGED:
		{
			KTaisuiWnd::UpdateTimesInfo();
		}
		break;
		
	case GDCNI_TAISUI_DLG_OPEN:
		{
            if (!KTaisuiWnd::IsVisible())
				KTaisuiWnd::Show();
		}	
		break;
		
	case GDCNI_TAISUI_DLG_CLOSE:
		{
            if (KTaisuiWnd::IsVisible())
				KTaisuiWnd::Hide();
		}
		break;

	case GDCNI_PROMPT_ADD_BUFF:
		{
			char* pMsg = (char*)uParam;
			if (pMsg)
			{
				char acceptBtn[COMMON_CLIENT_MSG_LEN_32] = { 0 };
				char refuseBtn[COMMON_CLIENT_MSG_LEN_32] = { 0 };
				snprintf(acceptBtn, sizeof(acceptBtn), ACCEPT_BIG5 );
				snprintf(refuseBtn, sizeof(refuseBtn), CANNEL_BIG5);
				
				KUiComMsgBox::GetSingleton().Show();
				KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(pMsg));
				KUiComMsgBox::GetSingleton().setBtnName(AnsiToUtf8(acceptBtn), AnsiToUtf8(refuseBtn));
				KUiComMsgBox::GetSingleton().setFristBtnCallback(AcceptAddBuff);
				KUiComMsgBox::GetSingleton().setSecondBtnCallback(RefuseAddBuff);
			}
		}
		break;	
	case GDCNI_CANCEL_PROMPT_ADD_BUFF:
		{
			if (KUiComMsgBox::GetSingleton().IsVisible())
				KUiComMsgBox::GetSingleton().Hide();
		}
		break;
	case GDCNI_OPEN_WINDOW:
		{
			WindowName wndName = (WindowName)nParam;
			KUiWndMgr::getSingleton().openAWindow(wndName);
		}
		break;
	case GDCNI_OPEN_TIMER:
		{
			pair<int, int>* timerArgs = (pair<int, int>*)uParam;
			char* content = (char*)nParam;
			KUiTimer::getSingleton().openTimer(timerArgs->first, content, timerArgs->second);
		}
		break;
	case GDCNI_FURY_CHANGE:
		{
			if (KUiFuryBox::IsVisible())
				KUiFuryBox::Update(uParam);
		}
		break;
	case GDCNI_AUTO_ATTACK_BLAST:
		{
			if (KUiFuryBox::IsVisible())
			{
				KUiFuryBox::AutoFury();
			}
		}
		break;
	case GDCNI_AUTO_REPAIR:
		{
			if (KUiShop::IsVisible())
			{
				KUiShop::GetSingleton().RepairAllItem();
			}
		}
		break;
	case GDCNI_FURY_WARNNING:
		{
			if (KUiFuryBox::IsVisible())
				KUiFuryBox::Warnning(uParam);
		}
		break;
	case GDCNI_PET_CALLED_OUT:
		{
			char* name = (char*)uParam;
			int petNpcIndex = nParam;
			KUiSelfPetFrame::getSingleton().show(name, petNpcIndex);
		}
		break;
	case GDCNI_PET_RELEASE:
		{
			KUiSelfPetFrame::getSingleton().hide();
		}
		break;
	case GDCNI_PET_UPDATE:
		{
			pair<int, int>* blood = (pair<int, int>*)uParam;
			if(!blood)
			{
				break;
			}

			int curBlood = blood->first;
			int maxBlood = blood->second;

			int npcIndex = nParam;
			if(KUiSelfPetFrame::getSingleton().getNpcIndex() == npcIndex)
			{
				KUiSelfPetFrame::getSingleton().update(curBlood, maxBlood);
			}//endif
		}
		break;
		//end for case
	case GDCNI_OPEN_TONG_RECRUIT:
		{
			KUiTongRecruitCentre::Show();
		}//end for case 
		break; 
	case GDCNI_OPEN_INSTANCE_REWARD:
		{
			KUiRandomCopyRewards::GetSingleton().ShowRewards(*((KUiInstanceReward*)uParam));
		}
		break;
		//end for case
	case GDCNI_OPEN_CREDIT_SHOP:
		{
			KUiCreditShop::Show();
		}
		break;
		//end for case 

	case GDCNI_UPDATA_TONG_RECRUIT:
		{
            KUiTongRecruitCentre::UpdateData(*((TongRecruitData *)uParam));
		}
		break;

	case GDCNI_UPDATA_SOCIAL_INFO:
		{
			TongInfoData *        pTongData = (TongInfoData *)uParam;

			if (pTongData->flag.m_IdentiyFlag & TIUI_SOCIAL_INFO)
			{
				KUiSocialInfo::Updata(*pTongData);
			}//endif

			if (pTongData->flag.m_IdentiyFlag & TIUI_EX_CHAT)
			{
//				KUiSocialInfo::Updata(*pTongData);
//				PlayerShowInfo::GetSingle().GetInfo(pTongData);
			}//endif
				
		}
		break;
	case GDCNI_CHAT_MINI_MAP_UPDATA:
		{
			ChatMiniMap::GetSingle().Update();
		}
		break;
	case GDCNI_SPECIAL_QUEST_DATA:
		{
			ChangedSpecialQuestData* questData = (ChangedSpecialQuestData*)uParam;
			int questId = nParam;
			KUiFSBibleSpecialQuestData::getSingleton().updateData(questId, *questData);
		}
		break;
	case GDCNI_GM_FEED_BACK:
		{
			GMCommunicationData* gmFeedBackMsg = (GMCommunicationData*)uParam;
			if(gmFeedBackMsg)
			{
				KUiGMCommunication::getSingleton().setGMMsg(gmFeedBackMsg->msg);
				KUiGMCommunication::getSingleton().showNewQuestion();
				if(!KUiGMCommunication::getSingleton().isVisible())
				{
					KUiSystemMessage::SystemMessage msg;
					msg._msgType = KUiSystemMessage::SystemMessage::GMFeedBack;
					KUiSystemMessage::GetSingleton().addMessage(msg);
				}
			}
		}
		break;
	case GDCNI_RECV_HIRE_DATA_EXP:
		{
			vector<ExpHirer>* data = (vector<ExpHirer>*)uParam;
			int startIndex = nParam;
			if(data)
			{
				KUiHire::GetSingleton().updateExp(*data, startIndex);
			}
		}
		break;
	case GDCNI_RECV_HIRE_DATA_FIGHT:
		{
			vector<FighterHirer>* data = (vector<FighterHirer>*)uParam;
			int startIndex = nParam;
			if(data)
			{
				KUiHire::GetSingleton().updateFighter(*data, startIndex);
			}
		}
		break;
	case GDCNI_HIRE_REQ_RET:
		{
			HireRetCode retCode = (HireRetCode)nParam;
			switch(retCode)
			{
			case HRC_HIRE_SUCCESS:
				{
					KUiHire::Hide();
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_SUCCESS);
 					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_BE_HIRED_SUCCESS:
				{
// 					if(KUiDeleyQuit::GetSingleton().GetQuitState() == TOMAINBEGIN)
// 					{
// 						KUiDeleyQuit::GetSingleton().Quit();
// 					}
// 					else
					{
						KUiDeleyQuit::GetSingleton().QuitToSelectRole();
					}
				}
				break;
			case HRC_SEARCH_NO_RECORD:
				{
					KUiHire::GetSingleton().clear();
 					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_NO_RECORD);
 					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_OPERATE_TOO_FAST:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_OPERATE_TOO_FAST);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_ALREADY_HAS_EMPLOYEE:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_ALREADY_HAS_EMPLOYEE);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYEE_OUT_OF_EMPLOY_TIME:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYEE_OUT_OF_EMPLOY_TIME);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYER_OUT_OF_EMPLOY_TIME:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_OUT_OF_EMPLOY_TIME);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYER_OUT_OF_MONEY:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_OUT_OF_MONEY);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_NO_EMPLOYEE:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_NO_EMPLOYEE);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYEE_ON_LINE:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYEE_ON_LINE);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYER_NOT_ENOUGH_EMPLOY_TIME:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_NOT_ENOUGH_EMPLOY_TIME);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYER_NOT_ENOUGH_MONEY:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_NOT_ENOUGH_MONEY);
					KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
				}
				break;
			case HRC_EMPLOYEE_FIGHT_MODE_LEVEL_REQUIRED:
				{
					char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYEE_FIGHT_MODE_LEVEL_REQUIRED);
					
					KUiComMsgBox::Show();
					KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(message));
					char	yesButton[COMMON_CLIENT_MSG_LEN_32];
					strcpy(yesButton, KMessageCentre::GetMessage( friend_message, KUiChatCentre::YES ));
					char	*noButton = KMessageCentre::GetMessage( friend_message, KUiChatCentre::NO );
					KUiComMsgBox::GetSingleton().setBtnName( AnsiToUtf8(yesButton), AnsiToUtf8(noButton));
				}
				break;
			default:
				{
					
				}
				break;
			}
		}
		break;
	case GDCNI_RECV_JINSHANBI:
		{
			KUiIBShop::GetSingleton().SetJinShanBi(uParam);
			KUiCreditShop::GetSingleton().SetJinShanBi(uParam);
		}
		break;
	case GDCNI_RECV_CREDITPOINT:
		{
			KUiIBShop::GetSingleton().SetCredit(uParam);
			KUiCreditShop::GetSingleton().SetCredit(uParam);
		}
		break;
	case GDCNI_RECV_POINT:
		{
			KUiIBShop::GetSingleton().SetPoint(uParam);
			KUiCreditShop::GetSingleton().SetPoint(uParam);
		}
		break;
	case GDCNI_RECV_MAXCREDITPOINT:
		{
			KUiIBShop::GetSingleton().SetMaxCreditPoint(uParam);
			KUiCreditShop::GetSingleton().SetMaxCreditPoint(uParam);
		}
		break;
	case GDCNI_RECV_CREDITSTATE:
		{
			KUiIBShop::GetSingleton().SetCreditStatus(uParam);
			KUiCreditShop::GetSingleton().SetCreditStatus(uParam);
		}
		break;
	case GDCNI_RECV_CREDITRETURNDATA:
		{
			KUiIBShop::GetSingleton().SetCreditReturnTime(uParam);
			KUiCreditShop::GetSingleton().SetCreditReturnTime(uParam);
		}
		break;

	case GDCNI_RECV_WORLD_COMBAT_SCORE:
		{
			if (nParam == 1)
			{
				KUiWorldCombatInfo::Show();
				KUiWorldCombatInfo::Update((const WorldCombatUIParam *)uParam);
			}//endif
			else
			{
				KUiWorldCombatInfo::Hide();
			}//endif

		}
		break;
	case GDCNI_DEBUG_TRACK_INJECT:
		{
			bool track = (bool)nParam;
			System::getSingleton().setTrackInject(track);
		}
		break;
	case GDCNI_DEBUG_PRINT_WINDOW:
		{
			const char* windowName = (const char*)uParam;
			PrintWindowProperty(windowName);
		}
		break;
	case GDCNI_LIST_STUDENT:
		{
			RecommedList* studentList = (RecommedList*)uParam;
			RecommendRewardInfo* pRewardInfo = (RecommendRewardInfo*)nParam;
			if(studentList)
			{
				KUiRecommend::getSingleton().show(*studentList, pRewardInfo->TotalRewardToAdd, pRewardInfo->TotalRewardTicketAdded);
			}
		}
		break;
	case GDCNI_OPEN_RECOMMEND_REPORT:
		{
			char* mastName = (char*)uParam;
			pair<int, int>* level = (pair<int, int>*)nParam;
			KUiRecommend::getSingleton().showReport(mastName, level->first, level->second);
		}
		break;
	case GDCNI_COMBAT_TOP_MEMBER_INFO:
		{
			UICombatTopMemberInfo* pCombatTopMemberInfo = (UICombatTopMemberInfo*)uParam;
			int topMemberCount = nParam;
			//Add By DarkMagic(DuanMu)
//			KUiBattleResult::GetSingleton().RefreshList(pCombatTopMemberInfo, topMemberCount);
		}
		break;
	case GDCNI_ASK_QUESTION:
		{
			KUiQuestionWindow::GetSingleton().ShowQuestion( uParam, nParam );
		}
		break;

	//经验保险相关
	case GDCNI_EXP_INSURANCE_STATE_NOTIFY:
		{
			KUiRoleExp::GetSingleton().RefreshTip();
		}
		break;
	case GDCNI_EXP_INSURANCE_REWARD_NOTIFY:
		{
			KUiRoleExp::GetSingleton().UpdateRewardExp();
		}
		break;
	case GDCNI_EXP_INSURANCE_REWARD:
		{
			KUiRewardExpNotify::GetSingleton().Notify( uParam, -1 );
		}
		break;
	case GDCNI_EXP_INSURANCE_ENTER_STATE:
		{
			KUiRoleExp::GetSingleton().RefreshTip();
			char* message = KMessageCentre::GetMessage( common_message, KUiRoleExp::CM_REWARDEXP_ENTER_LEVEL );
			if ( message )
			{
				KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( message ) );
			}
		}
		break;

	//任务保险相关
	case GDCNI_QUEST_INSURANCE_STATE_NOTIFY:
		{
			KUiRoleExp::GetSingleton().RefreshTip();
		}
		break;
	case GDCNI_QUEST_INSURANCE_REWARD_NOTIFY:
		{
			KUiRoleExp::GetSingleton().UpdateRewardTime();
		}
		break;
	case GDCNI_QUEST_INSURANCE_REWARD:
		{
			KUiRewardExpNotify::GetSingleton().Notify( -1, uParam );
		}
		break;
	case GDCNI_QUEST_INSURANCE_ENTER_STATE:
		{
			KUiRoleExp::GetSingleton().RefreshTip();
			char* message = KMessageCentre::GetMessage( common_message, KUiRoleExp::CM_REWARDTIME_ENTER_LEVEL );
			if ( message )
			{
				KUiErrorMessageBox::GetSingleton().AddMessage( AnsiToUtf8( message ) );
			}
		}

	case GDI_EXP_QUEST_INSURANCE_LEVEL_UP_NOTIFY:
		{
			KUiRoleExp::GetSingleton().UpdateRewardExp();
		}
		break;
		
	case GDCNI_COMPOUND_NEWITEM_NOTIFY:
		{
			KUiCompound::GetSingleton().AutoAddNewItem( uParam, (ItemPos*)nParam );
		}
		break;

	case GDCNI_UPDATE_SMALL_BATTLE_FIELD_RESULT:
		{
			KUiSmallBattleFieldResult::GetSingleton().ShowResult( reinterpret_cast< SMALL_BATTLE_FIELD_RESULT* >( uParam ) );
		}
		break;
	case GDCNI_MAP_PRONUNCIAMENTO_STR:
		{
			char * msg = (char *)uParam;
			if (msg != NULL)
			{
				KUiShizuBanner::getSingleton().ShowPronunciamento(msg, nParam);
			}
		}
		break;
	case GDCNI_STATUE_INFO:
		{
			KUiTongStatueMsg::GetSingleton().ShowStatueMsg( reinterpret_cast< UIStatueInfo* >( uParam ) );
		}
		break;
	case GDCNI_TITLEINFO_UPDATA:
		{
			KUiEquipment::GetSingleton().UpdataTitleInfo();
		}
		break;
	case GDCNI_SELF_PROPERTIES_UPDATE:
		{
			KUiEquipment::GetSingleton().UpdataAttribute( reinterpret_cast< UiPlayerProperties* >( uParam ) );
		}
		break;
	case GDCNI_PLAYER_PROPERTIES_UPDATE:
		{
			KUiTargetEquipment::GetSingleton().UpdataAttribute( reinterpret_cast< UiPlayerProperties* >( uParam ) );
		}
		break;
	case GDCNI_PLAYER_REAL_INFO:
		{
			const UIPlayerRealInfoEx* info = reinterpret_cast< UIPlayerRealInfoEx* >( uParam );
			KUiGenPersonalInfo::GetSingleton().UpdateInfo( *info );
		}
		break;
	case GDCNI_POINTLIST_REFRESH:
		{
			KUiPointListCharts::GetSingleton().Refresh( uParam );
		}
		break;
	case GDCNI_SHIZU_POPULARITY_UPDATE:
 		{
			KUiPointListCharts::GetSingleton().Refresh( uParam );
		}
		break;
	case GDCNI_ZHUHOU_POPULARITY_UPDATE:
		{
			KUiPointListCharts::GetSingleton().Refresh( uParam );
		}
		break;
	case GDCNI_COMBAT_KILL_RANK_UPDATE:
		{
			KUiPointListCharts::GetSingleton().Refresh( uParam );
		}
		break;
	case GDCNI_PLAY_ANIMATION:
		{
			Play_Animation * pData = (Play_Animation *)uParam;
			if (g_GetScreenWidth() == 1024)
			{
				g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT, pData->m_AnimationId1, nParam);
				g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT, pData->m_AnimationId2, nParam);
				g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT, pData->m_AnimationId3, nParam);
			}
			else
			{
				g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT, pData->m_AnimationId4, nParam);
			}
		}
		break;
	}
}

