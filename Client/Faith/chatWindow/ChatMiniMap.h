#ifndef CHAT_MINI_MAP
#define CHAT_MINI_MAP
#include "chatWindow/ChatWndProc.h"
#include "KDDraw.h"
#define UPDATA_TIMER_ID 1333
#define UPDATA_DT_TIME   2000//大约两秒更新一次

#define MINIMAP_BUTTON_PATH_FIND_BTN_ID 655
#define MINIMAP_BUTTON_KEY_JINGLIN_BTN_ID 656
#define MINIMAP_BUTTON_CURRENT_MAP_BTN_ID 657
#define MINIMAP_BUTTON_BIGWORD_MAP_BTN_ID 658
#define MINIMAP_BUTTON_SHOW_PLAYER_BTN_ID 659
#define MINIMAP_BUTTON_FIND_TEAM_BTM_ID   660

#define CONTROL_PUSHED    1
#define SHIFT_PUSHED      2
class ChatMiniMap
{
public:
	ChatMiniMap();
	~ChatMiniMap();
	void Create(HWND hParent,DlgProcessFun pFun,int width,int height);
	void ProcessPaint(HWND hwnd,HDC hdc);
	void ProcessLButtonDown(POINT pt,int flags);
	void Show();
	void Hide();
	void AdjustWindow();
	void Update();
	void LoadResourceIni();
	static ChatMiniMap& GetSingle();
	bool GetUptata() { return isNeedUpdata;}
	void UpdateCmpt(){isNeedUpdata = false;}
	HWND GetHandle() { return hDlg;}
	void SetPaintPos(int dx,int dy){ cx = dx;cy = dy;}
	void CreateSurface();
	void ReleaseSurface();
//	LPDIRECTDRAWSURFACE GetSurface() const {return lpddsurface;}
	void BltToDc(HDC hdc );
	void BitBackBufferToSurface(int x,int y);
	int  GetPaintX()const {return cx;}
	int GetPaintY() const {return cy;}
	int GetPaintWidth()const {return paintWidh;}
	int GetPaintHeight() const {return paintHeight;}

public:
	ChatButton& GetPathFindBtn() { return pathFind;}
	ChatButton& GetKeyJinglinBtn() { return keyJinglin;}
	ChatButton& GetCurrentWordBtn() { return currentMap;}
	ChatButton& GetBigWordMapBtn() { return bigWordMap;}
	ChatButton& GetShowPlayer() {return showPlayer;}
	ChatButton& GetFindTeamBtn( ) { return findTeam; }

public:
	void   ProcessPathFindBtnDown();
	void   ProcessKeyJinglinBtnDown();
	void   ProcessCurrentMapBtnDown();
	void   ProcessBigWodMapBtnDown();
	void   ProcessPlayerShpwBtnDown();
	void   ProcessFindTeamBtnDown( );

	static bool isShowPlayer;

private:
	void InsertPositionInEditBox(MapPosInfo& mapInfo);
	HWND hDlg;
	int width;
	int height;
	int paintWidh;
	int paintHeight;
	int x,y;
	bool isNeedUpdata;
	int cx,cy;
	int dx,dy;
	int extraX;
	int extarY;
//	HBITMAP hBitmap ;
	int bkBitmapIdx;
	ChatPoint drawPoint;
//	LPDIRECTDRAWSURFACE lpddsurface;


	ChatButton pathFind;
	ChatButton keyJinglin;
	ChatButton currentMap;
	ChatButton bigWordMap;
	ChatButton showPlayer;
	ChatButton findTeam;

	//字体的属性
	int m_nFontHeight;
	int m_nFontWeight;
	int m_nFontPointY;
	int m_nFontExtra;
	DWORD m_dwFontColor;
};
#endif