//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 20:42
//      File_base        : UiChatWindow
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "crtdbg.h"
#include "UiChatWindow.h"
#include "CoreShell.h"
#include "../KMessageCentre.h"
#include "UiLinkedItemTip.h"
#include "TLRadioButton.h"
#include "UiPlayerMenu.h"
#include "UiTipGenerator.h"
#include "UiErrorMessageBox.h"
#include "UiBubble.h"
#include "UiGMCommunication.h"
#include "UiChatCentre.h"
#include "UiItemTip.h"
#include "../UiSheetMgr.h"
#include "../UiAdapter.h"
#include "../../chatWindow/ChatMainDlg.h"
#include "UiESCDlg.h"
#include "SocialComDef.h"


#include "chatWindow/ChatCharContainer.h"
#include "ChatWindow/chatWnd.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "ui/UiCase/UiMapCentre.h"
#include "ui/UiCase/UiPathHelp.h"
#include "chatWindow/PlayerShowInfo.h"
#include "UiComMsgBox.h"
#include "UiIBShop.h"
#include "UiChatConfig.h"
#include "Ui/UiCase/UiEntrustComputer.h"

using namespace ClientMapInfo;


extern iCoreShell*		g_pCoreShell;

using namespace CEGUI;
using namespace CHAT;

template<>
KUiExtendChatWndBtn* KUiWndSingleton<KUiExtendChatWndBtn>::ms_Singleton = NULL;

KUiExtendChatWndBtn::KUiExtendChatWndBtn(const CEGUI::String& id_name)
: KUiWndSingleton<KUiExtendChatWndBtn>(id_name)
{
}

KUiExtendChatWndBtn::~KUiExtendChatWndBtn()
{
}

void KUiExtendChatWndBtn::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ExtChatWndSH/SHButton")->subscribeEvent(Window::EventMouseClick, 
			Event::Subscriber(&KUiExtendChatWndBtn::onClickExtendChatBtn, this));
	}
}

bool KUiExtendChatWndBtn::onClickExtendChatBtn(const CEGUI::EventArgs& args)
{
	KUiChannelCentre::GetSingleton().showExtendChatWnd();

	return true;
}

template<>
KUiEntrustComputerBtn* KUiWndSingleton<KUiEntrustComputerBtn>::ms_Singleton = NULL;

KUiEntrustComputerBtn::KUiEntrustComputerBtn( const CEGUI::String& id_name )
: KUiWndSingleton<KUiEntrustComputerBtn>( id_name )
{
}

KUiEntrustComputerBtn::~KUiEntrustComputerBtn()
{
}

void KUiEntrustComputerBtn::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		m_pThisWnd->getChild( "TaharezLook/EntrustComputerBtn/SHButton" )->subscribeEvent(
			Window::EventClicked, 
			Event::Subscriber( &KUiEntrustComputerBtn::onClickEntrustComputerBtn, this ) );
	}
}

bool KUiEntrustComputerBtn::onClickEntrustComputerBtn( const CEGUI::EventArgs& args )
{
	KUiEntrustComputer::GetSingleton().toggle();
	return true;
}


const char* KUiChanMgr::_emptyString = "";
#ifdef _DEBUG
const char* KUiChanMgr::_defaultColorString = "255,0,0";
const char* KUiChanMgr::_defaultFontString = "stzhongs-9";
#endif

KUiChanMgr::KUiChanMgr()
{	

}

KUiChanMgr& KUiChanMgr::getSinglton()
{
	static KUiChanMgr chanMgr;
	return chanMgr;
}

void KUiChanMgr::init()
{
	_chanList.clear();	
	_inputChanId = LOCAL_ROOM_ID;
	
	const KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();

	for(int i = 0; i < chanCfg.framesCfg.size(); ++i)
	{
		const KUiCfgLoader::ChatFrameCfg& frameCfg = chanCfg.framesCfg[i];
		KUiChannelCentre::GetSingleton().creatANewFrame(frameCfg.frameName.c_str());
	}

	KUiChannelCentre::GetSingleton().showFrame(UI_CHAT_WINDOW_INTERGRATION_FRAME_INDEX);

	Ui_Channel_Param coze;
	coze.dwChannelID = COSE_ROOM_ID;
	sprintf(coze.szChannelName, chanCfg.cozeChanName);
	regist(coze);

	coze.dwChannelID = COMBAT_INFO_ROOM_ID;
	sprintf(coze.szChannelName, chanCfg.combatChanName);
	regist(coze);

	//最近私聊相关数据
	clearLatestReciverList();
}

void KUiChanMgr::regist(Ui_Channel_Param& newChan)
{
	for(int i = 0; i < _chanList.size(); ++i)
	{
		if(newChan.dwChannelID == _chanList[i].dwChannelID)
		{
			return;
		}
	}

	_chanList.push_back(newChan);

	const KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();

	//查找到定制了该频道的页面，然后向找到的页面注册该频道
	for(int j = 0; j < chanCfg.framesCfg.size(); ++j)
	{
		const KUiCfgLoader::ChatFrameCfg& frameCfg = chanCfg.framesCfg[j];
		for(int k = 0; k < frameCfg.channelName.size(); ++k)
		{
			const string& chanRequired = frameCfg.channelName[k];
			if(chanRequired == string(newChan.szChannelName))
			{
				KUiChannelCentre::GetSingleton().registChannel(j, newChan.dwChannelID);
			}
		}
	}

	if(newChan.dwChannelID == COSE_ROOM_ID || newChan.dwChannelID == COMBAT_INFO_ROOM_ID)
	{
		return;
	}
	
	KUiChatInputWnd::GetSingleton().registChannel(newChan.dwChannelID, newChan.szChannelName);
}

void KUiChanMgr::unregist(int chanId)
{
	for(vector<Ui_Channel_Param>::iterator chanIt = _chanList.begin(); chanIt != _chanList.end(); ++chanIt)
	{
		if(chanIt->dwChannelID == chanId)
		{
			_chanList.erase(chanIt);
			break;
		}
	}

	KUiChannelCentre::GetSingleton().unregistChannel(chanId);
	KUiChatInputWnd::GetSingleton().unregistChannel(chanId);
}

void KUiChanMgr::unregistAll()
{
	for(int i = 0; i < _chanList.size(); ++i)
	{
		KUiChannelCentre::GetSingleton().unregistChannel(_chanList[i].dwChannelID);
		KUiChatInputWnd::GetSingleton().unregistChannel(_chanList[i].dwChannelID);
	}
	_chanList.clear();
}

int	KUiChanMgr::getChanIdByName(string name)
{
	for(int i = 0; i < _chanList.size(); ++i)
	{
		if(_chanList[i].szChannelName == name)
		{
			return _chanList[i].dwChannelID;
		}
	}

	return UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
}

const char* KUiChanMgr::getChanNameById(int chanId)
{
	//对战场频道做特例（当进入战场时，本地频道就是战场频道）
	if(MAP_ROOM_ID == chanId)
	{
		int isInCombat = g_pCoreShell->GetGameData( GDI_IS_PLAYER_IN_COMBAT_WORLD, NULL, NULL );
		if(isInCombat)
		{
			return KUiCfgLoader::getSingleton().getChannelData().battleChanName;
		}
	}

	for(int i = 0; i < _chanList.size(); ++i)
	{
		if(_chanList[i].dwChannelID == chanId)
		{
			return _chanList[i].szChannelName;
		}
	}
	return NULL;
}

void KUiChanMgr::getChanNameById(int chanId, char * nameBuffer, int bufferLen)
{
	if(MAP_ROOM_ID == chanId)
	{
		char channelName[COMMON_CLIENT_MSG_LEN_32];
		memset(channelName, 0, sizeof(channelName));

		g_pCoreShell->GetGameData(GDI_MAP_CHANNEL_NAME, (unsigned int)(channelName), bufferLen);
		if (strcmp(channelName, "") != 0)
		{
			int channelNameLen = sizeof(channelName);
			if (bufferLen < channelNameLen)
			{
				strncpy(nameBuffer, channelName, bufferLen);
				nameBuffer[bufferLen - 1] = 0;
			}
			else
			{
				strncpy(nameBuffer, channelName, channelNameLen);
				nameBuffer[bufferLen - 1] = 0;
			}
			return;
		}
	}

	int channelNameLen = sizeof(_chanList[0].szChannelName);
	for(int i = 0; i < _chanList.size(); ++i)
	{
		if(_chanList[i].dwChannelID == chanId)
		{
			if (bufferLen < channelNameLen)
			{
				strncpy(nameBuffer, _chanList[i].szChannelName, bufferLen);
				nameBuffer[bufferLen - 1] = 0;
			}
			else
			{
				strncpy(nameBuffer, _chanList[i].szChannelName, channelNameLen);
				nameBuffer[bufferLen - 1] = 0;
			}
		}
	}
}

bool KUiChanMgr::getChanInfo(int chanId, MapChannelInfo & info)
{
	g_pCoreShell->GetGameData(GDI_MAP_CHANNEL_INFO, (unsigned int)&info, 0);
	if (strcmp(info.szChannelName, "") == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

const char* KUiChanMgr::getChanColor(int chanId, bool bGM)
{
	if ( bGM )
	{
		chanId = SYSTEM_ROOM_ID;
	}
	const char* name = getChanNameById(chanId);
	if(!name)
	{
#ifdef _DEBUG
		return _defaultColorString;
#else
		return _emptyString;
#endif
	}

	const KUiCfgLoader::ChannelCfgData& channelCfgData = KUiCfgLoader::getSingleton().getChannelData();
	
	
	for(int i = 0; i < channelCfgData.chanStyle.size(); ++i)
	{
		const KUiCfgLoader::ChannelInfo& channelInfo = channelCfgData.chanStyle[i];
		if(channelInfo.chanName == name)
		{
			return channelInfo.chanContentColor.c_str();
		}
	}
#ifdef _DEBUG
	return _defaultColorString;
#else
	return _emptyString;
#endif
}


const char* KUiChanMgr::getChanFont(int chanId, bool bGM)
{
	const char* name = getChanNameById(chanId);
	if(!name)
	{
#ifdef _DEBUG
		return _defaultFontString;
#else
		return _emptyString;
#endif
	}

	const KUiCfgLoader::ChannelCfgData& channelCfgData = KUiCfgLoader::getSingleton().getChannelData();
	
	
	for(int i = 0; i < channelCfgData.chanStyle.size(); ++i)
	{
		const KUiCfgLoader::ChannelInfo& channelInfo = channelCfgData.chanStyle[i];
		if(channelInfo.chanName == name)
		{
			return channelInfo.chanContentFont.c_str();
		}
	}
#ifdef _DEBUG
	return _defaultFontString;
#else
	return _emptyString;
#endif
}

const char* KUiChanMgr::getChanNameColor(int chanId, bool bGM)
{
	if ( bGM )
	{
		chanId = SYSTEM_ROOM_ID;
	}

	const char* name = getChanNameById(chanId);
	if(!name)
	{
		return _emptyString;
	}

	const KUiCfgLoader::ChannelCfgData& channelCfgData = KUiCfgLoader::getSingleton().getChannelData();
	
	
	for(int i = 0; i < channelCfgData.chanStyle.size(); ++i)
	{
		const KUiCfgLoader::ChannelInfo& channelInfo = channelCfgData.chanStyle[i];
		if(channelInfo.chanName == name)
		{
			return channelInfo.chanNameColor.c_str();
		}
	}
	return _emptyString;
}

const char* KUiChanMgr::getChanNameFont(int chanId, bool bGM)
{
	if ( bGM )
	{
		chanId = SYSTEM_ROOM_ID;
	}

	const char* name = getChanNameById(chanId);
	if(!name)
	{
		return _emptyString;
	}

	const KUiCfgLoader::ChannelCfgData& channelCfgData = KUiCfgLoader::getSingleton().getChannelData();
	
	
	for(int i = 0; i < channelCfgData.chanStyle.size(); ++i)
	{
		const KUiCfgLoader::ChannelInfo& channelInfo = channelCfgData.chanStyle[i];
		if(channelInfo.chanName == name)
		{
			return channelInfo.chanNameFont.c_str();
		}
	}
	return _emptyString;
}

const char* KUiChanMgr::getPlayerNameColor()
{
	return KUiCfgLoader::getSingleton().getChannelData().playerNameColor;
}

const char* KUiChanMgr::getPlayerNameFont()
{
	return KUiCfgLoader::getSingleton().getChannelData().playerNameFont;
}

const char* KUiChanMgr::getItemColor(int color)
{
	if(color >= quality_count)
		return _emptyString;

	return KUiCfgLoader::getSingleton().getChannelData().itemNameColor[color];
}

const char* KUiChanMgr::getItemFont()
{
	return KUiCfgLoader::getSingleton().getChannelData().itemNameFont;
}

int	KUiChanMgr::prevInputChanId()
{
	int inputIndex = UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX;
	for(int i = 0; i < _chanList.size(); ++i)
	{
		if(_inputChanId == _chanList[i].dwChannelID)
		{
			inputIndex = i;
		}
	}

	if(UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX == inputIndex)
	{
		return UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
	}

	//上一个（私聊和战斗不算）
	do
	{
		--inputIndex;
		if(inputIndex < 0)
		{
			return UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
		}		
	}while(_chanList[inputIndex].dwChannelID == COSE_ROOM_ID || _chanList[inputIndex].dwChannelID == COMBAT_INFO_ROOM_ID);

	return _chanList[inputIndex].dwChannelID;
}

int	KUiChanMgr::nextInputChanId()
{
	int inputIndex = UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX;
	for(int i = 0; i < _chanList.size(); ++i)
	{
		if(_inputChanId == _chanList[i].dwChannelID)
		{
			inputIndex = i;
		}
	}

	if(UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX == inputIndex)
	{
		return UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
	}

	//下一个（私聊和战斗不算）
	do
	{
		++inputIndex;
		if(inputIndex > _chanList.size() - 1)
		{
			return UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
		}		
	}while(_chanList[inputIndex].dwChannelID == COSE_ROOM_ID || _chanList[inputIndex].dwChannelID == COMBAT_INFO_ROOM_ID);

	return _chanList[inputIndex].dwChannelID;
}

void KUiChanMgr::clearLatestReciverList()
{
	_curNameIndex = 0;
	_latestNameList.clear();
}

string KUiChanMgr::prevReciverName()
{
	if(_curNameIndex - 1 < 0 || _curNameIndex - 1 >= _latestNameList.size())
	{
		return "";
	}
	--_curNameIndex;
	return _latestNameList[_curNameIndex];
}

string KUiChanMgr::nextReciverName()
{
	if(_curNameIndex + 1 < 0 || _curNameIndex + 1 >= _latestNameList.size())
	{
		return "";
	}
	++_curNameIndex;
	return _latestNameList[_curNameIndex];
}

void KUiChanMgr::addReciverName(char* name)
{
	vector<string>::iterator nameIt = _latestNameList.begin();
	while(nameIt != _latestNameList.end())
	{
		string& curName = *nameIt;
		if(curName == name)
		{
			_latestNameList.erase(nameIt++);
			break;
		}
		else
		{
			++nameIt;
		}
	}

	_latestNameList.push_back(name);
	if(_latestNameList.size() > UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME)
	{
		_latestNameList.erase(_latestNameList.begin());
	}
}

string KUiChanMgr::getCurReciverName()
{
	if(_curNameIndex < 0 || _curNameIndex >= _latestNameList.size())
	{
		return "";
	}
	return _latestNameList[_curNameIndex];
}

const vector<string>& KUiChanMgr::getLatestReciver()
{
	return _latestNameList;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

template<> 
KUiChannelCentre* KUiWndSingleton<KUiChannelCentre>::ms_Singleton	= NULL;

KUiChannelCentre::KUiChannelCentre( const String& id_name ):
KUiWndSingleton<KUiChannelCentre>( id_name )
{
	d_extendChatBtn = NULL;
	
	for(int i = 0; i < FrameNum; ++i)
	{
		d_newMsgImg[i]		= NULL;
		d_frameBtn[i]		= NULL;
		d_framePanel[i]		= NULL;
		d_textCarrier[i]	= NULL;
		d_frameContent[i]	= NULL;
	}
}

KUiChannelCentre::~KUiChannelCentre()
{

}

void KUiChannelCentre::Init()
{
	d_resizing = false;
	d_maxHeight = KUiCfgLoader::getSingleton().getChannelData().windowMaxHeight;
	d_minHeight = KUiCfgLoader::getSingleton().getChannelData().windowMinHeight;

	d_delayTimeOut = 0;
	d_systemFrameIndex = UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX;
	m_pThisWnd->SetBottomWindow();
	getChild();
	
	showBackground(true);
}

void KUiChannelCentre::Show()
{
	//打开外置窗口的时候不让它打开
	if(IsWindowVisible(ChatMainDlg::hMainDlg))
	{
		return;
	}

	KUiWndSingleton<KUiChannelCentre>::Show();
	if(!ms_Singleton)
	{
		return;
	}
	
	if(ms_Singleton->d_extendChatBtn)
	{
		ms_Singleton->d_extendChatBtn->show();
	}	
	
	for(int i = 0; i < FrameNum; ++i)
	{
		if(ms_Singleton->d_newMsgImg[i])
		{
			KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(ms_Singleton->d_newMsgImg[i]);
		}
	}
}

void KUiChannelCentre::getChild()
{
	char childName[COMMON_CLIENT_MSG_LEN_128];
	
	m_pThisWnd->setZLevel(Window::SuperBottom);

	((TLStaticImage*)m_pThisWnd)->setDummyWnd(true);
	
	m_pThisWnd->subscribeEvent(Window::EventNewFrame, 
		Event::Subscriber(&KUiChannelCentre::updateSelf, this));

	for(int i = 0; i < FrameNum; ++i)
	{
		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d", i);
		d_framePanel[i] = (TLStaticImage*)m_pThisWnd->getChild(childName);
		d_framePanel[i]->setDummyWnd(true);

		d_framePanel[i]->subscribeEvent(Window::EventMouseWheel, 
		Event::Subscriber(&KUiChannelCentre::onTextPanelWheelChanged, this));

		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d/ChanneBtn", i);
		d_frameBtn[i] = (TLRadioButton*)d_framePanel[i]->getChild(childName);
		d_frameBtn[i]->subscribeEvent(TLRadioButton::EventMouseClick, 
			Event::Subscriber(&KUiChannelCentre::clickFrameBtn, this));
		d_frameBtn[i]->subscribeEvent(TLRadioButton::EventMouseButtonDown, 
			Event::Subscriber(&KUiChannelCentre::clickFrameBtnDown, this));

		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d/ChannelContent", i);
		d_frameContent[i] = (TLStaticText*)d_framePanel[i]->getChild(childName);
		d_frameContent[i]->setDummyWnd(true);
		
		//文字——Begin
		//创建每个分页的文字载体
		sprintf(childName, "channel_text_%d", i);
		d_textCarrier[i] = (TLStaticText*)WindowManager::getSingleton().createWindow(TLStaticText::WidgetTypeName, childName);
		d_frameContent[i]->addChildWindow(d_textCarrier[i]);
		d_textCarrier[i]->setDummyWnd(true);
		d_textCarrier[i]->setWidth(Absolute, 
			d_frameContent[i]->getAbsoluteWidth()
			- d_frameContent[i]->getLeftFrameWidth() - d_frameContent[i]->getRightFrameWidth());

		//计算排版裁剪区域
		d_frameContent[i]->setMetricsMode(Absolute);
		Rect textArea = d_frameContent[i]->getUnclippedPixelRect();

		textArea.d_left		+= d_frameContent[i]->getLeftFrameWidth();
		textArea.d_right	-= d_frameContent[i]->getRightFrameWidth();
		textArea.d_top		+= d_frameContent[i]->getTopFrameHeight();
		textArea.d_bottom	-= d_frameContent[i]->getBottomFrameHeight();
		
		//裁剪区域必须是相对底板的位置
 		Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 		textArea.setPosition(posOff);

		LORect clipper;
		cerectToLorect(&textArea, &clipper);

		for(int j = 0; j < KUiCfgLoader::getSingleton().getChannelData().maxCentence; ++j)
		{
			//为每个文字载体创建最大个数的单句话的载体--用于实际显示
			sprintf(childName, "channel_text_%d_%d", i, j);
			TLStaticText* aCentenceCtrl = (TLStaticText*)WindowManager::getSingleton().createWindow(TLStaticText::WidgetTypeName, childName);
			d_textItems[i].push_back(aCentenceCtrl);
			d_textCarrier[i]->addChildWindow(aCentenceCtrl);
			aCentenceCtrl->setDummyWnd(true);

			//设置排版参数
 			aCentenceCtrl->useLayout();
 			aCentenceCtrl->getLayout()->setClipper(clipper);
 			aCentenceCtrl->getLayout()->setBorderMode(false);
			
			aCentenceCtrl->subscribeEvent(TLStaticText::EventMouseDoubleClick, 
				Event::Subscriber(&KUiChannelCentre::clickText, this));
			
			aCentenceCtrl->subscribeEvent(TLStaticText::EventMouseButtonDown, 
				Event::Subscriber(&KUiChannelCentre::clickText, this));

			aCentenceCtrl->subscribeEvent(TLStaticText::EventMouseHover, 
				Event::Subscriber(&KUiChannelCentre::hoverText, this));

			aCentenceCtrl->subscribeEvent(TLStaticText::EventMouseMove, 
				Event::Subscriber(&KUiChannelCentre::hoverText, this));
			
			aCentenceCtrl->subscribeEvent(TLStaticText::EventMouseLeaves, 
				Event::Subscriber(&KUiChannelCentre::mouseOutText, this));
		}
		d_curTopItem[i] = 0;
		//文字——End

		//滑动条
		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d/Scrollbar", i);
		d_scrollBar[i] = (TLVertScrollbar*)d_framePanel[i]->getChild(childName);
		d_scrollBar[i]->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
			Event::Subscriber(&KUiChannelCentre::scroll, this));
		d_scrollBar[i]->setScrollPosition(1.0f);

		//最上最下按钮
		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d/ToTopBtn", i);
		d_toTopBtn[i] = (TLButton*)d_framePanel[i]->getChild(childName);
		d_toTopBtn[i]->subscribeEvent(TLButton::EventMouseClick, 
			Event::Subscriber(&KUiChannelCentre::clickToTop, this));

		
		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d/ToBottomBtn", i);
		d_toBottomBtn[i] = (TLButton*)d_framePanel[i]->getChild(childName);
		d_toBottomBtn[i]->subscribeEvent(TLButton::EventMouseClick, 
			Event::Subscriber(&KUiChannelCentre::clickToButtom, this));
		
 		sprintf(childName, "TaharezLook/ChannelCentre/ChannelPanel%d/NewMsgImg", i);
 		d_newMsgImg[i] = (TLStaticImage*)d_framePanel[i]->getChild(childName);
		KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_newMsgImg[i]);
 		d_newMsgImg[i]->hide();
		d_newMsgImg[i]->SetBottomWindow();
		d_newMsgImg[i]->setZLevel(Window::Bottom);
	}
	
	//链接hover状态图片
	d_hoverImage = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/SelectFrameImage");
	d_hoverImage->disable();
	d_hoverImage->setZLevel(Window::Top);
	d_hoverImage->setFrameEnabled(true);
	d_hoverImage->setBackgroundEnabled(false);

	//拖动按钮
	d_moveBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/MoveBtn");
	d_moveBtn->subscribeEvent(TLButton::EventMouseButtonDown, Event::Subscriber(&KUiChannelCentre::onMoveBtnDown, this));
	d_moveBtn->subscribeEvent(TLButton::EventMouseButtonUp, Event::Subscriber(&KUiChannelCentre::onMoveBtnUp, this));
	d_moveBtn->subscribeEvent(TLButton::EventMouseMove, Event::Subscriber(&KUiChannelCentre::onMoveBtnMove, this));

	//外置聊天按钮
	d_extendChatBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/ExtendChatWindow");
	//把这个按钮从聊天窗口中摘出来
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_extendChatBtn);
	Point pos = m_pThisWnd->getPosition(Absolute);
	pos.d_y += m_pThisWnd->getHeight(Absolute) - d_extendChatBtn->getHeight(Absolute);
	d_extendChatBtn->setPosition(Absolute, pos);

	d_extendChatBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChannelCentre::onClickExtendChatBtn, this));
	d_extendChatBtn->setRenderMode(false,2);
	d_extendChatBtn->setZLevel(Window::Bottom);

	d_chanBtnWidth = d_frameBtn[0]->getWidth(Absolute);
	
	m_pThisWnd->subscribeEvent(Window::EventNewFrame, Event::Subscriber(&KUiChannelCentre::updateSelf, this));
	
	//GM留言板按钮
