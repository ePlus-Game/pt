#include "KWin32.h"
#include "UiTaisuiWnd.h"
#include "CoreShell.h"
#include "CEGUIEvent.h"
#include "JiaziCommonDef.h"
#include "Ui\UiCase\UiTopMessage.h"
#include "Ui\KMessageCentre.h"
#include "Ui\UiCase\UiHelpInfo.h"
#include "KWin32Wnd.h"

extern iCoreShell* g_pCoreShell;
using namespace CEGUI;

template<> 
KTaisuiWnd*     KUiWndSingleton<KTaisuiWnd>::ms_Singleton	= NULL;
DWORD           KTaisuiWnd::m_Day=0;
DWORD           KTaisuiWnd::m_Month=0;
DWORD           KTaisuiWnd::m_JiaziEvent=0;
DWORD           KTaisuiWnd::m_ResTianGan=0;
DWORD           KTaisuiWnd::m_ResDizhi=0;
DWORD           KTaisuiWnd::m_AvailableTimes=0;
DWORD           KTaisuiWnd::m_WheeledTimes=0;

TLStaticText *  KTaisuiWnd::m_DayInfo=NULL;
TLStaticText *  KTaisuiWnd::m_MonthInfo=NULL;
TLStaticImage*  KTaisuiWnd::m_JiaziEventTxt[MAX_JIAZI_EVENT_NUM];
TLStaticImage*  KTaisuiWnd::m_TianXiangStatic=NULL;
TLStaticImage*  KTaisuiWnd::m_TianXiangDay=NULL;
TLStaticImage*  KTaisuiWnd::m_TianXiangMonth=NULL;
TLStaticImage*  KTaisuiWnd::m_WheelAnimation=NULL;
TLStaticImage*  KTaisuiWnd::m_CloseTip=NULL;
TLStaticImage*  KTaisuiWnd::m_WheelResAniTianGan=NULL;
TLStaticImage*  KTaisuiWnd::m_WheelResAniDizhi=NULL;
TLStaticText *  KTaisuiWnd::m_WheelSound=NULL;
TLStaticText *  KTaisuiWnd::m_JiaziEventSound=NULL;
TLStaticText *  KTaisuiWnd::m_TianXiangEventSound=NULL;
TLStaticImage*  KTaisuiWnd::m_JiaziEffect=NULL;
TLStaticImage*  KTaisuiWnd::m_TianXiangEffect=NULL;
/*
#ifdef _DEBUG
TLStaticText * KTaisuiWnd::m_TimesInfo=NULL;
#endif
*/

TLButton     * KTaisuiWnd::m_Close=NULL;
TLButton     * KTaisuiWnd::m_Wheel=NULL;
TLButton     * KTaisuiWnd::m_WheelAgain=NULL;
TLButton     * KTaisuiWnd::m_ShowGift=NULL;

bool           KTaisuiWnd::m_bShowFlag=false;
bool           KTaisuiWnd::m_bWheeling=false;
bool           KTaisuiWnd::m_bJiaziEventFlag=false;
bool           KTaisuiWnd::m_GiftShowFlag=false;      
unsigned long  KTaisuiWnd::m_GiftRecord=0;
bool           KTaisuiWnd::m_ChangeJiaziEvent=false;

KTaisuiWnd::UI_TAISUI_STATE KTaisuiWnd::m_State=UTS_IDLE;
KTaisuiWnd::TAISUI_WHEEL_ANI KTaisuiWnd::m_WheelAniInfo;

KTaisuiWnd::tagTAISUI_ANI_MINIPLAYER KTaisuiWnd::m_MiniPlayer;

//Taisui move pos effect
bool KTaisuiWnd::m_bMoveFlag=false;
KTaisuiWnd::TAISUI_MOVE_INFO KTaisuiWnd::m_MoveInfo;

#define  TAISUI_WHEEL_POS_MOVE_INTERVAL 2

#define  STR_TAISUI_NOTICE_NORMAL   1
#define  STR_TAISUI_NOTICE_IMPORT   2
//TaisuiWheel Animation
#define TAISUI_ANI_MIN_CIRCLE     1
#define TAISUI_INVALID_FRAME      -1
#define TAISUI_MAX_FRAME_NUM_NUM      100

#define MAX_TOP_MSG_LEN   128
#define TOP_MSG_DAY       3
#define TOP_MSG_MONTH     4
#define TOP_MSG_D_M       5
#define TOP_MSG_JIAZI     6

//Taisui Event Effect Pos
#define TAISUI_EFFECT_POS_START_X     0
#define TAISUI_EFFECT_POS_START_Y     360
#define TAISUI_EFFECT_POS_START_DIS   20
#define TAISUI_EFFECT_FRAME           10

//TianXiangEffect 
#define TIANXIANG_EFFECT_FRAME        20
//WheelAnimation frame index
static const unsigned long dwFrameTianGan[10]=
{
	{12},{20},{28},{37},{43},{52},{61},{69},{78},{87}
};

static const unsigned long dwFrameDizhi[12]=
{
    {95},{5},{12},{20},{28},{37},{43},{52},{62},{70},{79},{87}
};

//Wheel Animation Res Pos
#define TAISUI_RES_X 156
#define TAISUI_RES_Y 191

typedef struct tagResPos
{
    int iX;
	int iY;
}RES_POS;

static const RES_POS posTianGan[10]=
{
	//{256,85},{288,141},{287,202},{258,261},{201,291},{135,290},{80,260},{49,203},{49,138},{81,84}
	{264,92},{295,148},{295,213},{264,268},{209,300},{143,298},{87,265},{57,211},{58,144},{92,88}
};

static const RES_POS posDizhi[12]=
{
	//{209,23},{278,63},{318,130},{319,210},{280,277},{208,321},{130,319},{61,280},{21,207},{21,130},{61,60},{128,22}
	{214,31}, {284,71}, {325,141},{325,220}, {285,286}, {215,327}, {136,327},{67,289}, {28,	219}, {28,139}, {67	,68}, {134,	28}
};

KTaisuiWnd::KTaisuiWnd( const CEGUI::String & id_name )
: KUiWndSingleton<KTaisuiWnd>( id_name )
{
    memset(m_JiaziEventTxt,0,sizeof(m_JiaziEventTxt));
}

KTaisuiWnd::~KTaisuiWnd()
{

}

