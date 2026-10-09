#include "KCore.h"
#include "KTaisuiWheel.h"
#include "KTaisuiWheelServer.h"
#include "KProtocol.h"
#include "TaisuiSubProtocol.h"

#ifdef _SERVER
#include "CoreUseNameDef.h"
#include "ITaisuiWheelTianXiangMgr.h"
#include "ITaisuiWheelEventMgr.h"
#include "ITaisuiWheelResGenerator.h"
#include "ITaisuiWheelSettingMgr.h"
#include "KSubWorldSet.h"
//#include <Timer.h>

#include "buff_man.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "cfs_fs2_savedef.h"
#include "player_monitor.h"
#include "CoreRelated.h"

#else
#include "CoreShell.h"
#endif


/*************************************************
 *        Share part                             *
 *************************************************/

bool KTaisuiWheel::IsInited(void)const
{
     return m_IsInit; 
}

unsigned long KTaisuiWheel::GetCurrentWheeledTimes(void)const
{
#ifdef _SERVER
	_ASSERT(m_IsInit);
#else
	_ASSERT(m_IsWheelInited);
#endif

	return m_WheeledTimes;
}

unsigned long KTaisuiWheel::GetCurrentAvailableTimes(void)const
{
#ifdef _SERVER
	_ASSERT(m_IsInit);
#else
	_ASSERT(m_IsWheelInited);
#endif
    return m_AvailableTimes;
}

#ifdef _SERVER
/*************************************************
 *       ServerPart                              *
 *************************************************/

KTaisuiWheel::tagRESET_POINT::tagRESET_POINT()
{
	time_t tCurrent=UNIX_TMIE_STAMP;
	tm   * tTm=localtime(&tCurrent);
	m_Day=tTm->tm_mday;
	m_Month=tTm->tm_mon;
	m_Year=tTm->tm_year;
}

KTaisuiWheel::KTaisuiWheel()
{
	ReFresh();
	//Init call back function
    ProcessFunc[c2s_taisui_tian_gan]=&KTaisuiWheel::ProcessWheelTianGan;
	ProcessFunc[c2s_taisui_dizhi] =&KTaisuiWheel::ProcessWheelDiZhi;
	ProcessFunc[c2s_drop_chance] = &KTaisuiWheel::ProcessDropChance ;
	ProcessFunc[c2s_taisui_request_gift] = &KTaisuiWheel::ProcessShowGiftRes ;
}

void KTaisuiWheel::Init(const unsigned long dwPlayerIndex,const unsigned long dwConnectIndex )
{
	m_PlayerIndex=dwPlayerIndex;
	m_ConnectIndex=dwConnectIndex;
	if (m_IsDBReady)
		m_IsInit=true;
	_ASSERT(time!=NULL);
}

void KTaisuiWheel::ReFresh()
{
   /* m_IsInit=false;
	m_WheeledTimes=0;
	m_FirstOnlineFlag=true;
	m_AvailableTimes=GetMainTaisuiWheelSettingMgr()->GetFreeTimes();
	m_IsInit=false;
	m_CurState=SS_IDLE;
    m_Day=GetMainTianXiangMgr()->GetCurrentTianXiangDay();
	m_Month=GetMainTianXiangMgr()->GetCurrentTianXiangMonth();
	m_TianGanRes=0;
    m_DizhiRes=0;
	m_JiazeActivatedIndex=GetMainTaisuiWheelEventMgr()->GetNextTianXiangToActivate()-1;
   */

	m_IsInit=false;
	m_WheeledTimes=0;
	m_FirstOnlineFlag=true;
	m_AvailableTimes=0;
	m_IsInit=false;
	m_CurState=SS_IDLE;
    m_Day=0;
	m_Month=0;
	m_TianGanRes=0;
    m_DizhiRes=0;
	m_JiazeActivatedIndex=0;
	m_PlayerIndex=0;
	m_ConnectIndex=0;
	m_IsDBReady=false;
}

KTaisuiWheel::~KTaisuiWheel()
{
	ReFresh();
}

void KTaisuiWheel::SaveDB(void * pBuff)const
{
	//Save available time , wheeled time , resetpoint
	FS2DBOTHERINFOCOL * pINFO = (FS2DBOTHERINFOCOL *)(pBuff);
	
	unsigned long dwResetPosInfo=0;
	unsigned long dwYear=m_LastResetPoint.m_Year;
	unsigned long dwMonth=m_LastResetPoint.m_Month;
	unsigned long dwDay=m_LastResetPoint.m_Day;
    dwResetPosInfo=((dwYear)<<16)+(dwMonth<<8)+dwDay;
	pINFO->nResetPosInfo=dwResetPosInfo;
	pINFO->nTaisuiAvailableTimes=m_AvailableTimes;
	pINFO->nTaisuiWheeledTimes=m_WheeledTimes;
	
}

