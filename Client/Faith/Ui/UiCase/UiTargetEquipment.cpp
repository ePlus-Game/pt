#include "KWin32.h"
#include "UiTargetEquipment.h"
#include "../KMessageCentre.h"
#include "UiComMsgBox.h"

using namespace CEGUI;

extern iCoreShell*		g_pCoreShell;
const unsigned short	s_dwPingPerSecond	= 0;

template<> 
KUiTargetEquipment* KUiWndSingleton<KUiTargetEquipment>::ms_Singleton	= NULL;

KUiTargetEquipment::KUiTargetEquipment(const CEGUI::String& id_name):
KUiWndSingleton<KUiTargetEquipment>( id_name )
{
	pJiShiShoulder = 0;					
	pDaoShiShoulder = 0;
	pYiRenShoulder = 0;
	for(int i = 0; i < itempart_num; i++)
	{
		d_equip[i] = NULL;
	}

	m_selectedPlayerId = -1;

	m_pBaseInfo_Name	= NULL;
	m_pBaseInfo_Level	= NULL;
	m_pBaseInfo_Metier	= NULL;
	m_pBaseInfo_Title	= NULL;
	m_pRb_BaseInfoBtn	= NULL;
	m_pRb_AttributeBtn  = NULL;

	d_lianmen = NULL;
}

KUiTargetEquipment::~KUiTargetEquipment()
{
	if (NULL == ms_Singleton || NULL == ms_Singleton->m_pThisWnd )
		return;

	for(int j = 0; j < itempart_num; j++)
	{
		if ( d_equip[j] && d_equip[j]->getUserData() )
		{
			delete d_equip[j]->getUserData();
		}
	}
	
	for(int k = 0; k < gua_pos_count; k++)
	{
		if ( d_gua[k] && d_gua[k]->getUserData() )
		{
			delete d_gua[k]->getUserData();
		}
	}
}

void KUiTargetEquipment::getChild()
{	
	d_rolePanel					= (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/TargetEquipment/RoleInfo");
	d_rolePanel->setDummyWnd(true);

	//装备控件初始化
	d_equip[itempart_talisman]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Fabao");
	d_equip[itempart_amulet]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Yupei");
	d_equip[itempart_helm]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Toushi");
	d_equip[itempart_pendant]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Pifeng");
	d_equip[itempart_pendant]->setZLevel(Window::Top);
	d_equip[itempart_weapon]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Wuqi");
	d_equip[itempart_armor]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Yifu");
	d_equip[itempart_shoulder]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Hujian");
	d_equip[itempart_ring]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Jiezhi");
	d_equip[itempart_boots]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Xiezi");
	d_equip[itempart_cuff]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Shouzhuo");
	
	for(int j = 0; j < itempart_num; j++)
	{
		if(NULL == d_equip[j])
			continue;
		
		//设置通用属性
		d_equipGrid[j].setCtrl(d_equip[j]);
		d_equipGrid[j].addTip();
		
		KObjAtContRegion* region = new KObjAtContRegion();
		region->eContainer = UOC_EQUIPTMENT;
		region->Region.h = j;
		region->Region.Width = 2;			//2表示目标装备物品非挂位
		d_equip[j]->setUserData(region);
	}

	//挂位控件初始化
	d_gua[gua_pos_1]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei1");
	d_gua[gua_pos_2]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei2");
	d_gua[gua_pos_3]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei3");
	d_gua[gua_pos_4]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei4");
	d_gua[gua_pos_5]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei5");
	d_gua[gua_pos_6]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei6");
	d_gua[gua_pos_7]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei7");
	d_gua[gua_pos_8]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Guawei8");

	for(int k = 0; k < gua_pos_count; k++)
	{
		//设置通用属性
		d_guaGrid[k].setCtrl(d_gua[k]);
		d_guaGrid[k].addTip();

		KObjAtContRegion* region = new KObjAtContRegion();
		region->eContainer = UOC_EQUIPTMENT;
		region->Region.h = k;
		region->Region.Width = 3;//1表示目标装备挂位非物品
		d_gua[k]->setUserData(region);
	}

	d_state = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/State");
	d_name = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Name");
	d_shizhu = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Shizhu");
	d_zhuhou = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Zhuhou");
	d_lianmen = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Lianmen");		
	d_chenghao = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Chenhao");
	d_shengwang = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Shengwang");
	d_pkvalue = (TLStaticText*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/PKValue");

	d_touxiang = (TLStaticImage*)d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/Touxiang");
	//关闭按钮初始化
	d_close	= static_cast< TLButton* >( m_pThisWnd->getChild( "TaharezLook/TargetEquipment/Close" ) );
	d_close->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiTargetEquipment::onClose, this));

	pJiShiShoulder	= ms_Singleton->d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/JiShiShoulder");
	pDaoShiShoulder	= ms_Singleton->d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/DaoShiShoulder");
	pYiRenShoulder	= ms_Singleton->d_rolePanel->getChild("TaharezLook/TargetEquipment/RoleInfo/YiRenShoulder");

	initBaseInfoPanel();
	initRadioButtons();
	d_attributePanel.Init( m_pThisWnd );
}

