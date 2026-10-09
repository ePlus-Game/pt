// UiHire.cpp: implementation of the KUiHire class.
//
//////////////////////////////////////////////////////////////////////

#include "GameDataDef.h"
#include "UiHire.h"
#include "UiDelayQuit.h"
#include "Coreshell.h"
#include "../KMessageCentre.h"
#include "UiErrorMessageBox.h"
#include "UiRecommend.h"
#include <sstream>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern iCoreShell* g_pCoreShell;

template<> 
KUiHire* KUiWndSingleton<KUiHire>::ms_Singleton = NULL;

template<> 
KUiHireConfigExp* KUiWndSingleton<KUiHireConfigExp>::ms_Singleton = NULL;


template<> 
KUiHireConfigSalary* KUiWndSingleton<KUiHireConfigSalary>::ms_Singleton = NULL;

KUiHire::KUiHire( const CEGUI::String& id_name )
: KUiWndSingleton<KUiHire>( id_name )
, MaxBarCount(8)
, BarFilename_Exp("uisettings/layouts/HirePageBar_Exp.ls")
, BarFilename_Fighter("uisettings/layouts/HirePageBar_Fighter.ls")
, m_curExpPage(0)
, m_curFightPage(0)
, m_curExpPageItemCount(0)
, m_curFightPageItemCount(0)
, m_selMetier(enMetierSelectAll)
{
	
}

KUiHire::~KUiHire()
{

}

void KUiHire::Show()
{
	KUiWndSingleton<KUiHire>::Show();

	if(ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		//打开清空
		ms_Singleton->clearExpContent();
		ms_Singleton->clearFighterContent();
		
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeLow")->setText("0");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeHigh")->setText(PropertyHelper::intToString(MAX_LEVEL));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Hire/edtName")->setText("");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Hire/Shizu")->setText("");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/Hire/Zhuhou")->setText("");
		
		ms_Singleton->m_pMenuSelectMetier->hide();
		ms_Singleton->m_selMetier = enMetierSelectAll;
		ms_Singleton->m_pTxtMetier->setText( ms_Singleton->m_pBtnAll->getText() );
		
		ms_Singleton->m_expPageHaveNext		= true;
		ms_Singleton->m_fightPageHaveNext	= true;
		
		ms_Singleton->m_curExpPage = 0;
		ms_Singleton->m_curFightPage = 0;

		ms_Singleton->search(ms_Singleton->m_curExpPage, HT_EXP);

		ms_Singleton->m_pPage_Exp->show();
		ms_Singleton->m_pPage_Fighter->hide();
		((TLRadioButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/Hire/btnExpHire"))->setSelected(true);
	}
}

void KUiHire::Hide()
{
	KUiWndSingleton<KUiHire>::Hide();	
}

void KUiHire::Breathe()
{
	
}