// 	Window*	gmPanel = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/GM");
// 	gmPanel->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChannelCentre::onClickGM, this));

	//设置特殊绘制流程
	int height = m_pThisWnd->getHeight(Absolute);
	adjustWindowHeight(d_maxHeight);
//	m_pThisWnd->setRenderMode(true);
	m_pThisWnd->setRenderMode(false, 3);
	adjustWindowHeight(height);
	m_pThisWnd->beginUpdate();
}

bool KUiChannelCentre::validateFrameIndex(int frameIndex)
{	
	if(frameIndex < 0 || frameIndex >= FrameNum)
	{
		_ASSERT(0);
		return false;
	}
	return true;
}

void KUiChannelCentre::Hide( void )
{
	if ( ms_Singleton && ms_Singleton->d_extendChatBtn )
	{
		ms_Singleton->d_extendChatBtn->hide();
		for(int i = 0; i < FrameNum; ++i)
		{
			if(ms_Singleton->d_newMsgImg[i])
			{
				ms_Singleton->d_newMsgImg[i]->hide();
			}
		}
	}
	
	KUiWndSingleton<KUiChannelCentre>::Hide();
}

bool KUiChannelCentre::updateSelf(const EventArgs& timeArg)
{
	FrameEventArgs* args = (FrameEventArgs*)(&timeArg);
	d_delayTimeOut += args->elapse;
	if(d_delayTimeOut > 1500 && d_delayMsgs.size())
	{
		for(int i = 0; i < d_delayMsgs.size(); ++i)
		{
			dispatchMessage(d_delayMsgs[i].channelId, d_delayMsgs[i].msg.c_str());
		}
		d_delayMsgs.clear();
		d_delayTimeOut = 0;
	}

	static int redrawTime = 0;
	if(redrawTime > 500)
	{
		redrawTime = 0;
		m_pThisWnd->requestRedraw();
	}
	redrawTime += args->elapse;
	return true;
}

bool KUiChannelCentre::clickFrameBtnDown(const EventArgs& args)
{
	return true;
}

bool KUiChannelCentre::clickFrameBtn(const EventArgs& args)
{
	int clickFrameIndex = UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX;
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;

	for(int i = 0; i < FrameNum; ++i)
	{
		if(d_frameBtn[i] == pArgs->window)
		{
			//右键弹出配置窗口
			if(pArgs->button == RightButton && d_frameBtn[i]->getTooltipText().length() != 0)
			{
				KUiChanConfig::getSingleton().show(i);
			}
			else
			{
				showFrame(i);
				d_frameBtn[i]->setSelected(true);
				clickFrameIndex = i;
			}
			break;
		}
	}

	//inputbox要得到输入焦点
	if(KUiChatInputWnd::IsVisible())
	{
		KUiChatInputWnd::GetSingleton().show();
	}

	return true;
}

void KUiChannelCentre::hideAllFrame()
{
	for(int i = 0; i < FrameNum; ++i)
	{
		if(d_systemFrameIndex == i)
		{
			continue;
		}
		d_frameContent[i]->hide();
		d_scrollBar[i]->hide();
		d_frameBtn[i]->setSelected(false);
		d_toTopBtn[i]->hide();
		d_toBottomBtn[i]->hide();
	}
	
	d_resizing = false;
}

void KUiChannelCentre::showFrame(int chanIndex)
{
	hideAllFrame();

	if(!d_frameContent[chanIndex] 
		|| !d_scrollBar[chanIndex]
		|| !d_frameBtn[chanIndex]
		|| !d_toTopBtn[chanIndex]
		|| !d_toBottomBtn[chanIndex]
		|| !d_newMsgImg[chanIndex])
	{
		return;
	}

	d_frameContent[chanIndex]->show();
	d_scrollBar[chanIndex]->show();
	d_frameBtn[chanIndex]->setSelected(true);
	d_toTopBtn[chanIndex]->show();
	d_toBottomBtn[chanIndex]->show();
	d_newMsgImg[chanIndex]->hide();
}

int	KUiChannelCentre::splitSystemFrame(int frameIndex)
{
	if(!d_framePanel[frameIndex])
	{
		return false;
	}

	d_systemFrameIndex = frameIndex;
	Rect area = KUiChatInputWnd::GetSingleton().d_inputBox->getUnclippedPixelRect();
	Rect parentArea = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->getUnclippedPixelRect();
 	area.d_left = parentArea.d_right - d_framePanel[frameIndex]->getAbsoluteWidth();
 	area.d_right = parentArea.d_right;
	area.d_top = area.d_bottom - d_framePanel[frameIndex]->getAbsoluteHeight();

	d_framePanel[frameIndex]->setPosition(Absolute, area.getPosition());
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_framePanel[frameIndex]);
//	d_framePanel[frameIndex]->setRenderMode(true);
	d_framePanel[frameIndex]->setRenderMode(false, 3);
	d_framePanel[frameIndex]->SetBottomWindow();

	d_framePanel[frameIndex]->setZLevel(Window::SuperBottom);
	d_scrollBar[frameIndex]->setHeight(Absolute, d_framePanel[frameIndex]->getAbsoluteHeight()
		- d_toTopBtn[frameIndex]->getAbsoluteHeight() - d_toBottomBtn[frameIndex]->getAbsoluteHeight());
	d_toBottomBtn[frameIndex]->setYPosition(Absolute, d_scrollBar[frameIndex]->getAbsoluteYPosition() + d_scrollBar[frameIndex]->getAbsoluteHeight());
	
	d_frameContent[frameIndex]->setHeight(Absolute, d_framePanel[frameIndex]->getAbsoluteHeight());
	//计算排版裁剪区域
	d_frameContent[frameIndex]->setMetricsMode(Absolute);
	Rect textArea = d_frameContent[frameIndex]->getUnclippedPixelRect();

	textArea.setPosition(Point(0, 0));
	
	textArea.d_left		+= d_frameContent[frameIndex]->getLeftFrameWidth();
	textArea.d_right	-= d_frameContent[frameIndex]->getRightFrameWidth();
	textArea.d_top		+= d_frameContent[frameIndex]->getTopFrameHeight();
	textArea.d_bottom	-= d_frameContent[frameIndex]->getBottomFrameHeight();
	
	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	
	vector<TLStaticText*>& textItems = d_textItems[frameIndex];
	for(int i = 0; i < textItems.size(); ++i)
	{
		ILayout* layout = textItems[i]->getLayout();
		if(layout)
		{
			layout->clearLayout();
			layout->setClipper(clipper);
			textItems[i]->fitLayoutSize();
		}
	}
	d_frameBtn[frameIndex]->hide();
	return true;
}

int KUiChannelCentre::creatANewFrame(const char* frameName)
{
	for(int i = 0; i < FrameNum; ++i)
	{
		if(false == d_frameInfo[i]._used)
		{
			if(!d_newMsgImg[i] 
			|| !d_frameBtn[i]
			|| !d_framePanel[i]
			|| !d_frameContent[i])
			{
				continue;
			}
			d_frameInfo[i]._used = true;
			d_frameBtn[i]->setText(AnsiToUtf8(frameName));
			d_newMsgImg[i]->hide();

			d_wndWidth[i] = d_frameContent[i]->getUnclippedInnerRect().getWidth()
				- d_frameContent[i]->getLeftFrameWidth() - d_frameContent[i]->getRightFrameWidth();
			
			layoutBtn();
			d_framePanel[i]->show();
			d_curTopItem[i] = 0;

			vector<TLStaticText*>& textItems = d_textItems[i];
			for(int itemIndex = 0; itemIndex < textItems.size(); ++itemIndex)
			{
				ILayout* layout = textItems[itemIndex]->getLayout();
				if(layout)
				{
					layout->clearLayout();
					textItems[itemIndex]->fitLayoutSize();
				}
			}

			const char* sysname = KUiCfgLoader::getSingleton().getChannelData().systemChanName;
			if(strcmp(frameName, sysname) == false)
			{
				splitSystemFrame(i);
			}
			else
			{
				m_pThisWnd->addChildWindow(d_framePanel[i]);
			}

			return i;
		}
	}
	return UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX;
}

bool KUiChannelCentre::isExsitAFrame(char* frameName)
{
	for(int i = 0; i < FrameNum; ++i)
	{
		if(d_frameBtn[i]->getText() == String(AnsiToUtf8(frameName)))
		{
			return true;
		}
	}
	return false;
}

void KUiChannelCentre::closeAFrame(int frameIndex)
{
	if(!validateFrameIndex(frameIndex))
	{
		return;
	}

	d_frameInfo[frameIndex]._chanList.clear();
	d_frameInfo[frameIndex]._used = false;

	d_framePanel[frameIndex]->hide();

	layoutBtn();
}

void KUiChannelCentre::closeAllFrame()
{
	for(int i = 0; i < FrameNum; ++i)
	{
		closeAFrame(i);
	}
}

void KUiChannelCentre::registChannel(int frameIndex, int chanId)
{
	if(!validateFrameIndex(frameIndex))
	{
		return;
	}

	if(false == d_frameInfo[frameIndex]._used)
	{
		return;
	}

	//向该页面注册频道
	vector<int>& chans = d_frameInfo[frameIndex]._chanList;
	for(int i = 0; i < chans.size(); ++i)
	{
		if(chans[i] == chanId)
		{
			return;
		}
	}
	chans.push_back(chanId);
}

void KUiChannelCentre::unregistChannel(int frameIndex, int chanId)
{
	if(!validateFrameIndex(frameIndex))
	{
		return;
	}

	vector<int>& chanIds = d_frameInfo[frameIndex]._chanList;
	for(vector<int>::iterator itChanId = chanIds.begin(); itChanId != chanIds.end(); ++itChanId)
	{
		if(*itChanId == chanId)
		{
			chanIds.erase(itChanId);
			break;
		}
	}
}

void KUiChannelCentre::unregistChannel(int chanId)
{
	for(int i = 0; i < FrameNum; ++i)
	{
		unregistChannel(i, chanId);
	}
}

void KUiChannelCentre::freshFrameChannel(int frameIndex)
{
	if(!validateFrameIndex(frameIndex))
	{
		return;
	}
	
	const KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();
	
	if(frameIndex < 0 || frameIndex >= chanCfg.framesCfg.size())
	{
		return;
	}
	
	vector<int>& chanIds = d_frameInfo[frameIndex]._chanList;
	chanIds.clear();

	const KUiCfgLoader::ChatFrameCfg& frameCfg = chanCfg.framesCfg[frameIndex];
	for(int i = 0; i < frameCfg.channelName.size(); ++i)
	{
		const string& chanName = frameCfg.channelName[i];
		int chanId = KUiChanMgr::getSinglton().getChanIdByName(chanName);
		if(chanId != UI_CHAT_WINDOW_INVALIDATE_CHAN_ID)
		{
			chanIds.push_back(chanId);
		}
	}

	d_frameBtn[frameIndex]->setText(AnsiToUtf8(frameCfg.frameName.c_str()));
}

