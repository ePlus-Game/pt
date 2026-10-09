#include "KWin32.h"
#include "UiEquipment.h"
#include "UiDragItem.h"
#include "UiTradeConfirmBox.h"
#include "UiPlayerState.h"
#include "UiShop.h"
#include "../KMessageCentre.h"
#include "../UiSheetMgr.h"
#include "UiComMsgBox.h"
#include "UIChatWindow.h"
#include "UiChatCentre.h"
#include <sstream>
#include <string>
#include <iomanip>
#include "KTabFile.h"

using namespace CEGUI;
using namespace std;

extern iCoreShell*		g_pCoreShell;
const unsigned short	s_dwPingPerSecond	= 0;

const int GroupID_cbTitle					= 102;
const int GroupID_rbPageBtn					= 101;
const String INVALID_VALUE_SHOW				= "-";
const int GongXunParam						= 6;
const int FengLuparam						= 7;
const string GongXunToLevel					= "/settings/GongxunToLevel.txt";


//const int MAX_TITLECOUNT = 50;
const string BarLayoutsFileName				 = "uisettings/layouts/TitleInfoBar.ls";
ListboxTitleItem TitleItemList[ MAX_TITLEINFO_COUNT ];

template<> 
KUiEquipment* KUiWndSingleton<KUiEquipment>::ms_Singleton	= NULL;

KUiEquipment::KUiEquipment(const CEGUI::String& id_name):
KUiWndSingleton<KUiEquipment>( id_name )
{
	IUIMDLDataset* pDataset = NULL;
	int nRet = m_pUiMDLManager->queryDataSet( itemgroupcd_dataset, &pDataset );
	if ( success_errorcode != nRet && dataset_areadycreated_errorcode != nRet )
	{
		m_pUiMDLManager->createDataSet( itemgroupcd_dataset );
		m_pUiMDLManager->queryDataSet( itemgroupcd_dataset, &pDataset );
	}
	pDataset->setEventHandle( ms_Singleton );

	memset(d_guaImage, 0, sizeof(d_guaImage));

	pNameChild				= 0;
	pAttackChild			= 0;
	pMagicChild				= 0;
	pLingChild				= 0;
	pLiChild				= 0;
	pTiChild				= 0; 
	pShuChild				= 0; 
	pMingzhongChild			= 0; 
	pShanbiChild			= 0; 
	pHujiaChild				= 0; 
	pBaguakangxingChild		= 0; 
	pXuanmingkangxingChild	= 0; 
//	pTili					= 0; 
//	pJiyun					= 0; 
	pShizhu					= 0; 
	pZhuhou                 = 0;
//	pShengwang				= 0; 
//	pChenghao				= 0; 
	pJiShiShoulder			= 0;					
	pDaoShiShoulder			= 0;
	pYiRenShoulder			= 0;

	m_pBaseInfo_Name		= NULL;
	m_pBaseInfo_Level		= NULL;
	m_pBaseInfo_Metier		= NULL;
	m_pBaseInfo_Title		= NULL;
	m_pPhyCtritical			= NULL;
	m_pSpellCtritical		= NULL;
	m_pRb_BaseInfoBtn		= NULL;
	m_pRb_AttributeBtn		= NULL;
	m_pRb_TitleInfoBtn		= NULL;

	for(int i = 0; i < gua_pos_count; i++)
	{
		d_guaState[i] = GT_Invalid;
	}
}

KUiEquipment::~KUiEquipment()
{
	if ( ms_Singleton->m_pThisWnd )
	{
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
		}//*/
	}
}

void KUiEquipment::getChild()
{
	ZeroMemory( &ms_Singleton->m_RuntimeInfo, sizeof( KUiPlayerBaseInfo ) );
	ZeroMemory( &ms_Singleton->m_BaseInfo, sizeof( KUiPlayerRuntimeInfo ) );
	ZeroMemory( &ms_Singleton->m_RuntimeAttribute, sizeof( KUiPlayerAttribute ) );
	
	for(int i = 0; i < itempart_num; i++)
	{
		d_equip[i] = NULL;
	}

	m_pThisWnd->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiEquipment::onWindowOpen, this));
	m_pThisWnd->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiEquipment::onWindowClose, this));

	initRolePanel();
	
	
	pNameChild				=     m_pThisWnd->getChild("TaharezLook/Equipment/PlayerName");
	if(pNameChild)
		pNameChild->setHorizontalAlignment(HA_CENTRE);
	pNameChild->hide();
	d_equip[itempart_talisman]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Fabao");
	d_equip[itempart_amulet]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Yupei");
	d_equip[itempart_helm]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Toushi");
	d_equip[itempart_pendant]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Pifeng");
	d_equip[itempart_weapon]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Wuqi");
	d_equip[itempart_armor]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Yifu");
	d_equip[itempart_shoulder]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Hujian");
	d_equip[itempart_ring]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Jiezhi");
	d_equip[itempart_boots]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Xiezi");
	d_equip[itempart_cuff]		= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Shouzhuo");

	d_gua[gua_pos_1]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei1");
	d_gua[gua_pos_2]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei2");
	d_gua[gua_pos_3]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei3");
	d_gua[gua_pos_4]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei4");
	d_gua[gua_pos_5]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei5");
	d_gua[gua_pos_6]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei6");
	d_gua[gua_pos_7]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei7");
	d_gua[gua_pos_8]	= (TLGameObject*)d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guawei8");
	pAttackChild			= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Gongji");
	pMagicChild				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Zhoufa");
	pLingChild				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Ling");
	pLiChild				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Li");
	pTiChild				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Ti");
	pShuChild				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Shu");
	pMingzhongChild			= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Mingzhong");
	pShanbiChild			= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Shanbi");
	pHujiaChild				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Hujia");
	pBaguakangxingChild		= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Baguakangxing");
	pXuanmingkangxingChild	= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Xuanmingkangxing");
//	pTili					= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Tili");
//	pJiyun					= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Jiyun");
	pShizhu					= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Shizhu");
	pZhuhou                 = ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Zhuhou");
//	pGuojia                 = ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Guojia");
//	pShengwang				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Shengwang");
//	pChenghao				= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/Chenghao");

	pJiShiShoulder			= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/JiShiShoulder");
	pDaoShiShoulder			= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/DaoShiShoulder");
	pYiRenShoulder			= ms_Singleton->d_rolePanel->getChild("TaharezLook/Equipment/RoleInfo/YiRenShoulder");


	d_equip[itempart_pendant]->setZLevel(Window::Top);

	d_close	= (TLButton*)m_pThisWnd->getChild("TaharezLook/Equipment/RoleInfo/Close");

	pBaseInfoText = m_pThisWnd->getChild("TaharezLook/Equipment/baseInfo");
// 	if(pBaseInfoText)
// 		pBaseInfoText->setHorizontalAlignment(HA_CENTRE);
	//以防被兄弟控件遮挡
	d_close->setZLevel(Window::Top);
	m_pThisWnd->getChild("TaharezLook/Equipment_HelpBtn")->setZLevel(Window::Top);

	for(int j = 0; j < itempart_num; j++)
	{
		if(NULL == d_equip[j])
			continue;
		
		//设置通用属性
		d_equipGrid[j].setCtrl(d_equip[j]);
		d_equipGrid[j].addTip();
		d_equipGrid[j].showCompare(false);
		
		KObjAtContRegion* region = new KObjAtContRegion();
		region->eContainer = UOC_EQUIPTMENT;
		region->Region.h = j;
		region->Region.Width = 0;//0表示物品非挂位
		d_equip[j]->setUserData(region);
		
		d_equip[j]->subscribeEvent(TLGameObject::EventMouseButtonDown, Event::Subscriber(&KUiEquipment::onLBDown, this));
	}

	for(int k = 0; k < gua_pos_count; k++)
	{
		//设置通用属性
		d_guaGrid[k].setCtrl(d_gua[k]);
		d_guaGrid[k].addTip();
		d_equipGrid[j].showCompare(false);

		KObjAtContRegion* region = new KObjAtContRegion();
		region->eContainer = UOC_EQUIPTMENT;
		region->Region.h = k;
		region->Region.Width = 1;//1表示挂位非物品
		d_gua[k]->setUserData(region);

		d_guaEffect[k] = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, String("guaEffect") + iToString(k));
		d_guaEffect[k]->disable();
		d_guaEffect[k]->setHeight(Absolute, 1000);
		d_guaEffect[k]->setWidth(Absolute, 1000);
		KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_guaEffect[k]);
		d_guaEffect[k]->setZLevel(Window::Top);
		d_guaEffect[k]->setRenderMode(true);
	}

	d_close->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiEquipment::onClose, this));

	d_titlePanel.Init( m_pThisWnd, m_pWindowManager, m_pRootSheet );
	d_titlePanel.SetOutterTitle( m_pBaseInfo_Title );
	d_attributePanel.Init( m_pThisWnd );
	
	initRadioButtons();
}

