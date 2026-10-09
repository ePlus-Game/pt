//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-30
//      File_base        : npc_save
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : NPC存盘
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "CoreUtil.h"
#include "cfs_fs2_savedef.h"
#include "cfs_db_interface.h"
#include "KNpcSet.h"
#include "KNpc.h"
#include "KNpcTemplate.h"
#include "npc_save.h"
#include "buff_man.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"

#define MAX_PROCESSING_NPC_COUNT 32//最大同时处理的NPC数量

#define	COL_LIFE_PERCENT			4
#define	COL_MANA_PERCENT			5
#define	COL_LORD_GUID				6
#define COL_ROBBER_GUID				7
#define	COL_CITY_TAXRATE			8
#define	COL_NPC_RES					9
#define	COL_NPC_BUFF				10

struct NpcUnaryAttr
{
	int	nResType;
	int	nVal;
};

struct NpcSavedRes
{
	NpcUnaryAttr	npcRes[CITY_RES_TYPE_COUNT];
};

//---------------------------------new
struct _NpcDBHeader : _DBProcHeader 
{
	int nOp;
	int nNpcID;
};
//---------------------------------new

int NpcSave::s_ProcessingNpcCount = 0;
bool NpcSave::m_IsGlobalNpcInfoLoaded = false;
NpcSave::MemGlobalNpcSaveInfo NpcSave::m_GlobalNpcItem[NpcSave::__max_globalnpcitem_num];

bool NpcSave::IsBusy()
{
	return (s_ProcessingNpcCount > MAX_PROCESSING_NPC_COUNT);
}

void NpcSave::AddTask()
{
	s_ProcessingNpcCount++;
}

void NpcSave::RemoveTask()
{
	s_ProcessingNpcCount--;
}

void NpcSave::DbOpComplete( int dbOpResult, IProcRet* pRet )
{
	int nSize = 0;
	char* pPassBy = pRet->GetPassBy( nSize );

	if( pPassBy == NULL || 
		nSize == 0 || 
		nSize != sizeof(_NpcDBHeader) )
		return;

	_NpcDBHeader* pHeader = (_NpcDBHeader*)pPassBy;

				
	switch( pHeader->nOp )
	{
	case npc_save_db_op_load:
		LoadNpcComplete( dbOpResult, pPassBy, pRet );
		break;
	case npc_save_db_op_save:
		SaveNpcComplete( dbOpResult, pPassBy, pRet );
		break;
	case npc_save_db_op_delete:
		DeleteNpcComplete( dbOpResult, pPassBy, pRet );
		break;
	default:
		break;
	}

	RemoveTask();
}

bool NpcSave::LoadNpc(int npcIndex)
{
	if (npcIndex <= 0 || npcIndex >= MAX_NPC)
		return false;

	KNpc& npc = Npc[npcIndex];	
	const KNpcTemplate* pTemplate = npc.GetTemplate();
	if (pTemplate != NULL)
	{
		if (pTemplate->NeedSave())
		{
			_NpcDBHeader DBHeader;
			memset( &DBHeader, 0, sizeof(DBHeader) );
			
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_Npc;
			DBHeader.nOp = npc_save_db_op_load;
			DBHeader.nNpcID	= npc.GetId( );
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			//begin
			pParam->BeginPush( PN_LOADNPC );
			
			pParam->Push( pTemplate->GetGUID( ).data );
			pParam->Push( SubWorld[npc.GetSubWorldIndex()].m_SubWorldID );
			
			//end push
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			if( g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam ) )
			{
				npc.SetLoadSaveState(npc_load_save_state_loading);
				AddTask();
				return true;
			}
		}
	}

	return false;
}