void KUiHire::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		//初始化下拉列表
		m_pMenuSelectMetier = static_cast<StaticImage*>(m_pThisWnd->getChild("TaharezLook/Hire/menuMetier"));
		m_pMenuSelectMetier->hide();

		m_pBtnAll = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/All"));
		m_pBtnXuanfeng = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/xuanfeng"));
		m_pBtnXingtian = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/xingtian"));
		m_pBtnZhenren = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/zhenren"));
		m_pBtnTianshi = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/tianshi"));
		m_pBtnShoushi = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/shoushi"));
		m_pBtnYishi = static_cast<RadioButton*>(m_pMenuSelectMetier->getChild("TaharezLook/Hire/menuMetier/yishi"));

		m_pBtnAll->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));
		m_pBtnXuanfeng->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));
		m_pBtnXingtian->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));
		m_pBtnZhenren->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));
		m_pBtnTianshi->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));
		m_pBtnShoushi->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));
		m_pBtnYishi->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::menuSelectMetier_MouseClick, ms_Singleton));

		m_KindMap[m_pBtnAll] = enMetierSelectAll;
		m_KindMap[m_pBtnXuanfeng] = enMetierSelectXuanfeng;
		m_KindMap[m_pBtnXingtian] = enMetierSelectXingtian;
		m_KindMap[m_pBtnZhenren] = enMetierSelectZhenren;
		m_KindMap[m_pBtnTianshi] = enMetierSelectTianshi;
		m_KindMap[m_pBtnShoushi] = enMetierSelectShoushi;
		m_KindMap[m_pBtnYishi] = enMetierSelectYishi;
		
		m_pThisWnd->getChild("TaharezLook/Hire/btnMetierSelect")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiHire::btnSelectMetier_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/Hire/Metier")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnSelectMetier_MouseClick, ms_Singleton));

		m_pTxtMetier = static_cast<StaticText*>(m_pThisWnd->getChild("TaharezLook/Hire/Metier"));
		
		//经验佣兵页签和战斗佣兵页签
		m_pPage_Exp = static_cast<StaticImage*>(m_pThisWnd->getChild("TaharezLook/Hire/Page"));
		m_pPage_Fighter = static_cast<StaticImage*>(m_pThisWnd->getChild("TaharezLook/Hire/FighterPage"));

		//页签按钮的事件注册
		m_pThisWnd->getChild("TaharezLook/Hire/btnExpHire")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnExpHire_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/Hire/btnFighterHire")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnFighterHire_MouseClick, ms_Singleton));
		
		InitBars(*m_pPage_Exp, BarFilename_Exp, m_vBarList);
		InitBars(*m_pPage_Fighter, BarFilename_Fighter, m_vFighterBarList);

		//功能按钮事件注册
		m_pThisWnd->getChild("TaharezLook/Hire/btnSearch")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnSearch_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/Hire/btnPrev")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnPrev_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/Hire/btnNext")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnNext_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/Hire/btnHire")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnHire_MouseClick, ms_Singleton));
	
		m_pThisWnd->getChild("TaharezLook/Hire/Close")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHire::btnCloseBtn_MouseClick, ms_Singleton));

		//初始化显示窗口
		m_pPage_Fighter->hide();
		m_pPage_Exp->show();

		m_pPage_ExpBackImg = m_pThisWnd->getChild("TaharezLook/Hire/ExpBackImg");
		m_pPage_FighterBackImg = m_pThisWnd->getChild("TaharezLook/Hire/FighterBackImg");
		m_pPage_ExpBackImg->show();
		m_pPage_FighterBackImg->hide();

		m_pEdit_Shizu = m_pThisWnd->getChild("TaharezLook/Hire/Shizu");
		m_pEdit_Zhuhou = m_pThisWnd->getChild("TaharezLook/Hire/Zhuhou");
		m_pTxt_Shizu = m_pThisWnd->getChild("TaharezLook/Hire/ShizuTitle");
		m_pTxt_Zhuhou = m_pThisWnd->getChild("TaharezLook/Hire/ZhuhouTitle");
		m_pEdit_Shizu->show();
		m_pEdit_Zhuhou->show();
		m_pTxt_Shizu->show();
		m_pTxt_Zhuhou->show();

		//初始化数值
		((Editbox*)m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeLow"))->setNumberOnly(true);
		((Editbox*)m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeHigh"))->setNumberOnly(true);
		((Editbox*)m_pThisWnd->getChild("TaharezLook/Hire/edtName"))->setNumberOnly(false);
		((Editbox*)m_pThisWnd->getChild("TaharezLook/Hire/Zhuhou"))->setNumberOnly(false);
		((Editbox*)m_pThisWnd->getChild("TaharezLook/Hire/Shizu"))->setNumberOnly(false);
	}	
}
//职业菜单
bool KUiHire::btnSelectMetier_MouseClick( const CEGUI::EventArgs& args )
{
	if ( NULL != m_pMenuSelectMetier && !m_pMenuSelectMetier->isVisible() )
	{
		m_pMenuSelectMetier->moveToFront();
		m_pMenuSelectMetier->show();
	}
	else
	{
		m_pMenuSelectMetier->hide();
	}

	return true;	
}

