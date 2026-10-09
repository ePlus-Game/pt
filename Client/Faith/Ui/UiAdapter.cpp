//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/14/2006 10:13
//      File_base        : UiAdapter
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 对 CEGUI 简单的应用封装
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KMusic.h"
#include "KWin32Wnd.h"
#include "UiAdapter.h"
#include "iRepresentShell.h"
#include "CoreUseNameDef.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "CoreShell.h"
#include "iRepresentShell.h"
#include "Ui\ShortcutKey.h"
#include "CEGUI.h"
#include "TLTooltip.h"
#include "UiMDLInterface.h"
#include "cfs_filelogs.h"
#include "Ui\UiCase\UiLoginBg.h"
#include "Ui\UiCase\UiUpdateTip.h"
#include "Ui\UiCase\UiLogin.h"
#include "Ui\UiCase\UiSelPlayer.h"
#include "Ui\UiCase\UiNewPlayer.h"
#include "Ui\UiCase\UiChatCentre.h"
#include "Ui\UiCase\UiToolsControlBar.h"
#include "Ui\UiCase\UiMessageBox.h"
#include "Ui\UiCase\UiESCDlg.h"
#include "Ui\UiCase\UiRoleFace.h"
#include "Ui\UiCase\UiShortcutWnd.h"
#include "Ui\UiCase\UiChatWindow.h"
#include "Ui\UiCase\UiMailCentre.h"
#include "Ui\UiCase\UiMapCentre.h"
#include "Ui\UiCase\UiBufferWnd.h"
#include "Ui\UiCase\UiDeathBox.h"
#include "Ui\UiCase\UiDebufferWnd.h"
#include "Ui\UiCase\UiTargetFace.h"
#include "Ui\UiCase\UiTopMessage.h"
#include "Ui\UiCase\UiEquipment.h"
#include "Ui\UiCase\UiPKFilter.h"
#include "Ui\UiCase\UiNpcMsgBox.h"
#include "Ui\UiCase\UiQuestManage.h"
#include "Ui\UiCase\UiComMsgBox.h"
#include "Ui\UiCase\UiItemBox.h"
#include "Ui\UiCase\UiHelpCentre.h"
#include "Ui\UiCase\UiDragItem.h"
#include "Ui\UiCase\UiTradeBox.h"
#include "Ui\UiCase\UiSplitItemBox.h"
#include "Ui\UiCase\UiTargetbufferWnd.h"
#include "Ui\UiCase\UiPlayerState.h"
#include "Ui\UiCase\UiTradeConfirmBox.h"
#include "Ui\UiCase\UiTeamList.h"
#include "Ui\UiCase\UiCompound.h"
#include "Ui\UiCase\UiShop.h"
#include "Ui\UiCase\UiWorldCombatInfo.h"
#include "Ui\UiCase\UiErrorMessageBox.h"
#include "Ui\UiCase\UiItemTip.h"
#include "Ui\UiCase\UiLinkedItemTip.h"
#include "Ui\UiCase\UiStoreBox.h"
#include "Ui\UiCase\UiDuraAlert.h"
#include "Ui\UiCase\UiSystemMessage.h"
#include "Ui\UiCase\UiVendueWnd.h"
#include "Ui\UiCase\UiTongCreate.h"
#include "Ui\UiCase\UiTongManager.h"
#include "Ui\UiCase\UiCityManager.h"
#include "Ui\UiCase\UiCastBar.h"
#include "Ui\UiCase\UiTalisman.h"
#include "Ui\UiCase\UiTargetEquipment.h"
#include "Ui\UiCase\UiShortcutPlusWnd.h"
#include "Ui\UiCase\UiPlayerMenu.h"
#include "Ui\UiCase\UiSmith.h"
#include "Ui\UiCase\UiQuestTrack.h"
#include "Ui\UiCase\UiHelpInfo.h"
#include "Ui\UiCase\UiAutoConnect.h"
#include "Ui\UiCase\UiGameSetting.h"
#include "Ui\UiCase\UiStudySkillManage.h"
#include "Ui\UiCase\UiWaitingMsg.h"
#include "Ui\UiCase\UiDelcomfirm.h"
#include "Ui\UiCase\UiLevelUp.h"
#include "Ui\UiCase\UiLevelUpInfo.h"
#include "Ui\UiCase\UiDelayQuit.h"
#include "Ui\UiCase\UiTargetMenu.h"
#include "Ui\UiCase\UiRoleHead.h"
#include "Ui\UiCase\UiChangeMapWnd.h"
#include "CEGUISoundSetManager.h"
#include "Ui\UiCase\UiBubble.h"
#include "Ui\UiCase\UiBeginHelp.h"
#include "Ui\ShortcutKey.h"
#include "Ui\UiCase\UiRoleFacePopMenu.h"
#include "Ui\UiCase\UiRoleExp.h"
#include "Ui\UiCase\UiRaid.h"
#include "Ui\UiCase\UiTaisuiWnd.h"
#include "Ui\UiSheetMgr.h"
#include "UiGlobalEvent.h"
#include "UI\UiCase\UiSearchHelpWnd.h"
#include "Ui\UiCase\UiElf.h"
#include "Ui\UiCase\UiQueryWnd.h"
#include "Ui\UiCase\UiIBShop.h"
#include "Ui\UiCase\UiPathHelp.h"
#include "Ui\UiCase\UiFuryBox.h"
#include "Ui\UiCase\UiPetFrame.h"
#include "Ui\UiCase\UiTeamViewer.h"
#include "Ui\UiCase\UiTongRecruitCentre.h"
#include "Ui\UiCase\UiShizuBanner.h"
#include "Ui\UiCase\UiShortcutKeySetting.h"
#include "Ui\UiCase\UiRandomCopyRewards.h"
#include "Ui\UiCase\UiFSBible.h"
#include "Ui\UiCase\UiNpcNavigation.h"
#include "Ui\UiCase\UiGMCommunication.h"
#include "Ui\UiCase\UiHire.h"
#include "Ui\UiCase\UiCreditShop.h"
#include "ui/UiCase/UiItemOperPanel.h"
#include "ui/UiCase/UiIEWindow.h"
#include "ui\UiCase\UiRecommend.h"
#include "ui\UiCase\UiServerList.h"
#include "ui\UiCase\UiIEWindow.h"
#include "ui\UiCase\UiPetFrame.h"
#include "ui/UiCase/UiChatConfig.h"
#include "Ui\UiCase\UiGenPersonalInfo.h"

///////////////zpc insert////////////
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatMainDlg.h"

#include "chatWindow/ChatControlPanel.h"
#include "Ui/UiCase/UiBattleResult.h"
#include "Ui/UiCase/UiEntrustComputer.h"
#include "Ui/UiCase/UiInfoBar.h"
#include "ui/UiCase/UiQuestionWindow.h"
#include "Ui/UiCase/UiItemPassword.h"
#include "../NetConnect/NetConnectAgent.h"
#include "Ui/UiCase/UiPointListCharts.h"
#include "Ui/UiCase/UiRankButton.h"

#include "KColors.h"

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;
extern KMusic*			g_pMusicShell;
extern bool				g_bShowServerList;
extern BOOL				g_updateRet;
extern HANDLE			g_updateEvent;

KUiAdapter::KUiAdapter()
: m_bHIMC(false)
{
	ms_This				= this;
	m_pSound			= NULL;
	m_pRenderer			= NULL;
	m_pCEGUISystem		= NULL;
	m_pWindowManager	= NULL;
	m_bMouseInWindow	= FALSE;
	m_nFrameRate		= GAME_MAX_FPS;
	m_dwPing			= 0;
	m_fOldTimeElapsed	= 0;
	m_bHideUi			= false;
	m_hIMC              = 0;
}

KUiAdapter*	KUiAdapter::ms_This = NULL;
int	KUiAdapter::m_nCurMouse = MOUSE_CURSOR_NORMAL;

KUiAdapter::~KUiAdapter()
{
	//Destory shortcut key centre.
	KShortcutKeyCentre::UninitScript();
}

