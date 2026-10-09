#ifndef CHAT_CHAR_CONTAINER_H
#define CHAT_CHAR_CONTAINER_H



typedef struct InfoWndLayOut
{
	ILayout* pLayOut;
	ILayoutRender* pRender;
	POINT   pt;
	int     renderWidth;
	int     renderHeight;
	bool    isEmpty;
	InfoWndLayOut()
	{
		pLayOut = 0;
		pt.x = pt.y = 0;
		isEmpty = true;
		renderHeight = 0;
		renderWidth = 0;
		pRender  =  0;
	}
	const InfoWndLayOut& operator = (const InfoWndLayOut& info)
	{
		pLayOut = info.pLayOut;
		pt.x = info.pt.x;
		pt.y = info.pt.y;
		renderWidth = info.renderWidth;
		renderHeight = info.renderHeight;
		isEmpty = info.isEmpty;
		pRender = info.pRender;
		return *this;
	}
	InfoWndLayOut(const InfoWndLayOut& info)
	{
		pLayOut = info.pLayOut;
		pt.x = info.pt.x;
		pt.y = info.pt.y;
		renderWidth = info.renderWidth;
		renderHeight = info.renderHeight;
		isEmpty = info.isEmpty;
		pRender = info.pRender;
	}
}INFOWNDLAY,*LPINFOWNDLAY;
#define MAX_CHAT_INFO_SEG     80
typedef class ChatWndRender
{
public:
	ChatWndRender();
	~ChatWndRender();
	void ChatWndRenderCreate();
	void ChatWndRenderPopFront();
	void ChatWndRenderPushBack(const char*seg);
	void ChatWndRenderItself(HDC hdc);
	void ChatWndRenderScroll(int dt);
	void ChatWndRenderAutoScroll(int height);
	BOOL ChatWndRenderGetElement(int x,int y,LOElemInfo& info);
	void ChatWndRenderClear();
	void ChatWndRenderCopyLayoutText(ILayout* pLayout,const char* seg);
	void ChatWndRenderSetRenderPos(int pos);
public:
	HWND           hWnd;
	InfoWndLayOut  infoWndLayOut[MAX_CHAT_INFO_SEG];
//	GDIRender*     pRender;
	int            numberUsed;
	int            width ;
	DWORD          allHeight;
}CHATWNDRENDER,*LPCHATWNDRENDER;
#endif