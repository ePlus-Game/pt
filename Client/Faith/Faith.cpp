//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/26/2006 17:32
//      File_base        : Faith
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KSoundCache.h"
#include "GlobalDef.h"
#include "Faith.h"
#include "KWin32Wnd.h"
#include "iRepresentShell.h"
#include "Login/Login.h"
#include "NetConnect/NetConnectAgent.h"
#include "Ui/UiCase/UiGameSpace.h"
#include "cfs_filelogs.h"
#include "UiMDLInterface.h"
#include "mdump.h"

#include "Ui/KMessageCentre.h"
#include "Ui/UiCase/UiSearchHelpWnd.h"
#include "Ui/UiCase/UiChatWindow.h"

/////////////////////////////////
#include <list>
using namespace std;
#include <commctrl.h>
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatMainDlg.h"
/////////////////////////////////////

#include "loadSrcWnd/GDILoadBitmap.h"
#include "loadSrcWnd/ProcessBar.h"
using namespace LOAD_PROCESSBAR;
#include "loadSrcWnd/LoadSrcWnd.h"
#include "chatWindow/ChatMiniMap.h"


//////////////////////////////////////////////////////////////////////////
#include "KColors.h"
#include "ui/UiCase/UiMapCentre.h"
#include <atlbase.h>
extern CComModule _Module;

MiniDumper mDumper;

KClientCallback						g_ClientCallback;
struct iRepresentShell*				g_pRepresentShell		= NULL;
KMusic*								g_pMusicShell			= NULL;
iCoreShell*							g_pCoreShell			= NULL;
IInlinePicEngineSink*				g_pIInlinePicSink		= NULL;
int									g_bRepresent3			= NULL;
const unsigned short				s_dwBreathePerSecond	= 10000;
bool								g_bShowFPS				= false;
const unsigned short				g_uFPSOptimize			= 6;
bool								g_draw					= true;
extern KSoundCache					g_SoundCache;
bool								g_showtip				= true;
char								g_ServerInfo[64];	
char								g_Accounts[256];
bool								g_useServerList			= true;		

/************************************************************************/
/*					Implement class KB2Frame                            */
/************************************************************************/

KB2Frame::KB2Frame()
{
	m_dwPaintCounter		= 0;
	m_nPaintFpsLimit	= 0;
	m_hSemaphore		= NULL;
	m_WarningCount		= 0;
	memset( &m_devMode, 0, sizeof(m_devMode));
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	m_hRepresentModule	= NULL;
	m_bRepresent3		= FALSE;
#endif
}

KB2Frame::~KB2Frame()
{

}	