void KUiChannelCentre::showBackground(bool show)
{
	for(int i = 0; i < FrameNum; ++i)
	{
		d_frameContent[i]->setBackgroundEnabled(show);
		d_frameContent[i]->setFrameEnabled(show);
	}
}

void KUiChannelCentre::layoutBtn()
{
	int chanCount = 0;
	for(int i = 0; i < FrameNum; ++i)
	{
		if(!d_frameBtn[i]
		|| !d_newMsgImg[i])
		{
			continue;
		}

		if(false == d_frameInfo[i]._used)
		{
			d_frameBtn[i]->hide();
			continue;
		}
		d_frameBtn[i]->show();
		d_frameBtn[i]->setXPosition(Absolute, d_extendChatBtn->getWidth(Absolute) + d_chanBtnWidth * chanCount);
		d_newMsgImg[i]->setXPosition(Absolute, d_frameBtn[i]->getUnclippedInnerRect().d_left);
		d_newMsgImg[i]->setYPosition(Absolute, d_frameBtn[i]->getUnclippedInnerRect().d_top);
		++chanCount;
	}
}

void KUiChannelCentre::showSystemFrame(bool show)
{
	if(d_systemFrameIndex != UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX)
	{
		show?d_framePanel[d_systemFrameIndex]->show():d_framePanel[d_systemFrameIndex]->hide();
	}
}

bool KUiChannelCentre::onClickExtendChatBtn(const EventArgs& args)
{
	showExtendChatWnd();
	
	return true;
}

// bool KUiChannelCentre::onClickGM(const EventArgs& args)
// {
// 	if(!KUiGMCommunication::getSingleton().isVisible())
// 	{
// 		KUiGMCommunication::getSingleton().setYourMsg("");
// 		KUiGMCommunication::getSingleton().show();
// 		KUiGMCommunication::getSingleton().showCommit();
// 	}
// 	else
// 	{
// 		KUiGMCommunication::getSingleton().hide();
// 	}
// 	
// 	return true;
// }

void KUiChannelCentre::showExtendChatWnd()
{
#ifdef USING_CHAT_WINDOW
	if(IsWindowVisible(ChatMainDlg::hMainDlg))
	{
		ChatMainDlg::MainDlgShowWndChat(false);
		KUiMiniMap::Show();
		m_pThisWnd->show();
//		if(d_systemFrameIndex != UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX)
//		{
//			d_framePanel[d_systemFrameIndex]->show();
//		}
		showSystemFrame(true);
		
		for(int i = 0; i < FrameNum; ++i)
		{
			if(d_newMsgImg[i])
			{
				KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_newMsgImg[i]);
			}
		}
	}
	else
	{
		BOOL bHide = false;
		RECT rc;
		GetClientRect(GetDesktopWindow(),&rc);
		RECT extraWndRect;
		GetWindowRect(ChatMainDlg::hMainDlg,&extraWndRect);
		int height = extraWndRect.right - extraWndRect.left;
		if(rc.right - KWin32App::m_uScreenWidth<height)
			bHide = true;
		if(KWin32App::m_bFullScreen||bHide)
		{
			char comfirmString[COMMON_CLIENT_MSG_LEN_8];
			strcpy(comfirmString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().comfirmString));
			KUiComMsgBox::GetSingleton().setComMsgPosition();
			KUiComMsgBox::Show();
			KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(KMessageCentre::GetMessage(common_message, 101)));
			KUiComMsgBox::GetSingleton().setBtnName((utf8*)comfirmString);
			return;
		}
		KUiMiniMap::Hide();
		
		ChatMainDlg::AjustMainWindow();
		ChatMainDlg::MainDlgShowWndChat(true);
		SendMessage(ChatMainDlg::hMainDlg,WM_SYSCOMMAND,SC_RESTORE,0);
		KUiChatInputWnd::Hide();
		m_pThisWnd->hide();
		showSystemFrame(false);
		for(int i = 0; i < FrameNum; ++i)
		{
			KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->removeChildWindow(d_newMsgImg[i]);
		}
		ChatMainDlg::ReceiveFriendList();
	}

	//KUiIBNavigation::GetSingleton().MoveToRightEdge();
#endif

}
bool KUiChannelCentre::hoverText(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* textCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = textCtrl->getLayout();

	if(lay == NULL)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}

	Point pos = textCtrl->getUnclippedPixelRect().getPosition();
	Point off = textCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;

	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}

	if(elemInfo.gameObj._objType == LO_GO_NOTHING)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		d_hoverImage->hide();
		return false;
	}

	KUiAdapter::SetMouseRes( MOUSE_SUPER_LINK_PLAYER + elemInfo.gameObj._objType - 1 );
	return true;

// 	d_hoverImage->show();
// 
// 	//计算位置
// 	LORect& area = elemInfo.area;
// 	pos = Point(area.getLeft(), area.getTop()) - Point(d_hoverImage->getLeftFrameWidth(), d_hoverImage->getTopFrameHeight())
// 		+ textCtrl->getUnclippedPixelRect().getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
// 	d_hoverImage->setPosition(Absolute, pos);
// 	
// 	d_hoverImage->setWidth(Absolute, area.getWidth() + d_hoverImage->getLeftFrameWidth() + d_hoverImage->getRightFrameWidth());
// 	d_hoverImage->setHeight(Absolute, area.getHeight() + d_hoverImage->getTopFrameHeight() + d_hoverImage->getBottomFrameHeight());
// 
// 	return true;
}

bool KUiChannelCentre::mouseOutText(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* textCtrl = (TLStaticText*)mouse->window;

	d_hoverImage->hide();

	return true;
}

bool KUiChannelCentre::clickText(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* textCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = textCtrl->getLayout();

	if(lay == NULL)
		return false;
	
	Point pos = textCtrl->getUnclippedPixelRect().getPosition();
	Point off = textCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return false;
	}
	
	bool handled = false;
	switch(elemInfo.gameObj._objType)
	{
	case LO_GO_PLAYER:
		{
			if(LeftButton == mouse->button)
			{
				if(mouse->sysKeys & Shift)
				{
					PlayerInfo playerInfo;
					memset(&playerInfo,0,sizeof(PlayerInfo));
					char* name = NULL;
					unicodeToAnsi(elemInfo.content.get(), name);
					strcpy(playerInfo.szName,name);
					PlayerShowInfo::GetSingle().AddItem(playerInfo);
					delete [] name;	
				}
				else
				{
					if (elemInfo.gameObj._objId[1] == 0) //非蒙面状态
					{
						KUiChatInputWnd::GetSingleton().clearText();
						
						KUiChatInputWnd::GetSingleton().write("/");
						
						LOElemInfo name;
						name.elemType = LO_TEXT;
						name.content = const_cast<wchar_t*>(elemInfo.content.get());
						KUiChatInputWnd::GetSingleton().write(name);
						KUiChatInputWnd::GetSingleton().write(" ");
						
						KUiChatInputWnd::GetSingleton().show();
					}
				}
			}
			else if(RightButton == mouse->button)
			{
				char* name = NULL;
				unicodeToAnsi(elemInfo.content.get(), name);

				KUiPlayerBaseInfo tagRoleInfo;	
				g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, NULL );
				
				if ( 0 == strcmp(tagRoleInfo.Name, name) )
					return false;

				KUiPlayerItem tagPlayerID;
				memcpy(tagPlayerID.Name, name, strlen(name) + 1);
				g_pCoreShell->GetGameData(GDI_GET_NPC_ID_BY_NAME, (unsigned int)&tagPlayerID, NULL);
				g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, (int)tagPlayerID.uId);
				
				Point parentP = m_pThisWnd->getPosition(Absolute);
				Point newP = MouseCursor::getSingleton().getPosition();
				newP.d_y -= KUiPlayerMenu::GetSingleton().getArea().getHeight() + 20;
				KUiPlayerMenu::GetSingleton().setPos(newP);
				
				//之前取得的camou，1表示伪装
				int _iTeamID = 0;
				if (tagRoleInfo.bTeam)
				{
					_iTeamID = 10;
				}

				LOElemInfo * elemList = NULL;
				int elemCount = lay->getElemList(elemList);

				char tempText[COMMON_CLIENT_MSG_LEN_256];
				memset(tempText, 0, sizeof(tempText));
				size_t currentLength = 0;
				if (elemList != NULL)
				{
					char * str = NULL;
					if (elemCount > 1)
					{
						unicodeToAnsi(elemList[1].content.get(), str);
						if (strcmp(str, name) != 0)
						{
							for (int i = 1; i < elemCount; ++i)
							{
								unicodeToAnsi(elemList[i].content.get(), str);
								if (str == NULL)
								{
									continue;
								}
								
								size_t tempLen = strlen(str);
								if ((tempLen + currentLength) <= (COMMON_CLIENT_MSG_LEN_256 - 1))
								{
									strcat(tempText, str);
									currentLength += tempLen;
								}
								else
								{
									strncat(tempText, str, (COMMON_CLIENT_MSG_LEN_256 - 1 - currentLength));
									currentLength = COMMON_CLIENT_MSG_LEN_256 - 1;
								}
							}
						}
						else
						{
							for (int i = 3; i < elemCount; ++i)
							{
								unicodeToAnsi(elemList[i].content.get(), str);
								if (str == NULL)
								{
									continue;
								}
								
								size_t tempLen = strlen(str);
								if ((tempLen + currentLength) <= (COMMON_CLIENT_MSG_LEN_256 - 1))
								{
									strcat(tempText, str);
									currentLength += tempLen;
								}
								else
								{
									strncat(tempText, str, (COMMON_CLIENT_MSG_LEN_256 - 1 - currentLength));
									currentLength = COMMON_CLIENT_MSG_LEN_256 - 1;
								}
							}
						}
						delete[] str;
					}
				}

				tempText[COMMON_CLIENT_MSG_LEN_256 - 1] = 0;
				KUiPlayerMenu::GetSingleton().SetReportText(tempText, currentLength);
				KUiPlayerMenu::GetSingleton().ShowByClickText(name, elemInfo.gameObj._objId[0], elemInfo.gameObj._objId[1] > 0);
				delete[] name;
				name = NULL;
			}
			handled = true;
		}
		break;
	case LO_GO_ITEM:
		{
			if(mouse->sysKeys & Control && KUiChatInputWnd::IsVisible())
			{				
				KUiChatInputWnd::GetSingleton().write(elemInfo);
				KUiChatInputWnd::GetSingleton().show();
			}
			else
			{
				KUiTipGenerator::TipObject tipObj;
				tipObj.type = KUiTipGenerator::LinkedItem;
				SpliteHashId(elemInfo.gameObj._objId[0], tipObj.ids[0], tipObj.ids[1], tipObj.ids[2]);
				tipObj.ids[3]	= elemInfo.gameObj._objId[1];
				tipObj.ids[4]	= elemInfo.gameObj._objId[2];
				char* layoutDes = KUiTipGenerator::getSinglton().genLayoutDes(tipObj);
				KUiLinkedItemTip::GetSingleton().show(layoutDes, m_pThisWnd->getRect(Absolute), KUiLinkedItemTip::TopRight);
				char* compareLayoutText = KUiTipGenerator::getSinglton().genCompareLayoutDes(tipObj);
				KUiLinkedItemTip::GetSingleton().showCompare(compareLayoutText);
			}
			handled = true;
		}
		break;
	case LO_GO_POSITION:
		{
			if(mouse->sysKeys & Control && KUiChatInputWnd::IsVisible())
			{				
				KUiChatInputWnd::GetSingleton().write(elemInfo);
				KUiChatInputWnd::GetSingleton().show();
			}
			else
			{	
				if(elemInfo.gameObj._objId[3] == 1)
				{
					const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetMapInfo(elemInfo.gameObj._objId[0]);
					if(pMapInfo == 0)
						return true;
					KUiSceneTimeInfo mapInfo = {0};
					g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
					if(strcmp(pMapInfo->mapName,mapInfo.szSceneName) == 0)
					{
						g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)elemInfo.gameObj._objId[1], (int)elemInfo.gameObj._objId[2] * 2);
					}
					else
					{
						KUiChannelCentre::GetSingleton().toSysMsg(ChatString::ChatStringGetString().autoGoInfo);
					}

					handled = true;
					break;

				}
				KUiSceneTimeInfo mapInfo = { 0 };
				
				g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
				mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;

				if(elemInfo.gameObj._objId[0] == mapInfo.nSceneId)
				{
					g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)elemInfo.gameObj._objId[1], (int)elemInfo.gameObj._objId[2] * 2);
				}
				else
				{
// 					char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
// 					KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
					
					char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
					if(szMsg)
					{
						char errorMsg[COMMON_CLIENT_MSG_LEN_1024];
						memset(errorMsg, 0, sizeof(errorMsg));
						char mapName[COMMON_CLIENT_MSG_LEN_64];
						memset(mapName, 0, sizeof(mapName));
						
						//获得地图名
						KIniFile mapFile;
						if(mapFile.Load( "\\settings\\maplist.ini" ))
						{
							char mapId[COMMON_CLIENT_MSG_LEN_32];
							sprintf(mapId, "%d", elemInfo.gameObj._objId[0]);
							mapFile.GetString( "List", mapId, "", mapName, sizeof(mapName));
						}
						sprintf(errorMsg, szMsg, mapName, elemInfo.gameObj._objId[1], elemInfo.gameObj._objId[2]);
						KUiChannelCentre::GetSingleton().toSysMsg(errorMsg);
					}
				}
				handled = true;
			}
		}
		break;
	case LO_GO_CHANNEL:
		{
			int chanId = elemInfo.gameObj._objId[0];
			if(chanId != SYSTEM_ROOM_ID)
			{
				KUiChatInputWnd::GetSingleton().switchChannel(chanId);
				KUiChatInputWnd::GetSingleton().show();
				handled = true;
			}
		}
		break;
	case LO_GO_FACE:
		{
			int chanId = elemInfo.gameObj._objId[0];
			handled = true;
		}
		break;
	default:
		break;
	}
	
	if(KUiChatInputWnd::IsVisible())
	{
		KUiChatInputWnd::GetSingleton().show();
	}
	return handled;
}


void KUiChannelCentre::adjustLayoutPos(int panelIndex)
{
	int textHeight = d_textCarrier[panelIndex]->getUnclippedPixelRect().getHeight();
	int clipperHeight = d_frameContent[panelIndex]->getHeight(Absolute) 
		- d_frameContent[panelIndex]->getTopFrameHeight() - d_frameContent[panelIndex]->getBottomFrameHeight();
	
	Point offset = Point(d_frameContent[panelIndex]->getLeftFrameWidth(), d_frameContent[panelIndex]->getTopFrameHeight());
	if(textHeight <= clipperHeight)
	{
		d_textCarrier[panelIndex]->setPosition(Absolute, offset);
		return;
	}

	float scrollPos = d_scrollBar[panelIndex]->getScrollPosition();
	
	if(1.0f == scrollPos)
	{
		d_newMsgImg[panelIndex]->hide();
	}

	int yPos = (textHeight - clipperHeight) * scrollPos;
	
	Point newPos = Point(0 , -yPos - 0.5) + offset;	//+0.5是由于cegui底层函数BUG导致
	d_textCarrier[panelIndex]->setPosition(Absolute, newPos);
}

void KUiChannelCentre::relayoutText(int panelIndex)
{
	vector<TLStaticText*>& curItems = d_textItems[panelIndex];
	int& curTopItemIndex = d_curTopItem[panelIndex];

	int yPos = 0;
	for(int i = curTopItemIndex; i < curItems.size(); ++i)
	{
		TLStaticText* textItem = curItems[i];
		textItem->setYPosition(Absolute, yPos);
		int height = textItem->getUnclippedPixelRect().getHeight();
		yPos += height;
	}
	
	for(int j = 0; j < curTopItemIndex; ++j)
	{
		TLStaticText* textItem = curItems[j];
		textItem->setYPosition(Absolute, yPos);
		int height = textItem->getUnclippedPixelRect().getHeight();
		yPos += height;
	}

	d_textCarrier[panelIndex]->setHeight(Absolute, yPos);

	freshScrollBarStep(panelIndex);
}

void KUiChannelCentre::delayRecv(int chanId, char* msg)
{
	ChanMsg delayMsg;
	delayMsg.channelId = chanId;
	delayMsg.msg = msg;
	d_delayMsgs.push_back(delayMsg);

	if(d_delayMsgs.size() > 10)
	{
		for(int i = 0; i < d_delayMsgs.size(); ++i)
		{
			dispatchMessage(d_delayMsgs[i].channelId, d_delayMsgs[i].msg.c_str());
		}
		d_delayMsgs.clear();
		d_delayTimeOut = 0;
	}
}

