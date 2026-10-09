//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/17/2006 17:15
//      File_base        : UiBufferWnd
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiDebufferWnd.h"
#include "CoreUseNameDef.h"
#include "CoreShell.h"
#include "UiItemTip.h"
#include "UiMapCentre.h"
#include "UiRoleFace.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiDebufferCentre* KUiWndSingleton<KUiDebufferCentre>::ms_Singleton	= NULL;

KUiDebufferCentre::KUiDebufferCentre( const CEGUI::String& id_name ):
KUiWndSingleton<KUiDebufferCentre>( id_name )
{
	m_nActiveBufCount		= 0;
//	m_pGameObjectIconSet	= ImagesetManager::getSingleton().getImageset( "gameobject" );


}

KUiDebufferCentre::~KUiDebufferCentre()
{

}

void	KUiDebufferCentre::Init( void )
{
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setZLevel( Window::SuperBottom );
		ms_Singleton->InitBufferWnd();
		ms_Singleton->InitBufferPosition();
		ms_Singleton->m_pThisWnd->setRenderMode( false, 3 );
		ms_Singleton->m_pThisWnd->SetBottomWindow();
	}
}

void KUiDebufferCentre::Show( void )
{
	KUiWndSingleton<KUiDebufferCentre>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
		if ( ms_Singleton->m_nActiveBufCount == 0 )
			ms_Singleton->m_pThisWnd->hide();
}

bool KUiDebufferCentre::AddRoleBuffer( unsigned int uParam, CoverType eCoverType )
{
	if ( ms_Singleton == NULL || ms_Singleton->m_pThisWnd == NULL )
	{				
		KUiMiniMap::Show();
		KUiRoleFace::Show();
		KUiDebufferCentre::Show();
	}	
	else
	{
		if ( ms_Singleton->m_nActiveBufCount == ms_Singleton->m_nMaxBufCount )
		{
			return false;
		}

		if (!ms_Singleton->IsVisible())
			ms_Singleton->m_pThisWnd->show();
	}

// 	switch( eCoverType )
// 	{
// 	case cover:
// 		{
// 			ms_Singleton->CoverBuffer( (KBufferSyncInfo*)uParam );
// 		}
// 		break;
// 	case wrap:
// 		{
// 			ms_Singleton->WrapBuffer( (KBufferSyncInfo*)uParam );
// 		}
// 		break;
// 	case parallel:
// 		{
// 			ms_Singleton->ParallelBuffer( (KBufferSyncInfo*)uParam );
// 		}
// 		break;
// 	default:
// 		return false;
// 	}
	ms_Singleton->WrapBuffer( (KBufferSyncInfo*)uParam );
	
	return true;
}

