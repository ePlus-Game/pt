#ifndef K_SERVER_LIST_H
#define K_SERVER_LIST_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-04-6 17:16
//      File_base        : ServerList.h
//      File_ext         : .h
//      Author           : Brianyao(yaojie)
//      Description      : Declare for the Server List Elements
//
//////////////////////////////////////////////////////////////////////

#include<vector>

#define MAX_NET_NAME 128
#define MAX_REGION_NAME 128
#define MAX_SERVER_NAME 256
#define MAX_SERVER_STATE 128
#define MAX_SERVER_IP    256
#define	MAX_HSTRYSERVER_COUNT	5

class CRegionInfo;
struct tagNET_LIST;

class CServerInfo
{
	CString m_Name;
    CString m_State;
	CString m_IP;

	CRegionInfo * m_pParentRegion;
public:
	CServerInfo();
	~CServerInfo();
public:
	void    Initialize(CRegionInfo * pParent,const char * szName,const char * szState,const char * szIP);
	CString GetName(void)const;
	CString GetState(void)const;
	CString GetIP(void)const;
	const CRegionInfo * GetParentRegion(void)const;
private:
	CServerInfo(const CServerInfo &);
	const CServerInfo & operator = (const CServerInfo &);
};

class CRegionInfo
{
	CServerInfo * m_Severs;
	unsigned long m_ServerNum;
	CString       m_RegionName;

	tagNET_LIST * m_pParentNet;
public:
	CRegionInfo();
	~CRegionInfo();
public:
    void                Initialize(tagNET_LIST * pParentNet,const unsigned long dwServerNum,const char * szRegionName);
    void                SetServerInfo(const unsigned long dwServerIndex,const char * szName,const char * szState,const char * szIP);
public:
	CServerInfo *       GetServerInfo(const unsigned long dwIndex);  //May Return NULL if dwIndex is Invalid!
    CString             GetRegionName(void)const;
	unsigned long       GetServerNum();
	const tagNET_LIST * GetParentNet(void)const;
private:
	CRegionInfo(const CRegionInfo &);
	const CRegionInfo & operator = (const CRegionInfo &);
};

typedef struct tagNET_LIST
{
   CString       m_NetName;
   unsigned long m_RegionNum;
   CRegionInfo * m_RegionInfos;
}NET_LIST,*lpNET_LIST; 

typedef struct _HistoryServer
{
	char NetName[MAX_NET_NAME];
	char RegionName[MAX_REGION_NAME];
	char ServerName[MAX_SERVER_NAME];

} HistoryServer;

typedef struct _HistoryServerList
{
	int ServerCount;
	HistoryServer Server[MAX_HSTRYSERVER_COUNT];

} HistoryServerList;

#define CUR_SEL_FILE_PATH "UserData\\LastChoice.ini"

class KIniFile;
class CServerInfoList
{
    unsigned long m_NetListNum;
	NET_LIST    * m_Nets;
    CServerInfo * m_CurrentSelect;
	
//	char m_szLastServerName[MAX_SERVER_NAME];
//	char m_szLastRegionName[MAX_REGION_NAME];
//	char m_szLastNetName[MAX_NET_NAME];

	HistoryServerList m_HistoryServList;

public:
	CServerInfoList();
	~CServerInfoList();
public:
	BOOL ReadListFile(const char * szListFileName);   //May Failed!
	BOOL SaveCurSelToFile();                          //SaveTheCurSelToFile
	BOOL IsReady(void)const;
	const HistoryServerList& GetHistoryServList();

public:
	void InitializeNetList(const unsigned long dwIndex,const char * szNetName,const unsigned long dwRegionNum);
	void SetRegionInfo(const unsigned long dwNetIndex,const unsigned long dwRegionIndex,const unsigned long dwServerNum,const char * szRegionName);
	void SetServerInfo(const unsigned long dwNetINdex,const unsigned long dwRegionIndex,const unsigned long dwServerIndex,const char * szName,const char * szState,const char * szIP);
public:
	NET_LIST *          GetNetList(const unsigned long dwNetIndex);
	unsigned long       GetNetNum(void)const;
	void                SetCurrentSelect(CServerInfo * pInfo);
	void				AddServerToHstryList(const CServerInfo *pInfo);
	const CServerInfo * GetCurrentSelect()const;
private:
	BOOL ReadNetInfos(const char * szNetIndexName,KIniFile * pFile,const unsigned long dwNetIndex);
	BOOL ReadRegionInfos(const char * szRegionIndexName,KIniFile * pFile,const unsigned long dwNetIndex,const unsigned long dwRegionIndex);
	BOOL ReadHstryServerList();
	int FindInHistoryList(const CServerInfo *pInfo);
	void FillToHistoryList(int nIdx, const CServerInfo *pInfo);
private:
	CServerInfoList(const CServerInfoList &);
	const CServerInfoList & operator = (const CServerInfoList &); 
};

inline const HistoryServerList& CServerInfoList::GetHistoryServList()
{
	return m_HistoryServList;
}

inline void  CServerInfoList::SetCurrentSelect(CServerInfo * pInfo)
{
	if(pInfo)
		m_CurrentSelect=pInfo;
}

extern CServerInfoList g_ServerList;

#endif
