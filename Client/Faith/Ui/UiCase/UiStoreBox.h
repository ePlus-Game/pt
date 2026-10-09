 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/13/2006
//      File_base        : KUiStoreBox
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 储物箱
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiStoreBox_H
#define KUiStoreBox_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"
#include <list>
#include <string>

#include "TLGameObject.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "TLEditbox.h"
#include "TLRadioButton.h"
#include "UiCommonGrid.h"
#include "ItemCommonDef.h"

#define PAGE_COUNT	  5

#define BAG_WIDTH	  5
#define BAG_HEIGHT	  6

#define BAG_GRID_TOTAL_COUNT BAG_WIDTH * BAG_HEIGHT * PAGE_COUNT

#define EXTEND_WIDTH  5
#define EXTEND_HEIGHT 1

#define BAG_EXTEND_GRID_TOTAL_COUNT EXTEND_WIDTH * EXTEND_HEIGHT

#define UI_STORE_BOX_WND_NAME "TaharezLook/StoreBox"
#define UI_STORE_BOX_WND_NAME_MAX_LEN 256
#define UI_STORE_BOX_WND_PATH_1024 "uisettings/layouts1024/StoreBox.ls"
#define UI_STORE_BOX_WND_PATH "uisettings/layouts/StoreBox.ls"

using namespace CEGUI;

class KUiStoreBox
{
	friend class KUiItemBox;
	friend class KUiItemCDTracker;
	enum StringCode
	{
		Lock = 0,
		Unlock,
		PasswordDifferent,
		PasswordChangeSuccess,
		PasswordChangeFail,
		UnlockSuccess,
		UnlockFail,
		SetPasswordText,
		ChangePasswordText,
		ProcessLock,
		BagFull,
		StoreFull,
	};

	bool				_isLoad;

	//主窗口
	TLStaticImage*		_thisWindow;

	bool				_lbDown;

	TLGameObject*		_item[BAG_HEIGHT][BAG_WIDTH];
	KUiCommonGrid		_itemGrid[BAG_HEIGHT][BAG_WIDTH];
	KObjAtContRegion	_itemRegion[BAG_HEIGHT][BAG_WIDTH];

	TLGameObject*		_extendItem[EXTEND_HEIGHT][EXTEND_WIDTH];
	KUiCommonGrid		_extendItemGrid[EXTEND_HEIGHT][EXTEND_WIDTH];
	KObjAtContRegion	_extendItemRegion[EXTEND_HEIGHT][EXTEND_WIDTH];
	
	TLRadioButton*		_pageTab[PAGE_COUNT];
	
	int					_curPage;
	int					_size;

	TLButton*			_saveMoney;
	TLButton*			_getMoney;
	TLStaticText*		_jin;
	TLStaticText*		_yin;
	TLStaticText*		_tong;

	TLButton*			_close;
	
	std::pair<int, int>	uiPosToActPos(const std::pair<int, int>& uiPos);
	std::pair<int, int>	actPosToUiPos(const std::pair<int, int>& actPos);

	//金钱修改窗口……begin
	StaticImage*		_moneyBox;
	TLEditbox*			_mbox_jin;
	TLEditbox*			_mbox_yin;
	TLEditbox*			_mbox_tong;
	TLButton*			_mbox_ok;
	TLButton*			_mbox_cancel;

	//仓库密码相关
	TLButton*			m_btnModifyPassword;
	String				m_strCreatePassword;
	String				m_strModifyPassword;

	bool				_isSaveMoney;

	bool	onMboxShow(const EventArgs& e);
	bool	onMboxHide(const EventArgs& e);
	bool	onMboxOk(const EventArgs& e);
	bool	onMboxCancel(const EventArgs& e);
	bool	onMboxSaveMoney(const EventArgs& e);
	bool	onMboxGetMoney(const EventArgs& e);
	bool	onAddjustMoney(const EventArgs& e);
	bool	handleKeyDown(const EventArgs& e)
	{
		return true;
	}
	bool	btnModifyPassword_Clicked( const EventArgs& e );
	//金钱修改窗口……end
	bool	hasCreatedPassword();

	bool			onWndHide(const EventArgs& e);

	void			loadUi();
	void			showPage();
	void			getData();
	void			pickItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj);
	void			dropItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj);
	void			moveToItemBox(KObjAtContRegion& destPos);
	bool			autoEquipExtendBag(KObjAtContRegion& itemRegion);
	static void		clearAGrid(TLGameObject* gridCtrl, bool disable);
protected:
	virtual bool	onBDown(const EventArgs& e);
	virtual bool	onLBUp(const EventArgs& e);
	virtual bool	onLBLeave(const EventArgs& e);

	virtual bool	onExtendPosBDown(const EventArgs& e);
	virtual bool	onExtendLBUp(const EventArgs& e);
	virtual bool	onExtendLBLeave(const EventArgs& e);

	virtual bool	onClose(const EventArgs& e);
	virtual bool	onPageChanged(const EventArgs& e);
	bool			onClickLock(const EventArgs& e);
public:
	void			show();
	void			hide();
	bool			isVisible();

	void onBagSized(int newSize);
	void onItemExtendChanged(KObjAtContRegion* pObj, int add);
	void onItemChanged(KObjAtContRegion* pObj, int add);
	void onSetPasswordResult(bool ok);
	void onUnlock(bool ok);

	void RefreshBtnModifyPasswordStates();

	unsigned int	beginGroupCD(KItemGroupCD_C* pGroupCD);
	unsigned int	endGroupCD(KItemGroupCD_C* pGroupCD);

	void			ShowPasswordDlg();

    KUiStoreBox();
    ~KUiStoreBox();

	static KUiStoreBox& getSingleton();
};


#endif