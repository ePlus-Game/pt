//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/27/2007 13:09
//      File_base        : UiBubble
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiBubble.h"
#include "coreshell.h"
#include "UiChatWindow.h"
#include "../UiConfigManager.h"
#include "KWin32Wnd.h"
#include "KWin32.h"
#include "UiMapCentre.h"
#include "../UiSheetMgr.h"

extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;

_Bubble::_Bubble()
{
	d_thisWnd			= NULL;
	m_CloseTime			= 3;
}

_Bubble::~_Bubble()
{

}

void _Bubble::create(int index)
{
	char prefix[COMMON_CLIENT_MSG_LEN_8];
	sprintf(prefix, "%d", index);
	d_thisWnd = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout( UI_BUBBLETIP, prefix );
	if ( d_thisWnd )
	{
		d_thisWnd->setZLevel(Window::Top);
		d_thisWnd->isNpcHeadInfo = true;
		
		_tail = (TLStaticImage*)d_thisWnd->getChild(d_thisWnd->getName() + "/Tail");
		_tipText = (TLStaticText*)d_thisWnd->getChild(d_thisWnd->getName() + "/TipText");
		d_thisWnd->setRenderMode(true);
		
		d_thisWnd->hide();
		d_thisWnd->subscribeEvent(TLStaticText::EventHidden, Event::Subscriber(&_Bubble::onHide, this));
		d_thisWnd->subscribeEvent(TLStaticText::EventShown, Event::Subscriber(&_Bubble::onShow, this));
		d_thisWnd->subscribeEvent(TLStaticText::EventNewFrame, Event::Subscriber(&_Bubble::onNewFrame, this));
		d_thisWnd->subscribeEvent(TLStaticText::EventAlphaChanged, Event::Subscriber(&_Bubble::onWndAlphaChanged, this));
		
		_tipText->setPosition(Point(0, 0));
		
		KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_thisWnd);
		
		_tipText->useLayout();

		KIniFile iniFile;
		if (iniFile.Load(UI_BUBBLE_INI_FILE_PATH))
		{
			iniFile.GetInteger("Bubble", "CloseTime", 3, &m_CloseTime);
			if (m_CloseTime <= 0)
			{
				m_CloseTime = 3;
			}
		}
		else
		{
			m_CloseTime = 3;
		}
	}
}

void _Bubble::destory()
{
//	d_thisWnd->destroy();
}

void _Bubble::setId(int id)
{
	//g_pCoreShell->GetGameData(GDI_CHAT_BUBBLE, false, d_id);
	d_id = id;
	g_pCoreShell->GetGameData(GDI_CHAT_BUBBLE, true, d_id);
}

void _Bubble::show(char* transText, bool bGm)
{
	if(!d_thisWnd)
	{
		return;
	}

	static char	layoutText[LAYOUT_TEXT_MAX_LEN];

	if(strlen(transText) >= LAYOUT_TEXT_MAX_LEN)
	{
		return;
	}

	layoutText[0] = 0;
	formatText(transText, (char*)layoutText, bGm);
	_tipText->getLayout()->SetText((char*)layoutText);
	_tipText->getLayout()->flashLayout();
	
	if(_tipText->getLayout()->isHaveContent())
	{
		_tipText->setText("");
		_tipText->setLayoutOffset(_tipText->getLeftFrameWidth(), _tipText->getTopFrameHeight());
		_tipText->fitLayoutSize();
	}
	else
	{
		_tipText->setText(AnsiToUtf8((char*)layoutText));
		_tipText->setHeight(Absolute, 40);
		_tipText->setWidth(Absolute, 100);
	}
	
	//调整父窗口和尾巴的大小
	d_thisWnd->setHeight(Absolute, _tipText->getHeight(Absolute) + _tail->getHeight(Absolute));
	d_thisWnd->setWidth(Absolute, _tipText->getWidth(Absolute));

	_tail->setPosition(Absolute, Point(_tipText->getWidth(Absolute) / 2, _tipText->getHeight(Absolute)));

	d_thisWnd->setAlpha(0);
	d_thisWnd->OpenBox();
	d_thisWnd->setAutoCloseTime((float)m_CloseTime);
}

void _Bubble::hide(void)
{
	if(d_thisWnd)
	{
		d_thisWnd->hide();
	}
}

