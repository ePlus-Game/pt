#ifndef CHAT_PLAYER_BASE_INFO_DLG
#define CHAT_PLAYER_BASE_INFO_DLG
#include "GameDataDef.h"
struct TextInfo_
{
	int x;
	int y;
	char info[64+1];
	int value;
	DWORD color;
};

bool operator == (const KUiPlayerBaseInfo& info1,const KUiPlayerBaseInfo& info2);
bool operator == (const KUiPlayerRuntimeInfo& info1,const KUiPlayerRuntimeInfo&info2);
bool operator == (const KUiPlayerAttribute& info1,const KUiPlayerAttribute&info2);
#define CHATPLAYERSHOW_DLG_TIMER_ID 456
#define CHATPLAYERSHOW_DLG_TIMER_DT 5000
#define LIFE_SHOW_BUTTON_ID   555
#define MANA_SHOW_BUTTON_ID   556
#define EXP_SHOW_BUTTON_ID    557
class  ChatPlayerBaseInfoDlg
{
public:
	ChatPlayerBaseInfoDlg();
	~ChatPlayerBaseInfoDlg();
	void Create(HWND hwnd);
	void LoadSrcINI();
	void ProcessPaint(HWND hwnd ,HDC hdc);
	void Show();
	void Hide();
	void AdjustWindow();
	void SetTimer();
	static ChatPlayerBaseInfoDlg& GetSingle() ;

public:
	HWND  GetHandle() { return hDlg;}
	KUiPlayerAttribute& GetAttribute() {return playerAttr;}
	KUiPlayerBaseInfo& GetBaseInfo() { return baseInfo;}
	KUiPlayerRuntimeInfo& GetRunTimeInfo(){return runTimeInfo;}
	SOCIETY_INFO&    GetSocielTY() { return shehuiInfo;}
	void           DrawLifeShowControl(HDC hdc,HBITMAP hBkBitmap,HBITMAP hCurrentBitmap);
	void           DrawManaShowControl(HDC hdc,HBITMAP hBkBitmap,HBITMAP hCurrentBitmap);
	void           DrawExpShowControl(HDC hdc,HBITMAP hBkBitmap,HBITMAP hCurrentBitmap);
	void           UpdateExpShowTip();
	ChatButton&    GetLifeShowControl()  { return lifeShow;}
	ChatButton&    GetManaShowControl()  { return manaShow;}
	ChatButton&    GetExpShowControl()   { return expShow;}
private:
	HWND hDlg;
	int x;
	int y;
	int width;
	int height;
	int bkBitmapIdx;

	TextInfo_ name_a;
	TextInfo_ zhiye_a;
	TextInfo_ dengji_a;
	TextInfo_ sizhu_a;
	TextInfo_ zhuhou_a;
	TextInfo_ life_a;
	TextInfo_ magic_a;
	TextInfo_ exp_a;
	char  fontName[64+1];
	bool  isUseDefFont;

	KUiPlayerBaseInfo baseInfo;
	KUiPlayerRuntimeInfo runTimeInfo;
	KUiPlayerAttribute playerAttr;
	SOCIETY_INFO      shehuiInfo;

	//
	ChatButton lifeShow;
	ChatButton manaShow;
	ChatButton expShow;
	

//	int 


};
#endif