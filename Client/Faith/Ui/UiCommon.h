//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/14/2006 11:15
//      File_base        : UiCommon
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 界面应用模板
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KUIWNDSINGLETON_H
#define KUIWNDSINGLETON_H

#include "CEGUI.h"
#include "UiMDLInterface.h"
#include "TLStatic.h"
#include "CoreUseNameDef.h"
#include "KWin32Wnd.h"
#include "../UiSheetMgr.h"
#include "ItemCommonDef.h"
#include <map>
#include <string>
#include <fstream>
using namespace std;

#ifdef _DEBUG
#include <crtdbg.h>



inline void EnableMemLeakCheck()
{
   _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif//*/

enum EVENT_ID
{
	//LoginBK
	UI_LOGINBK_LOGIN = 1,
	UI_LOGINBK_EXIT,
	UI_LOGINBK_WEB,
	UI_LOGINBK_PAY,
	UI_LOGINBK_REGIST,
	//PassWord
	UI_PASSWORD_LOGIN,
	UI_PASSWORD_BACK,
	//SelRole
	UI_SELROLE_LOGIN,
	UI_SELROLE_DELETE,
	UI_SELROLE_NEW,
	UI_SELROLE_BACK,
	//NewRole
	UI_NEWROLE_NEW,
	UI_NEWROLE_CANCEL,
	UI_NEWROLE_BACK,
	UI_NEWROLE_ROLE_A,
	UI_NEWROLE_ROLE_B,
	UI_NEWROLE_ROLE_C,
	UI_NEWROLE_ROLE_D,
	UI_NEWROLE_ROLE_E,
	UI_NEWROLE_ROLE_F,
	UI_SELECTROLEA,
	UI_SELECTROLEB,
	//Chat room
	UI_CHATROOM_SEND,
	UI_CHATROOM_CLOSE,
	UI_CHATROOM_ADD,
	//Navigation
	UI_FRIEND_MGR,
	//Exit box
	UI_EXIT_GAMESPACE,
	UI_CANCEL_GAMESPACE,
	//Chat Channel
	UI_CHATCHANNEL_SEND,
	UI_CHANNEL_0,	
	UI_CHANNEL_1,
	UI_CHANNEL_2,
	UI_CHANNEL_3,
	UI_CHANNEL_4,
	//Chat & Friend centre
	UI_FRIEND_SHOW,
	UI_ENEMY_SHOW,
	UI_SCREEN_SHOW,
	UI_ROOM_SHOW,
	UI_TEMPORARY_SHOW,
	UI_ADD_FRIEND,
};

enum CLOSE_EVENT_ID
{
	PLAYER_RUN,
	PLAYER_DEATH,
	PLAYER_TRANSMISION,
	SERVER_DISCONNECT,
	GAMESPACE_CLICKED,
};

class KUiWnd : public IUIMDLEvent
{
public:
	KUiWnd();
	~KUiWnd()
	{
		m_pCEGUISystem		= NULL;
		m_pWindowManager	= NULL;
		m_pRootSheet		= NULL;
		m_pThisWnd			= NULL;
	}

public:
	virtual inline	void	CreateWnd		( const CEGUI::String& strLayout, const CEGUI::String& strName	);
	virtual inline	void	ReleaseWnd		( void															);
    virtual inline	void	ToggleVisibility( void															);
    virtual inline	bool	IsVisible		( void															);
	virtual inline	void	Show			( void															);
	virtual inline	void	Hide			( void															);
	virtual void			onCreate		( UIMDLEvent& rEvent											);
	virtual void			onRelease		( UIMDLEvent& rEvent											);
	virtual void			onChange		( UIMDLEvent& rEvent											);

protected:
	CEGUI::System*			m_pCEGUISystem;
	CEGUI::WindowManager*	m_pWindowManager;
	CEGUI::Window*			m_pRootSheet;
	CEGUI::Window*			m_pThisWnd;
	IUIMDL*					m_pUiMDLManager;					
};

inline void KUiWnd::CreateWnd( const CEGUI::String& strLayout, const CEGUI::String& strName )
{
	if ( m_pWindowManager )
	{
		m_pThisWnd = m_pWindowManager->loadWindowLayout( strLayout, strName );
	}
}

inline void KUiWnd::ReleaseWnd( void )
{
	if ( m_pWindowManager )
	{
		m_pWindowManager->destroyWindow( m_pThisWnd );
	}
}


inline void KUiWnd::ToggleVisibility()
{
	if ( m_pThisWnd )
	{
		if ( m_pThisWnd->isVisible() )
		{
			Hide();
		} 
		else
		{
			Show();
		}
	}
}

inline bool KUiWnd::IsVisible() 
{
	if ( m_pThisWnd )
	{
		return m_pThisWnd->isVisible();
	}
	return false;
}

inline void KUiWnd::Show( void )
{
	if ( m_pRootSheet && m_pThisWnd && m_pCEGUISystem )
	{
		m_pRootSheet->addChildWindow( m_pThisWnd );
		m_pThisWnd->show(); 
		//(static_cast<TLStaticImage *>(m_pThisWnd))->DoShow();
		//m_pCEGUISystem->plusShowEditNum( m_pThisWnd->getName() );
	}
}

inline void KUiWnd::Hide( void )
{
	if ( m_pRootSheet && m_pThisWnd )
	{
		m_pRootSheet->removeChildWindow( m_pThisWnd );
		m_pThisWnd->hide();
		//m_pCEGUISystem->minusShowEditNum( m_pThisWnd->getName() );
	}	
}

inline void	KUiWnd::onCreate( UIMDLEvent& rEvent )
{

}

inline void	KUiWnd::onRelease( UIMDLEvent& rEvent )
{

}

inline void	KUiWnd::onChange( UIMDLEvent& rEvent )
{

}



template<typename wndtype> 
class  KUiWndSingleton : public IUIMDLEvent
{
public:
	KUiWndSingleton( CEGUI::String strPath ) 
	{
		assert( !ms_Singleton ); 
		if ( g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768 )
		{
			char* temp = Utf8ToAnsi(m_strPath);
			changePath( Utf8ToAnsi(strPath), temp );
			m_strPath = temp;
		}
		else
		{
			m_strPath		= strPath;
		}
		ms_Singleton		= static_cast<wndtype*>(this); 
		m_pCEGUISystem		= CEGUI::System::getSingletonPtr();
		m_pWindowManager	= CEGUI::WindowManager::getSingletonPtr();
		m_pRootSheet		= KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
		m_pThisWnd			= NULL;
		//m_strPath			= strPath;
		m_nSex				= -1;
		m_nMetier			= -1;
		m_nSkillType		= -1;
		GetMDLPtr( &m_pUiMDLManager );
	}
	~KUiWndSingleton( void )
	{ 
		assert( ms_Singleton );
		ms_Singleton		= NULL;
		m_pCEGUISystem		= NULL;
		m_pWindowManager	= NULL;
		m_pRootSheet		= NULL;
		m_pThisWnd			= NULL;
	}

public:
    inline static wndtype&	GetSingleton	( void );
    inline static wndtype*	GetSingletonPtr	( void );
    inline static void 		ToggleVisibility( void );
    inline static BOOL		IsVisible		( void );
	inline static void		DestroyWindow	( void );
	inline static void		Show			( void );
	inline static void		Hide			( void );
	inline static bool		IsInit			( void );

	virtual void			onCreate		( UIMDLEvent& rEvent	);
	virtual void			onRelease		( UIMDLEvent& rEvent	);
	virtual void			onChange		( UIMDLEvent& rEvent	);
	virtual void			Init			( void					);

protected:
	void					changePath( char *pSounce, char *pResult );

protected:
    static wndtype*			ms_Singleton;
	CEGUI::System*			m_pCEGUISystem;
	CEGUI::WindowManager*	m_pWindowManager;
	CEGUI::Window*			m_pRootSheet;
	CEGUI::Window*			m_pThisWnd;
	CEGUI::String			m_strPath;
	IUIMDL*					m_pUiMDLManager;	
	int						m_nSex;
	int						m_nMetier;
	int						m_nSkillType;
};

template<typename wndtype>
inline wndtype& KUiWndSingleton<wndtype>::GetSingleton( void )
{  
	if(ms_Singleton->m_pThisWnd == NULL)
	{
#ifndef _DEBUG
		try
		{
#endif
			ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );

			if ( ms_Singleton->m_pThisWnd )
			{
				ms_Singleton->m_pThisWnd->hide(); 
				ms_Singleton->Init();
			}
#ifndef _DEBUG
		}
		catch (...)
		{
		}
#endif
	}
	assert( ms_Singleton );  
	return ( *ms_Singleton );  
}

