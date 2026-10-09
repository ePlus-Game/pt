 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/03/2006 16:56
//      File_base        : UiChatCentre
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UICHATCENTRE_H
#define UICHATCENTRE_H
#include "ChatDataDef.h"

#include "CEGUI.h"
#include "../uicommon.h"
#include "chatdatadef.h"
#include "CoreShell.h"
#include <map>
#include <list>
#include <string>
#include <vector>
#include "TLTreeItem.h"
#include "TLTree.h"
#include "TLVertScrollbar.h"
#include "TLTreeEx.h"
#include "../UiElem/TLEditbox.h"

using namespace CEGUI;

class TreeItemEx;

typedef std::vector<TreeItemEx*> TreeItemList;

enum ChatNotifyType
{
	PrivateChatNotify,
	RoomChatNotify,
	CreateChatRoomNotify,
	AddRoomMemberNotify,
	JoinRoomMemberNotify,
	AddFriendReturnNotify,
	KickMemberNotify,
	LeaveMemberNotify,
	ChangeRoomOwnerNotify,
	ChangeScreenNotify,
};

class ListBoxItemChatRoom : public ListboxTextItem
{
public:

	ListBoxItemChatRoom(const String& text, unsigned int item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = true);
	virtual ~ListBoxItemChatRoom( void ) {};
public:
	void	setRoomID( int roomID )
	{
		d_roomID = roomID;
	}
	int	getRoomID( void )
	{
		return d_roomID;
	}
private:
	int	d_roomID;
};


class KUiP2RPopmenu : public KUiWndSingleton<KUiP2RPopmenu>
{
public:
	KUiP2RPopmenu( const String& id_name );
	~KUiP2RPopmenu();
public:
	static void Show(const std::string& str, int nRoomID);
	void Init( void								);
protected:
	bool handleKicK( const EventArgs& args );
	bool handleScreen( const EventArgs& args );
	bool handleUpAdmin( const EventArgs& args );
	bool handleClose( const EventArgs& args );
	bool handleMouse( const EventArgs& args);
private:
	std::string d_string;
	int			d_room;
};


