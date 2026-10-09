#include"stdafx.h"/*
//#include"ServerList.h"
#include"KInifile.h"

#include "stdafx.h"
//#include "ServerList.h"

#define NO_DIRECT_X
#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"

#define	HSTRYSERVER_SEC_NAME	"LastChoice"
#define HSTRYSERVER_COUNT_KEY	"Count"
#define HSTRYSERVER_NET_KEY		"Net_%d"
#define HSTRYSERVER_REGION_KEY	"Region_%d"
#define HSTRYSERVER_SERVER_KEY	"Server_%d"


CServerInfoList g_ServerList;

CServerInfo::CServerInfo():
m_pParentRegion(NULL)
{/*Do Nothing at all*///}
/*
CServerInfo::~CServerInfo()
{
	/*Do Nothing at all*/
//}
/*
void CServerInfo::Initialize(CRegionInfo * pParent,const char * szName,const char * szState,const char * szIP)
{
	m_pParentRegion=pParent;
	m_Name=szName;
	m_State=szState;
	m_IP=szIP;
}

const CRegionInfo * CServerInfo::GetParentRegion(void)const
{
	return m_pParentRegion;
}

CString CServerInfo::GetState()const
{
	return m_State;
}

CString CServerInfo::GetName()const
{
	return m_Name;
}

CString CServerInfo::GetIP()const
{
	return m_IP;
}

CRegionInfo::CRegionInfo()
:m_Severs(NULL),m_ServerNum(0)
,m_pParentNet(NULL)
{/*Do Nothing *///}
/*

CRegionInfo::~CRegionInfo()
{
	if (m_Severs)
		delete [] m_Severs;
}

void CRegionInfo::Initialize(tagNET_LIST * pParentNet,const unsigned long dwServerNum,const char * szRegionName)
{
	m_ServerNum=dwServerNum;
	ASSERT(m_ServerNum>0);
	if(dwServerNum)
		m_Severs=new CServerInfo [dwServerNum];
	
	m_RegionName=szRegionName;
	m_pParentNet=pParentNet;
}


CString CRegionInfo::GetRegionName()const
{
	return m_RegionName;
}

CServerInfo * CRegionInfo::GetServerInfo(const unsigned long dwIndex)
{
	ASSERT(m_Severs && dwIndex<m_ServerNum);
	
	if (dwIndex<m_ServerNum)
		return &m_Severs[dwIndex];
	else
		return NULL;
}

const tagNET_LIST * CRegionInfo::GetParentNet(void)const
{
    return m_pParentNet;
}

unsigned long CRegionInfo::GetServerNum()
{
	return m_ServerNum;
}

void CRegionInfo::SetServerInfo(const unsigned long dwServerIndex,const char * szName,const char * szState,const char * szIP)
{
	ASSERT(dwServerIndex<m_ServerNum);
	m_Severs[dwServerIndex].Initialize(this,szName,szState,szIP);
}

CServerInfoList::CServerInfoList()
:m_NetListNum(0),m_Nets(NULL),m_CurrentSelect(NULL)
{}

CServerInfoList::~CServerInfoList()
{
	if (m_Nets)
	{
		for (int i=0;i<m_NetListNum;i++)
		{
           delete  [] m_Nets[i].m_RegionInfos;
		}//end for i

		delete [] m_Nets; 
	}//end for m_Nets
}

void CServerInfoList::AddServerToHstryList(const CServerInfo *pInfo)
{
	if (pInfo)
	{
		int nIdx = FindInHistoryList(pInfo);

		if(-1 == nIdx)
		{
			// 如果历史列表中没有这个服务器，则将其加入列表头部
			// 原始列表依次后移

			MoveMemory(&m_HistoryServList.Server[1], 
				&m_HistoryServList.Server[0], 
				sizeof(m_HistoryServList.Server) - sizeof(m_HistoryServList.Server[0])
				);

			FillToHistoryList(0, pInfo);

			++m_HistoryServList.ServerCount;

			if(m_HistoryServList.ServerCount >= MAX_HSTRYSERVER_COUNT)
				m_HistoryServList.ServerCount = MAX_HSTRYSERVER_COUNT;
		}
		else if(nIdx > 0)
		{
			// 如果历史列表中已有这个服务器，则将这个服务器移到
			// 列表首部

			MoveMemory(&m_HistoryServList.Server[1],
				&m_HistoryServList.Server[0],
				sizeof(m_HistoryServList.Server[0]) * nIdx
				);

			FillToHistoryList(0, pInfo);
		}
		else
		{
			// 如果这个服务器正好处于列表头部，则不用做任何操作
		}
	}
}

void CServerInfoList::FillToHistoryList(int nIdx, const CServerInfo *pInfo)
{
	strncpy(m_HistoryServList.Server[nIdx].ServerName,
		pInfo->GetName(),
		sizeof(m_HistoryServList.Server[nIdx].ServerName)
		);
	
	strncpy(m_HistoryServList.Server[nIdx].RegionName,
		pInfo->GetParentRegion()->GetRegionName(),
		sizeof(m_HistoryServList.Server[nIdx].RegionName)
		);

	strncpy(m_HistoryServList.Server[nIdx].NetName,
		pInfo->GetParentRegion()->GetParentNet()->m_NetName,
		sizeof(m_HistoryServList.Server[nIdx].NetName)
		);	
}

const CServerInfo * CServerInfoList::GetCurrentSelect()const
{
    return m_CurrentSelect;
}

void CServerInfoList::InitializeNetList(const unsigned long dwIndex,const char * szNetName,const unsigned long dwRegionNum)
{
  m_Nets[dwIndex].m_NetName=szNetName;
  m_Nets[dwIndex].m_RegionNum=dwRegionNum;
  m_Nets[dwIndex].m_RegionInfos=new CRegionInfo[dwRegionNum];
}

void CServerInfoList::SetRegionInfo(const unsigned long dwNetIndex,const unsigned long dwRegionIndex,const unsigned long dwServerNum,const char * szRegionName)
{
	ASSERT(m_Nets && dwNetIndex < m_NetListNum && dwRegionIndex<m_Nets[dwNetIndex].m_RegionNum);
	m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].Initialize(m_Nets+dwNetIndex,dwServerNum,szRegionName);
}

void CServerInfoList::SetServerInfo(const unsigned long dwNetIndex,const unsigned long dwRegionIndex,const unsigned long dwServerIndex,const char * szName,const char * szState,const char * szIP)
{
	ASSERT(m_Nets && dwNetIndex < m_NetListNum && dwRegionIndex < m_Nets[dwNetIndex].m_RegionNum && dwServerIndex<m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].GetServerNum());
    m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].SetServerInfo(dwServerIndex,szName,szState,szIP);
} 

NET_LIST * CServerInfoList::GetNetList(const unsigned long dwNetIndex)
{
	return &m_Nets[dwNetIndex];
}

unsigned long CServerInfoList::GetNetNum()const
{
	return m_NetListNum;
}

BOOL CServerInfoList::ReadListFile(const char * szListFileName)
{
	//Read Last Choice............................................
	ReadHstryServerList();
	//............................................................
	static char szNetNameBuffer[MAX_NET_NAME];

	KIniFile File;
	BOOL res=File.Load(szListFileName);
    ASSERT(res);
    
	if (!File.GetInteger("List","NetCount",0,(int *)&m_NetListNum) || m_NetListNum==0)
 	  return FALSE;
    
	m_Nets=new NET_LIST [m_NetListNum];

    for (int i=0;i<m_NetListNum;i++)
	{
        sprintf(szNetNameBuffer,"Net_%d",i);
        if (!ReadNetInfos(szNetNameBuffer,&File,i))
			return FALSE;
	}//end for i
	
	return TRUE;
}

BOOL CServerInfoList::IsReady()const
{
	return (m_Nets!=NULL);
}

#define MAX_NET_CUSTOM_NAME 128
#define MAX_REGION_INDEX    128

BOOL CServerInfoList::ReadNetInfos(const char * szNetIndexName,KIniFile * pFile,const unsigned long dwNetIndex)
{
	static char szNetCusBuffer[MAX_NET_NAME];
	static char szRegionIndexBuffer[MAX_REGION_INDEX];

	if (!pFile->GetString(szNetIndexName,"NetName","",szNetCusBuffer,MAX_NET_NAME-1))
		return FALSE;

	m_Nets[dwNetIndex].m_NetName=szNetCusBuffer;
	if (!pFile->GetInteger(szNetIndexName,"RegionNum",0,(int *)&m_Nets[dwNetIndex].m_RegionNum))
		return FALSE;
	ASSERT(m_Nets[dwNetIndex].m_RegionNum>0);
	
	m_Nets[dwNetIndex].m_RegionInfos=new CRegionInfo [m_Nets[dwNetIndex].m_RegionNum];
    
	for (int i=0;i<m_Nets[dwNetIndex].m_RegionNum;i++)
	{
		sprintf(szRegionIndexBuffer,"%s_Region_%d",szNetIndexName,i);
        if (!ReadRegionInfos(szRegionIndexBuffer,pFile,dwNetIndex,i))
			return FALSE;

	}//endif i

    return TRUE;
}

BOOL CServerInfoList::ReadRegionInfos(const char * szRegionIndexName,KIniFile * pFile,const unsigned long dwNetIndex,const unsigned long dwRegionIndex)
{
	static char szRegionName[MAX_REGION_NAME];
	static char szServerNameBuffer[2][MAX_SERVER_NAME];
	static char szServerStateBuffer[2][MAX_SERVER_STATE];
	static char szServerIPBuffer[2][MAX_SERVER_IP];
   
	if (!pFile->GetString(szRegionIndexName,"RegionName","",szRegionName,MAX_REGION_NAME-1))
		return FALSE;
	
	unsigned long dwServerNum=0;
	if (!pFile->GetInteger(szRegionIndexName,"ServerNum",0,(int *)&dwServerNum))
		return FALSE;

	ASSERT(dwServerNum>0);

	m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].Initialize(m_Nets+dwNetIndex,dwServerNum,szRegionName);

    for (int i=0;i<m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].GetServerNum();i++)
	{
	    //Read ServerInfos here
        sprintf(szServerNameBuffer[0],"%i_Server_Name",i);
		if (!pFile->GetString(szRegionIndexName,szServerNameBuffer[0],"",szServerNameBuffer[1],MAX_SERVER_NAME-1))
			return FALSE;

		//优先读取服务器下载的状态信息
		DWORD bCpyNum=GetPrivateProfileString(szServerNameBuffer[1],"State","",szServerStateBuffer[1],MAX_SERVER_STATE-1,"UserData\\state.ini");
		if (!bCpyNum)
		{
		 sprintf(szServerStateBuffer[0],"%i_Server_State",i);
		 pFile->GetString(szRegionIndexName,szServerStateBuffer[0],"",szServerStateBuffer[1],MAX_SERVER_STATE-1);
		}//endif
		
		sprintf(szServerIPBuffer[0],"%i_Server_IP",i);
		if (!pFile->GetString(szRegionIndexName,szServerIPBuffer[0],"",szServerIPBuffer[1],MAX_SERVER_IP-1))
			return FALSE;
		
		m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].SetServerInfo(i,szServerNameBuffer[1],szServerStateBuffer[1],szServerIPBuffer[1]);

//  		if (strcmp(szServerNameBuffer[1], m_HistoryServList.Server[0].ServerName)==0 && 
// 			strcmp(m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].GetRegionName(), m_HistoryServList.Server[0].RegionName)==0 && 
// 			strcmp(m_Nets[dwNetIndex].m_NetName, m_HistoryServList.Server[0].NetName)==0 )
//  		{
//              SetCurrentSelect(m_Nets[dwNetIndex].m_RegionInfos[dwRegionIndex].GetServerInfo(i));
//  		}//endif

	}//endfor i

    return TRUE;
}

BOOL CServerInfoList::ReadHstryServerList()
{
    char aConfigPath[MAX_PATH] ={0};
	g_GetRootPath(aConfigPath);
	strcat(aConfigPath, "\\");
	strcat(aConfigPath, CUR_SEL_FILE_PATH);

	GetPrivateProfileStruct(HSTRYSERVER_SEC_NAME, 
		HSTRYSERVER_SEC_NAME, 
		&m_HistoryServList, 
		sizeof(m_HistoryServList),
		aConfigPath
		);

	return TRUE;
}

BOOL CServerInfoList::SaveCurSelToFile()
{
	char aConfigPath[MAX_PATH] ={0};
	g_GetRootPath(aConfigPath);
	strcat(aConfigPath, "\\");
	strcat(aConfigPath, CUR_SEL_FILE_PATH);
	char aCapPath[MAX_PATH] = {0};

	WritePrivateProfileStruct(HSTRYSERVER_SEC_NAME,
		HSTRYSERVER_SEC_NAME,
		&m_HistoryServList,
		sizeof(m_HistoryServList),
		aConfigPath
		);

	return TRUE;
}

int CServerInfoList::FindInHistoryList(const CServerInfo *pInfo)
{
	for(int nServ = 0; nServ < m_HistoryServList.ServerCount; ++nServ)
	{
		if( strcmp(pInfo->GetName(), m_HistoryServList.Server[nServ].ServerName) )
			continue;

		if( strcmp(pInfo->GetParentRegion()->GetRegionName(), m_HistoryServList.Server[nServ].RegionName) )
			continue;

		if( strcmp(pInfo->GetParentRegion()->GetParentNet()->m_NetName, m_HistoryServList.Server[nServ].NetName) )
			continue;

		return nServ;
	}

	return -1;
}
*/