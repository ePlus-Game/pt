#include "KCore.h"
#include "KWarInfoManager.h"
#include "cfs_fs2_savedef.h"
#include "SocialUnit.h"

#define FS_WAR_SAVE_INTERVAL  600     //10 minute

//Loader Definition........................................................
unsigned long KWarInfoLoader::Parse(const unsigned long dwVersion,FSWarInfo * lpDest,void * pBuff,const unsigned long dwSize)
{
	if (dwVersion >= 1)
	{
		if( dwSize >= sizeof(FSWarInfo))
		{
			memcpy(lpDest,pBuff,sizeof(FSWarInfo));
			return sizeof(FSWarInfo);
		}//endif
		else
			return 0;
	}//endif
	else
		return 0;
}

static KWarInfoManager g_WarInfoManager;

KWarInfoManager::KWarInfoManager()
:m_IsInited(false),m_RecTimer(UNIX_TMIE_STAMP)
{
}

KWarInfoManager::~KWarInfoManager()
{ 
    /*Do Nothing at all*/
}

bool KWarInfoManager::AddRecord(const FSWarInfo & record)
{
	if (!m_IsInited)
		return false;

    //check duplicate condition
    std::list<FSWarInfo>::iterator it = SearchRecord(record.invaderGUID,record.mapID);
	
	if (it!=m_Infos.end())
	{
		if ((*it).warState == FS_WAR_STATE_INVALID)
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

FSWarInfo * KWarInfoManager::GetRecordByMapId(int mapID)
{
	if (!m_IsInited)
		return NULL;

	std::list<FSWarInfo>::iterator it=m_Infos.begin();
	while (it!=m_Infos.end())
	{
		if ((*it).mapID==mapID)
			break;
		
		++it;
	}
	
	if (it!=m_Infos.end() && (*it).warState != FS_WAR_STATE_INVALID && (*it).invaderGUID.data[0] != 0)
	{
        return &(*it);     //Duplicate!
	}//endif
	else
		return NULL;
}

FSWarInfo * KWarInfoManager::GetRecord(const FSGUID & invader , int mapID)
{
	if (!m_IsInited)
		return NULL;

	//check record exis
    std::list<FSWarInfo>::iterator it = SearchRecord(invader,mapID);
	
	if (it!=m_Infos.end() && (*it).warState != FS_WAR_STATE_INVALID)
	{
        return &(*it);     //Duplicate!
	}//endif
	else
		return NULL;
}


bool KWarInfoManager::DelRecord(const FSGUID & invader , const int mapID)
{
	if (!m_IsInited)
		return false;

	//check record exis
    std::list<FSWarInfo>::iterator it = SearchRecord(invader,mapID);
	
	if (it!=m_Infos.end())
	{
		m_Infos.erase(it++);
		
		SaveFromDBOP();
        
		return true;     //Duplicate!
	}//endif
	else
		return false;
}

bool KWarInfoManager::ChgState(const FSGUID & invader, int mapID, int newState)
{
	if (!m_IsInited)
		return false;

    //check record exis
    std::list<FSWarInfo>::iterator it = SearchRecord(invader,mapID);
	
	if (it!=m_Infos.end())
	{
		(*it).warState= newState;
        
		SaveFromDBOP();

        return true;     //Duplicate!
	}//endif
	else
		return false;
}

KWarInfoManager::SelfIterator::SelfIterator(void)
{
    m_CurIterator = g_WarInfoManager.m_Infos.begin();
}


FSWarInfo* KWarInfoManager::NextRecord(const FSGUID &invader  , SelfIterator & iter)
{
	if (!m_IsInited)
		return 0;
	
	FSWarInfo  * pRet=NULL;
    
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

void        KWarInfoManager::NotifyToSaveDB()
{
	SaveFromDBOP();
}

FSWarInfo * KWarInfoManager::NextRecord(SelfIterator & iter)
{
	FSWarInfo * pInfo = NULL;
	
	if (iter.m_CurIterator != m_Infos.end())
	{
        pInfo         =   &(*iter.m_CurIterator);
		++iter.m_CurIterator;
	}//endif

	return pInfo;
}

bool             KWarInfoManager::ExistWarInfo(const int nMap)
{
	std::list<FSWarInfo>::iterator it = m_Infos.begin();
	
	while (it!=m_Infos.end())
	{
		if ((*it).mapID == nMap)
		{
			if ((*it).warState == FS_WAR_STATE_INVALID)
				m_Infos.erase(it++);
			else
				return true;
		}
		else
			++it ;    
	}//endif

	return false;
}

bool             KWarInfoManager::ExistWarInfo(const FSGUID & invader)
{
	std::list<FSWarInfo>::iterator it = m_Infos.begin();
	
	while (it!=m_Infos.end())
	{
		if ((*it).invaderGUID == invader || (*it).defenderGUID == invader)
		{
			if ((*it).warState == FS_WAR_STATE_INVALID)
				m_Infos.erase(it++);
			else
				return true;
		}
		else
			++it ;    
	}//endif

	return false;
}

const FSGUID   * KWarInfoManager::GetInvaderFromDefender(const FSGUID & defender)
{
	if (!IsGUIDValid(defender))
		return 0;

	if (!m_IsInited)
		return 0;
	
    //check record exis
    std::list<FSWarInfo>::iterator it = m_Infos.begin();
	
	while (it!=m_Infos.end())
	{
		if ((*it).defenderGUID == defender)
			return &((*it).invaderGUID);
		
		if ((*it).invaderGUID == defender && IsGUIDValid((*it).defenderGUID))
		{
			return &((*it).defenderGUID);
        }//endif

        ++it ;    
	}//endif
	
	return 0;
}


std::list<FSWarInfo>::iterator   KWarInfoManager::SearchRecord(const FSGUID  & invader , const int mapID)
{
	std::list<FSWarInfo>::iterator it=m_Infos.begin();
	while (it!=m_Infos.end())
	{
       if ((*it).mapID==mapID &&  (*it).invaderGUID==invader)
		   break;

	   ++it;
	}

	return it;
}

bool                             KWarInfoManager::CleanInvalidRecord()
{
	bool  bChange = false;
	std::list<FSWarInfo>::iterator it=m_Infos.begin();
	while (it!=m_Infos.end())
	{
		if ( (*it).warState == FS_WAR_STATE_INVALID )
		{
			m_Infos.erase(it++);
			bChange = true;
		}//endif
		else
			++it;
	}//end for while

	return bChange;
}

KWarInfoManager & GetGlobalWarInfoManager(void)
{
   return g_WarInfoManager;
}

void KWarInfoManager::Initialize(void)
{
   LoadFromDBOP();
}

void KWarInfoManager::Breathe()
{
	if (m_IsInited)
	{
		bool bChange = CleanInvalidRecord();

		if (bChange || UNIX_TMIE_STAMP - m_RecTimer >= FS_WAR_SAVE_INTERVAL)
		{
			SaveFromDBOP();
			m_RecTimer = UNIX_TMIE_STAMP; 
		}//endif
	
	}//endif
}

void KWarInfoManager::LoadFromDBOP()
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_warinfo;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_GETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void KWarInfoManager::SaveFromDBOP()
{
	if (!IsInited())
		return ;

	char szBuf[8192];	
	unsigned char * pDestBuff = (unsigned char *) szBuf;
	unsigned long   dwTotalSize = 0;
	//Save the Version
	unsigned long   dwVersion =  FS_WAR_INFO_VERSION_NO;
	memcpy(pDestBuff,&dwVersion,sizeof(unsigned long));
	pDestBuff += sizeof(unsigned long);
	dwTotalSize+=sizeof (unsigned long );
	
    //Save the RecNum
	unsigned long   dwRecNum    = (unsigned long )g_WarInfoManager.m_Infos.size();
	memcpy(pDestBuff,&dwRecNum,sizeof(unsigned long));
	pDestBuff += sizeof(unsigned long);
	dwTotalSize+=sizeof (unsigned long );
	
	//Save the Rec
	
	std::list<FSWarInfo>::iterator it=g_WarInfoManager.m_Infos.begin();
	while (it!=g_WarInfoManager.m_Infos.end())
	{
		memcpy(pDestBuff,&(*it),sizeof(FSWarInfo));
		pDestBuff   += sizeof(FSWarInfo);
		dwTotalSize += sizeof(FSWarInfo);
		++it;
	}//end while

	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_warinfo;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	pParam->Push( BinPair( szBuf, dwTotalSize ) );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void KWarInfoManager::LoadWarInfoGlobalDataRet(int nDBOpeRst, int nDataSize,unsigned char *pData)
{
    if ( nDBOpeRst && pData != NULL && nDataSize>= 2 * sizeof(unsigned long) )
	{	
        unsigned char * pSrc       =(unsigned char * )pData;
      
		//Load the version info
		unsigned long   dwVersion  = 0;
		memcpy(&dwVersion,pSrc,sizeof(unsigned long));
		pSrc += sizeof(unsigned long);

		_ASSERT(dwVersion<= FS_WAR_INFO_VERSION_NO);
		if (dwVersion > FS_WAR_INFO_VERSION_NO)
			return ;

		//Load the RecNum
		unsigned long   dwRecNum=0;
		memcpy(&dwRecNum,pSrc,sizeof(unsigned long));
		pSrc += sizeof(unsigned long);
		
		KWarInfoLoader  loader;
		unsigned long   dwSizeLeft = nDataSize - 2 * sizeof(unsigned long );
		
		//Load the rec
		for (int iRec=0;iRec<dwRecNum;iRec++)
		{
            FSWarInfo temp;
			unsigned  long dwReaded = loader.Parse(dwVersion,&temp,pSrc,dwSizeLeft);
			if (dwReaded)
			{
				g_WarInfoManager.m_Infos.push_back(temp);
				
				dwSizeLeft -= dwReaded;
				pSrc += sizeof(FSWarInfo);
			}//endif
			else
				break;
			
		}//end for iRec
		
	}//endif

	g_WarInfoManager.m_IsInited = true;
}

bool KWarInfoManager::IsInited()const
{
	return m_IsInited;
}