class KUiPlayerInfo: public KUiWndSingleton<KUiPlayerInfo>
{
public:
	KUiPlayerInfo( const String& id_name );
	~KUiPlayerInfo();
public:
	virtual void Init( void								);
	void Updatedata( const PlayerInfo& player );
protected:
	bool handleClose( const EventArgs& args );
private:
	Window* m_Name;		
	Window* m_Level;
	Window* m_Organise;
	Window* m_Favor;
};
/*!
\brief
	Chat room class.	
*/
class KUiChatRoom : public KUiWnd
{
	friend class KUiChatCentre;
private:
    KUiChatRoom();
    ~KUiChatRoom();

public:
	void Show( bool bPrivate=true )
	{
		bool    bShowBefore = IsVisible();

		KUiWnd::Show();
		
		if (m_pThisWnd)
		{
			if (bPrivate)
			{
				m_pThisWnd->getChild(  m_pThisWnd->getName()+"/Add")->hide();	
			}//endif
			else
			{
				m_pThisWnd->getChild(  m_pThisWnd->getName()+"/Add")->show();
			}

			if (!bShowBefore)
			{
                ((TLStaticImage *)m_pThisWnd)->Shake(false,true,4);
			}//endif
			
		}

		if (d_inputBox)
		{
			d_inputBox->show();
			d_inputBox->activate();
		}//endif

		m_bPrivate=bPrivate;
	}//*/
	bool						isHaveFocus				( void														);
	void						write					( LOElemInfo& newElem										);
	void						write					( const char* ansiText										);
    void                        setOwnerState           ( const bool  bOwner);
private:
	/************************************************************************/
	/*						Virtual function                                */
	/************************************************************************/
	void						AddEvent				( void														);
	/************************************************************************/
	/*						Event function.                                 */
	/************************************************************************/
	bool						handleScroll			( const EventArgs& args										);		
	bool						handleClickList			( const EventArgs& args										);
    bool						handleSend				( const EventArgs& args										);
	bool						handleExit				( const EventArgs& args										);	
	bool						handleAdd				( const EventArgs& args										);	
	bool						handleKeyDown			( const EventArgs& args										);
    bool                        handleFaceHide          ( const EventArgs& args                                     );
	bool                        handleMinimize			( const EventArgs& args										);
	/************************************************************************/
	/*                      Chat operation send.                            */
	/************************************************************************/
	void						doP2PSendMessage		( void );
	void						chatToSomeone			( const std::string &strReceiver, BYTE *pMsg, int nMsgLen	);	
 	void						chatInRoom				( DWORD dwRoomId, BYTE *pMsg, int nMsgLen					);
	/************************************************************************/
	/*						Chat room Operation		                        */
	/************************************************************************/
	void						setChatMode				( bool bIsRoomChat											);
 	void						addMemberToRoomReq		( DWORD dwRoomId, const std::string &strName				);
 	void						kickRoomMember			( DWORD dwRoomId, const std::string &strName				);
	/************************************************************************/
	/*						Chat message recv.		                        */
	/************************************************************************/
	void						recvMsgFromSomeone		( BYTE *pMsg												);
	void						recvMsgFromRoom			( BYTE *pMsg												);
	/************************************************************************/
	/*						Chat room manager.		                        */
	/************************************************************************/
	void						onCreateChatRoomNotify	( BYTE *pMsg												);
 	void						onJoinRoomNotify		( BYTE *pMsg												);
 	void						onMemberLeaveRoomNotify	( BYTE *pMsg												);
 	void						onAddRoomMemberNotify	( BYTE *pMsg												);
 	void						onKickRoomMemberNotify	( BYTE *pMsg												);
 	void						adjustLayoutPos			( void														);
	void						doSendMessage			( void														);
	bool						handleKeyInput			( const CEGUI::EventArgs& args								);
	void						flashColor				( void														);
	void						showText				( void														);
	void						clearText				( void														);
	int							getSelectionText		( char*& text												);
	bool						clickText				( const CEGUI::EventArgs& args								);
	bool						hoverText				( const CEGUI::EventArgs& args								);
	bool						handleFaceBtnDown		( const EventArgs& args										);
	bool						handleSelectAFace		( const EventArgs& args										);
	bool						onHide( const EventArgs& args );
	////////render  添加剪贴板//////////
	void                        WriteTextFromClipbord(char* pText);

private:
	int							m_nRoomID;
	bool						m_bRoomChat;
	std::string					m_strTargetName;
	std::string					m_strMsgBuff[2];
	int                         m_CurBuffIndex;
	TLEditbox*					d_inputBox;
	CEGUI::TLTree*				d_listBox;
	TreeItemList				d_itemList;
	TLVertScrollbar*			d_scrollBar;
	TLButton*					d_faceBtn;
	TLStaticText*				d_facePanel;
    bool                        m_bPrivate;
	bool                        m_bOwner;
};


/*!
\brief
	This window is notify chat centre chat message arrive.	
*/
#define MAX_WORD_NUM  5

class KUiChatNotify : public KUiWndSingleton<KUiChatNotify>
{
public:
	KUiChatNotify( const String& id_name );
	~KUiChatNotify();
	
public:
	static void	Show( void );
	void	Init();

protected:
	bool	handleChatRoom				(const EventArgs & args);
	bool	handleShowRoomList			(const EventArgs & args);

private:
};

struct KChatRoomPtr
{
	KChatRoomPtr()
	{
		pRoom = NULL;
	}
	KUiChatRoom *pRoom;
};

