//////////////////////////////////////////////////////////////////////////
//	建筑物
//	lixuewu 2004.06.14

#ifndef _INC_KBUILDING_H_
#define _INC_KBUILDING_H_
//////////////////////////////////////////////////////////////////////////
//
#ifndef _SERVER
#include "KRepresentUnit.h"
#include <vector>

using std::vector;
#endif

#include "KIndexNode.h"
#include "KNpc.h"
#include "KLinkArray.h"
//////////////////////////////////////////////////////////////////////////
// 一个Set中包含的最大建筑数
#ifdef _SERVER
#define MAX_BUILDING	1024
#else
#define MAX_BUILDING	128
#endif

//////////////////////////////////////////////////////////////////////////
// 建筑物所处的状态
enum building_state
{
	building_build = 0,		// 建设中
	building_normal,		// 建成
	building_ruin,			// 衰败
	building_burn,			// 燃烧
	building_destroy,		// 毁坏	
	building_working,		// 正在工作的状态
	// <Add name="Adt.X" time="2005/12/28">	
	building_dummystate,	// 不存在状态
	// </Add>
	building_state_count,	// 状态总数
};

enum building_kind
{
	building_kind_normal = 0,
	building_kind_totem,
	building_kind_plant,
};
//////////////////////////////////////////////////////////////////////////
// 设定模板数据
struct KBuildingTemplate 
{
	enum {MAX_NAME_SIZE=32,DESC_INFO_SIZE = 256, DESC_TIP_SIZE = 256,};
	unsigned int uNpcID; // 使用的Npc外观
	unsigned int uKind;
#ifdef _SERVER
	char szScriptFile[FILE_NAME_LENGTH];
#else
	char szName[MAX_NAME_SIZE];
	char szIconFile[FILE_NAME_LENGTH];
	char szDescInfo[DESC_INFO_SIZE];
	char szDescTip[DESC_TIP_SIZE];
	// --> Rocker Edit Start 2005/10/24
	char szSmallIconFile[FILE_NAME_LENGTH];
	// <-- Rocker End
#endif
};

//////////////////////////////////////////////////////////////////////////
// 建筑系统设定模板,虽然大家都可以用但只有BuildingSet可以初始化
class KBuildingSetting
{
	friend class KBuildingSet;
public:
	enum {INVALID_ID = 0xFFFFFFFF};
    inline const KBuildingTemplate& operator[](unsigned int nIndex) const
    {
        _ASSERT(m_nCount > nIndex);
        return m_pSetting[nIndex];
    }
	inline unsigned int GetCount(void) const { return m_nCount; };
#ifndef _SERVER
	inline unsigned int GetTotemCount(void) const {return m_Totem.size();}

	inline unsigned int GetCanBuildCount(void) const {return m_CanBuild.size();}
	
	inline unsigned int GetPlantCount(void) const { return m_Plant.size(); }
		
	inline unsigned int Totem2BuildingID(unsigned int uTotem) 
	{
		if (uTotem < m_Totem.size())
		{
			return m_Totem[uTotem];
		}
		return INVALID_ID;
	}
	
	inline unsigned int BuildOrder2BuildingID(unsigned int uBuildOrder) 
	{
		if (uBuildOrder < m_CanBuild.size())
		{
			return m_CanBuild[uBuildOrder];
		}
		return INVALID_ID;
	}

	inline unsigned int Plant2BuildingID(unsigned int uPlant)
	{
		if (uPlant < m_Plant.size())
		{
			return m_Plant[uPlant];
		}
		return INVALID_ID;
	}
#endif
public:
	// 初始化与释放
	BOOL Init(void);
    void Release(void);
protected:
    KBuildingSetting():m_nCount(0u),m_pSetting(NULL){}
    ~KBuildingSetting(){ Release(); }
private:
    unsigned int m_nCount;
    KBuildingTemplate* m_pSetting;
#ifndef _SERVER
	typedef vector<unsigned int> BuildingTypeList;
	BuildingTypeList m_CanBuild;
	BuildingTypeList m_Totem;
	BuildingTypeList m_Plant;
#endif	
};

