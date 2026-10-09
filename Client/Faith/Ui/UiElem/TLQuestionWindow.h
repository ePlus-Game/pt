// TLQuestionWindow.h: interface for the TLQuestionWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLQUESTIONWINDOW_H__39141080_7210_49CD_9ED6_39ACF37F56D9__INCLUDED_)
#define AFX_TLQUESTIONWINDOW_H__39141080_7210_49CD_9ED6_39ACF37F56D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TLStatic.h"
#include "TLIEWindow.h" 
#include "TLEditBox.h"
#include "TLButton.h"
#include <string>

using namespace std;

namespace CEGUI
{

/********************************************************************
/*						class: TLQuestionWindow
*********************************************************************/

class TLQuestionWindow : public StaticImage
{
public:
	TLQuestionWindow( const String& type, const String& name );
	virtual ~TLQuestionWindow();

	virtual void initialise();
	virtual	void onShown( WindowEventArgs& e );
	virtual void onHidden( WindowEventArgs& e );
	virtual void injectTimePulse( DWORD curUpdateTime );

	bool	RefreshFile( unsigned int uParam );
	void	SendEmptyAnswer();
	void	setTotalTime( int totalTime );
public:
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget
	static const String TemplateLayoutsFileName;

private:
	bool	InputBox_TextChanged( const CEGUI::EventArgs& e );
	bool	InputBox_KeyDown( const CEGUI::EventArgs& e );
	bool	btnOK_Clicked( const CEGUI::EventArgs& e );
	bool	sendAnswer( bool isEmpty = false );
	void	setCountDownVisible( bool visible );

private:
	Window*			m_mainWindow;

	TLStaticImage*	m_IEWindow;
	TLStaticText*	m_txtPlusShow;
	TLStaticText*	m_txtShow;
	TLEditbox*		m_InputBox;
	TLButton*		m_BtnOK;
	TLStaticText*	m_txtTimeShow;
	TLStaticText*	m_txtTimeTitle;
	TLStaticText*	m_txtTimeUnit;
	Window*			m_parent;
	Window*			m_root;
	String			m_sMainWindowName;
	int				m_totalSeconds;
	int				m_currentSeconds;
	int				m_oldTime;
	string			m_filePath;
	string			m_curAccount;
	string			m_cfgQuestionFilePath;
};


/********************************************************************
/*						class: TLQuestionWindowFactory
*********************************************************************/

class TAHAREZLOOK_API TLQuestionWindowFactory : public WindowFactory
{
public:
	TLQuestionWindowFactory() : WindowFactory( TLQuestionWindow::WidgetTypeName ) { }
	~TLQuestionWindowFactory(){}
public:
	Window*	createWindow( const String& name );
	virtual void	destroyWindow( Window* window )	 { if ( window->getType() == d_type ) delete window; }
};


}
#endif // !defined(AFX_TLQUESTIONWINDOW_H__39141080_7210_49CD_9ED6_39ACF37F56D9__INCLUDED_)
