 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 02/01/2007
//      File_base        : KUiTargetEquipment
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 目标装备栏
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiTargetEquipment_H
#define KUiTargetEquipment_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "TLGameObject.h"
#include "TLButton.h"
#include "TLStatic.h"
#include "TLEditbox.h"
#include "GameDataDef.h"
#include "UiCommonGrid.h"
#include "UiEquipment.h"
#include "TLRadioButton.h"

using namespace CEGUI;

class KUiTargetEquipment : public KUiWndSingleton<KUiTargetEquipment>
{
protected:
	
private:
	//角色信息面版……begin
	TLStaticImage*	d_rolePanel;
	KUiAttributePage d_attributePanel;

	TLGameObject*	d_equip[itempart_num];
	TLGameObject*	d_gua[gua_pos_count];
	KUiCommonGrid	d_equipGrid[itempart_num];
	KUiCommonGrid	d_guaGrid[gua_pos_count];
	TLButton*		d_close;

	TLStaticText*	d_state;
	TLStaticText*	d_name;
	TLStaticText*	d_shizhu;
	TLStaticText*	d_zhuhou;
	TLStaticText*   d_lianmen;
	TLStaticText*	d_chenghao;
	TLStaticText*	d_shengwang;
	TLStaticText*	d_pkvalue;

	TLStaticImage*	d_touxiang;
	
	Window *pJiShiShoulder;					
	Window *pDaoShiShoulder;
	Window *pYiRenShoulder;	

	TLRadioButton* m_pRb_BaseInfoBtn;
	TLRadioButton* m_pRb_AttributeBtn;

	int		m_selectedPlayerId;

	TLStaticText* m_pBaseInfo_Name;
	TLStaticText* m_pBaseInfo_Level;
	TLStaticText* m_pBaseInfo_Metier;
	TLStaticText* m_pBaseInfo_Title;

	//角色信息面版……end

	void getChild();
	
	void initBaseInfoPanel();
	void initRadioButtons();

	void getEquipInfo();
	void getGuaInfo();
	void getRoleInfo();
	void drawItem(KObjAtContRegion* equipRegion);			//绘制一个装备
	String getEquipIdleImageName();						//当未放置装备时空装备栏格子的图片资源名
	void setItemImage(int itemId, TLGameObject::GameObject& obj);

	void drawGua(KObjAtContRegion* guaRegion);				//绘制一个挂
	void setGuaImage(int guaId, TLGameObject::GameObject& obj);
	String getGuaIdleImageName();						//当未放置装备时空装备栏格子的图片资源名
protected:
	bool onClose(const CEGUI::EventArgs& e);

	bool btnPagebtn_BaseInfo_SelectStateChanged( const EventArgs& e );
	bool btnPagebtn_Attribute_SelectStateChanged( const EventArgs& e );

	void updateBaseInfo( const char* name, int level, const char* metier );

public:
	int	getWndWidth();
	void onItemChanged(KObjAtContRegion* pObj, int add);
	void Init( void );
	void show();
	void UpdataAttribute( const UiPlayerProperties* attributes );
	void SetSelectedPlayerID( int nPlayerID );

    KUiTargetEquipment( const CEGUI::String& id_name	);
    ~KUiTargetEquipment(								);
};


#endif