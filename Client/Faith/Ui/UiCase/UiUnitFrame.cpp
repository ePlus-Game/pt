
#include "UiUnitFrame.h"
#include "../UiSheetMgr.h"


void resizeCtrlAsImage(TLStaticImage* image)
{
	if(image->getImage() == NULL)
	{
		image->setWidth(Absolute, 0);
		image->setHeight(Absolute, 0);
	}
	else
	{
		image->setWidth(Absolute, image->getImage()->getWidth());
		image->setHeight(Absolute, image->getImage()->getHeight());
	}
}

KUiUnitFrame::KUiUnitFrame()
{
	_style = UI_UNIT_FRAME_LEVEL_FLAG 
		| UI_UNIT_FRAME_PROFESSION_FLAG
		| UI_UNIT_FRAME_HEAD_FLAG	
		| UI_UNIT_FRAME_NAME_FLAG	
		| UI_UNIT_FRAME_BLOOD_FLAG	
		| UI_UNIT_FRAME_MANA_FLAG	
		| UI_UNIT_FRAME_EXP_FLAG;
}

KUiUnitFrame::~KUiUnitFrame()
{

}

void KUiUnitFrame::create(int id)
{
#ifndef _DEBUG
	try
	{
#endif
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_UNIT_FRAME_WINDOW_PATH, CEGUI::PropertyHelper::intToString(id));
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif
	
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);

	String parentWindowName = _thisWindow->getName();
	_levelText			= (TLStaticText*)_thisWindow->getChild(parentWindowName		+ "/Level");
	_professionImg		= (TLStaticImage*)_thisWindow->getChild(parentWindowName	+ "/Profession");
	_headImg			= (TLStaticImage*)_thisWindow->getChild(parentWindowName	+ "/Head");
	_nameText			= (TLStaticText*)_thisWindow->getChild(parentWindowName		+ "/Name");
	_bloodImg			= (TLProgressBar*)_thisWindow->getChild(parentWindowName	+ "/Blood");
	_bloodPercentText	= (TLStaticText*)_thisWindow->getChild(parentWindowName		+ "/BloodPercent");
	_manaImg			= (TLProgressBar*)_thisWindow->getChild(parentWindowName	+ "/Mana");
	_manaPercentText	= (TLStaticText*)_thisWindow->getChild(parentWindowName		+ "/ManaPercent");
	_expImage			= (TLProgressBar*)_thisWindow->getChild(parentWindowName	+ "/Exp");

	_headImg->disable();
	_thisWindow->setZLevel(Window::SuperBottom);
	
	setStyle(_style);
}

void KUiUnitFrame::setStyle(int style)
{
	if(!_thisWindow)
	{
		return;
	}

	_levelText->hide();
	_professionImg->hide();

	_headImg->hide();

	_nameText->hide();

	_bloodImg->hide();
	_bloodPercentText->hide();
	
	_manaImg->hide();
	_manaPercentText->hide();

	_expImage->hide();

	_style = style;

	if((style & UI_UNIT_FRAME_LEVEL_FLAG) == UI_UNIT_FRAME_LEVEL_FLAG)
	{
		_levelText->show();
	}

	if((style & UI_UNIT_FRAME_PROFESSION_FLAG) == UI_UNIT_FRAME_PROFESSION_FLAG)
	{
		_professionImg->show();
		resizeCtrlAsImage(_professionImg);
	}

	if((style & UI_UNIT_FRAME_HEAD_FLAG) == UI_UNIT_FRAME_HEAD_FLAG)
	{
		_headImg->show();
		resizeCtrlAsImage(_headImg);
	}

	if((style & UI_UNIT_FRAME_NAME_FLAG) == UI_UNIT_FRAME_NAME_FLAG)
	{
		_nameText->show();
	}

	if((style & UI_UNIT_FRAME_BLOOD_FLAG) == UI_UNIT_FRAME_BLOOD_FLAG)
	{
		_bloodImg->show();
//		_bloodPercentText->show();
	}

	if((style & UI_UNIT_FRAME_MANA_FLAG) == UI_UNIT_FRAME_MANA_FLAG)
	{
		_manaImg->show();
		_manaPercentText->show();
	}

	if((style & UI_UNIT_FRAME_EXP_FLAG) == UI_UNIT_FRAME_EXP_FLAG)
	{
		_expImage->show();
	}

	//x×ø±ê
	_levelText->setXPosition(Absolute, 0);
	_professionImg->setXPosition(Absolute, 0);

	int width = 0;
	if(_levelText->isVisible())
	{
		width = _levelText->getWidth(Absolute);
	}

	if(_professionImg->isVisible() && width < _professionImg->getWidth(Absolute))
	{
		width = _professionImg->getWidth(Absolute);
	}

	_headImg->setXPosition(Absolute, width);
	if(_headImg->isVisible())
	{
		width += _headImg->getWidth(Absolute);
	}

	_nameText->setXPosition(Absolute, width);
	_bloodImg->setXPosition(Absolute, width);
	_manaImg->setXPosition(Absolute, width);

	if(_bloodImg->isVisible())
	{
		_bloodPercentText->setXPosition(Absolute, _bloodImg->getWidth(Absolute) + _bloodImg->getXPosition(Absolute));
		width += _bloodImg->getWidth(Absolute) + _bloodPercentText->getWidth(Absolute);
	}
	if(_manaImg->isVisible())
	{
		_manaPercentText->setXPosition(Absolute, _manaImg->getWidth(Absolute) + _manaImg->getXPosition(Absolute));
		if(width < _manaPercentText->getXPosition(Absolute) + _manaPercentText->getWidth(Absolute))
		{
			width = _manaPercentText->getXPosition(Absolute) + _manaPercentText->getWidth(Absolute);
		}
	}

	//y×ø±ê
	int height = 0;

	if(_nameText->isVisible())
	{
		_nameText->setYPosition(Absolute, 0);
		height += _nameText->getHeight(Absolute);
	}

	if(_bloodImg->isVisible())
	{
		_bloodImg->setYPosition(Absolute, height);
		_bloodPercentText->setYPosition(Absolute, height);
		height += _bloodImg->getHeight(Absolute);
	}

	if(_manaImg->isVisible())
	{
		_manaImg->setYPosition(Absolute, height);
		_manaPercentText->setYPosition(Absolute, height);
		height += _manaImg->getHeight(Absolute);
	}

	if(_headImg->isVisible() && _headImg->getHeight(Absolute) > height)
	{
		height = _headImg->getHeight(Absolute);
	}

	if(_professionImg->isVisible())
	{
		_professionImg->setYPosition(Absolute, height - _professionImg->getHeight(Absolute));
	}

	if(_expImage->isVisible())
	{
		_expImage->setYPosition(Absolute, height);
		height += _expImage->getHeight(Absolute);
	}

	bool needResizePanel = false;
	if(width > _thisWindow->getWidth(Absolute))
	{
		needResizePanel = true;
	}
	if(height > _thisWindow->getHeight(Absolute))
	{
		needResizePanel = true;
	}
	_thisWindow->setWidth(Absolute, width);
	_thisWindow->setHeight(Absolute, height);

	if(needResizePanel)
	{
//		_thisWindow->setRenderMode(true);
		_thisWindow->setRenderMode(false);
	}
}