void KUiEquipment::Init( void )
{
	if (NULL == ms_Singleton || NULL == ms_Singleton->m_pThisWnd )
		return;
	
	d_wndPos = m_pThisWnd->getPosition(Absolute);
	
	getChild();
}

void KUiEquipment::Show()
{
	KUiWndSingleton<KUiEquipment>::Show();
	

	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->UpdateData();
		
		KUiPlayerAttribute PlayerInfo;
		g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&PlayerInfo, NULL );

		ms_Singleton->pJiShiShoulder->hide();
		ms_Singleton->pDaoShiShoulder->hide();
		ms_Singleton->pYiRenShoulder->hide();

		switch ( PlayerInfo.nSeries )  
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

		ms_Singleton->selectDefaultPage();
	}

	if(KUiShop::GetSingleton().IsVisible())
	{
		int offset = KUiShop::GetSingleton().getWndWidth() + 40;
		ms_Singleton->m_pThisWnd->setPosition(Absolute, ms_Singleton->d_wndPos + Point(offset, 0));
	}
	else
	{
		ms_Singleton->m_pThisWnd->setPosition(Absolute, ms_Singleton->d_wndPos);
	}
 	
	ms_Singleton->getEquipInfo();
	ms_Singleton->getGuaInfo();
}

int KUiEquipment::getWndWidth()
{
	return m_pThisWnd->getWidth();
}

void KUiEquipment::getEquipInfo()
{
 	KObjAtContRegion equipRegion[itempart_num];
	int nCount = g_pCoreShell->GetGameData(GDI_EQUIPMENT, (unsigned int)equipRegion, 0);
	for (int i = 0; i < itempart_num; i++)
	{
		if(NULL == d_equip[i])
			continue;
		KObjAtContRegion* _equpRegion = (KObjAtContRegion*)d_equip[i]->getUserData();
		_equpRegion->Obj = equipRegion[i].Obj;
		_equpRegion->Region.Height = equipRegion[i].Region.Height;
		drawItem(_equpRegion);
	}
}

void KUiEquipment::getGuaInfo()
{
 	KObjAtContRegion guaRegion[gua_pos_count];
 	g_pCoreShell->GetGameData(GDI_SELF_GUA_OVERVIEW, (unsigned int)guaRegion, 0);
	for(int i = 0; i < gua_pos_count; i++)
	{
		if(NULL == d_gua[i])
			continue;
		KObjAtContRegion* _guaRegion = (KObjAtContRegion*)d_gua[i]->getUserData();
		_guaRegion->Obj = guaRegion[i].Obj;
		drawGua(_guaRegion);
		
		d_guaState[i] = (GuaType)guaRegion[i].Obj.uId;
	}
}

void KUiEquipment::playGuaEffect(int guaIndex)
{
	static GuaType lastGuaState[gua_pos_count];
	static bool firstTime = true;

	if(firstTime)
	{
		for(int guaIndex = 0; guaIndex < gua_pos_count; ++guaIndex)
		{
			lastGuaState[guaIndex] = GT_Invalid;
		}
		firstTime = false;
	}

	GuaEffectDir dir;
	for(int i = 0; i < gua_pos_count; i++)
	{
		if(lastGuaState[i] != d_guaState[i])
		{
			lastGuaState[i] = d_guaState[i];
		}
		else
		{
			continue;
		}

		if(i < 3)
		{
			dir = GED_Heng;
		}
		else if(i == 3)
		{
			dir = GED_Youxie;
		}
		else if(i < 7)
		{
			dir = GED_Shu;
		}
		else
		{
			dir = GED_Zuoxie;
		}

		GuaType gt = d_guaState[i];
		if(GT_Invalid == d_guaState[i])
		{
			d_guaEffect[i]->hide();
		}
		else
		{
			const pair<int, int>& pos = KUiCfgLoader::getSingleton().getEquipmentCfg().pos[i];
			Point panelPos = m_pThisWnd->getUnclippedPixelRect().getPosition();
			d_guaEffect[i]->setXPosition(Absolute, panelPos.d_x + pos.first);
			d_guaEffect[i]->setYPosition(Absolute, panelPos.d_y + pos.second);
			d_guaEffect[i]->show();
			const Image* fitImage = d_guaImage[gt][dir];
			if ( fitImage )
			{
				d_guaEffect[i]->setImage(fitImage);
				d_guaEffect[i]->setHeight(Absolute, fitImage->getHeight());
				d_guaEffect[i]->setWidth(Absolute, fitImage->getWidth());
				d_guaEffect[i]->play();
			}

		}
	}
}

void KUiEquipment::drawItem(KObjAtContRegion* equip)
{
	TLGameObject::GameObject equipmentObj;
	if(CGOG_ITEM == equip->Obj.uGenre)
	{
		setItemImage(equip->Obj.uId, equipmentObj);
		equipmentObj.d_type = TLGameObject::item;
		equipmentObj.d_count = equip->Region.Height;
	}
	else
	{
		equipmentObj.d_gameobject = getEquipIdleImageName();
		equipmentObj.d_type = TLGameObject::idle;
	}
	d_equip[equip->Region.h]->setObject(equipmentObj);
}

void KUiEquipment::drawGua(KObjAtContRegion* guaRegion)
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

void KUiEquipment::setItemImage(int itemId, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, itemId );
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
	obj.d_EdgeframeIdx = tagItemInfo.colour;
}

String KUiEquipment::getEquipIdleImageName()
{
	return BACKGROUND_IMAGE;
}
					
void KUiEquipment::setGuaImage(int guaIndex, TLGameObject::GameObject& obj)
{
	KItemInfo tagItemInfo;
	g_pCoreShell->GetGameData( GDI_GUA_INFO_INDEX, (unsigned int)&tagItemInfo, guaIndex );
	obj.d_gameobjectSet = tagItemInfo.szImageSet;
	obj.d_gameobject = tagItemInfo.szImage;
}

String KUiEquipment::getGuaIdleImageName()
{
	return BACKGROUND_IMAGE;
}

