#ifndef ITAISUI_WHEEL_H
#define ITAISUI_WHEEL_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/20/2007 13:46
//      File_base        : ITaisuiWheel
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮逻辑接口
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "TaisuiWheelRule.h"

struct ITaisuiWheel
{
    virtual ~ITaisuiWheel(void){}

	virtual bool             IsInited(void)const=0;                 //只有当初始化后才可以进行余下事宜(对于客户端，只有它为true的时候界面才有效)        
	virtual unsigned long    GetCurrentWheeledTimes(void)const=0;   //获取当前已经旋转了的次数
	virtual unsigned long    GetCurrentAvailableTimes(void)const=0; //获取当前还可以旋转的次数
    virtual void             Breathe(void)=0;                       //消息循环

#ifdef _SERVER
	//Interfaces might ued by GM
	virtual void             SetCurrentAvailableTimes(const unsigned long dwTimes)=0; //设置当前还可以旋转的次数 
	virtual void             ProcessMsg(BYTE * pMsg,int iSize)=0;   //消息处理
#else
	//Interfaces for UI and Coreshell
	virtual void             ProcessMsg(BYTE * pMsg)=0;             //消息处理
	virtual unsigned long    GetCurrentWheeldTianGan(void)const=0;  //获取当前旋转结果--天干
	virtual unsigned long    GetCurrentWheeldDiZhi(void)const=0;    //获取当前旋转结果--地支
	virtual void             WheelTianGan(void)=0;
	virtual void             WheelDizhi(void)=0;
	virtual void             DropChance(void)=0;                    //放弃当前旋转机会
	virtual unsigned long    GetCurrentDay(void)=0;                 //获取当前天象
	virtual unsigned long    GetCurrentMonth(void)=0;
	virtual unsigned long    GetActivatingJiazi(void)=0;            //获取激活的甲子
	virtual void             ReFresh(void)=0;                       //重入
	virtual void             RequestGiftRes(void)=0;
#endif

};

#endif