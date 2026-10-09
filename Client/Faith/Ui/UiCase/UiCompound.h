//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/26/2006 20:02
//      File_base        : UiCompound
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UICOMPOUND_H
#define UICOMPOUND_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLGameObject.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "TLEditbox.h"
#include "TLRadioButton.h"
#include "UiCommonGrid.h"

#define UI_COMPOUND_SHENGJI_TEXT	"Shengji"
#define UI_COMPOUND_JIACHI_TEXT		"Jiachi"
#define UI_COMPOUND_FUYAO_TEXT		"Fuyao"
#define UI_COMPOUND_CHAIYAO_TEXT	"Chaiyao"
#define UI_COMPOUND_INVALID_TEXT	""

class KUiCompound : public KUiWndSingleton<KUiCompound>
{
public:
    KUiCompound( const CEGUI::String& id_name );
    ~KUiCompound(								);
public:
	static void Show( void ) {};
	static void Show(COMPOUNDTYPE eCompoundType);
	static void Update( int nRusult );
	static unsigned int UpdateData( void ); 
	void	Init( void );
	
	void	setPos(Point pos){	m_pThisWnd->setPosition(Absolute, pos);	}
	void	commit();
	void	AutoAddNewItem( int index, ItemPos* iPos );
private:
	void	freshLockedItem();
	
	void	hideAllBtn( void );
	bool	onBDown(const CEGUI::EventArgs& e);
	bool	handleExit( const CEGUI::EventArgs& args	);
	bool	handleOK( const CEGUI::EventArgs& args		);
	
	void	clear();
	void	clearIdx(int idx);
	void	clearSameObj(int id);

	void	getChild();
	void	freshInfo();
	std::string	getCompoundTypeStr(COMPOUNDTYPE type);

	std::string	getResultTip(const KUiCompoundRuleInfo& ruleInfo);
	std::string	getRuleFillTip(int rstCode);
	std::string getCompoundRstTip(int rstCode);


    bool onHide( const EventArgs& args );
    bool onShow( const EventArgs& args );
	
	bool selectPanel(const EventArgs& args);

	void	refreshItemInfo();
	bool	checkIsBusy();
private:
	CEGUI::TLGameObject*d_item[MAX_LEVELUP_ITEMS_COUNT];
	KObjAtContRegion	d_itemInfo[MAX_LEVELUP_ITEMS_COUNT];
	KUiCommonGrid		d_itemGrid[MAX_LEVELUP_ITEMS_COUNT];
	COMPOUNDTYPE		d_eCompoundType;
	CEGUI::Window*		d_money;

	TLButton*			d_shengjiBtn;
	TLButton*			d_jiachiBtn;
	TLButton*			d_chaiyaoBtn;
	TLButton*			d_fuyaoBtn;

	TLStaticImage*		d_costWindow;
	TLStaticText*		d_jin;
	TLStaticText*		d_yin;
	TLStaticText*		d_tong;

	TLStaticImage*		d_yunhunWindow;
	TLStaticText*		d_curYunhun;
	TLStaticText*		d_requireYunhun;

	TLStaticText*		d_result;
	TLStaticText*		d_ruleFitCondition;

	TLStaticImage*		d_beginImage;
	TLStaticImage*		d_endImage;
	TLStaticImage*		d_endFailImage;

	TLRadioButton*		_lift;
	TLRadioButton*		_compound;
	TLRadioButton*		_levelUp;

	bool				d_isBusy;
	DWORD				d_lastCommitTime;	
};

#endif