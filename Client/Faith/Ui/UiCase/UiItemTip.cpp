
#include "UiItemTip.h"
#include "UiTipGenerator.h"
#include "CoreShell.h"
#include "cfs_filelogs.h"
#include "../UiSheetMgr.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;

template<> 
KUiItemTip* KUiWndSingleton<KUiItemTip>::ms_Singleton	= NULL;

KUiItemTip::KUiItemTip(const CEGUI::String& id_name):
KUiWndSingleton<KUiItemTip>( id_name )
, d_closeBtn(NULL)
, d_tipText(NULL)
, d_compareTipText(NULL)
{
	GetSingleton();
}

KUiItemTip::~KUiItemTip()
{

}

void KUiItemTip::getChild()
{
	m_pThisWnd->setRenderMode(true);

	((TLStaticImage*)m_pThisWnd)->setFrameEnabled(false);
	((TLStaticImage*)m_pThisWnd)->setBackgroundEnabled(false);
	d_closeBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/ItemTip/Close");
	d_closeBtn->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiItemTip::close, this));
	d_closeBtn->hide();
	
	m_pThisWnd->disable();
	d_tipText			= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/ItemTip/Text");
	d_tipText->setPosition(Absolute, Point(0, 0));
	d_compareTipText	= (TLStaticText*)m_pThisWnd->getChild("TaharezLook/ItemTip/CompareText");
	d_compareTipText->setPosition(Absolute, Point(0, 0));
	d_tipText->disable();
}

bool KUiItemTip::close(const EventArgs& args)
{
	m_pThisWnd->hide();
	return true;
}

void KUiItemTip::Init()
{
	getChild();
}

void KUiItemTip::show(char* layoutText, CEGUI::Rect aroundArea, TipPos tipPos)
{
	System::getSingleton().getGUISheet()->addChildWindow(m_pThisWnd);
	if(strlen(layoutText) >= LAYOUT_TEXT_MAX_LEN)
	{
		return;
	}

	m_pThisWnd->show();
	
// 以下注释留作调试：请勿删除
// 	layoutText =	"<Layout width=200>"
//  					"<Seg text-align=left >"
//  					"<Obj type=text color=255,34,56 font-family=LiBian-16>abcdefg\n谢鉷hijkl\nmnopqr\nstuv\nwxyz</Obj>"
//  					"<Obj type=text font-family=SongTi-10 color=255,255,56>abcdef\ng谢鉷hij\nklmnopq\nrstuvw\nxyz</Obj>"
//  					"</Seg>"
//  					"<Seg text-align=center float=none>"
//  					"<Obj type=pic>set:bagua image:bagua1_normal</Obj>"
//  					"</Seg>"
//  					"</Layout>";

// 	layoutText =	" <Layout width=200>"
// 		"<Seg text-align=center float=wrap>"
// 		"<Obj color=0,255,0>武器名字</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=center float=wrap>"
// 		"<Obj color=0,205,255>[ 阴 ]</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=center float=wrap>"
// 		"<Obj color=255,153,0>[ 高级 ]</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=center float=wrap>"
// 		"<Obj color=204,153,255>“统帅之初品，可得人和之援”，唯此境界之名帅可以得其神展其威</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=left float=wrap>"
// 		"<Obj color=255,255,255>护甲 86<br><br>体 +15<br>灵 +24<br>力 +10<br>.</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=left float=wrap>"
// 		"<Obj color=51,102,255>灵 +1</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=left float=wrap>"
// 		"<Obj color=255,255,255>耐久：130/143.</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=left float=wrap>"
// 		"<Obj color=255,255,255>重量：26</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=left float=wrap>"
// 		"<Obj color=255,255,255>需求.燃叮.47</Obj>"
// 		"</Seg><Seg text-align=left float=wrap>"
// 		"<Obj color=255,255,255>需求职业： 玄风甲.</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=center float=wrap>"
// 		"<Obj color=0,255,0>御人 ( 6/9 )\n</Obj>"
// 		"<Obj color=150,150,150>御人玉佩\n</Obj>"
// 		"<Obj color=0,255,0>御人宝甲\n</Obj>"
// 		"<Obj color=150,150,150>御人披风\n</Obj>"
// 		"<Obj color=0,255,0>御人宝\n</Obj>"
// 		"<Obj color=150,150,150>御人宝甲\n</Obj>"
// 		"<Obj color=0,255,0>御人锁\n</Obj>"
// 		"<Obj color=0,255,0>御人戒指\n</Obj>"
// 		"<Obj color=0,255,0>御人护腕\n</Obj>"
// 		"<Obj color=0,255,0>.苏铰\n</Obj>"
// 		"</Seg>"
// 		"<Seg text-align=left float=wrap>"
// 		"<Obj color=0,255,0>.鬃. 灵 +10.</Obj>"
// 		"<Obj color=0,255,0>套装 力 +10.</Obj>"
// 		"<Obj color=150,150,150>(9) 套装 攻击致命一击率 +</Obj>"
// 		"</Seg>"
// 		"</Layout>";
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

void KUiItemTip::showCompare(char* layoutText)
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
	
	if(d_compareTipText->getLayout()->isHaveContent())
		d_compareTipText->show();
}

void KUiItemTip::adjustPos(CEGUI::Rect aroundArea, TipPos tipPos)
{
	Point	newPos = aroundArea.getPosition();

	Point	thisPos		= m_pThisWnd->getAbsolutePosition();
	int		thisHeight	= m_pThisWnd->getAbsoluteHeight();
	int		thisWidth	= m_pThisWnd->getAbsoluteWidth();
	
	switch(tipPos)
	{
	case KUiItemTip::Top:
		{
			newPos.d_y -= thisHeight;
		}
		break;
	case KUiItemTip::Bottom:
		{
			newPos.d_y += aroundArea.getHeight();
		}
		break;
	case KUiItemTip::Left:
		{
			newPos.d_x -= thisWidth;
		}
		break;
	case KUiItemTip::Right:
		{
			newPos.d_x += aroundArea.getWidth();			
		}
		break;
	case KUiItemTip::TopLeft:
		{
			newPos.d_y -= thisHeight;
			newPos.d_x -= thisWidth;
		}
		break;
	case KUiItemTip::TopRight:
		{
			newPos.d_y -= thisHeight;
			newPos.d_x += aroundArea.getWidth();
		}
		break;
	case KUiItemTip::BottomLeft:
		{
			newPos.d_y += aroundArea.getHeight();
			newPos.d_x -= thisWidth;
		}
		break;
	case KUiItemTip::BottomRight:
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


KUiItemTip::TipPos KUiItemTip::textToTipPos(char* text)
{
	if(text == NULL)
	{
		return Left;
	}

	if(!strcmp(text, "Left"))
	{
		return Left;
	}
	else if(!strcmp(text, "Right"))
	{
		return Right;
	}
	else if(!strcmp(text, "Top"))
	{
		return Top;
	}
	else if(!strcmp(text, "Bottom"))
	{
		return Bottom;
	}
	else if(!strcmp(text, "TopLeft"))
	{
		return TopLeft;
	}
	else if(!strcmp(text, "TopRight"))
	{
		return TopRight;
	}
	else if(!strcmp(text, "BottomLeft"))
	{
		return BottomLeft;
	}
	else if(!strcmp(text, "BottomRight"))
	{
		return Right;
	}
	
	return Left;
}
