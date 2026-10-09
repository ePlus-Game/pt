#include <windows.h>
#include <windowsx.h>
#include <list>
//#include <string>
using namespace std;
#include "layoutinterface.h"
#include "Ui/UiConfigManager.h"
#include "GameDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "ui/UiCommon.h"


#include "chatWindow/chatWnd.h"

#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"




ChatWndRender::ChatWndRender()
{
	numberUsed = 0;
	allHeight = 0;
}
ChatWndRender::~ChatWndRender()
{
	for(int i = 0; i < MAX_CHAT_INFO_SEG;i++)
	{
		infoWndLayOut[i].pLayOut->clearLayout();
		infoWndLayOut[i].pLayOut->Release();
		delete infoWndLayOut[i].pLayOut;
	}
}

void ChatWndRender::ChatWndRenderCreate()
{

	for(int i = 0; i < MAX_CHAT_INFO_SEG ; i++)
	{
		infoWndLayOut[i].pRender= new GDIRender;
		CreateLayout(&infoWndLayOut[i].pLayOut,infoWndLayOut[i].pRender);
	}
}
void ChatWndRender::ChatWndRenderClear()
{
	for(int i = 0; i < numberUsed;i++)
	{
		infoWndLayOut[i].pLayOut->clearLayout();
		infoWndLayOut[i].isEmpty = true;
		infoWndLayOut[i].pt.x = 0;
		infoWndLayOut[i].pt.y = 0;
		infoWndLayOut[i].renderHeight = 0;
		infoWndLayOut[i].renderWidth = 0;
	}
}

void ChatWndRender::ChatWndRenderSetRenderPos(int pos)
{
	infoWndLayOut[0].pt.y = pos;
	for(int i = 1; i < numberUsed;i++)
	{
		infoWndLayOut[i].pt.y = infoWndLayOut[i-1].pt.y + infoWndLayOut[i-1].renderHeight;
	}
}

