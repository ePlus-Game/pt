//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-26 12:07
//      File_base        : CoreRelated
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : Code related with existing core is place here. 
//						   Convenient for transplant chat module.
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#ifndef _CoreRelated_h
#define	_CoreRelated_h

#include "KCore.h"
#include "ChatDataDef.h"
#include "KPlayer.h"
#include "KGMCommand.h"
#include "cfs_fs2_savedef.h"
#include "ItemCommonDef.h"
#include "GameDataDef.h"
#include "relation_set.h"
#include "relation_template.h"
#include "SocialComDef.h"
#include "FilterText.h"
#include "buff_man.h"

using namespace CHAT;

inline const char*	GetPlayerName(int nPlayerIdx)
{
	return Npc[Player[nPlayerIdx].m_nIndex].Name;
}

inline int GetPlayerSeries(int nPlayerIdx)
{
	return Npc[Player[nPlayerIdx].m_nIndex].m_Series;
}

inline bool IsItemIdxValid(int nItemIdx)
{
	return nItemIdx > 0 && nItemIdx < MAX_ITEM;
}

inline bool ItemCanTrade(int nPlayerIdx, DWORD dwItemId)
{
	int nItemIdx = Player[nPlayerIdx].m_ItemList.SearchID(dwItemId);

	if( IsItemIdxValid(nItemIdx) )
		return (!Item[nItemIdx].IsLocked(Player[nPlayerIdx].GetNetConnectIdx()) && Item[nItemIdx].CanExchange() && !Item[nItemIdx].IsBind());
	else
		return false;
}

inline BOOL HasItemInEquipment(int nPlayerIdx, DWORD dwItemId)
{
	return Player[nPlayerIdx].m_ItemList.HaveNormalItemByID(dwItemId);
}

inline int GetTotalMoney(int nPlayerIdx)
{
	return Player[nPlayerIdx].m_ItemList.GetEquipmentMoney();
}

inline RelationTemplate* GetRelationTemplate(int nTplId)
{
	return RTM::Singleton().GetTemplate(nTplId);
}

inline RelationLayer* GetRelationLayer(int nTplId, int nLayer)
{
	RelationTemplate	*pTemplate = RTM::Singleton().GetTemplate(nTplId);
	return pTemplate ? pTemplate->GetLayer(nLayer) : NULL;
}

inline int GetJoinBuff(int nTplId, int nLayer)
{
	RelationLayer *pLayer = GetRelationLayer(nTplId, nLayer);
	return (NULL != pLayer) ? pLayer->JoinBuff : INVALID_BUFF_ID;
}

inline int GetLeaveBuff(int nTplId, int nLayer)
{
	RelationLayer *pLayer = GetRelationLayer(nTplId, nLayer);
	return (NULL != pLayer) ? pLayer->LeaveBuff : INVALID_BUFF_ID;
}

inline int GetBuffMustNotHaveOnJoin(int nTplId, int nLayer)
{
	RelationLayer *pLayer = GetRelationLayer(nTplId, nLayer);
	return (NULL != pLayer) ? pLayer->BuffMustNotHaveOnJoin : INVALID_BUFF_ID;
}

inline bool IsSocialTmplSave(int nTplId)
{
	RelationTemplate *pTmpl = GetRelationTemplate(nTplId);

	if(pTmpl)
		return pTmpl->IsTmplSave();
	else
		return false;
}

inline int	GetMinSubUnitCnt(int nTplId, int nLayer)
{
	RelationLayer *pLayer = GetRelationLayer(nTplId, nLayer);
	return pLayer ? pLayer->MinChildCount : 0;
}

inline int	GetMaxSubUnitCnt(int nTplId, int nLayer)
{
	RelationLayer *pLayer = GetRelationLayer(nTplId, nLayer);
	return pLayer ? pLayer->MaxChildCount : 0;
}

bool	LoadTextFilterExp(const char *szFile);
bool    LoadChatTxtFilterExp(const char * szFile);

#ifndef _SERVER
bool    LoadUserChatFilterExp(const char * szFile);
bool    LoadChatRecvFilterExp(const char * szFile);
#endif

inline	BOOL g_IsTextPass(const char *szText)
{
	return g_TextFilter.IsTextPass(szText);
}

