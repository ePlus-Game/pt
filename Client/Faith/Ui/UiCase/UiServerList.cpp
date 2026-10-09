// UiServerList.cpp: implementation of the KUiServerList class.
//
//////////////////////////////////////////////////////////////////////

#include "UiServerList.h"
#include <sstream>
#include "../KMessageCentre.h"
#include "KIniFile.h"
#include <map>
#include "UiLogin.h"
#include "../../Login/Login.h"
#include "UiLoginBg.h"
#include "../../FaithEncrypter.h"
#include "mdump.h"
#include "Wininet.h"
#include "ui/UiCase/UiComMsgBox.h"
#include "KCriticalSection.h"
#include "process.h"
#include "../UiConfigManager.h"
#include "UiInfoBar.h"

#define MAX_NET_FILE 1024


using namespace std;
using namespace CEGUI;

KCriticalSection g_serverListLock;
bool			g_updateOk = false;
HANDLE			g_updateEvent = NULL;
bool			g_bShowServerList = false;
BOOL			g_updateRet = FALSE;

void setUpdateOk( bool bOk )
{
	KAutoCriticalSection autoLock(g_serverListLock);
	g_updateOk = bOk;
}

bool isUpdateOk( void )
{
	KAutoCriticalSection autoLock(g_serverListLock);
	return g_updateOk;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const string PanelLayoutsFileName = "uisettings/layouts/ServerListPanel.ls";
const string TempServerListFileName   = "/UserData/tempserver.ini";
const string ServerListFileName   = "/UserData/server.ini";
const string EncriptTmpFile       = "/UserData/Encrypttmp.ini";
const string TempEncriptTmpFile   = "/UserData/TempEncrypttmp.ini";
const string BarLayoutsFileName = "uisettings/layouts/ServerListBar.ls";
const string RecentServerFileName = "/UserData/RecentServer.ini";
const string TempServerStateFileName = "/UserData/tempstate.ini";
const string ServerStateFileName = "/UserData/state.ini";
const string SectionName_RecentServerRegion = "Net_0_Region_0";

#define SERVER_STATUS_IMAGESET			"fuwuqizhuangtaitubiao"

#define SERVER_STATUS_IMAGE_WEIHU		"hei"
#define SERVER_STATUS_IMAGE_LIANGHAO	"lv"
#define SERVER_STATUS_IMAGE_FANMANG		"huang"
#define SERVER_STATUS_IMAGE_BAOMAN		"hong"
//#define SERVER_STATUS_IMAGE_TUIJIAN		"huang"

const colour RED_COLOUR = colour(1.0f, 0, 0);
const colour GREEN_COLOUR = colour(0, 1.0f, 0);
//const colour BLUE_COLOUR = colour(0, 0, 1.0f);
const colour GREY_COLOUR = colour(0.58f, 0.58f, 0.58f);
const colour YELLOW_COLOUR = colour(1.0f, 1.0f, 0);
const colour WHITE_COLOUR = colour(0, 1.0f, 1.0f);

colour WEIHU_COLOUR = GREY_COLOUR;
colour LIANGHAO_COLOUR = GREEN_COLOUR;
colour FANMANG_COLOUR = YELLOW_COLOUR;
colour BAOMAN_COLOUR = RED_COLOUR;

const Image* WEIHU_IMAGE = NULL;
const Image* LIANGHAO_IMAGE = NULL;
const Image* FANMANG_IMAGE = NULL;
const Image* BAOMAN_IMAGE = NULL;

string s_serverNameFormatString;
const string DEFAULT_SERVERNAME_FORMAT_STRING = "<Layout width=180><Seg text-align=left float=left><Obj type=text color=255,255,255 vertical-align=center font-family=stzhongs-9>%s</Obj></Seg></Layout>";


Point g_ServerListWndPos( 0.0f, 0.0f );

static bool		s_bFirst = true;

extern	KLogin		g_LoginLogic;

void CALLBACK InternetStatusCallBack(
									 HINTERNET hInternet,
									 DWORD dwContext,
									 DWORD dwInternetStatus,
									 LPVOID lpvStatusInformation,
									 DWORD dwStatusInformationLength
									 )
{
	
}

//int		KUiServerListStateUpdate::m_nCurrentHost;
//std::vector<std::string> KUiServerListStateUpdate::m_strHosts;

typedef unsigned __int32 CRC_UINT32;
static const CRC_UINT32 CRC_Table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
		0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
		0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
		0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
		0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
		0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
		0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
		0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
		0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
		0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
		0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
		0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
		0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
		0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
		0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
		0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
		0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
		0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
		0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
		0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
		0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
		0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
		0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
		0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
		0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
		0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
		0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
		0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
		0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
		0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
		0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
		0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
		0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
		0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
		0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
		0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
		0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
		0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
		0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
		0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
		0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
		0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
		0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
		0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
		0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
		0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
		0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
		0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
		0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
		0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
		0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
		0x2d02ef8dL
};

KUiServerListStateUpdate::KUiServerListStateUpdate()
{
	m_nCurrentHost = 0;
}

BOOL KUiServerListStateUpdate::HttpDownLoadFile(const char * szPath,const char * szDestPath)
{
	HINTERNET hSession = ::InternetOpen(
		0,
		INTERNET_OPEN_TYPE_PRECONFIG,
		0,
		0,
		0);
	if (!hSession) 
	{
		return FALSE;
	}
	
	DWORD tmpTimeOut = 0xffffffff;
	BOOL bOption = ::InternetSetOption( 
		hSession,
		INTERNET_OPTION_CONNECT_TIMEOUT,
		&tmpTimeOut, 
		sizeof(tmpTimeOut) );
	if ( !bOption )
	{
		return FALSE;
	}
	
	bOption = ::InternetSetOption( 
		hSession,
		INTERNET_OPTION_SEND_TIMEOUT,
		&tmpTimeOut, 
		sizeof(tmpTimeOut) );  
	if ( !bOption )
	{
		return FALSE;
	}

	bOption = ::InternetSetOption( 
		hSession,
		INTERNET_OPTION_RECEIVE_TIMEOUT,
		&tmpTimeOut, 
		sizeof(tmpTimeOut) );  
	if ( !bOption )
	{
		return FALSE;
	}
	
	INTERNET_STATUS_CALLBACK pCallBack = ::InternetSetStatusCallback( 
		hSession,
		(INTERNET_STATUS_CALLBACK)&InternetStatusCallBack );
	if ( pCallBack == INTERNET_INVALID_STATUS_CALLBACK )
	{
		return FALSE;
	}
    
    char buffer[MAX_NET_FILE];
    DWORD bytes_read = 0;
	
    HINTERNET hNetFile = ::InternetOpenUrl(
								hSession, 
								szPath,
								NULL, 
								0,
								INTERNET_FLAG_RELOAD,
								0);	
    if( !hNetFile )
        return FALSE;

	bOption = ::InternetSetOption( 
		hNetFile,
		INTERNET_OPTION_CONNECT_TIMEOUT,
		&tmpTimeOut, 
		sizeof(tmpTimeOut) );
	if ( !bOption )
	{
		return FALSE;
	}
	
	bOption = ::InternetSetOption( 
		hNetFile,
		INTERNET_OPTION_SEND_TIMEOUT,
		&tmpTimeOut, 
		sizeof(tmpTimeOut) );  
	if ( !bOption )
	{
		return FALSE;
	}
	
	bOption = ::InternetSetOption( 
		hNetFile,
		INTERNET_OPTION_RECEIVE_TIMEOUT,
		&tmpTimeOut, 
		sizeof(tmpTimeOut) );  
	if ( !bOption )
	{
		return FALSE;
	}
    
 	HANDLE hFile = ::CreateFile( 
		szDestPath,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	do
	{
		if (!::InternetReadFile (hNetFile, buffer, MAX_NET_FILE,  &bytes_read) )
		{
			break;
		}
		if ( !bytes_read )
		{
			break;
		}
		else
		{
			::WriteFile( hFile,  buffer, bytes_read, &bytes_read, NULL );
		}
	} while(TRUE);
	
	::CloseHandle( hFile );
	::InternetCloseHandle( hNetFile );
    ::InternetCloseHandle( hSession );
	
	return TRUE;
}

BOOL KUiServerListStateUpdate::FtpDownLoadFile(const char * szPath,const char * szDestPath)
{
	/*
	HINTERNET hSession = ::InternetOpen(
							0,
							INTERNET_OPEN_TYPE_PRECONFIG,
							0,
							0,
							0);
	if (!hSession) 
	{
		return FALSE;
	}
	
	DWORD tmpTimeOut = 10000;
	BOOL bOption = ::InternetSetOption(
						hSession,
						INTERNET_OPTION_CONNECT_TIMEOUT,
						&tmpTimeOut,
						sizeof(tmpTimeOut) );  
	if ( !bOption )
	{
		return FALSE;
	}

	bOption = ::InternetSetOption( 
					hSession,
					INTERNET_OPTION_SEND_TIMEOUT,
					&tmpTimeOut, 
					sizeof(tmpTimeOut) );  
	if ( !bOption )
	{
		return FALSE;
	}
	
	
	INTERNET_STATUS_CALLBACK pCallBack = ::InternetSetStatusCallback( 
											hSession,
											(INTERNET_STATUS_CALLBACK)&InternetStatusCallBack );
	if ( pCallBack == INTERNET_INVALID_STATUS_CALLBACK )
	{
		return FALSE;
	}
	
	HINTERNET hService = ::InternetConnect( 
							hSession, 
							putParam.addr,
							putParam.port,
							putParam.username,
							putParam.password,
							INTERNET_SERVICE_FTP, 
							INTERNET_FLAG_PASSIVE, 
							0 );
	if (!hService) 
	{
		return FALSE;
	}	
	
	HINTERNET hFtpFile = ::FtpGetFile(hService, putParam.serverfile,GENERIC_READ,FTP_TRANSFER_TYPE_BINARY, 0 );
	
	if ( !hFtpFile )
	{
		BOOL bRet = ::FtpPutFile(hService, putParam.locolfile, putParam.serverfile, FTP_TRANSFER_TYPE_BINARY, 0);		
		int err = ::GetLastError();
		::InternetCloseHandle( hFtpFile );
	}
	
	int nErr = ::GetLastError();							
	::InternetCloseHandle(hService);
	::InternetCloseHandle(hSession);
	
	::DeleteFile( putParam.locolfile );	
	//*/
	return TRUE;
}

unsigned      KUiServerListStateUpdate::CRC32(unsigned CRC, const void *pvBuf, int nLen)
{
    unsigned RetCode = 0;

    if (!pvBuf)
        return RetCode;

    __asm mov esi, [pvBuf]
    __asm mov ecx, [nLen]
    __asm mov eax, [CRC]
    __asm shr ecx, 3
    __asm xor eax, 0xffffffff

    __asm test ecx, ecx
    __asm jz CRC_Last7Bytes
    
    __asm push ebp
    __asm mov ebp, ecx

    __asm xor edx, edx
    __asm xor ecx, ecx

    __asm align 4

CRC_Loop1:
    // eax = CRC
    // edx first TableIndex
    // ecx second TableIndex

    // Process 4 bytes
    __asm mov ebx, [esi]
    __asm add esi, 4
    
    __asm mov dl, bl
    __asm mov cl, bh
    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4]
    __asm shr ebx, 16
    __asm xor eax, edi

    __asm xor cl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[ecx * 4]
    __asm mov dl, bl
    __asm xor eax, edi

    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4] 
    __asm mov cl, bh
    __asm xor eax, edi

    __asm mov ebx, [esi]
    __asm xor cl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[ecx * 4]
    __asm add esi, 4
    __asm xor eax, edi
    
    __asm mov dl, bl
    __asm mov cl, bh
    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4]
    __asm shr ebx, 16
    __asm xor eax, edi

    __asm xor cl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[ecx * 4]
    __asm mov dl, bl
    __asm xor eax, edi

    __asm xor dl, al
    __asm shr eax, 8
    __asm mov edi, CRC_Table[edx * 4] 
    __asm mov cl, bh
    __asm xor eax, edi

    __asm xor cl, al
    __asm shr eax, 8
    __asm xor eax, CRC_Table[ecx * 4]

    __asm dec ebp
    __asm jnz CRC_Loop1

    __asm pop ebp