void KTaisuiWheel::LoadDB(void * pBuff,unsigned long dwVersion)
{
    //Load availeble time , wheeled time , resetpoint

	FS2DBOTHERINFOCOL * pINFO = (FS2DBOTHERINFOCOL *)(pBuff);
	
	unsigned long dwResetPosInfo=pINFO->nResetPosInfo;
	m_LastResetPoint.m_Year=((dwResetPosInfo & 0xffff0000)>>16);
	m_LastResetPoint.m_Month=((dwResetPosInfo & 0x0000ff00)>>8);
	m_LastResetPoint.m_Day=(dwResetPosInfo & 0x000000ff);
    m_AvailableTimes=pINFO->nTaisuiAvailableTimes;
	m_WheeledTimes=pINFO->nTaisuiWheeledTimes;
	
	if (m_AvailableTimes==0 && m_WheeledTimes==0)  //Initialize DB Info
		m_AvailableTimes=GetMainTaisuiWheelSettingMgr()->GetFreeTimes();
	
	CheckReset();
    m_IsDBReady=true;
}

void KTaisuiWheel::SendOnlineSync()const
{
	SendSyncMsg();
	SendSyncTianXiang();
	SendJiaziActive();
}

void KTaisuiWheel::ProcessMsg(BYTE * pMsg,int iSize)
{
	_ASSERT(m_IsInit && KTaisuiWheelServer::IsInited());
	//Check the msg
	if (pMsg==NULL || iSize< sizeof(TAISUI_WHEEL_PROTOCOL_HEADER))
		return ;

	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)pMsg;
	if (pHeader->Protocol!=c2s_taisui_wheel)
		return ;
    
    c2s_taisui_wheel_sub_protocol SubProtocolType=(c2s_taisui_wheel_sub_protocol)pHeader->SubProtocol;
	if (SubProtocolType>=c2s_taisui_end)
		return ;

	if (iSize<pHeader->SubSize+sizeof(TAISUI_WHEEL_PROTOCOL_HEADER))
		return ;
    
	(this->*ProcessFunc[SubProtocolType])();
}

#define TAISUI_BREATHE_INTERVAL GAME_FPS

void KTaisuiWheel::Breathe()
{
	if ( g_SubWorldSet.GetGameTime() % TAISUI_BREATHE_INTERVAL == 0 )
	{
		if (m_IsDBReady && IsValidPlayer(m_PlayerIndex) && IsValidNpc(Player[m_PlayerIndex].GetNpcIndex()))
			m_IsInit=true;
		
		if (KTaisuiWheelServer::IsInited() && m_IsInit)
		{
			//Tian Xiang Sync Only
			CheckTianXiang();

			if ( g_SubWorldSet.GetGameTime() % (TAISUI_BREATHE_INTERVAL * 300) == 0)
			{
				//Reset logic
				if(CheckReset())
					SendReset();
			
			}//endif
			
			//Event logic
			CheckActivateEvent();
			
			//Online Send
			if (m_FirstOnlineFlag)
			{
				SendOnlineSync();
				m_FirstOnlineFlag=false;
			}//endif
		}//endif
		
	}//endif
}

void KTaisuiWheel::CheckActivateEvent()
{
    unsigned long dwEventIndex=GetMainTaisuiWheelEventMgr()->GetNextTianXiangToActivate()-1;
//	_ASSERT(dwEventIndex>=0 && dwEventIndex<=MAX_JIAZI_NUM);

	if (dwEventIndex!=m_JiazeActivatedIndex && dwEventIndex>=0 && dwEventIndex<=MAX_JIAZI_NUM)
	{
		m_JiazeActivatedIndex=dwEventIndex;
		SendJiaziActive();
	}//endif
}

bool KTaisuiWheel::CheckReset()
{	
	time_t tCurentTime=UNIX_TMIE_STAMP;
	tm * time=localtime(&tCurentTime);
    if (time->tm_year!=m_LastResetPoint.m_Year || 
		time->tm_mon != m_LastResetPoint.m_Month || (m_LastResetPoint.m_Day < time->tm_mday-1) ||
		(m_LastResetPoint.m_Day == time->tm_mday-1 && time->tm_hour>=GetMainTaisuiWheelSettingMgr()->GetSysResetTime()))
	{
           m_AvailableTimes=GetMainTaisuiWheelSettingMgr()->GetFreeTimes();
		   m_WheeledTimes=0;

		   m_LastResetPoint.m_Day=time->tm_mday;
		   m_LastResetPoint.m_Month=time->tm_mon;
		   m_LastResetPoint.m_Year=time->tm_year;
		   return true;
	}//endif
	return false;
}