//////////////////////////////////////////////////////////////////////////
// 建筑实体,建筑实体只存在于服务器
#ifdef _SERVER
class KBuilding 
{
	friend class KBuildingSet;
public:
	struct KBuildingCoreData
	{
		unsigned int nTemplateID;			// 建筑类型ID
		unsigned long dwState;				// 当前状态
		unsigned int nCurLife;				// 当前生命
		int nMpsX;
		int nMpsY;
		BYTE nLevel;						// 当前等级
		char dummy[31];						// 空余保留
	};
public: // 基础方法
	
	void Active(void);

	BOOL Init(unsigned int nTemplateID,
				unsigned int nIndex, const struct KMapPos& aPos);
	void Release(void);
	
public: // 客户服务器分离的方法
	KIndexNode m_Node;

	void BeginChangeLevel(void);			  // 进入建设状态建筑等级
	void EndChangeLevel(unsigned int uLevel); // 建筑升级完毕

	void OpenObstacle(void);	// 开启障碍
	void CloseObstacle(void);	// 关闭障碍

	BOOL ProducePolyMorph(unsigned int uType, unsigned int uProdcuetime); // 开始创建变身
	void ProduceFinshed(const struct KProduct* pProduct); // 对象生产完毕
	inline void SetState(unsigned long dwState) { m_dwState = dwState; }  // 变更建筑状态
	inline unsigned long GetState(void) const { return m_dwState; }	// 变更建筑状态
public:	// 私有数据访问函数
	inline KNpc* GetNpc(void) const { return m_pNpc; }	
	inline unsigned long GetTong(void) const { return m_dwTong; }
	inline unsigned int GetSettingID(void) const { return m_nTemplateID; }

	// <Add name="Adt.X" time="2005/11/09">
	bool IsPreLoad() const				// 直接摆在地表的建筑
	{
		return m_bPreLoad;
	}

	bool IsDynaicLoad() const			// 城市操作动态创建的建筑
	{
		return !m_bPreLoad;
	}

	void SetPreLoaded()					// 直接摆在地表的建筑
	{
		m_bPreLoad = true;
	}
	// </Add>	



	// <Add name="Adt.X" time="2005/12/28">
	void SetCoreData(KBuildingCoreData* pCoreData)
	{
		assert(m_pCoreData == 0);
		m_pCoreData = pCoreData;
	}

	KBuildingCoreData* GetCoreData()
	{
		return m_pCoreData;
	}
	// </Add>
	
private:
	KNpc*		 m_pNpc;				// 表象的Npc
	unsigned int m_nTemplateID;			// 建筑类型ID
	unsigned int m_nIndex;				// 在集合中的序号

	unsigned long m_dwTong;				// 建筑所属帮会
	unsigned long m_dwState;			// 建筑的状态
	
	unsigned long m_dwMpsX;				// 建筑物的建立位置
	unsigned long m_dwMpsY;
	// <Add name="Adt.X" time="2005/11/09">
	bool		  m_bPreLoad;			// 建筑的装载类型
	// </Add>
	

	// <Add name="Adt.X" time="2005/12/28">
	KBuildingCoreData* m_pCoreData;		// 用于战后修改数据.
	// </Add>	
protected:
	static const KBuildingSetting& s_BuildingSetting; // 设定
};
// 存储建筑物的尺寸
#define BUILDING_SAVE_BUFFER_SIZE sizeof(KBuilding::KBuildingCoreData)
#endif

//////////////////////////////////////////////////////////////////////////
// 建筑集合
// Add和CanPut都是用KMapPos来决定位置，但里边的nOffX,nOffY是被忽略的
// 也就是Building只能放在与格子对齐的位置
// m_Buildings 中的的第一个元素[0]是保留的,用于建设状态
#define Buildings KBuildingSet::GetInstance()
class KBuildingSet
{
public:
    static inline KBuildingSet& GetInstance(void) { return s_Self; }
public:
	// 初始化与释放, <只能自己初始化>
    BOOL Init(void);
    void Release(void);
public:
#ifdef _SERVER
	// 添加删除
    unsigned int Add(unsigned long nTemplateID,const struct KMapPos& aPos);
    void Remove(unsigned int nIndex);
	// 存储加载
	unsigned int Load(unsigned int uSubWorldIdx,const void* const pBuffer, unsigned int uLen);
	BOOL Save(unsigned int uIndex, void* const pBuffer, unsigned int uLen) const;
    //
    inline KBuilding& operator[](unsigned int nIndex)
    {
		_ASSERT(nIndex < MAX_BUILDING);
        return m_Buildings[nIndex];
    }
#else
	// 准备建设某类建筑,参数是窗口坐标
	void BeginBuildBuilding(unsigned int uType, unsigned int nTemplateID);
	void TryToBuildBuilding(unsigned int nX,unsigned int nY);
	void ValidateBuildBuilding(BOOL bCancel);
	void EndBuildBuilding(void);
	// 绘制函数
	void Draw(void);
	void DrawIcon(unsigned int uID, int X, int Y, int nAlpha);
	// 建筑种类个数
	unsigned int GetBuildingTypeCount(void) { return m_Setting.GetCount(); }
	unsigned int GetTotemTypeCount(void) { return m_Setting.GetTotemCount(); }
	unsigned int GetCanBuildTypeCount(void) { return m_Setting.GetCanBuildCount();}
	unsigned int GetPlantTypeCount(void) const { return m_Setting.GetPlantCount(); }
	
