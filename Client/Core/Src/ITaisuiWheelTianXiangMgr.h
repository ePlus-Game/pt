#ifndef ITAISUI_WHEEL_TIANXIANG_H
#define ITAISUI_WHEEL_TIANXIANG_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/23/2007 20:51
//      File_base        : ITaisuiWheelTianXiangMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮天象管理器
//                         天象数据库存档,天象走动，修改接口
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifdef _SERVER

struct ITianXiangMgr
{
	virtual ~ITianXiangMgr(void){}
	virtual bool          IsInited(void)const=0;
	virtual void          Breathe(void)=0;
	virtual unsigned long GetCurrentTianXiangDay(void)const=0;    //获取当前日天象、如果变化
	virtual void          SetCurrentTianXiangDay(unsigned long dwTianXiangIndex)=0;
	virtual unsigned long GetCurrentTianXiangMonth(void)const=0;    //获取当月前天象、如果变化
	virtual void          SetCurrentTianXiangMonth(unsigned long dwTianXiangIndex)=0;
	virtual void          Suspend(void)=0;                     //暂停天象
	virtual void          Resume(void)=0;                      //继续天象
	virtual void          LoadFromDB(void)=0;              //这个会影响m_IsInited
};
 
ITianXiangMgr *  GetMainTianXiangMgr();
void             ReleaseMainTianXiangMgr();

#endif

#endif