template<typename wndtype>
inline wndtype* KUiWndSingleton<wndtype>::GetSingletonPtr( void )
{  
	if(ms_Singleton->m_pThisWnd == NULL)
	{
#ifndef _DEBUG
		try
		{
#endif
			ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );

			if ( ms_Singleton->m_pThisWnd )
			{
				ms_Singleton->m_pThisWnd->hide(); 
				ms_Singleton->Init();
			}
#ifndef _DEBUG
		}
		catch (...)
		{
		}
#endif
	}
	assert( ms_Singleton );  
	return  ms_Singleton;   
}

template<typename wndtype>
void KUiWndSingleton<wndtype>::ToggleVisibility()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		if ( ms_Singleton->m_pThisWnd->isVisible() )
		{
			ms_Singleton->Hide();
		} 
		else
		{
			ms_Singleton->Show();
		}
	}
	else
	{
		if ( ms_Singleton )
		{
			ms_Singleton->Show();
		}
	}
}

template<typename wndtype>
BOOL KUiWndSingleton<wndtype>::IsVisible() 
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		return ms_Singleton->m_pThisWnd->isVisible();
	}
	return FALSE;
}

template<typename wndtype> inline
void KUiWndSingleton<wndtype>::DestroyWindow()
{
	if ( ms_Singleton )
	{
		delete ms_Singleton;
		ms_Singleton = NULL;
	}
}