void KTaisuiWnd::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KTaisuiWnd::OnHidden, this));
		m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KTaisuiWnd::OnKeyDown, this));

		m_DayInfo		= (TLStaticText *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Day");
	    assert(m_DayInfo);
		
		m_DayInfo->setTextColours(RGB(0,0,0));
		
		m_MonthInfo	    = (TLStaticText *)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Taisui/Month" );		
  	    assert(m_MonthInfo);

		m_MonthInfo->setTextColours(RGB(0,0,0));

		m_Close         = (TLButton     *)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Taisui/Close");
		assert(m_Close);

		m_Close->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KTaisuiWnd::OnClose, this));
		
		m_Wheel         = (TLButton     *)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Taisui/Wheel");
		assert(m_Wheel);
		m_Wheel->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KTaisuiWnd::OnWheel, this));

		m_WheelAgain    = (TLButton     *)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Taisui/Wheelagain");
		assert(m_WheelAgain);
		m_WheelAgain->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KTaisuiWnd::OnWheel, this));
        m_WheelAgain->hide();

		m_ShowGift    = (TLButton     *)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Taisui/ShowGift");
		assert(m_ShowGift);
		m_ShowGift->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KTaisuiWnd::ShowGiftUi, this));
        m_ShowGift->hide();

		/*
		m_TianXiangStatic = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/TianXiang");
        assert(m_TianXiangStatic); 
        */

		m_TianXiangDay = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/TianXiangRi");
        assert(m_TianXiangDay);

		m_TianXiangMonth = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/TianXiangYue");
        assert(m_TianXiangMonth);

		m_WheelAnimation = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/WheelAnimation");
		Point    pos     =  m_WheelAnimation->getAbsolutePosition();
        assert(m_WheelAnimation);
		
		m_WheelResAniTianGan = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/WheelResAnimationTianGan");
        m_WheelResAniTianGan->hide();
		assert(m_WheelResAniTianGan);

		m_WheelResAniDizhi = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/WheelResAnimationDizhi");
        m_WheelResAniDizhi->hide();
		assert(m_WheelResAniDizhi);
		
		m_JiaziEffect = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/JiaziEffect");
		m_JiaziEffect->hide();
		assert(m_JiaziEffect);

		m_TianXiangEffect = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/TianxiangEffect");
		m_TianXiangEffect->hide();
		assert(m_TianXiangEffect);
		
		//Init CloseTip
        m_CloseTip = (TLStaticImage * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/CloseTip");
		m_CloseTip->subscribeEvent(Window::EventHidden, Event::Subscriber(&KTaisuiWnd::OnCloseTipClose, this)); 
		
		assert(m_CloseTip);

		m_CloseTip->getChild("TaharezLook/Taisui/CloseTip/OK")->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KTaisuiWnd::OnCloseTipOk, this));
		m_CloseTip->getChild("TaharezLook/Taisui/CloseTip/Cancel")->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KTaisuiWnd::OnCloseTipCancel, this));
		m_CloseTip->setZLevel(Window::Top);
        m_CloseTip->hide();	

		m_WheelSound = (TLStaticText * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/WheelSound");
        m_WheelSound->hide();
		assert(m_WheelSound);

		m_JiaziEventSound = (TLStaticText * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/JiaziSound");
        m_JiaziEventSound->hide();
		assert(m_JiaziEventSound);

		m_TianXiangEventSound = (TLStaticText * )ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/TianXiangSound");
        m_TianXiangEventSound->hide();
		assert(m_TianXiangEventSound);

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Help")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KTaisuiWnd::OnHelp, this));

		char szWinName[256];
		char szImageName[256];

		for (int i=0;i<MAX_JIAZI_EVENT_NUM;i++)
		{
			sprintf(szWinName,"TaharezLook/Taisui/Jiazi%d",i+1);
            m_JiaziEventTxt[i]= (TLStaticImage *)ms_Singleton->m_pThisWnd->getChild(szWinName);
			assert(m_JiaziEventTxt[i]);

			sprintf(szImageName,"jiazi%d",i+1);
		 	m_JiaziEventTxt[i]->setImage("jiazievent",szImageName);
			m_JiaziEventTxt[i]->hide();
		}//end for i
/*
        #ifdef _DEBUG
		m_TimesInfo     = (TLStaticText *)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/Taisui/Times" );		
  	    m_TimesInfo->setTextColours(RGB(255,0,0));
		assert(m_TimesInfo);
        #endif
*/
		((TLStaticImage * )ms_Singleton->m_pThisWnd)->setDragMovingEnabled(false);

		ms_Singleton->m_pThisWnd->setZLevel(Window::Top);
	}//endif

}

void KTaisuiWnd::UpdataAll()
{
	UpdateTianXiang();
	UpdateTimesInfo();
	UpdateJiaziEvent();
}

#define TAISUI_POS_Y_800  g_GetScreenHeight()/2 - ms_Singleton->m_pThisWnd->getHeight(Absolute)/2
#define TAISUI_POS_X_800  g_GetScreenWidth()/2 - ms_Singleton->m_pThisWnd->getWidth(Absolute)/2

#define TAISUI_POS_Y_1024  g_GetScreenHeight()/2 - ms_Singleton->m_pThisWnd->getHeight(Absolute)/2
#define TAISUI_POS_X_1024  g_GetScreenWidth()/2 - ms_Singleton->m_pThisWnd->getWidth(Absolute)/2

bool KTaisuiWnd::OnKeyDown(const CEGUI::EventArgs & args)
{
	return true;
}

void KTaisuiWnd::Show()
{
	if (g_pCoreShell->GetGameData(GDI_GET_CLIENT_STATE,0,0))
	{
		MiniStop();
		KUiWndSingleton<KTaisuiWnd>::Show();
		ms_Singleton->m_pThisWnd->setModalState(true);
		UpdataAll();
		CheckButtonState();
		m_bShowFlag=false;

		if (g_GetScreenWidth()==1024)
		{
			//Move Effect
			Point pos(-600,TAISUI_POS_Y_1024);
			ms_Singleton->m_pThisWnd->setPosition(Absolute,pos);
		    MoveTo(TAISUI_POS_X_1024,TAISUI_POS_Y_1024,80,0);
		}
		else
		{
            //Move Effect
			Point pos(-600,TAISUI_POS_Y_800);
			ms_Singleton->m_pThisWnd->setPosition(Absolute,pos);
		    MoveTo(TAISUI_POS_X_800,TAISUI_POS_Y_800,60,0);
		}

	}//endif
	else
		m_bShowFlag=true;
}