bool KUiHire::menuSelectMetier_MouseClick( const CEGUI::EventArgs& args )
{
	WindowEventArgs* wargs = static_cast<WindowEventArgs*>(const_cast<EventArgs*>(&args));
	RadioButton* tmpWnd = static_cast<RadioButton*>(wargs->window);

	if(m_KindMap.find(tmpWnd) == m_KindMap.end())
	{
		return true;
	}
	m_selMetier = (MetierSelection)m_KindMap[tmpWnd];

	if ( NULL == tmpWnd || m_selMetier < 0 || m_selMetier >= enMetierSelectCount )
	{
		return false;
	}

	if ( NULL != m_pTxtMetier && NULL != m_pMenuSelectMetier )
	{
		m_pTxtMetier->setText( tmpWnd->getText() );		
		m_pMenuSelectMetier->hide();
	}
	return true;
}

void KUiHire::InitBars( StaticImage& page, const CEGUI::String& layoutFileName, vector<Window *>& barList )
{
	barList.clear();
	if ( NULL != &page )
	{
		ostringstream barPrefix;
		int titleHeight = page.getChild(page.getName() + "/NameTitle")->getSize(Absolute).d_height;
		int barHeight = -1;
		Point curBarPosition;

		for ( int i = 0; i < MaxBarCount; ++i )
		{
			barPrefix.str("");
			barPrefix<<page.getName()<<'_'<<i<<'_';
			Window* pCurrentBar= m_pWindowManager->loadWindowLayout(layoutFileName, barPrefix.str(), "", NULL, NULL, true);
			if ( NULL != pCurrentBar )
			{	
				if ( -1 == barHeight )
				{
					//由于使用模板，所以所有的bar刚load的时候是一样高的
					barHeight = pCurrentBar->getSize(Absolute).d_height;
				}
				curBarPosition.d_x = 0;
				curBarPosition.d_y = titleHeight + i * barHeight;
				pCurrentBar->subscribeEvent(StaticImage::EventMouseClick, Event::Subscriber(&KUiHire::btnPageBar_MouseClick, this));
				pCurrentBar->setPosition(Absolute, curBarPosition);
				page.addChildWindow(pCurrentBar);
				barList.push_back(pCurrentBar);
			}
		}
	}
}

void KUiHire::search(int pageIndex, HireType type)
{
	HireReqData data;
	data.metier = m_selMetier;
	data.startIndex = pageIndex * MaxBarCount;
	data.length = MaxBarCount;
	data.lowLevel = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeLow")->getText());
	data.highLevel = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeHigh")->getText());
	if(data.highLevel == 0)
	{
		data.highLevel = MAX_LEVEL;
		m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeHigh")->setText(PropertyHelper::intToString(MAX_LEVEL));
	}
	
	strncpy(data.name, Utf8ToAnsi(m_pThisWnd->getChild("TaharezLook/Hire/edtName")->getText()), sizeof(data.name));
	data.hiretype = type;
	strncpy(data.shizu, Utf8ToAnsi(m_pThisWnd->getChild("TaharezLook/Hire/Shizu")->getText()), sizeof(data.shizu));
	strncpy(data.zhuhou, Utf8ToAnsi(m_pThisWnd->getChild("TaharezLook/Hire/Zhuhou")->getText()), sizeof(data.zhuhou));

	g_pCoreShell->OperationRequest(GOI_HIRE_SEND_DATA_REQ, (uint)&data, NULL);
}

bool KUiHire::btnSearch_MouseClick( const CEGUI::EventArgs& args )
{
	if(m_pPage_Exp->isVisible())
	{
		search(m_curExpPage, HT_EXP);
	}
	else
	{
		search(m_curFightPage, HT_FIGHT);
	}

	return true;	
}

