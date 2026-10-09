#include "KCore.h"
#ifdef    _SERVER
#include "KTaisuiWheelSettingMgr.h"

#define DEFAULT_TAISUI_PASS_INTERVAL  1
#define DEFAULT_TAISUI_FREE_TIMES     1
#define DEFAULT_TAISUI_TOTAL_TIMES    1
#define DEFAULT_TAISUI_DEGRESS        0.5
#define DEFAULT_TAISUI_SYS_RESET_TIME 0
#define DEFAULT_TAISUI_BUFF_ID        0
#define DEFAULT_TAISUI_PROBABILITY    2
#define DEFAULT_DAY_SAME_BUFF         0
#define DEFAULT_MONTH_SAME_BUFF       0
#define DEFAULT_SHOW_RES_BUFF         0              

#define TaisuiLogOut(x)       CFS_FILELOGS::WriteDebugLog(x);
#define TaisuiLogOutMore(x,y) CFS_FILELOGS::WriteDebugLog(x,y);


#define MAX_PROBABILITY_SPACE     11999 

KTaisuiWheelSettingMgr::KTaisuiWheelSettingMgr()
:m_JiaziPassInterval(DEFAULT_TAISUI_PASS_INTERVAL),
 m_FreeTimesPerDay(DEFAULT_TAISUI_FREE_TIMES)
,m_TotalTimesPerDay(DEFAULT_TAISUI_TOTAL_TIMES),
 m_SystemResetTime(DEFAULT_TAISUI_SYS_RESET_TIME),
 m_Degress(DEFAULT_TAISUI_DEGRESS),
 m_DaySameBuff(DEFAULT_DAY_SAME_BUFF),
 m_MonthSameBuff(DEFAULT_MONTH_SAME_BUFF),
 m_ShowResBuff(DEFAULT_SHOW_RES_BUFF),
 m_InitFlag(false)
{
	memset(m_GradID,0,sizeof(m_GradID));
	memset(m_TianGanBuff,0,sizeof(m_TianGanBuff));
	memset(m_DizhiBuff,0,sizeof(m_DizhiBuff));

	for (int iEvnet=0;iEvnet<JIAZI_EVENT_NUM;iEvnet++)
	{
        m_JiaziEventID[iEvnet]=DEFAULT_TAISUI_BUFF_ID;
	}//endforiEvent

	for (int iTianGan=1;iTianGan<=MAX_TIANGAN_NUM;iTianGan++)
		for (int iDizhi=1;iDizhi<=MAX_DIZI_NUM;iDizhi++)
		{
            m_Probability[iTianGan-1][iDizhi-1].m_TianGan=DEFAULT_TAISUI_PROBABILITY;
			if ((iTianGan % 2 ==0 && iDizhi %2 ==0) || (iTianGan % 2 ==1 && iDizhi %2 ==1))
            {
				m_Probability[iTianGan-1][iDizhi-1].m_DiZhi=DEFAULT_TAISUI_PROBABILITY;
			}//endif
            else 
				m_Probability[iTianGan-1][iDizhi-1].m_DiZhi=0;
		}//end

	Init();
}

bool KTaisuiWheelSettingMgr::IsLoadedSuccess()const
{
	return m_InitFlag;
}

KTaisuiWheelSettingMgr::~KTaisuiWheelSettingMgr()
{
	Release();
}

void KTaisuiWheelSettingMgr::Release()
{
    m_InitFlag=false;

   	memset(m_GradID,0,sizeof(m_GradID));
	memset(m_TianGanBuff,0,sizeof(m_TianGanBuff));
	memset(m_DizhiBuff,0,sizeof(m_DizhiBuff));
}

void KTaisuiWheelSettingMgr::Init()
{
    if (!m_InitFlag)
	{
	  bool bSuc=false;	
	  bSuc=LoadSystemSetting(TAISUI_SYSTEM_SETTING_PATH);
	  _ASSERT(bSuc);

	  bSuc=LoadRuleSetting(TAISUI_RULE_SETTING_PATH);
	  _ASSERT(bSuc);

	  bSuc=LoadProbability(TAISUI_PROBABILITY_PATH);
	  _ASSERT(bSuc);

      m_InitFlag=true;
	}//endif
}