bool _Bubble::onHide(const EventArgs& e)
{
	((WindowEventArgs*)&e)->window->stopUpdate();
	g_pCoreShell->GetGameData(GDI_CHAT_BUBBLE, false, d_id);
	return true;
}

bool _Bubble::onShow(const EventArgs& e)
{
	((WindowEventArgs*)&e)->window->beginUpdate();
	return true;
}

bool _Bubble::onNewFrame(const EventArgs& e)
{
	if(!d_thisWnd)
	{
		return false;
	}

	_tipText->getLayout()->setAlpha(d_thisWnd->getAlpha());
	_tipText->setAlpha(d_thisWnd->getAlpha());
	return true;
}

void _Bubble::updatePosition(int x, int y)
{
	if(!d_thisWnd)
	{
		return;
	}

	if(NpcHeadPop == d_type)
	{		
		d_thisWnd->setXPosition(Absolute, x - d_thisWnd->getAbsoluteWidth() / 2);
		d_thisWnd->setYPosition(Absolute, y - d_thisWnd->getAbsoluteHeight() + UI_BUBBLE_WINDOW_Y_OFF);
	}
	else if(TeamPop == d_type)
	{
		
	}
}

bool _Bubble::onWndAlphaChanged(const EventArgs& e)
{
	_tipText->getLayout()->setAlpha(d_thisWnd->getAlpha());
	_tipText->setAlpha(d_thisWnd->getAlpha());
	return true;
}

