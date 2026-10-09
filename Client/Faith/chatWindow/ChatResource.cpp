#include "KWin32App.h"
#include "KWin32Wnd.h"
#include "chatWindow/ChatResource.h"
#include "loadSrcWnd/GDILoadBitmap.h"
#include "layoutinterface.h"
#include "chatWindow/ChatMainDlg.h"

#include "KIniFile.h"

void resourceBitmap::Reset()
{
	if(attr == BITMAP_COLOR_BPP_DYNAMIC)
	{
		if(hBitmap)
			DeleteObject(hBitmap);
		if(bitmapName[0]!=0)
		{
			
			hBitmap = LoadBitmapFromFile(bitmapName);
		}
		else
			hBitmap = 0;
	}
	else
	if(attr == BITMAP_COLOR_BPP_32)
	{
		if(!hBitmap)
			hBitmap = LoadBitmap32FromFile(bitmapName);
	}
}
ChatResource::ChatResource()
{

}
ChatResource::~ChatResource()
{
	while (!resourceList.empty())
	{
		resourceBitmap* src = resourceList.back();
		resourceList.pop_back();
		delete src;
		src = 0;
	}
}

bool ChatResource::LoadSrc()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH]={0},szValue[MAX_PATH]={0};
	char szImagePathIndex[]=CHAT_INI_FILE_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);
	int resourceNum = 0;
	iniFile.GetInteger("ChatResource","numberResource",0,&resourceNum);
	for(int index = 0; index < resourceNum;index++)
	{
		resourceBitmap * pResource = new resourceBitmap;
		sprintf(szValue,"resource%d",index);
		iniFile.GetString("ChatResource",szValue,"",szPath,BITMAP_NAME_LENGTH);
		if(szPath[0]!=0)
		{
			sprintf(pResource->bitmapName,"%s%s",szImagePathIndex,szPath);
			if(index >= 50&&index<=58)
			{
				pResource->attr = BITMAP_COLOR_BPP_32;
				pResource->hBitmap = LoadBitmap32FromFile(pResource->bitmapName);
			}
			else
			{
				pResource->attr = BITMAP_COLOR_BPP_DYNAMIC;
				pResource->hBitmap = LoadBitmapFromFile(pResource->bitmapName);
			}
		}
		resourceList.push_back(pResource);
	}
	return true;
}
void ChatResource::Reset()
{
	int size = resourceList.size();
	for(int idx = 0; idx < size;idx++)
	{
		resourceBitmap* pSrc = resourceList[idx];
		pSrc->Reset();
	}
}
ChatResource& ChatResource::GetSingle()
{
	static ChatResource res;
	return res;
}
resourceBitmap* ChatResource::GetResource(int id)
{
	unsigned int size = ( unsigned int )resourceList.size( );
	if( id >= size )
	{
		id = size -1;
	}
	return resourceList[id];
}