unsigned int KUiEquipment::UpdateData( void ) 
{
	if ( NULL == ms_Singleton )
	{
		return 0;
	}

	g_pCoreShell->GetGameData( GDI_PLAYER_RT_INFO, (unsigned int)&ms_Singleton->m_RuntimeInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&ms_Singleton->m_BaseInfo, NULL );
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&ms_Singleton->m_RuntimeAttribute, NULL );

	ms_Singleton->showCriticalRate( 
		ms_Singleton->m_RuntimeAttribute.nPhysExplode, 
		ms_Singleton->m_pPhyCtritical );
	
	ms_Singleton->showCriticalRate( 
		ms_Singleton->m_RuntimeAttribute.nMagicExplode, 
		ms_Singleton->m_pSpellCtritical );

	char szBuf[COMMON_CLIENT_MSG_LEN_32];
	char szBaseInfo[COMMON_CLIENT_MSG_LEN_32]={0};
	String str;
	char szZhiye[16];
	if ( ms_Singleton->pNameChild )
	{
		if ( ms_Singleton->m_BaseInfo.nSkillType < 0 )
		{
			switch(ms_Singleton->m_RuntimeAttribute.nSeries)
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
			sprintf( szBuf,ms_Singleton->m_BaseInfo.Name);//MSG_LEVEL, ms_Singleton->m_BaseInfo.Name, ms_Singleton->m_RuntimeAttribute.nLevel, szZhiye );
		
			sprintf(szBaseInfo,MSG_LEVEL,ms_Singleton->m_BaseInfo.Name,ms_Singleton->m_RuntimeAttribute.nLevel,szZhiye);
		}
		else
		{

			switch(ms_Singleton->m_RuntimeAttribute.nSeries)
			{
			case 0:
				if ( ms_Singleton->m_BaseInfo.nSkillType )
				{
					strcpy( szZhiye, ROLE_CAREER_JS_0);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_JS_1);
				}
				break;
			case 1:
				if ( ms_Singleton->m_BaseInfo.nSkillType )
				{
					strcpy( szZhiye, ROLE_CAREER_DS_0);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_DS_1);
				}
				break;
			case 2:
				if ( ms_Singleton->m_BaseInfo.nSkillType )
				{
					strcpy( szZhiye, ROLE_CAREER_YR_1);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_YR_0);
				}
			    break;
			default :
				if ( ms_Singleton->m_BaseInfo.nSkillType )
				{
					strcpy( szZhiye, ROLE_CAREER_JS_1);
				}
				else
				{
					strcpy( szZhiye, ROLE_CAREER_JS_0);
				}
				break;
			}
			sprintf( szBuf,ms_Singleton->m_BaseInfo.Name);// ms_Singleton->m_BaseInfo.Name, ms_Singleton->m_RuntimeAttribute.nLevel, szZhiye );
			sprintf(szBaseInfo,MSG_LEVEL,ms_Singleton->m_BaseInfo.Name,ms_Singleton->m_RuntimeAttribute.nLevel,szZhiye);
		}
		
		str = AnsiToUtf8( szBuf );
		ms_Singleton->pNameChild->setText( str );
		ms_Singleton->pNameChild->hide();
		str = AnsiToUtf8(szBaseInfo);
		if(ms_Singleton->pBaseInfoText)
			ms_Singleton->pBaseInfoText->setText(str);

		ms_Singleton->updateBaseInfo( 
			ms_Singleton->m_BaseInfo.Name, 
			ms_Singleton->m_RuntimeAttribute.nLevel, 
			szZhiye );
	}

	if ( ms_Singleton->pAttackChild )
	{
		sprintf( szBuf, "%d-%d",ms_Singleton->m_RuntimeAttribute.nPhysicsAttackLow >> 10, ms_Singleton->m_RuntimeAttribute.nPhysicsAttackHight >> 10 );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pAttackChild->setText( str );
	}

	if ( ms_Singleton->pMagicChild )
	{
		sprintf( szBuf, "%d-%d",ms_Singleton->m_RuntimeAttribute.nMagicAttackLow >> 10, ms_Singleton->m_RuntimeAttribute.nMagicAttackHight >> 10 );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pMagicChild->setText( str );
	}

	if ( ms_Singleton->pLingChild )
	{
		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nNimbus );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pLingChild->setText( str );
	}

	if ( ms_Singleton->pLiChild )
	{
		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nStrength >> 10 );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pLiChild->setText( str );
	}

	if ( ms_Singleton->pMagicChild )
	{
		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nBody );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pTiChild->setText( str );
	}

	if ( ms_Singleton->pShuChild )
	{
		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nArt >> 10 );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pShuChild->setText( str );
	}

	if ( ms_Singleton->pMingzhongChild )
	{
		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nAttackRate );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pMingzhongChild->setText( str );
	}

	if ( ms_Singleton->pShanbiChild )
	{
		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nJinkRate );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pShanbiChild->setText( str );
	}

	if ( ms_Singleton->pShanbiChild )
	{
		if ( ms_Singleton->m_RuntimeAttribute.nPhysicsDefendHight < -180 )
		{
			ms_Singleton->m_RuntimeAttribute.nPhysicsDefendHight = -180;
		}

		sprintf( szBuf, "%d",ms_Singleton->m_RuntimeAttribute.nPhysicsDefendHight );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pHujiaChild->setText( str );
	}

	if ( ms_Singleton->pBaguakangxingChild )
	{	
		if ( ms_Singleton->m_RuntimeAttribute.nEightDiaDefendHight < -180 )
		{
			ms_Singleton->m_RuntimeAttribute.nEightDiaDefendHight = -180;
		}

		sprintf( szBuf, "%d", ms_Singleton->m_RuntimeAttribute.nEightDiaDefendHight );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pBaguakangxingChild->setText( str );
	}

	if ( ms_Singleton->pXuanmingkangxingChild )
	{
		if ( ms_Singleton->m_RuntimeAttribute.nDarkDefendHight < -180 )
		{
			ms_Singleton->m_RuntimeAttribute.nDarkDefendHight = -180;
		}

		sprintf( szBuf, "%d", ms_Singleton->m_RuntimeAttribute.nDarkDefendHight );
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pXuanmingkangxingChild->setText( str );
	}

//	if ( ms_Singleton->pTili )
//	{
//		sprintf( szBuf, "------" );
//		str = AnsiToUtf8( szBuf ); 
//		ms_Singleton->pTili->setText( str );
//	}

//	if ( ms_Singleton->pJiyun )
//	{
//		sprintf( szBuf, "------" );
//		str = AnsiToUtf8( szBuf ); 
//		ms_Singleton->pJiyun->setText( str );
//	}
	SOCIETY_INFO Info;
	ZeroMemory( &Info, sizeof(SOCIETY_INFO) );
	g_pCoreShell->GetGameData( GDI_GET_SOCIETY, (unsigned int)&Info, NULL );
	if ( ms_Singleton->pShizhu )
	{
		if ( Info.szShizu == NULL||Info.szShizu[0]==0 )
		{
			sprintf( szBuf, "------" );
		}
		else
		{
			sprintf( szBuf, Info.szShizu );
		}
		
		str = AnsiToUtf8( szBuf ); 
		ms_Singleton->pShizhu->setText( str );
	}
	if(ms_Singleton->pZhuhou)
	{
		szBuf[0] = 0;
		if(Info.szZhuhou == 0||Info.szZhuhou[0]==0)
		{
			sprintf(szBuf,"------");
		}
		else
		{
			sprintf(szBuf,Info.szZhuhou);
		}
		str = AnsiToUtf8(szBuf);
		ms_Singleton->pZhuhou->setText(str);
	}

//	if ( ms_Singleton->pShengwang )
//	{
//		sprintf( szBuf, "------" );
//		str = AnsiToUtf8( szBuf ); 
//		ms_Singleton->pShengwang->setText( str );
//	}

//	if ( ms_Singleton->pChenghao )
//	{
//		sprintf( szBuf, "------" );
//		str = AnsiToUtf8( szBuf ); 
///		ms_Singleton->pChenghao->setText( str );
//	}
//*/
	return 0;
}

bool KUiEquipment::onWindowOpen(const EventArgs& e)
{
	for(int i = 0; i < GT_Num; ++i)
	{
		for(int j = 0; j < GED_Num; ++j)
		{
			const char* path = KUiCfgLoader::getSingleton().getEquipmentCfg().guaEffect[i][j];
			d_guaImage[i][j] = getImage(path);
		}
	}
	return true;
}

bool KUiEquipment::onWindowClose(const EventArgs& e)
{
	for(int i = 0; i < GT_Num; ++i)
	{
		for(int j = 0; j < GED_Num; ++j)
		{
			if(d_guaImage[i][j])
			{
				const char* path = d_guaImage[i][j]->getImageset()->getName().c_str();
				deleteImageSet(path);
				d_guaImage[i][j] = NULL;
			}
		}
	}

	for(int guaIndex = 0; guaIndex < gua_pos_count; ++guaIndex)
	{
		d_guaEffect[guaIndex]->setImage(NULL);
	}
	return true;
}

bool KUiEquipment::onLBDown(const CEGUI::EventArgs& e)
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if(arg->button != LeftButton)
		return false;

	TLGameObject* destObj,* sourObj;
	TLGameObject::GameObject destObjInfo, sourObjInfo;
	KObjAtContRegion* destRegion, * sourRegion;
	destObj = (TLGameObject*)arg->window;
	destObj->getObject(destObjInfo);
	destRegion = (KObjAtContRegion*)destObj->getUserData();
	sourObj = KUiDragItem::GetSingleton().getObj();
	sourObj->getObject(sourObjInfo);
	sourRegion = (KObjAtContRegion*)sourObj->getUserData();

	switch(KUiPlayerState::getSingleton().getState())
	{
	case KUiPlayerState::TRADE_NPC_NORMAL_REPAIR:
	case KUiPlayerState::TRADE_NPC_SPECIAL_REPAIR:
		{
			KUiTradeConfirmBox::GetSingleton().show(destRegion);
		}
		break;
	default:
		{
			KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
			if((arg->sysKeys & Control) && KUiChatInputWnd::IsVisible() || (pRoom &&  pRoom->IsVisible()  && (arg->sysKeys & Control) ) )
			{
				KItemInfo tagItemInfo;
				ZeroMemory( &tagItemInfo, sizeof(KItemInfo));
				g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, (int)destRegion->Obj.uId );
				
				int itemId = g_pCoreShell->GetGameData( GDI_GET_ITEM_ID_BY_INDEX, (unsigned int)destRegion->Obj.uId, NULL);
				
				int hashId = GenerateItemHashId(
					tagItemInfo.itemIdx.nGenre, 
					tagItemInfo.itemIdx.nDetail, 
					tagItemInfo.itemIdx.nParticular);
				
				LOElemInfo itemElem;
				wchar_t* itemContent = NULL;
				ansiToUnicode(tagItemInfo.szName, itemContent);
				wchar_t itemDescription[COMMON_CLIENT_MSG_LEN_64];
				itemDescription[0] = 0;
				wcscat(itemDescription, L"[");
				wcscat(itemDescription, itemContent);
				wcscat(itemDescription, L"]");
				
				itemElem.elemType = LO_TEXT;
				itemElem.isShowDes = true;
				itemElem.gameObj._objType = LO_GO_ITEM;
				itemElem.gameObj._objId[0] = hashId;
				itemElem.gameObj._objId[1] = tagItemInfo.itemIdx.nLevel;
				itemElem.gameObj._objId[2] = itemId;
				itemElem.content = itemContent;
				itemElem.description = itemDescription;
				
				if ( pRoom )
				{
					pRoom->write(itemElem);
				}
				else
				{
					KUiChatInputWnd::GetSingleton().write(itemElem);
					KUiChatInputWnd::GetSingleton().show();
				}
				
				
				delete[] itemContent;
				itemContent = NULL;
				
				
			}
			else if(sourObjInfo.d_type == TLGameObject::idle)
			{
				if(destObjInfo.d_type == TLGameObject::item)
				{
					sourObj->setObject(destObjInfo);
					*sourRegion = *destRegion;
					sourObj->setCanDrag(true);
				}
			}
			else//手上拿着的东西为item或者其他东西
			{
				int nRet = g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)sourRegion, (int)destRegion);
				KUiDragItem::GetSingleton().initItem();
			}
		}
		break;
	}
	return true;
}

