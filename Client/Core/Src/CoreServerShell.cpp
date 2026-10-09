/*****************************************************************************************
//	外界访问Core server 接口方法
//	Copyright : Kingsoft 2002
//	Author	:   Wooy (Wu yue)
//	CreateTime:	2002-12-20
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "CoreServerShell.h"
#include "KThread.h"
#include "KPlayer.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "guard_protocol_process.h"
#include "KPlayerSet.h"

#include "networkinterface.h"

#ifdef _STANDALONE
#else
#include "KGmProtocol.h"
#endif

#include "LuaFuns.h"
#include "KSortScript.h"
#include "KSubWorld.h"
#include "malloc.h"
#include "PlayerCreator.h"
#include "CoreRelated.h"
#include "KWarInfoManager.h"
#include "pool_combat_info_mgr.h"
#include "npc_save.h"
#include "keconomysys.h"

typedef iCoreServerShell	CoreServerShell;

static CoreServerShell	g_CoreServerShell;
unsigned long UNIX_TMIE_STAMP;

void g_InitCore();
void g_FSEyeInitCore( );

const KBASICPROP_ITEM* g_GetItemTemplate(
	IN int nGenre,
	IN int nDetailType,
	IN int nParticularType,
	IN int nLevel );
										 
#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreServerShell* CoreGetServerShell()
{
	return &g_CoreServerShell;
}

void CoreServerShell::Release()
{
	g_ReleaseCore();
}

int CoreServerShell::AddCharacter(
	const tagExtPointInfo& cExtPointInfo, 
	const tagExtPointInfo& cChangeExtPointInfo, 
	char* szAccName,
	char* szRoleName,
	unsigned long ulNetID,
	GUID* pGuid )
{
	int nIdx = 0;

	if ( szRoleName && szRoleName[0])
	{
		nIdx = PlayerSet.Add( szRoleName, pGuid );
		if (nIdx <= 0 || nIdx >= MAX_PLAYER)
			return 0;

		strcpy( Player[nIdx].m_AccoutName, szAccName );
		strcpy( Player[nIdx].m_PlayerName, szRoleName );

		Player[nIdx].m_nNetConnectIdx = ulNetID;
		Player[nIdx].m_ulLastSaveTime = g_SubWorldSet.m_nLoopRate;

		Player[nIdx].SetExtPoint( cExtPointInfo );
		for (int i = 0; i < MAX_EXT_POINT_COUNT; i++)
		{
			Player[nIdx].SetExtPoint(i, cExtPointInfo.nExtPoint[i]);
		}

		Player[nIdx].m_AntiEnthrall.PlayerOnline(cExtPointInfo.dwLimitOnlineSecond, 
			cExtPointInfo.dwLimitPlayTimeFlag
			);

		return nIdx;
	}
	return 0;
}

int CoreServerShell::DbOpComplete(
	int nPlayerIndex,
	IProcRet* pRet )
{
	if( nPlayerIndex == INVALID_VALUE )
		return GlobalDataProcess( pRet );
	else
		return Player[nPlayerIndex].PlayerDbOpComplete( pRet );
}

int g_nMaxPlayer;
long g_GameStartTime;

void CoreServerShell::Init( int nMaxPlayer )
{
	g_nMaxPlayer = nMaxPlayer;
	g_GameStartTime = UNIX_TMIE_STAMP;
	g_InitCore();
}

void CoreServerShell::FSEyeCfgInit( )
{
	g_FSEyeInitCore( );
}

const KBASICPROP_ITEM* CoreServerShell::FSEyeGetItem(
	IN int nGenre,
	IN int nDetailType,
	IN int nParticularType,
	IN int nLevel )
{
	return g_GetItemTemplate( nGenre, nDetailType, nParticularType, nLevel );
}

void CoreServerShell::AddPlayerToWorld(int nIndex)
{
	Player[nIndex].LaunchPlayer();
}

void CoreServerShell::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	PlayerSet.ProcessClientMessage(nIndex, pChar, nSize);
}

void CoreServerShell::ProcessGuardMessage(const char* pMsg, int nSize)
{
	g_GuardProtocolProcess.ProcessMsg((BYTE*)pMsg, nSize);
}

void CoreServerShell::ProcessPaysysMessage(int nIndex, const char* pChar, int nSize)
{
	PlayerSet.ProcessPaysysMessage( nIndex, pChar, nSize );
}

#include "buff_man.h"

void CoreServerShell::ClientDisconnect(int nIndex)
{
//	if( Player[nIndex].IsLogoutTiming() )
//		return;

	Player[nIndex].Offline();

	PlayerSet.PrepareRemove(nIndex);

	//防止泄漏
	BuffMgr::Singleton( ).PlayerOffline( Player[nIndex].m_nIndex );	
}

bool CoreServerShell::IsCanRemove( int nIndex )
{
	if (TRUE == Player[nIndex].IsCanRemove())
		return true;
	else
		return false;
}

void CoreServerShell::RemoveQuitingPlayer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	if (Player[nIndex].IsWaitingRemove())
	{
		PlayerSet.RemoveQuiting(nIndex);
	}
}

void CoreServerShell::RemovePlayer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.RemoveQuiting(nIndex);
}
//--------------------------------------------------------------------------
//	功能：从游戏世界获取数据
//	参数：unsigned int uDataId --> 表示获取游戏数据的数据项内容索引，其值为梅举类型
//							GAMEDATA_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uDataId的取值情况而定
//		  int nParam --> 依据uDataId的取值情况而定
//	返回：依据uDataId的取值情况而定。
//--------------------------------------------------------------------------
int	CoreServerShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：向游戏发送操作
//	参数：unsigned int uDataId --> Core外部客户对core的操作请求的索引定义
//							其值为梅举类型GAMEOPERATION_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uOperId的取值情况而定
//		  int nParam --> 依据uOperId的取值情况而定
//	返回：如果成功发送操作请求，函数返回非0值，否则返回0值。
//--------------------------------------------------------------------------
int	CoreServerShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{
	case SSOI_LAUNCH:	//启动服务
		nRet = OnLunch((LPVOID)uParam, (LPVOID)nParam);
		break;
	case SSOI_SHUTDOWN:	//关闭服务
		nRet = OnShutdown();
		break;
	default:
		nRet = 0;
		break;
	}

	return nRet;
}

int CoreServerShell::OnLunch(LPVOID pServer, LPVOID pController )
{
	g_SetServer(pServer);
	SetController( pController );

	KLuaScript * pStartScript =(KLuaScript*) g_GetScript("\\script\\ServerScript.lua");
	int i = 0;
	
	if (!pStartScript)
		g_DebugLog("Load ServerScript failed!");
	else
	{	
		pStartScript->CallFunction("StartGame",0,"");
	}

	PlayerSet.ReloadWelcomeMsg();

	LoadTaskGlobal( );

	return true;
}

int CoreServerShell::OnShutdown( )
{
	g_SetServer( NULL );

	SaveTaskGlobal( );

	NpcSave::SaveAllNpc();

	GetPoolCombatInfoManager().SaveFromDBOP();
	
	GetGlobalWarInfoManager().SaveFromDBOP();

	KEconomySysManager::Singleton().Save();
	return true;
}

//日常活动，core如果要寿终正寝则返回0，否则返回非0值
int CoreServerShell::Breathe()
{
	g_SubWorldSet.MainLoop();
	return true;
}

bool CoreServerShell::CheckProtocolSize(const char* pChar, int nSize)
{
	WORD wCheckSize;
	BYTE nProtocol = (BYTE)pChar[0];

	if (nProtocol >= c2s_end || nProtocol <= c2s_gameserverbegin)
	{
		CFS_FILELOGS::WriteDebugLog("Invalid network protocol(%d)\n", nProtocol);
		return false;
	}

	if (g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1] == -1)
	{
		wCheckSize = *(WORD*)&pChar[1] + PROTOCOL_MSG_SIZE;
	}
	else
	{
		wCheckSize = g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1];
	}
	if (wCheckSize != nSize)
	{
		CFS_FILELOGS::WriteDebugLog("Invalid network protocol(%d), should %d, but %d\n", nProtocol, wCheckSize, nSize);
		return false;
	}
	return true;
}


int CoreServerShell::AttachPlayer(const unsigned long lnID, GUID* pGuid)
{
	return PlayerSet.AttachPlayer(lnID, pGuid);
}

bool CoreServerShell::IsCharacterQuiting(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return FALSE;
	}	

	if (TRUE == Player[nIndex].IsWaitingRemove())
		return true;
	else
		return false;
}

int CoreServerShell::CreateNewRoleData(
		char	szAccName[],
		char	szName[],
		int		nSeries,
		int		nSex,
		int		nMapID,
		int		nImageHead,
		unsigned long ulNetID  )
{
	if( !IsNamePass( szName ) )
	{
		return FALSE;
	}

	ROLEPARAM rp;
	strncpy( rp.szAccName, szAccName, _NAME_LEN );
	strncpy( rp.szName, szName, _NAME_LEN );
	rp.nSeries	= nSeries;
	rp.nSex		= nSex;
	rp.nMapID	= nMapID;
	rp.nImageHead = nImageHead;
	rp.ulNetID	= ulNetID;

	return PlayerCreator::Singleton( ).CreateRole( rp );
}

bool CoreServerShell::IsTextPass(const char *szText)
{
	return g_IsTextPass(szText) ? true : false;
}

bool CoreServerShell::IsNamePass(const char *szText)
{
	return g_IsNamePass(szText) ? true : false;
}

#define MSG_QUESTION_APPEND_DESC_LOGIN 11386

bool CoreServerShell::GetRandomQuestion(
	BYTE* pProtocolBuff,
	int& protocolBuffSize,
	BYTE* pAnswerBuff,
	int& answerBuffSize)
{
	if (NULL == pProtocolBuff || protocolBuffSize <= 0 || NULL == pAnswerBuff || answerBuffSize <= 0)
		return false;

	if (!QuestionManager::Singleton().IsEnabled())
		return false;

	QuestionInstance question;
	if (QuestionManager::Singleton().GetRandomQuestion(question))
	{
		if (question.QuestionLength + sizeof(QUESTION) > protocolBuffSize)
			return false;

		memset(pProtocolBuff, 0, protocolBuffSize);
		QUESTION* pSendQuestion = (QUESTION*)pProtocolBuff;
		pSendQuestion->Protocol = s2c_byte_extend;
		pSendQuestion->ProtocolExtend = s2c_ex_protocol_question;
		pSendQuestion->wProtocolSize = sizeof(QUESTION) - sizeof(pSendQuestion->QuestionData) - 1;
		pSendQuestion->Timeout = 0;
		pSendQuestion->AppendDescStrId = MSG_QUESTION_APPEND_DESC_LOGIN;
		pSendQuestion->IsCompressed = question.IsQuestionCompressed ? TRUE : FALSE;
		memcpy(pSendQuestion->QuestionData, question.Question, question.QuestionLength);
		pSendQuestion->wProtocolSize += question.QuestionLength;
		protocolBuffSize = pSendQuestion->wProtocolSize + 1;

		if (question.AnswerSize > answerBuffSize)
			return false;

		int realAnswerSize = question.AnswerSize * sizeof(Character);

		memset(pAnswerBuff, 0, answerBuffSize);
		memcpy(pAnswerBuff, question.Answer, realAnswerSize);
		answerBuffSize = realAnswerSize;

		return true;
	}
	
	return false;
}