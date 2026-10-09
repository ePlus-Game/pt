//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/17/2006 17:15
//      File_base        : UiTargetbufferWnd
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiTargetbufferWnd.h"
#include "CoreUseNameDef.h"
#include "CoreShell.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiTargetbufferCentre* KUiWndSingleton<KUiTargetbufferCentre>::ms_Singleton	= NULL;

KUiTargetbufferCentre::KUiTargetbufferCentre( const CEGUI::String& id_name ):
KUiWndSingleton<KUiTargetbufferCentre>( id_name )
{
	m_nActiveBufCount		= 0;
}

KUiTargetbufferCentre::~KUiTargetbufferCentre()
{

}

bool KUiTargetbufferCentre::AddRoleBuffer( unsigned int uParam, CoverType eCoverType )
{	
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		if ( ms_Singleton->m_nActiveBufCount >= ms_Singleton->m_nMaxBufCount )
		{
			return false;
		}
		
		if ( !ms_Singleton->IsVisible() )
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

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->WrapBuffer( (KBufferSyncInfo*)uParam );
	}
	return true;
}

bool KUiTargetbufferCentre::DelBufferByIndex( unsigned int uParam )
{

	char szName[COMMON_CLIENT_MSG_LEN_16];
	sprintf( szName, "targetbuffer_%d", uParam );
	TLGameObject* pGO = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild( szName ));
	if ( pGO )
	{
		ms_Singleton->m_pThisWnd->removeChildWindow( szName );
		pGO->rename( "targetbuffer_temp" );
		pGO->clear();
	}

	int nIdx = uParam + 1;
	int nNewIdx = 0;
	for ( ; nIdx <  ms_Singleton->m_nActiveBufCount; ++nIdx )
	{
		nNewIdx = nIdx - 1;
		sprintf( szName, "targetbuffer_%d", nIdx);
		TLGameObject* pGOBack = static_cast<TLGameObject*>(ms_Singleton->m_pThisWnd->getChild( szName ));
		ms_Singleton->m_bufferSet[nNewIdx].pBufferObect	= pGOBack;
//		ms_Singleton->m_bufferSet[nNewIdx].nBufferID	= pGOBack->getBufferID();
		ms_Singleton->m_bufferSet[nNewIdx].pBufferObect->setBufferIdx( nNewIdx );
		sprintf( szName, "targetbuffer_%d", nNewIdx);
		pGOBack->rename( szName );
		int nWidth = ms_Singleton->m_pThisWnd->getWidth( Absolute );
		if ( nNewIdx < ms_Singleton->m_nMaxBufCountPerRow )
		{
			ms_Singleton->m_bufferSet[nNewIdx].pBufferObect->setPosition( Absolute, Point((nWidth - (nNewIdx + 1) * GAMEOBJECT_WIDTH_MIN),0) );
		}
		else
		{
			ms_Singleton->m_bufferSet[nNewIdx].pBufferObect->setPosition( Absolute, Point((nWidth - ((nNewIdx + 1)  - ms_Singleton->m_nMaxBufCountPerRow)*GAMEOBJECT_WIDTH_MIN),GAMEOBJECT_WIDTH_MIN) );
		}
	}

	ms_Singleton->m_bufferSet[ms_Singleton->m_nActiveBufCount - 1].pBufferObect	= pGO;
	sprintf( szName, "targetbuffer_%d", ms_Singleton->m_nActiveBufCount - 1);
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
	
	return true;
}

bool KUiTargetbufferCentre::DelRoleBuffer( unsigned int uParam )
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

bool KUiTargetbufferCentre::DelAllRoleBuffer( void )
{
	//zhangxin
	if(!ms_Singleton || !ms_Singleton->m_pThisWnd) {
		return false;
	}

	KTargetbufferSet::iterator it = ms_Singleton->m_bufferSet.begin();
	while ( it != ms_Singleton->m_bufferSet.end() )
	{
		if ( (*it).second.pBufferObect != NULL )
		{
			ms_Singleton->m_pThisWnd->removeChildWindow( (*it).second.pBufferObect );
			(*it).second.pBufferObect->clear();
		}
		it++;
	}
	ms_Singleton->m_nActiveBufCount = 0;
	ms_Singleton->m_bufferSet.clear();
	ms_Singleton->m_pThisWnd->requestRedraw();
	return true;
}

