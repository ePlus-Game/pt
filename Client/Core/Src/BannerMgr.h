//bannerπ‹¿Ì∆˜ xiehong-2007-11-29


#ifndef CORE_BANNER_MGR
#define CORE_BANNER_MGR

#include "CoreUseNameDef.h"

#ifdef _SERVER

#define CORE_BANNER_MGR_BANNER_COUNT 6

class ShizuBannerMgr
{
public:
	struct Style
	{
		char	text[COMMON_CLIENT_MSG_LEN_256];
		Style()
		{
			text[0] = 0;
		}
		Style(const Style& other)
		{
			strncpy(text, other.text, COMMON_CLIENT_MSG_LEN_256);
			text[COMMON_CLIENT_MSG_LEN_256 - 1] = 0;
		}
	};

private:
	Style	_banner[CORE_BANNER_MGR_BANNER_COUNT];

public:
	ShizuBannerMgr();
	~ShizuBannerMgr();
	
	static ShizuBannerMgr& getSingleton();

	bool	setBanner(int bannerIndex, Style newStyle);
	void	showBannerToPlayer(int playerIndex);
	void	showBannerToPlayer(int playerIndex, int bannerIndex);
};

#endif

#endif