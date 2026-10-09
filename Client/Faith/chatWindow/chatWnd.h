#ifndef _CHAT_WND_H
#define _CHAT_WND_H
//#include <vector>
/////编辑中Render中的字符，每当完成输出后，就会清空编辑的layout,并且设置这个格式字符/////////////
#define _CHAT_EIDT_DEFAULT_STRING   "<Layout width = 100000><Seg text-align=left></Seg></Layout>"

////按钮的三种状态标志///////////////
#define _CHAT_BUTTON_STATE_NORMAL    0
#define _CHAT_BUTTON_STATE_MOUSEOVER 1
#define _CHAT_BUTTON_STATE_MOUSEDOWN 2
#define _CHAT_BUTTON_STATE_DISABLE   3

//按钮处理函数指针

typedef LRESULT (CALLBACK *ChatWndProc)(HWND,UINT,WPARAM,LPARAM);
typedef BOOL (CALLBACK *DlgProcessFun)(HWND ,UINT,WPARAM,LPARAM);


///按钮资源结构体/////////
typedef struct ChatWndSRC
{
	ChatWndSRC();
	~ChatWndSRC();
	void   SetSrcIdx(int normal_id = -1,int hove_id = -1,int push_id = -1,int disable_id =-1);
	int    normal_idx ;
	int    hover_idx ;
	int    pushed_idx;
	int    disable_idx;
}CHATWNDSRC,*LPWNDSRC;

#define _CHAT_WND_ATTR_HAVE_TEXT   0x00000001
#define _CHAT_WND_ATTR_TEXT_H      0x00000002//横向排
#define _CHAT_WND_ATTR_COPY_SRC    0x00000004//
#define _CHAT_WND_ATTR_USE_DEFUALT_FONT 0x00000008
#define _CHAT_WND_ATTR_CHECKED_BUTTON   0x00000010
#define _CHAT_WND_ATTR_DISABLE          0x00000020