void KUiTargetbufferCentre::InitBufferPosition( void )
{
	//Initialize every buffer.
	m_nMaxBufCountPerRow =  (int)(m_pThisWnd->getWidth( Absolute ) / GAMEOBJECT_WIDTH_MIN);
	m_nMaxBufCount = (int)(m_pThisWnd->getWidth( Absolute ) / GAMEOBJECT_WIDTH_MIN) * (int)(m_pThisWnd->getHeight( Absolute ) / GAMEOBJECT_WIDTH_MIN);
	for ( int nIdx = 0; nIdx < m_nMaxBufCount; ++nIdx )
	{
		char szBufName[COMMON_CLIENT_MSG_LEN_16];
		sprintf( szBufName, "targetbuffer_%d", nIdx );
		TLGameObject* pGO = static_cast<TLGameObject*>(m_pWindowManager->createWindow( "TaharezLook/GameObject", szBufName ));
		if ( pGO )
		{
			pGO->setStateNotifyFun( &DelBufferByIndex );
		}
	}
}

int KUiTargetbufferCentre::FindBufferActive( unsigned int uBufferID )
{
	KTargetbufferSet::iterator it = m_bufferSet.begin();
	while ( it != m_bufferSet.end() )
	{
		if ( (*it).second.pBufferObect != NULL && (*it).second.pBufferObect->getBuffID() == uBufferID )
		{
			return (*it).first;
		}
		it++;
	}
	return -1;
}

void KUiTargetbufferCentre::CoverBuffer( const KBufferSyncInfo* buffInfo )
{
	int nPos = FindBufferActive( buffInfo->nBuffID );
	if ( nPos == -1 )
	{
		ParallelBuffer( buffInfo );
	}
	else
	{
		KUiTargetbufferCentre::DelRoleBuffer( buffInfo->nBuffID );
		ParallelBuffer( buffInfo );
	}
}
void KUiTargetbufferCentre::WrapBuffer( const KBufferSyncInfo* buffInfo )
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
void KUiTargetbufferCentre::ParallelBuffer( const KBufferSyncInfo* buffInfo )
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
			sprintf( szName, "targetbuffer_%d", m_nActiveBufCount );
			try
			{
				m_bufferSet[nIdx].pBufferObect = static_cast<TLGameObject*>(m_pWindowManager->getWindow(szName));
			}
			catch (...)
			{
				m_bufferSet[nIdx].pBufferObect = NULL;	
			}
			
			if ( m_bufferSet[nIdx].pBufferObect )
			{
				TLGameObject::GameObject GO;

				GO.d_gameobject = AnsiToUtf8( tagBufInfo.szImage );
				GO.d_count		= buffInfo->nPileCount;
				GO.d_disabledTime = 0;
				GO.d_coolingTime = 0;
				GO.d_intonateTime = buffInfo->nTime;
				GO.d_bufferIdx		= nIdx;
				GO.d_bufferID		= buffInfo->nBuffID;
				GO.d_type = TLGameObject::minibuffer;
				m_pThisWnd->addChildWindow( m_bufferSet[nIdx].pBufferObect );
				m_bufferSet[nIdx].pBufferObect->setObject( GO );
				m_bufferSet[nIdx].pBufferObect->setTooltipText( AnsiToUtf8(tagBufInfo.nDesc));
				//m_pThisWnd->addChildWindow( m_bufferSet[nIdx].pBufferObect );
				/*if ( nIdx < m_nMaxBufCountPerRow )
				{
					m_bufferSet[nIdx].pBufferObect->setPosition( Absolute, Point(((nIdx + 1) * GAMEOBJECT_WIDTH_MIN),0) );
				}
				else
				{
					m_bufferSet[nIdx].pBufferObect->setPosition( Absolute, Point((((nIdx + 1)  - m_nMaxBufCountPerRow)*GAMEOBJECT_WIDTH_MIN),GAMEOBJECT_WIDTH_MIN) );
				}*/
				if ( nIdx < m_nMaxBufCountPerRow )
				{
					m_bufferSet[nIdx].pBufferObect->setPosition( Absolute, Point(((nIdx) * GAMEOBJECT_WIDTH_MIN ),0) );
				}
				else
				{
					m_bufferSet[nIdx].pBufferObect->setPosition( Absolute, Point((((nIdx + 1)  - m_nMaxBufCountPerRow)*GAMEOBJECT_WIDTH_MIN),GAMEOBJECT_WIDTH_MIN) );
				}

				
				m_bufferSet[nIdx].pBufferObect->show();
				++ms_Singleton->m_nActiveBufCount;
			}
			return;
		}
	}
}

bool KUiTargetbufferCentre::AddBufferReq( int nBufferID )
{
	//	g_pCoreShell->OperationRequest( GOI_OPEN_BUFFER, nBufferID, NULL );
	return true;
}

bool KUiTargetbufferCentre::DelBufferReq( int nBufferID )
{
	g_pCoreShell->OperationRequest( GOI_DEL_BUFFER, nBufferID, NULL );
	return true;
}

void KUiTargetbufferCentre::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->InitBufferPosition();
	}
}
