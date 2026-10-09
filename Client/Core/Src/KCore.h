//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCore.h
// Date:	2000.08.08
// Code:	Daphnis Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KCore_H
#define KCore_H
//---------------------------------------------------------------------------
#ifdef _STANDALONE
	#define CORE_API
#else
 #ifdef CORE_EXPORTS
		#define CORE_API __declspec(dllexport)
	#else
		#define CORE_API __declspec(dllimport)
 #endif
#endif

//#define	defNEW_TONG
#pragma warning (disable: 4512)
#pragma warning (disable: 4786)

//---------------------------------------------------------------------------
#define	DIR_DOWN		0
#define	DIR_LEFTDOWN	1
#define	DIR_LEFT		2
#define	DIR_LEFTUP		3
#define	DIR_UP			4
#define	DIR_RIGHTUP		5
#define	DIR_RIGHT		6
#define	DIR_RIGHTDOWN	7
//---------------------------------------------------------------------------

#include "GlobalDef.h"
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#ifndef _SERVER
	#include "KSpriteCache.h"
	#include "KFont.h"
#endif
#include "KTabFile.h"
#include "KProtocol.h"
#include "KEngine.h"
#include "KScriptList.h"
#include "KScriptCache.h"
#include "MyAssert.H"
#include "cfs_filelogs.h"

#include "networkinterface.h"
#include "ministerinterface.h"
#ifndef _SERVER
#include "KMusic.h"
#include "KSoundCache.h"
#endif

#define TASKCONTENT
#define ITOA(NUMBER)  #NUMBER

#define __TEXT_LINE__(LINE) ITOA(LINE)

#define	NET_DEBUG
extern  KTabFile		g_OrdinSkillsSetting, g_MisslesSetting;

extern KTabFile		g_SkillLevelSetting;
extern KTabFile		g_NpcSetting;
extern KTabFile		g_RankTabSetting;

#ifndef _SERVER
extern KSoundCache		g_SoundCache;

extern KMusic			*g_pMusic;

extern unsigned int	* g_pAdjustColorTab;
extern unsigned int g_ulAdjustColorCount;

#endif

extern KTabFile		g_NpcKindFile; //记录Npc人物类型文件

#ifndef _SERVER
extern BOOL g_bUISelIntelActiveWithServer;//当前选择框是否与服务器端交互
extern BOOL g_bUISpeakActiveWithServer;
extern int g_bUISelLastSelCount;
#endif

#define QUESTIONSIZE (1024 * 50)
#ifndef _STANDALONE
#include "minilzo.h"
#else
#include "minilzo.h"
#endif

typedef union { void *vp; lzo_bytep bp; lzo_uint32 u32; long l; } lzo_align_t;

#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);


#ifndef _WIN32
typedef WORD *PWORD;
#endif

#ifdef _SERVER

enum DBMESSAGE
{
	DBMSG_PUSH,
	DBMSG_POP,
};

enum DBI_COMMAND
{
	DBI_PLAYERSAVE,  // 参数1 是否可以新建角色
	DBI_PLAYERLOAD,
	DBI_PLAYERDELETE,
	DBI_GETPLAYERLISTFROMACCOUNT,
};
BOOL CORE_API g_AccessDBMsgList(DBMESSAGE Msg,  int* pnPlayerIndex, DBI_COMMAND * pnDBICommand, void ** ppParam1, void ** ppParam2);
extern KLuaScript g_WorldScript;

class KDBMsgNode :public KNode
{
public:
	void * pParam1;
	void * pParam2;
	int	   nPlayerIndex;
	DBI_COMMAND Command;
	KDBMsgNode(){pParam1 = pParam2 = NULL; nPlayerIndex =  0;	Command = DBI_PLAYERSAVE;};
};

//add by zuolizhi for pic question
#include <vector>

