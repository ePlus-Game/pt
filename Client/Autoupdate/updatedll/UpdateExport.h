//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   UpdateExport.h
//  Version     :   1.0
//  Creater     :   Cheng bitao
//  Date        :   2002-11-28 17:24:39
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATEEXPORT_H_
#define _UPDATEEXPORT_H_

//******************************************************消息区域***************************************
#define defUPDATE_RESULT_INIT_FAILED				0x00
#define defUPDATE_RESULT_INIT_SUCCESS				0x01
#define defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED		0x02	
#define defUPDATE_RESULT_DOWNLOAD_INDEX_SUCCESS  	0x03
#define defUPDATE_RESULT_PROCESS_INDEX_FAILED		0x04
#define defUPDATE_RESULT_PROCESS_INDEX_SUCCESS		0x05
#define defUPDATE_RESULT_NOT_UPDATE_FILE			0x06
#define defUPDATE_RESULT_DOWNLOAD_FAILED			0x07
#define defUPDATE_RESULT_DOWNLOAD_SUCCESS			0x08
#define defUPDATE_RESULT_UPDATE_FAILED				0x09
#define defUPDATE_RESULT_UPDATE_SUCCESS				0x0A
#define defUPDATE_RESULT_UPDATE_SUCCESS_NEED_REBOOT 0x0B
#define defUPDATE_RESULT_LOAD_SOCKET_ERROR			0x0C				
#define defUPDATE_RESULT_USER_VERIFY_FAILED         0x0D
#define defUPDATE_RESULT_USER_VERIFY_SUCCESS        0x0E
#define defUPDATE_RESULT_INVALIDSN                  0x0F
#define defUPDATE_RESULT_PIRATICSN                  0x10
#define defUPDATE_RESULT_INHIBITIVESN               0x11
#define defUPDATE_RESULT_CONNECT_SERVER_FAILED      0x12
#define defUPDATE_RESULT_ERRORSN                    0x13
#define defUPDATE_RESULT_CANCEL                     0x14
#define defUPDATE_RESULT_UPDATESELF_SUCCESS			0x15
#define defUPDATE_RESULT_VERSION_NOT_ENOUGH         0x16
#define defUPDATE_RESULT_VERSION_MORE				0x17
#define defUPDATE_RESULT_VERSION_LATEST				0x18	//Modified by Fellow, 2003.11.13
//Modified by Fellow, 2003.12.9
//启动前和启动后运行程序的标志
#define defUPDATE_RESULT_RUNBEFORE					0x19
#define defUPDATE_RESULT_RUNAFTER					0x20

//hehongpeng add
//用来通知界面由多少个文件需要下载
#define	defUPDATE_RESULT_TOTALFILENUM				0x21

//通知界面：为非强制跟新且可以进行游戏  Add by Brianyao 2007
//表示需要更新，且为非强制更新
#define defUPDATE_RESULT_CANPLAY_UPDATE_NEXTIME     0x22
#define defUPDATE_RESULT_NEED_UPDATE                0x23

#define defUPDATE_STATUS_INITIALIZING               0x01
#define defUPDATE_STATUS_VERIFING                   0x02
#define defUPDATE_STATUS_PROCESSING_INDEX           0x03
#define defUPDATE_STATUS_DOWNLOADING                0x04
#define defUPDATE_STATUS_DOWNLOADING_FILE           0x05
#define defUPDATE_STATUS_UPDATING                   0x06

//*********************************************消息区域结束*********************************

#define PROXY_METHOD_DIRECT                 0
#define PROXY_METHOD_USEIE                  1
#define PROXY_METHOD_CUSTOM                 2

#define PROXY_VAR_LEN			            100



//回调函数
typedef int _stdcall FN_UPDATE_CALLBACK(int nCurrentStatus, long lParam);

typedef struct _tagDownloadFileStatus
{
	char	strFileName[MAX_PATH];		// 文件名
	DWORD	dwFileSize;					// 文件大小
	DWORD	dwFileDownloadedSize;		// 已经下载大小
} DOWNLOADFILESTATUS,*LPDOWNLOADFILESTATUS;

#pragma pack(push, 1)
//代理信息
typedef struct tagKPROXY_SETTING
{
    int  nProxyMethod;              //DIRECT/USEIE/CUSTOM
    int  nProxyMode;                //PROXY_MODE_... when PROXY_METHOD_CUSTOM
    
    char szHostAddr[PROXY_VAR_LEN];
    int  nHostPort;
    
    BOOL bUpdateAuth;
    char szUserName[PROXY_VAR_LEN];
    char szPassword[PROXY_VAR_LEN];

} KPROXY_SETTING;

//初始化接口的结构体
typedef struct tagKUPDATE_SETTING
{
	//毒霸只使用了一个版本，目前的游戏升级需要两个版本，
	//其中大版本是防止过久的版本不能使用自动更新
	int		nVersion;					//小版本号
	int		nMajorVersion;				//大版本号
	
    int     nUpdateMode;                // 0: Internet  1: LAN
    
    ULONG   ulTryTimes;                 // Times of try when download failed
    CHAR    szUpdateSite[MAX_PATH];     // Download host URL 
        
    BOOL    bAutoTryNextHost;           // Flag of try use next faster host when failed
    BOOL    bUseFastestHost;            // Flag use the fastest host
    
    BOOL    bUseVerify;					// 
    CHAR    szVerifyInfo[MAX_PATH];		// verifyinfo such as serial number.etc

    CHAR    szDownloadPath[MAX_PATH];   // Download destination path
    CHAR    szUpdatePath[MAX_PATH];     // Update destination path
	CHAR    szMainExecute[MAX_PATH];    // main program

    FN_UPDATE_CALLBACK *pfnCallBackProc; 

    KPROXY_SETTING ProxySetting;
    
	BOOL	bLog;
	BOOL    bExServerMode; //<------EXVERSION

} KUPDATE_SETTING;

#pragma pack(pop)

//提供的接口函数
int __stdcall Update_Init(const KUPDATE_SETTING UpdateSetting);
int __stdcall Update_UnInit();
int __stdcall Update_Start();
int __stdcall Update_Cancel();
//Add By Brianyao 2007
//只Download 不用更新,下次启动的时候更新.
int __stdcall Update_NextTime();

bool __stdcall GetUpdateLock(DWORD dwTime);
void __stdcall ReleaseUpdateLock();

#endif	//endof _UPDATEEXPORT_H_