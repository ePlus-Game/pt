//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:18   20:55
//      File_base        : SocialUtil
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#ifndef _SocialUtil_h
#define _SocialUtil_h

#include "SocialComDef.h"

class SocialUnit;
class SocialUnitAttr;

// None inline functions

bool	CheckPrivilege(int nPlayerIdx, int nTplId, int nLayer, int nOpeId);
void	SocialErrCode2Client(int nPlayerIdx, const SU_COMOPE_PARAM &param, int nCode);
int		CheckComOpeCond(int nPlayerIdx, int nTplId, int nLayer, int nOpeId);
void	DeductComOpeCost(int nPlayerIdx, int nTplId, int nLayer, int nOpeId);
int		CheckCreateCond(int nPlayerIdx, int nTplId, int nLayer);
void	DeductCreateCost(int nPlayerIdx, int nTplId, int nLayer);
int		SaveSubUnitsToBuf(char *pBuf, int nBufSize, SocialUnit *pUnit);
void	SetRecParentGuid(const char *szPlayerName, int nTplId, const FSGUID *pParentGuid);
int		GetCityTaxRateBuff();

void	SocialMsgToClient(int nPlayerIdx, const char *szMsg, int nMsgSize);
int		FormatMsg(char *pBuf, int nBufSize, const char *szFormat, ...);
int		FormatCreateUnitMsg(char *pBuf, int nBufSize, SocialUnit *pNewUnit, const char *szFormat);
int		FormatJoinLeaveUnitMsg(char *pBuf, int nBufSize, SocialUnit *pParent, const char *szFormat);
int		FormatSubUnitJoinLeaveMsg(char *pBuf, int nBufSize, SocialUnit *pSubUnit, const char *szFormat);
int		FormatAddRemoveUnitMsg(char *pBuf, int nBufSize, SocialUnit *pParent, const char *szFormat);
int		FormatChatPrivChgMsg(char *pBuf, 
							 int nBufSize, 
							 const char *szLauncherName,
							 SocialUnit *pChannelUnit, 
							 const char *szFormat
							 );
int     FormatChangeOwnerMsg(char *pBuf,
							 int   nBuffSize,
							 const char * szOldOwner,
							 const char * szNewOwner,
							 SocialUnit * pUnit,
							 const char * szFormat);
void	NotifyUnitBreak(int nPlayerIdx, int nTplId, int nLayer);
void	NotifyLeaveParentUnit(int nPlayerIdx, SocialUnit *pParent);
void	NotifySubUnitLeave(int nPlayerIdx, SocialUnit *pSubUnit);
void	NotifyRemovedFromParent(int nPlayerIdx, SocialUnit *pParent);
void	NotifyJoinParentUnit(int nPlayerIdx, SocialUnit *pParent);
void	NotifySubUnitJoined(int nPlayerIdx, SocialUnit *pSubUnit);
void	NotifyForbidChatMsg(int nPlayerIdx, const char *szLauncher, SocialUnit *pChannelUnit);
void	NotifyUnForbidChatMsg(int nPlayerIdx, const char *szLauncher, SocialUnit *pChannelUnit);
void	NotifyBeRemovedOffLine(int nPlayerIdx, int nTplId, int nLayer, const char *szFormat);
void	NotifyDeclareWarSucceed(int nPlayerIdx);
void	NotifyWarInfo(int nPlayerIdx);
void	NotifyRobRes(const char *szInvaderTongName, int nMapId, int nResType, int nValue);
void	NotifyRobCity(const char *szInvaderTongName, int nMapId);
void	NotifyCityDiscount(int nCityLordIdx);
void	NotifyMemberOnline(const char *szMemberName, SocialUnit *pParentUnit, bool bOnline);
void    NotifyCityLeagueBreak(const char* pUnitName, int nMapId);

void	UpdateLeafUnitPriv(int nNetId, SocialUnit *pUnit);
bool    CheckLeafUnitPrivVerion(SocialUnit * pUnit);
void	UpdateDBOnUnitJoin(int nNetId, SocialUnit *pParent, SocialUnit *pSubUnit);
void	UpdateDBOnUnitLeave(int nNetId, SocialUnit *pParent, SocialUnit *pSubUnit);
void	UpdateDBOnCreateUnit(int nNetId, SocialUnit *pNewUnit);

DWORD		GetChatRoomId(const SocialUnitAttr &attr);
const char*	GetUnitName(const SocialUnitAttr &attr);
int			GetCityMapId(const SocialUnitAttr &attr);
int         GetPoolMapId(const SocialUnitAttr &attr);
int         GetForceSubNum(const SocialUnitAttr & attr);


const char*		GetLayerName(int nTplId, int nLayer);

SocialUnit*		GetUpNUnit(SocialUnit *pUnit, int nLayer);
SocialUnit*		GetTopUnit(SocialUnit *pUnit);
SocialUnit*		GetLeafUnit(int nPlayerIdx, int nTplId);
const FSGUID*	GetUnitGuid(int nPlayerIdx, int nTplId, int nLayer);

