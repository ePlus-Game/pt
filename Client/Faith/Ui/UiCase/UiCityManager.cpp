//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 01/08/2007 14:44
//      File_base        : UiCityManager
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiCityManager.h"
#include "UiMDLInterface.h"
#include "CoreShell.h"
#include "ui\UiConfigManager.h"
#include "../KMessageCentre.h"

extern iCoreShell* g_pCoreShell;
const int Zero = 0;

using namespace CEGUI;

template<> 
KUiCityManager* KUiWndSingleton<KUiCityManager>::ms_Singleton	= NULL;

KUiCityManager::KUiCityManager( const CEGUI::String& id_name ):
KUiWndSingleton<KUiCityManager>( id_name )
{
	d_CityInfoPage_CityDevelop = NULL;
	d_CityInfoPage_CityTired	= NULL;
}


KUiCityManager::~KUiCityManager()
{
}

void KUiCityManager::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		d_CityTaxRate =0;
		
		ms_Singleton->d_CurBuildingID = -1;
	
		IUIMDLDataset* pCityDS = NULL;
		if ( success_errorcode != ms_Singleton->m_pUiMDLManager->queryDataSet( city_dataset, &pCityDS ) )
		{
			return;
		}
		pCityDS->setEventHandle( ms_Singleton );
		ms_Singleton->d_CityInfoPage = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_CITYBASEINFOPAGE , "", "", NULL, NULL, true );
		ms_Singleton->d_BuildingInfoPage = ms_Singleton->m_pWindowManager->loadWindowLayout( UI_CITYBUILDINGPAGE , "", "", NULL, NULL, true );
		ms_Singleton->m_pThisWnd->addChildWindow( ms_Singleton->d_CityInfoPage );
		ms_Singleton->m_pThisWnd->addChildWindow( ms_Singleton->d_BuildingInfoPage );
		ms_Singleton->hideAllPage();

		d_CityInfoPage_CityDevelop = static_cast< TLStaticText* >( d_CityInfoPage->getChild( "TaharezLook/CityStorageInfo/CityDevelop" ) );
		d_CityInfoPage_CityTired	= static_cast< TLStaticText* >( d_CityInfoPage->getChild( "TaharezLook/CityStorageInfo/CityTired" ) );

//		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/OkBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleSetRes, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/CancelBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleGetRes, ms_Singleton));
        
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBuildingInfoPage/OkBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleRepair, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityManager/BaseInfoPageBtn")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleShowBaseInfo, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityManager/BuildingInfoPageBtn")->subscribeEvent(RadioButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleShowBuildingInfo, ms_Singleton));

		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityManager/CloseBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleExit, ms_Singleton));
        ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityManager/Shuishou")->subscribeEvent(PushButton::EventMouseClick,Event::Subscriber(&KUiCityManager::handleEditTax, ms_Singleton));
		//ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityManager/ExitBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityManager::handleExit, ms_Singleton));

		for ( int nIdx = 0; nIdx < BUILDING_PER_PAGE; ++nIdx )
		{
			char szBuf[COMMON_CLIENT_MSG_LEN_64];
			sprintf( szBuf, "TaharezLook/CityBuildingInfoPage/Building%d", nIdx );
			Window* pBar = ms_Singleton->d_BuildingInfoPage->getChild( szBuf );
			if ( pBar )
			{
				int* pId = ms_Singleton->d_BuildID;
				sprintf( szBuf, "%s%s", szBuf, "/icon" );
				pBar->getChild( szBuf )->setUserData( pId + nIdx );
				pBar->getChild( szBuf )->subscribeEvent(TLGameObject::EventClicked, Event::Subscriber(&KUiCityManager::handleBuilding, ms_Singleton ) );
			}
		}

		char* pTempMsg = KMessageCentre::GetMessage( common_message, 16 );
		if ( NULL != pTempMsg )
		{
			d_strNothing = AnsiToUtf8( pTempMsg );
		}
		else
		{
			d_strNothing = "";
		}
	}
}

void	KUiCityManager::Show( void )
{
	KUiWndSingleton<KUiCityManager>::Show();
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->hideAllPage();
		ms_Singleton->d_CityInfoPage->show();
//		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/OkBtn")->show();
//		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/CancelBtn")->show();
	}
}

