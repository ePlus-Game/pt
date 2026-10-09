#ifndef K_TAISUIWHEELEVENTMGR_H
#define K_TAISUIWHEELEVENTMGR_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/24/2007 15:09
//      File_base        : KTaisuiWheelEventMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 甲子事件管理
//                         数据库需求:当前激活的甲子数
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "ITaisuiWheelEventMgr.h"
class KTaisuiWheelEventMgr:public ITaisuiWheelEventMgr
{
	unsigned long m_CurActivateIndex;            //当前激活的甲子Index
	bool          m_IsInited;                    //是否初始化完全
public:
	KTaisuiWheelEventMgr();
	~KTaisuiWheelEventMgr();
public:
	void          BreatheRes(const unsigned long dwTianGan,const unsigned long dwDiZhi);	//激活天象
	unsigned long GetNextTianXiangToActivate(void)const;
	inline bool   IsInited(void)const;
	void          LoadFromDB(void);              //这个会影响m_IsInited
private:
	void          SaveToDB(void);
};

inline bool KTaisuiWheelEventMgr::IsInited()const
{
	return m_IsInited;
}

#endif