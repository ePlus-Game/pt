#ifndef ITAISUIWHEELSETTINGMGR_H
#define ITAISUIWHEELSETTINGMGR_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/20/2007 9:44
//      File_base        : ITaisuiWheelSettingMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮配置接口声明
//
//      <Change_list>
/////////////////////////////////////////////////////////////////////

#ifdef _SERVER
#define           JIAZI_EVENT_NUM                  5
#define           TAISUI_RESET_INTERVAL            1440  //功能重置间隔分钟(24*60 一天)

#include "JiaziCommonDef.h"
#include "TaisuiWheelRule.h"

typedef struct tagTIAN_XIANG_PROBABILITY
{
    unsigned long m_TianGan;
	unsigned long m_DiZhi;
	tagTIAN_XIANG_PROBABILITY(int iTianGan=0,int iDizhi=0):m_TianGan(iTianGan),m_DiZhi(iDizhi){}
}TIAN_XIANG_PROBABILITY;

struct ITaisuiWheelSettingMgr
{
	virtual ~ITaisuiWheelSettingMgr(void){}
	//System Setting
	virtual bool                   IsLoadedSuccess(void)const=0;
	virtual unsigned long          GetFreeTimes(void)const=0;                                     //获取允许客户端功能重置间隔时间范围内的免费次数
	virtual unsigned long          GetTotalTimes(void)const=0;                                    //获取允许客户端功能重置间隔时间范围内的总次数
	virtual unsigned long          GetJiaziInterval(void)const=0;                                 //获取服务器甲子走动频率，以服务器分钟为单位
    virtual unsigned long          GetJiaziEventInfo(const unsigned long dwEventIndex)const=0;    //获取甲子事件ID	1~JIAZI_EVENT_NUM
	virtual unsigned long          GetSysResetTime(void)const=0;                                  //获取系统重置时间 0~23
	virtual unsigned long          GetDaySameBuffID(void)const=0;
	virtual unsigned long          GetMonthSameBuffID(void)const=0;
	virtual unsigned long          GetShowResBuffID(void)const=0;
	//Givft Setting
	virtual void                   GetGiftExpression(const unsigned long dwTianGan,const unsigned long dwDizhi,const TS_GIFT_GRAD dwGiftGrad ,TS_GIFT * pGift=NULL)const=0;                  //获取礼物表达式 dwTianXiangIndex: 1~MAX_JIAZI_NUM
    //Wheel Setting
	virtual TIAN_XIANG_PROBABILITY GetProbability(const unsigned long dwTianGan,const unsigned long dwDizhi)=0;
	//dwTianGan : 1~MAX_TIANGAN_NUM
	//dwDizhi:    1~MAX_DIZHI_NUM
	virtual float                  GetDregress(void)const=0;
};

ITaisuiWheelSettingMgr * GetMainTaisuiWheelSettingMgr(void);
void                     ReleaseMainTaisuiWheelSettingMgr(void);
#endif

#endif