CRC_Last7Bytes:

    __asm mov ecx, [nLen]

    __asm xor edx, edx
    __asm and ecx, 0x7
    __asm jz CRC_Exit

CRC_Loop2:

    __asm mov dl, [esi]
    __asm inc esi
    __asm xor dl, al
    __asm shr eax, 8
    __asm xor eax, CRC_Table[edx * 4]

    __asm dec ecx
    __asm jnz CRC_Loop2

CRC_Exit:

    __asm xor eax, 0xffffffff
    __asm mov [RetCode], eax

    return RetCode;
}

BOOL KUiServerListStateUpdate::Init()
{
	KIniFile	IniFile;
//<------EXVERSION
	if ( !IniFile.Load(AUTOUPDATE_INI) )
		return FALSE;

	m_nCurrentHost = 0;

	char szSiteKey[MAX_PATH];
	char szSite[MAX_PATH];
	int n = 1;	
	do 
	{
		szSite[0] = 0;
		szSiteKey[0] = 0;
		snprintf( szSiteKey, sizeof(szSiteKey), "ftpsite%d", n);
		IniFile.GetString("main", szSiteKey, "", szSite, MAX_PATH);
		if (szSite[0] != 0)
		{
			m_strHosts.push_back(szSite);
		}
		else
			break;
		n++;
	} while(szSite[0] != 0);	

	
	
	return (m_nCurrentHost >= 0);
}

bool  KUiServerListStateUpdate::CheckCRC(const char * szFileName,const unsigned long dwMatchCRC)
{
	KFile File;
	std::string szCRC;
    char *pvBuffer = NULL;
    int nRetCode   = 0;
    unsigned uResult    = 0;
    
    if (File.Open((char*)szFileName))
    {
        int nLen = File.Size();
        
        if (nLen > 0)
        {
            pvBuffer = new char[nLen + 1];
            if (pvBuffer != NULL)
            {
                nRetCode = File.Read(pvBuffer, nLen);
                if (nRetCode == nLen)
                    uResult = CRC32(0, pvBuffer, nLen);
            }
        }
        
    }
	File.Close();
    
    delete [] pvBuffer;
    
    return (uResult==dwMatchCRC);
}

LPCSTR KUiServerListStateUpdate::GetNextUpdateServer(int& nCurrent)
{
	if (m_nCurrentHost >= 0 && m_nCurrentHost < m_strHosts.size())
	{
		nCurrent = (m_nCurrentHost++);
		return ((LPCSTR)m_strHosts[nCurrent].c_str());
	}
	nCurrent = 0;
	return NULL;
}


unsigned long KUiServerListStateUpdate::ProcessCRC(HANDLE hFile)
{
	unsigned long dwCRC=0;
	unsigned long word=0;
	unsigned long dwSizeReaded=0;
	
	while(::ReadFile(hFile,&word,1,&dwSizeReaded,NULL) && dwSizeReaded!=0)
	{
		word=word-'0';
		dwCRC=dwCRC*10+word;
	}
	
	return dwCRC;
}

BOOL	KUiServerListStateUpdate::DownLoadFromPath(const char * szPath,const char * szDestPath)
{
	if ( szPath == NULL || szDestPath == NULL )
	{
		return FALSE;
	}
	
	return HttpDownLoadFile(szPath, szDestPath);
}

BOOL KUiServerListStateUpdate::ServerStateDownThread()
{
	BOOL bResult = FALSE;
	//下载服务器列表的网址获取,如网址为空则在之后的流程中从该配置其他结读取。
	char		URL[MAX_PATH];	
	KIniFile	IniFile;
	//<------EXVERSION
	if ( IniFile.Load(AUTOUPDATE_INI))
	{
		IniFile.GetString("main", "serverurl", "",URL, MAX_PATH);
	}

	//删除本地的旧版本server.ini.crc
	char szServerPath[MAX_PATH];
	g_GetRootPath(szServerPath);
	strcat(szServerPath, "\\UserData\\");
	g_CreatePath(szServerPath);
	strcat(szServerPath, "tempserver.ini");
	strcat(szServerPath, ".crc");

	unsigned long dwCRCLocal=0;	
	HANDLE hCRC = ::CreateFile(
						szServerPath,
						GENERIC_READ,
						FILE_SHARE_WRITE,
						NULL,OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL );

	if (hCRC!=INVALID_HANDLE_VALUE)
	{
		dwCRCLocal=ProcessCRC(hCRC);
		CloseHandle(hCRC);
		DeleteFile(szServerPath);
	}

	bool				bFixedDownServer	= ('\0' == URL[0]) ? false : true;
	int					nUpdateIndex		= 0;
	const char*			pSeverInfo			= NULL;
	IFaithEncrypter*	pEncrypter			= GetXOREncrypter();
	while ( ( pSeverInfo = GetNextUpdateServer( nUpdateIndex ) ) != NULL )
	{
		if(!bFixedDownServer)
		{
			_snprintf(URL, sizeof(URL), "%s%s", pSeverInfo, "index.ini");
		}

		strcat(URL,".crc");       //CRC的地址

		//Addition ServerState File Download first
		//Because no matter wheather the downloading is succsessful the process is goint right way
		char szStateURL[MAX_PATH];
		char szStateLocal[MAXPATH];
    
		g_GetRootPath(szStateLocal);
		strcat(szStateLocal, "\\UserData\\");
		g_CreatePath(szStateLocal);
		strcat(szStateLocal, "tempstate.ini");

		sprintf(szStateURL,URL);
		int iStateLen=strlen(szStateURL);
		szStateURL[iStateLen-13]=0;
		strcat(szStateURL,"state.ini");
		DownLoadFromPath(szStateURL,szStateLocal);
    
		//下载服务器上的 CRC
		bResult=DownLoadFromPath(URL,szServerPath);
		if (bResult)
		{
			//计算服务器上的CRC
			hCRC=CreateFile(szServerPath,GENERIC_READ,FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			
			unsigned long dwNewCRC=ProcessCRC(hCRC);
			CloseHandle(hCRC);
			
			//还原路径
			int lenLocal=strlen(szServerPath);
			szServerPath[lenLocal-4]=0;
			int lenServer=strlen(URL);
			URL[lenServer-4]=0;
			
			//只有当服务器上的和本地不同的时候才下载ServerList
			if (dwNewCRC!=dwCRCLocal )
			{
				bResult=DownLoadFromPath(URL,szServerPath);
				bResult=pEncrypter->Encrypt(szServerPath,szServerPath);
			}//endif
			else//服务器上的和本地CRC相同
			{
				//否则先解码本地文件Check CRC
				bResult=pEncrypter->Unencrypt(szServerPath,szServerPath);		
			
				if (bResult)
				{
					//再一次效验CRC，避免ServerList文件出错
					//因为用户有可能在解码的时候终止程序（断电等），那样,如果不检查就永远不会更新ServerList了	
					bool bRight=CheckCRC(szServerPath,dwNewCRC);
					if (!bRight)
					{		
						bResult=DownLoadFromPath(URL,szServerPath);
						pEncrypter->Encrypt(szServerPath, szServerPath);
						break;
					}
					else
					{
						pEncrypter->Encrypt(szServerPath, szServerPath);
						break;
					}
				}
				else
				{
					//本地Serverlist不存在（有可能被删了）
					bResult=DownLoadFromPath(URL,szServerPath);
					pEncrypter->Encrypt(szServerPath, szServerPath);
					break;
				}
				
			}//end else

			// 只要这个ftp能连接上，后面的ftp就不继续尝试了
			break;
		}
		else
			continue;

		if(bFixedDownServer)
			break;
	}

	// 注意: 调用了GetNextUpdateServer后
	// 一定要在 return之前一定要将m_nCurrentHost置为0
	m_nCurrentHost = 0;

	return bResult;
}

template<>
KUiServerList* KUiWndSingleton<KUiServerList>::ms_Singleton = NULL;

KUiServerList::KUiServerList( const String& id_name )
: KUiWndSingleton<KUiServerList>( id_name )
, m_recentPanel(NULL)
, m_loginList(NULL)
, m_recentList(NULL)
{
	m_recentPanel = new KServerPanel();
	m_loginList = new KLoginList();
	m_recentList = new KLoginList();
	m_templateBackGround = NULL;
	m_recentBackGround = NULL;
	m_exceedWidth = 0;
	m_recommendServerIndex = -1;
	m_mainBackGround = NULL;
	m_pScrollbar = NULL;
	for ( int i = 0; i < MAX_PANEL_LIMIT; i++ )
	{
		m_panelList[i] = NULL;
		m_backGroundList[i] = NULL;
	}

	m_selectedRecentServerBar = NULL;
	m_selectedRecentServerBar_Hover = NULL;
	m_selectedRecentServerBar_StatusImage = NULL;
	m_selectedRecentServerBar_ServerIP = NULL;
	m_selectedRecentServerBar_ServerName = NULL;
	m_selectedRecentServerBar_btnShowList = NULL;
	m_selectedRecentServerBar_ServerStatus = NULL;
	m_currentRecentServerBar= NULL;

	m_templateRect.setSize( Size( 0.0f, 0.0f ) );
	m_templateRect.setPosition( Point( 0.0f, 0.0f ) );
	
	m_recentServerCount = 0;

	char* szTempMsg = KMessageCentre::GetMessage( serverlist_message, 7 );
	if ( NULL != szTempMsg )
	{
		s_serverNameFormatString = szTempMsg;
	}
}

KUiServerList::~KUiServerList()
{
	if ( NULL != m_recentPanel )
	{
		delete m_recentPanel;
		m_recentPanel = NULL;
	}

	if ( NULL != m_loginList )
	{
		delete m_loginList;
		m_loginList = NULL;
	}

	if ( NULL != m_recentList )
	{
		delete m_recentList;
		m_recentList = NULL;
	}

	for ( int i = 0; i < MAX_PANEL_LIMIT; i++ )
	{
		if ( NULL != m_panelList[i] )
		{
			delete m_panelList[i];
			m_panelList[i] = NULL;
		}
	}
}

void KUiServerList::Show( bool bTempServerList )
{
	if ( !KUiServerList::IsVisible() )
	{
		//new KUiServerList( "uisettings/layouts/ServerList.ls" );
		KUiWndSingleton<KUiServerList>::Show();
		//KUiLoginBackGround::GetSingleton().HideServerInfo();
		if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
		{
			ms_Singleton->m_pThisWnd->activate();
			if ( s_bFirst == false )
			{
				ms_Singleton->UpdataServerList( bTempServerList );
			}
			else
			{
				ms_Singleton->UpdataServerList( false );
			}
			
			s_bFirst = false;
		}
	}
}

void KUiServerList::Hide()
{
	KUiWndSingleton<KUiServerList>::Hide();
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pWindowManager ) && ( NULL != ms_Singleton->m_pThisWnd ) )
	{
		if ( ms_Singleton->m_pWindowManager )
		{
// 			ms_Singleton->m_pWindowManager->destroyWindow( ms_Singleton->m_pThisWnd );
// 			KUiServerList::DestroyWindow();
		}
	}
}