void KTaisuiWheel::SendReset()const
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+sizeof(S2C_TAISUI_RESET)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=s2c_taisui_wheel;
	pHeader->SubProtocol=s2c_taisui_reset;
	pHeader->SubSize=sizeof(S2C_TAISUI_RESET);

	S2C_TAISUI_RESET * pProtocol = (S2C_TAISUI_RESET *) (pHeader+1);
	pProtocol->m_AvailableTimes=m_AvailableTimes;
	pProtocol->m_WheeledTimes=m_WheeledTimes;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+pHeader->SubSize;
	pHeader->Len=dwProtocolSize-1;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_ConnectIndex, (BYTE*)&m_MsgBuffer, dwProtocolSize);
}

void KTaisuiWheel::SendTianGanRes()const
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+sizeof(S2C_CAL_TIAN_GAN)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=s2c_taisui_wheel;
	pHeader->SubProtocol=s2c_taisui_cal_res;
	pHeader->SubSize=sizeof(S2C_CAL_TIAN_GAN);
	
	S2C_CAL_TIAN_GAN * pProtocol = (S2C_CAL_TIAN_GAN *) (pHeader+1);
	pProtocol->m_TianGan=m_TianGanRes;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+pHeader->SubSize;
	pHeader->Len=dwProtocolSize-1;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_ConnectIndex, (BYTE*)&m_MsgBuffer, dwProtocolSize);

}

void KTaisuiWheel::SetCurrentAvailableTimes(const unsigned long dwTimes)
{
	if (m_IsInit && m_AvailableTimes)
	{
		m_AvailableTimes = dwTimes;
		SendSyncMsg();
	}//endif
}

void KTaisuiWheel::SendJiaziActive()const
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+sizeof(S2C_TAISUI_EVNET_SYNC)<MAX_MSG_BUFFER);
	
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=s2c_taisui_wheel;
	pHeader->SubProtocol=s2c_taisui_jiazi;
	pHeader->SubSize=sizeof(S2C_TAISUI_EVNET_SYNC);
	
	S2C_TAISUI_EVNET_SYNC * pProtocol = (S2C_TAISUI_EVNET_SYNC *) (pHeader+1);
	pProtocol->m_CurrentActivateIndex=m_JiazeActivatedIndex;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+pHeader->SubSize;
	pHeader->Len=dwProtocolSize-1;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_ConnectIndex, (BYTE*)&m_MsgBuffer, dwProtocolSize);
	
}

void KTaisuiWheel::CheckTianXiang()
{
	bool bSend=false;	
	
	if (m_Day!=GetMainTianXiangMgr()->GetCurrentTianXiangDay()
		|| m_Month!=GetMainTianXiangMgr()->GetCurrentTianXiangMonth())
		bSend=true;
	
	
	m_Day=GetMainTianXiangMgr()->GetCurrentTianXiangDay();
	m_Month=GetMainTianXiangMgr()->GetCurrentTianXiangMonth();
	
//	_ASSERT(m_Day>0 && m_Day<=MAX_JIAZI_NUM && m_Month>0 && m_Month<=MAX_JIAZI_NUM);
    
	if (bSend)
		SendSyncTianXiang();
	
}

bool KTaisuiWheel::SendSyncMsg()const
{
    _ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+sizeof(S2C_TAISUI_SYNC)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=s2c_taisui_wheel;
	pHeader->SubProtocol=s2c_taisui_sync;

    S2C_TAISUI_SYNC * pProtocol= (S2C_TAISUI_SYNC *) (pHeader+1);
	pProtocol->m_WheeledTimes=m_WheeledTimes;
	pProtocol->m_AvailableTimes=m_AvailableTimes;
    pHeader->SubSize=sizeof(S2C_TAISUI_SYNC);
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+pHeader->SubSize;
	pHeader->Len=dwProtocolSize-1;

	if (g_pServer != NULL && SUCCEEDED(g_pServer->PackDataToClient(m_ConnectIndex, (BYTE*)&m_MsgBuffer, dwProtocolSize)))
		return true;
	else
		return false;
}