void KUiChannelCentre::recvMessage(int chanId, BYTE* byBuffer )
{
	m_pThisWnd->beginUpdate();

	PCHATROOMMSG_TO_SOMEONE pChatMsg = (PCHATROOMMSG_TO_SOMEONE)byBuffer;	
	
	if(LOCAL_ROOM_ID == chanId)
	{
		//附近频道的消息，显示聊天气泡
		KUiBubbleManager::getSington().showBubble((char*)&pChatMsg->msg, _Bubble::NpcHeadPop ,pChatMsg->senderPlayerId, pChatMsg->gm);
	}

	char tempText[COMMON_CLIENT_MSG_LEN_32];
	
	char* segText = d_msg;

	segText[0] = 0;
	
	strcat(segText, "<Seg f=wrap>");
	//频道
	char chanName[COMMON_CLIENT_MSG_LEN_32];
	memset(chanName, 0, sizeof(chanName));
	KUiChanMgr::getSinglton().getChanNameById(chanId, chanName, sizeof(chanName));

	if(chanName != NULL && chanName[0] != 0)
	{
		strcat(segText, "<Obj t=text v-a=bottom s-d=true color=");
		MapChannelInfo channelInfo;
		if (chanId == MAP_ROOM_ID && KUiChanMgr::getSinglton().getChanInfo(chanId, channelInfo))
		{
			ChangeTextColor(segText, channelInfo.szColor, chanId, pChatMsg->gm);
			strcat(segText, " f-f=");
			ChangeTextFont(segText, channelInfo.szFont, chanId, pChatMsg->gm);
		}
		else
		{
			ChangeTextColor(segText, KUiChanMgr::getSinglton().getChanColor(chanId, pChatMsg->gm), chanId, pChatMsg->gm);
			strcat(segText, " f-f=");
			ChangeTextFont(segText, KUiChanMgr::getSinglton().getChanFont(chanId, pChatMsg->gm), chanId, pChatMsg->gm);
		}
		strcat(segText, " gt=chan id=");
		sprintf(tempText, "%d", chanId);
		strcat(segText, tempText);
		strcat(segText, " d=[");
		strcat(segText, chanName);
		strcat(segText, "]></Obj>");
	}

	const char* curChanName = KUiChanMgr::getSinglton().getChanNameById(chanId);
	const char* shizhuChanName = KUiCfgLoader::getSingleton().getChannelData().shizuChanName;
	const char* zhuhouChanName = KUiCfgLoader::getSingleton().getChannelData().zhuhouChanName;
	const char* leagueChanName = KUiCfgLoader::getSingleton().getChannelData().leagueChanName;
	if(curChanName != NULL && (!strcmp(shizhuChanName, curChanName) || !strcmp(zhuhouChanName, curChanName) || !strcmp( leagueChanName, curChanName ) ))
	{
		//在社会关系频道内聊天 显示官阶
		if(enSULayer_Gens == pChatMsg->unitRank)
		{
			strcat(segText, "<Obj t=text v-a=bottom color=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().zhuzhangColor);
			strcat(segText, " f-f=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().zhuzhangFont);
			strcat(segText, ">[");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().zhuzhang);
			strcat(segText, "]</Obj>");
		}
		else if(enSULayer_Tong == pChatMsg->unitRank)
		{
			strcat(segText, "<Obj t=text v-a=bottom color=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().houzhuColor);
			strcat(segText, " f-f=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().houzhuFont);
			strcat(segText, ">[");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().houzhu);
			strcat(segText, "]</Obj>");
		}
		else if(enSULayer_League == pChatMsg->unitRank)
		{
			strcat(segText, "<Obj t=text v-a=bottom color=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().leagueColor);
			strcat(segText, " f-f=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().leagueFont);
			strcat(segText, ">[");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().leagueLeader);
			strcat(segText, "]</Obj>");
		}
	}

	//名称
	if(pChatMsg->senderName != NULL && pChatMsg->senderName[0] != 0)
	{
		strcat(segText, "<Obj t=text v-a=bottom s-d=true color=");
		if ( pChatMsg->gm == 0 )
		{
			strcat(segText, KUiChanMgr::getSinglton().getPlayerNameColor());
		}
		else
		{
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(SYSTEM_ROOM_ID,false));
		}
		//strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId));
		strcat(segText, " f-f=");
		if ( pChatMsg->gm == 0 )
		{
			strcat(segText, KUiChanMgr::getSinglton().getPlayerNameFont());
		}
		else
		{
			strcat(segText, KUiChanMgr::getSinglton().getChanFont(SYSTEM_ROOM_ID, false));
		}		
		//strcat(segText, KUiChanMgr::getSinglton().getChanFont(chanId));
		strcat(segText, " gt=player");
		sprintf(tempText, " id=%d id1=%d", pChatMsg->senderPlayerId, pChatMsg->camou);
		strcat(segText, tempText);
		strcat(segText, " d=[");
		strcat(segText, pChatMsg->senderName);
		strcat(segText, "]:>");
		strcat(segText, pChatMsg->senderName);
		strcat(segText, "</Obj>");
	}
	
	if(SYSTEM_ROOM_ID != chanId && pChatMsg->gm == 0 )
	{
		if(KUiCfgLoader::getSingleton().getChannelData().personalText[0] != 0 && pChatMsg->senderPlayerId != INVALID_PLAYER_INDEX)
		{
			strcat(segText, "<Obj t=text v-a=bottom s-d=true color=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().personalTextColor);
			strcat(segText, " f-f=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().personalTextFont);
			strcat(segText, " d=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().personalText);
			strcat(segText, "></Obj>");
		}
	}
	else
	{
		if(KUiCfgLoader::getSingleton().getChannelData().officialText[0] != 0)
		{
			strcat(segText, "<Obj t=text v-a=bottom s-d=true color=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().officialTextColor);
			strcat(segText, " f-f=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().officialTextFont);
			strcat(segText, " d=");
			strcat(segText, KUiCfgLoader::getSingleton().getChannelData().officialText);
			strcat(segText, "></Obj>");
		}
	}

	if(pChatMsg->msg[0] == '<')
	{
		chatTextToLoelem((const char*)&pChatMsg->msg, segText, chanId, pChatMsg->gm);
	}
	else	//系统消息可能不带标签
	{
		static char sysMsg[COMMON_CLIENT_MSG_LEN_256];
		sprintf(sysMsg, "<N= %s>", (const char*)&pChatMsg->msg);
		chatTextToLoelem(sysMsg, segText, chanId, pChatMsg->gm);
	}
	
	strcat(segText, "</Seg>");

// 	char* myName = NULL;
// 	g_pCoreShell->GetGameData(GDI_GET_MY_NAME, (unsigned int)&myName, NULL);

	delayRecv(chanId, segText);
	//dispatchMessage(chanId, segText);
}

void KUiChannelCentre::ChangeTextColor(char * segText, const char * color, int chanId, bool bGM)
{
	if (bGM)
	{
		strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGM));
	}
	else
	{
		strcat(segText, color);
	}
}

void KUiChannelCentre::ChangeTextFont(char * segText, const char * font, int chanId, bool bGM)
{
	if (bGM)
	{
		strcat(segText, KUiChanMgr::getSinglton().getChanFont(chanId, bGM));
	}
	else
	{
		strcat(segText, font);
	}
}

void KUiChannelCentre::chatTextToLoelem(const char* text, char* segText, int chanId, bool bGm)
{
	if(NULL == text || NULL == segText)
	{
		return;
	}

	int textLen = strlen(text);

	if(text[0] != '<' || text[textLen - 1] != '>')
	{
		return;
	}

	std::vector<char> formatTexts[COMMON_CLIENT_MSG_LEN_256];
	int index = 0;
	for(int i = 0; i < textLen; ++i)
	{
		if(text[i] == '<')
		{
			continue;
		}
		else if(text[i] == '>')
		{
			formatTexts[index].push_back(0);
			index++;
			continue;
		}
		else if(text[i] == 1)
		{
			formatTexts[index].push_back('<');
		}
		else if(text[i] == 2)
		{
			formatTexts[index].push_back('>');
		}
		else
		{
			formatTexts[index].push_back(text[i]);
		}
	}

	char tempText[COMMON_CLIENT_MSG_LEN_32];
	char msgText[COMMON_CLIENT_MSG_LEN_512];
	msgText[0] = 0;
	for(int j = 0; j < index; ++j)
	{
		memset(msgText, 0, COMMON_CLIENT_MSG_LEN_512);

		char* curText = &formatTexts[j][0];
		if(formatTexts[j][0] == 'I')
		{
			int id[3] = {0, 0, 0};
			int retCode = sscanf(curText, "I=%d|%d|%d|%[^\0]", &id[0], &id[1], &id[2], msgText);
			if(retCode != 4)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom ");
			strcat(segText, " show-des=true gotype=item des=");
			strcat(segText, msgText);
			strcat(segText, " goid=");
			sprintf(tempText, "%d", id[0]);
			strcat(segText, tempText);
			strcat(segText, " goid1=");
			sprintf(tempText, "%d", id[1]);
			strcat(segText, tempText);
			strcat(segText, " goid2=");
			sprintf(tempText, "%d", id[2]);
			strcat(segText, tempText);
			
			ItemType type;
			SpliteHashId(id[0], type.genre, type.detail, type.particular);
			type.level = id[1];
			int color = g_pCoreShell->GetGameData( GDI_GET_ITEM_QUALITY_BY_TYPE, (unsigned int)&type, NULL );

			strcat(segText, " color=");
			strcat(segText, KUiChanMgr::getSinglton().getItemColor(color));
			strcat(segText, " font-family=");
			strcat(segText, KUiChanMgr::getSinglton().getItemFont());
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'N')
		{
			int retCode = sscanf(curText, "N=%[^\0]", msgText);
			if(retCode != 1)
			{
				continue;
			}

			strcat(segText, "<Obj type=text vertical-align=bottom color=");
			MapChannelInfo channelInfo;
			if (KUiChanMgr::getSinglton().getChanInfo(chanId, channelInfo) && chanId == MAP_ROOM_ID)
			{
				ChangeTextColor(segText, channelInfo.szColor, MAP_ROOM_ID, bGm);
				strcat(segText, " font-family=");
				ChangeTextFont(segText, channelInfo.szFont, MAP_ROOM_ID, bGm);
			}
			else
			{
				ChangeTextColor(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGm), chanId, bGm);
				strcat(segText, " font-family=");
				ChangeTextFont(segText, KUiChanMgr::getSinglton().getChanFont(chanId, bGm), chanId, bGm);
			}

			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'F')
		{
			int id;
			int retCode = sscanf(curText, "F=%d", &id);
			if(retCode != 1)
			{
				continue;
			}
			sprintf(tempText, "%d", id);

			strcat(segText, "<Obj type=pic vertical-align=bottom color=");
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGm));

			strcat(segText, " gotype=face goid=");
			
			strcat(segText, " color=");
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGm));

			//根据goid得到内容和描述
			char imagePath[COMMON_CLIENT_MSG_LEN_64] = {0};
			char description[COMMON_CLIENT_MSG_LEN_32] = {0};
			const KUiCfgLoader::FacePanelCfgData& facePanelCfg  = KUiCfgLoader::getSingleton().getFaceData();
			for(int i = 0; i < facePanelCfg.faceList.size(); ++i)
			{
				if(facePanelCfg.faceList[i].index == id)
				{
					strcpy(imagePath, facePanelCfg.faceList[i].image);
					strcpy(description, facePanelCfg.faceList[i].description);
					break;
				}
			}
			strcat(segText, " des=");
			strcat(segText, description);
			strcat(segText, ">");
			strcat(segText, imagePath);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'P')
		{
			int id[4] = {0, 0, 0,0};
			int retCode = sscanf(curText, "P=%d|%d|%d|%d|%[^\0]", &id[0], &id[1], &id[2], &id[3], msgText);
			if(retCode != 5)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom color=");
			strcat(segText, KUiChanMgr::getSinglton().getChanColor(chanId, bGm));

			strcat(segText, " show-des=true gotype=pos des=");
			strcat(segText, msgText);
			strcat(segText, " goid=");
			sprintf(tempText, "%d", id[0]);
			strcat(segText, tempText);
			strcat(segText, " goid1=");
			sprintf(tempText, "%d", id[1]);
			strcat(segText, tempText);
			strcat(segText, " goid2=");
			sprintf(tempText, "%d", id[2]);
			strcat(segText, tempText);

			strcat(segText, " goid3=");
			sprintf(tempText, "%d", id[3]);
			strcat(segText, tempText);
			
			strcat(segText, " color=");
			strcat(segText, KUiCfgLoader::getSingleton().getPosLinkCfg().color);
			strcat(segText, " font-family=");
			strcat(segText, KUiCfgLoader::getSingleton().getPosLinkCfg().font);
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else
		{
			continue;
		}
	}
}

void KUiChannelCentre::playCozeSound()
{
	playSound(KUiCfgLoader::getSingleton().getSoundEffectCfg().recvNewCoze);
}

void KUiChannelCentre::recvCozeMessage(BYTE* byBuffer )
{
	PCHATMSG_BY_NAME cozeMsg = (PCHATMSG_BY_NAME)byBuffer;
		
	char tempText[COMMON_CLIENT_MSG_LEN_128];

	char* segText = d_msg;

	segText[0] = 0;
	strcat(segText, "<Seg float=wrap>");
	//名称
	if(cozeMsg->name != NULL && cozeMsg->name[0] != 0)
	{
		strcat(segText, "<Obj type=text vertical-align=bottom show-des=true color=");
		strcat(segText, KUiChanMgr::getSinglton().getChanColor(COSE_ROOM_ID, false));
		strcat(segText, " font-family=");
		strcat(segText, KUiChanMgr::getSinglton().getPlayerNameFont());
		strcat(segText, " gotype=player");
		sprintf(tempText, " id=%d id1=%d", cozeMsg->npcId, cozeMsg->camou);
		strcat(segText, tempText);
		strcat(segText, " des=");

		if(cozeMsg->isRecive)
		{
			sprintf(tempText, KUiCfgLoader::getSingleton().getChannelData().reciveText, cozeMsg->name);
			strcat(segText, tempText);
			KUiChatInputWnd::GetSingleton().setLastSender(cozeMsg->name);
			playCozeSound();
		}
		else
		{
			sprintf(tempText, KUiCfgLoader::getSingleton().getChannelData().sayText, cozeMsg->name);
			strcat(segText, tempText);
			KUiChatInputWnd::GetSingleton().setLastReciver(cozeMsg->name);
		}
		strcat(segText, ">");
		strcat(segText, cozeMsg->name);
		strcat(segText, "</Obj>");
	}
	chatTextToLoelem((const char*)&cozeMsg->msg, segText, COSE_ROOM_ID, false);
	
	strcat(segText, "</Seg>");
	dispatchMessage(COSE_ROOM_ID, segText);
}

void KUiChannelCentre::recvCustomMessage(int channelId, vector<CommonChatData>& messageData)
{
// 	KUiCfgLoader::getSingleton().getChatCfg();
// 	list<LOElemInfo> elemInfo(messageData.size());
// 
// 	for(int i = 0; i < messageData.size(); ++i)
// 	{
// 		LOElemInfo chanElem;
// 		chanElem.elemType = LO_TEXT;
// 		if(chanElem.gameObj._objType == LO_GO_NOTHING)
// 		{
// 			chanElem.content = messageData[i].msg;
// 			chanElem.isShowDes = false;
// 		}
// 		else
// 		{
// 			chanElem.description = messageData[i].msg;
// 			chanElem.isShowDes = true;
// 		}
// 		chanElem.gameObj = messageData[i].gameObj;
// 		chanElem._color = ;
// 		chanElem.isShowDes = true;
// 		elemInfo.push_front(chanElem);
// 	}
// 	char customMessage[COMMON_CLIENT_MSG_LEN_512];
// 
// 	int len = strlen(message);
// 	strncpy(customMessage, message, len < COMMON_CLIENT_MSG_LEN_512 ? len : COMMON_CLIENT_MSG_LEN_512);
// 	sprintf(customMessage, "<T=\n%s>", message);
// 	list<LOElemInfo> elemInfo;
// 	chatTextToLoelem(customMessage, elemInfo);
// 
// 	const char* chanName = KUiChanMgr::getSinglton().getChanName(channelId);
// 	if(chanName != NULL && chanName[0] != 0)
// 	{
// 		wchar_t* chanNameUnicode = NULL;
// 		
// 		ansiToUnicode(chanName, chanNameUnicode);
// 		int textLen = wcslen(chanNameUnicode) + 5;
// 		wchar_t* formatChanName = new wchar_t[textLen + 1];
// 		formatChanName[0] = 0;
// 		wcscat(formatChanName, L"[");
// 		wcscat(formatChanName, chanNameUnicode);
// 		wcscat(formatChanName, L"]: ");
// 		delete[] chanNameUnicode;
// 		chanNameUnicode = NULL;
// 		
// 		LOElemInfo chanElem;
// 		chanElem.elemType = LO_TEXT;
// 		chanElem.description = formatChanName;
// 		chanElem.gameObj._objType = LO_GO_CHANNEL;
// 		chanElem.gameObj._objId[0] = channelId;
// 		chanElem._color = KUiChanMgr::getSinglton().getCurColor(channelId);
// 		chanElem.isShowDes = true;
// 		elemInfo.push_front(chanElem);
// 	}
// 	dispatchMessage(channelId, elemInfo);
// 
// 	clearElemsText(elemInfo);
}

void KUiChannelCentre::recvCustomMessage(int channelId, char* message)
{
	m_pThisWnd->beginUpdate();

	dispatchMessage(channelId, message);
}

void 
KUiChannelCentre::recvCustomMessageConst( int channelId, const char* message )
{
	m_pThisWnd->beginUpdate();
	
	dispatchMessage( channelId, message );
}

void KUiChannelCentre::dispatchMessage(int chanId, const char* segText)
{
	for(int i = 0; i < FrameNum; ++i)
	{
		vector<int>& chanIdsOfCurPanel = d_frameInfo[i]._chanList;
		for(int j = 0; j < chanIdsOfCurPanel.size(); ++j)
		{
			//如果当前面版订阅了该频道则向该面版中加入这条频道的新消息
			if(chanIdsOfCurPanel[j] == chanId)
			{
				addAMessage(i, segText);
				relayoutText(i);
				adjustLayoutPos(i);
				showNewMsg(i);
			}
		}
	}
}

void KUiChannelCentre::toSysMsg(const char* message)
{
	if(NULL == message)
	{
		return;
	}

	ChatMainDlg::InsertSystemMsg(message);

	char buf[sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG];
	PCHATROOMMSG_TO_SOMEONE	msgData = (PCHATROOMMSG_TO_SOMEONE)buf;

	int msgLen = strlen(message);

	msgData->senderName[0] = 0;
	if(msgLen > MAXSIZE_CHAT_MSG)
	{
		msgLen = MAXSIZE_CHAT_MSG;
	}
	msgData->msgLen = msgLen;
	
	memcpy(msgData->msg, message, msgLen);
	msgData->msg[msgLen] = '\0';
	recvMessage(SYSTEM_ROOM_ID, (BYTE*)buf);
}

void KUiChannelCentre::addAMessage(int panelIndex, const char* segText)
{
	TLStaticText* textCarrier = d_textCarrier[panelIndex];
	
	vector<TLStaticText*>& curItems = d_textItems[panelIndex];

	//先决定要用哪个item去显示当前的文字
	TLStaticText* last = curItems[curItems.size() - 1];
	bool textFull =  last->getUnclippedPixelRect().getHeight() > 0 ? true : false;
	
	int canUseItemIndex = 0;
	if(textFull)
	{
		canUseItemIndex = d_curTopItem[panelIndex];
	}
	else
	{
		for(int i = 0; i < curItems.size(); ++i)
		{
			TLStaticText* thisItem = curItems[i];
			bool canUse = thisItem->getUnclippedPixelRect().getHeight() > 0 ? false : true;
			if(canUse)
			{
				canUseItemIndex = i;
				break;
			}
		}
	}
	
	static char cacheMsg[LAYOUT_TEXT_MAX_LEN + 1];
	sprintf(cacheMsg, "<Layout width=%d height=600>%s</Layout>", d_wndWidth[panelIndex], segText);
	
	TLStaticText* flashItem = curItems[canUseItemIndex];
	flashItem->getLayout()->SetText(cacheMsg);
	flashItem->getLayout()->flashLayout();
	flashItem->fitLayoutSize(false);

	//文字内容还没装满
	if(!textFull) //&& canUseItemIndex != curItems.size() - 1)
	{
		return;
	}

	//更新topitem
	++d_curTopItem[panelIndex];
	if(d_curTopItem[panelIndex] == curItems.size())
	{
		d_curTopItem[panelIndex] = 0;
	}
}

void KUiChannelCentre::showNewMsg(int frameIndex)
{	
	if(!validateFrameIndex(frameIndex))
	{
		return;
	}
	//当前面板不显示新消息
	if(d_frameContent[frameIndex]->isVisible() && d_scrollBar[frameIndex]->getScrollPosition() == 1.0f)
	{
		return;
	}

	//系统页面单拎出来了，总是显示的
	if(frameIndex == d_systemFrameIndex)
	{
		return;
	}
	
	d_newMsgImg[frameIndex]->show();
	d_newMsgImg[frameIndex]->setCycCount(1);
	d_newMsgImg[frameIndex]->play(0, 49);
}

bool KUiChannelCentre::onTextPanelWheelChanged(const EventArgs& args)
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	int frameIndex = UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX;
	for(int i = 0; i < FrameNum; ++i)
	{
		if(d_framePanel[i] == eventArgs->window)
		{
			frameIndex = i;
			break;
		}
	}

	if(UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX == frameIndex)
	{
		return false;
	}

	if(d_scrollBar[frameIndex]->isVisible())
	{
		d_scrollBar[frameIndex]->setScrollPosition(d_scrollBar[frameIndex]->getScrollPosition()
			- d_scrollBar[frameIndex]->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiChannelCentre::scroll(const EventArgs& args)
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	for(int i = 0; i < FrameNum; ++i)
	{
		if(scrollCtrl->window != d_scrollBar[i])
			continue;
		
		adjustLayoutPos(i);
	}
	return true;
}

bool KUiChannelCentre::clickToTop(const EventArgs& args)
{
	WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
	for(int i = 0; i < FrameNum; ++i)
	{
		if(eventArgs->window != d_toTopBtn[i])
			continue;
		
		d_scrollBar[i]->setScrollPosition(0.0f);
		adjustLayoutPos(i);
	}

	if(KUiChatInputWnd::IsVisible())
	{
		KUiChatInputWnd::GetSingleton().show();
	}
	return true;
}

bool KUiChannelCentre::clickToButtom(const EventArgs& args)
{
	WindowEventArgs* eventArgs = (WindowEventArgs*)&args;
	for(int i = 0; i < FrameNum; ++i)
	{
		if(eventArgs->window != d_toBottomBtn[i])
			continue;
		
		d_scrollBar[i]->setScrollPosition(1.0f);
		adjustLayoutPos(i);
	}

	if(KUiChatInputWnd::IsVisible())
	{
		KUiChatInputWnd::GetSingleton().show();
	}
	return true;
}

void KUiChannelCentre::adjustWindowHeight(int height)
{
	if(height > d_maxHeight)
	{
		height = d_maxHeight;
	}
	if(height < d_minHeight)
	{
		height = d_minHeight;
	}

	int oldheight = m_pThisWnd->getAbsoluteHeight();
	if(oldheight == height)
	{
		return;
	}

	m_pThisWnd->setYPosition(Absolute, m_pThisWnd->getYPosition(Absolute) + oldheight - height);
	m_pThisWnd->setHeight(Absolute, height);
	for(int i = 0; i < FrameNum; ++i)
	{
		if(i == d_systemFrameIndex)
		{
			continue;
		}
		static int frameBtnHeight = d_frameBtn[i]->getAbsoluteHeight();
		d_frameContent[i]->setHeight(height - frameBtnHeight - d_frameContent[i]->getPosition(Absolute).d_y);
		d_frameBtn[i]->setYPosition(Absolute, height - frameBtnHeight);
//		d_newMsgImg[i]->setYPosition(Absolute, height - frameBtnHeight);

		d_toBottomBtn[i]->setYPosition(Absolute, height - frameBtnHeight - d_toBottomBtn[i]->getAbsoluteHeight());

		d_scrollBar[i]->setHeight(Absolute, d_toBottomBtn[i]->getYPosition(Absolute) - d_scrollBar[i]->getYPosition(Absolute));

		//重新设置排版裁减区域
		d_frameContent[i]->setMetricsMode(Absolute);		
		Rect textArea = d_frameContent[i]->getUnclippedPixelRect();

		textArea.d_left		+= d_frameContent[i]->getLeftFrameWidth();
		textArea.d_right	-= d_frameContent[i]->getRightFrameWidth();
		textArea.d_top		+= d_frameContent[i]->getTopFrameHeight();
		textArea.d_bottom	-= d_frameContent[i]->getBottomFrameHeight();
		
		//裁剪区域必须是相对底板的位置
 		Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 		textArea.setPosition(posOff);

		LORect clipper;
		cerectToLorect(&textArea, &clipper);

		for(int j = 0; j < d_textItems[i].size(); ++j)
		{
			TLStaticText* aCentenceCtrl = d_textItems[i][j];
			aCentenceCtrl->getLayout()->setClipper(clipper);
		}

		adjustLayoutPos(i);
		freshScrollBarStep(i);
	}
}

bool KUiChannelCentre::onMoveBtnDown(const EventArgs& args)
{
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	
	if ( pArgs && pArgs->button == LeftButton )
		d_resizing = true;
	
	return true;
}

bool KUiChannelCentre::onMoveBtnUp(const EventArgs& args)
{	
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	
	if ( pArgs && pArgs->button == LeftButton )
		d_resizing = false;

	return true;
}

bool KUiChannelCentre::onMoveBtnMove(const EventArgs& args)
{
	if(false == d_resizing)
		return true;

	Point mousePos = MouseCursor::getSingleton().getPosition();
	
	int yPos = m_pThisWnd->getYPosition(Absolute);
	int oldHeight = m_pThisWnd->getAbsoluteHeight();
	int newHeight = oldHeight + (yPos - mousePos.d_y);
	adjustWindowHeight(newHeight);

	return true;
}

void KUiChannelCentre::freshScrollBarStep(int panelIndex)
{
	if(panelIndex < 0 || panelIndex >= FrameNum)
	{
		_ASSERT(0);
		return;
	}
	TLStaticText* thisTextCtrl = d_textCarrier[panelIndex];
	TLStaticText* thisClipperCtrl = d_frameContent[panelIndex];
	if(NULL == thisTextCtrl)
	{
		_ASSERT(0);
		return;
	}
	
	int textHeight = thisTextCtrl->getAbsoluteHeight();
	int clipperHeight = thisClipperCtrl->getHeight(Absolute) 
		- thisClipperCtrl->getTopFrameHeight() - thisClipperCtrl->getBottomFrameHeight();
	
	if(textHeight <= clipperHeight)
	{
		d_scrollBar[panelIndex]->setStepSize(1.0f);
	}
	else
	{
		//float step = (float)clipperHeight / (2 * (layoutHeight - clipperHeight));
		float step = (float)17 / (textHeight - clipperHeight);
		d_scrollBar[panelIndex]->setStepSize(step);
	}
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiChatInputWnd* KUiWndSingleton<KUiChatInputWnd>::ms_Singleton	= NULL;

KUiChatInputWnd::KUiChatInputWnd( const String& id_name ):
KUiWndSingleton<KUiChatInputWnd>( id_name )
{
	d_lbdown = false;
}

KUiChatInputWnd::~KUiChatInputWnd()
{

}

void KUiChatInputWnd::show( void )
{
	KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
	if ( pRoom && pRoom->IsVisible() )
	{
		return;
	}
	KUiWndSingleton<KUiChatInputWnd>::Show();

	Window* parent = m_pThisWnd->getParent();
	for(int i = 0; i < parent->getChildCount(); ++i)
	{
		Window* brother = parent->getChildAtIdx(i);
		brother->deactivate();
	}

	d_inputBox->activate();
	m_pThisWnd->activate();
	d_inputBox->show();
	d_inputBox->showCaratImm();
}

void KUiChatInputWnd::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->getChild();
	}
}

void KUiChatInputWnd::getChild()
{
	m_pThisWnd->hide();
	m_pThisWnd->setRenderMode( false, 3 );
	m_pThisWnd->setZLevel(Window::SuperBottom);
	m_pThisWnd->SetBottomWindow();
	//频道选择
	d_chanPopBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/Bugle");
	d_chanPopBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiChatInputWnd::clickChanPopBtn, this));
	d_chanPopBtn->subscribeEvent(PushButton::EventMouseDoubleClick, Event::Subscriber(&KUiChatInputWnd::clickChanPopBtn, this));
	//把频道输入框放到跟窗口下，单独作为一个窗口绘制
	d_chanSelMenu = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/ChannelMenu");
	m_pThisWnd->removeChildWindow(d_chanSelMenu);