void KUiEquipment::onItemChanged(KObjAtContRegion* pObj, int add)
{
	if ( ms_Singleton == NULL )
	{
		return;
	}

	if ( ms_Singleton->m_pThisWnd == NULL )
	{
		return;
	}

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
	playGuaEffect(pObj->Obj.uId);
}

void KUiEquipment::onChange(UIMDLEvent& rEvent)
{
	if ( ms_Singleton == NULL || m_pThisWnd == NULL )
	{
		return;
	}
	IUIMDLDataset* pIDataset = rEvent.pDataSet;
	if ( pIDataset == NULL)
		return;

	UIMDLDatasetRecord &rRecord = pIDataset->getDataRecord( rEvent.nRecordIndex );
	KItemGroupCD_C* pCD = (KItemGroupCD_C*)rRecord.pRecordData;
	if(pCD == NULL)
		return;

	for(int i = 0; i < itempart_num; ++i)
	{
		if(NULL == d_equip[i])
			continue;
		KItemInfo tagItemInfo;
		ZeroMemory( &tagItemInfo, sizeof(KItemInfo));
		KObjAtContRegion* itemRegion = (KObjAtContRegion*)d_equip[i]->getUserData();
		g_pCoreShell->GetGameData( GDI_ITEM_INFO_INDEX, (unsigned int)&tagItemInfo, (int)itemRegion->Obj.uId );
		if ( tagItemInfo.nGroup == pCD->nGroup )
		{
			if ( pCD->ulCDTime == 0 )
			{
				d_equip[i]->setState( TLGameObject::normalState );
			}
			if ( d_equip[i]->getState() == TLGameObject::normalState )
			{
				d_equip[i]->setCoolingTime(pCD->ulCDTime);
				d_equip[i]->intonate();
			}
		}
	}	
}

void KUiEquipment::doCD(int equipIndex, int cooldownTime)
{
}

bool KUiEquipment::onClose(const CEGUI::EventArgs& e)
{
	Hide();
	return true;
}

bool KUiEquipment::btnPagebtn_BaseInfo_SelectStateChanged( const EventArgs& e )
{
	showRolePanel();
	d_titlePanel.RefreshLabel();
	return true;
}

bool KUiEquipment::btnPagebtn_Attribute_SelectStateChanged( const EventArgs& e )
{
	if ( ( NULL != m_pRb_AttributeBtn ) && ( NULL != d_rolePanel ) )
	{
		if ( m_pRb_AttributeBtn->isSelected() )
		{
			d_rolePanel->setVisible( false );
			d_titlePanel.SetVisible( false );
			d_attributePanel.SetVisible( true );
			d_attributePanel.RefreshGuoZhanAttribute();
			g_pCoreShell->OperationRequest( GOI_REFRESH_SELF_PROPERTIES, NULL, NULL );
		}
	}
	return true;
}

bool KUiEquipment::btnPagebtn_TitleInfo_SelectStateChanged( const EventArgs& e )
{
	if ( ( NULL != m_pRb_TitleInfoBtn ) && ( NULL != d_rolePanel ) )
	{
		if ( m_pRb_TitleInfoBtn->isSelected() )
		{
			d_rolePanel->setVisible( false );
			d_titlePanel.SetVisible( true );
			d_titlePanel.RefreshBar();
			d_attributePanel.SetVisible( false );
		}
	}
	return true;
}

void KUiEquipment::UpdataTitleInfo()
{
	d_titlePanel.RefreshBar();
}

void KUiEquipment::UpdataAttribute( const UiPlayerProperties* attributes )
{
	if ( NULL != attributes )
	{
		SOCIETY_INFO Info;
		ZeroMemory( &Info, sizeof(SOCIETY_INFO) );
		g_pCoreShell->GetGameData( GDI_GET_SOCIETY, (unsigned int)&Info, NULL );
		
		d_attributePanel.RefreshAttribute( attributes, Info.szShizu, Info.szZhuhou );
	}
}

void KUiEquipment::updateBaseInfo( const char* name, int level, const char* metier )
{
	if ( ( NULL != m_pBaseInfo_Name) && ( NULL != m_pBaseInfo_Level ) && ( NULL != m_pBaseInfo_Metier ) && ( NULL != m_pBaseInfo_Title ) )
	{
		if ( ( NULL != name ) && ( NULL != metier ) && ( NULL != metier ) )
		{
			m_pBaseInfo_Name->setText( AnsiToUtf8( name ) );
			m_pBaseInfo_Level->setText( iToString( level ) );
			m_pBaseInfo_Metier->setText( AnsiToUtf8( metier ) );
			String sCurTitle = d_titlePanel.GetCurrentTitle();
			m_pBaseInfo_Title->setText( sCurTitle );
		}
	}
}

void KUiEquipment::showRolePanel()
{
	if ( ( NULL != m_pRb_BaseInfoBtn ) && ( NULL != d_rolePanel ) )
	{
		if ( m_pRb_BaseInfoBtn->isSelected() )
		{
			d_rolePanel->setVisible( true );
			d_titlePanel.SetVisible( false );
			d_attributePanel.SetVisible( false );
		}
	}
}

void KUiEquipment::initRadioButtons()
{
	UI_RELEASE_TRY
	if ( NULL != m_pThisWnd )
	{
		TLRadioButton* pTempRadioButton = NULL;
		m_pRb_BaseInfoBtn = static_cast< TLRadioButton* >( m_pThisWnd->getChild( "TaharezLook/Equipment/Pagebtn_BaseInfo" ) );
		m_pRb_BaseInfoBtn->setGroupID( GroupID_rbPageBtn );
		m_pRb_BaseInfoBtn->subscribeEvent(
			Window::EventSelectStateChanged, 
			Event::Subscriber( &KUiEquipment::btnPagebtn_BaseInfo_SelectStateChanged, this ) );
		
		m_pRb_AttributeBtn = static_cast< TLRadioButton* >( m_pThisWnd->getChild( "TaharezLook/Equipment/Pagebtn_Attribute" ) );
		m_pRb_AttributeBtn->setGroupID( GroupID_rbPageBtn );
		m_pRb_AttributeBtn->subscribeEvent(
			Window::EventSelectStateChanged, 
			Event::Subscriber( &KUiEquipment::btnPagebtn_Attribute_SelectStateChanged, this ) );
		
		m_pRb_TitleInfoBtn = static_cast< TLRadioButton* >( m_pThisWnd->getChild( "TaharezLook/Equipment/Pagebtn_TitleInfo" ) );
		m_pRb_TitleInfoBtn->setGroupID( GroupID_rbPageBtn );
		m_pRb_TitleInfoBtn->subscribeEvent(
			Window::EventSelectStateChanged, 
			Event::Subscriber( &KUiEquipment::btnPagebtn_TitleInfo_SelectStateChanged, this ) );
	}
	UI_RELEASE_CATCH
}

void KUiEquipment::initRolePanel()
{
	UI_RELEASE_TRY
	if ( NULL != m_pThisWnd )
	{
		d_rolePanel			= (TLStaticImage*)m_pThisWnd->getChild("TaharezLook/Equipment/RoleInfo");
		m_pPhyCtritical		= static_cast< TLStaticText* >( d_rolePanel->getChild( d_rolePanel->getName() + "/PhyCtritical" ) );
		m_pSpellCtritical	= static_cast< TLStaticText* >( d_rolePanel->getChild( d_rolePanel->getName() + "/SpellCtrirical" ) );
		
		Window* pBaseInfoPanel = d_rolePanel->getChild( d_rolePanel->getName() + "/BaseInfoPanel" );
		m_pBaseInfo_Name	= static_cast< TLStaticText* >( pBaseInfoPanel->getChild( pBaseInfoPanel->getName() + "/Name" ) );
		m_pBaseInfo_Level	= static_cast< TLStaticText* >( pBaseInfoPanel->getChild( pBaseInfoPanel->getName() + "/Level" ) );
		m_pBaseInfo_Metier	= static_cast< TLStaticText* >( pBaseInfoPanel->getChild( pBaseInfoPanel->getName() + "/Metier" ) );
		m_pBaseInfo_Title	= static_cast< TLStaticText* >( pBaseInfoPanel->getChild( pBaseInfoPanel->getName() + "/Title" ) );
	}
	UI_RELEASE_CATCH
}

