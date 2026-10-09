#include "../UiSheetMgr.h"
#include "UiSplitItemBox.h"
#include "UiDragItem.h"

using namespace CEGUI;

KUiSplitItemBox::KUiSplitItemBox()
{
	load();
}

KUiSplitItemBox::~KUiSplitItemBox()
{
	
}

void KUiSplitItemBox::load()
{
#ifndef _DEBUG
	try
	{
#endif
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SPLIT_ITEM_BOX_PATH);
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);

	_thisWindow->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiSplitItemBox::onWindowOpen, this));
	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiSplitItemBox::onWindowClose, this));

	_slider		= (TLMiniHorzScrollbar*)_thisWindow->getChild("TaharezLook/SplitItemBox/Slider");
	_slider->subscribeEvent(Window::EventScrollPositionChanged, Event::Subscriber(&KUiSplitItemBox::onScroll, this));

	_count		= (TLEditbox*)_thisWindow->getChild("TaharezLook/SplitItemBox/Count");
	_count->disable();

	_sour		= (TLGameObject*)_thisWindow->getChild("TaharezLook/SplitItemBox/Sour");
	_dest		= (TLGameObject*)_thisWindow->getChild("TaharezLook/SplitItemBox/Dest");
	_sourGrid.setCtrl(_sour);
	_sourGrid.addTip();
	_destGrid.setCtrl(_dest);
	_destGrid.addTip();

	_sour->setUserData(&_sourRegion);
	_dest->setUserData(&_sourRegion);
	
	//确定取消按钮
	_ok		= (TLButton*)_thisWindow->getChild("TaharezLook/SplitItemBox/Ok");
	_cancel = (TLButton*)_thisWindow->getChild("TaharezLook/SplitItemBox/Cancel");
	_ok->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSplitItemBox::onOk, this));
	_cancel->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiSplitItemBox::onCancel, this));

	_thisWindow->hide();
}

void KUiSplitItemBox::open(KObjAtContRegion& itemRegion, TLGameObject::GameObject& objInfo)
{
	if(!_thisWindow)
	{
		return;
	}

	if(objInfo.d_type == TLGameObject::idle || itemRegion.Region.Height <= 1)
		return;
	
	//记录总数
	_total = objInfo.d_count;
	_slider->setStepSize(1.0f / (_total - 2));
	_slider->setScrollPosition(2.98e-008f);
	_slider->setScrollPosition(2.98e-008f);
	
	//初始默认为移动一个
	objInfo.d_count--;
	_sour->setObject(objInfo);
	objInfo.d_count = 1;
	_dest->setObject(objInfo);

	_count->setText(PropertyHelper::intToString(objInfo.d_count));

	_sourRegion = itemRegion;
	_sourRegion.Region.Height = 1;

	_thisWindow->show();
}

bool KUiSplitItemBox::onWindowOpen(const EventArgs& args)
{
	_thisWindow->setModalState(true);
	return true;
}

bool KUiSplitItemBox::onWindowClose(const EventArgs& args)
{
	_thisWindow->setModalState(false);
	return true;
}

bool KUiSplitItemBox::onScroll(const EventArgs& args)
{
	float scrollPos = _slider->getScrollPosition();
	int splitCount = (_total - 2) * scrollPos + 1;

	_count->setText(PropertyHelper::intToString(splitCount));

	_sour->setCount(_total - splitCount);
	_dest->setCount(splitCount);
	_sourRegion.Region.Height = splitCount;
	return true;
}

bool KUiSplitItemBox::onOk(const EventArgs& arg)
{
	_thisWindow->hide();

	TLGameObject* dragItem = KUiDragItem::GetSingleton().getObj();
	*(KObjAtContRegion*)dragItem->getUserData() = _sourRegion;

	TLGameObject::GameObject objInfo;
	_dest->getObject(objInfo);
	objInfo.d_count = atoi(_count->getText().c_str());

	dragItem->setObject(objInfo);
	dragItem->setCanDrag(true);

	return true;
}

bool KUiSplitItemBox::onCancel(const EventArgs& arg)
{
	_thisWindow->hide();
	return true;
}

KUiSplitItemBox& KUiSplitItemBox::getSingleton()
{
	static KUiSplitItemBox singleton;
	return singleton;
}

