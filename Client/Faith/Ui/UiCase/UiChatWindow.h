//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/13/2006 20:42
//      File_base        : UiChatWindow
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UI_CHAT_CHANNEL
#define UI_CHAT_CHANNEL

#include "CEGUI.h"
#include "../uicommon.h"
#include "TLStatic.h"
#include "TLButton.h"
#include "TLRadioButton.h"
#include "TLEditbox.h"
#include "ChatDataDef.h"
#include "GameDataDef.h"
#include "TLVertScrollbar.h"
#include "../UiConfigManager.h"
#include <vector>
#include <list>
#include "chatWindow/ChatWnd.h"


#define FrameNum 7
#define CHAN_MAX_TEXT_LEN 1024
#define UI_CHAT_WINDOW_INVALIDATE_CHAN_ID -1
#define UI_CHAT_WINDOW_INVALIDATE_CHAN_INDEX -1
#define UI_CHAT_WINDOW_INTERGRATION_FRAME_INDEX 0
#define UI_CHAT_WINDOW_MAX_CHAN_COUNT 10
#define UI_CHAT_WINDOW_COZE_DETERMINANT "<Obj type=%[^>]>/%[^ ] %[^\0]"
#define UI_CHAT_WINDOW_LAYOUT_MAX_HEIGHT 1000
#define UI_CHAT_WINDOW_INPUT_LAYOUT_CLIPPER_EXTEND 10
#define UI_CHAT_WINDOW_INPUT_MAX_CACHE_SENDED_TEXT_COUNT 10
#define UI_CHAT_WINDOW_UPDATE_TIMEPUSE 1000
#define UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME 5

using namespace CEGUI;
using namespace std;

class KUiExtendChatWndBtn : public KUiWndSingleton<KUiExtendChatWndBtn>
{
public:
	KUiExtendChatWndBtn(const CEGUI::String& id_name);
	~KUiExtendChatWndBtn();

	void	Init();
protected:
	bool	onClickExtendChatBtn(const CEGUI::EventArgs& args);
};

class KUiEntrustComputerBtn : public KUiWndSingleton<KUiEntrustComputerBtn>
{
public:
	KUiEntrustComputerBtn(const CEGUI::String& id_name);
	virtual ~KUiEntrustComputerBtn();
	
	void	Init();
protected:
	bool	onClickEntrustComputerBtn(const CEGUI::EventArgs& args);
};

class KUiChanMgr
{
	vector<Ui_Channel_Param>				_chanList;

	int										_inputChanId;
	const static char*						_emptyString;
#ifdef _DEBUG
	const static char*						_defaultColorString;
	const static char*						_defaultFontString;
#endif

	
	vector<string>							_latestNameList;
	int										_curNameIndex;
public:
	KUiChanMgr();
	~KUiChanMgr(){}

	const char* getChanNameById(int chanId);
	void		getChanNameById(int chanId, char * nameBuffer, int bufferLen);
	int			getChanIdByName(string name);

	bool		getChanInfo(int chanId, MapChannelInfo & info);
	const char*	getChanColor(int chanId, bool bGM);
	const char*	getChanNameColor(int chanId, bool bGM);
	const char*	getChanFont(int chanId, bool bGM);
	const char*	getChanNameFont(int chanId, bool bGM);

	const char* getPlayerNameColor();
	const char*	getPlayerNameFont();

	const char* getItemColor(int color);
	const char* getItemFont();

	void	init();
	void	regist(Ui_Channel_Param& newChan);
	void	unregist(int chanId);
	void	unregistAll();

	int		getInputChanId(){	return _inputChanId;	};
	void	setInputChanId(int inputChanId){	_inputChanId = inputChanId;	};
	int		prevInputChanId();
	int		nextInputChanId();

	//最近密语相关	
	void	clearLatestReciverList();
	string	prevReciverName();
	string	nextReciverName();
	void	addReciverName(char* name);
	const vector<string>& getLatestReciver();
	string	getCurReciverName();

	static KUiChanMgr& getSinglton();
};

class KUiChannelCentre: public KUiWndSingleton<KUiChannelCentre>
{
	//UI控件参数
	TLStaticImage*	d_framePanel[FrameNum];
	
