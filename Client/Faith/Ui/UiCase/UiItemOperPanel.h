//xiehong 2008-3-19 把打造合成升级合成到一个面板——大家默哀

#ifndef UI_ITEM_OPER_PANEL
#define UI_ITEM_OPER_PANEL

#define UI_ITEM_OPER_PANEL_WINDOW_PATH_1024 "uisettings/layouts1024/ItemOperPanel.ls"
#define UI_ITEM_OPER_PANEL_WINDOW_PATH "uisettings/layouts/ItemOperPanel.ls"
#define UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT 9

#include "TLStatic.h"
#include "TLButton.h"
#include "TLRadioButton.h"
#include "TLGameObject.h"
#include "UiCommonGrid.h"
#include "GameDataDef.h"

class KUiItemOperPanel
{
	enum OPER_STATE
	{
		INVALID_STATE = -1,
		LIFT = 0,
		COMPOUND,
		LEVELUP,
	};

	TLStaticImage*		_thisWindow;
	TLGameObject*		_item[UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT];
	KUiCommonGrid		_itemGrid[UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT];
	KObjAtContRegion	_itemInfo[UI_ITEM_OPER_PANEL_MAX_ITEM_COUNT];

	TLRadioButton*		_lift;
	TLRadioButton*		_compound;
	TLRadioButton*		_levelUp;

	TLStaticImage*		_liftRequirePanel;
	TLStaticText*		_materialRequireText;
	TLStaticText*		_moneyRequireText;
	TLStaticText*		_rateText;

	TLStaticImage*		_compoundRequirePanel;
	TLStaticText*		_compoundMoneyRequireText;

	OPER_STATE			_curState;

	char				_ruleMsg[LAYOUT_TEXT_MAX_LEN];

	int					_ruleId;

	TLStaticImage*		_generateItemImage;
	ItemType			_generateItem;
	bool				_generateItemValidate;

	TLStaticImage*		_liftBackImage;
	TLStaticImage*		_compoundBackImage;
	
	TLButton*			_liftCommit;
	TLButton*			_compoundCommit;
	TLButton*			_autoCompoundCommit;

	TLStaticImage*		_liftBeginAnimation;
	TLStaticImage*		_compBeginAnimation;
	TLStaticImage*		_endImage;
	TLStaticImage*		_endFailImage;

	bool				m_enableAutoCommit;

	String				m_autoCommitBtnText;
private:
	void	load();
	void	clear();
	void	clearRequireInfo();
	void	clearGameObj(TLGameObject* goCtrl);
	void	clearSameObj( int itemIndex );
	
	void	freshOpInfo();
	void	freshLiftInfo();
	void	freshCompoundInfo();
	void	freshLevelUpInfo();
	void	freshItemCount();

	bool	canMake(SmithRule& rule);

	bool	clickGrid(const EventArgs& args);
	bool	selectPanel(const EventArgs& args);
	bool	onClose(const EventArgs& args);
	bool	onHoverRate(const EventArgs& args);
	bool	onLevaeRate(const EventArgs& args);
	bool	onCommit(const EventArgs& args);
	bool	onAutoCommit(const EventArgs& args);
	bool	onShow(const EventArgs& args);
	bool	onHide(const EventArgs& args);
	bool	onHoverIcon(const EventArgs& args);
	bool	onLevaeIcon(const EventArgs& args);
	
	int		itemCountOfAType(ItemType& type);

	void	freshLockedItem();

	void	showTip();

	void	doCommit();
public:
	KUiItemOperPanel();
	~KUiItemOperPanel();

	static KUiItemOperPanel& getSingleton();

	bool	isVisible();	
	void	show();
	void	hide();
	void	toggle();
	
	void	onEndSmith(int code);

	void	showLift();
	void	showCompound();
	void	showLevelUp();
	void	setPos(Point pos){	_thisWindow->setPosition(Absolute, pos);	}

	void	EnableAutoCommit();
	void	DisableAutoCommit();
	bool	isAutoCommit();
};

#endif