void NpcSave::LoadNpcComplete( int dbOpResult, char* pPassBy, IProcRet* pRet )
{	
	_NpcDBHeader* pHeader = (_NpcDBHeader*)pPassBy;

	int npcIndex = NpcSet.SearchID( pHeader->nNpcID );	
	if (npcIndex <= 0 || npcIndex >= MAX_NPC)
		return;
	
	KNpc& npc = Npc[npcIndex];
	const KNpcTemplate* pTemplate = npc.GetTemplate();

	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_load_npc))
	{
		LogEventParam logParam;
		logParam.event = log_event_npc_save_op_load;
		logParam.param1 = pTemplate->GetGUID( );
		g_pLogSystem->Log(logParam);
	}
	
	if ( dbOpResult )
	{
		npc.SetLoadSaveState(npc_load_save_state_loaded);

		int rowCount = pRet->GetRowCount();
		int colCount = pRet->GetColCount();
		
		if (rowCount == 1)
		{	
			char* pColData;
			int colDataSize;

			int lifePercentage = 0;	
			int manaPercentage = 0;
			int	nCityTaxRate = 0;
			int nCityGoodsDiscount = 100;

			pRet->GetData(0, COL_LIFE_PERCENT, lifePercentage);		
			npc.SetCurrentLifePercentage(lifePercentage);

			pRet->GetData(0, COL_MANA_PERCENT, manaPercentage);	
			npc.SetCurrentManaPercentage(manaPercentage);

			// 如果colDataSize = 0，表示此GUID为空

			FSGUID Guid;

			pRet->GetData(0, COL_LORD_GUID, Guid.data, sizeof(Guid.data));
			npc.SetLord(Guid);

			pRet->GetData(0, COL_ROBBER_GUID, Guid.data, sizeof(Guid.data));
			npc.SetRobber(Guid);

			pRet->GetData(0, COL_CITY_TAXRATE, nCityTaxRate );	
			if(nCityTaxRate < 0 || nCityTaxRate > 100)
			{
				_ASSERT(false);
				nCityTaxRate = DEFAULT_CITY_DISCOUNT;
			}
			npc.m_UnaryAttrMgr.Set(nuai_city_taxrate, nCityTaxRate);

			colDataSize = pRet->GetData(0, COL_NPC_BUFF, &pColData );
			BuffMgr::Singleton().LoadBuff(npcIndex, (unsigned char*)pColData, colDataSize);

			colDataSize = pRet->GetData(0, COL_NPC_RES, &pColData );
			if( colDataSize == sizeof(NpcSavedRes) )
			{
				NpcSavedRes	*pSaveRes = (NpcSavedRes*)pColData;
				
				for(int nType = 0; nType < CITY_RES_TYPE_COUNT; ++nType)
					npc.m_UnaryAttrMgr.Set(pSaveRes->npcRes[nType].nResType, pSaveRes->npcRes[nType].nVal);
			}
			else
			{
				_ASSERT(false);
				goto PARSE_ERROR;
			}
					
			return;	

PARSE_ERROR:
#ifdef _DEBUG
			CFS_FILELOGS::WriteDebugLog("[Error]: LoadNpc: parser data error!!!");
#endif
			return;
		}		
	}
	else
	{
		npc.SetLoadSaveState(npc_load_save_state_waiting);	
	}
}

bool NpcSave::SaveNpc(int npcIndex)
{
	if (npcIndex <= 0 || npcIndex >= MAX_NPC)
		return false;

	KNpc& npc = Npc[npcIndex];	
	const KNpcTemplate* pTemplate = npc.GetTemplate();
	if (pTemplate != NULL)
	{
		if (pTemplate->NeedSave())
		{
			if( (npc.m_UnaryAttrMgr[nuai_deathmode] & npc_deathmode_autodel) && m_IsGlobalNpcInfoLoaded)
				SaveNpcGlobalInfo(npcIndex);

			NpcSavedRes NpcRes = {0};

			for(int nResType = 0; nResType < CITY_RES_TYPE_COUNT; ++nResType)
			{
				NpcRes.npcRes[nResType].nResType = nResType + nuai_lord_res0;
				NpcRes.npcRes[nResType].nVal = npc.m_UnaryAttrMgr[nResType + nuai_lord_res0];
			}

			static const int NPC_SAVEBUF_SIZE = 8192;
			int nSaveSize = NPC_SAVEBUF_SIZE;
			char	npcSaveBuf[NPC_SAVEBUF_SIZE];

			BuffMgr::Singleton().SaveBuff(
				npcIndex, 
				(unsigned char*)npcSaveBuf, 
				nSaveSize );

			_NpcDBHeader DBHeader;
			memset( &DBHeader, 0, sizeof(DBHeader) );

			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_Npc;
			DBHeader.nOp = npc_save_db_op_save;
			DBHeader.nNpcID	= npc.GetId( );
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			//begin
			pParam->BeginPush( PN_SAVENPC );
			
			pParam->Push( pTemplate->GetGUID( ).data );
			pParam->Push( SubWorld[npc.GetSubWorldIndex()].m_SubWorldID );
			pParam->Push( npc.GetMapX() );
			pParam->Push( npc.GetMapY() );
			pParam->Push( npc.GetCurrentLifePercentage() );
			pParam->Push( npc.GetCurrentManaPercentage() );
			pParam->Push( npc.GetLord().data );
			pParam->Push( npc.GetRobber().data );
			pParam->Push( npc.m_UnaryAttrMgr[nuai_city_taxrate] );
			pParam->Push( BinPair( (void*)&NpcRes, sizeof(NpcRes) ) );
			pParam->Push( BinPair( npcSaveBuf, nSaveSize ) );

			//end push
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			if( g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam ) )
			{
				npc.SetLoadSaveState(npc_load_save_state_saving);
				AddTask();
				
				return true;
			}
		}
	}

	return false;
}

