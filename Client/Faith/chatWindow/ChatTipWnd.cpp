#include <windows.h>
#include <commctrl.h>
#include <list>
#include <vector>
using namespace std;
#include "GameDataDef.h"
#include "layoutinterface.h"
#include "ChatDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "coreShell.h"
#include "Ui/UiCommon.h"
#include "ui/UiCase/UiTeamList.h"
#include "Ui/KMessageCentre.h"

#include "loadSrcWnd/GDILoadBitmap.h"
#include "Ui/UiCase/UiChatWindow.h"

#include "chatWindow/chatManager.h"
#include "SocialComDef.h"
#include "ui/UiCommon.h"
#include "chatWindow/PlayerShowInfo.h"

using namespace UIMDL;
extern iCoreShell*		g_pCoreShell;
ChatTipWnd::ChatTipWnd()
{
	
}
ChatTipWnd::~ChatTipWnd()
{

}
void ChatTipWnd::ChatTipAdjustWindow(HWND hwnd)
{
	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId       = enSUTplId_Tong;
	tagSocietyIdx.Layer            = enSULayer_Gens;
	tagSocietyIdx.Operation        = enSUO_AddSubUnit;
	bool isShiZu = false;
	int buttonNum = 2;
	if (g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ))
	{
		buttonNum = 1;
	}
	else
	{
		buttonNum = 2;
	}
	
	tagSocietyIdx.Layer            = enSULayer_Tong;
	
	if (g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
		buttonNum = 0;
	}
	else
	{
		if (isShiZu)
		{
			buttonNum = 1;
		}
		else
		{
			buttonNum = 2;
		}
	}


	int x =0,y=0;
	int width,height;
	width = buttons[0].ChatWndGetRect().right;
	height = buttons[0].ChatWndGetRect().bottom * (_CHAT_WND_BUTTON_NUM - buttonNum);
	POINT pt;
	GetCursorPos(&pt);
//	pt.x -= (width>>1);
	pt.y += 5;
	RECT rectTask;
	GetWindowRect(FindWindow("Shell_TrayWnd",0),&rectTask);
	int dt = 0;
	if(pt.y+height > rectTask.top)
		dt = pt.y+height - rectTask.top;
	MoveWindow(hWndHandle,pt.x,pt.y-dt,width,height,TRUE);
//	MoveWindow(hwnd,pt.x,pt.y,width,height,TRUE);
}
void ChatTipWnd::ChatTipClickPersonalButton()
{
	ILayout* pEdit = B2ChatDialog::chatManager.ChatManagerGetEditBox()->pEditLayOut;
//	pEdit->clearLayout();
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ClearText( );
	pEdit->SetText(_CHAT_EIDT_DEFAULT_STRING);
	pEdit->setSelection(0,0);
	WCHAR name[256] = {0};
	wsprintfW(name,L"%s%s%s",L"/",playerInfo.content.get(),L" ");
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditInsertChar(name);
}
void ChatTipWnd::ChatTipClickTradeButton()
{
	g_pCoreShell->TradeApplyStart(playerInfo.gameObj._objId[0]);
}
void ChatTipWnd::ChatTipClickFollowButton()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_FOLLOW_SOMEONE, (unsigned int)playerInfo.gameObj._objId[0], 0);
	}
}
void ChatTipWnd::ChatTipClickTeamButton()
{
	KUiPlayerItem tagPlayer;
	char* name = NULL;
	unicodeToAnsi(playerInfo.content.get(), name);
	strncpy( tagPlayer.Name, name, CLIENT_NAME_AND_TITLE_MAX + 1);
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	tagPlayer.uId = playerInfo.gameObj._objId[0];
	
	
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
	delete [] name;
}

void ChatTipWnd::ChatTipClickArmButton()
{
/*	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM, (UINT)playerInfo.gameObj._objId[0], NULL);
	}*/

	PlayerInfo info;
	memset(&info,0,sizeof(PlayerInfo));
	char *name = NULL;
	unicodeToAnsi(playerInfo.content.get(), name);
	strcpy(info.szName, name);
	delete[] name;
	PlayerShowInfo::GetSingle().AddItem(info);
}

void ChatTipWnd::ChatTipClickHideButton()
{
	if ( g_pCoreShell )
	{
		char* name = NULL;
		unicodeToAnsi(playerInfo.content.get(), name);
		g_pCoreShell->OperationRequest( GOI_CHAT_ADD_BLACK_LIST, (unsigned int)name, NULL );
		delete [] name;
	}
}

