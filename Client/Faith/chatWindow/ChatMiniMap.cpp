#include "KWin32App.h"
#include <windowsx.h>
#include <commctrl.h>
#include <vector>
using std::vector;
#include "layoutinterface.h"


#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatMiniMap.h"
#include "Login/Login.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "chatWindow/ChatResource.h"
#include "KIniFile.h"
#include "ui/UiCase/UiPathHelp.h"
#include "ui/UiCase/UiMapCentre.h"
#include "Ui/UiCase/UiNpcNavigation.h"
#include "ui/UiCase/UiTeamViewer.h"

#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"

extern iCoreShell*		g_pCoreShell;


#include "resource.h"
 bool ChatMiniMap::isShowPlayer = false;
ChatMiniMap::ChatMiniMap()
{
	hDlg = 0;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
//	lpddsurface = 0;

	m_nFontHeight		= 12;
	m_nFontWeight		= 400;
	m_nFontPointY		= 2;
	m_dwFontColor		= ( 0xffffffff );
}
ChatMiniMap::~ChatMiniMap()
{
}
void ChatMiniMap::Create(HWND hParent,DlgProcessFun pFun,int width,int height)
{
	hDlg = CreateDialog(KWin32App::m_hInstance,MAKEINTRESOURCE(IDD_MINI_MAP),hParent,pFun);
	if(hDlg == 0)
		return;
	LoadResourceIni();
	cx = 0;
	cy = 0;
	AdjustWindow();
//	Show();
}
void ChatMiniMap::ProcessLButtonDown(POINT pt,int flags)
{
	if(pt.x < extraX||pt.x > extraX+paintWidh||
		pt.y < extarY||pt.y > extarY + paintHeight)
		return;
		
	Position destPos;
	destPos.x = pt.x - extraX;
	destPos.y = pt.y - extarY;

	MapPosInfo mapInfo;
	g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_MINI_POS, (unsigned)&mapInfo, (int)&destPos);
	if(flags==CONTROL_PUSHED)
	{
		InsertPositionInEditBox(mapInfo);
	}
	else
	{
		g_pCoreShell->OperationRequest( GOI_GOTO_POS, (unsigned)mapInfo.pos.x, (int)mapInfo.pos.y * 2);
	}
}
void ChatMiniMap::InsertPositionInEditBox(MapPosInfo& mapInfo)
{
	LOElemInfo itemElem;
	wchar_t* itemContent = NULL;
	char linkName[COMMON_CLIENT_MSG_LEN_128];

	sprintf(linkName, "%s(%d,%d)", mapInfo.mapName, mapInfo.pos.x, mapInfo.pos.y);
	ansiToUnicode(linkName, itemContent);
	wchar_t itemDescription[COMMON_CLIENT_MSG_LEN_64];
	itemDescription[0] = 0;
	wcscat(itemDescription, L"[");
	wcscat(itemDescription, itemContent);
	wcscat(itemDescription, L"]");
	
	itemElem.elemType = LO_TEXT;
	itemElem.isShowDes = true;
	itemElem.gameObj._objType = LO_GO_POSITION;
	itemElem.gameObj._objId[0] = mapInfo.mapId;
	itemElem.gameObj._objId[1] = mapInfo.pos.x;
	itemElem.gameObj._objId[2] = mapInfo.pos.y;
	itemElem.content = itemContent;
	itemElem.description = itemDescription;

	B2ChatDialog::chatManager.ChatManagerGetEditBox()->chatEditInsertElem(itemElem);
	B2ChatDialog::chatManager.ChatManagerGetEditBox()->ChatEditWndSetFocus(TRUE);
	
	
	delete[] itemContent;
	itemContent = NULL;
}
void ChatMiniMap::ProcessPaint(HWND hwnd,HDC hdc)
{
	if(g_LoginLogic.GetStatus() == LL_S_IN_GAME )
	{
		HDC hdcbuffer = CreateCompatibleDC(hdc);
		HDC hbkDc = CreateCompatibleDC(hdc);
		HBITMAP hbkBitmap1 = CreateCompatibleBitmap(hdc,width,height);
		HBITMAP hTeamp1 = (HBITMAP)SelectObject(hbkDc,hbkBitmap1);
		DrawBitmap(hbkDc,ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,width,height);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc,paintWidh,paintHeight);
		HBITMAP hTemp = (HBITMAP)SelectObject(hdcbuffer,hBitmap);
		drawPoint.x = 0;
		drawPoint.y = 0;
		drawPoint.hdc = (unsigned long)hdcbuffer;
		g_pCoreShell->OperationRequest(GOI_MINI_MAP_DRAW_ON_DC,(unsigned long)&drawPoint,0);

		

		BitBlt(hbkDc,extraX,extarY,paintWidh,paintHeight,hdcbuffer,0,0,SRCCOPY);

		/*绘制当前地图的名字*/
		HFONT hFont = CreateFont(
			m_nFontHeight,
			0,
			0,
			0,
			m_nFontWeight,
			FALSE,
			FALSE,
			FALSE,
			ANSI_CHARSET,
			OUT_TT_ONLY_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			FF_ROMAN,
			ChatString::ChatStringGetString( ).chatDefualtFont );
		HFONT hOldFont = SelectFont( hbkDc, hFont );
		SetBkMode( hbkDc, TRANSPARENT );
		SetTextCharacterExtra( hbkDc, m_nFontExtra );
		DWORD dwOldColor = SetTextColor( hbkDc, m_dwFontColor );
		KUiSceneTimeInfo tagSceneMapTime = { 0 };
		g_pCoreShell->SceneMapOperation( GSMOI_SCENE_TIME_INFO, ( unsigned int )&tagSceneMapTime, NULL );
		tagSceneMapTime.szSceneName[ COMMON_CLIENT_MSG_LEN_32-1 ] = 0;
		SIZE strSize = { 0 };
		int length = strlen( tagSceneMapTime.szSceneName );
		GetTextExtentPoint32( hbkDc, tagSceneMapTime.szSceneName, length, &strSize );
		int nFontX = ( width - strSize.cx ) / 2;
		int nFontY = m_nFontPointY;

		
		TextOut( hbkDc, 
			nFontX, 
			nFontY, 
			tagSceneMapTime.szSceneName, 
			length );

		SetTextColor( hbkDc, dwOldColor );
		SetBkMode( hbkDc, OPAQUE );

		BitBlt(hdc,0,0,width,height,hbkDc,0,0,SRCCOPY);
		SelectFont( hbkDc, hOldFont );
		DeleteFont( hFont );
		SelectObject(hdcbuffer,hTemp);
		DeleteObject(hBitmap);
		DeleteDC(hdcbuffer);
		SelectObject(hbkDc,hbkBitmap1);
		DeleteObject(hbkBitmap1);
		DeleteDC(hbkDc);
	}
}
void ChatMiniMap::Show()
{
//	SetWindowPos(hDlg,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE);
	ShowWindow(hDlg,SW_NORMAL);
}
void ChatMiniMap::Hide()
{
	ShowWindow(hDlg,SW_HIDE);
}
void ChatMiniMap::AdjustWindow()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
	AdjustWindowRectEx(&rClient,GetWindowLong(ChatMainDlg::hMainDlg,GWL_STYLE),GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));
	int mx = rc.left-rClient.left + x;
	int my = rc.top -rClient.top+ y;
	MoveWindow(hDlg,mx,my,width,height,TRUE);
}

