#ifndef _LOOK_FRIEND_INFO_POP_DLG
#define _LOOK_FRIEND_INFO_POP_DLG

#define TALK_PERSONAL_BTN_ID  879
#define MAKE_TEAM_BTN_ID     880
#define DELETE_MEMBER_BTN_ID    881
#define TRADE_BTN_ID          882
#define LOOK_INFO_BTN_ID      883
#define PINGBI_BTN_ID         884
#define FRIEND_INFO_BTN_ID    885

#include "chatWindow/OnwerPlayerInfo.h"
class LookFriendInfoDlg
{
public:
	LookFriendInfoDlg();
	~LookFriendInfoDlg();
public:
	ChatButton& GetTalkPersonalBtn(){return talkPersonalBtn;}
	ChatButton& GetMakeTeamBtn() {return makeTeamBtn;}
	ChatButton& GetDeleteBtn() {return deleteMemberBtn;}
	ChatButton& GetLookInfoBtn() {return lookInfoBtn;}
	ChatButton& GetPingBiInfoBtn() {return pingBiBtn;}
	ChatButton& GetFriendInfoBtn() { return friendInfoBtn;}
	HWND        GetDlgItemHandle() const { return hDlg;}
	static LookFriendInfoDlg& GetSingle() ;
	
public:
	BOOL        CreateDlg(HWND hParent,DlgProcessFun processFun);
	void        LoadIniSrc(HWND hParent);

	void        ProcessTalkPersonalLButtonDown();
	void        ProcessMakeTeamLButtonDown();
	void        ProcessDeleteLButtonDown();
	void        ProcessLookInfoLButtonDown();
	void        ProcessPingBiLButtonDown();
	void        ProcessFriendInfoLButtonDown();
	void        ProcessKillFocus();
	void        AdjustWindow();
	void        Show();
	void        Hide();
	void        SetPlayerProcessControl(LPPLAYERCONTROL pControl){pPlayerControl = pControl;}
private:
	ChatButton talkPersonalBtn;
	ChatButton makeTeamBtn;
	ChatButton deleteMemberBtn;
	ChatButton lookInfoBtn;
	ChatButton pingBiBtn;
	ChatButton friendInfoBtn;
	HWND      hDlg;//
	int       width;//
	LPPLAYERCONTROL pPlayerControl;
};
#endif