void	KUiCityManager::hideAllPage( void )
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
//		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/OkBtn")->hide();
//		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/CancelBtn")->hide();
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBuildingInfoPage/OkBtn")->hide();
	}

	d_CityInfoPage->hide();
	d_BuildingInfoPage->hide();
}

void	KUiCityManager::onCreate( UIMDLEvent& rEvent	)
{

}

void	KUiCityManager::onRelease( UIMDLEvent& rEvent	)
{

}

void	KUiCityManager::onChange( UIMDLEvent& rEvent	)
{
	int nIdx = rEvent.nRecordIndex;
	IUIMDLDataset* pCityInfo = rEvent.pDataSet;
	if ( pCityInfo	)
	{
		UIMDLDatasetRecord rRecord = pCityInfo->getDataRecord( nIdx );
		if ( rRecord.pRecordData )
		{
			CityInfoParam* pCityInfo = (CityInfoParam*)rRecord.pRecordData;
			if ( pCityInfo )
			{
				switch( pCityInfo->eType )
				{
				case base_info_city:
					updateCityBaseInfo( pCityInfo );
					break;
				case building_info_city:
					updateBuildingInfo( pCityInfo );
					break;
				}
			}
		}
	}
}

bool	KUiCityManager::handleExit( const CEGUI::EventArgs& args	)
{
	Hide();
	return true;	
}

bool	KUiCityManager::handleSetRes( const CEGUI::EventArgs& args		)
{
	KUiCityResMgr::Show( enSUO_ContributeCityRes );
	return true;
}

bool	KUiCityManager::handleGetRes( const CEGUI::EventArgs& args		)
{
	KUiCityResMgr::Show( enSUO_DistillCityRes );
	return true;
}

bool	KUiCityManager::handleRepair( const CEGUI::EventArgs& args		)
{
/*	IUIMDLDataset* pCityOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( city_operation, &pCityOper ) )
	{
		return false;
	}
	CityOperParam tagCityOper;
	ZeroMemory( &tagCityOper, sizeof(CityOperParam) );	
	tagCityOper.eType = repair_city_building_oper;
	tagCityOper.nBuildID = d_CurBuildingID;
	pCityOper->updateRecord( repair_city_building_oper, &tagCityOper, sizeof(CityOperParam) );
*/
  return true;
}

bool	KUiCityManager::handleShowBaseInfo( const CEGUI::EventArgs& args	)
{
	hideAllPage();
//	ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/OkBtn")->show();
//	ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBaseInfoPage/CancelBtn")->show();
	d_CityInfoPage->show();
	return true;
}

bool	KUiCityManager::handleShowBuildingInfo( const CEGUI::EventArgs& args	)
{
	hideAllPage();
//	ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityBuildingInfoPage/OkBtn")->show();
	d_BuildingInfoPage->show();
	return true;
}

bool    KUiCityManager::handleEditTax( const CEGUI::EventArgs & args)
{
	KUiCityTaxEditer::Show(d_CityTaxRate);
	return true;
}

bool	KUiCityManager::handleBuilding( const CEGUI::EventArgs& args	)
{
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	if ( pArgs->window )
	{
		int* pId = (int*)pArgs->window->getUserData();
		if ( pId )
		{
			d_CurBuildingID = *pId;
		}
	}
	return true;
}

