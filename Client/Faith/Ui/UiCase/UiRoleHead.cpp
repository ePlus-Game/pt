//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/09/2007 12:25
//      File_base        : UiRoleHead
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "UiRoleHead.h"
#include "Coreshell.h"
#include "../LayoutRender.h"

extern iCoreShell*		g_pCoreShell;

KUiLayoutUnit::KUiLayoutUnit()
{
	parentControl = NULL;
	infoConrtrol = NULL;
//	d_layout = NULL;
//	Init();
}
KUiLayoutUnit::KUiLayoutUnit(DWORD id)
{
	Init(id);
	
}
KUiLayoutUnit::~KUiLayoutUnit()
{
//	d_layout->Release();
	if ( parentControl )
	{
		CEGUI::WindowManager::getSingleton().destroyWindow(parentControl);
		parentControl = NULL;
		infoConrtrol = NULL;
		CEGUI::WindowManager::getSingleton().cleanNpcHeadWindow();
	}
	
}

#define PLAYER_UI_INFO_DEFAULT_WIDTH 200
#define PLAYER_UI_INFO_DEFAULT_HEIGHT 200
void KUiLayoutUnit::Init( DWORD id )
{
	const char windowName[] = "TaharezLook/npcHeadInfoBk/";
	char controlName[COMMON_CLIENT_MSG_LEN_512]={0};

	sprintf(controlName,"%s%ul",windowName,id);

	parentControl = (TLStaticImage*)(CEGUI::WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName,controlName));
	if(parentControl == 0)
		return ;
	strcat(controlName,"/infoText");
	infoConrtrol = (TLStaticText*)(CEGUI::WindowManager::getSingleton().createWindow(TLStaticText::WidgetTypeName,controlName));
	if(infoConrtrol == 0)
		return;
	parentControl->addChildWindow(infoConrtrol);
	Window* pRoot = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
	pRoot->addChildWindow(parentControl);
	parentControl->setDummyWnd(true);
	parentControl->setRenderMode(false);
	parentControl->setZLevel(Window::SuperBottom);
	infoConrtrol->setDummyWnd(true);
	infoConrtrol->useLayout();
	parentControl->isNpcHeadInfo = true;
	
	
}



void KUiLayoutUnit::UpDateInfo( char* pText)
{
	if(pText)
	{
		ILayout* pLayout = infoConrtrol->getLayout();
		pLayout->clearLayout();
		pLayout->SetText(pText);
		pLayout->flashLayout();
		LORect rc = pLayout->getRenderArea(false);
		d_width = rc.getWidth();
		d_height = rc.getHeight();
		parentControl->setWidth(Absolute,d_width);
		parentControl->setHeight(Absolute,d_height);
		infoConrtrol->setWidth(Absolute,d_width);
		infoConrtrol->setHeight(Absolute,d_height);
		infoConrtrol->setXPosition(Absolute,0);
		infoConrtrol->setYPosition(Absolute,0);
		parentControl->setRenderMode(false,TEXTURE_PICTH_TYP_BLT_PLAYERHEADINFO);
		parentControl->requestRedraw();
	}
}
void KUiLayoutUnit::SetPosition( int x, int y )
{
	d_nX = x;
	d_nY = y;
	int fx = d_nX - (d_width>>1);
	int fy = d_nY - d_height;
	
	if ( NULL == infoConrtrol )
	{
		return;
	}

	ILayout* pLayout = infoConrtrol->getLayout();
	
	if ( pLayout && parentControl)
	{
		parentControl->setXPosition(Absolute,fx);
		parentControl->setYPosition(Absolute,fy);
		if(fx+d_width<0||fx>(int)(KWin32App::m_uScreenWidth)-1||
		   fy+d_height<0||fy>(int)(KWin32App::m_uScreenHeight)-1)
			parentControl->hide();
		else
			parentControl->show();
	}
}

void KUiLayoutUnit::Show(bool nShow /* = true */)
{
	if ( NULL == parentControl )
	{
		return;
	}

	if(nShow)
		parentControl->show();
	else
		parentControl->hide();
}

void KUiLayoutUnit::Redraw()
{
 infoConrtrol->requestRedraw();;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

KUiLayoutManager::KUiLayoutManager()
{
}

KUiLayoutManager::~KUiLayoutManager()
{
	_LayoutSet::iterator it = d_layoutSet.begin();
	while ( it != d_layoutSet.end() )
	{
		if ( it->second )
		{
			delete it->second;
		}
		++it;
	}
}

KUiLayoutManager&	KUiLayoutManager::GetSingleton( void )
{
	static KUiLayoutManager g_lm;
	return g_lm;
}

void	KUiLayoutManager::AddLayoutUnit(DWORD dwID, char* info ,int x /* = 0 */,int y /* = 0 */)
{

//	if(name == 0)
////		return;
//	DWORD id = g_FileName2Id(name);
	_LayoutSet::iterator it = d_layoutSet.find( dwID );
	if ( it != d_layoutSet.end() && it->second )
	{
		it->second->UpDateInfo( info );

	}
	else
	{
		d_layoutSet[dwID] = new KUiLayoutUnit(dwID);
		if ( d_layoutSet[dwID] )
		{
			d_layoutSet[dwID]->UpDateInfo( info );

		}
	}
//	Render();
}


void	KUiLayoutManager::SetLayoutUnitPosition(DWORD dwID, int x, int y )
{
//	if(name == NULL)
//		return;
//	DWORD id = g_FileName2Id(name);
	_LayoutSet::iterator it = d_layoutSet.find( dwID );
	if ( it != d_layoutSet.end() && it->second )
	{
		it->second->SetPosition( x, y );
	}
}

void	KUiLayoutManager::DelLayoutUnit(DWORD dwID )
{
//	if(name == NULL||name[0] == 0)
	//	return ;
//	DWORD id = g_FileName2Id(name);
	_LayoutSet::iterator it = d_layoutSet.find( dwID );
	if ( it != d_layoutSet.end() && it->second )
	{
		it->second->Show(false);
		delete it->second;
		it->second = NULL;
		d_layoutSet.erase(it);
		
	}
}

void	KUiLayoutManager::RenderLayoutUnit(DWORD dwID )
{
//	DWORD id = g_FileName2Id(name);
	_LayoutSet::iterator it = d_layoutSet.find( dwID );
	if ( it != d_layoutSet.end() && it->second )
	{
		it->second->Show();
	}
}


void	KUiLayoutManager::Render( void )
{
	_LayoutSet::iterator it = d_layoutSet.begin();
	while ( it != d_layoutSet.end() )
	{
		if ( it->second )
		{
			it->second->Show();
			it->second->Redraw();
			
		}
		++it;
	}	
}
void KUiLayoutManager::Hide(DWORD dwID)
{
//	DWORD id = g_FileName2Id(name);
	_LayoutSet::iterator it = d_layoutSet.find( dwID );
	if ( it != d_layoutSet.end() && it->second )
	{
		it->second->Show(false);
	}
}
void KUiLayoutManager::Hide()
{
	_LayoutSet::iterator it = d_layoutSet.begin();
	while ( it != d_layoutSet.end() )
	{
		if ( it->second )
		{
			it->second->Show(false);
		}
		++it;
	}	
}
