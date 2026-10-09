//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KBasPropTbl.CPP
// Date:	2002.08.14
// Code:	DongBo
// Desc:    cpp file. 本文件实现的类用于从tab file中读出道具的初始属性,
//			并生成对应的属性表
//---------------------------------------------------------------------------

#include "KCore.h"
#include "KTabFile.h"
#include "MyAssert.H"
#include "KBasPropTbl.h"
#include "KItem.h"

#ifndef _SERVER
#include "QueryInfo.h"
#endif

#define		TABFILE_MINE				"minebase.txt"
#define		TABFILE_TASK				"questkey.txt"
#define		TABFILE_MEDICINE			"potion.txt"
#define		TABFILE_MEDMATERIAL			"medmaterialbase.txt"
#define		TABFILE_WEAPON				"weapon.txt"
#define		TABFILE_RANGEWEAPON			"rangeweapon.txt"
#define		TABFILE_ARMOR				"armor.txt"
#define		TABFILE_HELM				"helm.txt"
#define		TABFILE_BOOT				"boot.txt"
#define		TABFILE_SHOULDER			"shoulder.txt"
#define		TABFILE_AMULET				"amulet.txt"
#define		TABFILE_RING				"ring.txt"
#define		TABFILE_CUFF				"cuff.txt"
#define		TABFILE_PENDANT				"pendant.txt"
#define		TABFILE_HORSE				"horse.txt"
#define		TABFILE_TOWNPORTAL			"townportal.txt"
#define		TABFILE_MAGICORSCRIPT		"magicscript.txt"	
#define		TABFILE_MATERIALS			"material.txt"
#define		TABFILE_SKILLBOOK			"skillbook.txt"
#define		TABFILE_TARGETITEM			"targetitem.txt"
#define		TABFILE_TALISMAN			"talisman.txt"
#define		TABFILE_ENCHASEITEM			"enchaseitem.txt"
#define		TABFILE_IBITEM				"ibitem.txt"
#define		TABFILE_CHARM				"charm.txt"


// 以下定义的结构用于辅助从tabfile中读出属性的初始值
typedef struct tagPROPINFO
{
	int		m_nType;		// 属性的类型. 详见 PI_VARTYPE_...系列定义
	union
	{
	char*	m_pszBuf;		// 指向字符串缓冲区的指针
	int*	m_pnData;		// 指向int变量的指针
	}m_pData;
	int		m_nBufSize;		// 缓冲区的长度
} PROPINFO;
#define		PI_VARTYPE_CHAR		0
#define		PI_VARTYPE_INT		1

char*	TABFILE_ITEM[] = 
{
	TABFILE_WEAPON,			//"MeleeWeapon.txt",
	TABFILE_ARMOR,			//"Armor.txt",
	TABFILE_HELM,			//"Helm.txt",
	TABFILE_BOOT,			//"Boot.txt",
	TABFILE_SHOULDER,		//"Shoulder.txt",
	TABFILE_AMULET,			//"Amulet.txt",
	TABFILE_RING,			//"Ring.txt",
	TABFILE_CUFF,			//"Cuff.txt",
	TABFILE_PENDANT,		//"Pendant.txt",
	TABFILE_HORSE,			//"Horse.txt",
	TABFILE_MEDICINE,
	TABFILE_TASK,	
	TABFILE_MATERIALS,
	TABFILE_TARGETITEM,
	TABFILE_TALISMAN,
	TABFILE_ENCHASEITEM,
	TABFILE_IBITEM,
	TABFILE_CHARM,
};

/******************************************************************************
	功能:	从tab file中读入特定的数据记录
	入口:	pTF: 工具类指针, 用此工具类读取tab file
			nRow: 读第nRow项记录
			pPI[i].m_nType: 给出欲读的记录中第i个域的类型, 可能是整型或字符串
			pPI[i].m_pData: 将数据读到此缓冲区中
			cbFields: 每项记录包含这么多的域
	出口:	成功时返回非零, m_pBuf 指向分配的内存
			失败时返回零
******************************************************************************/
BOOL LoadRecord(IN KTabFile* pTF, IN int nRow,
				IN OUT const PROPINFO* pPI, IN int cbFields)
{
	BOOL bEC = TRUE;

	nRow += 2;	// 加1: 跳过tabfile的第一行. 该行给出的是各列的名称
				// 再加1: KTabFile::GetInteger()函数要求nRow从1开始算起

	// 逐个读入各项属性
	for (int n = 0; n < cbFields; n++)
	{
		if (PI_VARTYPE_INT == (pPI+n)->m_nType)
		{	// 读入 int 型数据
			if (FALSE == pTF->GetInteger(nRow, n+1, -1, (pPI+n)->m_pData.m_pnData))
				{ _ASSERT(FALSE); bEC = FALSE; }
		}
		else
		{	// 读入字符串型数据
			_ASSERT(PI_VARTYPE_CHAR == (pPI+n)->m_nType);

			if (FALSE == pTF->GetString(nRow, n+1, "", (pPI+n)->m_pData.m_pszBuf,
										(pPI+n)->m_nBufSize))
				{ _ASSERT(FALSE); bEC = FALSE; }
		}
	}
	return bEC;
}