ChatMiniMap& ChatMiniMap::GetSingle()
{
	static ChatMiniMap miniMap;
	return miniMap;
}
void ChatMiniMap::Update()
{
	isNeedUpdata = true;
}
void ChatMiniMap::ProcessPlayerShpwBtnDown()
{
	g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_BODY, 0, isShowPlayer);
	isShowPlayer= !isShowPlayer;
	KUiMiniMap::showPlayer = isShowPlayer;
}
void ChatMiniMap::ProcessPathFindBtnDown()
{
	if(KUiPathHelp::GetSingleton().IsVisible())
		KUiPathHelp::Hide();
	else
		KUiPathHelp::Show();
}
void ChatMiniMap::ProcessKeyJinglinBtnDown()
{
	if ( KUiNpcNavigation::getSingleton().isVisible() )
	{
		KUiNpcNavigation::getSingleton().hide();
	}
	else
	{
		KUiNpcNavigation::getSingleton().toggle();
	}
}
void ChatMiniMap::ProcessCurrentMapBtnDown()
{
	KUiSceneMap::getSinglton().toggle();
	if(g_pCoreShell)
	{
		Size absSize = KUiMiniMap::GetSingleton().GetParent()->getChild("TaharezLook/MiniMap/MiniMapWnd")->getAbsoluteSize();
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
				SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
				( (int)(absSize.d_width)| ((int)(absSize.d_height) << 16)));
	}
}
void ChatMiniMap::ProcessBigWodMapBtnDown()
{
	if ( KUiBigMap::getSinglton().isVisible() )
	{
		KUiBigMap::getSinglton().hide();
	}
	else
	{
		KUiBigMap::getSinglton().show();
	}
}