//	d_chanSelMenu->setZLevel(Window::Top);
	d_chanSelMenu->setHeight(Absolute, 200);
	d_chanSelMenu->setRenderMode(false);
	d_chanSelMenu->setZLevel(Window::Bottom);
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_chanSelMenu);

	d_chanSelMenu->hide();

	TLButton* chanSelBtnTemp = (TLButton*)d_chanSelMenu->getChild("TaharezLook/ChannelCentre/InputWnd/ChannelMenu/ChanSelBtnTemplate");
	TLButton* chanCtrlBtnTemp = (TLButton*)d_chanSelMenu->getChild("TaharezLook/ChannelCentre/InputWnd/ChannelMenu/ChanCtrlBtnTemplate");
	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		d_chanSelBtn[i] = (TLButton*)WindowManager::getSingleton().createWindow("TaharezLook/Button");
		useTemplate(d_chanSelBtn[i], chanSelBtnTemp);
		
		d_chanSelMenu->addChildWindow(d_chanSelBtn[i]);
		d_chanSelBtn[i]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatInputWnd::clickChanBtn, this));
		d_chanSelBtn[i]->hide();

		d_chanCtrlBtn[i] = (TLButton*)WindowManager::getSingleton().createWindow("TaharezLook/Button");
		useTemplate(d_chanCtrlBtn[i], chanCtrlBtnTemp);

		d_chanSelMenu->addChildWindow(d_chanCtrlBtn[i]);
		d_chanCtrlBtn[i]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatInputWnd::clickChanCtrlBtn, this));
		d_chanCtrlBtn[i]->hide();

		d_chanId[i] = UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
	}
	d_chanBtnYOff = chanSelBtnTemp->getAbsolutePosition().d_y;

	//最近密语面板
	d_latestChatMenu = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/LatestChatMenu");
	m_pThisWnd->removeChildWindow(d_latestChatMenu);
	d_latestChatMenu->setZLevel(Window::Top);
	d_latestChatMenu->setHeight(Absolute, 200);
	d_latestChatMenu->setRenderMode(false);
	d_latestChatMenu->hide();
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_latestChatMenu);

	TLButton* btnTemp = (TLButton*)d_latestChatMenu->getChild("TaharezLook/ChannelCentre/InputWnd/LatestChatMenu/BtnTemplate");
	for(int j = 0; j < UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME; ++j)
	{
		d_latestChatBtn[j] = (TLButton*)WindowManager::getSingleton().createWindow("TaharezLook/Button");
		useTemplate(d_latestChatBtn[j], btnTemp);
		
		d_latestChatMenu->addChildWindow(d_latestChatBtn[j]);
		d_latestChatBtn[j]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatInputWnd::clickLatestBtn, this));
		d_latestChatBtn[j]->hide();
	}


	//输入框
	d_inputBox = (TLEditbox*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/Input");
	//输入与快捷键事件绑定
	d_inputBox->subscribeEvent(TLEditbox::EventShown, Event::Subscriber(&KUiChatInputWnd::onInputBoxShow, this));
	d_inputBox->subscribeEvent(TLEditbox::EventHidden, Event::Subscriber(&KUiChatInputWnd::onInputBoxClose, this));
	d_inputBox->subscribeEvent(TLEditbox::EventKeyDown, Event::Subscriber(&KUiChatInputWnd::handleKeyDown, this));
	d_inputBox->subscribeEvent(TLEditbox::EventCharacterKey, Event::Subscriber(&KUiChatInputWnd::handleKeyInput, this));
	//鼠标挪动出现选中区域的效果
	d_inputBox->subscribeEvent(Editbox::EventMouseButtonDown, Event::Subscriber(&KUiChatInputWnd::handleLBDown, this));
	d_inputBox->subscribeEvent(Editbox::EventMouseButtonUp, Event::Subscriber(&KUiChatInputWnd::handleLBUp, this));
	d_inputBox->subscribeEvent(Editbox::EventMouseMove, Event::Subscriber(&KUiChatInputWnd::handleMouseMove, this));
	//输入框排版
	d_inputBox->useLayout();
	d_inputBox->setLayoutOffset(0, 0);

	//表情面板按钮
	d_faceBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/face");
	d_faceBtn->subscribeEvent(PushButton::EventMouseDoubleClick, Event::Subscriber(&KUiChatInputWnd::handleFaceBtnDown, this));
	d_faceBtn->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatInputWnd::handleFaceBtnDown, this));

	//表情面板
	d_facePanel = (TLStaticText*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/facepanel");
	const KUiCfgLoader::FacePanelCfgData& facePanelCfg = KUiCfgLoader::getSingleton().getFaceData();

	Point facePos(d_facePanel->getLeftFrameWidth(), d_facePanel->getTopFrameHeight());
	for(int k = 0; k < facePanelCfg.faceList.size(); ++k)
	{
		const char* faceName = facePanelCfg.faceList[k].image;
		TLStaticImage* faceImage = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", 
			String("FaceImage") + iToString(k));
		
		faceImage->setBackgroundEnabled(false);
		faceImage->setFrameEnabled(false);
		const Image* image = getImage(faceName);
		if(NULL == image)
		{
			continue;
		}
		faceImage->setImage(image);

		d_facePanel->addChildWindow(faceImage);

		faceImage->setPosition(Absolute, facePos);
		faceImage->setWidth(Absolute, image->getWidth());
		faceImage->setHeight(Absolute, image->getHeight());
		facePos.d_x += image->getWidth();
		
		if(facePos.d_x > facePanelCfg.wndWidth)
		{
			facePos.d_y += image->getHeight();
			facePos.d_x = d_facePanel->getLeftFrameWidth();
		}
		
		if(d_facePanel->getWidth(Absolute) < facePos.d_x + image->getWidth())
		{
			d_facePanel->setWidth(Absolute, facePos.d_x + image->getWidth());
		}
		d_facePanel->setHeight(Absolute, facePos.d_y + image->getHeight() + d_facePanel->getBottomFrameHeight());

		faceImage->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatInputWnd::handleSelectAFace, this));
		faceImage->subscribeEvent(PushButton::EventMouseEnters, Event::Subscriber(&KUiChatInputWnd::handleFaceMouseIn, this));
		faceImage->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiChatInputWnd::handleFaceMouseOut, this));

		faceImage->setUserString("goid", iToString(facePanelCfg.faceList[k].index));
		faceImage->play();
		faceImage->setCyc(true);
	}
	d_facePanelSelectFrameImage = (TLStaticImage*)d_facePanel->getChild("TaharezLook/ChannelCentre/InputWnd/facepanel/SelectFrameImage");
	d_facePanelSelectFrameImage->setBackgroundEnabled(false);
	d_facePanelSelectFrameImage->setFrameEnabled(true);
	d_facePanel->removeChildWindow(d_facePanelSelectFrameImage);
	d_facePanel->addChildWindow(d_facePanelSelectFrameImage);
	
	d_facePanelSelectFrameImage->disable();
    d_facePanelSelectFrameImage->hide();
	d_facePanelSelectFrameImage->setZLevel(Window::Top);
	//表情面版排版
	d_facePanel->hide();
	m_pThisWnd->removeChildWindow(d_facePanel);
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_facePanel);
	d_facePanel->setRenderMode(true);

	Point facePanelAbPos;
	facePanelAbPos = d_faceBtn->getUnclippedPixelRect().getPosition();
	facePanelAbPos.d_y -= d_facePanel->getAbsoluteHeight() + 10;//上移10个像素否则不太好看
	d_facePanel->setPosition(Absolute, facePanelAbPos);

	//发送按钮
	d_sendBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/ChannelCentre/InputWnd/Send");
	d_sendBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiChatInputWnd::handleSend, this));
	
	//窗口关闭
	m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiChatInputWnd::onHide, this));
	clearText();
	d_curCacheIndex		= 0;

	//默认窗口关闭
	m_pThisWnd->hide();

	//初始化某些变量
	d_lastSender[0]		= 0;
	d_lastReciver[0]	= 0;
	
	d_justOpenFromKey = false;
}