void	KUiCityManager::updateCityBaseInfo( CityInfoParam* pParam )
{
	if ( pParam == NULL )
	{
		return;
	}

	d_CityTaxRate = pParam->tagBaseInfo.nTaxRate;

	char szBuff[COMMON_CLIENT_MSG_LEN_128];
	d_CityInfoPage->getChild("TaharezLook/CityBaseInfo")->getChild("TaharezLook/CityBaseInfo/ZhuhouName")->setText( AnsiToUtf8( pParam->tagBaseInfo.szZhuhouName ) );
    d_CityInfoPage->getChild("TaharezLook/CityBaseInfo")->getChild("TaharezLook/CityBaseInfo/KingName")->setText( AnsiToUtf8( pParam->tagBaseInfo.szKingName ) );
	sprintf( szBuff, "%d", pParam->tagBaseInfo.nShizuCount );
	d_CityInfoPage->getChild("TaharezLook/CityBaseInfo")->getChild("TaharezLook/CityBaseInfo/ShizuCount")->setText( AnsiToUtf8( szBuff ) );
	sprintf( szBuff, "%d", pParam->tagBaseInfo.nProsonCount );
	d_CityInfoPage->getChild("TaharezLook/CityBaseInfo")->getChild("TaharezLook/CityBaseInfo/PersonCount")->setText( AnsiToUtf8( szBuff ) );

	sprintf( szBuff, "%d", pParam->tagBaseInfo.nCopperCount );
	d_CityInfoPage->getChild("TaharezLook/CityStorageInfo")->getChild("TaharezLook/CityStorageInfo/Copper")->setText( AnsiToUtf8( szBuff ) );
	sprintf( szBuff, "%d", pParam->tagBaseInfo.nFlixCount );
	d_CityInfoPage->getChild("TaharezLook/CityStorageInfo")->getChild("TaharezLook/CityStorageInfo/Flix")->setText( AnsiToUtf8( szBuff ) );    
	sprintf( szBuff, "%d", pParam->tagBaseInfo.nWoodCount );
	d_CityInfoPage->getChild("TaharezLook/CityStorageInfo")->getChild("TaharezLook/CityStorageInfo/Wood")->setText( AnsiToUtf8( szBuff ) );

    //int j,y,t;
	//sysMoneyToUiMoney( pParam->tagBaseInfo.nMoney, j, y, t );
	sprintf( szBuff, "%d", pParam->tagBaseInfo.nMoney );
	d_CityInfoPage->getChild("TaharezLook/CityStorageInfo")->getChild("TaharezLook/CityStorageInfo/Money_j")->setText( AnsiToUtf8( szBuff ) );    
	sprintf( szBuff, "%d", Zero );
	d_CityInfoPage->getChild("TaharezLook/CityStorageInfo")->getChild("TaharezLook/CityStorageInfo/Money_y")->setText( AnsiToUtf8( szBuff ) );    
	sprintf( szBuff, "%d", Zero );
	d_CityInfoPage->getChild("TaharezLook/CityStorageInfo")->getChild("TaharezLook/CityStorageInfo/Money_t")->setText( AnsiToUtf8( szBuff ) );    

	if ( ( NULL != d_CityInfoPage_CityDevelop ) && ( NULL != d_CityInfoPage_CityTired ) )
	{
		char numDevelopment[11] = { 0 };
		_snprintf( numDevelopment, sizeof( numDevelopment ), "%u", pParam->tagBaseInfo.nDevelopment );
		d_CityInfoPage_CityDevelop->setText( AnsiToUtf8( numDevelopment ) );

		char numTiredness[11] = { 0 };
		_snprintf( numTiredness, sizeof( numTiredness ), "%u", pParam->tagBaseInfo.nTiredness );
		d_CityInfoPage_CityTired->setText( AnsiToUtf8( numTiredness ) );
	}

	DWORD dwMapID=g_pCoreShell->GetGameData(GDI_MAP_ID,0,0);
    const KUiCfgLoader::CityResCfg &  cityResCfg= KUiCfgLoader::getSingleton().getCityResCfg();

	const KUiCfgLoader::CityImageCfg & cityImageCfg=KUiCfgLoader::getSingleton().getCityImageCfg();
	for (int i=0;i<4;i++)
	{
		if (dwMapID==cityImageCfg.CityMapID[i])
		{
            TLStaticImage * pImageWnd=(TLStaticImage *)d_CityInfoPage->getChild("TaharezLook/CityBaseInfo")->getChild("TaharezLook/CityBaseInfo/CityIcon");
			if (pImageWnd)
				pImageWnd->setImage(cityImageCfg.CityIConSet,cityImageCfg.CityImage[i]);
		}//endif

		if (dwMapID==cityResCfg.CityMapID[i])
		{
			if ( KUiCfgLoader::getSingleton().getCityResCfg().UseRes )
			{
				sprintf( szBuff, "%d", cityResCfg.CityResB[i] );
				d_CityInfoPage->getChild("TaharezLook/CityTreeInfo")->getChild("TaharezLook/CityTreeInfo/Copper")->setText( AnsiToUtf8( szBuff ) );
				sprintf( szBuff, "%d", cityResCfg.CityResF[i] );
				d_CityInfoPage->getChild("TaharezLook/CityTreeInfo")->getChild("TaharezLook/CityTreeInfo/Flix")->setText( AnsiToUtf8( szBuff ) );
				sprintf( szBuff, "%d", cityResCfg.CityResW[i] );
				d_CityInfoPage->getChild("TaharezLook/CityTreeInfo")->getChild("TaharezLook/CityTreeInfo/Wood")->setText( AnsiToUtf8( szBuff ) );   
			}
			else
			{
				d_CityInfoPage->getChild("TaharezLook/CityTreeInfo")->getChild("TaharezLook/CityTreeInfo/Copper")->setText( d_strNothing );
				d_CityInfoPage->getChild("TaharezLook/CityTreeInfo")->getChild("TaharezLook/CityTreeInfo/Flix")->setText( d_strNothing );
				d_CityInfoPage->getChild("TaharezLook/CityTreeInfo")->getChild("TaharezLook/CityTreeInfo/Wood")->setText( d_strNothing );   			
			}
		}//endif

		if (dwMapID == cityResCfg.CityMapID[i])
		{
			d_CityInfoPage->getChild("TaharezLook/CityMaintenanceInfo")->setText(AnsiToUtf8( cityResCfg.CityDesc[i]));
		}//endif
	}//end for i
}