void KTaisuiWheel::ProcessWheelTianGan()
{
	 _ASSERT(m_IsInit);
//	 _ASSERT(m_CurState==SS_IDLE);

	 if (!IsValidPlayer(m_PlayerIndex))
		 return ;

	 if (m_CurState!=SS_IDLE)
		 return;

	 if (m_AvailableTimes<=0)
		 return;

	 m_CurState=SS_WHEEL_TIAN_GAN;

	 //Availeble Time degress
	 m_AvailableTimes-=1;
	 m_WheeledTimes+=1;
    
	 m_TianGanRes=0;
	 m_DizhiRes=0;

	 //Log used 
     Player[m_PlayerIndex].GetPlayerStatistic().UseTaisui();
	 
	 if (g_PlayerMonitor.IsNeedRecord(m_PlayerIndex, player_action_use_taisui_wheel))
	 {
		 RecordPlayerActionParam param;
		 param.PlayerIndex = m_PlayerIndex;
		 param.Action = player_action_use_taisui_wheel;
		 snprintf(param.Desc, sizeof(param.Desc), PLAYER_ACTION_USE_TAISUI_WHEEL);
		 g_PlayerMonitor.RecordPlayerAction(param);
	 }

	 //CheckBuff
     BuffMgr & buff_mgr=BuffMgr::Singleton();
	 
	 unsigned long  dwCurTianXiangDay=GetMainTianXiangMgr()->GetCurrentTianXiangDay();
     unsigned long  dwCurTianXiangMonth=GetMainTianXiangMgr()->GetCurrentTianXiangMonth();

	 _ASSERT(dwCurTianXiangDay>0 && dwCurTianXiangDay<=MAX_JIAZI_NUM);
	 _ASSERT(dwCurTianXiangMonth>0 && dwCurTianXiangMonth<=MAX_JIAZI_NUM);

	 if (!(dwCurTianXiangDay>0 && dwCurTianXiangDay<=MAX_JIAZI_NUM) || !(dwCurTianXiangMonth>0 && dwCurTianXiangMonth<=MAX_JIAZI_NUM))
		 return ;

	 //使日结果相同
	 if (buff_mgr.IsHaveBuff(Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetDaySameBuffID()))
	 {
		 unsigned long dwDayTianGan=0;
		 unsigned long dwDayDizhi=0;
          
		 TianXiangToTianGanDiZhi(dwCurTianXiangDay,&dwDayTianGan,&dwDayDizhi);
		 _ASSERT(dwDayTianGan>0 && dwDayTianGan<=MAX_TIANGAN_NUM);
		 _ASSERT(dwDayDizhi>0 && dwDayDizhi<=MAX_DIZI_NUM);
		 m_TianGanRes=dwDayTianGan;

	 }//endif
     //使月结果相同
	 if (buff_mgr.IsHaveBuff(Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetMonthSameBuffID()))
	 {
		 unsigned long dwMonthTianGan=0;
		 unsigned long dwMonthDizhi=0;
		 
		 TianXiangToTianGanDiZhi(dwCurTianXiangMonth,&dwMonthTianGan,&dwMonthDizhi);
		 _ASSERT(dwMonthTianGan>0 && dwMonthTianGan<=MAX_TIANGAN_NUM);
		 _ASSERT(dwMonthDizhi>0 && dwMonthDizhi<=MAX_DIZI_NUM);
		 m_TianGanRes=dwMonthTianGan;

      }//endif

	 GetMainTaisuiWheelResGenerator()->GenRes(&m_TianGanRes,&m_DizhiRes,GetMainTaisuiWheelEventMgr()->GetNextTianXiangToActivate());
     _ASSERT(m_TianGanRes!=0  && m_TianGanRes <= MAX_TIANGAN_NUM && m_DizhiRes!=0 && m_DizhiRes<=MAX_DIZI_NUM);
     
	 if (!(m_TianGanRes!=0  && m_TianGanRes <= MAX_TIANGAN_NUM && m_DizhiRes!=0 && m_DizhiRes<=MAX_DIZI_NUM))
		 return ;

	 SendTianGanRes(); 
} 

