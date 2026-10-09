/********************************************************************
	created:	2004/10/14
	file base:	PackagerEx
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/

#if !defined(AFX_PACKAGEREX_H__8D4B3741_9242_4A56_AFA5_A05D0B54EDE2__INCLUDED_)
#define AFX_PACKAGEREX_H__8D4B3741_9242_4A56_AFA5_A05D0B54EDE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WINDOWS_
	#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#endif

/*
 * Identifier was truncated to '255' characters 
 * in the debug information
 */
#pragma warning(disable : 4786)

#include "CriticalSection.h"
#include "Buffer.h"

#include "Utils.h"
#include "Macro.h"

#include <map>

/*
 * Nonstandard extension used : zero-sized array in struct/union
 */
#pragma warning(disable: 4200)

/*
 * namespace OnlineGameLib::Win32
 */

namespace OnlineGameLib {
namespace Win32 {

/*
 * CPackagerEx class
 */
template < class T > class CPackagerEx  
{
public:

	CPackagerEx( size_t bufferSize = 65536 /* 1024*64 */, size_t maxFreeBuffers = 16 );
	virtual ~CPackagerEx();

	/*
	 * Send
	 */
	CBuffer *GetHeadPack( T cID, size_t packLength = 512 );
	CBuffer *GetNextPack( T cID );
	
	void AddData( T cID, const char * const pData, size_t dataLength, unsigned long lnUserData = 0 );
	void AddData( T cID, const BYTE * const pData, size_t dataLength, unsigned long lnUserData = 0 );

	void DelData( T cID );

	/*
	 * Recv
	 */
	CBuffer *PackUp( const void *pData, size_t dataLength );

	/*
	 * Common
	 */
	bool ReSet( size_t bufferSize, size_t maxFreeBuffers ) { return m_theAllocator.ReSet( bufferSize, maxFreeBuffers ); }

	void Empty();

	static const void * Peek( const void *pData, size_t index = 0 ) { 
		if ( !pData )
		{ 
			return NULL; 
		} 
		return ( const void * )( ( const char * )pData + index ); 
	}
	
private:

	CCriticalSection	m_csSend;
	CCriticalSection	m_csRecv;

	CBuffer::Allocator	m_theAllocator;

	typedef std::map< T, CBuffer * >	BUFFER_MAP;

	BUFFER_MAP			m_theSend;
	BUFFER_MAP			m_theRecv;

	/*
	 * +-----+-----+-----+-----+
	 * | 8 7 | 6 5 | 4 3 | 2 1 |  <==> enumPackFlag, sizeof( BYTE )
	 * +-----+-----+-----+-----+
	 *    A     B     C     D
	*/
	enum enumPackFlag
	{
		enumPackHeader	= 0xc0,	// A segment
		enumPackMiddle	= 0x30, // B segment
		enumPackTail	= 0xc,	// C segment
		enumPackErr		= 0x3	// D segment
	};

	bool _Header( BYTE cFlag )	{ return ToBool<int>( cFlag & enumPackHeader ); }
	bool _Middle( BYTE cFlag )	{ return ToBool<int>( cFlag & enumPackMiddle ); }
	bool _Tail( BYTE cFlag )	{ return ToBool<int>( cFlag & enumPackTail ); }
	bool _Error( BYTE cFlag )	{ return ToBool<int>( cFlag & enumPackErr ); }

};

template < class T >
CPackagerEx<T>::CPackagerEx( size_t bufferSize /* = 65536  */, size_t maxFreeBuffers /* = 16  */ )
		: m_theAllocator( bufferSize, maxFreeBuffers )
{

}

template < class T >
CPackagerEx<T>::~CPackagerEx()
{
	Empty();
}

template < class T >
void CPackagerEx<T>::AddData( T cID, const char * const pData, size_t dataLength, unsigned long lnUserData /* = 0 */ )
{
	CCriticalSection::Owner lock( m_csSend );

	BUFFER_MAP::iterator it;

	if ( m_theSend.end() != ( it = m_theSend.find( cID ) ) )
	{
		CBuffer *pBuffer = m_theSend[cID];

		ASSERT( pBuffer );

		pBuffer->SetUserData( lnUserData );

		pBuffer->AddData( pData, dataLength );
	}
	else
	{
		CBuffer *pBuffer = m_theAllocator.Allocate();

		ASSERT( pBuffer );

		pBuffer->SetUserData( lnUserData );
		
		pBuffer->AddData( pData, dataLength );
		
		m_theSend[cID] = pBuffer;
	}
}

template < class T >
void CPackagerEx<T>::AddData( T cID, const BYTE * const pData, size_t dataLength, unsigned long lnUserData /* = 0 */ )
{
	AddData( cID, reinterpret_cast< const char * >( pData ), dataLength, lnUserData );
}

template < class T >
void CPackagerEx<T>::DelData( T cID )
{
	CCriticalSection::Owner lock( m_csSend );

	BUFFER_MAP::iterator it;

	if ( m_theSend.end() != ( it = m_theSend.find( cID ) ) )
	{
		CBuffer *pBuffer = m_theSend[cID];

		SAFE_RELEASE( pBuffer );

		m_theSend.erase( cID );
	}
}

template < class T >
CBuffer *CPackagerEx<T>::GetHeadPack( T cID, size_t packLength /* = 512 */ )
{
	CCriticalSection::Owner lock( m_csSend );

	BUFFER_MAP::iterator it;

	if ( m_theSend.end() != ( it = m_theSend.find( cID ) ) )
	{
		CBuffer *pBuffer = m_theSend[cID];

		int nTypeSize = sizeof( T );
		CBuffer *pPack = pBuffer->GetHeadPack( packLength - 1 - nTypeSize /* enumPackHeader + cID */);

		if ( NULL == pPack )
		{
			return NULL;
		}

		if ( 0 == pPack->GetUsed() )
		{
			pPack->Release();

			return NULL;
		}

		CBuffer *pNewBufer = m_theAllocator.Allocate();

		/*
		 * Add a prototype
		 */
		BYTE *pTypeContent = ( BYTE * )&cID;
		pNewBufer->AddData( pTypeContent, nTypeSize );

		/*
		 * Add a flag
		 */
		BYTE cFlag = enumPackHeader;

		if ( !pBuffer->HaveNextPack() )
		{
			cFlag |= enumPackTail;
		}
		
		pNewBufer->AddData( cFlag );

		/*
		 * Add a user data into this buffer
		 */
		unsigned long lnUserData = pBuffer->GetUserData();

		pNewBufer->AddData( ( const char * )( &lnUserData ), sizeof( unsigned long ) );

		/*
		 * Add some data into this buffer
		 */
		pNewBufer->AddData( pPack->GetBuffer(), pPack->GetUsed() );

		pPack->Release();

		return pNewBufer;
	}

	return NULL;
}

template < class T >
CBuffer *CPackagerEx<T>::GetNextPack( T cID )
{
	CCriticalSection::Owner lock( m_csSend );

	BUFFER_MAP::iterator it;

	if ( m_theSend.end() != ( it = m_theSend.find( cID ) ) )
	{
		CBuffer *pBuffer = m_theSend[cID];

		CBuffer *pPack = pBuffer->GetNextPack();

		if ( NULL == pPack )
		{
			return NULL;
		}

		if ( 0 == pPack->GetUsed() )
		{
			pPack->Release();

			return NULL;
		}

		CBuffer *pNewBufer = m_theAllocator.Allocate();

		int nTypeSize = sizeof( T );
		BYTE *pTypeContent = ( BYTE * )&cID;
		pNewBufer->AddData( pTypeContent, nTypeSize );

		BYTE cFlag = enumPackMiddle;

		if ( !pBuffer->HaveNextPack() )
		{
			cFlag |= enumPackTail;
		}

		pNewBufer->AddData( cFlag );

		/*
		 * Add a user data into this buffer
		 */
		unsigned long lnUserData = pBuffer->GetUserData();

		pNewBufer->AddData( ( const char * )( &lnUserData ), sizeof( unsigned long ) );

		/*
		 * Add some data into this buffer
		 */
		pNewBufer->AddData( pPack->GetBuffer(), pPack->GetUsed() );

		pPack->Release();

		return pNewBufer;
	}

	return NULL;	
}

template < class T >
CBuffer *CPackagerEx<T>::PackUp( const void *pData, size_t dataLength )
{
	CCriticalSection::Owner lock( m_csRecv );

	if ( NULL == pData || dataLength < 3 /* cID + cPackFlag + ... */ )
	{
		return NULL;
	}

	T cID = *( const T * )( CPackagerEx::Peek( pData ) );

	BUFFER_MAP::iterator it;

	if ( m_theRecv.end() == ( it = m_theRecv.find( cID ) ) )
	{
		CBuffer *pPack = m_theAllocator.Allocate();

		m_theRecv[cID] = pPack;
	}
	
	CBuffer *pBuffer = m_theRecv[cID];
	
	ASSERT( pBuffer );
	
	int ncIDLen = sizeof( T );
	BYTE cFlag = *( const BYTE * )( CPackagerEx::Peek( pData, ncIDLen ) );

	const size_t nDataBegin = ncIDLen + 1 + sizeof( unsigned long );
	
	if ( _Header( cFlag ) )
	{
		pBuffer->Empty();
		
		pBuffer->AddData( ( ( const char * )pData + nDataBegin ), dataLength - nDataBegin );
	}
	
	if ( _Middle( cFlag ) )
	{
		pBuffer->AddData( ( ( const char * )pData + nDataBegin ), dataLength - nDataBegin );
	}
	
	if ( _Tail( cFlag ) )
	{
		unsigned long lnUserData = *( const unsigned long * )( ( const char * )pData + 2 );

		pBuffer->SetUserData( lnUserData );

		pBuffer->AddRef();
		
		return pBuffer;
	}
	
	if ( _Error( cFlag ) )
	{
		ASSERT( NULL && "CPackagerEx::PackUp - Invalid package!" );
	}

	return NULL;	
}

template < class T >
void CPackagerEx<T>::Empty()
{
	/*
	 * Send
	 */
	{
		CCriticalSection::Owner lock( m_csSend );

		BUFFER_MAP::iterator it;

		for ( it = m_theSend.begin(); it != m_theSend.end(); it ++ )
		{
			CBuffer *pBuffer = (*it).second;

			SAFE_RELEASE( pBuffer );
		}

		m_theSend.erase( m_theSend.begin(), m_theSend.end() );
	}

	/*
	 * Recv
	 */
	{
		CCriticalSection::Owner lock( m_csRecv );

		BUFFER_MAP::iterator it;

		for ( it = m_theRecv.begin(); it != m_theRecv.end(); it ++ )
		{
			CBuffer *pBuffer = (*it).second;

			SAFE_RELEASE( pBuffer );
		}

		m_theRecv.erase( m_theRecv.begin(), m_theRecv.end() );
	}
}

} // End of namespace OnlineGameLib
} // End of namespace Win32

#endif // !defined(AFX_PACKAGEREX_H__8D4B3741_9242_4A56_AFA5_A05D0B54EDE2__INCLUDED_)
