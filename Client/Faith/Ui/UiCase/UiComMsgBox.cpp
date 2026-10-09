#include "KWin32.h"
#include "GlobalDef.h"
#include "UiComMsgBox.h"
#include "../UiSheetMgr.h"
#include "../UiConfigManager.h"

extern iCoreShell*		g_pCoreShell;

template<> 
KUiComMsgBox* KUiWndSingleton<KUiComMsgBox>::ms_Singleton	= NULL;

KUiComMsgBox::KUiComMsgBox(const CEGUI::String& id_name):
KUiWndSingleton<KUiComMsgBox>( id_name )
, d_bModalStatus(false)
, d_msgText(NULL)
, d_msgLayoutText(NULL)
, d_fristBtn(NULL)
, d_secondBtn(NULL)
, d_thirdBtn(NULL)
, d_icon(NULL)
, d_position(0,0)
, d_msgEditBg(NULL)
//, d_msgInput(NULL)
{
	d_style = "Normal";
}

KUiComMsgBox::~KUiComMsgBox()
{
	
}

void KUiComMsgBox::ConfirmUseYiBuItem( void )
{
	if ( g_pCoreShell && ms_Singleton && ms_Singleton->d_editText )
	{
		g_pCoreShell->OperationRequest( GOI_USE_YIBU_ITEM, (unsigned int)Utf8ToAnsi( ms_Singleton->d_editText->getText() ), NULL );
	}	
}

void KUiComMsgBox::CannelUseYiBuItem( void )
{
	if ( g_pCoreShell && ms_Singleton && ms_Singleton->d_editText )
	{
		g_pCoreShell->OperationRequest( GOI_USE_YIBU_ITEM, (unsigned int)"", NULL );
	}	

}

void KUiComMsgBox::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		//ms_Singleton->m_pThisWnd->setRenderMode(false, 3);
		ms_Singleton->getChild();
	}
}

void KUiComMsgBox::Show()
{
	KUiWndSingleton<KUiComMsgBox>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->setMetricsMode(Absolute);
		ms_Singleton->d_fristBtn->setMetricsMode(Absolute);
		ms_Singleton->d_secondBtn->setMetricsMode(Absolute);
		ms_Singleton->d_thirdBtn->setMetricsMode(Absolute);
		ms_Singleton->d_fristBtn->hide(); 
		ms_Singleton->d_secondBtn->hide();
		ms_Singleton->d_thirdBtn->hide();
		if ( ms_Singleton->d_bModalStatus )
		{
			ms_Singleton->m_pThisWnd->setModalState(true);
		}
		ms_Singleton->m_pThisWnd->show();
		ms_Singleton->m_pThisWnd->activate();

		if ( ms_Singleton->d_style == "UseItem")
		{
			KUiComMsgBox::GetSingleton().setFristBtnCallback(ConfirmUseYiBuItem);
			KUiComMsgBox::GetSingleton().setSecondBtnCallback(CannelUseYiBuItem);
			char yesString[COMMON_CLIENT_MSG_LEN_8];
			char noString[COMMON_CLIENT_MSG_LEN_8];
			strcpy(yesString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().yesString));
			strcpy(noString, (char*)AnsiToUtf8(KUiCfgLoader::getSingleton().getCommonCfg().noString));
			KUiComMsgBox::GetSingleton().setBtnName((utf8*)yesString, (utf8*)noString);
		}
	}
}

