#include "KCore.h"
#include "keconomysys.h"
#include "KSubWorldSet.h"
#include "ConfigManager.h"

//constructor and destructor
KEconomySysManager::KEconomySysManager()
{
	m_bLoaded		= false;
	m_bDirty		= false;
	m_bSettingLoad	= false;
}

KEconomySysManager::~KEconomySysManager()
{
	//do?
}

KEconomySysManager& KEconomySysManager::Singleton()
{
	static KEconomySysManager esm;
	return esm;
}

//
void KEconomySysManager::InitEconomySys()
{
	KIniFile inifile;

	ConfigManager& cm = ConfigManager::Singleton();
	int nSwitch = cm.GetGlobalVariable(global_var_tong_war_economy_sys_switch);
	if (nSwitch == 0)
		return;
	
	if (inifile.Load(MAP_SETTING_FILE_PATH))
	{
		for (int i = 0; i < MAX_WAR_MAP_NUM; i++)
		{
			char szKeyName[32];
			snprintf(szKeyName, sizeof(szKeyName), "map%d", i + 1);  //TongWarMapID从1开始的
			szKeyName[31] = 0;
			
			int nMapId = INVALID_WORLD_ID;
			inifile.GetInteger("TongWarMapID", szKeyName, INVALID_WORLD_ID, &nMapId);
			
			if (nMapId != INVALID_WORLD_ID)
			{
				m_EconomyData[i].nMapId = nMapId;
			}
		}

		m_bSettingLoad = true;
		Load();
	}
	else
	{
		_ASSERT(0);
	}
}

#define ECONOMY_DATA_NEED_SAVE_INTERVAL  (3 * 60 * GAME_FPS)
#define ECONOMY_DATA_FORCE_SAVE_INTERVAL (10 * 60 * GAME_FPS)
void KEconomySysManager::Breathe()
{
	if (!IsValidInitAndLoad())
		return;

	if ((g_SubWorldSet.GetGameTime() % ECONOMY_DATA_NEED_SAVE_INTERVAL == 0) && IsNeedSave())
		Save();

	if (g_SubWorldSet.GetGameTime() % ECONOMY_DATA_FORCE_SAVE_INTERVAL == 0)
		Save();
}

//属性值相关操作
bool KEconomySysManager::GetAttrValue(int nMapId, int AttrType, int enAttrId, DWORD& nRet)
{
	DWORD* pValue = GetAttrReference(nMapId, AttrType, enAttrId);
	if (pValue == NULL)
		return false;

	nRet = *pValue;

	return true;
}

bool KEconomySysManager::SetAttrValue(int nMapId, int AttrType, int enAttrId, DWORD nNewValue)
{
	DWORD* pValue = GetAttrReference(nMapId, AttrType, enAttrId);
	if (pValue == NULL)
		return false;

	*pValue = nNewValue;
	SetDataChangedFlag(true);

	return true;
}

bool KEconomySysManager::AddAttrValue(int nMapId, int AttrType, int enAttrId, DWORD nIncrement)
{
	DWORD* pValue = GetAttrReference(nMapId, AttrType, enAttrId);
	if (pValue == NULL || nIncrement == 0)
		return false;

	DWORD nOldValue	   = *pValue;
	DWORD nFinallyValue = nOldValue + nIncrement;

	if (nFinallyValue <= nOldValue)
		return false;

	*pValue = nFinallyValue;
	SetDataChangedFlag(true);

	return true;
}

bool KEconomySysManager::DecAttrValue(int nMapId, int AttrType, int enAttrId, DWORD nDecrement)
{
	DWORD* pValue = GetAttrReference(nMapId, AttrType, enAttrId);
	if (pValue == NULL || nDecrement == 0)
		return false;
	
	DWORD nOldValue	   = *pValue;
	if (nOldValue < nDecrement)
		return false;

	DWORD nFinallyValue = nOldValue - nDecrement;
	if (nFinallyValue >= nOldValue)
		return false;
	
	*pValue = nFinallyValue;
	SetDataChangedFlag(true);

	return true;
}

