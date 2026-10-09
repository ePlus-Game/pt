#include "UiTopMessage.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"

template<> 
KUiTopMessage* KUiWndSingleton<KUiTopMessage>::ms_Singleton	= NULL;

KUiTopMessage::KUiTopMessage(const CEGUI::String& id_name):
KUiWndSingleton<KUiTopMessage>( id_name )
{

}

void KUiTopMessage::Init()
{
	if ( ms_Singleton && m_pThisWnd )
	{
		m_pThisWnd->setRenderMode(true);
		m_pThisWnd->beginUpdate();
		m_pThisWnd->disable();
	
		d_defaultText = m_pThisWnd->getText();
		d_defaultFont = const_cast<Font*>(m_pThisWnd->getFont());
		d_defaultColor = ((TLStaticText*)m_pThisWnd)->getTextColours();
		d_defaultSpeed = ((TLStaticText*)m_pThisWnd)->getRollSpeedH();
		d_loopTime = -1;

		m_pThisWnd->setZLevel(Window::SuperBottom);
		m_pThisWnd->SetBottomWindow();
		
		m_pThisWnd->subscribeEvent(Window::EventNewFrame, Event::Subscriber(&KUiTopMessage::updateSelf, ms_Singleton));
	}
}

KUiTopMessage::~KUiTopMessage()
{
	
}

void KUiTopMessage::setText(String message)
{
	if(!m_pThisWnd)
	{
		return;
	}

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(m_pThisWnd);
	m_pThisWnd->beginUpdate();

	StaticText* thisWnd = (StaticText*)m_pThisWnd;
 	thisWnd->setText(message);
	thisWnd->setOff(thisWnd->getWidth(Absolute), 0);
}

void KUiTopMessage::setStyle(CommonStyle& style)
{
	if(!m_pThisWnd)
	{
		return;
	}

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(m_pThisWnd);
	m_pThisWnd->beginUpdate();

	StaticText* thisWnd = (StaticText*)m_pThisWnd;
	thisWnd->setTextColours(style.color);
	String fontName = AnsiToUtf8(style.font);
	if(FontManager::getSingleton().isFontPresent(fontName))
	{
		thisWnd->setFont(fontName);
	}
	else
	{
		thisWnd->setFont(System::getSingleton().getDefaultFont());
	}
	thisWnd->setRollSpeedH(style.speed);
	
	Font* font = NULL;
	if(FontManager::getSingleton().isFontPresent(AnsiToUtf8(style.font)))
	{
		font = FontManager::getSingleton().getFont(AnsiToUtf8(style.font));
	}
	if(!font)
	{
		font = System::getSingleton().getDefaultFont();
	}

	int textExtent = 0;
	if(font)
	{
		textExtent = font->getTextExtent(thisWnd->getText());
	}
	d_loopTime = (textExtent + thisWnd->getWidth(Absolute)) * style.second * 1000 / style.speed;
}

void KUiTopMessage::setStyle(CommonStyle2& style)
{
	if(!m_pThisWnd)
	{
		return;
	}

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(m_pThisWnd);
	m_pThisWnd->beginUpdate();
	
	StaticText* thisWnd = (StaticText*)m_pThisWnd;
	const KUiCfgLoader::TopMessageCfg& topMsgCfg = KUiCfgLoader::getSingleton().getTopMessageCfg();

	style.msgId--;
	String msg("");
	if(style.msgId < topMsgCfg.msgs.size() && style.msgId >= 0)
	{
		msg = AnsiToUtf8(topMsgCfg.msgs[style.msgId].c_str());
	}
	setText(msg);

	//字体
	style.fontId--;
	String fontName("");
	if(style.fontId < topMsgCfg.fonts.size() && style.fontId >= 0)
	{
		fontName = AnsiToUtf8(topMsgCfg.fonts[style.fontId].c_str());
	}
	if(FontManager::getSingleton().isFontPresent(fontName))
	{
		thisWnd->setFont(fontName);
	}
	else
	{
		thisWnd->setFont(System::getSingleton().getDefaultFont());
	}

	//颜色
	thisWnd->setTextColours(style.color);
	//滚动速度
	thisWnd->setRollSpeedH(style.speed);
	//滚动圈速
	int textExtent = 0;
	Font* font = const_cast<Font*>(thisWnd->getFont());
	if(font)
	{
		textExtent = font->getTextExtent(thisWnd->getText());
	}
	d_loopTime = (textExtent + thisWnd->getWidth(Absolute)) * style.second * 1000 / style.speed;
}

void KUiTopMessage::resetText()
{
	if(!m_pThisWnd)
	{
		return;
	}
	
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(m_pThisWnd);
	StaticText* thisWnd = (StaticText*)m_pThisWnd;
 	thisWnd->setText(d_defaultText);
	thisWnd->setTextColours(d_defaultColor);
	thisWnd->setFont(d_defaultFont);
	thisWnd->setRollSpeedH(d_defaultSpeed);

	thisWnd->setOff(thisWnd->getWidth(Absolute), 0);
}

bool KUiTopMessage::updateSelf(const EventArgs& e)
{
	if(m_pThisWnd->getText() != d_defaultText)
	{
		FrameEventArgs* args = (FrameEventArgs*)&e;
		
		d_loopTime -= args->elapse;
		if(d_loopTime <= 0)
		{
			resetText();
		}
	}

	return true;
}