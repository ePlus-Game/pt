//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:5:9   15:01
//      File_base        : AntiEnthrall
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
#ifndef _AntiEnthrall_h
#define _AntiEnthrall_h

#include "GlobalDef.h"
#include <time.h>

class AntiEnthrall
{
public:
	AntiEnthrall();

	void PlayerOnline(DWORD dwEnthrallOnlineTime, DWORD dwFlag);
	void IncOnlineTime(int nPlayerIdx);
	int  GetCurState();
	void SetEnforceState( DWORD dwAntiState ,int nPlayerIndex);

private:
	bool IsStatEnthrall();
	void NotifyStateChange(int nPlayerIdx);

public:
	enum
	{
		enAntiEnthrall_InValid,
		enAntiEnthrall_Normal,
		enAntiEnthrall_Weariness,
		enAntiEnthrall_Insalubrity,
	};

	enum
	{	
		WEARINESS_EXP_SCALE = 2,
		INC_ONLINETIME_INTERVAL = GAME_FPS * 120,
	};

	enum
	{
		ENTHRALL_NOREALNAME = 1,
		ENTHRALL_REALNAME = 3,
	};
	
private:
	// Not allow copy operation
	AntiEnthrall(const AntiEnthrall &rhs);
	AntiEnthrall& operator= (const AntiEnthrall &rhs);

private:
	DWORD	m_TotalOnlineTime;
	DWORD	m_PreStatTime;
	DWORD	m_StatEnthrallFlag;
	DWORD   m_EnforceState;
};

inline AntiEnthrall::AntiEnthrall() :
	m_TotalOnlineTime(0),
	m_PreStatTime(0),
	m_StatEnthrallFlag(0),
	m_EnforceState(enAntiEnthrall_InValid)
{

}

inline bool AntiEnthrall::IsStatEnthrall()
{
	return ENTHRALL_NOREALNAME == m_StatEnthrallFlag || ENTHRALL_REALNAME == m_StatEnthrallFlag;
}

#endif // #ifndef _AntiEnthrall_h