void _Bubble::formatText(const char* transText, char* layoutText, bool bGM)
{
	if(NULL == layoutText || NULL == transText)
	{
		return;
	}

	int textLen = strlen(transText);

	if(transText[0] != '<' || transText[textLen - 1] != '>')
	{
		return;
	}

	std::vector<char> formatTexts[COMMON_CLIENT_MSG_LEN_256];
	int index = 0;
	for(int i = 0; i < textLen; ++i)
	{
		if(transText[i] == '<')
		{
			continue;
		}
		else if(transText[i] == '>')
		{
			formatTexts[index].push_back(0);
			index++;
			continue;
		}
		else if(transText[i] == 1)
		{
			formatTexts[index].push_back('<');
		}
		else if(transText[i] == 2)
		{
			formatTexts[index].push_back('>');
		}
		else
		{
			formatTexts[index].push_back(transText[i]);
		}
	}

	layoutText[0] = 0;
	sprintf(layoutText, "<Layout width=%d margin-top=%d margin-left=%d margin-right=%d margin-bottom=%d>", 
		KUiCfgLoader::getSingleton().getChatBubbleCfg().windowWidth,
		KUiCfgLoader::getSingleton().getChatBubbleCfg().topMargin,
		KUiCfgLoader::getSingleton().getChatBubbleCfg().leftMargin,
		KUiCfgLoader::getSingleton().getChatBubbleCfg().RightMargin,
		KUiCfgLoader::getSingleton().getChatBubbleCfg().bottomMargin);
	strcat(layoutText, "<Seg text-align=left>");

	char msgText[COMMON_CLIENT_MSG_LEN_512];
	msgText[0] = 0;
	
	for(int j = 0; j < index; ++j)
	{
		char* curText = &formatTexts[j][0];
		if(formatTexts[j][0] == 'I')
		{
			int retCode = sscanf(curText, "I=%*d|%*d|%*d|%[^/0]", msgText);
			if(retCode != 1)
			{
				continue;
			}
			strcat(layoutText, "<Obj type=text vertical-align=bottom color=");
			strcat(layoutText, KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, bGM));
			strcat(layoutText, " font-family=");
			strcat(layoutText, KUiChanMgr::getSinglton().getChanFont(LOCAL_ROOM_ID, bGM));
			strcat(layoutText, ">");
			strcat(layoutText, msgText);
			strcat(layoutText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'N')
		{
			int retCode = sscanf(curText, "N=%[^\0]", msgText);
			if(retCode != 1)
			{
				continue;
			}
			strcat(layoutText, "<Obj type=text vertical-align=bottom color=");
			strcat(layoutText, KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, bGM));
			strcat(layoutText, " font-family=");
			strcat(layoutText, KUiChanMgr::getSinglton().getChanFont(LOCAL_ROOM_ID, bGM));
			strcat(layoutText, ">");
			strcat(layoutText, msgText);
			strcat(layoutText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'F')
		{
			int imageIndex = 0;
			int retCode = sscanf(curText, "F=%d", &imageIndex);
			if(retCode != 1)
			{
				continue;
			}

			strcat(layoutText, "<Obj type=pic vertical-align=bottom>");

			const KUiCfgLoader::FacePanelCfgData& facePanelCfg  = KUiCfgLoader::getSingleton().getFaceData();
			for(int i = 0; i < facePanelCfg.faceList.size(); ++i)
			{
				if(facePanelCfg.faceList[i].index == imageIndex)
				{
					strcpy(msgText, facePanelCfg.faceList[i].image);
					break;
				}
			}
			strcat(layoutText, msgText);
			strcat(layoutText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'P')
		{
			int retCode = sscanf(curText, "P=%*d|%*d|%*d|%*d|%[^\0]", msgText);
			if(retCode != 1)
			{
				continue;
			}
			strcat(layoutText, "<Obj type=text vertical-align=bottom color=");
			strcat(layoutText, KUiChanMgr::getSinglton().getChanColor(LOCAL_ROOM_ID, bGM));
			strcat(layoutText, " font-family=");
			strcat(layoutText, KUiChanMgr::getSinglton().getChanFont(LOCAL_ROOM_ID, bGM));
			strcat(layoutText, ">");
			strcat(layoutText, msgText);
			strcat(layoutText, "</Obj>");
		}
		else
		{
			continue;
		}
	}
	strcat(layoutText, "</Seg>");
	strcat(layoutText, "</Layout>");
}




KUiBubbleManager::KUiBubbleManager()
{
	for(int i = 0; i < UI_BUBBLE_MAX_COUNT; ++i)
	{
		d_bubbleList[i].create(i);
	}
}

KUiBubbleManager::~KUiBubbleManager()
{
	for(int i = 0; i < UI_BUBBLE_MAX_COUNT; ++i)
	{
		d_bubbleList[i].destory();
	}
}

int KUiBubbleManager::findIdle(_Bubble::BubbleType type, int id)
{
	//如果有该NPC正在说话，则找到该NPC的Bubble
	for(int i = 0; i < UI_BUBBLE_MAX_COUNT; ++i)
	{
		if(d_bubbleList[i].getId() == id && d_bubbleList[i].getType() == type)
			return i;
	}

	int idleIndex = UI_BUBBLE_INVALIDE_INDEX;
	float curLeftTime = 100000;
	for(int j = 0; j < UI_BUBBLE_MAX_COUNT; ++j)
	{
		if(d_bubbleList[j].d_thisWnd->isVisible() == false)
		{
			return j;
		}
		else if(d_bubbleList[j].d_thisWnd->getAutoCloseTime() < curLeftTime)
		{
			curLeftTime = d_bubbleList[j].d_thisWnd->getAutoCloseTime();
			idleIndex = j;
		}
	}

	return idleIndex;
}

void KUiBubbleManager::showBubble(char* layoutText, _Bubble::BubbleType type, int id, bool bGm)
{
	if(KUiSceneMap::getSinglton().isVisible() || KUiBigMap::getSinglton().isVisible())
	{
		return;
	}

	int bubbleIndex = findIdle(type, id);
	if(UI_BUBBLE_INVALIDE_INDEX == bubbleIndex)
	{
		return;
	}

	d_bubbleList[bubbleIndex].setId(id);
	d_bubbleList[bubbleIndex].setType(type);

	d_bubbleList[bubbleIndex].show(layoutText, bGm);
	d_bubbleList[bubbleIndex].updatePosition(-1000,-1000);
}

void KUiBubbleManager::updateBubble(_Bubble::BubbleType type, int id, Position pos)
{
	int bubbleIndex = findIdle(type, id);
	if(UI_BUBBLE_INVALIDE_INDEX == bubbleIndex)
	{
		return;
	}

	d_bubbleList[bubbleIndex].updatePosition(pos.x, pos.y);
}

void KUiBubbleManager::closeAll()
{
	for(int i = 0; i < UI_BUBBLE_MAX_COUNT; ++i)
	{
		d_bubbleList[i].hide();
	}
}