bool KUiHire::btnPrev_MouseClick( const CEGUI::EventArgs& args )
{	
	if(m_pPage_Exp->isVisible())
	{
		if(m_curExpPage - 1 >= 0)
		{
			search(m_curExpPage - 1, HT_EXP);
		}
	}
	else
	{
		if(m_curFightPage - 1 >= 0)
		{
			search(m_curFightPage - 1, HT_FIGHT);
		}
	}

	return true;
}

bool KUiHire::btnNext_MouseClick( const CEGUI::EventArgs& args )
{
	if(m_pPage_Exp->isVisible())
	{
		if(m_expPageHaveNext)
			search(m_curExpPage + 1, HT_EXP);
	}
	else
	{
		if(m_fightPageHaveNext)
			search(m_curFightPage + 1, HT_FIGHT);
	}

	return true;	
}

//点击关闭
bool KUiHire::btnCloseBtn_MouseClick( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

//点击雇佣
bool KUiHire::btnHire_MouseClick( const CEGUI::EventArgs& args )
{
	//m_selMoney是每小时的时间
	if(m_selMoney != -1 && m_selMoney / 60 > g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0))
	{
		char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_NOT_ENOUGH_MONEY);
		KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
		return true;
	}

	//战斗雇佣本人的时间至少是1分钟
	int employTime = g_pCoreShell->GetGameData( GDI_GET_EMPLOY_TIME, NULL, NULL );
	if(m_pPage_Fighter->isVisible() && employTime < HireMinFighterHireTime)
	{
		char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_EMPLOYER_OUT_OF_EMPLOY_TIME);
		KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
		return true;
	}


	int lowLevel = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeLow")->getText());
	int heighLevel = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/Hire/LevelRangeHigh")->getText());

	bool isExpPanel = m_pPage_Exp->isVisible();
	
	char name[COMMON_CLIENT_MSG_LEN_32];
	strncpy(name, Utf8ToAnsi(m_name), sizeof(name));
	g_pCoreShell->OperationRequest(GOI_HIRE_SEND_HIRE_REQ, (uint)name, NULL);
	return true;
}

//切换exp
bool KUiHire::btnExpHire_MouseClick( const CEGUI::EventArgs& args )
{
	if ( NULL != m_pPage_Fighter && NULL != m_pPage_Exp )
	{
		if(!m_pPage_Exp->isVisible())
		{
			m_pPage_Fighter->hide();
			m_pPage_Exp->show();
			m_pPage_ExpBackImg->show();
			m_pPage_FighterBackImg->hide();
			
			m_pEdit_Shizu->show();
			m_pEdit_Zhuhou->show();
			m_pTxt_Shizu->show();
			m_pTxt_Zhuhou->show();
			if(!m_curExpPageItemCount)
			{
				search(0, HT_EXP);
			}
		}
	}
	return true;
}

//切换fighter
bool KUiHire::btnFighterHire_MouseClick( const CEGUI::EventArgs& args )
{
	if ( NULL != m_pPage_Fighter && NULL != m_pPage_Exp )
	{
		if(!m_pPage_Fighter->isVisible())
		{
			m_pPage_Exp->hide();
			m_pPage_Fighter->show();
			m_pPage_ExpBackImg->hide();
			m_pPage_FighterBackImg->show();
		
			m_pEdit_Shizu->hide();
			m_pEdit_Zhuhou->hide();
			m_pTxt_Shizu->hide();
			m_pTxt_Zhuhou->hide();
			if(!m_curFightPageItemCount)
			{
				search(0, HT_FIGHT);
			}
		}
	}
	return true;
}