	//文字显示
	TLStaticText*	d_frameContent[FrameNum];
	TLStaticText*	d_textCarrier[FrameNum];
	vector<TLStaticText*>	d_textItems[FrameNum];
	int				d_curTopItem[FrameNum];				//最上面那个文字控件的下标、用于下一次文字刷新时替换掉
	int				d_wndWidth[FrameNum];
	TLStaticImage*	d_hoverImage;						//链接hover状态图片

	//控件
	TLRadioButton*	d_frameBtn[FrameNum];
	TLVertScrollbar*d_scrollBar[FrameNum];
	TLStaticImage*	d_newMsgImg[FrameNum];
	TLButton*		d_extendChatBtn;

	TLButton*		d_toTopBtn[FrameNum];
	TLButton*		d_toBottomBtn[FrameNum];

	TLButton*		d_moveBtn;
	
	int				d_chanBtnWidth;
	
	int				d_maxHeight;
	int				d_minHeight;
	bool			d_resizing;

	int				d_systemFrameIndex;					//系统页面单独做特殊处理
	
	struct FrameInfo
	{
		vector<int>		_chanList;
		bool			_used;
		FrameInfo()
		{
			_chanList.clear();
			_used = false;
		}
	};

	FrameInfo		d_frameInfo[FrameNum];

	struct ChanMsg
	{
		string	msg;
		int		channelId;
	};
	vector<ChanMsg>	d_delayMsgs;
	int d_delayTimeOut;
public:
	KUiChannelCentre(const CEGUI::String& id_name);
	~KUiChannelCentre();
	
	void	Init();
	static void Show();

	bool	updateSelf(const EventArgs& timeArg);

	//特殊处理——系统面板从主聊天窗口中剥离出来
	int		splitSystemFrame(int frameIndex);

	int		creatANewFrame(const char* frameName);
	bool	isExsitAFrame(char* frameName);
	void	closeAFrame(int frameIndex);
	void	closeAllFrame();

	void	registChannel(int frameIndex, int chanId);
	void	unregistChannel(int chanId);
	void	unregistChannel(int frameIndex, int chanId);
	
	void	freshFrameChannel(int frameIndex);

	void	delayRecv(int chanId, char* msg);
	void	recvMessage(int chanId, BYTE* byBuffer);
	void	recvCustomMessage(int channelId, char* message);
	void	recvCustomMessage(int channelId, vector<CHAT::CommonChatData>& message);
	void	recvCustomMessageConst(int channelId, const char* message);
	void	recvCozeMessage(BYTE* byBuffer );
	void	playCozeSound();

	static void Hide();

	void	showFrame(int frameIndex);
	void	showBackground(bool show);

	void    showSystemFrame(bool show);

	void    toSysMsg(const char* msg);

	void	showExtendChatWnd();

protected:
	bool	clickFrameBtnDown(const EventArgs& args);
	bool	clickFrameBtn(const EventArgs& args);
    bool	clickText(const EventArgs& args);
    bool	hoverText(const EventArgs& args);
    bool	mouseOutText(const EventArgs& args);
	bool	scroll(const EventArgs& args);
	bool	onTextPanelWheelChanged(const EventArgs& args);
    bool	clickToTop(const EventArgs& args);
	bool	clickToButtom(const EventArgs& args);

	bool	onMoveBtnDown(const EventArgs& args);
	bool	onMoveBtnUp(const EventArgs& args);
	bool	onMoveBtnMove(const EventArgs& args);

	bool	onClickExtendChatBtn(const EventArgs& args);
//	bool	onClickGM(const EventArgs& args);
private:
	bool	validateFrameIndex(int frameIndex);
	void	layoutChanMenu();
	void	getChild();
	void	hideAllFrame();

	int		getChannelIndexById(int channelId);
	void	layoutBtn();

	void	adjustWindowHeight(int height);
	void	freshScrollBarStep(int panelIndex);

	//接收数据时做必要的转换
	void	ChangeTextColor(char * segText, const char * color, int chanId, bool bGM);
	void	ChangeTextFont(char * segText, const char * font, int chanId, bool bGM);
	void	chatTextToLoelem(const char* text, char* segText, int chanId,bool bGm);
	
	//文字的位置
	void	dispatchMessage(int chanId, const char* segStr);
	void	addAMessage(int panelIndex, const char* segStr);
	void	relayoutText(int panelIndex);
	void	adjustLayoutPos(int panelIndex);
	void	showNewMsg(int frameIndex);