void ChatMiniMap::ProcessFindTeamBtnDown( )
{
	KUiTeamViewer::ToggleVisibility( );
}
void ChatMiniMap::LoadResourceIni()
{
	KIniFile iniFile;
	char szPath[MAX_PATH] = {0};
	char szValue[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		iniFile.Load(_CHAT_CFG_FILE_1024);
	else
		iniFile.Load(_CHAT_CFG_FILE);
	iniFile.GetInteger("ChatMiniMapDlg","x",0,&x);
	iniFile.GetInteger("ChatMiniMapDlg","y",0,&y);
	iniFile.GetInteger("ChatMiniMapDlg","bkSrc",0,&bkBitmapIdx);
	iniFile.GetInteger("ChatMiniMapDlg","extraX",0,&extraX);
	iniFile.GetInteger("ChatMiniMapDlg","extraY",0,&extarY);
	iniFile.GetInteger("ChatMiniMapDlg","width",0,&width);
	iniFile.GetInteger("ChatMiniMapDlg","height",0,&height);
	iniFile.GetInteger("ChatMiniMapDlg","paintWidth",0,&paintWidh);
	iniFile.GetInteger("ChatMiniMapDlg","paintHeight",0,&paintHeight);

	int numberIdx = 0;
	iniFile.GetInteger("miniMapIconIdx","numbers",0,&numberIdx);
	for(int index = 0; index < numberIdx;index++)
	{
		char buffer[256] = {0};
		sprintf(buffer,"%s%d","idx",index);
		int idx = 0;
		iniFile.GetInteger("miniMapIconIdx",buffer,0,&idx);
		drawPoint.bitmapHandle[index] = (unsigned long)ChatResource::GetSingle().GetResource(idx)->hBitmap;
	}


	///按钮/////////////////
	
	int bx = 0,by = 0,bWidth = 0,bHeight = 0;
	int r = 0,g = 0,b= 0;
	int normalIdx = 0,hoveIdx = 0,disableIdx = 0;
	iniFile.GetInteger("findPath","x",0,&bx);
	iniFile.GetInteger("findPath","y",0,&by);
	iniFile.GetInteger("findPath","width",0,&bWidth);
	iniFile.GetInteger("findPath","height",0,&bHeight);
	pathFind.ChatWndCreate(MINIMAP_BUTTON_PATH_FIND_BTN_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW|WS_CLIPCHILDREN,hDlg,"","button",bx,by,bWidth,bHeight);
	iniFile.GetInteger("findPath","normalStateSrcID",0,&normalIdx);
	iniFile.GetInteger("findPath","hoverSTateSrcID",0,&hoveIdx);
	iniFile.GetInteger("findPath","disableStateSrcID",0,&disableIdx);
	pathFind.ChatWndSetResource(normalIdx,hoveIdx,-1,disableIdx);
	iniFile.GetString("findPath","tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0]!=0)
		pathFind.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	pathFind.SetWndProcessFun(ChatWndProcessFun::ProcessMiniMapPathFindBtn);



	iniFile.GetInteger("keyJinglin","x",0,&bx);
	iniFile.GetInteger("keyJinglin","y",0,&by);
	iniFile.GetInteger("keyJinglin","width",0,&bWidth);
	iniFile.GetInteger("keyJinglin","height",0,&bHeight);
	keyJinglin.ChatWndCreate(MINIMAP_BUTTON_KEY_JINGLIN_BTN_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW|WS_CLIPCHILDREN,hDlg,"","button",bx,by,bWidth,bHeight);
	iniFile.GetInteger("keyJinglin","normalStateSrcID",0,&normalIdx);
	iniFile.GetInteger("keyJinglin","hoverSTateSrcID",0,&hoveIdx);
	iniFile.GetInteger("keyJinglin","disableStateSrcID",0,&disableIdx);
	keyJinglin.ChatWndSetResource(normalIdx,hoveIdx,-1,disableIdx);
	iniFile.GetString("keyJinglin","tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0]!=0)
		keyJinglin.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	keyJinglin.SetWndProcessFun(ChatWndProcessFun::ProcessMiniMapKeyJinglinBtn);

	iniFile.GetInteger("currentMap","x",0,&bx);
	iniFile.GetInteger("currentMap","y",0,&by);
	iniFile.GetInteger("currentMap","width",0,&bWidth);
	iniFile.GetInteger("currentMap","height",0,&bHeight);
	currentMap.ChatWndCreate(MINIMAP_BUTTON_CURRENT_MAP_BTN_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW|WS_CLIPCHILDREN,hDlg,"","button",bx,by,bWidth,bHeight);
	iniFile.GetInteger("currentMap","normalStateSrcID",0,&normalIdx);
	iniFile.GetInteger("currentMap","hoverSTateSrcID",0,&hoveIdx);
	iniFile.GetInteger("currentMap","disableStateSrcID",0,&disableIdx);
	currentMap.ChatWndSetResource(normalIdx,hoveIdx,-1,disableIdx);
	iniFile.GetString("currentMap","tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0]!=0)
		currentMap.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	currentMap.SetWndProcessFun(ChatWndProcessFun::ProcessMiniMapCurrentMapBtn);

	iniFile.GetInteger("bigWordMap","x",0,&bx);
	iniFile.GetInteger("bigWordMap","y",0,&by);
	iniFile.GetInteger("bigWordMap","width",0,&bWidth);
	iniFile.GetInteger("bigWordMap","height",0,&bHeight);
	bigWordMap.ChatWndCreate(MINIMAP_BUTTON_BIGWORD_MAP_BTN_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW|WS_CLIPCHILDREN,hDlg,"","button",bx,by,bWidth,bHeight);
	iniFile.GetInteger("bigWordMap","normalStateSrcID",0,&normalIdx);
	iniFile.GetInteger("bigWordMap","hoverSTateSrcID",0,&hoveIdx);
	iniFile.GetInteger("bigWordMap","disableStateSrcID",0,&disableIdx);
	bigWordMap.ChatWndSetResource(normalIdx,hoveIdx,-1,disableIdx);
	iniFile.GetString("bigWordMap","tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0]!=0)
		bigWordMap.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	bigWordMap.SetWndProcessFun(ChatWndProcessFun::ProcessMiniMapBigWordMapBtn);

	iniFile.GetInteger("showPlayer","x",0,&bx);
	iniFile.GetInteger("showPlayer","y",0,&by);
	iniFile.GetInteger("showPlayer","width",0,&bWidth);
	iniFile.GetInteger("showPlayer","height",0,&bHeight);
	showPlayer.ChatWndCreate(MINIMAP_BUTTON_SHOW_PLAYER_BTN_ID,WS_VISIBLE|WS_CHILD|BS_OWNERDRAW|WS_CLIPCHILDREN,hDlg,"","button",bx,by,bWidth,bHeight);
	iniFile.GetInteger("showPlayer","normalStateSrcID",0,&normalIdx);
	iniFile.GetInteger("showPlayer","hoverSTateSrcID",0,&hoveIdx);
	iniFile.GetInteger("showPlayer","disableStateSrcID",0,&disableIdx);
	showPlayer.ChatWndSetResource(normalIdx,hoveIdx,-1,disableIdx);
	iniFile.GetString("showPlayer","tooltipInfo","",szValue,MAX_PATH);
	if(szValue[0]!=0)
		showPlayer.ChatWndTipCreate(TTS_NOPREFIX,szValue,100);
	showPlayer.SetWndProcessFun(ChatWndProcessFun::ProcessMiniMapShowPlayerBtn);

	iniFile.GetInteger( "findTeam", "x", 0, &bx );
	iniFile.GetInteger( "findTeam", "y", 0, &by );
	iniFile.GetInteger( "findTeam", "width", 0, &bWidth );
	iniFile.GetInteger( "findTeam", "height", 0, &bHeight );
	findTeam.ChatWndCreate( 
		MINIMAP_BUTTON_FIND_TEAM_BTM_ID, 
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW | WS_CLIPCHILDREN,
		hDlg,
		"",
		"button",
		bx,
		by,
		bWidth,
		bHeight );
	iniFile.GetInteger( "findTeam", "normalStateSrcID", 0, &normalIdx );
	iniFile.GetInteger( "findTeam", "hoverSTateSrcID", 0, &hoveIdx );
	iniFile.GetInteger( "findTeam", "disableStateSrcID", 0, &disableIdx );
	findTeam.ChatWndSetResource( normalIdx,hoveIdx, -1, disableIdx );
	iniFile.GetString( "findTeam", "tooltipInfo", "", szValue, MAX_PATH );
	if( szValue[0] != 0 )
		findTeam.ChatWndTipCreate( TTS_NOPREFIX, szValue, 100 );
	findTeam.SetWndProcessFun( ChatWndProcessFun::ProcessMiniMapFindTeamBtn );

	/*
	 *  字体读取
	 */

	iniFile.GetInteger( "ChatMiniMapDlg" ,"fontHeight", 12, &m_nFontHeight );

	iniFile.GetInteger( "ChatMiniMapDlg", "fontWeight", 400, &m_nFontWeight );

	iniFile.GetInteger( "ChatMiniMapDlg", "fontPointY", 2,&m_nFontPointY );

	iniFile.GetInteger( "ChatMiniMapDlg", "fontExtra", 2, &m_nFontExtra );

	char fontColor[ 128 ] = { 0 };

	iniFile.GetString( "ChatMiniMapDlg", "fontColor", "255,255,255",fontColor, 128 );
	int red = 0,
		blue = 0,
		green = 0;
	sscanf( fontColor, "%d,%d,%d", &red, &green, &blue );
	m_dwFontColor = RGB( red, blue, green );





	
}
void ChatMiniMap::CreateSurface()
{
/*	if(lpddsurface)
		lpddsurface->Release();
	lpddsurface = g_pDirectDraw->CreateSurface(paintWidh,paintHeight);*/

}
void ChatMiniMap::ReleaseSurface()
{
/*	if(lpddsurface)
		lpddsurface->Release();
	lpddsurface = 0;*/
}
void ChatMiniMap::BltToDc(HDC hdc)
{
/*	if(hBitmap)
	{
		DeleteObject(hBitmap);
		hBitmap = 0;
	}
	if(lpddsurface)
	{
		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		lpddsurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
		hBitmap = CreateBitmap(paintWidh,paintHeight,1,16,ddsd.lpSurface);
		lpddsurface->Unlock(NULL);
		HDC hdc1 = CreateCompatibleDC(hdc);
		HBITMAP hTemp = (HBITMAP)SelectObject(hdc1,hBitmap);
		BitBlt(hdc,0,0,paintWidh,paintHeight,hdc1,0,0,SRCCOPY);
		SelectObject(hdc1,hTemp);
		DeleteObject(hBitmap);
		hBitmap = 0;
		DeleteDC(hdc1);

	}*/
}
