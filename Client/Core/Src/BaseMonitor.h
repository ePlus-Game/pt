//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-25
//      File_base        : BaseMonitor
//      File_ext         : .h
//      Author           : ÐìÏþ¸Õ
//      Description      : ¼àÊÓÆ÷»ùÀà
//
//////////////////////////////////////////////////////////////////////

#ifndef _BASE_MONITOR_H_
#define _BASE_MONITOR_H_

#include "ItemCommonDef.h"

typedef struct tagBuffSetuped
{
	int BuffID;
	unsigned long BuffIndex;
	unsigned long CreatureBuffIndex;
} BuffSetuped;

#define MAX_BUFF_TO_BE_SETUP 400

class BaseMonitor
{
public:
	
	BaseMonitor();
	~BaseMonitor();

	void CleanUp();

	void BeginAddBuff();

	bool AddBuff(int buffID);

	void EndAddBuff();

	void SetupBuff(int npcIndex, bool clearAll = false);

	void SetupCurrentBuff(int npcIndex);

private:

	int m_BuffToBeAddCount;

	int m_CurrentBuffIndexCount;

	int m_BuffIDToBeAdd[MAX_BUFF_TO_BE_SETUP];

	BuffSetuped m_CurrentBuff[MAX_BUFF_TO_BE_SETUP];

};

#endif //_BASE_MONITOR_H_