//=============================================================================

KLibOfBPT::KLibOfBPT()
{
}

KLibOfBPT::~KLibOfBPT()
{
}

/******************************************************************************
	功能：	总控，从tab file中读入道具,魔法等原始数据
	出口:	相关数据存在m_BPTWeapon,m_BPTWeaponDirty等成员变量中
******************************************************************************/
BOOL KLibOfBPT::Init( void )
{
	// 初始化
	KBasicPropertyTable*	paryBPT[] = {	
											&m_BPTWeapon,
											&m_BPTArmor,
											&m_BPTHelm,
											&m_BPTBoot,
											&m_BPTShoulder,
											&m_BPTAmulet,
											&m_BPTRing,
											&m_BPTCuff,
											&m_BPTPendant,
											&m_BPTHorse,
											&m_BPTMedicine,
											&m_BPTQuest,
											&m_BPTMaterials,
											&m_BPTTargetItem,
											&m_BPTTalisman,
											&m_BPTEnchaseItem,
											&m_BPTIBItem,
											&m_BPTCharm,
										};
	// 将tab file逐个读入
	const int cbNumOfTables = sizeof(paryBPT)/sizeof(paryBPT[0]);
	for (int i = 0; i < cbNumOfTables; i++)
	{
		KBPT_Item* pTemp = (KBPT_Item*)paryBPT[i];
		if ( pTemp )
		{
			pTemp->Init(i);
			pTemp->Load();
		}
	}
	
	return TRUE;
}

const KBASICPROP_ITEM*	KLibOfBPT::GetTargetItemRecord(IN int i) const
{
	return m_BPTTargetItem.GetRecord(i);
}

const KBASICPROP_ITEM* KLibOfBPT::GetWeaponRecord(IN int i) const
{
	return m_BPTWeapon.GetRecord(i);
}

