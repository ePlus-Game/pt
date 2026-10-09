//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 7/28/2007
//      File_base        : KUiTrafficLight
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 通用状态显示管理器
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////


#ifndef UITRAFFICLIGHT_H
#define UITRAFFICLIGHT_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "UiItemTip.h"
#include "GameDataDef.h"
#include <string>

#define UI_TRAFFICLIGHT_MAX_COUNT 20

using namespace std;

class KUiTrafficLight
{
	friend class KUiTrafficLightManager;
	
	string						_lightName;
	TLStaticImage*				_imageCtrl;
	string						_tipText;
	KUiItemTip::TipPos			_tipPos;

private:
	KUiTrafficLight();
	~KUiTrafficLight();
	
	bool	onMouseEnters(const EventArgs& e);
	bool	onMouseLeaves(const EventArgs& e);
	bool	onMouseMove(const EventArgs& e);
	bool	onMouseClick(const EventArgs& e);

	void	showTip();

	void	create();
	void	destory();
public:
	void	setProperty(const char* imagePath, Position pos, bool isCyc, const char* tipText, KUiItemTip::TipPos tipPos);

	void	lightup();
	void	terminate();
};

class KUiTrafficLightManager
{
	KUiTrafficLight _lights[UI_TRAFFICLIGHT_MAX_COUNT];
public:
	KUiTrafficLightManager();
	~KUiTrafficLightManager();

	KUiTrafficLight*	find(string lightName);
	void				closeAll();
	KUiTrafficLight*	createALight(string lightName);
	bool				destoryALight(string lightName);

	static KUiTrafficLightManager& getSinglton()
	{
		static KUiTrafficLightManager singlton;
		return singlton;
	}
	
};

#endif