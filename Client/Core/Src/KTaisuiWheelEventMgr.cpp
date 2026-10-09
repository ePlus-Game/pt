#include "KCore.h"
#include "KTaisuiWheelEventMgr.h"
#include "ITaisuiWheelSettingMgr.h"
#include "JiaziCommonDef.h"
#include "KTaisuiWheelServer.h"

KTaisuiWheelEventMgr::KTaisuiWheelEventMgr():m_CurActivateIndex(0),m_IsInited(true)
{
}

KTaisuiWheelEventMgr::~KTaisuiWheelEventMgr()
{}

void KTaisuiWheelEventMgr::BreatheRes(const unsigned long dwTianGan,const unsigned long dwDiZhi)
{
	_ASSERT(m_IsInited);

	unsigned long dwJiaziIndex=TianGanDiZhiToTianXiang(dwTianGan,dwDiZhi);
    if (dwJiaziIndex==m_CurActivateIndex+1)
	{
       m_CurActivateIndex=dwJiaziIndex;
	   
	   if (m_CurActivateIndex==MAX_JIAZI_NUM)
		   m_CurActivateIndex=0;

	   SaveToDB();
	}//endif
}

unsigned long KTaisuiWheelEventMgr::GetNextTianXiangToActivate()const
{
	_ASSERT(m_IsInited);
	_ASSERT(m_CurActivateIndex+1>=0 && m_CurActivateIndex<=MAX_JIAZI_NUM);
	return m_CurActivateIndex+1;
}

void          KTaisuiWheelEventMgr::LoadFromDB()
{
	//Need to do here! This may edit later
	unsigned long dwDBValue=GetCurTaisuiEvent();
	if (dwDBValue>=0 && dwDBValue<=MAX_JIAZI_NUM)
		m_CurActivateIndex=dwDBValue;
	else 
		SaveToDB();   //Init DB

	m_IsInited=true;
}

void          KTaisuiWheelEventMgr::SaveToDB()
{
   //Need to do here!
	if (m_CurActivateIndex<=MAX_JIAZI_NUM && m_CurActivateIndex>=0)
		SetCurTaisuiEvent(m_CurActivateIndex);	
    SaveTaisuiGlobal();
}

ITaisuiWheelEventMgr * GetMainTaisuiWheelEventMgr(void)
{
  static ITaisuiWheelEventMgr * pManager=NULL;
  if (pManager==NULL)
  {
      pManager= new KTaisuiWheelEventMgr;
  }//endif
  
  return pManager;
}

void                   ReleaseMainTaisuiWheelEventMgr(void)
{
  delete GetMainTaisuiWheelEventMgr();
}