void KTaisuiWnd::UpdateTianXiang()
{
	DWORD dwRes=g_pCoreShell->GetGameData(GDI_GET_CUR_TIAN_XIANG,0,0);
	m_Month=(dwRes>>16);
	m_Day=(dwRes & 0x0000ffff);
	
	if (m_Month && m_Day && m_MonthInfo && m_DayInfo)
	{ 
        ShowTianXiang();
	}//endif
}

void KTaisuiWnd::UpdateJiaziEvent()
{
	if (m_State!=UTS_WHEEL_DIZHI)
	{
		unsigned long dwNewJiaziEvent=g_pCoreShell->GetGameData(GDI_GET_ACTIVATING_TIAN_XIANG,0,0);
		m_JiaziEvent=dwNewJiaziEvent;
		ShowJiaziEvent();
	}//endif
	else
	{
		if (m_ResDizhi==0 || m_ResTianGan==0)
		{
			unsigned long dwNewJiaziEvent=g_pCoreShell->GetGameData(GDI_GET_ACTIVATING_TIAN_XIANG,0,0);
			m_JiaziEvent=dwNewJiaziEvent;
			ShowJiaziEvent();
		}//endif
		else
		{
			unsigned long dwEventIndex=TianGanDiZhiToTianXiang(m_ResTianGan,m_ResDizhi);
			unsigned long dwNewJiaziEvent=g_pCoreShell->GetGameData(GDI_GET_ACTIVATING_TIAN_XIANG,0,0);
			
			if (dwEventIndex==dwNewJiaziEvent)
			{
				m_ChangeJiaziEvent=true;
			}//endif
			else
			{
				m_JiaziEvent=m_ChangeJiaziEvent;
			}

		}//end else
		
	}//end else
}

void KTaisuiWnd::UpdateDizhiRes()
{
	m_ResDizhi=g_pCoreShell->GetGameData(GDI_GET_WHEEL_DI_ZHI,0,0);
	if (m_ResDizhi)
		ShowDizhiRes();
}

void KTaisuiWnd::UpdateTianGanRes()
{	
	m_ResTianGan=g_pCoreShell->GetGameData(GDI_GET_WHEEL_TIAN_GAN,0,0);
	if (m_ResTianGan)
		ShowTianGanRes();
}

void KTaisuiWnd::Hide()
{
	if (!IsVisible())
		return ;
	
	if (m_CloseTip && m_CloseTip->isVisible())
		return ;
	
	
	if (m_CloseTip)
	{
		m_CloseTip->show();

		const char * pMsg = NULL;
		String str;
		if (m_State==UTS_IDLE )
		{
			if (m_ShowGift && m_ShowGift->isVisible() && !m_ShowGift->isDisabled())
			{
				pMsg = KMessageCentre::GetMessage(taisui_message,STR_TAISUI_NOTICE_IMPORT);
				if (pMsg)
					str=AnsiToUtf8(pMsg);
			}//end if
			else
			{
				pMsg = KMessageCentre::GetMessage(taisui_message,STR_TAISUI_NOTICE_NORMAL);
				if (pMsg)
					str=AnsiToUtf8(pMsg);
			}//end else
			
		}//endif
		else
		{
			pMsg  = KMessageCentre::GetMessage(taisui_message,STR_TAISUI_NOTICE_IMPORT);
			if (pMsg)
				str   = AnsiToUtf8(pMsg);	
		}
		
		if (pMsg)
			((TLStaticText *)m_CloseTip->getChild("TaharezLook/Taisui/CloseTip/Notice"))->setText(str);
		
		//m_CloseTip->setModalState(true);
		return ;
	}//endif
	
}

bool KTaisuiWnd::OnClose(const CEGUI::EventArgs& args )
{
	Hide();

	/*if (KTaisuiWnd::IsVisible())
	{
		if (ms_Singleton->m_pThisWnd)
			ms_Singleton->m_pThisWnd->setModalState(true);
	}//endif
	*/

    return true;
}

bool KTaisuiWnd::OnWheel(const CEGUI::EventArgs& args )
{

//	MiniPlay();
    
/*  #ifdef _DEBUG
	if (m_ResTianGan && m_ResDizhi)
	{
		assert(true);
	}//endif
    #endif
*/
    if (g_pCoreShell->GetGameData(GDI_GET_CLIENT_STATE,0,0))
	{
		switch (m_State)
		{
		   case UTS_IDLE:
			   {
				   if ( m_Wheel && m_Close )
				   {
					    g_pCoreShell->OperationRequest(GOI_WHEEL_TIAN_GAN,0,0);
					    m_State=UTS_WHEEL_TIANGAN;
						LoopWheel();
						m_Wheel->disable();
						m_Close->disable();
				   }

			   }

		   break;
		
		   case UTS_WHEEL_TIANGAN:
			   {	
					if ( m_WheelAgain && m_Close )
					{
						g_pCoreShell->OperationRequest(GOI_WHEEL_DI_ZHI,0,0);
						m_State=UTS_WHEEL_DIZHI;
						LoopWheel();
						m_WheelAgain->disable();
						m_Close->disable();
					   //m_pThisWnd->setModalState(false);
					}

			   }
		   break;

		}//end switch
		//*//*
	}//endif


	return true;
}

void KTaisuiWnd::Disconnect()
{
	if (m_State!=UTS_IDLE)
	{
       Hide();
	}//endif
}

void KTaisuiWnd::EnableHide()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		EventArgs args;
		ms_Singleton->OnCloseTipOk(args);
	}//endif
}

bool KTaisuiWnd::OnCloseTipOk(const CEGUI::EventArgs& args)
{
	if (m_State!=UTS_IDLE)
		g_pCoreShell->OperationRequest(GOI_DROP_CHANCE,0,0);

    m_State=UTS_IDLE;
    m_ResTianGan=0;
	m_ResDizhi=0;
	m_GiftRecord=0;
	m_GiftShowFlag=false;
	m_bJiaziEventFlag=false;
	m_ChangeJiaziEvent=false;
	
	ShowTianXiang();  //LowLightAgain if day same or month same
	ms_Singleton->m_pThisWnd->setModalState(false);
	
/*	m_WheelAniInfo.m_CircleCounter=0;
	m_WheelAniInfo.m_FinalCircle=false;*/
	m_WheelAniInfo.m_HeadingFrame=-1;
	m_bWheeling=false;
	/*
	if (m_WheelAnimation && m_WheelAnimation->isPlaying())
	{
        m_WheelAnimation->stop();
	}//endif
	*/

	MiniStop();

	if (m_WheelResAniTianGan && m_WheelResAniTianGan->isVisible())
	{
		m_WheelResAniTianGan->setCyc(false);
		m_WheelResAniTianGan->stop();
		m_WheelResAniTianGan->hide();
	}
	
	if (m_WheelResAniDizhi && m_WheelResAniDizhi->isVisible())
	{
		m_WheelResAniDizhi->setCyc(false);
		m_WheelResAniDizhi->stop();
		m_WheelResAniDizhi->hide();
	}//endif

	if (m_JiaziEffect && m_JiaziEffect->isVisible())
	{
		m_JiaziEffect->stop();
		m_JiaziEffect->hide();
	}

	if (m_TianXiangEffect && m_TianXiangEffect->isVisible())
	{
		m_TianXiangEffect->stop();
		m_TianXiangEffect->hide();
	}

	if (m_ShowGift && m_ShowGift->isVisible())
	{
        m_ShowGift->hide();
	}//endif

	m_CloseTip->hide();
	//m_CloseTip->setModalState(false);

	KUiWndSingleton<KTaisuiWnd>::Hide();

	return true;
}