typedef std::list<CEGUI::String> RoomNameList;
class KUiChatRoomListMenu : public KUiWndSingleton<KUiChatRoomListMenu>
{
public:
	KUiChatRoomListMenu(const String & id_Name);
	~KUiChatRoomListMenu();

public:
	void			Init					();
	void			AddPlayerName			(std::string playerName);
	void			DeletePlayerName		(std::string playerName);
	bool			IsNameListEmpty			();
	void			SetPos					(Point pos);
	static void		Show					();

protected:
	bool			Button0_MouseClick		(const EventArgs & args);
	bool			Button1_MouseClick		(const EventArgs & args);
	bool			Button2_MouseClick		(const EventArgs & args);
	bool			Button3_MouseClick		(const EventArgs & args);
	bool			Button4_MouseClick		(const EventArgs & args);

	void			ClickButton				(TLButton * button);
	void			RefreshPlayerName		();
	void			ResizeMenu				();

protected:
	enum ChatRoomListMenuButton
	{
		Button0 = 0,
		Button1,
		Button2,
		Button3,
		Button4,
		ButtonCount,
	};
	RoomNameList m_NameList;
	TLButton * m_ButtonList[ButtonCount];
};

typedef std::map<int,KChatRoomPtr> KUiChatRoomSetP2R;
typedef std::map<String,KChatRoomPtr> KUiChatRoomSetP2P;

struct KChatMsg
{
	KChatMsg()
	{
		dwRoomID = -1;
	}
	std::string	strMsg;
	std::string	strSender;
	DWORD		dwRoomID;
};
typedef std::list<KChatMsg> KChatMsgList;

class TreeItemEx : public TLTreeItem
{
public:;
	TreeItemEx( const String& text, uint item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = false ); 
	~TreeItemEx() {}
public:

	inline void	setIDEx( DWORD dwID )
	{
		d_dwID = dwID;
	}

	inline DWORD	getIDEx()
	{
		return d_dwID;
	}


	virtual void drawSelf(KRenderCache* panelCache, const Point& position, const Rect& clipper)
	{
	   if (d_selected && (d_pushedImage != NULL))
	   {
			panelCache->cacheImage(d_pushedImage, position, clipper);
	   }

	   const Font* fnt = getFont();
	   if (fnt != NULL)
	   {
	   		colour temp = colour(0.25f, 0.25f, 0.25f, 0.25f);
			Rect finalArea(position, clipper.getSize());
		    if ( d_isOnLine )
			{
				panelCache->cacheText(d_itemText, finalArea, clipper, fnt, LeftAligned, d_textCols.d_top_left);
			}
			else
			{
				panelCache->cacheText(d_itemText, finalArea, clipper, fnt, LeftAligned, temp);
			}

	   }

	}
	
	void	setIsViewOtherColor( bool bIsOnline )
	{
		d_isOnLine = bIsOnline;
	}

	bool	getIsViewOtherColor()
	{
		return d_isOnLine;
	}
private:
	DWORD	d_dwID;
	bool	d_isOnLine;
};