void KUiServerList::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->setRenderMode(false, 3);		
	}
}

void KUiServerList::ServerSelected( 
	ServerInfo& serverInfo,
	KServerPanel* sender, 
	bool acceptAtOnce)
{
	if ( NULL != ms_Singleton )
	{
		ms_Singleton->m_serverInfo.Name = ms_Singleton->getRealServerName( serverInfo.Name );

		if ( serverInfo.IP.empty() )
		{
			map< string, ServerInfo >::const_iterator it = ms_Singleton->m_id_info_map.find( ms_Singleton->m_serverInfo.Name );
			if ( it != ms_Singleton->m_id_info_map.end() )
			{
				ms_Singleton->m_serverInfo.IP = it->second.IP;
			}
			else
			{
				ms_Singleton->m_serverInfo.IP = serverInfo.IP;
			}
		}
		else
		{
			ms_Singleton->m_serverInfo.IP = serverInfo.IP;
		}
		
		ms_Singleton->m_serverInfo.State = serverInfo.State;
		ms_Singleton->m_serverInfo.Region = serverInfo.Region;

		if ( ( ms_Singleton->m_recentPanel != sender ) && ( NULL != ms_Singleton->m_recentPanel ) && ( NULL != ms_Singleton->m_selectedRecentServerBar_Hover ) )
		{
			ms_Singleton->m_recentPanel->clearSelectedServer();
			ms_Singleton->m_selectedRecentServerBar_Hover->hide();
		}

		for ( int i = 0; i < MAX_PANEL_LIMIT; i++ )
		{
			if ( NULL != ms_Singleton->m_panelList[i] )
			{
				if ( sender != ms_Singleton->m_panelList[i] )
				{
					ms_Singleton->m_panelList[i]->clearSelectedServer();
				}
			}
		}

		if ( sender == ms_Singleton->m_recentPanel )
		{
			ms_Singleton->refreshSelectedRecentServerBar( ms_Singleton->m_serverInfo );
		}

		if ( NULL != ms_Singleton->m_recentBackGround )
		{
			ms_Singleton->m_recentBackGround->hide();
		}

		if ( acceptAtOnce )
		{
			ms_Singleton->AcceptServer();
		}
	}
}

void KUiServerList::UpdataServerList( bool bTempServerList )
{
#ifndef _DEBUG
	try
#endif
	{

	g_bShowServerList = false;
	setUpdateOk(false);
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		g_ServerListWndPos = ms_Singleton->m_pThisWnd->getPosition( Absolute );

		//初始化登录列表，并填充ServerPanel
		if ( bTempServerList )
		{
			m_loginList->LoadLoginList(TempServerListFileName, bTempServerList);
		}
		else
		{
			m_loginList->LoadLoginList(ServerListFileName, bTempServerList);
		}
		
		//LoadName_RegionMap();

		//Load_ID_Name_Map必须在m_loginList->LoadLoginList()之后调用
		Load_ID_Name_Map();
		
		checkRecentFileVersion();

		//delInvalidRecentServerInfo必须在Load_ID_Name_Map()之后调用
		delInvalidRecentServerInfo();

		//初始化控件
		if ( NULL == m_mainBackGround )
		{
			m_mainBackGround = m_pThisWnd->getChild( "TaharezLook/ServerList/MainBg" );

			m_selectedRecentServerBar = m_mainBackGround->getChild( "TaharezLook/ServerList/MainBg/SelectedRecentServerBar" );
			m_selectedRecentServerBar->subscribeEvent(
				Window::EventMouseClick,
				Event::Subscriber(&KUiServerList::recentSelectedBar_MouseClick, this));
			m_selectedRecentServerBar->subscribeEvent(
				Window::EventMouseDoubleClick,
				Event::Subscriber(&KUiServerList::recentSelectedBar_MouseDoubleClick, this));

			m_selectedRecentServerBar_Hover = static_cast< StaticImage* >( m_selectedRecentServerBar->getChild( "TaharezLook/ServerList/MainBg/SelectedRecentServerBar/Hover" ) );
			m_selectedRecentServerBar_StatusImage = static_cast< StaticImage* >( m_selectedRecentServerBar->getChild( "TaharezLook/ServerList/MainBg/SelectedRecentServerBar/ServerStatusImage" ) );
			m_selectedRecentServerBar_ServerName = static_cast< StaticText* >( m_selectedRecentServerBar->getChild( "TaharezLook/ServerList/MainBg/SelectedRecentServerBar/ServerName" ) );
			m_selectedRecentServerBar_ServerIP = static_cast< StaticText* >( m_selectedRecentServerBar->getChild( "TaharezLook/ServerList/MainBg/SelectedRecentServerBar/ServerIP" ) );
			m_selectedRecentServerBar_ServerStatus = static_cast< StaticText* >( m_selectedRecentServerBar->getChild( "TaharezLook/ServerList/MainBg/SelectedRecentServerBar/ServerStatus" ) );
			
			m_selectedRecentServerBar_btnShowList = static_cast< PushButton* >( m_mainBackGround->getChild( "TaharezLook/ServerList/MainBg/btnShowRecentList" ) );
			m_selectedRecentServerBar_btnShowList->subscribeEvent(
				PushButton::EventClicked,
				Event::Subscriber(&KUiServerList::btnShowRecentList_Click, this));


			m_pThisWnd->getChild("TaharezLook/ServerList/btnEnter")->subscribeEvent(
				Window::EventMouseClick,
				Event::Subscriber(&KUiServerList::btnEnter_MouseClick, this));
			
			m_pThisWnd->subscribeEvent(
				Window::EventKeyDown,
				Event::Subscriber(&KUiServerList::thisWnd_KeyDown, this));
			
			m_pThisWnd->getChild("TaharezLook/ServerList/btnExit")->subscribeEvent(
				Window::EventMouseClick,
				Event::Subscriber(&KUiServerList::btnExit_MouseClick, this));
			
			m_pScrollbar = static_cast<TLMiniHorzScrollbar*>( m_pThisWnd->getChild( "TaharezLook/ServerList/Slider" ) );
			
			//CEGUI有问题，水平滚动条的位置不能为0，只能是一个很小的数
			m_pScrollbar->setScrollPosition(2.98e-008f);
			m_pScrollbar->setScrollPosition(2.98e-008f);
			
			m_pScrollbar->subscribeEvent(TLMiniHorzScrollbar::EventScrollPositionChanged, 
				Event::Subscriber(&KUiServerList::sbScrollBar_handleScroll, this));

			m_recentBackGround = m_mainBackGround->getChild( "TaharezLook/ServerList/MainBg/RecentBg" );

			//根据控件初始化颜色
			WEIHU_COLOUR = static_cast< StaticText* >( m_pThisWnd->getChild( "TaharezLook/ServerList/TxtWeihu" ) )->getTextColours();
			LIANGHAO_COLOUR = static_cast< StaticText* >( m_pThisWnd->getChild( "TaharezLook/ServerList/TxtLianghao" ) )->getTextColours();
			FANMANG_COLOUR = static_cast< StaticText* >( m_pThisWnd->getChild( "TaharezLook/ServerList/TxtFanmang" ) )->getTextColours();
			BAOMAN_COLOUR = static_cast< StaticText* >( m_pThisWnd->getChild( "TaharezLook/ServerList/TxtBaoman" ) )->getTextColours();

			WEIHU_IMAGE = static_cast< StaticImage* >( m_pThisWnd->getChild( "TaharezLook/ServerList/ImgWeihu" ) )->getImage();
			LIANGHAO_IMAGE = static_cast< StaticImage* >( m_pThisWnd->getChild( "TaharezLook/ServerList/ImgLianghao" ) )->getImage();
			FANMANG_IMAGE = static_cast< StaticImage* >( m_pThisWnd->getChild( "TaharezLook/ServerList/ImgFanmang" ) )->getImage();
			BAOMAN_IMAGE = static_cast< StaticImage* >( m_pThisWnd->getChild( "TaharezLook/ServerList/ImgBaoman" ) )->getImage();
			
			LoadTemplateRect();
		}

		int curPanelNum = 0;
	
		int totalWidth = m_mainBackGround->getAbsoluteWidth();
		
		int curRightEdge_XPosition = m_templateRect.getPosition().d_x;

		for ( int k = 0; k < m_loginList->getNets().size(); k++ )
		{
			NetInfo& curNet = m_loginList->getNets()[k];
			for ( int m = 0; m < curNet.Regions.size(); m++ )
			{
				string RegionBackgroundName; 
				RegionBackgroundName = RegionBackgroundName + "TaharezLook/ServerList/MainBg/Bg" + "[" + curNet.Regions[m].SectionName +  "]";
				const string RecommendServerSectionName = KMessageCentre::GetMessage(serverlist_message, 6);
				
				//取得服务器列表的底板
				Window* curParentBg = NULL;
				if ( m_mainBackGround->isChild( AnsiToUtf8( RegionBackgroundName.c_str() ) ) )
				{
					curParentBg = m_mainBackGround->getChild( AnsiToUtf8( RegionBackgroundName.c_str() ) );
				}
				else
				{
					//curParentBg = NULL;
					curParentBg = WindowManager::getSingleton().createWindow( "TaharezLook/StaticImage", AnsiToUtf8( RegionBackgroundName.c_str() ) );
					m_mainBackGround->addChildWindow( curParentBg );
					SetTemplateBackground( curParentBg );
				}
				
				if ( ( NULL != curParentBg ) && ( curPanelNum < MAX_PANEL_LIMIT ) )
				{
					if ( RecommendServerSectionName == curNet.Regions[m].SectionName )
					{
						//SyncRegionName(curNet.Regions[m]);
						//推荐服务器和最近登录服务器都在最左侧
// 						curParentBg->setXPosition( Absolute, m_recentBackGround->getXPosition(Absolute) );
// 						int recommend_YPosition = m_recentBackGround->getYPosition(Absolute) + m_recentBackGround->getAbsoluteHeight();
// 						curParentBg->setYPosition( Absolute, recommend_YPosition );
// 						curParentBg->setHeight( Absolute, m_recentBackGround->getAbsoluteHeight() );
						//curLeft += curParentBg->getAbsoluteWidth();

						m_recommendServerIndex = curPanelNum;
						if ( NULL == m_panelList[curPanelNum] )
						{
							m_panelList[curPanelNum] = new KServerPanel();
						}
						
						if ( NULL != m_panelList[curPanelNum] )
						{
							m_panelList[curPanelNum]->SetServerSelected_CallBack(ServerSelected);
							m_panelList[curPanelNum]->LoadServerList(
								&(curNet.Regions[m]),
								m_pWindowManager,
								curParentBg );	
						}
					}
					else
					{
						curParentBg->setXPosition( Absolute, curRightEdge_XPosition );
						curRightEdge_XPosition += curParentBg->getAbsoluteWidth();

						if ( NULL == m_panelList[curPanelNum] )
						{
							m_panelList[curPanelNum] = new KServerPanel();
						}

						if ( NULL != m_panelList[curPanelNum] )
						{
							m_panelList[curPanelNum]->SetServerSelected_CallBack(ServerSelected);
							m_panelList[curPanelNum]->LoadServerList(
								&(curNet.Regions[m]),
								m_pWindowManager,
								curParentBg );
						}
					}
					
					m_backGroundList[curPanelNum] = curParentBg;
					curPanelNum++;
				}
			}
		}
		
		m_exceedWidth = curRightEdge_XPosition - totalWidth;
		if ( NULL != m_pScrollbar )
		{
			m_pScrollbar->setVisible( m_exceedWidth > 0 );
		}
		
		m_recentList->LoadLoginList( RecentServerFileName, bTempServerList, false );
		
		m_recentPanel->SetServerSelected_CallBack(ServerSelected);
		
		vector<NetInfo>& netInfo = m_recentList->getNets();
		
		if ( m_recentList->getNetCount() > 0  && !netInfo.empty() && netInfo[0].Regions.size() > 0 )
		{
			//SyncRegionName(netInfo[0].Regions[0]);

			m_recentServerCount = netInfo[0].Regions[0].Servers.size();
			
			if ( m_recentServerCount > 0 )
			{
				m_recentPanel->LoadServerList(
					&(netInfo[0].Regions[0]),
					m_pWindowManager,
					m_recentBackGround,
					false,
					true );
				
				m_recentPanel->SetTitleVisible( false );
			}
		}

		if ( NULL != m_recentBackGround )
		{
			m_recentBackGround->setVisible( m_recentServerCount > 0 );
		}

		m_recentPanel->SelectDefaultServer();
	}

	}
