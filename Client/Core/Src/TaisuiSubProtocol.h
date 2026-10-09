#ifndef TAISUI_SUB_PROTOCOL_H
#define TAISUI_SUB_PROTOCOL_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/25/2007 14:32
//      File_base        : TaisuiSubProtocol
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮子协议
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#pragma	pack(push, 1)

enum s2c_taisui_wheel_sub_protocol
{
  s2c_taisui_sync,                //旋转次数同步据同步
  s2c_taisui_cal_res,             //预先计算结果(只发回来天干)
  s2c_taisui_gift,                //奖励(地支结果和奖励)
  s2c_taisui_tianxiang,            //天象同步
  s2c_taisui_jiazi,                //全局甲子激活状态同步
  s2c_taisui_reset,                //功能重置
  s2c_taisui_end,
};

#define MAX_TAISUI_S2C_NUM 8

enum c2s_taisui_wheel_sub_protocol
{
  c2s_taisui_tian_gan,            //旋转天干
  c2s_taisui_dizhi,               //旋转地支
  c2s_drop_chance,                //放弃当前旋转机会
  c2s_taisui_request_gift,        //请求显示结果
  c2s_taisui_end,
};

#define MAX_TAISUI_C2S_NUM 8

typedef struct tagS2C_TAISUI_SYNC
{
  unsigned short            m_WheeledTimes;
  unsigned short            m_AvailableTimes;
}S2C_TAISUI_SYNC;

typedef struct tagS2C_TIAN_XIANG_SYNC
{
  unsigned short            m_Month;
  unsigned short            m_Day;
}S2C_TIAN_XIANG;

typedef struct tagS2C_EVENT_SYNC
{
  unsigned long             m_CurrentActivateIndex;  //Activate info just
}S2C_TAISUI_EVNET_SYNC;

typedef struct tagS2C_TAISUI_RESET
{
  unsigned short            m_WheeledTimes;
  unsigned short            m_AvailableTimes;
}S2C_TAISUI_RESET;

typedef struct tagS2C_CAL_TIAN_GAN
{
  unsigned long             m_TianGan;
}S2C_CAL_TIAN_GAN;

typedef struct tagS2C_GIFT_RES
{
	unsigned char           m_DizhiRes;
	unsigned char           m_Grand; 
	unsigned short          m_JiaziEvent;   //0 means invalid 
}S2C_CAL_GIFT_RES;

#pragma pack(pop)

#endif