void ChatTipWnd::ChatTipClickFriendButton()
{
	char* name = NULL;
	unicodeToAnsi(playerInfo.content.get(), name);
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)(name), CHAT::GROUPID_NONE );
	delete [] name;
}
void ChatTipWnd::ChatTipClickSizhuButton()
{
	   UIMDL::GetMDLPtr ( &pUiMdl	);
		char* name = NULL;
		unicodeToAnsi(playerInfo.content.get(), name);
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= enSULayer_Gens;
		tagTongOper.nOperationID	= enSUO_AddSubUnit;
		strcpy(tagTongOper.szName,name);
		delete [] name;
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != pUiMdl->queryDataSet( tong_operation, &pTongOper ) )
		{
			return ;
		}//endif
		
		pTongOper->updateRecord( enSULayer_Gens, &tagTongOper, sizeof( TongOperParam ) );
}
void ChatTipWnd::ChatTipClickJoinTeamButton()
{
	KUiPlayerItem tagPlayer;

	char* name = NULL;
	unicodeToAnsi(playerInfo.content.get(), name);
	strncpy( tagPlayer.Name, name, CLIENT_NAME_AND_TITLE_MAX + 1);
	delete [] name;
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	tagPlayer.uId = playerInfo.gameObj._objId[0];

	g_pCoreShell->TeamOperation(TEAM_OI_APPLY_JOIN, (unsigned int)&tagPlayer, NULL );
	return ;
}
void ChatTipWnd::ChatTipClickZhuhouButton()
{
	UIMDL::GetMDLPtr ( &pUiMdl	);
	char* name = NULL;
	unicodeToAnsi(playerInfo.content.get(), name);
	if (name[0]!=0)
	{
		TongOperParam tagTongOper;
		ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
		tagTongOper.nTemplateID		= enSUTplId_Tong;
		tagTongOper.nLayerID		= enSULayer_Tong;
		tagTongOper.nOperationID	= enSUO_AddSubUnit;
		strcpy(tagTongOper.szName,name);
		delete [] name;
		
		IUIMDLDataset* pTongOper = NULL;
		if ( success_errorcode != pUiMdl->queryDataSet( tong_operation, &pTongOper ) )
		{
			return ;
		}//endif
		
		pTongOper->updateRecord( enSULayer_Tong, &tagTongOper, sizeof( TongOperParam ) );	
		
	}//endif

	return ;
}

LRESULT CALLBACK ChatTipWnd::ChatTipChildButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_MOUSEMOVE:
		{
			for(int i = 0; i < _CHAT_WND_BUTTON_NUM;i++)
			{
				ChatButton* pButtton = B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipGetButton()+i;
				if(pButtton->ChatWndGetHandle() == hwnd)
				{
					if(pButtton->ChatWndGetState()!= _CHAT_BUTTON_STATE_MOUSEOVER)
					{
						pButtton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
						pButtton->ChatWndUpdate();
					}
				}
				else
				{
					if(pButtton->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
					{
						pButtton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
						pButtton->ChatWndUpdate();
					}
				}
			}
			return 0;
		}
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			B2ChatDialog::SendKeyDownMsgToMainWnd(msg,wParam,lParam);

		}
		return FALSE;
	case WM_LBUTTONDOWN:
		{
			int id = GetWindowLong(hwnd,GWL_ID);
			switch(id)
			{
			case _CHAT_TIP_PERSONAL_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickPersonalButton();
				break;
			case _CHAT_TIP_TRADE_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickTradeButton();
				break;
			case _CHAT_TIP_TEAM_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickTeamButton();
				break;
			case _CHAT_TIP_FOLLOW_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickFollowButton();
				break;
			case _CHAT_TIP_ARM_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickArmButton();
				break;
			case _CHAT_TIP_HIDE_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickHideButton();
				break;
			case _CHAT_TIP_FRIEND_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickFriendButton();
				break;
			case _CHAT_TIP_SIZHU_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickSizhuButton();
				break;
			case _CHAT_TIP_ZHUHOU_BUTTON_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickZhuhouButton();
				break;
			case _CHAT_TIP_SHENGQING_JIARU_ID:
				B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipClickJoinTeamButton();
				break;

			}
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		}
		return 0;

	}
	for(int i = 0; i<_CHAT_WND_BUTTON_NUM;i++)
	{
		ChatButton* pButtton = B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipGetButton()+i;
		if(pButtton->ChatWndGetHandle() == hwnd)
		{
			break;
		}
	}
	return CallWindowProc(B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipGetButtonPoc(i),hwnd,msg,wParam,lParam);
}
BOOL CALLBACK ChatTipWnd::ChatTipWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipProcessCreate(hwnd);
//			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipAdjustWindow(hwnd);
			SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		}
		return 0;
	case WM_DRAWITEM:
		{
			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipProcessDrawItem((LPDRAWITEMSTRUCT)lParam);
//			return 0;
		}
		return 0;
	case WM_PAINT:
		{
//			B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipAdjustWindow(hwnd);	
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
		}
		return 0;
	case WM_ERASEBKGND:
		{

		}
		return TRUE;
	case WM_DESTROY:
		{
		}
		return 0;
	}
	return 0;
}

