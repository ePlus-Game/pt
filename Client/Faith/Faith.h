//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/26/2006 17:31
//      File_base        : Faith
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////



#ifndef FAITH_H
#define	FAITH_H

#include "KEngine.h"
#include "KPakList.h"
#include "KMp3Music.h"
#include "CoreShell.h"
#include "ErrorCode.h"
#include "Ui/UiAdapter.h"
#include <vector>

#ifdef _USEROBOT
	#include "../Login/Login.h"
	#include "RobotControl.h"
#endif

// Macro define
#define MAXSIZE_OSINFO			128

#ifdef _USEROBOT
	enum enLOGINSTEP
	{
		enCnnServer = 0, 
		enAccountLogin, 
		enAccountLoginRep, 
		enEnterGame, 
		enCreateAccount, 
		enInGameRep, 
		enLoginFinish,
		enLoginIdle, 
	};
#endif


struct KClientCallback : public IClientCallback
{
	void CoreDataChanged		( unsigned int uDataId, unsigned int uParam, int nParam	);
};

class KB2Frame : public KWin32Frame
{
public:
	KB2Frame();
	virtual~KB2Frame();

private:
	BOOL				GameInit		  (												);
	BOOL				GameLoop		  (												);
	BOOL				GameExit		  (												);
	int					HandleInput		  ( UINT uMsg, WPARAM wParam, LPARAM lParam		);
	void				SetFocus		  (												);
	BOOL				InitRepresentShell( BOOL bFullScreen, int nWidth, int nHeight	);

	void				CheckFPS		  (	int FPS 									);

private:
	KMp3Music			m_Music;
	KDirectSound		m_Sound;
	KTimer				m_Timer;
	DWORD				m_dwPaintCounter;
	DWORD				m_dwStartTime;
	KPakList			m_pakList;
	KIniFile			m_iniFile;
	int					m_nPaintFpsLimit;
	HANDLE				m_hSemaphore;
	KUiAdapter			m_uiAdapter;
	int					m_WarningCount;
	std::vector<int>	m_vFPSCounter;
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	HMODULE				m_hRepresentModule;
	int					m_bRepresent3;
	DEVMODE				m_devMode;
#endif
};

class B2Application : public KWin32App
{
public:
	B2Application()				{ KClientError::Error_Init();	 }
	virtual ~B2Application()	{ KClientError::Error_Release(); }

public:
	BOOL		CreateApplication		( KWin32AppParam &rAppParam			);
	BOOL		RunApplication			(									);
	
private:
	void		OutputExceptionMessage	( const TCHAR* szMessage			) const;
	void		LOGSystemInfo			(									);
	void		WriteHardware			(									);
	const char *GetOSInfo				(									);

public:
	BOOL Blaze();
};

extern bool					g_draw;
extern bool					g_bShowFPS;
extern const unsigned short	g_GamePaintLimit;
extern bool					g_showtip;
extern char					g_ServerInfo[64];
extern char					g_Accounts[256];

#endif