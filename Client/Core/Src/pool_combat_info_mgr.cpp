#include "KCore.h"
#include "pool_combat_info_mgr.h"
#include "cfs_fs2_savedef.h"
#include "SocialUnit.h"

#define FS_WAR_SAVE_INTERVAL  900     //15 minute
#define FS_WAR_VALID_INTERVAL 604800  //24 hour * 7

//Loader Definition...............................................................................................................................

unsigned long KPoolCombatInfoLoader::Parse(const unsigned long dwVersion,PoolCombatInfo * lpDest,void * pBuff,const unsigned long dwSize)
{
	if (dwVersion >= 1)
	{
		if( dwSize >= sizeof(PoolCombatInfo))
		{
			memcpy(lpDest,pBuff,sizeof(PoolCombatInfo));
			return sizeof(PoolCombatInfo);
		}//endif
		else
			return 0;
	}//endif
	else
		return 0;
}

static KPoolCombatInfoManager g_PoolInfoManager;

KPoolCombatInfoManager::KPoolCombatInfoManager()
:m_IsInited(false),m_RecTimer(UNIX_TMIE_STAMP)
{
	/*Do Nothing at all*/
}

KPoolCombatInfoManager::~KPoolCombatInfoManager()
{ 
    /*Do Nothing at all*/
}

bool KPoolCombatInfoManager::AddRecord(const PoolCombatInfo & record)
{
	if (!m_IsInited)
		return false;

    //check duplicate condition
    std::list<PoolCombatInfo>::iterator it = SearchRecord(record.invaderGUID,record.mapID);
	
	if (it!=m_Infos.end())
	{
		if ((*it).warState == FS_POOL_COMBAT_STATE_INVALID)
		{
			m_Infos.erase(it++);
		}//endif
		else
			return false;     //Duplicate!
	}//endif

	//Insert 
	m_Infos.push_front(record); //Push to the front because might be used immeadetely
    
	SaveFromDBOP();

	return true;
}

PoolCombatInfo * KPoolCombatInfoManager::GetRecord(const FSGUID & invader , int mapID)
{
	if (!m_IsInited)
		return NULL;

	//check record exis
    std::list<PoolCombatInfo>::iterator it = SearchRecord(invader,mapID);
	
	if (it!=m_Infos.end())
	{
        return &(*it);     //Duplicate!
	}//endif
	else
		return NULL;
}

bool             KPoolCombatInfoManager::ExistRecord(const int nMapID)
{
	std::list<PoolCombatInfo>::iterator it = m_Infos.begin();
	while (it != m_Infos.end())
	{
		if ((*it).mapID == nMapID)
		{
			if ((*it).warState == FS_POOL_COMBAT_STATE_INVALID)
			{
				m_Infos.erase(it++);
			}
			else
				return true;
		}//endif
		else
			++it;
	}//end for while

	return false;
}

bool             KPoolCombatInfoManager::ExistRecord(const FSGUID & invader)
{
	std::list<PoolCombatInfo>::iterator it = m_Infos.begin();
	while (it != m_Infos.end())
	{
		if ((*it).invaderGUID == invader)
		{
			if ((*it).warState == FS_POOL_COMBAT_STATE_INVALID)
			{
				m_Infos.erase(it++);
			}
			else
				return true;
		}//endif
		else
			++it;
	}//end for while
	
	return false;
}

bool KPoolCombatInfoManager::DelRecord(const FSGUID & invader , const int mapID)
{
	if (!m_IsInited)
		return false;

	//check record exis
    std::list<PoolCombatInfo>::iterator it = SearchRecord(invader,mapID);
	
	if (it!=m_Infos.end())
	{
		m_Infos.erase(it++);
		
		SaveFromDBOP();
        
		return true;     
	}//endif
	else
		return false;
}

bool KPoolCombatInfoManager::ChgState(const FSGUID & invader, int mapID, int newState)
{
	if (!m_IsInited)
		return false;

    //check record exis
    std::list<PoolCombatInfo>::iterator it = SearchRecord(invader,mapID);
	
	if (it!=m_Infos.end())
	{
		(*it).warState= newState;
        
		SaveFromDBOP();

        return true;     //Duplicate!
	}//endif
	else
		return false;
}

KPoolCombatInfoManager::SelfIterator::SelfIterator(void)
{
    m_CurIterator = g_PoolInfoManager.m_Infos.begin();
}


PoolCombatInfo* KPoolCombatInfoManager::NextRecord(const FSGUID &invader  , SelfIterator & iter)
{
	if (!m_IsInited)
		return 0;
	
	PoolCombatInfo  * pRet=NULL;
    
	while (iter.m_CurIterator != m_Infos.end())
	{
		if ((*iter.m_CurIterator).invaderGUID == invader)
		{
			pRet = &(*iter.m_CurIterator);
			++iter.m_CurIterator;
			break;
		}
		
		++ iter.m_CurIterator;
	}//end while 
	
    return pRet;
}

void        KPoolCombatInfoManager::NotifyToSaveDB()
{
	SaveFromDBOP();
}