int	KUiUnitFrame::getStyle()
{
	return _style;
}

void KUiUnitFrame::setHeadImg(const Image* image)
{
	if(!_thisWindow)
	{
		return;
	}

	_headImg->setImage(image);
}

void KUiUnitFrame::setLevel(int level)
{
	if(!_thisWindow)
	{
		return;
	}

	_levelText->setText(PropertyHelper::intToString(level));
}

void KUiUnitFrame::setProfessionImage(const Image* image)
{
	if(!_thisWindow)
	{
		return;
	}

	_professionImg->setImage(image);
}

void KUiUnitFrame::setName(const char* name)
{
	if(!_thisWindow)
	{
		return;
	}

	_nameText->setText(AnsiToUtf8(name));
}

void KUiUnitFrame::setBlood(int cur, int max)
{
	if(!_thisWindow)
	{
		return;
	}

	_bloodImg->setProgress(((float)cur) / max);
	int percent = cur * 100 / max;
	_bloodPercentText->setText(PropertyHelper::intToString(percent));
}

void KUiUnitFrame::setMana(int cur, int max)
{
	if(!_thisWindow)
	{
		return;
	}

	_manaImg->setProgress(((float)cur) / max);
	int percent = cur * 100 / max;
	_manaPercentText->setText(PropertyHelper::intToString(percent));
}

void KUiUnitFrame::setExp(int cur, int max)
{
	if(!_thisWindow)
	{
		return;
	}

	_expImage->setProgress(((float)cur) / max);
}

void KUiUnitFrame::setPos(Point pos)
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->setPosition(Absolute, pos);
}

void KUiUnitFrame::setSize(int flag, Size size)
{
	if(!_thisWindow)
	{
		return;
	}

	switch(flag)
	{
	case UI_UNIT_FRAME_LEVEL_FLAG:
		{
			_levelText->setSize(Absolute, size);
		}
		break;
	case UI_UNIT_FRAME_NAME_FLAG:
		{
			_nameText->setSize(Absolute, size);
		}
		break;
	case UI_UNIT_FRAME_BLOOD_FLAG:
		{
			_bloodImg->setSize(Absolute, size);
			_bloodPercentText->setHeight(Absolute, size.d_height);
		}
		break;
	case UI_UNIT_FRAME_MANA_FLAG:
		{
			_manaImg->setSize(Absolute, size);
			_manaPercentText->setHeight(Absolute, size.d_height);
		}
		break;
	case UI_UNIT_FRAME_EXP_FLAG:
		{
			_expImage->setSize(Absolute, size);
		}
		break;
	}
}

void KUiUnitFrame::show()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();
}

void KUiUnitFrame::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

bool KUiUnitFrame::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}

KUiUnitFrameMgr::KUiUnitFrameMgr()
{
	_curIndex = 0;
}

KUiUnitFrameMgr::~KUiUnitFrameMgr()
{
	
	
}

KUiUnitFrameMgr& KUiUnitFrameMgr::getSingleton()
{
	static KUiUnitFrameMgr singleton;
	return singleton;
}

int KUiUnitFrameMgr::createAUnit()
{
	if(_curIndex >= UI_UF_MGR_MAX_UNIT_COUNT)
	{
		return UI_UF_MGR_INVALID_ID;
	}

	_frames[_curIndex].create(_curIndex);

	return _curIndex++;
}

KUiUnitFrame* KUiUnitFrameMgr::find(int index)
{
	if(index >= _curIndex || index < 0)
	{
		return NULL;
	}
	return &_frames[index];
}