inline  BOOL g_IsChatPass(const char * szText)
{
	return g_ChatTxtFilter.IsTextPass(szText);
}

#ifndef _SERVER
inline BOOL  g_FilterChatText(const char * szText,const char cReplace)
{
	return g_ChatTxtFilter.ReplaceInvalidText(szText,cReplace);
}

inline BOOL  g_FilterUserChatText(const char * szText,const char cReplace)
{
	return g_UserChatFilter.ReplaceInvalidText(szText,cReplace);
}

inline BOOL  g_FilterChatRecvText(const char * szText,const char cReplace)
{
	return g_ChatRecvFilter.ReplaceInvalidText(szText,cReplace);
}
#endif

inline BOOL g_IsNamePass(const char *szName)
{
	return IsRule(szName) && g_IsTextPass(szName);	
}
// 

#ifdef _SERVER
void DumpInvalidItemOpeStack(const bool bDumpStack,const char * szExtraComment,const int nLayer);
void ChatErrCodeToClient(int nPlayerIdx, int nErrCode);
void NewMailNotify(int nPlayerIdx, int nMailCnt, const char *szSenderName);
void ChatOpeNotify(int nPlayerIdx, BYTE subProtocol, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
void ForEachLocalPlayer(int	nSayerNpcIdx,
						CHATCALLBACK pCallBack,  
						const void *pCallbackParam, 
						BROADCASTFILTER pFilter = NULL, 
						unsigned int uFilterPassby = 0
						);

bool SystemSendCustomMail(const char * szReceiver,
						  const char * szTitle,
						  const char * szContent,
						  const char * szSender);

bool SystemSendMail(const char *szReceiver, 
	const char *szTitle,
	const char *szContent,
	DWORD dwPostMoney,
	DWORD dwMailCost,
	DWORD dwPlusCount,
	Item_Identifier *pPlusData
    );

bool SysSendMailToAll(const char *szTitle,
	const char *szContent,
	DWORD dwPostMoney,
	DWORD dwMailCost,
	DWORD dwPlusCount,
	Item_Identifier *pPlusData,
	int requireLevel
	);

inline void	SendDataToClient(int nPlayerIdx, void *pData, int nLen)
{
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, pData, nLen);
}

inline void DecMoney(int nPlayerIdx, int nMoney, bool statisticFlag = false)
{
	Player[nPlayerIdx].Pay(nMoney, statisticFlag);
}

inline void AddMoney(int nPlayerIdx, int nMoney, bool statisticFlag = false)
{
	Player[nPlayerIdx].Earn(nMoney, statisticFlag);
}

inline int GetNetConnectIdx(int nPlayerIdx)
{
	return Player[nPlayerIdx].GetNetConnectIdx();
}

inline void	GetItemTransData(int nPlayerIdx, TItemtransfersData &data, DWORD dwItemId)
{
	Player[nPlayerIdx].GetItemTransData(data, dwItemId);
}

inline int GetPlayerLevel(int nPlayerIdx)
{
	return Player[nPlayerIdx].GetLevel();
}

inline int GetItemReqLevel(int nPlayerIdx, DWORD dwItemId)
{
	int nItemIdx = Player[nPlayerIdx].m_ItemList.SearchID(dwItemId);
	
	if( IsItemIdxValid(nItemIdx) )
		return Item[nItemIdx].GetLevelRequirement();
	else
		return 0;
}

inline int GetQualityLabel(int nPlayerIdx, DWORD dwItemId)
{
	int nItemIdx = Player[nPlayerIdx].m_ItemList.SearchID(dwItemId);

	if( IsItemIdxValid(nItemIdx) )
		return Item[nItemIdx].GetQualityLabel();
	else
		return 0;
}

inline const FSGUID& GetPlayerGuid(int nPlayerIdx)
{
	return Player[nPlayerIdx].GetGUID();
}

inline void GetItemGuid(int nPlayerIdx, DWORD dwItemId, FSGUID &guid)
{
	Player[nPlayerIdx].GetItemGuid(guid, dwItemId);	
}

inline void GetItemName(int nPlayerIdx, char *pName, int nBufSize, DWORD dwItemId)
{
	Player[nPlayerIdx].GetItemName(pName, nBufSize, dwItemId);
}


