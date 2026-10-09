//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:30   15:46
//      File_base        : SocialAllocator
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "SocialAllocator.h"
#include "SocialUnit.h"
#include <new>

#define		SOCIAL_ALLOCATE_GRANULARITY		512
#define		MAXCOUNT_SOCIAL_UNIT			(1024 * SOCIAL_ALLOCATE_GRANULARITY)
#define		MAXCOUNT_SOCIAL_ATTR			MAXCOUNT_SOCIAL_UNIT

template<class ALLOCTYPE, int GRANULARITY, int MAXCOUNT>
class SocialUnitAllocator
{
public:
	ALLOCTYPE*	Alloc(int nParam1, int nParam2)
	{
		void *ptr = m_allocator._alloc();

		if(NULL == ptr)
			return NULL;

		new (ptr) ALLOCTYPE(nParam1, nParam2);

		return (ALLOCTYPE*)ptr;
	}

	void	Free(ALLOCTYPE	*ptr)
	{
		if(ptr)
		{
			ptr->~ALLOCTYPE();
			m_allocator._free(ptr);
		}
	}

private:
	__simpleallocator<sizeof(ALLOCTYPE), GRANULARITY, MAXCOUNT>	m_allocator;
};

class UnitAttrAllocator
{
	enum
	{
		// sizeof(SOCIALUNIT_ATTR) - 1 = 3
		// sizeof(SOCIALUNIT_ATTR) - 1 + nDataSize
		BUF_SIZE_1 = 7,		// 3 + 4,
		BUF_SIZE_2 = 11,	// 3 + 8,
		BUF_SIZE_3 = 20,	// 3 + 17,
		BUF_SIZE_4 = 36,	// 3 + 33,
		BUF_SIZE_5 = 87,	// 3 + 84
		BUF_SIZE_6 = 516,	// 3 + 513,
	};

public:
	void*	Alloc(int nSize);
	void	Free(void *ptr, int nSize);

private:
	__simpleallocator<BUF_SIZE_1, SOCIAL_ALLOCATE_GRANULARITY, MAXCOUNT_SOCIAL_ATTR> m_alloc1;
	__simpleallocator<BUF_SIZE_2, SOCIAL_ALLOCATE_GRANULARITY, MAXCOUNT_SOCIAL_ATTR> m_alloc2;
	__simpleallocator<BUF_SIZE_3, SOCIAL_ALLOCATE_GRANULARITY, MAXCOUNT_SOCIAL_ATTR> m_alloc3;
	__simpleallocator<BUF_SIZE_4, SOCIAL_ALLOCATE_GRANULARITY, MAXCOUNT_SOCIAL_ATTR> m_alloc4;
	__simpleallocator<BUF_SIZE_5, SOCIAL_ALLOCATE_GRANULARITY, MAXCOUNT_SOCIAL_ATTR> m_alloc5;
	__simpleallocator<BUF_SIZE_6, SOCIAL_ALLOCATE_GRANULARITY, MAXCOUNT_SOCIAL_ATTR> m_alloc6;
};

void* UnitAttrAllocator::Alloc(int nSize)
{
	if(nSize <= 0)
		return NULL;

	if(nSize == BUF_SIZE_3)
	{
		return m_alloc3._alloc();
	}
	else if(nSize < BUF_SIZE_3)
	{
		if(nSize <= BUF_SIZE_1)
			return m_alloc1._alloc();
		else if(nSize <= BUF_SIZE_2)
			return m_alloc2._alloc();
		else
			return m_alloc3._alloc();
	}
	else
	{
		if(nSize <= BUF_SIZE_4)
			return m_alloc4._alloc();
		else if(nSize <= BUF_SIZE_5)
			return m_alloc5._alloc();
		else if (nSize <= BUF_SIZE_6)
			return m_alloc6._alloc();
		else
			return NULL;
	}
}

void UnitAttrAllocator::Free(void *ptr, int nSize)
{
	if(NULL == ptr)	
		return;

	if(nSize == BUF_SIZE_3)		
	{
		m_alloc3._free(ptr);
	}
	else if(nSize < BUF_SIZE_3)
	{
		if(nSize <= BUF_SIZE_1)
			m_alloc1._free(ptr);
		else if(nSize <= BUF_SIZE_2)
			m_alloc2._free(ptr);
		else
			m_alloc3._free(ptr);
	}
	else
	{
		if(nSize <= BUF_SIZE_4)
			m_alloc4._free(ptr);
		else if(nSize <= BUF_SIZE_5)
			m_alloc5._free(ptr);
		else if (nSize <= BUF_SIZE_6)
			m_alloc6._free(ptr);
		else
			_ASSERT(false);
	}
}

UnitAttrAllocator	g_AttrAllocator;
SocialUnitAllocator<SocialUnit, 
					SOCIAL_ALLOCATE_GRANULARITY, 
					MAXCOUNT_SOCIAL_UNIT>	g_UnitAllocator;

SocialUnit*	SocialAllocator::AllocUnit(int nTplId, int nLayer)
{
	return g_UnitAllocator.Alloc(nTplId, nLayer);	
}

void SocialAllocator::FreeUnit(SocialUnit *pUnit)
{
	g_UnitAllocator.Free(pUnit);
}

void* SocialAllocator::AllocBuf(int nSize)
{
	return g_AttrAllocator.Alloc(nSize);
}

void SocialAllocator::FreeBuf(void *ptr, int nSize)
{
	g_AttrAllocator.Free(ptr, nSize);
}