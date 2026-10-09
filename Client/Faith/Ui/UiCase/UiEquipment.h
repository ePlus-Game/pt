 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/06/2006
//      File_base        : KUiEquipment
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 装备栏
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiEquipment_H
#define KUiEquipment_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"
#include <map>
#include <list>
#include <string>
#include "TLGameObject.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "TLEditbox.h"
#include "GameDataDef.h"
#include "UiCommonGrid.h"
#include "TLVertScrollbar.h"
#include "TLListbox.h"
#include "TLListView.h"
using namespace CEGUI;

#define CtrlNameLen 128


typedef vector< Window* > TitleBarList;

class ListboxTitleItem : public ListboxTextItem
{
public:
	ListboxTitleItem( ) : CEGUI::ListboxTextItem( "", 0, NULL, false, false ){}
};

const int MAX_TITLEINFO_COUNT = 100;

enum Message_Equipment
{
	ME_PERMANENT_STATE = 11,
	ME_NO_TITLE = 101,
	ME_TIP_FORMAT_TEXT,
	ME_STATE_NOT_GRANTED,
	ME_STATE_GRANTED,
	ME_TIP_NEXT_LEVEL,
};

/********************************************************************
/*						class: KUiTitlePage
/*						caolei+ 2008.9.22
*********************************************************************/
class KUiTitlePage
{
public:
	KUiTitlePage();

	void Init( Window* pParentWindow, WindowManager* pWindowManager, Window* pRootSheet );
	void SetVisible( bool visible );
	void RefreshBar();
	void RefreshLabel();

	String GetCurrentTitle();
	void SetOutterTitle( Window* pWnd );

private:
	bool cbShowMyTitle_SelectStateChanged( const EventArgs& e );
	bool cbShowRandomTitle_SelectStateChanged( const EventArgs& e );
	bool PopupTitle_MouseClick( const EventArgs& e );
	bool TitleListbox_MouseClick( const EventArgs& e );
	
	void setBar( const ListViewItem* pItem, const UiTitleInfo& pTitleInfoList );
	bool TitleList_UpdateBar( const EventArgs& e );

	String formatToolTip( const char* szToopTip, const char* szFormatStr = NULL );
	void refreshRadioButton();
	void setBarColor( const Window& bar, const colour& col );

private:
	Window*			m_pParent;
	Window*			m_pThisWnd;
	TLListView*		m_pList;
	Window*			m_pCurSelect;
	RadioButton*	m_pShowMyTitle;
	RadioButton*	m_pShowRandomTitle;
	WindowManager*	m_pWindowManager;
	TLListbox*		m_pListbox;

	//特例，刷新装备页的称号显示,如果今后多处需要动态刷新称号可以改为vector
	TLStaticText*	m_pOutterTitle;
	
	TitleBarList	m_barList;
	int				m_barHeight;
	int				m_exceedHeight;
	int				m_pageHeight;
	String			m_sCurSelected;

	UiTitleInfo		m_titleInfo[ MAX_TITLEINFO_COUNT ];
	UiTitleInfo		m_titleInfo_listbox[ MAX_TITLEINFO_COUNT ];
	int				m_infoCount;
	int				m_infoCount_listbox;
	String			m_strNoTitle;
	String			m_strPermanent;

	string			m_strState_notGranted;
	string			m_strState_Granted;
	string			m_strTipFormatTxt_nextLevel;
	string			m_strTipFormatTxt_normal;

	colour			m_clNotGrant;
	colour			m_clGrant;
	colour			m_clNextLevel;

	string			m_yearShow;
	string			m_monShow;
	string			m_dayShow;
	string			m_hourShow;
	string			m_minuteShow;
	string			m_dayString;
	string			m_hourString;
	string			m_minString;
	string			m_tipString;
	string			m_timeOverString;
	string			m_timeOverStringForTip;
	bool			m_isRadioButtonLocked;
};

/********************************************************************
/*						class: KUiAttributePage
/*						caolei+ 2008.9.22	
*********************************************************************/
class KUiAttributePage 
{
public:
	KUiAttributePage();

	void Init( Window* pParentWindow );
	void SetVisible( bool visible );
	void RefreshAttribute( const UiPlayerProperties* attributes, const char* szShizuName, const char* szZhuhouName );
	void RefreshGuoZhanAttribute();

private:
	bool sbScrollBar_handleScroll( const EventArgs& e );
	bool Panel_MouseWheel( const CEGUI::EventArgs& e );

	void initBaseInfoPanel();
	void initShizuPanel();
	void initZhuhouPanel();
	void initGuozhanPanel();

	void refreshBaseInfoPanel( const UiPlayerProperties* attributes );
	void refreshShizuPanel( const UiPlayerProperties* attributes );
	void refreshZhuhouPanel( const UiPlayerProperties* attributes );
	void refreshGuozhanPanel();
	int	 GetJunJie( int gongXun );

private:
	Window*			m_pParent;
	Window*			m_pThisWnd;
	Window*			m_pList;
	TLVertScrollbar* m_pScrollbar;

	//基本信息面板控件
	Window*			m_pBaseInfoPanel;
	TLStaticText*	m_pBaseInfo_Huoyuedu;
	TLStaticText*	m_pBaseInfo_HuoyueduCur;
	TLStaticText*	m_pBaseInfo_ShizuGongxun;
	TLStaticText*	m_pBaseInfo_ShizuGongxunCur;