//DB相关操作
void KEconomySysManager::Save()
{
	//如果尚未载入，Save会导致无效数据把数据库中的数据覆盖了
	if (!IsValidInitAndLoad())
		return;

	char szBuff[8192];
	_ASSERT(8192 > sizeof(unsigned long) + sizeof(unsigned long) + MAX_WAR_MAP_NUM * sizeof(DBStoreData));
	if (8192 <= sizeof(unsigned long) + sizeof(unsigned long) + MAX_WAR_MAP_NUM * sizeof(DBStoreData))
		return;
	
	unsigned char* pInfoBuff   = (unsigned char* )szBuff;
	unsigned long  dwTotalSize = 0;

	//1.保存版本号
	unsigned long dwVersion = FS_ECONOMY_INFO_VERSION;
	memcpy(pInfoBuff, &dwVersion, sizeof(unsigned long));
	pInfoBuff   += sizeof(unsigned long);
	dwTotalSize += sizeof(unsigned long);

	//2.保存记录个数，应该是4个
	unsigned long dwRecNum = MAX_WAR_MAP_NUM;
	memcpy(pInfoBuff, &dwRecNum, sizeof(unsigned long));
	pInfoBuff   += sizeof(unsigned long);
	dwTotalSize += sizeof(unsigned long);

	//3.保存经济数据
	DBStoreData TempData;
	for (int i = 0; i < MAX_WAR_MAP_NUM; ++i)
	{
		TempData.nMapId = (BYTE )m_EconomyData[i].nMapId;

		TempData.dbCityPopulation       = m_EconomyData[i].NeedSaveData[needsave_economy_population     ];
		TempData.dbCityBaseDevelopment  = m_EconomyData[i].NeedSaveData[needsave_economy_basedevelopment];
		TempData.dbCityTiredness        = m_EconomyData[i].NeedSaveData[needsave_economy_tiredness      ];
		TempData.dbBuildingLevel		= m_EconomyData[i].NeedSaveData[needsave_economy_buildinglevel  ];
		memcpy(TempData.ScriptData, m_EconomyData[i].ScriptVariableData, sizeof(m_EconomyData[i].ScriptVariableData));

		memcpy(pInfoBuff, &TempData, sizeof(DBStoreData));
		pInfoBuff    += sizeof(DBStoreData);
		dwTotalSize  += sizeof(DBStoreData);
	}

	//4.调用存储过程 
	_GlobalHeader DBHeader;
	memset(&DBHeader, 0, sizeof(DBHeader));

	DBHeader.ulNetID   = -1;
	DBHeader.ProcType  = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_economy;

	IProcParam* pParam = g_pController->GetProcParam();

	//begin push
	pParam->BeginPush(PN_SETGLOBAL);

	pParam->Push(DBHeader.nGlobalID);
	pParam->Push(BinPair(szBuff, dwTotalSize));

	//end push
	pParam->EndPush((char* )&DBHeader, sizeof(DBHeader));

	if (g_pController->CallProc(cfs_db_cnn_global_npcsave, pParam))
	{
		SetDataChangedFlag(false);
	}

}

void KEconomySysManager::Load()
{
	_GlobalHeader DBHeader;
	memset(&DBHeader, 0, sizeof(DBHeader));

	DBHeader.ulNetID   = -1;
	DBHeader.ProcType  = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_economy;

	IProcParam* pParam = g_pController->GetProcParam();

	//begin push
	pParam->BeginPush(PN_GETGLOBAL);

	pParam->Push(DBHeader.nGlobalID);

	//end push
	pParam->EndPush((char* )&DBHeader, sizeof(DBHeader));

	g_pController->CallProc(cfs_db_cnn_global_npcsave, pParam);
}

