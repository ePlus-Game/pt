//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KBasPropTbl.h
// Date:	2002.08.14
// Code:	DongBo
// Desc:    header file. 本文件定义的类用于从tab file中读出道具的初始属性,
//			并生成对应的属性表
//---------------------------------------------------------------------------

#ifndef	KBasPropTblH
#define	KBasPropTblH

#include "ItemCommonDef.h"

//干................掉
struct KMagicAttrib
{
	int				nAttribType;					//属性类型
	int				nValue[3];						//属性参数
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};
//--------------------------------------

class KBasicPropertyTable			// 缩写: BPT,用于派生类
{
public:
	KBasicPropertyTable();
	~KBasicPropertyTable();

// 以下是核心成员变量
protected:
	void*		m_pBuf;						// 指向属性表缓冲区的指针
											// 属性表是一个结构数组,
											// 其具体类型由派生类决定
	int			m_nNumOfEntries;			// 属性表含有多少项数据

// 以下是辅助性的成员变量
    int         m_nSizeOfEntry;				// 每项数据的大小(即结构的大小)
	char		m_szTabFile[MAX_PATH];		// tabfile的文件名

// 以下是对外接口
public:
	virtual BOOL Load(void );        // 从tabfile中读出初始属性值, 填入属性表
	int NumOfEntries() const { return m_nNumOfEntries; }

// 以下是辅助函数
protected:
	BOOL GetMemory();
	void ReleaseMemory();
	void SetCount(int);
	virtual BOOL LoadRecord(int i, KTabFile* pTF) = 0;
};

//=============================================================================

class KBPT_Item : public KBasicPropertyTable
{
public:
	KBPT_Item();
	~KBPT_Item();

// 以下是对外接口
public:
	const KBASICPROP_ITEM* GetRecord(IN int) const;
	const KBASICPROP_ITEM* FindRecordByName(IN const char* name ) const;
	const KBASICPROP_ITEM* FindRecord(IN int, IN int, IN int) const;
	void Init(IN int);
// 以下是辅助函数
protected:
	virtual BOOL LoadRecord(int i, KTabFile* pTF);
};

//=============================================================================

class KLibOfBPT
{
public:
	KLibOfBPT();
	~KLibOfBPT();

// 以下是核心成员变量
protected:
	KBPT_Item			m_BPTHorse;
	KBPT_Item			m_BPTWeapon;
	KBPT_Item			m_BPTArmor;
	KBPT_Item			m_BPTHelm;
	KBPT_Item			m_BPTBoot;
	KBPT_Item			m_BPTShoulder;
	KBPT_Item			m_BPTAmulet;
	KBPT_Item			m_BPTRing;
	KBPT_Item			m_BPTCuff;
	KBPT_Item			m_BPTPendant;
	KBPT_Item			m_BPTMedicine;
	KBPT_Item			m_BPTQuest;
	KBPT_Item			m_BPTMaterials;			// 生产材料表
	KBPT_Item			m_BPTTargetItem;
	KBPT_Item			m_BPTTalisman;
	KBPT_Item			m_BPTEnchaseItem;
	KBPT_Item			m_BPTIBItem;
	KBPT_Item			m_BPTCharm;


public:

	BOOL Init( void );

 	const KBASICPROP_ITEM*	GetTargetItemRecord(IN int) const;   
	const KBASICPROP_ITEM*	GetWeaponRecord(IN int) const;
	const int					GetWeaponRecordNumber() const;
	const KBASICPROP_ITEM*	GetArmorRecord(IN int) const;
	const int					GetArmorRecordNumber() const;
	const KBASICPROP_ITEM*	GetHelmRecord(IN int) const;
	const int					GetHelmRecordNumber() const;
	const KBASICPROP_ITEM*	GetBootRecord(IN int) const;
	const int					GetBootRecordNumber() const;
	const KBASICPROP_ITEM*	GetShoulderRecord(IN int) const;
	const int					GetShoulderRecordNumber() const;
	const KBASICPROP_ITEM*	GetAmuletRecord(IN int) const;
	const int					GetAmuletRecordNumber() const;
	const KBASICPROP_ITEM*	GetRingRecord(IN int) const;
	const int					GetRingRecordNumber() const;
	const KBASICPROP_ITEM*	GetCuffRecord(IN int) const;
	const int					GetCuffRecordNumber() const;
	const KBASICPROP_ITEM*	GetPendantRecord(IN int) const;
	const int					GetPendantRecordNumber() const;
	const KBASICPROP_ITEM*	GetHorseRecord(IN int) const;
	const int					GetHorseRecordNumber() const;

	const int					GetMedicineRecordNumber() const;
	const KBASICPROP_ITEM*		GetMedicineRecord(IN int i) const;

	const int					GetIBRecordNumber() const;
	const KBASICPROP_ITEM*		GetIBRecord(IN int i) const;
		
	const KBASICPROP_ITEM*	FindMedicine(IN int, IN int) const;
	const KBASICPROP_ITEM*		GetQuestRecord(IN int) const;
	const int					GetQuestRecordNumber() const;
	const KBASICPROP_ITEM*	GetTownPortalRecord(IN int) const;
	const int					GetTownPortalRecordNumber() const;
	const KBASICPROP_ITEM*		GetMine(IN int) const;
	const KBASICPROP_ITEM*	GetMagicOrScriptRecord(IN int nIndex ) const;
	const int					GetMagicOrScriptNumber() const;

	const KBASICPROP_ITEM*	GetMaterialRecord(IN int) const;
	const int					GetMaterialRecordNumber() const;

	const KBASICPROP_ITEM*	GetSkillBookRecord(int nIndex) const;
	const int					GetSkillBookRecordNumber() const;

	const KBASICPROP_ITEM*	GetTalismanRecord(int detail, int particular, int level) const;
	const int					GetTalismanRecordNumber() const;

	const KBASICPROP_ITEM*	GetEnchaseItemRecord(int index) const;
	const int					GetEnchaseItemRecordNumber() const;

	const KBASICPROP_ITEM*	GetWeaponRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetArmorRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetHelmRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetBootRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetShoulderRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetAmuletRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetRingRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetCuffRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetPendantRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetTalismanRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetMedicineRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetIBRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetMaterialRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetQuestRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetTargetItemRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetEnchaseItemRecordByName(const char* name);
	const KBASICPROP_ITEM*	GetHorseRecordByName(const char* name);

	const KBASICPROP_ITEM*	GetCharmRecord(IN int) const;
	const int				GetCharmRecordNumber() const;
	const KBASICPROP_ITEM*	GetCharmRecordByName(const char* name);	
};

#endif		// #ifndef KBasPropTblH