BOOL KB2Frame::GameInit()
{
	char szCurDir[256];
	::GetCurrentDirectory(256, szCurDir);

	std::string strCurPath;
	strCurPath += szCurDir;

	std::string strFontPath;

	strFontPath = strCurPath + "\\data\\simsun.ttc";
	::DeleteFile(strFontPath.c_str());

	strFontPath = strCurPath + "\\data\\arial.ttf";
	::DeleteFile(strFontPath.c_str());

	strFontPath = strCurPath + "\\data\\CuLi.TTF";
	::DeleteFile(strFontPath.c_str());

	strFontPath = strCurPath + "\\data\\SIMLI.TTF";
	::DeleteFile(strFontPath.c_str());

	strFontPath = strCurPath + "\\data\\stzhongs.ttf";
	::DeleteFile(strFontPath.c_str());

	LoadSrcWnd loadSrcWnd;
	CreateAlphaTableForRGB16BIT565();
	g_SetRootPath( NULL );
	g_SetFilePath( DEFAULT_ROOT_DIR );
	if ( !m_pakList.Open( PACKAGE_INI ) )
	{
		KClientError::Error_SetErrorCode( ERR_T_FILE_NO_FOUND, PACKAGE_INI );
		return FALSE;
	}

	if ( !m_iniFile.Load( CONFIG_INI ) )
	{
		KClientError::Error_SetErrorCode( ERR_T_FILE_NO_FOUND, CONFIG_INI );
		return FALSE;
	}

	mDumper.LoadConfig();

	KIniFile ini;
//<----EXVERSION
	if ( ini.Load( VERSION_CFG))
	{
		char curVersion[32];
		int nMajorVersion = 0;
		int nVersion = 0;
		int nNeedVersion = 0;
		ini.GetInteger( "Version", "MajorVersion", 0, &nMajorVersion );
		ini.GetInteger( "Version", "Version", 0, &nVersion );
		ini.GetInteger( "Version", "NeedVersion", 0, &nNeedVersion );
		sprintf( curVersion, "%d,%d,%d", nMajorVersion, nVersion, nNeedVersion );

		char oldVersion[32];
//<-----EXVERSION	
		m_iniFile.GetString( "LastLogin", LAST_VERSION, "", oldVersion, 32 );
		if ( oldVersion[0] == 0 )
		{
			g_showtip = true;
		}
		else
		{
			if ( strcmp( oldVersion, curVersion ) == 0 )
			{
				g_showtip = false;
			}
			else
			{
				g_showtip = true;
			}//*/
		}
	}

	::EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &m_devMode);
	DEVMODE	devMode;
	memset( &devMode, 0, sizeof(devMode));
	memcpy( &devMode, &m_devMode, sizeof(devMode));
	if ( devMode.dmBitsPerPel != 16 && g_showtip)
	{
		//if ( IDYES == MessageBox( g_GetMainHWnd(), MSG_16BIT_ADVISE, ERROR_MSGBOX_TITLE, MB_YESNO )) 
		{
			devMode.dmBitsPerPel = 16;
			devMode.dmFields = DM_BITSPERPEL|DM_DISPLAYFREQUENCY;
			::ChangeDisplaySettings( &devMode, CDS_UPDATEREGISTRY );
		}
	}

	//init for ole
	OleInitialize( 0 );
	CoInitialize(NULL);

	_Module.Init( NULL, g_GethInstance() );
	AtlAxWinInit();
	
	m_hSemaphore = ::CreateSemaphore( NULL, 0, 1, FS2ONLINE_GUID );
	int bScreen = FALSE;
	m_iniFile.GetInteger( _TT("GameSetting"), _TT("FullOrWin"), FALSE, &bScreen );
	g_SetFullScreen( bScreen );
	
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	m_iniFile.GetInteger( _TT("Client"), _TT("Represent"), 0, &m_bRepresent3 );
#endif
	int nScreenW = 0;
	int nScreenH = 0;
	m_iniFile.GetInteger( _TT("GameSetting"), _TT("ScreenWidth"), DEFAULT_SCREEN_WIDTH, &nScreenW );
	m_iniFile.GetInteger( _TT("GameSetting"), _TT("ScreenHeight"), DEFAULT_SCREEN_HEIGHT, &nScreenH );

	g_SetScreenHeight( nScreenH );
	g_SetScreenWidth( nScreenW );

	int bFontShadow = TRUE;
	m_iniFile.GetInteger( _TT("GameSetting"), _TT("FontShadow"), TRUE, &bFontShadow );
	g_SetFontBorder( bFontShadow > 0 ? true : false );
	//初始化显示设备
	if ( !InitRepresentShell( g_IsFullScreen(), g_GetScreenWidth(), g_GetScreenHeight() ) )
	{
		KClientError::Error_SetErrorCode( ERR_T_REPRESENT2_INIT_FAILED, NULL );
		return FALSE;
	}
	

	loadSrcWnd.LoadSrcWndLoadCfgINT();
	loadSrcWnd.LoadSrcWndCreate(KWin32App::m_hMainWnd);
	loadSrcWnd.LoadSrcWndLoadToDestProcess(0.2f,NULL);
	/////////////
	//加载资源
 	if ( !m_uiAdapter.UiInit( g_pRepresentShell ) )
	{
		KClientError::Error_SetErrorCode( ERR_T_MODULE_INIT_FAILED, _TT("UiInit") );
		return FALSE;
	}

	m_Sound.Init();
	loadSrcWnd.LoadSrcWndLoadToDestProcess(0.4f,NULL);
	
	//加载设定配置表格

	if ( (g_pCoreShell = CoreGetShell()) == NULL )
	{
		KClientError::Error_SetErrorCode( ERR_T_MODULE_INIT_FAILED, _TT("CoreGetShell") );
		return FALSE;
	}

	g_pCoreShell->SetRepresentShell( g_pRepresentShell );

	g_pCoreShell->InitSimplifiedNpc( FALSE );

	g_pCoreShell->SetMusicInterface( (KMusic*)&m_Music );

	g_pCoreShell->SetCallDataChangedNofify( &g_ClientCallback );

	g_pCoreShell->SetRepresentAreaSize( g_GetScreenWidth(), g_GetScreenHeight() );

	g_pMusicShell = &m_Music;
	loadSrcWnd.LoadSrcWndLoadToDestProcess(0.2f,NULL);

	//初始化网络设备
	if ( !g_NetConnectAgent.Initialize() )
	{
		KClientError::Error_SetErrorCode( ERR_T_MODULE_INIT_FAILED, _TT("NetConnectAgent") );
		return FALSE;
	}

	m_dwPaintCounter = 0;
	m_Timer.Start();

	loadSrcWnd.LoadSrcWndLoadToDestProcess(0.1f,NULL);
	//打开界面

 	if( !m_uiAdapter.UiStart() )
 	{
 		KClientError::Error_SetErrorCode( ERR_T_MODULE_INIT_FAILED, _TT("UiStart") );
 		return FALSE;
	}
	loadSrcWnd.LoadSrcWndLoadToDestProcess(0.1f,NULL);
	g_LoginLogic.LoginStart();
	loadSrcWnd.LoadSrcWndDestroy();

