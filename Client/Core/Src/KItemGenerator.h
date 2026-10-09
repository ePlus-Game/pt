//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemGenerator.h
// Date:	2002.08.26
// Code:	DongBo
// Desc:    header file. 本文件定义的类用于生成道具
//---------------------------------------------------------------------------

#ifndef	KItemGeneratorH
#define	KItemGeneratorH

#include "KBasPropTbl.h"
#include "KItem.h"

#include <vector>

#define		IN
#define		OUT

class KItemGenerator
{
	friend class KItem;
public:
	KItemGenerator();
	~KItemGenerator();

protected:
	KLibOfBPT	m_BPTLib;

// 	int			m_EquipNumOfEntries[equip_detailnum];
// 	int			m_MedNumOfEntries;

public:
	BOOL Init( void );

	BOOL Gen_Item(
		IN int nGenre,
		IN int nDetailType,
		IN int nParticularType,
		IN int nLevel,
		IN int nItemCount,
		IN OUT KItem* pItem );
	
	KLibOfBPT* GetLibOfBPT()
	{
		return &m_BPTLib;
	}

	const KBASICPROP_ITEM* GetItemTemplate(
		IN int nGenre,
		IN int nDetailType,
		IN int nParticularType,
		IN int nLevel
		);
	const KBASICPROP_ITEM* GetItemTemplate( 
		const char* name );

};

extern KItemGenerator	g_ItemGen;			//	装备生成器

#endif