void KUiComMsgBox::getChild()
{
	String parentWndName = "TaharezLook/ComMsgBox_Normal";
	//d_msgText = (StaticText*)m_pThisWnd->getChild(parentWndName + "/Text");
	d_msgText = m_pThisWnd->getChild(parentWndName + "/MultiLineText");
	d_msgLayoutText = static_cast<TLStaticText*>(m_pThisWnd->getChild(parentWndName + "/Text"));
	d_icon = (StaticImage*)m_pThisWnd->getChild(parentWndName + "/Icon");
	d_fristBtn = (TLButton*)m_pThisWnd->getChild(parentWndName + "/FristBtn");
	d_secondBtn = (TLButton*)m_pThisWnd->getChild(parentWndName + "/SecondBtn");
	d_thirdBtn = (TLButton*)m_pThisWnd->getChild(parentWndName + "/ThridBtn");

	d_msgEditBg = m_pThisWnd->getChild(parentWndName + "/EditBg");
	d_editText = (TLEditbox*)d_msgEditBg->getChild(parentWndName + "/EditBg/EditBox");

	d_fristBtn->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiComMsgBox::onFristBtnDown, this));
	d_secondBtn->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiComMsgBox::onSecondBtnDown, this));
	d_thirdBtn->subscribeEvent(TLButton::EventClicked, Event::Subscriber(&KUiComMsgBox::onThirdBtnDown, this));
	d_position = Point(m_pThisWnd->getXPosition(Absolute), m_pThisWnd->getYPosition(Absolute));
	if ( d_style == "UseItem" ||
		d_style == "DongJie" ||
		d_style == "JinYan")
	{
		d_editText->setZLevel( Window::SuperTop );
		d_editText->show();
		d_msgEditBg->show();
	}
	else
	{
		d_editText->hide();
		d_msgEditBg->hide();
	}//*/

	ZeroMemory(d_layoutTextHead, sizeof(d_layoutTextHead));
	sprintf(d_layoutTextHead, "<Layout width=%d>", (int)(d_msgLayoutText->getAbsoluteWidth()) );

	init();
}

void KUiComMsgBox::setStyle(Style newStyle)
{
	switch(newStyle)
	{
	case Normal:
		d_style = "Normal";
		break;
	case Question:
		d_style = "Question";
		break;
	case Plaint:
		d_style = "Plaint";
		break;
	case Warning:
		d_style = "Warning";
		break;
	case Mistake:
		d_style = "Mistake";
		break; 
	case UseItem:
		d_style = "UseItem";
		break;
	case DongJie:
		d_style = "DongJie";
		break;
	case JinYan:
		d_style = "JinYan";
		break;
	}
	String parentWndName = "TaharezLook/ComMsgBox_Normal";
	m_pThisWnd = WindowManager::getSingleton().getWindow( parentWndName );
	getChild();
}

void KUiComMsgBox::setTopMost()
{
	if ( m_pThisWnd )
	{
		m_pThisWnd->setZLevel(Window::SuperTop);
	}
}

void KUiComMsgBox::setBtnName(String fristbtnName, String secondBtnName, String thirdName)
{
	if(!m_pThisWnd)
	{
		return;
	}

	int btnCount = 0;
	float wndWide = m_pThisWnd->getWidth();
	if(fristbtnName != "")
	{
		btnCount = 1;
		d_fristBtn->setText(fristbtnName);
		d_fristBtn->show();
	}
	else
	{
		return;
	}

	if(secondBtnName != "")
	{
		btnCount = 2;
		d_secondBtn->setText(secondBtnName);
		d_secondBtn->show();
	}
	else
	{
		d_fristBtn->setXPosition(Absolute, (wndWide - d_fristBtn->getWidth()) / 2);
		return;
	}
	if(thirdName != "")
	{
		btnCount = 3;
		d_thirdBtn->setText(thirdName);
		d_thirdBtn->show();
	}
	else
	{
		d_fristBtn->setXPosition(Absolute, (wndWide / 2 - d_fristBtn->getWidth()) / 2);
		d_secondBtn->setXPosition(Absolute, wndWide / 2 + (wndWide / 2 - d_fristBtn->getWidth()) / 2);
		return;
	}
	d_fristBtn->setXPosition(Absolute, (wndWide / 3 - d_fristBtn->getWidth()) / 2);
	d_secondBtn->setXPosition(Absolute, wndWide / 3 + (wndWide / 3 - d_fristBtn->getWidth()) / 2);
	d_thirdBtn->setXPosition(Absolute, wndWide * 2 / 3 + (wndWide / 3 - d_fristBtn->getWidth()) / 2);
}

