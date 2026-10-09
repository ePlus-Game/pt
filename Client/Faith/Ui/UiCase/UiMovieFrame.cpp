

#include "UiMovieFrame.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"
#include "CoreUseNameDef.h"


KUiMovieFrame::KUiMovieFrame()
{
	load();
}

KUiMovieFrame::~KUiMovieFrame()
{

}

void KUiMovieFrame::load()
{
	_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_MOVIE_FRAME_WINDOW_PATH);
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT_2)->addChildWindow(_thisWindow);
	_thisWindow->setRenderMode(true);
	_thisWindow->setFadeInTime(1.0f);
	_thisWindow->setFadeOutTime(1.0f);
	_thisWindow->setAlpha(0.0f);

	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiMovieFrame::onHide, this));
	_thisWindow->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMovieFrame::onClick, this));
	_thisWindow->subscribeEvent(Window::EventFadingInEnd, Event::Subscriber(&KUiMovieFrame::onFadingInEnd, this));

	_topFrame = (TLStaticImage*)_thisWindow->getChild("TaharezLook/MovieFrame/Top");
	_bottomFrame = (TLStaticImage*)_thisWindow->getChild("TaharezLook/MovieFrame/Bottom");

	_headImg = (TLStaticImage*)_thisWindow->getChild("TaharezLook/MovieFrame/NpcHeadImg");

	_msgText = (TLStaticText*)_thisWindow->getChild("TaharezLook/MovieFrame/NpcTalkMsg");
	_msgText->useLayout();

	_questPanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/MovieFrame/QuestPanel");
	for(int i = 0; i < UI_MOVIE_QUEST_BOTTOM_COUNT; ++i)
	{
		_questTitleImage[i] = (TLStaticImage*)_questPanel->getChild(String("TaharezLook/MovieFrame/QuestPanel/Title") + PropertyHelper::intToString(i + 1));
		_questBtn[i] = (TLButton*)_questPanel->getChild(String("TaharezLook/MovieFrame/QuestPanel/QuestBtn") + PropertyHelper::intToString(i + 1));
	}
}

void KUiMovieFrame::beginScene()
{
	hideCtrls();
	
	_topFrame->show();
	_bottomFrame->show();

	_thisWindow->OpenBox();
	_thisWindow->beginUpdate();

	KUiSheetRefCounter::getSinglton().addRef(UI_DEFAULT_GUISHEET_ROOT_2);
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT_2);
}

void KUiMovieFrame::endScene()
{	
	hideCtrls();
	
	_topFrame->show();
	_bottomFrame->show();
	_thisWindow->CloseBox();
}

void KUiMovieFrame::hideCtrls()
{
	_topFrame->hide();
	_bottomFrame->hide();
	_msgText->hide();
	_headImg->hide();
	_questPanel->hide();
	_headImg->hide();
}

void KUiMovieFrame::npcWantTalk(const KUiMovieScene* npcTalkMessage)
{
	beginScene();

	if(NULL == npcTalkMessage)
		return;
	
	//显示NPC要说的话
	sprintf(_msg, 
		"<Layout width=%d>"
			"<Seg float=wrap>"
				"<Obj type=text vertical-align=bottom color=%s font-family=%s>%s</Obj>"
			"</Seg>"
		"</Layout>", 
		(int)_msgText->getWidth(Absolute), 
		KUiCfgLoader::getSingleton().getMovieSceneCfg().textColor,
		KUiCfgLoader::getSingleton().getMovieSceneCfg().textFont,
		npcTalkMessage->mainText.c_str());
	
	_msgText->getLayout()->SetText(_msg);

 	for(int i = 0; i < UI_MOVIE_QUEST_BOTTOM_COUNT; ++i)
 	{
 		if(i < npcTalkMessage->selectionText.size())
 		{
 			_questBtn[i]->setText(AnsiToUtf8(npcTalkMessage->selectionText[i].c_str()));
 			_questBtn[i]->show();
 		}
 		else
 		{
 			_questBtn[i]->hide();
 		}
 	}

	//显示图片
	const map<int, string>& commImageCfg = KUiCfgLoader::getSingleton().getCommImage();
	map<int, string>::const_iterator curIt = commImageCfg.find(npcTalkMessage->imageId);
	if(curIt != commImageCfg.end())
	{
		const Image* image = getImage(curIt->second.c_str());
		_headImg->setImage(image);
		//_headImg->show();
	}
	else
	{
		_headImg->hide();
	}
}

bool KUiMovieFrame::onFadingInEnd(const EventArgs& arg)
{
	_headImg->show();
	_questPanel->show();
	_msgText->show();
	return true;
}

bool KUiMovieFrame::onHide(const EventArgs& arg)
{
	_thisWindow->setAlpha(0.0f);
	_thisWindow->stopUpdate();	
	if(KUiSheetRefCounter::getSinglton().removeRef(UI_DEFAULT_GUISHEET_ROOT_2))
	{
		KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);
	}
	return true;
}

bool KUiMovieFrame::onClick(const EventArgs& arg)
{
	endScene();
	return true;
}