bool KUiChatInputWnd::onInputBoxShow(const EventArgs& args)
{
	d_inputBox->beginUpdate();
	return true;
}

bool KUiChatInputWnd::onInputBoxClose(const EventArgs& args)
{
	d_inputBox->stopUpdate();
	return true;
}

void KUiChatInputWnd::useTemplate(TLButton* wnd, TLButton* templateWnd)
{
	wnd->setHeight(Absolute, templateWnd->getHeight(Absolute));
	wnd->setWidth(Absolute, templateWnd->getWidth(Absolute));
	wnd->setXPosition(Absolute, templateWnd->getXPosition(Absolute));
	wnd->setNormalImage(templateWnd->getNormalImage());
	wnd->setHoverImage(templateWnd->getHoverImage());
	wnd->setPushedImage(templateWnd->getPushedImage());
	wnd->setDisabledImage(templateWnd->getDisabledImage());
	wnd->setFont(templateWnd->getFont());
	wnd->setNormalTextColour(templateWnd->getNormalTextColour());
	wnd->setDisabledTextColour(templateWnd->getDisabledTextColour());
	wnd->setHoverTextColour(templateWnd->getHoverTextColour());
	wnd->setPushedTextColour(templateWnd->getPushedTextColour());

}

void KUiChatInputWnd::registChannel(int chanId, char* chanName)
{
	const char* channelColor = KUiChanMgr::getSinglton().getChanColor(chanId, false);
	float red, green, bule;
	bool colorValid = false;
	colour chanColor;

	if(sscanf(channelColor, "%f,%f,%f", &red, &green, &bule) == 3)
	{
		colorValid = true;
		chanColor = colour(red / 255, green / 255, bule / 255, 1);
	}

	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(UI_CHAT_WINDOW_INVALIDATE_CHAN_ID != d_chanId[i])
		{
			continue;
		}
		d_chanId[i] = chanId;
		
		if(chanName != NULL)
		{
			d_chanSelBtn[i]->setText(AnsiToUtf8(chanName));
			
			if(colorValid)
			{
				d_chanSelBtn[i]->setNormalTextColour(chanColor);
				d_chanSelBtn[i]->setPushedTextColour(chanColor);
				d_chanSelBtn[i]->setHoverTextColour(chanColor);
			}

			d_chanCtrlBtn[i]->setText(AnsiToUtf8(MSG_OPEN));
			d_chanCtrlBtn[i]->setNormalTextColour(colour(0, 1, 0, 1));
			d_chanCtrlBtn[i]->setPushedTextColour(colour(0, 1, 1, 1));
			d_chanCtrlBtn[i]->setHoverTextColour(colour(0, 1, 0.5, 1));
		}
		break;
	}
	
	if(chanId == LOCAL_ROOM_ID)
	{
		d_chanPopBtn->setText(AnsiToUtf8(chanName));
		if(colorValid)
		{
			d_chanPopBtn->setNormalTextColour(chanColor);
			d_chanPopBtn->setPushedTextColour(chanColor);
			d_chanPopBtn->setHoverTextColour(chanColor);
		}
	}
	layoutChanMenu();
}

void KUiChatInputWnd::unregistChannel(int chanId)
{
	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(chanId == d_chanId[i])
		{
			d_chanId[i] = UI_CHAT_WINDOW_INVALIDATE_CHAN_ID;
			break;
		}
	}

	if(chanId == KUiChanMgr::getSinglton().getInputChanId())
	{
		for(int j = 0; j < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++j)
		{
			if(UI_CHAT_WINDOW_INVALIDATE_CHAN_ID != d_chanId[j])
			{
				switchChannel(d_chanId[j]);
			}
		}
	}
	layoutChanMenu();
}

void KUiChatInputWnd::layoutChanMenu()
{
	int yPos = d_chanBtnYOff;
	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(d_chanId[i] != UI_CHAT_WINDOW_INVALIDATE_CHAN_ID)
		{
			d_chanSelBtn[i]->setYPosition(Absolute, yPos);
			d_chanSelBtn[i]->show();

			d_chanCtrlBtn[i]->setYPosition(Absolute, yPos);
			d_chanCtrlBtn[i]->show();

			yPos += d_chanSelBtn[i]->getAbsoluteHeight();
		}
		else
		{
			d_chanSelBtn[i]->hide();
			d_chanCtrlBtn[i]->hide();
		}
	}

	yPos += d_chanBtnYOff;
	d_chanSelMenu->setHeight(Absolute, yPos);

	yPos = 0;
	for(int j = 0; j < UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME; ++j)
	{
		if(d_latestChatBtn[j]->getText().empty())
		{
			d_latestChatBtn[j]->hide();
			continue;
		}
		d_latestChatBtn[j]->setYPosition(Absolute, yPos);

		d_latestChatBtn[j]->show();
		yPos += d_chanSelBtn[j]->getAbsoluteHeight();
	}
	d_latestChatMenu->setHeight(Absolute, yPos);
}

bool KUiChatInputWnd::clickChanPopBtn(const EventArgs& args)
{
	if(d_chanSelMenu->isVisible() == false)
	{
		Point leftTopPos = d_chanPopBtn->getUnclippedPixelRect().getPosition();
		//leftTopPos.d_x -= d_chanSelMenu->getAbsoluteWidth();
		leftTopPos.d_y -= d_chanSelMenu->getAbsoluteHeight() + 2;

		Window* root = d_chanSelMenu->getParent();

		root->removeChildWindow(d_chanSelMenu);
		root->addChildWindow(d_chanSelMenu);
		d_chanSelMenu->setPosition(Absolute, leftTopPos);
		d_chanSelMenu->show();

		leftTopPos.d_y -= d_latestChatMenu->getAbsoluteHeight();
		root->removeChildWindow(d_latestChatMenu);
		root->addChildWindow(d_latestChatMenu);
		d_latestChatMenu->setPosition(Absolute, leftTopPos);
		d_latestChatMenu->show();

		freshChanName();
	}
	else
	{
		d_chanSelMenu->hide();
		d_latestChatMenu->hide();
	}
	return true;
}

bool KUiChatInputWnd::clickChanBtn(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLButton* chanBtn = (TLButton*)mouse->window;

	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(d_chanSelBtn[i] == chanBtn && d_chanId[i] != COSE_ROOM_ID && d_chanId[i] != COMBAT_INFO_ROOM_ID)
		{
			switchChannel(d_chanId[i]);
			break;
		}
	}
	
	d_chanSelMenu->hide();
	d_latestChatMenu->hide();
	d_inputBox->activate();

	return true;
}

bool KUiChatInputWnd::clickLatestBtn(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLButton* chanBtn = (TLButton*)mouse->window;
	
	clearText();

	const vector<string>& reciverNameList = KUiChanMgr::getSinglton().getLatestReciver();
	const char * name = NULL;
	for (int i = 0; i < reciverNameList.size() && i < UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME; ++i)
	{
		if (chanBtn == d_latestChatBtn[i])
		{
			name = reciverNameList[i].c_str();
		}
	}
	
	if (name != NULL)
	{
		
		write("/");
		write(name);
		write(" ");
	}

	d_latestChatMenu->hide();
	d_chanSelMenu->hide();
	d_inputBox->activate();

	return true;
}

void KUiChatInputWnd::recordReciver(char* lastReciver)
{
	KUiChanMgr::getSinglton().addReciverName(lastReciver);

	const vector<string>& reciverNameList = KUiChanMgr::getSinglton().getLatestReciver();
	for(int i = 0; i < reciverNameList.size() && i < UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME; ++i)
	{
		string name = reciverNameList[i];
		char chanName[COMMON_CLIENT_MSG_LEN_16] = {0};
		if (strcmp(name.c_str(), "") != 0)
		{
			sprintf(chanName, "%s%d", KUiCfgLoader::getSingleton().getChannelData().cozeChanName, i + 1);

			char playerName[COMMON_CLIENT_MSG_LEN_256] = {0};
			char layoutWidth[COMMON_CLIENT_MSG_LEN_16] = {0};
			itoa((strlen(name.c_str()) * 6 + 12), layoutWidth, 10);
			strcat(playerName, "<Layout width=");
			strcat(playerName, layoutWidth);
			strcat(playerName, ">");
			strcat(playerName, "<Seg text-align=center>");
			strcat(playerName, "<Obj type=text");
			strcat(playerName, " color=255,255,255");
			strcat(playerName, " vertical-align=center");
			strcat(playerName, " font-family=stzhongs-9>");
			strcat(playerName, name.c_str());
			strcat(playerName, "</Obj></Seg></Layout>");
			d_latestChatBtn[i]->setTooltipText(AnsiToUtf8(playerName));
		}

		d_latestChatBtn[i]->setText(AnsiToUtf8(chanName));
	}

	layoutChanMenu();
}

void KUiChatInputWnd::clearLatestChatMenu()
{
	KUiChanMgr::getSinglton().clearLatestReciverList();
	for(int i = 0; i < UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME; ++i)
	{
		d_latestChatBtn[i]->setText("");
	}

	layoutChanMenu();

	d_lastReciver[0] = 0;
	d_lastSender[0] = 0;
}

bool KUiChatInputWnd::clickChanCtrlBtn(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLButton* ctrl = (TLButton*)mouse->window;

	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(d_chanCtrlBtn[i] != ctrl)
		{
			continue;
		}

		if(d_chanCtrlBtn[i]->getText() == String(AnsiToUtf8(MSG_OPEN)))
		{
			d_chanCtrlBtn[i]->setText(AnsiToUtf8(MSG_CLOSE));
			d_chanCtrlBtn[i]->setNormalTextColour(colour(1, 0, 0, 1));
			d_chanCtrlBtn[i]->setPushedTextColour(colour(1, 0, 1, 1));
			d_chanCtrlBtn[i]->setHoverTextColour(colour(1, 0, 0.5f, 1));
			KUiChannelCentre::GetSingleton().unregistChannel(d_chanId[i]); 
			B2ChatDialog::chatManager.ChatManagerChannelCloseOne(d_chanId[i]);
		}
		else
		{
			d_chanCtrlBtn[i]->setNormalTextColour(colour(0, 1, 0, 1));
			d_chanCtrlBtn[i]->setPushedTextColour(colour(0, 1, 1, 1));
			d_chanCtrlBtn[i]->setHoverTextColour(colour(0, 1, 0.5f, 1));
			d_chanCtrlBtn[i]->setText(AnsiToUtf8(MSG_OPEN));
			
			const KUiCfgLoader::ChannelCfgData& chanCfg = KUiCfgLoader::getSingleton().getChannelData();
			B2ChatDialog::chatManager.ChatManagerChannelEnable(d_chanId[i]);
			//查找到定制了该频道的页面，然后向找到的页面注册该频道
			for(int j = 0; j < chanCfg.framesCfg.size(); ++j)
			{
				const KUiCfgLoader::ChatFrameCfg& frameCfg = chanCfg.framesCfg[j];
				for(int k = 0; k < frameCfg.channelName.size(); ++k)
				{
					const string& chanRequired = frameCfg.channelName[k];
					if(chanRequired == string(Utf8ToAnsi(d_chanSelBtn[i]->getText())))
					{
						KUiChannelCentre::GetSingleton().registChannel(j, d_chanId[i]);
					}
				}
			}
		}
		break;
	}
	
	d_inputBox->activate();

	return true;
}

void KUiChatInputWnd::clearText( void )
{
	d_inputBox->getLayout()->clearLayout();
	d_inputBox->getLayout()->SetText("<Layout width=2500 height=25><Seg text-align=left float=right></Seg></Layout>");

	//裁剪区域必须是相对底板的位置
	Rect textArea = d_inputBox->getUnclippedPixelRect();
	Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	clipper.setPos(clipper.getLeft(), clipper.getTop() - UI_CHAT_WINDOW_INPUT_LAYOUT_CLIPPER_EXTEND);
	clipper.setHeight(clipper.getHeight() + 100 + UI_CHAT_WINDOW_INPUT_LAYOUT_CLIPPER_EXTEND);
	d_inputBox->getLayout()->setClipper(clipper);
}

bool KUiChatInputWnd::handleKeyInput( const EventArgs& args )
{
	//当使用快捷键打开输入框的时候不要连带把快捷键显示出来
	if(d_justOpenFromKey)
	{
		d_justOpenFromKey = false;
		return true;
	}
	
	int selStart = 0;
	int selEnd = 0;
	d_inputBox->getLayout()->getSelection(selStart, selEnd);

	int delWordNum = selEnd - selStart;

	KeyEventArgs* key = (KeyEventArgs*)&args;
	
	if(key->codepoint < ' ')//如果是输入的控制字符
		return false;

	//\n和\是程序
	if(key->codepoint == '\n' || key->codepoint == '\\')
	{
		return true;
	}

	wchar_t inputWord[2];
	inputWord[0] = key->codepoint;
	inputWord[1] = 0;
	char* ansiWord = NULL;
	unicodeToAnsi(inputWord, ansiWord);
	write(ansiWord);
	delete[] ansiWord;
	ansiWord = NULL;

	return true;
}

int KUiChatInputWnd::layoutGOCount(ILayout* layout, LOGameObjType gotype)
{
	LOElemInfo* elem = NULL;
	int elemCount = layout->getElemList(elem);

	int itemCount = 0;
	for(int i = 0; i < elemCount; ++i)
	{
		if(elem[i].gameObj._objType == gotype)
		{
			++itemCount;
		}
	}
	delete[] elem;
	return itemCount;
}

void KUiChatInputWnd::write(LOElemInfo& newElem)
{
	int insertWordCount = 0;
	if(newElem.isShowDes == true || newElem.elemType == LO_IMAGE)
	{
		insertWordCount = 1;
	}
	else
	{
		int insertWordsLen = newElem.content.len();
		for(int i = 0; i <= insertWordsLen; ++i)
		{
			if(newElem.content[i] != '\n' && newElem.content[i] >= ' ')
			{
				newElem.content[insertWordCount++] = newElem.content[i];
			}
		}
		newElem.content[insertWordCount] = 0;
		insertWordCount--;
	}

	int selStart = 0;
	int selEnd = 0;
	d_inputBox->getLayout()->getSelection(selStart, selEnd);
	int delWordCount = selStart > selEnd ? selStart - selEnd : selEnd - selStart;
	
	int maxInsertCount = KUiCfgLoader::getSingleton().getChannelData().inputBoxMaxWordCount - d_inputBox->getLayout()->getWordCount() + delWordCount;
	if(maxInsertCount <= 0)
	{
		return;
	}
	else if(maxInsertCount < insertWordCount)
	{
		newElem.content[maxInsertCount] = 0;
	}
	
	if(LO_GO_ITEM == newElem.gameObj._objType 
		&& layoutGOCount(d_inputBox->getLayout(), LO_GO_ITEM) >= MAXSIZE_SYNC_ITEM_COUNT)
	{
		return;
	}

	newElem.vAlign = LO_VA_BOTTOM;
	d_inputBox->getLayout()->insertElem(newElem);
	d_inputBox->getLayout()->flashLayout();
	flashColor();
	showText();

	d_inputBox->showCaratImm();
	d_inputBox->requestRedraw();
}

void KUiChatInputWnd::write(const char* ansiText)
{
	wchar_t* unicodeText = NULL;
	ansiToUnicode(ansiText, unicodeText);
	LOElemInfo newElem;
	newElem.elemType = LO_TEXT;
	newElem.content = unicodeText;
	newElem.description = unicodeText;
	newElem.vAlign = LO_VA_BOTTOM;
	newElem.isShowDes = false;
	
	write(newElem);

	delete[] unicodeText;
	unicodeText = NULL;

	d_inputBox->requestRedraw();
}

bool KUiChatInputWnd::handleLBDown( const EventArgs& args )
{
	d_lbdown = true;

	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLEditbox* chanCtrl = (TLEditbox*)mouse->window;
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	int selectionStartIndex = d_inputBox->getLayout()->wordIndexAtPixel(xPos, yPos);
	d_inputBox->getLayout()->setSelection(selectionStartIndex, selectionStartIndex);

	d_inputBox->captureInput();

	d_inputBox->requestRedraw();
	return true;
}

