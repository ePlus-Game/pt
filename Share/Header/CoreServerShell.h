/*****************************************************************************************
//	外界访问服务版Core的接口方法定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-20
------------------------------------------------------------------------------------------
	外界（如界面系统）通过此接口从Core获取游戏世界数据。
*****************************************************************************************/

#ifndef CORESERVERSHELL_H
#define CORESERVERSHELL_H

struct IProcRet;
struct QuestionInstance;

//=========================================================
// Core外部客户对core的操作请求的索引定义
//=========================================================
enum SERVER_SHELL_OPERATION_INDEX
{
	SSOI_LAUNCH = 1,				//启动服务
	SSOI_SHUTDOWN,					//关闭服务
};

//=========================================================
// Core外部客户向core获取游戏数据的数据项内容索引定义
//=========================================================
//各数据项索引的相关参数uParam与nParam如果在注释中未提及，则传递定值0。
//如果特别指明返回值含义，则成功获取数据返回1，未成功返回0。
enum GAMEDATA_INDEX
{
	SGDI_CHARACTER_NAME,
};

class iCoreServerShell
{
public:

	void Init( int nMaxPlayer );
	void ClientDisconnect(int nIndex);
	void RemoveQuitingPlayer(int nIndex);
	void RemovePlayer( int nIndex );
	bool IsCharacterQuiting(int nIndex);
	bool IsCanRemove( int nIndex );
	bool CheckProtocolSize(const char* pChar, int nSize);
	void ProcessClientMessage(int nIndex, const char* pChar, int nSize);
	void ProcessGuardMessage(const char* pMsg, int nSize);
	void ProcessPaysysMessage(int nIndex, const char* pChar, int nSize);
	

	int CreateNewRoleData(
		char	szAccName[],
		char	szName[],
		int		nSeries,
		int		nSex,
		int		nMapID,
		int		nImageHead,
		unsigned long ulNetID );
	
	int DbOpComplete(
		int nPlayerIndex,
		IProcRet* pRet );

	int  AttachPlayer(const unsigned long lnID, GUID* pGuid);
	void AddPlayerToWorld(int nIndex);
	int  AddCharacter(
		const tagExtPointInfo& cExtPointInfo, 
		const tagExtPointInfo& cChangeExtPointInfo, 
		char* szAccName,
		char* szRoleName,
		unsigned long ulNetID,
		GUID* pGuid );

	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam);
	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam);
	int  Breathe();
	void Release();
	bool IsTextPass(const char *szText);
	bool IsNamePass(const char *szText);

	void FSEyeCfgInit( );
	const KBASICPROP_ITEM* FSEyeGetItem( 	
		IN int nGenre,
		IN int nDetailType,
		IN int nParticularType,
		IN int nLevel  );

	bool GetRandomQuestion(
		BYTE* pProtocolBuff,
		int& protocolBuffSize,
		BYTE* pAnswerBuff,
		int& answerBuffSize);

private:
	int	 OnLunch(LPVOID pServer, LPVOID pController);
	int	 OnShutdown();
};


#ifndef CORE_EXPORTS

#ifndef __linxu
	extern "C" 
#endif
	iCoreServerShell* CoreGetServerShell();

#endif

#endif