class KUiChatCentre : public KUiWndSingleton<KUiChatCentre>
{
	enum {
		friendPage,
		enemyPage,
		screenPage,
		chatroomPage,
		temporaryPage,
	};
public:
	enum  FRIEND_WINDOW_MESSAGE
	{
		INPUT_ADD_FRIEND_NAME = 1,
		INPUT_DEL_FRIEND_NAME,
		SELECT_DEL_FRIEND_NAME,
		INPUT_SCREEN_PLAYER_NAME,
		IS_DELETE_FRIEND,
		INPUT_NEW_GROUP_NAME,
		NEW_NAME_GROUP,
		IS_DELETE_CROUP,
		SELECT_DEL_GROUP_NAME,
		INPUT_PLAYERNAME_TO_GROUP,
		IS_DEL_ENEMY_NAME,
		SELECT_DEL_ENEMY_NAME,
		IS_DEL_SCREEN_NAME,
		SELECT_DEL_SCREEN_NAME,
		YES,
		NO,
		DEL,
		DELELE_FRIEND,
		DELELE_ENEMY,
		IS_ADD_SCREEN,
		IS_IN_FRIEND_LIST,
		IS_IN_ENEMY_LIST,
		IS_IN_SCREEN_LIST,
		IS_IN_TEMPLATE_LIST,
		SELECT_PLAYER_TO_SCREEN,
		ADD_TO_CHATROOM,
		CREATE_CHATROOM,
		SELF,
        INPUT_ADD_ENEMY_NAME,
		ADD_TO_FRIEND,
		INPUT_GOUPNAME_IN,
	};
public:
    KUiChatCentre( const String& id_name	);
    ~KUiChatCentre(							);

public:
	KUiChatRoom*				getActiveChatRoom( void );
	void						setActiveChatRoom( KUiChatRoom* pRoom );
	static void					Show							( void															);
	static void                 Hide                            ( void                                                          );
	void						Init							( void															);
	/************************************************************************/
	/*				Notify function                                         */
	/************************************************************************/
	static void					ProcessChatNotify				( DWORD dwNotifyID, DWORD dwParam, BYTE *pMsg					);
	static void					ProcessFriendNotify				( DWORD dwNotifyID, BYTE *pMsg									);
	/************************************************************************/
	/*				Chat room operation for private chat                    */
	/************************************************************************/
	static int					RecvPrivateChatMsg				(																);
	static void					OpenChatRoomReqP2P				( const std::string& strTargetName, const std::string& strMsg	);
	/************************************************************************/
	/*				Chat room operation for chat in room                    */
	/************************************************************************/
	static void					clickAddToChatRoom				( int nRoomID );
	static void					OpenChatRoomReqP2R				( void															);
	static void					CloseChatRoomP2R				( int nRoom														);
	static void                 OfflineOperation                (void         );
	static void                 PkValueChangeNotify             (const char * pName,int nPkValue             );
	KChatMsgList		&		GetChatMsgList					(																);
	/************************************************************************/
	/*			    Close all of the chat window                            */
	/************************************************************************/
	static void					CloseAllChatRoom				( void															);
	void						AddChatRoom( const char* szRoomName, int nRoomID );
	void						DelChatRoom( int nRoomID );

private:
	void						chatTextToLoelem		( const char* text, char* segText, bool bSelf, bool bP2P = true			);
	/************************************************************************/
	/*                          Common function                             */
	/************************************************************************/
	bool						clickAddFriendBtn				( const EventArgs& args											);
	bool						clickDelFriendBtn				( const EventArgs& args											);
	bool						clickCloseBtn					( const EventArgs& args											);
	void						HideAllList						( void															);
	void                        sortListByOnline                ( TLTreeEx * pList                                              );
	/*!
	\brief
		Common popmenu.
	*/
	bool                        HandleScrol                     ( const EventArgs& args	                                        );
	bool						clickFListMenu					( const EventArgs& args											);
	bool						closeFListMenu					( const EventArgs& args											);
	bool						clickCommonChat					( const EventArgs& args											);
	bool						clickChat						( const EventArgs& args											);
	bool						clickInvite						( const EventArgs& args											);
	bool						clickFriendDetail				( const EventArgs& args											);
	bool						clickPreventChat				( const EventArgs& args											);
	bool						clickDeleteFriend				( const EventArgs& args											);
    void                        showAllFriendSubManu            ( void                                                          );
	bool						ListMenuShow 					( const EventArgs& args											);
	bool						ListMenuHide					( const EventArgs& args											);
	void                        disableAllList                  ( void                                                          );
	void                        enableAllList                   ( void                                                          );
	
	/************************************************************************/
	/*							Friend list page                            */
	/************************************************************************/
	bool						showFriendList					( const EventArgs& args											);		
    bool                        freindlistDC                    ( const EventArgs& args                                         );
	/*!
	\brief
		Friend group popmenu.
	*/
	