#ifndef _DEBUG
	catch (...)
	{
		Hide();
		return;
	}
#endif
}

bool KUiServerList::btnEnter_MouseClick( const CEGUI::EventArgs& e )
{
	const MouseEventArgs& args = static_cast<const MouseEventArgs&>( e );
	if ( LeftButton == args.button )
	{
		AcceptServer();
		return true;
	}
	else
	{
		return false;
	}
}

bool KUiServerList::btnExit_MouseClick( const CEGUI::EventArgs& e )
{
	const MouseEventArgs& args = static_cast<const MouseEventArgs&>( e );
	if ( LeftButton == args.button )
	{
		Hide();
		g_LoginLogic.ReturnToIdleStatus();
		if ( g_GetMainApp() )
		{
			g_GetMainApp()->StopApplication();
		}
		return true;
	}
	else
	{
		return false;
	}
}

void KUiServerList::syncServerStatus( BarList& source, BarList& dest )
{
	map<string, string>	statusMap;
	for ( int i = 0 ; i < source.size(); i++ )
	{
		string serverIP = WStringToAString(source[i]->getChild(source[i]->getName() + "/ServerIP")->getText());
		string serverStatus = WStringToAString(source[i]->getChild(source[i]->getName() + "/ServerStatus")->getText());
		statusMap[serverIP] = serverStatus;
	}

	for ( int j = 0; j < dest.size(); j++ )
	{
		string destServerIP = WStringToAString(dest[j]->getChild(dest[j]->getName() + "/ServerIP")->getText());
		dest[j]->getChild(dest[j]->getName() + "/ServerStatus")->setText(AnsiToUtf8(statusMap[destServerIP].c_str()));
	}
}

void KUiServerList::SaveRecentServer()
{
	checkRecentFileVersion();
	KIniFile recentIni;
	bool serverIPSaved = false;
	int	 savePos = 0;
	recentIni.Load( RecentServerFileName.c_str());

	recentIni.WriteInteger( "[List]", "Version", KUiCfgLoader::getSingleton().getServerListCfg().RecentFileVersion );
	recentIni.WriteInteger("[List]", "NetCount", 1);
	recentIni.WriteString("[Net_0]", "NetName", "recentShow");
	recentIni.WriteInteger("[Net_0]", "RegionNum", 1);
	
	int iniServerCount = 0;
	recentIni.GetInteger( SectionName_RecentServerRegion.c_str(), "ServerNum", 0, &iniServerCount );
	const int serverCount = iniServerCount;
	
	int i = 0;
	for ( i = 0; i < serverCount; i++ )
	{
		//获取服务器IP
		char serverIP[COMMON_CLIENT_MSG_LEN_32] = { 0 };
		recentIni.GetString("[Net_0_Region_0]", (iToStr(i) + "_Server_IP").c_str(), "", serverIP, COMMON_CLIENT_MSG_LEN_32);
		string strServerIP = serverIP;

		char serverName[COMMON_CLIENT_MSG_LEN_64] = { 0 };
		recentIni.GetString("[Net_0_Region_0]", (iToStr(i) + "_Server_Name").c_str(), "", serverName, COMMON_CLIENT_MSG_LEN_64);
		string strServerName = serverName;

		//if ( ( strcmp(serverIP, m_serverInfo.IP.c_str()) == 0 ) || ( strcmp(serverName, m_serverInfo.Name.c_str()) == 0 ) )
		if ( ( strServerIP == m_serverInfo.IP ) || ( strServerName == m_serverInfo.Name) )
		{
			serverIPSaved = true;
			savePos = i;
			break;
		}
	}

	recentIni.WriteString( 
		SectionName_RecentServerRegion.c_str() , 
		"RegionName", 
		KMessageCentre::GetMessage(serverlist_message, 5) );
	if ( !serverIPSaved )
	{
		const int maxServerCount = KUiCfgLoader::getSingleton().getServerListCfg().MaxRecentServerCount;
		if ( maxServerCount <= serverCount )
		{
			//如果最近服务器列表已满，则清除最后一条服务器记录（注意，RecentServer.ini显示的时候是倒序的）
			for ( i = 0; i < serverCount - 1; i++ )
			{
				ServerInfo tempInfo;
				KLoginList::LoadServerInfo( recentIni, SectionName_RecentServerRegion, i + 1, tempInfo );
				KLoginList::WriteServerInfo( recentIni, SectionName_RecentServerRegion, i, tempInfo );
			}
			ServerInfo tmpServerInfo = m_serverInfo;
			tmpServerInfo.IP = "";
			KLoginList::WriteServerInfo( recentIni, SectionName_RecentServerRegion, serverCount - 1,  tmpServerInfo );
		}
		else
		{
			//最近服务器未满，则将当前服务器列为第一条最近服务器（注意，RecentServer.ini显示的时候是倒序的）
			recentIni.WriteInteger( SectionName_RecentServerRegion.c_str(), "ServerNum", serverCount + 1 );
			ServerInfo tmpServerInfo = m_serverInfo;
			tmpServerInfo.IP = "";
			KLoginList::WriteServerInfo( recentIni, SectionName_RecentServerRegion, serverCount, tmpServerInfo );
		}
	}
	else
	{
		//如果该IP已经存在，那么将该IP的信息和最后一条记录的信息进行交换
		if ( serverCount - 1 != savePos )
		{
			ServerInfo tempSI;
			KLoginList::LoadServerInfo( recentIni, SectionName_RecentServerRegion, serverCount - 1, tempSI );
			ServerInfo tmpServerInfo = m_serverInfo;
			tmpServerInfo.IP = "";
			KLoginList::WriteServerInfo( recentIni, SectionName_RecentServerRegion, serverCount - 1, tmpServerInfo );
			KLoginList::WriteServerInfo( recentIni, SectionName_RecentServerRegion, savePos, tempSI );
		}
	}
	recentIni.Save(RecentServerFileName.c_str());
}

bool KUiServerList::thisWnd_KeyDown( const CEGUI::EventArgs& e )
{
	switch (static_cast<const KeyEventArgs&>(e).scancode)
    {
	case Key::Return:
		{	
			AcceptServer();
		}
		break;
	default:
		return true;
	}
	return true;
}

void KUiServerList::AcceptServer()
{
	bool bServerExists = false;
	if ( m_serverInfo.IP.empty() )
	{
		bServerExists = m_recentPanel->SelectDefaultServer();
	}
	else
	{
		bServerExists = true;
	}
	
	if ( bServerExists )
	{	
		SaveRecentServer();
		
		int nBegin = m_serverInfo.IP.find_first_of( ":" );
		int nEnd = m_serverInfo.IP.length() - 1;
		
		string ip =  m_serverInfo.IP.substr( 0, nBegin );
		string port =  m_serverInfo.IP.substr( nBegin + 1, nEnd- nBegin );
		
		DWORD dwPort = atoi( port.c_str() );
		
		g_LoginLogic.SetGameServerIP( ip.c_str(), dwPort );
		
		string serverNameShow = m_serverInfo.Name;
		
		KUiLoginBackGround::GetSingleton().SetServerInfo( AnsiToUtf8( serverNameShow.c_str() ) );
		KUiInfoBarPing::GetSingleton().SetServerName( serverNameShow );
		Hide();
		KUiLogin::Show();
	}
}

