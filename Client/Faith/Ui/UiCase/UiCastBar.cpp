#include "KWin32.h"
#include "GameDataDef.h"
#include "UiCastBar.h"
#include "../KMessageCentre.h"

using namespace CEGUI;

template<> 
KUiCastBar* KUiWndSingleton<KUiCastBar>::ms_Singleton	= NULL;

KUiCastBar::KUiCastBar(const CEGUI::String& id_name):
KUiWndSingleton<KUiCastBar>( id_name )
{
	d_curPercent = 0;
	d_leftTime = 0;
}

KUiCastBar::~KUiCastBar()
{
	
}

void KUiCastBar::getChild()
{
	if(!m_pThisWnd)
	{
		return;
	}

	m_pThisWnd->setRenderMode(true);
	d_progressbarHead = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/CastBar/ProgressBarHead");
	d_progressbarTail = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/CastBar/ProgressBarTail");

	d_progressbarBody = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/CastBar/ProgressBarBody");
	d_progressbarBody_Normal = (TLStaticImage*)d_progressbarBody->getChild("TaharezLook/CastBar/ProgressBarBody/Normal");
	d_progressbarBody_Complete = (TLStaticImage*)d_progressbarBody->getChild("TaharezLook/CastBar/ProgressBarBody/Complete");
	d_progressbarBody_Cancel = (TLStaticImage*)d_progressbarBody->getChild("TaharezLook/CastBar/ProgressBarBody/Cancel");

	d_progressbarFrame_Normal = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/CastBar/ProgressBarFrame");
	d_progressbarFrame_HighLight = (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/CastBar/HeighLightFrame");
	
	d_msg = (TLStaticText*)m_pThisWnd->getChild("TaharezLook/CastBar/Message");
	d_time = (TLStaticText*)m_pThisWnd->getChild("TaharezLook/CastBar/Time");
}

bool KUiCastBar::onNewFrame(const CEGUI::EventArgs& e)
{
	if(!m_pThisWnd)
	{
		return false;
	}

	FrameEventArgs* frame = (FrameEventArgs*)&e;
	int elapse = frame->elapse;
	
	if(d_leftTime > 0)
	{
		char leftTimeText[32] = "";
		sprintf(leftTimeText, "%.1f", d_leftTime / 1000);
		d_time->setText(leftTimeText);
		
		d_curPercent = d_curPercent + elapse * (100 - d_curPercent) / d_leftTime;
		
		d_progressbarBody->setWidth(Absolute, d_progressBarMaxLen * d_curPercent / 100);
		
		d_progressbarHead->setXPosition(Absolute, d_progressbarBody->getPosition(Absolute).d_x
			+ d_progressbarBody->getWidth(Absolute) - 5);
		d_leftTime -= elapse;
		d_fidoutLeftTime = FidoutTime;
	}
	else if(d_fidoutLeftTime > 0 && FidoutTime > 0)
	{
		float fidoutPercent = (float)d_fidoutLeftTime / FidoutTime;
		m_pThisWnd->setAlpha(fidoutPercent);
		d_progressbarBody->setAlpha(fidoutPercent);
		d_progressbarHead->setAlpha(fidoutPercent);
		d_progressbarTail->setAlpha(fidoutPercent);

		d_progressbarBody_Normal->setAlpha(fidoutPercent);
		d_progressbarBody_Complete->setAlpha(fidoutPercent);
		d_progressbarBody_Cancel->setAlpha(fidoutPercent);
		
		d_progressbarFrame_Normal->setAlpha(fidoutPercent);
		d_progressbarFrame_HighLight->setAlpha(fidoutPercent);
		d_fidoutLeftTime -= elapse;
	}
	else if(m_pThisWnd->isVisible())
	{
		m_pThisWnd->setVisible(false);
		m_pThisWnd->stopUpdate();
	}

	return true;
}


void KUiCastBar::cast(int curPercent, float leftTime, int msgCode)
{	
	if(!m_pThisWnd)
	{
		return;
	}
	
	Show();
	leftTime *= 1000;//传进来的是秒
	m_pThisWnd->beginUpdate();
	m_pThisWnd->setAlpha(1.0f);
	d_progressbarHead->setAlpha(1.0f);
	d_progressbarBody->setAlpha(1.0f);
	d_progressbarTail->setAlpha(1.0f);
	
	d_progressbarBody_Normal->setAlpha(1.0f);
	d_progressbarBody_Complete->setAlpha(1.0f);
	d_progressbarBody_Cancel->setAlpha(1.0f);
	
	d_progressbarFrame_Normal->setAlpha(1.0f);
	d_progressbarFrame_HighLight->setAlpha(1.0f);
	
	d_curPercent	= curPercent;
	d_leftTime		= leftTime;
	refreshMsg(msgCode);
	
	char leftTimeText[32] = "";
	sprintf(leftTimeText, "%.1f", d_leftTime / 1000);
	d_time->setText(leftTimeText);
		
	d_progressbarBody->setWidth(Absolute, d_progressBarMaxLen * d_curPercent / 100);
	
	d_progressbarHead->setXPosition(Absolute, d_progressbarBody->getPosition(Absolute).d_x
		+ d_progressbarBody->getWidth(Absolute));

	d_progressbarBody_Normal->show();
	d_progressbarBody_Complete->hide();
	d_progressbarBody_Cancel->hide();

	d_progressbarFrame_Normal->show();
	d_progressbarFrame_HighLight->hide();
}


void KUiCastBar::delay(int delayTime, int msgCode)
{
	if(!m_pThisWnd)
	{
		return;
	}

	if(d_leftTime <= 0 || d_curPercent >= 100)
		return;
	delayTime *= 1000;//传进来的是秒
	
	float newPercent = d_curPercent - delayTime * (100 - d_curPercent) / d_leftTime;
	if(newPercent >= 0)
	{
		d_curPercent = newPercent;
		d_leftTime += delayTime;
	}
	else
	{
		d_leftTime = d_leftTime * 100 / (100 - d_curPercent);
		d_curPercent = 0;
	}
	refreshMsg(msgCode);
}

void KUiCastBar::cancel(int msgCode)
{
	if(!m_pThisWnd)
	{
		return;
	}

	cast(d_curPercent, 0, msgCode);
	refreshMsg(-1);

	d_progressbarBody_Normal->hide();
	d_progressbarBody_Complete->hide();
	d_progressbarBody_Cancel->show();

	d_progressbarFrame_Normal->hide();
	d_progressbarFrame_HighLight->show();
}

void KUiCastBar::complete(int msgCode)
{
	if(!m_pThisWnd)
	{
		return;
	}

	cast(100, 0.001f, msgCode);
	refreshMsg(msgCode);

	d_progressbarBody_Normal->show();
	d_progressbarBody_Complete->hide();
	d_progressbarBody_Cancel->hide();

	d_progressbarFrame_Normal->show();
	d_progressbarFrame_HighLight->hide();
}

void KUiCastBar::refreshMsg(int msgCode)
{
	if(!m_pThisWnd || !d_msg)
	{
		return;
	}

	if(delayed_action_msg_none != msgCode)
	{
		char* message = KMessageCentre::GetMessage(castbar_message, msgCode);
		d_msg->setText(AnsiToUtf8(message));
	}
}

void KUiCastBar::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->getChild();
		
		ms_Singleton->d_progressBarMaxLen = ms_Singleton->m_pThisWnd->getWidth(Absolute)
			- ms_Singleton->d_progressbarBody->getPosition(Absolute).d_x - ms_Singleton->d_progressbarHead->getWidth(Absolute) - BoardLeftWidth;

//		ms_Singleton->m_pThisWnd->setHeight(Absolute, ms_Singleton->d_progressbarHead->getHeight(Absolute));
		TLStaticImage* thisWnd = (TLStaticImage*)ms_Singleton->m_pThisWnd;
		thisWnd->subscribeEvent(TLStaticImage::EventNewFrame, Event::Subscriber(&KUiCastBar::onNewFrame, ms_Singleton));
		thisWnd->subscribeEvent(TLStaticImage::EventHidden, Event::Subscriber(&KUiCastBar::onHide, ms_Singleton));
		thisWnd->hide();
	}
}
bool KUiCastBar::onHide(const CEGUI::EventArgs& e)
{
//	m_pThisWnd->stopUpdate();
	return true;
}