bool KUiChatInputWnd::handleMouseMove( const EventArgs& args )
{
	if(!d_lbdown)
		return true;

	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLEditbox* chanCtrl = (TLEditbox*)mouse->window;
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x;
	if(xPos < 1)
	{
		xPos = 1;
	}
	xPos = xPos - off.d_x;
	int yPos = 8;
	int selectionEndIndex = d_inputBox->getLayout()->wordIndexAtPixel(xPos, yPos);

	d_inputBox->getLayout()->setSelection(-1, selectionEndIndex);

	d_inputBox->requestRedraw();
	return true;
}

bool KUiChatInputWnd::handleLBUp( const EventArgs& args )
{
	d_lbdown = false;
	return true;
}

bool KUiChatInputWnd::handleKeyDown( const EventArgs& args )
{	
	int startIndex = 0;
	int endIndex = 0;

	const KeyEventArgs& key = static_cast<const KeyEventArgs&>(args);

	switch(key.scancode)
	{
	case Key::Return:
		{
			doSendMessage();
//			Hide();
		}
		break;
	case Key::Delete:
		{
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			if(startIndex == endIndex)
				d_inputBox->getLayout()->setSelection(startIndex, startIndex + 1);
			d_inputBox->getLayout()->eraseSelection();
			flashColor();
		}
		break;
	case Key::Backspace:
		{
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			if(startIndex == endIndex)
				d_inputBox->getLayout()->setSelection(startIndex - 1, startIndex);
			d_inputBox->getLayout()->eraseSelection();
			flashColor();
		}
		break;
	case Key::ArrowLeft:
		{
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			endIndex = endIndex > 0 ? endIndex - 1 : 0;
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , endIndex);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(endIndex , endIndex);
			}
		}
		break;
	case Key::ArrowRight:
		{
			int startIndex = 0;
			int endIndex = 0;
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			endIndex += 1;
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , endIndex);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(endIndex , endIndex);
			}
		}
		break;
	case Key::Home:
		{
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , 0);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(0 , 0);
			}
		}
		break;
	case Key::End:
		{
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , 100000);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(100000 , 100000);
			}
		}
		break;
	case Key::V:
		{
			if(key.sysKeys & Control)
			{
				if(OpenClipboard(NULL) == false)
					break;
				////张鹏程添加，为unicode
				HGLOBAL hData = 0;//GetClipboardData(CF_OEMTEXT);
				UINT format = 0; // 从第一种格式值开始枚举
				while(format = EnumClipboardFormats(format))
				{
					if(format == CF_TEXT)
					{
						hData = GetClipboardData(CF_TEXT);
						if(NULL == hData)
						{
							CloseClipboard();
					         break;
						}
					    char* data = (char*)GlobalLock(hData);
						int len = strlen(data);
						if(len >= LAYOUT_TEXT_MAX_LEN)
						{
							memcpy(d_msg, data, LAYOUT_TEXT_MAX_LEN);
							d_msg[LAYOUT_TEXT_MAX_LEN - 1] = '\0';
						}
						else
						{
							strcpy(d_msg, data);
						}
						GlobalUnlock(hData);
							
						CloseClipboard();
						WriteFaceFromClipbord(d_msg);	
					}
					else
					if(format == CF_UNICODETEXT)
					{
						hData = GetClipboardData(CF_UNICODETEXT);
						if(NULL == hData)
						{
							CloseClipboard();
					         break;
						}
						wchar_t* wData = (wchar_t*)GlobalLock(hData);
						char* data = 0;
						unicodeToAnsi(wData,data);
						GlobalUnlock(hData);		
						CloseClipboard();
						int len = strlen(data);
						if(len >= LAYOUT_TEXT_MAX_LEN)
						{
							memcpy(d_msg, data, LAYOUT_TEXT_MAX_LEN);
							d_msg[LAYOUT_TEXT_MAX_LEN - 1] = '\0';
						}
						else
						{
							strcpy(d_msg, data);
						}
						write(d_msg);
						delete [] data;
					}
				}
			}
		}
		break;
	case Key::C:
		{
			if(key.sysKeys & Control)
			{
				if(OpenClipboard(NULL) == false)
					break;

				if(EmptyClipboard() == false)
				{
					CloseClipboard();
					break;
				}
				
				char* ansiText = NULL;
				
				int len = getSelectionText(ansiText);
				
				HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
				if(NULL == hData)
				{
					CloseClipboard();
					break;
				}
				
				char* pszData = (char*)GlobalLock(hData);
				if(NULL == pszData)
				{
					CloseClipboard();
					break;
				}
				
				memcpy(pszData, ansiText, len);
				pszData[len] = 0;
				GlobalUnlock(hData);
				SetClipboardData(CF_TEXT, hData);

				CloseClipboard();

				delete[] ansiText;
			}
		}
		break;
	case Key::X:
		{
			if(key.sysKeys & Control)
			{
				if(OpenClipboard(NULL) == false)
					break;

				if(EmptyClipboard() == false)
				{
					CloseClipboard();
					break;
				}
				
				char* ansiText = NULL;
				
				int len = getSelectionText(ansiText);
				
				HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
				if(NULL == hData)
				{
					CloseClipboard();
					break;
				}
				
				char* pszData = (char*)GlobalLock(hData);
				if(NULL == pszData)
				{
					CloseClipboard();
					break;
				}
				
				memcpy(pszData, ansiText, len);
				pszData[len] = 0;
				GlobalUnlock(hData);
				SetClipboardData(CF_TEXT, hData);

				CloseClipboard();

				delete[] ansiText;
				
				d_inputBox->getLayout()->eraseSelection();
			}
		}
		break;
	case Key::ArrowUp:
		{
			if(haveCachedMessage(true))
			{
				clearText();
				prevMessage(d_inputBox->getLayout());
				showText();
			}
		}
		break;
	case Key::ArrowDown:
		{
			if(haveCachedMessage(false))
			{
				clearText();
				nextMessage(d_inputBox->getLayout());
				showText();
			}
		}
		break;
	case Key::PageUp:
		{
			int chanId = KUiChanMgr::getSinglton().prevInputChanId();
			if(chanId != UI_CHAT_WINDOW_INVALIDATE_CHAN_ID)
			{
				switchChannel(chanId);
			}
			else
			{
				string name = KUiChanMgr::getSinglton().getCurReciverName();
				if(name != "")
				{
					clearText();
					write("/");
					write(name.c_str());
					write(" ");
					KUiChanMgr::getSinglton().prevReciverName();
				}
			}
		}
		break;
	case Key::PageDown:
		{
			static bool switchName = true;
			int chanId = KUiChanMgr::getSinglton().nextInputChanId();
			string nextName = KUiChanMgr::getSinglton().nextReciverName();
			if(nextName != "")
			{
				switchName = true;
				clearText();
				write("/");
				write(nextName.c_str());
				write(" ");
			}
			else if(chanId!= UI_CHAT_WINDOW_INVALIDATE_CHAN_ID)
			{
				if(switchName)
				{
					clearText();
					switchName = false;
				}
				switchChannel(KUiChanMgr::getSinglton().nextInputChanId());
			}
		}
		break;
	case Key::Escape:
		{
			if ( !KUiAdapter::EscHideDialog() )
			{
				KUiExit::Show();
			}
			g_pCoreShell->LockSomeoneAction(0);
			g_pCoreShell->Stop();
		}
		break;
	case Key::F1:
	case Key::F2:
	case Key::F3:
	case Key::F4:
	case Key::F5:
	case Key::F6:
	case Key::F7:
	case Key::F8:
	case Key::F9:
	case Key::F10:
	case Key::F11:
	case Key::F12:
	case Key::Tab:
		{
			return false;
		}
		break;
	}
	showText();
	
	d_inputBox->showCaratImm();
	d_inputBox->requestRedraw();
	
	return true;
}

int KUiChatInputWnd::getSelectionText(char*& text)
{
	static wchar_t copyText[LAYOUT_TEXT_MAX_LEN];
	wchar_t mySubText[LAYOUT_TEXT_MAX_LEN] = {0};
	copyText[0] = 0;

	int selStart = 0;
	int selEnd = 0;
	d_inputBox->getLayout()->getSelection(selStart, selEnd);
	if(selStart > selEnd)
	{
		int temp = selStart;
		selStart = selEnd;
		selEnd = temp;
	}
				
	LOElemInfo* elemList = NULL;
	int elemCount = d_inputBox->getLayout()->getElemList(elemList);
	int curIndex = 0;
	int copyIndex = 0;
	for(int i = 0; i < elemCount; ++i)
	{
		int elemStart = 0;
		int elemEnd = 0;
		const wchar_t* subText = NULL;
		mySubText[0] = 0;
		if(elemList[i].isShowDes)
		{
			if(curIndex >= selStart && curIndex < selEnd)
			{
				elemStart = 0;
				elemEnd = elemList[i].description.len();
				subText = elemList[i].description.get();
				wcscpy(mySubText,subText);
				
			}
			++curIndex;
		}
		else if(elemList[i].elemType == LO_IMAGE)
		{
			if(curIndex >= selStart && curIndex < selEnd)
			{
				elemStart = 0;
				elemEnd = elemList[i].content.len();
				subText = elemList[i].content.get();
				wcscpy(mySubText,L"<face>");
				wcscat(mySubText,L"<id>");
				wchar_t temp[256] = {0};
				wsprintfW(temp,L"%d",elemList[i].gameObj._objId[0]);
				wcscat(mySubText,temp);
				wcscat(mySubText,L"</id></face>");
				elemEnd = wcslen(mySubText);
			}
			++curIndex;
		}
		else
		{
			int elemWordCount = elemList[i].content.len();

			elemEnd = elemWordCount;
			if(selStart >= curIndex && selStart < curIndex + elemWordCount)
			{
				elemStart = selStart - curIndex;
			}

			if(selEnd > curIndex && selEnd <= curIndex + elemWordCount )
			{
				elemEnd = selEnd - curIndex;
			}

			if(selEnd <= curIndex || selStart >= curIndex + elemWordCount)
			{
				elemStart = 0;
				elemEnd = 0;
			}
			subText = elemList[i].content.get();
			wcscpy(mySubText,subText);
			curIndex += elemWordCount;
		}

		for(int j = elemStart; j < elemEnd; ++j)
		{
			copyText[copyIndex++] = mySubText[j];
		}
	}
	delete[] elemList;
	elemList = NULL;

	copyText[copyIndex] = 0;
	return unicodeToAnsi(copyText, text);
}

void KUiChatInputWnd::flashColor()
{
	//判断是否私聊，如果是则改变其颜色，否则显示当前颜色
	LOElemInfo* elemList;

	int elemCount = d_inputBox->getLayout()->getElemList(elemList);
	if(elemCount <= 0)
	{
		return;
	}

	const wchar_t* firstContent = elemList[0].content.get();
	wchar_t cozeName[COMMON_CLIENT_MSG_LEN_64];
	cozeName[0] = 0;
	swscanf(firstContent, L"/%[^ ] %*[^\0]", cozeName);
	int nameLen = wcslen(cozeName);
	int textLen = wcslen(firstContent);

	if(cozeName[0] != 0 && (nameLen + 2 <= textLen || elemCount > 1))
	{
		d_inputBox->getLayout()->setColor(KUiChanMgr::getSinglton().getChanColor(COSE_ROOM_ID, false));
	}
	else
	{
		int chanId = KUiChanMgr::getSinglton().getInputChanId();

		MapChannelInfo channelInfo;
		if (chanId == MAP_ROOM_ID && KUiChanMgr::getSinglton().getChanInfo(chanId, channelInfo))
		{
			d_inputBox->getLayout()->setColor(channelInfo.szColor);
		}
		else
		{
			d_inputBox->getLayout()->setColor(KUiChanMgr::getSinglton().getChanColor(chanId, false));
		}
	}

	delete[] elemList;
}

void KUiChatInputWnd::showText()
{
	//这里主要调整layout相对于控件的位置，以保证光标在控件的裁减区域内
	int startIndex = 0;
	int endIndex = 0;
	d_inputBox->getLayout()->getSelection(startIndex, endIndex);

	int caratPosXInLayout = d_inputBox->getLayout()->getPosAtWordIndex(endIndex).x;
	int inputBoxWidth = d_inputBox->getUnclippedPixelRect().getWidth();
	int inputBoxHeight = d_inputBox->getUnclippedPixelRect().getHeight();
	int layoutOffX = d_inputBox->getLayoutOffset().d_x;

	int layoutHeight = d_inputBox->getLayout()->getRenderArea().getHeight();
	if(layoutHeight == 0)
	{
		layoutHeight = 14;//特殊处理，当没有内容的时候则给定一个排版高度
	}
	if(caratPosXInLayout + layoutOffX < 0)
	{
		d_inputBox->setLayoutOffset(-caratPosXInLayout, inputBoxHeight - layoutHeight);
	}
	else if(caratPosXInLayout + layoutOffX > inputBoxWidth)
	{
		d_inputBox->setLayoutOffset(inputBoxWidth - caratPosXInLayout, inputBoxHeight - layoutHeight);
	}
	else
	{
		d_inputBox->setLayoutOffset(layoutOffX, inputBoxHeight - layoutHeight);
	}
}

bool KUiChatInputWnd::canSay(int chatInputId)
{
	
	bool bGM = g_pCoreShell->GetGameData(GDI_IS_GM, NULL, NULL) > 0 ? true : false;
	if ( bGM )
	{
		return true;
	}
	if(GLOBAL_ROOM_ID == chatInputId || MAP_ROOM_ID == chatInputId)
	{
		int intervalTime = 0;
		g_pCoreShell->GetGameData(GDI_MAP_CHANNEL_TIME, (unsigned int)&intervalTime, 0);
		
		static DWORD lastTimeSend = ::GetTickCount() + intervalTime;
		if(::GetTickCount() - lastTimeSend < intervalTime)
		{
			char* szMsg = MSG_CHAT_TOOFAST;
			char	buf[sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG];
			PCHATROOMMSG_TO_SOMEONE	pRoomMsg = (PCHATROOMMSG_TO_SOMEONE)buf;
			B2ChatDialog::chatManager.ChatClientInsertSystemMsg(MSG_CHAT_TOOFAST);
			//strncpy(pRoomMsg->senderName, CHAT_SENDER_SYSTEMNAME, sizeof(pRoomMsg->senderName));
			pRoomMsg->senderName[0] = 0;
			pRoomMsg->msgLen = strlen(szMsg);
			
			memcpy(pRoomMsg->msg, szMsg, pRoomMsg->msgLen);
			pRoomMsg->msg[pRoomMsg->msgLen] = '\0';
			KUiChannelCentre::GetSingleton().recvMessage(SYSTEM_ROOM_ID, (BYTE*)buf);
			return false;
		}
		else
		{
			lastTimeSend = ::GetTickCount();
		}
	}
	return true;
}

bool KUiChatInputWnd::isCurInput()
{
	return d_inputBox->isVisible();
}

