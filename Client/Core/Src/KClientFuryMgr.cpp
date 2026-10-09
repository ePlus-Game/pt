#include "KCore.h"
#include "KClientFuryMgr.h"
#include "common_fury_def.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "CoreShell.h"
#include "AutoRobotMgr.h"

KClientFuryMgr::KClientFuryMgr():m_FuryExp(0)
{
    /*Do Nothing inside at all*/
}

KClientFuryMgr::~KClientFuryMgr()
{
	/*Do Nothing at all*/
}

KClientFuryMgr & KClientFuryMgr::Singlton()
{
	static KClientFuryMgr gClientFuryMgr;
    return gClientFuryMgr;
}

int              KClientFuryMgr::GetCurFuryExp()const
{
	return m_FuryExp;
}

void KClientFuryMgr::ExplodeCurFury()
{
	if (m_FuryExp == 100)
	{
		C2S_FURY_EXPLODE explodeCommand;
		explodeCommand.ProtocolType = c2s_fury_explode;
		
		if ( g_pClient )
		{
			g_pClient->SendPackToServer(g_ConnectID, &explodeCommand, sizeof(explodeCommand));
		}//endif
	}//endif
} 

void KClientFuryMgr::ProcessMsg(BYTE * pMsg)
{
	S2C_FURY_SYNC * pFruy = (S2C_FURY_SYNC *)pMsg;
	switch (pFruy->SubProtocolType)
	{
	case s2c_fury_exp_sync:
		{
            m_FuryExp   =  pFruy->nAdditionalParam;
			//Notify UI here
			CoreDataChanged(GDCNI_FURY_CHANGE,m_FuryExp,0);
			AutoRobotMgr::Singleton().SetBlastExp(m_FuryExp);
		}//end for case 
		break;

	case s2c_fury_warning:
		{
            //Notify UI here
           CoreDataChanged(GDCNI_FURY_WARNNING,pFruy->nAdditionalParam,0);
		}//end for case 
		break;

	default:break;
	}
}