	unsigned int TotemID2BuildingID(unsigned int uID) { return m_Setting.Totem2BuildingID(uID); }
	unsigned int BuildOrderID2BuildingID(unsigned int uID) { return m_Setting.BuildOrder2BuildingID(uID); }
	unsigned int PlantID2BuildingID(unsigned int uID) { return m_Setting.Plant2BuildingID(uID); }
	// 描述信息
	BOOL GetName(unsigned int uID, char* const pBuffer, unsigned int uLen);
	BOOL GetDescText(unsigned int uID, char* const pBuffer, unsigned int uLen);
	BOOL GetTipText(unsigned int uID, char* const pBuffer, unsigned int uLen); 
	BOOL GetIconName(unsigned int uID, char* const pBuffer, unsigned int uLen); 
	// --> Rocker Edit Start 2005/10/24
	BOOL GetSmallIconName(unsigned int uID, char* const pBuffer, unsigned int uLen);
	// <-- Rocker End
#endif
	// <Mod name="Adt.X" time="2005/11/08">
	// 变成_SEVER _CLIENT都有的.
	int  GetBuildingIdByNpcSettingId(int nNpcSettingId);
	// </Mod>	
	
	const KBuildingSetting& GetSetting(void) const { return m_Setting; }
	// 检查是否可以放置某类建筑
	BOOL CanPut(unsigned long nTemplateID,const struct KMapPos& aPos, BOOL bCheckTerrain);
private:
	const class KNpcTemplate* GetNpcTemplate(unsigned int uID);
private:
#ifdef _SERVER
    KBuilding m_Buildings[MAX_BUILDING];	// 建筑池
    class KLinkArray m_UsedIndexs;			// 以用表
    class KLinkArray m_FreeIndexs;			// 未用表
#else
	KRUImage m_Image;						// 建设外观
	unsigned int m_uSceneID;				// 图形在场景中的ID
	unsigned int m_uBuildType;				// 建筑类型
	
	KRUImage m_Icon;						// 用于绘制的图标
#endif
private:
    KBuildingSetting m_Setting;				// 基础设定模板
private:
    static KBuildingSet s_Self;				// 自己
private:
    KBuildingSet();							// 只有自己能构造
public:
    ~KBuildingSet();
};

//////////////////////////////////////////////////////////////////////////
// 地形图标
#ifndef _SERVER
#define MAX_TERRAIN	5

#define TerrainInfo KTerrainInfo::GetInstance()

class KTerrainInfo
{
public:
    static inline KTerrainInfo& GetInstance(void) { return s_Self; }	
public:
	BOOL Init();
	BOOL Release();
	void DrawIcon(unsigned int uID, int X, int Y, int nAlpha);
	BOOL GetDescText(unsigned int uID, char* const pBuffer, unsigned int uLen);
	unsigned int GetCount(void) { return m_uInfoCount; }
protected:
	struct TerrainInfo_t
	{
		enum{ DESC_INFO_SIZE = 256 };
		char szIcon[FILE_NAME_LENGTH];
		char szDesc[DESC_INFO_SIZE];
	};
private:
	KRUImage m_Icon;
	TerrainInfo_t* m_pSettings;
	unsigned int m_uInfoCount;
private:
	static KTerrainInfo s_Self;
private:
    KTerrainInfo();							// 只有自己能构造
public:	
	~KTerrainInfo();
};
#endif

#endif //_INC_KBUILDING_H_

// end of file
