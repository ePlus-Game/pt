//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/27/2006 19:16
//      File_base        : UiMailCentre
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIMAILCENTRE_H
#define UIMAILCENTRE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include ".\TLTreeItem.h"
#include "TLButton.h"
#include "TLVertScrollbar.h"
#include "UiChatCentre.h"
#include <string>

using namespace std;

class TreeItemMail : public CEGUI::TLTreeItem
{
public:
	TreeItemMail( const CEGUI::String& text, CEGUI::uint item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = true );
	~TreeItemMail();

public:
	void	setMailID( int nMailID )
	{
		d_nMailID = nMailID;
	}
	int		getMailID()
	{
		return d_nMailID;
	}

	static const colour  DefaultReadColour;

private:
	int	d_nMailID;
};

void	processPayMoney();
void	processDeleteMail();
void	processPostMoney();
void	processSendBackMail();

class KUiMailCentre : public KUiWndSingleton<KUiMailCentre>
{
	friend void	processPayMoney();
	friend void	processDeleteMail();
	friend void processPostMoney();
	friend void	processSendBackMail();
public:
	KUiMailCentre(  const CEGUI::String& id_name	);
	~KUiMailCentre(									);

	friend class KUiAdapter;
public:
	static void Show();
	static void OnRecvMailRet( BYTE* byParam ); 
	static void OnSendMailRet( BYTE* byParam ); 
	static void OnDelMailRet( unsigned int uParam ); 
	void		OnGetPlusItem( int nRet );
	void		Init();

	void		OnRecvMailListRet( BYTE* byParam );
	inline int	GetPayMoney() { return d_payMoney; };

	void		RecvObj();

private:
	bool	handleClose( const CEGUI::EventArgs& args );
	bool	handleCallBackMail( const CEGUI::EventArgs& args );
	bool	handleClearText( const CEGUI::EventArgs& args );
    bool	handleSendMail( const CEGUI::EventArgs& args );
	bool	handleDelMail ( const CEGUI::EventArgs& args );
	bool	handleShowRecv( const CEGUI::EventArgs& args );
	bool	handleShowSend( const CEGUI::EventArgs& args );
	bool	handleOpenMail(const CEGUI::EventArgs& args );
	bool	onMouseMove(const CEGUI::EventArgs& e);
	bool	onMouseLeave(const CEGUI::EventArgs& e);
	bool	onLBDown(const CEGUI::EventArgs& e);
	bool	onMouseClickMoney(const CEGUI::EventArgs& e);
	bool	onMouseCheckMoney(const CEGUI::EventArgs& e);
	bool	onMouseLeaveMoney(const CEGUI::EventArgs& e);
	bool	onMouseClickItem(const CEGUI::EventArgs& e);
	bool	onHandlePayMoney(const CEGUI::EventArgs& e);
	bool	onHandlePostMoney(const CEGUI::EventArgs& e);
	bool	handleKeyDown	( const CEGUI::EventArgs& args		);
	bool    edtTitle_TextChanged( const CEGUI::EventArgs& args );
//	bool	handleShown( const CEGUI::EventArgs& args );
//	bool	handleHidden( const CEGUI::EventArgs& args );
	bool	handleTreeScroll( const CEGUI::EventArgs& args );
	bool	PopMenu_MouseWheel( const CEGUI::EventArgs& e );

	bool	handleSendBack( const CEGUI::EventArgs& args );
	bool	handleOpenFriendList( const CEGUI::EventArgs& args );
	
	int		FillFriendList( const String& nameFilter );
	
	bool	btnSelRecent_MouseClick( const CEGUI::EventArgs& args );
	bool	btnNextPage_MouseClick( const CEGUI::EventArgs& args );
	bool	btnPrevPage_MouseClick( const CEGUI::EventArgs& args );

	bool	handleSelFriend( const CEGUI::EventArgs& args );
	bool	handleFriendScroll( const CEGUI::EventArgs& args );

	void	HideFriendsList();
	void	ChangeMoneyTile();
	void	UpdateSendTax(bool bHasItem = false);

	void	CleanNameList();
	void	ShowNameList( int itemHeight );

	void	RefreshFriendList(const String& filter, bool forceVisible = false );
	void	RefreshRecentList(const String& filter);

	void	RefreshPageShow();

	void	ShowPageItems( bool show = true );
	void	ShowCapability( int mailCount );
//	void	SetCurrentPage( int curPage );
	void	GotoPage( int toPage );
private:
	CEGUI::Window*	d_recvWnd;
	CEGUI::Window*	d_sendWnd;
	TLButton*		d_sendBack;
	int				d_curMailId;
	bool			d_bPayMoney;
	bool			d_bCanDrag;

	static int		d_recvMoney;
	static int		d_payMoney;
	bool			m_hasMoney;
	bool			m_hasItem;

	bool			d_bSend;
	bool			m_lockReceiverNameText;
	
	CEGUI::TLVertScrollbar*		d_pMailTreeScrol;

	CEGUI::StaticImage*			d_pMoney;
	CEGUI::Window*				d_pPayTax;

	typedef std::vector<TreeItemMail*> MailTreeItemList;
	MailTreeItemList	d_vFreeList;

	static MAIL_PARAM d_tempMail;

	int	d_sendTextTax;
	int d_sendItemTax;
	//static ItemType d_curItem;

	// friends list popup	
	int							d_maxFiendsNum;
	TLButton*					d_pSelFriend;			//弹出窗口的pop按钮
	
	CEGUI::TLTree*				d_PopMenu;				//弹出窗口
	int							d_oriPosMenuYPosition;
	StaticImage*				d_PopMenuBG;			//背景图
	CEGUI::TLVertScrollbar*		d_pFriendScrol;

	TreeItemList                d_PopupItemList;

	TLButton*					m_btnSelRecent;			//弹出最近联系人窗口的按钮
	int							m_maxMailCount;
	StaticText*					m_capability;
	TLButton*					m_btnNextPage;
	TLButton*					m_btnPrevPage;
	int							m_curPage;
	int							m_totalPage;
	StaticText*					m_pageShow;
	Editbox*					m_pRecverNameEditbox;
};

#endif