#ifdef USING_CHAT_WINDOW
	ChatMainDlg chatDlg;
	chatDlg.MainDlgInit();
#endif	

	return TRUE;
}

BOOL KB2Frame::GameExit()
{
	g_NetConnectAgent.Destroy();
	

	ReleaseMDL();

	g_SoundCache.Release();
	m_Music.Close();
	m_Sound.Exit();
	
	if ( g_pCoreShell )
	{
		g_pCoreShell->SetRepresentShell( NULL );
		g_pCoreShell->SetClient( NULL, -1 );
		g_pCoreShell->SetMusicInterface( NULL );
		g_pCoreShell->Release();
		g_pCoreShell = NULL;
	}

	if( !m_uiAdapter.UiExit() )
 	{
 		KClientError::Error_SetErrorCode( ERR_T_MODULE_INIT_FAILED, _TT("UiExit") );
 		return FALSE;
	}

	g_pMusicShell = NULL;


	if ( g_pRepresentShell )
	{
		g_pRepresentShell->Release();
		g_pRepresentShell = NULL;
	}


#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	if (m_hRepresentModule)
	{
		::FreeLibrary( m_hRepresentModule );
		m_hRepresentModule = NULL;
	}
#endif

	::ShowCursor(TRUE);

	m_pakList.Close();

	if ( m_hSemaphore )
	{
		::CloseHandle(m_hSemaphore);
	}
	
	//解决双开问题
	//m_devMode.dmFields = DM_BITSPERPEL|DM_DISPLAYFREQUENCY;
	//::ChangeDisplaySettings( &m_devMode, 0 );

	::CoUninitialize();

	return TRUE;
}

