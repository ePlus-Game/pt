#ifndef K_TAISUI_WHEEL_SETTING_H
#define K_TAISUI_WHEEL_SETTING_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/23/2007 9:14
//      File_base        : KTaisuiWheelSettingMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ITaisuiWheelSettingMgr 的接口实现
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifdef _SERVER
#include "ITaisuiWheelSettingMgr.h"

//Taisui Path
#define  TAISUI_SYSTEM_SETTING_PATH  "\\settings\\taisuiwheelsys.ini"
#define  TAISUI_RULE_SETTING_PATH    "\\settings\\taisuiwheelrule.ini"
#define  TAISUI_PROBABILITY_PATH     "\\settings\\taisuiwheelproabability.txt"

//Error Code defines here
#define  TAISUI_SETTING_SUC          0x00000000
#define  INI_FILE_OPEN_ERROR         0x00000001

class KTaisuiWheelSettingMgr:public ITaisuiWheelSettingMgr
{
	unsigned long          m_JiaziPassInterval;                                                             //Minite per step
	unsigned long          m_FreeTimesPerDay;
	unsigned long          m_TotalTimesPerDay;
	unsigned long          m_SystemResetTime;
	unsigned long          m_DaySameBuff;
	unsigned long          m_MonthSameBuff;
	unsigned long          m_ShowResBuff;
	
	unsigned long          m_JiaziEventID[JIAZI_EVENT_NUM];                                                  //Jiazi Event
	//Gift ...
	unsigned long          m_GradID[MAX_GRAD-1];           //Normal 状态下不用填
	unsigned long          m_TianGanBuff[MAX_TIANGAN_NUM];
	unsigned long          m_DizhiBuff[MAX_DIZI_NUM];    
	
	TIAN_XIANG_PROBABILITY m_Probability[MAX_TIANGAN_NUM][MAX_DIZI_NUM]; 	
	float                  m_Degress;
	
	bool                   m_InitFlag;
public:
    KTaisuiWheelSettingMgr(void);
	~KTaisuiWheelSettingMgr(void);
public:
	bool                   IsLoadedSuccess(void)const;
	unsigned long          GetSysResetTime(void)const;
	unsigned long          GetFreeTimes(void)const;                                     //获取允许客户端功能重置间隔时间范围内的免费次数
	unsigned long          GetTotalTimes(void)const;                                    //获取允许客户端功能重置间隔时间范围内的总次数
	unsigned long          GetJiaziInterval(void)const;                                 //获取服务器甲子走动频率，以服务器分钟为单位
    unsigned long          GetJiaziEventInfo(const unsigned long dwEventIndex)const;    //获取甲子事件ID	
	virtual unsigned long  GetDaySameBuffID(void)const;
	virtual unsigned long  GetMonthSameBuffID(void)const;
	//Givft Setting
	void                   GetGiftExpression(const unsigned long dwTianGan,const unsigned long dwDizhi,const TS_GIFT_GRAD dwGiftGrad ,TS_GIFT * pGift=NULL)const;        //获取礼物表达式
    //Wheel Setting
	TIAN_XIANG_PROBABILITY GetProbability(const unsigned long dwTianGan,const unsigned long dwDizhi);
	float                  GetDregress(void)const;
	unsigned long          GetShowResBuffID(void)const;
private:
	void				   Init(void);
	void				   Release(void);
private:
	bool                   LoadSystemSetting(const char * szSystemSettingName);
	bool				   LoadRuleSetting(const char * szRuleSettingName);
	bool				   LoadProbability(const char * szProbabilityName);        
	TIAN_XIANG_PROBABILITY ParseStringToProbability(char * szProbability)const;
	unsigned long          ParseInteger(char * & pString)const;
};

#endif

#endif
