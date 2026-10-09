//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/29/2007 10:20
//      File_base        : UiTeamViewer
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "CoreShell.h"
#include "UiTeamViewer.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiTeamViewer* KUiWndSingleton<KUiTeamViewer>::ms_Singleton	= NULL;

KUiTeamViewer::KUiTeamViewer( const CEGUI::String& id_name )
: KUiWndSingleton<KUiTeamViewer>( id_name )
, d_pJoinTeamBtn(NULL)
, d_pRefreshBtn(NULL)
, d_pScroll(NULL)
, d_pTeamsList(NULL)
, d_pTeamMembers(NULL)
, d_curLeaderId(TEAMVIEWER_INVALID_TEAMLEADER_ID)
{
	for (int i = 0; i < MAX_TEAM_BASIC_INFO_LIST_SIZE; ++i)
	{
		d_pTeam[i] = NULL;
		d_teamLeaderId[i] = TEAMVIEWER_INVALID_TEAMLEADER_ID;
	}
}

KUiTeamViewer::~KUiTeamViewer()
{
}

void KUiTeamViewer::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		d_pTeamsList	= m_pThisWnd->getChild("TaharezLook/TeamViewer/TeamsList");
		d_pTeamMembers	= d_pTeamsList->getChild("TaharezLook/TeamViewer/TeamsList/Members");
		d_pJoinTeamBtn	= static_cast<TLButton*>(m_pThisWnd->getChild("TaharezLook/TeamViewer/JoinTeamBtn"));
		d_pRefreshBtn	= static_cast<TLButton*>(m_pThisWnd->getChild("TaharezLook/TeamViewer/RefreshBtn"));
		d_pScroll		= static_cast<TLVertScrollbar*>(m_pThisWnd->getChild("TaharezLook/TeamViewer/Scrollbar"));

		d_pScroll->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
			Event::Subscriber(&KUiTeamViewer::handleScroll, this));
		d_pTeamMembers->subscribeEvent(Window::EventMouseWheel, 
			Event::Subscriber(&KUiTeamViewer::handleWheelChanged, this));
		d_pJoinTeamBtn->subscribeEvent(Window::EventMouseClick, 
			Event::Subscriber(&KUiTeamViewer::handleRequestJoinTeam, this));
		d_pRefreshBtn->subscribeEvent(Window::EventMouseClick, 
			Event::Subscriber(&KUiTeamViewer::handleRefresh, this));
		
		m_pThisWnd->getChild("TaharezLook/TeamViewer/Close")->subscribeEvent(
			Window::EventMouseClick, Event::Subscriber(&KUiTeamViewer::handleClose, this));

		for (int i = 0; i < MAX_TEAM_BASIC_INFO_LIST_SIZE; ++i)
		{
			// 添加一个队伍信息
			char szName[COMMON_CLIENT_MSG_LEN_128];
			ZeroMemory(szName, COMMON_CLIENT_MSG_LEN_128);
			sprintf(szName, "TaharezLook/TeamViewer/TeamsList/TeamViewerMember%d", i);
			d_pTeam[i] = m_pWindowManager->loadWindowLayout(
			TEAMVIEWERMEMBER_FILENAME, szName, "", NULL, NULL, true);
			d_pTeam[i]->subscribeEvent(Window::EventMouseButtonDown, 
				Event::Subscriber(&KUiTeamViewer::handleTeamClicked, this));
			d_pTeam[i]->show();
			Point p = d_pTeam[i]->getPosition(Absolute);
			p = Point(p.d_x, p.d_y + i*(d_pTeam[i]->getHeight(Absolute)));
			d_pTeam[i]->setPosition(Absolute, p);

			d_pTeamMembers->addChildWindow(d_pTeam[i]);
		}
		//d_pTeamMembers->setHeight(Absolute, 
		//	MAX_TEAM_DISPLAY*d_pTeam[0]->getHeight(Absolute));
		//d_pScroll->setStepSize(d_pTeamsList->getHeight()/d_pTeamMembers->getHeight());
	}
}

void KUiTeamViewer::Show( void )
{
    KUiWndSingleton<KUiTeamViewer>::Show();
	g_pCoreShell->OperationRequest(GOI_LIST_TEAM, NULL, NULL);		
}

