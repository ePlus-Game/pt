// UiInfoBar.h: interface for the KUiInfoBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIINFOBAR_H__16399AEE_2EB8_41B3_82D2_513B030E98AA__INCLUDED_)
#define AFX_UIINFOBAR_H__16399AEE_2EB8_41B3_82D2_513B030E98AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../UiCommon.h"

//////////////////////////////////////////////////////////////////////////
//				class KUiInfoBarPing
//////////////////////////////////////////////////////////////////////////
class KUiInfoBarPing : public KUiWndSingleton<KUiInfoBarPing>
{
public:
	KUiInfoBarPing( const CEGUI::String& id_name );
	virtual ~KUiInfoBarPing();

	void Init();
	void UpdateNetInfo( DWORD nPing );
	void SetServerName( string& serverName );

private:
	void RefreshPingImage( DWORD ping );
	void RefreshPingTips( DWORD ping );

	bool window_MouseEnters( const CEGUI::EventArgs& e );
	bool window_MouseLeaves( const CEGUI::EventArgs& e );

private:
	TLStaticImage*	m_imgFast;
	TLStaticImage*	m_imgNormal;
	TLStaticImage*	m_imgSlow;
	TLStaticText*	m_stServerName;

	DWORD			m_dwPing;
	int				m_oldPingstate;
	DWORD			m_oldPing;

	const int		m_pingInnerDelay;
	int				m_LagStateSpace;
};

//////////////////////////////////////////////////////////////////////////
//				class KUiInfoBarTime
//////////////////////////////////////////////////////////////////////////
class KUiInfoBarTime : public KUiWndSingleton<KUiInfoBarTime>
{
public:
	KUiInfoBarTime( const CEGUI::String& id_name );
	virtual ~KUiInfoBarTime();
	
	void Init();
	static void Breathe();
	void DoBreathe();

private:
	void FlipTime();
	void RefreshTime();

	bool window_MouseEnters( const CEGUI::EventArgs& e );
	bool window_MouseLeaves( const CEGUI::EventArgs& e );

private:
	TLStaticText*	m_stDate;
	TLStaticText*	m_stTime;
	const int		s_breatheInterval;
	int				m_FlipInterval;
	int				m_curTick;
	int				m_curFlipTick;
	string			m_TimeTipFormatString;
};

#endif // !defined(AFX_UIINFOBAR_H__16399AEE_2EB8_41B3_82D2_513B030E98AA__INCLUDED_)