	char	d_msg[LAYOUT_TEXT_MAX_LEN + 1];
};

/*!
\brief
	Chat input window.	
*/
class KUiChatInputWnd : public KUiWndSingleton<KUiChatInputWnd>
{
	friend class KUiChannelCentre;
	friend class ChatEditBox;

	typedef			vector<LOElemInfo> Centence;
public:
	KUiChatInputWnd(  const CEGUI::String& id_name  );
	~KUiChatInputWnd( void							);

public:
	void			show();
	void			Init				( void							);

	bool			isCurInput();

	void			doSendMessage();
	void			sendGMCommand( string command );
	
	void			registChannel(int chanId, char* chanName);
	void			unregistChannel(int chanId);
	void			switchChannel(int chanId);

	void			setLastSender(char* lastSender);
	void			setLastReciver(char* lastReciver);
	void			showLastSender();
	void			showLastReciver();

	void			cacheMessage(LOElemInfo* elem, int elemCount);
	void			prevMessage(ILayout* pLayout);
	void			nextMessage(ILayout* pLayout);

	bool            haveCachedMessage(bool prev) const;
	void			clearAllCachedMessage();
	bool			isCentenceEqual(Centence& centence1, Centence& centence2);

	void			write(LOElemInfo& newElem);
	void			write(const char* ansiText);
	void			clearText();

	void			clearLatestChatMenu();

	void			freshChanName();
private:
	void			getChild();
	
	void			showText();

	bool			onInputBoxShow(const EventArgs& args);
	bool			onInputBoxClose(const EventArgs& args);

    bool			handleKeyDown(const EventArgs& args);
    bool			handleSelectAFace(const EventArgs& args);
    bool			handleFaceMouseIn(const EventArgs& args);
    bool			handleFaceMouseOut(const EventArgs& args);
    bool			handleFaceBtnDown(const EventArgs& args);
    bool			handleLBDown(const EventArgs& args);
    bool			handleLBUp(const EventArgs& args);
    bool			handleMouseMove(const EventArgs& args);
	bool			handleSend(const EventArgs& args);
	bool			handleKeyInput(const EventArgs& args);
	bool			clickChanPopBtn(const EventArgs& args);
	bool			clickChanBtn(const EventArgs& args);
	bool			clickLatestBtn(const EventArgs& args);
	bool			clickChanCtrlBtn(const EventArgs& args);
	bool			onHide(const EventArgs& args);
	
	void			recordReciver(char* lastReciver);
	
	void			useTemplate(TLButton* wnd, TLButton* templateWnd);
	void			layoutChanMenu();
	void			flashColor();
	int				getSelectionText(char*& text);

	bool			canSay(int chatInputId);
	static int		layoutGOCount(ILayout* layout, LOGameObjType gotype);
	//render  add,为粘贴表情//////////////////////////////////////////////////////////////////////////
	void            WriteFaceFromClipbord(char* pText);
private:
	char			d_lastReciver[COMMON_CLIENT_MSG_LEN_32];
	char			d_lastSender[COMMON_CLIENT_MSG_LEN_32];
	
	list<Centence>	d_msgCache;
	int				d_curCacheIndex;


	//频道选择面板（包括最近密语）
	TLButton*		d_chanPopBtn;
	int				d_chanBtnYOff;

	TLStaticImage*	d_chanSelMenu;
	
	TLButton*		d_chanSelBtn[UI_CHAT_WINDOW_MAX_CHAN_COUNT];
	TLButton*		d_chanCtrlBtn[UI_CHAT_WINDOW_MAX_CHAN_COUNT];
	int				d_chanId[UI_CHAT_WINDOW_MAX_CHAN_COUNT];

	TLStaticImage*	d_latestChatMenu;
	TLButton*		d_latestChatBtn[UI_CHAT_MAX_SAVED_LATEST_CHAT_NAME];

	//
	TLEditbox*		d_inputBox;
	TLButton*		d_faceBtn;
	TLStaticText*	d_facePanel;
	TLStaticImage*	d_facePanelSelectFrameImage;
	TLButton*		d_sendBtn;
	bool			d_lbdown;
	char			d_msg[LAYOUT_TEXT_MAX_LEN + 1];
	bool			d_justOpenFromKey;
};

#endif 