template<typename wndtype> inline
void KUiWndSingleton<wndtype>::Show( void )
{
#ifndef _DEBUG
	try
	{
#endif
		if ( ms_Singleton->m_pRootSheet && ms_Singleton->m_pThisWnd == NULL )
		{
			ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath );

			assert( ms_Singleton->m_pThisWnd );
			if ( ms_Singleton->m_pThisWnd )
			{
				ms_Singleton->m_pRootSheet->addChildWindow( ms_Singleton->m_pThisWnd );
				//ms_Singleton->m_pThisWnd->activate();
				ms_Singleton->m_pThisWnd->show(); 
				ms_Singleton->Init();
				(static_cast<TLStaticImage *>(ms_Singleton->m_pThisWnd))->DoShow();
			}
		}
		else
		{
			ms_Singleton->m_pRootSheet->addChildWindow( ms_Singleton->m_pThisWnd );
			//ms_Singleton->m_pThisWnd->activate();
			ms_Singleton->m_pThisWnd->show(); 
			(static_cast<TLStaticImage *>(ms_Singleton->m_pThisWnd))->DoShow();
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		KUiWndSingleton<wndtype>::Hide();
	}
#endif
}

template<typename wndtype> inline 
void KUiWndSingleton<wndtype>::Hide( void )
{
	if ( ms_Singleton && ms_Singleton->m_pRootSheet && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pRootSheet->removeChildWindow( ms_Singleton->m_pThisWnd );
		ms_Singleton->m_pThisWnd->hide();
	}
}
template<typename wndtype> inline 
void	KUiWndSingleton<wndtype>::onCreate( UIMDLEvent& rEvent	)
{

}

template<typename wndtype> inline 
void	KUiWndSingleton<wndtype>::onRelease( UIMDLEvent& rEvent	)
{

}

template<typename wndtype> inline 
void	KUiWndSingleton<wndtype>::onChange( UIMDLEvent& rEvent	)
{
	
}

template<typename wndtype> inline 
void	KUiWndSingleton<wndtype>::Init( void )
{
	
}

template<typename wndtype> inline 
bool	KUiWndSingleton<wndtype>::IsInit( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
		return true;
	
	return false;
}

template<typename wndtype> inline
void	KUiWndSingleton<wndtype>::changePath( char *pSounce, char *pResult )
{
	char tempbuf[COMMON_CLIENT_MSG_LEN_64] = {0};
	int  slen = strlen(UI_LAYOUT_PATH);
	int	 dlen = strlen(pSounce);
	strncpy(tempbuf, pSounce + slen,  dlen - slen);
	strncpy(pResult, UI_1024_PATH, sizeof(UI_1024_PATH) );
	strcat(pResult, tempbuf);
}

int		uiMoneyToSysMoney(int jin, int yin, int tong);

void	sysMoneyToUiMoney(int money, int& jin, int& yin, int& tong);

CEGUI::String iToString(int num);

//string iToStr(int num);

inline string iToStr(int num)
{
	char newString[20];
	_itoa(num, newString, 10);
	return string(newString);
}

void	lorectToCerect(void* lorect, CEGUI::Rect* cerect);

void	cerectToLorect(CEGUI::Rect* cerect, void* lorect);

bool	_vendueFindItemFromMDL( void* pParam0, void* pParam1 );

void	ansiToUnicode(const char* ansiText, wchar_t*& unicodeText);

int		unicodeToAnsi(const wchar_t* unicodeText, char*& ansiText);

void	printWnd(int layer, CEGUI::Window* wnd, char* fileName);

void	PrintWindowProperty(const char* windowName);

void	closeUiWnd(CLOSE_EVENT_ID e);

const Image* 	getImage(const char* imagePath);

void deleteImageSet(const char* setName);

void playSound(const char* soundPath);

#undef UI_RELEASE_TRY
#ifndef _DEBUG
#define UI_RELEASE_TRY try{
#else
#define UI_RELEASE_TRY
#endif

#undef UI_RELEASE_CATCH
#ifndef _DEBUG
#define UI_RELEASE_CATCH }catch(...){}
#else
#define UI_RELEASE_CATCH
#endif

#undef UI_RELEASE_CATCH_RETURN
#ifndef _DEBUG
#define UI_RELEASE_CATCH_RETURN(expr) }catch(...){return expr;}
#else
#define UI_RELEASE_CATCH_RETURN(expr)
#endif

#undef UI_RELEASE_CATCH_RETURN_NOTHING
#ifndef _DEBUG
#define UI_RELEASE_CATCH_RETURN_NOTHING }catch(...){return;}
#else
#define UI_RELEASE_CATCH_RETURN_NOTHING
#endif

#undef UI_RELEASE_CATCH_BEGIN
#ifndef _DEBUG
#define UI_RELEASE_CATCH_BEGIN }catch(...){
#else
#define UI_RELEASE_CATCH_BEGIN
#endif

#undef UI_RELEASE_CATCH_END
#ifndef _DEBUG
#define UI_RELEASE_CATCH_END }
#else
#define UI_RELEASE_CATCH_END
#endif
#endif 
