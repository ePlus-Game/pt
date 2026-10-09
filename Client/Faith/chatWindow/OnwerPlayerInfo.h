#ifndef _OWNER_PLAYER_INFO_H
#define _OWNER_PLAYER_INFO_H

//////////////////////////
#define _PLAYER_CONTROL_STATE_NORMAL   0
#define _PLAYER_CONTROL_STATE_MOUSEOVER 1
#define _PLAYER_CONTROL_STATE_MOUSEDOWN 2
#define _PLAYER_CONTROL_STATE_LEFTLINE  3
#define _PLAYER_CONTROL_STATE_LEFTLINE_SELECT 4
#define _PLAYER_NAME_LEN               16
#define _PLAYER_METIER_LEN             16
#define _PLAYER_AT_WHERE_LEN           20
#define _PLAYER_GROUP_LEN              150
//////////////////////////
typedef struct OwnerRect 
{
	int x,y;
	int width,height;
}ONRECT,*LPONRECT;

#define _CHAT_PLAYER_INFO_TYP_FRIEND 0
#define _CHAT_PLAYER_INFO_TYP_ENEMY  1
#define _CHAT_PLAYER_INFO_TYP_PINGBI 2
#define _CHAT_PLAYER_INFO_TYP_ROOM   3
#define _CHAT_PLAYER_INFO_TYP_TEMP   4
typedef struct ChatPlayerInfo
{
	UCHAR   playerName[_PLAYER_NAME_LEN];
	int     playerLevel;
	UCHAR   playerMetier[_PLAYER_METIER_LEN];
	UCHAR   playerPlace[_PLAYER_AT_WHERE_LEN];
	UCHAR   PlayerSizhu[_PLAYER_GROUP_LEN];
	UCHAR   PlayerZhuhou[_PLAYER_GROUP_LEN];
	BOOL    isPlayerOnline;
	int     type;
	ChatPlayerInfo& operator  = ( const ChatPlayerInfo& info)
	{
		strcpy((char*)playerName,(char*)info.playerName);
		strcpy((char*)playerMetier,(char*)info.playerMetier);
		strcpy((char*)playerPlace,(char*)info.playerPlace);
		strcpy((char*)PlayerSizhu,(char*)info.PlayerSizhu);
		strcpy((char*)PlayerZhuhou,(char*)info.PlayerZhuhou);
		isPlayerOnline = info.isPlayerOnline;
		playerLevel = info.playerLevel;
		type = info.playerLevel;
		return *this;
	}
	ChatPlayerInfo(const ChatPlayerInfo& info)
	{
		strcpy((char*)playerName,(char*)info.playerName);
		strcpy((char*)playerMetier,(char*)info.playerMetier);
		strcpy((char*)playerPlace,(char*)info.playerPlace);
		strcpy((char*)PlayerSizhu,(char*)info.PlayerSizhu);
		strcpy((char*)PlayerZhuhou,(char*)info.PlayerZhuhou);
		isPlayerOnline = info.isPlayerOnline;
		playerLevel = info.playerLevel;
		type = info.playerLevel;
	}
	ChatPlayerInfo()
	{
		playerName[0] = 0;
		playerLevel = 0;
		strcpy((CHAR*)playerMetier,"----");
		strcpy((CHAR*)playerPlace,"----");
		strcpy((CHAR*)PlayerSizhu,"----");
		strcpy((CHAR*)PlayerZhuhou,"----");
		isPlayerOnline = FALSE;
		type = 0;
	}
}CHATPALYERINFO,*LPCHATPLAYERINFO;

#define _CONTROL_FONT_NAME_SIZE 256
#define _CONTROL_USED_IN_FRIEND 0
#define _CONTROL_USED_IN_CLAN 1