void KUiAdapter::UiStartGame( void )
{
	g_NetConnectAgent.RegisterMsgTargetObject( s2c_byte_extend,		NULL				);
	//解决一进入游戏就会出现tip的问题
	RECT rt;
	::GetWindowRect( g_GetMainHWnd(), &rt );
	::SetCursorPos(rt.left + (rt.right - rt.left) / 2, rt.top + (rt.bottom - rt.top) / 2 );

	if ( ms_This == NULL )
	{
		return;
	}

	if ( CEGUI::ImagesetManager::getSingleton().isImagesetPresent( UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0 ) )
	{
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_CREATE_JS_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_CREATE_JS_1 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_CREATE_DS_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_CREATE_DS_1 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_CREATE_YR_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_CREATE_YR_1 ) );

		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_SEL_JS_1  ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_SEL_JS_0  ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_SEL_DS_1 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_SEL_DS_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_SEL_YR_1 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset(  UI_LOGINBK_IMAGESET_NAME_SEL_YR_0 ) );
		
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset( UI_LOGINBK_IMAGESET_NAME_CLICK_JS_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset( UI_LOGINBK_IMAGESET_NAME_CLICK_JS_1 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset( UI_LOGINBK_IMAGESET_NAME_CLICK_DS_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset( UI_LOGINBK_IMAGESET_NAME_CLICK_DS_1 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset( UI_LOGINBK_IMAGESET_NAME_CLICK_YR_0 ) );
		CEGUI::ImagesetManager::getSingleton().destroyImageset( CEGUI::ImagesetManager::getSingleton().getImageset( UI_LOGINBK_IMAGESET_NAME_CLICK_YR_1 ) );
	}

	ms_This->StopTitleMusic();

	KUiMiniMap::Show();
	KUiShortcutPlusWndShowHide::Show();
	KUiExtendChatWndBtn::Show();
	KUiEntrustComputerBtn::Show();
	//KUiElfBtn::Show();
	KUiElf::Show();
	KUiShortcutPlusWnd::Show();
	KUiChannelCentre::Show();//*/
	KUiDragItem::Show();
	KUiDragItem::GetSingleton().initItem();
	//KUiDragItem::GetSingleton().setVisible(false);
	KUiTopMessage::Show();
	KUiSystemMessage::Show();
	KUiMiniNaviation::Show();
 	KUiNaviation::Show();
	KUiNaviationEx::Show();
	KUiTargeMenu::Show();
	KUiTargeMenu::Hide();
	KUiRoleFace::Show();
	KUiRoleExp::Show();
	KUiBufferCentre::Show();
	KUiDebufferCentre::Show();
	KUiRoleFacePopMenu::Show();
	KUiRoleFacePopMenu::Hide();
	//KUiBeginHelp::GetSingleton().setAlwayTop(true);
	//KUiBeginHelp::Show();
	KUiChatCentre::GetSingleton();
	GlobalEventSet::getSingleton().subscribeEvent(TLGameObject::EventMouseMove, 
		Event::Subscriber(&KUiDragItem::onMouseMove, &KUiDragItem::GetSingleton()));

	KUiSearchHelpWnd::Show();
	KUiSearchHelpWnd::Hide();
	//KUiElf::Show();
	//KUiElf::Hide();
	
	KUiFuryBox::Show();
	KUiExit::Hide();
	KUiElfPopMenu::Show();
	KUiElfPopMenu::Hide();
	KUiIBShop::Hide();
	KUiCreditShop::Hide();
	KUiPathHelp::Hide();
	KUiIBNavigation::Show();
	KUiRankButton::Show();
	KUiCreditShopNavigation::Show();
	
	BOOL b = KShortcutKeyCentre::InitScript();
	if(!b)
	{
		CFS_FILELOGS::WriteLog("shortcut key load fail...\n");
	}

	KUiItemTip::GetSingleton().Hide();
	KUiLinkedItemTip::GetSingleton().Hide();
	KUiShortcutWnd::Show();//*/
	KUiTongRecruitCentre::Hide();
	KUiSocialInfo::Hide();
	KUiSKSetting::getSinglton().show();
	KUiSKSetting::getSinglton().hide();

	KUiHire::Show();
	KUiHire::Hide();
	KUiHireConfigExp::Show();
	KUiHireConfigExp::Hide();
	KUiHireConfigSalary::Show();
	KUiHireConfigSalary::Hide();

	KUiIBQuantityInput::Show();
	KUiIBQuantityInput::Hide();

	KUiRepayConfirm::Show();
	KUiRepayConfirm::Hide();

	KUiIBShopResultMessage::Show();
	KUiIBShopResultMessage::Hide();

	KUiEntrustComputer::GetSingleton().ResetAll();

	KUiEntrustSkill::Show();
	KUiEntrustSkill::Hide();

	KUiInfoBarPing::Show();
	KUiInfoBarTime::Show();
	KUiChatRoomListMenu::Show();
	KUiChatRoomListMenu::Hide();
}

void KUiAdapter::UiEndGame( void )
{
	if ( ms_This == NULL )
	{
		return;
	}
	ms_This->PlayTitleMusic();
	ms_This->InitMusicVolume();

	SetMouseRes( MOUSE_CURSOR_NORMAL );

	KUiNaviation::Hide();
	KUiNaviationEx::Hide();
	KUiRoleFace::Hide();
	KUiRoleExp::Hide();
	KUiTargetFace::Hide();
	KUiShortcutWnd::Hide();
	KUiShortcutPlusWndShowHide::Hide();	
	KUiExtendChatWndBtn::Hide();	
	KUiEntrustComputerBtn::Hide();
	KUiElfBtn::Hide();
	KUiShortcutPlusWnd::Hide();
	KUiChannelCentre::Hide();
	KUiMiniNaviation::Hide();
	KUiChatNotify::Hide();
	KUiMailCentre::Hide();
	KUiSceneMap::getSinglton().hide();
 	KUiMiniMap::Hide();
	KUiBigMap::getSinglton().hide();
	KUiBufferCentre::Hide();//
	KUiDeathBox::Hide();
	KUiDebufferCentre::Hide();
	KUiTopMessage::Hide();
	KUiPKFilter::Hide();
	KUiNpcMsgBox::Hide();
	KUiComMsgBox::Hide();
	KUiDragItem::Hide();
	KUiTargetbufferCentre::Hide();
	KUiTradeConfirmBox::Hide();
	KUiTeamList::SetDisapearFlag();
	KUiTeamList::Hide();
	KUiShop::Hide();
	KUiCompound::Hide();
	KUiErrorMessageBox::GetSingleton().hide();
	KUiItemTip::Hide();
	KUiLinkedItemTip::Hide();
	KUiChatCentre::Hide();
	KUiSystemMessage::Hide();
	KUiChatInputWnd::Hide();
	KUiVendueWnd::Hide();
	KUiTongCreate::Hide();
	KUiCityManager::Hide();
	KUiCityResMgr::Hide();
	KUiTongOperMgr::Hide();
	KUiTongStatueMsg::Hide();
	KUiCastBar::Hide();
	KUiTalisman::getSingleton().hide();
	KUiTargetEquipment::Hide();
	KUiPlayerMenu::Hide();
	KUiSmith::getSingleton().hide();
	KUiQuestTrack::Hide();
	KUiAutoConnect::Hide();
	KUiStudySkillManage::Hide();
	KUiWaitingMsg::GetSingleton().QuitMsg();
	KUiDelComfirm::Hide();
	KUiLevelUp::Hide();
	KUiLevelUpInfo::Hide();
	KUiDeleyQuit::Hide();
	KUiTargeMenu::Hide();
	KUiRoleFacePopMenu::Hide();
	UiCloseNoNpcDlg();
	KTaisuiWnd::Hide();
	KUiSearchHelpWnd::Hide();
	KUiQueryWnd::Hide();
	ChatMainDlg::MainDlgShowWndChat(false);
	B2ChatDialog::chatManager.ChatManagerClearInfoWnd();
	KUiTeamHideShow::Hide();
	KUiIBShop::Hide();
	KUiCreditShop::Hide();
	KUiExit::Hide();
	KUiPathHelp::Hide();
	KUiIBNavigation::Hide();
	KUiRankButton::Hide();
	KUiCreditShopNavigation::Hide();
	KUiFuryBox::Hide();
	KUiSelfPetFrame::getSingleton().hide();
	KUiTeamViewer::Hide();
	KUiTongRecruitCentre::Hide();
	KUiSocialInfo::Hide();
	KUiShizuBanner::getSingleton().clean();
	KUiDuraAlert::getSingleton().hide();
	KUiChanConfig::getSingleton().hide();
	KUiRandomCopyRewards::GetSingleton().Hide();
	KUiElfPopMenu::Hide();
	KUiElf::Hide();
	KUiHire::Hide();
	KUiHireConfigExp::Hide();
	KUiHireConfigSalary::Hide();
	KUiFSBible::getSingleton().hide();
	KUiRecommend::getSingleton().hide();
	//KUiServerList::Hide();
	KUiSelfPetFrame::getSingleton().hide();
	//Add by DarkMagic(DuanMu)
//	KUiBattleResult::Hide();
	KUiInfoBarPing::Hide();
	KUiInfoBarTime::Hide();
	KUiQuestionWindow::Hide();
	KUiRewardExpNotify::Hide();

	KUiItemPassword::Hide();
	KUiItemPassword_Create::Hide();
	KUiItemPassword_Modify::Hide();
	
	KUiSmallBattleFieldResult::Hide();

	KUiEntrustComputer::GetSingleton().ClosePanel();
	KUiEntrustSkill::Hide();
	KUiChatRoomListMenu::Hide();
	KUiGenPersonalInfo::Hide();
	KUiPointListCharts::Hide();
}

void KUiAdapter::HideUi( void )
{
	if ( !ms_This )
	{
		return;
	}	
	if ( ms_This->m_bHideUi )
	{
		ms_This->m_bHideUi = false;
	}
	else
	{
		ms_This->m_bHideUi = true;
	}
	

}

void KUiAdapter::SetMouseRes( int nMouseRes )
{
	if ( ms_This && ms_This->m_pCEGUISystem && 
		!ms_This->m_mouseRes.empty() && 
		!ms_This->m_mouseRes[nMouseRes].empty() &&
		ms_This->m_nCurMouse != nMouseRes )
	{
		ms_This->m_pCEGUISystem->setDefaultMouseCursor( (ms_This->m_mouseRes[nMouseRes].c_str()), (CEGUI::utf8*)UI_DEFAULT_MOUSEARROW, CEGUI::WindowDraw );
		ms_This->m_nCurMouse = nMouseRes;
	}
}

int KUiAdapter::GetMouseRes( void )
{
	return ms_This->m_nCurMouse;
}

bool KUiAdapter::EscHideDialog(	void )
{
	bool bHide = false;

	if ( KUiTargetFace::IsVisible() )
	{
		KUiTargetFace::Hide();
		bHide = true;
	}

	if ( !KUiDragItem::GetSingletonPtr()->getObj()->isEmpty() )
	{
		KUiDragItem::GetSingletonPtr()->initItem();
		return true;
	}

	if ( KUiLRSkillWnd::IsVisible() )
	{
		KUiLRSkillWnd::Hide();
		bHide = true;
	}

	if ( KUiQuestManage::IsVisible() )
	{
		KUiQuestManage::Hide();
		bHide = true;
	}

	if ( KUiStudySkillManage::IsVisible() )
	{
		KUiStudySkillManage::Hide();
		bHide = true;
	}

	if (KUiCommonMsgBox::IsVisible())
	{
		KUiCommonMsgBox::Hide();
		bHide = true;
	}

	if (KUiChatCentre::IsVisible())
	{
		KUiChatCentre::Hide();
		bHide = true;
	}
	
	if (KUiExit::IsVisible())
	{
		KUiExit::Hide();
		bHide = true;
	}
	
	if (KUiMailCentre::IsVisible())
	{
		CEGUI::EventArgs e;
		KUiMailCentre::GetSingletonPtr()->handleClose(e);
		bHide = true;
	}

	if (KUiSceneMap::getSinglton().isVisible())
	{
		KUiSceneMap::getSinglton().hide();
		bHide = true;
	}
	
	if(KUiBigMap::getSinglton().isVisible())
 	{
		KUiBigMap::getSinglton().hide();
 		bHide = true;
 	}
	
	if (KUiPKFilter::IsVisible())
	{
		KUiPKFilter::Hide();
		bHide = true;
	}	

	if (KUiEquipment::IsVisible())
	{
		KUiEquipment::Hide();
		bHide = true;
	}
	

	if (KUiNpcMsgBox::IsVisible())
	{	
		KUiNpcMsgBox::Hide();
		bHide = true;
	}

	if (KUiQuestManage::IsVisible())
	{
		KUiQuestManage::Hide();
		bHide = true;
	}	

	if (KUiItemBox::getSingleton().isVisible())
	{
		KUiItemBox::getSingleton().hide();
		bHide = true;
	}
	
	if (KUiTradeBox::IsVisible())
	{
		KUiTradeBox::Hide();
		bHide = true;
	}

	if (KUiTradeConfirmBox::IsVisible())
	{
		KUiTradeConfirmBox::Hide();
		bHide = true;
	}

	if (KUiShop::IsVisible())
	{
		KUiShop::Hide();
		bHide = true;
	}	

	if (KUiItemTip::IsVisible())
	{
		KUiItemTip::Hide();
		bHide = true;
	}

	if (KUiLinkedItemTip::IsVisible())
	{
		KUiLinkedItemTip::Hide();
		bHide = true;
	}
	if (KUiCompound::IsVisible())
	{
		KUiCompound::Hide();
		bHide = true;
	}

	if (KUiStoreBox::getSingleton().isVisible())
	{
		KUiStoreBox::getSingleton().hide();
		bHide = true;
	}

	if (KUiVendueWnd::IsVisible())
	{
		KUiVendueWnd::Hide();
		bHide = true;
	}

	if (KUiTongCreate::IsVisible())
	{
		KUiTongCreate::Hide();
		bHide = true;
	}

	if (KUiCityManager::IsVisible())
	{
		KUiCityManager::Hide();
		bHide = true;
	}

	if (KUiCityResMgr::IsVisible())
	{
		KUiCityResMgr::Hide();
		bHide = true;
	}

	if (KUiTongOperMgr::IsVisible())
	{
		KUiTongOperMgr::Hide();
		bHide = true;
	}

	if (KUiTongStatueMsg::IsVisible())
	{
		KUiTongStatueMsg::Hide();
		bHide = true;
	}

	if (KUiTongManager::IsVisible())
	{
		KUiTongManager::Hide();
		bHide = true;
	}

	if (KUiTalisman::getSingleton().isVisible())
	{
		KUiTalisman::getSingleton().hide();
		bHide = true;
	}

	if (KUiTargetEquipment::IsVisible())
	{
		KUiTargetEquipment::Hide();
		bHide = true;
	}

	if (KUiPlayerMenu::IsVisible())
	{
		KUiPlayerMenu::Hide();
		bHide = true;
	}
	
	//zhangxin080418 KUiHelpInfo不再使用，功能合并到封神宝典中
	/*if (KUiHelpInfo::IsVisible())
	{
		KUiHelpInfo::Hide();
		bHide = true;
	}*/
	
	if (KUiGameSetting::IsVisible())
	{
		KUiGameSetting::Hide();
	}
	
	if (KUiDelComfirm::IsVisible())
	{
		KUiDelComfirm::Hide();
		bHide = true;
	}

	if (KUiLevelUpInfo::IsVisible())
	{
		KUiLevelUpInfo::Hide();
		bHide = true;
	}

	if (KUiDeleyQuit::IsVisible())
	{
		KUiDeleyQuit::Hide();
		bHide = true;
	}
	
	if (KUiTargeMenu::IsVisible())
	{
		KUiTargeMenu::Hide();
		bHide = true;
	}

	/*if (KUiBeginHelp::IsVisible())
	{
		KUiBeginHelp::Hide();
		bHide = true;
	}//*/

	if (KUiRoleFacePopMenu::IsVisible())
	{
		KUiRoleFacePopMenu::Hide();
		bHide = true;
	}

	if (KUiMailCentre::IsVisible())
	{
		KUiMailCentre::Hide();
		bHide = true;
	}

	if(KUiRaid::getSinglton().isVisible())
	{
		KUiRaid::getSinglton().hide();
		bHide = true;
	}

	if (KTaisuiWnd::IsVisible())
	{
		KTaisuiWnd::Hide();
		bHide = true;
	}

	if (KUiElfPopMenu::IsVisible())
	{
		KUiElfPopMenu::Hide();
		bHide = true;
	}
	
	if ( KUiSmith::getSingleton().isVisible() )
	{
		KUiSmith::getSingleton().hide();
		bHide = true;
	}

	if ( KUiIBShop::IsVisible() )
	{
		KUiIBShop::Hide();
		bHide = true;
	}

	if ( KUiCreditShop::IsVisible() )
	{
		KUiCreditShop::Hide();
		bHide = true;
	}
	
	if ( KUiChatInputWnd::IsVisible() )
	{
		KUiChatInputWnd::GetSingleton().clearText();
		KUiChatInputWnd::Hide();
		bHide = true;
	}

	if ( KUiTeamViewer::IsVisible() )
	{
		KUiTeamViewer::Hide();
		bHide = true;
	}

	if (KUiSocialInfo::IsVisible())
	{
		KUiSocialInfo::Hide();
		bHide = true;
	}

	if (KUiIEWindow::IsVisible())
	{
		KUiIEWindow::Hide();
		bHide = true;
	}
	
	if (KUiFSBible::getSingleton().isVisible())
	{
		KUiFSBible::getSingleton().hide();
		bHide = true;
	}
	
	if (KUiItemOperPanel::getSingleton().isVisible())
	{
		KUiItemOperPanel::getSingleton().hide();
		bHide = true;
	}
	
	if (KUiGMCommunication::getSingleton().isVisible())
	{
		KUiGMCommunication::getSingleton().hide();
		bHide = true;
	}

	if (KUiNpcNavigation::getSingleton().isVisible())
	{
		KUiNpcNavigation::getSingleton().hide();
		bHide = true;
	}

	if (KUiRandomCopyRewards::GetSingleton().IsVisible())
	{
		KUiRandomCopyRewards::GetSingleton().Hide();
		bHide = true;
	}

	if (KUiHire::IsVisible())
	{
		KUiHire::Hide();
		bHide = true;
	}

	if (KUiHireConfigExp::IsVisible())
	{
		KUiHireConfigExp::Hide();
		bHide = true;
	}

	if (KUiHireConfigSalary::IsVisible())
	{
		KUiHireConfigSalary::Hide();
		bHide = true;
	}

	if ( KUiIBShopResultMessage::IsVisible() )
	{
		KUiIBShopResultMessage::Hide();
		bHide = true;
	}

	if ( KUiRepayConfirm::IsVisible() )
	{
		KUiRepayConfirm::Hide();
		bHide = true;
	}

	if ( KUiIBQuantityInput::IsVisible() )
	{
		KUiIBQuantityInput::Hide();
		bHide = true;
	}

	if(KUiRecommend::getSingleton().isVisible())
	{
		KUiRecommend::getSingleton().hide();
		bHide = true;
	}
// 	if ( KUiServerList::IsVisible() )
// 	{
// 		KUiServerList::Hide();
// 		bHide = true;
// 	}

	if ( KUiEntrustComputer::IsVisible() )
	{
		KUiEntrustComputer::GetSingleton().ClosePanel();
		bHide = true;
	}

	if ( KUiRewardExpNotify::IsVisible() )
	{
		KUiRewardExpNotify::Hide();
		bHide = true;
	}

	if ( KUiItemPassword::IsVisible() )
	{
		KUiItemPassword::Hide();
		bHide = true;
	}

	if ( KUiItemPassword_Create::IsVisible() )
	{
		KUiItemPassword_Create::Hide();
		bHide = true;
	}


	if ( KUiItemPassword_Modify::IsVisible() )
	{
		KUiItemPassword_Modify::Hide();
		bHide = true;
	}

	if ( KUiSmallBattleFieldResult::IsVisible() )
	{
		KUiSmallBattleFieldResult::Hide();
		bHide = true;
	}

	if (KUiEntrustSkill::IsVisible())
	{
		KUiEntrustSkill::Hide();
	}
	if ( KUiPointListCharts::IsVisible() )
	{
			KUiPointListCharts::Hide();
			bHide = true;
	}

	if ( KUiGenPersonalInfo::IsVisible() )
	{
		KUiGenPersonalInfo::Hide();
		bHide = true;
	}

	
	KUiShizuBanner::getSingleton().Hide();
	KUiChanConfig::getSingleton().hide();
	KUiChatRoomListMenu::Hide();

	return bHide;
}

void	KUiAdapter::PlayTitleMusic( void )
{
	char	szMusic[128] = "";
	KIniFile	Ini;
	if (Ini.Load(MAP_SETTING_FILE))
	{
		int	nCount = 0;
		Ini.GetInteger("JustLaunched", "TitleMusicCount", 0, &nCount);
		if (nCount > 0)
		{
			char	szKey[16];
			sprintf(szKey, "TitleMusic_%d", rand() % nCount);
			Ini.GetString("JustLaunched", szKey, "", szMusic, sizeof(szMusic));
			if (szMusic[0])
			{				
				if ( g_pMusicShell && !g_pMusicShell->IsPlaying() )
				{
					g_pMusicShell->Stop();
					g_pMusicShell->Open((char*)szMusic);
					g_pMusicShell->Play(true);
				}
			}
		}
	}
}

void	KUiAdapter::StopTitleMusic( void )
{
	if (g_pMusicShell)
	{
		g_pMusicShell->Stop();
		g_pMusicShell->Close();
	}
}

bool	KUiAdapter::InitMouseRes( void )
{
	KIniFile ini;
	if ( ini.Load( MAP_SETTING_FILE ) )
	{
		int nCount = 0;
		ini.GetInteger( "MouseRes", "Count", 0, &nCount );
		for ( int nIdx = 0; nIdx < nCount; ++nIdx )
		{
			char szBuf[COMMON_CLIENT_MSG_LEN_32];
			sprintf( szBuf, "%d", nIdx );
			char szRes[COMMON_CLIENT_MSG_LEN_256];
			ini.GetString( "MouseRes", szBuf, "cursor/normal.pak", szRes, COMMON_CLIENT_MSG_LEN_256);
			m_mouseRes.push_back( szRes );
		}
		return true;
	}
	return false;
}


int KUiAdapter::UiInit( iRepresentShell *pRepresentShell )
{
	InitMouseRes();

	// LSL
	m_pRenderer = new CEGUI::DirectX7Renderer( pRepresentShell );
	m_pSound	= new CEGUI::UIDXSound;

	if ( m_pRenderer && m_pSound )
	{
		if ( m_pCEGUISystem = new CEGUI::System( m_pRenderer, m_pSound ) )
		{
			// set window manager
			m_pWindowManager = CEGUI::WindowManager::getSingletonPtr();

			// Load the scheme to Initialize the VanillaSkin which we selected.
			CEGUI::SchemeManager::getSingleton().loadScheme( UI_DEFAULT_SKIN_SCHEME );

			// set default mouse image.
			KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );

			// Regist all TL window factory.
			registerAllFactories();

			// set default tooltip
			m_pCEGUISystem->setTooltip( CEGUI::TLTooltip::WidgetTypeName );				

			// set root sheet
			Window* sheet = KUiSheetMgr::getSinglton().createNewSheet(UI_DEFAULT_GUISHEET_ROOT);
			if(!sheet)
			{
				return FALSE;
			}
			KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);
			KUiSheetMgr::getSinglton().createNewSheet(UI_DEFAULT_GUISHEET_ROOT_2);

			m_pCEGUISystem->setSingleClickTimeout(3.0f);

			int nErr = CreateMDL();
			if ( nErr == success_errorcode )
			{
				IUIMDL* pMDL = NULL;
				nErr = GetMDLPtr( &pMDL );
				if ( nErr == success_errorcode )
				{
					nErr = pMDL->createDataSet( vendue_operation );
				}
			}

			// DefaultAscIIFont
			for (int i = 0; i < KUiCfgLoader::getSingleton().getFontName().count; i++)
			{
				FontManager::getSingleton().addDefaultAscIIFontName(
					KUiCfgLoader::getSingleton().getFontName().vecFontName[i]);
			}
		}
	}