const int KLibOfBPT::GetWeaponRecordNumber() const
{
	return m_BPTWeapon.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetArmorRecord(IN int i) const
{
	return m_BPTArmor.GetRecord(i);
}

const int KLibOfBPT::GetArmorRecordNumber() const
{
	return m_BPTArmor.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetHelmRecord(IN int i) const
{
	return m_BPTHelm.GetRecord(i);
}

const int KLibOfBPT::GetHelmRecordNumber() const
{
	return m_BPTHelm.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetBootRecord(IN int i) const
{
	return m_BPTBoot.GetRecord(i);
}

const int KLibOfBPT::GetBootRecordNumber() const
{
	return m_BPTBoot.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetShoulderRecord(IN int i) const
{
	return m_BPTShoulder.GetRecord(i);
}

const int KLibOfBPT::GetShoulderRecordNumber() const
{
	return m_BPTShoulder.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetAmuletRecord(IN int i) const
{
	return m_BPTAmulet.GetRecord(i);
}

const int KLibOfBPT::GetAmuletRecordNumber() const
{
	return m_BPTAmulet.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetRingRecord(IN int i) const
{
	return m_BPTRing.GetRecord(i);
}

const int KLibOfBPT::GetRingRecordNumber() const
{
	return m_BPTRing.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetCuffRecord(IN int i) const
{
	return m_BPTCuff.GetRecord(i);
}

const int KLibOfBPT::GetCuffRecordNumber() const
{
	return m_BPTCuff.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetPendantRecord(IN int i) const
{
	return m_BPTPendant.GetRecord(i);
}

const int KLibOfBPT::GetPendantRecordNumber() const
{
	return m_BPTPendant.NumOfEntries();
}

const KBASICPROP_ITEM* KLibOfBPT::GetHorseRecord(IN int i) const
{
	return m_BPTHorse.GetRecord(i);
}

const int KLibOfBPT::GetHorseRecordNumber() const
{
	return m_BPTHorse.NumOfEntries();
}

const KBASICPROP_ITEM* KLibOfBPT::GetCharmRecord(IN int i) const
{
	return m_BPTCharm.GetRecord(i);
}

const int KLibOfBPT::GetCharmRecordNumber() const
{
	return m_BPTCharm.NumOfEntries();
}

const KBASICPROP_ITEM* KLibOfBPT::GetMedicineRecord(IN int i) const
{
	return m_BPTMedicine.GetRecord(i);
}

const KBASICPROP_ITEM*	KLibOfBPT::GetIBRecord(IN int i) const
{
	return m_BPTIBItem.GetRecord( i );
}

const int KLibOfBPT::GetMedicineRecordNumber() const
{
	return m_BPTMedicine.NumOfEntries();
}

const int KLibOfBPT::GetIBRecordNumber() const
{
	return m_BPTIBItem.NumOfEntries();
}

const KBASICPROP_ITEM* KLibOfBPT::GetQuestRecord(IN int i) const
{
	return m_BPTQuest.GetRecord(i);
}

const int KLibOfBPT::GetQuestRecordNumber() const
{
	return m_BPTQuest.NumOfEntries();
}

const KBASICPROP_ITEM* KLibOfBPT::GetMaterialRecord(IN int i) const
{
	return m_BPTMaterials.GetRecord(i);
}

const int KLibOfBPT::GetMaterialRecordNumber() const
{
	return m_BPTMaterials.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetTalismanRecord(int detail, int particular, int level) const
{
	return m_BPTTalisman.FindRecord(detail, particular, level);
}

const int KLibOfBPT::GetTalismanRecordNumber() const
{
	return m_BPTTalisman.NumOfEntries();
}

const KBASICPROP_ITEM* KLibOfBPT::GetEnchaseItemRecord(int index) const
{
	return m_BPTEnchaseItem.GetRecord(index);
}

const int KLibOfBPT::GetEnchaseItemRecordNumber() const
{
	return m_BPTEnchaseItem.NumOfEntries();
}

const KBASICPROP_ITEM*	KLibOfBPT::GetWeaponRecordByName(const char* name)
{
	return m_BPTWeapon.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetArmorRecordByName(const char* name)
{
	return m_BPTArmor.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetHelmRecordByName(const char* name)
{
	return m_BPTHelm.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetBootRecordByName(const char* name)
{
	return m_BPTBoot.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetShoulderRecordByName(const char* name)
{
	return m_BPTShoulder.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetAmuletRecordByName(const char* name)
{
	return m_BPTAmulet.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetRingRecordByName(const char* name)
{
	return m_BPTRing.FindRecordByName( name );
}
const KBASICPROP_ITEM*	KLibOfBPT::GetCuffRecordByName(const char* name)
{
	return m_BPTCuff.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetPendantRecordByName(const char* name)
{
	return m_BPTPendant.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetTalismanRecordByName(const char* name)
{
	return m_BPTTalisman.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetMedicineRecordByName(const char* name)
{
	return m_BPTMedicine.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetIBRecordByName(const char* name)
{
	return m_BPTIBItem.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetMaterialRecordByName(const char* name)
{
	return m_BPTMaterials.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetQuestRecordByName(const char* name)
{
	return m_BPTQuest.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetTargetItemRecordByName(const char* name)
{
	return m_BPTTargetItem.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetEnchaseItemRecordByName(const char* name)
{
	return m_BPTEnchaseItem.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetHorseRecordByName(const char* name)
{
	return m_BPTHorse.FindRecordByName( name );
}

const KBASICPROP_ITEM*	KLibOfBPT::GetCharmRecordByName(const char* name)
{
	return m_BPTCharm.FindRecordByName( name );
}

//=============================================================================

KBasicPropertyTable::KBasicPropertyTable()
{
	m_pBuf = NULL;
	m_nNumOfEntries = 0;
	m_nSizeOfEntry = 0;
	m_szTabFile[0] = 0;
}

KBasicPropertyTable::~KBasicPropertyTable()
{
	ReleaseMemory();
}

/******************************************************************************
	功能:	记录tab file中共有多少项数据记录
******************************************************************************/
void KBasicPropertyTable::SetCount(int cbCount)
{
	_ASSERT(cbCount>0);
	_ASSERT(0==m_nNumOfEntries);	// this function is supposed to be called only once
	m_nNumOfEntries = cbCount;
}

/******************************************************************************
	功能:	分配内存,用于保存从tab file中读入的数据
	入口:	m_nNumOfEntries: 共有这么多项数据记录
			m_nSizeOfEntry: 每项数据记录的大小(字节)
	出口:	成功时返回非零, m_pBuf 指向分配的内存
			失败时返回零
******************************************************************************/
BOOL KBasicPropertyTable::GetMemory()
{
	_ASSERT(NULL == m_pBuf);
	_ASSERT(m_nNumOfEntries > 0 && m_nSizeOfEntry > 0);

	BOOL bEC = FALSE;
	const int nMemSize = m_nSizeOfEntry * m_nNumOfEntries;
	void* pBuf = new BYTE[nMemSize];
	_ASSERT(pBuf != NULL);
	if (pBuf != NULL)
	{
		m_pBuf = pBuf;
		bEC = TRUE;
	}
	return bEC;
}

/******************************************************************************
	功能:	释放内存
******************************************************************************/
void KBasicPropertyTable::ReleaseMemory()
{
	if (m_pBuf)
	{
		delete []m_pBuf;
		m_pBuf = NULL;
		m_nNumOfEntries = 0;
	}
}

/******************************************************************************
	功能:	读入tab file中的全部数据
	入口:	m_szTabFile: 文件名
	出口:	成功时返回非零, 全部数据读入m_pBuf所指缓冲区中.
				m_nNumOfEntries 给出共读入多少项数据
			失败时返回零
******************************************************************************/
BOOL KBasicPropertyTable::Load(void)
{
	BOOL bEC = FALSE;
	KTabFile	theLoader;

	// 加载tab file
	g_SetRootPath(NULL);

	char	szFileName[FILE_NAME_LENGTH];
    char    szTABFilePath[FILE_NAME_LENGTH];
    
    sprintf(szTABFilePath, "%s\\", TABFILE_PATH );

	g_UnitePathAndName(szTABFilePath, m_szTabFile, szFileName);
	if (FALSE == theLoader.Load(szFileName))
		{ _ASSERT(FALSE); return bEC; }

	// 确定file内给出了多少项记录
	const int cbItems = theLoader.GetHeight() - 1;	// 第一行给出各列名称,
	if (cbItems < 0)							// 实际数据从第2行开始给出.
		{ _ASSERT(FALSE); return bEC; }
	SetCount(cbItems);

	// 分配内存，构建属性表
	if (FALSE == GetMemory())
		{ _ASSERT(FALSE); return bEC; }

	// 将属性记录逐条读入
	int i;
	for (i = 0; i < cbItems; i++)
	{
		if (FALSE == LoadRecord(i, &theLoader))
			{ _ASSERT(FALSE); return bEC; }
	}

	bEC = TRUE;
	return bEC;
}

/******************************************************************************/

KBPT_Item::KBPT_Item()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_ITEM);
}

KBPT_Item::~KBPT_Item()
{
}

void KBPT_Item::Init(IN int i)
{
	::strcpy(m_szTabFile, TABFILE_ITEM[i]);
}

BOOL KBPT_Item::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	// 初始化
	KBASICPROP_ITEM* pBuf = (KBASICPROP_ITEM*)m_pBuf;
	pBuf = pBuf + i;	// 读入的属性记在 pBuf 所指结构中

	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,	pBuf->szName, sizeof(pBuf->szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nParticularType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nObjIdx), 0},
		{ PI_VARTYPE_CHAR,	pBuf->szIntro, sizeof(pBuf->szIntro)},
		{ PI_VARTYPE_CHAR,	pBuf->szImageSetName, sizeof(pBuf->szImageSetName)},
		{ PI_VARTYPE_CHAR,	pBuf->szImageName, sizeof(pBuf->szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nColor), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nQuality), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nYao), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nYaoRate), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->sYaoGroup), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nSellPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nItemWeight), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nStack), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nDurability), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nEquiSetID), 0},
		{ PI_VARTYPE_CHAR,  (char*)&(pBuf->scriptFile),sizeof(pBuf->scriptFile)},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->szBuffTarget), sizeof(pBuf->szBuffTarget)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->BasicBuff.BuffID[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->BasicBuff.BuffID[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->BasicBuff.BuffID[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nSkillID), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nSkillTarget), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nGroup), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nCDTime), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->ActionTime), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nLogCode), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nItemRes), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqLevel), 0},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->szReqPro), sizeof(pBuf->szReqPro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqProperty[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqProperty[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqProperty[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqProperty[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqProperty[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nReqProperty[5]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->btRepair), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->btCanDiscard), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->btBuffSwitch), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->btCanExchage), 0},		
		{ PI_VARTYPE_INT,  (char*)&(pBuf->btCanYao), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->btCanUpdate), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->Potential), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->TalismanBuff), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->CanSell), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->DeathDropType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->RestrictCount), 0},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->SoundEffects[item_sound_effect_pickup]), sizeof(pBuf->SoundEffects[item_sound_effect_pickup])},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->SoundEffects[item_sound_effect_putdown]), sizeof(pBuf->SoundEffects[item_sound_effect_putdown])},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->SoundEffects[item_sound_effect_use]), sizeof(pBuf->SoundEffects[item_sound_effect_use])},
		{ PI_VARTYPE_INT, (char*)&(pBuf->CostSkillExp), 0},
		{ PI_VARTYPE_INT, (char*)&(pBuf->DropNpcTemplateID), 0},
		{ PI_VARTYPE_INT, (char*)&(pBuf->DisplayID), 0},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->InlayDesc), sizeof(pBuf->InlayDesc)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->EquipBind), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->IBType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->IBLiveTime), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->IBUseCount), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->PickupBind), 0},
		{ PI_VARTYPE_CHAR,  pBuf->IBBuyType, sizeof(pBuf->IBBuyType)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->UseType), 0},
		{ PI_VARTYPE_CHAR,	pBuf->szBigImageSetName, sizeof(pBuf->szBigImageSetName)},
		{ PI_VARTYPE_CHAR,	pBuf->szBigImageName, sizeof(pBuf->szBigImageName)},
		{ PI_VARTYPE_CHAR, (char*)&(pBuf->szMapAreaTarget), sizeof(pBuf->szMapAreaTarget)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->nMaxFlushTimes), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->IsExpItem), 0},

	};

	// 逐个读入各项属性
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