void KUiTargetEquipment::Init( void )
{
	if (NULL == ms_Singleton || NULL == ms_Singleton->m_pThisWnd )
		return;
	
	getChild();
}

void KUiTargetEquipment::show()
{
	KUiWndSingleton<KUiTargetEquipment>::Show();

	getEquipInfo();
	getGuaInfo();
	getRoleInfo();

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		KTargetInfo tagTargetInfo;
		g_pCoreShell->GetGameData( GDI_PLAYER_TARGET_INFO, (unsigned int)&tagTargetInfo, NULL );

		ms_Singleton->pJiShiShoulder->hide();
		ms_Singleton->pDaoShiShoulder->hide();
		ms_Singleton->pYiRenShoulder->hide();

		switch ( tagTargetInfo.nMetier )  
		{
		case 0:
			ms_Singleton->pJiShiShoulder->show();
			break;
		case 1:
			ms_Singleton->pDaoShiShoulder->show();
			break;
		case 2:
			ms_Singleton->pYiRenShoulder->show();
			break;
		}
	}

}

int KUiTargetEquipment::getWndWidth()
{
	return m_pThisWnd->getWidth();
}

void KUiTargetEquipment::getEquipInfo()
{
 	KObjAtContRegion equipRegion[itempart_num];
	int nCount = g_pCoreShell->GetGameData(GDI_PARADE_EQUIPMENT, (unsigned int)equipRegion, 0);
	for (int i = 0; i < itempart_num; i++)
	{
		if(NULL == d_equip[i])
			continue;
		KObjAtContRegion* _equpRegion = (KObjAtContRegion*)d_equip[i]->getUserData();
		_equpRegion->Obj = equipRegion[i].Obj;
		drawItem(_equpRegion);
	}
}

void KUiTargetEquipment::getGuaInfo()
{
 	KObjAtContRegion guaRegion[gua_pos_count];
 	g_pCoreShell->GetGameData(GDI_TARGET_GUA_OVERVIEW, (unsigned int)guaRegion, 0);
	for(int i = 0; i < gua_pos_count; i++)
	{
		if(NULL == d_gua[i])
			continue;
		KObjAtContRegion* _guaRegion = (KObjAtContRegion*)d_gua[i]->getUserData();
		_guaRegion->Obj = guaRegion[i].Obj;
		drawGua(_guaRegion);
	}
}

void KUiTargetEquipment::drawItem(KObjAtContRegion* equip)
{
	TLGameObject::GameObject equipmentObj;
	if(CGOG_ITEM == equip->Obj.uGenre)
	{
		setItemImage(equip->Obj.uId, equipmentObj);
		equipmentObj.d_type = TLGameObject::item;
	}
	else
	{
		equipmentObj.d_gameobject = getEquipIdleImageName();
		equipmentObj.d_type = TLGameObject::idle;
	}
	d_equip[equip->Region.h]->setObject(equipmentObj);
}

void KUiTargetEquipment::drawGua(KObjAtContRegion* guaRegion)
{
	TLGameObject::GameObject guaObj;
	if(CGOG_ITEM == guaRegion->Obj.uGenre)
	{
		setGuaImage(guaRegion->Region.h, guaObj);
		guaObj.d_type = TLGameObject::item;
	}
	else
	{
		guaObj.d_gameobject = getGuaIdleImageName();
		guaObj.d_type = TLGameObject::idle;
	}
	d_gua[guaRegion->Region.h]->setObject(guaObj);
}

void KUiTargetEquipment::setItemImage(int itemId, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, itemId );
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
}

String KUiTargetEquipment::getEquipIdleImageName()
{
	return BACKGROUND_IMAGE;
}
					
void KUiTargetEquipment::setGuaImage(int guaIndex, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData( GDI_TARGET_GUA_INFO_INDEX, (unsigned int)&tagItemInfo, guaIndex );
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
}

String KUiTargetEquipment::getGuaIdleImageName()
{
	return BACKGROUND_IMAGE;
}

