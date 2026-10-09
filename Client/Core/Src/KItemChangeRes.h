//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemChangeRes.h
// Date:	2002.12
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------

#ifndef	KItemChangeResH
#define	KItemChangeResH

#include "KTabFile.h"
// lixuewu 2004.03.10 大手术只保持了接口基本不变,后边就没有打修改标记


#include "KNpcResNode.h"

const int Default_PalIndex = 0;

#ifndef _SERVER
#define MAX_HUE_PAL 9

class KItemPalToHue
{
private:
	typedef struct ItemHueData 
	{
		unsigned int uCount;
		struct  HueData{
			unsigned int uHue[MAX_HUE_PAL];
		} *pData;
	};	
public:
	BOOL Init(void);

	unsigned int GetHue( int partIndex, unsigned int nRole, unsigned int nRes, unsigned int nPal ) const
	{
		if ( partIndex >= BODY_PART_MAX || partIndex < 0 )
			return 0;

		if (nRole < (MAX_ROLE * 2) && nRes < m_ItemHueData[nRole][partIndex].uCount && nPal < MAX_HUE_PAL)
		{
			return m_ItemHueData[nRole][partIndex].pData[nRes].uHue[nPal];
		}
		return 0;
	}

	KItemPalToHue()
	{
		memset(m_ItemHueData, 0, sizeof(m_ItemHueData));
	}
	~KItemPalToHue()
	{
		for(unsigned int n = MAX_ROLE * 2 ; n-- ; )
		{
			for(unsigned int i = BODY_PART_MAX ; i-- ; )
			{
				if (NULL != m_ItemHueData[n][i].pData) 
				{
					delete[] m_ItemHueData[n][i].pData; // 释放内存
				}
				m_ItemHueData[n][i].pData = NULL;
				m_ItemHueData[n][i].uCount = 0;
			}
		}	
	}

private:
	ItemHueData m_ItemHueData[MAX_ROLE * 2][BODY_PART_MAX];
};

extern KItemPalToHue g_ItemPalToHue;
#endif


class KItemChangeRes
{
public:
	BOOL Init();
	int GetPal( int partIndex, int nParticular, int nLevel );

	KItemChangeRes();
	~KItemChangeRes();
private:
	typedef struct ItemChangResData 
	{
		unsigned int nCount;
		struct ResData {
			unsigned int nResIdx;
			unsigned int nPalIdx;
		}*pData;
	};
	ItemChangResData m_ItemChangeRes[BODY_PART_MAX];
};

extern KItemChangeRes g_ItemChangeRes;
#endif