void KUiChatInputWnd::doSendMessage()
{
	d_inputBox->deactivate();
	m_pThisWnd->hide();

	int chatInputId = KUiChanMgr::getSinglton().getInputChanId();
	
	if(UI_CHAT_WINDOW_INVALIDATE_CHAN_ID == chatInputId)
	{
		return;
	}

	if(SYSTEM_ROOM_ID == chatInputId)
	{
 		char* text = NULL;
 		d_inputBox->getLayout()->setSelection(0, 1000000);
 		getSelectionText(text);
 		
 		if(text[0] == '?' && text[1] == 'g' && text[2] == 'm' 
 		&& text[3] == ' ' && text[4] == 'd' && text[5] == 's')
 		{
 			//do nothing
 		}
 		else if(text[0] == 'c' && text[1] == '?' && text[2] == 'g' && text[3] == 'm' 
 		&& text[4] == ' ' && text[5] == 'd' && text[6] == 's')
 		{
 			//do nothing
 		}
		else if(text[0] == '?' && text[1] == 'g' && text[2] == 'm' 
			&& text[3] == ' ' && text[4] == 'd' && text[5] == 'w')
		{
			//do nothing
 		}
		else if ( strncmp("?gm DoSct", text, sizeof("?gm DoSct") - 1 ) == 0 )
		{
			//do nothing
		}
		else if ( strncmp("?gm RunSctFile", text, sizeof("?gm RunSctFile") - 1 ) == 0 )
		{
			//do nothing
		}
		else if ( strncmp("?gm RSF", text, sizeof("?gm RSF") - 1 ) == 0 )
		{
			//do nothing
		}
		else if ( strncmp("?gm ReLoadSct", text, sizeof("?gm ReLoadSct") - 1 ) == 0 )
		{
			//do nothing
		}
		else if ( strncmp("?gm RLS", text, sizeof("?gm RLS") - 1 ) == 0 )
		{
			//do nothing
		}
		else if ( strncmp("?gm ReLoadAllSct", text, sizeof("?gm ReLoadAllSct") - 1 ) == 0 )
		{
			//do nothing
		}
		else if ( strncmp("?gm RLAS", text, sizeof("?gm RLAS") - 1 ) == 0 )
		{
			//do nothing
		}
 		else if(text[0] != 0 )
 		{
 			KUiGMCommunication::getSingleton().setYourMsg(text);
 			KUiGMCommunication::getSingleton().show();
 			KUiGMCommunication::getSingleton().showCommit();
 			clearText();
 			return;
 		}
 		delete[] text;
	}

	LOElemInfo* elemList = NULL;
	int elemCount = d_inputBox->getLayout()->getElemList(elemList);
	if(elemCount <= 0)
	{
		return;
	}

	cacheMessage(elemList, elemCount);

	d_msg[0] = 0;
	int textLen = 0;
	char textHead[COMMON_CLIENT_MSG_LEN_64];
	memset(textHead, 0, COMMON_CLIENT_MSG_LEN_64);
	for(int i = 0; i < elemCount; ++i)
	{
		if(elemList[i].gameObj._objType == LO_GO_ITEM)
		{
			sprintf(textHead, "<I=%d|%d|%d|", 
				elemList[i].gameObj._objId[0], 
				elemList[i].gameObj._objId[1], 
				elemList[i].gameObj._objId[2]);
		}
		else if(elemList[i].gameObj._objType == LO_GO_NOTHING)
		{
			strcpy(textHead, "<N=");
		}
		else if(elemList[i].gameObj._objType == LO_GO_FACE)
		{
			sprintf(textHead, "<F=%d", 
				elemList[i].gameObj._objId[0]);
		}
		else if(elemList[i].gameObj._objType == LO_GO_POSITION)
		{
			sprintf(textHead, "<P=%d|%d|%d|%d|", 
				elemList[i].gameObj._objId[0], 
				elemList[i].gameObj._objId[1], 
				elemList[i].gameObj._objId[2], 
				elemList[i].gameObj._objId[3]);
		}

		//判断是否超长
		textLen += strlen(textHead);
		if(textLen > LAYOUT_TEXT_MAX_LEN)
		{
			break;
		}

		char* ansiText = NULL;
		int ansiTextLen = 0;
		if(elemList[i].isShowDes)
		{
			ansiTextLen = unicodeToAnsi(elemList[i].description.get(), ansiText);
		}
		else
		{
			ansiTextLen = unicodeToAnsi(elemList[i].content.get(), ansiText);
		}
		for(int j = 0; j < ansiTextLen; ++j)
		{
			if(ansiText[j] == '<')
			{
				ansiText[j] = 1;
			}
			else if(ansiText[j] == '>')
			{
				ansiText[j] = 2;
			}
		}
		//判断是否超长
		textLen += strlen(ansiText);		
		if(textLen > LAYOUT_TEXT_MAX_LEN)
		{
			break;
		}
		
		strcat(d_msg, textHead);
		if(elemList[i].gameObj._objType != LO_GO_FACE)
		{
			strcat(d_msg, ansiText);
		}
		strcat(d_msg, ">");

		delete[] ansiText;
		ansiText = NULL;
	}

	delete[] elemList;
	elemList = NULL;

	char cozeName[COMMON_CLIENT_MSG_LEN_64];
	cozeName[0] = 0;
	sscanf(d_msg, "<N=/%[^ ] %[^\0]", cozeName, d_msg + 3);

 	if(cozeName[0] != 0)
	{
		d_msg[0] = '<';
		d_msg[1] = 'N';
		d_msg[2] = '=';
		if(strlen(d_msg) > 4)
		{
			g_pCoreShell->OperationRequest( GOI_SEND_CHAT_DATE_P2P, (UINT)cozeName, (int)d_msg);
		}
 	}
 	else if(canSay(chatInputId))
 	{
 		g_pCoreShell->OperationRequest( GOI_SEND_CHAT_DATE_P2R, (UINT)chatInputId, (int)d_msg);
 	}

	clearText();
}

void KUiChatInputWnd::cacheMessage(LOElemInfo* toCachedElems, int elemCount)
{
	Centence newCentence;
	for(int i = 0; i < elemCount; ++i)
	{
		newCentence.push_back(toCachedElems[i]);
	}

	list<Centence>::iterator centenceIt = d_msgCache.begin();
	//这里d_curCacheIndex可能会是等于d_msgCache.size()，因此要判断一下
	if(d_curCacheIndex >= d_msgCache.size())
	{
		d_curCacheIndex = d_msgCache.size() - 1;
	}
	if(d_curCacheIndex < 0)
	{
		d_curCacheIndex = 0;
	}
	for(int j = 0; j < d_curCacheIndex; ++j)
	{
		++centenceIt;
	}

	if(isCentenceEqual(*centenceIt, newCentence))
	{
		++d_curCacheIndex;
		return;
	}

	d_msgCache.push_back(newCentence);
	int cacheUseSize = d_msgCache.size();
	if(cacheUseSize > UI_CHAT_WINDOW_INPUT_MAX_CACHE_SENDED_TEXT_COUNT)
	{
		d_msgCache.pop_front();
	}
	d_curCacheIndex = d_msgCache.size();
}

bool KUiChatInputWnd::haveCachedMessage(bool prev) const
{
	if(prev)
	{
		if(d_curCacheIndex - 1 < 0 || d_curCacheIndex - 1 >= d_msgCache.size())
		{
			return false;
		}
	}
	else
	{
		if(d_curCacheIndex + 1 < 0 || d_curCacheIndex + 1 >= d_msgCache.size())
		{
			return false;
		}
	}

	return true;
}
void KUiChatInputWnd::prevMessage(ILayout* pLayout)
{
	if(d_curCacheIndex - 1 < 0 || d_curCacheIndex - 1 >= d_msgCache.size())
	{
		return;
	}

	--d_curCacheIndex;
	
	list<Centence>::iterator centenceIt = d_msgCache.begin();
	for(int i = 0; i < d_curCacheIndex; ++i)
	{
		++centenceIt;
	}
	Centence& curCentence = *centenceIt;

	for(int j = 0; j < curCentence.size(); ++j)
	{
		pLayout->insertElem(curCentence[j]);
	}

	pLayout->flashLayout();
}

void KUiChatInputWnd::nextMessage(ILayout* pLayout)
{
	if(d_curCacheIndex + 1 < 0 || d_curCacheIndex + 1 >= d_msgCache.size())
	{
		return;
	}

	++d_curCacheIndex;

	list<Centence>::iterator centenceIt = d_msgCache.begin();
	for(int i = 0; i < d_curCacheIndex; ++i)
	{
		++centenceIt;
	}
	Centence& curCentence = *centenceIt;

	for(int j = 0; j < curCentence.size(); ++j)
	{
		pLayout->insertElem(curCentence[j]);
	}

	pLayout->flashLayout();
	showText();
}

void KUiChatInputWnd::clearAllCachedMessage()
{
	d_msgCache.clear();
	d_curCacheIndex = 0;
}

bool KUiChatInputWnd::isCentenceEqual(Centence& centence1, Centence& centence2)
{
	if(centence1.size() != centence2.size())
	{
		return false;
	}

	for(int i = 0; i < centence1.size(); ++i)
	{
		LOElemInfo& sub1 = centence1[i];
		LOElemInfo& sub2 = centence2[i];
		
		if(sub1 != sub2)
		{
			return false;
		}
	}
	return true;
}

void KUiChatInputWnd::setLastSender(char* lastSender)
{
	if(strlen(lastSender) > COMMON_CLIENT_MSG_LEN_32)
		return;
	
	strcpy(d_lastSender, lastSender);
}

void KUiChatInputWnd::setLastReciver(char* lastReciver)
{
	if(strlen(lastReciver) > COMMON_CLIENT_MSG_LEN_32)
		return;

	strcpy(d_lastReciver, lastReciver);

	recordReciver(lastReciver);
}

void KUiChatInputWnd::showLastSender()
{
	if(0 == d_lastSender[0])
	{
		return;
	}

	clearText();
	write("/");
	write(d_lastSender);
	write(" ");
	
	show();
	d_justOpenFromKey = true;
}

void KUiChatInputWnd::showLastReciver()
{
	if(0 == d_lastReciver[0])
	{
		return;
	}

	clearText();
	write("/");
	write(d_lastReciver);
	write(" ");
	
	show();	
	d_justOpenFromKey = true;
}

void KUiChatInputWnd::switchChannel(int chanId)
{
	if(UI_CHAT_WINDOW_INVALIDATE_CHAN_ID == chanId)
	{
		return;
	}
	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(d_chanId[i] == chanId && d_chanId[i] != COSE_ROOM_ID && d_chanId[i] != COMBAT_INFO_ROOM_ID)
		{
		//	d_chanPopBtn->setText(AnsiToUtf8(KUiChanMgr::getSinglton().getChanNameById(chanId)));
			d_chanPopBtn->setText(d_chanSelBtn[i]->getText());
			d_chanPopBtn->setNormalTextColour(d_chanSelBtn[i]->getNormalTextColour());
			d_chanPopBtn->setPushedTextColour(d_chanSelBtn[i]->getPushedTextColour());
			d_chanPopBtn->setHoverTextColour(d_chanSelBtn[i]->getHoverTextColour());
			d_chanPopBtn->setDisabledTextColour(d_chanSelBtn[i]->getDisabledTextColour());
			KUiChanMgr::getSinglton().setInputChanId(d_chanId[i]);
			flashColor();
			break;
		}
	}
	if(chanId == SYSTEM_ROOM_ID)
	{
		d_chanPopBtn->setText(AnsiToUtf8(KUiCfgLoader::getSingleton().getChannelData().gmChanName));
	}
}

bool KUiChatInputWnd::handleSend( const EventArgs& args )
{
	doSendMessage();
//	KUiChatInputWnd::Hide();
	return true;
}

bool KUiChatInputWnd::handleFaceMouseIn( const EventArgs& args )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticImage* faceCtrl = (TLStaticImage*)mouse->window;
	
	//获得tip排版文字
	int faceId = atoi(faceCtrl->getUserString("goid").c_str());
	
	char tipText[COMMON_CLIENT_MSG_LEN_32];

	const KUiCfgLoader::FacePanelCfgData& facePanelCfg  = KUiCfgLoader::getSingleton().getFaceData();
	for(int i = 0; i < facePanelCfg.faceList.size(); ++i)
	{
		if(faceId == facePanelCfg.faceList[i].index)
		{
			strcpy(tipText, facePanelCfg.faceList[i].description);
			break;
		}
	}

	char layoutText[COMMON_CLIENT_MSG_LEN_1024];
	sprintf(layoutText, facePanelCfg.tipLayoutText, tipText);

	//计算tip位置并显示tip
	KUiItemTip::GetSingleton().show(layoutText, faceCtrl->getInnerRect(), KUiItemTip::Right);
	
	//显示边框
	d_facePanelSelectFrameImage->show();

	d_facePanelSelectFrameImage->setPosition(Absolute, faceCtrl->getPosition(Absolute)
		- Point(d_facePanelSelectFrameImage->getLeftFrameWidth(), d_facePanelSelectFrameImage->getTopFrameHeight()));

	d_facePanelSelectFrameImage->setWidth(Absolute, faceCtrl->getWidth(Absolute)
		+ d_facePanelSelectFrameImage->getLeftFrameWidth() + d_facePanelSelectFrameImage->getRightFrameWidth());

	d_facePanelSelectFrameImage->setHeight(Absolute, faceCtrl->getHeight(Absolute)
		+ d_facePanelSelectFrameImage->getTopFrameHeight() + d_facePanelSelectFrameImage->getBottomFrameHeight());
	return true;
}

bool KUiChatInputWnd::handleFaceMouseOut( const EventArgs& args )
{
	d_facePanelSelectFrameImage->hide();
	KUiItemTip::Hide();
	return true;
}

bool KUiChatInputWnd::handleSelectAFace( const EventArgs& args )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticImage* faceCtrl = (TLStaticImage*)mouse->window;
	
	LOGameObject go;
	go._objType = LO_GO_FACE;
	go._objId[0] = atoi(faceCtrl->getUserString("goid").c_str());

	if(go._objType != LO_GO_FACE)
	{
		return false;
	}

	//根据goid得到内容和描述
	wchar_t* content = NULL;
	wchar_t* description = NULL;
	const KUiCfgLoader::FacePanelCfgData& facePanelCfg  = KUiCfgLoader::getSingleton().getFaceData();

	for(int i = 0; i < facePanelCfg.faceList.size(); ++i)
	{
		if(facePanelCfg.faceList[i].index == go._objId[0])
		{
			ansiToUnicode(facePanelCfg.faceList[i].image, content);
			ansiToUnicode(facePanelCfg.faceList[i].description, description);
			break;
		}
	}
	if(NULL == content || NULL == description)
	{
		return false;
	}

	LOElemInfo imageElem;

	imageElem.elemType = LO_IMAGE;
	imageElem.isShowDes = false;
	imageElem.gameObj = go;
	imageElem.content = content;
	imageElem.description = description;
	write(imageElem);
	delete[] content;
	delete[] description;
	content = NULL;
	description = NULL;
	
	d_facePanel->hide();
	show();
	return true;
}

bool KUiChatInputWnd::handleFaceBtnDown( const EventArgs& args )
{
	if(d_facePanel->isVisible())
	{
		d_facePanel->hide();
	}
	else
	{
		d_facePanel->show();
		d_facePanel->getRoot()->addChildWindow(d_facePanel);
	}
	return true;
}

bool KUiChatInputWnd::onHide( const EventArgs& args )
{
	d_facePanel->hide();
	d_latestChatMenu->hide();
	d_chanSelMenu->hide();
	return true;
}

void KUiChatInputWnd::WriteFaceFromClipbord(char* pText)
{
	vector<LOElemInfo*> elemInfoList;
	ChatEditBox::ChatEditGetElemListForClipbord(pText,&elemInfoList);
	if(!elemInfoList.empty())
	{
		int size = elemInfoList.size();
		for(int i = 0; i < size; i++)
		{
			LOElemInfo* pElemInfo = elemInfoList[i];
			write(*pElemInfo);
		}
	}

	///清除///
	while (!elemInfoList.empty())
	{
		LOElemInfo* pElemInfo = elemInfoList.back();
		elemInfoList.pop_back();
		delete pElemInfo;
		pElemInfo = 0;
	}
	elemInfoList.clear();
}

void KUiChatInputWnd::freshChanName()
{
	for(int i = 0; i < UI_CHAT_WINDOW_MAX_CHAN_COUNT; ++i)
	{
		if(d_chanId[i] == MAP_ROOM_ID)
		{
			MapChannelInfo channelInfo;
			if (KUiChanMgr::getSinglton().getChanInfo(d_chanId[i], channelInfo))
			{
				d_chanSelBtn[i]->setText(AnsiToUtf8(channelInfo.szChannelName));

				int color_r = 255;
				int color_g = 255;
				int color_b = 255;
				sscanf(channelInfo.szColor, "%d,%d,%d", &color_r, &color_g, &color_b);

				DWORD textColor = RGB(color_b, color_g, color_r);
				d_chanSelBtn[i]->setNormalTextColour(textColor);
				d_chanSelBtn[i]->setHoverTextColour(textColor);
				d_chanSelBtn[i]->setPushedTextColour(textColor);
			}
			else
			{
				d_chanSelBtn[i]->setText(AnsiToUtf8(KUiChanMgr::getSinglton().getChanNameById(d_chanId[i])));

				int color_r = 255;
				int color_g = 255;
				int color_b = 255;
				sscanf(KUiChanMgr::getSinglton().getChanColor(d_chanId[i], false), "%d,%d,%d", &color_r, &color_g, &color_b);

				DWORD textColor = RGB(color_b, color_g, color_r);
				d_chanSelBtn[i]->setNormalTextColour(textColor);
				d_chanSelBtn[i]->setHoverTextColour(textColor);
				d_chanSelBtn[i]->setPushedTextColour(textColor);
			}
		}
		if(d_chanId[i] == SYSTEM_ROOM_ID)
		{
			d_chanSelBtn[i]->setText(AnsiToUtf8(KUiCfgLoader::getSingleton().getChannelData().gmChanName));
		}
	}

	int inputId = KUiChanMgr::getSinglton().getInputChanId();
	if(inputId == MAP_ROOM_ID)
	{
		MapChannelInfo channelInfo;
		if (KUiChanMgr::getSinglton().getChanInfo(inputId, channelInfo))
		{
			d_chanPopBtn->setText(AnsiToUtf8(channelInfo.szChannelName));
			
			int color_r = 255;
			int color_g = 255;
			int color_b = 255;
			sscanf(channelInfo.szColor, "%d,%d,%d", &color_r, &color_g, &color_b);

			DWORD textColor = RGB(color_b, color_g, color_r);
			d_chanPopBtn->setNormalTextColour(textColor);
			d_chanPopBtn->setHoverTextColour(textColor);
			d_chanPopBtn->setPushedTextColour(textColor);
		}
		else
		{
			d_chanPopBtn->setText(AnsiToUtf8(KUiChanMgr::getSinglton().getChanNameById(inputId)));

			int color_r = 255;
			int color_g = 255;
			int color_b = 255;
			sscanf(KUiChanMgr::getSinglton().getChanColor(inputId, false), "%d,%d,%d", &color_r, &color_g, &color_b);

			DWORD textColor = RGB(color_b, color_g, color_r);
			d_chanPopBtn->setNormalTextColour(textColor);
			d_chanPopBtn->setHoverTextColour(textColor);
			d_chanPopBtn->setPushedTextColour(textColor);
		}
	}
	if(inputId == SYSTEM_ROOM_ID)
	{
		d_chanPopBtn->setText(AnsiToUtf8(KUiCfgLoader::getSingleton().getChannelData().gmChanName));
	}
}

void KUiChatInputWnd::sendGMCommand( string command )
{
	d_inputBox->getLayout()->clearLayout();
 	string layoutcommand; 
 	layoutcommand = layoutcommand + "<Layout width=2500 height=25><Seg text-align=left float=right><Obj type=text color=255,255,255 vertical-align=center font-family=stzhongs-9>" + command + "</Obj></Seg></Layout>";
	char *bb = ( char* )( layoutcommand.c_str() );
	d_inputBox->getLayout()->formatText( bb );
	d_inputBox->getLayout()->SetText( bb );
	KUiChanMgr::getSinglton().setInputChanId( SYSTEM_ROOM_ID );
	doSendMessage();
}