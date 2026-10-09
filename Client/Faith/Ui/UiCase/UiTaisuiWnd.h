#ifndef UI_TAISUI_WND_H
#define UI_TAISUI_WND_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/30/2007 18:05
//      File_base        : UiTaisuiWnd
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : Ì«ËêÖ®ÂÖUi½çÃæ
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreShell.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "GameDataDef.h"


#define MAX_JIAZI_EVENT_NUM 60

class KTaisuiWnd : public KUiWndSingleton<KTaisuiWnd>
{
	//TianXiang...
	static DWORD           m_Day;
	static DWORD           m_Month;
	//JiaziEvent
	static DWORD           m_JiaziEvent;
    //WheeldRes
	static DWORD           m_ResTianGan;
	static DWORD           m_ResDizhi;
    //TimesInfo
	static DWORD           m_AvailableTimes;
	static DWORD           m_WheeledTimes;
	//ChildWindow
	static TLStaticText *  m_DayInfo;
	static TLStaticText *  m_MonthInfo;
    static TLStaticImage*  m_JiaziEventTxt[MAX_JIAZI_EVENT_NUM];
    static TLStaticImage*  m_TianXiangStatic;
	static TLStaticImage*  m_TianXiangDay;
	static TLStaticImage*  m_TianXiangMonth;
	static TLStaticImage*  m_WheelAnimation;
	static TLStaticImage*  m_WheelResAniTianGan;
    static TLStaticImage*  m_WheelResAniDizhi;
    static TLStaticImage*  m_CloseTip;
	static TLStaticImage*  m_JiaziEffect;
	static TLStaticImage*  m_TianXiangEffect;
	/*
    #ifdef _DEBUG
    static TLStaticText * m_TimesInfo;
    #endif
    */
	static TLButton     * m_Close;
	static TLButton     * m_Wheel;
	static TLButton     * m_WheelAgain;
	static TLButton     * m_ShowGift;
	//Sound Used Buff
	static TLStaticText * m_WheelSound;
	static TLStaticText * m_JiaziEventSound;
	static TLStaticText * m_TianXiangEventSound;

	enum   UI_TAISUI_STATE
	{
      UTS_IDLE,
	  UTS_WHEEL_TIANGAN,
	  UTS_WHEEL_DIZHI,
	};

	static UI_TAISUI_STATE  m_State;

	//Breathe flag
	static bool          m_bShowFlag;
	static bool          m_bWheeling;
	static bool          m_bJiaziEventFlag;
	static bool          m_GiftShowFlag;
	static unsigned long m_GiftRecord;

	//Wheel Animation
	typedef struct tagTAISUI_WHEEL_ANI
	{
	  int                m_HeadingFrame;
	  /*
	  int                m_CurrentFrame;
	  unsigned long      m_CircleCounter;
	  bool               m_FinalCircle;
	  */
	  tagTAISUI_WHEEL_ANI();
	  ~tagTAISUI_WHEEL_ANI();
	}TAISUI_WHEEL_ANI;

	typedef struct tagTAISUI_ANI_MINIPLAYER
	{
	  enum play_state
	  {
			speedup=0,
			speeddown	
	  };

      int                m_CurFrameSpeed;
	  int                m_CurFrame;
	  int                m_TotalFrameCounter;
	  int                m_ImageFrameCounter;
	  unsigned long      m_Timer;
	  bool               m_bPlay;
      play_state         m_State;

      tagTAISUI_ANI_MINIPLAYER();
	  ~tagTAISUI_ANI_MINIPLAYER();
	}tagTAISUI_ANI_MINIPLAYER;
    
	static TAISUI_WHEEL_ANI         m_WheelAniInfo;
    static tagTAISUI_ANI_MINIPLAYER m_MiniPlayer;
	//Show Pos Moving info
	typedef struct tagTAISUI_MOVE_INFO
	{
      int                m_Dx;
	  int                m_Dy;
	  int                m_DestX;
	  int                m_DestY;
	  unsigned long      m_MoveTimer;
	  tagTAISUI_MOVE_INFO();
	}TAISUI_MOVE_INFO;

	static TAISUI_MOVE_INFO     m_MoveInfo;
	static bool                 m_bMoveFlag;
	static bool                 m_ChangeJiaziEvent;

public:
	KTaisuiWnd(  const CEGUI::String& id_name );
	~KTaisuiWnd(								);
public:
	static void Show();
	static void Hide();
	static void Breathe();
	static void EnableHide();
public:
	static void UpdataAll();
	static void UpdateTianXiang();
	static void UpdateJiaziEvent();
	static void UpdateTianGanRes();
	static void UpdateDizhiRes();
	static void UpdateTimesInfo();
	static void Disconnect();
	static void NotifyGiftShow(const unsigned long dwGrand);
	static void NotifyEventShow();
	void        Init();	
private:
	bool        OnKeyDown( const CEGUI::EventArgs & args);
	bool        OnClose( const CEGUI::EventArgs& args );
	bool        OnWheel( const CEGUI::EventArgs& args );
	bool        OnCloseTipOk(const CEGUI::EventArgs& args);
	bool        OnCloseTipCancel(const CEGUI::EventArgs& args);
	bool        ShowGiftUi(const CEGUI::EventArgs& args);
	bool        OnCloseTipClose(const CEGUI::EventArgs& args);
	bool        OnHidden(const CEGUI::EventArgs& args);
	bool        OnHelp(const CEGUI::EventArgs& args);
private:
    static void ShowJiaziEvent();
	static void ShowTianXiang();
	static void ShowTianGanRes();
	static void ShowDizhiRes();
	static void HilightRiYue();
	static void LoopWheel();
	static void WheelTo(const unsigned long dwIndex,bool bTianGan);
	static void CheckButtonState();
	static void AnimationBreathe();
	static void PosMoveBreathe();
	static void EndWheelAnimation();
	static void TopMessage(const char * szString);
	static bool IsAnimationStop();
	static void CheckForGiftShow();
	static void ShowWheelResAniamtion();
private:
	static void MoveTo(const int iDestX,const int iDestY, const int iDx,const int iDy);
	static void GiftEffect(const unsigned long dwGrand);
	static void JiazeEffect(void);
	static void PlayWheelSound(void);
	static void PlayJiaziSound(void);
	static void PlayTianXiangSound(void);
private:
	static void MiniPlay();
	static void PlayStopAt(const int nStopAtIndex);
	static void MiniBreathe();
	static void MiniStop();
	static bool MiniIsStop();
};

#endif