#ifdef _DEBUG
    InitOldFont();
#endif

	return TRUE;
}

int KUiAdapter::UiStart()
{
	PlayTitleMusic();
	InitMusicVolume();

	new KUiBeginHelp( UI_FIRSTLOGIN_HELP );
	new KUiCommonMsgBox( UI_COMMONMSGBOX );
	new KUiUpdateTip( UI_UPDATETIP );
	new KUiLoginBackGround( UI_LOGINBK );
	new KUiNewPlayerInfo( "uisettings/layouts/NewRoleInfo.ls" );
	new KUiNewPlayer( UI_NEWROLE );
	new KUiSelPlayer( UI_SELROLE );
	new KUiLogin( UI_PASSWORD );	
	new KUiChatCentre( UI_CHATCENTRE );		
	new KUiNaviation( UI_NAVICATION );
	new KUiNaviationEx( "uisettings/layouts/NavigationEx.ls" );
	new KUiExit( UI_EXITBOX );
	new KUiRoleFace( UI_ROLEFACE );
	new KUiCastSkillExp( "uisettings/layouts/castskillexp.ls" );
	new KUiRoleExp( UI_ROLEEXP );
	new	KUiTargetFace( UI_TARGETFACE );
	new KUiLRSkillWnd( "uisettings/layouts/lrskillwnd.ls" );
	new KUiShortcutWnd( UI_SHORTCUTWND );
	new KUiShortcutPlusWnd( UI_SHORTCUTPLUSWND );	
	new KUiChannelCentre( UI_CHATCHANNELWND );
	new KUiMiniNaviation( UI_MININAVICATION );
	new KUiChatNotify( UI_CHATMSGNOTIFY );
	new KUiMailCentre( UI_MAILCENTRE );
	new KUiMiniMap( UI_MINIMAP );
	new KUiBufferCentre( UI_BUFFERWND );
	new KUiDeathBox( UI_DEATHWND );
	new KUiDebufferCentre( UI_DEBUFFERWND );
	new KUiTopMessage( UI_TOPMESSAGE );
	new KUiEquipment( UI_EQUIPMENT );
	new KUiPKFilter( UI_PKFILTER );
	new	KUiNpcMsgBox( UI_NPCMSG );
	new KUiQuestManage( UI_QUESTMANAGE );
	new KUiComMsgBox( UI_COMMSGBOX );
	new KUiHelpCentre;
	new KUiDragItem(UI_DRAGITEM);
	new KUiTradeBox(UI_TRADEBOX);
	new KUiTargetbufferCentre( UI_TARGETBUFFERWND ) ;
	new KUiPlayerState();
	new KUiTradeConfirmBox( UI_TRADECONFIRMBOX );
	new KUiTeamList( UI_TEAMLIST );
	new KUiShop( UI_SHOP );
	new KUiCompound( UI_COMPOUND );
	new KUiItemTip( UI_ITEM_TIP );
	new KUiLinkedItemTip( UI_LINKED_ITEM_TIP );
	new KUiSystemMessage( UI_SYSTEM_MESSAGE );
	new KUiChatInputWnd( UI_CHAT_INPUT );
	new KUiVendueWnd( UI_VENDUE_WND );
	new KUiTongCreate( UI_TONGCREATE );
	new KUiCityManager( UI_CITYMANAGER );
	new	KUiCityResMgr( UI_CITYRESMGR );
	new KUiTongOperMgr( UI_TONGOPERMGR );
	new KUiTongManager( UI_TONGMANAGER );
	new KUiCastBar( UI_CAST_BAR );
	new KUiTargetEquipment( UI_TARGET_EQUIPMENT );
	new KUiPlayerMenu( UI_PLAYER_MENU );
	new KUiQuestTrack( UI_QUEST_TRACK );
	//new KUiHelpInfo( UI_HELPINFO );   //需要注释掉，KUiHelpInfo窗口已经不在使用，功能合并到封神宝典中
	new KUiGameSetting( UI_GAMESET_BOARD );
	new KUiAutoConnect( UI_AUTO_CONNECT );
	new KUiStudySkillManage( UI_STUDESKILL );
	new KUiWaitingMsg( UI_WAITINGMSG );
	new KUiDelComfirm( UI_DELETE_COMFIRM );
	new KUiLevelUp( UI_LEVELUP );
	new KUiLevelUpInfo( UI_LEVELUP_INFO );
	new KUiDeleyQuit( UI_DELAYQUIT );
	new KUiTargeMenu( UI_TARGET_MENU );
	new KUiChangeMapWnd( UI_CHANGEMAPWND );
	new KUiRoleFacePopMenu( UI_ROLEFACE_POPMENU);
	new KUiPlayerInfo("uisettings/layouts/FriendInfo.ls");
	new KUiP2RPopmenu("uisettings/layouts/ChatP2pPopMenu.ls");
	new KTaisuiWnd("uisettings/layouts/taisui.ls");  //TaisuiWnd
	new KUiWorldCombatInfo("uisettings/layouts/WorldCombatScore.ls");
	new KUIGlobalEvent();
	new KUiSearchHelpWnd("uisettings/layouts/SearchHelpWnd.ls");
	new KUiElf("uisettings/layouts/Elf.ls");
	new	KUiElfPopMenu("uisettings/layouts/ElfPopMenu.ls");
	new KUiQueryWnd("uisettings/layouts/QueryWnd.ls");
    new KUiCityTaxEditer("uisettings/layouts/Shuishou.ls");
	new KUiTeamHideShow("uisettings/layouts/TeamHideShow.ls");
	new KUiIBShop("uisettings/layouts/IBShop.ls");
	new KUiCreditShop("uisettings/layouts/CreditShop.ls");

	new KUiIBNavigation("uisettings/layouts/IBNavigation.ls");
	new KUiRankButton("uisettings/layouts/RankingButton.ls");
	new KUiCreditShopNavigation("uisettings/layouts/CreditShopNavigation.ls");

	new KUiFuryBox("uisettings/layouts/FruyBox.ls");

	new KUiShortcutPlusWndShowHide("uisettings/layouts/shorcutwndshowhide.ls");
	new KUiExtendChatWndBtn("uisettings/layouts/ExtChatWndShowHide.ls");
	new KUiEntrustComputerBtn( "uisettings/layouts/EntrustComputerBtn.ls" );
	new KUiElfBtn("uisettings/layouts/ElfBtn.ls");

	new KUiTeamViewer("uisettings/layouts/teamviewer.ls");
	
	new KUiTongRecruitCentre("uisettings/layouts/TongCentre.ls");

	new KUiSocialInfo("uisettings/layouts/SocialInfo.ls");
	//render add//
	new KUiPathHelp(UI_PATH_HELP_H);

	//caolei+
	new KUiRandomCopyRewards("uisettings/layouts/RandomCopyRewards.ls");
	new KUiHire("uisettings/layouts/Hire.ls");
	new KUiHireConfigExp("uisettings/layouts/HireConfig_Exp.ls");
	new KUiHireConfigSalary("uisettings/layouts/HireConfig_Salary.ls");
	new KUiIBQuantityInput("uisettings/layouts/IBQuantityInput.ls");
	new KUiRepayConfirm("uisettings/layouts/RepayConfirm.ls");
	new KUiIBShopResultMessage("uisettings/layouts/IBShopResultMessage.ls");
	new KUiIEWindow("uisettings/layouts/IEWindow.ls");
	new KUiEntrustComputer( "uisettings/layouts/EntrustComputer.ls" );
	new KUiGenPersonalInfo("uisettings/layouts/GenPersonalInfo.ls");
	//new KUiServerList("uisettings/layouts/ServerList.ls");

	//Add by DarkMagic(DuanMu) add
//	new KUiBattleResult("uisettings/layouts/BattleResult.ls");

	new KUiInfoBarPing( "uisettings/layouts/InfoBar_Ping.ls" );
	new KUiInfoBarTime( "uisettings/layouts/InfoBar_Time.ls" );
	
	new KUiQuestionWindow( "uisettings/layouts/UiQuestionWindow.ls" );
	new KUiItemPassword( "uisettings/layouts/ItemPassword.ls" );
	new KUiItemPassword_Create( "uisettings/layouts/ItemPassword_Create.ls" );
	new KUiItemPassword_Modify( "uisettings/layouts/ItemPassword_Modify.ls" );
	
	new KUiSmallBattleFieldResult( "uisettings/layouts/SmallBattleFieldResult.ls" );

	new KUiPointListCharts( "uisettings/layouts/Charts.ls" );
	new KUiServerList( "uisettings/layouts/ServerList.ls" );

	new KUiRewardExpNotify( "uisettings/layouts/RewardExpNotify.ls" );

	new KUiTongStatueMsg("uisettings/layouts/TongStatueMsg.ls");

	new KUiEntrustSkill(AnsiToUtf8("uisettings/layouts/EntrustSkillwnd.ls"));
	new KUiChatRoomListMenu("uisettings/layouts/ChatRoomListMenu.ls");
	return TRUE;
}