bool  KTaisuiWheelSettingMgr::LoadSystemSetting(const char * szSystemSettingName)
{
	_ASSERT(szSystemSettingName!=NULL);
    KIniFile  kSysFile;
	if (!kSysFile.Load(szSystemSettingName))
	{
        TaisuiLogOut("[Taisui]System settings file read failed!\n")
		TaisuiLogOut("[Taisui]Set to the default system setting...\n")
		return false;
	}//endif
    
	int res=0;

	//Load Normal setting
    res=kSysFile.GetInteger("SystemSetting","GlobalJiaziInterval",DEFAULT_TAISUI_PASS_INTERVAL,(int *)&m_JiaziPassInterval);

	if (!res || m_JiaziPassInterval==0)
	{
		TaisuiLogOut("[Taisui] GlobalJiaziInterval is InvalidValue!\n")
		m_JiaziPassInterval=DEFAULT_TAISUI_PASS_INTERVAL;	
        return false;   
	}//endif

	TaisuiLogOutMore("[Taisui]JiaziInterval=%d\n",m_JiaziPassInterval);

	res=kSysFile.GetInteger("SystemSetting","FreeTimesPerDay",DEFAULT_TAISUI_FREE_TIMES,(int *)&m_FreeTimesPerDay);

	if (!res || m_FreeTimesPerDay==0)
	{
		TaisuiLogOut("[Taisui] FreeTimesPerDay is InvalidValue!\n")
		m_FreeTimesPerDay=DEFAULT_TAISUI_FREE_TIMES;	
        return false;
	}//endif
    
	TaisuiLogOutMore("[Taisui]FreeTimesPerDay=%d\n",m_FreeTimesPerDay);

	res=kSysFile.GetInteger("SystemSetting","TotalTimesPerDay",DEFAULT_TAISUI_TOTAL_TIMES,(int *)&m_TotalTimesPerDay);

	if(!res || m_TotalTimesPerDay==0)
	{
		TaisuiLogOut("[Taisui] TotalTimePerDay is InvalidValue!\n")
		m_TotalTimesPerDay=DEFAULT_TAISUI_TOTAL_TIMES;	
		return false;
	}//endif

	TaisuiLogOutMore("[Taisui]m_TotalTimesPerDay=%d\n",m_TotalTimesPerDay);

	res=kSysFile.GetInteger("SystemSetting","SysResetTime",DEFAULT_TAISUI_SYS_RESET_TIME,(int *)&m_SystemResetTime);

	if (!res || m_SystemResetTime>=24)
	{
		TaisuiLogOut("[Taisui] SysResetTime is InvalidValue!\n")
		m_SystemResetTime=DEFAULT_TAISUI_SYS_RESET_TIME;	
		return false;
	}//endif

	TaisuiLogOutMore("[Taisui] SysResetTime =%d\n",m_SystemResetTime);

	//Load Global Degress Info
	res=kSysFile.GetFloat("SystemSetting","GlobalJiaziDegress",DEFAULT_TAISUI_DEGRESS,(float *)&m_Degress);

	if (!res || m_Degress >10)
	{
		TaisuiLogOut("[Taisui] GlobalJiaziDegress is InvalidValue,<=100 isNeeded!\n");
		m_Degress=DEFAULT_TAISUI_DEGRESS;	
	}//endif

	TaisuiLogOutMore("[Taisui] GlobalJiaziDegress =%f\n",m_Degress);

	//Wheel Buff ID
	res=kSysFile.GetInteger("SystemSetting","DaySameBuff",DEFAULT_DAY_SAME_BUFF,(int *)&m_DaySameBuff);
	
	if (!res || m_DaySameBuff==0)
	{
		TaisuiLogOut("[Taisui] Day same buff is InvalidValue!\n")
		m_DaySameBuff=DEFAULT_DAY_SAME_BUFF;	
		return false;
	}//endif
	
	TaisuiLogOutMore("[Taisui] Day Same BuffID =%d\n",m_DaySameBuff);

	res=kSysFile.GetInteger("SystemSetting","MonthSameBuff",DEFAULT_MONTH_SAME_BUFF,(int *)&m_MonthSameBuff);
	
	if (!res || m_MonthSameBuff==0)
	{
		TaisuiLogOut("[Taisui] Month same buff is InvalidValue!\n")
		m_MonthSameBuff=DEFAULT_MONTH_SAME_BUFF;	
		return false;
	}//endif
	
	TaisuiLogOutMore("[Taisui] Month Same BuffID =%d\n",m_MonthSameBuff);
	
	res=kSysFile.GetInteger("SystemSetting","ShowResBuff",DEFAULT_SHOW_RES_BUFF,(int *)&m_ShowResBuff);
	
	if (!res)
	{
		TaisuiLogOut("[Taisui] Show Res buff is InvalidValue!\n")
		m_ShowResBuff=DEFAULT_SHOW_RES_BUFF;	
		return false;
	}//endif
	
	TaisuiLogOutMore("[Taisui] Show Res BuffID =%d\n",m_ShowResBuff);
	
	//Load Jiazi Evnet Setting
	const char * szKeyHead="Event%d";
	char  szKeyName[16];

	for (int i=1;i<=JIAZI_EVENT_NUM;i++)
	{
         sprintf(szKeyName,szKeyHead,i);
		 res=kSysFile.GetInteger("JiaziEvent",szKeyName,DEFAULT_TAISUI_BUFF_ID,(int *)&(m_JiaziEventID[i-1]));
		 
		 if (!res)
		 {
			 TaisuiLogOut("[Taisui] Event is InvalidValue! SetToDefault....\n")
   		     m_JiaziEventID[i-1]=DEFAULT_TAISUI_BUFF_ID;
			 return false;
		 }//endif

		 TaisuiLogOutMore("[Taisui]TaisuiEventBuffID =%d\n",m_JiaziEventID[i-1]);

	}//end for i

	return true;
}