typedef class PlayerInfoControl
{
public:
	PlayerInfoControl();
	~PlayerInfoControl();

public:
	void PlayerControlCreate(int x,int y,int width,int height,int id,const char*font = 0);
	void PlayerControlDrawItem(HDC hdc,bool isUseDefaultFont);
	void PlayerControlDrawItem();
	void PlayerControlUpdate();
	void PlayerControlProcessLButtonDown();
	void PlayerControlProcessLButtonBLCLK();
	void PlayerControlProcessMouseMove();
	void PlayerControlProcessMouseLeave();
public:
	int PlayerControlGetState()const {return state;}
	const ONRECT& PlayerControlGetPosRect() {return posRect;}
	ONRECT& PlayerControlGetPosRectControl(){return posRect;}
	const CHATWNDSRC* PlayerControlGetSource() {return pSource;}
	BOOL   PlayerControlIsEnable() const {return enable;}
	COLORREF   PlayerControlGetOnlineFontColor(){return fontOnlineColor;}
	COLORREF   PlayerControlGetOnlineFontMouseOverColor() const { return fontOnlineMouseOverColor;}
	COLORREF   PlayerControlGetOnlineFontSelectColor() const { return fontOnlineSelectColor;}
	COLORREF   PlayerControlGetLeftlineFontSelectColor() const { return fontLeftSelectColor;}
	COLORREF   PlayerControlGetLeftlineFontColor() {return fontLeftLineColor;}
	void       PlayerControlSetOnlineFontColor(COLORREF onlineColor){ fontOnlineColor =onlineColor;}
	void       PlayerControlSetOnlineFontMouseOverColor(COLORREF onlineMouseOverColor){ fontOnlineMouseOverColor = onlineMouseOverColor;}
	void       PlayerControlSetOnlineFontSelectColor(COLORREF onlineSelectColor) { fontOnlineSelectColor = onlineSelectColor;}
	void       PlayerControlSetLeftlineFontSelectColor(COLORREF leftlineColor) { fontLeftSelectColor = leftlineColor;}
	void       PlayerControlSetLeftlineFontColor(COLORREF leftlineColor){ fontLeftLineColor = leftlineColor;}
	ChatPlayerInfo& PlayerControlGetPlayerInfo() {return playerInfo;}
	void PlayerControlSetArePartWidth(int* nameWidth,int* metierWidth,
		                              int* levelWidth);
	void PlayerControlSetSelected(BOOL selected,BOOL update);

public:
	void PlayerControlSetState(int state){this->state = state;}
	void PlayerControlEnable(int enable){this->enable = enable;}
	void PlayerControlSetFont(const char* pszFont){ strcpy(pszFontName,pszFont);}
	void PlayerControlSetSource(CHATWNDSRC* pSrc){pSource = pSrc;}
	void PlayerControlSetLeftSource(CHATWNDSRC* pLeftSrc){pLeftSource = pLeftSrc;}
	void PlayerControlSetParent(HWND hwnd){hParentHandle = hwnd;}
	BOOL PlayerControlIsSelected() { return IsSelected;}

protected:
	///////控件的状态//////
	int state;
	//////////////
	int id;
	////控件的大小和位置//////
	ONRECT posRect;
	////控件的资源//////////
	CHATWNDSRC* pSource;
	CHATWNDSRC* pLeftSource;
	/////控件是否可用///////
	BOOL   enable;
	////控件字体////////
	char  pszFontName[_CONTROL_FONT_NAME_SIZE];
	COLORREF fontOnlineColor;
	COLORREF fontOnlineMouseOverColor;
	COLORREF fontOnlineSelectColor;
	COLORREF fontLeftSelectColor;
	COLORREF fontLeftLineColor;
	//////////////////
	HWND hParentHandle;
	BOOL IsSelected;
	////////各个部分所占的宽度////////////////
	
	int* nameItemWidth ;
	int* metierItemWidth ;
	int* levelItemWidth ;
//	int* groupItemWidth ;
//	int* placeItemWidth ;
	////////////////////////////
	ChatPlayerInfo playerInfo;
}PLAYERCONTROL,*LPPLAYERCONTROL;

void DrawTextItem(HDC hdc,COLORREF color,int x,int y,int width,int height,const char* fontName,const char* pText,COLORREF lineColor,BOOL isUseDefualFont,BOOL isDrawLine = TRUE);

#endif