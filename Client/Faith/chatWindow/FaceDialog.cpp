#include <windows.h>
#include <commctrl.h>
#include <vector>
using    namespace std;
#include "layoutinterface.h"
#include "chatWindow/faceDialog.h"
#include "Ui/UiCase/UiChatWindow.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "loadSrcWnd/GDILoadBitmap.h"
#include "chatWindow/ChatResource.h"

FaceDialog::FaceDialog()
{
	hRgn = 0;
	bkSrcIdx = 0;
	pFaceLayOut = 0;
}
FaceDialog::~FaceDialog()
{
	if(hRgn)
		DeleteObject(hRgn);
	if(pFaceLayOut)
	{
		pFaceLayOut->clearLayout();
		pFaceLayOut->Release();
		delete pFaceLayOut;
	}
}
void FaceDialog::FaceDialogClickFace()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hShowFace,&pt);
	LOElemInfo elem;
	HDC hdc = GetWindowDC(hShowFace);
	HDC tempDC = ((GDIRender*)B2ChatDialog::pFaceRender)->hCurrentDC ;
	((GDIRender*)B2ChatDialog::pFaceRender)->hCurrentDC = hdc;
	pt.x -= startFaceX;
	pt.y -= startFaceY;
	if(pFaceLayOut->pickupElem(pt.x,pt.y,elem)==FALSE)
	{
		ReleaseDC(hShowFace,hdc);
		return;
	}

	const KUiCfgLoader::FacePanelCfgData& faceCfg = KUiCfgLoader::getSingleton().getFaceData();
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->chatEditInsertElem(elem);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatWndUpdate();
	((GDIRender*)B2ChatDialog::pFaceRender)->hCurrentDC = tempDC;
	ReleaseDC(hShowFace,hdc);
	FaceDialogShow(FALSE);

}
void FaceDialog::FaceDialogShow(BOOL bShow)
{
	isShow = bShow;
	if(bShow)
	{
		if(!IsWindowVisible(hFaceDlg))
			ShowWindow(hFaceDlg,SW_SHOW);
	}
	else
	{
		if(IsWindowVisible(hFaceDlg))
			ShowWindow(hFaceDlg,SW_HIDE);
	}
}
BOOL CALLBACK FaceDialog::FaceDialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_INITDIALOG:
//		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogAdjustWindow(hwnd);
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogInit(hwnd);
		SetClassLong(hwnd,GCL_HCURSOR,(LONG)ChatMainDlg::hCursor);
		return 0;
	case WM_COMMAND:
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogClickFace();
		return 0;
	case WM_DRAWITEM:
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogOwnerDraw((LPDRAWITEMSTRUCT)lParam);
		return 0;
	case WM_PAINT:
		return 0;
	case WM_ERASEBKGND:
		{
		}
		return TRUE;
	case WM_DESTROY:
		{
			if(B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->hRgn)
				DeleteObject(B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->hRgn);
		}
		return false;
	}
	return FALSE;
}

void FaceDialog::FaceDialogAdjustWindow(HWND hwnd)
{
		
	RECT rc;
	HWND hWnd = GetDesktopWindow();
	RECT deskRect;
	GetWindowRect(hWnd,&deskRect);
	::GetWindowRect(B2ChatDialog::chatManager.ChatManagerGetShowButton()->ChatWndGetHandle(),&rc);
	int x = 0;
	int y = rc.top - wndRect.bottom -offsetY;
	if(rc.right+wndRect.right>deskRect.right)
		x = deskRect.right-wndRect.right;
	else
		x = rc.right;

	MoveWindow(hwnd,x,y,wndRect.right,wndRect.bottom,TRUE);

}

