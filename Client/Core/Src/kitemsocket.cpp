//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 11:41
//      File_base        : KItemSocket
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KItemGenerator.h"
#include "kitemsocket.h"
#include "kiteminlayaddontable.h"

KItemSocket::KItemSocket()
{
	Reset();
}

KItemSocket::~KItemSocket()
{

}

bool KItemSocket::Set( const InlayStuff& stuff )
{
	m_inlaySruff = stuff;
	m_itemTemplate = g_ItemGen.GetItemTemplate( 
								m_inlaySruff.nGenre,
								m_inlaySruff.nDetail,
								m_inlaySruff.nParticular,
								m_inlaySruff.nLevel );
	if ( IsEmpty() )
	{
		Reset();
		return false;
	}
	else
	{
		return true;
	}
}

void KItemSocket::Reset( void )
{
	memset( &m_inlaySruff, 0, sizeof(m_inlaySruff) );
	m_itemTemplate = NULL;
}

bool KItemSocket::IsEmpty( void )
{
	if ( m_itemTemplate == NULL || 
		( m_inlaySruff.nGenre == 0 && 
		m_inlaySruff.nDetail == 0 && 
		m_inlaySruff.nParticular == 0 && 
		m_inlaySruff.nLevel == 0 ) )
	{
		return true;
	}
	else
	{
		return false;
	}	
}

bool KItemSocket::GetInlayStuff( InlayStuff& stuff )
{
	if ( !IsEmpty() )
	{
		stuff = m_inlaySruff;
		return true;
	}
	else
	{
		return false;
	}
}

int KItemSocket::GetInlayGroupID( void )
{
	if ( !IsEmpty() )
	{
		return m_itemTemplate->nGroup;
	}
	else
	{
		return -1;
	}	
}

bool KItemSocket::GetInlayEffect( InlayEffect* effect )
{
	if ( effect && !IsEmpty() )
	{
		InlayEffect* pEffectArray = effect;
		for ( int nIdx = 0; nIdx < ITEM_BUFF_COUNT; ++nIdx )
		{
			pEffectArray[nIdx].nBuffID = m_itemTemplate->BasicBuff.BuffID[nIdx];
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool KItemSocket::GetInlayYaoByBuffID(  short buffID, int* yaoID )
{
	if ( IsEmpty() || yaoID == NULL )
	{
		return false;
	}
	else
	{
		*yaoID = yao_invalid;
		KItemInlayAddOnTable& inlayAddOn = KItemInlayAddOnTable::Singleton();
		int count = inlayAddOn.GetInlayAddOnCount( m_itemTemplate->sYaoGroup );
		for( int i = 0; i < count; i++ )
		{
			const InlayAddOn* AddOn = inlayAddOn.GetInlayAddOnByGroupAndBuffID( m_itemTemplate->sYaoGroup, buffID, 0 );
			if ( AddOn )
			{
				//if ( AddOn->Probability > g_Random( 10000 ) )
				{
					if ( g_RandPercent( AddOn->YangRate ) )
					{
						*yaoID = yao_yang;
					}
					else if ( g_RandPercent( AddOn->YinRate ) )
					{
						*yaoID = yao_yin;
					}
					else
					{
						*yaoID = yao_invalid;
					}
					break;
				}
			}
		}
		return true;
	}
}
