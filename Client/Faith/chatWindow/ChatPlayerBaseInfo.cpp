#include "KWin32App.h"
#include <windowsx.h>
#include <commctrl.h>
#include <vector>
using std::vector;
#include "layoutinterface.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPlayerBaseInfoDlg.h"

#include "chatWindow/ChatWndProc.h"
#include "chatWindow/ChatResource.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "KIniFile.h"

#include "resource.h"


ChatPlayerBaseInfoDlg::ChatPlayerBaseInfoDlg()
{
	hDlg = 0;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	memset(&baseInfo,0,sizeof(baseInfo));
	memset(&runTimeInfo,0,sizeof(runTimeInfo));
	memset(&playerAttr,0,sizeof(playerAttr));
	memset(&shehuiInfo,0,sizeof(shehuiInfo));

}

ChatPlayerBaseInfoDlg::~ChatPlayerBaseInfoDlg()
{
	KillTimer(hDlg,CHATPLAYERSHOW_DLG_TIMER_ID);

}
void ChatPlayerBaseInfoDlg::ProcessPaint(HWND hwnd,HDC hdc1)
{
	HDC hdc = CreateCompatibleDC(hdc1);
	HBITMAP hBitmap1 = CreateCompatibleBitmap(hdc1,width,height);
	HBITMAP hTemp = (HBITMAP)SelectObject(hdc,hBitmap1);
	DrawBitmap(hdc,ChatResource::GetSingle().GetResource(bkBitmapIdx)->hBitmap,width,height);

	char buffer[1024+1] = {0};
	char bufferZhiye[512+1] = {0};
	if(baseInfo.nSkillType < 0)
	{
		switch(playerAttr.nSeries)
		{
		case 0:
			strcpy(bufferZhiye,ROLE_CAREER_JS);
			break;
		case 1:
			strcpy(bufferZhiye,ROLE_CAREER_DS);
			break;
		case 2:
			strcpy(bufferZhiye,ROLE_CAREER_YR);
			break;
		default:
			strcpy(bufferZhiye,ROLE_CAREER_JS);
			break;
		}
		
	}
	else
	{
		switch(playerAttr.nSeries)
		{
		case 0:
			if(baseInfo.nSkillType)
			{
				strcpy(bufferZhiye,ROLE_CAREER_JS_0);
			}
			else
			{
				strcpy(bufferZhiye,ROLE_CAREER_JS_1);
			}
			break;
		case 1:
			if(baseInfo.nSkillType)
			{
				strcpy(bufferZhiye,ROLE_CAREER_DS_0);
			}
			else
			{
				strcpy(bufferZhiye,ROLE_CAREER_DS_1);
			}
			break;
		case 2:
			if(baseInfo.nSkillType)
			{
				strcpy(bufferZhiye,ROLE_CAREER_YR_1);
			}
			else
			{
				strcpy(bufferZhiye,ROLE_CAREER_YR_0);
			}
			break;
		default:
			if(baseInfo.nSkillType)
			{
				strcpy(bufferZhiye,ROLE_CAREER_JS_0);
			}
			else
			{
				strcpy(bufferZhiye,ROLE_CAREER_JS_1);
			}
			break;
			
		}
	}
	
	int fontHeight = ChatString::ChatStringGetString().chatDefualtFontHeight;	


	HFONT hFont = CreateFont(12,0,0,0,
		FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,FF_ROMAN,
		ChatString::ChatStringGetString().chatDefualtFont);
	HFONT hTempFont = (HFONT)SelectObject(hdc,hFont);
	SetTextColor(hdc,name_a.color);
	SetBkMode(hdc,TRANSPARENT);
	int eX = 0,eY = 0;
	//姓名
	buffer[0] = 0;
	SetTextColor(hdc,name_a.color);
	TextOut(hdc,name_a.x,name_a.y,name_a.info,strlen(name_a.info));
	SIZE  size;
	GetTextExtentPoint32(hdc,name_a.info,strlen(name_a.info),&size);
	eX = name_a.x + size.cx;
	eY = name_a.y;
	TextOut(hdc,eX+2,eY,baseInfo.Name,strlen(baseInfo.Name));

	
	
	//职业
	buffer[0] = 0;
	SetTextColor(hdc,zhiye_a.color);
	TextOut(hdc,zhiye_a.x,zhiye_a.y,zhiye_a.info,strlen(zhiye_a.info));

	GetTextExtentPoint32(hdc,zhiye_a.info,strlen(zhiye_a.info),&size);
	eX = zhiye_a.x + size.cx;
	eY = zhiye_a.y;
	TextOut(hdc,eX+2,eY,bufferZhiye,strlen(bufferZhiye));
	//等级
	buffer[0] = 0;
	SetTextColor(hdc,dengji_a.color);
	TextOut(hdc,dengji_a.x,dengji_a.y,dengji_a.info,sizeof(dengji_a.info));
	GetTextExtentPoint32(hdc,dengji_a.info,strlen(dengji_a.info),&size);
	eX = dengji_a.x + size.cx;
	eY = dengji_a.y;
	sprintf(buffer,"%d",playerAttr.nLevel);
	TextOut(hdc,eX+2,eY,buffer,strlen(buffer));
	//
	SetTextColor(hdc,sizhu_a.color);
	buffer[0] = 0;
	TextOut(hdc,sizhu_a.x,sizhu_a.y,sizhu_a.info,strlen(sizhu_a.info));
	GetTextExtentPoint32(hdc,sizhu_a.info,strlen(sizhu_a.info),&size);
	eX = sizhu_a.x + size.cx;
	eY = sizhu_a.y;
	if(shehuiInfo.szShizu !=0)
		TextOut(hdc,eX+2,eY,shehuiInfo.szShizu,strlen(shehuiInfo.szShizu));

	buffer[0] = 0;
	SetTextColor(hdc,zhuhou_a.color);
	TextOut(hdc,zhuhou_a.x,zhuhou_a.y,zhuhou_a.info,strlen(zhuhou_a.info));
	GetTextExtentPoint32(hdc,zhuhou_a.info,strlen(zhuhou_a.info),&size);
	eX = zhuhou_a.x + size.cx;
	eY = zhuhou_a.y;
	if(shehuiInfo.szZhuhou != 0)
		TextOut(hdc,eX+2,eY,shehuiInfo.szZhuhou,strlen(shehuiInfo.szZhuhou));
//
	//血//

	unsigned char red = 0;
	unsigned char green = 255;
	unsigned char blue = 0;
	DWORD color = 0;
	float persent = (float)runTimeInfo.nLife/(float)runTimeInfo.nLifeFull;
	if(persent>0.5f)//小于50%,只有r增加
	{
		red = (UCHAR)((255.0f)*(1.0f - persent)*2.0f);
		color = RGB(red,green,blue);
	}
	else
	{
		red = 255;
		green = (UCHAR)(255.0f*(persent)*2);
		color = RGB(red,green,blue);
	}
	buffer[0] = 0;
	SetTextColor(hdc,color);
	sprintf(buffer,"%s%s",life_a.info," ");
	TextOut(hdc,life_a.x,life_a.y,buffer,strlen(buffer));

	SetTextColor(hdc,magic_a.color);
	buffer[0] = 0;
	sprintf(buffer,"%s%s",magic_a.info," ");
	TextOut(hdc,magic_a.x,magic_a.y,buffer,strlen(buffer));

	buffer[0] = 0;

	SetTextColor(hdc,exp_a.color);
	sprintf(buffer,"%s%s",exp_a.info," ");
	TextOut(hdc,exp_a.x,exp_a.y,buffer,strlen(buffer));

	BitBlt(hdc1,0,0,width,height,hdc,0,0,SRCCOPY);
	SelectObject(hdc,hTempFont);
	DeleteObject(hFont);
	SelectObject(hdc,hTemp);
	DeleteObject(hBitmap1);
	DeleteDC(hdc);
}
void ChatPlayerBaseInfoDlg::Create(HWND hwnd)
{
	hDlg = CreateDialog(KWin32App::m_hInstance,MAKEINTRESOURCE(IDD_PLAYER_BASE_INFO),hwnd,ChatWndProcessFun::ChatPlayerBaseInfoDlgProc);
	LoadSrcINI();
}
void ChatPlayerBaseInfoDlg::LoadSrcINI()
{

	KIniFile iniFile;
	char szValue[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		iniFile.Load(_CHAT_CFG_FILE_1024);
	else
		iniFile.Load(_CHAT_CFG_FILE);
	iniFile.GetInteger("playerInfoDlg","x",0,&x);
	iniFile.GetInteger("playerInfoDlg","y",0,&y);
	iniFile.GetInteger("playerInfoDlg","width",0,&width);
	iniFile.GetInteger("playerInfoDlg","height",0,&height);
	iniFile.GetString("playerInfoDlg","fontName","",fontName,64+1);

	TFONT tFont;
	strcpy(tFont.fontName,fontName);
	HDC hdc =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	isUseDefFont = false;
	EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
	DeleteDC(hdc);
	if(tFont.isInSystem == false)
	{
		fontName[0] = 0;
		strcpy(fontName,ChatString::ChatStringGetString().chatDefualtFont);
		isUseDefFont = true;
	}

	iniFile.GetInteger("playerInfoDlg","bkSrc",0,&bkBitmapIdx);
	int red = 0,green = 0,blue = 0;
	iniFile.GetInteger("sizhu","x",0,&sizhu_a.x);
	iniFile.GetInteger("sizhu","y",0,&sizhu_a.y);
	iniFile.GetString("sizhu","text","",sizhu_a.info,64+1);
	iniFile.GetString("sizhu","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	sizhu_a.color = RGB(red,green,blue);

	iniFile.GetInteger("name","x",0,&name_a.x);
	iniFile.GetInteger("name","y",0,&name_a.y);
	iniFile.GetString("name","text","",name_a.info,64+1);
	iniFile.GetString("name","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	name_a.color = RGB(red,green,blue);

	iniFile.GetInteger("zhuhou","x",0,&zhuhou_a.x);
	iniFile.GetInteger("zhuhou","y",0,&zhuhou_a.y);
	iniFile.GetString("zhuhou","text","",zhuhou_a.info,64+1);
	iniFile.GetString("zhuhou","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	zhuhou_a.color = RGB(red,green,blue);

	iniFile.GetInteger("zhiye","x",0,&zhiye_a.x);
	iniFile.GetInteger("zhiye","y",0,&zhiye_a.y);
	iniFile.GetString("zhiye","text","",zhiye_a.info,64+1);
	iniFile.GetString("zhiye","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	zhiye_a.color = RGB(red,green,blue);

	iniFile.GetInteger("dengji","x",0,&dengji_a.x);
	iniFile.GetInteger("dengji","y",0,&dengji_a.y);
	iniFile.GetString("dengji","text","",dengji_a.info,64+1);
	iniFile.GetString("dengji","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	dengji_a.color = RGB(red,green,blue);


	iniFile.GetInteger("life","x",0,&life_a.x);
	iniFile.GetInteger("life","y",0,&life_a.y);
	iniFile.GetString("life","text","",life_a.info,64+1);
	iniFile.GetString("life","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	life_a.color = RGB(red,green,blue);

	iniFile.GetInteger("magic","x",0,&magic_a.x);
	iniFile.GetInteger("magic","y",0,&magic_a.y);
	iniFile.GetString("magic","text","",magic_a.info,64+1);
	iniFile.GetString("magic","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	magic_a.color = RGB(red,green,blue);

	iniFile.GetInteger("exp","x",0,&exp_a.x);
	iniFile.GetInteger("exp","y",0,&exp_a.y);
	iniFile.GetString("exp","text","",exp_a.info,64+1);	
	iniFile.GetString("exp","rgb","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&red,&green,&blue);
	exp_a.color = RGB(red,green,blue);

	int x = 0,y = 0,width = 0,height = 0;
	iniFile.GetInteger("baseInfolifeBtn","x",0,&x);
	iniFile.GetInteger("baseInfolifeBtn","y",0,&y);
	iniFile.GetInteger("baseInfolifeBtn","width",0,&width);
	iniFile.GetInteger("baseInfolifeBtn","height",0,&height);
	lifeShow.ChatWndCreate(LIFE_SHOW_BUTTON_ID,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|BS_OWNERDRAW,hDlg,"","button",x,y,width,height);
	int currentIdx =0;
	int bkIdx = 0;
	iniFile.GetInteger("baseInfolifeBtn","hBkBitmap",0,&bkIdx);
	iniFile.GetInteger("baseInfolifeBtn","hCurrentBitmap",0,&currentIdx);
	lifeShow.ChatWndSetResource(bkIdx,currentIdx,-1,-1);
	lifeShow.SetWndProcessFun(ChatWndProcessFun::ProcessPlayerLifeShowProc);
	
	//////////////////////////////////////////////////////

	iniFile.GetInteger("baseInfomanaBtn","x",0,&x);
	iniFile.GetInteger("baseInfomanaBtn","y",0,&y);
	iniFile.GetInteger("baseInfomanaBtn","width",0,&width);
	iniFile.GetInteger("baseInfomanaBtn","height",0,&height);
	manaShow.ChatWndCreate(MANA_SHOW_BUTTON_ID,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|BS_OWNERDRAW,hDlg,"","button",x,y,width,height);
	iniFile.GetInteger("baseInfomanaBtn","hBkBitmap",0,&bkIdx);
	iniFile.GetInteger("baseInfomanaBtn","hCurrentBitmap",0,&currentIdx);
	manaShow.ChatWndSetResource(bkIdx,currentIdx,-1,-1);
	manaShow.SetWndProcessFun(ChatWndProcessFun::ProcessPlayerManaShowProc);

	iniFile.GetInteger("baseinfoExpBtn","x",0,&x);
	iniFile.GetInteger("baseinfoExpBtn","y",0,&y);
	iniFile.GetInteger("baseinfoExpBtn","width",0,&width);
	iniFile.GetInteger("baseinfoExpBtn","height",0,&height);
	expShow.ChatWndCreate(MANA_SHOW_BUTTON_ID,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|BS_OWNERDRAW,hDlg,"","button",x,y,width,height);
	iniFile.GetInteger("baseinfoExpBtn","hBkBitmap",0,&bkIdx);
	iniFile.GetInteger("baseinfoExpBtn","hCurrentBitmap",0,&currentIdx);
	expShow.ChatWndSetResource(bkIdx,currentIdx,-1,-1);
	expShow.SetWndProcessFun(ChatWndProcessFun::ProcessPlayerExpShowProc);
	expShow.ChatWndTipCreate(TTS_NOPREFIX,"",256);

	

}
void ChatPlayerBaseInfoDlg::AdjustWindow()
{
	RECT rc;
	GetWindowRect(ChatMainDlg::hMainDlg,&rc);
	RECT rClient;
	GetClientRect(ChatMainDlg::hMainDlg,&rClient);
	AdjustWindowRectEx(&rClient,GetWindowLong(ChatMainDlg::hMainDlg,GWL_STYLE),GetMenu(ChatMainDlg::hMainDlg)!=NULL,GetWindowExStyle(ChatMainDlg::hMainDlg));
	int cx = rc.left -rClient.left + x;
	int cy = rc.top -rClient.top+ y;
	MoveWindow(hDlg,cx,cy,width,height,TRUE);
}
void ChatPlayerBaseInfoDlg::SetTimer()
{
	::SetTimer(hDlg,CHATPLAYERSHOW_DLG_TIMER_ID,CHATPLAYERSHOW_DLG_TIMER_DT,ChatWndProcessFun::ChatPlayerBaseInfoDlgTimerProc);
}
ChatPlayerBaseInfoDlg& ChatPlayerBaseInfoDlg::GetSingle()
{
	static ChatPlayerBaseInfoDlg dlg;
	return dlg;
}
void ChatPlayerBaseInfoDlg::Show()
{
	ShowWindow(hDlg,SW_SHOW);
}
void ChatPlayerBaseInfoDlg::Hide()
{
	ShowWindow(hDlg,SW_HIDE);
}


bool operator == (const KUiPlayerBaseInfo& info1,const KUiPlayerBaseInfo& info2)
{
	if(info1.nSkillType != info2.nSkillType)
		return false;
	return true;
}

bool operator == (const KUiPlayerRuntimeInfo& info1,const KUiPlayerRuntimeInfo&info2)
{
	if(info1.nExperience != info2.nExperience)
		return false;
	if(info1.nExperienceFull != info2.nExperienceFull)
		return false;
	if(info1.nCurLevelExperience != info2.nCurLevelExperience)
		return false;
	if(info1.nLife != info2.nLife)
		return false;
	if(info1.nLifeFull != info2.nLifeFull)
		return false;
	if(info1.nMana != info2.nManaFull)
		return false;
	return true;
}
bool operator == (const KUiPlayerAttribute& info1,const KUiPlayerAttribute&info2)
{
	if(info1.nLevel != info2.nLevel)
		return false;
	if(info1.nSeries!= info2.nSeries)
		return false;
	return true;
}

void ChatPlayerBaseInfoDlg::DrawLifeShowControl(HDC hdc,HBITMAP hBkBitmap,HBITMAP hCurrentBitmap)
{
	const RECT& rc = lifeShow.ChatWndGetRect();
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hTemp = SelectBitmap(hdcBuffer,hBitmap);
	DrawBitmap(hdcBuffer,hBkBitmap,rc.right,rc.bottom);
	float currentLife = (float)runTimeInfo.nLife;
	float fullLife = (float)runTimeInfo.nLifeFull;
	int currentLength = (int)((float)rc.right*currentLife/fullLife);
	HDC hdcBuffer1 = CreateCompatibleDC(hdc);
	HBITMAP hBitmap1 = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hTemp1 = SelectBitmap(hdcBuffer1,hBitmap1);
	DrawBitmap(hdcBuffer1,hCurrentBitmap,rc.right,rc.bottom);
	BitBlt(hdcBuffer,0,0,currentLength,rc.bottom,hdcBuffer1,0,0,SRCCOPY);
	int fontHeight = 0;
	unsigned flags = FW_NORMAL;
	fontHeight = 9;
	HFONT  hFont = CreateFont(fontHeight,0,0,0,flags,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,ChatString::ChatStringGetString().chatDefualtFont);
	HFONT hTempFont  =  SelectFont(hdcBuffer,hFont);
	SetBkMode(hdcBuffer,TRANSPARENT);
	DWORD color = SetTextColor(hdcBuffer,RGB(255,255,255));
	char buffer[256]={0};
	sprintf(buffer,"%d/%d",runTimeInfo.nLife,runTimeInfo.nLifeFull);
	SIZE size;
	int len = strlen(buffer);
	GetTextExtentPoint32(hdcBuffer,buffer,len,&size);
	int cx = (rc.right-size.cx)/2;
	int cy =  (rc.bottom - size.cy)/2;
	TextOut(hdcBuffer,cx,cy,buffer,len);
	SetBkMode(hdcBuffer,OPAQUE);
	SetTextColor(hdcBuffer,color);
	BitBlt(hdc,0,0,rc.right,rc.bottom,hdcBuffer,0,0,SRCCOPY);
	SelectFont(hdcBuffer,hTempFont);
	DeleteFont(hFont);
	SelectBitmap(hdcBuffer1,hTemp1);
	DeleteObject(hBitmap1);
	DeleteDC(hdcBuffer1);
	SelectBitmap(hdcBuffer,hBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcBuffer);

}
void ChatPlayerBaseInfoDlg::DrawManaShowControl(HDC hdc,HBITMAP hBkBitmap,HBITMAP hCurrentBitmap)
{
	const RECT& rc = manaShow.ChatWndGetRect();
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hTemp = SelectBitmap(hdcBuffer,hBitmap);
	DrawBitmap(hdcBuffer,hBkBitmap,rc.right,rc.bottom);
	float currentMana= (float)runTimeInfo.nMana;
	float fullMana = (float)runTimeInfo.nManaFull;
	int currentLength = (int)((float)rc.right*currentMana/fullMana);
	HDC hdcBuffer1 = CreateCompatibleDC(hdc);
	HBITMAP hBitmap1 = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hTemp1 = SelectBitmap(hdcBuffer1,hBitmap1);
	DrawBitmap(hdcBuffer1,hCurrentBitmap,rc.right,rc.bottom);
	BitBlt(hdcBuffer,0,0,currentLength,rc.bottom,hdcBuffer1,0,0,SRCCOPY);
	unsigned flags = FW_NORMAL;
	int fontHeight = 0;
	if(isUseDefFont)
		fontHeight = 9;
	else
	{
		fontHeight = 12;
		flags = FW_BOLD;
	}
	HFONT  hFont = CreateFont(fontHeight,0,0,0,flags,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,fontName);
	HFONT hTempFont  =  SelectFont(hdcBuffer,hFont);
	SetBkMode(hdcBuffer,TRANSPARENT);
	DWORD color = SetTextColor(hdcBuffer,RGB(255,255,255));
	char buffer[256]={0};
	sprintf(buffer,"%d/%d",runTimeInfo.nMana,runTimeInfo.nManaFull);
	SIZE size;
	int len = strlen(buffer);
	GetTextExtentPoint32(hdcBuffer,buffer,len,&size);
	int cx = (rc.right-size.cx)/2;
	int cy =  (rc.bottom - size.cy)/2;
	TextOut(hdcBuffer,cx,cy,buffer,len);
	SetBkMode(hdcBuffer,OPAQUE);
	SetTextColor(hdcBuffer,color);
	BitBlt(hdc,0,0,rc.right,rc.bottom,hdcBuffer,0,0,SRCCOPY);
	SelectFont(hdcBuffer,hTempFont);
	DeleteFont(hFont);
	SelectBitmap(hdcBuffer1,hTemp1);
	DeleteObject(hBitmap1);
	DeleteDC(hdcBuffer1);
	SelectBitmap(hdcBuffer,hBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcBuffer);
}
void ChatPlayerBaseInfoDlg::DrawExpShowControl(HDC hdc,HBITMAP hBkBitmap,HBITMAP hCurrentBitmap)
{
	const RECT& rc = expShow.ChatWndGetRect();
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hTemp = SelectBitmap(hdcBuffer,hBitmap);
	DrawBitmap(hdcBuffer,hBkBitmap,rc.right,rc.bottom);
	float currentExp = (float)runTimeInfo.nExperience;
	float fullExp = (float)runTimeInfo.nExperienceFull;
	int currentLength = (int)((float)rc.right*currentExp/fullExp);
	HDC hdcBuffer1 = CreateCompatibleDC(hdc);
	HBITMAP hBitmap1 = CreateCompatibleBitmap(hdc,rc.right,rc.bottom);
	HBITMAP hTemp1 = SelectBitmap(hdcBuffer1,hBitmap1);
	DrawBitmap(hdcBuffer1,hCurrentBitmap,rc.right,rc.bottom);
	BitBlt(hdcBuffer,0,0,currentLength,rc.bottom,hdcBuffer1,0,0,SRCCOPY);
	BitBlt(hdc,0,0,rc.right,rc.bottom,hdcBuffer,0,0,SRCCOPY);
	SelectBitmap(hdcBuffer1,hTemp1);
	DeleteObject(hBitmap1);
	DeleteDC(hdcBuffer1);
	SelectBitmap(hdcBuffer,hBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcBuffer);
}
void ChatPlayerBaseInfoDlg::UpdateExpShowTip()
{
	char buffer[256] = {0};
	sprintf(buffer,"%s%d/%d",ChatString::ChatStringGetString().playerExpExtra,runTimeInfo.nExperience,runTimeInfo.nExperienceFull);
	expShow.ChatWndUpdataTipText(buffer);
}