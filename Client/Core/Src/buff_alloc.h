//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_alloc
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _BUFF_ALLOC_H_
#define _BUFF_ALLOC_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Header Include
//
/////////////////////////////////////////////////////////////////////////////

#include "buff_def.h"
#include <new>
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////
#define MY_ALIGN(A,L)	(((A) + (L) - 1) & ~((L) - 1))
#define ALIGN_SIZE(A)	MY_ALIGN((A),sizeof(double))

template< class _alloctype, int GRANULARITY, int MAXCOUNT >
class __allocator
{
	typedef struct _allocitem 
	{
		union
		{
			_allocitem* next;
			unsigned char obj[ALIGN_SIZE(sizeof( _alloctype ))];
		};
	}ALLOCITEM,*PALLOCITEM;

	enum{ __alloc_slot = MAXCOUNT/GRANULARITY };

public:

	__allocator( )
	{
		memset( 
			&this->m_DataBlock, 
			0,
			sizeof(void*) * __alloc_slot );

		this->m_pFree = NULL;

		this->m_nCurSlot = 0;
	}

	~__allocator( )
	{
		_freeslot( );
	}

	_alloctype* _alloc(  )
	{
		PALLOCITEM ptr = NULL;

		if( this->m_pFree == NULL &&
			!_refill( ) )
			return NULL;

		ptr = this->m_pFree;
		m_pFree = this->m_pFree->next;

		_alloctype* obj = (_alloctype*)&ptr->obj;

		new ( obj ) _alloctype;

		return obj;
	}

	void _free( _alloctype* ptr )
	{

		if( ptr == NULL )
			return;

		ptr->~_alloctype( );
		
		PALLOCITEM fptr = (PALLOCITEM)ptr;
		fptr->next = this->m_pFree;
		this->m_pFree = fptr;

	}

private:

	bool _refill( )
	{
		if( this->m_nCurSlot >= __alloc_slot )
			return false;

		void* pAlloc = 
		malloc( sizeof( ALLOCITEM ) * GRANULARITY );

		if( pAlloc )
		{
			this->m_DataBlock[m_nCurSlot] = pAlloc;
			this->m_nCurSlot++;

			PALLOCITEM pAItem = (PALLOCITEM)pAlloc;

			for( int nLoopCount = 0; nLoopCount < GRANULARITY; nLoopCount++ )
			{
				pAItem->next = this->m_pFree;
				this->m_pFree = pAItem;
				pAItem++;
			}
			return true;
		}
		else
			return false;
	}

	void _freeslot( )
	{
		for( int nLoopCount = 0; nLoopCount <= this->m_nCurSlot; nLoopCount++ )
		{
			if( this->m_DataBlock[nLoopCount] )
			{
				free( this->m_DataBlock[nLoopCount] );
				this->m_DataBlock[nLoopCount] = NULL;
			}
		}

		this->m_nCurSlot = 0;
	}

private:
	
	PALLOCITEM	m_pFree;
	int			m_nCurSlot;
	void*		m_DataBlock[__alloc_slot];
};

//////////////////////////////////////////////////////////////////////////////

template< class _savetype, int MAXCOUNT >
class __nestedlist
{
	typedef struct _bucket
	{
		_bucket*	next;
		_savetype	obj;
	};

public:

	__nestedlist( )
	{
		clear( );
	}
	~__nestedlist( ){}

	void clear( )
	{
		m_header	=	NULL;
		m_free		=	NULL;
		m_nested	=	NULL;
		m_nestedc	=	0;
		
		memset( m_bucket, 0, sizeof(m_bucket) );
		
		for( int nLoopCount = 0; nLoopCount < MAXCOUNT; nLoopCount++ )
		{
			m_bucket[nLoopCount].next = m_free;
			m_free	=	&m_bucket[nLoopCount];
		}
	}

	int	add( _savetype& obj )
	{
		if( m_free )
		{
			_bucket* freebucket = m_free;
			m_free = m_free->next;

			freebucket->obj = obj;

			//support add node nested count unlimited
			if( m_nestedc )
			{
				freebucket->next = m_nested;
				m_nested = freebucket;
			}
			else
			{
				freebucket->next = m_header;
				m_header = freebucket;
			}

			return TRUE;
		}
		else
			return FALSE;
	}

	template< class FUN >
		int foreach( FUN fun, BUFF_ENV_PARAM& Env )
	{
		_bucket*	prev;
		_bucket*	cur;

		prev= m_header;
		cur = m_header;

		m_nestedc++;

		while( cur )
		{
			int nret = fun( cur->obj, Env );

			if( nret &  buff_ret_keep )
			{
				prev = cur;
				cur	= prev->next;
			}
			else
			{
				if( nret & buff_ret_del )
				{
					//support delete node nested count 1
					if( m_nestedc <= 1 )
					{
						if( cur == prev )
						{
							//delete header
							m_header = cur->next;
							prev = m_header;
							
							//free
							cur->next = m_free;
							m_free = cur;
							memset( &m_free->obj, 0, sizeof(m_free->obj) );
							
							cur	= prev;
						}
						else
						{
							prev->next = cur->next;
							
							//free
							cur->next = m_free;
							m_free = cur;
							memset( &m_free->obj, 0, sizeof(m_free->obj) );
							//
							
							cur = prev->next;
						}
					}
				}

				if( nret & buff_ret_break )
					break;
			}
		}

		m_nestedc--;

		if( m_nested )
		{
			prev->next = m_nested;
			m_nested = NULL;
		}

		return TRUE;
	}

private:
	int			m_nestedc;
	_bucket*	m_nested;
	_bucket*	m_header;
	_bucket*	m_free;
	_bucket		m_bucket[MAXCOUNT];
};

//////////////////////////////////////////////////////////////////////////////

#endif