#define MAX_RULE_KEY_LEN 8
static  char szTianGanKey[MAX_TIANGAN_NUM][MAX_RULE_KEY_LEN]=
{
	"Jia","Yi","Bing","Ding","Wu","Ji","Geng","Xin","Ren","Gui"
};

static  char szDizhiKey[MAX_DIZI_NUM][MAX_RULE_KEY_LEN]=
{
	"Zi","Chou","Yin","Mao","Chen","Si","Wu","Wei","Shen","You","Xu","Hai"
}; 

bool  KTaisuiWheelSettingMgr::LoadRuleSetting(const char * szRuleSettingName)
{
	_ASSERT(szRuleSettingName!=NULL);

	KIniFile kProFile;
	if(!kProFile.Load(szRuleSettingName))
	{
        TaisuiLogOut("[Taisui] Load Gift Rule failed,Set to default.\n");
		return false;
	}//endif

	bool    bFinalRes=true;

	for (int iTianGan=0;iTianGan<MAX_TIANGAN_NUM;iTianGan++)
	{
		int res=0;
        res=kProFile.GetInteger("TianGanBuff",szTianGanKey[iTianGan],DEFAULT_TAISUI_BUFF_ID,(int *)&(m_TianGanBuff[iTianGan]));

		if (res==0)
		{
		    m_TianGanBuff[iTianGan]=DEFAULT_TAISUI_BUFF_ID;
			TaisuiLogOutMore("TianGanBuffID %d IsInvalid! SetToDefault!\n",iTianGan)
            bFinalRes=false;
		}//endif

	}//endfor iTianGan

	for (int iDizhi=0;iDizhi<MAX_DIZI_NUM;iDizhi++)
	{
		int res=0;
        res=kProFile.GetInteger("DiZhiBuff",szDizhiKey[iDizhi],DEFAULT_TAISUI_BUFF_ID,(int *)&(m_DizhiBuff[iDizhi]));
		
		if (res==0)
		{
			m_DizhiBuff[iDizhi]=DEFAULT_TAISUI_BUFF_ID;
			TaisuiLogOutMore("TianDizhiBuffID %d IsInvalid! SetToDefault!\n",iDizhi)
            bFinalRes=false;
		}//endif
	}//endfor iTianGan

	int  res=0;
	//YueRi
	res=kProFile.GetInteger("GiftAdditionBuff","DaySame",DEFAULT_TAISUI_BUFF_ID,(int *)&(m_GradID[0]));
	if (!res)
	{
		m_GradID[0]=DEFAULT_TAISUI_BUFF_ID;
		TaisuiLogOut("Day Same BuffID is Invalid!\n")
        bFinalRes=false;
	}//endif

	res=kProFile.GetInteger("GiftAdditionBuff","MonthSame",DEFAULT_TAISUI_BUFF_ID,(int *)&(m_GradID[1]));
	
	if (!res)
	{
		m_GradID[1]=DEFAULT_TAISUI_BUFF_ID;
		TaisuiLogOut("Month Same BuffID is Invalid!\n")
		bFinalRes=false;
	}//endif

	res=kProFile.GetInteger("GiftAdditionBuff","DayAndMonthSame",DEFAULT_TAISUI_BUFF_ID,(int *)&(m_GradID[2]));
	
	if (!res)
	{
		m_GradID[2]=DEFAULT_TAISUI_BUFF_ID;
		TaisuiLogOut("[Taisui]DayAndMonth Same BuffID is Invalid!\n")
		bFinalRes=false;
	}//endif

	TaisuiLogOut("[Taisui]Taisui Rule loaded Complete!\n");
    return bFinalRes;
}

