//////////////////////////////////////////////////////////////////////////
//	建筑物(KBuilding.cpp)
//	lixuewu 2004.06.14

#include "KCore.h"

#include "KBuilding.h"
#include "KEngine.h"
#include "KSubWorldSet.h"

#include "KPlayer.h"
#include "KSortScript.h"
#include "KNpcTemplate.h"

#include "KProtocol.h"
#include "KSiegeProducer.h"
#include "KNpc.h"

#ifndef _SERVER
#include "CoreShell.h"
#include "iRepresentshell.h"
#include "Scene\KScenePlaceC.h"
#endif

#include "KCityProtocolDef.h"
#include "KSubWorld.h"
#include "KNpc.h"

extern KLuaScript *g_pNpcLevelScript;

#ifndef _SERVER
extern iRepresentShell* g_pRepresent;
#endif

//////////////////////////////////////////////////////////////////////////
// @KBuildingSetting


// 建筑类型设定文件
#define BUILDING_SETTING_FILE "\\settings\\buildings.txt"
// 设定文件的表头格式
enum
{
	BUILDING_NAME = 1,
	BUILDING_SPR_NPC,
	BUILDING_SCRIPT_NORMAL,
	BUILDING_ICON,
	BUILDING_DESC,
	BUILDING_TIP,
	BUILDING_CANBUILD,
	BUILDING_KIND,
// --> Rocker Edit Start 2005/10/24
	BULIDING_SMALL_ICON,
// <-- Rocker End
	BUILDING_TABLE_COL_COUNT
};

//************************************************************************
// 初始化模板设定
//************************************************************************
BOOL KBuildingSetting::Init(void)
{
    if (NULL == m_pSetting) 
    {
        KTabFile aTabFile;
		g_SetRootPath(NULL);
        if (aTabFile.Load(BUILDING_SETTING_FILE))
        {
            const unsigned int nCount = aTabFile.GetHeight() - 1;
            _ASSERT(nCount >= 1);
			_ASSERT(BUILDING_TABLE_COL_COUNT == aTabFile.GetWidth()+1);
            m_pSetting = new KBuildingTemplate[nCount];
            if (NULL != m_pSetting) 
            {
                for(unsigned int nIndex = 0; nIndex < nCount; nIndex++)
                {
                    const unsigned int nRow = nIndex + 2;
					KBuildingTemplate& aSetting = m_pSetting[nIndex];
					aTabFile.GetInteger(nRow, BUILDING_SPR_NPC, 0,(int*)&(aSetting.uNpcID));
#ifdef _SERVER
					aTabFile.GetString(nRow, BUILDING_SCRIPT_NORMAL, "", aSetting.szScriptFile, sizeof(aSetting.szScriptFile));
#else
					aTabFile.GetString(nRow, BUILDING_NAME, "", aSetting.szName, sizeof(aSetting.szName));
					aTabFile.GetString(nRow, BUILDING_ICON, "", aSetting.szIconFile, sizeof(aSetting.szIconFile));
					aTabFile.GetString(nRow, BUILDING_DESC, "", aSetting.szDescInfo, sizeof(aSetting.szDescInfo));
					aTabFile.GetString(nRow, BUILDING_TIP, "", aSetting.szDescTip, sizeof(aSetting.szDescTip));
					// --> Rocker Edit Start 2005/10/24
					aTabFile.GetString(nRow, BULIDING_SMALL_ICON, "", aSetting.szSmallIconFile, sizeof(aSetting.szSmallIconFile));
					// <-- Rocker End
					int nTemp = 0;
#endif
					aTabFile.GetInteger(nRow, BUILDING_KIND, 0, (int*)&aSetting.uKind);
#ifndef _SERVER
					if (aSetting.uKind == building_kind_totem)
						m_Totem.push_back(nIndex);
					aTabFile.GetInteger(nRow, BUILDING_CANBUILD, 0, &nTemp);
					if (nTemp != 0)
					{
						if (aSetting.uKind == building_kind_plant)
							m_Plant.push_back(nIndex);
						else if (aSetting.uKind == building_kind_normal)
							m_CanBuild.push_back(nIndex);
					}
#endif
                }
                m_nCount = nCount;
                return TRUE;
            }
        }
    }
    return FALSE;
}

//************************************************************************
// 释放资源
//************************************************************************
void KBuildingSetting::Release(void)
{
    if (NULL != m_pSetting) 
    {
        delete[] m_pSetting;
		m_pSetting = NULL;
#ifndef _SERVER
		m_CanBuild.clear();
		m_Totem.clear();
#endif
		m_nCount = 0;
    }
	return;
}