	//氏族信息面板控件
	Window*			m_pShizuPanel;
	TLStaticText*	m_pShizu_Name;
	TLStaticText*	m_pShizu_Renqi;
	TLStaticText*	m_pShizu_MemberCount;
	TLStaticText*	m_pShizu_Level;
	TLStaticText*	m_pShizu_Flourish;
	TLStaticText*	m_pShizu_ZhuhouGongxun;
	TLStaticText*	m_pShizu_BuildingCount1;
	TLStaticText*	m_pShizu_BuildingCount2;
	TLStaticText*	m_pShizu_Money;
	TLStaticText*	m_pShizu_Resource1;
	TLStaticText*	m_pShizu_Resource2;
	TLStaticText*	m_pShizu_Resource3;

	//诸侯信息面板控件
	Window*			m_pZhuhouPanel;
	TLStaticText*	m_pZhuhou_Name;
	TLStaticText*	m_pZhuhou_Renqi;
	TLStaticText*	m_pZhuhou_MemberCount;
	TLStaticText*	m_pZhuhou_ShizuCount;
	TLStaticText*	m_pZhuhou_Flourish;
	TLStaticText*	m_pZhuhou_Train;
	TLStaticText*	m_pZhuhou_BuildingCount1;
	TLStaticText*	m_pZhuhou_BuildingCount2;
	TLStaticText*	m_pZhuhou_Money;
	TLStaticText*	m_pZhuhou_Resource1;
	TLStaticText*	m_pZhuhou_Resource2;
	TLStaticText*	m_pZhuhou_Resource3;

	//城市面板空间
	Window*			m_pGuozhanPanel;
	TLStaticText*	m_pGuozhanPanel_Gongxun;
	TLStaticText*	m_pGuozhanPanel_Junjie;
	TLStaticText*	m_pGuozhanPanel_Fenglu;
	vector< int >	m_gongxunToLevel;

	int	m_exceedHeight;
	int	m_listHeight;
};

class KUiEquipment : public KUiWndSingleton<KUiEquipment>
{
private:
	//角色信息面版……begin
	Point d_wndPos;
	TLStaticImage*		d_rolePanel;
	KUiTitlePage		d_titlePanel;
	KUiAttributePage	d_attributePanel;

	TLGameObject*	d_equip[itempart_num];
	TLGameObject*	d_gua[gua_pos_count];
	TLStaticImage*	d_guaEffect[gua_pos_count];
	KUiCommonGrid	d_equipGrid[itempart_num];
	KUiCommonGrid	d_guaGrid[gua_pos_count];

	const Image*	d_guaImage[GT_Num][GED_Num];	//挂位特效图片
	GuaType			d_guaState[gua_pos_count];		//挂位状态

	TLButton*		d_close;

	KUiPlayerBaseInfo		m_BaseInfo;
	KUiPlayerRuntimeInfo	m_RuntimeInfo;
	KUiPlayerAttribute		m_RuntimeAttribute;
	Window *pNameChild;				
	Window *pAttackChild;
	Window *pMagicChild;				
	Window *pLingChild;				
	Window *pLiChild;				
	Window *pTiChild;				
	Window *pShuChild;				
	Window *pMingzhongChild;
	Window *pShanbiChild;		
	Window *pHujiaChild;			
	Window *pBaguakangxingChild;
	Window *pXuanmingkangxingChild;
//	Window *pTili;	
//	Window *pJiyun;					
	Window *pShizhu;					
//	Window *pShengwang;
//	Window *pChenghao;
	Window* pBaseInfoText;
	TLStaticText* m_pBaseInfo_Name;
	TLStaticText* m_pBaseInfo_Level;
	TLStaticText* m_pBaseInfo_Metier;
	TLStaticText* m_pBaseInfo_Title;

	TLStaticText* m_pPhyCtritical;
	TLStaticText* m_pSpellCtritical;

	RadioButton* m_pRb_BaseInfoBtn;
	RadioButton* m_pRb_AttributeBtn;
	RadioButton* m_pRb_TitleInfoBtn;

	Window* pZhuhou;
	Window *pJiShiShoulder;					
	Window *pDaoShiShoulder;
	Window *pYiRenShoulder;	
	//角色信息面版……end

	
	void getChild();
	
	void initRolePanel();	
	void initRadioButtons();
	void selectDefaultPage();
	void showCriticalRate( int criticalNum, Window* pWindowShow );

	void getEquipInfo();
	void getGuaInfo();
	void playGuaEffect(int guaIndex);
	void drawItem(KObjAtContRegion* equipRegion);			//绘制一个装备
	void drawGua(KObjAtContRegion* guaRegion);				//绘制一个装备
	String getEquipIdleImageName();						//当未放置装备时空装备栏格子的图片资源名
	void setGuaImage(int guaId, TLGameObject::GameObject& obj);
	String getGuaIdleImageName();						//当未放置装备时空装备栏格子的图片资源名
	void setItemImage(int itemId, TLGameObject::GameObject& obj);
	void doCD(int equipIndex, int cooldownTime);

	void updateBaseInfo( const char* name, int level, const char* metier );

protected:
	bool onLBDown(const EventArgs& e);
	bool onClose(const EventArgs& e);
	bool onWindowOpen(const EventArgs& e);
	bool onWindowClose(const EventArgs& e);

	bool btnPagebtn_BaseInfo_SelectStateChanged( const EventArgs& e );
	bool btnPagebtn_Attribute_SelectStateChanged( const EventArgs& e );
	bool btnPagebtn_TitleInfo_SelectStateChanged( const EventArgs& e );
	void showRolePanel();

public:
	static unsigned int UpdateData( void );

	void Init( void );
	void onChange(UIMDLEvent& rEvent);
	int	getWndWidth();
	void onItemChanged(KObjAtContRegion* pObj, int add);
	static void Show();
	static void ShowTitle();

	void UpdataTitleInfo();
	void UpdataAttribute( const UiPlayerProperties* attributes );
    KUiEquipment( const String& id_name	);
    ~KUiEquipment(								);
};





#endif