const char* getMetierText(Metier metier)
{
	static char retText[COMMON_CLIENT_MSG_LEN_32];
	retText[0] = 0;

	if(metier.major == 0)
	{
		if(metier.minor	== (BYTE)-1)
		{
			strcpy(retText, ROLE_CAREER_JS);
		}
		else if(metier.minor == 0)
		{
			strcpy(retText, ROLE_CAREER_JS_1);
		}
		else if(metier.minor == 1)
		{
			strcpy(retText, ROLE_CAREER_JS_0);
		}
	}
	else if(metier.major == 1)
	{
		if(metier.minor	== (BYTE)-1)
		{
			strcpy(retText, ROLE_CAREER_DS);
		}
		else if(metier.minor == 0)
		{
			strcpy(retText, ROLE_CAREER_DS_1);
		}
		else if(metier.minor == 1)
		{
			strcpy(retText, ROLE_CAREER_DS_0);
		}

	}
	else if(metier.major == 2)
	{
		if(metier.minor	== (BYTE)-1)
		{
			strcpy(retText, ROLE_CAREER_YR);
		}
		else if(metier.minor == 0)
		{
			strcpy(retText, ROLE_CAREER_YR_0);
		}
		else if(metier.minor == 1)
		{
			strcpy(retText, ROLE_CAREER_YR_1);
		}

	}

	return retText;
}

const char* getDualityNumberText(DualityNumber number)
{
	static char text[COMMON_CLIENT_MSG_LEN_32];
	sprintf(text, "%d-%d", number.low, number.high);
	
	return text;
}

void KUiHire::setExpContent(int index, BYTE sex, char* name, Metier metier, int level, char* shizu, char* zhuhou)
{
	if(index < 0 || index >= m_vBarList.size())
	{
		return;
	}
	
	if(!m_vBarList[index])
	{
		return;
	}

	if(strlen(name) == 0)
	{
		m_vBarList[index]->hide();
		return;
	}

	m_vBarList[index]->show();

	String barName = m_vBarList[index]->getName();
	m_vBarList[index]->getChild(barName + "/Name")->setText(AnsiToUtf8(name));
	m_vBarList[index]->getChild(barName + "/Sex")->setText(AnsiToUtf8((sex == 0) ? MALE : FEMALE));
	m_vBarList[index]->getChild(barName + "/Metier")->setText(AnsiToUtf8(getMetierText(metier)));
	m_vBarList[index]->getChild(barName + "/Level")->setText(PropertyHelper::intToString(level));
	m_vBarList[index]->getChild(barName + "/Shizu")->setText(AnsiToUtf8(shizu));
	m_vBarList[index]->getChild(barName + "/Zhuhou")->setText(AnsiToUtf8(zhuhou));
	m_vBarList[index]->setUserString("Name", AnsiToUtf8(name));

	m_vBarList[index]->setUserString("Money", "-1");
}

void KUiHire::clearExpContent()
{
	for(int index = 0; index < m_vFighterBarList.size(); ++index)
	{
		setExpContent(index, 1, "", Metier(), 0, "", "");
	}
}

void KUiHire::setFighterContent(int index, char* name, Metier profession, int level, DualityNumber attack,
								DualityNumber magic, int hujia, int blood, int j, int y, int t)
{
	if(index < 0 || index >= m_vFighterBarList.size())
	{
		return;
	}
	
	if(!m_vFighterBarList[index])
	{
		return;
	}

	if(strlen(name) == 0)
	{
		m_vFighterBarList[index]->hide();
		return;
	}

	m_vFighterBarList[index]->show();

	String barName = m_vFighterBarList[index]->getName();
	m_vFighterBarList[index]->getChild(barName + "/Name")->setText(AnsiToUtf8(name));
	m_vFighterBarList[index]->getChild(barName + "/Metier")->setText(AnsiToUtf8(getMetierText(profession)));
	m_vFighterBarList[index]->getChild(barName + "/Level")->setText(PropertyHelper::intToString(level));
	m_vFighterBarList[index]->getChild(barName + "/AttackPower")->setText(AnsiToUtf8(getDualityNumberText(attack)));
	m_vFighterBarList[index]->getChild(barName + "/Zhoufa")->setText(AnsiToUtf8(getDualityNumberText(magic)));
	m_vFighterBarList[index]->getChild(barName + "/Armor")->setText(PropertyHelper::intToString(hujia));
	m_vFighterBarList[index]->getChild(barName + "/Blood")->setText(PropertyHelper::intToString(blood));
	m_vFighterBarList[index]->getChild(barName + "/Salary_j")->setText(PropertyHelper::intToString(j));
	m_vFighterBarList[index]->getChild(barName + "/Salary_y")->setText(PropertyHelper::intToString(y));
	m_vFighterBarList[index]->getChild(barName + "/Salary_t")->setText(PropertyHelper::intToString(t));
	
	m_vFighterBarList[index]->setUserString("Name", AnsiToUtf8(name));
	m_vFighterBarList[index]->setUserString("Money", PropertyHelper::intToString(j * 10000 + y * 100 + t));
}