void KUiAdapter::UiPaint( int nGameLoop )
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi)
	{
		m_pCEGUISystem->renderGUI();
	}
}

void KUiAdapter::UiPaintNpcHeadInfo(int nGameLoop)
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi)
	{
		m_pCEGUISystem->renderNpcHeadInfo();
	}
}
void KUiAdapter::UiPaintBottomOldWindow(int nGameLoop)
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi)
		m_pCEGUISystem->renderBottomOldWindow();
}
void KUiAdapter::UiPaintNewMode(int nGameLoop )
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi)
	{
		m_pCEGUISystem->renderGUINewMode();
	}
}
void KUiAdapter::UiPaintOldMode(int nGameLoop )
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi)
	{
		m_pCEGUISystem->renderGUIOldMode();
	}
}

void KUiAdapter::UiBreathe( void  )
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi )
	{
		if ( KUiChangeMapWnd::IsVisible() )
		{
			if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME )
			{
				KUiChangeMapWnd::GetSingleton().EndLoading();
			}
			KUiChangeMapWnd::Breathe();
		}

		if ( KUiDeleyQuit::IsVisible() )
		{
			KUiDeleyQuit::GetSingletonPtr()->DisplayDelay();
		}

		if ( KUiAutoConnect::IsVisible() )
		{
			// 自动重连倒数记时
			if ( g_LoginLogic.GetStatus() != LL_S_ACCOUNT_CONFIRMING )
			{
				KUiAutoConnect::GetSingletonPtr()->DisplayAutoConnect();
			}
			else
			{
				// 自动连接等待进入
				KUiAutoConnect::GetSingletonPtr()->WaitAutoConnect();
			}
		}
		else 
		{
			KUiWaitingMsg::GetSingleton().WaitConnect();
		}

		if ( KUiSystemMessage::IsVisible() )
		{
			KUiSystemMessage::GetSingletonPtr()->Breathe();
		}

		if (KTaisuiWnd::IsVisible())
		{
			KTaisuiWnd::Breathe();
		}//endif

		if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME )
		{
			if ( KUiGameSetting::GetSingletonPtr()->GetShowSearchHelp() || 
				  KUiGameSetting::GetSingletonPtr()->GetShowSimpleHelp() )
			{
				if ( KUiSearchHelpWnd::IsVisible() )
				{
					KUiSearchHelpWnd::GetSingletonPtr()->Breathe();
				}
				if ( KUiElf::IsVisible() )
				{
					KUiElf::GetSingletonPtr()->Breathe();
				}
			}

			if ( KUiInfoBarTime::IsVisible() )
			{
				KUiInfoBarTime::Breathe();
			}
			
#ifdef __QUESTION_WND_TEST
			KUiQuestionWindow::Breathe();
#endif
		}
		else
		{
			DWORD dwRet = ::WaitForSingleObject( g_updateEvent, 0 );
			if ( !KUiServerList::IsVisible() && 
				isUpdateOk() && 
				g_bShowServerList && 
				dwRet == WAIT_OBJECT_0 )
			{
				KUiComMsgBox::GetSingleton().init();
				KUiComMsgBox::GetSingleton().Hide();
				KUiServerList::Hide();
				if ( g_updateRet == TRUE )
				{
					KUiServerList::Show( true );
				}
				else
				{
					KUiServerList::Show( false );
				}				
			}

		}

		
		if ( KUiRandomCopyRewards::IsVisible() )
		{
			KUiRandomCopyRewards::Breathe();
		}