void KUiEquipment::selectDefaultPage()
{
	if ( NULL != m_pRb_BaseInfoBtn )
	{
		m_pRb_BaseInfoBtn->setSelected( true );
	}
}

void KUiEquipment::showCriticalRate( int criticalNum, Window* pWindowShow )
{
	char ctriticalShow[ COMMON_CLIENT_MSG_LEN_32 ] = { 0 };
	if ( criticalNum > 1000 )
	{
		criticalNum = 1000;
	}
	float ctriticalRate = criticalNum / 10.0f;
	_snprintf( ctriticalShow, sizeof( ctriticalShow ), "%.1f%%", ctriticalRate );
	ctriticalShow[ sizeof( ctriticalShow ) - 1 ] = 0;
	if ( NULL != pWindowShow )
	{
		pWindowShow->setText( AnsiToUtf8( ctriticalShow ) );
	}
}

void KUiEquipment::ShowTitle()
{
	Show();
	if ( ( NULL != ms_Singleton ) && ( NULL != ms_Singleton->m_pRb_TitleInfoBtn ) )
	{
		ms_Singleton->m_pRb_TitleInfoBtn->setSelected( true );
	}
}
/********************************************************************
/*						class: KUiTitlePage
*********************************************************************/

KUiTitlePage::KUiTitlePage()
{
	m_pParent			= NULL;
	m_pThisWnd			= NULL;
	m_pShowMyTitle		= NULL;
	m_pShowRandomTitle	= NULL;
	m_pWindowManager	= NULL;
	m_pList				= NULL;
	m_pListbox			= NULL;
	m_pCurSelect		= NULL;
	m_pOutterTitle		= NULL;

	m_infoCount			= -1;
	m_infoCount_listbox = -1;
	m_barHeight			= -1;
	m_exceedHeight		= 0;
	m_pageHeight		= 0;
	ZeroMemory( m_titleInfo, sizeof( m_titleInfo ) );
	ZeroMemory( m_titleInfo_listbox, sizeof( m_titleInfo_listbox ) );

	m_isRadioButtonLocked = false;
}

void KUiTitlePage::Init( Window* pParentWindow, WindowManager* pWindowManager, Window* pRootSheet )
{
	UI_RELEASE_TRY
	if ( ( NULL != pParentWindow ) && ( NULL != pWindowManager ) )
	{
		m_pWindowManager = pWindowManager;
		m_pParent = pParentWindow;

		//取得当前窗口
		m_pThisWnd = m_pParent->getChild( "TaharezLook/Equipment/TitleInfoPanel" );
		
		String wndName = m_pThisWnd->getName();

		m_pList = static_cast< TLListView* >( m_pThisWnd->getChild( wndName + "/TitleList" ) );
		m_pList->SetBarLayoutsFileName( BarLayoutsFileName );
		m_pList->SetTitleSpaceHeight( m_pThisWnd->getChild( wndName + "/TitleTitle" )->getHeight( Absolute ) );
		m_pList->subscribeEvent(
			Window::EventListViewUpdateItem,
			Event::Subscriber( &KUiTitlePage::TitleList_UpdateBar, this ) );

		m_pageHeight = m_pList->getAbsoluteHeight();

		m_pShowMyTitle = static_cast< RadioButton* >( m_pThisWnd->getChild( wndName + "/ShowMyTitle" ) );
		m_pShowRandomTitle = static_cast< RadioButton* >( m_pThisWnd->getChild( wndName + "/ShowRandomTitle" ) );

		m_pShowMyTitle->subscribeEvent(
			Window::EventSelectStateChanged,
			Event::Subscriber( &KUiTitlePage::cbShowMyTitle_SelectStateChanged, this ) );

		m_pShowRandomTitle->subscribeEvent(
			Window::EventSelectStateChanged,
			Event::Subscriber( &KUiTitlePage::cbShowRandomTitle_SelectStateChanged, this ) );

		m_pListbox = static_cast< TLListbox* >( m_pThisWnd->getChild( wndName + "/TitleListbox" ) );
		m_pListbox->setStaticBackGroudImage("ty_tip1_tu","full_image");
		m_pListbox->setZLevel( Window::Top );
		m_pListbox->subscribeEvent( 
			TLListbox::EventMouseClick,
			Event::Subscriber( &KUiTitlePage::TitleListbox_MouseClick, this ) );

		m_pThisWnd->getChild( wndName + "/PopupTitle" )->subscribeEvent(
			Window::EventMouseClick, 
			Event::Subscriber( &KUiTitlePage::PopupTitle_MouseClick, this ) );

		m_pCurSelect = m_pThisWnd->getChild( wndName + "/CurSelect" );

		m_pShowMyTitle->setGroupID( GroupID_cbTitle );
		m_pShowRandomTitle->setGroupID( GroupID_cbTitle );

		m_strNoTitle			= AnsiToUtf8( KMessageCentre::GetMessageSafe( title_info_message, ME_NO_TITLE ) );
		m_strPermanent			= AnsiToUtf8( KMessageCentre::GetMessageSafe( title_info_message, ME_PERMANENT_STATE ) );

		m_strState_notGranted	= KMessageCentre::GetMessageSafe( title_info_message, ME_STATE_NOT_GRANTED );
		m_strState_Granted		= KMessageCentre::GetMessageSafe( title_info_message, ME_STATE_GRANTED );
		m_strTipFormatTxt_nextLevel = KMessageCentre::GetMessageSafe( title_info_message, ME_TIP_NEXT_LEVEL );
		m_strTipFormatTxt_normal	= KMessageCentre::GetMessageSafe( title_info_message, ME_TIP_FORMAT_TEXT );

		m_yearShow		= KMessageCentre::GetMessageSafe( time_message, 0 );
		m_monShow		= KMessageCentre::GetMessageSafe( time_message, 1 );
		m_dayShow		= KMessageCentre::GetMessageSafe( time_message, 2 );
		m_hourShow		= KMessageCentre::GetMessageSafe( time_message, 3 );
		m_minuteShow	= KMessageCentre::GetMessageSafe( time_message, 4 );
		m_dayString		= KMessageCentre::GetMessageSafe( time_message, 5 );
		m_hourString	= KMessageCentre::GetMessageSafe( time_message, 6 );
		m_minString		= KMessageCentre::GetMessageSafe( time_message, 7 );
		m_tipString		= KMessageCentre::GetMessageSafe( time_message, 8 );
		m_timeOverString = KMessageCentre::GetMessageSafe( time_message, 9 );
		m_timeOverStringForTip = KMessageCentre::GetMessageSafe( time_message, 10 );
		int r = 0, g = 0, b = 0;

		const char* tmpColorText = KUiCfgLoader::getSingleton().getEquipmentCfg().grantColor;
		sscanf( tmpColorText, "%d,%d,%d", &r, &g, &b);
		m_clGrant.set( 
			static_cast< float >( r ) / 255.0f ,
			static_cast< float >( g ) / 255.0f ,
			static_cast< float >( b ) / 255.0f );

		tmpColorText = KUiCfgLoader::getSingleton().getEquipmentCfg().notGrantColor;
		sscanf( tmpColorText, "%d,%d,%d", &r, &g, &b);
		m_clNotGrant.set( 
			static_cast< float >( r ) / 255.0f,
			static_cast< float >( g ) / 255.0f,
			static_cast< float >( b ) / 255.0f );

		tmpColorText = KUiCfgLoader::getSingleton().getEquipmentCfg().nextLevelColor;
		sscanf( tmpColorText, "%d,%d,%d", &r, &g, &b);
		m_clNextLevel.set( 
			static_cast< float >( r ) / 255.0f,
			static_cast< float >( g ) / 255.0f,
			static_cast< float >( b ) / 255.0f );
		
	}
	UI_RELEASE_CATCH
}

bool KUiTitlePage::cbShowMyTitle_SelectStateChanged( const EventArgs& e )
{
	if ( ( NULL != m_pShowMyTitle ) && ( NULL != m_pListbox ) && ( !m_isRadioButtonLocked ) )
	{
		if ( m_pShowMyTitle->isSelected() )
		{
			ListboxTitleItem* curItem = static_cast< ListboxTitleItem* >( m_pListbox->getFirstSelectedItem() );
			if ( NULL != curItem )
			{
				g_pCoreShell->OperationRequest( GOI_SELECT_TITLE, NULL, curItem->getID() );	
			}
		}
	}
	return true;
}

bool KUiTitlePage::cbShowRandomTitle_SelectStateChanged( const EventArgs& e )
{
	if ( ( NULL != m_pShowRandomTitle ) && ( !m_isRadioButtonLocked ) )
	{
		if ( m_pShowRandomTitle->isSelected() )
		{
			g_pCoreShell->OperationRequest( GOI_SELECT_TITLE, NULL, -1 );
		}
	}
	return true;
}

