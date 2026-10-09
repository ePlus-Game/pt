#ifndef I_TAISUI_WHEEL_EVENT_MGR_H
#define I_TAISUI_WHEEL_EVENT_MGR_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/23/2007 21:16
//      File_base        : ITaisuiWheelEventMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 甲子事件管理
//                         将旋转结果传入，给出激活的事件ID
//                         数据库存储全局激活状态 
//      <Change_list>
//////////////////////////////////////////////////////////////////////

struct ITaisuiWheelEventMgr
{
	virtual ~ITaisuiWheelEventMgr(void){}
    virtual  void          BreatheRes(const unsigned long dwTianGan,const unsigned long dwDiZhi)=0;	//转动结果激活天象
	virtual  unsigned long GetNextTianXiangToActivate(void)const=0;
	virtual  bool          IsInited(void)const=0;   
	virtual  void          LoadFromDB(void)=0;              
};

ITaisuiWheelEventMgr * GetMainTaisuiWheelEventMgr(void);
void                   ReleaseMainTaisuiWheelEventMgr(void);

#endif