void KTaisuiWheel::ProcessWheelDiZhi()
{
	_ASSERT(m_IsInit);

	if (!IsValidPlayer(m_PlayerIndex))
		return ;

//	_ASSERT(m_CurState==SS_WHEEL_TIAN_GAN);

	if (m_CurState!=SS_WHEEL_TIAN_GAN)
		return;

	//CheckBuff
	BuffMgr & buff_mgr=BuffMgr::Singleton();
	
	unsigned long  dwCurTianXiangDay=GetMainTianXiangMgr()->GetCurrentTianXiangDay();
	unsigned long  dwCurTianXiangMonth=GetMainTianXiangMgr()->GetCurrentTianXiangMonth();
	
	_ASSERT(dwCurTianXiangDay>0 && dwCurTianXiangDay<=MAX_JIAZI_NUM);
	 _ASSERT(dwCurTianXiangMonth>0 && dwCurTianXiangMonth<=MAX_JIAZI_NUM);

	//使日结果相同
	if (buff_mgr.IsHaveBuff(Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetDaySameBuffID()))
	{
		unsigned long dwDayTianGan=0;
		unsigned long dwDayDizhi=0;
		
		TianXiangToTianGanDiZhi(dwCurTianXiangDay,&dwDayTianGan,&dwDayDizhi);
		_ASSERT(dwDayTianGan>0 && dwDayTianGan<=MAX_TIANGAN_NUM);
		_ASSERT(dwDayDizhi>0 && dwDayDizhi<=MAX_DIZI_NUM);

		if (m_TianGanRes==dwDayTianGan)
			m_DizhiRes=dwDayDizhi;

		//Clear Buff
		buff_mgr.ClearBuffByTempID(Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetDaySameBuffID());
	
	}//endif
	//使月结果相同
	if (buff_mgr.IsHaveBuff(Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetMonthSameBuffID()))
	{
		unsigned long dwMonthTianGan=0;
		unsigned long dwMonthDizhi=0;
		
		TianXiangToTianGanDiZhi(dwCurTianXiangMonth,&dwMonthTianGan,&dwMonthDizhi);
		_ASSERT(dwMonthTianGan>0 && dwMonthTianGan<=MAX_TIANGAN_NUM);
		_ASSERT(dwMonthDizhi>0 && dwMonthDizhi<=MAX_DIZI_NUM);
		if (m_TianGanRes==dwMonthTianGan)
			m_DizhiRes=dwMonthDizhi;
        //Clear Buff
		buff_mgr.ClearBuffByTempID(Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetMonthSameBuffID());
	 
	 }//endif

	m_CurState=SS_WHEEL_DI_ZHI;

	_ASSERT(m_TianGanRes>0 && m_TianGanRes<=MAX_TIANGAN_NUM);
    _ASSERT(m_DizhiRes>0 && m_DizhiRes<=MAX_DIZI_NUM);

    //发送地支的结果 并发送奖励
    unsigned long dwFinalRes=TianGanDiZhiToTianXiang(m_TianGanRes,m_DizhiRes);

	_ASSERT(dwFinalRes<=MAX_JIAZI_NUM && dwFinalRes>0);

	if (dwFinalRes>MAX_JIAZI_NUM || dwFinalRes==0)
	{
        m_CurState = SS_IDLE;
		return;
	}//endif
	
	//Calculate the gift
	unsigned long    dwEventIndex=0;
    if (dwFinalRes==GetMainTaisuiWheelEventMgr()->GetNextTianXiangToActivate())
	{
		if (dwFinalRes % 12==0)
		{
			dwEventIndex=GetMainTaisuiWheelSettingMgr()->GetJiaziEventInfo((dwFinalRes/12));
		//	BroadCastMsg(GIFT_JIAZI_EVENT); //不再有甲子事件了 。。
		}//endif
		
		m_JiazeActivatedIndex = dwFinalRes;
	//	SendJiaziActive();

	}//endif

	//Breathe event
	GetMainTaisuiWheelEventMgr()->BreatheRes(m_TianGanRes,m_DizhiRes);

	//Grand test
	CheckTianXiang();
	unsigned long   dwGrand=0;
	
	if (m_Month==dwFinalRes)
		dwGrand+=2;

	if (m_Day==dwFinalRes)
		dwGrand+=1;

	//Send res msg
	TS_GIFT        gift;
    GetMainTaisuiWheelSettingMgr()->GetGiftExpression(m_TianGanRes,m_DizhiRes,dwGrand,&gift);
    AddGift(&gift,dwEventIndex);
	SendGiftRes(m_DizhiRes,dwGrand,dwEventIndex);
	//Broad cast
	if (dwGrand==3)
	{
      BroadCastMsg(GIFT_D_AND_MONTH);
	}//endif

	//Jiazi Event

    SendJiaziActive();
	if (m_JiazeActivatedIndex==MAX_JIAZI_NUM)
	{
		m_JiazeActivatedIndex=0;
		SendJiaziActive();
	}//endif

	m_DizhiRes=0;
	m_TianGanRes=0;

	m_CurState=SS_IDLE;
}

