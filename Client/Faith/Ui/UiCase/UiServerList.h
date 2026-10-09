// UiServerList.h: interface for the KUiServerList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UISERVERLIST_H__4FE13C6D_70AC_42E6_85C3_2D4960DA8B81__INCLUDED_)
#define AFX_UISERVERLIST_H__4FE13C6D_70AC_42E6_85C3_2D4960DA8B81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../UiCommon.h"
#include <vector>
#include <set>
#include "TLVertScrollbar.h"
#include "TLMiniHorzScrollbar.h" 

using namespace std;
using namespace CEGUI;

class KServerPanel;
class KLoginList;

struct ServerInfo
{
	string Name;
	string IP;
	string State;
	string Region;
	string Prefix;
	string Prefix2;
	string Postfix;
	string Postfix2;
	ServerInfo()	
	{ 
		Name = "";
		IP = "";
		State = "";
		Region = "";
	};

	ServerInfo( const ServerInfo& si )	
	{ 
		Name = si.Name;
		IP = si.IP;
		State = si.State;
		Region = si.Region;
		Prefix = si.Prefix;
		Prefix2 = si.Prefix2;
		Postfix = si.Postfix;
		Postfix2 = si.Postfix2;
	};
};

struct RegionInfo
{
	string Name;
	int    ServerCount;
	string SectionName;
	vector<ServerInfo> Servers;
};

struct NetInfo
{
	string Name;
	int    RegionCount;
	string SectionName;
	vector<RegionInfo> Regions;
};

typedef vector<Window*> BarList;
typedef set< string > BarNameSet;
typedef map< Window*, ServerInfo > BarServerInfoMap;

void	EncryptServerFile(const string& fileName);	
void	UnencryptServerFile(const string& fileName);
bool    UnencryptServerFileToTmp(const string & serverFile , const string & tmpFile);
void    DeleteTmpFile(const string & tmpFile);
bool	IsFileEncrypted(const string& fileName);
std::string	WStringToAString(const CEGUI::String& utf8String);

const int MAX_PANEL_LIMIT = 20;

class KUiServerListStateUpdate
{
public:
	KUiServerListStateUpdate();
	~KUiServerListStateUpdate() {};
public:
	BOOL				Init( void );
	BOOL				ServerStateDownThread( void );	
private:
	BOOL				HttpDownLoadFile(const char * szPath,const char * szDestPath);
	BOOL				FtpDownLoadFile(const char * szPath,const char * szDestPath);
	unsigned			CRC32(unsigned CRC, const void *pvBuf, int nLen);
	bool				CheckCRC(const char * szFileName,const unsigned long dwMatchCRC);
	LPCSTR				GetNextUpdateServer(int& nCurrent);
	unsigned long		ProcessCRC(HANDLE hFile);
	BOOL				DownLoadFromPath(const char * szPath,const char * szDestPath);
private:
	int		m_nCurrentHost;
	std::vector<std::string> m_strHosts;
};

/********************************************************************
/*						class: KUiServerList
*********************************************************************/
class KUiServerList : public KUiWndSingleton<KUiServerList>  
{
public:
	KUiServerList( const CEGUI::String& id_name );
	virtual ~KUiServerList();

	static void		Show( bool bTempServerList = false );
	static void		Hide();
	void			Init();

	static string  getRealServerName( string& oriServerName );

private:
	void 	UpdataServerList( bool bTempServerList );
	bool	btnEnter_MouseClick( const CEGUI::EventArgs& e );
	bool	btnExit_MouseClick( const CEGUI::EventArgs& e );
	bool	thisWnd_KeyDown( const CEGUI::EventArgs& e );
	bool	btnShowRecentList_Click( const CEGUI::EventArgs& e );
	bool	recentSelectedBar_MouseClick( const CEGUI::EventArgs& e );
	bool	recentSelectedBar_MouseDoubleClick( const CEGUI::EventArgs& e );

	void	syncServerStatus( BarList& source, BarList& dest );
	void	SaveRecentServer();
	void	AcceptServer();
	//void	LoadName_RegionMap();
	//void	SyncRegionName( RegionInfo& source );
	void	Load_ID_Name_Map();
	void	SetTemplateBackground( Window* wnd );
	void	LoadTemplateRect();
	bool	sbScrollBar_handleScroll( const CEGUI::EventArgs& e );
	bool	checkRecentFileVersion();
	bool	delInvalidRecentServerInfo();
	void	refreshSelectedRecentServerBar( const ServerInfo& si );
	
	static void ServerSelected(  
		ServerInfo&	serverInfo,
		KServerPanel* sender, 
		bool acceptAtOnce = false /*该参数加入是为了双击Panle后直接选择服务器进入*/);

private:
	KServerPanel*	m_recentPanel;
	KLoginList*		m_loginList;
	KLoginList*		m_recentList;
	KServerPanel*	m_panelList[MAX_PANEL_LIMIT];
	Window*			m_backGroundList[MAX_PANEL_LIMIT];
	Window*			m_recentBackGround;
	Window*			m_templateBackGround;
	Window*			m_mainBackGround;