bool KTaisuiWnd::OnCloseTipCancel(const CEGUI::EventArgs& args)
{
    m_CloseTip->hide();
	//m_CloseTip->setModalState(false);
	
	/*if (KTaisuiWnd::IsVisible())
	{
		if (ms_Singleton->m_pThisWnd)
			ms_Singleton->m_pThisWnd->setModalState(true);
	}//endif
    */

	return true;
}

void KTaisuiWnd::UpdateTimesInfo()
{  
	unsigned long dwRes=g_pCoreShell->GetGameData(GDI_GET_TIMES_INFO,0,0);
	m_AvailableTimes=(dwRes>>16);
	m_WheeledTimes=(dwRes & 0x0000ffff);
   
/*  #ifdef _DEBUG
	if (m_TimesInfo)
	{
		String str;
		char   szString[256];
		sprintf(szString,"还可以转%d次，已经转了%d次",m_AvailableTimes,m_WheeledTimes);
		str= AnsiToUtf8( szString );
		m_TimesInfo->setText(str);
	}
    #endif
*/
}

void  KTaisuiWnd::ShowJiaziEvent()
{
    for (int i=0;i<m_JiaziEvent;i++)
	{
		if (m_JiaziEventTxt[i])
		{
			m_JiaziEventTxt[i]->show();
		}//endif
	}//end for i
	
	for (int j=m_JiaziEvent;j<MAX_JIAZI_EVENT_NUM;j++)
	{
		if(m_JiaziEventTxt[j])
			m_JiaziEventTxt[j]->hide();
	}//end for j
}

void KTaisuiWnd::ShowTianXiang()
{
	if (/*m_TianXiangStatic &&*/ m_TianXiangDay && m_TianXiangMonth)
	{
        char          szJiaziImageTxtName[32];
		
		sprintf(szJiaziImageTxtName,"jiazi%d",m_Day);
		m_TianXiangDay->setImage("tianxiangtxtlow",szJiaziImageTxtName);
		
		sprintf(szJiaziImageTxtName,"jiazi%d",m_Month);
		m_TianXiangMonth->setImage("tianxiangtxtlow",szJiaziImageTxtName);
	    /*	
		char szImageName[32];
		sprintf(szImageName,"riyue%d",1);
		m_TianXiangStatic->setImage("riyue",szImageName);
		*/

	}//endif

}

void KTaisuiWnd::HilightRiYue()
{
	if (/*m_TianXiangStatic &&*/ m_TianXiangDay && m_TianXiangMonth)
	{
		unsigned long dwImageIndex=1;
        char          szJiaziImageTxtName[32];
		
        if (m_State!=UTS_WHEEL_TIANGAN && m_ResTianGan && m_ResDizhi)
		{
            unsigned long dwJiaziIndex=TianGanDiZhiToTianXiang(m_ResTianGan,m_ResDizhi);
			//Notice 在UTS_WHEEL_TIANGAN 的情况下执行会有逻辑问题
			if (dwJiaziIndex==m_Day)
			{
				dwImageIndex++;
				sprintf(szJiaziImageTxtName,"jiazi%d",dwJiaziIndex);
				m_TianXiangDay->setImage("tianxiangtxthigh",szJiaziImageTxtName);
			}//endif
			else
			{
				sprintf(szJiaziImageTxtName,"jiazi%d",m_Day);
				m_TianXiangDay->setImage("tianxiangtxtlow",szJiaziImageTxtName);
			}
			
			if (dwJiaziIndex==m_Month)
			{
				dwImageIndex+=2;
				sprintf(szJiaziImageTxtName,"jiazi%d",dwJiaziIndex);
				m_TianXiangMonth->setImage("tianxiangtxthigh",szJiaziImageTxtName);
			}
			else
			{
				sprintf(szJiaziImageTxtName,"jiazi%d",m_Month);
				m_TianXiangMonth->setImage("tianxiangtxtlow",szJiaziImageTxtName);
			}
			
		}//endif
		/*
		char szImageName[32];
		sprintf(szImageName,"riyue%d",dwImageIndex);
		m_TianXiangStatic->setImage("riyue",szImageName);
		*/
	}//endif
}

void KTaisuiWnd::CheckButtonState()
{
	if (m_State==UTS_IDLE)
	{
		if (m_Wheel)
		{
			m_Wheel->show();

			if (m_WheelAgain)
				m_WheelAgain->hide();
			
			if (m_AvailableTimes==0 )
				m_Wheel->disable();
			else
				m_Wheel->enable();
		}//endif

	}//endif
    else
	{ 
        if (m_WheelAgain)
		{
			m_WheelAgain->show();
			if (m_State!=UTS_WHEEL_DIZHI)
			m_WheelAgain->enable();
			
			if (m_Wheel)
				m_Wheel->hide();

		}//endif
	}

	/*
	if (m_Wheel)
	{
		if (m_AvailableTimes==0 && m_State==UTS_IDLE)
			m_Wheel->disable();
		else
		{
			if (g_pCoreShell->GetGameData(GDI_GET_CLIENT_STATE,0,0) && !m_bWheeling)
				m_Wheel->enable();
		}//end else
	}//endif
  */

	if (m_Close)
		m_Close->enable();
}
/*
#ifdef _DEBUG
static char * szTianGan="甲乙丙丁戊己庚辛壬癸";
static char * szDizhi="子丑寅卯辰巳午未申酉戌亥";
#endif
*/
void KTaisuiWnd::ShowTianGanRes()
{	
	WheelTo(m_ResTianGan,true);
	/*
#ifdef _DEBUG
	if (m_TimesInfo && m_ResTianGan)
	{
		String str;
		char   szString[5];
		szString[3]=0;
		memcpy(szString,(void *)&szTianGan[2*(m_ResTianGan-1)],2);
		str= AnsiToUtf8( szString );
		m_TimesInfo->setText(str);
	}
#endif
	*/
}

