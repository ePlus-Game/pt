//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 14:48
//      File_base        : KItemSocketSet
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "kitemsocketset.h"
#include "kiteminlayrule.h"
#include "KItem.h"

bool KItemSocketSet::CreateSocket( void )
{
	m_itemSocketSet.resize(	m_itemSocketSet.size() + 1 );
	return true;
}

void KItemSocketSet::DestroySocket( void )
{
	m_itemSocketSet.clear();
}

int KItemSocketSet::GetSocketCount( void )
{
	return m_itemSocketSet.size();
}

int KItemSocketSet::GetUseSocketCount( void )
{
	int count = 0;

	_ItemSocketSet::iterator it = m_itemSocketSet.begin();

	while ( it != m_itemSocketSet.end() )
	{
		if ( !it->IsEmpty() )
		{
			count++;
		}
		++it;
	}
	return count;
}

bool KItemSocketSet::SetInlayStuffBySocketIdx( int socketIdx, const InlayStuff& stuff )
{
	if ( IsValidSocketIndex( socketIdx ) )
	{
		KItemSocket itemSocket;
		if ( itemSocket.Set( stuff ) )
		{
			m_itemSocketSet[socketIdx] = itemSocket;
			return true;
		} 
		else
		{
			return false;
		}
	}
	else
	{	
		return false;
	}
}

bool KItemSocketSet::GetInlayStuffBySocketIdx( int socketIdx, InlayStuff& stuff )
{
	KItemSocket socket;
	if ( GetSocket( socketIdx, &socket ) )
	{		
		return socket.GetInlayStuff( stuff );
	}
	else
	{
		return false;
	}
}

bool KItemSocketSet::GetInlayEffectBySocketIdx( int socketIdx, InlayEffect* effect )
{
	KItemSocket socket;
	if ( GetSocket( socketIdx, &socket ) )
	{
		return socket.GetInlayEffect( effect );
	}
	else
	{
		return false;
	}
}

bool KItemSocketSet::GetInlayYaoBySocketIdxAndBuffID( int socketIdx, short buffID, int* yaoID )
{
	KItemSocket socket;
	if ( GetSocket( socketIdx, &socket ) )
	{
		return socket.GetInlayYaoByBuffID( buffID, yaoID );
	}
	else
	{
		return false;
	}
}

int	KItemSocketSet::GetInlayGroupIDBySocketIdx( int socketIdx )
{
	KItemSocket socket;
	if ( GetSocket( socketIdx, &socket ) )
	{
		return socket.GetInlayGroupID();
	}
	else
	{
		return false;
	}
}

bool	KItemSocketSet::IsEmptyBySocketIdx( int socketIdx )
{
	KItemSocket socket;
	if ( GetSocket( socketIdx, &socket ) )
	{
		return socket.IsEmpty();
	}
	else
	{
		return false;
	}
}

bool KItemSocketSet::GetInlaySpecialEffect( const KItem* item, InlayEffect* effect )
{
	if ( effect == NULL && 
		( GetUseSocketCount() == 0 || 
		GetUseSocketCount() == 0) )
	{
		return false;
	}
	else
	{
		KItemInlayRule& iir = KItemInlayRule::Singleton();
		std::string strKey;
		int nIdx = 0;
		for (nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
		{
			if ( nIdx < GetUseSocketCount() )
			{
				int groupID = m_itemSocketSet[nIdx].GetInlayGroupID();
				char szItemID[256];
				sprintf( szItemID, "%d", groupID );
				strKey += szItemID;
			}
			else if ( nIdx < GetSocketCount() )
			{
				strKey += "-1";
			}
			else
			{
				strKey += "0";
			}
		}
		if ( iir.IsRight( strKey.c_str(), item ) )
		{
			ItemInlayRule inlayRule;
			iir.GetInlayEffect( strKey.c_str(), inlayRule );
			for (nIdx = 0; nIdx < MAX_SPECIALEFFECT_COUNT; ++nIdx )
			{
				effect[nIdx].nBuffID = inlayRule.buffIDArray[nIdx];
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool KItemSocketSet::GetInlaySpecialEffectName( const KItem* item, std::string& name )
{
	if ( ( GetUseSocketCount() == 0 || 
		GetUseSocketCount() == 0) )
	{
		return false;
	}
	else
	{
		KItemInlayRule& iir = KItemInlayRule::Singleton();
		std::string strKey;
		int nIdx = 0;
		for (nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
		{
			if ( nIdx < GetUseSocketCount() )
			{
				int groupID = m_itemSocketSet[nIdx].GetInlayGroupID();
				char szItemID[256];
				sprintf( szItemID, "%d", groupID );
				strKey += szItemID;
			}
			else if ( nIdx < GetSocketCount() )
			{
				strKey += "-1";
			}
			else
			{
				strKey += "0";
			}
		}
		if ( iir.IsRight( strKey.c_str(), item ) )
		{
			ItemInlayRule inlayRule;
			iir.GetInlayEffect( strKey.c_str(), inlayRule );
			name = inlayRule.szInfo;
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool KItemSocketSet::IsValidSocketIndex( int socketIdx )
{
	if ( socketIdx < 0 || socketIdx >= m_itemSocketSet.size() )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool KItemSocketSet::GetSocket( int socketIdx, KItemSocket* socket )
{
	if ( IsValidSocketIndex( socketIdx ) )
	{
		*socket = m_itemSocketSet[socketIdx];
		return true;		
	}
	else
	{
		return false;
	}
	
}