// void KUiServerList::LoadName_RegionMap()
// {
// 	for ( int k = 0; k < m_loginList->getNets().size(); k++ )
// 	{
// 		NetInfo& curNet = m_loginList->getNets()[k];
// 		for ( int m = 0; m < curNet.Regions.size(); m++ )
// 		{
// 			RegionInfo& curRegion = curNet.Regions[m];
// 			const string RecommendServerSectionName = KMessageCentre::GetMessage(serverlist_message, 6);
// 			if ( curRegion.SectionName != RecommendServerSectionName )   //给推荐服务器做特例
// 			{
// 				for ( int i = 0; i < curRegion.Servers.size(); i++)
// 				{
// 					ServerInfo& curServer = curRegion.Servers[i];
// 					m_name_regionMap[curServer.Name] = curServer.Region;
// 				}
// 			}
// 		}
// 	}
// }

// void KUiServerList::SyncRegionName( RegionInfo& source )
// {
// 	for ( int i = 0; i < source.Servers.size(); i++ )
// 	{
// 		ServerInfo& curServer = source.Servers[i];
// 		map<string, string>::iterator it = m_name_regionMap.find(curServer.Name);
// 		if ( m_name_regionMap.end() != it)
// 		{
// 			curServer.Region = m_name_regionMap[curServer.Name];
// 		}
// 	}
// }

void KUiServerList::SetTemplateBackground( Window* wnd )
{
	if ( NULL != wnd )
	{	
		LoadTemplateRect();
		wnd->setSize( Absolute, m_templateRect.getSize() );
		wnd->setYPosition( Absolute, m_templateRect.getPosition().d_y );
	}
}

bool KUiServerList::sbScrollBar_handleScroll( const CEGUI::EventArgs& e )
{
	if ( ( m_exceedWidth > 0 ) && ( NULL != m_pScrollbar ) )
	{
		Point newPos;
		float scrollPos = m_pScrollbar->getScrollPosition();
		int scrollSize = - scrollPos * m_exceedWidth;
		
		if ( NULL != m_recentBackGround )
		{
			//对最左边的“最近登录服务器列表”和 “推荐服务器列列表”做特例
			newPos.d_x = scrollSize;
			newPos.d_y = m_recentBackGround->getAbsoluteYPosition();
			m_recentBackGround->setPosition(Absolute, newPos);
		}

		if ( ( m_recommendServerIndex >= 0 ) && ( NULL != m_backGroundList[m_recommendServerIndex] ) )
		{
			newPos.d_x = scrollSize;
			newPos.d_y = m_backGroundList[m_recommendServerIndex]->getAbsoluteYPosition();
			m_backGroundList[m_recommendServerIndex]->setPosition(Absolute, newPos);			
		}
		
		//其他常规服务器列表
		int i = 0;
		for ( i = 0; i < MAX_PANEL_LIMIT; i++ )
		{
			if ( i != m_recommendServerIndex )	//跳过推荐服务器
			{
				int posIndex = i + 1 - ( i > m_recommendServerIndex && m_recommendServerIndex >= 0 );
				if ( NULL != m_backGroundList[i] )
				{
					newPos.d_x = m_backGroundList[i]->getAbsoluteWidth() * posIndex + scrollSize;
					newPos.d_y = m_backGroundList[i]->getAbsoluteYPosition();
					m_backGroundList[i]->setPosition(Absolute, newPos);
				}
			}
		}
	}

	return true;	
}

//返回值表示文件版本是否是最新
bool KUiServerList::checkRecentFileVersion()
{
	if ( IsFileEncrypted( RecentServerFileName ) )
	{
		DeleteTmpFile( RecentServerFileName );
		return false;
	}

	KIniFile recentIni;
	if ( recentIni.Load( RecentServerFileName.c_str() ) )
	{
		int curVersion = KUiCfgLoader::getSingleton().getServerListCfg().RecentFileVersion;
		int fileVersion = 0;
		recentIni.GetInteger( "List", "Version", -1, &fileVersion );

		if ( ( fileVersion != -1 ) && ( fileVersion == curVersion ) )
		{
			return true;
		}
		else
		{
			DeleteTmpFile( RecentServerFileName );
			return false;
		}
	}
	else
	{
		DeleteTmpFile( RecentServerFileName );
		return false;
	}
}

void KUiServerList::Load_ID_Name_Map()
{
	const string RecommendServerSectionName = KMessageCentre::GetMessage( serverlist_message, 6 );
	m_id_info_map.clear();

	vector<NetInfo>& nets = m_loginList->getNets();
	for ( int k = 0; k < nets.size(); k++ )
	{
		NetInfo& curNet = nets[k];
		for ( int m = 0; m < curNet.Regions.size(); m++ )
		{
			RegionInfo& curRegion = curNet.Regions[m];		
			if ( curRegion.SectionName != RecommendServerSectionName )   //给推荐服务器做特例
			{
				for ( int i = 0; i < curRegion.Servers.size(); i++)
				{
					ServerInfo& curServer = curRegion.Servers[i];
					m_id_info_map[curServer.Name] = curServer;
				}
			}
		}
	}
}

string KUiServerList::getRealServerName( string& oriServerName )
{
	int tokenPos = oriServerName.find( "-" );
	if ( string::npos != tokenPos )
	{
		return oriServerName.substr( tokenPos + 1, oriServerName.length() - tokenPos - 1 );
	}
	else
	{
		return oriServerName;
	}
}

bool KUiServerList::delInvalidRecentServerInfo()
{
#ifndef _DEBUG
	try
#endif
	{
		KIniFile recentIni;
		if ( recentIni.Load( RecentServerFileName.c_str() ) )
		{
			int iniServerCount = 0;
			recentIni.GetInteger( SectionName_RecentServerRegion.c_str(), "ServerNum", 0, &iniServerCount );
			const int serverCount = iniServerCount;
			vector< ServerInfo > tempList;
			
			for ( int i = 0; i < serverCount; ++i )
			{
				ServerInfo tempInfo;	
				KLoginList::LoadServerInfo( recentIni, SectionName_RecentServerRegion, i, tempInfo );
				if ( m_id_info_map.find( tempInfo.Name ) != m_id_info_map.end() )
				{
					tempList.push_back( tempInfo );
				}
			}

			int serverDeleted = serverCount - tempList.size();
			
			if ( serverDeleted > 0 )
			{
				recentIni.EraseSection( SectionName_RecentServerRegion.c_str() );

				if ( serverCount - serverDeleted > 0 )
				{
					recentIni.WriteString( 
						SectionName_RecentServerRegion.c_str() , 
						"RegionName", 
						KMessageCentre::GetMessage( serverlist_message, 5 ) );
					
					recentIni.WriteInteger( SectionName_RecentServerRegion.c_str(), "ServerNum", serverCount - serverDeleted );
					
					for ( int j = 0; j < tempList.size(); ++j )
					{
						KLoginList::WriteServerInfo( recentIni, SectionName_RecentServerRegion, j, tempList[j] );
					}
					recentIni.Save( RecentServerFileName.c_str() );
				}
				else
				{
					recentIni.Clear();
					DeleteTmpFile( RecentServerFileName );
				}
			}
			return true;
		}
		else
		{
			return false;
		}	
	}
#ifndef _DEBUG
	catch (...)
	{
		return false;
	}
#endif
}

bool KUiServerList::btnShowRecentList_Click( const CEGUI::EventArgs& e )
{
	if ( ( m_recentServerCount > 0 ) && ( NULL != m_recentBackGround ) )
	{
		m_recentBackGround->setVisible( ! m_recentBackGround->isVisible() );
		return true;
	}
	else
	{
		return false;
	}
}

void KUiServerList::refreshSelectedRecentServerBar( const ServerInfo& si )
{
	bool isValid = ( NULL != m_selectedRecentServerBar ) 
		&& ( NULL != m_selectedRecentServerBar_ServerName ) 
		&& ( NULL != m_selectedRecentServerBar_Hover )
		&& ( NULL != m_selectedRecentServerBar_ServerStatus )
		&& ( NULL != m_selectedRecentServerBar_StatusImage )
		&& ( NULL != m_selectedRecentServerBar_ServerStatus );

	if ( isValid )
	{
		m_selectedRecentServerBar_ServerName->setText( AnsiToUtf8( si.Name.c_str() ) );
		m_selectedRecentServerBar_Hover->show();
		m_selectedRecentServerBar_ServerStatus->setText( AnsiToUtf8( si.State.c_str() ) );
		KServerPanel::setStatusImage( si.State.c_str(), *m_selectedRecentServerBar_StatusImage, *m_selectedRecentServerBar_ServerStatus );

		m_currentRecentServerBar = m_recentPanel->getCurrentSelectedBar();
	}
}

bool KUiServerList::recentSelectedBar_MouseClick( const CEGUI::EventArgs& e )
{
	const MouseEventArgs& args = static_cast<const MouseEventArgs&>( e );
	if ( LeftButton == args.button )
	{
		if ( ( NULL != m_recentPanel ) && ( NULL != m_selectedRecentServerBar_Hover ) )
		{
			bool hasSelected = m_selectedRecentServerBar_Hover->isVisible();
			if ( !hasSelected )
			{
				m_recentPanel->SetActiveBar( m_currentRecentServerBar );
			}
			//btnShowRecentList_Click( e );
		}
		return true;	
	}
	else
	{
		return false;
	}
}

bool KUiServerList::recentSelectedBar_MouseDoubleClick( const CEGUI::EventArgs& e )
{
	const MouseEventArgs& args = static_cast<const MouseEventArgs&>( e );
	if ( LeftButton == args.button )
	{
		if ( NULL != m_recentPanel )
		{
			m_recentPanel->SetActiveBar( m_currentRecentServerBar, true );
		}
		return true;
	}
	else
	{
		return false;
	}
}

void KUiServerList::LoadTemplateRect()
{
	if ( NULL == m_templateBackGround )
	{
		if ( ( NULL != m_mainBackGround ) && m_mainBackGround->isChild( "TaharezLook/ServerList/MainBg/Bg[Net_0_Region_0]" ) )
		{
			m_templateBackGround = m_mainBackGround->getChild( "TaharezLook/ServerList/MainBg/Bg[Net_0_Region_0]" );
			m_templateRect.setPosition( m_templateBackGround->getPosition( Absolute ) );
			m_templateRect.setSize( m_templateBackGround->getSize( Absolute ) );
		}
		else
		{
			m_templateRect.setPosition( Point( 222.0f, 0.0f ) );
			m_templateRect.setSize( Size( 228.0f, 485.0f ) );
		}
	}		
}

