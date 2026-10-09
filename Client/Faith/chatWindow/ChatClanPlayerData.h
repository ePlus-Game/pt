#ifndef _CHATCLANPLAYERDATA_H_
#define _CHATCLANPLAYERDATA_H_
#endif

#define _CLAN_MAX_MEMBER_NUM 10

#define WM_DATA_REQUEST_SUCCEED (WM_USER + 10)
#define WM_CLAN_DATA_REQUEST_SUCCEED (WM_USER + 11)

class ChatClanPlayerData : public IUIMDLEvent
{
public:
	ChatClanPlayerData();
	~ChatClanPlayerData();
	BOOL InitInterface();
	void RequestDataList(HWND callWnd, int layerID, FSGUID * id = NULL);
	void onCreate(UIMDLEvent& rEvent);
	void onRelease(UIMDLEvent& rEvent);
	void onChange(UIMDLEvent& rEvent);
	void AddMember(const char * name, HWND callWnd, int layerID);
	void ModifyAnnoucement(const char * text, int layerID);
	void GetAnnoucement(int layerID);
	void DeleteMember(FSGUID guid, HWND callWnd, int layerID);
	void ForbidChat(FSGUID guid, int layerID);
	void UnforbidChat(FSGUID guid, int layerID);
	void Demise(FSGUID guid, int layerID);
	static ChatClanPlayerData & GetPlayerData();
public:
	IUIMDL * m_pInterface;
	IUIMDLDataset * m_pDataset;
	char m_pClanName[CLIENT_NAME_AND_TITLE_MAX];
	char m_ClanAnnoucement[COMMON_CLIENT_MSG_LEN_512];
	char m_LuedAnnoucement[COMMON_CLIENT_MSG_LEN_512];
	TongPageData m_DataList[TONGMEMBER_MAX_NUM];
	int m_iPlayerNum;
	HWND m_hCallWnd;
	HWND m_hGetListWnd;
};