bool  KTaisuiWheelSettingMgr::LoadProbability(const char * szProbabilityName)
{
	_ASSERT(szProbabilityName!=NULL);
	
	KTabFile kProFile;
	if(!kProFile.Load(szProbabilityName))
	{
		TaisuiLogOut("[Taisui] Load Probability failed,Set to default.\n");
		return false;
	}//endif

	char     szStringBuffer[256];
	memset(szStringBuffer,0,256);

	for (int iTianGan=1;iTianGan<=MAX_TIANGAN_NUM;iTianGan++)
	{
		int nProbabilitySpace = 0;

		for (int iDizhi=1;iDizhi<=MAX_DIZI_NUM;iDizhi++)
		{
			BOOL Res=TRUE;
			kProFile.GetString(iTianGan+1,iDizhi+1,"",szStringBuffer,256);
			TIAN_XIANG_PROBABILITY Probability=ParseStringToProbability(szStringBuffer);
			
			if (Probability.m_TianGan<0 || Probability.m_TianGan>100 ||Probability.m_DiZhi<0 || Probability.m_DiZhi>100)
			{
				TaisuiLogOut("[Taisui]Taisui Probability loaded Failed Setit to default!\n");	
            }//endif
            else
			{
				m_Probability[iTianGan-1][iDizhi-1].m_TianGan=Probability.m_TianGan;
				m_Probability[iTianGan-1][iDizhi-1].m_DiZhi=Probability.m_DiZhi;
			}//end else

			//Check the Probability
			if (iTianGan % 2 == 0)
			{
                if (iDizhi % 2 == 1)
				{
                    _ASSERT(m_Probability[iTianGan-1][iDizhi-1].m_DiZhi == 0);
					if (m_Probability[iTianGan-1][iDizhi-1].m_DiZhi !=0)
					{
                        m_Probability[iTianGan-1][iDizhi-1].m_DiZhi = 0;
					}//endif

				}//endif

			}//endif
			else
			{
				if (iDizhi % 2 == 0)
				{
                    _ASSERT(m_Probability[iTianGan-1][iDizhi-1].m_DiZhi == 0);
					if (m_Probability[iTianGan-1][iDizhi-1].m_DiZhi !=0)
					{
                        m_Probability[iTianGan-1][iDizhi-1].m_DiZhi = 0;
					}//endif
					
				}//endif

			}//end else
			
			//Check the ability space
			if (nProbabilitySpace + m_Probability[iTianGan-1][iDizhi-1].m_DiZhi > MAX_PROBABILITY_SPACE )
			{
				m_Probability[iTianGan-1][iDizhi-1].m_DiZhi = 0;
			}//endif

            #ifdef _DEBUG
			if (m_Probability[iTianGan-1][iDizhi-1].m_DiZhi!=0)
			{
				_ASSERT(TianGanDiZhiToTianXiang(iTianGan,iDizhi)!=0xffffffff);
			}//endif
            #endif
		}//end for dizhi	

	}//end for itiangan

	TaisuiLogOut("[Taisui]Taisui Probability loaded Complete!\n");	
	
	return true;
}   



TIAN_XIANG_PROBABILITY KTaisuiWheelSettingMgr::ParseStringToProbability(char * szProbability)const
{
  TIAN_XIANG_PROBABILITY res;
  if (szProbability[0]==0)
	  return res;
  else
  {
	  res.m_TianGan = ParseInteger(szProbability);
	  if (szProbability[0]=='|')
	  {		
		  ++szProbability;
		  res.m_DiZhi   = ParseInteger(szProbability);
	  }

	 return res;
  }
}

unsigned long KTaisuiWheelSettingMgr::GetDaySameBuffID()const
{
	return m_DaySameBuff;
}

unsigned long KTaisuiWheelSettingMgr::GetMonthSameBuffID()const
{
	return m_MonthSameBuff;
}

unsigned long KTaisuiWheelSettingMgr::ParseInteger(char * & pString)const
{
  unsigned long dwRes=0;
  while( (pString[0]!='|') && pString[0]!=0)
  {
	  dwRes=dwRes*10+pString[0]-(unsigned long)('0');
	  ++pString;
  }//end for while
  return dwRes;
}

unsigned long KTaisuiWheelSettingMgr::GetFreeTimes(void)const
{
	//_ASSERT(m_InitFlag);
	return m_FreeTimesPerDay;
}

unsigned long KTaisuiWheelSettingMgr::GetTotalTimes(void)const
{
//	_ASSERT(m_InitFlag);
	return m_TotalTimesPerDay;
}

unsigned long KTaisuiWheelSettingMgr::GetJiaziInterval(void)const
{
 //   _ASSERT(m_InitFlag);
	return m_JiaziPassInterval;
}	
  
unsigned long KTaisuiWheelSettingMgr::GetJiaziEventInfo(const unsigned long dwEventIndex)const
{
   // _ASSERT(m_InitFlag && dwEventIndex<=JIAZI_EVENT_NUM);
	return m_JiaziEventID[dwEventIndex-1];
}

void  KTaisuiWheelSettingMgr::GetGiftExpression(const unsigned long dwTianGan,const unsigned long dwDizhi ,const TS_GIFT_GRAD dwGiftGrad ,TS_GIFT * pGift)const
{
  //  _ASSERT(m_InitFlag && dwTianGan<=MAX_TIANGAN_NUM && dwDizhi<=MAX_DIZI_NUM && dwGiftGrad<=MAX_GRAD-1);
    if(pGift)
	{
		pGift->dwTianGanBuff=m_TianGanBuff[dwTianGan-1];
		pGift->dwDizhiBuff=m_DizhiBuff[dwDizhi-1];
		pGift->dwAdditionBuff=0;
		if(dwGiftGrad>0)
			pGift->dwAdditionBuff=m_GradID[dwGiftGrad-1];
		else pGift->dwAdditionBuff=0;
	}//endif
}

unsigned long KTaisuiWheelSettingMgr::GetSysResetTime(void)const
{
//	_ASSERT(m_InitFlag);
    return m_SystemResetTime;
}

//Wheel Setting
TIAN_XIANG_PROBABILITY KTaisuiWheelSettingMgr::GetProbability(const unsigned long dwTianGan,const unsigned long dwDizhi)
{
 //   _ASSERT(m_InitFlag && dwTianGan<=MAX_TIANGAN_NUM && dwDizhi<=MAX_DIZI_NUM);
    return m_Probability[dwTianGan-1][dwDizhi-1];
}

float         KTaisuiWheelSettingMgr::GetDregress(void)const
{
//	_ASSERT(m_InitFlag);
	return m_Degress;
}

unsigned long  KTaisuiWheelSettingMgr::GetShowResBuffID(void)const
{
    return m_ShowResBuff;
}

ITaisuiWheelSettingMgr * GetMainTaisuiWheelSettingMgr(void)
{
	static ITaisuiWheelSettingMgr * pManager=NULL;
	if (pManager==NULL)	
		pManager=new KTaisuiWheelSettingMgr;
    
	_ASSERT(pManager!=NULL);
    return pManager;
}

void                     ReleaseMainTaisuiWheelSettingMgr(void)
{
    delete GetMainTaisuiWheelSettingMgr();
}

#endif
