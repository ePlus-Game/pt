#include "UiTrafficLight.h"
#include "../UiConfigManager.h"
#include "../UiSheetMgr.h"
#include "CoreShell.h"

extern iCoreShell* g_pCoreShell;

#define LIGHTNAME_NEWMAIL "NewMail"

KUiTrafficLight::KUiTrafficLight()
{
	_lightName = "";
	_imageCtrl = NULL;
	_tipText = "";
	_tipPos = KUiItemTip::Left;
}

KUiTrafficLight::~KUiTrafficLight()
{

}

void KUiTrafficLight::create()
{
	_lightName = "";
	
	static int id = 0;
	char prefix[COMMON_CLIENT_MSG_LEN_8];
	sprintf(prefix, "traff%d", id++);
	_imageCtrl = (TLStaticImage*)WindowManager::getSingleton().createWindow("TaharezLook/StaticImage", prefix);
	_imageCtrl->subscribeEvent(Window::EventMouseEnters, Event::Subscriber(&KUiTrafficLight::onMouseEnters, this));
	_imageCtrl->subscribeEvent(Window::EventMouseMove, Event::Subscriber(&KUiTrafficLight::onMouseMove, this));
	_imageCtrl->subscribeEvent(Window::EventMouseLeaves, Event::Subscriber(&KUiTrafficLight::onMouseLeaves, this));
	_imageCtrl->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiTrafficLight::onMouseClick, this));
	
	_imageCtrl->setDummyWnd(true);
	_imageCtrl->setRenderMode(true);
	_imageCtrl->setFrameEnabled(false);
	_imageCtrl->setBackgroundEnabled(false);
	_imageCtrl->hide();
	
	_imageCtrl->setZLevel(Window::Bottom);
	_imageCtrl->SetBottomWindow();
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_imageCtrl);
}

void KUiTrafficLight::destory()
{
	_imageCtrl->destroy();
}

void KUiTrafficLight::setProperty(const char* imagePath, Position pos, bool isCyc, const char* tipText, KUiItemTip::TipPos tipPos)
{
	const Image* image = getImage(imagePath);
	_imageCtrl->setWidth(Absolute, image->getWidth());
	_imageCtrl->setHeight(Absolute, image->getHeight());
	_imageCtrl->setImage(image);
	if ( LIGHTNAME_NEWMAIL == _lightName )
	{
		_imageCtrl->setDummyWnd(false);
	}

	Point imagePos(pos.x, pos.y);
	_imageCtrl->setPosition(Absolute, imagePos);
	_imageCtrl->setCyc(isCyc);
	
	_tipText = tipText;
	_tipPos = tipPos;
}

void KUiTrafficLight::lightup()
{
	_imageCtrl->setCycCount(5);
	_imageCtrl->play();
}

void KUiTrafficLight::terminate()
{
	_imageCtrl->hide();
}

bool KUiTrafficLight::onMouseEnters(const EventArgs& e)
{
	showTip();
	
	return true;
}

bool KUiTrafficLight::onMouseLeaves(const EventArgs& e)
{
	KUiItemTip::Hide();
	return true;
}

bool KUiTrafficLight::onMouseMove(const EventArgs& e)
{
	if(KUiItemTip::IsVisible() == false)
		showTip();
	
	return true;
}

bool KUiTrafficLight::onMouseClick( const EventArgs& e )
{
	if ( LIGHTNAME_NEWMAIL == _lightName )
	{
 		KUiSceneTimeInfo mapInfo;
 		ZeroMemory(&mapInfo, sizeof(KUiSceneTimeInfo));
 		g_pCoreShell->SceneMapOperation( GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
		int mailNpcIndex = 0;
		
		switch (mapInfo.nSceneId)
		{
		case 1:	//北海
			{
				mailNpcIndex = KUiCfgLoader::getSingleton().getMailCfg().MailNpcIndex_Beihai;
			}
			break;
		case 2:	//昆仑
			{
				mailNpcIndex = KUiCfgLoader::getSingleton().getMailCfg().MailNpcIndex_Kunlun;
			}
			break;
		case 3:	//九黎
			{
				mailNpcIndex = KUiCfgLoader::getSingleton().getMailCfg().MailNpcIndex_Jiuli;
			}
			break;
		case 4:	//朝歌
			{
				mailNpcIndex = KUiCfgLoader::getSingleton().getMailCfg().MailNpcIndex_Chaoge;
			}
			break;
		}

		if ( 0 != mailNpcIndex )
		{
			g_pCoreShell->OperationRequest(GOI_GOTO_MAILCENTRE, mailNpcIndex, 0);
		}
	}

	return true;
}

void KUiTrafficLight::showTip()
{
	KUiItemTip::GetSingleton().show(const_cast<char*>(_tipText.c_str()), _imageCtrl->getUnclippedPixelRect(), _tipPos);
}



KUiTrafficLightManager::KUiTrafficLightManager()
{
	const vector<KUiCfgLoader::TrafficLight>& trafficLightList = KUiCfgLoader::getSingleton().getTrafficLightCfg();

	for(int i = 0; i < UI_TRAFFICLIGHT_MAX_COUNT; ++i)
	{
		_lights[i].create();
	}

	for(int j = 0; j < trafficLightList.size(); ++j)
	{
		const KUiCfgLoader::TrafficLight& trafficLight = trafficLightList[j];

		KUiTrafficLight* aLight = createALight(trafficLight.type);
		if(aLight == NULL)
		{
			continue;
		}
		aLight->setProperty(trafficLight.imagePath, trafficLight.pos, trafficLight.imageCyc, trafficLight.tipText, trafficLight.tipPos);
		//aLight->lightup();
	}
}

KUiTrafficLightManager::~KUiTrafficLightManager()
{
	for(int i = 0; i < UI_TRAFFICLIGHT_MAX_COUNT; ++i)
	{
//		_lights[i].destory();
	}
}

KUiTrafficLight* KUiTrafficLightManager::find(string lightName)
{
	for(int i = 0; i < UI_TRAFFICLIGHT_MAX_COUNT; ++i)
	{
		if(lightName == _lights[i]._lightName)
		{
			return &_lights[i];
		}
	}
	return NULL;
}

KUiTrafficLight* KUiTrafficLightManager::createALight(string lightName)
{
	//如果已经存在则创建不成功
	if(find(lightName) != NULL)
	{
		return NULL;
	}

	for(int i = 0; i < UI_TRAFFICLIGHT_MAX_COUNT; ++i)
	{
		if("" == _lights[i]._lightName)
		{
			_lights[i]._lightName = lightName;
			return &_lights[i];
		}
	}
	return NULL;
}

bool KUiTrafficLightManager::destoryALight(string lightName)
{
	for(int i = 0; i < UI_TRAFFICLIGHT_MAX_COUNT; ++i)
	{
		if(lightName == _lights[i]._lightName)
		{
			_lights[i]._lightName = "";
			return true;
		}
	}
	return false;
}

void KUiTrafficLightManager::closeAll()
{
	for(int i = 0; i < UI_TRAFFICLIGHT_MAX_COUNT; ++i)
	{
		_lights[i].terminate();
	}
}