void KTaisuiWheel::ProcessDropChance()
{
   if (m_CurState==SS_WHEEL_TIAN_GAN || m_CurState==SS_WHEEL_DI_ZHI)
   {
      m_CurState=SS_IDLE;
   }//endif
}

void KTaisuiWheel::ProcessShowGiftRes(void)
{
   if (m_IsInit && IsValidPlayer(m_PlayerIndex))
   {
	   BuffMgr & gBuffMan=BuffMgr::Singleton();
       gBuffMan.AddNpcBuff(Player[m_PlayerIndex].GetNpcIndex(),Player[m_PlayerIndex].GetNpcIndex(),GetMainTaisuiWheelSettingMgr()->GetShowResBuffID());      
   }//endif
}

void KTaisuiWheel::AddGift(const TS_GIFT * pGift/* =NULL */,unsigned long dwEventAddition)
{
	if (!IsValidPlayer(m_PlayerIndex))
		return ;

    if (pGift!=NULL)
	{
		BuffMgr & gBuffMan=BuffMgr::Singleton();	
		gBuffMan.AddNpcBuff(Player[m_PlayerIndex].GetNpcIndex(),Player[m_PlayerIndex].GetNpcIndex(),pGift->dwTianGanBuff);
		gBuffMan.AddNpcBuff(Player[m_PlayerIndex].GetNpcIndex(),Player[m_PlayerIndex].GetNpcIndex(),pGift->dwDizhiBuff);
        gBuffMan.AddNpcBuff(Player[m_PlayerIndex].GetNpcIndex(),Player[m_PlayerIndex].GetNpcIndex(),pGift->dwAdditionBuff);
	}//endif

	if (dwEventAddition!=0)
	{
        BuffMgr & gBuffMan=BuffMgr::Singleton();
		gBuffMan.AddNpcBuff(Player[m_PlayerIndex].GetNpcIndex(),Player[m_PlayerIndex].GetNpcIndex(),dwEventAddition);
	}//endif
}

void KTaisuiWheel::SendGiftRes(const unsigned long dwDizhi, const unsigned long dwGrand, const unsigned long dwEventID/* =0 */)const
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+sizeof(S2C_CAL_GIFT_RES)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=s2c_taisui_wheel;
	pHeader->SubProtocol=s2c_taisui_gift;
	pHeader->SubSize=sizeof(S2C_CAL_GIFT_RES);

    S2C_CAL_GIFT_RES * pProtocol= (S2C_CAL_GIFT_RES *) (pHeader+1);
	pProtocol->m_DizhiRes=(unsigned char )dwDizhi;
	pProtocol->m_Grand=(unsigned char)dwGrand;
	pProtocol->m_JiaziEvent=(unsigned short)dwEventID;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+pHeader->SubSize;
	pHeader->Len=dwProtocolSize-1;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_ConnectIndex, (BYTE*)&m_MsgBuffer, dwProtocolSize);

}

void KTaisuiWheel::SendSyncTianXiang()const
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+sizeof(S2C_TIAN_XIANG)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=s2c_taisui_wheel;
	pHeader->SubProtocol=s2c_taisui_tianxiang;
	
    S2C_TIAN_XIANG * pProtocol= (S2C_TIAN_XIANG *) (pHeader+1);
	pProtocol->m_Day=m_Day;
	pProtocol->m_Month=m_Month;

	pHeader->SubSize=sizeof(S2C_TIAN_XIANG);

	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)+pHeader->SubSize;
	pHeader->Len=dwProtocolSize-1;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(m_ConnectIndex, (BYTE*)&m_MsgBuffer, dwProtocolSize);
}

#define  MAX_BROAD_CAST_LEN 100

void KTaisuiWheel::BroadCastMsg(const char * szString)
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	char pMsg[MAX_BROAD_CAST_LEN];
	sprintf(pMsg,"%s%s",Player[m_PlayerIndex].GetPlayerName(),szString);

	if (pMsg == NULL)
		return;

	char font[MAX_SHOW_BANNER_FONT_LENGTH] = { 0 };
	snprintf(font, sizeof(font), "stzhongs-10");
	font[sizeof(font) - 1] = 0;
	
	int msgLength = strlen(pMsg);
	if (msgLength <= 0 || msgLength >= MAX_SHOW_BANNER_MSG_LENGTH)
		return;

	char sendBuff[COMMON_SHOW_BANNER_BUFF_LENGTH];
	int sendSize = PrepareShowBannerBuff(sendBuff, sizeof(sendBuff), pMsg, sizeof(pMsg), font, sizeof(font), 0xffffffff, 50, 1, 2);
	
	int playerCount = PlayerSet.GetPlayerNumber();
	int playerIndex = PlayerSet.GetFirstPlayer();
	for (int i = 0; i < playerCount; i++)
	{
		if (playerIndex > 0 )
		{
			if (playerIndex!=m_PlayerIndex)
			{
				KPlayer& player = Player[playerIndex];
				
				if (g_pServer != NULL)
					g_pServer->PackDataToClient(player.GetNetConnectIdx(), sendBuff, sendSize);
			}//endif
			
			playerIndex = PlayerSet.GetNextPlayer();
		}
		else
		{
			break;
		}
	}
	
	return;
}

