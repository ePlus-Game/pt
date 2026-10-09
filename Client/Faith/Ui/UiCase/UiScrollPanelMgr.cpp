
#include "UiScrollPanelMgr.h"


KUiScrollPanelMgr::KUiScrollPanelMgr(TLStaticImage* clipper, TLStaticImage* panel, TLVertScrollbar* scrollbar)
{
	_clipper	= clipper;
	_panel		= panel;
	_scrollbar	= scrollbar;
	
	if(!checkValid())
	{
		return;
	}

	_scrollbar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiScrollPanelMgr::onScroll, this));	
	_panel->subscribeEvent(TLStaticImage::EventMouseWheel, Event::Subscriber(&KUiScrollPanelMgr::onWheelChanged, this));
	_panel->subscribeEvent(Window::EventSized, Event::Subscriber(&KUiScrollPanelMgr::onSized, this));
	_clipper->subscribeEvent(Window::EventSized, Event::Subscriber(&KUiScrollPanelMgr::onSized, this));
}

KUiScrollPanelMgr::~KUiScrollPanelMgr()
{
	
}

bool KUiScrollPanelMgr::checkValid()
{
	if(!_clipper)
	{
		return false;
	}
	
	if(!_panel)
	{
		return false;
	}
	
	if(!_scrollbar)
	{
		return false;
	}

	return true;
}

void KUiScrollPanelMgr::resized()
{
	if(!checkValid())
	{
		return;
	}

	int clipperHeight = _clipper->getAbsoluteHeight();
	int panelHeight = _panel->getAbsoluteHeight();
	
	if(panelHeight <= clipperHeight)
	{
		_scrollbar->hide();
		_panel->setPosition(Absolute, Point(0, 0));
	}
	else
	{
		_scrollbar->show();
	
		float off = -_panel->getYPosition(Absolute);
		if(off <= 0)
		{
			_panel->setYPosition(Absolute, 0);
			_scrollbar->setScrollPosition(0.0f);
		}
		else if(off < panelHeight - clipperHeight)
		{
			float scrollPos = off / (panelHeight - clipperHeight);
			_scrollbar->setScrollPosition(scrollPos);
		}
		else
		{
			_panel->setYPosition(Absolute, clipperHeight - panelHeight);
			_scrollbar->setScrollPosition(1.0f);
		}
	}
}

bool KUiScrollPanelMgr::onSized(const EventArgs& args)
{
	resized();
	return true;
}

bool KUiScrollPanelMgr::onScroll(const EventArgs& args)
{
	float scrollPos = _scrollbar->getScrollPosition();

	int clipperHeight = _clipper->getAbsoluteHeight();
	int panelHeight = _panel->getAbsoluteHeight();

	int exceedSize = (panelHeight - clipperHeight) * scrollPos;
	_panel->setYPosition(Absolute, -exceedSize);
	return true;
}

bool KUiScrollPanelMgr::onWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(_scrollbar->isVisible())
	{
		_scrollbar->setScrollPosition(_scrollbar->getScrollPosition()
			- _scrollbar->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}