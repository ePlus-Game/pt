//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-06-26 17:24
//      File_base        : PlayerCreator.cpp
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : Create New Player Function
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "PlayerCreator.h"
#include "BaseValue.h"
#include "ItemCommonDef.h"
#include "time.h"
#include "KItem.h"
#include "KItemGenerator.h"
#include "cfs_db_interface.h"
#include "ConfigManager.h"
#include "kItemdateparser.h"
/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

PlayerCreator::_re_pos	PlayerCreator::m_sRevivalPos[MAX_TOTAL_COUNT_REVIVAL] = {0};
char					PlayerCreator::m_szRoleTemplateBuf[MAX_ROLE_COUNT * NEWROLELEN] = {0};
char*					PlayerCreator::m_pBuf[MAX_ROLE_COUNT]	=	{0};

/////////////////////////////////////////////////////////////////////////////
//
//              Function Definitions
//
/////////////////////////////////////////////////////////////////////////////

PlayerCreator& PlayerCreator::Singleton( )
{
	static PlayerCreator PC;
	
	return 	PC;
}

int PlayerCreator::Init( )
{
	int nRoleType;
	int nLoopCount;
	char szFileName[MAX_PATH];

	g_SetRootPath(".\\");

	for( nLoopCount = 0; nLoopCount < MAX_ROLE_COUNT; nLoopCount++ )
		m_pBuf[nLoopCount] = &m_szRoleTemplateBuf[nLoopCount * NEWROLELEN];

	for( nLoopCount = 0; nLoopCount < MAX_ROLE_COUNT; nLoopCount++ )
	{
		sprintf( szFileName, PLAYERCREATOR_FILE, nLoopCount );

		nRoleType =  ( nLoopCount ==0 || nLoopCount == 1 ) ? 
		enRoleType_Knight : nRoleType;

		nRoleType =  ( nLoopCount ==2 || nLoopCount == 3 ) ? 
		enRoleType_Enchanter : nRoleType;

		nRoleType =  ( nLoopCount ==4 || nLoopCount == 5 ) ? 
		enRoleType_Monstrous : nRoleType;

		GetRoleFromIni( (BYTE*)m_pBuf[nLoopCount], szFileName, nRoleType );
	}

	GetRevivalFromIni( REVIVALID_FILENAME );
	
	return TRUE;
}

int PlayerCreator::CreateRole(
	ROLEPARAM& RP )
{
	int nIndex = RP.nSeries * ROLE_NO + RP.nSex;
	
	if ( nIndex < 0 || nIndex >= MAX_ROLE_COUNT )
	{
		return FALSE;
	}
	
	if ( m_pBuf[nIndex] )
	{
		PFS2DBSAVEITEM	pRoleItem = (PFS2DBSAVEITEM)m_pBuf[nIndex];
		PFS2DBBASECOLSET pRole = &pRoleItem->tagBaseColSet;

		_DBProcHeader DBHeader = {0};
		DBHeader.ulNetID = RP.ulNetID;
		DBHeader.ProcType = Proc_CreateRole;
		
		unsigned nNativeID = GetRevivalPos( RP.nMapID );
		char guid[sizeof(FSGUID)] = { 0 };
		if (g_pController != NULL)
		{
			g_pController->GenGUID(guid, g_GuidPadding);
		}
		
		DWORD dwCreateIP = 0;
		if( RP.ulNetID != -1 )
		{
			if ( g_pServer != NULL )
			{
				const char* szIP = g_pServer->GetClientInfo( RP.ulNetID );
				dwCreateIP = inet_addr( szIP );
			}
		}

		IProcParam* pParam = g_pController->GetProcParam( );

		pParam->BeginPush( PN_CREATEROLE );

		pParam->Push( RP.szAccName );
		pParam->Push( RP.szName );
		pParam->Push( RP.nSex != 0 );
		pParam->Push( RP.nSeries );
		pParam->Push( pRole->nRoleLevel );
		pParam->Push( pRole->unMoney );
		pParam->Push( pRole->unMoneyInBox );
		pParam->Push( RP.nMapID );
		pParam->Push( nNativeID );
		pParam->Push( 0 );
		pParam->Push( BinPair( (void*)&pRole->tagNumericInfoCol, sizeof(FS2DBNUMINFOCOL) ) );
		pParam->Push( BinPair( pRoleItem->tagSkillListCol.pListData, pRoleItem->tagSkillListCol.nListSize ) );
		pParam->Push( BinPair( pRoleItem->tagItemListCol.pListData, pRoleItem->tagItemListCol.nListSize ) );
		pParam->Push( guid );
		pParam->Push( RP.nImageHead );
		pParam->Push( dwCreateIP );

		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

		return g_pController->CallProc( cfs_db_cnn_dblink, pParam );
	}


	return FALSE;
}