void KUiHire::clearFighterContent()
{
	for(int index = 0; index < m_vFighterBarList.size(); ++index)
	{
		setFighterContent(index, "", Metier(), 0, DualityNumber(), DualityNumber(), 0, 0, 0, 0, 0);
	}
}

void KUiHire::updateFighter(vector<FighterHirer>& fitherData, int recordStart)
{
// 	if(fitherData.size() && fitherData[0].name[0] != 0)
// 	{
// 		m_curFightPage = m_requestFightPage > 0 ? m_requestFightPage : 0;
// 	}
// 	else
// 	{
// 		return;
	// 	}
	
	m_fightPageHaveNext = false;
	m_curFightPage = recordStart / MaxBarCount;
	if(fitherData.size() && fitherData[0].name[0] != 0)
	{
		m_fightPageHaveNext = true;
		m_pThisWnd->getChild("TaharezLook/Hire/btnHire")->enable();
	}
	else
	{
		//未返回任何条目
		if(m_pPage_Fighter->isVisible())
		{
			m_pThisWnd->getChild("TaharezLook/Hire/btnHire")->disable();
			char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_NO_RECORD);
			KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
		}
	}
	clearFighterContent();

	int index = 0;
	for(int i = 0; i < fitherData.size(); ++i)
	{
		FighterHirer& hirer = fitherData[i];
	
		hirer.salary *= 3600;//秒薪转换成小时薪
		setFighterContent(i, hirer.name, hirer.metier, hirer.level, 
			hirer.attack, hirer.magic, hirer.armor, hirer.blood, 
			hirer.salary / 10000, (hirer.salary % 10000) / 100, hirer.salary % 100);
	}
	m_curFightPageItemCount = fitherData.size();
}

void KUiHire::updateExp(vector<ExpHirer>& fitherData, int recordStart)
{
// 	if(fitherData.size() && fitherData[0].name[0] != 0)
// 	{
// 		m_curExpPage = m_requestExpPage > 0 ? m_requestExpPage : 0;
// 	}
// 	else
// 	{
// 		return;
// 	}

	m_expPageHaveNext = false;
	m_curExpPage = recordStart / MaxBarCount;
	if(fitherData.size() && fitherData[0].name[0] != 0)
	{
		m_expPageHaveNext = true;
		m_pThisWnd->getChild("TaharezLook/Hire/btnHire")->enable();
	}
	else
	{
		//未返回任何条目
		if(m_pPage_Exp->isVisible())
		{
			m_pThisWnd->getChild("TaharezLook/Hire/btnHire")->disable();
			char* message = KMessageCentre::GetMessage(hire_op_message, KUiHire::HOM_NO_RECORD);
			KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(message));
		}
	}
	clearExpContent();

	int index = 0;
	for(int i = 0; i < fitherData.size(); ++i)
	{
		ExpHirer& hirer = fitherData[i];
		setExpContent(i, hirer.sex, hirer.name, hirer.metier, hirer.level, hirer.shizu, hirer.zhuhou);
	}
	m_curExpPageItemCount = fitherData.size();
}

void KUiHire::clear()
{
	if(m_pPage_Exp->isVisible())
	{
		clearExpContent();
	}
	else
	{
		clearFighterContent();
	}
}

