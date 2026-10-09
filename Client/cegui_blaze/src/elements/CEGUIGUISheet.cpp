/************************************************************************
	filename: 	CEGUIGUISheet.cpp
	created:	28/8/2004
	author:		Paul D Turner
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "elements/CEGUIGUISheet.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const String GUISheet::WidgetTypeName( (utf8*)"DefaultWindow" );

/*************************************************************************
    Let this Window beathe.
*************************************************************************/
void GUISheet::breathe(void)
{
	// render any child windows
	if ( d_itBreatheList != d_breatheList.end() )
	{
		if ( (*d_itBreatheList).first && (*d_itBreatheList).first->isVisible() && (*d_itBreatheList).second )
		{
			(*d_itBreatheList).second();
		}
		
		d_itBreatheList++;
	}
	else
	{
		d_itBreatheList = d_breatheList.begin();
	}
}

/*************************************************************************
	Cause window to update itself and any attached children
*************************************************************************/
void GUISheet::update(DWORD curTimeCount)
{
	if ( d_updateList.empty() )
		return;

	for(int i = 0; i < d_updateList.size(); ++i)
	{
		Window* curWnd = d_updateList[i];
		//String name = curWnd->getName();
		if(curWnd->isNeddUpdate())
		{
			curWnd->injectTimePulse(curTimeCount);
		}
	}
}

void GUISheet::addUpdateWindow(Window* wnd)
{
	for(UpdateList::iterator it = d_updateList.begin(); it != d_updateList.end(); ++it)
	{
		if(*it == wnd)
			return;
	}
	d_updateList.push_back(wnd);
}

void GUISheet::removeUpdateWindow(Window* wnd)
{
	for(UpdateList::iterator it = d_updateList.begin(); it != d_updateList.end(); ++it)
	{
		if(*it == wnd)
		{
			d_updateList.erase(it);
			return;
		}
	}
}

void GUISheet::onCharacter(KeyEventArgs& e)
{
	Window::onCharacter( e );
	e.handled = true;
}


} // End of  CEGUI namespace section