// 		if ( KUiIBShop::IsVisible() )
// 		{
// 			KUiIBShop::Breathe();
// 		}
	}
	int count = CEGUI::System::getSingleton().getShowEditNum();
	HWND hWnd = g_GetMainHWnd();
	static bool isSetContext = false;
//	int focus = B2ChatDialog::chatManager.ChatManagerGetEditBox()->focus;
	if ( count > 0 )
	{
		if(isSetContext&&m_hIMC)
		{

			ImmAssociateContext(hWnd, m_hIMC);
			isSetContext =false;
			m_hIMC = 0;
		}
	}
	else
	{
		if(!m_hIMC)
		{
			m_hIMC = ImmAssociateContext(hWnd, NULL);
			isSetContext = true;
		}
	}

	if ( GetAsyncKeyState(VK_ESCAPE) && 0x8000 )
	{
		if ( KUiIEWindow::IsVisible() )
		{
			KUiIEWindow::Hide();
		}
	}

	KUiNaviation::GetSingleton().Breath(); 
}

void KUiAdapter::UiTimePulse( DWORD newTimeCount )
{
	if ( m_pCEGUISystem && ms_This  && !ms_This->m_bHideUi )
	{
		m_pCEGUISystem->injectTimePulse( newTimeCount );
//		m_fOldTimeElapsed = fTimeElapsed;
	}
}

