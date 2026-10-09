//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 14:26
//      File_base        : MailManager_S
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
#ifndef _MailManager_S_h
#define _MailManager_S_h

#include "CoreRelated.h"
#include "ChatCommon.h"
#include "cfs_db_interface.h"

#define MAIL_LIVINGDATA			30		//30 days
#define MAIL_SENDERMAXMAIL		500		//
#define MAIL_RECEIVERMAXMAIL	100		//


#define	MAX_MAIL_MONEY			100000000

class MailManager_S : public MailManager
{
public:
	MailManager_S();
	void	ClearCacheInfo();	
	void	PlayerOnLine(int nPlayerIdx);
	void	PlayerOffLine(int nPlayerIdx);
	void	ProcessDBRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	int		ProcessProtocol(enMailDBOpe enOpe, int nPlayerIdx, BYTE *pData, int nDataSize);

protected:
	void	LoadMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	LoadMailListRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	DelMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	SendMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	GetOutPlusRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	GetOutMoneyRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	CloseMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	QueryNewMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);
	void	ReturnMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet);

	int		SendMailReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		LoadMailListReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		LoadMailReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		GetOutPlusReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		GetOutMoneyReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		CloseMailReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		DelMailReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		QueryNewMailReq(int nPlayerIdx, BYTE *pData, int nDataSize);
	int		ReturnMailReq(int nPlayerIdx, BYTE *pData, int nDataSize);

	int		UpdateMailReq(int nPlayerIdx, enMailDBOpe enDBOpe);
	void	MailMoneyReq(int nPlayerIdx, const char *szReceiverName, DWORD dwMoney);
	void	DelCurMail(int nPlayerIdx);

protected:
	int		MailId2Idx(DWORD dwMailId);

private:
	typedef struct _MailPlus_Info
	{
		bool	bGetOut;
		CHAT_MAILPLUS_ITEM	plusItem;
 
	} MAILPLUS_INFO, *PMAILPULS_INFO;

	typedef struct _UpdateInfo_Cache
	{
		char	curGetOutPlusIdx;
		DWORD	newPostMoney;
		DWORD	newCostMoney;

	} UPDATEINFO_CACHE, *PUPDATEINFO_CACHE;

	typedef struct _MailInfo_Cache
	{
		bool				hasChanged;
		BYTE				mailPlusCount;	
		char				mailIdx;
		DWORD				mailCost;
		UPDATEINFO_CACHE	updateInfo;
		MAILPLUS_INFO		mailPlus[MAXCOUNT_MAILPLUS];

	} MAILINFO_CACHE, *PMAILINFO_CACHE;

	typedef struct _MailListInfo_Cache
	{
		BYTE		state;
		BYTE		senderType;
		DWORD		mailCost;
		DWORD		postMoney;
		DWORD		mailId;
		char		senderName[MAXSIZE_ROLENAME];

	} MAILLISTINFO_CACHE, *PMAILLISTINFO_CACHE;

	typedef struct _SendMailInfo_Cache
	{
		char		receiverName[MAXSIZE_ROLENAME];

	} SENDMAILINFO_CACHE, *PSENDMAILINFO_CACHE;

	typedef int (MailManager_S::*PCLIENTREQPROC)(int nPlayerIdx, BYTE *pReqData, int nDataSize);
	typedef void (MailManager_S::*PDBRETPROC)(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet );

	enMailDBOpe				m_CurDBOpe;
	MAILINFO_CACHE			m_MailInfo;
	SENDMAILINFO_CACHE		m_SendMailInfo;
	MAILLISTINFO_CACHE		m_MailListInfo[MAX_MAILCOUNT_PERPLAYER];

	static	PDBRETPROC		m_DBRetPRoc[enMailDBOpe_Num];
	static	PCLIENTREQPROC	m_ClientReqProc[enMailDBOpe_Num];
};

inline void	MailManager_S::ProcessDBRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	_ASSERT(m_CurDBOpe > enMailDBOpe_None && m_CurDBOpe < enMailDBOpe_Num);

	if(m_CurDBOpe > enMailDBOpe_None && m_CurDBOpe < enMailDBOpe_Num && m_DBRetPRoc[m_CurDBOpe])
		(this->*m_DBRetPRoc[m_CurDBOpe])(nDBOpeRst, nPlayerIdx, pRet );
}

inline int MailManager_S::ProcessProtocol(enMailDBOpe enOpe, int nPlayerIdx, BYTE *pData, int nDataSize)
{
	_ASSERT(enOpe > enMailDBOpe_None && enOpe < enMailDBOpe_Num);

	if (!IsValidPlayer(nPlayerIdx))
		return chat_err_none;

	if ( Player[nPlayerIdx].GetUIServerState().GetUIState( player_ui_mail ) !=  player_ui_state_open)
		return chat_err_none;

	if(enOpe > enMailDBOpe_None && enOpe < enMailDBOpe_Num && m_ClientReqProc[enOpe])
		return (this->*m_ClientReqProc[enOpe])(nPlayerIdx, pData, nDataSize);
	else
		return chat_err_none;
}

inline int MailManager_S::MailId2Idx(DWORD dwMailId)
{
	if(INVALID_MAIL_ID == dwMailId)
		return -1;

	for(int i = 0; i < MAX_MAILCOUNT_PERPLAYER; ++i)
	{
		if(m_MailListInfo[i].mailId == dwMailId)
			return i;
	}

	return -1;
}

inline int	MailManager_S::CloseMailReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	_ASSERT(m_MailInfo.mailIdx != -1);

	m_MailInfo.mailIdx = -1;

	return chat_err_none;
}

inline void MailManager_S::PlayerOnLine(int nPlayerIdx)
{
	QueryNewMailReq(nPlayerIdx, NULL, 0);
}

inline void MailManager_S::PlayerOffLine(int nPlayerIdx)
{
	m_CurDBOpe = enMailDBOpe_None;

	ClearCacheInfo();
}

inline void MailManager_S::ClearCacheInfo()
{
	m_MailInfo.mailIdx = -1;
	
	for(int nLoop = 0; nLoop < MAX_MAILCOUNT_PERPLAYER; ++nLoop)
		m_MailListInfo[nLoop].mailId = INVALID_MAIL_ID;
}

#endif // _MailManager_S_h