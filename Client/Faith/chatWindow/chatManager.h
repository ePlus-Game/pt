#ifndef _CHAT_MANAGER_H
#define _CHAT_MANAGER_H

#define CHAT_MANAGER_SCROLLBAR_INI_NAME		"ChatScrollBar"
#define CHAT_MANAGER_SCROLLBAR_X			"x"
#define CHAT_MANAGER_SCROLLBAR_Y			"y"
#define CHAT_MANAGER_SCROLLBAR_WIDTH		"width"
#define CHAT_MANAGER_SCROLLBAR_HEIGHT		"height"
#define CHAT_MANAGER_SCROLLBAR_UP_WIDTH		"up_width"
#define CHAT_MANAGER_SCROLLBAR_UP_HEIGHT	"up_height"
#define CHAT_MANAGER_SCROLLBAR_DOWN_WIDTH	"down_width"
#define CHAT_MANAGER_SCROLLBAR_DOWN_HEIGHT	"down_height"
#define CHAT_MANAGER_SCROLLBAR_BAR_WIDTH	"scroll_bar_width"
#define CHAT_MANAGER_SCROLLBAR_BAR_HEIGHT	"scroll_bar_height"
#define CHAT_MANAGER_SCROLLBAR_RECT_WIDTH	"scroll_bar_rect_Width"

class ChatManagerScrollBar : public ChatScrollBar
{
public:
	ChatManagerScrollBar();
	~ChatManagerScrollBar();
	
public:
	void	ChatScrollBarLoadIniCtg();
	void	ChatScrollBarSetScrolls(int allLength, int oneLength);
	void	SetBarRectPos(int pos);
	bool	IsClickScrollBar(const POINT & pt);
	bool	IsClickRect(const POINT & pt);
	int 	MoveBarRect(const POINT & pt);
	int		GetBarRectPos();
	int		GetOneLineLength();
	int		GetBarRectHeight();
};

//////////////////////////////////////////////////////


#define WM_SHOWTASK   WM_USER+100

//////////////////////////////////////////////////////


#define _TOP_BUTTON_INDEX  0
#define _UP_BUTTON_INDEX   1
#define _DOWN_BUTTON_INDEX 2
#define _END_BUTTON_INDEX  3
#define _CLOSEWND_BUTTON_INDEX 4
#define _CLOSE_BUTTON_INDEX 5

#define _CHAT_BUTTON_ID_TOP       1020
#define _CHAT_BUTTON_ID_UP        1021
#define _CHAT_BUTTON_ID_DOWN      1022
#define _CHAT_BUTTON_ID_END       1023
#define _CHAT_BUTTON_ID_CLOSE     1024
#define _CHAT_BUTTON_ID_CLOSE_ITSELF 1025

#define _CHAT_STATIC_PAGE_ID      1040
#define _CHAT_EDIT_CHAT_ID        1041
#define _CHAT_COMBOBOX_CHAT_ID    1042
#define _CHAT_SHOWFACE_BUTTON     1043


/////////////////////////////////
#define _CHAT_BUTTON_DOWN_TIMER_ID 1
#define _CHAT_BUTTON_UP_TIMER_ID   2
#define _CHAT_BUTTON_TOP_TIMER_ID  3
#define _CHAT_BUTTON_END_TIMER_ID  4

#define _CHAT_NORMAL_BUTTON_NUMBER 6


#define _CHAT_SRC_PATH              "ui/imagesets/image/"
#define _CHAT_SRC_X                 "x"
#define _CHAT_SRC_Y                 "y"
#define _CHAT_SRC_WIDTH             "width"
#define _CHAT_SRC_HEIGHT            "height"
#define _CHAT_SRC_CLIP              "isClip"
#define _CHAT_SRC_CLIP_COLOR_R      "clipColorR"
#define _CHAT_SRC_CLIP_COLOR_G      "clipColorG"
#define _CHAT_SRC_CLIP_COLOR_B      "clipColorB"
#define _CHAT_SRC_NORMAL             "normalSrc"
#define _CHAT_SRC_MOUSEOVER          "mouseOverSrc"
#define _CHAT_SRC_MOUSEDOWN          "mouseDownSrc"


#define _CHAT_NEED_UPDATE_TIMER_ID    2008
#define _CHAT_NEED_UPDATE_TIMER_DT    100