void KUiTargetEquipment::onItemChanged(KObjAtContRegion* pObj, int add)
{
	if ( ms_Singleton == NULL || ms_Singleton->m_pThisWnd == NULL )
	{
		return;
	}

	if(m_pThisWnd->isVisible() == false)
		return;
	
	TLGameObject::GameObject objInfo;
	if(NULL == d_equip[pObj->Region.h])
	{
		return;
	}

	if(add)//如果是增加物品或更新物品
	{
		if(!pObj->Obj.uGenre)
			return;
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_equip[pObj->Region.h]->getUserData();
		itemRegion->Obj = pObj->Obj;
		setItemImage(pObj->Obj.uId, objInfo);
		objInfo.d_type = TLGameObject::item;
		objInfo.d_count = pObj->Region.Height;
		d_equip[pObj->Region.h]->setObject(objInfo);
	}
	else//如果是减少物品
	{
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_equip[pObj->Region.h]->getUserData();
		itemRegion->Obj.uGenre = CGOG_NOTHING;
		objInfo.d_gameobject = getEquipIdleImageName();
		objInfo.d_type = TLGameObject::idle;
		d_equip[pObj->Region.h]->setObject(objInfo);
	}
	getGuaInfo();
}

void KUiTargetEquipment::getRoleInfo()
{
	TargetPlayerInfo playerInfo;
	g_pCoreShell->GetGameData(GDI_TARGET_ROLE_INFO, (unsigned int)&playerInfo, 0);

	char name[COMMON_CLIENT_MSG_LEN_64];
	d_chenghao->setText(AnsiToUtf8(playerInfo.chenghao));
	d_shizhu->setText(AnsiToUtf8(playerInfo.shizu));
	d_zhuhou->setText(AnsiToUtf8(playerInfo.zhuhou));
	if ( d_lianmen && playerInfo.lianmen )
		d_lianmen->setText(AnsiToUtf8(playerInfo.lianmen));
	d_pkvalue->setText(iToString(playerInfo.pkValue));
	KTargetInfo	targetInfo;
	g_pCoreShell->GetGameData(GDI_GET_PLAYER_INFO_BY_NPCID, (UINT)&targetInfo, playerInfo.npcId);

	String str;
	if ( d_name )
	{
		char szZhiye[16];
		if ( playerInfo.skillType < 0 )
		{
			switch( playerInfo.metier )
			{
			case 0:
				strcpy( szZhiye, ROLE_CAREER_JS);
				break;
			case 1:
				strcpy( szZhiye, ROLE_CAREER_DS);
				break;
			case 2:
				strcpy( szZhiye, ROLE_CAREER_YR);
			    break;
			default :
				strcpy( szZhiye, ROLE_CAREER_JS);
				break;
			}
			sprintf(name, MSG_LEVEL, playerInfo.name, targetInfo.nLevel, szZhiye);
		}
		else
		{
			switch(playerInfo.metier)
			{
			case 0:
				if ( playerInfo.skillType )
				{
					strcpy( szZhiye, ROLE_CAREER_JS_0);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_JS_1);
				}
				break;
			case 1:
				if ( playerInfo.skillType )
				{
					strcpy( szZhiye, ROLE_CAREER_DS_0);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_DS_1);
				}
				break;
			case 2:
				if ( playerInfo.skillType )
				{
					strcpy( szZhiye, ROLE_CAREER_YR_1);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_YR_0);
				}
			    break;
			default :
				if ( playerInfo.skillType )
				{
					strcpy( szZhiye, ROLE_CAREER_JS_1);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_JS_0);
				}
				break;
			}
			sprintf(name, MSG_LEVEL, playerInfo.name, targetInfo.nLevel, szZhiye);
		}			
		d_name->setText(AnsiToUtf8(name));

		updateBaseInfo( playerInfo.name, targetInfo.nLevel, szZhiye );
	}

	//头像
	if( targetInfo.szHeadImage != NULL
		&& targetInfo.szHeadImageSet != NULL
		&& strcmp(targetInfo.szHeadImage, "") != 0 
		&& strcmp(targetInfo.szHeadImageSet, "") != 0 )
	{
	 	d_touxiang->setImage( targetInfo.szHeadImageSet, targetInfo.szHeadImage );	
	}
	else
	{
		String strRoleFaceImageName;
		switch (targetInfo.nMetier)  
		{
		case 0:
			if (!targetInfo.nSex)
			{
				strRoleFaceImageName = "MidManFace";
			}
			else
			{
				strRoleFaceImageName = "MidWomanFace";
			}
			break;
		case 1:
			if (!targetInfo.nSex)
			{
				strRoleFaceImageName = "MidManFace0";
			}
			else
			{
				strRoleFaceImageName = "MidWomanFace0";
			}
			break;
		case 2:
			if (!targetInfo.nSex)
			{
				strRoleFaceImageName = "MidManFace1";
			}
			else
			{
				strRoleFaceImageName = "MidWomanFace1";
			}
			break;
		}
		if ( !strRoleFaceImageName.empty() )
		{
 			d_touxiang->setImage( "icon", strRoleFaceImageName );	
		}
	}
}

bool KUiTargetEquipment::onClose(const CEGUI::EventArgs& e)
{
	Hide();
	return true;
}