void KEconomySysManager::LoadComplete(int nDBOpRst, int nDataSize, unsigned char* pData)
{
	if (nDBOpRst && pData != NULL && nDataSize >= 2 * sizeof(unsigned long))
	{
		unsigned char* pSrc = (unsigned char* )pData;

		//1.载入并判断版本信息
		unsigned long dwVersion = 0;
		memcpy(&dwVersion, pSrc, sizeof(unsigned long));
		pSrc += sizeof(unsigned long);

		_ASSERT(dwVersion <= FS_ECONOMY_INFO_VERSION);
		if (dwVersion > FS_ECONOMY_INFO_VERSION)
			return;

		//2.载入Rec个数，必需是4个，因为存了4个
		unsigned long dwRecNum = 0;
		memcpy(&dwRecNum, pSrc, sizeof(unsigned long));
		_ASSERT(dwRecNum == MAX_WAR_MAP_NUM);
		if (dwRecNum != MAX_WAR_MAP_NUM)
			return;

		pSrc += sizeof(unsigned long);
		unsigned long dwSizeLeft = nDataSize - 2 * sizeof(unsigned long);
		_ASSERT(dwSizeLeft == MAX_WAR_MAP_NUM * sizeof(DBStoreData));
		if (dwSizeLeft != MAX_WAR_MAP_NUM * sizeof(DBStoreData))
			return;

		//3.载入数据
		DBStoreData* pInfo = NULL;
		for (int i = 0; i < MAX_WAR_MAP_NUM; ++i)
		{
			pInfo = (DBStoreData* )pSrc;
			int iter = GetIterByMapId(pInfo->nMapId);
			
			if (iter != -1)
			{
				m_EconomyData[iter].NeedSaveData[needsave_economy_population     ]  = pInfo->dbCityPopulation;
				m_EconomyData[iter].NeedSaveData[needsave_economy_basedevelopment]  = pInfo->dbCityBaseDevelopment;
				m_EconomyData[iter].NeedSaveData[needsave_economy_tiredness      ]  = pInfo->dbCityTiredness;
				m_EconomyData[iter].NeedSaveData[needsave_economy_buildinglevel  ]	= pInfo->dbBuildingLevel;
				memcpy(m_EconomyData[iter].ScriptVariableData, pInfo->ScriptData, sizeof(pInfo->ScriptData));
			}
			
			pSrc += sizeof(DBStoreData);
		}
	}

	m_bLoaded = true;
}

bool KEconomySysManager::IsValidInitAndLoad()
{
	return m_bLoaded && m_bSettingLoad;
}

//功能函数
int  KEconomySysManager::TestOperateValidity(int nMapId, int AttrType, int enAttrId)
{
	if (!IsValidInitAndLoad())
		return -1;

	if (nMapId == INVALID_WORLD_ID)
		return -1;

	switch (AttrType)
	{
	case attrtype_needsave:
		if (enAttrId <= needsave_economy_begin || enAttrId >= needsave_economy_end)
			return -1;
		break;

	case 	attrtype_notneedsave:
		if (enAttrId <= notneedsave_economy_begin || enAttrId >= notneedsave_economy_end)
			return -1;
		break;

	case attrtype_scriptvariable:
		if (enAttrId < 0 || enAttrId >= SCRIPT_VARIABLE_DATA_NUM)
			return -1;
		break;

	default://Data Type检查不合格会在这里跳走
		return -1;
	}
	
	int iter = GetIterByMapId(nMapId);
	if (iter == -1)
		return -1;

	return iter;
}

int  KEconomySysManager::GetIterByMapId(int nMpaId)
{
	int iter = -1;
	
	if (nMpaId == INVALID_WORLD_ID)
		return iter;
	
	for (int i = 0; i < MAX_WAR_MAP_NUM; ++i)
	{
		if (m_EconomyData[i].nMapId == nMpaId)
		{
			iter = i;
			break;
		}
	}
	
	return iter;
}

DWORD* KEconomySysManager::GetAttrReference(int nMapId, int AttrType, int enAttrId)
{
	int iter = TestOperateValidity(nMapId, AttrType, enAttrId);
	if (iter == -1)
		return NULL;

	switch (AttrType)
	{
	case attrtype_needsave:
		if (enAttrId > needsave_economy_begin && enAttrId < needsave_economy_end)
			return &(m_EconomyData[iter].NeedSaveData[enAttrId]);
		break;
		
	case 	attrtype_notneedsave:
		if (enAttrId > notneedsave_economy_begin && enAttrId < notneedsave_economy_end)
			return &(m_EconomyData[iter].NotNeedSaveData[enAttrId]);
		break;
		
	case attrtype_scriptvariable:
		if (enAttrId >= 0 || enAttrId < SCRIPT_VARIABLE_DATA_NUM)
			return &(m_EconomyData[iter].ScriptVariableData[enAttrId]);
		break;
	}

	return NULL;
}