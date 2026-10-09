#include "KCore.h"
#include "ITaisuiWheelSettingMgr.h"
#include "KTaisuiWheelTianXiangMgr.h"
#include "KTaisuiWheelServer.h"

#define DEFAULT_TIAN_XIANG_DAY   1
#define DEFAULT_TIAN_XIANG_MONTH 1

KTaisuiWheelTianXiangMgr::KTaisuiWheelTianXiangMgr():m_CurTianXiangDay(DEFAULT_TIAN_XIANG_DAY),m_CurTianXiangMonth(DEFAULT_TIAN_XIANG_MONTH)
,m_TimeCounter(UNIX_TMIE_STAMP),m_TimeInterval(GetMainTaisuiWheelSettingMgr()->GetJiaziInterval()*60),m_IsSuspend(false),m_SuspendPass(0),m_IsInited(true)
{
} 

KTaisuiWheelTianXiangMgr::~KTaisuiWheelTianXiangMgr()
{}

void KTaisuiWheelTianXiangMgr::Suspend()
{
    m_IsSuspend=true;
	m_SuspendPass=UNIX_TMIE_STAMP-m_TimeCounter;
}

void KTaisuiWheelTianXiangMgr::Resume()
{
    m_IsSuspend=false;
	m_TimeCounter=UNIX_TMIE_STAMP+m_SuspendPass;
}

void KTaisuiWheelTianXiangMgr::Breathe()
{
    if (m_IsInited && !m_IsSuspend)
	{
		time_t tCurrent=UNIX_TMIE_STAMP;
		if (tCurrent-m_TimeCounter>=m_TimeInterval)
		{
			m_CurTianXiangDay+=1;

			if (m_CurTianXiangDay==MAX_JIAZI_NUM/2+1 || m_CurTianXiangDay==MAX_JIAZI_NUM+1)
			{
				m_CurTianXiangMonth+=1;
				if (m_CurTianXiangMonth>MAX_JIAZI_NUM)
					m_CurTianXiangMonth=1;
			}//endif

			if (m_CurTianXiangDay>MAX_JIAZI_NUM)
					m_CurTianXiangDay=1;
	
			SaveToDB();
			m_TimeCounter=tCurrent;
		}//endif
	}//endif
}

unsigned long KTaisuiWheelTianXiangMgr::GetCurrentTianXiangDay()const
{
//	_ASSERT(m_IsInited);
    return m_CurTianXiangDay;
}

void KTaisuiWheelTianXiangMgr::SetCurrentTianXiangDay(unsigned long dwTianXiangIndex)
{
//	_ASSERT(m_IsInited);
	if (dwTianXiangIndex != m_CurTianXiangDay)
	{
        m_CurTianXiangDay=dwTianXiangIndex;
		SaveToDB();
	}//endif
}

unsigned long KTaisuiWheelTianXiangMgr::GetCurrentTianXiangMonth()const
{	
//	_ASSERT(m_IsInited);
    return m_CurTianXiangMonth;
}

void KTaisuiWheelTianXiangMgr::SetCurrentTianXiangMonth(unsigned long dwTianXiangIndex)
{
//	_ASSERT(m_IsInited);
	if (dwTianXiangIndex != m_CurTianXiangMonth)
	{
        m_CurTianXiangMonth=dwTianXiangIndex;
		SaveToDB();
	}//endif
}


ITianXiangMgr *  GetMainTianXiangMgr()
{
	static ITianXiangMgr * pManager=NULL;
	if (pManager==NULL)
	{
		pManager=new KTaisuiWheelTianXiangMgr;
	}//endif
	
	return pManager;
}

void          ReleaseMainTianXiangMgr()
{
    delete GetMainTianXiangMgr();
}

void          KTaisuiWheelTianXiangMgr::LoadFromDB(void)
{
	//Need to do here
    unsigned long dwDBDay=GetTaisuiTianXiangDay();
	if (dwDBDay>0 && dwDBDay<=MAX_JIAZI_NUM)
		m_CurTianXiangDay=dwDBDay;	
    else
		SaveToDB();   //Initialize db

	unsigned long dwDBMonth=GetTaisuiTianXiangMonth();
	if (dwDBMonth>0 && dwDBDay<=MAX_JIAZI_NUM)
		m_CurTianXiangMonth=dwDBMonth;
	else 
		SaveToDB();   //Initilize db

	//This may change when DB is used
	m_IsInited=true;
}

void          KTaisuiWheelTianXiangMgr::SaveToDB(void)const
{
	//Need to do here
	if (m_CurTianXiangDay<=MAX_JIAZI_NUM && m_CurTianXiangDay>0)
		SetTaisuiTianXiangDay(m_CurTianXiangDay);
    
	if (m_CurTianXiangMonth<=MAX_JIAZI_NUM && m_CurTianXiangMonth>0)
		SetTaisuiTianXiangMonth(m_CurTianXiangMonth);
	
	SaveTaisuiGlobal();
}