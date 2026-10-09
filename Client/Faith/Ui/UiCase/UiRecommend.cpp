
#include "UiRecommend.h"
#include "../UiSheetMgr.h"
#include "UiScrollPanelMgr.h"
#include "Coreshell.h"
#include "../UiConfigManager.h"
#include "UiComMsgBox.h"

extern iCoreShell*		g_pCoreShell;

KUiRecommend::KUiRecommend()
{
	loadUi();
}

KUiRecommend::~KUiRecommend()
{

}
	
KUiRecommend& KUiRecommend::getSingleton()
{
	static KUiRecommend singleton;
	return singleton;
}

void KUiRecommend::hideAllItems()
{
	for(int i = 0; i < MAX_STUDENT_COUNT; ++i)
	{
		if(!_items[i])
		{
			continue;
		}
		_items[i]->hide();
	}
}

char* KUiRecommend::getMoneyLayout(int money, int color)
{
	int j = money / 10000;
	int y = (money % 10000) / 100;
	int t = money % 100;

	static char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	
	strcpy(layoutText, "<Layout><Seg text-align=left>");
	strcat(layoutText, getMoneyLayoutObj(money, color));
	strcat(layoutText, "</Seg></Layout>");

	return layoutText;
}

char* KUiRecommend::getMoneyLayoutObj(int money, int color)
{
	int j = money / 10000;
	int y = (money % 10000) / 100;
	int t = money % 100;

	static char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	char tempText[COMMON_CLIENT_MSG_LEN_256];
	
	strcpy(layoutText, "");
	if(j > 0)
	{
		sprintf(tempText, "<Obj c=%x>%d </Obj>", color, j);
		strcat(layoutText, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getJinImagePath());
		strcat(layoutText, tempText);
	}
	
	if(y > 0)
	{
		sprintf(tempText, "<Obj c=%x>%d </Obj>", color, y);
		strcat(layoutText, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getYinImagePath());
		strcat(layoutText, tempText);
	}
	
	if(t > 0)
	{
		sprintf(tempText, "<Obj c=%x>%d </Obj>", color, t);
		strcat(layoutText, tempText);
		sprintf(tempText, "<Obj type=pic>%s</Obj>", KUiCfgLoader::getSingleton().getTongImagePath());
		strcat(layoutText, tempText);
	}

	return layoutText;
}

void KUiRecommend::show(RecommedList& studentList, int rewardToAdd, int totalRewardTicketAdded)
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->show();

	hideAllItems();

	int itemIndex = 0;

	int height = 0;
	for(int i = 0; i < MAX_STUDENT_COUNT; ++i)
	{
		if(!_items[i])
		{
			continue;
		}

		if(itemIndex >= studentList.size())
		{
			break;
		}

		_items[itemIndex]->show();

		Window* nameTxt = _items[itemIndex]->getChild(PropertyHelper::intToString(itemIndex + 1) + String("TaharezLook/RecommendItem/Name"));
		Window* levelTxt = _items[itemIndex]->getChild(PropertyHelper::intToString(itemIndex + 1) + String("TaharezLook/RecommendItem/Level"));
		TLStaticText* moneyTxt = (TLStaticText*)_items[itemIndex]->getChild(PropertyHelper::intToString(itemIndex + 1) + String("TaharezLook/RecommendItem/Money"));

		RecommendItem& student = studentList[itemIndex];
		nameTxt->setText(AnsiToUtf8(student.Name));
		
		levelTxt->setText(PropertyHelper::intToString(student.Level));
		
		colour textColor = moneyTxt->getTextColours();
		int color = textColor.getARGB();
		moneyTxt->useLayout();
		//moneyTxt->getLayout()->SetText(getMoneyLayout(student.RewardMoeny, color));
		moneyTxt->setText(PropertyHelper::intToString(student.TotalRewardMoney));

		height += _items[itemIndex]->getAbsoluteHeight();

		++itemIndex;
	}

	_panel->setHeight(Absolute, height);

	colour textColor = _totalMoney->getTextColours();
	int color = textColor.getARGB();
	//_totalMoney->getLayout()->SetText(getMoneyLayout(totalMoeny, color));
	_totalMoney->setText(PropertyHelper::intToString(totalRewardTicketAdded));

	textColor = _money->getTextColours();
	color = textColor.getARGB();
	//_money->getLayout()->SetText(getMoneyLayout(money, color));
	_money->setText(PropertyHelper::intToString(rewardToAdd));

	_studentCount->setText(PropertyHelper::intToString(studentList.size()));

	_scrollbar->setScrollPosition(0.0f);
}
	