bool KUiDebufferCentre::DelBufferByIndex( unsigned int uParam )
{

	char szName[COMMON_CLIENT_MSG_LEN_16];
	sprintf( szName, "debuffer_%d", uParam );
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild( szName ));
	if ( pGO )
	{
		ms_Singleton->m_pThisWnd->removeChildWindow( szName );
		pGO->rename( "debuffer_temp" );
		pGO->clear();
	}

	int nIdx = uParam + 1;
	int nNewIdx = 0;
	for ( ; nIdx <  ms_Singleton->m_nActiveBufCount; ++nIdx )
	{
		nNewIdx = nIdx - 1;
		sprintf( szName, "debuffer_%d", nIdx);
		TLGameObject* pGOBack = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild( szName ));
		ms_Singleton->m_bufferSet[nNewIdx].pBufferObect	= pGOBack;
		ms_Singleton->m_bufferSet[nNewIdx].pBufferObect->setBufferIdx( nNewIdx );
		sprintf( szName, "debuffer_%d", nNewIdx);
		pGOBack->rename( szName );
		int nWidth = ms_Singleton->m_pThisWnd->getWidth( Absolute );
		if ( nNewIdx < ms_Singleton->m_nMaxBufCountPerRow )
		{
			ms_Singleton->m_bufferSet[nNewIdx].pBufferObect->setPosition( Absolute, Point((nWidth - (nNewIdx + 1) * GAMEOBJECT_WIDTH),0) );
		}
		else
		{
			ms_Singleton->m_bufferSet[nNewIdx].pBufferObect->setPosition( Absolute, Point((nWidth - ((nNewIdx + 1)  - ms_Singleton->m_nMaxBufCountPerRow)*GAMEOBJECT_WIDTH),GAMEOBJECT_HEIGHT) );
		}
	}

	ms_Singleton->m_bufferSet[ms_Singleton->m_nActiveBufCount - 1].pBufferObect	= pGO;
	sprintf( szName, "debuffer_%d", ms_Singleton->m_nActiveBufCount - 1);
	if ( !WindowManager::getSingleton().isWindowPresent( szName ) )
	{
		pGO->rename( szName );
	}
	else
	{
		int a = 0;
		a++;
	}
	pGO->setBufferIdx( ms_Singleton->m_nActiveBufCount - 1 );

	--ms_Singleton->m_nActiveBufCount;	
	
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
		if ( ms_Singleton->m_nActiveBufCount == 0 )
			ms_Singleton->m_pThisWnd->hide();
		
	return true;
}

bool KUiDebufferCentre::DelRoleBuffer( unsigned int uParam )
{
	int nIdx = ms_Singleton->FindBufferActive( uParam );
	if ( nIdx != -1 )
	{
		DelBufferByIndex( nIdx );
		ms_Singleton->m_pThisWnd->requestRedraw();
		return true;
	}
	return false;
}

bool KUiDebufferCentre::DelAllRoleBuffer( void )
{
	KDebufferSet::iterator it = ms_Singleton->m_bufferSet.begin();
	while ( it != ms_Singleton->m_bufferSet.end() )
	{
		if ( (*it).second.pBufferObect != NULL )
		{
			(*it).second.pBufferObect->clear();
		}
		it++;
	}
	ms_Singleton->m_nActiveBufCount = 0;
	
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
		ms_Singleton->m_pThisWnd->hide();

	ms_Singleton->m_bufferSet.clear();
	return true;
}

void KUiDebufferCentre::InitBufferWnd	( void )
{
	//Initialize buffer window position.
	KUiMiniMap::Show();
	KUiRoleFace::Show();
	Window* pFace = m_pWindowManager->getWindow( "TaharezLook/RoleFace" );
	Window* pMiniMap = m_pWindowManager->getWindow( "TaharezLook/MiniMap" );
	if ( pFace && pMiniMap )
	{
		Point absPoint;
		int x = pFace->getXPosition( Absolute ) + pFace->getWidth( Absolute ) * 2;
		absPoint.d_x = x;
		absPoint.d_y = GAMEOBJECT_HEIGHT * 2;
		Size absSize;
		absSize.d_width = g_GetScreenWidth() - pFace->getWidth( Absolute ) * 2 - pMiniMap->getWidth( Absolute );
		absSize.d_height = GAMEOBJECT_HEIGHT * 2;
//		m_pThisWnd->setPosition( Absolute, absPoint );
//		m_pThisWnd->setSize( Absolute, absSize );
	}
}
void KUiDebufferCentre::InitBufferPosition( void )
{
	//Initialize every buffer.
	m_nMaxBufCountPerRow =  (int)(m_pThisWnd->getWidth( Absolute ) / GAMEOBJECT_WIDTH);
	m_nMaxBufCount = (int)(m_pThisWnd->getWidth( Absolute ) / GAMEOBJECT_WIDTH) * (int)(m_pThisWnd->getHeight( Absolute ) / GAMEOBJECT_HEIGHT);
	for ( int nIdx = 0; nIdx < m_nMaxBufCount; ++nIdx )
	{
		char szBufName[COMMON_CLIENT_MSG_LEN_16];
		sprintf( szBufName, "debuffer_%d", nIdx );
		TLGameObject* pGO = static_cast<TLGameObject*>(m_pWindowManager->createWindow( "TaharezLook/GameObject", szBufName ));
		if ( pGO )
		{
			pGO->setStateNotifyFun( &DelBufferByIndex );
			pGO->subscribeEvent( TLGameObject::EventMouseEnters,Event::Subscriber(&KUiDebufferCentre::onMouseIn, this));
			pGO->subscribeEvent( TLGameObject::EventMouseMove,Event::Subscriber(&KUiDebufferCentre::onMouseHover, this));
			pGO->subscribeEvent( TLGameObject::EventMouseLeaves,Event::Subscriber(&KUiDebufferCentre::onMouseLeave, this));
		}
	}
}