void NpcSave::SaveNpcComplete(int dbOpResult, char* pPassBy, IProcRet* pRet )
{
	_NpcDBHeader* pHeader = (_NpcDBHeader*)pPassBy;
	
	int npcIndex = NpcSet.SearchID(pHeader->nNpcID);
	if (npcIndex <= 0 || npcIndex >= MAX_NPC)
		return;

	KNpc& npc = Npc[npcIndex];
	const KNpcTemplate* pTemplate = npc.GetTemplate();

	npc.SetLoadSaveState(npc_load_save_state_loaded);

	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_load_npc))
	{
		LogEventParam logParam;
		logParam.event = log_event_npc_save_op_save;
		logParam.param1 = pTemplate->GetGUID( );
		g_pLogSystem->Log(logParam);
	}

}

bool NpcSave::DeleteNpc(int npcIndex)
{
	if (npcIndex <= 0 || npcIndex >= MAX_NPC)
		return false;

	KNpc& npc = Npc[npcIndex];	
	const KNpcTemplate* pTemplate = npc.GetTemplate();
	if (pTemplate != NULL)
	{
		if (pTemplate->NeedSave())
		{
			if( (npc.m_UnaryAttrMgr[nuai_deathmode] & npc_deathmode_autodel) && m_IsGlobalNpcInfoLoaded )
				DeleteNpcGlobalInfo(npcIndex);

			npc.SetLoadSaveState(npc_load_save_state_deleted);

			_NpcDBHeader DBHeader;
			memset( &DBHeader, 0, sizeof(DBHeader) );
			
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_Npc;
			DBHeader.nOp = npc_save_db_op_delete;
			DBHeader.nNpcID	= npc.GetId( );
			
			IProcParam* pParam = g_pController->GetProcParam( );
			
			//begin
			pParam->BeginPush( PN_DELETENPC );
			
			pParam->Push( pTemplate->GetGUID( ).data );
			pParam->Push( SubWorld[npc.GetSubWorldIndex()].m_SubWorldID );
			
			//end push
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			if( g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam ) )
			{
				AddTask();
				
				return true;
			}
		}
	}

	return false;
}

void NpcSave::DeleteNpcComplete(int dbOpResult, char* pPassBy, IProcRet* pRet )
{
	_NpcDBHeader* pHeader = (_NpcDBHeader*)pPassBy;	

	int npcIndex = NpcSet.SearchID(pHeader->nNpcID);
	if (npcIndex <= 0 || npcIndex >= MAX_NPC)
		return;
	
	KNpc& npc = Npc[npcIndex];	
	const KNpcTemplate* pTemplate = npc.GetTemplate();
	
	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_delete_npc))
	{
		LogEventParam logParam;
		logParam.event = log_event_npc_save_op_delete;
		logParam.param1 = pTemplate->GetGUID( );
		g_pLogSystem->Log(logParam);
	}

}

bool NpcSave::IsHaveSaved(int npcIndex)
{
	return -1 != GetNpcItemPos(npcIndex);
}

#define SAVEITEMBUFLEN 8192

bool NpcSave::SaveNpcGlobalInfo(int npcIndex)
{
	// 非 -1 表示存储这个npc，-1表示刷新数据
	if(-1 != npcIndex)
	{
		if( IsHaveSaved(npcIndex) )
		return true;

		int nEmptyPos = GetEmptyNpcItemPos();
		
		if(-1 == nEmptyPos)
			return false;

		GlobalNpcSaveItem saveItem;
		saveItem.npcIndexInfo = MAKELONG(Npc[npcIndex].m_Level, Npc[npcIndex].m_NpcSettingIdx);
		saveItem.subworldId = SubWorld[Npc[npcIndex].GetSubWorldIndex()].m_SubWorldID;
		Npc[npcIndex].GetMpsPos(&saveItem.xPos, &saveItem.yPos);

		m_GlobalNpcItem[nEmptyPos].saveItem = saveItem;
		m_GlobalNpcItem[nEmptyPos].isValid = true;
	}

	char szBuf[SAVEITEMBUFLEN];
	DBGlobalNpcSaveInfo *pSaveInfo = (DBGlobalNpcSaveInfo*)szBuf;
	pSaveInfo->count = 0;
	GlobalNpcSaveItem *pSaveItem = pSaveInfo->item;
	
	for(int nIdx = 0; nIdx < __max_globalnpcitem_num; ++nIdx)
	{
		if(m_GlobalNpcItem[nIdx].isValid)
		{
			*pSaveItem = m_GlobalNpcItem[nIdx].saveItem;
			++pSaveItem;
			++pSaveInfo->count;
		}
	}

	int nSaveSize = sizeof(DBGlobalNpcSaveInfo) - sizeof(GlobalNpcSaveItem);
	nSaveSize += pSaveInfo->count * sizeof(GlobalNpcSaveItem);

	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_npcsave;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	pParam->Push( BinPair( szBuf, nSaveSize ) );

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam ) ? true : false;
}