BOOL KB2Frame::GameLoop()
{
	static KTimer s_fpstime;
	static KTimer s_gamelooptime;
	static int nFPS = 0;
	static int nGameLoop = 0;

	static unsigned int nPaintCounter	= 0;

	///设置丢失变量//
	static bool isDeviceLost = false;


	g_NetConnectAgent.Breathe();

	//每10秒向gameserver送一个ping包
	static unsigned long ulTime = time(NULL);
	if( time(NULL) - ulTime > g_dwPingPerSecond )
	{
		PING_COMMAND	PingCmd;
		PingCmd.ProtocolType = c2s_ping;
		PingCmd.m_dwTime	 = ::GetTickCount();
		g_NetConnectAgent.SendMsg( &PingCmd, sizeof(PING_COMMAND) );
		ulTime = time( NULL );		
	}//*/

	DWORD curTimer = m_Timer.GetElapse();

	static DWORD oldTime = 0;

	if ( curTimer - oldTime >= 1000 / GAME_FPS )
	{
		oldTime = curTimer;
		//add by render..
		//焦点切换检查//
		ChatMainDlg::CheckFocus();
		g_pCoreShell->Breathe();
		m_uiAdapter.UiBreathe();
		m_uiAdapter.UiTimePulse( ::GetTickCount() );		
		s_gamelooptime.GetFPS( &nGameLoop );
	}		
	else if ( (unsigned int)(nPaintCounter * 1000) <= curTimer * g_GamePaintLimit)
	{
		nPaintCounter++;

		if(isDeviceLost == false)
		{
			g_pRepresentShell->RepresentBegin( false, 0 );

			if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME )
			{
				// 顺序一定不能改变！！！
				g_WndGameSpace.BreatheWindow();
				if(g_draw && !KWin32Frame::s_minisized)
				{
					g_WndGameSpace.PaintWindow();
				}
			}
			
			if ( g_bShowFPS )
			{
				char szFPS[COMMON_CLIENT_MSG_LEN_64];
				sprintf( szFPS, "GAMELOOP:%d FPS:%d", nGameLoop, nFPS );
				CEGUI::FontManager* pCeguiFont = CEGUI::FontManager::getSingletonPtr();
				if ( pCeguiFont )
				{
					CEGUI::Font *pFnt = pCeguiFont->getFont( CEGUI::AnsiToUtf8( "stzhongs-10" ) );
					if ( pFnt )
					{
						CEGUI::Point point( (500 - pFnt->getTextExtent(szFPS) / 2), 10 );
						CEGUI::Size	size( pFnt->getTextExtent(szFPS), pFnt->getLineSpacing());
						CEGUI::Rect rect(point,size);
						CEGUI::ColourRect color;
						color.setColours(CEGUI::colour( 0xffffffff ) );
						pFnt->drawText( szFPS, rect, 0, CEGUI::Centred, color );
					}
				}
			}

			g_pRepresentShell->RepresentEnd();
			
			if(g_draw && !KWin32Frame::s_minisized)
			{
				g_pRepresentShell->RepresentEnd();
				
				if(g_LoginLogic.GetStatus() == LL_S_IN_GAME)
				{
					m_uiAdapter.UiPaintNpcHeadInfo(NULL);
					KUiMiniMap::PaintMiniMap();
					if(!KUiSceneMap::getSinglton().isVisible())
						m_uiAdapter.UiPaintBottomOldWindow(NULL);
					
				}
				
				g_pRepresentShell->RepresentEnd();
				m_uiAdapter.UiPaintNewMode(NULL);
				if(g_LoginLogic.GetStatus() == LL_S_IN_GAME)
					g_WndGameSpace.PaintUiEffect();
				m_uiAdapter.UiPaintOldMode(NULL);
			}
			g_pRepresentShell->RepresentEnd();
			//得到米你地图信息//
//			g_pRepresentShell->BltBackBufferToSurface(miniMap.GetSurface(),miniMap.GetPaintX(),miniMap.GetPaintY(),miniMap.GetPaintWidth(),miniMap.GetPaintHeight());
			g_pRepresentShell->UpdataSceen();	
			
			s_fpstime.GetFPS( &nFPS );

			CheckFPS(nFPS);
		}
		if(g_pDirectDraw->SurfaceLost())
		{
			
			if(g_IsFullScreen() == false)
			{
				if(GetActiveWindow()==KWin32App::m_hMainWnd)
				{
					if(g_pRepresentShell->ProcessSurfaceLost(g_GetScreenWidth(),g_GetScreenHeight(),g_IsFullScreen()))
					{
						isDeviceLost = false;
					}
					else
					{
						isDeviceLost = true;
					}
				}
				else
				{
					isDeviceLost = true;
					if(g_pDirectDraw->RestoreSurface())
						isDeviceLost = false;
				}
			}
		}
		else
		{
			isDeviceLost = false;
		}
	}
	else
	{
		Sleep(1);
	}

	return TRUE;
}

int KB2Frame::HandleInput( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	int nRet = m_uiAdapter.UiProcessInput( uMsg, wParam, lParam );
	if ( g_pCoreShell && nRet <= 0 )
	{
		nRet |= g_WndGameSpace.handleInput( uMsg, wParam, lParam );
	}
	return nRet;
}

void KB2Frame::SetFocus()
{
	m_uiAdapter.SetFocus();
}