int PlayerCreator::GetRoleFromIni(
	BYTE* pData,
	const char* szFileName,
	int nRoleType )
{
	KIniFile IniFile;
	int nTemp = 0;
	int nLoopCount = 0;

	if( IniFile.Load( szFileName ) == FALSE )
		return FALSE;

	PFS2DBSAVEITEM	pRoleItem = (PFS2DBSAVEITEM)pData;
	FS2DBBASECOLSET& RoleBase = pRoleItem->tagBaseColSet;

	RoleBase.nSkillSeries = role_skillseries_invalid;
	RoleBase.nMaxCreditPoint = 0;
	RoleBase.nCreditPoint	 = 0;
	RoleBase.uCreditState	 = 0;
	RoleBase.uReturnDate	 = 0;
	RoleBase.nPoint			 = 0;	
	RoleBase.nPointPlus      = 0;

	
	IniFile.GetInteger( "ROLE", "istrength", 0, &RoleBase.tagNumericInfoCol.nStrength );
	IniFile.GetInteger( "ROLE", "inimbus", 0, &RoleBase.tagNumericInfoCol.nDexterity );

	IniFile.GetInteger( "ROLE", "ibody", 0, &RoleBase.tagNumericInfoCol.nConstitution );
	IniFile.GetInteger( "ROLE", "iart", 0, &RoleBase.tagNumericInfoCol.nIntellect );

	IniFile.GetInteger( "ROLE", "ifightexp", 0, (int*)&RoleBase.tagNumericInfoCol.unExp );
	IniFile.GetInteger( "ROLE", "ifightlevel", 0, &RoleBase.nRoleLevel );
	
	int nCurPlayerLevel = RoleBase.nRoleLevel;
	
	IniFile.GetInteger( "ROLE", "imoney", 0, (int*)&RoleBase.unMoney );
	IniFile.GetInteger( "ROLE", "isavemoney", 0, (int*)&RoleBase.unMoneyInBox );
	
	IniFile.GetInteger( "ROLE", "ifiveprop", 0, &nTemp  );
	RoleBase.enRoleType = (enumROLETYPE)nTemp;

	IniFile.GetInteger( "ROLE", "bsex", 0, &nTemp );
	RoleBase.enRoleSex	= ( enCFSROLESEX )nTemp;

	IniFile.GetInteger( "ROLE", "imaxlife", 0, &RoleBase.tagNumericInfoCol.nMaxLife );
	IniFile.GetInteger( "ROLE", "imaxinner", 0, &RoleBase.tagNumericInfoCol.nMaxMana );
	IniFile.GetInteger( "ROLE", "iweightmax", 0, &RoleBase.tagNumericInfoCol.nWeightMax );

	// 人物的基本属性会影响生命和法力上限，但这里不能先把上限加上去
	// 因为Load数据的时候又会加，但初始生命和法力必需是满的，所以这里
	// 先把生命和法力加上去

	int nAddedLife = PlayerBaseNumeric::Body2LifeUpLimit(nRoleType, RoleBase.tagNumericInfoCol.nConstitution);
	int nAddedMana = PlayerBaseNumeric::Nimbus2ManaUpLimit(nRoleType, RoleBase.tagNumericInfoCol.nDexterity);

	RoleBase.tagNumericInfoCol.nLife	= RoleBase.tagNumericInfoCol.nMaxLife + nAddedLife;
	RoleBase.tagNumericInfoCol.nMana	= RoleBase.tagNumericInfoCol.nMaxMana + nAddedMana;
	
	IniFile.GetInteger( "ROLE", "irevivalid", 0, &RoleBase.nReviveMapID );
	IniFile.GetInteger( "ROLE", "irevivalx", 0, &RoleBase.nReviveMapX );
	IniFile.GetInteger( "ROLE", "irevivaly", 0, &RoleBase.nReviveMapY );

	RoleBase.bUseRevivePosition = TRUE;

	// Initial skill data
	pRoleItem->tagSkillListCol.nListSize = 0;
	pRoleItem->tagSkillListCol.pListData = pData + sizeof(FS2DBSAVEITEM);
	BYTE *pSkillBuf = (BYTE*)pRoleItem->tagSkillListCol.pListData;
	*pSkillBuf = CUR_SKILL_VERSION;
	*(pSkillBuf + 1) = (BYTE)ConfigManager::Singleton().GetGlobalVariable(global_var_skill_logic_version);
	PDBSkillData pSkillData = (PDBSkillData)(pSkillBuf + 2);
	pRoleItem->tagSkillListCol.nListSize += 2;

	static const int SKILL_KEY_SIZE = 32;
	int		nSkillCount;
	char	szSkillSecName[SKILL_KEY_SIZE];

	IniFile.GetInteger( "SKILLS", "COUNT", 0, &nSkillCount);

	for(int i = 0; i < nSkillCount; ++i)
	{
		int	nSkillId, nLevel, nStatus;

		sprintf(szSkillSecName, "SKILL%d", i + 1);
		IniFile.GetInteger( szSkillSecName, "skillid", 0, &nSkillId );
		IniFile.GetInteger( szSkillSecName, "level", 0, &nLevel );
		IniFile.GetInteger( szSkillSecName, "status", 0, &nStatus );

		pSkillData->skillId			= (WORD)nSkillId;
		pSkillData->skillLevel		= (WORD)nLevel;
		pSkillData->status			= (BYTE)nStatus;
		pSkillData->leftCoolDownTime = 0;
				
		++pSkillData;
		pRoleItem->tagSkillListCol.nListSize += sizeof(DBSkillData);
	}

	//Init Item Data

	int nItemCount = 0;
	pRoleItem->tagItemListCol.pListData = pData + 
	( sizeof(FS2DBSAVEITEM) + pRoleItem->tagSkillListCol.nListSize );

	TDBItemData_Base* pItemData = (TDBItemData_Base*)pRoleItem->tagItemListCol.pListData;
	TDBItemData_Base* pItemDataForPack = (TDBItemData_Base*)pRoleItem->tagItemListCol.pListData;

	IniFile.GetInteger( "ITEMS", "COUNT", 0, &nItemCount);

	pRoleItem->tagItemListCol.nListSize	= 0;

	memset( &pItemData->ImmData, -1, sizeof(KImmediacyParam) * MAX_IMMEDIACY_ITEM );
	pItemData->nVersion = ITEM_VERSION;	
	char szItemName[SKILL_KEY_SIZE];

	KItemDateMgr idMgr(ITEM_VERSION, pItemDataForPack);

	KItem tempItem;

	int successItemCount = 0;
	for( nLoopCount = 0; nLoopCount < nItemCount; nLoopCount++ )
	{
		sprintf( szItemName, "ITEM%d", nLoopCount + 1 );

		int genreType, detailType, particularType, level, count, pos, posX, posY;
		
		IniFile.GetInteger( szItemName, "igenretype", 0, &genreType );
		IniFile.GetInteger( szItemName, "idetailtype", 0, &detailType );
		IniFile.GetInteger( szItemName, "iparticulartype", 0, &particularType );
		IniFile.GetInteger( szItemName, "ilevel", 0, &level );
		IniFile.GetInteger( szItemName, "itemcount", 1, &count );
		IniFile.GetInteger( szItemName, "ilocal", 0, &pos );
		IniFile.GetInteger( szItemName, "ix", 0, &posX );
		IniFile.GetInteger( szItemName, "iy", 0, &posY );
		
		if (TRUE == g_ItemGen.Gen_Item(genreType, detailType, particularType, level, count, &tempItem ))
		{
			if ( idMgr.Pack(pos, posX, posY,tempItem ) > 0 )
			{
				idMgr.Next();
				successItemCount++;
			}			
		}		
	}

	pItemData->nItemlistLength = successItemCount;

	pRoleItem->tagItemListCol.nListSize = idMgr.GetSizeOfTDBItemData() - idMgr.GetSizeOf_TDBItemData() + (nLoopCount * idMgr.GetSizeOf_TDBItemData());

	pRoleItem->tagEnListCol.nListSize	= 0;
	pRoleItem->tagFriListCol.nListSize	= 0;
	pRoleItem->tagRsvListCol.nListSize	= 0;
	pRoleItem->tagTaskListCol.nListSize	= 0;


	return TRUE;
}


