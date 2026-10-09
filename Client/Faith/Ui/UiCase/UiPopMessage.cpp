#include "KWin32.h"
#include "UiPopMessage.h"
#include "ChatDataDef.h"


KUiPopMessage::KUiPopMessage(String wndType)
{
	StaticText* popWnd = NULL;
	try
	{
		popWnd = (StaticText*)WindowManager::getSingletonPtr()->loadWindowLayout(wndType);
	}
	catch (...)
	{
		popWnd = (StaticText*)WindowManager::getSingletonPtr()->getWindow("uisettings/layouts/PopMessage");
	}
	
	d_newWnd = (StaticText*)WindowManager::getSingleton().createWindow("TaharezLook/StaticText");
	
 	d_newWnd->setProperty("AbsolutePosition", popWnd->getProperty("AbsolutePosition"));
 	d_newWnd->setProperty("AbsoluteMaxSize", popWnd->getProperty("AbsoluteMaxSize"));
 	d_newWnd->setProperty("AbsoluteMinSize", popWnd->getProperty("AbsoluteMinSize"));
 	d_newWnd->setProperty("AbsoluteSize", popWnd->getProperty("AbsoluteSize"));
	
	d_newWnd->setProperty("FrameEnabled", popWnd->getProperty("FrameEnabled"));
	d_newWnd->setProperty("BackgroundEnabled", popWnd->getProperty("BackgroundEnabled"));
	d_newWnd->setProperty("FrameColours", popWnd->getProperty("FrameColours"));
	d_newWnd->setProperty("BackgroundColours", popWnd->getProperty("BackgroundColours"));
	d_newWnd->setProperty("BackgroundImage", popWnd->getProperty("BackgroundImage"));

	d_newWnd->setProperty("TopLeftFrameImage", popWnd->getProperty("TopLeftFrameImage"));
	d_newWnd->setProperty("TopRightFrameImage", popWnd->getProperty("TopRightFrameImage"));
	d_newWnd->setProperty("BottomLeftFrameImage", popWnd->getProperty("BottomLeftFrameImage"));
	d_newWnd->setProperty("BottomRightFrameImage", popWnd->getProperty("BottomRightFrameImage"));
	d_newWnd->setProperty("LeftFrameImage", popWnd->getProperty("LeftFrameImage"));
	d_newWnd->setProperty("RightFrameImage", popWnd->getProperty("RightFrameImage"));
	d_newWnd->setProperty("TopFrameImage", popWnd->getProperty("TopFrameImage"));
	d_newWnd->setProperty("BottomFrameImage", popWnd->getProperty("BottomFrameImage"));

	d_newWnd->setProperty("FadeInTime", popWnd->getProperty("FadeInTime"));
	d_newWnd->setProperty("FadeOutTime", popWnd->getProperty("FadeOutTime"));
	d_newWnd->setProperty("AutoCloseTime", popWnd->getProperty("AutoCloseTime"));

	d_newWnd->subscribeEvent(Static::EventHidden, Event::Subscriber(&KUiPopMessage::onClose, this));
}

KUiPopMessage::~KUiPopMessage()
{
	System::getSingleton().getGUISheet()->removeChildWindow(d_newWnd);
	delete d_newWnd;
	d_newWnd = NULL;
}

void KUiPopMessage::show(String message, Point pos)
{
	System::getSingleton().getGUISheet()->addChildWindow(d_newWnd);
	d_newWnd->setMetricsMode(Absolute);
	d_newWnd->setText(message);
	d_newWnd->setPosition(Absolute, pos);
	d_newWnd->OpenBox();
}

bool KUiPopMessage::onClose(const CEGUI::EventArgs& arg)
{
	delete this;
	return true;
}