void KUiTitlePage::RefreshBar()
{
	if ( NULL != m_pListbox )
	{
		m_infoCount_listbox = g_pCoreShell->GetGameData( GDI_GET_TITLE_INFO, reinterpret_cast< unsigned int >( m_titleInfo_listbox ), MAX_TITLEINFO_COUNT );
		m_pListbox->resetList();
		TitleItemList[0].setText( m_strNoTitle );
		TitleItemList[0].setID( 0 );
		m_pListbox->addItem( &TitleItemList[0] );

		for ( int i = 0; i < m_infoCount_listbox; ++i )
		{
			TitleItemList[i + 1].setText( AnsiToUtf8( m_titleInfo_listbox[i].Name ) );
			TitleItemList[i + 1].setID( m_titleInfo_listbox[i].ID );
			m_pListbox->addItem( &TitleItemList[i + 1] );
		}
	}
	
	if ( NULL != m_pList )
	{
		m_infoCount = g_pCoreShell->GetGameData( GDI_GET_DETAIL_TITLE_INFO, reinterpret_cast< unsigned int >( m_titleInfo ), MAX_TITLEINFO_COUNT );
		m_pList->RefreshBars( m_titleInfo, m_infoCount );
	}
	
	refreshRadioButton();
	RefreshLabel();
}

void KUiTitlePage::setBar( const ListViewItem* pItem, const UiTitleInfo& pTitleInfoList )
{
	UI_RELEASE_TRY
	if ( NULL != pItem )
	{
		Window* pBar = pItem->GetBar();
		if ( NULL != pBar )
		{
			String currentBarName = pBar->getName();
			pBar->getChild( currentBarName + "/Title" )->setText( AnsiToUtf8( pTitleInfoList.Name ) );
			pBar->getChild( currentBarName + "/Type" )->setText( AnsiToUtf8( KMessageCentre::GetMessageSafe( title_info_message, pTitleInfoList.Type ) ) );
			
			//默认状态显示为“-”，根据类型刷新状态的显示
			pBar->getChild( currentBarName + "/State" )->setText( INVALID_VALUE_SHOW );
			pBar->setTooltipText( formatToolTip( pTitleInfoList.Tip ) );


			switch ( pTitleInfoList.Type )
			{
			case title_type_permanent:
				{
					pBar->getChild( currentBarName + "/State" )->setText( m_strPermanent );
					pBar->setTooltipText( formatToolTip( m_strState_Granted.c_str() ) );
					setBarColor( *pBar, m_clGrant );
				}
				break;
			case title_type_time_limited:
				{
					tm* expiredTime = localtime( reinterpret_cast< const time_t* >( &( pTitleInfoList.State ) ) );
					time_t expTime = *( reinterpret_cast< const time_t* >( &( pTitleInfoList.State ) ) );
					time_t current  = time( NULL );
					if ( NULL != expiredTime )
					{
						string dayLeft;
						double diff = difftime( expTime, current );  
						string sTimeShow;
						if( diff > 0)
						{
							ostringstream repayTime;
							repayTime << m_tipString << expiredTime->tm_year + 1900 << m_yearShow
								<< expiredTime->tm_mon + 1 << m_monShow
								<< expiredTime->tm_mday << m_dayShow ;
							string diffPostfix;
							int day = diff / ( 3600 * 24 );
							if( day != 0)
								diffPostfix = m_dayString;
							if( day == 0)
							{
								day = diff / 3600;
								if( day != 0)
									diffPostfix = m_hourString;
							}
							if( day == 0 )
							{
								day = diff / 60;
								diffPostfix = m_minString;
							}
							ostringstream dayLeftTime;
							dayLeftTime << day << diffPostfix;
							dayLeft = dayLeftTime.str();
							sTimeShow = repayTime.str();
						}
						else 
						{
							dayLeft = m_timeOverString;
							sTimeShow = m_timeOverStringForTip;
						}
						pBar->getChild( currentBarName + "/State" )->setText( AnsiToUtf8( dayLeft.c_str() ) );
						pBar->setTooltipText( formatToolTip( sTimeShow.c_str() ) );
					}

					setBarColor( *pBar, m_clGrant );
				}
				break;
			case title_type_upgradable:
				{
					/*pBar->getChild( currentBarName + "/State" )->setText( iToString( pTitleInfoList.State ) );*/
					if ( title_active_state_no == pTitleInfoList.ActiveState )
					{
						pBar->getChild( currentBarName + "/State" )->setText( AnsiToUtf8( m_strState_notGranted.c_str() ) );
						pBar->setTooltipText( formatToolTip( m_strState_notGranted.c_str() ) );
						setBarColor( *pBar, m_clNotGrant );
					}
					else if ( title_active_state_yes == pTitleInfoList.ActiveState )
					{
						pBar->getChild( currentBarName + "/State" )->setText( AnsiToUtf8( m_strState_Granted.c_str() ) );
						pBar->setTooltipText( formatToolTip( m_strState_Granted.c_str() ) );
						setBarColor( *pBar, m_clGrant );
					}
					else if ( title_active_state_next == pTitleInfoList.ActiveState || title_active_state_first == pTitleInfoList.ActiveState )
					{
						pBar->getChild( currentBarName + "/State" )->setText( AnsiToUtf8( m_strState_notGranted.c_str() ) );
						string stateValue = iToStr( pTitleInfoList.State );
						char showTip[ COMMON_CLIENT_MSG_LEN_512 ] = { 0 };
						_snprintf( showTip, sizeof( showTip ), pTitleInfoList.Tip, stateValue.c_str() );
						/*pBar->setTooltipText( formatToolTip( stateValue.c_str(), m_strTipFormatTxt_nextLevel.c_str() ) );*/
						pBar->setTooltipText( formatToolTip( showTip ) );
						setBarColor( *pBar, m_clNextLevel );
					}
				}
				break;
			}
		}
	}
	UI_RELEASE_CATCH
}

bool KUiTitlePage::PopupTitle_MouseClick( const EventArgs& e )
{
	if ( NULL != m_pListbox )
	{
		if ( m_pListbox->isVisible() )
		{
			m_pListbox->hide();
		}
		else
		{
			m_pListbox->show();
		}
	}
	return true;
}

bool KUiTitlePage::TitleListbox_MouseClick( const EventArgs& e )
{
	UI_RELEASE_TRY
	const WindowEventArgs& pWinEvent = static_cast< const WindowEventArgs& >( e );
	TLListbox* pListbox = static_cast< TLListbox* >( pWinEvent.window );

	if ( NULL != pListbox )
	{
		ListboxTitleItem* curItem = static_cast< ListboxTitleItem* >( pListbox->getFirstSelectedItem() );
		
		if ( ( curItem == NULL ) && ( pListbox->getItemCount() > 0 ) )
		{
			pListbox->setItemSelectState( static_cast< int >( 0 ), true );
			curItem  = static_cast< ListboxTitleItem* >( pListbox->getListboxItemFromIndex( 0 ) );
		}
		
		pListbox->hide();
		
		if ( NULL != curItem )
		{
			m_pCurSelect->setText( curItem->getText() );
			g_pCoreShell->OperationRequest( GOI_SELECT_TITLE, NULL, curItem->getID() );	
		}
	}
	UI_RELEASE_CATCH_RETURN( false )
	return true;
}

String KUiTitlePage::GetCurrentTitle()
{
	return m_sCurSelected;
}

void KUiTitlePage::SetOutterTitle( Window* pWnd )
{
	if ( NULL != pWnd )
	{
		m_pOutterTitle = static_cast< TLStaticText* >( pWnd );
	}
}	

void KUiTitlePage::RefreshLabel()
{
	if ( m_infoCount < 0 )
	{
		m_infoCount = g_pCoreShell->GetGameData( GDI_GET_TITLE_INFO, reinterpret_cast< unsigned int >( m_titleInfo ), MAX_TITLEINFO_COUNT );
	}
	
	const int curRealTitle	= g_pCoreShell->GetGameData( GDI_GET_SELF_TITLE, NULL, NULL );
	
	if ( ( NULL != m_pOutterTitle ) && ( NULL != m_pCurSelect ) )
	{
		m_pOutterTitle->setText( m_strNoTitle );
		m_pCurSelect->setText( m_strNoTitle );
		m_sCurSelected = m_strNoTitle; 
		
		for ( int k = 0; k < m_infoCount; k++ )
		{
			if ( curRealTitle == m_titleInfo_listbox[k].ID )
			{
				m_sCurSelected = AnsiToUtf8( m_titleInfo_listbox[k].Name );
				m_pOutterTitle->setText( m_sCurSelected );
				m_pCurSelect->setText( m_sCurSelected );
				break;
			}
		}
	}
} 

