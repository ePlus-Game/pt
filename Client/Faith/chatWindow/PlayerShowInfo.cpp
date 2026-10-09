#include "PlayerShowInfo.h"

#include <vector>
using std::vector;

#include "layoutinterface.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/faceDialog.h"
#include  "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatTipWndItem.h"

#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "ui/UiCase/UiChatWindow.h"
#include "../KMessageCentre.h"

#include "CoreShell.h"
extern iCoreShell*		g_pCoreShell;
PlayerShowInfo::PlayerShowInfo()
{
	playerInfoList.clear();
}
PlayerShowInfo::~PlayerShowInfo()
{
	playerInfoList.clear();
}

void PlayerShowInfo::GetBaseInfo(PlayerInfo& playerInfo)
{
	DWORD id = g_FileName2Id(playerInfo.szName);
	playerList::iterator it = playerInfoList.find(id);
	if(it != playerInfoList.end() )
	{
		while(it != playerInfoList.begin())
		{
			playerInfoList.erase(playerInfoList.begin());
		}
		it->second = playerInfo;
		ShowInfoInText(&playerInfo);
		playerInfoList.erase(playerInfoList.begin());
	}
}
void PlayerShowInfo::ShowInfoInText(PlayerInfo* info)
{
	if(info)
	{
		char buffer[1024+1] ={ 0};
		char tempBuff[512+1]={0};
		char tempBuff1[512+1]={0};
		sprintf(tempBuff,"[%s]",info->szName);
		strcpy(buffer,tempBuff);
		if ( info->sSkillType < 0 )
		{
			switch(info->sMetier)
			{
				case 0:
					strcpy( tempBuff, ROLE_CAREER_JS);
					break;
				case 1:
					strcpy( tempBuff, ROLE_CAREER_DS);
					break;
				case 2:
					strcpy( tempBuff, ROLE_CAREER_YR);
					break;
				default :
					strcpy( tempBuff, ROLE_CAREER_JS);
					break;
				}
			}
	       else
		   {
				switch(info->sMetier)
				{
				case 0:
					if ( info->sSkillType )
					{
						strcpy( tempBuff, ROLE_CAREER_JS_0);
					}
					else
					{
						strcpy( tempBuff, ROLE_CAREER_JS_1);
					}
					break;
				case 1:
					if ( info->sSkillType )
					{
						strcpy( tempBuff, ROLE_CAREER_DS_0);
					}
					else
					{
						strcpy( tempBuff, ROLE_CAREER_DS_1);
					}
					break;
				case 2:
					if (info->sSkillType )
					{
						strcpy( tempBuff, ROLE_CAREER_YR_1);
					}
					else
					{
						strcpy( tempBuff, ROLE_CAREER_YR_0);
					}
					break;
				default :
					if ( info->sSkillType )
					{
						strcpy( tempBuff, ROLE_CAREER_JS_1);
					}
					else
					{
						strcpy( tempBuff, ROLE_CAREER_JS_0);
					}
					break;
				}
		   }
		   sprintf(tempBuff1,"  %s%d[%s]",ChatString::ChatStringGetString().titlePlayerLevel,info->sLevel,tempBuff);
		strcat(buffer,tempBuff1);
		tempBuff1[0] = 0;
		if(info->szZhuhou[0]!=0)
			sprintf(tempBuff1,"  %s<%s>",ChatString::ChatStringGetString().playerShowInfoZhuhou,info->szZhuhou);
		else
			sprintf(tempBuff1,"  %s<%s>",ChatString::ChatStringGetString().playerShowInfoZhuhou, ChatString::ChatStringGetString().playerNotHaveZhuhou);
		strcat(buffer,tempBuff1);
		tempBuff1[0] = 0;
		if(info->szShizu[0]!=0)
			sprintf(tempBuff1,"  %s<%s>",ChatString::ChatStringGetString().playerShowInfoSizhu,info->szShizu);
		else
			sprintf(tempBuff1,"  %s<%s>",ChatString::ChatStringGetString().playerShowInfoSizhu,ChatString::ChatStringGetString().playerNotHaveSizhu);
		strcat(buffer,tempBuff1);
		B2ChatDialog::chatManager.ChatClientInsertPlayerInfoMsg(buffer);
		char mainChatBuffer[1024+256+1] = {0};
		sprintf(mainChatBuffer,"<Seg float=wrap><Obj color=%s>%s</Obj></Seg>",KUiChanMgr::getSinglton().getChanColor(SYSTEM_ROOM_ID, false),buffer);
		KUiChannelCentre::GetSingleton().recvCustomMessage(SYSTEM_ROOM_ID,mainChatBuffer);
	}
}
PlayerShowInfo& PlayerShowInfo::GetSingle()
{
	static PlayerShowInfo showInfo;
	return showInfo;
}

#define MAX_LEN_TIME_REQUEST_INTERVAL 2000
void PlayerShowInfo::AddItem(PlayerInfo& name)
{
	static DWORD dwLastTime = 0;
	
	if ( timeGetTime() - dwLastTime >= MAX_LEN_TIME_REQUEST_INTERVAL )
	{
		
		DWORD id = g_FileName2Id(name.szName);
		playerList::iterator it = playerInfoList.find(id);
		if(it == playerInfoList.end())
		{
			g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)name.szName, NULL );
			playerInfoList[id] = name;
		}

		dwLastTime = timeGetTime();
	}//endif
	else
	{
		const char * szWarnning = KMessageCentre::GetMessage(friend_message,32);
		if (szWarnning && szWarnning[0])
			KUiChannelCentre::GetSingleton().toSysMsg(szWarnning);
	}//end else

}