//////////////////////////////////////////////////////////////////////////
// @KBuildingSet

// 自己的唯一实例
KBuildingSet KBuildingSet::s_Self;

//************************************************************************
// 必要的初始化工作
//************************************************************************
KBuildingSet::KBuildingSet()
{
//	Init();
#ifndef _SERVER
	m_uSceneID = 0;
	m_uBuildType = 0xFFFFFFFF;

	m_Image.nFrame = 0;
	m_Image.nType = ISI_T_SPR;
	m_Image.Color.Color_b.a = 255;
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_Image.uImage = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	m_Image.oPosition.nX = 0;
	m_Image.oPosition.nY = 0;
	m_Image.oPosition.nZ = 0;
	
	m_Icon.nFrame = 0;
	m_Icon.nType = ISI_T_SPR;
	m_Icon.Color.Color_b.a = 255;
	m_Icon.bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
	m_Icon.uImage = 0;
	m_Icon.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Icon.bRenderFlag = 0;
	m_Icon.oPosition.nX = 0;
	m_Icon.oPosition.nY = 0;
	m_Icon.oPosition.nZ = 0;
#endif
}

//************************************************************************
// 析构
//************************************************************************
KBuildingSet::~KBuildingSet()
{
	Release();
	return;
}

//************************************************************************
// 初始化
//************************************************************************
BOOL KBuildingSet::Init(void)
{
#ifdef _SERVER
    m_FreeIndexs.Init(MAX_BUILDING);
    m_UsedIndexs.Init(MAX_BUILDING);
	
    for(unsigned int i = MAX_BUILDING - 1; i > 0; i--)
    {
        m_FreeIndexs.Insert(i);
    }
#endif
    return m_Setting.Init();
}

//************************************************************************
// 释放资源
//************************************************************************
void KBuildingSet::Release(void)
{
	// what should i do ?
}

//************************************************************************
//	通过建筑类型获得相关Npc的模板
//	uID 建筑类型
//************************************************************************
const KNpcTemplate* KBuildingSet::GetNpcTemplate(unsigned int uID)
{
	const KBuildingTemplate& aTemplate = m_Setting[uID];	
	const unsigned int nNpcTemplateId = aTemplate.uNpcID;
	
	KNpcTemplate* pNpcTemplate = g_pNpcTemplate[nNpcTemplateId][0];
	if (pNpcTemplate == NULL)
	{
		pNpcTemplate = new KNpcTemplate;
		if (pNpcTemplate != NULL)
		{
			pNpcTemplate->InitNpcBaseData(nNpcTemplateId);
			pNpcTemplate->m_NpcSettingIdx = nNpcTemplateId;
			pNpcTemplate->m_bHaveLoadedFromTemplate = TRUE;
			KLuaScript * pLevelScript = NULL;		
			
#ifdef _SERVER
			pLevelScript = (KLuaScript*)g_GetScript(
				pNpcTemplate->m_dwLevelSettingScript
				);
			
			if (pLevelScript == NULL)
				pLevelScript = g_pNpcLevelScript;
#else
			KLuaScript LevelScript;
			if (!pNpcTemplate->m_szLevelSettingScript[0])
				pLevelScript = g_pNpcLevelScript;
			else
			{
				LevelScript.Init();
				if (!LevelScript.Load(pNpcTemplate->m_szLevelSettingScript))
				{
					g_DebugLog ("[error]致命错误,无法正确读取%s", pNpcTemplate->m_szLevelSettingScript);
					_ASSERT(0);
					pLevelScript = g_pNpcLevelScript;
				}
				else
					pLevelScript = &LevelScript;
			}
#endif
			g_pNpcTemplate[nNpcTemplateId][0] = pNpcTemplate;
		}
	}
	return pNpcTemplate;
}

//************************************************************************
// 检查该类型建筑是否可以放置在指定位置
// nTemplateID 建筑类型
// aPos 地图位置信息
//************************************************************************
BOOL KBuildingSet::CanPut(unsigned long nTemplateID,const KMapPos& aPos, BOOL bCheckTerrain)
{
	const KNpcTemplate* pNpcTemplate = GetNpcTemplate(nTemplateID);
	if (pNpcTemplate != NULL)
	{
		// 测试下建筑尺寸变大是否可以好一些
		const unsigned int nW = pNpcTemplate->m_nBarrierWidth;
		const unsigned int nH = pNpcTemplate->m_nBarrierHeight;
		return SubWorld[aPos.nSubWorld].m_Region[aPos.nRegion].CanBuild(aPos.nSubWorld, aPos.nMapX, aPos.nMapY, nW, nH, bCheckTerrain);		
	}
	return FALSE;
}

