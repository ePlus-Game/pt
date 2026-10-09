//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:11   9:39
//      File_base        : SocialUnit
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

#ifndef _SocialUnit_h
#define _SocialUnit_h

#include <vector>
#include "SocialComDef.h"
#include "privilege_set.h"
#include "SocialUtil.h"
#include "SocialUnitAttr.h"

using std::vector;

class SocialUnit
{
public:
	class UnitIterator
	{
	public:
		friend class SocialUnit;
		UnitIterator() : nIdx(0) {};
	private:
		int	nIdx;
	};

public:
	SocialUnit(int nTplId, int nLayer);
		
	int			ProcessOperation(int nLauncherIdx, void *pParam, int nMsgSize);
	void		SetParent(SocialUnit *pParent);
	int			GetTplId() const;
	int			GetLayer() const;
	int			GetChildCount() const;
	bool		IsOwner(const char *szPlayerName);
	void		PlayerOnLine(int nPlayerIdx, bool bOnline);
	int			SaveUnitToBuf(char *pBuf, int nBufSize);
	bool		IsAllSubUnitLoad() const;

	void		SetOwnerName(const char *szName);
	void		SetUnitGuid(const FSGUID &guid);
	void		SetSubUnitCnt(int nCnt);
	void		ChgLeafUnitMaxJoinedLayer(int nMaxLayer);

	void		AddSubUnit(SocialUnit *pUnit);
	void		SetAllSubUnitLoadFlag(bool bFlag);
	bool		CreateChannel(int nPlayerIdx);
	const char*	GetOwnerName() const;
		
	PrivilegeSet&		GetPrivilegeSet();
	SocialUnitAttr&		GetUnitAttr();
	SocialUnit*			GetParent() const;
	const FSGUID&		GetUnitGuid() const;
	SocialUnit*			NextSubUnit(UnitIterator &iter);
	void                RecountSubUnitNum(void);
	int                 GetTotalPlayerNum(void);
	int                 GetOnlinePlayerNum(void);
	int                 GetPlayerAvgLevel(void);
	bool                SetScriptUseData(const unsigned long dwNum,const int nIndex);
	bool                GetScriptUseData(const int nIndex,unsigned long & nValueRet);
	bool				SendInvitationToSubPlayer(const char * szTitle, const char * szConstant, const char * szSender, const char * coupleName);

private:
	// Initialize functoins
	void	BindOpe2Proc();
	void	InitProcAccess(int nTplId, int nLayer);

	// Functions process client request
	int		CreateUnit(int nLauncherIdx, void *pParam, int nMsgSize);
	int		AddSubUnit(int nLauncherIdx, void *pParam, int nMsgSize);		
	int		RemoveSubUnit(int nLauncherIdx, void *pParam, int nMsgSize);
	int		ForbidChatInUnit(int nLauncherIdx, void *pParam, int nMsgSize);
	int		UnForbidChatInUnit(int nLauncherIdx, void *pParam, int nMsgSize);
	int		BreakUnit(int nLauncherIdx, void *pParam, int nMsgSize);
	int		LeaveUnit(int nLauncherIdx, void *pParam, int nMsgSize);
	int		PubAnnouncement(int nLauncherIdx, void *pParam, int nMsgSize);
	int		GetSubList(int nLauncherIdx, void *pParam, int nMsgSize);
	int		GetAnnouncement(int nLuancherIdx, void *pParam, int nMsgSize);
	int     ChangeUnitOwner(int nLuancherIdx, void *pParam, int nMsgSize);
	int     ReqJoinHighLevel(int nLuancherIdx, void *pParam, int nMsgSize);
	int     ReqRecruitInfo(int nLuancherIdx,void * pParam,int nMsgSize);
	int     AddRecruitInfo(int nLuancherIdx,void * pParam,int nMsgSize);
	int     DelRecruitInfo(int nLuancherIdx,void * pParam,int nMsgSize);

	// Assistant functions
	void    SendMailToSubPlayer(const char * szTitle,const char * szConstant);
	void	RemoveSubUnit(SocialUnit *pUnit);
	void	BreakUnitPassive(int nPlayerIdx);
	bool	IsLeaf() const;
	void	ClearLeafUnitPrivilege(int nLayer);
	void	AddLeafUnitPrivilege(int nLayer);
	void	SyncLeafUnitPrivilege();
	void	OnJoinParentUnit(SocialUnit *pParent);
	void	OnSubUnitJoin(SocialUnit *pSubUnit);
	void	OnLeaveParentUnit(SocialUnit *pParent);
	void	OnSubUnitLeave(SocialUnit *pSubUnit);
	void	ChatPrivilegeOpe(SocialUnit *pChannelUnit, bool bForbid);
	void	ChatChannelOpe(SocialUnit *pChannelUnit, bool bAddChannel);
	int		GetSubListInfo(int nStartOffset, char *pOutBuf, int &nLeftSize);
	int		GetUnitTransferInfo(char *pOutBuf, int nBufSize);
	int		OnJoinRoom(int nPlayerIdx, DWORD dwRoomId);
	void	OnLeaveRoom(int nPlayerIdx, DWORD dwRoomId);
	void	GetUnitPlayerCount(int &nTotalCount, int &nTotalOnlineCount);
	void    GetUnitPlayerLevelAndCount(int & nCount,int & nLevel);
	void    CheckDelayAddLeaveBuff(const char * szOwnerName,const int nLeaveLayer);
	bool    IsHasRefuseBuff(int nTargetPlayerIndex, int nLuncherIndex);
	
private:
	SocialUnit(const SocialUnit &rhs);
	SocialUnit& operator= (const SocialUnit &rhs);

