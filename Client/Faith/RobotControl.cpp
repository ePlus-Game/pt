// RobotControl.cpp: implementation of the CRobotControl class.
// by Cooler 2004-06-30
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"

#include "RobotControl.h"
#include "CoreShell.h"
#include "KProtocol.h"
#include "GameDataDef.h"
#include "time.h"
#include "RobotScript.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern iCoreShell	*g_pCoreShell;

ROBOTACTIONINFO CRobotControl::m_tagActionInfo;

CRobotControl::CRobotControl()
{
	memset(&m_tagActionInfo, 0, sizeof(ROBOTACTIONINFO));
	memset(m_szScriptName, 0, SIZE_SCRIPTFILENAME);
	m_tagActionInfo.enAction = enActionNone;
	m_pRobotScript = NULL;
}

CRobotControl::~CRobotControl()
{
	if(m_pRobotScript)
	{
		delete m_pRobotScript;
		m_pRobotScript = NULL;
	}
}

BOOL CRobotControl::Init(LPCSTR pcScriptName)
{
	if(NULL == pcScriptName)
	{
		return FALSE;
	}

	memset(&m_tagActionInfo, 0, sizeof(ROBOTACTIONINFO));
	strcpy(m_szScriptName, pcScriptName);
	m_tagActionInfo.enAction = enActionNone;
	
	srand((unsigned)time(NULL));

	return TRUE;
}

int CRobotControl::RandomNum(int nMin, int nMax)
{
	if(nMin > nMax)
	{
		return nMin;
	}

	int nRet = 0;
	nRet = rand() % (nMax - nMin + 1) + nMin;

	return nRet;
}

void CRobotControl::Heartbeat()
{
	switch(m_tagActionInfo.enAction)
	{
	case enActionNone:
		ActionNone();
		break;
	case enActionStand:
		ActionStand();
		break;
	case enActionSitdown:
		ActionSitdown();
		break;
	case enActionWalk:
		ActionWalk();
		break;
	case enActionRun:
		ActionRun();
		break;
	case enActionAttackRun:
		ActionRun(TRUE);
		break;
	}
}

void CRobotControl::ActionNone()
{
	char szCallName[128] = {0};

	sprintf(szCallName, "%s()", RS_ROBOTACTION);
	CallScript(szCallName);
}

void CRobotControl::ActionStand()
{
	if(m_tagActionInfo.dwRunCircles > 0)
	{
		m_tagActionInfo.dwRunCircles --;
	}
	else
	{
		m_tagActionInfo.enAction = enActionNone;
	}
}

void CRobotControl::ActionSitdown()
{
	static BOOL bFirstTime = TRUE;

	if(m_tagActionInfo.dwRunCircles > 0)
	{
		if(bFirstTime)
		{
			g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PA_SIT, 0);
			bFirstTime = FALSE;
		}
		m_tagActionInfo.dwRunCircles --;
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PA_SIT, 0);
		m_tagActionInfo.enAction = enActionNone;
		bFirstTime = TRUE;
	}
}

void CRobotControl::ActionWalk()
{
	// No walk at this game
}

void CRobotControl::ActionRun(BOOL bDoAttack)
{
	static BOOL bFirstTime = TRUE;
	static int nAttackKeepTime = 0;

	if(bDoAttack && nAttackKeepTime > 0)
	{
		nAttackKeepTime--;
		return;
	}

	if(m_tagActionInfo.dwRunCircles > 0)
	{
		static int nLastPosX = 0, nLastPosY = 0;
		static int nKeepTick = 0;
		static int nDir = 0;
		if(bFirstTime)
		{
			srand((unsigned)time(NULL));
			bFirstTime = FALSE;
			nLastPosX = 0;
			nLastPosY = 0;
			nKeepTick = 0;
			nDir = 0;
		}

		int nCurPosX = 0, nCurPosY = 0;
		g_pCoreShell->GetPlayerPos(nCurPosX, nCurPosY);
		if(bDoAttack)
		{
			if(DoAttack())
			{
				nAttackKeepTime = 60;
				return;
			}
		}

		if(nKeepTick == 0 || 
			(nCurPosX == nLastPosX && nCurPosY == nLastPosY))
		{
			nKeepTick = RandomNum(18, 720);
			nDir = RandomNum(0, 7);
			if(nDir != 0)
			{
				nDir = nDir * 8 -1;
			}
		}
		
		nLastPosX = nCurPosX;
		nLastPosY = nCurPosY;
		g_pCoreShell->Goto(nDir, 0);
		nKeepTick --;

		m_tagActionInfo.dwRunCircles --;
	}
	else
	{
		m_tagActionInfo.enAction = enActionNone;
		bFirstTime = TRUE;
	}
}