void	KUiCityManager::updateBuildingInfo( CityInfoParam* pParam )
{
	if ( pParam == NULL )
	{
		return;
	}

	for ( int nIdx = 0; nIdx < BUILDING_PER_PAGE; ++nIdx )
	{
		char szBuf[COMMON_CLIENT_MSG_LEN_64];
		sprintf( szBuf, "TaharezLook/CityBuildingInfoPage/Building%d", nIdx );
		Window* pBar = d_BuildingInfoPage->getChild( szBuf );
		if ( pBar )
		{
			sprintf( szBuf, "%s%s", szBuf, "/icon" );
			TLGameObject* pOgj = (TLGameObject*)pBar->getChild( szBuf );
			TLGameObject::GameObject tagGO;
			tagGO.d_type = TLGameObject::item;		
			tagGO.d_gameobjectSet	= AnsiToUtf8( pParam->tagBuilding[nIdx].szBuildingImageSet );
			tagGO.d_gameobject		= AnsiToUtf8( pParam->tagBuilding[nIdx].szBuildingImage );
			tagGO.d_count			= 0;
			pOgj->setObject( tagGO );
			int* pId = (int*)pOgj->getUserData();
			*pId = pParam->tagBuilding[nIdx].nBuildID;

			sprintf( szBuf, "%s%s", szBuf, "/name" );
			pBar->getChild( szBuf )->setText( pParam->tagBuilding[nIdx].szBuildingName );

			sprintf( szBuf, "%s%s", szBuf, "/info" );
			pBar->getChild( szBuf )->setText( pParam->tagBuilding[nIdx].szBuildingInfo );
		}
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

template<> 
KUiCityResMgr* KUiWndSingleton<KUiCityResMgr>::ms_Singleton	= NULL;

KUiCityResMgr::KUiCityResMgr( const CEGUI::String& id_name ):
KUiWndSingleton<KUiCityResMgr>( id_name )
{

}


KUiCityResMgr::~KUiCityResMgr()
{
}

void KUiCityResMgr::Show( enSocialUnitOperation eOperType )
{
	ms_Singleton->eType = eOperType;
	KUiWndSingleton<KUiCityResMgr>::Show();
}

void KUiCityResMgr::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/OkBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityResMgr::handleOK, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/CancelBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityResMgr::handleExit, ms_Singleton));
//		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/CloseBtn")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiCityResMgr::handleExit, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/Copper")->setText("0");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/Flix")->setText("0");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/Wood")->setText("0");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/Money_j")->setText("0");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/Money_y")->setText("0");
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/CityResMgr/Money_t")->setText("0");
	}
}

bool KUiCityResMgr::handleOK( const CEGUI::EventArgs& args )
{
	CityOperParam tagCityOper;
	ZeroMemory( &tagCityOper, sizeof( CityOperParam ) );

	tagCityOper.tagCityRes.nCopperCount = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/CityResMgr/Copper")->getText() ) );
	tagCityOper.tagCityRes.nFlixCount = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/CityResMgr/Flix")->getText() ) );
	tagCityOper.tagCityRes.nWoodCount = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/CityResMgr/Wood")->getText() ) ) ;
	int j = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/CityResMgr/Money_j")->getText() ) );
	int y = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/CityResMgr/Money_y")->getText() ) ) ;
	int t = atoi( Utf8ToAnsi( m_pThisWnd->getChild("TaharezLook/CityResMgr/Money_t")->getText() ) );
	tagCityOper.tagCityRes.nMoney = uiMoneyToSysMoney( j, y, t );

	IUIMDLDataset* pCityOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pCityOper ) )
	{
		Hide();
		return false;
	}
    //Terrable methods
	TongOperParam tagTongParam;
    tagTongParam.nOperationID = eType;
	CityOperParam * pAddress=&tagCityOper;
	memcpy(tagTongParam.szTip ,&pAddress,sizeof(CityOperParam *));
	
	pCityOper->updateRecord( eType, &tagTongParam, sizeof( TongOperParam ) );
	
	tagTongParam.nOperationID = enSUO_ReqCityInfo;
	pCityOper->updateRecord( enSUO_ReqCityInfo, &tagTongParam,sizeof(TongOperParam));

	Hide();
	return true;
}