	bool						clickAddGroup					( const EventArgs& args											);
	bool						clickRenameGroup				( const EventArgs& args											);
	bool						clickDeleteGroup				( const EventArgs& args											);
	bool						clickAddToGroup					( const EventArgs& args											);
	bool						clickPlayerInfo					( const CEGUI::EventArgs& args									);
	bool						clickOpenRoom					( const CEGUI::EventArgs& args									);
	bool						clickLeaveRoom					( const CEGUI::EventArgs& args									);
    bool                        clickAddToFriend                ( const CEGUI::EventArgs& args									);
	bool                        clickAddToSpecificGroup         ( const CEGUI::EventArgs& args                                  );
    bool                        clickInviteShizu                ( const CEGUI::EventArgs& args                                  );
	bool                        clickInviteZhuhou               ( const CEGUI::EventArgs& args                                  );
	/*!
	\brief
		Friend list OkWindow function. 		
	*/
	bool						OnOk							( const EventArgs& args											);
	bool						OnCancel						( const EventArgs& args											);
	bool						onOkWndShow						( const EventArgs& args											);
	bool						onOkWndHide						( const EventArgs& args											);
	/*!
	\brief
		Friend list callback function. 		
	*/
	void						onRecvFriendListArrive			( void															);
	void						onRecvBlackFriendListArrive		( int nGroupID													);
	void						onRecvEmeyFriendListArrive		( int nGroupID													);
	void						onRecvTemporaryFriendListArrive	( int nGroupID													);

	/************************************************************************/
	/*							Enemy list page                             */
	/************************************************************************/
	bool						showEnemyList					( const EventArgs& args											);
    bool                        clickAddEnemyBtn                ( const EventArgs& args											);
	bool						clickDelEnemyBtn				( const EventArgs& args											);
	/************************************************************************/
	/*							Screen list page                            */
	/************************************************************************/
	bool						showScreenList					( const EventArgs& args											);
	bool						clickAddScreenBtn				( const EventArgs& args											);
	bool						clickDelScreenBtn				( const EventArgs& args											);
	/************************************************************************/
	/*							Chat room list page                             */
	/************************************************************************/
	bool						showChatList					( const EventArgs& args											);
	bool						handleDCList					( const EventArgs& args											);
	/************************************************************************/
	/*							Temporary list page                         */
	/************************************************************************/
	bool						showTemporaryList				( const EventArgs& args											);
	/************************************************************************/
	/*							Chat room operation notify	                */
	/************************************************************************/
	void						onCreateChatRoomNotify			( DWORD dwRoomID, BYTE *pMsg									);
	void						onRecvMsgFromSomeone			( CHAT::PCHATMSG_BY_NAME pMsg									);
	bool						handleCreateRoom				( const EventArgs& args											);	
	//likun
	bool						handleMouseClick				( const EventArgs& args );
	bool						clickSelectItem					( const EventArgs& args );
	bool						handleScreenShortCut( const EventArgs& args );
	void						commonAddItem( int nID, Tree *pTreeList, unsigned int iGroupId );
	bool						handleFriendScroll( const EventArgs& args );
	/*************************************************************************/
	/*                          Drag Item handler                            *
	 *************************************************************************/

	bool                        handleAcceptInList        (const EventArgs& args );
	bool                        handleDragItemMove        (const EventArgs& args );
    
	/*****************************************************************************/
	/*                          PopupWindowHandler                               */
	/*****************************************************************************/

	enum PopupState
	{
        PS_INVALID = 0,
	    PS_FRIENDLIST ,	
		PS_GROUPLIST  ,
	};

	bool                        handleOkWindowPopUp       (const EventArgs& args);

	void                        prepareForFriendPopUpList ();
	void                        prepareForGroupPopUpList  ();