void KTaisuiWnd::ShowDizhiRes()
{
	WheelTo(m_ResDizhi,false);
	/*
#ifdef _DEBUG
	if (m_TimesInfo && m_ResDizhi)
	{
		String str;
		char   szString[5];
		szString[5]=0;
		memcpy(szString,(void *)&szTianGan[2*(m_ResTianGan-1)],2);
		memcpy(szString+2,(void *)&szDizhi[2*(m_ResDizhi-1)],2);
		str= AnsiToUtf8( szString );
		m_TimesInfo->setText(str);
	}
#endif
	*/
}

void KTaisuiWnd::Breathe()
{
	if (ms_Singleton == NULL || ms_Singleton->m_pThisWnd == NULL)
		return ;
/*
//AutoClicking Robot
#ifdef _DEBUG
    if (ms_Singleton && ms_Singleton->m_pThisWnd && ms_Singleton->m_pThisWnd->isVisible() && (m_State==UTS_IDLE || m_State==UTS_WHEEL_TIANGAN) 
		&& m_Wheel && !m_Wheel->isDisabled() )
	{
		if (g_pCoreShell->GetGameData(GDI_GET_CLIENT_STATE,0,0))
		{
			switch (m_State)
			{
			case UTS_IDLE:
				{
					if ( m_Wheel && m_Close )
					{
						g_pCoreShell->OperationRequest(GOI_WHEEL_TIAN_GAN,0,0);
						m_State=UTS_WHEEL_TIANGAN;
						LoopWheel();
						m_Wheel->disable();
						m_Close->disable();
					}
					
				}
				
				break;
				
			case UTS_WHEEL_TIANGAN:
				{	
					if ( m_Wheel && m_Close )
					{
						g_pCoreShell->OperationRequest(GOI_WHEEL_DI_ZHI,0,0);
						m_State=UTS_WHEEL_DIZHI;
						LoopWheel();
						m_Wheel->disable();
						m_Close->disable();
					}
					
				}
				break;
				
			}//end switch
			//*//*
		}//endif
		

	}//endif

#endif
*/

	if (m_bShowFlag)
		Show();

	if (m_bMoveFlag)
		PosMoveBreathe();

	if (m_bWheeling)
		AnimationBreathe();	
	
	Point p=ms_Singleton->m_pThisWnd->getPosition(Absolute);
	if ( p.d_x >= 800 )
	{
		bool bXShake=false;
		bool bYShake=false;
		
		if (m_MoveInfo.m_Dx!=0)
			bXShake=true;
		
		if (m_MoveInfo.m_Dy!=0)
			bYShake=true;
		
		((TLStaticImage * ) ms_Singleton->m_pThisWnd)->Shake(bXShake,bYShake,4);
		
		m_MoveInfo.m_DestX=0;
		m_MoveInfo.m_DestY=0;
		m_MoveInfo.m_Dx=0;
		m_MoveInfo.m_Dy=0;
		m_MoveInfo.m_MoveTimer=0;
		m_bMoveFlag=false;
		
		ms_Singleton->m_pThisWnd->enable();
	//	((TLStaticImage * )ms_Singleton->m_pThisWnd)->setDragMovingEnabled(true);

		CEGUI::EventArgs   args;
	    ms_Singleton->OnCloseTipOk(args);
	}//endif
}

KTaisuiWnd::tagTAISUI_WHEEL_ANI::tagTAISUI_WHEEL_ANI()
{
	m_HeadingFrame=-1;
/*	m_CircleCounter=0;
	m_FinalCircle=false;
	m_CurrentFrame=0;
*/
}

KTaisuiWnd::tagTAISUI_WHEEL_ANI::~tagTAISUI_WHEEL_ANI()
{/**/}

/*
void KTaisuiWnd::AnimationBreathe()
{
 if (m_WheelAniInfo.m_HeadingFrame!=-1)
   {
       if (IsAnimationStop())
	   {
		   if ((m_WheelAniInfo.m_CircleCounter<TAISUI_ANI_MIN_CIRCLE || m_WheelAnimation->getCurFrameIdx()>m_WheelAniInfo.m_HeadingFrame 
			   && m_WheelAnimation->getCurFrameIdx()!=TAISUI_MAX_FRAME_NUM_NUM-1 ) && !m_WheelAniInfo.m_FinalCircle)
		   {
			   //StillLoop
			   int index=m_WheelAnimation->getCurFrameIdx();

			   if (index!=TAISUI_MAX_FRAME_NUM_NUM-1)
				   m_WheelAnimation->play(index,TAISUI_MAX_FRAME_NUM_NUM-1);
			   else
                   m_WheelAnimation->play(0,TAISUI_MAX_FRAME_NUM_NUM-1);
			   
			   m_WheelAniInfo.m_CircleCounter+=1;
			   m_WheelAniInfo.m_CurrentFrame=0;
               PlayWheelSound();
		   }
		   else
		   {
               if (m_WheelAniInfo.m_FinalCircle)
			   {
				  int index=m_WheelAnimation->getCurFrameIdx();   
				   //转动完毕
				  EndWheelAnimation();
			   }//endif
			   else
			   {
                   m_WheelAniInfo.m_FinalCircle=true;
				   int index=m_WheelAnimation->getCurFrameIdx();
				   assert(m_WheelAniInfo.m_HeadingFrame<TAISUI_MAX_FRAME_NUM_NUM && m_WheelAniInfo.m_HeadingFrame>=0 && (m_WheelAniInfo.m_HeadingFrame>=index || index==TAISUI_MAX_FRAME_NUM_NUM-1));

				   if (index!=TAISUI_MAX_FRAME_NUM_NUM-1)
					   m_WheelAnimation->play(index,m_WheelAniInfo.m_HeadingFrame);
				   else
					   m_WheelAnimation->play(0,m_WheelAniInfo.m_HeadingFrame);

			       m_WheelAniInfo.m_CircleCounter+=1;
				   m_WheelAniInfo.m_CurrentFrame=m_WheelAniInfo.m_HeadingFrame+1;
				   if (m_WheelAniInfo.m_CurrentFrame>=TAISUI_MAX_FRAME_NUM_NUM-1)
					   m_WheelAniInfo.m_CurrentFrame=0;
				   
				   PlayWheelSound();
			   }//end else
			    
		   }//end else

	   }//endif if (!m_WheelAnimation->isPlaying())

   }//endif
   else
   { 
	   //Loop frame
	   if (IsAnimationStop())
	   {
		   int index=m_WheelAnimation->getCurFrameIdx();

		   if (index!=TAISUI_MAX_FRAME_NUM_NUM-1)
			   m_WheelAnimation->play(index,TAISUI_MAX_FRAME_NUM_NUM-1);
		   else
               m_WheelAnimation->play(0,TAISUI_MAX_FRAME_NUM_NUM-1);

		   m_WheelAniInfo.m_CircleCounter+=1;
		   m_WheelAniInfo.m_CurrentFrame=0;

		   PlayWheelSound();
	   }//endif
   
   }//end else

}
*/

