
#include "UiSwichScene.h"
#include "../UiSheetMgr.h"

KUiSwitchSceneAnimation::KUiSwitchSceneAnimation()
{
	loadUi();
}

KUiSwitchSceneAnimation::~KUiSwitchSceneAnimation()
{

}

KUiSwitchSceneAnimation& KUiSwitchSceneAnimation::getSingleton()
{
	static KUiSwitchSceneAnimation singleton;
	return singleton;
}

void KUiSwitchSceneAnimation::loadUi()
{
	_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SWITCH_SCENE_WINDOW_PATH);
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT_2)->addChildWindow(_thisWindow);

	_animation = (TLStaticImage*)_thisWindow->getChild("TaharezLook/SwitchScene/Animation");
	_animation->disable();
	_thisWindow->setRenderMode(true);
	_thisWindow->setFadeInTime(1.0f);
	_thisWindow->setFadeOutTime(1.0f);
	_thisWindow->setAlpha(0.0f);

	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiSwitchSceneAnimation::onHide, this));
	_thisWindow->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSwitchSceneAnimation::onClick, this));
	_thisWindow->subscribeEvent(Window::EventFadingInEnd, Event::Subscriber(&KUiSwitchSceneAnimation::onFadingInEnd, this));
}

bool KUiSwitchSceneAnimation::isVisible()
{
	return _thisWindow->isVisible();
}

void KUiSwitchSceneAnimation::hide()
{
	_thisWindow->hide();
}

void KUiSwitchSceneAnimation::show()
{
	_thisWindow->show();
}

void KUiSwitchSceneAnimation::playAnimation()
{
	_animation->setCyc(true);
	_animation->play();
}

void KUiSwitchSceneAnimation::stopAnimation()
{
	_animation->stop();
}

void KUiSwitchSceneAnimation::preAnimation()
{
	_thisWindow->OpenBox();
	_thisWindow->beginUpdate();
	
	KUiSheetRefCounter::getSinglton().addRef(UI_DEFAULT_GUISHEET_ROOT_2);
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT_2);
}

void KUiSwitchSceneAnimation::postAnimation()
{
	stopAnimation();
	_thisWindow->CloseBox();
}

bool KUiSwitchSceneAnimation::onFadingInEnd(const EventArgs& arg)
{
	playAnimation();
	return true;
}

bool KUiSwitchSceneAnimation::onHide(const EventArgs& arg)
{
	_thisWindow->setAlpha(0.0f);
	_thisWindow->stopUpdate();	
	if(KUiSheetRefCounter::getSinglton().removeRef(UI_DEFAULT_GUISHEET_ROOT_2))
	{
		KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);
	}
	return true;
}

bool KUiSwitchSceneAnimation::onClick(const EventArgs& arg)
{
	postAnimation();
	return true;
}