	typedef	int	(SocialUnit::*POPEFUNC)(int nPlayerIdx, void *pParam, int nMsgSize);
	
private:
	static	POPEFUNC		m_opeProcs[enSUO_Num];
	bool					m_IsProcExist[enSUO_Num];
	char					m_ownerName[MAXSIZE_ROLENAME];
	FSGUID					m_unitGuid;
	
	BYTE					m_tplId;
	BYTE					m_layer;
	BYTE					m_maxJoinedLayer;		// 仅对叶节点有效
	char					m_subUnitCnt;
	bool					m_isAllSubUnitLoad;
	
	SocialUnit*				m_pParent;
	vector<SocialUnit*>		m_subUnits;
	PrivilegeSet			m_privSet;
	SocialUnitAttr			m_attrs;

	int                     m_AnuncmentVersion;

	unsigned long           m_ScriptDatas[MAX_SOCIAL_SCRIPT_DATA_NUM_NONEEDSAVE];
};

inline int SocialUnit::GetTplId() const
{
	return m_tplId;
}

inline int SocialUnit::GetLayer() const
{
	return m_layer;
}

inline SocialUnit* SocialUnit::GetParent() const
{
	return m_pParent;
}

inline void SocialUnit::SetParent(SocialUnit *pParent)
{
	m_pParent = pParent;
}

inline bool SocialUnit::IsLeaf() const
{
	//Notce : m_subUnits.size == 0 不一定是Leaf 因为在RemoveSubUnit 时候没有讲节点对应的容器位置去掉.
	return (0 == m_subUnits.size() && (m_tplId!=enSUTplId_Tong || (m_tplId==enSUTplId_Tong && m_layer==enSULayer_Player) ));
}

inline bool SocialUnit::IsOwner(const char *szPlayerName)
{
	return szPlayerName ? !strcmp(m_ownerName, szPlayerName) : false;
}

inline void SocialUnit::OnSubUnitJoin(SocialUnit *pSubUnit)
{
	AddSubUnit(pSubUnit);
	++m_subUnitCnt;
}

inline void SocialUnit::OnSubUnitLeave(SocialUnit *pSubUnit)
{
	RemoveSubUnit(pSubUnit);
	--m_subUnitCnt;
}

inline PrivilegeSet& SocialUnit::GetPrivilegeSet()
{
	return m_privSet;
}

inline SocialUnitAttr&	SocialUnit::GetUnitAttr()
{
	return m_attrs;
}	

inline const FSGUID& SocialUnit::GetUnitGuid() const
{
	return m_unitGuid;
}

inline SocialUnit*	SocialUnit::NextSubUnit(UnitIterator &iter)
{
	int	nSize = (int)m_subUnits.size();

	for(int nLoop = iter.nIdx; nLoop < nSize; ++nLoop)
	{
		if(m_subUnits[nLoop])
		{
			iter.nIdx = nLoop + 1;
			return m_subUnits[nLoop];
		}
	}

	return NULL;
}

inline void	SocialUnit::SetOwnerName(const char *szName)
{
	if(szName)
	{
		strncpy(m_ownerName, szName, sizeof(m_ownerName));
		m_ownerName[sizeof(m_ownerName) - 1] = 0;
	}//endif
}

inline void SocialUnit::SetUnitGuid(const FSGUID &guid)
{
	m_unitGuid = guid;
}

inline void	SocialUnit::SetAllSubUnitLoadFlag(bool bFlag)
{
	m_isAllSubUnitLoad = bFlag;
}

inline const char*	SocialUnit::GetOwnerName() const
{
	return m_ownerName;
}

inline void	SocialUnit::SetSubUnitCnt(int nCnt)
{
	_ASSERT(nCnt > 0);

	if(nCnt > 0)
		m_subUnitCnt = (char)nCnt;
}

inline int	SocialUnit::GetChildCount() const
{
	return m_subUnitCnt;
}

inline bool	SocialUnit::IsAllSubUnitLoad() const
{
	return m_isAllSubUnitLoad;
}

#endif