inline void RemoveItem(int nPlayerIdx, DWORD dwItemId)
{
	int nItemIdx = Player[nPlayerIdx].m_ItemList.SearchID(dwItemId);
	Player[nPlayerIdx].m_ItemList.Remove(nItemIdx);
	ItemSet.Remove(nItemIdx);
}

inline int AddItem(int nPlayerIdx, const TItemtransfersData *pItemData)
{
	return Player[nPlayerIdx].AddItem(pItemData);
}

inline bool IsPlayerIdxValid(int nPlayerIdx)
{
	return nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER;
}

inline void AddBuffToPlayer(int nPlayerIdx, int nBuffId)
{
	if(INVALID_BUFF_ID != nBuffId)
	{
		int nNpcIdx = Player[nPlayerIdx].m_nIndex;
		BuffMgr::Singleton().AddNpcBuff(nNpcIdx, nNpcIdx, nBuffId);
	}
}

// 队伍相关
inline int GetTeamId(int nPlayerIdx)
{
	if (IsValidPlayer(nPlayerIdx) && Player[nPlayerIdx].GetTeamInfo().IsInTeam())	
		return Player[nPlayerIdx].GetTeamInfo().GetTeamId();
	else
		return INVALID_TEAM_ID;
}

int	 GetTeamMember(int nTeamId, int *pOutRst, int nCapacity);

inline bool IsTeamCaptain(int nTeamdId, int nPlayerIdx)
{
	if(INVALID_TEAM_ID != nTeamdId)
	{
		if(g_TeamS[nTeamdId].GetCaptain() == nPlayerIdx)
			return true;
	}

	return false;
}

//------------------------------- Declaration of class PlayerInfoToIndex --------------------------------

class PlayerInfoToIndex
{
private:
	typedef	map<string, int>	NAMETOINDEXCONT;
	NAMETOINDEXCONT		m_NameToIndexCont;

public:
	class Iterator
	{
	public:
		friend class PlayerInfoToIndex;

		Iterator(PlayerInfoToIndex &owner)
		{
			iter = owner.m_NameToIndexCont.begin();
		}

	private:
		NAMETOINDEXCONT::iterator iter;
	};

	friend class Iterator;

public:
	int		GetIndexByName(const string &strName);
	void	PlayerOnLine(int nPlayerIndex);
	void	PlayerOffLine(int nPlayerIndex);
	void	ForEach(CHATCALLBACK pCallBack,  const void *pCallbackParam,
				BROADCASTFILTER pFilter = NULL, unsigned int uFilterPassby = 0);

	int		NextPlayerIdx(Iterator &iter);
	bool	IsPlayerOnline(const string &strName);
};

inline	int PlayerInfoToIndex::GetIndexByName(const string &strName)
{
	NAMETOINDEXCONT::iterator it = m_NameToIndexCont.find(strName);

	return it != m_NameToIndexCont.end() ? it->second : INVALID_PLAYER_INDEX;
}

inline void PlayerInfoToIndex::PlayerOnLine(int nPlayerIdx)
{
	if(nPlayerIdx > 0 && nPlayerIdx < CHAT_MAX_PLAYER)
	{
		m_NameToIndexCont[GetPlayerName(nPlayerIdx)] = nPlayerIdx;
	}
}

inline void PlayerInfoToIndex::PlayerOffLine(int nPlayerIdx)
{
	if(nPlayerIdx > 0 && nPlayerIdx < CHAT_MAX_PLAYER)
	{
		m_NameToIndexCont.erase( GetPlayerName(nPlayerIdx) );
	}
}

inline bool	PlayerInfoToIndex::IsPlayerOnline(const string &strName)
{
	return INVALID_PLAYER_INDEX != GetIndexByName(strName);
}

extern PlayerInfoToIndex	g_PlayerInfoToIndex;

// 社会关系相关
inline RelationSet& GetRelationSet(int nPlayerIdx)
{
	return Player[nPlayerIdx].GetRelationSet();
}

int PrepareShowBannerBuff(char* pBuff,
						  int buffSize,
						  const char* pMsg,
						  int msgSize,
						  const char* pFont,
						  int fontSize,
						  int color,
						  int param1,
						  int param2,
						  int type);

