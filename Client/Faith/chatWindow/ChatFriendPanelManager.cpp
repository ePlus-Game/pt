#include <windows.h>
#include <commctrl.h>
#include <vector>
#include <list>
#include <string>
using std::string;
using std::list;
using std::vector;
#include "layoutinterface.h"
#include "GameDataDef.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/GDIRender.h"
#include "chatWindow/ChatCharContainer.h"
#include "chatWindow/chatWnd.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "ChatDataDef.h"
#include "coreshell.h"


#include "chatWindow/ChatTipWndItem.h"
#include "chatWindow/ChatTipWnd.h"
#include "chatWindow/ChatPage.h"
#include "chatWindow/faceDialog.h"
#include "chatWindow/chatManager.h"
#include "chatWindow/chatDialog.h"
#include "chatWindow/ChatMainDlg.h"
#include "chatWindow/ChatControlPanel.h"

#include "chatWindow/EntrustComputerDlg.h"
#include "chatWindow/ChatWndProc.h"
extern iCoreShell*							g_pCoreShell;
ChatFriendPanelManager::ChatFriendPanelManager()
{
	memset(ppControls,0,sizeof(LPPLAYERCONTROL));
	numberControls=0;

	onlineColor = 0;
	leftlineColor = 0;
	chatWndListUpdata = false;
	isUseDefaultFont= false;
	needUpdate = false;
}
ChatFriendPanelManager::~ChatFriendPanelManager()
{
	for(int i = 0; i < 200 ; i++)
	{
		delete ppControls[i];
		ppControls[i] = 0;
	}
}

ChatFriendPanelManager& ChatFriendPanelManager::ChatFriendManagerGet()
{
	static ChatFriendPanelManager firendPanelManager;
	return firendPanelManager;
}
void ChatFriendPanelManager::ChatFriendManagerShowInfoDlg()
{
	firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(TRUE);
}
void ChatFriendPanelManager::ChatFriendManagerHideInfoDlg()
{
	firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(FALSE);
}



void ChatFriendPanelManager::ChatFriendManagerInit(HWND hwnd)
{
	KIniFile  iniFile;
	firendPanel.ChatFriendCreate(hwnd);
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH] = {0};
	char szImagePathIndex[]=_CHAT_SRC_PATH;
	char szImagePath[MAX_PATH] = {0};
	if(g_GetScreenWidth() == 1024)
		strcpy(szPath,_CHAT_CFG_FILE_1024);
	else
		strcpy(szPath,_CHAT_CFG_FILE);
	iniFile.Load(szPath);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_WIDTH,0,&controlWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_CHAT_SRC_HEIGHT,0,&controlHeight);


	char font[FONT_SIZE]={0};
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_LIST_INI_SRC_FONT,"",font,FONT_SIZE);
	TFONT tFont;
	strcpy(tFont.fontName,font);
	HDC hdc =CreateCompatibleDC(NULL);
	LOGFONT logFont;
	logFont.lfFaceName[0]=0;
	logFont.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hdc,&logFont,(FONTENUMPROC)EnumFontProc,(LPARAM)&tFont,0);
	DeleteDC(hdc);
	if(tFont.isInSystem)
	{
		isUseDefaultFont = false;
		strcpy(pszFont,font);
	}
	else
	{
		isUseDefaultFont = true;
	    strcpy(pszFont,ChatString::ChatStringGetString().chatDefualtFont);
	}

	int r,g,b;
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontHoverColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineMouseOverColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"onlineFontSelectColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	onlineSelectColor = RGB(r,g,b);

	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"leftlinefontColor","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	leftlineColor = RGB(r,g,b);
	iniFile.GetString(_PLAYER_LIST_INI_SRC_NAME,"leftlinefontSelect","",szValue,MAX_PATH);
	sscanf(szValue,"%d,%d,%d",&r,&g,&b);
	leftlineSelectColor = RGB(r,g,b);

	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_NAME_WIDTH,0,&nameItemWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_LEVEL_WIDTH,0,&levelItemWidth);
	iniFile.GetInteger(_PLAYER_LIST_INI_SRC_NAME,_PLAYER_ITEM_CONTROL_INI_METIER_WIDTH,0,&metierItemWidth);
	DlgProcessFun funArray[] = {ChatWndProcessFun::FriendDlgProc,ChatWndProcessFun::EnemyDlgProc,ChatWndProcessFun::PingBiDlgProc};
	for(int i = 0; i < _PLAYER_INFO_PAGE_NUM ; i++)
	{
		playerInfoDlg[i].PlayerInfoDlfCreate(hwnd,funArray[i]);
		playerInfoDlg[i].PlayerInfoDlgLoadSRC(playerInfoDlg[i].hDlg);
	}
	firendPanel.ChatFriendSetCurrentDlg(&playerInfoDlg[0]);

	addDlg.ChatHintDlgCreate(B2ChatDialog::m_hDialog);
	addDlg.ChatHintDlgLoadSource();
	deleteDlg.ChatHintDlgCreate(B2ChatDialog::m_hDialog);
	deleteDlg.ChatHintDlgLoadSource();
	
	
}
void ChatFriendPanelManager::ChatFriendManagerAdd(PLAYERCONTROL*& pControls)
{
	if(numberControls == _CHAT_MAX_FRIENDS)
		return ;
	ppControls[numberControls] = pControls;
	numberControls++;
	ChatPlayerInfo & playerInfo = pControls->PlayerControlGetPlayerInfo();
	if(playerInfo.type == _CHAT_PLAYER_INFO_TYP_FRIEND)
	{
		playerInfoDlg[_CHAT_PLAYER_INFO_FRIEND].PlayerInfoDlgAdd(pControls);
	}
	else
	if(playerInfo.type == _CHAT_PLAYER_INFO_ENEMY)	
	{
		playerInfoDlg[_CHAT_PLAYER_INFO_ENEMY].PlayerInfoDlgAdd(pControls);
	}
}