#define _BUTTON_ID_TO_TIMER_ID     1000
#define _TIMER_PROCESS_TIME        200
#define _FLASH_ALL_TIME            3.0f
void CALLBACK ButtonFlashTimerProc(HWND hwnd,UINT msg,UINT timer_id,DWORD currentTime);
/////窗口的父类////////
typedef class ChatWnd //: public  _IChatWnd
{
public:
	ChatWnd();
	~ChatWnd();
	virtual BOOL  ChatWndCreate(int id,long style,HWND parent,const TCHAR* text,const TCHAR* className,
		int x=0,int y=0,int width=0,int height=0) ;
	///创建tip函数//
	virtual BOOL ChatWndTipCreate(long style,char* text,int maxWidth,const TCHAR* titleText = 0);
	////////更新////////
	virtual void  ChatWndUpdate();
	virtual void  ChatWndUpdataTipText(char* text);
	virtual void  ChatWndSetTipPos();
	virtual HWND  ChatWndGetTipHandle() const {return hTipWnd;}
	////////显示////////////
	virtual void  ChatWndShow(int flags) ;
	virtual void  ChatWndShowTip(BOOL bShow);

	////////移动///////////
	virtual void  ChatWndMove(int x,int y,int width,int height,bool redraw);
	////////绘制//////////
	virtual void  ChatWndDrawItem(HDC hdc) = 0;
	
//	virtual void  ChatWndSetResource(LPWNDSRC pSrc);
	////////返回窗口句柄//////////
	virtual HWND  ChatWndGetHandle() const;
	////////返回ID///////////
	virtual int   ChatWndGetID() const ;
	////////设置显示状态////////////
	virtual void  ChatWndSetState(int state) ;
	////////返回显示状态////////////////
	virtual int   ChatWndGetState() const;
	////////返回按钮的剪裁区域//////////////
	virtual HRGN& ChatWndGetRgn() ;
	////////返回父类指针/////////////
	virtual ChatWnd*   ChatWndGetChatWndPointer() = 0;
	void             SetWndProcessFun(ChatWndProc pProcessFun);
	////////根据图片得到剪裁区域///////////////////
	static HRGN   BitmapToRgn(HBITMAP hBitmap,HRGN& hRgn,COLORREF color);
	////////按钮的矩形区域///////////////////
	virtual  const RECT&  ChatWndGetRect() const;
	virtual  HWND ChatWndGetParentHandle() const { return hControlParent;}
	virtual  void         ChatWndSetText(const char* pText);
	virtual  void         ChatWndSetAttr(int attr)
	{
		chatWndattr|=attr;
	}
	virtual  void        ChatWndDeleteSrc()
	{
		if(chatWndattr&_CHAT_WND_ATTR_COPY_SRC)
		{
			if(pTextInfo)
			{
				delete [] pTextInfo;
				pTextInfo = 0;
			}
			if(hChatRgn)
				DeleteObject(hChatRgn);
		}
	}
	virtual void         ChatWndSetAllAttr(int attr){this->chatWndattr = attr;}
	virtual const char*   ChatWndGetText() const {return pTextInfo;}
	virtual void          ChatWndSetTextNormalColor(COLORREF nColor) { normalColor = nColor;}
	virtual void          ChatWndSetTimer();
	virtual void          ChatWndKillTimer();
	virtual void          ChatWndSetFlashTime(float time){flashTime = time;}
	virtual COLORREF      ChatWndGetNormalColor() const { return normalColor;}
	virtual CHATWNDSRC&      ChatWndGetResource()  { return wndSrc;}
	virtual void          ChatWndSetFont(const char* pFont)
	{
		if(pFont==0) return;
		font[0]=0;
		strcpy(font,pFont);
	}
	virtual const char*  ChatWndGetFontName() const {return font;}
	BOOL           isFlashing;              //是否闪烁
	float          flashTime;            //闪烁时间
	float          startFlashTime;
	float          flashAllTime;
	float          startAllFlashTime;
	int          x,y;
	virtual    RECT& ChatWndGetControlRect() {return rect;}
	virtual     int  ChatWndGetAttr() const {return chatWndattr;}

	virtual   bool  ChatWndTipEnable() const { return tipEnable;}
	virtual   WNDPROC ChatWndGetProcessFun() const {return processFun;}
	virtual   void    ChatWndSetResource(int normal_idx,int hover_idx,int pushed_idx,int diable_idx);
protected:
	DWORD        chatWndattr;
	HWND         hChatWndHandle;       //窗口句柄
	HWND         hControlParent;//
	HWND         hTipWnd;//TIP提示句柄//
	int         tipMaxWidth;//tip提示最大宽度/
	int          hChatWndID;           //窗口ID
	int          iChatWndState;        //窗口状态
	HRGN         hChatRgn;             //用于不规则窗口，不规则区域
	CHATWNDSRC   wndSrc;
//	LPWNDSRC     pWndSrc;              //窗口的资源，要是只有一种状态的窗口则三种资源都相同
	RECT         rect;                 //窗口的矩形区域
	COLORREF     normalColor;
	char*        pTextInfo;
	char         font[256];
	TIMERPROC    farTimerProc;
	UINT         timerID;
	bool         tipEnable;
	WNDPROC      processFun;
}CHATWND,*LPCHATWND;
typedef class ChatButton : public ChatWnd
{
public:
	ChatButton();
	~ChatButton();
	virtual void ChatWndDrawItem(HDC hdc);
	virtual LPCHATWND   ChatWndGetChatWndPointer();
	virtual BOOL  ChatWndCreate(int id,long style,HWND parent,const TCHAR* text,const TCHAR* className,
		int x=0,int y=0,int width=0,int height=0) ;
	void SetEnableColor();
	void SetDisableColor();
protected:
public:
	COLORREF m_EnableColor;
	COLORREF m_DisableColor;
	
}BUTTON,*LPBTTUON;

#define _CLICK_FLAG_RBUTTON  0x000001
#define _CLICK_FLAG_CONTROL  0x000002
#define _CLICK_FLAG_SHIFT    0x000004
typedef class ChatInfoWnd : public ChatWnd
{
public:
	ChatInfoWnd();
	~ChatInfoWnd();
	virtual void ChatWndDrawItem(HDC hdc);
	virtual LPCHATWND   ChatWndGetChatWndPointer();
	void ChatWndProcessClickText(POINT& pt,int flags);
	//注意这个函数传递近来的buffer不要为空。
	static void ChatWndMakePlayerBaseInfo(char* buffer);
	static LRESULT CALLBACK ChatInfoWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void ChatInfoWndSetFocus(BOOL focus);
	BOOL ChatInfoWndGetFocus()const {return focus;}
	void ChatInfoWndClearChatInfo();
	bool ChatInfoWndIsScroll() const {return isScroll;}
	void ChatInfoWndEnableScroll() {isScroll = TRUE;}
	void ChatInfoWndDisableScroll() {isScroll = FALSE;}
public:
	int renderX ;
	int renderY;
	RECT renderRect;
private:
	BOOL focus;
	bool isScroll;
}CHATINFOWND,*LPCHATINFOWND;


#define _CHAT_EDIT_MOUSE_EXTRA      3

#define _CHAT_EDIT_CPY_HOT_KEY      1000
#define _CHAT_EDIT_PAST_HOT_KEY     1001
#define _CHAT_EDIT_CPY_DELETE_HOT_KEY 1002
#define _CHAT_EDIT_FLIP_CTFMON_HOT_KEY 1003
#define _CHAT_EDIT_FLIP_EN_HOT_KEY     1004