int KUiDebufferCentre::FindBufferActive( unsigned int uBufferID )
{
	KDebufferSet::iterator it = m_bufferSet.begin();
	while ( it != m_bufferSet.end() )
	{
		if ( (*it).second.pBufferObect && 
			(*it).second.pBufferObect->getBuffID() == uBufferID && 
			(*it).second.pBufferObect != NULL )
		{
			return (*it).first;
		}
		it++;
	}
	return -1;
}

void KUiDebufferCentre::CoverBuffer( const KBufferSyncInfo* buffInfo )
{
	int nPos = FindBufferActive( buffInfo->nBuffID );
	if ( nPos == -1 )
	{
		ParallelBuffer( buffInfo );
	}
	else
	{
		KUiDebufferCentre::DelRoleBuffer( buffInfo->nBuffID );
		ParallelBuffer( buffInfo );
	}
}
void KUiDebufferCentre::WrapBuffer( const KBufferSyncInfo* buffInfo )
{
	int nPos = FindBufferActive( buffInfo->nBuffID );
	if ( nPos == -1 )
	{
		ParallelBuffer( buffInfo );
	}
	else
	{
		TLGameObject::GameObject GO;
		KBufferInfo tagBufInfo;
		g_pCoreShell->GetGameData( GDI_GET_BUFFER_INFO, (unsigned int)&tagBufInfo, buffInfo->nTempBuffID );
		if ( strcmp( tagBufInfo.szImage, "" ) == 0 || strcmp( tagBufInfo.szImage, "0" ) == 0 )
		{
			return;
		}
		GO.d_gameobject = AnsiToUtf8( tagBufInfo.szImage );
		GO.d_count		= buffInfo->nPileCount;
		GO.d_disabledTime = 0;
		GO.d_coolingTime = 0;
		GO.d_intonateTime = buffInfo->nTime;
		GO.d_bufferIdx		= nPos;
		GO.d_bufferID		= buffInfo->nBuffID;
		GO.d_type = TLGameObject::buffer;
		m_pThisWnd->addChildWindow( m_bufferSet[nPos].pBufferObect );
		m_bufferSet[nPos].pBufferObect->setObject( GO );
		m_bufferSet[nPos].pBufferObect->setTooltipText( AnsiToUtf8(tagBufInfo.nDesc));
		m_bufferSet[nPos].pBufferObect->show();
	}
}
void KUiDebufferCentre::ParallelBuffer( const KBufferSyncInfo* buffInfo )
{
	int nIdx = 0;
	for ( nIdx = 0; nIdx < m_nMaxBufCount; ++nIdx )
	{
		if (	m_bufferSet[nIdx].pBufferObect == NULL || 
			(	m_bufferSet[nIdx].pBufferObect && 
				m_bufferSet[nIdx].pBufferObect->getState() == TLGameObject::idleState) )
		{
			if ( m_nActiveBufCount >= m_nMaxBufCount )
			{
				return;
			}
			KBufferInfo tagBufInfo;
			g_pCoreShell->GetGameData( GDI_GET_BUFFER_INFO, (unsigned int)&tagBufInfo, buffInfo->nTempBuffID );
			if ( strcmp( tagBufInfo.szImage, "" ) == 0 || strcmp( tagBufInfo.szImage, "0" ) == 0 )
			{
				return;
			}

			char szName[COMMON_CLIENT_MSG_LEN_16];
			sprintf( szName, "debuffer_%d", m_nActiveBufCount );
			m_bufferSet[nIdx].pBufferObect = static_cast<TLGameObject*>(m_pWindowManager->getWindow(szName));
			//加入tip显示
			m_bufferSet[nIdx].pBufferObect->setUserString("BuffId", iToString(buffInfo->nTempBuffID));
			TLGameObject::GameObject GO;

			GO.d_gameobject = AnsiToUtf8( tagBufInfo.szImage );
			GO.d_count		= buffInfo->nPileCount;
			GO.d_disabledTime = 0;
			GO.d_coolingTime = 0;
			GO.d_intonateTime = buffInfo->nTime;
			GO.d_bufferIdx		= nIdx;
			GO.d_bufferID		= buffInfo->nBuffID;
			GO.d_type = TLGameObject::buffer;
			m_pThisWnd->addChildWindow( m_bufferSet[nIdx].pBufferObect );
			m_bufferSet[nIdx].pBufferObect->setObject( GO );
			m_bufferSet[nIdx].pBufferObect->setTooltipText( AnsiToUtf8(tagBufInfo.nDesc));
			int nWidth = m_pThisWnd->getWidth( Absolute );
			if ( nIdx < m_nMaxBufCountPerRow )
			{
				m_bufferSet[nIdx].pBufferObect->setPosition( Absolute, Point((nWidth - (nIdx + 1) * GAMEOBJECT_WIDTH),0) );
			}
			else
			{
				m_bufferSet[nIdx].pBufferObect->setPosition( Absolute, Point((nWidth - ((nIdx + 1)  - m_nMaxBufCountPerRow)*GAMEOBJECT_WIDTH),GAMEOBJECT_HEIGHT) );
			}
			
			m_bufferSet[nIdx].pBufferObect->show();
			++ms_Singleton->m_nActiveBufCount;
			return;
		}
	}
}