PoolCombatInfo * KPoolCombatInfoManager::NextRecord(SelfIterator & iter)
{
	PoolCombatInfo * pInfo = NULL;
	
	if (iter.m_CurIterator != m_Infos.end())
	{
        pInfo         =   &(*iter.m_CurIterator);
		++iter.m_CurIterator;
	}//endif

	return pInfo;
}

std::list<PoolCombatInfo>::iterator   KPoolCombatInfoManager::SearchRecord(const FSGUID  & invader , const int mapID)
{
	std::list<PoolCombatInfo>::iterator it=m_Infos.begin();
	while (it!=m_Infos.end())
	{
       if ((*it).mapID==mapID &&  (*it).invaderGUID==invader)
		   break;

	   ++it;
	}

	return it;
}

bool  KPoolCombatInfoManager::CleanInvalidRecord()
{
	bool bChange    = false;
	std::list<PoolCombatInfo>::iterator it=m_Infos.begin();
	while (it!=m_Infos.end())
	{
		if ( (*it).warState == FS_POOL_COMBAT_STATE_INVALID)
		{
			m_Infos.erase(it++);
			bChange = true;
		}//endif
		else
			++it;
	}//end for while

	return bChange;
}

KPoolCombatInfoManager & GetPoolCombatInfoManager(void)
{
   return g_PoolInfoManager;
}

void KPoolCombatInfoManager::Initialize(void)
{
   LoadFromDBOP();
}

void KPoolCombatInfoManager::Breathe()
{
	if (m_IsInited)
	{
		bool bChange = CleanInvalidRecord();

		if (UNIX_TMIE_STAMP - m_RecTimer >= FS_WAR_SAVE_INTERVAL || bChange)
		{
			SaveFromDBOP();
			m_RecTimer = UNIX_TMIE_STAMP; 
		}//endif
	
	}//endif
}

void KPoolCombatInfoManager::LoadFromDBOP()
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID   = -1;
	DBHeader.ProcType  = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_poolinfo;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_GETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void KPoolCombatInfoManager::SaveFromDBOP()
{
	char szBuf[8192];	
	unsigned char * pDestBuff = (unsigned char *) szBuf;
	unsigned long   dwTotalSize = 0;
	//Save the Version
	unsigned long   dwVersion   = (unsigned long )FS_POOL_COMBAT_VERSION_NO;
	
	memcpy(pDestBuff,&dwVersion,sizeof(unsigned long));
	pDestBuff += sizeof(unsigned long);
	dwTotalSize+=sizeof (unsigned long );

    //Save the RecNum
	unsigned long   dwRecNum    = (unsigned long )g_PoolInfoManager.m_Infos.size();
	
	memcpy(pDestBuff,&dwRecNum,sizeof(unsigned long));
	pDestBuff += sizeof(unsigned long);
	dwTotalSize+=sizeof (unsigned long );
	
	//Save the Rec
	
	std::list<PoolCombatInfo>::iterator it=g_PoolInfoManager.m_Infos.begin();
	while (it!=g_PoolInfoManager.m_Infos.end())
	{
		memcpy(pDestBuff,&(*it),sizeof(PoolCombatInfo));
		pDestBuff   += sizeof(PoolCombatInfo);
		dwTotalSize += sizeof(PoolCombatInfo);
		++it;
	}//end while

	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID   = -1;
	DBHeader.ProcType  = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_poolinfo;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	pParam->Push( BinPair( szBuf, dwTotalSize ) );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void KPoolCombatInfoManager::LoadWarInfoGlobalDataRet(int nDBOpeRst, int nDataSize,unsigned char *pData)
{
    if ( nDBOpeRst && pData != NULL && nDataSize >= 2 * sizeof(unsigned long))
	{
        unsigned char * pSrc=(unsigned char * )pData;
		//Load the Version
		unsigned long   dwVersion   = (unsigned long )0;
		memcpy(&dwVersion,pSrc,sizeof(unsigned long));
		pSrc += sizeof(unsigned long);

		_ASSERT(dwVersion <= FS_POOL_COMBAT_VERSION_NO);
		if (dwVersion > FS_POOL_COMBAT_VERSION_NO)
			return ;
		
		//Load the RecNum
        unsigned long   dwRecNum=0;
		memcpy(&dwRecNum,pSrc,sizeof(unsigned long));
		pSrc += sizeof(unsigned long);

		//Loader initialize
		KPoolCombatInfoLoader infoLoader;

		unsigned long dwLeftSize = nDataSize - 2 * sizeof(unsigned long );

		for (int iRec=0;iRec<dwRecNum;iRec++)
		{
            PoolCombatInfo        temp;
			unsigned long dwParsedSize = infoLoader.Parse(dwVersion,&temp,pSrc,dwLeftSize);
			if (dwParsedSize)
			{
				g_PoolInfoManager.m_Infos.push_back(temp);
				dwLeftSize -= dwParsedSize;
				pSrc += sizeof(PoolCombatInfo);
			}//endif
			else
				break;

		}//end for iRec

	}//endif

	g_PoolInfoManager.m_IsInited = true;
}

bool KPoolCombatInfoManager::IsInited()const
{
	return m_IsInited;
}