#define _CHAT_EDIT_CTFMON_DES_LENGTH 200

#define _CHAT_WND_H_SUBTEXT_FOR_CLIPBORD "<face><id>"
#define _CHAT_WND_H_SUBTEXT_END_FOR_CLIPBORD "</id></face>"
#define _CHAT_WND_H_FACE_ID_FORMAT           "<face><id>%d</id></face>"

#define _CHAT_WND_EDIT_CURSOR_FLASH_TIME 500
#define _CHAT_WND_EDIT_CURSOR_FLASH_TIME_ID 111

typedef class ChatEditBox:public ChatWnd
{
public:
	ChatEditBox();
	~ChatEditBox();
	virtual void ChatWndDrawItem(HDC hdc);
	virtual BOOL ChatWndCreate(int id,long style,HWND parent,const TCHAR* text,const TCHAR* className, int x/* =0 */,int y/* =0 */,int width/* =0 */,int height/* =0 */);
	virtual LPCHATWND ChatWndGetChatWndPointer();
//	static LRESULT CALLBACK ChatEditProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void   ChatWndProcessLButtonDown(WPARAM wParam ,LPARAM lParam);
	void   ChatEditInsertChar(WCHAR* wChar);
	void   chatEditInsertElem(LOElemInfo& Info);
	void   ChatEditWndProcessCaretLeftMove();
	void   ChatEditWndProcessCaretRightMove();
	void   ChatEditWndProcessDeleteChar(bool isLeft);
	void   ChatEditWndProcessMouseMove(WPARAM wParam,LPARAM lParam);
	void   ChatEditWndProcessLButtonUp(WPARAM wParam,LPARAM lParam);
	void   ChatEditWndProcessReturn();
	void   ChatEditWndProcessProcessHotKey(WPARAM wParam,LPARAM lParam);
	void   ChatEditWndSetFocus(BOOL focus);
	void   ChatEditWndProcessKeyDelete();
	void   ChatEditWndProcessKeyHome();
	void   ChatEditWndProcessKeyEnd();
	void   ChatEditWndShowText();
	bool   ChatEditIsCloseInfo();
	void   ChatEditSetCursorFlash();
	void   ChatEditKillCursor();
	static void ChatEditDelteReturnChar(char* pText);
	static void ChatEditDeleteReturnCharW(wchar_t* pText);
	void   ChatEditWndProcessChangeChannel(bool moveup);
	void   ChatEditSetChannelColor(int channelID);
	void   ChatEditInsertBordClipText(char* pText);
	static bool ChatEditGetElemForClipbord(char* pText,int pos,int length,bool isImage,LOElemInfo*&pElemInfo);
	static void ChatEditGetElemListForClipbord(char* pText,vector<LOElemInfo*>* pElemInfoList);
	void   ClearText( );
protected:
	void   ChatEditWndFashColor();
public:
	ILayout* pEditLayOut;
	BOOL     focus;
	POINT    pt;
	BOOL     bCtlPushed;
	int      mouseExtra;
	int      channelIndex;
	HIMC     hImc;
	float    lastSendTime;
	bool     isCloseInfo;
	bool     showCursor;
	int      dx;
}CHATEDITBOX ,*LPCHATEDITBOX;

#define COMBO_BOX_ID_PERSONAL   0
#define COMBO_BOX_ID_SYSTEM     1
#define COMBO_BOX_ID_WORLD      2
#define COMBO_BOX_ID_NEAR       3

#define NUMBER_CHANNALS_CAN_TALK 8

#define MAP_CHANNALES_TALK		 7
#define NEAR_CHANNALES_TALK      6
#define WORLD_CHANNALES_TALK     5
#define TEAM_CHANNALES_TALK      4
#define SHIZU_CHANNALES_TALK     3
#define GUOJIA_CHANNALES_TALK    2
#define ZHUHOU_CHANNALES_TALK    1
#define SYSTEM_CHANNALS_TALK     0

#define _COMBOBOX_SELECTED_BUTTON_ID   750
#define _COMBOBOX_DOWN_DIALOG_SYS_BUTTON_ID 751
#define _COMBOBOX_DOWN_DIALOG_ZHUHOU_BUTTON_ID 752
#define _COMBOBOX_DOWN_DIALOG_GUOJIA_BUTTON_ID 753
#define _COMBOBOX_DOWN_DIALOG_SHIZU_BUTTON_ID 754
#define _COMBOBOX_DOWN_DIALOG_TEAM_BUTTON_ID  755
#define _COMBOBOX_DOWN_DIALOG_WORLD_BUTTON_ID 756
#define _COMBOBOX_DOWN_DIALOG_NEAR_BUTTON_ID  757
#define _COMBOBOX_DOWN_DIALOG_MAP_BUTTON_ID   758