typedef struct _Pic_Question_Config 
{
	#define MAXMAPCOUNT 100
	
	unsigned int nSendByteSec;
	unsigned int nPicQuesAnswerLimit;
	unsigned int nPicMaxError;
	unsigned int nNumInterval;
	unsigned int nDisPicCount;
	unsigned int nDisMaxAlpha;
	unsigned int nFrtMinAlpha;
	unsigned int nPicQuesCount;
	unsigned int nNumQuesCount;
	unsigned int nNumQuesEnd;
	unsigned int nCompress;
	unsigned int nSwitch;
	unsigned int nNoQMap[MAXMAPCOUNT];
	
	bool IsNoQMap( unsigned int MapID )
	{
		for( int nLoopCount = 0; nLoopCount < MAXMAPCOUNT; nLoopCount++ )
			if( MapID == nNoQMap[nLoopCount] )
				return true;

		return false;
	}

}PQ_CONFIG,*PPQ_CONFIG;

typedef struct _Pic_Question
{
	unsigned int	nLen;
	char			szQBuf[QUESTIONSIZE];
	union{
		unsigned int	nAnswer;
		char			szAnswer[4];
	};

}PIC_QUESTION,*PPIC_QUESTION;

typedef struct _PIC
{
	int	nLen;
	char szBuffer[QUESTIONSIZE];
}PIC,*PPIC;

typedef std::vector<PIC> PIC_VECTOR;
typedef std::vector<PIC_QUESTION> QUESTION_VECTOR;
typedef std::vector<unsigned int> LEVELKILLNPCVECTOR;

typedef struct _RGB555 
{	
	WORD _B : 5;
	WORD _G : 5;
	WORD _R : 5;
	WORD _T : 1;
}RGB555,*PRGB555;

#define RMASK 0x7C00
#define GMASK 0x03E0
#define BMASK 0x001F

#endif

#ifndef _SERVER
class KImageNode : public KNode
{
public:
	char	m_szFile[32];
	int		m_nFrame;
	int		m_nXpos;
	int		m_nYpos;
};
#endif

//--------------------------------------------------------->
#define b_isascii(_c) ( (unsigned char)(_c) < 0x80 )
#define b_isprint(_c) ( ( (unsigned char)(_c) > 0x20 ) && ( (unsigned char)(_c) < 0x7E ) )
#define b_isnumber(_c) ( ( (unsigned char)(_c) >= 0x30 ) && ( (unsigned char)(_c) <= 0x39 ) )
#define b_ischar(_c) ( ( ( (unsigned char)(_c) >= 0x41 ) && ( (unsigned char)(_c) <= 0x5a ) ) || ( ( (unsigned char)(_c) >= 0x61 ) && ( (unsigned char)(_c) <= 0x7a ) ) )

#define b_isprintgbklead(_c) ( ( ( (unsigned char)(_c) >= 0x81 ) && ( (unsigned char)(_c) <= 0xa0 ) ) || ( ( (unsigned char)(_c) >= 0xaa ) && ( (unsigned char)(_c) <= 0xfe ) ) )
#define b_isprintgbktrail(_c) ( ( (unsigned char)(_c) >= 0x40 ) && ( (unsigned char)(_c) <= 0xfe )  )

inline bool IsRule( const char* szText )
{
	int nLoopCount = 0;
	char	c;
	while( (c = szText[nLoopCount]) != 0 )
	{
		if( b_isascii( c ) )
		{
			if( !b_isnumber( c ) && !b_ischar( c ) )
				return false;
		}
		else
		{
			if( !b_isprintgbklead( c ) )
				return false;
			else
			{
				if( b_isprintgbktrail( szText[nLoopCount + 1] ) )
				{
					nLoopCount += 2;
					continue;
				}
				else
					return false;
			}
		}
		
		nLoopCount ++;
	}
	
	return true;
}
//--------------------------------------------------------->

#ifndef _SERVER
extern char* g_GetStringRes(int nStringID, char * szString, int nMaxLen);
#endif