/******************************************************************************
	功能:	获取指定的装备的属性
	入口:	i: 要求获取第i项装备的属性
	出口:	成功时返回指向该装备属性的指针(一个KBASICPROP_EQUIPMENT结构)
			失败时返回NULL
******************************************************************************/
const KBASICPROP_ITEM* KBPT_Item::GetRecord(int i) const
{
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_ITEM*)m_pBuf) + i) : NULL;
}

const KBASICPROP_ITEM* KBPT_Item::FindRecordByName(IN const char* name ) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_ITEM* pData = NULL;
	if ( name == NULL )
	{
		return NULL;
	}
	// 以下使用顺序查找的算法. 若原始数据是按m_nDetailType及m_nLevel排序的,
	// 则可进行算法优化
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_ITEM* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (strcmp( name, pEqu->szName) == 0 )
		{
			pData = pEqu;
			//break;
		}
#ifndef _SERVER
		string res = pEqu->szName;
		if (pEqu->nYao != 0)
		{
			string::size_type i = res.find( name );

			if ( i != string::npos )
			{
				pData = pEqu;
				ItemInfo::getSingleton().d_itemResult.d_vecItem.push_back( pData );
			}
		}
#endif

	}
	return pData;
}

/******************************************************************************
	功能:	获取指定的装备属性记录
	入口:	nDetailType: 具体类别
			nParticularType: 详细类别
			nLevel: 等级
	出口:	成功时返回指向该记录的指针
			失败时返回NULL
******************************************************************************/
const KBASICPROP_ITEM* KBPT_Item::FindRecord( IN int nDetailType,
														IN int nParticularType,
														IN int nLevel) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_ITEM* pData = NULL;
	// 以下使用顺序查找的算法. 若原始数据是按m_nDetailType及m_nLevel排序的,
	// 则可进行算法优化
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_ITEM* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (nDetailType == pEqu->nDetailType &&
			nParticularType == pEqu->nParticularType &&
			nLevel == pEqu->nLevel)
		{
			pData = pEqu;
			break;
		}
	}
	return pData;
}

//============================================================================
