#ifndef CHAT_WND_PROC_H
#define CHAT_WND_PROC_H

#define WM_DATA_REQUEST_SUCCEED (WM_USER + 10)

namespace ChatWndProcessFun
{
	////////////频道按钮//////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK  ProcessWorldPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK  ProcessSynthetizePageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK  ProcessNearPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK  ProcessOrgPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK  ProcessFightPageButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK  ProcessPersonalButtonFun(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	//////////信息滚动条///////////////////////////
	LRESULT CALLBACK  ProcessChatInfoWnd(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	//委托界面切换按钮
	LRESULT CALLBACK ProcessEntrustChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//委托界面隐藏/显示按钮
	LRESULT CALLBACK ProcessEntrustHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//好友界面切换按钮
	LRESULT CALLBACK ProcessFriendChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//好友界面隐藏/显示按钮
	LRESULT CALLBACK ProcessFriendHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//切换按钮////////////////////////////////
	LRESULT CALLBACK ProcessChangeChatButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	//隐藏游戏主窗口按钮
	LRESULT CALLBACK ProcessHideGameWndButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	//face 按钮//
	LRESULT CALLBACK ProcessShowFaceButton(HWND hwnd,UINT msg,WPARAM wParam ,LPARAM lParam);
	//top
	LRESULT CALLBACK ProcessChatInfoTopButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessChatInfoEndButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessChatInfoUpButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessChatInfoDownButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	///tip//
	LRESULT CALLBACK ProcessTipItemCloseButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);

	LRESULT CALLBACK ProcessTipShowControl(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	//////////频道选择按钮//////////////////
	LRESULT CALLBACK ProcessChannelChangeButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	//////////输入框//////////////////////////////////
	LRESULT CALLBACK ProcessInputEditWnd(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	///分页切换键盘//
	LRESULT CALLBACK ProcessChatInfoPageButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessFrindInfoPageButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessEntrustPageButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessClanPageButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ProcessLuedPageButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//////////好友分页//////////////////////////////////////////////
	LRESULT CALLBACK ProcessFriendListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK processEnemyListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK processPingbiListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	///页面好友list///////////////////////////////////////////////////////
	BOOL CALLBACK   FriendDlgProc(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	BOOL CALLBACK   EnemyDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	BOOL CALLBACK   PingBiDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	///删除按钮//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK ProcessDeleteFriendListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessAddFriendListButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessShowLeftButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);

	///控制输入框光标闪烁的时间函数//////////////////////////////////////////////////////////////////////////
	void CALLBACK EditCursorFlashProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime);

	//////点击好友名字按钮状态//
	BOOL    CALLBACK LookFriendInfoDlgProc(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLookInfoPersonalButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLookInfoMakeTeamButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLookInfoDeleteButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLookInfoLookButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLookInfoPingBi(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLookInfoFriendButton(HWND hwnd ,UINT msg,WPARAM wParam,LPARAM lParam);


	////////小地图//
	BOOL CALLBACK  ChatMiniMapDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	VOID CALLBACK ChatMiniMapUpataTimeProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime);

	//小地图按钮
	LRESULT CALLBACK ProcessMiniMapPathFindBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessMiniMapKeyJinglinBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessMiniMapCurrentMapBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessMiniMapBigWordMapBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessMiniMapShowPlayerBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessMiniMapFindTeamBtn(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);



	//基本信息显示
	BOOL CALLBACK  ChatPlayerBaseInfoDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	VOID CALLBACK  ChatPlayerBaseInfoDlgTimerProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime);
	//血条控件
	LRESULT CALLBACK ProcessPlayerLifeShowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessPlayerManaShowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessPlayerExpShowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	//氏族面板切换
	LRESULT CALLBACK ProcessClanListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessLuedListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK ProcessNationListButton(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	//氏族分页
	BOOL CALLBACK ProcessClanProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	BOOL CALLBACK ProcessLuedProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	BOOL CALLBACK ProcessNationProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

	//氏族面板游戏切换
	LRESULT CALLBACK ProcessClanChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//氏族面板隐藏/显示
	LRESULT CALLBACK ProcessClanHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//氏族页面按钮
	LRESULT CALLBACK ProcessClanModifyBulletinButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ProcessClanAddMemberButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ProcessClanDeleteMemberButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//添加/删除氏族成员对话框处理函数
	BOOL CALLBACK AddMemberDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK DeleteMemberDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//氏族页面弹出菜单按钮处理函数
	BOOL CALLBACK ClanPopMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ClanPopMenuAddFrienBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ClanPopMenuInviteTeamBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK ClanPopMenuPrivateChatBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK ClanPopMenuParticularInfoBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK ClanPopMenuDemiseBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK ClanPopMenuFireBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);

	BOOL CALLBACK LuedPopMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuAddFrienBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuInviteTeamBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuPrivateChatBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuParticularInfoBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuDemiseBntProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuForbidBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK LuedPopMenuUnforbidBntProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//诸侯页面按钮
	LRESULT CALLBACK LuedModifyBulletinButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK LuedAddClanButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK LuedDeleteClanButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//添加删除氏族对话框
	BOOL CALLBACK AddClanDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL CALLBACK DeleteClanDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//诸侯页面隐藏/显示
	LRESULT CALLBACK LuedChangeButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	//诸侯页面切换
	LRESULT CALLBACK LuedHideShowButton(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif