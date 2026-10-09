#include "KCore.h"
#include "insurance_common.h"

InsuranceSettingMgr & InsuranceSettingMgr::Singleton()
{
	static InsuranceSettingMgr mgr;
	return mgr;
}

InsuranceSettingMgr::InsuranceSettingMgr()
:m_bEnabledManully(false),m_bLoaded(false),
 m_ImediateRewardRate(0),m_MinInsuranceValue(0),m_MaxInsuranceValue(0),m_MinBuyInsuranceLevel(0),m_MaxBuyInsuranceLevel(0),
 m_TongbaoJinShanbiRate_Tongbao(1),m_TongbaoJinShanbiRate_Jinshanbi(0)
{
	ZeroMemory(m_nInsuranceRates,sizeof(m_nInsuranceRates));
    #ifndef _SERVER
	ZeroMemory(m_ErrorCode,sizeof(m_ErrorCode));
	ZeroMemory(m_NotifyMsg,sizeof(m_NotifyMsg));
    #endif
}

InsuranceSettingMgr::~InsuranceSettingMgr()
{}

void InsuranceSettingMgr::Load(const char * szSetting)
{
	if (szSetting == NULL)
		return ;

	KIniFile  kSetting;
	if (!kSetting.Load(szSetting))
	{
		return ;
	}//endif

	kSetting.GetInteger("CommonSettings","ImmediateRewardRate",0,&m_ImediateRewardRate);
	kSetting.GetInteger("CommonSettings","MinInsuranceValue",0,&m_MinInsuranceValue);
    kSetting.GetInteger("CommonSettings","MaxInsuranceValue",0,&m_MaxInsuranceValue);
	kSetting.GetInteger("CommonSettings","MinBuyInsuranceLevel",0,&m_MinBuyInsuranceLevel);
	kSetting.GetInteger("CommonSettings","MaxBuyInsuranceLevel",0,&m_MaxBuyInsuranceLevel);
	kSetting.GetInteger("CommonSettings","TongBaoToJinShanBiRate_TongBao",1,&m_TongbaoJinShanbiRate_Tongbao);
	kSetting.GetInteger("CommonSettings","TongBaoToJinShanBiRate_Jinshanbi",0,&m_TongbaoJinShanbiRate_Jinshanbi);

	if (m_TongbaoJinShanbiRate_Tongbao == 0)
		m_TongbaoJinShanbiRate_Tongbao = 1;

	
	char szSectionName[128] = "";
	for (int n = 1; n < MAX_INSURANCE_REWARD_PLAYER_LEVEL ; n ++ )
	{
		sprintf(szSectionName,"%d",n);
		kSetting.GetInteger("LevelReward",szSectionName,0,&m_nInsuranceRates[n]);	
	}//end for n

    #ifndef _SERVER
	for (int nErrorCode = insurance_error_none; nErrorCode < insurance_error_end ; nErrorCode ++ )
	{
		sprintf(szSectionName,"%d",nErrorCode);
		BOOL readResult = kSetting.GetString("ErorrCode", szSectionName, "", (char *)&(m_ErrorCode[nErrorCode]), MAX_ERROR_CODE_LEN);
		m_ErrorCode[nErrorCode][MAX_ERROR_CODE_LEN -1] = 0;
	}//end for nErrorCode

	for (int nNotify = 0 ; nNotify < MAX_NOTIFY_MSG_NUM ; nNotify ++ )
	{
		sprintf(szSectionName,"%d",nNotify);
		BOOL readResult = kSetting.GetString("NotifyMsg", szSectionName, "", (char *)&(m_NotifyMsg[nNotify]), MAX_NOTIFY_MSG_LEN);
		m_NotifyMsg[nNotify][MAX_ERROR_CODE_LEN -1] = 0;
	}
    #endif

	m_bLoaded = true;
}

bool InsuranceSettingMgr::IsEnabled()const
{
	return (m_bLoaded && m_bEnabledManully);
}


int InsuranceSettingMgr::GetImmediateRewardRate()const
{
	return m_ImediateRewardRate;
}

int InsuranceSettingMgr::GetMaxBuyInsuranceLevel()const
{
	return m_MaxBuyInsuranceLevel;
}

int InsuranceSettingMgr::GetMinBuyInsuranceLevel()const
{
	return m_MinBuyInsuranceLevel;
}

int InsuranceSettingMgr::GetMaxInsuranceValue()const
{
	return m_MaxInsuranceValue;
}

int InsuranceSettingMgr::GetMinInsuranceValue()const
{
	return m_MinInsuranceValue;
}

int InsuranceSettingMgr::GetInsuranceRateByLevel(int nPlayerLevel)const
{
	if (nPlayerLevel <= 0 || nPlayerLevel > MAX_INSURANCE_REWARD_PLAYER_LEVEL )
		return 0 ;

	return m_nInsuranceRates[nPlayerLevel];
}

#ifndef _SERVER
const char * InsuranceSettingMgr::GetErrorCodeString(const InsuranceOpCode code)
{
	if (code > insurance_error_begin && code < insurance_error_end)
		return m_ErrorCode[code];
	else
		return NULL;
}

const char * InsuranceSettingMgr::GetNotifyCodeString(const int nNotiy)
{
	if (nNotiy >=0 && nNotiy < MAX_NOTIFY_MSG_LEN)
		return m_NotifyMsg[nNotiy];
	else
		return NULL;
}
#endif

int InsuranceSettingMgr::InsuranceToJinShanBi(const int nInsuranceValue)const
{
	if (m_TongbaoJinShanbiRate_Tongbao != 0)
		return (nInsuranceValue * m_TongbaoJinShanbiRate_Jinshanbi * 10000 / m_TongbaoJinShanbiRate_Tongbao);
	else
		return 0;
}