class ChatWnd;

void CALLBACK   InfoWndUpdate(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime);
typedef class ChatManager
{
public:
	ChatManager();
	~ChatManager();
public:
	LRESULT ChatManagerProcessMouseWheel(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessOnwerDraw(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessMouseMove(HWND hwnd,WPARAM wParm,LPARAM lParam);
	LRESULT ChatManagerProcessMouseLeave(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessButtonCommand(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessMButtonEvent(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessMouseHover(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessKeyDown(HWND hwnd,WPARAM wParam,LPARAM lParam);
	LRESULT ChatManagerProcessPaint(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void    ChatManagerSendMessageToServer();
	void    ChatManagerRegistChannel(Ui_Channel_Param& channel);
	void    ChatManagerReceiveChatMsg(int channelID ,BYTE* msg);
	void    ChatManagerReceiveCustomMsg(int channelID,char* msg);
	void    ChatManagerReceiveCozeMsg(BYTE* byBuffer);
	void    ChatClientInsertSystemMsg(const char* pBuffer,bool isImage = false);
	void    RecalculateScroll();

	void    ChatClientInsertSystemMsg(ILayout* pLayout);

	void    ChatClientInsertPlayerInfoMsg(const char* pBuffer);
	const char* ChatManagerGetChannelName(int channelID);
	static void  ChatManageTextToLoelem(const char* text, char* segText, int chanId, bool bGM);
	int     ChatManagerRecMsgAtPage(int chanID);
public:
	BOOL    ChatManagerCreateWnd(HWND hwnd);
	void    ChatManagerSetCurrentPage(int pageIndex);
	ChatEditBox* ChatManagerGetEditBox();
	ChatInfoWnd* ChatManagerGetInfoWnd();
	ChatManagerScrollBar * ChatManagerGetScrollBar();
	int              ChatManagerGetCurrentPage() const;

	int          ChatManagerGetPageNumber() const {return chatPages.size();}
	ChatPage* ChatManagerGetPage(int index);
	Ui_Channel_Param& ChatManagerGetIndexChannel(int index){return chatChannels[index];}
	int               ChatManagerGetChannelNumber() {return chatChannels.size();}
	FaceDialog*  ChatManagerGetFaceDlg() { return &faceDialog;}
	ChatButton*  ChatManagerGetShowButton(){return &showButton;}
	ChatTipWnd*  ChatManagerGetTipPlayerWnd() {return &playerRelate;}
	ChatTipWndItem* ChatManagerGetTipItemWnd() { return &itemInfo;}

	void            ChatManagerClearInfoWnd();
	ChatUiComboBox& ChatManagerGetUiComboBox()  {return uiComboBox;}
	bool            ChatManagerChannelClose(int channelID);
	void            ChatManagerChannelCloseOne(int channelID);
	void            ChatManagerChannelEnable(int channelID);
	void            SwapChannel();
	static void		ChangeTextColor(char * segText, const char * color, int chanId, bool bGM);
	static void		ChangeTextFont(char * segText, const char * font, int chanId, bool bGM);
	bool			IsClickScrollBarDownOrUpBnt(const POINT & pt);
	bool			IsClickScrollBarRect(const POINT & pt);
	void			ProcessScrollBarRectMove(const POINT & pt);
	void			ClickScrollBar(const POINT & pt);

public:
	ChatInfoWnd       infoWnd;
	ChatEditBox       edit;
	ChatButton        normalButton[_CHAT_NORMAL_BUTTON_NUMBER];
	ChatButton        showButton;
	int                 currentPageIndex ;
	BOOL                isShowMainWnd;
	static bool         isCurrrent;
	static bool         needUpdate;
	static DWORD        updataTime;
	static DWORD        startUpdateTime;
	vector<ChatPage*>   chatPages;
	vector<Ui_Channel_Param> chatChannels;
	vector<int>              chatChannelsClose;

protected:
	char                strMsg[LAYOUT_TEXT_MAX_LEN + 1];
protected:
	ChatManagerScrollBar	m_ScrollBar;
	FaceDialog				faceDialog;
	ChatTipWndItem			itemInfo;
	//////tip////////////////
	ChatTipWnd          playerRelate;
	ChatUiComboBox      uiComboBox;
}CHATMANAGER,*LPCHATMANAGER;

#endif