void KTaisuiWnd::AnimationBreathe()
{
	MiniBreathe();
	if (MiniIsStop())
		EndWheelAnimation();
	
}

void KTaisuiWnd::PosMoveBreathe()
{
	if (ms_Singleton->m_pThisWnd)
	{
		unsigned long dwCurrent=GetTickCount();
		if (dwCurrent-m_MoveInfo.m_MoveTimer>=TAISUI_WHEEL_POS_MOVE_INTERVAL)
		{
			Point CurPos=ms_Singleton->m_pThisWnd->getAbsolutePosition();
			if ( abs(CurPos.d_x-m_MoveInfo.m_DestX)<=abs(m_MoveInfo.m_Dx)
				&& abs(CurPos.d_y-m_MoveInfo.m_DestY)<=abs(m_MoveInfo.m_Dy)
				)
			{
				
				bool bXShake=false;
				bool bYShake=false;

				if (m_MoveInfo.m_Dx!=0)
					bXShake=true;
				
				if (m_MoveInfo.m_Dy!=0)
					bYShake=true;
				
				  ((TLStaticImage * ) ms_Singleton->m_pThisWnd)->Shake(bXShake,bYShake,4);

                  m_MoveInfo.m_DestX=0;
				  m_MoveInfo.m_DestY=0;
				  m_MoveInfo.m_Dx=0;
				  m_MoveInfo.m_Dy=0;
				  m_MoveInfo.m_MoveTimer=0;
				  m_bMoveFlag=false;

				  ms_Singleton->m_pThisWnd->enable();
				 // ((TLStaticImage * )ms_Singleton->m_pThisWnd)->setDragMovingEnabled(true);

			}//endif
			else
			{
				CurPos.d_x+=m_MoveInfo.m_Dx;
				CurPos.d_y+=m_MoveInfo.m_Dy;
				ms_Singleton->m_pThisWnd->setPosition(Absolute,CurPos);
                ms_Singleton->m_pThisWnd->requestRedraw();
			}//end else 

			m_MoveInfo.m_MoveTimer=GetTickCount();
		}//endif
	}//endif 
}

void KTaisuiWnd::EndWheelAnimation()
{
	ShowWheelResAniamtion();
	
	//Refresh the infos
	m_WheelAniInfo.m_HeadingFrame=-1;
	/*
	m_WheelAniInfo.m_CircleCounter=0;
	m_WheelAniInfo.m_FinalCircle=false;
	*/
	m_bWheeling=false;

	CheckButtonState();

	//Check for Final cirle 
	if (m_State== UTS_WHEEL_DIZHI)
	{
		if (m_ChangeJiaziEvent)
		{
            m_JiaziEvent=TianGanDiZhiToTianXiang(m_ResTianGan,m_ResDizhi);
			ShowJiaziEvent();
			m_ChangeJiaziEvent=false;
		}//endif

		CheckForGiftShow();
		
		
		m_ResTianGan=0;
		m_ResDizhi=0;
		m_State=UTS_IDLE;	   

		if (m_ShowGift)
		{
			m_ShowGift->show();
			m_ShowGift->enable();
			m_WheelAgain->hide();
		}//endif

	}//endif
}

void KTaisuiWnd::LoopWheel()
{
	m_bWheeling=true;
	/*if ( m_WheelAnimation )
	{
		int index=m_WheelAnimation->getCurFrameIdx();
		if (index!=TAISUI_MAX_FRAME_NUM_NUM-1)
			m_WheelAnimation->play(index,TAISUI_MAX_FRAME_NUM_NUM-1);
		else
			m_WheelAnimation->play(0,TAISUI_MAX_FRAME_NUM_NUM-1);

		PlayWheelSound();
	}//endif*/
	
	//m_WheelAniInfo.m_CurrentFrame=0;
	MiniPlay();
   
}

void KTaisuiWnd::WheelTo(const unsigned long dwIndex,const bool bTianGan)
{
   if (bTianGan)
   {
	   assert(dwIndex<=10);
      // m_WheelAniInfo.m_HeadingFrame=dwFrameTianGan[dwIndex-1];
	   PlayStopAt(dwFrameTianGan[dwIndex-1]);
   }//endif
   else
   {
       assert(dwIndex<=12);
     //  m_WheelAniInfo.m_HeadingFrame=dwFrameDizhi[dwIndex-1];
	   PlayStopAt(dwFrameDizhi[dwIndex-1]);
   }//end else
}

void KTaisuiWnd::TopMessage(const char * szString)
{
	if (szString!=NULL)
	{
		char   szBuff[MAX_TOP_MSG_LEN];
		String str;

		sprintf(szBuff,szString);

		str= AnsiToUtf8( szBuff );
		KUiTopMessage::GetSingleton().setText(str);
	}//endif
}

bool KTaisuiWnd::IsAnimationStop()
{
	return MiniIsStop();
}

