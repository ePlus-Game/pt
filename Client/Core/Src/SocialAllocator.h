//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:29   17:50
//      File_base        : SocialAllocator
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : Copyed and Changed from zolazuo(zuolizhi)'s code.
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#ifndef _SocialAllocator_h
#define	_SocialAllocator_h

template<int ITEMSIZE, int GRANULARITY, int MAXCOUNT>
class __simpleallocator
{
	typedef struct _allocitem 
	{
		union
		{
			_allocitem* next;
			unsigned char obj[ITEMSIZE];
		};
	}ALLOCITEM,*PALLOCITEM;

	enum{ __alloc_slot = MAXCOUNT/GRANULARITY };

public:

	__simpleallocator( )
	{
		memset(&this->m_DataBlock, 0, sizeof(void*) * __alloc_slot );

		this->m_pFree = NULL;
		this->m_nCurSlot = 0;
	}

	~__simpleallocator( )
	{
		_freeslot( );
	}

	void* _alloc(  )
	{
		PALLOCITEM ptr = NULL;

		if( this->m_pFree == NULL && !_refill( ) )
			return NULL;

		ptr = this->m_pFree;
		m_pFree = this->m_pFree->next;

		return ptr->obj;
	}

	void _free( void* ptr )
	{
		if( ptr == NULL )
			return;

		PALLOCITEM fptr = (PALLOCITEM)ptr;
		fptr->next = this->m_pFree;
		this->m_pFree = fptr;
	}

private:

	bool _refill( )
	{
		if( this->m_nCurSlot >= __alloc_slot )
			return false;

		void* pAlloc = malloc( sizeof( ALLOCITEM ) * GRANULARITY );

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
		for( int nLoopCount = 0; nLoopCount < this->m_nCurSlot; nLoopCount++ )
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

class SocialUnit;

class SocialAllocator
{
public:
	static SocialUnit*	AllocUnit(int nTplId, int nLayer);
	static void			FreeUnit(SocialUnit *pUnit);
	static void*		AllocBuf(int nSize);
	static void			FreeBuf(void *ptr, int nSize);
};

#endif