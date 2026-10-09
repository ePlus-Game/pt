#ifndef _CHAT_PAGE_H
#define _CHAT_PAGE_H


#define _PAGE_ID_SYNTHESIS    0
#define _PAGE_ID_NEAR         1
#define _PAGE_ID_WORLD        2
#define _PAGE_ID_SYSTEM       3
#define _PAGE_ID_PERSONAL     4
#define _PAGE_ID_ORG          5
#define _PAGE_ID_FIGHT        6

#define _CHAT_PAGE_MAX_NUMBER           10
#define _CHAT_PAGE_BUTTON_ID_SYNTHESIS  1000
#define _CHAT_PAGE_BUTTON_ID_NEAR       1001
#define _CHAT_PAGE_BUTTON_ID_WORLD      1002
#define _CHAT_PAGE_BUTTON_ID_SYSTEM     1003
#define _CHAT_PAGE_BUTTON_ID_PERSONAL   1004
#define _CHAT_PAGE_BUTTON_ID_ORG        1005
#define _CHAT_PAGE_BUTTON_ID_FIGHT      1006


typedef class ChatPage
{
public:
	ChatPage();
	~ChatPage();
public:
	void  ChatPageCreatePageButton(HWND hwnd,const char*pagesName);
	void  ChatPageRegistChannel(Ui_Channel_Param& channel);
	void  ChatPageShow();
	void  ChatPageDown();
	void  ChatPageUp();
	void  ChatPageLineUp();
	void  ChatPageLineDown();
	const Ui_Channel_Param& ChatPageGetChannel(int i);
	int   ChatPageGetChannelNumbers() const{return channel.size();}
	ChatButton&  ChatPageGetButton() ;
	void  ChatPageInsertSeg(const char* pText);
	POINT& ChatPageGetDrawPoint();
	void ChatPageSetShowWnd(ChatWnd* pInfo);
	void ChatPageCloseChannel(int channelID);
	int  ChatPageGetID() const { return pageID;}
	bool IsHavePersonalChannel();
public:
	ChatWndRender        chatWndRender;
private:
	ChatButton          pageButton;
	ChatWnd*        pInfoWnd;
	vector<Ui_Channel_Param> channel;
	POINT             drawPoint;
	int               pageID;
	char              name[64];
}CHATPAGE,*LPCHATPAGE;
#endif