void ChatFriendPanelManager::ChatFriendManagerAdd(ChatPlayerInfo& playerInfo)
{
	if(numberControls == _CHAT_MAX_FRIENDS)
		return ;
	PLAYERCONTROL* pControls = new PLAYERCONTROL;
	pControls->PlayerControlCreate(0,0,controlWidth,controlHeight,0,0);
	pControls->PlayerControlGetPlayerInfo() = playerInfo;
	pControls->PlayerControlSetFont(pszFont);
	pControls->PlayerControlSetArePartWidth(&nameItemWidth,&metierItemWidth,&levelItemWidth);
	pControls->PlayerControlSetOnlineFontColor(onlineColor);
	pControls->PlayerControlSetOnlineFontSelectColor(onlineSelectColor);
	pControls->PlayerControlSetOnlineFontMouseOverColor(onlineMouseOverColor);
	pControls->PlayerControlSetLeftlineFontSelectColor(leftlineSelectColor);
	pControls->PlayerControlSetLeftlineFontColor(leftlineColor);
	ppControls[numberControls] = pControls;
	
	//g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)playerInfo.playerName, NULL );
	
	numberControls++;
	if(playerInfo.type  == _CHAT_PLAYER_INFO_TYP_FRIEND)
	{
		pControls->PlayerControlSetParent(playerInfoDlg[_CHAT_PLAYER_INFO_FRIEND].hDlg);
		playerInfoDlg[_CHAT_PLAYER_INFO_FRIEND].PlayerInfoDlgAdd(pControls);
	}
	else
	if(playerInfo.type == _CHAT_PLAYER_INFO_ENEMY)	
	{
		pControls->PlayerControlSetParent(playerInfoDlg[_CHAT_PLAYER_INFO_ENEMY].hDlg);
		playerInfoDlg[_CHAT_PLAYER_INFO_ENEMY].PlayerInfoDlgAdd(pControls);
	}
	else
	if(playerInfo.type == _CHAT_PLAYER_INFO_TYP_PINGBI)
	{
		pControls->PlayerControlSetParent(playerInfoDlg[_CHAT_PLAYER_INFO_PINGBI].hDlg);
		playerInfoDlg[_CHAT_PLAYER_INFO_PINGBI].PlayerInfoDlgAdd(pControls);
	}
		
}
void ChatFriendPanelManager::ChatFriendManagerShowFriendPage(bool show)
{
	if(show)
	{
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFirendShow(TRUE);
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(TRUE);
		ChatControlPanel::ChatPanelGetPanel().ChatPanelShow(TRUE);


		B2ChatDialog::ChatDialogShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipItemWnd()->ChatTipWndShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetTipPlayerWnd()->ChatTipShow(FALSE);
		B2ChatDialog::chatManager.ChatManagerGetFaceDlg()->FaceDialogShow(FALSE);


		EntrustComputerDlg::EntrustDlgGetSingleton().EntrustDlgShowDlg(FALSE);
	}
	else
	{
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFirendShow(FALSE);
		ChatFriendPanelManager::ChatFriendManagerGet().firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgShowDlg(FALSE);
	}
}
void ChatFriendPanelManager::ChatFriendManagerDestroyWindow()
{
		for(int i = 0;i < _PLAYER_INFO_PAGE_NUM;i++)
			DestroyWindow(playerInfoDlg[i].hDlg);
		DestroyWindow(deleteDlg.hDlg);
		DestroyWindow(addDlg.hDlg);
		DestroyWindow(ChatControlPanel::ChatPanelGetPanel().hPanelDlg);
		DestroyWindow(firendPanel.CHatFriendGetHandle());
}
void ChatFriendPanelManager::ChatFriendManegerGetPlayerInfo(PlayerInfo& playerGameInfo)
{
	for(int i = 0; i < numberControls;i++)
	{
		ChatPlayerInfo& info = ppControls[i]->PlayerControlGetPlayerInfo();
		if(strcmp((CHAR*)info.playerName,playerGameInfo.szName)==0)
		{
			info.playerLevel = playerGameInfo.sLevel;
			if(playerGameInfo.szZhuhou[0]!=0)
			{
				strcat((char*)info.PlayerZhuhou,playerGameInfo.szZhuhou);
			}
			if(playerGameInfo.szShizu[0]!=0)
			{
				strcat((char*)info.PlayerSizhu,playerGameInfo.szShizu);
			}
			if ( playerGameInfo.sSkillType < 0 )
			{
				switch(playerGameInfo.sMetier)
				{
				case 0:
					strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_JS);
					break;
				case 1:
					strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_DS);
					break;
				case 2:
					strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_YR);
					break;
				default :
					strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_JS);
					break;
				}
			}
	       else
		   {
				switch(playerGameInfo.sMetier)
				{
				case 0:
					if ( playerGameInfo.sSkillType )
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_JS_0);
					}
					else
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_JS_1);
					}
					break;
				case 1:
					if ( playerGameInfo.sSkillType )
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_DS_0);
					}
					else
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_DS_1);
					}
					break;
				case 2:
					if ( playerGameInfo.sSkillType )
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_YR_1);
					}
					else
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_YR_0);
					}
					break;
				default :
					if ( playerGameInfo.sSkillType )
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_JS_1);
					}
					else
					{
						strcpy( (CHAR*)info.playerMetier, ROLE_CAREER_JS_0);
					}
					break;
				}
		   }
		   strcpy((char*)info.playerPlace,"----");
		   for(int index = 0; index<_PLAYER_INFO_PAGE_NUM;index++)
		   {
			   if(firendPanel.ChatFriendGetCurrentDlg() == &playerInfoDlg[index])
				   break;
		   }
		   if(index == info.type)
			   firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgUpdateDrawArea();
		   return;
		}
	}
}
void ChatFriendPanelManager::ChatFriendManagerClearControl()
{
	for(int i = 0; i < numberControls;i++)
	{
		delete ppControls[i];
		ppControls[i] = 0;
	}
	numberControls = 0;
}
void ChatFriendPanelManager::ChatFriendManagerReceiveFriendList()
{
	DWORD friendPageInfo[MAX_FRIENDGROUP_COUNT] = {0};
	char	m_GroupNameList[MAX_FRIENDGROUP_COUNT][CLIENT_NAME_AND_TITLE_MAX+1];
	int nGroupCount = g_pCoreShell->GetGameData(GDI_CHAT_GROUP_INFO,(UINT)friendPageInfo,(int)m_GroupNameList);
	ChatFriendManagerClearControl();
	for(int i = 0; i < _PLAYER_INFO_PAGE_NUM; i ++)
	{
		playerInfoDlg[i].PlayerInfoDlgProcessClearControl();
	}
	for(int nFriendGroupIdx = 0 ; nFriendGroupIdx < nGroupCount ; nFriendGroupIdx++)
	{
		CHAT::_UI_Chat_ObjInfo  m_FriendNameList[COMMON_CLIENT_MSG_LEN_256] = {0};
		int nFriendInGroupCount = g_pCoreShell->GetGameData( GDI_CHAT_FRIENDS_IN_A_GROUP, (unsigned int)friendPageInfo[nFriendGroupIdx], (int)m_FriendNameList );
		if ( nFriendInGroupCount > COMMON_CLIENT_MSG_LEN_256)
		{
			nFriendInGroupCount = COMMON_CLIENT_MSG_LEN_256;
		}
		int type = _CHAT_PLAYER_INFO_FRIEND;
		if(friendPageInfo[nFriendGroupIdx] == (int)CHAT::GROUPID_BLACK)
			type = _CHAT_PLAYER_INFO_PINGBI;
		else
		if(friendPageInfo[nFriendGroupIdx] == (int)CHAT::GROUPID_TEMP)
			type = _CHAT_PLAYER_INFO_TEMP;
		else
		if(friendPageInfo[nFriendGroupIdx] == (int)CHAT::GROUPID_ENEMY)
			type = _CHAT_PLAYER_INFO_ENEMY;

		for ( int nFriendIdx = 0; nFriendIdx < nFriendInGroupCount; ++nFriendIdx )
		{
			ChatPlayerInfo playerInfo;
			strcpy((CHAR*)playerInfo.playerName,m_FriendNameList[nFriendIdx].szName);
			playerInfo.isPlayerOnline = m_FriendNameList[nFriendIdx].bOnline;
			playerInfo.type = type;
			playerInfo.playerLevel = m_FriendNameList[nFriendIdx].nLevel;
			strcpy((char*)playerInfo.playerMetier,"----");

			if (m_FriendNameList[nFriendIdx].nLevel)
			{
				switch(m_FriendNameList[nFriendIdx].nSeries)
				{
				case 0:
					strcpy( (CHAR*)playerInfo.playerMetier, ROLE_CAREER_JS);
					break;
				case 1:
					strcpy( (CHAR*)playerInfo.playerMetier, ROLE_CAREER_DS);
					break;
				case 2:
					strcpy( (CHAR*)playerInfo.playerMetier, ROLE_CAREER_YR);
					break;
				default :
					strcpy( (CHAR*)playerInfo.playerMetier, ROLE_CAREER_JS);
					break;
				}
			}//endif
			
			strcpy((char*)playerInfo.playerPlace,"----");

			ChatFriendManagerAdd(playerInfo);
		}
	}
	for(int j = 0; j <_PLAYER_INFO_PAGE_NUM; j++)
	{
		playerInfoDlg[j].PlayerInfoSetState(_PLAYER_LIST_CONTROL_NORMAL_SHOW);
		playerInfoDlg[j].PlayerInfoQSort(_PLAYER_LIST_SORT_BYNAME);
		playerInfoDlg[j].scroll_bar.ChatScrollBarSetScrolls(playerInfoDlg[j].allFriendsLength,controlHeight+playerInfoDlg[j].intermission);
	}
	firendPanel.ChatFriendGetCurrentDlg()->PlayerInfoDlgUpdate();

}