void ChatTipWnd::ChatTipProcessCreate(HWND hwnd)
{
#define _CHAT_BUTTON_TEXT   "textInfo"
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	char* buttonsName[] = {"tipWndPersonal","tipWndTrade","tipWndTeam","tipWndFollow","tipWndArm","tipWndHide","tipWndFriend","tipWndSizhu","tipWndZhuhou","tipWndJoinTeam"};
	int x,y;
	int width;
	int height;
	int isClip;
	int r,g,b;
	int id = _CHAT_TIP_PERSONAL_BUTTON_ID;
	for(int i = 0;i<_CHAT_WND_BUTTON_NUM;i++)
	{
		iniFile.GetInteger(buttonsName[i],_CHAT_SRC_X,0,&x);
		iniFile.GetInteger(buttonsName[i],_CHAT_SRC_Y,0,&y);
		iniFile.GetInteger(buttonsName[i],_CHAT_SRC_WIDTH,0,&width);
		iniFile.GetInteger(buttonsName[i],_CHAT_SRC_HEIGHT,0,&height);
		iniFile.GetInteger(buttonsName[i],_CHAT_SRC_CLIP,0,&isClip);

/*		if(isClip ==1)
		{
			iniFile.GetInteger(buttonsName[i],_CHAT_SRC_CLIP_COLOR_R,0,&r);
			iniFile.GetInteger(buttonsName[i],_CHAT_SRC_CLIP_COLOR_G,0,&g);
			iniFile.GetInteger(buttonsName[i],_CHAT_SRC_CLIP_COLOR_B,0,&b);
		}*/
		buttons[i].ChatWndCreate(id+i,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY ,hwnd,"","button",x,y,width,height);
		int normal_idx = 0,hovermal_idx = 0;

		iniFile.GetInteger(buttonsName[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(buttonsName[i],"mouseOverSrcIdx",0,&hovermal_idx);
		buttons[i].ChatWndSetResource(normal_idx,hovermal_idx,-1,-1);
		buttons[i].ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		char textInfo[256]={0};
		iniFile.GetString(buttonsName[i],_CHAT_BUTTON_TEXT,"",textInfo,256);
		if(textInfo[0]!=0)
		{
			buttons[i].ChatWndSetText(textInfo);
			buttons[i].ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
			iniFile.GetString(buttonsName[i],"fontColor","",szValue,MAX_PATH);
			sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			buttons[i].ChatWndSetTextNormalColor(RGB(r,g,b));
			char font[FONT_SIZE]={0};
			iniFile.GetString(buttonsName[i],"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				buttons[i].ChatWndSetFont(font);
			else
			{
				buttons[i].ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				buttons[i].ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		iniFile.GetString(buttonsName[i],"tooltipInfo","",szValue,MAX_PATH);
	    if(szValue[0] != 0)
		     buttons[i].ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
		buttonsProc[i] = (WNDPROC)SetWindowLong(buttons[i].ChatWndGetHandle(),GWL_WNDPROC,(LONG)(ChatTipWnd::ChatTipChildButtonProc));
	}

}
void ChatTipWnd::ChatTipShow(BOOL bShow)
{
	isShow = bShow;
	if(bShow)
	{
	//	SetWindowPos(hWndHandle,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
		
		if(!IsWindowVisible(hWndHandle))
		{
			ShowWindow(hWndHandle,SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(hWndHandle))
		{
			ShowWindow(hWndHandle,SW_HIDE);
		}
	}
}
void ChatTipWnd::ChatTipProcessDrawItem(LPDRAWITEMSTRUCT lpdis)
{
	for(int i = 0; i < _CHAT_WND_BUTTON_NUM;i++ )
	{
		if(buttons[i].ChatWndGetID() == lpdis->CtlID)
		{
			buttons[i].ChatWndDrawItem(lpdis->hDC);
			return ;
		}
	}
}