//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 01/08/2007 14:44
//      File_base        : UiCityManager
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UICITYMANAGER_H
#define UICITYMANAGER_H

#define BUILDING_PER_PAGE 10

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "SocialComDef.h"
#include "TLGameObject.h"
#include "TLEditbox.h"

class KUiCityManager : public KUiWndSingleton<KUiCityManager>
{
public:
    KUiCityManager( const CEGUI::String& id_name );
    ~KUiCityManager(								);
public:
	virtual void	onCreate	( UIMDLEvent& rEvent	);
	virtual void	onRelease	( UIMDLEvent& rEvent	);
	virtual void	onChange	( UIMDLEvent& rEvent	);
	static void		Show		( void					);
	void			Init		( void					);

private:
	
	bool	handleExit( const CEGUI::EventArgs& args	);
	bool	handleSetRes( const CEGUI::EventArgs& args		);
	bool	handleGetRes( const CEGUI::EventArgs& args		);
	bool	handleRepair( const CEGUI::EventArgs& args		);
	bool	handleShowBaseInfo( const CEGUI::EventArgs& args	);
	bool	handleShowBuildingInfo( const CEGUI::EventArgs& args	);
	bool	handleBuilding( const CEGUI::EventArgs& args	);
	bool    handleEditTax( const CEGUI::EventArgs & args);
	void	updateCityBaseInfo( CityInfoParam* pParam );
	void	updateBuildingInfo( CityInfoParam* pParam );
	void	hideAllPage( void );
private:
	CEGUI::Window*	d_CityInfoPage;
	CEGUI::Window*	d_BuildingInfoPage;
	int				d_BuildID[BUILDING_PER_PAGE];
	int				d_CurBuildingID;
	int             d_CityTaxRate;

	TLStaticText*	d_CityInfoPage_CityDevelop;
	TLStaticText*	d_CityInfoPage_CityTired;
	String			d_strNothing;
};

class KUiCityResMgr : public KUiWndSingleton<KUiCityResMgr>
{
public:
   KUiCityResMgr( const CEGUI::String& id_name );
    ~KUiCityResMgr(								);
public:
	static void		Show( void ) {};
	static void		Show( enSocialUnitOperation eOperType  );
	void			Init		( void					);
protected:
	bool	handleExit( const CEGUI::EventArgs& args	);
	bool	handleOK( const CEGUI::EventArgs& args		);
private:
	enSocialUnitOperation	eType;
};

class KUiCityTaxEditer: public KUiWndSingleton<KUiCityTaxEditer>
{
public:
	KUiCityTaxEditer( const CEGUI::String& id_name );
    ~KUiCityTaxEditer(								);
public:
	static void		Show( int nTaxRate );
	void			Init		( void					);
protected:
	bool	        handleExit( const CEGUI::EventArgs& args	);
	bool	        handleOK( const CEGUI::EventArgs& args		);
	bool            handleAddjust( const CEGUI::EventArgs& args );
	bool            handleKeyDown( const CEGUI::EventArgs& args );
private:
    TLEditbox     * d_count;
	int             d_Rate;
};

#endif