/********************************************************************
/*						class: KServerPanel
*********************************************************************/
KServerPanel::KServerPanel()
: m_pPanel(NULL)
, m_pCurrentSelectedBar(NULL)
//, m_pWindowManager(NULL)
, m_pScrollbar(NULL)
, m_pList(NULL)
, m_regionInfo(NULL)
{
	ServerSelected_CallBack = NULL;
	m_barHeight = -1;
	m_leastBarCount = 0;
	m_pageHeight = 0;
	m_totalHeight = 0;
	m_exceedHeight = 0;
	m_useRegionPrefix = false;
	m_reverseList = false;
}

KServerPanel::~KServerPanel()
{
}

void KServerPanel::LoadServerList( 
	RegionInfo* region,
	WindowManager* pWindowManager, 
	Window* parentWindow,  
	bool useRegionPrefix /*= false*/, 
	bool reverseList /*=false*/ )
{	
	innerLoadServerList(region, pWindowManager, parentWindow, useRegionPrefix, reverseList);
}

void KServerPanel::innerLoadServerList( 
	RegionInfo* region,
	CEGUI::WindowManager* pWindowManager, 
	Window* parentWindow, 
	bool useRegionPrefix /*= false*/,
	bool reverseList /*= false*/)
{
	if ( NULL != region && NULL != pWindowManager && NULL != parentWindow )
	{
		//m_barList.clear();
		m_bar_serverinfo_map.clear();
		m_pWindowManager = pWindowManager;
		m_useRegionPrefix = useRegionPrefix;
		m_reverseList = reverseList;
		m_regionInfo = region;
		
		ostringstream panelName;
		panelName<<parentWindow->getName()<<"/"<<m_regionInfo->Name;
		
		if ( NULL == m_pPanel )
		{
			m_pPanel = pWindowManager->loadWindowLayout(PanelLayoutsFileName, panelName.str(), "", NULL, NULL, true);

			if ( NULL != m_pPanel )
			{
				m_pList = m_pPanel->getChild(m_pPanel->getName() + "/List");
				m_pScrollbar = static_cast<TLVertScrollbar*>(m_pPanel->getChild(m_pPanel->getName() + "/Scrollbar"));
				m_pScrollbar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
					Event::Subscriber(&KServerPanel::sbScrollBar_handleScroll, this));

				//设置Panel的大小和父窗口一样
				m_pPanel->setSize(Absolute, parentWindow->getSize(Absolute));
				//设置List的高度
				//int titleHeight = m_pPanel->getChild(m_pPanel->getName() + "/RegionName")->getHeight(Absolute);
				int listHeight = m_pPanel->getHeight(Absolute) - m_pList->getAbsoluteYPosition();
				m_pList->setHeight(Absolute, listHeight);
				//设置页面的高度和List的高度一样，为后面的滚动做准备
				m_pageHeight = listHeight;
				
				//设置滚动条的高度和List的高度一样
				m_pScrollbar->setHeight(Absolute, listHeight);
				
				parentWindow->addChildWindow(m_pPanel);
			}
		}
		
		if ( NULL != m_pPanel )
		{
			m_pPanel->getChild(m_pPanel->getName() + "/RegionName")->setText(AnsiToUtf8(m_regionInfo->Name.c_str()));
			m_pScrollbar->setScrollPosition( 0.0f );
			m_pPanel->show();
			
			InitBars();			
		}
	}
}

bool KServerPanel::sbScrollBar_handleScroll( const CEGUI::EventArgs& e )
{
	if ( m_exceedHeight > 0 )
	{
		float scrollPos = m_pScrollbar->getScrollPosition();
		int scrollSize = - scrollPos * m_exceedHeight;
		
		for ( int i = 0; i < m_barList.size(); i++ )
		{
			Point newPos;
			newPos.d_x = m_barList[i]->getAbsoluteXPosition();
			newPos.d_y = m_barList[i]->getAbsoluteHeight() * i + scrollSize;
			m_barList[i]->setPosition(Absolute, newPos);
		}
	}
	return true;
}

bool KServerPanel::Panel_MouseWheel( const CEGUI::EventArgs& e )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&e;
	
	if(m_pScrollbar->isVisible())
	{
		float newPos = m_pScrollbar->getScrollPosition() - m_pScrollbar->getStepSize() * eventArgs->wheelChange;
		m_pScrollbar->setScrollPosition(newPos);
	}
	return true;		
}

void KServerPanel::InitBars()
{
	if ( m_pWindowManager == NULL )
	{
		return;
	}
	ostringstream curBarName;

	if ( NULL != m_pWindowManager )
	{
		Window* pTempBar= m_pWindowManager->loadWindowLayout(BarLayoutsFileName, m_pPanel->getName() + "tempBar", "", NULL, NULL, true);
		m_barHeight = pTempBar->getAbsoluteHeight();
		m_pWindowManager->destroyWindow(pTempBar);
	}

	if ( m_barHeight <= 0 )
	{
		return;
	}
	
	m_leastBarCount = m_pageHeight / m_barHeight;

	m_totalHeight = m_barHeight * m_regionInfo->Servers.size();
	m_exceedHeight = m_totalHeight - m_pageHeight; 

	if ( m_exceedHeight > 0 )
	{
		float stepSize = static_cast< float >( m_barHeight  ) / static_cast< float >( m_exceedHeight );
		m_pScrollbar->setStepSize( ( ( stepSize > 0.0 ) && ( stepSize < 1.0 ) ) ? stepSize : 1.0 );
	}

	m_pScrollbar->setVisible( m_exceedHeight > 0);

	int showServerCount = (m_regionInfo->Servers.size() > m_leastBarCount) ? m_regionInfo->Servers.size() : m_leastBarCount;
	for ( int i = 0; i < showServerCount/*m_serverCount*/; i++ )
	{
		curBarName.str("");
		curBarName<<m_pPanel->getName()<<'_'<<i<<'_';
		
		if ( m_barNameSet.count( curBarName.str() ) == 0 )
		{
			Window* pCurrentBar= m_pWindowManager->loadWindowLayout(BarLayoutsFileName, curBarName.str(), "", NULL, NULL, true);

			if ( NULL != pCurrentBar )
			{
				//SetBar(i, pCurrentBar);
				m_barList.push_back( pCurrentBar );
				m_barNameSet.insert( curBarName.str() );

				TLStaticText* pBarServerName = static_cast< TLStaticText* >( pCurrentBar->getChild(pCurrentBar->getName() + "/ServerName") );
				pBarServerName->useLayout();
				
				//设置裁剪区域
				if ( NULL != m_pList )
				{
					LORect pannelclipper;
					Rect textArea = m_pList->getUnclippedPixelRect();
					//Point itemPos(textArea.d_left, textArea.d_top);
					Point clipperPos;
					clipperPos.d_x = textArea.d_left - g_ServerListWndPos.d_x;
					clipperPos.d_y = textArea.d_top - g_ServerListWndPos.d_y;
					textArea.setPosition(clipperPos);
					cerectToLorect(&textArea, &pannelclipper);
					pBarServerName->getLayout()->setClipper(pannelclipper);
				}
				
				pCurrentBar->subscribeEvent(
					StaticImage::EventMouseClick, 
					Event::Subscriber(&KServerPanel::btnBar_MouseClick, this));
				
				pCurrentBar->subscribeEvent(
					StaticImage::EventMouseDoubleClick, 
					Event::Subscriber(&KServerPanel::btnBar_MouseDoubleClick, this));
				
				pCurrentBar->subscribeEvent(
					TLVertScrollbar::EventMouseWheel, 
					Event::Subscriber(&KServerPanel::Panel_MouseWheel, this));

				m_pList->addChildWindow( pCurrentBar );
			}
		}
	}

	RefreshBars();
}

void KServerPanel::SetBar( int i, Window* pCurrentBar )
{
	if ( NULL == pCurrentBar )
	{
		return;
	}

	Point curBarPosition;

	pCurrentBar->show();

	int serverIndex = 0;
	if ( m_reverseList )
	{
		serverIndex = m_regionInfo->Servers.size() - 1 - i;
	}
	else
	{
		serverIndex = i;
	}
	
	if ( serverIndex < m_regionInfo->Servers.size() && serverIndex >= 0 )
	{
		string fullServerNameShow;

		ServerInfo& curServer = m_regionInfo->Servers[serverIndex];
		if ( m_useRegionPrefix )
		{
			fullServerNameShow = fullServerNameShow + "(" + curServer.Region + ")" + curServer.Name;
		}
		else
		{
			char sz_fullName[COMMON_CLIENT_MSG_LEN_1024] = { 0 };
			if ( s_serverNameFormatString.empty() )
			{
//				fullServerNameShow = curServer.Name;	
				_snprintf( 
					sz_fullName, 
					sizeof( sz_fullName ), 
					DEFAULT_SERVERNAME_FORMAT_STRING.c_str(), 
					curServer.Name.c_str() );
				
				sz_fullName[COMMON_CLIENT_MSG_LEN_1024 - 1] = 0;
				
				fullServerNameShow = sz_fullName;
			}
			else
			{
				_snprintf( 
					sz_fullName, 
					sizeof( sz_fullName ), 
					s_serverNameFormatString.c_str(), 
					curServer.Prefix.c_str(),
					curServer.Prefix2.c_str(),
					curServer.Name.c_str(),
					curServer.Postfix.c_str(),
					curServer.Postfix2.c_str() );
				
				sz_fullName[COMMON_CLIENT_MSG_LEN_1024 - 1] = 0;
				
				fullServerNameShow = sz_fullName;
			}
		}

		m_bar_serverinfo_map.insert( BarServerInfoMap::value_type( pCurrentBar, curServer ) ); // ) [pCurrentBar] = curServer;
		//pCurrentBar->getChild(pCurrentBar->getName() + "/ServerName")->setText( AnsiToUtf8( fullServerNameShow.c_str() ) );
		TLStaticText* pBarServerName = static_cast< TLStaticText* >( pCurrentBar->getChild(pCurrentBar->getName() + "/ServerName") );
		char* layoutTxt = const_cast< char * >( fullServerNameShow.c_str() );
		pBarServerName->setText( "" );
		pBarServerName->getLayout()->formatText( layoutTxt );
		pBarServerName->getLayout()->SetText( layoutTxt );
		pBarServerName->getLayout()->flashLayout();
		
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerIP")->hide();
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerIP")->setText( AnsiToUtf8( curServer.IP.c_str() ) );
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerStatus")->setText( AnsiToUtf8( curServer.State.c_str() ) );	
		StaticImage* serverStatusImage = static_cast<StaticImage*>(pCurrentBar->getChild(pCurrentBar->getName() + "/ServerStatusImage"));
		StaticText*  serverStatusTxt = static_cast<StaticText*>(pCurrentBar->getChild(pCurrentBar->getName() + "/ServerStatus"));
		setStatusImage( curServer.State.c_str(), *serverStatusImage, *serverStatusTxt);
	}
	else
	{
		TLStaticText* pBarServerName = static_cast< TLStaticText* >( pCurrentBar->getChild(pCurrentBar->getName() + "/ServerName") );
 		if ( NULL != pBarServerName->getLayout() )
 		{
			pBarServerName->getLayout()->clearLayout();
		}
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerName")->setText( AnsiToUtf8("") );
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerIP")->hide();
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerIP")->setText( AnsiToUtf8("") );
		pCurrentBar->getChild(pCurrentBar->getName() + "/ServerStatus")->setText( AnsiToUtf8("") );
		StaticImage* serverStatusImage = static_cast<StaticImage*>(pCurrentBar->getChild(pCurrentBar->getName() + "/ServerStatusImage"));
		StaticText*  serverStatusTxt = static_cast<StaticText*>(pCurrentBar->getChild(pCurrentBar->getName() + "/ServerStatus"));
		setStatusImage( "", *serverStatusImage, *serverStatusTxt);
	}

// 	if ( -1 == m_barHeight )
// 	{
// 		//由于使用模板，所以所有的bar刚load的时候是一样高的
// 		m_barHeight = pCurrentBar->getSize(Absolute).d_height;
// 		
// 		m_totalHeight = m_barHeight * m_serverCount;
// 		m_exceedHeight = m_totalHeight - m_pageHeight; 
// 	}
	curBarPosition.d_x = 0;
	curBarPosition.d_y = i * m_barHeight;

	pCurrentBar->setPosition(Absolute, curBarPosition);
		
	//m_pList->addChildWindow(pCurrentBar);
	//m_barList.push_back(pCurrentBar);
}

