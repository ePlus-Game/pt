//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 14:58
//      File_base        : ChatCommon
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
#ifndef _ChatCommon_h
#define _ChatCommon_h

#include "GlobalDef.h"
#include <vector>
#include <string>

using std::vector;
using namespace std;

class MailManager
{
protected:
	MailManager() {};
	~MailManager() {};

	int CheckMail(int nPlayerIdx, const char *szTitle, DWORD dwPostMoney, int nContentSize, 
				const char *pContent, int nItemCount, const DWORD *pItemIds, int nTax);
};

//----------------------- declares of class PlayerStatusChangeList ------------------------

class PlayerStatusChangeList
{
public:
	class Iterator
	{
	public:
		friend class PlayerStatusChangeList;

		Iterator() : m_Index(0) {};
		
	private:
		int	m_Index;
	};

	enum enOperation
	{
		enOperation_Offline  = 0,
		enOperation_Online   = 1,
//      enOperation_PkChange = 2,
	};

private:
	enum 
	{ 
		__default_list_size = 32, 
		__default_life_time = 2,
	};

	typedef struct _ListEntry
	{
		BYTE	  operation;
		short int leftTime;
		string	  playerName;

	} ListEntry;

	typedef vector<ListEntry>	PLAYERLIST;

public:
	void	Init();
	void	Active();
	void	OnLine(int nPlayerIdx);
	void	OffLine(int nPlayerIdx);
//	void    PkValueChange(int nPlayerIdx);
	bool	NextEntry(Iterator &iter, string &strPlayerName, int &nOpe);

private:
	ListEntry& GetEmptyEntry();
	void	AddEntry(int nPlayerIdx, enOperation ope);

private:
	PLAYERLIST	m_StatusList;
};

inline void PlayerStatusChangeList::Init()
{
	m_StatusList.reserve(__default_list_size);
}

inline void PlayerStatusChangeList::OnLine(int nPlayerIdx)
{
	AddEntry(nPlayerIdx, enOperation_Online);
}

inline void PlayerStatusChangeList::OffLine(int nPlayerIdx)
{
	AddEntry(nPlayerIdx, enOperation_Offline);
}
/*
inline void PlayerStatusChangeList::PkValueChange(int nPlayerIdx)
{
	AddEntry(nPlayerIdx,enOperation_PkChange);
}
*/
//extern PlayerStatusChangeList	g_PlayerStatusChgList;

#endif // #ifndef _ChatCommon_h