//************************************************************************
// 增加一个建筑到世界中
// nTemplateID 建筑类型编号
// aPos 在世界中的位置
//************************************************************************
#ifdef _SERVER
unsigned int KBuildingSet::Add(unsigned long nTemplateID,const KMapPos& aPos)
{
	_ASSERT(nTemplateID < m_Setting.GetCount());

	const int nIndex = m_FreeIndexs.GetNext(0);
	if (nIndex > 0)
	{
//		if (CanPut(nTemplateID, aPos, bCheckTerrain))
		{
			KBuilding& aBuilding = m_Buildings[nIndex];
			if (aBuilding.Init(nTemplateID, nIndex, aPos))
			{
				m_FreeIndexs.Remove(nIndex);
				m_UsedIndexs.Insert(nIndex);
				return nIndex;
			}
		}
	}
	return 0;
}
#endif

//************************************************************************
// 从世界中把指定的建筑移除
// nIndex 指定建筑的编号
//************************************************************************
#ifdef _SERVER
void KBuildingSet::Remove(unsigned int nIndex)
{
    _ASSERT(nIndex <= MAX_BUILDING);

    KBuilding& aBuilding = m_Buildings[nIndex];
    aBuilding.Release();
    m_UsedIndexs.Remove(nIndex);
    m_FreeIndexs.Insert(nIndex);

	return;
}
#endif

//************************************************************************
// 存储指定建筑数据到内存中
// uIndex 建筑索引
// pBuffer 数据指针
// uLen 可写入的最大尺寸,由于实现的要求uLen必须刚好合适
//************************************************************************
#ifdef _SERVER
BOOL KBuildingSet::Save(unsigned int uIndex, void* const pBuffer, unsigned int uLen) const
{
	if (uIndex < MAX_BUILDING && pBuffer != NULL) 
	{
		if (uLen == BUILDING_SAVE_BUFFER_SIZE)
		{
			const KBuilding& aBuilding = m_Buildings[uIndex];
			if (aBuilding.m_pNpc != NULL)
			{
				KBuilding::KBuildingCoreData* const pSaveData = (KBuilding::KBuildingCoreData*)pBuffer;
				pSaveData->nTemplateID = aBuilding.m_nTemplateID;
				pSaveData->dwState = aBuilding.m_dwState;
				pSaveData->nMpsX = aBuilding.m_dwMpsX;
				pSaveData->nMpsY = aBuilding.m_dwMpsY;
				KNpc* const pNpc = aBuilding.m_pNpc;
				pSaveData->nCurLife = pNpc->m_UnaryAttrMgr[nuai_curlife];
				pSaveData->nLevel = pNpc->m_Level;
				//pNpc->GetMpsPos(&pSaveData->nMpsX, &pSaveData->nMpsY);
				return TRUE;
			}
		}		
	}
	return FALSE;
}
#endif

//************************************************************************
// 加载的方式向世界增级建筑
//************************************************************************
#ifdef _SERVER
unsigned int KBuildingSet::Load(unsigned int uSubWorldIdx, const void* pBuffer, unsigned int uLen)
{
	if (pBuffer != NULL && uLen == BUILDING_SAVE_BUFFER_SIZE)
	{
		const KBuilding::KBuildingCoreData* const pSaveData = (const KBuilding::KBuildingCoreData* const)pBuffer;

		KMapPos aMapPos;
		aMapPos.nSubWorld = uSubWorldIdx;
		SubWorld[uSubWorldIdx].Mps2Map(pSaveData->nMpsX, pSaveData->nMpsY, &aMapPos.nRegion, &aMapPos.nMapX, &aMapPos.nMapY, &aMapPos.nOffX, &aMapPos.nOffY);
		const unsigned long uTemplateID = pSaveData->nTemplateID;
	
		const unsigned int uRet = Add(uTemplateID, aMapPos);
		if (uRet > 0 )
		{
			KBuilding& aBuilding = m_Buildings[uRet];
			KNpc* const pNpc = aBuilding.m_pNpc;
			if (pNpc != NULL)
			{
				pNpc->m_UnaryAttrMgr.Set(nuai_curlife, pSaveData->nCurLife);
				pNpc->m_Level = pSaveData->nLevel;
				aBuilding.m_dwState = pSaveData->dwState;
			}
			return uRet;
		}
	}
	return 0;
}
#endif

