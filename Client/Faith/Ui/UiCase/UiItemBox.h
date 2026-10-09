
//物品栏（道具栏）   xiehong 2006-09-12 

#ifndef KUiItemBox_H
#define KUiItemBox_H

#include "CEGUI.h"
#include "CoreShell.h"

#include "TLGameObject.h"
#include "TLStatic.h"
#include "TLButton.h"
#include "TLRadioButton.h"
#include "ItemCommonDef.h"
#include "UiCommonGrid.h"
#include "UiMDLInterface.h"

#define PAGECOUNT	  5
#define BAG_WIDTH	  5
#define BAG_HEIGHT	  6
#define EXTEND_WIDTH  5
#define EXTEND_HEIGHT 1

#define UI_ITEM_BOX_WINDOW_PATH_1024 "uisettings/layouts1024/ItemBox.ls"
#define UI_ITEM_BOX_WINDOW_PATH "uisettings/layouts/ItemBox.ls"
#define UI_ITEM_BOX_WINDOW_NAME "TaharezLook/ItemBox"
#define UI_ITEM_BOX_CTRL_NAME_LEN 256

using namespace CEGUI;
using namespace std;

class KUiItemBox
{
	friend class KUiItemCDTracker;
	
	//控件
	TLStaticImage*	_thisWindow;

	TLGameObject*	_item[BAG_HEIGHT][BAG_WIDTH];
	KUiCommonGrid	_itemGrid[BAG_HEIGHT][BAG_WIDTH];
	KObjAtContRegion _itemRegion[BAG_HEIGHT][BAG_WIDTH];
	TLStaticImage*	 _lockedImage[BAG_HEIGHT][BAG_WIDTH];

	TLGameObject*	_extendItem[EXTEND_HEIGHT][EXTEND_WIDTH];
	KUiCommonGrid	_extendItemGrid[EXTEND_HEIGHT][EXTEND_WIDTH];
	KObjAtContRegion _extendItemRegion[EXTEND_HEIGHT][EXTEND_WIDTH];
	
	TLRadioButton*	_pageTab[PAGECOUNT];

	TLStaticText*   _weight;
	TLStaticText*   _jin;
	TLStaticText*   _yin;
	TLStaticText*   _tong;

	TLStaticText *	m_InsteadSpecieGold;
	TLStaticText *	m_InsteadSpecieSilver;
	TLStaticText *	m_InsteadSpecieCopper;

	int				m_InsteadIndex;

	TLButton*		_jinshanBi;
	
	TLButton*		_close;

	int				_curPage;
	int				_size;

	bool			_loaded;
	
	bool			_lbDown;

	void			initMDL();
	void			loadUi();
	void			getData();
	void			pickItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj);
	void			dropItem(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo, TLGameObject* destObj);
	void			moveToStoreBox(KObjAtContRegion& sourPos);
	void			sellItem( KObjAtContRegion* itemRegion );
	void			useItem( KObjAtContRegion* itemRegion );
	void			repairItem( KObjAtContRegion* itemRegion );
	void			showPage();
	pair<int, int>	uiPosToActPos(const pair<int, int>& uiPos);
	pair<int, int>	actPosToUiPos(const pair<int, int>& actPos);

	bool			autoEquipExtendBag(KObjAtContRegion& itemRegion);
	
	static void		clearAGrid(TLGameObject* gridCtrl, bool disable);

	void			InsteadSpecieToUiSpecie(unsigned long insteadSpecie, unsigned long & gold, unsigned long & silver, unsigned long & copper);

protected:
	virtual bool	onBDown(const EventArgs& e);
	virtual bool	onLBUp(const EventArgs& e);
	virtual bool	onLBLeave(const EventArgs& e);
	
	virtual bool	onExtendPosBDown(const EventArgs& e);
	virtual bool	onExtendLBUp(const EventArgs& e);
	virtual bool	onExtendLBLeave(const EventArgs& e);

	virtual bool	onClose(const EventArgs& e);
	virtual bool	onPageChanged(const EventArgs& e);

public:
	
	static KObjAtContRegion _sellItemRegion;

	void			freshLockedItem();
	void			show();
	void			hide();
	bool			isVisible();
	void			updatePropertys();
	void			onItemChanged(KObjAtContRegion* pObj, int add);
	void			onItemExtendChanged(KObjAtContRegion* pObj, int add);
	void			onWeighChanged(int cur, int max);
	void			onBagSized(int newSize);
	void			onJinShanBiChanged(int newCount);
	void			UpdateInsteadSpecie();
	int				GetInsteadSpecieIndex();
	
	static void		processSellItem();

	//cd 相关
	unsigned int	beginGroupCD(KItemGroupCD_C* pGroupCD);
	unsigned int	endGroupCD(KItemGroupCD_C* pGroupCD);

    KUiItemBox();
    ~KUiItemBox();

	static KUiItemBox& getSingleton();
};


class KUiItemCDTracker : IUIMDLEvent
{
	IUIMDL*			_mdlMgr;

private:
	void	onCreate(UIMDLEvent& rEvent);
	void	onRelease(UIMDLEvent& rEvent);
	void	onChange(UIMDLEvent& rEvent);
	void	initMDL();
public:
	KUiItemCDTracker();
	~KUiItemCDTracker();

	static KUiItemCDTracker& getSingleton();
};

class KUiItemHelper
{
public:
	static void	sendItemLink( KObjAtContRegion &destPos );
	static void setItemImage(int itemId, TLGameObject::GameObject& obj);
	static void playItemPickSoundEffect(const ItemType& itemType);
	static void playItemDropSoundEffect(const ItemType& itemType);
	static void playItemUsingSoundEffect(const ItemType& itemType);
};

#endif