BOOL CRobotControl::DoAttack()
{
	KUiPlayerItem tagItem;
	int nKind;

	// Player always in center screen
	int x = 400;
	int y = 300;

	for(int i=x-32; i<=x+32; i+=32)
	{
		for(int j=y-32; j<=y+32; j+=32)
		{
			if(i == x && j == y)
			{
				continue;
			}

			if(g_pCoreShell->FindSelectNPC(i, j, relation_enemy, 
					false, (void *)&tagItem, nKind))
			{
				// Get skill
				KUiPlayerImmedItemSkill tagSkill;
				g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&tagSkill, 0);

				// Attack npc
				if(!g_pCoreShell->
					LockSomeoneUseSkill(tagItem.nIndex, tagSkill.IMmediaSkill[0].uId))
				{
					g_pCoreShell->LockSomeoneAction(0);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

// This function need server open the GM instruction support
void CRobotControl::SendGmInstruction(LPCSTR pcInstruction)
{
	if(pcInstruction == NULL)
	{
		return;
	}

	int nLen = strlen(pcInstruction) + SIZE_LEAD_GMINSTRUCTION;
	int nIsGM = 0;

	size_t chatsize = sizeof(CHAT_CHANNELCHAT_CMD) + nLen + nIsGM;
	size_t pckgsize = sizeof(tagExtendProtoHeader) + chatsize;
	tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)malloc(pckgsize);
	if(pExHeader == NULL)
	{
		return;
	}
	pExHeader->ProtocolType = c2s_extendchat;
	pExHeader->wLength = pckgsize - 1;

	CHAT_CHANNELCHAT_CMD* pCccCmd = (CHAT_CHANNELCHAT_CMD*)(pExHeader + 1);
	pCccCmd->ProtocolType = chat_channelchat;
	pCccCmd->wSize = chatsize - 1;
	pCccCmd->packageID = -1;
	pCccCmd->filter = 1;
	pCccCmd->channelid = 0;	// Send GM instruction, channel ID must be 0
	pCccCmd->cost = 0;	// Send GM instruction cost nothing
	pCccCmd->sentlen = nLen + nIsGM;
	BYTE *pStringHead = (BYTE *)(pCccCmd + 1);
	if(nIsGM)
	{
		*pStringHead = 0x01;
		pStringHead++;
	}
	memcpy(pStringHead, LEAD_GMINSTRUCTION, SIZE_LEAD_GMINSTRUCTION);
	pStringHead += SIZE_LEAD_GMINSTRUCTION;
	memcpy(pStringHead, pcInstruction, nLen - SIZE_LEAD_GMINSTRUCTION);

	g_pCoreShell->SendNewDataToServer(pExHeader, pckgsize);

	free(pExHeader);
}

BOOL CRobotControl::CallScript(LPCSTR pcScript)
{
	if(NULL == pcScript)
	{
		return FALSE;
	}

	if(NULL == m_pRobotScript)
	{
		m_pRobotScript = new KLuaScript;
		if(NULL == m_pRobotScript)
		{
			return FALSE;
		}

		BOOL bScriptInitSuccess = TRUE;
		if(!m_pRobotScript->Init())
		{
			bScriptInitSuccess = FALSE;
		}

		if(bScriptInitSuccess)
		{
			if(!m_pRobotScript->RegisterFunctions(g_RobotScriptFuns, 
												GetRobotScriptNum()))
			{
				bScriptInitSuccess = FALSE;
			}
		}
		
		//Load Robot Script Functions
		if(!m_pRobotScript->Load(m_szScriptName))
		{
			bScriptInitSuccess = FALSE;
		}

		if(!bScriptInitSuccess)
		{
			delete m_pRobotScript;
			m_pRobotScript = NULL;
			return FALSE;
		}
	}

	if(m_pRobotScript->LoadBuffer((PBYTE)pcScript, strlen(pcScript))) 
	{
		return m_pRobotScript->ExecuteCode();
	}

	return FALSE;
}

void CRobotControl::SetAction(CONST ROBOTACTIONINFO &tagAction)
{
	m_tagActionInfo = tagAction;
}

void CRobotControl::SendRobotNotify(enNOTIFYTYPE enNotify)
{
	char szCallName[128] = {0};

	sprintf(szCallName, "%s(%d)", RS_NOTIFYROBOT, enNotify);
	CallScript(szCallName);
}