BOOL KB2Frame::InitRepresentShell( BOOL bFullScreen, int nWidth, int nHeight )
{
	if (g_pRepresentShell == NULL)
	{
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
		if (m_hRepresentModule == NULL &&
			(m_hRepresentModule = ::LoadLibrary( m_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2)) == NULL )
		{
			KClientError::Error_SetErrorCode( ERR_T_LOAD_MODULE_FAILED, m_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2 );
			return FALSE;
		}
		fnCreateRepresentShell pCreate = (fnCreateRepresentShell)::GetProcAddress( m_hRepresentModule, CREATE_REPRESENT_SHELL_FUN );
		if ( pCreate == NULL || (g_pRepresentShell = pCreate()) == NULL )
		{
			KClientError::Error_SetErrorCode( (pCreate == NULL) ? ERR_T_MODULE_UNCORRECT : ERR_T_MODULE_INIT_FAILED, m_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2 );
			return FALSE;
		}
#else
		g_pRepresentShell = CreateRepresentShell();
#endif
	}
	if(g_pRepresentShell->Create( nWidth, nHeight, bFullScreen != 0 ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
void KB2Frame::CheckFPS( int FPS )
{
	if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME && m_WarningCount < 3 )
	{ 
		if ( m_vFPSCounter.size() < (g_GamePaintLimit*10) )
		{
			m_vFPSCounter.push_back(FPS);
		}
		else
		{
			int totle = 0;
			int average;
			for( int i = 0; i < m_vFPSCounter.size(); i++ )
			{
				totle += m_vFPSCounter[i];
			}
			average = totle/(g_GamePaintLimit*10);
			m_vFPSCounter.clear();
			
			if ( average < 20 )
			{
				m_WarningCount++;
				KUiChannelCentre::GetSingleton().toSysMsg(KMessageCentre::GetMessage(common_message,4));
				//KUiSearchHelpWnd::GetSingleton().ShowSimpleHelp(KMessageCentre::GetMessage(common_message,4));
			}
		}
	}
}

/************************************************************************/
/*						Implement Class B2Application                   */
/************************************************************************/

BOOL B2Application::CreateApplication( KWin32AppParam &rAppParam )
{
	CFS_FILELOGS::WriteLog("Query your system...\n");
	LOGSystemInfo();
	CFS_FILELOGS::WriteLog("Query finished.\n");

	try
    {
		if ( KWin32App::CreateApplication( rAppParam ))
		{
			
			return TRUE;
		}
    }
    catch ( CEGUI::Exception& E )
    {
        OutputExceptionMessage( CEGUI::Utf8ToAnsi( E.getMessage() ) );
    }
    catch (std::exception& E)
    {
        OutputExceptionMessage( E.what() );
    }
  /*  catch( ... )
    {
        OutputExceptionMessage( _TT("Unknown exception was caught!") );
    }//*/	

	KWin32App::StopApplication();
	return FALSE;
}

BOOL B2Application::RunApplication()
{
//	try
    {
		return KWin32App::RunApplication();
    }
/*    catch ( CEGUI::Exception& E )
    {
        OutputExceptionMessage( CEGUI::Utf8ToAnsi( E.getMessage() ) );
    }
    catch (std::exception& E)
    {
        OutputExceptionMessage( E.what() );
    }
/*    catch( ... )
    {
        OutputExceptionMessage( _TT("Unknown exception was caught!") );
    }//*/	
	KWin32App::StopApplication();
	return FALSE;

}

void B2Application::OutputExceptionMessage( const TCHAR* szMessage ) const
{
    ::MessageBox( 0, szMessage, _TT("CEGUI - Exception"), MB_OK|MB_ICONERROR );
}

void B2Application::LOGSystemInfo()
{
	// Write OS info
	CFS_FILELOGS::WriteLog("%s\n", GetOSInfo());
	// Write CPU info
	HKEY hKey;
	char szCPUID[MAXPATH+1] = {0};
	char szCPUName[MAXPATH+1] = {0};
	if(ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0, KEY_QUERY_VALUE, &hKey))
	{
		DWORD dwCPUIDSize = sizeof(szCPUID);
		DWORD dwCPUNameSize = sizeof(szCPUName);
		::RegQueryValueEx(hKey, "Identifier", NULL, NULL, (LPBYTE)szCPUID, &dwCPUIDSize);
		::RegQueryValueEx(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)szCPUName, &dwCPUNameSize);
		
		::RegCloseKey(hKey);
	}

	if(strlen(szCPUID) == 0)
	{
		strcpy(szCPUID, "Unknown");
	}

	if(strlen(szCPUName) == 0)
	{
		strcpy(szCPUName, "Unknown");
	}

	CFS_FILELOGS::WriteLog("CPU Identifier (%s)\n", szCPUID);
	CFS_FILELOGS::WriteLog("CPU Name (%s)\n", szCPUName);

	// Write hardware info
	WriteHardware();

	// Write memory info
	MEMORYSTATUS tagMemInfo;
	::GlobalMemoryStatus(&tagMemInfo);
	CFS_FILELOGS::WriteLog("Total physical memory %d Mb (Free %d Mb)\n", 
		tagMemInfo.dwTotalPhys/(1024*1024), 
		tagMemInfo.dwAvailPhys/(1024*1024));
	CFS_FILELOGS::WriteLog("Total paging memory %d Mb (Free %d Mb)\n", 
		tagMemInfo.dwTotalPageFile/(1024*1024), 
		tagMemInfo.dwAvailPageFile/(1024*1024));
	CFS_FILELOGS::WriteLog("Total virtual memory %d Mb (Free %d Mb)\n", 
		tagMemInfo.dwTotalVirtual/(1024*1024), 
		tagMemInfo.dwAvailVirtual/(1024*1024));
}