bool KUiCityResMgr::handleExit( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}
/*****************************************************************
 *****************************************************************/

#define MAX_TAX_RATE     100
#define MIN_TAX_RATE     80

template<> 
KUiCityTaxEditer* KUiWndSingleton<KUiCityTaxEditer>::ms_Singleton	= NULL;

KUiCityTaxEditer::KUiCityTaxEditer( const CEGUI::String& id_name ):
KUiWndSingleton<KUiCityTaxEditer>( id_name )
{
	d_count = NULL;
	d_Rate  = -1;
}


KUiCityTaxEditer::~KUiCityTaxEditer()
{

}

void KUiCityTaxEditer::Init()
{
	d_count = (TLEditbox*)m_pThisWnd->getChild("TaharezLook/Shuishou/Input");
	d_count->setIsOnlyNumber(true);

	m_pThisWnd->getChild("TaharezLook/Shuishou/Ok")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiCityTaxEditer::handleOK, this));
	m_pThisWnd->getChild("TaharezLook/Shuishou/Cancel")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiCityTaxEditer::handleExit, this));

	d_count->subscribeEvent(Editbox::EventTextChanged, Event::Subscriber(&KUiCityTaxEditer::handleAddjust, this));
	d_count->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiCityTaxEditer::handleKeyDown, this));
}

void KUiCityTaxEditer::Show( int nTaxRate )
{
	KUiWndSingleton<KUiCityTaxEditer>::Show();
    if (ms_Singleton && ms_Singleton->d_count && ms_Singleton->d_Rate==-1)
	{
	 	 ms_Singleton->d_Rate=nTaxRate;
	     String	newCount = iToString(nTaxRate);
         ms_Singleton->d_count->resetText(newCount);
	}//endif
}

bool KUiCityTaxEditer::handleOK( const CEGUI::EventArgs& args )
{
	String newCount = d_count->getText();
	int nCount = atoi(newCount.c_str());
	if (nCount>=MIN_TAX_RATE && nCount<=MAX_TAX_RATE)
	{
		g_pCoreShell->OperationRequest(GOI_SOCIETY_SET_CITY_TEX_RATE,nCount,0);
		d_Rate = nCount;
	}//endif

	Hide();
	return true;
}

bool KUiCityTaxEditer::handleExit( const CEGUI::EventArgs& args )
{
	Hide();
	return true;
}

bool KUiCityTaxEditer::handleAddjust(const CEGUI::EventArgs& args )
{
    String newCount = d_count->getText();
	//判断是否数字，把非数字字符去掉
	for(int i = 0; i < newCount.length(); i++)
	{
		int num = newCount[i];
		if(num < 48 || num > 57)
		{
			newCount.erase(i, 1);
			i--;
			continue;
		}
	}
	//判断是否超过总数
	int nCount = atoi(newCount.c_str());
	
	if(newCount.length() < 2 || nCount==10)
	{
        m_pThisWnd->getChild("TaharezLook/Shuishou/Ok")->setEnabled(false);
		return true;
	}
	else
        m_pThisWnd->getChild("TaharezLook/Shuishou/Ok")->setEnabled(true);

	if(nCount> MAX_TAX_RATE)
	{
		nCount = MAX_TAX_RATE;
	}//endif

	if (nCount< MIN_TAX_RATE)
	{
		nCount = MIN_TAX_RATE;
	}//endif
	
	newCount = iToString(nCount);
    d_count->resetText(newCount);
	
	return true;
}

bool KUiCityTaxEditer::handleKeyDown(const CEGUI::EventArgs& args )
{
    return true;
}