void KTaisuiWnd::ShowWheelResAniamtion()
{
	if (m_State==UTS_WHEEL_TIANGAN && m_WheelResAniTianGan)
	{   
		Point  pos;
		Point  TargetPos;
		assert(m_ResTianGan<=10 && m_ResTianGan>0);

		switch(m_ResTianGan)
		{
		case 1:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Jia")->getAbsolutePosition();
			}
			break;

        case 2:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Yi")->getAbsolutePosition();
			}
			break;

		case 3:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Bing")->getAbsolutePosition();
			}
			break;

		case 4:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Ding")->getAbsolutePosition();
			}
			break;

		case 5:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Wu")->getAbsolutePosition();
			}
			break;
			
        case 6:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Ji")->getAbsolutePosition();
			}
			break;
			
		case 7:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Geng")->getAbsolutePosition();
			}
			break;
			
		case 8:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Xin")->getAbsolutePosition();
			}
			break;	

		case 9:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Ren")->getAbsolutePosition();
			}
			break;
			
		case 10:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/Gui")->getAbsolutePosition();
			}
			break;		
		}//end for switch 

		pos.d_x=TargetPos.d_x-TAISUI_RES_X;
		pos.d_y=TargetPos.d_y-TAISUI_RES_Y;

		m_WheelResAniTianGan->setPosition(Absolute,pos);
		m_WheelResAniTianGan->show();

		m_WheelResAniTianGan->setCyc(true);	
        m_WheelResAniTianGan->play();
	}//endif

	if (m_State==UTS_WHEEL_DIZHI && m_WheelResAniDizhi)
	{   
	
		Point  pos;
		Point TargetPos;
		assert(m_ResDizhi<=12 && m_ResDizhi>0);

		switch(m_ResDizhi)
		{
		case 1:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/zi")->getAbsolutePosition();
			}
			break;
			
        case 2:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/chou")->getAbsolutePosition();
			}
			break;
			
		case 3:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/yin")->getAbsolutePosition();
			}
			break;
			
		case 4:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/mao")->getAbsolutePosition();
			}
			break;
			
		case 5:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/chen")->getAbsolutePosition();
			}
			break;
			
        case 6:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/si")->getAbsolutePosition();
			}
			break;
			
		case 7:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/wu")->getAbsolutePosition();
			}
			break;
			
		case 8:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/wei")->getAbsolutePosition();
			}
			break;	
			
		case 9:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/shen")->getAbsolutePosition();
			}
			break;
			
		case 10:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/you")->getAbsolutePosition();
			}
			break;
			
		case 11:
			{
				TargetPos = ms_Singleton->ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/xu")->getAbsolutePosition();
			}
			break;
			
		case 12:
			{
				TargetPos = ms_Singleton->m_pThisWnd->getChild("TaharezLook/Taisui/hai")->getAbsolutePosition();
			}
			break;	
		}//end for switch 

		pos.d_x=TargetPos.d_x-TAISUI_RES_X;
		pos.d_y=TargetPos.d_y-TAISUI_RES_Y;
		
		m_WheelResAniDizhi->setPosition(Absolute,pos);
		m_WheelResAniDizhi->show();
		
		m_WheelResAniDizhi->setCyc(true);	
		m_WheelResAniDizhi->play();
	}//endif
}

void KTaisuiWnd::CheckForGiftShow()
{
	if (m_GiftShowFlag)
	{
		GiftEffect(m_GiftRecord);
		((TLStaticImage * ) ms_Singleton->m_pThisWnd)->Shake(0,true,6);
		
		m_GiftShowFlag=false;
		m_GiftRecord=0;
	}//endif

	if (m_bJiaziEventFlag)
	{
		JiazeEffect();
		m_bJiaziEventFlag=false;
		((TLStaticImage * ) ms_Singleton->m_pThisWnd)->Shake(0,true,6);
	}//endif
}

void KTaisuiWnd::NotifyGiftShow(const unsigned long dwGrand)
{
    m_GiftShowFlag=true;
	m_GiftRecord=dwGrand;
}

void KTaisuiWnd::NotifyEventShow()
{
    m_bJiaziEventFlag=true;
}

void KTaisuiWnd::GiftEffect(const unsigned long dwGrand)
{
	if (/*m_TianXiangStatic &&*/ m_TianXiangDay && m_TianXiangMonth)
	{
        char          szJiaziImageTxtName[32];
		
		if (dwGrand==1 || dwGrand==3)
		{
			sprintf(szJiaziImageTxtName,"jiazi%d",m_Day);
			m_TianXiangDay->hide();
			m_TianXiangDay->setImage("tianxiangtxthigh",szJiaziImageTxtName);
			m_TianXiangDay->show();
			m_TianXiangDay->Shake(true,false,8);
		}//endif
		
		if (dwGrand>=2)
		{
			sprintf(szJiaziImageTxtName,"jiazi%d",m_Month);
			m_TianXiangMonth->hide();
			m_TianXiangMonth->setImage("tianxiangtxthigh",szJiaziImageTxtName);
			m_TianXiangMonth->show();
			m_TianXiangMonth->Shake(true,false,8);
		}
	/*	
		char szImageName[32];
		sprintf(szImageName,"riyue%d",dwGrand+1);
		m_TianXiangStatic->setImage("riyue",szImageName);	
		*/
	}//endif

	switch (dwGrand)
	{
	case 1:
        TopMessage(KMessageCentre::GetMessage(taisui_message,TOP_MSG_DAY));
		break;
    case 2:
        TopMessage(KMessageCentre::GetMessage(taisui_message,TOP_MSG_MONTH));
		break;
	case 3:
		TopMessage(KMessageCentre::GetMessage(taisui_message,TOP_MSG_D_M));
		break;
	}

	if (m_TianXiangEffect)
	{
		m_TianXiangEffect->show();
		m_TianXiangEffect->play(0,TIANXIANG_EFFECT_FRAME-1);
	}//endif

	PlayTianXiangSound();
	
}

void KTaisuiWnd::JiazeEffect(void)
{
	TopMessage(KMessageCentre::GetMessage(taisui_message,TOP_MSG_JIAZI));
	
	if (m_JiaziEvent!=0)
	((TLStaticImage *)m_JiaziEventTxt[m_JiaziEvent-1])->Shake(true,false,8);
	
	PlayJiaziSound();

	if (m_JiaziEffect)
	{
		Point pos;
		pos.d_x=TAISUI_EFFECT_POS_START_X+(TAISUI_EFFECT_POS_START_DIS * (m_JiaziEvent /12));
		pos.d_y=TAISUI_EFFECT_POS_START_Y;

		m_JiaziEffect->setPosition(Absolute,pos);
		m_JiaziEffect->show();
		m_JiaziEffect->play(0,TAISUI_EFFECT_FRAME-1);	
	}//endif

}

void KTaisuiWnd::PlayWheelSound(void)
{
	if (m_WheelSound)
	{
		m_WheelSound->hide();
		m_WheelSound->show();
		m_WheelSound->hide();
	}//endif

}

void KTaisuiWnd::PlayJiaziSound(void)
{
    if (m_JiaziEventSound)
	{
		m_JiaziEventSound->hide();
		m_JiaziEventSound->show();
		m_JiaziEventSound->hide();
	}//endif

}

