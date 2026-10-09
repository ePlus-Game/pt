#ifndef TAISUI_WHEEL_TIAN_XIANG_H
#define TAISUI_WHEEL_TIAN_XIANG_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/20/2007 11:01
//      File_base        : TaisuiWheelTianXiang
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮天象
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _SERVER
struct ITianXiang
{
	virtual ~ITianXiang(void){}
	virtual unsigned long GetCurrentTianXiang(void)const=0;      //获取当前的天象，对于服务器是TianXiangMgr管理的内容，而对于客户端，是TianXiangMgr同步的结果
	virtual unsigned long GetGlobalActivatedTianXiang(void)=0;   //获取全局范围内刚刚激活的天象.
	virtual void          Breathe(void)=0;                      
};
#endif

#endif