//************************************************************************
// 绘制当前正尝试建设的建筑
//************************************************************************
#ifndef _SERVER
void KBuildingSet::Draw(void)
{
	g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, FALSE);
}
#endif

//************************************************************************
// 绘制所有建筑图标
// uID 建筑类型
// X, Y 屏幕坐标
//************************************************************************
#ifndef _SERVER
void KBuildingSet::DrawIcon(unsigned int uID, int X, int Y, int nAlpha)
{
	if (uID < m_Setting.GetCount())
	{
		const KBuildingTemplate& aTemplate = m_Setting[uID];
		if (aTemplate.szIconFile[0] != 0)
		{
			strcpy(m_Icon.szImage, aTemplate.szIconFile);
			m_Icon.Color.Color_b.a = nAlpha;
			m_Icon.oPosition.nX = X;
			m_Icon.oPosition.nY = Y;
			m_Icon.nISPosition = IMAGE_IS_POSITION_INIT;
			m_Icon.uImage = 0;
			g_pRepresent->DrawPrimitives(1, &m_Icon, RU_T_IMAGE, TRUE);
		}
	}
}
#endif

//************************************************************************
// 获得建筑物名称
// uID 建筑物的类型
// pBuffer 存放描述的内存区
// uLen 内存区域的尺寸
//************************************************************************
#ifndef _SERVER
BOOL KBuildingSet::GetName(unsigned int uID, char* const pBuffer, unsigned int uLen)
{
	if (uID < m_Setting.GetCount())
	{
		if (pBuffer != NULL && uLen >= KBuildingTemplate::MAX_NAME_SIZE)
		{
			const KBuildingTemplate& aTemplate = m_Setting[uID];
			strcpy(pBuffer, aTemplate.szName);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

//************************************************************************
// 获得简单描述
// uID 建筑物的类型
// pBuffer 存放描述的内存区
// uLen 内存区域的尺寸
//************************************************************************
#ifndef _SERVER
BOOL KBuildingSet::GetDescText(unsigned int uID, char* const pBuffer, unsigned int uLen)
{
	if (uID < m_Setting.GetCount())
	{
		if (pBuffer != NULL && uLen >= KBuildingTemplate::DESC_INFO_SIZE)
		{
			const KBuildingTemplate& aTemplate = m_Setting[uID];
			strcpy(pBuffer, aTemplate.szDescInfo);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

//************************************************************************
// 获得详细描述
// uID 建筑物的类型
// pBuffer 存放描述的内存区
// uLen 内存区域的尺寸
//************************************************************************
#ifndef _SERVER
BOOL KBuildingSet::GetTipText(unsigned int uID, char* const pBuffer, unsigned int uLen)
{
	if (uID < m_Setting.GetCount())
	{
		if (pBuffer != NULL && uLen >= KBuildingTemplate::DESC_TIP_SIZE)
		{
			const KBuildingTemplate& aTemplate = m_Setting[uID];
			strcpy(pBuffer, aTemplate.szDescTip);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

//************************************************************************
// 获得建筑图标路径
// uID 建筑物的类型
// pBuffer 存放描述的内存区
// uLen 内存区域的尺寸
//************************************************************************
#ifndef _SERVER
BOOL KBuildingSet::GetIconName(unsigned int uID, char* const pBuffer, unsigned int uLen)
{
	if (uID < m_Setting.GetCount())
	{
		if (pBuffer != NULL && uLen >= KBuildingTemplate::DESC_TIP_SIZE)
		{
			const KBuildingTemplate& aTemplate = m_Setting[uID];
			strcpy(pBuffer, aTemplate.szIconFile);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

//************************************************************************
// 获得建筑图标路径
// uID 建筑物的类型
// pBuffer 存放描述的内存区
// uLen 内存区域的尺寸
//************************************************************************
#ifndef _SERVER
BOOL KBuildingSet::GetSmallIconName(unsigned int uID, char* const pBuffer, unsigned int uLen)
{
	if (uID < m_Setting.GetCount())
	{
		if (pBuffer != NULL && uLen >= FILE_NAME_LENGTH)
		{
			const KBuildingTemplate& aTemplate = m_Setting[uID];
			strcpy(pBuffer, aTemplate.szSmallIconFile);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

int KBuildingSet::GetBuildingIdByNpcSettingId(int nNpcSettingId)
{
	for (int i=0; i<m_Setting.GetCount(); i++)
	{
		const KBuildingTemplate& aTemplate = m_Setting[i];
		if (aTemplate.uNpcID == nNpcSettingId)
			return i;
	}

	return 0;
}

//************************************************************************
// 客户端准备建筑指定建筑
// nTemplateID 建筑类型,传入的是可建设编号
//************************************************************************
#ifndef _SERVER
void KBuildingSet::BeginBuildBuilding(unsigned int uType,unsigned int nTemplateID)
{
//	_ASSERT(nTemplateID < m_Setting.GetCanBuildCount());

	if (m_uBuildType == 0xFFFFFFFF)
	{
		if (uType == building_kind_normal)
		{
			nTemplateID = m_Setting.BuildOrder2BuildingID(nTemplateID);			
		}
		else if (uType == building_kind_plant)
		{
			nTemplateID = m_Setting.Plant2BuildingID(nTemplateID);
		}
		else
		{
			return;
		}

		if (nTemplateID < m_Setting.GetCount())
		{
			char szNpcTypeName[FILE_NAME_LENGTH];
			const KBuildingTemplate& aTemplate = m_Setting[nTemplateID];
			g_NpcSetting.GetString(aTemplate.uNpcID + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
			if (szNpcTypeName[0] != 0)
			{
				const KNpcResNode* m_pcResNode = g_NpcResList.GetNpcRes(szNpcTypeName);
				if (m_pcResNode != NULL)
				{
					if (m_pcResNode->CheckPartExist(NORMAL_NPC_PART_NO)) 
					{
						m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, 0, 0, "", m_Image.szImage, sizeof(m_Image.szImage));
						if (m_Image.szImage[0] != 0)
						{
							m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
							m_Image.uImage = 0;
							m_uBuildType = nTemplateID;
							KObjAtRegion pInfo;	
							memset( &pInfo, 0, sizeof(KObjAtRegion));
							pInfo.Obj.uGenre = CGOG_BUILDING;
//							CoreDataChanged(GDCNI_BEGIN_BUILD_BUILDING, (unsigned int)&pInfo, 0);
						}
					}				
				}
			}
		}
	}
}
#endif

//************************************************************************
// 看指定位置是否可以建设指定类型的建筑
// nX,nY 准备放置的位置
//************************************************************************
#ifndef _SERVER
void KBuildingSet::TryToBuildBuilding(unsigned int nX, unsigned int nY)
{
	if (m_uBuildType != 0xFFFFFFFF)
	{
		int nSrcX = nX;int nSrcY = nY;
		g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX, nSrcY, 0);
		KMapPos aMapPos = {0};
		SubWorld[0].Mps2Map(nSrcX, nSrcY, &aMapPos.nRegion, &aMapPos.nMapX, &aMapPos.nMapY, &aMapPos.nOffX, &aMapPos.nOffY);
		SubWorld[0].Map2Mps(aMapPos.nRegion, aMapPos.nMapX, aMapPos.nMapY, 0, 0, &nSrcX, &nSrcY);
		if (aMapPos.nRegion >= 0)
		{
			m_Image.oPosition.nX = nSrcX;
			m_Image.oPosition.nY = nSrcY;
			g_ScenePlace.MoveObject(CGOG_BUILDING, 0, nSrcX, nSrcY, 0, m_uSceneID);

			if (CanPut(m_uBuildType, aMapPos, TRUE))
			{
				m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				m_Image.Color.Color_b.a = 0xFF;
			}
			else
			{
				m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
				m_Image.Color.Color_dw = g_pAdjustColorTab[1];
			}
		}
	}
}
#endif

//************************************************************************
// 用户确认的结果
// bCancel 表示是取消还是放置
//************************************************************************
#ifndef _SERVER
void KBuildingSet::ValidateBuildBuilding(BOOL bCancel)
{
	if (m_uBuildType != 0xFFFFFFFF)
	{
		if (bCancel)
		{
			CITY_BUILD_BUILDING tBuildingBuild;
			tBuildingBuild.ProtocolType = c2sfamily_city;
			tBuildingBuild.DataSize = sizeof(tBuildingBuild)-1;
			tBuildingBuild.CityOperatorId = c2s_build_building;
			tBuildingBuild.dwTemplateID = m_uBuildType;
			tBuildingBuild.nMpsX = m_Image.oPosition.nX;
			tBuildingBuild.nMpsY = m_Image.oPosition.nY;
			g_pClient->SendPackToServer(g_ConnectID,&tBuildingBuild, sizeof(tBuildingBuild));
		}
		g_ScenePlace.RemoveObject(CGOG_BUILDING, 0, m_uSceneID);
		m_uSceneID = 0;
		m_uBuildType = 0xFFFFFFFF;
	}
}
#endif

//************************************************************************
// 结束建设操作,通知确认
//************************************************************************
#ifndef _SERVER
void KBuildingSet::EndBuildBuilding(void)
{
	if (m_uBuildType != 0xFFFFFFFF)
	{
		KMapPos aMapPos = {0};
		const int nSrcX = m_Image.oPosition.nX;
		const int nSrcY = m_Image.oPosition.nY;
		SubWorld[0].Mps2Map(nSrcX, nSrcY, &aMapPos.nRegion, &aMapPos.nMapX, &aMapPos.nMapY, &aMapPos.nOffX, &aMapPos.nOffY);
//		char szMsg[64];
//		sprintf(szMsg, "X:%d, Y:%d", nSrcX/32, nSrcY/32);
//		::MessageBox(0,szMsg, "", 0);
		if (CanPut(m_uBuildType, aMapPos, TRUE)) 
		{
//			CoreDataChanged(GDCNI_END_BUILD_BUILDING , 1, m_uBuildType);
		}
		else
		{
			// 把错误信息放到统一的一处
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, "此处不能建设!");
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
//			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			
//			CoreDataChanged(GDCNI_END_BUILD_BUILDING , 0, m_uBuildType);
			g_ScenePlace.RemoveObject(CGOG_BUILDING, 0, m_uSceneID);
			m_uSceneID = 0;
			m_uBuildType = 0xFFFFFFFF;
		}
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
// @KBuilding
#ifdef _SERVER
const KBuildingSetting& KBuilding::s_BuildingSetting = Buildings.GetSetting();
#endif

//************************************************************************
// 实例化建筑
//************************************************************************
#ifdef _SERVER
BOOL KBuilding::Init(unsigned int nTemplateID, unsigned int nIndex, const KMapPos& aPos)
{

	m_dwState = building_build;
	
	// <Add name="Adt.X" time="2005/11/09">
	m_bPreLoad = false;
	// </Add>	
	
	// <Add name="Adt.X" time="2005/12/28">
	m_pCoreData = 0;
	// </Add>

	m_nTemplateID = nTemplateID;

	m_nIndex = nIndex;
	m_Node.m_nIndex = nIndex;
	
	m_pNpc = NULL;

	const int nSubWorldIdx = aPos.nSubWorld;
	const int nRegionIdx = aPos.nRegion;
	const int nMapX = aPos.nMapX;
	const int nMapY = aPos.nMapY;
	const KBuildingTemplate& aTemplate = s_BuildingSetting[m_nTemplateID];	
	if (nRegionIdx >= 0)
	{
		int nNpcIdx = NpcSet.Add(MAKELONG(1, aTemplate.uNpcID), nSubWorldIdx, nRegionIdx, nMapX, nMapY);
		if (nNpcIdx > 0)
		{
			SubWorld[nSubWorldIdx].m_Region[nRegionIdx].AddNpcRef(nNpcIdx);
			const KBuildingTemplate& aTemplate = s_BuildingSetting[m_nTemplateID];	
			m_pNpc = &Npc[nNpcIdx];
			m_pNpc->GetMpsPos((int*)&m_dwMpsX, (int*)&m_dwMpsY);
			m_pNpc->m_ActionScriptID = g_FileName2Id((char*)aTemplate.szScriptFile);
			m_pNpc->m_Kind = kind_building;
			m_pNpc->m_UnaryAttrMgr.Set(nuai_curlife, 1);
			m_dwState = building_build;
			m_pNpc->SetBuildingIdx(nIndex);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

//************************************************************************
// 废弃建筑
//************************************************************************
#ifdef _SERVER
void KBuilding::Release(void)
{
	// Modify by Cooler -->
	// 2006-5-11 16:46
	//if (NULL != m_pNpc) 
	if(NULL != m_pNpc && m_pNpc->m_Kind == kind_building) 
	// End modify by Cooler <--
	{
		//SubWorld[m_pNpc->m_SubWorldIndex].m_WorldMessage.Send(GWM_NPC_DEL, m_pNpc->m_Index);
		
		// --> Rocker Edit Start 2005/07/28 改成立即删除比较妥当
		int nRegion = m_pNpc->m_RegionIndex;
		int nSubWorldIdx = m_pNpc->m_SubWorldIndex;
		SubWorld[nSubWorldIdx].m_Region[nRegion].DecNpcRef(m_pNpc->m_Index);
		SubWorld[nSubWorldIdx].m_Region[nRegion].RemoveNpc(m_pNpc->m_Index);
		NpcSet.Remove(m_pNpc->m_Index);
		// <-- Rocker End


		m_pNpc = NULL;
	}
	m_nIndex = 0;
}
#endif

//************************************************************************
// 建筑激活的必要运算,主要是状态切换
// 理想状态是
//		  |         |<=waring=>burn|
// build=>|normal<=>|              |=>destroy
//        |         |<=normal=>ruin|
//
// 目前只有从build到normal的切换
//************************************************************************
#ifdef _SERVER
void KBuilding::Active(void)
{
	if (m_pNpc != NULL)
	{
		// 状态处理
		switch(m_dwState) 
		{
		case building_build:
			{
				// 修建完成，切换入下一个状态，使脚本生效
				if (m_pNpc->m_UnaryAttrMgr[nuai_curlife] == m_pNpc->m_CompAttrMgr[ncai_lifeuplimit])
				{
					m_dwState = building_normal;				
					BuildingType type = (BuildingType)this->GetSettingID();					
					int nSubWorldIndex = m_pNpc->m_SubWorldIndex;
					if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD) 
					{
						int nSiegeWorldIndex = SubWorld[nSubWorldIndex].m_SiegeWorldIdx;
						if (nSiegeWorldIndex>=0)
						{
							//gSubSiegeWorld[nSiegeWorldIndex].IncBuildingNum(type);
						}
					}					
				}
			}
			break;
		case building_normal:
			{
				if (m_pNpc->m_UnaryAttrMgr[nuai_curlife] == m_pNpc->m_CompAttrMgr[ncai_lifeuplimit])
				{
					m_dwState = building_ruin;
				}
			}
			break;
		case building_ruin:
		case building_burn:
			{
				if (m_pNpc->m_UnaryAttrMgr[nuai_curlife] == m_pNpc->m_CompAttrMgr[ncai_lifeuplimit])
				{
					m_dwState = building_normal;
				}
			}
			break;
		default:
			break;
		}
	}
	return;
}
#endif

//************************************************************************
// 开启障碍,使建筑障碍生效
//************************************************************************
#ifdef _SERVER
void KBuilding::OpenObstacle(void)
{
	if (m_pNpc != NULL)
	{
		m_pNpc->m_bHaveBarrier = TRUE;
	}
}
#endif

//************************************************************************
// 关闭障碍,使建筑障碍失效
//************************************************************************
#ifdef _SERVER
void KBuilding::CloseObstacle(void)
{
	if (m_pNpc != NULL)
	{
		m_pNpc->m_bHaveBarrier = FALSE;
	}
}
#endif

//************************************************************************
// 开始创建变身
// uType 变身的类型
// uProductTime 生产所需时间
//************************************************************************
#ifdef _SERVER
BOOL KBuilding::ProducePolyMorph(unsigned int uType, unsigned int uProductTime)
{
	if (m_pNpc != NULL)
	{
		const int nSubWorld = m_pNpc->m_SubWorldIndex;
		const int nNpcIdx = m_pNpc->m_Index;
		const ProductType nProductType = gSiegeProducer.getProductType(siege_player, uType);
		if (nProductType >=0 )
		{
			if (gSiegeProducer.canProduct(nSubWorld, nProductType))
			{
				gSiegeProducer.createProduct(m_pNpc->m_Index, nProductType, uProductTime);
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif

//************************************************************************
// 对象生产完毕
//************************************************************************
#ifdef _SERVER
void KBuilding::ProduceFinshed(const struct KProduct* pProduct)
{
	if (pProduct != NULL && m_pNpc != NULL)
	{
		m_pNpc->PolyMorph(23, TRUE, 0, 10, 1000);		
	}
}
#endif

//************************************************************************
// 进入建设状态
// uLevel 新的等级
//************************************************************************
#ifdef _SERVER
void KBuilding::BeginChangeLevel(void)
{
//	m_dwState = building_build;
}
#endif

//************************************************************************
// 更改建筑等级 
// uLevel 新的等级
//************************************************************************
#ifdef _SERVER
void KBuilding::EndChangeLevel(unsigned int uLevel)
{
	_ASSERT(m_pNpc->m_Kind == kind_building);
	const unsigned int uNpcLevel = uLevel + 1;
	_ASSERT(uNpcLevel < MAX_LEVEL);
	if (NULL != m_pNpc)
	{
		m_pNpc->m_Level = uNpcLevel;
//		m_dwState = building_normal;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
// @KTerrainInfo
#ifndef _SERVER
KTerrainInfo KTerrainInfo::s_Self;

#define TERRIAN_ICON_FILE	"\\settings\\terrains.txt"
enum
{
	TERRIAN_ID = 1,
	TERRIAN_ICON,
	TERRIAN_DESC,
	TERRIAN_TABLE_COL_COUNT,
};
#endif
//************************************************************************
// 构造函数
//************************************************************************
#ifndef _SERVER
KTerrainInfo::KTerrainInfo()
{
	m_uInfoCount = 0;
	m_pSettings = NULL;

	m_Icon.nFrame = 0;
	m_Icon.nType = ISI_T_SPR;
	m_Icon.Color.Color_b.a = 255;
	m_Icon.bRenderStyle = IMAGE_RENDER_STYLE_3LEVEL;
	m_Icon.uImage = 0;
	m_Icon.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Icon.bRenderFlag = 0;
	m_Icon.oPosition.nX = 0;
	m_Icon.oPosition.nY = 0;
	m_Icon.oPosition.nZ = 0;
//	Init();
}
#endif

//************************************************************************
// 析构函数
//************************************************************************
#ifndef _SERVER
KTerrainInfo::~KTerrainInfo()
{
	Release();
}
#endif

//************************************************************************
//  初始化工作
//************************************************************************
#ifndef _SERVER
BOOL KTerrainInfo::Init(void)
{
    if (NULL == m_pSettings) 
    {
        KTabFile aTabFile;
		g_SetRootPath(NULL);
        if (aTabFile.Load(TERRIAN_ICON_FILE))
        {
            const unsigned int nCount = aTabFile.GetHeight() - 1;
            _ASSERT(nCount >= 1);
			_ASSERT(TERRIAN_TABLE_COL_COUNT == aTabFile.GetWidth()+1);
            m_pSettings = new KTerrainInfo::TerrainInfo_t[nCount];
            if (NULL != m_pSettings) 
            {
                for(unsigned int nIndex = 0; nIndex < nCount; nIndex++)
                {
                    const unsigned int nRow = nIndex + 2;
					KTerrainInfo::TerrainInfo_t& aIcon = m_pSettings[nIndex];
					aTabFile.GetString(nRow, TERRIAN_ICON, "", aIcon.szIcon, sizeof(aIcon.szIcon));
					aTabFile.GetString(nRow, TERRIAN_DESC, "", aIcon.szDesc, sizeof(aIcon.szDesc));
                }
                m_uInfoCount = nCount;
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif

//************************************************************************
// 释放工作
//************************************************************************
#ifndef _SERVER
BOOL KTerrainInfo::Release(void)
{
	if (m_pSettings != NULL)
	{
		delete[] m_pSettings;
		m_pSettings=NULL;
		m_uInfoCount = 0;
		return TRUE;
	}
	return FALSE;
}
#endif

//************************************************************************
// 绘制图标
// uID 类型编号
// x, y 位置
// nAlpha 透明度 
//************************************************************************
#ifndef _SERVER
void KTerrainInfo::DrawIcon(unsigned int uID, int X, int Y, int nAlpha)
{
	if (uID >= 0 && uID < m_uInfoCount)
	{
		const KTerrainInfo::TerrainInfo_t& aTemplate = m_pSettings[uID];
		if (aTemplate.szIcon[0] != 0)
		{
			strcpy(m_Icon.szImage, aTemplate.szIcon);
			m_Icon.Color.Color_b.a = nAlpha;
			m_Icon.oPosition.nX = X;
			m_Icon.oPosition.nY = Y;
			m_Icon.nISPosition = IMAGE_IS_POSITION_INIT;
			m_Icon.uImage = 0;
			g_pRepresent->DrawPrimitives(1, &m_Icon, RU_T_IMAGE, TRUE);
		}
	}	
}
#endif

//************************************************************************
// 取得地形描述 
// uID 地形编号
// pBuffer 字符缓冲
// uLen 缓冲区尺寸
//************************************************************************
#ifndef _SERVER
KTerrainInfo::GetDescText(unsigned int uID, char* const pBuffer, unsigned int uLen)
{
	if (uID < m_uInfoCount)
	{
		if (pBuffer != NULL && uLen >= KTerrainInfo::TerrainInfo_t::DESC_INFO_SIZE)
		{
			const KTerrainInfo::TerrainInfo_t& aIcon = m_pSettings[uID];
			strcpy(pBuffer, aIcon.szDesc);
			return TRUE;
		}
	}
	return FALSE;
}
#endif


// end of file