int KUiAdapter::UiExit( void )
{
	KUiCfgLoader::getSingleton().save();

	KUiPlayerInfo::DestroyWindow();
	KUiCommonMsgBox::DestroyWindow();
	KUiUpdateTip::DestroyWindow();
	KUiNewPlayerInfo::DestroyWindow();
	KUiNewPlayer::DestroyWindow();
	KUiSelPlayer::DestroyWindow();
	KUiLogin::DestroyWindow();
	KUiLoginBackGround::DestroyWindow();
	KUiChatCentre::DestroyWindow();
	KUiNaviation::DestroyWindow();
	KUiNaviationEx::DestroyWindow();
	KUiExit::DestroyWindow();
	KUiRoleFace::DestroyWindow();
	KUiRoleExp::DestroyWindow();
	KUiCastSkillExp::DestroyWindow();
	KUiTargetFace::DestroyWindow();
	KUiShortcutWnd::DestroyWindow();
	KUiShortcutPlusWnd::DestroyWindow();
	KUiLRSkillWnd::DestroyWindow();
	KUiChannelCentre::DestroyWindow();
	KUiMiniNaviation::DestroyWindow();
	KUiChatNotify::DestroyWindow();
	KUiMailCentre::DestroyWindow();
	KUiMiniMap::DestroyWindow();
	KUiBufferCentre::DestroyWindow();
	KUiDeathBox::DestroyWindow();
	KUiDebufferCentre::DestroyWindow();
	KUiTopMessage::DestroyWindow();
	KUiPKFilter::DestroyWindow();
	KUiEquipment::DestroyWindow();
	KUiNpcMsgBox::DestroyWindow();
	KUiQuestManage::DestroyWindow();
	KUiComMsgBox::DestroyWindow();
	KUiHelpCentre::DestroyWindow();
	KUiDragItem::DestroyWindow();
	KUiTradeBox::DestroyWindow();
	KUiTargetbufferCentre::DestroyWindow();
	delete KUiPlayerState::getSingletonPtr();
	KUiTradeConfirmBox::DestroyWindow();
	KUiTeamList::DestroyWindow();
	KUiShop::DestroyWindow();
	KUiItemTip::DestroyWindow();
	KUiLinkedItemTip::DestroyWindow();
	KUiCompound::DestroyWindow();
	KUiSystemMessage::DestroyWindow();
	KUiChatInputWnd::DestroyWindow();
	KUiVendueWnd::DestroyWindow();
	KUiTongCreate::DestroyWindow();
	KUiCityManager::DestroyWindow();
	KUiCityResMgr::DestroyWindow();
	KUiTongOperMgr::DestroyWindow();
	KUiTongStatueMsg::DestroyWindow();
	KUiTongManager::DestroyWindow();
	KUiIEWindow::DestroyWindow();
	KUiCastBar::DestroyWindow();
	KUiTargetEquipment::DestroyWindow();
	KUiPlayerMenu::DestroyWindow();
	KUiQuestTrack::DestroyWindow();
	//KUiHelpInfo::DestroyWindow();	//需要注释掉，KUiHelpInfo窗口已经不在使用，功能合并到封神宝典中
	KUiGameSetting::DestroyWindow();
	KUiAutoConnect::DestroyWindow();
	KUiStudySkillManage::DestroyWindow();
	KUiWaitingMsg::GetSingleton().EndLogin();
	KUiWaitingMsg::GetSingleton().QuitMsg();
	KUiWaitingMsg::DestroyWindow();
	KUiDelComfirm::DestroyWindow();
	KUiLevelUp::DestroyWindow();
	KUiLevelUpInfo::DestroyWindow();
	KUiDeleyQuit::DestroyWindow();
	KUiTargeMenu::DestroyWindow();
	KUiBeginHelp::DestroyWindow();
	KUiChangeMapWnd::DestroyWindow();
	KUiRoleFacePopMenu::DestroyWindow();
	KUiP2RPopmenu::DestroyWindow();
	KTaisuiWnd::DestroyWindow();
	KUiSearchHelpWnd::DestroyWindow();
	KUiElf::DestroyWindow();
	KUiElfPopMenu::DestroyWindow();
	KUiQueryWnd::DestroyWindow();
	KUiCityTaxEditer::DestroyWindow();
	KUiTeamHideShow::DestroyWindow();
	KUiIBShop::DestroyWindow();
	KUiCreditShop::DestroyWindow();
	KUiPathHelp::DestroyWindow();
	KUiIBNavigation::DestroyWindow();
	KUiRankButton::DestroyWindow();
	KUiCreditShopNavigation::DestroyWindow();

	KUiFuryBox::DestroyWindow();
	KUiShortcutPlusWndShowHide::DestroyWindow();
	KUiExtendChatWndBtn::DestroyWindow();
	KUiEntrustComputerBtn::DestroyWindow();
	KUiElfBtn::DestroyWindow();
	KUiTongRecruitCentre::DestroyWindow();

	KUiTeamViewer::DestroyWindow();
	KUiSocialInfo::DestroyWindow();
	KUiRandomCopyRewards::DestroyWindow();
	KUiHire::DestroyWindow();
	KUiHireConfigExp::DestroyWindow();
	KUiHireConfigSalary::DestroyWindow();
	KUiIBQuantityInput::DestroyWindow();
	KUiRepayConfirm::DestroyWindow();
	KUiIBShopResultMessage::DestroyWindow();
	KUiWorldCombatInfo::DestroyWindow();
	KUiEntrustComputer::DestroyWindow();
	KUiInfoBarPing::DestroyWindow();
	KUiInfoBarTime::DestroyWindow();
	KUiQuestionWindow::DestroyWindow();
	KUiServerList::DestroyWindow();

	KUiItemPassword::DestroyWindow();
	KUiItemPassword_Create::DestroyWindow();
	KUiItemPassword_Modify::DestroyWindow();
	KUiRewardExpNotify::DestroyWindow();

	KUiEntrustSkill::DestroyWindow();

	KUiSmallBattleFieldResult::DestroyWindow();
	KUiPointListCharts::DestroyWindow();

	KUiChatRoomListMenu::DestroyWindow();
	KUiGenPersonalInfo::DestroyWindow();

	//Add by DarkMagic(DuanMu) add
//	KUiBattleResult::DestroyWindow();

	if (KUIGlobalEvent::getSingletonPtr())
	{
		delete KUIGlobalEvent::getSingletonPtr();
		//KUIGlobalEvent::getSingletonPtr()
	}

	if ( m_pCEGUISystem )
	{
		delete m_pCEGUISystem;
		m_pCEGUISystem = NULL;
	}
	if ( m_pRenderer )
	{
		delete m_pRenderer;
		m_pRenderer = NULL;
	}
	// LSL
	if ( m_pSound )
	{
		delete m_pSound;
		m_pSound = NULL;
	}	

	return TRUE;
}

