//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright(C)   2008
//
//      Created_datetime : 2008-09-02  
//      File_base        : keconomysys
//      File_ext         : h
//      Author           : Wu Shaohui
//      Description      : City Economy Sysment
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#ifndef _economy_system_h_
#define _economy_system_h_

enum enEconomyNeedSaveAttribute
{
	needsave_economy_begin = -1,
	
	needsave_economy_population,			//城市人口
	needsave_economy_basedevelopment,		//城市基础发展值
	needsave_economy_tiredness,				//城市疲劳度
	needsave_economy_buildinglevel,			//建筑等级
	
	needsave_economy_end,
};

enum enEconomyNotNeedSaveAttribute
{
	notneedsave_economy_begin = -1,

	notneedsave_economy_populationproportion,	//城市人口比重
	notneedsave_economy_developincrement,		//城市发展度增量

	notneedsave_economy_end,
};

enum enAttrType
{
	attrtype_begin = -1,

	attrtype_needsave,
	attrtype_scriptvariable,
	attrtype_notneedsave,

	attrtype_end,
};

#define SCRIPT_VARIABLE_DATA_NUM 10
#define CAPTURE_CITY_TIME_ITER 9	//这个用于记录城市被攻占的时间
#define IsValidAttrType(type) (type > attrtype_begin && type < attrtype_end)
#define FS_ECONOMY_INFO_VERSION 1
#define MAX_WAR_MAP_NUM         4
#define MAP_SETTING_FILE_PATH   "/settings/tongwar.ini"

typedef struct tagEconomyData
{
	int nMapId;
	DWORD NeedSaveData[needsave_economy_end];
	DWORD NotNeedSaveData[notneedsave_economy_end];
	DWORD ScriptVariableData[SCRIPT_VARIABLE_DATA_NUM];

	tagEconomyData():nMapId(INVALID_WORLD_ID)
	{
		ZeroMemory(NeedSaveData,    sizeof(NeedSaveData));
		ZeroMemory(NotNeedSaveData, sizeof(NotNeedSaveData));
		ZeroMemory(ScriptVariableData,  sizeof(ScriptVariableData));
	}
}EconomyData;

class KEconomySysManager
{
public:
	//Constructor and Destructor
	KEconomySysManager();
	~KEconomySysManager();
	static KEconomySysManager& Singleton();

	//
	void InitEconomySys();
	void Breathe();

	//操作要存的非脚本变量
	bool GetAttrValue(int nMapId, int AttrType, int enAttrId, DWORD& nRet);
	bool SetAttrValue(int nMapId, int AttrType, int enAttrId, DWORD nNewValue);
	bool AddAttrValue(int nMapId, int AttrType, int enAttrId, DWORD nIncrement);
	bool DecAttrValue(int nMapId, int AttrType, int enAttrId, DWORD nDecrement);

	//DB 操作相关
	void Save();
	void Load();
	void LoadComplete(int nDBOpRst, int nDataSize, unsigned char* pData);
	bool IsValidInitAndLoad();

	//
	void  SetCaptureCityTime(int nMapID, DWORD dwTime);

private:
	EconomyData m_EconomyData[MAX_WAR_MAP_NUM];
	bool        m_bLoaded;
	bool        m_bDirty;
	bool		m_bSettingLoad;

private:
	bool IsNeedSave();
	void SetDataChangedFlag(bool bFlag);

	int  TestOperateValidity(int nMapId, int nAttrType, int enAttrId);//注意，这个函数才是获得合法可用iter的唯一方法，不应该调用GetIterByMapId函数来获得iter
	int  GetIterByMapId(int nMapId);								//只被TestOperateValidity函数使用
	DWORD* GetAttrReference(int nMapId, int AttrType, int enAttrId);
};

#pragma	pack(push, 1)

typedef struct tagDBStoreData  //这里是要存入DB的数据的结构
{
	BYTE nMapId;

	DWORD dbCityPopulation;
	DWORD dbCityBaseDevelopment;
	DWORD dbCityTiredness;
	DWORD dbBuildingLevel;

	DWORD ScriptData[SCRIPT_VARIABLE_DATA_NUM];
	
	tagDBStoreData():nMapId(INVALID_WORLD_ID),dbCityPopulation(0),dbCityBaseDevelopment(0),dbCityTiredness(0), dbBuildingLevel(0)
	{
		ZeroMemory(ScriptData, sizeof(ScriptData));
	}

}DBStoreData;

#pragma pack(pop)

inline bool KEconomySysManager::IsNeedSave()
{
	return m_bDirty;
}

inline void KEconomySysManager::SetDataChangedFlag(bool bFlag)
{
	m_bDirty = bFlag;
}

inline void KEconomySysManager::SetCaptureCityTime(int nMapID, DWORD dwTime)
{
	int iter = GetIterByMapId(nMapID);
	if ( iter == -1 )
		return ;

	m_EconomyData[iter].ScriptVariableData[CAPTURE_CITY_TIME_ITER] = dwTime;
	Save();
}


#endif