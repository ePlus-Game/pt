#include "KCore.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "ITaisuiWheelSettingMgr.h"
#include "ITaisuiWheel.h"
#include "KTaisuiWheelServer.h"
#include "ITaisuiWheelEventMgr.h"
#include "ITaisuiWheelResGenerator.h"
#include "ITaisuiWheelTianXiangMgr.h"

#define DEFAULT_GLOBAL_TASK_SAVE_INTERVAL 10

ITaisuiWheelSettingMgr   * KTaisuiWheelServer::m_Setting = NULL;
ITaisuiWheelEventMgr     * KTaisuiWheelServer::m_EventMgr= NULL;
ITaisuiWheelResGenerator * KTaisuiWheelServer::m_ResGenerator = NULL;
ITianXiangMgr            * KTaisuiWheelServer::m_TianXiangMgr = NULL;
bool                       KTaisuiWheelServer::m_IsInit = false;
//unsigned long              KTaisuiWheelServer::m_Timer = 0 ;

static KTaisuiWheelServer g_TaisuiWheelServer;
extern TaisuiGlobal       g_TaisuiGlobal={1,1,0};

unsigned long KTaisuiGlobalLoader::Parse(const unsigned long dwVersion , TaisuiGlobal * lpDest,void * pBuff, const unsigned long dwSize )
{
	if (dwVersion >= 1)
	{
		if( dwSize >= sizeof(TaisuiGlobal))
		{
			memcpy(lpDest,pBuff,sizeof(TaisuiGlobal));
			return sizeof(TaisuiGlobal);
		}//endif
		else
			return 0;
	}//endif
	else
		return 0;
}

//TaisuiSys

void LoadTaisuiGlobal( )
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_taisui;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_GETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

#define MAX_TAISUI_SAVE_BUF_SIZE 512

void SaveTaisuiGlobal( )
{
	_ASSERT(sizeof(unsigned long) + sizeof(g_TaisuiGlobal) < MAX_TAISUI_SAVE_BUF_SIZE);

	char           pSaveBuff[MAX_TAISUI_SAVE_BUF_SIZE];
	unsigned long  dwUsedSize = 0;

	char       *   pDest      = pSaveBuff;
	unsigned long  dwVersion = FS_TAISUI_DATA_VERSION;

	//Save Version First
	memcpy(pDest,&dwVersion,sizeof(unsigned long));
	pDest      += sizeof(unsigned long);
	dwUsedSize += sizeof(unsigned long);
		
	//Save Taisui Global
	memcpy(pDest,&g_TaisuiGlobal,sizeof(g_TaisuiGlobal));
	pDest      += sizeof(g_TaisuiGlobal);
	dwUsedSize += sizeof(g_TaisuiGlobal);

	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_taisui;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	pParam->Push( BinPair( pSaveBuff, dwUsedSize ) );
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

//Taisui System
unsigned long GetTaisuiTianXiangDay(void)
{
	return g_TaisuiGlobal.dwTianXiangDay;
}

unsigned long GetTaisuiTianXiangMonth(void)
{
	return g_TaisuiGlobal.dwTianXiangMonth;
}

unsigned long GetCurTaisuiEvent(void)
{
	return g_TaisuiGlobal.dwJiaziEventIndex;
}

void          SetTaisuiTianXiangDay(unsigned long dwDay)
{
	g_TaisuiGlobal.dwTianXiangDay=dwDay;
}

void          SetTaisuiTianXiangMonth(unsigned long dwMonth)
{
	g_TaisuiGlobal.dwTianXiangMonth=dwMonth;
}

void          SetCurTaisuiEvent(const unsigned long dwJiaziIndex)
{
	g_TaisuiGlobal.dwJiaziEventIndex=dwJiaziIndex;
}

KTaisuiWheelServer::KTaisuiWheelServer()
{
  /*Do Nothing at all*/
}

KTaisuiWheelServer::~KTaisuiWheelServer()
{
	Release();
}

void KTaisuiWheelServer::Init()
{
	if (!m_IsInit)
	{		

	   m_Setting =GetMainTaisuiWheelSettingMgr();    
	   //May do without setting
	   if (!m_Setting->IsLoadedSuccess()) 
		   return;
       
	   m_EventMgr=GetMainTaisuiWheelEventMgr();
	   m_ResGenerator=GetMainTaisuiWheelResGenerator();
	   m_TianXiangMgr=GetMainTianXiangMgr();

       LoadTaisuiGlobal();
	}//endif
}

bool KTaisuiWheelServer::IsInited()
{
	return m_IsInit;
}

void KTaisuiWheelServer::Release()
{
	if (m_IsInit)
	{
		ReleaseMainTaisuiWheelSettingMgr();
		m_Setting = NULL;
		ReleaseMainTianXiangMgr();
		m_TianXiangMgr = NULL;
		ReleaseMainTaisuiWheelEventMgr();
		m_EventMgr = NULL;
		ReleaseMainTaisuiWheelResGenerator();
		m_TianXiangMgr = NULL;
		m_IsInit=false;
	}
}

void KTaisuiWheelServer::ProcessGlobalDBTaisui(int nDBOpeRst, int nDataSize, unsigned char *pData)
{
	if (nDataSize && pData && nDataSize >= sizeof(unsigned long))
	{	
		unsigned  char *         pSrc =  pData;
		//Load Version
		unsigned long       dwVersion = 0 ;
		memcpy(&dwVersion,pSrc,sizeof(unsigned long));	
		pSrc += sizeof(unsigned long);
		
		_ASSERT(dwVersion <= FS_TAISUI_DATA_VERSION );
		if (dwVersion > FS_TAISUI_DATA_VERSION)
			return ;
		
		//Load global data
		KTaisuiGlobalLoader loader;
		TaisuiGlobal        tempGlobal;
		unsigned long       dwReaded = loader.Parse(dwVersion,&tempGlobal,pSrc,nDataSize - sizeof(unsigned long));
		
		if (dwReaded)
		{	
			memcpy(&g_TaisuiGlobal,&tempGlobal,sizeof(g_TaisuiGlobal));
			GetMainTianXiangMgr()->LoadFromDB();
			GetMainTaisuiWheelEventMgr()->LoadFromDB();
		}

	}//endif

}

void KTaisuiWheelServer::Breathe()
{
	//Check init
	if (!m_IsInit)
	{
        if (m_Setting!=NULL && m_Setting->IsLoadedSuccess()
			&& m_TianXiangMgr!=NULL && m_TianXiangMgr->IsInited()
			&& m_EventMgr!=NULL && m_EventMgr->IsInited() )
			m_IsInit=true;

	//	m_Timer=time(NULL);
	}//endif

	if (m_IsInit)
	{
		m_TianXiangMgr->Breathe();
		
		int playerCount = 0;
		int playerIndex = PlayerSet.GetFirstPlayer();
		
		while (playerIndex)
		{
			ITaisuiWheel * pWheel=Player[playerIndex].GetTaisuiWheelSys();
			
			if (pWheel)
				pWheel->Breathe();
			
			playerIndex=PlayerSet.GetNextPlayer();
		}//end while
	}//end if 
}

KTaisuiWheelServer & KTaisuiWheelServer::Singleton()
{
	return g_TaisuiWheelServer;
}