void KUiComMsgBox::setMsg(String msg, bool bInput)
{
	if(!m_pThisWnd)
		return;
	
	if (bInput)
		d_msgEditBg->show();
	else
		d_msgEditBg->hide();
	
	d_msgLayoutText->hide();

	d_msgText->setText(msg);
	d_msgText->show();
}

void KUiComMsgBox::setLayoutMsg(char* msg, bool hasHead)
{
	if(!m_pThisWnd)
		return;

	d_msgEditBg->hide();
	d_msgText->hide();

	char	layoutText[COMMON_CLIENT_MSG_LEN_1024];
	ZeroMemory(layoutText, sizeof(layoutText));

	if (msg)
	{
		if (hasHead)
			snprintf(layoutText, sizeof(layoutText), "%s", msg);
		else
			snprintf(layoutText, sizeof(layoutText), "%s%s", d_layoutTextHead, msg);
	}
	layoutText[sizeof(layoutText) - 1] = 0;
	
	d_msgLayoutText->useLayout();
	d_msgLayoutText->getLayout()->formatText(layoutText);
	d_msgLayoutText->getLayout()->SetText(layoutText);
	d_msgLayoutText->getLayout()->flashLayout();
	d_msgLayoutText->show();
}

bool KUiComMsgBox::onFristBtnDown(const CEGUI::EventArgs& e)
{	
	if(d_callback1 != NULL)
		d_callback1();
	init();
	Hide();
	return true;
}

bool KUiComMsgBox::onSecondBtnDown(const CEGUI::EventArgs& e)
{	
	if(d_callback2 != NULL)
		d_callback2();
	init();
	Hide();
	return true;
}

bool KUiComMsgBox::onThirdBtnDown(const CEGUI::EventArgs& e)
{	
	if(d_callback3 != NULL)
		d_callback3();
	init();
	Hide();
	return true;
}

void KUiComMsgBox::setFristBtnCallback(void (*func)())
{
	d_callback1 = func;
}

void KUiComMsgBox::setSecondBtnCallback(void (*func)())
{
	d_callback2 = func;
}

void KUiComMsgBox::setThirdBtnCallback(void (*func)())
{
	d_callback3 = func;
}

void KUiComMsgBox::init()
{
	d_callback1 = NULL;
	d_callback2 = NULL;
	d_callback3 = NULL;
}

//设置为模态对话框状态
void KUiComMsgBox::setModalStatus( const bool bModel )
{
	d_bModalStatus = bModel;
}


void KUiComMsgBox::Hide()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		if ( ms_Singleton->d_bModalStatus )
		{
			ms_Singleton->d_bModalStatus = false;
			ms_Singleton->m_pThisWnd->setModalState(false);
		}
		ms_Singleton->d_editText->setText("");
		ms_Singleton->m_pThisWnd->setPosition(Absolute, ms_Singleton->d_position);
		KUiWndSingleton<KUiComMsgBox>::Hide();
	}
}

//设置对话框位置
void KUiComMsgBox::setComMsgPosition()
{
	if(!m_pThisWnd)
	{
		return;
	}

	d_position = Point(m_pThisWnd->getXPosition(Absolute), m_pThisWnd->getYPosition(Absolute));
	float fWidth = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->getWidth(Absolute);
	float fHeight = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->getHeight(Absolute);
	if ( fWidth > 0 &&  fHeight > 0)
	{
		m_pThisWnd->setPosition(Absolute, Point( (fWidth/2 - m_pThisWnd->getWidth(Absolute)/2), fHeight/6) );
	}
}

const String& KUiComMsgBox::getEditText( void )
{
	return d_editText->getText();
}