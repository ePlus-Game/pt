#include "UiPetFrame.h"
#include "UiUnitFrame.h"
#include "UiRoleFace.h"
#include "../UiConfigManager.h"
#include "Coreshell.h"

extern iCoreShell*		g_pCoreShell;


KUiSelfPetFrame::KUiSelfPetFrame()
{
	_npcIndex = UI_PET_FRAME_INVALID_ID;
	_frameIndex = KUiUnitFrameMgr::getSingleton().createAUnit();
	connectToSelfFrame();
	loadStyle();
		
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

	petFrame->getWindow()->subscribeEvent(Window::EventMouseClick, 
			Event::Subscriber(&KUiSelfPetFrame::clickFrame, this));
}

KUiSelfPetFrame::~KUiSelfPetFrame()
{
	
}
	
KUiSelfPetFrame& KUiSelfPetFrame::getSingleton()
{
	static KUiSelfPetFrame singleton;
	return singleton;
}

void KUiSelfPetFrame::connectToSelfFrame()
{
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

// 	const Rect selfFrameArea = KUiRoleFace::GetSingleton().getArea();
// 	Point pos(selfFrameArea.d_left + selfFrameArea.getWidth() / 2, selfFrameArea.d_bottom);

	Point pos;
	pos.d_x = KUiCfgLoader::getSingleton().getPetCfg().selfPetPos.first;
	pos.d_y = KUiCfgLoader::getSingleton().getPetCfg().selfPetPos.second;
	petFrame->setPos(pos);

	Size bloodSize;
	bloodSize.d_width = KUiCfgLoader::getSingleton().getPetCfg().selfPetBloodSize.first;
	bloodSize.d_height = KUiCfgLoader::getSingleton().getPetCfg().selfPetBloodSize.second;
	petFrame->setSize(UI_UNIT_FRAME_BLOOD_FLAG, bloodSize);
}

void KUiSelfPetFrame::loadStyle()
{
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

	const Image* image = getImage(KUiCfgLoader::getSingleton().getPetCfg().selfPetHeadImage);
	if(image)
	{
		petFrame->setHeadImg(image);
	}

	int style = UI_UNIT_FRAME_HEAD_FLAG	
		| UI_UNIT_FRAME_BLOOD_FLAG
		| UI_UNIT_FRAME_NAME_FLAG;
	petFrame->setStyle(style);
	petFrame->hide();
}

void KUiSelfPetFrame::show()
{
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

	petFrame->show();
}

void KUiSelfPetFrame::show(char* name, int petNpcIndex)
{
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

	petFrame->show();
	if(name)
	{
		petFrame->setName(KUiCfgLoader::getSingleton().getPetCfg().selfPetExtendName);
	}

	_npcIndex = petNpcIndex;
}

void KUiSelfPetFrame::hide()
{
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

	petFrame->hide();
}

void KUiSelfPetFrame::update(int curBlood, int maxBlood)
{
	KUiUnitFrame* petFrame = KUiUnitFrameMgr::getSingleton().find(_frameIndex);
	if(!petFrame)
	{
		return;
	}

	if(!petFrame->isVisible())
	{
		return;
	}

	petFrame->setBlood(curBlood, maxBlood);
}

int	KUiSelfPetFrame::getNpcIndex()
{
	return _npcIndex;
}

bool KUiSelfPetFrame::clickFrame(const EventArgs& args)
{
	g_pCoreShell->SelectNPC(_npcIndex);
	return true;
}