CEGUI::String KUiTitlePage::formatToolTip( const char* szToopTip, const char* szFormatStr /*= NULL*/ )
{
	char tipText[ COMMON_CLIENT_MSG_LEN_512 ] = { 0 };
	if ( NULL != szFormatStr )
	{
		_snprintf( tipText, sizeof( tipText ), szFormatStr, szToopTip );
		return String( AnsiToUtf8( tipText ) );
	}
	else
	{
		if ( m_strTipFormatTxt_normal.empty() )
		{
			return String( AnsiToUtf8( szToopTip ) );
		}
		else
		{
			_snprintf( tipText, sizeof( tipText ), m_strTipFormatTxt_normal.c_str(), szToopTip );
			return String( AnsiToUtf8( tipText ) );
		}
	}
}

void KUiTitlePage::SetVisible( bool visible )
{
	if ( NULL != m_pThisWnd )
	{
		m_pThisWnd->setVisible( visible );
	}
}

bool KUiTitlePage::TitleList_UpdateBar( const EventArgs& e )
{
	const ListViewEventArgs& lvArgs = static_cast< const ListViewEventArgs& >( e );
	const ListViewItem* pItem = lvArgs.GetListViewItem();
	const void* pData = pItem->GetData();
	setBar( pItem, *( static_cast< const UiTitleInfo* >( pData ) ) );
	return true;
}

void KUiTitlePage::refreshRadioButton()
{
	//这里是服务器发来消息刷新界面，所以不需要出发SelecteStatueChanged消息
	//SelecteStatueChanged会给服务器发请求，导致死循环
	m_isRadioButtonLocked = true;
	const int curTitle	= g_pCoreShell->GetGameData( GDI_GET_SELECTED_TITLE, NULL, NULL );
	if ( curTitle != -1 )
	{
		if ( NULL != m_pShowMyTitle )
		{
			m_pShowMyTitle->setSelected( true );
		}
	}
	else
	{
		if ( NULL != m_pShowRandomTitle )
		{
			m_pShowRandomTitle->setSelected( true );
		}
	}
	m_isRadioButtonLocked = false;
}

void KUiTitlePage::setBarColor( const Window& bar, const colour& col )
{
	static_cast< TLStaticText* >( bar.getChild( bar.getName() + "/Title"	) )->setTextColours( col );
	static_cast< TLStaticText* >( bar.getChild( bar.getName() + "/Type"		) )->setTextColours( col );
	static_cast< TLStaticText* >( bar.getChild( bar.getName() + "/State"	) )->setTextColours( col );
}
/********************************************************************
/*						class: KUiAttributePage
*********************************************************************/
KUiAttributePage::KUiAttributePage()
{
	m_pParent					= NULL;
	m_pThisWnd					= NULL;
	m_pList						= NULL;
	m_pScrollbar				= NULL;

	//基本信息面板控件
	m_pBaseInfoPanel			= NULL;
	m_pBaseInfo_Huoyuedu		= NULL;
	m_pBaseInfo_HuoyueduCur		= NULL;
	m_pBaseInfo_ShizuGongxun	= NULL;
	m_pBaseInfo_ShizuGongxunCur = NULL;
	
	//氏族信息面板控件
	m_pShizuPanel				= NULL;
	m_pShizu_Name				= NULL;
	m_pShizu_Renqi				= NULL;
	m_pShizu_MemberCount		= NULL;
	m_pShizu_Level				= NULL;
	m_pShizu_Flourish			= NULL;
	m_pShizu_ZhuhouGongxun		= NULL;
	m_pShizu_BuildingCount1		= NULL;
	m_pShizu_BuildingCount2		= NULL;
	m_pShizu_Money				= NULL;
	m_pShizu_Resource1			= NULL;
	m_pShizu_Resource2			= NULL;
	m_pShizu_Resource3			= NULL;
	
	//诸侯信息面板控件
	m_pZhuhouPanel				= NULL;
	m_pZhuhou_Name				= NULL;
	m_pZhuhou_Renqi				= NULL;
	m_pZhuhou_MemberCount		= NULL;
	m_pZhuhou_ShizuCount		= NULL;
	m_pZhuhou_Flourish			= NULL;
	m_pZhuhou_Train				= NULL;
	m_pZhuhou_BuildingCount1	= NULL;
	m_pZhuhou_BuildingCount2	= NULL;
	m_pZhuhou_Money				= NULL;
	m_pZhuhou_Resource1			= NULL;
	m_pZhuhou_Resource2			= NULL;
	m_pZhuhou_Resource3			= NULL;
	
	//城市面板空间
	m_pGuozhanPanel				=NULL;
	m_pGuozhanPanel_Gongxun		=NULL;
	m_pGuozhanPanel_Junjie		=NULL;
	m_pGuozhanPanel_Fenglu		=NULL;
	m_gongxunToLevel.push_back( -1 );
	m_exceedHeight	= 0;
	m_listHeight	= 0;
}


void KUiAttributePage::Init( Window* pParentWindow )
{
	UI_RELEASE_TRY
	if ( NULL != pParentWindow )
	{
		m_pParent = pParentWindow;
		m_pThisWnd = m_pParent->getChild( m_pParent->getName() + "/AttributePanel" );
		
		String wndName = m_pThisWnd->getName();
		
		m_pList = m_pThisWnd->getChild( wndName + "/ChildPanel" );
		m_listHeight = m_pList->getAbsoluteHeight();
		m_pList->subscribeEvent( TLVertScrollbar::EventMouseWheel, Event::Subscriber( &KUiAttributePage::Panel_MouseWheel, this ) );
		initBaseInfoPanel();
		initShizuPanel();
		initZhuhouPanel();
		initGuozhanPanel();
		m_pScrollbar = static_cast< TLVertScrollbar* >( m_pThisWnd->getChild( wndName + "/Scrollbar" ) );
		m_pScrollbar->subscribeEvent(
			TLVertScrollbar::EventScrollPositionChanged, 
			Event::Subscriber( &KUiAttributePage::sbScrollBar_handleScroll, this ) );

		m_exceedHeight = m_listHeight - m_pThisWnd->getHeight( Absolute );
		if ( m_exceedHeight > 0 )
		{
			float stepSize = static_cast< float >( 30.0  ) / static_cast< float >( m_exceedHeight );
			m_pScrollbar->setStepSize( ( ( stepSize > 0.0 ) && ( stepSize < 1.0 ) ) ? stepSize : 1.0 );
			m_pScrollbar->setVisible( true );
		}
		else
		{
			m_pScrollbar->setVisible( false );
		}

		g_pCoreShell->OperationRequest( GOI_REFRESH_SELF_PROPERTIES, NULL, NULL );
	}
	UI_RELEASE_CATCH
}


bool KUiAttributePage::sbScrollBar_handleScroll( const EventArgs& e )
{
	if ( ( m_exceedHeight > 0 ) && ( NULL != m_pScrollbar ) )
	{
		float scrollPos = m_pScrollbar->getScrollPosition();
		int scrollSize = - scrollPos * m_exceedHeight;

		m_pList->setXPosition( Absolute, m_pList->getAbsoluteXPosition() );
		m_pList->setYPosition( Absolute, scrollSize );
	}
	return true;	
}

void KUiAttributePage::RefreshAttribute( const UiPlayerProperties* attributes, const char* szShizuName, const char* szZhuhouName )
{
	if ( NULL != attributes )
	{
		refreshBaseInfoPanel( attributes );
		refreshShizuPanel( attributes );
		refreshZhuhouPanel( attributes );
	}

	refreshGuozhanPanel();
	
	//显示氏族和诸侯名字
	if ( NULL != m_pShizu_Name )
	{
		if ( NULL == szShizuName || szShizuName[0] == 0 )
		{
			m_pShizu_Name->setText( INVALID_VALUE_SHOW );
		}
		else
		{
			m_pShizu_Name->setText( AnsiToUtf8( szShizuName ) );
		}
	}
	if ( NULL != m_pZhuhou_Name )
	{
		if ( NULL == szZhuhouName || szZhuhouName[0] == 0 )
		{
			m_pZhuhou_Name->setText( INVALID_VALUE_SHOW );
		}
		else
		{
			m_pZhuhou_Name->setText( AnsiToUtf8( szZhuhouName ) );
		}
	}
}

void KUiAttributePage::initBaseInfoPanel()
{
	UI_RELEASE_TRY
	if ( NULL != m_pList )
	{
		m_pBaseInfoPanel = m_pList->getChild( m_pList->getName() + "/BaseInfoPanel" );
		String panelName = m_pBaseInfoPanel->getName();

		m_pBaseInfo_Huoyuedu		= static_cast< TLStaticText* >( m_pBaseInfoPanel->getChild( panelName + "/Huoyuedu" ) );
		m_pBaseInfo_HuoyueduCur		= static_cast< TLStaticText* >( m_pBaseInfoPanel->getChild( panelName + "/HuoyueduCur" ) );
		m_pBaseInfo_ShizuGongxun	= static_cast< TLStaticText* >( m_pBaseInfoPanel->getChild( panelName + "/ShizuGongxun" ) );
		m_pBaseInfo_ShizuGongxunCur	= static_cast< TLStaticText* >( m_pBaseInfoPanel->getChild( panelName + "/ShizuGongxunCur" ) );
	}
	UI_RELEASE_CATCH
}

