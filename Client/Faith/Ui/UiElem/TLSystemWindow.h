// UiSysWindow.h: interface for the KUiSysWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UISYSWINDOW_H__0FC864C4_49EF_4673_B9B9_7AC1AABDAB48__INCLUDED_)
#define AFX_UISYSWINDOW_H__0FC864C4_49EF_4673_B9B9_7AC1AABDAB48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "CEGUIStaticImage.h"
#include "TLStatic.h"

using namespace CEGUI;

namespace CEGUI
{
class TLSystemWindow : public StaticImage
{
public:
	TLSystemWindow( const String& type, const String& name );
	virtual ~TLSystemWindow();

	virtual void	onShown(WindowEventArgs& e);
	virtual void	onHidden(WindowEventArgs& e);

	void setEnabled( bool setting );
	void setPosition( MetricsMode mode, const Point& position );
	void setPosition( int dx, int dy );

	void setSize( MetricsMode mode, const Size& size );
	void setSize( int width, int height );
	void initialise();
	
	HWND GetHWnd() const //»ñµÃ´°¿Ú¾ä±ú
	{
		return m_hWnd;
	}
public:
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget

private:
	static	LRESULT	 CALLBACK SysWndProc(
		HWND hWnd,      // handle to window
		UINT message,      // message identifier
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);

private:
	HWND	m_hWnd; // window handle
	static bool m_windowClassRegistered;
};

/*!
\brief
	Factory class for producing TLListbox objects
*/
class TAHAREZLOOK_API TLSystemWindowFactory : public WindowFactory
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	TLSystemWindowFactory(void) : WindowFactory(TLSystemWindow::WidgetTypeName) { }
	~TLSystemWindowFactory(void){}


	/*!
	\brief
		Create a new Window object of whatever type this WindowFactory produces.

	\param name
		A unique name that is to be assigned to the newly created Window object

	\return
		Pointer to the new Window object.
	*/
	Window*	createWindow(const String& name);


	/*!
	\brief
		Destroys the given Window object.

	\param window
		Pointer to the Window object to be destroyed.

	\return
		Nothing.
	*/
	virtual void	destroyWindow(Window* window)	 { if (window->getType() == d_type) delete window; }
};

} // End of  CEGUI namespace section




#endif // !defined(AFX_UISYSWINDOW_H__0FC864C4_49EF_4673_B9B9_7AC1AABDAB48__INCLUDED_)