BOOL InitSkillSetting();
BOOL InitNpcSetting();
void g_ReleaseCore();

inline int GetRandomNumber(int nMin, int nMax)
{
	return g_Random(nMax - nMin + 1) + nMin;
}

//-->Rocker 2004/11/21 塔型几率计算: 闭区间
#define MAX_CHANCE_TABLE_ELEMENT 200
#define MAX_RAND_NUMBER 0x7fff
extern int g_ChanceTable[MAX_CHANCE_TABLE_ELEMENT];
inline int AdditionalRandomNumber(int nMin, int nMax)
{
	if ((nMin < 0)||(nMax < 0)||(nMax < nMin))
		return nMin;
	if (nMax - nMin > MAX_CHANCE_TABLE_ELEMENT)
		return GetRandomNumber(nMin, nMax);

	int nAdd = nMin;
	int i = 0;
	for (i=0; i<nMax - nMin; i++)
		nAdd += i;
	
	int nScale = nAdd;

	for (i=0; i<nMax - nMin; i++)
	{
		g_ChanceTable[i] = nAdd / nScale * MAX_RAND_NUMBER;
		nAdd--;
	}

	for (i=0; i<nMax - nMin; i++)
	{
		int t = 0;
		if (g_Random(MAX_RAND_NUMBER - t) <= g_ChanceTable[i])
			return nMin + i;
		t = t + g_ChanceTable[i];
	}

	return nMax;
}
//<--Rocker 

#ifdef _SERVER
void g_SetServer(LPVOID pServer);
extern IServer* g_pServer;

#else
void g_SetClient(LPVOID pClient, int nConnectID);
extern IClient* g_pClient;
extern int		g_ConnectID;

#endif

extern int g_nScreenWidth;
extern int g_nScreenHeight;

#ifdef _SERVER
#include "ILogSystem.h"
extern ILogSystem* g_pLogSystem;
#endif

#ifdef _SERVER
#include "cfs_db_interface.h"
#include "cfs_fs2_savedef.h"

enum
{
	global_data_task = 1,
	global_data_taisui,
	global_data_npcsave,
	global_data_instance,
	global_data_warinfo,
	global_data_poolinfo,
	global_data_economy,
	global_data_end
};

struct _GlobalHeader : _DBProcHeader 
{
	int nGlobalID;
};

int GetTaskGlobal( int nIndex );
void SetTaskGlobal( int nIndex, int nValue );
void LoadTaskGlobal( );
void SaveTaskGlobal( );

//-------------------------------------------------------------------------
//副本全局数据
typedef struct _InstanceGlobalData
{
	_InstanceGlobalData()
	{
		dwInstanceId = 0;
	}

	DWORD dwInstanceId;
} InstanceGlobalData;

DWORD NewInstanceId();//创建新的副本编号
void LoadInstanceGlobalData();//加载副本全局数据
void SaveInstanceGlobalData();//保存副本全局数据

#include <map>
typedef std::map<DWORD,DWORD> INDEXIDMAP;
typedef std::map<std::string,DWORD> INDEXNAMEMAP;

//-------------------------------------------------------------------------

#define MAX_TASK_VALUE_COUNT	500
struct _GlobalTask {
	int nVer;
	int nTaskValue[MAX_TASK_VALUE_COUNT];
};

int GlobalDataProcess( 
	IProcRet* pRet );

void SetController( LPVOID pController );
extern IController* g_pController;
extern unsigned long UNIX_TMIE_STAMP;
extern unsigned long TIMEZONE_CORRECT;

#ifndef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <algorithm>
#include <time.h>
extern void getcallstack( unsigned long ulStack[], int nLevel );

inline void strlwr( char* str )
{
	for ( ; *str != '\0'; str++ )
		*str = tolower(*str);
}
#else
#include "time.h"
#endif
#endif
//---------------------------------------------------------------------------
#endif
