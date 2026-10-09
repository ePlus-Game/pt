//ÊÏ×åbanner ui xiehong-2007-11-29

#ifndef UI_SHIZU_BANNER
#define UI_SHIZU_BANNER

#include "TLStatic.h"

#define UI_SHIZU_BANNER_MAX_COUNT	6
#define UI_SHIZU_BANNER_WINDOW_PATH_1024	"uisettings/layouts1024/ShizuBanner.ls"
#define UI_SHIZU_BANNER_WINDOW_PATH	"uisettings/layouts/ShizuBanner.ls"
#define UI_SHIZU_BANNER_INI_FILE_PATH  "UiSettings/uicfg.ini"

class KUiShizuBanner
{
	TLStaticImage*	_thisWindow;
	TLStaticText*	_msg[UI_SHIZU_BANNER_MAX_COUNT];

	TLStaticText *	m_Pronunciamento;

	int		m_shizuBannerPosX;
	int		m_shizuBannerPosY;
	int		m_jiugongPosX;
	int		m_jiugongPosY;
public:
	KUiShizuBanner();
	~KUiShizuBanner();
	
	static KUiShizuBanner& getSingleton();

	void	loadUi();
	void	showBanner(int index, char* bannerText);
	void	clean();
	void	ShowPronunciamento(char * msg, int msgLen);
	void	Hide();
	void	SetBannerPos(int & x, int & y);
};

#endif