int PlayerCreator::GetRevivalFromIni( const char* szFileName )
{
	char szSection[SECTIONSIZE] = {0};
	char szNextSection[SECTIONSIZE];
	char szKey[SECTIONSIZE];

	KIniFile IniFile;
	
	if( IniFile.Load( szFileName ) == FALSE )
		return FALSE;

	IniFile.GetNextSection( szSection, szNextSection );

	do
	{
		int nMap = atoi( szNextSection + 1 );
		int nCount = 0;
		
		IniFile.GetInteger( szNextSection , COUNT, 0, &nCount );

		if ( nCount > 0 )
		{
		
			for ( int nLoopCount = 0; nLoopCount < nCount; nLoopCount++ )
			{
				int nRevivalPos = 0;
				sprintf( szKey, KEYNAME"%2.2d", nLoopCount );
				
				if( IniFile.GetInteger( szNextSection, szKey, 0, &nRevivalPos ) == TRUE )
					AddRevivalPos( nMap, nRevivalPos );
			}			
		}

		strncpy( szSection, szNextSection, SECTIONSIZE );
	}while ( IniFile.GetNextSection( szSection, szNextSection ) ) ;

	return TRUE;
}

int PlayerCreator::AddRevivalPos( int nMap, int nRevivalPos )
{
	for( int nLoopCount = 0; nLoopCount < MAX_TOTAL_COUNT_REVIVAL; nLoopCount++ )
	{
		if( m_sRevivalPos[nLoopCount].nMap == 0 )
		{
			m_sRevivalPos[nLoopCount].nMap = nMap;
			m_sRevivalPos[nLoopCount].nRevivalPos = nRevivalPos;
			return TRUE;
		}
	}
	return FALSE;
}