bool NpcSave::LoadGlobalNpcInfo()
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_npcsave;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_GETGLOBAL );

	pParam->Push( DBHeader.nGlobalID );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam ) ? true : false;
}

void NpcSave::LoadGlobalNpcInfoRet(int nDBOpeRst, int nDataSize, char *pData)
{
	m_IsGlobalNpcInfoLoaded = true;

	if( nDBOpeRst && pData != NULL )
	{
		DBGlobalNpcSaveInfo *pSaveInfo = (DBGlobalNpcSaveInfo*)pData;

		for(int nLoop = 0; nLoop < pSaveInfo->count; ++nLoop)
		{
			int nWorldIdx = g_SubWorldSet.SearchWorld(pSaveInfo->item[nLoop].subworldId);
			
			if(-1 != nWorldIdx)
			{
				int nNpcIdx = NpcSet.Add(pSaveInfo->item[nLoop].npcIndexInfo,
					nWorldIdx,
					pSaveInfo->item[nLoop].xPos,
					pSaveInfo->item[nLoop].yPos
					);
				
				int nMode = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_deathmode];
				nMode |= npc_deathmode_autodel;
				Npc[nNpcIdx].m_UnaryAttrMgr.Set( nuai_deathmode, nMode );
				
				if(IsValidNpc(nNpcIdx))
				{
					if( IsHaveSaved(nNpcIdx) )
						continue;
					
					int nEmptyPos = GetEmptyNpcItemPos();
					
					if(-1 == nEmptyPos)
						continue;
					
					m_GlobalNpcItem[nEmptyPos].saveItem = pSaveInfo->item[nLoop];
					m_GlobalNpcItem[nEmptyPos].isValid = true;
				}//endif
			}
		}
	}
}

bool NpcSave::DeleteNpcGlobalInfo(int npcIndex)
{
	int nPos = GetNpcItemPos(npcIndex);

	if(-1 != nPos)
	{
		m_GlobalNpcItem[nPos].isValid = false;
		SaveNpcGlobalInfo( -1 );
		return true;
	}
	else
		return false;
}

int NpcSave::GetNpcItemPos(int npcIndex)
{
	int nNpcIdxInfo = MAKELONG(Npc[npcIndex].m_Level, Npc[npcIndex].m_NpcSettingIdx);
	int nWorldId = SubWorld[Npc[npcIndex].m_SubWorldIndex].m_SubWorldID;

	for(int nLoop = 0; nLoop < __max_globalnpcitem_num; ++nLoop)
	{
		if(m_GlobalNpcItem[nLoop].isValid)
		{
			if(m_GlobalNpcItem[nLoop].saveItem.npcIndexInfo == nNpcIdxInfo &&
				m_GlobalNpcItem[nLoop].saveItem.subworldId == nWorldId)
				return nLoop;
		}
	}

	return -1;
}

int NpcSave::GetEmptyNpcItemPos()
{
	for(int nPos = 0; nPos < __max_globalnpcitem_num; ++nPos)
	{
		if(!m_GlobalNpcItem[nPos].isValid)
			return nPos;
	}

	return -1;
}

void NpcSave::SaveAllNpc()
{
	int npcIndex = 0;
	while (npcIndex = NpcSet.GetNextIdx(npcIndex))
	{
		KNpc& saveNpc = Npc[npcIndex];
		if(NULL != saveNpc.GetTemplate()
			&& saveNpc.GetTemplate()->NeedSave() 
			&& npc_load_save_state_loaded == saveNpc.GetLoadSaveState())
		{
			saveNpc.Save();
		}
	}

	if (m_IsGlobalNpcInfoLoaded)
		SaveNpcGlobalInfo(-1);      //刷新NPCGlobal数据
}