int GlobalAddItemToPlayer(
	int playerIndex,
	int itemGenre,
	int itemDetail,
	int itemParticular,
	int itemLevel,
	int itemCount,
	int& actualAddCount,
	ItemCountType statisticType, 
	enumLogEvent logEvent);

#endif // #ifdef _SERVER

#ifndef _SERVER

#include "CoreShell.h"

inline void ShowChatErrorMsg(const char *szErrMsg)
{
	CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szErrMsg, 0);
}

inline void	SendDataToServer(void *pData, int nLen)
{
	if ( g_pClient )
	{
		g_pClient->SendPackToServer(g_ConnectID, pData, nLen);
	}
}

//错误消息（显示在屏幕中上位置）
inline void ShowErrorMessage(const char* msg)
{
	if (msg != NULL)
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)msg, 0);
}

//系统消息（显示在系统消息频道）
inline void ShowSystemMessage(const char* msg)
{
	if (msg != NULL)
		CoreDataChanged(GDCNI_APPEND_MESSAGE, (unsigned int)msg, SYSTEM_ROOM_ID);
}

const char* GetItemName(int nItemClass,int nDetailType,int nParticualrType,int nLevel);
void ShowMsgInSysRoom(const char *szMsg);

#endif // #ifndef _SERVER

//转化为小写
inline void strtolower( char* pszStr )
{
	int nPos = 0;
	int c;
	
	while ( ( c = pszStr[nPos] ) ) 
	{
		if( isupper( c ) )
			pszStr[nPos] = tolower( c );
		
		nPos++;
	}
}

//是否含有SQL关键字
bool HasSqlKeyWord( const char* szStr, int strBuffLen );

bool CompressProtocol(BYTE* protocolBuff, unsigned int protocolLength, unsigned int protocolBuffLength, unsigned int headerLength = 3);
bool DecompressProtocol(const BYTE* compressedProtocolBuff, BYTE* decompressBuff, unsigned int decompressBuffLength, unsigned int headerLength = 3);

#ifdef _SERVER

//系统参数数值类型
enum enumSystemVarValueType
{
	system_var_value_type_int = 0,
	system_var_value_type_string,
	system_var_value_type_blob,

	system_var_value_type_count
};

//系统参数
enum enumSystemVar
{
	//问答相关
	system_var_question_enabled = 0,
	system_var_question_pool_size,
	system_var_question_refresh_interval,
	system_var_question_compress,
	system_var_question_template,
	system_var_question_keep_time,
	system_var_question_timeout,
	system_var_question_forbid_time,
	system_var_question_char_count_min,
	system_var_question_char_count_max,
	system_var_question_font_size_min,
	system_var_question_font_size_max,
	system_var_question_overlap_min,
	system_var_question_overlap_max,
	system_var_question_plus_percent,
	system_var_question_angle_min,
	system_var_question_angle_max,
	system_var_question_xtrans_min,
	system_var_question_xtrans_max,
	system_var_question_ytrans_min,
	system_var_question_ytrans_max,
	system_var_question_xscale_min,
	system_var_question_xscale_max,
	system_var_question_yscale_min,
	system_var_question_yscale_max,
	system_var_question_image_height,
	system_var_question_noise_scale_min,
	system_var_question_noise_scale_max,
	system_var_question_noise_char_min,
	system_var_question_noise_char_max,
	system_var_question_bad_answer_clear_interval,
	system_var_question_bad_answer_max_count,
	system_var_question_long_term_bad_answer_clear_interval,
	system_var_question_long_term_bad_answer_max_count,
	system_var_question_bad_answer_stage1_keep_time,
	system_var_question_bad_answer_stage2_keep_time,

	system_var_count
};

struct _SystemVarHeader : _DBProcHeader 
{
	enumSystemVar SystemVar;
	enumSystemVarValueType ValueType;
};

void GetSystemVar(enumSystemVar systemVar, enumSystemVarValueType valueType);
void ProcessGetSystemVar(IProcRet* pRet, _SystemVarHeader* pHeader);

#endif

#ifndef _SERVER
bool ParseQuestionProtocol( BYTE* pMsg, UIQuestionData& uiQuestionData );
#endif

#ifndef _SERVER
bool PrintReadableNumber(char* outputBuff, size_t outputBuffSize, DWORD number, int stepSize = 3);
#endif

#endif