#else

/*************************************************
 *       ClientPart                              *
 * Notice:只有接到同步包后才可以转动             *
 *        没有接到同步包的情况下之能接受天气改变 *
 *************************************************/

KTaisuiWheel::KTaisuiWheel()
{
	ReFresh();

	ProcessFunc[s2c_taisui_sync]=ProcessSyncMsg;
	ProcessFunc[s2c_taisui_cal_res] = ProcessTianGan;
	ProcessFunc[s2c_taisui_gift] = ProcessGift;
	ProcessFunc[s2c_taisui_tianxiang] =ProcessTianXiang;
	ProcessFunc[s2c_taisui_jiazi] = ProcessJiaziEvnet;
    ProcessFunc[s2c_taisui_reset] = ProcessReset;
}

void KTaisuiWheel::Init()
{/*Do Nothing at all*/}

KTaisuiWheel::~KTaisuiWheel()
{
	ReFresh();
}

void KTaisuiWheel::Breathe()
{
	if (m_IsInit)
	{

	}//endif
}

unsigned long KTaisuiWheel::GetCurrentWheeldTianGan(void)const
{
    return m_TianGanRes;
}

unsigned long KTaisuiWheel::GetCurrentWheeldDiZhi(void)const
{
    return m_DizhiRes;
}


void KTaisuiWheel::WheelTianGan()
{
	_ASSERT(m_AvailableTimes>=1);

	if (m_AvailableTimes>=1)
	{
       m_WheeledTimes+=1;
	   m_AvailableTimes-=1;
       CoreDataChanged(GDCNI_WHEEL_TIMES_CHANGED,0,0);
	   SendWheelTianGan();
	}//endif
}

void KTaisuiWheel::WheelDizhi()
{
	_ASSERT(m_TianGanRes!=0);

	if (m_TianGanRes)
	{
       SendWheelDizhi();
	}//endif

}

void KTaisuiWheel::DropChance()
{
	m_TianGanRes=0;
	m_DizhiRes=0;
	SendDropChance();
}

void KTaisuiWheel::RequestGiftRes()
{
    SendShowResOP();
}

void KTaisuiWheel::SendDropChance()
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=c2s_taisui_wheel;
	pHeader->SubProtocol=c2s_drop_chance;
	pHeader->SubSize=0;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER);
	pHeader->Len=dwProtocolSize-1;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&m_MsgBuffer, dwProtocolSize);

}

void KTaisuiWheel::SendWheelTianGan()
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=c2s_taisui_wheel;
	pHeader->SubProtocol=c2s_taisui_tian_gan;
	pHeader->SubSize=0;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER);
	pHeader->Len=dwProtocolSize-1;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&m_MsgBuffer, dwProtocolSize);
}

void KTaisuiWheel::SendShowResOP()
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=c2s_taisui_wheel;
	pHeader->SubProtocol=c2s_taisui_request_gift;
	pHeader->SubSize=0;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER);
	pHeader->Len=dwProtocolSize-1;
	
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&m_MsgBuffer, dwProtocolSize);
}

void KTaisuiWheel::SendWheelDizhi()
{
	_ASSERT(m_IsInit);
	_ASSERT(sizeof(TAISUI_WHEEL_PROTOCOL_HEADER)<MAX_MSG_BUFFER);
    
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)m_MsgBuffer;
	pHeader->Protocol=c2s_taisui_wheel;
	pHeader->SubProtocol=c2s_taisui_dizhi;
	pHeader->SubSize=0;
	
	unsigned long dwProtocolSize=sizeof(TAISUI_WHEEL_PROTOCOL_HEADER);
	pHeader->Len=dwProtocolSize-1;

	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,(BYTE*)&m_MsgBuffer, dwProtocolSize);
}