void FaceDialog::FaceDialogInit(HWND hDlg)
{
#define FACE_DLG_NAME    "faceDialog"
	KIniFile iniFile;
	CHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH,szPath);
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	int startX,startY,numbersPerLine;
	int sWidth,sHeight;
	int width,height;
	int isClip;
	int r,g,b;
	iniFile.Load(szPath);

	iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_X,0,&offsetX);
	iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_Y,0,&offsetY);
	iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_WIDTH,0,&width);
	iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_HEIGHT,0,&height);


	iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_CLIP,0,&isClip);
	if(isClip==1)
	{
		iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_CLIP_COLOR_R,0,&r);
		iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_CLIP_COLOR_G,0,&g);
		iniFile.GetInteger(FACE_DLG_NAME,_CHAT_SRC_CLIP_COLOR_B,0,&b);

	}
	iniFile.GetInteger(FACE_DLG_NAME,"bkBitmapIdx",0,&bkSrcIdx);

	if(isClip)
	{
//		ChatWnd::BitmapToRgn(hBkSrc,hRgn,RGB(r,g,b));
//		SetWindowRgn(hDlg,hRgn,TRUE);
	}

	iniFile.GetInteger(FACE_DLG_NAME,"startFaceX",0,&startY);
	iniFile.GetInteger(FACE_DLG_NAME,"startFaceY",0,&startX);


	iniFile.GetInteger(FACE_DLG_NAME,"numbersPerLine",0,&numberFacePerLine);
	numbersPerLine = numberFacePerLine;
	iniFile.GetInteger(FACE_DLG_NAME,"sWidth",0,&sWidth);
	nWidth = sWidth;
	iniFile.GetInteger(FACE_DLG_NAME,"sHeight",0,&sHeight);
	nHeight = sHeight;

	startFaceX = startX;
	startFaceY = startY;
	const KUiCfgLoader::FacePanelCfgData& faceCfg = KUiCfgLoader::getSingleton().getFaceData();
	wndRect.left = wndRect.top = 0;
	wndRect.right = width;
	wndRect.bottom = height;

	hShowFace = CreateWindowEx(0,"static","",WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|SS_OWNERDRAW|SS_NOTIFY ,0,0,width,height,hDlg,HMENU(100),KWin32App::m_hInstance,0);

	/////////////////////
	CreateLayout(&pFaceLayOut,B2ChatDialog::pFaceRender);
	
	char facePanelLayout[LAYOUT_TEXT_MAX_LEN] = {0};
	
	char tempText[COMMON_CLIENT_MSG_LEN_128];
	sprintf(facePanelLayout, "<Layout width=%d>", faceCfg.wndWidth);
	strcat(facePanelLayout , "<Seg text-align=left float=none>");
	for(int j = 0; j < faceCfg.faceList.size(); ++j)
	{
		sprintf(tempText, "<Obj type=pic gotype=face goid=%d>%s</Obj>",
			faceCfg.faceList[j].index,
			faceCfg.faceList[j].image);
		strcat(facePanelLayout, tempText);
	}
	strcat(facePanelLayout , "</Seg>");
	strcat(facePanelLayout , "</Layout>");
	
	pFaceLayOut->SetText(facePanelLayout);
	pFaceLayOut->flashLayout();
//	FaceDialogAdjustWindow(hDlg);
	return;
}

void FaceDialog::FaceDialogOwnerDraw(LPDRAWITEMSTRUCT lpdis)
{
	if(lpdis->hwndItem == hShowFace)
	{
		HDC hdc = CreateCompatibleDC(lpdis->hDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(lpdis->hDC,wndRect.right,wndRect.bottom);
		HBITMAP hOld = (HBITMAP)SelectObject(hdc,hBitmap);
		DrawBitmap(hdc,ChatResource::GetSingle().GetResource(bkSrcIdx)->hBitmap,wndRect.right,wndRect.bottom,0,0);
		HDC hOldDc = ((GDIRender*)B2ChatDialog::pFaceRender)->hCurrentDC;
		((GDIRender*)B2ChatDialog::pFaceRender)->hCurrentDC = hdc;
		pFaceLayOut->Render(startFaceX,startFaceY,0);
		BitBlt(lpdis->hDC,0,0,wndRect.right,wndRect.bottom,hdc,0,0,SRCCOPY);
		((GDIRender*)B2ChatDialog::pFaceRender)->hCurrentDC = hOldDc;	
		SelectObject(hdc,hOld);
		DeleteObject(hBitmap);
		DeleteDC(hdc);


	}
}