void B2Application::WriteHardware()
{
	HKEY hKey;
	if(ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SYSTEM\\CurrentControlSet\\Enum\\PCI",
		0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hKey))
	{
		int nCurKeyIndex = 0;
		char szCurKeyName[MAXPATH+1] = {0};
		while(ERROR_NO_MORE_ITEMS != ::RegEnumKey(hKey, 
			nCurKeyIndex, szCurKeyName, MAXPATH+1))
		{
			HKEY hCurSubKey;
			if(ERROR_SUCCESS == ::RegOpenKeyEx(hKey, szCurKeyName,
				0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS, &hCurSubKey))
			{
				int nCurSubKeyIndex = 0;
				char szCurSubKeyName[MAXPATH+1] = {0};
				while(ERROR_NO_MORE_ITEMS != ::RegEnumKey(hCurSubKey, 
					nCurSubKeyIndex, szCurSubKeyName, MAXPATH+1))
				{
					HKEY hCurValKey;
					if(ERROR_SUCCESS == ::RegOpenKeyEx(hCurSubKey, szCurSubKeyName,
						0, KEY_QUERY_VALUE, &hCurValKey))
					{
						int nCurValKeyIndex = 0;
						char szCurGetVal1[MAXPATH+1] = {0};
						DWORD dwCurGetValSize1 = sizeof(szCurGetVal1);
						::RegQueryValueEx(hCurValKey, "Class", NULL, NULL, 
							(LPBYTE)szCurGetVal1, &dwCurGetValSize1);
						if(strcmp(szCurGetVal1, "Display") == 0 || 
							strcmp(szCurGetVal1, "Net") == 0 || 
							strcmp(szCurGetVal1, "MEDIA") == 0)
						{
							char szCurGetVal2[MAXPATH+1] = {0};
							DWORD dwCurGetValSize2 = sizeof(szCurGetVal2);
							::RegQueryValueEx(hCurValKey, "DeviceDesc", NULL, NULL, 
								(LPBYTE)szCurGetVal2, &dwCurGetValSize2);
							CFS_FILELOGS::WriteLog("%s (%s)\n", szCurGetVal1, szCurGetVal2);
						}

						::RegCloseKey(hCurValKey);
					}
					
					nCurSubKeyIndex ++;
				}

				::RegCloseKey(hCurSubKey);
			}

			nCurKeyIndex ++;
		}
		
		::RegCloseKey(hKey);
	}
}