//点击bar的事件
bool KUiHire::btnPageBar_MouseClick( const CEGUI::EventArgs& args )
{
	WindowEventArgs* wargs = static_cast<WindowEventArgs*>(const_cast<EventArgs*>(&args));
	if(m_pPage_Exp->isVisible())
	{
		RefreshBars(m_vBarList, *wargs->window);
	}
	else
	{
		RefreshBars(m_vFighterBarList, *wargs->window);
	}
	return true;
}

void KUiHire::RefreshBars( vector<Window *>& barList, const Window& activeBar )
{
	CEGUI::String hoverName;
	for ( int i = 0; i < barList.size(); ++i )
	{
		if ( 0 == barList[i]->getName().compare(activeBar.getName()) )
		{
			hoverName = activeBar.getName() + "/Hover";
			activeBar.getChild(hoverName)->setVisible(true);
			activeBar.getChild(hoverName)->setEnabled(false);
			m_name = activeBar.getUserString("Name");
			m_selMoney = PropertyHelper::stringToInt(activeBar.getUserString("Money"));
		}
		else
		{
			hoverName = barList[i]->getName() + "/Hover";
			barList[i]->getChild(hoverName)->setVisible(false);			
			barList[i]->getChild(hoverName)->setEnabled(true);
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
//
//	KUiHireConfig实现
//
//////////////////////////////////////////////////////////////////////////

void KUiHireConfigExp::showEmpployTime()
{
	int employTime = g_pCoreShell->GetGameData( GDI_GET_EMPLOY_TIME, NULL, NULL );

	int multiple = 1;
	switch(m_curSelType)
	{
	case EHT_NORMAL:
		{
			multiple = 1;
		}
		break;
	case EHT_FAST:
		{
			multiple = 2;
		}
		break;
	case EHT_VERY_FAST:
		{
			multiple = 4;
		}
		break;
	}

	employTime = employTime / multiple;

	char timeText[COMMON_CLIENT_MSG_LEN_32];
	sprintf(timeText, "%d%s%d%s", employTime / 3600, HOUR, (employTime / 60) % 60, MINITE);
	m_pThisWnd->getChild("TaharezLook/HireConfig/Time")->setText(AnsiToUtf8(timeText));
}

void KUiHireConfigExp::Show()
{
	KUiWndSingleton<KUiHireConfigExp>::Show();
	
	ms_Singleton->m_curSelType = EHT_NORMAL;
	ms_Singleton->showEmpployTime();
	ms_Singleton->m_expHireTypeBtn[EHT_NORMAL]->setSelected(true);
	ms_Singleton->m_expHireTypeBtn[EHT_FAST]->setSelected(false);
	ms_Singleton->m_expHireTypeBtn[EHT_VERY_FAST]->setSelected(false);
}

void KUiHireConfigExp::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/HireConfig/btnOK")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHireConfigExp::btnOK_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireConfig/btnCancel")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHireConfigExp::btnCancel_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireConfig/btnClose")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHireConfigExp::btnCancel_MouseClick, ms_Singleton));

		m_expHireTypeBtn[EHT_NORMAL] = (TLRadioButton*)m_pThisWnd->getChild("TaharezLook/HireConfig/HeightRadio1");
		m_expHireTypeBtn[EHT_FAST] = (TLRadioButton*)m_pThisWnd->getChild("TaharezLook/HireConfig/HeightRadio");
		m_expHireTypeBtn[EHT_VERY_FAST] = (TLRadioButton*)m_pThisWnd->getChild("TaharezLook/HireConfig/LowRadio");
		for(int i = 0; i < EHT_TYPE_COUNT; ++i)
		{
			m_expHireTypeBtn[i]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiHireConfigExp::btnHireType_MouseClick, this));
		}
		m_pThisWnd->setZLevel(Window::Top);
		m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiHireConfigExp::window_hiden, this));
	}
}


KUiHireConfigExp::KUiHireConfigExp( const CEGUI::String& id_name )
: KUiWndSingleton<KUiHireConfigExp>( id_name )
{
	m_curSelType = EHT_NORMAL;
}