void ChatWndRender::ChatWndRenderCopyLayoutText(ILayout* pLayout,const char* seg)
{
#define MAX_SELECTION_S 1000000
#define MAX_SELECTION_E 1000000
	if(pLayout == 0)
		return;
	LOElemInfo* pElem = 0;
	int count = pLayout->getElemList(pElem);
	if(count<=0)
		return;
	char buffer[LAYOUT_TEXT_MAX_LEN+1] = {0};
	sprintf(buffer,"<Layout width=%d height=2000>%s</Layout>",width,seg);
//	sprintf(buffer,"<Layout width=%d height=2000><Seg text-align=left></Seg></Layout>",width);
	buffer[LAYOUT_TEXT_MAX_LEN] = 0;
	HDC hdc = GetWindowDC(hWnd);


	HDC hOld  = 0;
	int rx = 0,ry = 0;
	LORect loRC;
	rx = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderX;
	ry = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY;
	loRC.setPos(rx,ry);
	RECT& rc = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderRect;
	loRC.setWidth(rc.right - rc.left);
	loRC.setHeight(rc.bottom - rc.top);
	if(numberUsed == 0)
	{
		hOld = ((GDIRender*)infoWndLayOut[0].pRender)->hCurrentDC ;
		((GDIRender*)infoWndLayOut[0].pRender)->hCurrentDC = hdc;
		if(infoWndLayOut[0].isEmpty == false)
			infoWndLayOut[0].pLayOut->clearLayout();
		infoWndLayOut[0].pLayOut->SetText(buffer);
		for(int i = 0;i<count;i++)
		{
			pElem[i].vAlign = LO_VA_BOTTOM;
			infoWndLayOut[0].pLayOut->setSelection(MAX_SELECTION_S,MAX_SELECTION_E);
			infoWndLayOut[0].pLayOut->insertElem(pElem[i]);
		}
		numberUsed++;
		
		infoWndLayOut[0].pLayOut->setClipper(loRC);
		infoWndLayOut[0].pLayOut->flashLayout();
		infoWndLayOut[0].renderWidth = infoWndLayOut[0].pLayOut->getRenderArea(false).getWidth();
		infoWndLayOut[0].renderHeight = infoWndLayOut[0].pLayOut->getRenderArea(false).getHeight();
		infoWndLayOut[0].pt.y = ry;
		infoWndLayOut[0].pt.x = rx;
		
		infoWndLayOut[0].isEmpty = false;
		allHeight +=infoWndLayOut[0].renderHeight;
		((GDIRender*)infoWndLayOut[0].pRender)->hCurrentDC = hOld;
	}
	else
	if(numberUsed < MAX_CHAT_INFO_SEG)
	{
		hOld = ((GDIRender*)infoWndLayOut[numberUsed].pRender)->hCurrentDC ;
		((GDIRender*)infoWndLayOut[numberUsed].pRender)->hCurrentDC = hdc;

		if(infoWndLayOut[numberUsed].isEmpty == false)
			infoWndLayOut[numberUsed].pLayOut->clearLayout();
		infoWndLayOut[numberUsed].pLayOut->SetText(buffer);
		for(int i = 0;i<count;i++)
		{
			pElem[i].vAlign = LO_VA_BOTTOM;
			infoWndLayOut[numberUsed].pLayOut->setSelection(MAX_SELECTION_S,MAX_SELECTION_E);
			infoWndLayOut[numberUsed].pLayOut->insertElem(pElem[i]);
		}
		infoWndLayOut[numberUsed].pLayOut->setClipper(loRC);
		infoWndLayOut[numberUsed].pLayOut->flashLayout();
		infoWndLayOut[numberUsed].renderHeight = infoWndLayOut[numberUsed].pLayOut->getRenderArea(false).getHeight();
		infoWndLayOut[numberUsed].renderWidth = infoWndLayOut[numberUsed].pLayOut->getRenderArea(false).getWidth();
		infoWndLayOut[numberUsed].pt.y= infoWndLayOut[numberUsed-1].pt.y + infoWndLayOut[numberUsed-1].renderHeight;
		infoWndLayOut[numberUsed].pt.x = rx;
		infoWndLayOut[numberUsed].isEmpty = false;
		allHeight +=infoWndLayOut[numberUsed].renderHeight;
		((GDIRender*)infoWndLayOut[numberUsed].pRender)->hCurrentDC = hOld;
		numberUsed++;

	}
	else
	if(numberUsed == MAX_CHAT_INFO_SEG)
	{
		InfoWndLayOut  tempInfoLay = infoWndLayOut[0];
		for(int i = 0; i < MAX_CHAT_INFO_SEG-1;i++)
		{
			infoWndLayOut[i] = infoWndLayOut[i+1];
			infoWndLayOut[i].pt.y -= tempInfoLay.renderHeight;
		}
		allHeight-= tempInfoLay.renderHeight;
		if(tempInfoLay.isEmpty == false)
			tempInfoLay.pLayOut->clearLayout();
		hOld = ((GDIRender*)tempInfoLay.pRender)->hCurrentDC ;
		((GDIRender*)tempInfoLay.pRender)->hCurrentDC = hdc;
		tempInfoLay.pLayOut->SetText(buffer);
		for(int index = 0;index<count;index++)
		{
			pElem[index].vAlign = LO_VA_BOTTOM;
			tempInfoLay.pLayOut->setSelection(MAX_SELECTION_S,MAX_SELECTION_E);
			tempInfoLay.pLayOut->insertElem(pElem[index]);
		}
		tempInfoLay.pt.y = infoWndLayOut[numberUsed-2].pt.y + infoWndLayOut[numberUsed-2].renderHeight;
		tempInfoLay.pt.x = rx;
		tempInfoLay.pLayOut->setClipper(loRC);
		tempInfoLay.pLayOut->flashLayout();
		tempInfoLay.renderWidth = tempInfoLay.pLayOut->getRenderArea(false).getWidth();
		tempInfoLay.renderHeight = tempInfoLay.pLayOut->getRenderArea(false).getHeight();
		tempInfoLay.isEmpty = false;
		infoWndLayOut[numberUsed-1] = tempInfoLay;
		allHeight +=infoWndLayOut[numberUsed-1].renderHeight;
		((GDIRender*)tempInfoLay.pRender)->hCurrentDC = hOld;
	}
	ReleaseDC(hWnd,hdc);
	delete [] pElem;
	return;



}
void ChatWndRender::ChatWndRenderPushBack(const char* seg)
{

	static char  buffer[LAYOUT_TEXT_MAX_LEN + 40+1] = {0};
	buffer[0] = 0;
	if(strlen(seg)>=LAYOUT_TEXT_MAX_LEN )
	{
		return;
	}
	sprintf(buffer,"<Layout width=%d height=2000>\n",width);
	strcat(buffer,seg);
	strcat(buffer,"</Layout>");
	buffer[LAYOUT_TEXT_MAX_LEN+40] = 0;
	HDC hdc = GetWindowDC(hWnd);
	
	HDC hOld  = 0;

	int rx = 0,ry = 0;
	LORect loRC;
	rx = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderX;
	ry = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderY;
	loRC.setPos(rx,ry);
	RECT& rc = B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->renderRect;
	loRC.setWidth(rc.right - rc.left);
	loRC.setHeight(rc.bottom - rc.top);
	if(numberUsed == 0)
	{
		hOld = ((GDIRender*)infoWndLayOut[0].pRender)->hCurrentDC ;
		((GDIRender*)infoWndLayOut[0].pRender)->hCurrentDC = hdc;
		if(infoWndLayOut[0].isEmpty == false)
			infoWndLayOut[0].pLayOut->clearLayout();
		infoWndLayOut[0].pLayOut->SetText(buffer);
		numberUsed++;
		infoWndLayOut[0].pLayOut->setClipper(loRC);
		infoWndLayOut[0].pLayOut->flashLayout();
		infoWndLayOut[0].renderWidth = infoWndLayOut[0].pLayOut->getRenderArea(false).getWidth();
		infoWndLayOut[0].renderHeight = infoWndLayOut[0].pLayOut->getRenderArea(false).getHeight();
		infoWndLayOut[0].pt.y = ry;
		infoWndLayOut[0].pt.x = rx;
		infoWndLayOut[0].isEmpty = false;
		allHeight +=infoWndLayOut[0].renderHeight;
		((GDIRender*)infoWndLayOut[0].pRender)->hCurrentDC = hOld;
	}
	else
	if(numberUsed < MAX_CHAT_INFO_SEG)
	{
		hOld = ((GDIRender*)infoWndLayOut[numberUsed].pRender)->hCurrentDC ;
		((GDIRender*)infoWndLayOut[numberUsed].pRender)->hCurrentDC = hdc;

		if(infoWndLayOut[numberUsed].isEmpty == false)
			infoWndLayOut[numberUsed].pLayOut->clearLayout();
		infoWndLayOut[numberUsed].pLayOut->SetText(buffer);
		infoWndLayOut[numberUsed].pLayOut->setClipper(loRC);
		infoWndLayOut[numberUsed].pLayOut->flashLayout();
		infoWndLayOut[numberUsed].renderHeight = infoWndLayOut[numberUsed].pLayOut->getRenderArea(false).getHeight();
		infoWndLayOut[numberUsed].renderWidth = infoWndLayOut[numberUsed].pLayOut->getRenderArea(false).getWidth();
		infoWndLayOut[numberUsed].pt.y= infoWndLayOut[numberUsed-1].pt.y + infoWndLayOut[numberUsed-1].renderHeight;
		infoWndLayOut[numberUsed].pt.x = rx;
		infoWndLayOut[numberUsed].isEmpty = false;
		allHeight +=infoWndLayOut[numberUsed].renderHeight;
		((GDIRender*)infoWndLayOut[numberUsed].pRender)->hCurrentDC = hOld;
		numberUsed++;

	}
	else
	if(numberUsed == MAX_CHAT_INFO_SEG)
	{
		InfoWndLayOut  tempInfoLay = infoWndLayOut[0];
		for(int i = 0; i < MAX_CHAT_INFO_SEG-1;i++)
		{
			infoWndLayOut[i] = infoWndLayOut[i+1];
			infoWndLayOut[i].pt.y -= tempInfoLay.renderHeight;
		}
		allHeight-= tempInfoLay.renderHeight;
		if(tempInfoLay.isEmpty == false)
			tempInfoLay.pLayOut->clearLayout();
		hOld = ((GDIRender*)tempInfoLay.pRender)->hCurrentDC ;
		((GDIRender*)tempInfoLay.pRender)->hCurrentDC = hdc;
		tempInfoLay.pLayOut->SetText(buffer);
		tempInfoLay.pLayOut->setClipper(loRC);
		tempInfoLay.pt.y = infoWndLayOut[numberUsed-2].pt.y + infoWndLayOut[numberUsed-2].renderHeight;
		tempInfoLay.pt.x = rx;
		tempInfoLay.pLayOut->flashLayout();
		tempInfoLay.renderWidth = tempInfoLay.pLayOut->getRenderArea(false).getWidth();
		tempInfoLay.renderHeight = tempInfoLay.pLayOut->getRenderArea(false).getHeight();
		tempInfoLay.isEmpty = false;
		infoWndLayOut[numberUsed-1] = tempInfoLay;
		allHeight +=infoWndLayOut[numberUsed-1].renderHeight;
		((GDIRender*)tempInfoLay.pRender)->hCurrentDC = hOld;
	}
	ReleaseDC(hWnd,hdc);
	return;
}