void KTaisuiWheel::ProcessMsg(BYTE * pMsg)
{
  //做子协议合法分析
	
	TAISUI_WHEEL_PROTOCOL_HEADER * pHeader=(TAISUI_WHEEL_PROTOCOL_HEADER *)pMsg;
	if (pHeader->Protocol!=s2c_taisui_wheel)
		return ;
    
    s2c_taisui_wheel_sub_protocol SubProtocolType=(s2c_taisui_wheel_sub_protocol)pHeader->SubProtocol;
	if (SubProtocolType>s2c_taisui_end)
		return;

	(this->*ProcessFunc[SubProtocolType])((BYTE *)(pHeader+1));
}

void KTaisuiWheel::ProcessSyncMsg(BYTE * pSubMsg)
{
	S2C_TAISUI_SYNC * pProtocol= (S2C_TAISUI_SYNC *)pSubMsg;
	m_AvailableTimes=pProtocol->m_AvailableTimes;
	m_WheeledTimes=pProtocol->m_WheeledTimes;

	m_IsWheelInited= true ;

	if (m_IsTianXiangInited && m_IsJiaziEventInited)
		m_IsInit=true;

	CoreDataChanged(GDCNI_WHEEL_TIMES_CHANGED,0,0);
}

void KTaisuiWheel::ProcessJiaziEvnet(BYTE * pSubMsg)
{
	S2C_TAISUI_EVNET_SYNC * pProtocol = (S2C_TAISUI_EVNET_SYNC *) pSubMsg;
	m_JiazeActivatedIndex=pProtocol->m_CurrentActivateIndex;
	m_IsJiaziEventInited=true;
	
	if (m_IsWheelInited && m_IsTianXiangInited)
		m_IsInit=true;

	CoreDataChanged(GDCNI_JIAZI_EVENT_CHANGED,0,0);
}

void KTaisuiWheel::ProcessTianXiang(BYTE * pSubMsg)
{
	S2C_TIAN_XIANG * pProtocol=(S2C_TIAN_XIANG *) pSubMsg;
	m_Day=pProtocol->m_Day;
	m_Month=pProtocol->m_Month;

	m_IsTianXiangInited = true;

	if (m_IsWheelInited && m_IsJiaziEventInited)
		m_IsInit = true;

	CoreDataChanged(GDCNI_TIAN_XIANG_CHANGED,0,0);
}

void KTaisuiWheel::ProcessReset(BYTE * pSubMsg)
{
    S2C_TAISUI_RESET * pProtocol = (S2C_TAISUI_RESET *) pSubMsg;
	m_AvailableTimes = pProtocol->m_AvailableTimes;
	m_WheeledTimes   = pProtocol->m_WheeledTimes;
}

void KTaisuiWheel::ProcessTianGan(BYTE * pSubMsg)
{
	S2C_CAL_TIAN_GAN    *  pProtocol = (S2C_CAL_TIAN_GAN *) pSubMsg;
	m_TianGanRes = pProtocol->m_TianGan;

	CoreDataChanged(GDCNI_TAISUI_WHEEL_TIANGAN_RES,0,0);
}

void KTaisuiWheel::ProcessGift(BYTE * pSubMsg)
{
    S2C_CAL_GIFT_RES    *  pProtocol = (S2C_CAL_GIFT_RES *) pSubMsg;
	m_DizhiRes = pProtocol->m_DizhiRes;

	CoreDataChanged(GDCNI_TAISUI_WHEEL_DIZHI_RES,pProtocol->m_Grand,pProtocol->m_JiaziEvent);
}

unsigned long    KTaisuiWheel::GetCurrentDay()
{
	_ASSERT(m_IsTianXiangInited);
	return m_Day;
}      

unsigned long    KTaisuiWheel::GetCurrentMonth(void)
{
	_ASSERT(m_IsTianXiangInited);
	return m_Month;
}

unsigned long    KTaisuiWheel::GetActivatingJiazi(void)
{
	_ASSERT(m_IsJiaziEventInited);
	return m_JiazeActivatedIndex;
}

void             KTaisuiWheel::ReFresh()
{
    m_WheeledTimes=0;
	m_AvailableTimes=0;
	m_IsInit=false;
	m_TianGanRes=0;
	m_DizhiRes=0;
    m_Day=0;
	m_Month=0;
	m_IsTianXiangInited=false;
	m_IsWheelInited=false;
	m_IsJiaziEventInited=false;
	m_JiazeActivatedIndex=0;
	CoreDataChanged(GDCNI_TAISUI_DLG_CLOSE,0,0);
}
#endif
