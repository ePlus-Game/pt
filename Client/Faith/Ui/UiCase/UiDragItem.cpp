#include "KWin32.h"
#include "UiDragItem.h"
#include "KWin32Wnd.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

template<> 
KUiDragItem* KUiWndSingleton<KUiDragItem>::ms_Singleton	= NULL;

KUiDragItem::KUiDragItem(const CEGUI::String& id_name):
KUiWndSingleton<KUiDragItem>( id_name )
{	
	ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		TLGameObject* thisObj = (TLGameObject*)ms_Singleton->m_pThisWnd;
		//设置位置信息
		static KObjAtContRegion thisRegion;
		thisRegion.eContainer = UOC_GAMESPACE;
		thisObj->setUserData(&thisRegion);
		thisObj->SetHand(true);
		initItem();
		
	}
}

KUiDragItem::~KUiDragItem()
{

}

TLGameObject* KUiDragItem::getObj()
{
	return (TLGameObject*)m_pThisWnd;
}

void KUiDragItem::initItem()
{
	TLGameObject* thisObj = (TLGameObject*)m_pThisWnd;
	//设置类型信息
	TLGameObject::GameObject thisObjInfo;
	thisObjInfo.d_type = TLGameObject::idle;
	thisObjInfo.d_gameobject = BACKGROUND_IMAGE;
	thisObj->setObject(thisObjInfo);
	//
	KObjAtContRegion* region = (KObjAtContRegion*)thisObj->getUserData();
	region->Obj.uId = -1;
	region->Obj.uGenre = CGOG_NOTHING;
	//取消拖动
	thisObj->setCanDrag(false);
	thisObj->setPosition(Absolute, Point(-1,-1));
	thisObj->setEnabled(false);
}

bool KUiDragItem::onMouseMove(const CEGUI::EventArgs& e)
{
	TLGameObject* thisObj = (TLGameObject*)m_pThisWnd;
	if(false == thisObj->getCanDrag())
		return false;

	Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();

	
	Rect area = m_pThisWnd->getUnclippedPixelRect();
	Point pos = area.getPosition();

	Point absPos = mousePos - Point(area.getWidth()/2, area.getHeight()/2+5);
	
	m_pThisWnd->setPosition( Absolute, absPos );

	bool needRepos = false;
	if(area.d_right > g_GetScreenWidth())
	{
		pos.d_x = g_GetScreenWidth() - area.getWidth();
		needRepos = true;
	}
	if(area.d_bottom > g_GetScreenHeight())
	{
		pos.d_y = g_GetScreenHeight() - area.getHeight();
		needRepos = true;
	}
	if(area.d_top < 0)
	{
		pos.d_y = 0;
		needRepos = true;
	}
	if(area.d_left < 0)
	{
		pos.d_x = 0;
		needRepos = true;
	}
	if(needRepos)
	{
		m_pThisWnd->setPosition(Absolute, pos);
	}

	if (IsVisible())
	{
		m_pThisWnd->setZLevel(Window::Top);
		Show();
	}
	return false;
}