int PlayerCreator::GetRevivalPos( int nMap )
{
	for( int nLoopCount = 0; nLoopCount < MAX_TOTAL_COUNT_REVIVAL; nLoopCount++ )
	{
		if( m_sRevivalPos[nLoopCount].nMap == nMap )
			return m_sRevivalPos[nLoopCount].nRevivalPos;
	}

	return FALSE;
}

int PlayerCreator::GetInitialSkillCount(int nProf, int nSex) const
{
	int nIndex = nProf * ROLE_NO + nSex;

	if(nIndex >= 0 && nIndex < MAX_ROLE_COUNT)
	{
		PFS2DBSAVEITEM	pRoleItem = (PFS2DBSAVEITEM)m_pBuf[nIndex];
		return (pRoleItem->tagSkillListCol.nListSize - 2) / sizeof(DBSkillData);
	}
	else
		return 0;
}

const void* PlayerCreator::GetInitialSkillData(int nProf, int nSex) const
{
	int nIndex = nProf * ROLE_NO + nSex;

	if(nIndex >= 0 && nIndex < MAX_ROLE_COUNT)
	{
		PFS2DBSAVEITEM pRoleItem = (PFS2DBSAVEITEM)m_pBuf[nIndex];
		return (char*)pRoleItem->tagSkillListCol.pListData + 2;
	}
	else
		return NULL;
}