void KServerPanel::SetServerSelected_CallBack( 
	void (*func)( 
		ServerInfo& serverInfo,
		KServerPanel* sender,  
		bool acceptAtOnce) )
{
	ServerSelected_CallBack = func;	
}

void KServerPanel::clearSelectedServer()
{
	if ( NULL != m_pCurrentSelectedBar )
	{
		String hoverName = m_pCurrentSelectedBar->getName() + "/Hover";
		m_pCurrentSelectedBar->getChild(hoverName)->setVisible(false);			
		m_pCurrentSelectedBar->getChild(hoverName)->setEnabled(true);
		m_pCurrentSelectedBar = NULL;
	}
}

// void KServerPanel::RefreshStatus()
// {
// 	for ( int i = 0; i < m_barList.size(); i++ )
// 	{
// 		char* statusTxt = Utf8ToAnsi(m_barList[i]->getChild(m_barList[i]->getName() + "/ServerStatus")->getText());
// 		StaticImage* statusImage= static_cast<StaticImage*>(m_barList[i]->getChild(m_barList[i]->getName() + "/ServerStatusImage"));
// 		StaticText*  serverStatusTxt = static_cast<StaticText*>(m_barList[i]->getChild(m_barList[i]->getName() + "/ServerStatus"));
// 		setStatusImage(statusTxt, *statusImage, *serverStatusTxt);
// 	}
// }

void KServerPanel::setStatusImage( const char* statusTxt, StaticImage& statusImg, StaticText& status )
{
	const char* serverStatus = statusTxt;

	//显示服务器状态图片
	if ( strcmp(serverStatus, KMessageCentre::GetMessage(serverlist_message, 0)) == 0 )
	{
		statusImg.setImage( WEIHU_IMAGE );
		status.setTextColours( WEIHU_COLOUR );
	}
	else if ( strcmp(serverStatus, KMessageCentre::GetMessage(serverlist_message, 1)) == 0)
	{
		statusImg.setImage( LIANGHAO_IMAGE );
		status.setTextColours( LIANGHAO_COLOUR );
	}
	else if ( strcmp(serverStatus, KMessageCentre::GetMessage(serverlist_message, 2)) == 0)
	{
		statusImg.setImage( FANMANG_IMAGE );
		status.setTextColours( FANMANG_COLOUR );
	}
	else if ( strcmp(serverStatus, KMessageCentre::GetMessage(serverlist_message, 3)) == 0)
	{
		statusImg.setImage( BAOMAN_IMAGE );
		status.setTextColours( BAOMAN_COLOUR );
	}
	else if ( strcmp(serverStatus, KMessageCentre::GetMessage(serverlist_message, 4)) == 0)
	{
		statusImg.setImage( FANMANG_IMAGE );
		status.setTextColours( FANMANG_COLOUR );
	}
	else
	{
		statusImg.setImage(NULL);
	}		
}

bool KServerPanel::SelectDefaultServer()
{
	if ( m_barList.size() > 0 )
	{
		Window* activeBar = m_barList[0];
		SetActiveBar( activeBar );
		return true;
	}
	else
	{
		return false;
	}
}

bool KServerPanel::btnBar_MouseDoubleClick( const CEGUI::EventArgs& e )
{
	const MouseEventArgs& args = static_cast<const MouseEventArgs&>( e );
	if ( LeftButton == args.button )
	{
		Window* activeBar = args.window;
		if ( m_bar_serverinfo_map.count( activeBar ) )//activeBar->getChild(activeBar->getName() + "/ServerName")->getText().length() > 0 )
		{
			SetActiveBar( activeBar, true );
			return true;
		}
		else
		{
			return false;
		}	
	}
	else
	{
		return false;
	}
}

