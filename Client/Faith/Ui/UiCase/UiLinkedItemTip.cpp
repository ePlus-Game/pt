
#include "UiLinkedItemTip.h"
#include "UiTipGenerator.h"
#include "CoreShell.h"
#include "KWin32Wnd.h"
#include "../UiSheetMgr.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

template<> 
KUiLinkedItemTip* KUiWndSingleton<KUiLinkedItemTip>::ms_Singleton	= NULL;

KUiLinkedItemTip::KUiLinkedItemTip(const CEGUI::String& id_name):
KUiWndSingleton<KUiLinkedItemTip>( id_name )
{
	
}

KUiLinkedItemTip::~KUiLinkedItemTip()
{

}

void KUiLinkedItemTip::getChild()
{
	m_pThisWnd->setRenderMode(true);

	d_closeBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/LinkedItemTip/Close");
	d_closeBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiLinkedItemTip::close, this));
//	d_closeBtn->hide();

	d_tipText			= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/LinkedItemTip/Text");
	d_tipText->setPosition(Absolute, Point(0, 0));
	d_compareTipText	= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/LinkedItemTip/CompareText");
	d_compareTipText->setPosition(Absolute, Point(0, 0));
	d_tipText->disable();
}

bool KUiLinkedItemTip::close(const EventArgs& args)
{
	Hide();
	return true;
}

void KUiLinkedItemTip::Init()
{
	getChild();
}

void KUiLinkedItemTip::show(char* layoutText, CEGUI::Rect aroundArea, TipPos tipPos)
{
	if(strlen(layoutText) >= LAYOUT_TEXT_MAX_LEN)
	{
		return;
	}

	KUiWndSingleton<KUiLinkedItemTip>::Show();
	
	d_tipText->useLayout();
	d_tipText->getLayout()->formatText(layoutText);
	d_tipText->getLayout()->SetText(layoutText);
	d_tipText->getLayout()->flashLayout();
	
	if(d_tipText->getLayout()->isHaveContent())
	{
		d_tipText->setText("");
		d_tipText->setLayoutOffset(d_tipText->getLeftFrameWidth(), d_tipText->getTopFrameHeight());
		d_tipText->fitLayoutSize(false);
	}
	else
	{
		d_tipText->setText(AnsiToUtf8(layoutText));
		d_tipText->setHeight(Absolute, 130);
		d_tipText->setWidth(Absolute, 200);
	}

	//重新调整窗口大小
	m_pThisWnd->setWidth(Absolute, d_tipText->getWidth(Absolute));
	m_pThisWnd->setHeight(Absolute, d_tipText->getHeight(Absolute));

	adjustPos(aroundArea, tipPos);

	d_compareTipText->hide();
}

void KUiLinkedItemTip::showCompare(char* layoutText)
{
	if(strlen(layoutText) >= LAYOUT_TEXT_MAX_LEN)
	{
		return;
	}

	d_compareTipText->useLayout();
	d_compareTipText->getLayout()->formatText(layoutText);
	d_compareTipText->getLayout()->SetText(layoutText);
	d_compareTipText->getLayout()->flashLayout();
	
	d_compareTipText->setText("");
	d_compareTipText->setLayoutOffset(d_compareTipText->getLeftFrameWidth(), d_compareTipText->getTopFrameHeight());
	d_compareTipText->fitLayoutSize(false);

	if(d_tipText->getUnclippedInnerRect().d_left - d_tipText->getAbsoluteWidth() < 0)
	{
		d_compareTipText->setXPosition(Absolute, d_tipText->getAbsoluteWidth());
	}
	else
	{
		d_compareTipText->setXPosition(Absolute, -d_compareTipText->getAbsoluteWidth());
	}

	if(d_tipText->getUnclippedPixelRect().d_bottom > g_GetScreenHeight())
	{
		d_compareTipText->setYPosition(g_GetScreenHeight() - d_compareTipText->getAbsoluteHeight());
	}

	if(d_compareTipText->getLayout()->isHaveContent())
		d_compareTipText->show();
}

void KUiLinkedItemTip::adjustPos(CEGUI::Rect aroundArea, TipPos tipPos)
{
	Point	newPos = aroundArea.getPosition();

	Point	thisPos		= m_pThisWnd->getAbsolutePosition();
	int		thisHeight	= m_pThisWnd->getAbsoluteHeight();
	int		thisWidth	= m_pThisWnd->getAbsoluteWidth();
	
	switch(tipPos)
	{
	case KUiLinkedItemTip::Top:
		{
			newPos.d_y -= thisHeight;
		}
		break;
	case KUiLinkedItemTip::Bottom:
		{
			newPos.d_y += aroundArea.getHeight();
		}
		break;
	case KUiLinkedItemTip::Left:
		{
			newPos.d_x -= thisWidth;
		}
		break;
	case KUiLinkedItemTip::Right:
		{
			newPos.d_x += aroundArea.getWidth();			
		}
		break;
	case KUiLinkedItemTip::TopLeft:
		{
			newPos.d_y -= thisHeight;
			newPos.d_x -= thisWidth;
		}
		break;
	case KUiLinkedItemTip::TopRight:
		{
			newPos.d_y -= thisHeight;
			newPos.d_x += aroundArea.getWidth();
		}
		break;
	case KUiLinkedItemTip::BottomLeft:
		{
			newPos.d_y += aroundArea.getHeight();
			newPos.d_x -= thisWidth;
		}
		break;
	case KUiLinkedItemTip::BottomRight:
		{
			newPos.d_y += aroundArea.getHeight();
			newPos.d_x += aroundArea.getWidth();
		}
		break;
	}
	
	//根据游戏世界的大小来调整（tip可能会超出显示范围）
	int gameWindowWidth = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->getAbsoluteWidth();
	int gameWindowHeight = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->getAbsoluteHeight();
	if(newPos.d_x + thisWidth > gameWindowWidth)
	{
		if(Right == tipPos || TopRight == tipPos || BottomRight == tipPos)
		{
			newPos.d_x = aroundArea.d_left - thisWidth;
		}
		else
		{
			newPos.d_x = gameWindowWidth - thisWidth - 1;
		}
	}
	if(newPos.d_x < 0)
	{
		if(Left == tipPos || TopLeft == tipPos || BottomLeft == tipPos)
		{
			newPos.d_x = aroundArea.d_right;
		}
		else
		{
			newPos.d_x = 1;
		}
	}
	if(newPos.d_y + thisHeight > gameWindowHeight)
	{
		if(Bottom == tipPos)
		{
			newPos.d_y = aroundArea.d_top - thisHeight;
		}
		else
		{
			newPos.d_y = gameWindowHeight - thisHeight - 1;
		}
	}
	if(newPos.d_y < 0)
	{
		if(Top == tipPos)
		{
			newPos.d_y = aroundArea.d_bottom;
		}
		else
		{
			newPos.d_y = 1;
		}
	}
	m_pThisWnd->setPosition(Absolute, newPos);
}