int KUiAdapter::UiProcessInput( unsigned int uMsg, unsigned int uParam, int nParam )
{
	int nRet = 0;
	if ( m_pCEGUISystem == NULL || ( ms_This && g_LoginLogic.GetStatus() == LL_S_IN_GAME && ms_This->m_bHideUi) )
	{
		return nRet;
	}

	WPARAM wParam = uParam;
	LPARAM lParam = nParam;

	HWND hWnd = g_GetMainHWnd();

	switch ( uMsg )
	{
	case WM_CLOSE:
		if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME )
		{
			if (!EscHideDialog())
				KUiExit::Show();
		}
		else if ( KUiChangeMapWnd::IsVisible() )
		{
			KUiChangeMapWnd::GetSingleton().EndLoading();
			g_LoginLogic.NotifyDisconnect();
		}
		else
		{
			::DestroyWindow( g_GetMainHWnd() );
		}
		nRet = true;
		break;
	case WM_KEYUP:
		{
			if ( VK_PROCESSKEY  != uParam )
			{
				CEGUI::utf32 uScanCode = ( (CEGUI::utf32)lParam >> 16 ) & 0xFF;
				bool bExtendKey = ( lParam & 0x01000000 ) != false; 
				nRet = m_pCEGUISystem->injectKeyUp( uScanCode + ( bExtendKey ? 0x80 : 0 ) );
			}
		}
		break;

	case WM_KEYDOWN:
		{
			if ( VK_PROCESSKEY  != uParam )
			{
				/*if ( VK_RETURN == uParam )
				{
					if (g_LoginLogic.GetStatus() == LL_S_IN_GAME && KUiChatInputWnd::GetSingleton().IsVisible())
					{
						m_hIMC = ImmGetContext(hWnd);
						if (m_hIMC)
						{
							ImmAssociateContext(hWnd, NULL);
						}
						ImmReleaseContext(hWnd, m_hIMC);
					}
					else
					{
						if ( m_hIMC )
						{
							ImmAssociateContext(hWnd, m_hIMC);
							m_hIMC = NULL;
						}
					}

				}*/
				CEGUI::utf32 uScanCode = ( (CEGUI::utf32)lParam >> 16 ) & 0xFF;
				bool bExtendKey = ( lParam & 0x01000000 ) != false; 
				nRet = m_pCEGUISystem->injectKeyDown( uScanCode + ( bExtendKey ? 0x80 : 0 ) );
				if ( KUiBeginHelp::IsVisible() && VK_ESCAPE != uParam )
				{
					if(!EscHideDialog())
						KUiExit::Show();
				}
			}
		}
		break;
	case WM_CHAR:
	case WM_IME_CHAR:
		{			
		//	nRet = m_pCEGUISystem->injectChar( CEGUI::AnsiToUtf8( (const char *)uParam ) );
#ifdef _MBCS 

			if ( wParam > 0x80 )
			{
				char strChar[4];
				strChar[0] = (wParam >> 8)	& 0xFF;
				strChar[1] = (wParam)		& 0xFF;
				strChar[2] = (wParam >> 24) & 0xFF;
				strChar[3] = (wParam >> 16) & 0xFF;
				CEGUI::utf32 u32 = 0;
				::MultiByteToWideChar( CP_ACP, 0, (const char *)&strChar, -1, (unsigned short *)&u32, sizeof(u32) );
				nRet = m_pCEGUISystem->injectChar( u32 );
			}
			else
			{
				nRet = m_pCEGUISystem->injectChar( wParam );
			}
#else
			nRet = m_pCEGUISystem->injectChar( (CEGUI::utf32)wParam );
#endif
		}
		break;
	case WM_NCMOUSEMOVE:
		MouseLeaves();
		break;
	case WM_MOUSEMOVE:
		MouseEnters();
		nRet = m_pCEGUISystem->injectMousePosition((float)(LOWORD(lParam)), (float)(HIWORD(lParam)));
		//nRet = m_pCEGUISystem->isMouseInWnd();
		KShortcutKeyCentre::SetIsMouseInWnd(nRet > 0 ? true : false);
		KShortcutKeyCentre::MouseMove( (float)(LOWORD(lParam)), (float)(HIWORD(lParam)), false );
		break;
	case WM_LBUTTONDBLCLK:
		nRet = m_pCEGUISystem->injectMouseDoubleClick(CEGUI::LeftButton);
		break;
	case WM_RBUTTONDBLCLK:
		nRet = m_pCEGUISystem->injectMouseDoubleClick(CEGUI::RightButton);
		break;
	case WM_LBUTTONDOWN:
		nRet = m_pCEGUISystem->injectMouseButtonDown(CEGUI::LeftButton);
		break;
	case WM_LBUTTONUP:
		nRet = m_pCEGUISystem->injectMouseButtonUp(CEGUI::LeftButton);		
		break;
	case WM_RBUTTONDOWN:
		// 右键最先处理释放手中物品消息 --刘思亮
		if ( !KUiDragItem::GetSingletonPtr()->getObj()->isEmpty() )
		{
			KUiDragItem::GetSingletonPtr()->initItem();
			nRet = 1;
		}
		else if ( g_UseItem.uId > 0 && !KUiComMsgBox::IsVisible() )
		{
			CannelUseItem();
		}
		else
		nRet = m_pCEGUISystem->injectMouseButtonDown(CEGUI::RightButton);
		break;
	case WM_RBUTTONUP:
		m_pCEGUISystem->injectMouseButtonUp(CEGUI::RightButton);
		nRet = 0 ;
		break;
	case WM_MBUTTONDOWN:
		nRet = m_pCEGUISystem->injectMouseButtonDown(CEGUI::MiddleButton);
		break;
	case WM_MBUTTONUP:
		nRet = m_pCEGUISystem->injectMouseButtonUp(CEGUI::MiddleButton);
		break;
	case WM_MOUSEWHEEL:
		nRet = m_pCEGUISystem->injectMouseWheelChange(static_cast<float>((short)HIWORD(wParam)) / static_cast<float>(120));
		break;
 	case WM_MOUSEHOVER:
 		nRet = m_pCEGUISystem->injectMouseHover();
 		break;
	case WM_DISPLAYCHANGE:
		{

				BOOL bHide = false;
				RECT rc;
				GetClientRect(GetDesktopWindow(),&rc);
				RECT extraWndRect;
				GetWindowRect(ChatMainDlg::hMainDlg,&extraWndRect);
				int height = extraWndRect.right - extraWndRect.left;
				if(rc.right - KWin32App::m_uScreenWidth<height)
					bHide = true;
				if(KWin32App::m_bFullScreen||bHide)
				{
					if(IsWindowVisible(ChatMainDlg::hMainDlg))
					{
						ChatMainDlg::MainDlgShowWndChat(FALSE);
						KUiChannelCentre::Show();
					}
				}
	
		}
		break;

	case WM_SIZE:
	case WM_MOVE:
	case WM_MOVING:
		{
#ifdef  USING_CHAT_WINDOW
			RECT rc;
			::GetWindowRect(hWnd,&rc);
			RECT rc1;
			GetWindowRect(ChatMainDlg::hMainDlg,&rc1);
			int width = rc1.right - rc1.left;
			int height = rc1.bottom - rc1.top;
			MoveWindow(ChatMainDlg::hMainDlg,rc.right,rc.top,width,height,TRUE);
	//		ShowWindow(ChatMainDlg::hMainDlg,SW_NORMAL);
#endif
		}
		break;
	default:
		return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return nRet;
}


