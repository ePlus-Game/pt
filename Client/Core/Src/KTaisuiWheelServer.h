#ifndef KTAISUI_WHEEL_SERVER_H
#define KTAISUI_WHEEL_SERVER_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/23/2007 20:36
//      File_base        : KTaisuiWheelServer
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮服务器管理类
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifdef _SERVER

#define  FS_TAISUI_DATA_VERSION 1

//TaisuiSystem

//Basic infomation struct declaration
#pragma	pack(push, 1)
typedef struct _TaisuiGlobal
{
    unsigned long dwTianXiangDay;
	unsigned long dwTianXiangMonth;
	unsigned long dwJiaziEventIndex;
}TaisuiGlobal;                     //Version 1 used

#pragma pack(pop)

class KTaisuiGlobalLoader          //Version 1
{
public:
	unsigned long Parse(const unsigned long dwVersion , TaisuiGlobal * lpDest,void * pBuff, const unsigned long dwSize );
};


unsigned long GetTaisuiTianXiangDay(void);
unsigned long GetTaisuiTianXiangMonth(void);
unsigned long GetCurTaisuiEvent(void);
void          SetTaisuiTianXiangDay(unsigned long dwDay);
void          SetTaisuiTianXiangMonth(unsigned long dwMonth);
void          SetCurTaisuiEvent(unsigned long dwEventIndex);

void          LoadTaisuiGlobal( );
void          SaveTaisuiGlobal( );

struct ITaisuiWheelSettingMgr;
struct ITianXiangMgr;
struct ITaisuiWheelResGenerator;
struct ITaisuiWheelEventMgr;

class KTaisuiWheelServer
{
	static ITaisuiWheelSettingMgr   *  m_Setting;
    static ITianXiangMgr            *  m_TianXiangMgr;
    static ITaisuiWheelResGenerator *  m_ResGenerator;
	static ITaisuiWheelEventMgr     *  m_EventMgr;

	static bool                        m_IsInit;
	//static unsigned long               m_Timer;
public:
	KTaisuiWheelServer(void);
	~KTaisuiWheelServer(void);
public:
	static KTaisuiWheelServer & Singleton(void);
	static void                 Init(void);
	static void                 Release(void);
	static bool                 IsInited(void);
public:
    static void                 Breathe(void);
	static void                 ProcessGlobalDBTaisui(int nDBOpeRst, int nDataSize, unsigned char *pData);
};
#endif

#endif