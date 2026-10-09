#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiFuryBox.h"
#include "CoreShell.h"
#include "CEGUIEvent.h"

extern iCoreShell* g_pCoreShell;
using namespace CEGUI;

#define FURY_EFFECT_DX 50
#define FURY_EFFECT_DY 0

#define FURY_POS_X_FOR_1024  501
#define FURY_POS_Y_FOR_1024  688


template<> 
KUiFuryBox*     KUiWndSingleton<KUiFuryBox>::ms_Singleton	= NULL;

KUiFuryBox::KUiFuryBox(const CEGUI::String& id_name )
:KUiWndSingleton<KUiFuryBox>(id_name),d_CurExp(0)
{

}

KUiFuryBox::~KUiFuryBox()
{

}

void KUiFuryBox::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		m_pThisWnd->setVisible(true);
		m_pThisWnd->setZLevel(Window::SuperBottom);
		m_pThisWnd->setRenderMode(true);
		m_pThisWnd->SetBottomWindow();

        ms_Singleton->d_FuryAnimation = (TLStaticImage *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/FuryBox/FuryExpMovie");
		const Image    *     pImage   = ms_Singleton->d_FuryAnimation->getImage();
        Imageset       *     pImageset= (Imageset       *)pImage->getImageset();
        KSprite        *     pSpr     = pImageset->getSpr();
		
		ms_Singleton->d_TotalFuryAniFrame    = pSpr->GetFrames();
		ms_Singleton->d_FuryHover            = (TLStaticImage *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/FuryBox/HoverFury");
        ms_Singleton->d_FuryHover ->hide();

		pImage   = ms_Singleton->d_FuryHover->getImage();
        pImageset= (Imageset       *)pImage->getImageset();
        pSpr     = pImageset->getSpr();
		
		ms_Singleton->d_TotalFuryHoverFrame  = pSpr->GetFrames();
		ms_Singleton->d_ClickRect            =(TLButton *)ms_Singleton->m_pThisWnd->getChild("TaharezLook/FuryBox/ClickRect");
		ms_Singleton->d_ClickRect->setZLevel(Window::Top);
		ms_Singleton->d_ClickRect->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiFuryBox::HandleMouseClick, this));
        ms_Singleton->d_ClickRect->disable();
		ms_Singleton->m_pThisWnd->disable();

		char szBuff[128];
		szBuff[0];
		for (int i=0;i<10;i++)
		{ 
			sprintf(szBuff,"TaharezLook/FuryBox/Num%d",i);
			m_pThisWnd->getChild(szBuff)->setZLevel(Window::SuperTop);
			m_pThisWnd->getChild(szBuff)->disable();
			m_pThisWnd->getChild(szBuff)->hide();
		}//end for i

	}//endif
}

void KUiFuryBox::Update(const int nExp)
{
	if (ms_Singleton)
	{
		if (ms_Singleton->d_CurExp != nExp)
		{
			//Check the state
			if (nExp == 100 && ms_Singleton->d_CurExp<100)  //HoverBlood
			{
				ms_Singleton->d_ClickRect->enable();
				ms_Singleton->m_pThisWnd->enable();
                ms_Singleton->d_FuryHover->setCyc(true);
				ms_Singleton->d_FuryHover->play();
                ms_Singleton->d_FuryHover->show();

				if (g_GetScreenWidth() == 1024)
				{
					int nPosNew[2];
					nPosNew[0] = FURY_POS_X_FOR_1024;
					nPosNew[1] = FURY_POS_Y_FOR_1024;
					
					g_pCoreShell->OperationRequest(GOI_SET_EFFECT_POS,6,(int)nPosNew);
				}//endif

				g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT,6,0);
			}//endif

			if (ms_Singleton->d_CurExp == 100 && nExp<100 ) //UnHoverBlood
			{
				if (ms_Singleton->d_FuryHover->isVisible(true))
				{
					int nCurFrameIndex = ms_Singleton->d_FuryHover->getCurFrameIdx();
					ms_Singleton->d_ClickRect->disable();
					ms_Singleton->m_pThisWnd->disable();
					ms_Singleton->d_FuryHover->setCyc(false);
					ms_Singleton->d_FuryHover->play(nCurFrameIndex,ms_Singleton->d_TotalFuryHoverFrame-1,true);
				}//endif
				
				ms_Singleton->HideAllWarningNum();
			}//endif

			ms_Singleton->d_CurExp = nExp;
			ms_Singleton->RefreshAnimation();
		}//endif
	}//endif
}

void KUiFuryBox::RefreshAnimation()
{
	int nNewFrame = (d_CurExp * (d_TotalFuryAniFrame - 1)) / 101  ;
	int nCurFrame = d_FuryAnimation->getCurFrameIdx();

    if (nCurFrame!=nNewFrame)
	{
		if (nCurFrame <= d_TotalFuryAniFrame-1 && nNewFrame<nCurFrame)
		{
            d_FuryAnimation->play(0,nNewFrame);
		}
		else
			d_FuryAnimation->play(nCurFrame,nNewFrame);
	}//endif
	
}

bool KUiFuryBox::HandleMouseClick(const EventArgs& args)
{
	if (d_CurExp == 100)
	{
		if (g_GetScreenWidth() == 1024)
		{
			int nPosNew[2];
			nPosNew[0] = FURY_POS_X_FOR_1024;
			nPosNew[1] = FURY_POS_Y_FOR_1024;
			
			g_pCoreShell->OperationRequest(GOI_SET_EFFECT_POS,5,(int)nPosNew);
		}//endif
		
		g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT,5,0);

        //d_FuryAnimation->Shake(false,true,10);
		d_ClickRect->disable();
		ms_Singleton->m_pThisWnd->disable();

        g_pCoreShell->OperationRequest(GOI_FURY_EXPLODE,0,0);
	}//endif

	return true;
}

void KUiFuryBox::Warnning(const int nNum)
{
    ms_Singleton->ShowWarningNum(nNum);
}

void KUiFuryBox::HideAllWarningNum()
{
	char szBuff[128];
	szBuff[0];
    for (int i=0;i<10;i++)
	{ 
		sprintf(szBuff,"TaharezLook/FuryBox/Num%d",i);
        m_pThisWnd->getChild(szBuff)->hide();
	}//end for i
}

void KUiFuryBox::ShowWarningNum(int nNum)
{
	char szBuff[128];
	szBuff[0];

	for (int i=0;i<10;i++)
	{ 
		sprintf(szBuff,"TaharezLook/FuryBox/Num%d",i);
		if (i == nNum)
			m_pThisWnd->getChild(szBuff)->show();
		else
            m_pThisWnd->getChild(szBuff)->hide();
	}//end for i
}

void KUiFuryBox::AutoFury()
{
	if (ms_Singleton->d_CurExp == 100)
	{
		if (g_GetScreenWidth() == 1024)
		{
			int nPosNew[2];
			nPosNew[0] = FURY_POS_X_FOR_1024;
			nPosNew[1] = FURY_POS_Y_FOR_1024;
			
			g_pCoreShell->OperationRequest(GOI_SET_EFFECT_POS,5,(int)nPosNew);
		}//endif
		
		g_pCoreShell->OperationRequest(GOI_PLAY_EFFECT,5,0);
		
        //d_FuryAnimation->Shake(false,true,10);
		ms_Singleton->d_ClickRect->disable();
		ms_Singleton->m_pThisWnd->disable();
		
        g_pCoreShell->OperationRequest(GOI_FURY_EXPLODE,0,0);
	}//endif
}