void KUiRecommend::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

bool KUiRecommend::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}

	return _thisWindow->isVisible();
}

void KUiRecommend::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_RECOMMEND_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_RECOMMEND_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	
	_thisWindow->hide();
	
	_getBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Recommend/Get");
	_cancelBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Recommend/Cancel");
	_closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/Recommend/Close");

	TLStaticImage* clipper = (TLStaticImage*)_thisWindow->getChild("TaharezLook/Recommend/Clipper");
	_panel = (TLStaticImage*)clipper->getChild("TaharezLook/Recommend/Clipper/Panel");

	_scrollbar = (TLVertScrollbar*)_thisWindow->getChild("TaharezLook/Recommend/Scrollbar");

	_getBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiRecommend::onGet, this));
	_cancelBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiRecommend::onClose, this));
	_closeBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiRecommend::onClose, this));

	static KUiScrollPanelMgr autoScroll(clipper, _panel, _scrollbar);

	_money = (TLStaticText*)_thisWindow->getChild("TaharezLook/Recommend/ThisTimeMoney");
	_totalMoney = (TLStaticText*)_thisWindow->getChild("TaharezLook/Recommend/TotalMoney");
	_studentCount = (TLStaticText*)_thisWindow->getChild("TaharezLook/Recommend/Count");
	_money->useLayout();
	_totalMoney->useLayout();

	int yPos = 0;
	for(int i = 0; i < MAX_STUDENT_COUNT; ++i)
	{
#ifndef _DEBUG
		try
		{
#endif
			_items[i] = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_RECOMMEND_ITEM_WINDOW_PATH, 
				PropertyHelper::intToString(i + 1));
			_items[i]->setYPosition(Absolute, yPos);
			_items[i]->setXPosition(Absolute, 0);
			yPos += _items[i]->getHeight(Absolute);
			
			_panel->addChildWindow(_items[i]);
#ifndef _DEBUG
		}
		catch (...)
		{
			_items[i] = NULL;
		}
#endif
	}
}

bool KUiRecommend::onClose(const EventArgs& e)
{	
	hide();
	return true;
}

bool KUiRecommend::onGet(const EventArgs& e)
{
    g_pCoreShell->OperationRequest(GOI_GET_REWARD, NULL, NULL);
	hide();
	return true;
}

void processReport()
{
    g_pCoreShell->OperationRequest(GOI_STUDENT_REPORT, NULL, NULL);
}

void KUiRecommend::showReport(const char* masterName, int lastLevel, int curLevel)
{
	const char* formatText = KUiCfgLoader::getSingleton().getRecommendCfg().studentReportText;

	char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	sprintf(layoutText, formatText, masterName, lastLevel, curLevel);
	
	KUiComMsgBox::GetSingleton().setComMsgPosition();
	KUiComMsgBox::Show();
	char yesString[COMMON_CLIENT_MSG_LEN_8];
	char noString[COMMON_CLIENT_MSG_LEN_8];
	strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
	strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
	KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);

	KUiComMsgBox::GetSingleton().setLayoutMsg(layoutText);
	
	KUiComMsgBox::GetSingleton().setFristBtnCallback(processReport);
}