KUiHireConfigExp::~KUiHireConfigExp()
{
	
}

bool KUiHireConfigExp::btnOK_MouseClick( const CEGUI::EventArgs& args )
{
	g_pCoreShell->OperationRequest(GOI_HIRE_SEND_WANT_TO_BE_HIRED_REQ, (uint)HT_EXP, (int)m_curSelType);
	Hide();
	return true;
}

bool KUiHireConfigExp::btnCancel_MouseClick( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

bool KUiHireConfigExp::btnHireType_MouseClick( const CEGUI::EventArgs& args )
{
	WindowEventArgs* windowArg = (WindowEventArgs*)&args;
	for(int i = 0; i < EHT_TYPE_COUNT; ++i)
	{
		if(windowArg->window == m_expHireTypeBtn[i])
		{
			m_curSelType = (ExpHireType)i;
			showEmpployTime();
			break;
		}
	}
	return true;
}

bool KUiHireConfigExp::window_hiden( const CEGUI::EventArgs& args )
{
	KUiDeleyQuit::GetSingleton().setTimerAct(true);
	return true;
}

/********************************************************************
/*						class: KUiHireSalary 实现
*********************************************************************/

void KUiHireConfigSalary::Init()
{
	if ( NULL != ms_Singleton && NULL != ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/HireSalary/btnOK")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHireConfigSalary::btnOK_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireSalary/btnCancel")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHireConfigSalary::btnCancel_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireSalary/btnClose")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiHireConfigSalary::btnCancel_MouseClick, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_j")->subscribeEvent(Window::EventTextChanged, Event::Subscriber(&KUiHireConfigSalary::input_TextChanged, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_y")->subscribeEvent(Window::EventTextChanged, Event::Subscriber(&KUiHireConfigSalary::input_TextChanged, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_t")->subscribeEvent(Window::EventTextChanged, Event::Subscriber(&KUiHireConfigSalary::input_TextChanged, ms_Singleton));
		m_pThisWnd->setZLevel(Window::Top);
		m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiHireConfigSalary::window_hiden, this));
	}
}

bool KUiHireConfigSalary::input_TextChanged( const CEGUI::EventArgs& args )
{
	int j = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_j")->getText());
	int y = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_y")->getText());
	int t = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_t")->getText());
	
	int money = j * 10000 + y * 100 + t;
	money *= 3600;
	char* moenyLayout = KUiRecommend::getMoneyLayout(money, 0xffffffff);
	TLStaticText* hourMoneyText = (TLStaticText*)m_pThisWnd->getChild("TaharezLook/HireSalary/HourMoney");
	hourMoneyText->useLayout();
	hourMoneyText->getLayout()->SetText(moenyLayout);
	
	return true;
}

bool KUiHireConfigSalary::btnOK_MouseClick( const CEGUI::EventArgs& args )
{
	int j = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_j")->getText());
	int y = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_y")->getText());
	int t = PropertyHelper::stringToInt(m_pThisWnd->getChild("TaharezLook/HireSalary/Salary_t")->getText());
	
	int money = j * 10000 + y * 100 + t;

	money = money > 0 ? money : 1;
	g_pCoreShell->OperationRequest(GOI_HIRE_SEND_WANT_TO_BE_HIRED_REQ, (uint)HT_FIGHT, money);
	Hide();
	return true;
}

bool KUiHireConfigSalary::btnCancel_MouseClick( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

KUiHireConfigSalary::KUiHireConfigSalary( const CEGUI::String& id_name )
: KUiWndSingleton<KUiHireConfigSalary>( id_name )
{
	
}

KUiHireConfigSalary::~KUiHireConfigSalary()
{
	
}

bool KUiHireConfigSalary::window_hiden( const CEGUI::EventArgs& args )
{
	KUiDeleyQuit::GetSingleton().setTimerAct(true);
	return true;
}