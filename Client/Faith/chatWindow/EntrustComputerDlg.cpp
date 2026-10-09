#include "KWin32App.h"
#include <commctrl.h>
#include <windowsx.h>
#include <vector>
using std::vector;
#include "layoutinterface.h"
#include "chatWindow/chatWnd.h"

#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/EntrustComputerDlg.h"

#include "KIniFile.h"
#include "loadSrcWnd/GDILoadBitmap.h"
#include "chatWindow/ChatControlPanel.h"
#include "chatWindow/ChatResource.h"

#include "resource.h"
#include "CoreShell.h"

#include "AutoTrust.h"
#include "chatWindow/ChatWndProc.h"
#include "../Ui/UiCase/UiEntrustComputer.h"

extern iCoreShell*							g_pCoreShell;


CheckedButtonGroup::CheckedButtonGroup()
{
	mainButtonEnable = false;
	lastMainButtonEnable = false;
}
CheckedButtonGroup::~CheckedButtonGroup()
{
	while(!childButtonList.empty())
	{
		ChatButton* pButton = childButtonList.back();
		childButtonList.pop_back();
		delete pButton;
	}
}
/*bool CheckedButtonGroup::CheckedButtonIsChangedSelected()
{
	if(lastMainButtonEnable != mainButtonEnable)
		return true;
	if(lastMainButtonEnable)
	{
		int size = lastChildButtonEnabel.size();
		for(int i = 0;i<size;i++)
		{
			if(lastChildButtonEnabel[i]!=childButtonEnabel[i])
				return true;
		}
	}
	return false;
}*/
void CheckedButtonGroup::CheckedButtonSetAutoAttack(bool enable)
{
	if(enable)
	{
		mainButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
		mainButtonEnable = true;
		mainButton.ChatWndUpdate();
		int size = childButtonList.size();
		for(int i = 0; i < size; i ++)
		{
			ChatButton* pButton = childButtonList[i];
			pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pButton->ChatWndUpdate();
			childButtonEnabel[i] = false;
		}
		CheckedButtonSetLastSelected();
	}
	else
	{
		mainButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		mainButtonEnable = false;
		mainButton.ChatWndUpdate();
		int size = childButtonList.size();
		for(int i = 0; i < size; i ++)
		{
			ChatButton* pButton = childButtonList[i];
			pButton->ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			pButton->ChatWndUpdate();
			childButtonEnabel[i] = false;
		}
		CheckedButtonSetLastSelected();
	}

}
void CheckedButtonGroup::CheckedButtonSetLastSelected()
{
	lastMainButtonEnable = mainButtonEnable;
	int size = lastChildButtonEnabel.size();
	for(int i = 0;i<size;i++)
	{
		lastChildButtonEnabel[i] = childButtonEnabel[i];
	}
}
void CheckedButtonGroup::CheckedButtonDrawTextBk(HDC hParentDC)
{
	int mode = SetBkMode(hParentDC,TRANSPARENT);
	int fontHeight = 0;
	int fontWeight = 0;
	RECT rect = mainButton.ChatWndGetControlRect();
	if(mainButton.ChatWndGetAttr()&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
	{
		fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
		fontWeight = FW_NORMAL;
	}
	else
	{
		fontHeight = rect.bottom-4;
		fontWeight = FW_BOLD;
	}
	HFONT hFont = CreateFont(fontHeight,0,0,0,fontWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,mainButton.ChatWndGetFontName());
	HFONT hOldFont = (HFONT)SelectObject(hParentDC,hFont);
	int color = SetTextColor(hParentDC,mainButton.ChatWndGetNormalColor());
						SIZE textSize;
	int len = strlen(mainButton.ChatWndGetText());
	GetTextExtentPoint32(hParentDC,mainButton.ChatWndGetText(),len,&textSize);
	int drawX = mainButton.x + rect.right+1;
	int drawY = mainButton.y + rect.bottom/2-textSize.cy/2;
	RECT rc;
	rc.left = drawX ;
	rc.top = drawY;
	rc.right = rc.left +textSize.cx;
	rc.bottom = rc.top + textSize.cy;
	TextOut(hParentDC,drawX,drawY,mainButton.ChatWndGetText(),len);
	SetTextColor(hParentDC,color);
	SelectObject(hParentDC,hOldFont);
	SetBkMode(hParentDC,mode);
	DeleteObject(hFont);

	int size = childButtonList.size();
	for(int i = 0; i < size; i++)
	{
		int mode = SetBkMode(hParentDC,TRANSPARENT);
		int fontHeight = 0;
		int fontWeight = 0;
		ChatButton* pButton = childButtonList[i];
		RECT rect = pButton->ChatWndGetControlRect();
		if(pButton->ChatWndGetAttr()&_CHAT_WND_ATTR_USE_DEFUALT_FONT)
		{
			fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;
			fontWeight = FW_NORMAL;
		}
		else
		{
			fontHeight = rect.bottom-4;
			fontWeight = FW_BOLD;
		}
		HFONT hFont = CreateFont(fontHeight,0,0,0,fontWeight,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,pButton->ChatWndGetFontName());
		HFONT hOldFont = (HFONT)SelectObject(hParentDC,hFont);
		int color = SetTextColor(hParentDC,pButton->ChatWndGetNormalColor());
							SIZE textSize;
		int len = strlen(pButton->ChatWndGetText());
		GetTextExtentPoint32(hParentDC,pButton->ChatWndGetText(),len,&textSize);
		int drawX = pButton->x + rect.right+1;
		int drawY = pButton->y + rect.bottom/2-textSize.cy/2;
		RECT rc;
		rc.left = drawX ;
		rc.top = drawY;
		rc.right = rc.left +textSize.cx;
		rc.bottom = rc.top + textSize.cy;
		TextOut(hParentDC,drawX,drawY,pButton->ChatWndGetText(),len);
		SetTextColor(hParentDC,color);
		SelectObject(hParentDC,hOldFont);
		SetBkMode(hParentDC,mode);
		DeleteObject(hFont);
	}
}
void CheckedButtonGroup::CheckedButtonUpdate()
{
	mainButton.ChatWndUpdate();
	int size = childButtonList.size();
	for(int i = 0; i < size;i++)
	{
		ChatButton* pButton = childButtonList[i];
		pButton->ChatWndUpdate();
	}
}
bool CheckedButtonGroup::CheckedButtonProcessLButtonDown(WPARAM wParam ,LPARAM lParam)
{
	int buttonID = LOWORD(wParam);
	if(HIWORD(wParam) == BN_CLICKED)
	{
		if(buttonID == mainButton.ChatWndGetID())
		{
			if(mainButtonEnable)
			{
				int size = childButtonList.size();
				for(int i = 0; i < size; i ++)
				{
					ChatButton* pButton = childButtonList[i];
					if(pButton->ChatWndGetState() != _CHAT_BUTTON_STATE_DISABLE)
					{
						pButton->ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
						pButton->ChatWndUpdate();
					}
					childButtonEnabel[i] = false;
				}
				mainButtonEnable = false;
				if(mainButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEDOWN)
				{
					mainButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					mainButton.ChatWndUpdate();
				}
			}
			else
			{
				int size = childButtonList.size();
				for(int i = 0; i < size; i ++)
				{
					ChatButton* pButton = childButtonList[i];
					if(pButton->ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
					{
						pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
						pButton->ChatWndUpdate();
					}
				}
				mainButtonEnable = true;
				if(mainButton.ChatWndGetState() == _CHAT_BUTTON_STATE_NORMAL)
				{
					mainButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					mainButton.ChatWndUpdate();
				}
			}
			return true;
		}

		if(!mainButtonEnable)
			return false;
		int size = childButtonList.size();
		for(int index = 0; index < size; index++)
		{
			ChatButton* pButton = childButtonList[index];
			if(pButton->ChatWndGetID() == buttonID)
			{
				if(pButton->ChatWndGetState() == _CHAT_BUTTON_STATE_DISABLE)
				{
					childButtonEnabel[index] = false;
				}
				else if(childButtonEnabel[index] == true&&pButton->ChatWndGetState()==_CHAT_BUTTON_STATE_MOUSEDOWN)
				{
					pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
					pButton->ChatWndUpdate();
					childButtonEnabel[index] = false;
					
				}
				else
				{
					pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);
					pButton->ChatWndUpdate();
					childButtonEnabel[index] = true;
					MutexButtonsIndex& mutexIndex = mutexIndexList[index];
					if(mutexIndex.numberButtons >0)
					{
						for(int i = 0; i < mutexIndex.numberButtons; i ++ )
						{
							if(mutexIndex.buttonIndexList[i] != index)
							{
								ChatButton* pMutexButton = childButtonList[mutexIndex.buttonIndexList[i]];
								if(childButtonEnabel[mutexIndex.buttonIndexList[i]])
								{
									pMutexButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
									pMutexButton->ChatWndUpdate();
									childButtonEnabel[mutexIndex.buttonIndexList[i]] = false;
								}
							}
						}
					}
				}
				return true;
			}
		}

	}
	return false;
}
bool CheckedButtonGroup::CheckedButtonDrawItem(LPDRAWITEMSTRUCT lpdis)
{
	if(mainButton.ChatWndGetID() == lpdis->CtlID)
	{
		mainButton.ChatWndDrawItem(lpdis->hDC);
		return true;
	}
	int size = childButtonList.size();
	for(int i = 0; i < size; i ++)
	{
		ChatButton* pButton  = childButtonList[i];
		if(pButton->ChatWndGetID() == lpdis->CtlID)
		{
			pButton->ChatWndDrawItem(lpdis->hDC);
			return true;
		}
	}
	return false;
}

/*bool CheckedButtonGroup::CheckedButtonProcessMouseLeave(WPARAM wParam,LPARAM lParam)
{
	POINT mouse_pt;
	POINT pt;
	GetCursorPos(&mouse_pt);
	RECT btn_rect;
	pt.x = mouse_pt.x;
	pt.y = mouse_pt.y;
	GetClientRect(mainButton.ChatWndGetHandle(),&btn_rect);
	ScreenToClient(mainButton.ChatWndGetHandle(),&pt);
	if(IsInRect(pt,btn_rect))
	{
		if(mainButton.ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN)
		{
			mainButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			mainButton.ChatWndUpdate();
			return true;
		}
	}
	if(!mainButtonEnable)
		return false;
	int size = childButtonList.size();
	for(int i = 0; i < size; i ++)
	{
		ChatButton* pButton = childButtonList[i];
		pt.x = mouse_pt.x;
		pt.y = mouse_pt.y;
		GetClientRect(pButton->ChatWndGetHandle(),&btn_rect);
		ScreenToClient(pButton->ChatWndGetHandle(),&pt);
		if(IsInRect(pt,btn_rect))
		{
			if(pButton->ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN)
			{
				pButton->ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
				pButton->ChatWndUpdate();
				return true;
			}
		}

	}
	return false;
}*/
/*bool CheckedButtonGroup::CheckedButtonProcessMouseMove(WPARAM wParam,LPARAM lParam)
{
	if(mainButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		mainButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		mainButton.ChatWndUpdate();
	}
	if(!mainButtonEnable)
		return true;
	int size = childButtonList.size();
	for(int i = 0; i < size; i++)
	{
		ChatButton* pButton  = childButtonList[i];
		if(pButton->ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
		{
			pButton->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
			pButton->ChatWndUpdate();
		}
	}
	return true;
}*/
EntrustComputerDlg::EntrustComputerDlg()
{
	dlgBkBitmapIdx = 0;
	hDlg = 0;
	dlgPosX = dlgPosY = 0;
}

EntrustComputerDlg::~EntrustComputerDlg()
{

}
void  EntrustComputerDlg::EntrustDlgShowDlg(bool show)
{
	if(show)
	{
		ShowWindow(hDlg,SW_SHOW);
		
	}
	else
	{
		ShowWindow(hDlg,SW_HIDE);
	}
}
void EntrustComputerDlg::EntrustDlgAdjustWindow()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
	//RECT rcLeft;
//	GetClientRect(ChatControlPanel::ChatPanelGetPanel().hPanelDlg,&rcLeft);
//	int height = rClient.bottom;
//	int width = rClient.right-rcLeft.right;
	AdjustWindowRectEx(&rClient,GetWindowLong(ChatMainDlg::hMainDlg,GWL_STYLE),GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));

	int dx = rClient.left;
	int dy = rClient.top;
	int x = rc.left - dx+dlgPosX;
	int y = rc.top-dy+dlgPosY;
	MoveWindow(hDlg,x,y,dlgWidth ,dlgHeight,TRUE);
}
EntrustComputerDlg& EntrustComputerDlg::EntrustDlgGetSingleton()
{
	static EntrustComputerDlg entrustDlg;
	return entrustDlg;
}
void EntrustComputerDlg::EntrustDlgLoadSrc()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(_ENTRUST_PAGE_NAME_INI,"BkSrcIdx",0,&dlgBkBitmapIdx);
	iniFile.GetInteger(_ENTRUST_PAGE_NAME_INI,"width",0,&dlgWidth);
	iniFile.GetInteger(_ENTRUST_PAGE_NAME_INI,"height",0,&dlgHeight);


	/*titleButton-source*/
	int normal_idx = 0,hover_idx = 0,pushed_idx = 0,disable_idx = 0;;
	iniFile.GetInteger("entrust-title","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("entrust-title","mouseOverSrcIdx",0,&hover_idx);
	iniFile.GetInteger("entrust-title","mouseDownSrcIdx",0,&pushed_idx);
	titleButton.ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);
	titleButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEDOWN);

	/*okbutton-source*/

	iniFile.GetInteger("entrust-ok","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("entrust-ok","mouseOverSrcIdx",0,&hover_idx);
	iniFile.GetInteger("entrust-ok","mouseDownSrcIdx",0,&pushed_idx);
	okButton.ChatWndSetResource(normal_idx,hover_idx,pushed_idx,-1);

	//autoattack-src/////////////////////////////////////////////////
	//maiButton////////////////////////////////////////
	iniFile.GetInteger("autoAttack","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("autoAttack","mouseDownSrcIdx",0,&pushed_idx);
	autoAttackGroup.mainButton.ChatWndSetResource(normal_idx,-1,pushed_idx,-1);
	//child//
	int size = autoAttackGroup.childButtonList.size();
	const char* autoAttackChildButton[] = {"autoAttack-level-higher","autoAttack-level-lower","autoAttack-go-home","autoAttack-only-normal-set"};
	for(int i = 0; i < size; i ++)
	{
		iniFile.GetInteger(autoAttackChildButton[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(autoAttackChildButton[i],"mouseDownSrcIdx",0,&pushed_idx);
		iniFile.GetInteger(autoAttackChildButton[i],"disableSrcIdx",0,&disable_idx);
		ChatButton* pButton = autoAttackGroup.childButtonList[i];
		pButton->ChatWndSetResource(normal_idx,-1,pushed_idx,disable_idx);

	}	

	///autoPickup - src//
	//mainbutton//
	iniFile.GetInteger("autoPickup","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("autoPickup","mouseDownSrcIdx",0,&pushed_idx);
	autoPickUpGroup.mainButton.ChatWndSetResource(normal_idx,-1,pushed_idx,-1);
	//child//
	size = autoPickUpGroup.childButtonList.size();
	const char* autoPickUpChildButton[] = {"autoPickup-dear_first","autoPickup-blue","autoPickup-green", "autoPickup-medicine"};
	for(i = 0; i < size; i ++)
	{
		iniFile.GetInteger(autoPickUpChildButton[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(autoPickUpChildButton[i],"mouseDownSrcIdx",0,&pushed_idx);
		iniFile.GetInteger(autoPickUpChildButton[i],"disableSrcIdx",0,&disable_idx);
		ChatButton* pButton = autoPickUpGroup.childButtonList[i];
		pButton->ChatWndSetResource(normal_idx,-1,pushed_idx,disable_idx);
	}	

	//autopickup//
	//mainbutton//
	iniFile.GetInteger("autoUseItem","normalSrcIdx",0,&normal_idx);
	iniFile.GetInteger("autoUseItem","mouseDownSrcIdx",0,&pushed_idx);
	autoUseItemGroup.mainButton.ChatWndSetResource(normal_idx,-1,pushed_idx,-1);
	//child//
	size = autoUseItemGroup.childButtonList.size();
	const char* autoUseItemChildButton[] = {"autoUseItem-hp-60","autoUseItem-hp-40","autoUseItem-hp-owner","autoUseItem-mp-20"};
	for(i = 0; i < size; i ++)
	{
		iniFile.GetInteger(autoUseItemChildButton[i],"normalSrcIdx",0,&normal_idx);
		iniFile.GetInteger(autoUseItemChildButton[i],"mouseDownSrcIdx",0,&pushed_idx);
		iniFile.GetInteger(autoUseItemChildButton[i],"disableSrcIdx",0,&disable_idx);
		ChatButton* pButton = autoUseItemGroup.childButtonList[i];
		pButton->ChatWndSetResource(normal_idx,-1,pushed_idx,disable_idx);

	}	

	//隐藏/显示按钮和切换按钮
	int button_Pos_X = 0;
	int button_Pos_Y = 0;
	int button_Width = 0;
	int button_Height = 0;
	normal_idx = 0;
	hover_idx = 0;

	iniFile.GetInteger(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT, "x", 0, &button_Pos_X);
	iniFile.GetInteger(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT, "y", 0, &button_Pos_Y);
	iniFile.GetInteger(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT, "width", 0, &button_Width);
	iniFile.GetInteger(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT, "height", 0, &button_Height);
	iniFile.GetInteger(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT, "normalSrcIdx", 0, &normal_idx);
	iniFile.GetInteger(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT, "mouseOverSrcIdx",0,&hover_idx);

	m_wndHideShowBnt.ChatWndCreate(_ENTRUST_HIDE_SHOW_WND_BNT_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hDlg, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height);
	m_wndHideShowBnt.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	char textInfo[256]={0};
	iniFile.GetString(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT,"textInfo","",textInfo,256);
	if(textInfo[0]!=0)
	{
		m_wndHideShowBnt.ChatWndSetText(textInfo);
		m_wndHideShowBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT,"fontColor","",szValue,MAX_PATH);

		int r = 0;
		int g = 0;
		int b = 0;
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		m_wndHideShowBnt.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_ENTRUST_HIDE_SHOW_WND_BNT_TEXT,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_wndHideShowBnt.ChatWndSetFont(font);
		else
		{
			m_wndHideShowBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_wndHideShowBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	m_wndHideShowBnt.SetWndProcessFun(ChatWndProcessFun::ProcessEntrustHideShowButton);
	m_wndHideShowBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndHideShowBnt.ChatWndUpdate();

	iniFile.GetInteger(_ENTRUST_CHANGE_WND_BNT_TEXT, "x", 0, &button_Pos_X);
	iniFile.GetInteger(_ENTRUST_CHANGE_WND_BNT_TEXT, "y", 0, &button_Pos_Y);
	iniFile.GetInteger(_ENTRUST_CHANGE_WND_BNT_TEXT, "width", 0, &button_Width);
	iniFile.GetInteger(_ENTRUST_CHANGE_WND_BNT_TEXT, "height", 0, &button_Height);
	iniFile.GetInteger(_ENTRUST_CHANGE_WND_BNT_TEXT, "normalSrcIdx", 0, &normal_idx);
	iniFile.GetInteger(_ENTRUST_CHANGE_WND_BNT_TEXT, "mouseOverSrcIdx",0,&hover_idx);
	
	m_wndChangeWndBnt.ChatWndCreate(_ENTRUST_CHANGE_WND_BNT_ID, WS_VISIBLE|WS_CHILD|BS_OWNERDRAW,
		hDlg, "", "button", button_Pos_X, button_Pos_Y, button_Width, button_Height);
	m_wndChangeWndBnt.ChatWndSetResource(normal_idx,hover_idx,-1,-1);
	memset(textInfo, 0, 256);
	iniFile.GetString(_ENTRUST_CHANGE_WND_BNT_TEXT,"textInfo","",textInfo,256);
	if(textInfo[0]!=0)
	{
		m_wndChangeWndBnt.ChatWndSetText(textInfo);
		m_wndChangeWndBnt.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString(_ENTRUST_CHANGE_WND_BNT_TEXT,"fontColor","",szValue,MAX_PATH);

		int r = 0;
		int g = 0;
		int b = 0;
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		m_wndChangeWndBnt.ChatWndSetTextNormalColor(RGB(r,g,b));

		char font[FONT_SIZE]={0};
		iniFile.GetString(_ENTRUST_CHANGE_WND_BNT_TEXT,"font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			m_wndChangeWndBnt.ChatWndSetFont(font);
		else
		{
			m_wndChangeWndBnt.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			m_wndChangeWndBnt.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	m_wndChangeWndBnt.SetWndProcessFun(ChatWndProcessFun::ProcessEntrustChangeButton);
	m_wndChangeWndBnt.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
	m_wndChangeWndBnt.ChatWndUpdate();


}
void EntrustComputerDlg::EntrustDlgInitFromCtfIni(HWND hParent)
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);

	iniFile.GetInteger(_ENTRUST_PAGE_NAME_INI,"x",0,&dlgPosX);

	iniFile.GetInteger(_ENTRUST_PAGE_NAME_INI,"y",0,&dlgPosY);
	hDlg = CreateDialog(KWin32App::m_hInstance,MAKEINTRESOURCE(IDD_ENTRUST_DLG),hParent,(DLGPROC)EntrustComputerDlg::EntrustDlgProc);

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	iniFile.GetInteger("entrust-title","x",0,&x);
	iniFile.GetInteger("entrust-title","y",0,&y);
	iniFile.GetInteger("entrust-title","width",0,&width);
	iniFile.GetInteger("entrust-title","height",0,&height);
	titleButton.ChatWndCreate(_ENTRUST_TITLE_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
		                      hDlg,"","button",x,y,width,height);
	//字体//
	char infoText[256] = {0};
	iniFile.GetString("entrust-title","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		titleButton.ChatWndSetText(infoText);
		titleButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("entrust-title","fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		titleButton.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("entrust-title","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			titleButton.ChatWndSetFont(font);
		else
		{
			titleButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			titleButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}

	iniFile.GetInteger("entrust-ok","x",0,&x);
	iniFile.GetInteger("entrust-ok","y",0,&y);
	iniFile.GetInteger("entrust-ok","width",0,&width);
	iniFile.GetInteger("entrust-ok","height",0,&height);
	okButton.ChatWndCreate(_ENTRUST_OK_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
		                      hDlg,"","button",x,y,width,height);
	iniFile.GetString("entrust-ok","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		okButton.ChatWndSetText(infoText);
		okButton.ChatWndSetAttr(_CHAT_WND_ATTR_TEXT_H);
		iniFile.GetString("entrust-ok","fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		okButton.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("entrust-ok","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			okButton.ChatWndSetFont(font);
		else
		{
			okButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			okButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	ShowWindow(okButton.ChatWndGetHandle(),SW_HIDE);
	EntrustDlgAutoAtackIni(hParent);
	EntrustDlgAutoPickUpIni(hParent);
	EntrustDlgAutoUseItemIni(hParent);
	EntrustDlgLoadSrc();

}

void EntrustComputerDlg::EntrustDlgAutoAtackIni(HWND hParent)
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	///// mainbutton///
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	char infoText[256] = {0};
	iniFile.GetInteger("autoAttack","x",0,&x);
	iniFile.GetInteger("autoAttack","y",0,&y);
	iniFile.GetInteger("autoAttack","width",0,&width);
	iniFile.GetInteger("autoAttack","height",0,&height);
	autoAttackGroup.mainButton.ChatWndCreate(_ENTRUST_AUTOATTACK_MAIN_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
		                      hDlg,"","button",x,y,width,height);
	iniFile.GetString("autoAttack","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		autoAttackGroup.mainButton.ChatWndSetText(infoText);
		iniFile.GetString("autoAttack","fontColor","",szValue,MAX_PATH);
		sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		autoAttackGroup.mainButton.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("autoAttack","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			autoAttackGroup.mainButton.ChatWndSetFont(font);
		else
		{
			autoAttackGroup.mainButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			autoAttackGroup.mainButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	autoAttackGroup.mainButton.ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);

	const char* autoAttackChildButton[] = {"autoAttack-level-higher","autoAttack-level-lower","autoAttack-go-home","autoAttack-only-normal-set"};
	int autoAttackChildID = _ENTRUST_AUTOATTACK_ENEMY_LEVEL_HIGHER_BUTTON_ID;
	for(int i = 0 ; i < _ENTRUST_AUTOATTACK_CHILD_BUTTONS_NUMBERS; i++)
	{
		iniFile.GetInteger(autoAttackChildButton[i],"x",0,&x);
		iniFile.GetInteger(autoAttackChildButton[i],"y",0,&y);
		iniFile.GetInteger(autoAttackChildButton[i],"width",0,&width);
		iniFile.GetInteger(autoAttackChildButton[i],"height",0,&height);
		ChatButton* pButton = new ChatButton;
		pButton->ChatWndCreate(autoAttackChildID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
								  hDlg,"","button",x,y,width,height);

		iniFile.GetString(autoAttackChildButton[i],"textInfo","",infoText,256);
		autoAttackChildID++;
		if(infoText[0]!=0)
		{
			pButton->ChatWndSetText(infoText);
			iniFile.GetString(autoAttackChildButton[i],"fontColor","",szValue,MAX_PATH);
	    	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			pButton->ChatWndSetTextNormalColor(RGB(r,g,b));
			char font[FONT_SIZE]={0};
			iniFile.GetString(autoAttackChildButton[i],"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				pButton->ChatWndSetFont(font);
			else
			{
				pButton->ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				pButton->ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		pButton->ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);
		autoAttackGroup.childButtonList.push_back(pButton);
		autoAttackGroup.childButtonEnabel.push_back(false);
		autoAttackGroup.lastChildButtonEnabel.push_back(false);
		MutexButtonsIndex mutexIndex;
		autoAttackGroup.mutexIndexList.push_back(mutexIndex);
		pButton->ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
	}
	int  enable = 0;
	iniFile.GetInteger("autoAttack-go-home","enable",0,&enable);
	if(enable == 0)
	{
		ChatButton* button = autoAttackGroup.childButtonList[_ENTRUST_AUTOATTACK_GO_HOME_BUTTON_INDEX];
	//	EnableWindow(button->ChatWndGetHandle(),false);
		ChatButton* button1 = autoAttackGroup.childButtonList[_ENTRUST_AUTOATTACK_ONLY_NORMAL_SET_BUTTON_INDEX];
	//	EnableWindow(button1->ChatWndGetHandle(),false);
	}
	

}

void EntrustComputerDlg::EntrustDlgAutoPickUpIni(HWND hParent)
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	///// mainbutton///
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	char infoText[256] = {0};
	iniFile.GetInteger("autoPickup","x",0,&x);
	iniFile.GetInteger("autoPickup","y",0,&y);
	iniFile.GetInteger("autoPickup","width",0,&width);
	iniFile.GetInteger("autoPickup","height",0,&height);
	autoPickUpGroup.mainButton.ChatWndCreate(_ENTRUST_AUTOPICKUP_MAIN_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
		                      hDlg,"","button",x,y,width,height);
	iniFile.GetString("autoPickup","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		autoPickUpGroup.mainButton.ChatWndSetText(infoText);
		iniFile.GetString("autoPickup","fontColor","",szValue,MAX_PATH);
	    sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		autoPickUpGroup.mainButton.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("autoPickup","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			autoPickUpGroup.mainButton.ChatWndSetFont(font);
		else
		{
			autoPickUpGroup.mainButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			autoPickUpGroup.mainButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	autoPickUpGroup.mainButton.ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);

	const char* autoAttackChildButton[] = {"autoPickup-dear_first","autoPickup-blue", "autoPickup-green", "autoPickup-medicine"};
	int autoAttackChildID = _ENTRUST_AUTOPICKUP_DEAR_FIRST_BUTTON_ID;
	for(int i = 0 ; i < _ENTRUST_AUTOPICKUP_CHILD_BUTTONS_NUMBERS; i++)
	{
		iniFile.GetInteger(autoAttackChildButton[i],"x",0,&x);
		iniFile.GetInteger(autoAttackChildButton[i],"y",0,&y);
		iniFile.GetInteger(autoAttackChildButton[i],"width",0,&width);
		iniFile.GetInteger(autoAttackChildButton[i],"height",0,&height);
		ChatButton* pButton = new ChatButton;
		pButton->ChatWndCreate(autoAttackChildID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
								  hDlg,"","button",x,y,width,height);

		iniFile.GetString(autoAttackChildButton[i],"textInfo","",infoText,256);
		autoAttackChildID++;
		if(infoText[0]!=0)
		{
			pButton->ChatWndSetText(infoText);
			iniFile.GetString(autoAttackChildButton[i],"fontColor","",szValue,MAX_PATH);
	    	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			pButton->ChatWndSetTextNormalColor(RGB(r,g,b));
			char font[FONT_SIZE]={0};
			iniFile.GetString(autoAttackChildButton[i],"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				pButton->ChatWndSetFont(font);
			else
			{
				pButton->ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				pButton->ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		pButton->ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);
		autoPickUpGroup.childButtonList.push_back(pButton);
		autoPickUpGroup.childButtonEnabel.push_back(false);
		autoPickUpGroup.lastChildButtonEnabel.push_back(false);
		MutexButtonsIndex mutexIndex;
		autoPickUpGroup.mutexIndexList.push_back(mutexIndex);
		pButton->ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
	}

	MutexButtonsIndex& mutexIndex = autoPickUpGroup.mutexIndexList[_ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_INDEX];
	mutexIndex.MutexIndexAddIndex(_ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_INDEX);
	mutexIndex.MutexIndexAddIndex(_ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_INDEX);
	MutexButtonsIndex& mutexIndex1 = autoPickUpGroup.mutexIndexList[_ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_INDEX];
	mutexIndex1.MutexIndexAddIndex(_ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_INDEX);
	mutexIndex1.MutexIndexAddIndex(_ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_INDEX);

}
void EntrustComputerDlg::EntrustDlgAutoUseItemIni(HWND hParent)
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH]={0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	///// mainbutton///
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	char infoText[256] = {0};
	iniFile.GetInteger("autoUseItem","x",0,&x);
	iniFile.GetInteger("autoUseItem","y",0,&y);
	iniFile.GetInteger("autoUseItem","width",0,&width);
	iniFile.GetInteger("autoUseItem","height",0,&height);
	autoUseItemGroup.mainButton.ChatWndCreate(_ENTRUST_AUTOUSEITEM_MAIN_BUTTON_ID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
		                      hDlg,"","button",x,y,width,height);
	iniFile.GetString("autoUseItem","textInfo","",infoText,256);
	if(infoText[0]!=0)
	{
		autoUseItemGroup.mainButton.ChatWndSetText(infoText);
		iniFile.GetString("autoUseItem","fontColor","",szValue,MAX_PATH);
	    sscanf(szValue,"%d,%d,%d",&r,&g,&b);
		autoUseItemGroup.mainButton.ChatWndSetTextNormalColor(RGB(r,g,b));
		char font[FONT_SIZE]={0};
		iniFile.GetString("autoUseItem","font","",font,FONT_SIZE);
		TFONT tFont;
		strcpy(tFont.fontName,font);
		HDC hdc =CreateCompatibleDC(NULL);
		LOGFONT logFont;
		logFont.lfFaceName[0]=0;
		logFont.lfCharSet = DEFAULT_CHARSET;
		EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
		DeleteDC(hdc);
		if(tFont.isInSystem)
			autoUseItemGroup.mainButton.ChatWndSetFont(font);
		else
		{
			autoUseItemGroup.mainButton.ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
			autoUseItemGroup.mainButton.ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
		}
	}
	autoUseItemGroup.mainButton.ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);

	const char* autoUseItemChildButton[] = {"autoUseItem-hp-60","autoUseItem-hp-40","autoUseItem-hp-owner","autoUseItem-mp-20"};
	int autoAttackChildID = _ENTRUST_AUTOUSEITEM_HP_60_BUTTON_ID;
	for(int i = 0 ; i < _ENTRUST_AUTOUSEITEM_CHILD_BUTTONS_NUMBERS; i++)
	{
		iniFile.GetInteger(autoUseItemChildButton[i],"x",0,&x);
		iniFile.GetInteger(autoUseItemChildButton[i],"y",0,&y);
		iniFile.GetInteger(autoUseItemChildButton[i],"width",0,&width);
		iniFile.GetInteger(autoUseItemChildButton[i],"height",0,&height);
		ChatButton* pButton = new ChatButton;
		pButton->ChatWndCreate(autoAttackChildID,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW|WS_CLIPCHILDREN|BS_NOTIFY,
								  hDlg,"","button",x,y,width,height);

		iniFile.GetString(autoUseItemChildButton[i],"textInfo","",infoText,256);
		autoAttackChildID++;
		if(infoText[0]!=0)
		{
			pButton->ChatWndSetText(infoText);
			iniFile.GetString(autoUseItemChildButton[i],"fontColor","",szValue,MAX_PATH);
	        sscanf(szValue,"%d,%d,%d",&r,&g,&b);
			pButton->ChatWndSetTextNormalColor(RGB(r,g,b));
			char font[FONT_SIZE]={0};
			iniFile.GetString(autoUseItemChildButton[i],"font","",font,FONT_SIZE);
			TFONT tFont;
			strcpy(tFont.fontName,font);
			HDC hdc =CreateCompatibleDC(NULL);
			LOGFONT logFont;
			logFont.lfFaceName[0]=0;
			logFont.lfCharSet = DEFAULT_CHARSET;
			EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
			DeleteDC(hdc);
			if(tFont.isInSystem)
				pButton->ChatWndSetFont(font);
			else
			{
				pButton->ChatWndSetFont(ChatString::ChatStringGetString().chatDefualtFont);
				pButton->ChatWndSetAttr(_CHAT_WND_ATTR_USE_DEFUALT_FONT);
			}
		}
		pButton->ChatWndSetAttr(_CHAT_WND_ATTR_CHECKED_BUTTON);
		autoUseItemGroup.childButtonList.push_back(pButton);
		autoUseItemGroup.childButtonEnabel.push_back(false);
		autoUseItemGroup.lastChildButtonEnabel.push_back(false);
		MutexButtonsIndex mutexIndex;
		autoUseItemGroup.mutexIndexList.push_back(mutexIndex);
		pButton->ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
	}

	MutexButtonsIndex& mutexIndex = autoUseItemGroup.mutexIndexList[_ENTRUST_AUTOUSEITEM_HP_60_BUTTON_INDEX];
	mutexIndex.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_HP_60_BUTTON_INDEX);
	mutexIndex.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_HP_40_BUTTON_INDEX);
	mutexIndex.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_OWNER_BUTTON_INDEX);
	MutexButtonsIndex& mutexIndex1 = autoUseItemGroup.mutexIndexList[_ENTRUST_AUTOUSEITEM_HP_40_BUTTON_INDEX];
	mutexIndex1.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_HP_60_BUTTON_INDEX);
	mutexIndex1.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_HP_40_BUTTON_INDEX);
	mutexIndex1.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_OWNER_BUTTON_INDEX);

	MutexButtonsIndex& mutexIndex2 = autoUseItemGroup.mutexIndexList[_ENTRUST_AUTOUSEITEM_OWNER_BUTTON_INDEX];
	mutexIndex2.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_HP_60_BUTTON_INDEX);
	mutexIndex2.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_HP_40_BUTTON_INDEX);
	mutexIndex2.MutexIndexAddIndex(_ENTRUST_AUTOUSEITEM_OWNER_BUTTON_INDEX);

	ChatButton* button = autoUseItemGroup.childButtonList[_ENTRUST_AUTOUSEITEM_OWNER_BUTTON_INDEX];
	ShowWindow(button->ChatWndGetHandle(),SW_HIDE);
/*	ChatButton* button1 = autoUseItemGroup.childButtonList[_ENTRUST_AUTOUSEITEM_MP_40_BUTTON_INDEX];
	button1->x = button->x;
	button1 ->y = button->y;
	button1->ChatWndMove(button->x,button->y,button1->ChatWndGetRect().right,button1->ChatWndGetRect().bottom,true);*/
}
BOOL CALLBACK EntrustComputerDlg::EntrustDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	switch(msg)
	{
	case WM_INITDIALOG:
		{

		}
		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgProcessPaint(hdc);
			EndPaint(hwnd,&ps);
		}
		return 0;
	case WM_ERASEBKGND:
		{
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgProcessPaint((HDC)wParam);
		}
		return true;
	case WM_DRAWITEM:
		{
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgDrawItem((LPDRAWITEMSTRUCT)lParam);
		}
		return false;
	case WM_MOUSEMOVE:
		{
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgProcessMouseMove(wParam,lParam);
		}
		return false;
	case WM_MOUSELEAVE:
		{
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgProcessMouseLeave(wParam,lParam);
		}
		return false;
	case WM_COMMAND:
		{
			EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgProcessCommand(wParam,lParam);
			if( HIWORD( wParam ) == BN_CLICKED )
			{
				KUiEntrustComputer::GetSingleton().RefreshUI( LOWORD(wParam) );	
			}
		}
		return false;
	}
	return 0;
}
void EntrustComputerDlg::EntrustDlgProcessPaint(HDC hdc)
{
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,dlgWidth,dlgHeight);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc1,hBitmap);
	DrawBitmap(hdc1,ChatResource::GetSingle().GetResource(dlgBkBitmapIdx)->hBitmap,dlgWidth,dlgHeight);
//	autoAttackGroup.CheckedButtonDrawTextBk(hdc1);
//	autoPickUpGroup.CheckedButtonDrawTextBk(hdc1);
//	autoUseItemGroup.CheckedButtonDrawTextBk(hdc1);
	BitBlt(hdc,0,0,dlgWidth,dlgHeight,hdc1,0,0,SRCCOPY);
	SelectObject(hdc1,hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdc1);
}
void EntrustComputerDlg::EntrustDlgProcessPushOkButton(WPARAM wParam,LPARAM lParam)
{
	EntrustDlgProcessAutoAttack();
	EntrustDlgProcessAutoPickUp();
	EntrustDlgProcessAutoUseItem();
}
void EntrustComputerDlg::EntrustDlgProcessAutoAttack()
{
	if(autoAttackGroup.lastMainButtonEnable!=autoAttackGroup.mainButtonEnable)
		g_pCoreShell->OperationRequest( GOI_AUTOATTACK_SWITCH, NULL, NULL );
	if(autoAttackGroup.mainButtonEnable)
	{
		int size = autoAttackGroup.childButtonEnabel.size();
		for(int i = 0;i<size;i++)
		{
			if(autoAttackGroup.childButtonEnabel[i]!=autoAttackGroup.lastChildButtonEnabel[i])
			{
				switch(i)
				{
				case _ENTRUST_AUTOATTACK_ENEMY_LEVEL_HIGHER_BUTTON_INDEX:
					{
						g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS,AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER,0);
					}
					break;
				case _ENTRUST_AUTOATTACK_ENEMY_LEVEL_LOWER_BUTTON_INDEX:
					{
						g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS,AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER,0);
					}
					break;
				case _ENTRUST_AUTOATTACK_GO_HOME_BUTTON_INDEX:
					{
						g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS,AUTO_ATTACK_FLAG_GO_BACK,0);
					}
					break;
				case _ENTRUST_AUTOATTACK_ONLY_NORMAL_SET_BUTTON_INDEX:
					{
						g_pCoreShell->OperationRequest(GOI_AUTOATTACK_ITEM_FLAGS,AUTO_ATTACK_FLAG_SELL_CHEAP,0);
					}
					break;

				}
			}
		}
	}
	autoAttackGroup.CheckedButtonSetLastSelected();
}
void EntrustComputerDlg::EntrustDlgProcessAutoPickUp()
{
	if(autoPickUpGroup.lastMainButtonEnable!=autoPickUpGroup.mainButtonEnable)
	{
		/////发送消息到core开启相应功能
		g_pCoreShell->OperationRequest(GOI_AUTOPICKUP_ITEM_SWITCH,NULL,NULL);
	}
	if(autoPickUpGroup.mainButtonEnable)
	{
		if (autoPickUpGroup.childButtonEnabel[_ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_INDEX]
			|| autoPickUpGroup.childButtonEnabel[_ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_INDEX])
		{
			if (autoPickUpGroup.childButtonList[_ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX]->ChatWndGetState() ==_CHAT_BUTTON_STATE_DISABLE)
			{
				autoPickUpGroup.childButtonList[_ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX]->ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
				autoPickUpGroup.childButtonList[_ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX]->ChatWndUpdate();
			}
		}
		else
		{
			autoPickUpGroup.childButtonList[_ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX]->ChatWndSetState(_CHAT_BUTTON_STATE_DISABLE);
			autoPickUpGroup.childButtonList[_ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX]->ChatWndUpdate();
			autoPickUpGroup.childButtonEnabel[_ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX] = false;
		}
		int size = autoPickUpGroup.childButtonEnabel.size();
		for(int i = 0;i<size;i++)
		{
			if(autoPickUpGroup.childButtonEnabel[i]!=autoPickUpGroup.lastChildButtonEnabel[i])
			{
				//////
				//发送消息到core开启相应功能
				switch(i)
				{
				case _ENTRUST_AUTOPICKUP_DEAR_FIRST_BUTTON_INDEX:
					{
						g_pCoreShell->OperationRequest(GOI_AUTOPICKUP_ITEM_FLAGS,AUTO_PICKUP_ITEM_DEAR_SET_FIRST,0);
					}
					break;
				case _ENTRUST_AUTOPICKUP_OVER_BLUE_BUTTON_INDEX:
					{
						if(autoPickUpGroup.childButtonEnabel[i])	
							g_pCoreShell->OperationRequest(GOI_AUTO_PICKUP_TYPE_FLAGS,AUTO_PICKUP_ITEM_NOT_WHITE,1);
						else
						{
							g_pCoreShell->OperationRequest(GOI_AUTO_PICKUP_TYPE_FLAGS,AUTO_PICKUP_ITEM_NOT_WHITE,0);
						}
					}
					break;
				case _ENTRUST_AUTOPICKUP_OVER_GREEN_BUTTON_INDEX:
					{
						if(autoPickUpGroup.childButtonEnabel[i])
							g_pCoreShell->OperationRequest( GOI_AUTO_PICKUP_TYPE_FLAGS,AUTO_PICKUP_ITEM_ONLY_GREEN,1);
						else
						{
							g_pCoreShell->OperationRequest( GOI_AUTO_PICKUP_TYPE_FLAGS,AUTO_PICKUP_ITEM_ONLY_GREEN,0);
						}
					}
					break;
				case _ENTRUST_AUTOPICKUP_MEDICINE_BUTTON_INDEX:
					{
						if(autoPickUpGroup.childButtonEnabel[i])
						{
							g_pCoreShell->OperationRequest( GOI_AUTO_PICKUP_PICK_MEDICINE, 0, 0);
						}
						else
						{
							g_pCoreShell->OperationRequest( GOI_AUTO_PICKUP_PICK_MEDICINE, 1, 0);
						}
					}
					break;
				}
			}
		}
	}
	autoPickUpGroup.CheckedButtonSetLastSelected();
}
void EntrustComputerDlg::EntrustDlgProcessAutoUseItem()
{
	if(autoUseItemGroup.lastMainButtonEnable!=autoUseItemGroup.mainButtonEnable)
	{
		/////发送消息到core开启相应功能
		g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_SWITCH,0,autoUseItemGroup.mainButtonEnable);
	}
	if(autoUseItemGroup.mainButtonEnable)
	{
		int size = autoUseItemGroup.childButtonEnabel.size();
		for(int i = 0;i<size;i++)
		{
			if(autoUseItemGroup.childButtonEnabel[i]!=autoUseItemGroup.lastChildButtonEnabel[i])
			{
				//////
				//发送消息到core开启相应功能
				switch(i)
				{
				case _ENTRUST_AUTOUSEITEM_HP_60_BUTTON_INDEX:
					g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS,AUTO_USE_HP_MEDICINE_LIFE_1,autoUseItemGroup.childButtonEnabel[i]);
					break;
				case _ENTRUST_AUTOUSEITEM_HP_40_BUTTON_INDEX:
					g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS,AUTO_USE_HP_MEDICINE_LIFE_2,autoUseItemGroup.childButtonEnabel[i]);
					break;
				case _ENTRUST_AUTOUSEITEM_MP_40_BUTTON_INDEX:
					g_pCoreShell->OperationRequest(GOI_AUTOUSE_ITEM_FLAGS,AUTO_USE_MP_MEDICINE_1,autoUseItemGroup.childButtonEnabel[i]);
					break;
				}
			}
		}
	}
	autoUseItemGroup.CheckedButtonSetLastSelected();
}
void EntrustComputerDlg::EntrustDlgProcessCommand(WPARAM wParam,LPARAM lParam)
{
	if(autoAttackGroup.CheckedButtonProcessLButtonDown(wParam,lParam))
	{
		EntrustDlgProcessAutoAttack();
		return;
	}
	if(autoPickUpGroup.CheckedButtonProcessLButtonDown(wParam,lParam))
	{
		EntrustDlgProcessAutoPickUp();
		return;
	}
	if(autoUseItemGroup.CheckedButtonProcessLButtonDown(wParam,lParam))
	{
		EntrustDlgProcessAutoUseItem();
		return;
	}
/*	int id = LOWORD(wParam);
	if(HIWORD(wParam)==BN_CLICKED)
	{
		if(id == okButton.ChatWndGetID())
		{
			EntrustDlgProcessPushOkButton(wParam,lParam);
		}
	}*/
}
void EntrustComputerDlg::EntrustDlgProcessMouseMove(WPARAM wParam,LPARAM lParam)
{
	TRACKMOUSEEVENT tme;
	tme.cbSize=sizeof(TRACKMOUSEEVENT);
	tme.dwFlags=TME_HOVER|TME_LEAVE;
	tme.dwHoverTime=1000;
	tme.hwndTrack=hDlg;
	_TrackMouseEvent(&tme);

	if(okButton.ChatWndGetState() == _CHAT_BUTTON_STATE_MOUSEOVER)
	{
		okButton.ChatWndSetState(_CHAT_BUTTON_STATE_NORMAL);
		okButton.ChatWndUpdate();
		return;
	}
}
void EntrustComputerDlg::EntrustDlgProcessMouseLeave(WPARAM wParam,LPARAM lParam)
{
	POINT mouse_pt;
	POINT pt;
	GetCursorPos(&mouse_pt);
	RECT btn_rect;
	pt.x = mouse_pt.x;
	pt.y = mouse_pt.y;
	GetClientRect(okButton.ChatWndGetHandle(),&btn_rect);
	ScreenToClient(okButton.ChatWndGetHandle(),&pt);
	if(IsInRect(pt,btn_rect))
	{
		if(okButton.ChatWndGetState() !=_CHAT_BUTTON_STATE_MOUSEDOWN)
		{
			okButton.ChatWndSetState(_CHAT_BUTTON_STATE_MOUSEOVER);
			okButton.ChatWndUpdate();
			return;
		}
	}


}
void EntrustComputerDlg::EntrustDlgDrawItem(LPDRAWITEMSTRUCT lpdis)
{
	switch(lpdis->CtlID)
	{
	case _ENTRUST_TITLE_BUTTON_ID:
		titleButton.ChatWndDrawItem(lpdis->hDC);
		return;
	case _ENTRUST_OK_BUTTON_ID:
		okButton.ChatWndDrawItem(lpdis->hDC);
		return;
	}
	if(autoAttackGroup.CheckedButtonDrawItem(lpdis))
		return;
	if(autoPickUpGroup.CheckedButtonDrawItem(lpdis))
		return;
	if(autoUseItemGroup.CheckedButtonDrawItem(lpdis))
		return;
}