const char *B2Application::GetOSInfo()
{
	OSVERSIONINFO tagVerInfo;
	tagVerInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	static char szOSInfo[MAXSIZE_OSINFO] = {0};
	if(strlen(szOSInfo) == 0 && ::GetVersionEx(&tagVerInfo))
	{
		if(tagVerInfo.dwMajorVersion == 3)
		{
			if(tagVerInfo.dwMinorVersion == 51)
			{
				strcpy(szOSInfo, "Windows NT 3.51");
			}
		}
		else if(tagVerInfo.dwMajorVersion == 4)
		{
			if(tagVerInfo.dwMinorVersion == 0)
			{
				if(tagVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
				{
					strcpy(szOSInfo, "Windows NT 4.0");
				}
				else if(tagVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				{
					strcpy(szOSInfo, "Windows 95");
				}
			}
			else if(tagVerInfo.dwMinorVersion == 10)
			{
				strcpy(szOSInfo, "Windows 98");
			}
			else if(tagVerInfo.dwMinorVersion == 90)
			{
				strcpy(szOSInfo, "Windows Me");
			}
		}
		else if(tagVerInfo.dwMajorVersion == 5)
		{
			if(tagVerInfo.dwMinorVersion == 0)
			{
				strcpy(szOSInfo, "Windows 2000");
			}
			else if(tagVerInfo.dwMinorVersion == 1)
			{
				strcpy(szOSInfo, "Windows XP");
			}
		}

		if(strlen(szOSInfo) == 0)
		{
			strcpy(szOSInfo, "Unknown Windows");
		}

		if(strcmp(tagVerInfo.szCSDVersion, "C") == 0)
		{
			strcat(szOSInfo, " ");
			strcat(szOSInfo, "OSR2");
		}
		else if(strcmp(tagVerInfo.szCSDVersion, "A") == 0)
		{
			strcat(szOSInfo, " ");
			strcat(szOSInfo, "SE");
		}
		else
		{
			if(strlen(tagVerInfo.szCSDVersion) > 0)
			{
				strcat(szOSInfo, " ");
			}
			strcat(szOSInfo, tagVerInfo.szCSDVersion);
		}
	}

	return szOSInfo;
}

/************************************************************************/
/*							Application Main()                          */
/************************************************************************/

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#ifdef _DEBUG
	EnableMemLeakCheck();
//	_CrtSetBreakAlloc(1475137);

#endif

	memset( g_Accounts, 0, 256 );

// 	if ( lpCmdLine[0] == '\0' )
// 	{
// 		::MessageBox(g_GetMainHWnd(), APPLICATION_ERROR_6, APPLICATION_NAME, MB_OK );
// 		return 0;
// 	}
	if ( lpCmdLine[0] == '\0' )
	{
		g_useServerList = true;
	}
	else
	{
		g_useServerList = false;
		
		std::string strCmdLine = lpCmdLine;
		int nBegin = strCmdLine.find_first_of( ":" );
		int nEnd = strCmdLine.find_first_of( " " );
		
		std::string ip = strCmdLine.substr(0, nBegin );
		std::string port = strCmdLine.substr(nBegin + 1, nEnd- nBegin - 1 );
		std::string info = strCmdLine.substr(nEnd + 1, strCmdLine.size() );
		
		DWORD dwPort = atoi( port.c_str() );
		memcpy(g_ServerInfo, info.c_str(), 64 );
		
		g_LoginLogic.SetGameServerIP( ip.c_str(), dwPort );
	}


	KWin32AppParam s4ClientParam;
	s4ClientParam.hInstance					= hInstance;
	s4ClientParam.bMulti					= TRUE;
	s4ClientParam.bShowMouse				= TRUE;
	s4ClientParam.bFullScreen				= FALSE;
	s4ClientParam.uScreenHeight				= 0;
	s4ClientParam.uScreenWidth				= 0;
	s4ClientParam.uMouseHoverTimeSetting	= DEFAULT_MOUSEHOVER_TIME;
	s4ClientParam.pMainFrame				= new KB2Frame; 
	kstrcpy( s4ClientParam.szAppName, APPLICATION_NAME );
	
	BOOL bOk = FALSE;
	B2Application s4Client;
	CFS_FILELOGS::WriteLog("FSOnline2 client start...\n");
	bOk = s4Client.CreateApplication( s4ClientParam );
	if(bOk)
	{
		CFS_FILELOGS::WriteLog("FSOnline2 client start Ok!\n");
		s4Client.RunApplication();
	}
	else
	{
		CFS_FILELOGS::WriteLog("FSOnline2 client start Fail!\n");
	}

	CFS_FILELOGS::WriteLog("FSOnline2 client exit\n\n");

	return 0;
}

