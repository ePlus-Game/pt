// RobotControl.h: interface for the CRobotControl class.
// by Cooler 2004-06-30
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROBOTCONTROL_H__782BD716_0F3E_4F78_B93B_35525D1CAC47__INCLUDED_)
#define AFX_ROBOTCONTROL_H__782BD716_0F3E_4F78_B93B_35525D1CAC47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KLuaScript.h"

#define LEAD_GMINSTRUCTION			"?gm ds "
#define SIZE_LEAD_GMINSTRUCTION		7
#define SIZE_SCRIPTFILENAME			128

#define RS_ROBOTACTION				"RobotAction"
#define RS_NOTIFYROBOT				"NotifyRobot"

enum enACTIONTYPE
{
	enActionNone = 0, 
	enActionStand, 
	enActionSitdown, 
	enActionWalk, 
	enActionRun, 
	enActionAttackRun, 
	enActionEnd, 
};

enum enNOTIFYTYPE
{
	enNotifyEnterGame = 0, 
	enNotifyLeaveGame, 
	enNotifyLowBlood, 
	enNotifyLowMana, 
};

typedef struct tagROBOTACTIONINFO
{
	enACTIONTYPE enAction;
	DWORD dwRunCircles;
}ROBOTACTIONINFO, *PROBOTACTIONINFO;

class CRobotControl  
{
public:
	CRobotControl();
	virtual ~CRobotControl();

	BOOL Init(LPCSTR pcScriptName);
	void Heartbeat();
	void SendRobotNotify(enNOTIFYTYPE enNotify);
	
	static void SendGmInstruction(LPCSTR pcInstruction);
	static void SetAction(CONST ROBOTACTIONINFO &tagAction);
	
protected:
	static ROBOTACTIONINFO m_tagActionInfo;
	char m_szScriptName[SIZE_SCRIPTFILENAME];
	KLuaScript *m_pRobotScript;

	int RandomNum(int nMin, int nMax);
	void ActionNone();
	void ActionStand();
	void ActionSitdown();
	void ActionWalk();
	void ActionRun(BOOL bDoAttack = FALSE);
	BOOL DoAttack();

	BOOL CallScript(LPCSTR pcScript);
};

#endif // !defined(AFX_ROBOTCONTROL_H__782BD716_0F3E_4F78_B93B_35525D1CAC47__INCLUDED_)