bool KUiDebufferCentre::AddBufferReq( int nBufferID )
{
//	g_pCoreShell->OperationRequest( GOI_OPEN_BUFFER, nBufferID, NULL );
	return true;
}

bool KUiDebufferCentre::DelBufferReq( int nBufferID )
{
	g_pCoreShell->OperationRequest( GOI_DEL_BUFFER, nBufferID, NULL );
	return true;
}

void KUiDebufferCentre::Hide( void )
{
	KUiWndSingleton<KUiDebufferCentre>::Hide();
	DelAllRoleBuffer();
}

bool KUiDebufferCentre::onMouseHover( const CEGUI::EventArgs& args )
{
	MouseEventArgs* event = (MouseEventArgs*)&args;

	KBufferInfo tagBufInfo;

	int buffId = atoi(event->window->getUserString("BuffId").c_str());

	g_pCoreShell->GetGameData( GDI_GET_BUFFER_INFO, (unsigned int)&tagBufInfo,  buffId);
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show(tagBufInfo.nDesc, event->window->getUnclippedInnerRect(), KUiItemTip::BottomLeft);
	
	return true;
}

bool KUiDebufferCentre::onMouseIn( const CEGUI::EventArgs& args )
{
	MouseEventArgs* event = (MouseEventArgs*)&args;

	KBufferInfo tagBufInfo;

	int buffId = atoi(event->window->getUserString("BuffId").c_str());

	g_pCoreShell->GetGameData( GDI_GET_BUFFER_INFO, (unsigned int)&tagBufInfo,  buffId);
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show(tagBufInfo.nDesc, event->window->getUnclippedInnerRect(), KUiItemTip::BottomLeft);

	return true;
}

bool KUiDebufferCentre::onMouseLeave( const CEGUI::EventArgs& args )
{
	KUiItemTip::Hide();

	return true;
}