bool KTaisuiWnd::ShowGiftUi(const CEGUI::EventArgs& args)
{
    g_pCoreShell->OperationRequest(GOI_TAISUI_SHOW_RES,0,0);
	Point CurPos=ms_Singleton->m_pThisWnd->getAbsolutePosition();
	MoveTo(1200,CurPos.d_y,80,0);
	
	m_ShowGift->disable();
	
	return true;
}

bool KTaisuiWnd::OnCloseTipClose(const CEGUI::EventArgs& args)
{
	if (m_CloseTip)
	{
        //m_CloseTip->setModalState(false);
	}//endif

	return true;
}

bool KTaisuiWnd::OnHidden(const CEGUI::EventArgs& args)
{
    if (m_pThisWnd)
	{
		m_pThisWnd->setModalState(false);
		//m_CloseTip->setModalState(false);

	}
	return true;
}

void KTaisuiWnd::PlayTianXiangSound()
{
	if (m_TianXiangEventSound)
	{
		m_TianXiangEventSound->hide();
		m_TianXiangEventSound->show();
		m_TianXiangEventSound->hide();
	}//endif
}

//TaisuiWheel move info
KTaisuiWnd::tagTAISUI_MOVE_INFO::tagTAISUI_MOVE_INFO()
:m_Dx(0),m_Dy(0),m_DestX(0),m_DestY(0),m_MoveTimer(GetTickCount())
{	}

void KTaisuiWnd::MoveTo(const int iDestX,const int iDestY, const int iDx,const int iDy)
{
   if (!m_bMoveFlag)
   {
        m_bMoveFlag=true;
		m_MoveInfo.m_DestX=iDestX;
		m_MoveInfo.m_DestY=iDestY;
		m_MoveInfo.m_Dx=iDx;
		m_MoveInfo.m_Dy=iDy;
		m_MoveInfo.m_MoveTimer=GetTickCount();

		((TLStaticImage * )ms_Singleton->m_pThisWnd)->setDragMovingEnabled(false);
   }//endif
}

#define MINI_PLAYER_MAX_FRAME_SPEED  6
#define MINI_PLAYER_SPEED_CHANGE_INTERVAL 20
#define MINI_PLAYER_FRAME_INTERVAL   5
#define MINI_PLAYER_SOUND_FRAME_INTERVAL 36

KTaisuiWnd::tagTAISUI_ANI_MINIPLAYER::tagTAISUI_ANI_MINIPLAYER():m_CurFrameSpeed(0),m_CurFrame(0)
,m_TotalFrameCounter(0),m_Timer(0),m_bPlay(false){}

KTaisuiWnd::tagTAISUI_ANI_MINIPLAYER::~tagTAISUI_ANI_MINIPLAYER()
{
  /*Do Nothing at all*/
}


void KTaisuiWnd::MiniPlay()
{
	if (m_WheelAnimation)
	{
		m_MiniPlayer.m_CurFrameSpeed=1;
		m_MiniPlayer.m_Timer=GetTickCount();
        m_MiniPlayer.m_TotalFrameCounter=0;
		m_MiniPlayer.m_ImageFrameCounter=0;
		m_MiniPlayer.m_bPlay=true;
		m_MiniPlayer.m_State=tagTAISUI_ANI_MINIPLAYER::speedup;
		m_WheelAnimation->play(m_MiniPlayer.m_CurFrame,m_MiniPlayer.m_CurFrame);
	}//endif
}

void KTaisuiWnd::MiniBreathe()
{
	if (m_WheelAnimation && m_MiniPlayer.m_bPlay )
	{
		unsigned long dwCurTime=GetTickCount();
		
		if (dwCurTime-m_MiniPlayer.m_Timer>=MINI_PLAYER_FRAME_INTERVAL)
		{
			m_MiniPlayer.m_TotalFrameCounter+=1;
			m_MiniPlayer.m_CurFrame=(m_MiniPlayer.m_CurFrame+m_MiniPlayer.m_CurFrameSpeed) % TAISUI_MAX_FRAME_NUM_NUM;

			m_MiniPlayer.m_ImageFrameCounter+=m_MiniPlayer.m_CurFrameSpeed;

			if (m_MiniPlayer.m_ImageFrameCounter>=MINI_PLAYER_SOUND_FRAME_INTERVAL)	
			{
				PlayWheelSound();
				m_MiniPlayer.m_ImageFrameCounter=0;
			}//endif

			if (m_MiniPlayer.m_TotalFrameCounter>=MINI_PLAYER_SPEED_CHANGE_INTERVAL)
			{
				m_MiniPlayer.m_TotalFrameCounter=0;

				switch(m_MiniPlayer.m_State)
				{
			    	case (tagTAISUI_ANI_MINIPLAYER::speedup):
                    {
						if (m_MiniPlayer.m_CurFrameSpeed<MINI_PLAYER_MAX_FRAME_SPEED)
							m_MiniPlayer.m_CurFrameSpeed+=1;
						else
						{
							if (m_WheelAniInfo.m_HeadingFrame>=0)
                                  m_MiniPlayer.m_State=tagTAISUI_ANI_MINIPLAYER::speeddown;
						}
					}
					break;

					case (tagTAISUI_ANI_MINIPLAYER::speeddown):
					{
					    if (m_MiniPlayer.m_CurFrameSpeed>1)
							m_MiniPlayer.m_CurFrameSpeed-=1;
					}
					break;

				}
			
			}//endif
			
			m_WheelAnimation->play(m_MiniPlayer.m_CurFrame,m_MiniPlayer.m_CurFrame);	
			m_MiniPlayer.m_Timer=GetTickCount();

			if (m_MiniPlayer.m_State==tagTAISUI_ANI_MINIPLAYER::speeddown && m_MiniPlayer.m_CurFrameSpeed==1 && m_MiniPlayer.m_CurFrame==m_WheelAniInfo.m_HeadingFrame)
			{
				m_MiniPlayer.m_bPlay=false;
			}//endif
			
		}//endif
	
	}//endif
}

void KTaisuiWnd::PlayStopAt(const int nStopAtIndex)
{
    m_WheelAniInfo.m_HeadingFrame=nStopAtIndex;
}

void KTaisuiWnd::MiniStop()
{
	m_MiniPlayer.m_bPlay=false;
}

bool KTaisuiWnd::MiniIsStop()
{
	return !m_MiniPlayer.m_bPlay;
}

bool KTaisuiWnd::OnHelp(const CEGUI::EventArgs& args)
{
	//zhangxin KUiHelpInfo窗口已经不在使用，功能合并到封神宝典中
	//KUiHelpInfo::GetSingleton().Show(); 
	return true;
}