void KUiAttributePage::initShizuPanel()
{
	UI_RELEASE_TRY
	if ( NULL != m_pList )
	{
		m_pShizuPanel		= m_pList->getChild( m_pList->getName() + "/ShizuPanel" );
		String panelName	= m_pShizuPanel->getName();

		m_pShizu_Name			= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/ShizuName" ) );
		m_pShizu_Renqi			= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Renqi" ) );
		m_pShizu_MemberCount	= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/MemberCount" ) );
		m_pShizu_Level			= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Level" ) );
		m_pShizu_Flourish		= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Flourish" ) );
		m_pShizu_ZhuhouGongxun	= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/ZhuhouGongxun" ) );
		m_pShizu_BuildingCount1	= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/BuildingCount1" ) );
		m_pShizu_BuildingCount2	= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/BuildingCount2" ) );
		m_pShizu_Money			= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Money" ) );
		m_pShizu_Resource1		= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Resource1" ) );
		m_pShizu_Resource2		= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Resource2" ) );
		m_pShizu_Resource3		= static_cast< TLStaticText* >( m_pShizuPanel->getChild( panelName + "/Resource3" ) );
	}
	UI_RELEASE_CATCH
}

void KUiAttributePage::initZhuhouPanel()
{
	UI_RELEASE_TRY
	if ( NULL != m_pList )
	{
		m_pZhuhouPanel		= m_pList->getChild( m_pList->getName() + "/ZhuhouPanel" );
		String panelName	= m_pZhuhouPanel->getName();

		m_pZhuhou_Name				= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/ZhuhouName" ) );
		m_pZhuhou_Renqi				= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Renqi" ) );
		m_pZhuhou_MemberCount		= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/MemberCount" ) );
		m_pZhuhou_ShizuCount		= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/ShizuCount" ) );
		m_pZhuhou_Flourish			= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Flourish" ) );
		m_pZhuhou_Train				= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Train" ) );
		m_pZhuhou_BuildingCount1	= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/BuildingCount1" ) );
		m_pZhuhou_BuildingCount2	= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/BuildingCount2" ) );
		m_pZhuhou_Money				= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Money" ) );
		m_pZhuhou_Resource1			= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Resource1" ) );
		m_pZhuhou_Resource2			= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Resource2" ) );
		m_pZhuhou_Resource3			= static_cast< TLStaticText* >( m_pZhuhouPanel->getChild( panelName + "/Resource3" ) );
	}
	UI_RELEASE_CATCH
}

void KUiAttributePage::initGuozhanPanel()
{
	UI_RELEASE_TRY
		if ( NULL != m_pList )
		{
			m_pGuozhanPanel					= m_pList->getChild( m_pList->getName() + "/GuozhanPanel" );
			String panelName				= m_pGuozhanPanel->getName();
			
			m_pGuozhanPanel_Gongxun			= static_cast< TLStaticText* >( m_pGuozhanPanel->getChild( panelName + "/Gongxun" ) );
			m_pGuozhanPanel_Junjie			= static_cast< TLStaticText* >( m_pGuozhanPanel->getChild( panelName + "/Junjie" ) );
			m_pGuozhanPanel_Fenglu			= static_cast< TLStaticText* >( m_pGuozhanPanel->getChild( panelName + "/Fenglu" ) );
			KTabFile buff_qlevel_File;
			if( buff_qlevel_File.Load( GongXunToLevel.c_str() ) )
			{
				const int recordCount = buff_qlevel_File.GetHeight() - 1;
				int columnCount = buff_qlevel_File.GetWidth();
				
				for ( int row = 0; row < recordCount; ++row )
				{	
					int tempRes = 0;
					buff_qlevel_File.GetInteger( row + 2, columnCount, 0, &tempRes );
					m_gongxunToLevel.push_back( tempRes );
				}
				
			}
		}
	UI_RELEASE_CATCH
	
}

void KUiAttributePage::refreshBaseInfoPanel( const UiPlayerProperties* attributes )
{
	if ( NULL != attributes )
	{
		if ( NULL != m_pBaseInfo_Huoyuedu )
		{
			m_pBaseInfo_Huoyuedu->setText( iToString( attributes->ActiveDegreeTotal ) );
		}
		if ( NULL != m_pBaseInfo_HuoyueduCur )
		{
			m_pBaseInfo_HuoyueduCur->setText( iToString( attributes->ActiveDegreeCurrent ) );
		}
	}
}

void KUiAttributePage::refreshShizuPanel( const UiPlayerProperties* attributes )
{
	if ( NULL != attributes )
	{
		bool isShizuExists	= ( attributes->ShizuPlayerCount > 0 );
		bool isControlValid	= ( NULL != m_pShizu_Renqi ) && ( NULL != m_pShizu_MemberCount );
		
		if ( isControlValid )
		{
			if ( isShizuExists )
			{
				m_pShizu_Renqi->setText( iToString( attributes->ShizuPopularity ) );
				m_pShizu_MemberCount->setText( iToString( attributes->ShizuPlayerCount ) );
			}
			else
			{
				m_pShizu_Renqi->setText( INVALID_VALUE_SHOW );
				m_pShizu_MemberCount->setText( INVALID_VALUE_SHOW );
			}
		}
	}
}

void KUiAttributePage::refreshZhuhouPanel( const UiPlayerProperties* attributes )
{
	if ( NULL != attributes )
	{
		bool isZhuhouExists	= ( attributes->ZhuhouShizuCount > 0 );
		bool isControlValid	= ( NULL != m_pZhuhou_Renqi ) && ( NULL != m_pZhuhou_MemberCount ) && ( NULL != m_pZhuhou_ShizuCount );
		
		if ( isControlValid )
		{
			if ( isZhuhouExists )
			{
				m_pZhuhou_Renqi->setText( iToString( attributes->ZhuhouPopularity ) );
				m_pZhuhou_MemberCount->setText( iToString( attributes->ZhuhouPlayerCount ) );
				m_pZhuhou_ShizuCount->setText( iToString( attributes->ZhuhouShizuCount ) );
			}
			else
			{
				m_pZhuhou_Renqi->setText( INVALID_VALUE_SHOW );
				m_pZhuhou_MemberCount->setText( INVALID_VALUE_SHOW );
				m_pZhuhou_ShizuCount->setText( INVALID_VALUE_SHOW );
			}
		}
	}
}
void KUiAttributePage::refreshGuozhanPanel()
{
	if( !g_pCoreShell )
		return ;
	int gongXun = g_pCoreShell->GetGameData( GDI_GET_PLUS_POINT_BY_PLUSPOINT_INDEX, GongXunParam, NULL );
	int fengLu	= g_pCoreShell->GetGameData( GDI_GET_PLUS_POINT_BY_PLUSPOINT_INDEX, FengLuparam, NULL );
	int junJie	= GetJunJie( gongXun );
	if(	m_pGuozhanPanel_Gongxun && m_pGuozhanPanel_Junjie && m_pGuozhanPanel_Fenglu)
	{
		m_pGuozhanPanel_Gongxun->setText( ( iToString( gongXun ) ) );
		m_pGuozhanPanel_Junjie->setText( ( iToString( junJie ) ) );
		m_pGuozhanPanel_Fenglu->setText( ( iToString( fengLu ) ) );
	}
}

int KUiAttributePage::GetJunJie( int gongXun )
{
	for( vector< int >::iterator it = m_gongxunToLevel.begin(); m_gongxunToLevel.end() != it; ++it)
	{
		if ( gongXun <= ( *it ) )
			break;
	}
	if ( m_gongxunToLevel.end() != it )
		return ( it - m_gongxunToLevel.begin() );
	else
	{	if( gongXun > ( * ( m_gongxunToLevel.end() - 1 ) ) )
			return ( m_gongxunToLevel.end() - m_gongxunToLevel.begin() ) - 1;
		return -1;
	}
}

void KUiAttributePage::RefreshGuoZhanAttribute()
{
	refreshGuozhanPanel();
	m_pScrollbar->setScrollPosition( 0.0f );
}

bool KUiAttributePage::Panel_MouseWheel( const CEGUI::EventArgs& e )
{
	MouseEventArgs* eventArgs = ( MouseEventArgs* )( &e );
	if( !m_pScrollbar )
		assert( FALSE );
	if( m_pScrollbar->isVisible() )
	{
		float newPos = m_pScrollbar->getScrollPosition() - m_pScrollbar->getStepSize() * eventArgs->wheelChange;
		m_pScrollbar->setScrollPosition( newPos );
	}
	return TRUE;	
}
void KUiAttributePage::SetVisible( bool visible )
{ 
	if ( NULL != m_pThisWnd )
	{
		m_pThisWnd->setVisible( visible );
	}
}
	
