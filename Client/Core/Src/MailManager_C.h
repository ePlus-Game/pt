//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 14:24
//      File_base        : MailManager_C
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
#ifndef _MailManager_C_h
#define _MailManager_C_h

#include "ChatCommon.h"
#include "ChatDataDef.h"
#include "ConfigManager.h"

using namespace CHAT;

class MailManager_C : MailManager
{
public:
	void	Init();
	
	int		SendMail(const MAIL_PARAM *pMailParam);

	int		LoadMailListReq(int nPage);
	int		LoadMailReq(DWORD dwMailId);
	int		GetOutItemReq(DWORD dwMailId, int nIndex);
	int		GetOutMoneyReq(DWORD dwMailId);
	int		CloseMailReq(DWORD dwMailId);
	int		DelMailReq(DWORD dwMailId);
	int		ReturnMailReq(DWORD dwMailId);

	void	LoadMailListRet(BYTE *pData);
	void	DelMailRet(DWORD dwMailId);
	void	LoadMailRet(BYTE *pData);
	void	GetOutPlusRet(BYTE *pData);
	void	GetOutMoneyRet(BYTE *pData);

	BYTE*	GetMailData(DWORD dwMailId);
	int		GetNewMailCount();

	inline int	GetSendTextTax() {return m_SendTextTax;}
	inline int	GetSendItemTax() {return m_SendItemTax;}

private:
	bool	CheckPreTime();

private:
	typedef map<DWORD, CHAT_MAILDATA_CLIENT>	MAILCONT;

	MAILCONT	m_MailCont;
	int			m_SendTextTax;
	int			m_SendItemTax;

	int         m_TotalMailCont;

	DWORD		m_preOperationTime;
};

inline void MailManager_C::DelMailRet(DWORD dwMailId)
{
	m_MailCont.erase(dwMailId);
}

inline void MailManager_C::Init()
{
	m_MailCont.clear();
	m_preOperationTime = 0;
	m_SendTextTax = ConfigManager::Singleton().GetGlobalVariable(global_var_sendmail_text_tax);
	m_SendItemTax = ConfigManager::Singleton().GetGlobalVariable(global_var_sendmail_item_tax);
	m_TotalMailCont = 0;
}

inline bool	MailManager_C::CheckPreTime()
{
	DWORD curTime = ::GetTickCount();
	if (curTime - m_preOperationTime < 300)
		return false;
	else
		m_preOperationTime = curTime;

	return true;
}

#endif