#ifndef INSURANCE_SETTING_MGR_H
#define INSURANCE_SETTING_MGR_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 04/18/2008 9:33
//      File_base        : insurance_setting_mgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 保险配置读取
//
//      <Change_list>    
//////////////////////////////////////////////////////////////////////

#pragma pack(push,1)

enum InsuranceOpCode
{
	insurance_error_begin = 0,
	insurance_error_none ,
	insurance_error_add_failed,
	insurance_error_add_wrong_level,
	insurance_error_add_max_exeed,
	insurance_error_add_min_lower,
	insurance_error_add_pay_failed,

	insurance_error_fetch_failed,
	insurance_error_fetch_not_enough,
	insurance_error_fetch_too_much,

	insurance_error_end
};

enum InsuranceS2CSubProtocol
{
	s2c_insurance_info_sync  = 1,
	s2c_insurance_op_code,
	s2c_insurance_end,
};

enum InsuranceC2SSubProtocol
{
	c2s_add_insurance_value  = 1,
	c2s_get_insurance_money,
	c2s_insurance_end,		
};

typedef struct tagS2C_INSURANCE_COMMON_HEADER
{
	BYTE    nProtocol;
	WORD    nLen;
	BYTE    nSubProtocol;
}S2C_INSURANCE_COMMON_HEADER;

struct S2C_INSURANCE_BASE_INFO:public tagS2C_INSURANCE_COMMON_HEADER
{
	INT     nCurrentInsuranceValue;
	INT     nTotalMoneyGot;           
	INT     nMoneyLeftToGet;      
};

struct S2C_INSURANCE_OP_CODE:public tagS2C_INSURANCE_COMMON_HEADER
{
	BYTE    nOpCode;
};

typedef struct tagC2S_INSURANCE_PROTOCOL
{
	BYTE    nProtocol;
	WORD    nLen;
	BYTE    nSubProtocol;
	int     nParam;
}C2S_INSURANCE_PROTOCOL;

#pragma  pack (pop)

#define MAX_INSURANCE_REWARD_PLAYER_LEVEL 120                                    //增值保险涉及到的玩家最高等级
#define INSURANCE_SETTINGS_PATH           "\\settings\\insurance.ini"
#define MAX_ERROR_CODE_LEN                64
#define MAX_NOTIFY_MSG_LEN                128
#define MAX_NOTIFY_MSG_NUM                20

class InsuranceSettingMgr
{
	bool m_bEnabledManully;
	bool m_bLoaded;

	//Common settings .............................................
	int  m_ImediateRewardRate;
	int  m_MinInsuranceValue;
	int  m_MaxInsuranceValue;
	int  m_MinBuyInsuranceLevel;
	int  m_MaxBuyInsuranceLevel;
	int  m_TongbaoJinShanbiRate_Tongbao;
	int  m_TongbaoJinShanbiRate_Jinshanbi;
	//Level reward rates ..........................................
	int  m_nInsuranceRates[MAX_INSURANCE_REWARD_PLAYER_LEVEL + 1];

    #ifndef _SERVER
	char m_ErrorCode[insurance_error_end][MAX_ERROR_CODE_LEN];
	char m_NotifyMsg[MAX_NOTIFY_MSG_NUM][MAX_NOTIFY_MSG_LEN];
    #endif

public:
	InsuranceSettingMgr(void);
	~InsuranceSettingMgr(void);

	static InsuranceSettingMgr & Singleton(void);

public:

	void Load(const char * szSetting);
	bool IsEnabled(void)const;       
	int  GetInsuranceRateByLevel(int nPlayerLevel)const;
	int  GetImmediateRewardRate(void)const;
	int  GetMinInsuranceValue(void)const;
	int  GetMaxInsuranceValue(void)const;
	int  GetMinBuyInsuranceLevel(void)const;
	int  GetMaxBuyInsuranceLevel(void)const;
	int  InsuranceToJinShanBi(const int nInsuranceValue)const;

    #ifndef _SERVER
	const char * GetErrorCodeString(const InsuranceOpCode code);
	const char * GetNotifyCodeString(const int nNotiy);
    #endif

private: // Forbidden to use
	InsuranceSettingMgr(const InsuranceSettingMgr &);
	const InsuranceSettingMgr & operator = (const InsuranceSettingMgr &);
};

#endif