bool KUiTargetEquipment::btnPagebtn_BaseInfo_SelectStateChanged( const EventArgs& e )
{
	if ( ( NULL != m_pRb_BaseInfoBtn ) && ( NULL != d_rolePanel ) )
	{
		if ( m_pRb_BaseInfoBtn->isSelected() )
		{
			d_rolePanel->setVisible( true );
			d_attributePanel.SetVisible( false );
		}
	}
	return true;
}

bool KUiTargetEquipment::btnPagebtn_Attribute_SelectStateChanged( const EventArgs& e )
{
	if ( ( NULL != m_pRb_AttributeBtn ) && ( NULL != d_rolePanel ) )
	{
		if ( m_pRb_AttributeBtn->isSelected() )
		{
			d_rolePanel->setVisible( false );
			d_attributePanel.SetVisible( true );
			g_pCoreShell->OperationRequest( GOI_REFRESH_PLAYER_PROPERTIES, m_selectedPlayerId, NULL );
		}
	}
	return true;
}

void KUiTargetEquipment::UpdataAttribute( const UiPlayerProperties* attributes )
{
	if ( NULL != attributes )
	{
		TargetPlayerInfo playerInfo;
		g_pCoreShell->GetGameData( GDI_TARGET_ROLE_INFO, (unsigned int)&playerInfo, 0 );
		d_attributePanel.RefreshAttribute( attributes, playerInfo.shizu, playerInfo.zhuhou );
	}
}

void KUiTargetEquipment::SetSelectedPlayerID( int nPlayerID )
{
	m_selectedPlayerId = nPlayerID;
}

void KUiTargetEquipment::updateBaseInfo( const char* name, int level, const char* metier )
{
	if ( ( NULL != m_pBaseInfo_Name) && ( NULL != m_pBaseInfo_Level ) && ( NULL != m_pBaseInfo_Metier ) && ( NULL != m_pBaseInfo_Title ) )
	{
		if ( ( NULL != name ) && ( NULL != metier ) && ( NULL != level ) )
		{
			m_pBaseInfo_Name->setText( AnsiToUtf8( name ) );
			m_pBaseInfo_Level->setText( iToString( level ) );
			m_pBaseInfo_Metier->setText( AnsiToUtf8( metier ) );
		}

		UiNpcTitle npcTitle;
		ZeroMemory( &npcTitle, sizeof( npcTitle ) );
		npcTitle.NpcId = m_selectedPlayerId;
		g_pCoreShell->GetGameData( GDI_GET_NPC_TITLE, reinterpret_cast< int >( &npcTitle ), NULL );
		m_pBaseInfo_Title->setText( AnsiToUtf8( npcTitle.Title ) );
	}	
}

void KUiTargetEquipment::initRadioButtons()
{
	UI_RELEASE_TRY
	if ( NULL != m_pThisWnd )
	{
		m_pRb_BaseInfoBtn = static_cast< TLRadioButton* >( m_pThisWnd->getChild( "TaharezLook/TargetEquipment/Pagebtn_BaseInfo" ) );
		m_pRb_BaseInfoBtn->subscribeEvent(
			Window::EventSelectStateChanged, 
			Event::Subscriber( &KUiTargetEquipment::btnPagebtn_BaseInfo_SelectStateChanged, this ) );
		
		m_pRb_AttributeBtn = static_cast< TLRadioButton* >( m_pThisWnd->getChild( "TaharezLook/TargetEquipment/Pagebtn_Attribute" ) );
		m_pRb_AttributeBtn->subscribeEvent(
			Window::EventSelectStateChanged, 
			Event::Subscriber( &KUiTargetEquipment::btnPagebtn_Attribute_SelectStateChanged, this ) );
	}
	UI_RELEASE_CATCH
}

void KUiTargetEquipment::initBaseInfoPanel()
{
	UI_RELEASE_TRY
	if ( NULL != d_rolePanel )
	{
		Window* pChildBaseInfoPanel = ms_Singleton->d_rolePanel->getChild( "TaharezLook/TargetEquipment/RoleInfo/BaseInfoPanel" );
		pChildBaseInfoPanel->setZLevel( Window::Top );
		m_pBaseInfo_Name	= static_cast< TLStaticText* >( pChildBaseInfoPanel->getChild( pChildBaseInfoPanel->getName() + "/Name" ) );
		m_pBaseInfo_Level	= static_cast< TLStaticText* >( pChildBaseInfoPanel->getChild( pChildBaseInfoPanel->getName() + "/Level" ) );
		m_pBaseInfo_Metier	= static_cast< TLStaticText* >( pChildBaseInfoPanel->getChild( pChildBaseInfoPanel->getName() + "/Metier" ) );
		m_pBaseInfo_Title	= static_cast< TLStaticText* >( pChildBaseInfoPanel->getChild( pChildBaseInfoPanel->getName() + "/Title" ) );
	}
	UI_RELEASE_CATCH
}