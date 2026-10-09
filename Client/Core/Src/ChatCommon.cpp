//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 15:31
//      File_base        : ChatCommon
//      File_ext         : cpp
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
#include "KCore.h"
#include "ChatCommon.h"
#include "ChatDataDef.h"
#include "CoreRelated.h"

using namespace CHAT;

int MailManager::CheckMail(int nPlayerIdx, const char *szTitle, DWORD dwPostMoney, int nContentSize, 
			const char *pContent, int nItemCount, const DWORD *pItemIds, int nTax)
{
	// reserve one byte for '\0'
	if(nContentSize > MAXSIZE_MAILTEXT - 1)
	{
		return chat_err_contentlenexceed;
	}

	if(nItemCount > MAXCOUNT_MAILPLUS || nItemCount < 0)
	{
		return chat_err_accessorycountexceed;
	}

	if( GetTotalMoney(nPlayerIdx) < dwPostMoney || 
		GetTotalMoney(nPlayerIdx) < (dwPostMoney + nTax) )
	{
		return char_err_moneynotenough;
	}

	for(int i = 0; i < nItemCount; ++i)
	{
		DWORD itemId = pItemIds[i];
		if( HasItemInEquipment(nPlayerIdx, itemId) <= 0 )
		{
			return chat_err_invaliditeminmail;
		}
		else
		{
			if( !ItemCanTrade(nPlayerIdx, itemId) )
				return chat_err_invaliditeminmail;
		}
	}

	//int nMailSize = sizeof(DBTASK_SENDMAILS_REQ) + nContentSize + nItemCount * sizeof(CHAT_MAILPLUS_ITEM);

	int nMailSize = nContentSize + nItemCount * sizeof(CHAT_MAILPLUS_ITEM);

	if(nMailSize > MAXSIZE_MAIL)
	{
		return chat_err_maillengthexceed;
	}

	return chat_err_none;	
}

//----------------------- definitions of class PlayerStatusChangeList ---------------------------

PlayerStatusChangeList	g_PlayerStatusChgList;

void PlayerStatusChangeList::Active()
{
	int nListSize = m_StatusList.size();

	for(int nLoop = 0; nLoop < nListSize; ++nLoop)
	{
		if("" != m_StatusList[nLoop].playerName)
		{
			--m_StatusList[nLoop].leftTime;

			if(m_StatusList[nLoop].leftTime <= 0)
				m_StatusList[nLoop].playerName = "";
		}
	}
}

PlayerStatusChangeList::ListEntry& PlayerStatusChangeList::GetEmptyEntry()
{
	int nListSize = m_StatusList.size();

	for(int nLoop = 0; nLoop < nListSize; ++nLoop)
	{
		if ("" == m_StatusList[nLoop].playerName)
			return m_StatusList[nLoop];
	}

	ListEntry entry;
	entry.playerName = "";
	m_StatusList.push_back(entry);

	return m_StatusList.back();
}

void PlayerStatusChangeList::AddEntry(int nPlayerIdx, enOperation ope)
{
	ListEntry &entry = GetEmptyEntry();

	entry.playerName = GetPlayerName(nPlayerIdx);
	entry.leftTime = __default_life_time;
	entry.operation = ope;
}

bool PlayerStatusChangeList::NextEntry(Iterator &iter, string &strPlayerName, int &nOpe)
{
	int nListSize = m_StatusList.size();

	for(int nLoop = iter.m_Index; nLoop < nListSize; ++nLoop)
	{
		++iter.m_Index;

		if( 0 != m_StatusList[nLoop].playerName.c_str()[0])
		{
			strPlayerName = m_StatusList[nLoop].playerName;
			nOpe = m_StatusList[nLoop].operation;
			return true;
		}
	}

	return false;
}