void KUiAdapter::MouseEnters( void )
{
    if ( !m_bMouseInWindow && m_pCEGUISystem )
    {
        m_bMouseInWindow = true;
		if ( m_pCEGUISystem->isDrawMouseCursor() )
		{
			::ShowCursor(false);
		}
    }
}

void KUiAdapter::MouseLeaves( void )
{
    if ( m_bMouseInWindow && m_pCEGUISystem )
    {
        m_bMouseInWindow = false;
		if ( m_pCEGUISystem->isDrawMouseCursor() )
		{
			::ShowCursor(true);
		}
    }
}

void KUiAdapter::InitOldFont( void )
{
	char	Buffer[128];
	int			nCount, nId, i;
	KIniFile	Ini;

	char		Section[8];
	//----卸载字体----
	if (g_pRepresentShell && Ini.Load(MAP_SETTING_FILE) &&
		Ini.GetInteger(FONT_SECTION, "Count", 0, &nCount))
	{
		for (i = 0; i < nCount; i++)
		{
			itoa(i, Section, 10);
			if (Ini.GetInteger(FONT_SECTION, Section, 0, &nId))
				g_pRepresentShell->ReleaseAFont(nId);
		}
	}

	Ini.Load(MAP_SETTING_FILE);

	//----载入字体----
	if (g_pRepresentShell && Ini.GetInteger(FONT_SECTION, "Count", 0, &nCount))
	{
		for (i = 0; i < nCount; i++)
		{
			itoa(i, Section, 10);
			if (Ini.GetInteger(FONT_SECTION, Section, 0, &nId))
			{
				strcat(Section, "_File");
				if (Ini.GetString(FONT_SECTION, Section, "", Buffer, sizeof(Buffer)) &&
						Buffer[0])
				{
					g_pRepresentShell->CreateAFont(Buffer, CHARACTER_CODE_SET_GBK, nId);
				}
			}
		}			
	}
}


//初始化系统选项
void	KUiAdapter::InitGameSet( void )
{
	//初始化音乐音量
	int  itemp = 100;
	bool btemp = false;
	KIniFile ini;
	if ( !ini.Load(CONFIG_INI) || g_pCoreShell == NULL )
	{
		return ;
	}
	ini.GetInteger("GameSetting", "MusicSet", 0, &itemp);
	g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, itemp );

	ini.GetInteger("GameSetting", "VoiceSet", 0, &itemp);
	g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_SOUND_VALUE, itemp );
	System::getSingleton().getSound()->SetVolume(itemp);

	ini.GetInteger("GameSetting", "FullOrWin", 0, &itemp);
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_SCREEN_WAY, btemp );

	ini.GetInteger("GameSetting", "ShowPlayer", true, &itemp );
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_PLAYER, btemp );
	
	ini.GetInteger("GameSetting", "ShowNpc",  true, &itemp );
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_NPC, btemp );
	
	ini.GetInteger("GameSetting", "ShowShadow", true, &itemp );
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SHOW_SHADOW, btemp );


	ini.GetInteger("GameSetting", "DrawGround", true, &itemp );
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_GROUND, btemp );
	
	ini.GetInteger("GameSetting", "DrawSmallObj",  true, &itemp );
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_SMALLOBJ, btemp );
	
	ini.GetInteger("GameSetting", "DrawLargeObj", true, &itemp );
	itemp == 0 ? btemp = false : btemp = true;
	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DRAW_LARGEOBJ, btemp );
}

void KUiAdapter::SetFocus( void )
{
	if ( m_pCEGUISystem )
		m_pCEGUISystem->resetSyskey();
}

//关闭快捷方式打开的所有界面
void	KUiAdapter::UiCloseNoNpcDlg( void )
{
	KUiEquipment::Hide();
	KUiTradeBox::Hide();
	KUiStoreBox::getSingleton().hide();
	KUiTongManager::Hide();
	//KUiHelpInfo::Hide(); KUiHelpInfo窗口已经不在使用，功能合并到封神宝典中
	KUiItemBox::getSingleton().hide();
	KUiGameSetting::Hide();
	KUiQuestManage::Hide();
	KUiChatCentre::Hide();
	//Add by DarkMagic(DuanMu) add
//	KUiBattleResult::Hide();
}

void	KUiAdapter::ReFreshUi( )
{
	int sheetCount = KUiSheetMgr::getSinglton().getSheetCount();
	for(int i = 0; i < sheetCount; ++i)
	{
		Window* root = KUiSheetMgr::getSinglton().find(i);
		for(int j = 0; j < root->getChildCount(); ++j)
		{
			Window* child = root->getChildAtIdx(j);
			child->requestRedraw();
		}
	}
}

void	KUiAdapter::InitMusicVolume( void )
{
	//初始化音乐音量
	//和进入游戏时音乐大小保持一致
	int  itemp = 100;
	bool btemp = false;
	KIniFile ini;
	if ( ini.Load(CONFIG_INI) && g_pCoreShell )
	{
		ini.GetInteger("GameSetting", "MusicSet", 0, &itemp);
		g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, itemp );

		ini.GetInteger("GameSetting", "VoiceSet", 0, &itemp);
		g_pCoreShell->OperationRequest( GOI_OPTION_SETTING, OPTION_SOUND_VALUE, itemp );
	}
}