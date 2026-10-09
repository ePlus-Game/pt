#ifndef K_TAISUI_WHEEL_TIANXIANG_MGR_H
#define K_TAISUI_WHEEL_TIANXIANG_MGR_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/24/2007 9:54
//      File_base        : KTaisuiWheelTianXiangMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮天象管理实现
//                         数据库需求：全局天象(日,月)存储
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "ITaisuiWheelTianXiangMgr.h"
#include <time.h>
 
class KTaisuiWheelTianXiangMgr:public ITianXiangMgr
{
	unsigned long          m_CurTianXiangMonth;
	unsigned long          m_CurTianXiangDay;
	unsigned long          m_TimeInterval;
    time_t                 m_TimeCounter;

	unsigned long          m_SuspendPass;
	bool                   m_IsSuspend;

	bool                   m_IsInited;
public:
	KTaisuiWheelTianXiangMgr(void);
    ~KTaisuiWheelTianXiangMgr(void);
public:
	inline bool   IsInited(void)const;
	void          Breathe(void);
	unsigned long GetCurrentTianXiangDay(void)const;    //获取当前日天象、如果变化
	void          SetCurrentTianXiangDay(unsigned long dwTianXiangIndex);
	unsigned long GetCurrentTianXiangMonth(void)const;    //获取当月前天象、如果变化
	void          SetCurrentTianXiangMonth(unsigned long dwTianXiangIndex);
	void          Suspend(void);                     //暂停天象
	void          Resume(void);                      //继续天象
	void          LoadFromDB(void);             //关系到m_IsInited
private:
	void          SaveToDB(void)const;
};
	
inline bool KTaisuiWheelTianXiangMgr::IsInited(void)const
{
	return m_IsInited;
}

#endif