bool KUiTeamViewer::handleTeamClicked(const CEGUI::EventArgs& args)
{
	ClearSelectedTeam();
	WindowEventArgs* winArg = (WindowEventArgs*)(&args);

	for (int i = 0; i < MAX_TEAM_BASIC_INFO_LIST_SIZE; ++i)
	{
		if (d_pTeam[i]->isVisible() && winArg->window == d_pTeam[i])
		{
			String str = d_pTeam[i]->getName() + "/BtnHover";
			d_pTeam[i]->getChild(str)->setVisible(true);
			d_curLeaderId = d_teamLeaderId[i];
			return true;
		}
	}

	return true;
}

bool KUiTeamViewer::handleRequestJoinTeam(const CEGUI::EventArgs& args)
{
	if (d_curLeaderId == TEAMVIEWER_INVALID_TEAMLEADER_ID)
		return true;
		
	KUiPlayerItem tagPlayer;
	tagPlayer.nData = 0;
	tagPlayer.nIndex = 0;
	tagPlayer.nParam = 0;
	tagPlayer.uId = d_curLeaderId;
	ZeroMemory(tagPlayer.Name, sizeof(tagPlayer.Name));
	g_pCoreShell->TeamOperation(TEAM_OI_APPLY_JOIN, (unsigned int)&tagPlayer, NULL );

	return true;
}

bool KUiTeamViewer::handleScroll(const CEGUI::EventArgs& args)
{
	float scrollPos = d_pScroll->getScrollPosition();
	float offset_y = (d_pTeamMembers->getHeight(Absolute) - d_pTeamsList->getHeight(Absolute)) * scrollPos;

	Point p = d_pTeamMembers->getPosition(Absolute);
	d_pTeamMembers->setPosition(Absolute, Point(p.d_x, -offset_y));

	return true;
}

bool KUiTeamViewer::handleWheelChanged( const CEGUI::EventArgs& args )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;

	if(d_pScroll->isVisible())
	{ 
		d_pScroll->setScrollPosition(d_pScroll->getScrollPosition() - \
			d_pScroll->getStepSize() * eventArgs->wheelChange);
	}
	return true;
}

bool KUiTeamViewer::handleRefresh(const CEGUI::EventArgs& args)
{
	g_pCoreShell->OperationRequest(GOI_LIST_TEAM, NULL, NULL);
	return true;
}

bool KUiTeamViewer::handleClose(const CEGUI::EventArgs& args)
{
	Hide();
	return true;
}

void KUiTeamViewer::Update(TeamBasicInfo* pTeamInfo, int nTeamCount)
{
	Clear();

	for (int teamIdx = 0; teamIdx < nTeamCount; ++teamIdx)
	{
		String leaderName = d_pTeam[teamIdx]->getName() + "/LeaderName";
		String memberCount = d_pTeam[teamIdx]->getName() + "/TeamMemberCount";
		char memberNum[2];
		itoa(pTeamInfo->MemberCount, memberNum, 10);

		d_pTeam[teamIdx]->getChild(leaderName)->setText(AnsiToUtf8(pTeamInfo->CaptainName));
		d_pTeam[teamIdx]->getChild(memberCount)->setText(AnsiToUtf8(memberNum));
		d_pTeam[teamIdx]->show();
		
		d_teamLeaderId[teamIdx] = pTeamInfo->CaptainNpcID;

		pTeamInfo++;
	}

	d_pTeamMembers->setHeight(Absolute, nTeamCount*d_pTeam[0]->getHeight(Absolute));
	if (d_pTeamsList->getHeight() < d_pTeamMembers->getHeight())
	{
		d_pScroll->setStepSize(d_pTeamsList->getHeight()/d_pTeamMembers->getHeight());
	}
	else
	{
		d_pScroll->hide();
	}
}

void KUiTeamViewer::Clear()
{
	ClearSelectedTeam();
	for (int i = 0; i < MAX_TEAM_BASIC_INFO_LIST_SIZE; ++i)
	{
		d_pTeam[i]->hide();
		d_teamLeaderId[i] = TEAMVIEWER_INVALID_TEAMLEADER_ID;
	}
}

void KUiTeamViewer::ClearSelectedTeam()
{
	for (int i = 0; i < MAX_TEAM_BASIC_INFO_LIST_SIZE; ++i)
	{
		if (d_pTeam[i] && d_pTeam[i]->isVisible())
		{
			String str = d_pTeam[i]->getName() + "/BtnHover";
			d_pTeam[i]->getChild(str)->setVisible(false);
		}
	}
}