void ChatWndRender::ChatWndRenderAutoScroll(int height)
{
	if(numberUsed==0)
		return;
	if(height>=allHeight)
		return;
	if(!B2ChatDialog::chatManager.ChatManagerGetInfoWnd()->ChatInfoWndIsScroll())
	{
		B2ChatDialog::chatManager.normalButton[_DOWN_BUTTON_INDEX].ChatWndSetTimer();
		return;
	}

	infoWndLayOut[numberUsed-1].pt.y = height - infoWndLayOut[numberUsed-1].renderHeight;
	if(numberUsed==1)
		return;
	for(int i = numberUsed-2;i>=0;i--)
	{
		infoWndLayOut[i].pt.y = infoWndLayOut[i+1].pt.y - infoWndLayOut[i].renderHeight;
	}
}
void ChatWndRender::ChatWndRenderItself(HDC hdc)
{
	
	for(int i = 0; i < numberUsed; i++)
	{
		HDC hOld = ((GDIRender*)infoWndLayOut[i].pRender)->hCurrentDC = hdc;
		infoWndLayOut[i].pLayOut->Render(infoWndLayOut[i].pt.x,infoWndLayOut[i].pt.y,0);
		((GDIRender*)infoWndLayOut[i].pRender)->hCurrentDC = hOld;
	}
}

void ChatWndRender::ChatWndRenderScroll(int dt)
{
	infoWndLayOut[0].pt.y += dt;
	for(int i = 1; i < numberUsed;i++)
	{
		infoWndLayOut[i].pt.y = infoWndLayOut[i-1].pt.y + infoWndLayOut[i-1].renderHeight;
	}
}

BOOL ChatWndRender::ChatWndRenderGetElement(int x,int y,LOElemInfo& info)
{
	for(int i = 0 ; i <numberUsed; i ++)
	{
		int cy = y - infoWndLayOut[i].pt.y;
		int cx = x - infoWndLayOut[i].pt.x;
		if(infoWndLayOut[i].pLayOut->pickupElem(cx,cy,info) == true)
			return true;
	}
	return false;
}

