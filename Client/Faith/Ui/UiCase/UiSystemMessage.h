 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10-26
//      File_base        : KUiSystemMessage
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 系统提示信息：交易、组队……
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiSystemMessage_H
#define KUiSystemMessage_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"

#include "TLStatic.h"
#include "TLButton.h"

#include "LIST"

using namespace CEGUI;

#define SystemMessage_Name_Len 256
#define SystemMessage_Max_Count 5
#define SystemMessage_Ball_Count 5
#define SystemMessage_Cancel_Time 30
/*#define SystemMessage_CancelLevelUp_Time 60*/

class KUiSystemMessage : public KUiWndSingleton<KUiSystemMessage>
{
public:
	struct SystemMessage
	{
		enum MessageType
		{
			Idle,
			TeamRequest,
			TradeRequest,
			//TradeRefuse,
			SocialComfirm,
			TeamApplyJoin,
			GMFeedBack,
			LevelUp,
		};
		int			_id;
		char		_name[SystemMessage_Name_Len];
		MessageType _msgType;

		SystemMessage()
		{
			_id = -1;
			strcpy(_name, "");
			_msgType = Idle;
		}

		
		SystemMessage(const SystemMessage& other)
		{
			_id = other._id;
			strcpy(_name, other._name);
			_msgType = other._msgType;
		}

		bool operator==(const SystemMessage& other)
		{
			if(this->_id == other._id &&
				_msgType == other._msgType &&
				!strcmp(_name, other._name))
			{
				return true;
			}
			return false;
		}
	};
	
private:

	/*typedef std::list<SystemMessage> MsgList;
	MsgList			d_msgList;//*/
	typedef std::map<int, SystemMessage> MsgList;
	MsgList			d_msgList;

	int				d_selectMsg;

	TLStaticImage*	d_msgBall[SystemMessage_Ball_Count];
	TLStaticImage*	d_msgComfirmBox;
	TLStaticText*	d_msgbox_text;
	TLButton*		d_msgbox_yes;
	TLButton*		d_msgbox_no;
	TLButton*		d_msgbox_cancel;
	Point			d_msgBallOriPos[SystemMessage_Ball_Count];

	char			d_layoutTextHead[COMMON_CLIENT_MSG_LEN_64];
	
	//caol+
	int				d_msgBallSpeed;
	int				d_msgBallPlayCycCount;
	bool			d_bMsgBallHasPlayed[SystemMessage_Ball_Count];

	bool			d_AutoRefuse;
	DWORD			d_msgDuring[SystemMessage_Ball_Count];		// 消息停留时间

	void getChild();
	void refreshUi();
	void showTip(int msgIndex);
	void showComfirmBox(int msgIndex);
	void genMsg(const SystemMessage& msg, char* msgText);
	void removeAMessage(int index);
	void showMsg(int msgIndex);

	//caol+
	void UpdateMsgBall();

protected:
	bool onBDown(const CEGUI::EventArgs& e);
	bool onMMove(const CEGUI::EventArgs& e);
	bool onMLeave(const CEGUI::EventArgs& e);
	bool onYes(const CEGUI::EventArgs& e);
	bool onNo(const CEGUI::EventArgs& e);
	bool onCancel(const CEGUI::EventArgs& e);
	void CancelIt(void);
	/*
	bool onMouseOnText(const CEGUI::EventArgs & e);
	bool onMouseClickText(const CEGUI::EventArgs & e);
	*/
	
public:
	void clear();
	void	Init();
	static void	Show();
	void addMessage(const SystemMessage& newMsg);
	void Breathe();

    KUiSystemMessage( const CEGUI::String& name		);
    ~KUiSystemMessage(								);
};


#endif