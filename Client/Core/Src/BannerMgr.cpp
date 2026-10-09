
#include "KCore.h"

#include "BannerMgr.h"
#include "CoreRelated.h"
#include "KPlayer.h"

#ifdef _SERVER
ShizuBannerMgr::ShizuBannerMgr()
{

}

ShizuBannerMgr::~ShizuBannerMgr()
{

}

ShizuBannerMgr& ShizuBannerMgr::getSingleton()
{
	static ShizuBannerMgr singleton;
	return singleton;
}

bool ShizuBannerMgr::setBanner(int bannerIndex, Style newStyle)
{
	if(bannerIndex < 0 || bannerIndex >= CORE_BANNER_MGR_BANNER_COUNT)
	{
		return false;
	}

	_banner[bannerIndex] = newStyle;
	return true;
}

void ShizuBannerMgr::showBannerToPlayer(int playerIndex)
{
	if(!IsValidPlayer(playerIndex))
	{
		return;
	}

	for(int i = 0; i < CORE_BANNER_MGR_BANNER_COUNT; ++i)
	{
		showBannerToPlayer(playerIndex, i);
	}
}

void ShizuBannerMgr::showBannerToPlayer(int playerIndex, int bannerIndex)
{
	if(!IsValidPlayer(playerIndex))
	{
		return;
	}

	if(bannerIndex < 0 || bannerIndex >= CORE_BANNER_MGR_BANNER_COUNT)
	{
		return;
	}

	Style& curBanner = _banner[bannerIndex];

	char sendBuff[COMMON_SHOW_BANNER_BUFF_LENGTH];
	int sendSize = PrepareShowBannerBuff(sendBuff, sizeof(sendBuff), curBanner.text, sizeof(curBanner.text), NULL, 0, 0, bannerIndex, 1, 2);
	
	KPlayer& player = Player[playerIndex];
			
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(player.GetNetConnectIdx(), sendBuff, sendSize);
}

#endif