typedef class ChatUiComboBox
{
public:
	ChatUiComboBox();
	~ChatUiComboBox();
	bool UiComboBoxLoadIniFile(HWND hParent);
	bool UiComBoBoxCreate();
	void UiComBoBoxDrawButton(HDC hdc);
	void UiComBoBoxAdjustDownDlg();
	void UiComBoBoxProcessClickButton();
	void UiComBoBoxDrawDownDlgButton(WPARAM wParam,LPARAM lParam);
	void uiComboBoxShowDownDialog(bool show)
	{
		isShowDownDlg = show;
		if(show)
		{
			if(!IsWindowVisible(hDownDialg))
			{
				ShowWindow(hDownDialg,SW_SHOW);
			}
		}
		else
		{
			if(IsWindowVisible(hDownDialg))
			{
				ShowWindow(hDownDialg,SW_HIDE);
			}
		}
	}
	static LRESULT UiComboBoxDownDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static LRESULT UiComBoBoxDownDlgButtonProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
public:
	HWND hDownDialg;
	ChatButton  selectItemButton;
	ChatButton  downDialgButtons[NUMBER_CHANNALS_CAN_TALK];
	bool        downDialgButtonEnable[NUMBER_CHANNALS_CAN_TALK];
	int currentSelected;
	bool isShowDownDlg;
	WNDPROC     buttonsProc[NUMBER_CHANNALS_CAN_TALK];
protected:

}COMBOBOX, *LPCOMBOBOX;

#define SCROLLBAR_STATE_NORMAL     0
#define SCROLLBAR_STATE_MOUSEOVER  1
#define SCROLLBAR_STATE_MOUSEDOWN  2

typedef class ChatScrollBar
{
public:
	ChatScrollBar();
	~ChatScrollBar();
public:
//	void ChatScrollBarSetPos(float persent);
	void ChatScrollBarSetScrolls(int allLength,int oneLength);
	BOOL ChatScrollBarProcessClickUpButton(const POINT& pt);
	BOOL ChatScrollBarProcessClickDownButton(const POINT& pt);
	BOOL ChatScrollBarProcessClickScrollBar(const POINT& pt);
	int ChatScrollBarProcessMouseMove(const POINT& pt);
	BOOL ChatScrollBarProcessClickScroll();
	void ChatScrollBarLoadIniCtg();
	void ChatScrolBarUpdate();
	void ChatScrollBarDrawItem(HDC hdc);

	void ChatScrollBarSetParent(HWND hParent);
	void ChatScrollBarMoveWindow(int x,int y,int width,int height,bool updata);

public:
	int ChatScrollBarGetScrollType() const { return scrollType;}
	int ChatScrollBarGetScrollBarX() const { return scrollBarX;}
	int ChatScrollBarGetScrollBarY() const { return scrollBarY;}
	int ChatScrollBarGetScrollBarWidth() const { return scrollBarWidth;}
	int ChatScrollBarGetScrollBarHeight() const { return scrollBarHeight;}
	int ChatScrollBarGetScrollBarUpWidth() const { return scrollBarUpWidth;}
	int ChatScrollBarGetScrollBarUpHeight() const { return scrollBarUpHeight;}
	int ChatScrollBarGetScrollBarDownWidth() const { return scrollBarDownWidth;}
	int ChatScrollBarGetScrollBarDownHeight() const { return scrollBarDownHeight;}
	int ChatScrollBarGetScrollLength() const { return scrollLength;}
	HWND ChatScrollBarGetParentWnd() const { return hScrollParent;}
	bool ChatScrollBarIsSetCapture() const { return isSetCapture;}
	void ChatScrollBarSetCapture()  { isSetCapture = true;} 
	void ChatScrollBarReleaseCapture() { isSetCapture = false;}
	void ChatScrocllBarMoveUp();
	void ChatScrollBarMoveDown();
public:
	CHATWNDSRC  scrollUpPart;
	CHATWNDSRC  scrollDownPart;
	CHATWNDSRC  scrollBarPart;
	CHATWNDSRC  scrollRectPart;	
protected:
	int scrollType;
	int scrollBarX;
	int scrollBarY;
	int scrollBarWidth;
	int scrollBarHeight;
	
	int scrollBarRectWidth;
	int scrollBarRectHeight;
	int scrollBarRectState;
	int scrollBarRectPos;
	int scrollBarDt;

	int scrollBarUpWidth;
	int scrollBarUpHeight;
	int scrollBarUpState;

	int scrollBarDownWidth;
	int scrollBarDownHeight;
	int scrollBarDownSate;
	
	int scrollLength;
	int scrollLengthRectWidth;
	//一行的高度
	int oneLineLength;
	HWND hScrollParent;

	bool isSetCapture;

	POINT currentMousePos;	
}CHATSCROLLBAR,*LPCHATSCROLLBAR;

#endif