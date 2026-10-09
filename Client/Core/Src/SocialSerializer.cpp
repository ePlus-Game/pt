//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:19   15:31
//      File_base        : SocialSerializer
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "SocialSerializer.h"
#include "SocialUnit.h"
#include "KSubWorldSet.h"

int	ChangeRecord(
	int nDBOpeType, 
	const char *szProcName, 
	SocialUnit* pUnit,
	unsigned long ulNetID )
{
	if( pUnit == NULL )
		return FALSE;

	// 玩家节点不是记录单位
	if( pUnit->GetLayer() <= enSULayer_Player )
	{
		_ASSERT(false);
		return FALSE;
	}

	SocialUnit	*pParentUnit = pUnit->GetParent();
	SocialUnitAttr	&attr = pUnit->GetUnitAttr();
	char *pNodeName = NULL;
	int	 nAttrSize = attr.GetAttr(enSUAttr_UnitName, pNodeName);

	if( pNodeName == NULL || nAttrSize == 0 )
		return FALSE;

	char szSaveBuf[SAVETEMPBUFLEN];
	
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= ulNetID;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= nDBOpeType;
	DBHeader.ntplId		= pUnit->GetTplId( );
	DBHeader.Guid		= pUnit->GetUnitGuid();
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( szProcName );
	
	pParam->Push( pUnit->GetTplId() );//paramTemplateId		TPLID_BUF
	pParam->Push( pUnit->GetLayer() );//paramLayerId			LAYERID_BUF
	pParam->Push( pUnit->GetUnitGuid().data );//paramNodeGUID			UNITGUID_BUF

	if( pParentUnit == NULL )
		pParam->Push( "" );//paramParentGUID		PGUID_BUF
	else
		pParam->Push( pParentUnit->GetUnitGuid( ).data );//paramParentGUID		PGUID_BUF

	pParam->Push( pNodeName );//paramNodeName			UNITNAME_BUF

	pParam->Push( pUnit->GetOwnerName( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( pUnit->GetChildCount( ) );//paramChildCount		SUBUNITCNT_BUF

	int	nSaveSize = attr.SaveAttrToBuf( szSaveBuf, sizeof(szSaveBuf) );
	pParam->Push( BinPair( szSaveBuf, nSaveSize ) );//paramAttributeData	UNITATTR_BUF

	nSaveSize = SaveSubUnitsToBuf( szSaveBuf , sizeof(szSaveBuf), pUnit );
	pParam->Push( BinPair( szSaveBuf, nSaveSize ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
	
#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillUpdateRecordReq() failed!");
#endif

	return nRet;
}

SocialSerializer& SocialSerializer::Singleton()
{
	static SocialSerializer	ss;

	return ss;
}

void SocialSerializer::LoadAllSubUnitReq(
	int nNetId, 
	int nTplId, 
	int nParentLayer, 
	const FSGUID &parentGuid)
{
	if( !m_dbTaskList.AddTask(parentGuid) )
		return;

	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_LoadAllSubUnit;
	DBHeader.ntplId		= nTplId;
	DBHeader.Guid		= parentGuid;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SEARCHUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF

	if(nParentLayer == enSULayer_Player + 1)
	{
		pParam->Push( parentGuid.data );//paramNodeGUID			UNITGUID_BUF
		pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	}
	else if(nParentLayer > enSULayer_Player + 1)
	{
		pParam->Push( NullPair( ) );//paramNodeGUID			UNITGUID_BUF
		pParam->Push( parentGuid.data );//paramParentGUID		PGUID_BUF
	}
	else
	{
		pParam->Push( NullPair( ) );//paramNodeGUID			UNITGUID_BUF
		pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	}

	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );

#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillAllSubUnitReq() failed!");
#endif
}

void SocialSerializer::AddRecordReq(int nNetId, SocialUnit *pUnit)
{
	int nRet = ChangeRecord( 
		enSoc_DBOpe_AddUnit,
		PN_ADDUNIT, pUnit, nNetId );
	
#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillAddRecordReq() failed!");
#endif
}


void SocialSerializer::UpdateRecordReq(int nNetId, SocialUnit *pUnit)
{
	int nRet = ChangeRecord( 
		enSoc_DBOpe_UpdateUnit,
		PN_UPDATEUNIT, pUnit, nNetId );
}

void SocialSerializer::RemoveRecordReq(int nNetId, SocialUnit *pUnit)
{
	if( pUnit == NULL )
		return;
	
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_RemoveUnit;
	DBHeader.ntplId		= pUnit->GetTplId( );
	DBHeader.Guid		= pUnit->GetUnitGuid( );
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_REMOVEUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( pUnit->GetUnitGuid( ).data );//paramNodeGUID			UNITGUID_BUF
	pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_dblink, pParam );

#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillRemoveRecordReq() failed!");
#endif
}

void SocialSerializer::UpdatePGuidReq(int nNetId, SocialUnit *pUnit)
{
	if( pUnit == NULL )
		return;

	// 玩家节点不是记录单位
	if( pUnit->GetLayer() <= enSULayer_Player )
	{
		_ASSERT(false);
		return;
	}

	SocialUnit	*pParentUnit = pUnit->GetParent();
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_UpdatePGuid;
	DBHeader.ntplId		= pUnit->GetTplId( );
	DBHeader.Guid		= pUnit->GetUnitGuid( );
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_UPDATEUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( pUnit->GetUnitGuid( ).data );//paramNodeGUID			UNITGUID_BUF

	if( pParentUnit == NULL )
		pParam->Push( "" );//paramParentGUID		PGUID_BUF
	else
		pParam->Push( pParentUnit->GetUnitGuid( ).data );//paramParentGUID		PGUID_BUF

	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
	
#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillUpdatePGuidReq() failed!");
#endif
}

void SocialSerializer::UpdateAttrReq(int nNetId, SocialUnit *pUnit)
{
	if( pUnit == NULL )
		return;
	
	// 玩家节点不是记录单位
	if( pUnit->GetLayer() <= enSULayer_Player )
	{
		_ASSERT(false);
		return;
	}

	SocialUnitAttr	&attr = pUnit->GetUnitAttr();
	char szSaveBuf[SAVETEMPBUFLEN];

	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_UpdateAttr;
	DBHeader.ntplId		= pUnit->GetTplId( );
	DBHeader.Guid		= pUnit->GetUnitGuid( );
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_UPDATEUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( pUnit->GetUnitGuid( ).data );//paramNodeGUID			UNITGUID_BUF
	pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF

	int	nSaveSize = attr.SaveAttrToBuf( szSaveBuf, sizeof(szSaveBuf) );
	pParam->Push( BinPair( szSaveBuf, nSaveSize ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
	
#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillUpdateAttrReq() failed!");
#endif
}

void SocialSerializer::UpdateAppDataReq(int nNetId, SocialUnit *pUnit)
{
	if( pUnit == NULL )
		return;
	
	// 玩家节点不是记录单位
	if( pUnit->GetLayer() <= enSULayer_Player )
	{
		_ASSERT(false);
		return;
	}
	
	SocialUnitAttr	&attr = pUnit->GetUnitAttr();
	char szSaveBuf[SAVETEMPBUFLEN];
	
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_UpdateAppData;
	DBHeader.ntplId		= pUnit->GetTplId( );
	DBHeader.Guid		= pUnit->GetUnitGuid( );
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_UPDATEUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( pUnit->GetUnitGuid( ).data );//paramNodeGUID			UNITGUID_BUF
	pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF

	int nSaveSize = SaveSubUnitsToBuf( szSaveBuf , sizeof(szSaveBuf), pUnit );
	pParam->Push( BinPair( szSaveBuf, nSaveSize ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );

#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillUpdateAppDataReq() failed!");
#endif
}

void	SocialSerializer::UpdateSubUnitCntReq(int nNetId, SocialUnit *pUnit)
{
	if( pUnit == NULL )
		return;
	
	// 玩家节点不是记录单位
	if( pUnit->GetLayer() <= enSULayer_Player )
	{
		_ASSERT(false);
		return;
	}
	
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_UpdateSUCnt;
	DBHeader.ntplId		= pUnit->GetTplId( );
	DBHeader.Guid		= pUnit->GetUnitGuid( );
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_UPDATEUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( pUnit->GetUnitGuid( ).data );//paramNodeGUID			UNITGUID_BUF
	pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( pUnit->GetChildCount( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );

#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillUpdateSUCntReq() failed!");
#endif
}

void SocialSerializer::CheckUnitNameReq(int nNetId, const C2S_CREATEUNIT_REQ &c2sReq)
{
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_CheckUnitName;
	DBHeader.ntplId		= 0;
	DBHeader.Req		= c2sReq;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_COUNTUNIT );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( NullPair( ) );//paramNodeGUID			UNITGUID_BUF
	pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	pParam->Push( c2sReq.unitName );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_dblink, pParam );
	
#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillCheckUnitNameReq() failed!");
#endif
}

void SocialSerializer::LoadTreeUpReq(int nNetId, int nTplId, const FSGUID &guid)
{
	if( !m_dbTaskList.AddTask(guid) )
		return;
	
	_SocialDBHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	= nNetId;
	DBHeader.ProcType	= Proc_Social;
	DBHeader.nOp		= enSoc_DBOpe_LoadOwnTree;
	DBHeader.ntplId		= nTplId;
	DBHeader.Guid		= guid;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SEARCHTREEUP );
	
	pParam->Push( NullPair( ) );//paramTemplateId		TPLID_BUF
	pParam->Push( NullPair( ) );//paramLayerId			LAYERID_BUF
	pParam->Push( guid.data );//paramNodeGUID			UNITGUID_BUF
	pParam->Push( NullPair( ) );//paramParentGUID		PGUID_BUF
	pParam->Push( NullPair( ) );//paramNodeName			UNITNAME_BUF
	pParam->Push( NullPair( ) );//paramLeaderName		OWNERNAME_BUF
	pParam->Push( NullPair( ) );//paramChildCount		SUBUNITCNT_BUF
	pParam->Push( NullPair( ) );//paramAttributeData	UNITATTR_BUF
	pParam->Push( NullPair( ) );//paramPlusData			APPENDDATA_BUF
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );

#ifdef _DEBUG
	if(TRUE != nRet)
		CFS_FILELOGS::WriteDebugLog("SocialSerializer::FillLoadTreeUpReq() failed!");
#endif
}

bool SocDBTaskList::AddTask(const FSGUID &guid)
{
	DWORD	dwTime = g_SubWorldSet.GetGameTime();

	int		nEmptyPos = -1;

	// 检测是否有相同的GUID
	for(int nLoop = 0; nLoop < MAXNUM_DBTASK; ++nLoop)
	{
		if(m_taskList[nLoop].isInUse)
		{
			if(m_taskList[nLoop].id == guid)
			{
				if(dwTime - m_taskList[nLoop].startTime >= DBTASK_TIMEOUT)
				{
					nEmptyPos = nLoop;
					break;
				}
				else
					return false;
			}
		}
		else
		{
			if(-1 == nEmptyPos)
				nEmptyPos = nLoop;
		}
	}

	if(-1 != nEmptyPos)
	{
		m_taskList[nEmptyPos].isInUse = true;
		m_taskList[nEmptyPos].startTime = dwTime;
		memcpy(&m_taskList[nEmptyPos].id, &guid, sizeof(guid));

		return true;
	}
	else
		return false;
}

void SocDBTaskList::RemoveTask(const FSGUID &guid)
{
	for(int nLoop = 0; nLoop < MAXNUM_DBTASK; ++nLoop)
	{
		if(!m_taskList[nLoop].isInUse)
			continue;

		if(m_taskList[nLoop].id == guid)
		{
			m_taskList[nLoop].isInUse = false;
			break;
		}	
	}
}