	Window*			m_selectedRecentServerBar;
	StaticImage*	m_selectedRecentServerBar_Hover;
	StaticImage*	m_selectedRecentServerBar_StatusImage;
	StaticText*		m_selectedRecentServerBar_ServerStatus;
	StaticText*		m_selectedRecentServerBar_ServerName;
	StaticText*		m_selectedRecentServerBar_ServerIP;
	PushButton*		m_selectedRecentServerBar_btnShowList;

	Window*			m_currentRecentServerBar;

	TLMiniHorzScrollbar* m_pScrollbar; 

	ServerInfo		m_serverInfo;
	int				m_exceedWidth;
	int				m_recommendServerIndex;

	map< string, string >		m_name_regionMap;
	map< string, ServerInfo >	m_id_info_map;
	Rect						m_templateRect;
	int							m_recentServerCount;
};

/********************************************************************
/*						class: KServerPanel
*********************************************************************/
class KServerPanel
{
public:
	KServerPanel();
	virtual ~KServerPanel();

	void LoadServerList(  
		RegionInfo* region,
		WindowManager* pWindowManager, 
		Window* parentWindow,
		bool useRegionPrefix = false,
		bool reverseList = false);
	
	void SetServerSelected_CallBack(  
		void (*func)(  
			ServerInfo&	serverInfo,
			KServerPanel* sender,  
			bool acceptAtOnce /*= false*/) );

	Window* getPanel() { return m_pPanel; };
	void	clearSelectedServer();

	BarList& GetBarList()	{ return m_barList; };
//	void	RefreshStatus();
	bool	SelectDefaultServer();
	void	InsertServer(ServerInfo& newServer);

	void	SetTitleVisible( bool visible );
	static void	setStatusImage( const char* statusTxt, StaticImage& statusImg, StaticText& status );

	Window* getCurrentSelectedBar() { return m_pCurrentSelectedBar; };
	void	SetActiveBar( Window* activeBar, bool acceptAtOnce = false );
private:	
	bool	sbScrollBar_handleScroll(const CEGUI::EventArgs& e);
	bool	Panel_MouseWheel(const CEGUI::EventArgs& e);
	bool	btnBar_MouseClick( const CEGUI::EventArgs& e );
	bool	btnBar_MouseDoubleClick( const CEGUI::EventArgs& e );

	void	InitBars();
	void	RefreshBars();
	void	SetBar( int num, Window* pCurrentBar );
	
	void	innerLoadServerList(
		RegionInfo* region,
		WindowManager* m_pWindowManager,
		Window* parentWindow,
		bool useRegionPrefix = false,
		bool reverseList = false);
public:
	
private:
	void (*ServerSelected_CallBack)( 
		ServerInfo& serverInfo,
		KServerPanel* sender,
		bool acceptAtOnce = false);

	Window*				m_pPanel;
	Window*				m_pList;
	TLVertScrollbar*	m_pScrollbar;
	Window*				m_pCurrentSelectedBar;
	WindowManager*		m_pWindowManager;

	BarList	m_barList;
	BarNameSet m_barNameSet;

	int		m_pageHeight;
	int		m_totalHeight;
	int		m_exceedHeight;
	int		m_barHeight;
	bool	m_useRegionPrefix;
	bool	m_reverseList;
	int		m_leastBarCount;
	
	RegionInfo*	m_regionInfo;
	BarServerInfoMap m_bar_serverinfo_map;
};


/********************************************************************
/*						class: KLoginList
*********************************************************************/

class KLoginList
{
public:
	KLoginList();
	~KLoginList();
	void	LoadLoginList( const string& fileName, bool bTempServerList, bool useEncrypt = true );

	vector<NetInfo>&	getNets();
	int				getVersion();
	int				getNetCount();
	
	static void	LoadServerInfo( KIniFile& ini, const string& sectionName, int index, ServerInfo& outInfo );
	static void	WriteServerInfo( KIniFile& ini, const string& sectionName, int index, const ServerInfo& inInfo );

private:
	void	innerLoadLoginList( const string& fileName, bool bTempServerList );
	void	LoadServerStatusList();
	void	LoadRegions(KIniFile& ini, NetInfo& destNet);
	void	LoadServers(KIniFile& ini, RegionInfo& destRegion);

public:

private:
	vector<NetInfo>	m_nets;
	KIniFile	m_serverStateIni;

	int			m_Version;
	int			m_NetCount;
};

unsigned int __stdcall DownLoadTempServerList( void* param );
void ShowLocalServerList( void );
bool isUpdateOk( void );


#endif // !defined(AFX_UISERVERLIST_H__4FE13C6D_70AC_42E6_85C3_2D4960DA8B81__INCLUDED_)