bool KServerPanel::btnBar_MouseClick( const CEGUI::EventArgs& e )
{
	const MouseEventArgs& args = static_cast<const MouseEventArgs&>( e );
	if ( LeftButton == args.button )
	{
		Window* activeBar = args.window;
		if ( m_bar_serverinfo_map.count( activeBar ) ) //activeBar->getChild(activeBar->getName() + "/ServerName")->getText().length() > 0 )
		{
			SetActiveBar( activeBar );
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void KServerPanel::SetActiveBar( Window* activeBar, bool acceptAtOnce )
{
	if ( NULL == activeBar )
	{
		return;
	}
	
	String hoverName;
	if ( NULL != m_pCurrentSelectedBar )
	{
		hoverName = m_pCurrentSelectedBar->getName() + "/Hover";
		m_pCurrentSelectedBar->getChild(hoverName)->setVisible(false);			
		m_pCurrentSelectedBar->getChild(hoverName)->setEnabled(true);
	}
	
	hoverName = activeBar->getName() + "/Hover";
	activeBar->getChild(hoverName)->setVisible(true);
	activeBar->getChild(hoverName)->setEnabled(false);
	
	m_pCurrentSelectedBar = activeBar;
	
	if ( NULL != ServerSelected_CallBack )
	{
		BarServerInfoMap::iterator iter = m_bar_serverinfo_map.find( activeBar );
		if ( m_bar_serverinfo_map.end() != iter )
		{
 			ServerInfo& serverInfo = (*iter).second;
 			(*ServerSelected_CallBack)( serverInfo, this, acceptAtOnce );
		}
		
// 		serverInfo.Name = WStringToAString( activeBar->getChild(activeBar->getName() + "/ServerName")->getText() );
// 		serverInfo.IP = WStringToAString( activeBar->getChild(activeBar->getName() + "/ServerIP")->getText() );
// 		serverInfo.State = WStringToAString( activeBar->getChild(activeBar->getName() + "/ServerStatus")->getText() );
		
	}	
}

void KServerPanel::InsertServer( ServerInfo& newServer )
{
//	ostringstream curBarName;

// 	if ( m_regionInfo-> )
// 	{
// 	}
// 	for ( int i = 0; i < m_barList.size(); i++ )
// 	{
// 		curBarName.str("");
// 		curBarName<<m_pPanel->getName()<<'_'<<i<<'_';
// 		
// 		Window* pCurrentBar= m_barList[i];
// 		
// 		if ( NULL != pCurrentBar )
// 		{
// 			SetBar(i, pCurrentBar);
// 		}
// 	}
}

void KServerPanel::RefreshBars()
{
	for ( int i = 0; i < m_barList.size(); i++ )
	{
		Window* curBar = m_barList[i];
		SetBar(i, curBar);
	}
}

void KServerPanel::SetTitleVisible( bool visible )
{
	if ( NULL != m_pPanel )
	{
		m_pPanel->getChild( m_pPanel->getName() + "/RegionName" )->setVisible( visible );
	}
}

void EncryptServerFile( const string& fileName )
{
// #ifdef _DEBUG
// 	
// #else
	if ( !IsFileEncrypted(fileName) )
	{
		IFaithEncrypter* fileEncripter = GetXOREncrypter();
		
		char szCurDir[COMMON_CLIENT_MSG_LEN_256];
		::GetCurrentDirectory(COMMON_CLIENT_MSG_LEN_256, szCurDir);
		string strIniFileName = szCurDir + fileName;
		
		fileEncripter->Encrypt(strIniFileName.c_str(), strIniFileName.c_str());
	}
/*#endif*/
}

bool IsFileEncrypted( const string& fileName )
{
	char szPath[COMMON_CLIENT_MSG_LEN_256];
	::GetCurrentDirectory( COMMON_CLIENT_MSG_LEN_256 , szPath );
	strcat( szPath, fileName.c_str() );

	DWORD dwVer = GetPrivateProfileInt( "List", "Version", 0xffffffff, szPath );
	if ( dwVer == 0xffffffff )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UnencryptServerFileToTmp(const string & serverFile , const string & tmpFile)
{
	if ( IsFileEncrypted(serverFile) )	
	{
		IFaithEncrypter* fileEncripter = GetXOREncrypter();
		
		char szCurDir[COMMON_CLIENT_MSG_LEN_256];
		::GetCurrentDirectory(COMMON_CLIENT_MSG_LEN_256, szCurDir);
		string strIniFileName = szCurDir + serverFile;
		string strTmpFileName = szCurDir + tmpFile;
		
		return fileEncripter->Unencrypt(strIniFileName.c_str(), strTmpFileName.c_str()) == TRUE ? true : false;
	}
	else
	{
		return false;
	}
}

void DeleteTmpFile(const string & tmpFile)
{
	char szCurDir[COMMON_CLIENT_MSG_LEN_256];
	::GetCurrentDirectory(COMMON_CLIENT_MSG_LEN_256, szCurDir);
	string strIniFileName = szCurDir + tmpFile;

	::DeleteFile(strIniFileName.c_str());
}

void UnencryptServerFile( const string& fileName )
{
	if ( IsFileEncrypted(fileName) )	
	{
		IFaithEncrypter* fileEncripter = GetXOREncrypter();
		
		char szCurDir[COMMON_CLIENT_MSG_LEN_256];
		::GetCurrentDirectory(COMMON_CLIENT_MSG_LEN_256, szCurDir);
		string strIniFileName = szCurDir + fileName;

		fileEncripter->Unencrypt(strIniFileName.c_str(), strIniFileName.c_str());
	}
}

std::string WStringToAString( const CEGUI::String& utf8String )
{
	return string(Utf8ToAnsi(utf8String));
}

/********************************************************************
/*						class: KServerInfo
*********************************************************************/
KLoginList::KLoginList()
: m_NetCount(0)
, m_Version(0)
{
	
}

KLoginList::~KLoginList()
{
	
}

void KLoginList::LoadLoginList( const string& fileName, bool bTempServerList, bool useEncrypt /*= true*/ )
{
	if ( useEncrypt )
	{
		string curEncryptFile = ( bTempServerList == true ? TempEncriptTmpFile : EncriptTmpFile );
		if ( UnencryptServerFileToTmp( fileName , curEncryptFile ) )
		{
			innerLoadLoginList( curEncryptFile , bTempServerList );
		}
		else
		{
			innerLoadLoginList( fileName, bTempServerList );
		}
		
		DeleteTmpFile( curEncryptFile );
		
		EncryptServerFile(fileName);
	}
	else
	{
		innerLoadLoginList(fileName, bTempServerList);
	}
}

vector<NetInfo>&	KLoginList::getNets()
{
	return m_nets; 
};

int	KLoginList::getVersion()
{
	return m_Version; 
};

int	KLoginList::getNetCount()
{
	return m_NetCount; 
};

void KLoginList::innerLoadLoginList( const string& fileName, bool bTempServerList )
{
	if ( bTempServerList )
	{
		m_serverStateIni.Load(TempServerStateFileName.c_str());
	}
	else
	{
		m_serverStateIni.Load(ServerStateFileName.c_str());
	}	

	KIniFile ini;
	if( !ini.Load( fileName.c_str() ) )
	{
		return;
	}

	ini.GetInteger("[List]", "Version", 0, &m_Version);
	ini.GetInteger("[List]", "NetCount", 0, &m_NetCount);

	m_nets.clear();
	for ( int i = 0; i < m_NetCount; i++ )
	{
		NetInfo curNetInfo;
		curNetInfo.SectionName = "Net_" + iToStr(i);

		char szNetName[COMMON_CLIENT_MSG_LEN_64];
		ZeroMemory(szNetName, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
		ini.GetString( 
			curNetInfo.SectionName.c_str(), 
			"NetName", 
			"", 
			szNetName, 
			COMMON_CLIENT_MSG_LEN_64 );

		szNetName[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
		curNetInfo.Name = szNetName;

		ini.GetInteger( 
			curNetInfo.SectionName.c_str(), 
			"RegionNum", 
			0,
			&curNetInfo.RegionCount);

		LoadRegions(ini, curNetInfo);

		m_nets.push_back(curNetInfo);
	}

	m_serverStateIni.Clear();
}

void KLoginList::LoadRegions( KIniFile& ini, NetInfo& destNet )
{
	for ( int i = 0; i < destNet.RegionCount; i++ )
	{
		RegionInfo curRegionInfo;
		curRegionInfo.SectionName = destNet.SectionName + "_Region_" + iToStr(i);

		char szRegionName[COMMON_CLIENT_MSG_LEN_64];
		ZeroMemory(szRegionName, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
		ini.GetString( 
			curRegionInfo.SectionName.c_str(), 
			"RegionName", 
			"", 
			szRegionName, 
			COMMON_CLIENT_MSG_LEN_64 );
		
		szRegionName[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
		curRegionInfo.Name = szRegionName;

		ini.GetInteger( 
			curRegionInfo.SectionName.c_str(),
			"ServerNum",
			0,
			&curRegionInfo.ServerCount);

		LoadServers(ini, curRegionInfo);
		destNet.Regions.push_back(curRegionInfo);
	}
}

void KLoginList::LoadServers( KIniFile& ini, RegionInfo& destRegion )
{
	for ( int i = 0; i < destRegion.ServerCount; i++ )
	{
		ServerInfo curServerInfo;
		
		KLoginList::LoadServerInfo(ini, destRegion.SectionName, i, curServerInfo);
// 		//取得服务器名称
// 		char szServerName[COMMON_CLIENT_MSG_LEN_64];
// 		string strKeyName_ServerName = iToStr(i) + "_Server_Name";
// 		ZeroMemory(szServerName, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
// 		ini.GetString( 
// 			destRegion.SectionName.c_str(), 
// 			strKeyName_ServerName.c_str(), 
// 			"", 
// 			szServerName,
// 			COMMON_CLIENT_MSG_LEN_64 );
// 		
// 		szServerName[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
// 		curServerInfo.Name = szServerName;
// 
// 		//取得服务器IP
// 		char szServerIP[COMMON_CLIENT_MSG_LEN_64];
// 		string strKeyName_ServerIP = iToStr(i) + "_Server_IP";
// 		ZeroMemory(szServerIP, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
// 
// 		ini.GetString( 
// 			destRegion.SectionName.c_str(), 
// 			strKeyName_ServerIP.c_str(), 
// 			"", 
// 			szServerIP,
// 			COMMON_CLIENT_MSG_LEN_64 );
// 		
// 		szServerIP[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
// 		curServerInfo.IP = szServerIP;
// 
// 		//取得服务器默认状态
// 		char szServerState[COMMON_CLIENT_MSG_LEN_64];
// 		string strKeyName_ServerState = iToStr(i) + "_Server_State";
// 		ZeroMemory(szServerState, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
// 		
// 		ini.GetString( 
// 			destRegion.SectionName.c_str(), 
// 			strKeyName_ServerState.c_str(), 
// 			"", 
// 			szServerState,
// 			COMMON_CLIENT_MSG_LEN_64 );
// 		
// 		szServerState[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
// 		curServerInfo.State = szServerState;

		//取得服务器最新状态
		char szServerNewState[COMMON_CLIENT_MSG_LEN_64];
		string realServerName = KUiServerList::getRealServerName( curServerInfo.Name );
		m_serverStateIni.GetString(
			realServerName.c_str(),
			"State",
			curServerInfo.State.c_str(),
			szServerNewState,
			COMMON_CLIENT_MSG_LEN_64 );
		curServerInfo.State = szServerNewState;

		//取得服务器的Region
		curServerInfo.Region = destRegion.Name;
		
		destRegion.Servers.push_back(curServerInfo);
	}
}

void KLoginList::LoadServerInfo( KIniFile& ini, const string& sectionName, int index, ServerInfo& outInfo )
{
	//取得服务器名称
	char szServerName[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerName = iToStr(index) + "_Server_Name";
	ZeroMemory(szServerName, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
	ini.GetString( 
		sectionName.c_str(),
		strKeyName_ServerName.c_str(), 
		"", 
		szServerName,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerName[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.Name = szServerName;
	
	//取得服务器IP
	char szServerIP[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerIP = iToStr(index) + "_Server_IP";
	ZeroMemory(szServerIP, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
	
	ini.GetString( 
		sectionName.c_str(), 
		strKeyName_ServerIP.c_str(), 
		"", 
		szServerIP,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerIP[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.IP = szServerIP;
	
	//取得服务器默认状态
	char szServerState[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerState = iToStr(index) + "_Server_State";
	ZeroMemory(szServerState, sizeof(char) * COMMON_CLIENT_MSG_LEN_64);
	
	ini.GetString( 
		sectionName.c_str(), 
		strKeyName_ServerState.c_str(), 
		"", 
		szServerState,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerState[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.State = szServerState;	

	//服务器名字前缀
	char szServerPrefix[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerPrefix = iToStr( index ) + "_Server_Prefix";
	ZeroMemory( szServerPrefix, sizeof( szServerPrefix ) );
	ini.GetString( 
		sectionName.c_str(),
		strKeyName_ServerPrefix.c_str(), 
		"", 
		szServerPrefix,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerPrefix[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.Prefix = szServerPrefix;

	//服务器名字前缀2
	char szServerPrefix2[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerPrefix2 = iToStr( index ) + "_Server_Prefix2";
	ZeroMemory( szServerPrefix2, sizeof( szServerPrefix2 ) );
	ini.GetString( 
		sectionName.c_str(),
		strKeyName_ServerPrefix2.c_str(), 
		"", 
		szServerPrefix2,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerPrefix2[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.Prefix2 = szServerPrefix2;

	//服务器名字后缀
	char szServerPostfix[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerPostfix = iToStr( index ) + "_Server_Postfix";
	ZeroMemory( szServerPostfix, sizeof( szServerPostfix ) );
	ini.GetString( 
		sectionName.c_str(),
		strKeyName_ServerPostfix.c_str(), 
		"", 
		szServerPostfix,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerPostfix[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.Postfix = szServerPostfix;

	//服务器名字后缀2
	char szServerPostfix2[COMMON_CLIENT_MSG_LEN_64];
	string strKeyName_ServerPostfix2 = iToStr( index ) + "_Server_Postfix2";
	ZeroMemory( szServerPostfix2, sizeof( szServerPostfix2 ) );
	ini.GetString( 
		sectionName.c_str(),
		strKeyName_ServerPostfix2.c_str(), 
		"", 
		szServerPostfix2,
		COMMON_CLIENT_MSG_LEN_64 );
	
	szServerPostfix2[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
	outInfo.Postfix2 = szServerPostfix2;
}
void KLoginList::WriteServerInfo( KIniFile& ini, const string& sectionName, int index, const ServerInfo& inInfo )
{
	string keyName = "";
	string indexShow = iToStr( index );

// 	keyName = indexShow + "_Server_ID";
// 	ini.WriteInteger(
// 		sectionName.c_str(), 
// 		keyName.c_str(), 
// 		inInfo.ID );
	
	keyName = indexShow + "_Server_Name";
	ini.WriteString(
		sectionName.c_str(), 
		keyName.c_str(),
		inInfo.Name.c_str() );
	
	keyName = indexShow + "_Server_IP";
	ini.WriteString(
		sectionName.c_str(), 
		keyName.c_str(), 
		inInfo.IP.c_str() );

	keyName = indexShow + "_Server_State";
	ini.WriteString(		
		sectionName.c_str(),
		keyName.c_str(), 
		inInfo.State.c_str() );

	//前缀和后缀信息不需要存储
// 	keyName = indexShow + "_Server_Prefix";
// 	ini.WriteString(
// 		sectionName.c_str(), 
// 		keyName.c_str(),
// 		inInfo.Prefix.c_str() );
// 
// 	keyName = indexShow + "_Server_Postfix";
// 	ini.WriteString(
// 		sectionName.c_str(), 
// 		keyName.c_str(),
// 		inInfo.Postfix.c_str() );
}

unsigned int __stdcall DownLoadTempServerList( void* param )
{
	::ResetEvent( g_updateEvent );
	KUiServerListStateUpdate update;
	update.Init();
	g_updateRet = update.ServerStateDownThread();
	setUpdateOk(true);	
	::SetEvent( g_updateEvent );
	::_endthreadex(0);
	return 0;
}

void ShowLocalServerList( void )
{
	KUiServerList::Hide();
	KUiServerList::Show( false );
}