	bool                        handleMouseClickForPopup  (const EventArgs& args);
    void                        handleClickInPop1         (void);
	void                        ResetPopupWindow          (void);
	void                        AddToPupupWindow          (const TreeItemEx *  pItem);
private:
	bool                        posInRect                 (const Point    & pos ,const Rect & rect );
private:
	void						showIsOperation( FRIEND_WINDOW_MESSAGE msg );
	static void					YesDelGroup(void);
	static void					IsAddScreenList(void);
	static void					IsDelFriend(void);
	static void					IsDelScreen(void);
	static void					IsDelEnemy(void);
	void						setTreePos( TLTreeEx *pTree, Window *pWindow );
private:
    static const char*			FriendButtonID;
	static const char*			EnemyButtonID;
	static const char*			ScreenButtonID;
    static const char*			CharRoomButtonID;
	static const char*			TemporaryButtonID;
	static const char*			AddFriendButtonID;
	KUiChatRoomSetP2R			m_ChatRoomSet;
	KUiChatRoomSetP2P			m_PrivateChatSet;
	KChatMsgList				m_ChatMsgList;
	KUiChatRoom*				m_pActiveRoom;
	DWORD						m_GroupIDList[MAX_FRIENDGROUP_COUNT];
	char						m_GroupNameList[MAX_FRIENDGROUP_COUNT][CLIENT_NAME_AND_TITLE_MAX+1];
	TreeItemEx *				m_pCurSelectItem;		//当前选中的菜单项的名字
	//好友列表的弹出菜单及菜单项
	Window*						m_pFriendListMenu;		//好友操作菜单
	PushButton*					m_pCommonChat;
	PushButton*					m_pChat;				//和选定好友私聊
	PushButton*					m_pInvite;				//邀请好友入队
	PushButton*					m_pFriendDetail;		//好友详细信息
	PushButton*					m_pPreventChat;			//阻止某人与你私聊（将其加入黑名单）
	PushButton*					m_pDeleteFriend;		//删除好友
	PushButton*                 m_pAddToFriend;         //加为好友
	PushButton*                 m_pAddToSepecificGroup; //加好友到特殊组
	PushButton*					m_pPlayerInfo;			//添加好友到组	 
	PushButton*					m_pInviteShizu;			//添加好友到组	
	PushButton*					m_pInviteZhuhou;		//添加好友到组	
	//好友分组的弹出菜单及菜单项
	Window*						m_pFriendGroupMenu;		//好友弹出菜单以下为菜单项
	PushButton*					m_pAddGroup;			//添加一个新分组
	PushButton*					m_pRenameGroup;			//重命名选定分组
	PushButton*					m_pDeleteGroup;			//删除该分组
	PushButton*					m_pAddToGroup;			//添加好友到组	 
	PushButton*					m_pOpenRoom;
	PushButton*					m_pLeaveRoom;	        //通用弹出窗口
	StaticImage*				m_pOkWindow;			//通用弹出窗口
	Window*                     m_pOkPopup;             //弹出窗口的pop按钮
	Editbox*					m_pName;				//通用弹出窗口上的编辑框
	StaticText*					m_pOkWinText;
	//好友详细信息窗口
	PushButton*					m_pFriendCloseBtn;
	//底部显示的按钮
	PushButton*					m_pAddFriend;			//添加好友
	PushButton*					m_pDelFriend;			//删除好友
	PushButton*					m_pInvFriend;			//邀请好友入队
	PushButton*					m_pAddEnemy;			//添加仇人
	PushButton*					m_pDelEnemy;			//删除仇人
	PushButton*					m_pAddScreen;			//加入黑名单
	PushButton*					m_pDelScreen;			//黑名单删除
	PushButton*					m_pCreateRoom;			
	
	Window*						m_FriendList;	
	Window*						m_EnemyList;	
	Window*						m_ScreenList;	
	Window*						m_ChatRoomList;
	Window*						m_TemporaryList;
	int							m_State;		
	TreeItemList				m_freelist;	
	
	//滚动条
	CEGUI::Window				*m_pFriendTreeCliper;
	CEGUI::TLTree				*m_pFriendTree;
	CEGUI::TLVertScrollbar		*m_pFriendTreeScrol;

	//根窗口
	Window*						 m_root;
	int							 m_nCurRoomID;

	//DragItem
	bool                         m_IsDraging;
	//Popup
	CEGUI::TLTree               *m_Popup1;
	TLStaticImage               *m_PopupBack; 
    PopupState                   m_PopupState;
	TreeItemList                 m_PopupItemList;
	Size                         m_OkNormalSize;
};

#endif