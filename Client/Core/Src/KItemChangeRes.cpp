//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemChangeRes.h
// Date:	2002.12
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KEngine.h"
#include "KItemChangeRes.h"
#include "KItem.h"
#include "CoreUseNameDef.h"

//////////////////////////////////////////////////////////////////////////
// 全局
KItemChangeRes	g_ItemChangeRes;
#ifndef _SERVER
KItemPalToHue g_ItemPalToHue;
#endif
//////////////////////////////////////////////////////////////////////////
// 路径
#define ITEMRES_PATH "\\settings\\item"
//////////////////////////////////////////////////////////////////////////
// 资源参照表
static const char* ItemResFileNames[] = 
{
	"HelmRes.txt",
	"ArmorRes.txt",
	"MeleeRes.txt",	
	"ShoulderRes.txt",
	"BootRes.txt",
	"CuffRes.txt",
	"HorseRes.txt",
	"PendantRes.txt",
};

#ifndef _SERVER
static const char* ItemHueFileNames[] = 
{
	"HelmPal",
	"ArmorPal",
	"MeleePal",
	"ShoulderPal",
	"BootPal",
	"CuffPal",
	"HorsePal",
	"PendantPal",
};
#endif


//////////////////////////////////////////////////////////////////////////
// 构造
KItemChangeRes::KItemChangeRes()
{
	memset(m_ItemChangeRes, 0, sizeof(m_ItemChangeRes));
}
//////////////////////////////////////////////////////////////////////////
// 析构
KItemChangeRes::~KItemChangeRes()
{
	for(unsigned int i = BODY_PART_MAX ; i-- ; )
	{
		if (NULL != m_ItemChangeRes[i].pData) 
		{
			delete[] m_ItemChangeRes[i].pData; // 释放内存
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
BOOL KItemChangeRes::Init()
{
	KTabFile aTabFile;
    char    szTABFileName[FILE_NAME_LENGTH] = { 0 };
	const int fileNameArrayLength = sizeof( ItemResFileNames ) / sizeof ( char* );
	
	for(unsigned int i = BODY_PART_MAX ; i-- ; )
	{
		if ( i >= fileNameArrayLength )
			return FALSE;
		if ( NULL == ItemResFileNames || NULL == ItemResFileNames[ i ] )
			return FALSE;

		//sprintf(szTABFileName, ITEMRES_PATH"\\%03d\\%s", nVersion,ItemResFileNames[i]);
		snprintf( szTABFileName, FILE_NAME_LENGTH, ITEMRES_PATH"\\res\\%s", ItemResFileNames[i] );
		szTABFileName[ FILE_NAME_LENGTH - 1 ] = 0;
		if (aTabFile.Load(szTABFileName)) 
		{
			_ASSERT(aTabFile.GetWidth() > 1);
			const int nRowCount = aTabFile.GetHeight() - 1;
			_ASSERT(nRowCount > 0);
			if (m_ItemChangeRes[i].pData = new ItemChangResData::ResData[nRowCount]) // 这里就是 = 不是写错的
			{
				for (unsigned int h = 0 ; h < nRowCount ; h++ ) // 去掉表头，开始一个一个读
				{
					ItemChangResData::ResData& Data = m_ItemChangeRes[i].pData[h];
					const unsigned int nRow = h + 2;
					aTabFile.GetInteger(nRow, 2, 0 , (int*)&(Data.nResIdx));
					aTabFile.GetInteger(nRow, 3, Default_PalIndex, (int*)&(Data.nPalIdx));
					Data.nResIdx -=2;					
				}
				m_ItemChangeRes[i].nCount = nRowCount;
			}
		}
	}
	return TRUE;
}

int KItemChangeRes::GetPal( int partIndex, int nParticular, int nLevel )
{
	if ( partIndex >= BODY_PART_MAX || partIndex < 0 )
		return Default_PalIndex;
	if ( m_ItemChangeRes[ partIndex ].nCount <= 0 )
		return Default_PalIndex;

	ItemChangResData& curRes = m_ItemChangeRes[ partIndex ];
	int resultPal = curRes.pData[ 0 ].nPalIdx;
	if ( 0 != nLevel )
	{
		const int nRow = nParticular * 10 + nLevel;
		resultPal = curRes.nCount > nRow ? curRes.pData[ nRow ].nPalIdx : Default_PalIndex;
	}

	return resultPal;
}
//////////////////////////////////////////////////////////////////////////
//
#ifndef _SERVER
BOOL KItemPalToHue::Init(void)
{
	KTabFile aTabFile;
    char    szTABFileName[FILE_NAME_LENGTH];
	const int fileNameArrayLength = sizeof( ItemHueFileNames ) / sizeof ( char* );
	
	for(unsigned int n = MAX_ROLE * 2 ; n-- ; )
	{
		for(unsigned int i = BODY_PART_MAX ; i-- ; )
		{
			if ( i >= fileNameArrayLength )
				return FALSE;
			if ( NULL == ItemHueFileNames || NULL == ItemHueFileNames[ i ] )
				return FALSE;

			snprintf( szTABFileName, FILE_NAME_LENGTH, ITEMRES_PATH"\\pal\\%s_%d.txt", ItemHueFileNames[i], n );
			szTABFileName[ FILE_NAME_LENGTH - 1 ] = 0;
			if (aTabFile.Load(szTABFileName)) 
			{
				_ASSERT(aTabFile.GetWidth() > 1);
				const int nRowCount = aTabFile.GetHeight() - 1;
				_ASSERT(nRowCount > 0);
				if (m_ItemHueData[n][i].pData = new ItemHueData::HueData[nRowCount]) // 这里就是 = 不是写错的
				{
					for (unsigned int h = 0 ; h < nRowCount ; h++ ) // 去掉表头，开始一个一个读
					{
						ItemHueData::HueData& Data = m_ItemHueData[n][i].pData[h];
						Data.uHue[0] = 0;
						const unsigned int nRow = h + 2;
						for(unsigned int n = 1 ; n < MAX_HUE_PAL ; n++)
						{
							aTabFile.GetInteger(nRow, n + 1, 0 , (int*)&(Data.uHue[n]));
						}
					}
					m_ItemHueData[n][i].uCount = nRowCount;
				}
			}
		}
	}
	return TRUE;
}
#endif