bool	IsUnitOwner(int nPlayerIdx, int nTplId, int nLayer);



bool AddBuffToUnit(SocialUnit *pUnit, int nBuffId);
void RefreshPlayerInfoAttr(int nPlayerIdx, bool bOnline);
void RefreshPlayerInfoAttr(SocialUnitAttr &attr, int nPlayerIdx, bool bOnline);

DWORD GetSocialInstanceId(SocialUnit *pUnit, int worldTemplateId);
bool SetSocialInstanceId(SocialUnit *pUnit, int worldTemplateId, DWORD instanceId);


bool SetSaveScriptData(SocialUnit* pUnit, int nIndex, unsigned long dwData);
bool GetSaveScriptData(SocialUnit* pUnit, int nIndex, unsigned long* pData);

int			GetLordByMapID(int nMapID);
SocialUnit* GetLordSocialUnitByMapID(int nMapID);

inline bool IsAttrIdValid(int nAttrId)
{
	return nAttrId > enSUAttr_None && nAttrId < enSUAttr_Num;
}

inline bool IsOpeIdValid(int nOpeId)
{
	return nOpeId > enSUO_None && nOpeId < enSUO_Num;
}

inline	bool IsTplIdValid(int nTplId)
{
	return enSUTplId_Team == nTplId || enSUTplId_Tong == nTplId;
}

inline bool	IsLayerValid(int nTplId, int nLayer)
{
	if( !IsTplIdValid(nTplId) )
		return false;
	
	if(enSUTplId_Team == nTplId)
		return nLayer > enSULayer_None && nLayer < enSUTeam_LayerNum;
	else if(enSUTplId_Tong == nTplId)
		return nLayer > enSULayer_None && nLayer < enSUTong_LayerNum;

	return false;
}

inline bool IsGUIDValid(const FSGUID &guid)
{
	return guid.data[0] != '\0';
}

inline void NotifyLeaveParentUnit(int nPlayerIdx, SocialUnit *pParent)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatJoinLeaveUnitMsg(szMsg,
										   sizeof(szMsg),
										   pParent,
										   MSG_SOCIAL_LEAVE_UNIT
										  );
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void	NotifySubUnitLeave(int nPlayerIdx, SocialUnit *pSubUnit)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatSubUnitJoinLeaveMsg(szMsg,
											  sizeof(szMsg),
											  pSubUnit,
											  MSG_SOCIAL_SUBUNIT_LEAVE
											 );
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void	NotifyRemovedFromParent(int nPlayerIdx, SocialUnit *pParent)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatAddRemoveUnitMsg(szMsg,
										   sizeof(szMsg),
										   pParent,
										   MSG_SOCIAL_REMOVE_UNIT
										  );
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void NotifyJoinParentUnit(int nPlayerIdx, SocialUnit *pParent)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatJoinLeaveUnitMsg(szMsg,
										   sizeof(szMsg),
										   pParent,
										   MSG_SOCIAL_JOIN_UNIT
										  );
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void NotifySubUnitJoined(int nPlayerIdx, SocialUnit *pSubUnit)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatSubUnitJoinLeaveMsg(szMsg,
											  sizeof(szMsg),
											  pSubUnit,
											  MSG_SOCIAL_SUBUNIT_JOIN
											 );
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void NotifyForbidChatMsg(int nPlayerIdx, const char* szLauncher, SocialUnit *pChannelUnit)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatChatPrivChgMsg(szMsg,
										 sizeof(szMsg),
										 szLauncher,
										 pChannelUnit,
										 MSG_SOCIAL_FORBID_CHAT
										);
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void NotifyUnForbidChatMsg(int nPlayerIdx, const char *szLauncher, SocialUnit *pChannelUnit)
{
	char	szMsg[MAXSIZE_HINT_MSG];
	int		nSize = FormatChatPrivChgMsg(szMsg,
										 sizeof(szMsg),
										 szLauncher,
										 pChannelUnit,
										 MSG_SOCIAL_UNFORBIG_CHAT
										);
	SocialMsgToClient(nPlayerIdx, szMsg, nSize);
}

inline void UpdateDBOnUnitLeave(int nNetId, SocialUnit *pParent, SocialUnit *pSubUnit)
{
	UpdateDBOnUnitJoin(nNetId, pParent, pSubUnit);
}

inline void NotifyUnitBreak(int nPlayerIdx, int nTplId, int nLayerId)
{	
	NotifyBeRemovedOffLine(nPlayerIdx, nTplId, nLayerId, MSG_SOCIAL_UNIT_BREAK);
}

inline int FormatCreateUnitMsg(char *pBuf, int nBufSize, SocialUnit *pNewUnit, const char *szFormat)
{
	return FormatJoinLeaveUnitMsg(pBuf, nBufSize, pNewUnit, szFormat);
}

#endif