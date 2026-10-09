#ifndef K_TAISUI_WHEEL_H
#define K_TAISUI_WHEEL_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/24/2007 20:35
//      File_base        : KTaisuiWheel
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 每个客户对应的太岁之轮系统
//                         数据库需求:存储尚可转动次数、已经转动次数
//                         上次功能重置的时间点(避免全局清库操作)
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "ITaisuiWheel.h"
#include "TaisuiSubProtocol.h"
#define  MAX_MSG_BUFFER 256

class KTaisuiWheel:public ITaisuiWheel
{
	unsigned long    m_WheeledTimes;                     //已经旋转的次数
	unsigned long    m_AvailableTimes;                   //可以旋转的次数
	bool             m_IsInit;                           //是否已经初始化

	char             m_MsgBuffer[MAX_MSG_BUFFER];        //消息发送用
    
	unsigned long    m_Day;                              //当前天象(日)
	unsigned long    m_Month;                            //当前天象(月)
	
	unsigned long    m_TianGanRes;                       //当前旋转的天干(0 表示没有结果)
	unsigned long    m_DizhiRes;                         //当前旋转的地支(0 表示没有结果)

	//Event info
    unsigned long    m_JiazeActivatedIndex;              //甲子激活情况

#ifdef _SERVER
	//Web connect and player info
	unsigned long    m_PlayerIndex;
	unsigned long    m_ConnectIndex;     
    //state info
	enum SERVER_STATE
	{
       SS_IDLE,                                          //没有任何动作
	   SS_WHEEL_TIAN_GAN,                                //已经转动了天干
	   SS_WHEEL_DI_ZHI  ,                                //已经转动了地支
	};      

	SERVER_STATE     m_CurState;                         //当前的状态
    //Reset info
    #pragma pack(push,1)
	typedef struct tagRESET_POINT
	{
       BYTE           m_Day;
	   BYTE           m_Month;
	   unsigned short m_Year;
       public:
	   tagRESET_POINT();
	}RESET_POINT;
    #pragma pack (pop)

	RESET_POINT      m_LastResetPoint; 
	bool             m_FirstOnlineFlag;    
	bool             m_IsDBReady;
    //Msg call 
	void (KTaisuiWheel::*ProcessFunc[c2s_taisui_end])(void);
#else
	bool                 m_IsTianXiangInited;
	bool                 m_IsWheelInited;
	bool                 m_IsJiaziEventInited;
	//Msg call 
    void (KTaisuiWheel::*ProcessFunc[s2c_taisui_end])(BYTE *);
#endif

public:
	KTaisuiWheel(void);
	~KTaisuiWheel();
public:
    //Share Functions
	void             Init(
#ifdef _SERVER
		const unsigned long dwPlayerIndex,const unsigned long dwConnectIndex
#endif
		);

	bool             IsInited(void)const;
	unsigned long    GetCurrentWheeledTimes(void)const;   //获取当前已经旋转了的次数
	unsigned long    GetCurrentAvailableTimes(void)const; //获取当前还可以旋转的次数
    void             Breathe(void);                       //消息循环
    void             ReFresh(void);
	//Server Used Only
#ifdef _SERVER
	void             ProcessMsg(BYTE * pMsg,int iSize);   //消息处理
	//Interfaces might ued by GM
	void             SetCurrentAvailableTimes(const unsigned long dwTimes); //设置当前还可以旋转的次数 
	void             LoadDB(void *,unsigned long dwVersion);                //只有Load成功时m_IsInit才等于true 主要Load 可转动次数、已经转动次数和上次清库时间
	void             SaveDB(void *)const;                                   //记录所有
#else
    //Client Used
	void             ProcessMsg(BYTE * pMsg);           //消息处理
	//Interfaces for UI and Coreshell
	unsigned long    GetCurrentWheeldTianGan(void)const;  //获取当前旋转结果--天干
	unsigned long    GetCurrentWheeldDiZhi(void)const;    //获取当前旋转结果--地支
	void             WheelTianGan(void);
	void             WheelDizhi(void);
	void             DropChance(void);                    //放弃当前旋转机会
	void             RequestGiftRes(void);
	unsigned long    GetCurrentDay(void);                  //获取当前天象
	unsigned long    GetCurrentMonth(void);
	unsigned long    GetActivatingJiazi(void);            //获取激活的甲子
#endif

private:
    //Private share

#ifdef _SERVER 
	//Private Server
    //Breathe child function
	void             CheckTianXiang(void);                      
	bool             CheckReset(void);                         //查看是否重置，是则重置并返回true
	void             CheckActivateEvent(void);
    //Msg 
    void             SendOnlineSync(void)const;                //刚上线的Sync
	bool             SendSyncMsg(void)const;                   //发送同步信息
	void             SendSyncTianXiang(void)const;             //发送天象信息
	void             SendReset(void)const;                     //发送功能重置消息
	void             SendTianGanRes(void)const;
	void             SendGiftRes(const unsigned long dwDizhi,
		                         const unsigned long dwGrand,
								 const unsigned long dwEventID=0)const;

	void             SendJiaziActive(void)const;
	
	void             ProcessWheelTianGan(void); 
	void             ProcessWheelDiZhi(void);
	void             ProcessDropChance(void);
	void             ProcessShowGiftRes(void);
	//Operation 
	void             AddGift(const TS_GIFT * pGift=NULL,unsigned long dwEventAddition=0);
	void             BroadCastMsg(const char * szString);
#else
	//Private Client
	//Msg
	void             SendShowResOP(void);
	void             SendWheelTianGan(void);   
	void             SendWheelDizhi(void);
	void             SendDropChance(void);
	void             ProcessSyncMsg(BYTE * pSubMsg);              //处理同步消息
	void             ProcessJiaziEvnet(BYTE * pSubMsg);           //同步甲子事件
	void             ProcessTianXiang(BYTE * pSubMsg);            //处理天象同步
	void             ProcessReset(BYTE * pSubMsg);                //处理功能重置
	void             ProcessTianGan(BYTE * pSubMsg);              //处理天干回包
	void             ProcessGift(BYTE * pSubMsg);                 //处理奖励回包
	void             ProcessChangeWheelTime(BYTE * pSubMsg);
#endif
	
};

#endif