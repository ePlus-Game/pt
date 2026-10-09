///////////////////////////////////////////////////////////////
//	文 件 名 : GameVideo.h
//	文件功能 :
//	作    者 : chenshanglin
//	创建时间 : 2005年9月24日
//	备    注 :
//	历史记录 : 
//  录像格式 :	录像头部 || 录像内容 || 用户配置文件 || 注释 || 注释索引  (2005-11-19)
//				录像头部 || 录像内容 || 用户配置文件 || 注释 (2005-11-19)
///////////////////////////////////////////////////////////////
#ifndef _GameVideo_h_slchen
#define _GameVideo_h_slchen

#include <cstdio>
#include <fstream>
#include <ctime>
#include <map>
#include <io.h>

using namespace std;

class GameVideo
{
private:
	// 播放录像时需要保持消息响应的按钮坐标
	typedef struct tagButtonPos
	{
		short int	snLeft;
		short int	snTop;
		short int	snRight;
		short int	snBottom;

	} ButtonPos, *PButtonPos;

	typedef struct tagVideoHeader
	{
		// 指令数、录像时间、录像长度、注释次数、录像时分辨率、版本号
		DWORD		dwCRC;
		int			nInsNum;
		int			nVideoTimeLen;
		int			nVideoLen;
		int			nNotesLen;
		int			nNoteNum;
		int			nUiSettingLen;
		short int	snWidth;
		short int	snHeight;
		BYTE		bVersion;

	} VideoHeader, *PVideoHeader;

	// 用于存储带CRC校验的数据块头部
	typedef struct tagCRCDataLen
	{
		DWORD	dwCRC;						// 数据块的CRC校验(数据块不包括dwDataLen)
		DWORD	dwDataLen;					// 数据块的长度

	} CRCDataLen, *PCRCDataLen;

	typedef multimap<DWORD, DWORD>		NoteIndex;	// 注释的索引，指令和注释ID
	typedef map<DWORD, BYTE*>			Note;		// 注释ID和注释

public:
	enum
	{
		// 网络数据类型，将保存到指令文件中
		enum_NDT_GateWayAndGameServer = 1,			// 特殊类型，任何操作码都不允许为 1 
		enum_NDT_GateWay = 2,
		enum_NDT_GameServer = 3,
		enum_NDT_NULL = 4,								// 表示GameLoop过程中没有保存网络数据
	
		// 录像指令码，将保存到指令文件中
		// 录像指令可能和网络数据类型相连，所以指令码不能和网络数据类型的值相同
		enum_InsCode_PlayerInput = 5,
		enum_InsCode_GameLogic = 6,    
		enum_InsCode_CoreBreathe = 7,
		enum_InsCode_UiPaint = 8,

		// NPC Index 用来标识接下来的数据是NPC index
		// NPC Index 一般会夹在两条指令之间，所以
		// 这个标识不能和指令码相同
		enum_Type_NpcIndex = 9,

		// 因为窗口的显示和时间相关，只有过了指定的时间间隔
		// 才会绘制窗口，如果我们的逻辑执行过快，会导致窗口
		// 没有被正确的绘制到指定的位置，从而导致点击关闭窗口
		// 时出现问题，所以这里决定把定时器被调用几次记录下来
		// 这里目前针对 UiImage.cpp 中的
		// IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer)
		enum_Type_TimePassed = 10,

		// 国战时建筑物的建造进度，避免快放时进度条跟不上
		enum_Type_BuildingSchedule = 11,

		// 国战主进度条
		enum_Type_MainSchedule = 12, 
	};

	// 保存数据的操作类型，只是用来控制保存操作，不存入指令文件
	enum
	{
		enum_SaveOpe_PlayerInput,
		enum_SaveOpe_NetData,
		enum_SaveOPe_NetDataCode,
		enum_SaveOpe_CoreBreathe,
		enum_SaveOpe_UiPaint,
		enum_SaveOpe_NpcIndex,
		enum_SaveOpe_TimePassed,
		enum_SaveOpe_BuildingSchedule,
		enum_SaveOpe_MainSchedule,
	};

	enum
	{
		enum_InsCode_Size = 1,			// 指令码存储大小(字节)
		enum_NDT_Size = 1,				// 网络数据类型存储大小(字节)
		enum_NpcIndexFlag_Size = 1,		// npc 标记存储大小
		enum_TimePassedFlag_Size = 1,	

		enum_Null_Loop = -1,		

		BUF_SIZE = 0x10000,
	
		// 压缩的时候是每64k压缩成一块，播放的时候，每一块解压后也是64k
		// 但是有可能执行某条指令时，所需要的所有参数并不全部在当前解压
		// 后的缓冲区中，这时候我们必须要读入下一块压缩块，并把它解压到
		// 播放缓冲中，这样的话，播放缓冲区的大小就会超过64k，所以这里
		// 把它的大小扩充12k
		PLAY_BUF_SIZE = 0x11000,

		SINGLE_NOTE_SIZE = 0x400,		// 单条注释的长度(播放时用于存放读出来的注释数据)

		DEFAULT_SPEED_FRE = 10,			// 快进频率，每多少秒(正常播放时间)显示一帧

		ACTIVE_BUTTON_NUM = 6,
	};

	enum PlayError
	{
		enum_Play_Error = -1,			// 无法恢复的错误
		enum_Play_OK = 0,				// 播放成功
		enum_Play_SizeError = 1,		// 分辨率不对
		enum_File_CRCError	= 2,		// CRC校验失败
		enum_Note_Error	= 3,			// 注释错误
		enum_Create_Config_Error =4,	// 创建配置文件错误
	};
	
	enum VideoState
	{
		enum_State_Idle,				// 空闲
		enum_State_Video,				// 录像
		enum_State_Speed,				// 快进
		enum_State_NormalPlay,			// 播放
		enum_State_FastPlay,			// 快速播放
		enum_State_TempStopPlay,		// 暂停播放
		enum_State_PlayFinished,		// 播放完毕
		enum_State_Edit,				// 编辑录像，释放所有消息
		enum_State_FreeMsg,				// 在按下停止后，需要释放消息，以响应用户的选择
	};

public:
	GameVideo();	
	~GameVideo();

	bool	SaveOperation(int nArgNum, int nOpeCode, bool bSaveOpeCode, ...);
	bool	SaveVideoFile(const char *szFilePath = NULL);

	int		PreparePlayingVideo(const char *szFilePath);
	
	BYTE*	GetNetPackage(size_t &uSize, int &nDataType);
	int		GetNpcIndex();
	int		GetBuildingSchedule();
	void	GetMainSchedule(DWORD &dwPassed, DWORD &dwOffset);
	int		GetTimePassed();
	bool	TestFlagData(int nFlagData, int nDataSize);
	int		GetMsg(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

	int		GetState();
	bool	IsPlaying();
	bool	IsVideoing();
	bool	IsSpeed();
	BOOL	IsPlayFinished();
	
	bool	Video();
	void	StopVideo();
	void	Play();
	void	Pause();
	void	StopPlay();
	void	FastPlay();
	void	Speed();
	void	EditVideo();

	bool	AddNote(BYTE *pNotes, int nNotesLen);
	bool	SaveNote();

	int		GetCurPlayTime();
	int		GetTotalVideoLen();
	int		GetCurVideoLen();

	int		GetWidth();
	int		GetHeight();
	
	bool	VideoInit();
	BOOL	IsEditing();
	void	SetInputPasswordFlag(BOOL bFlag);
	BOOL	IsInputPassword();
	void	FreeMsg();

	//	用于在播放结束，界面关闭窗口时清空状态，因为enum_State_Finished状态
	//  会有副作用，影响rainbow发包，导致用户不能进入游戏
	void	ReturnToIdle();
	BOOL	HasPlayedVideo();
	void	SetSaveLoginChoiesFlag(BOOL bFlag);
	BOOL	GetSaveLoginChoiesFlag();
	BOOL	LoadInitialConfig(const char *szUiConfigFile);
	BYTE*	GetNote(DWORD dwNoteID);
	void	DeleteNote(DWORD dwNoteID);
	void	SetEnterGameFlag(bool bFlag);

protected:
	bool	FlushBuffer(BYTE *pBuffer, int &nUsedLen, FILE *fp, bool bCompressed = false);
	int		NormalPlay(LPMSG lpMsg);
	int		FastPlay(LPMSG lpMsg);
	bool	FillBuf();
	
	void	InitActiveButtonPos();
	BOOL	ReadNote();
	void	CRCBlockToFile(BYTE *pBuf, DWORD dwLen, FILE *fp, DWORD &dwRstLen);
	BOOL	IsNoteValid(BYTE *pNote);
	BOOL	CreateConfigFileFromVideo();
	
	//----------------- inline functions ------------------------------

	void	GetDataFromBuf(BYTE *pBuf, void *pData, int nSize);
	bool	SaveDataToBuf(void *pData, int nSize);
	void	SetState(int nState);
	void	ReleaseNoteResource();

	BOOL	IsInActiveButtonRegin(int nX, int nY);
	BOOL	IsInPlayCtrlRegion(int nX, int nY);
	BOOL	MakeDir(const char *szDir);
	
	//-----------------------------------------------------------------

private:
	// 禁止对此对象进行拷贝和赋值
	GameVideo(const GameVideo &rhs);
	operator = (const GameVideo &rhs);

private:

	int			m_nPlayBufUsed;				// 录像时m_bInsBuf中当前写入位置，播放时表示缓冲区读取的位置	
	int			m_nNoteBufUsed;				// 播放时注释缓冲区使用的长度
	int			m_nPlayBufDataLen;			// 播放时，缓冲区中数据的长度，不一定是满的
	int			m_nNoteBufDataLen;			// 播放时，注释缓冲区中数据的长度，不一定是满的
	int			m_nSaveNoteBufUsed;			// 播放时，添加注释缓冲区使用的位置
	int			m_nInsNum;					// 录像文件中指令的个数
	int			m_nCurIns;					// 当前执行的指令
	int			m_nCurVideoState;
	int			m_nPreVideoState;
	long		m_lStartTime;				// 录像时，表示录像开始时间，播放时，表示已经播放的时间，以秒为单位
	long		m_lEndTime;					// 录像时，表示录像结束时间，播放时，表示录像总时间，以秒为单位	
	int			m_nInsPerShowFrame;			// 快进时每多少条指令显示一桢，用这个变量的主要目的是为了对快进的优化，避免每条指令执行完都计算时间
	int			m_nInsPerSec;				// 录像时每秒指令数

	int			m_nVideoLen;				// 录像的长度，不包括播放时添加的注释
	int			m_nPlayedLen;				// 已经读到缓冲区中的录像录像文件长度，不包括添加的注释
	int			m_nOriNoteNum;				// 录像中原有的注释个数
	int			m_nUiSettingLen;			// 录像时表示配置的实际长度，播放时表示配置的存储长度
	int			m_nPreInsShowFrame;			// 快进时上一次显示一桢时的指令数

	short int	m_snWidth;					// 录像分辨率长
	short int	m_snHeight;					// 录像分辨率宽

	ButtonPos	m_ButtonPos[ACTIVE_BUTTON_NUM];		// 播放录像时按钮的位置
	ButtonPos	m_PlayCtrlRegion;			// 播放控件的大小

	NoteIndex	m_NoteIndex;				// 注释的索引，key为对应的指令标号，value为注释ID
	Note		m_Note;						// 注释，key为ID，value为注释

	BOOL		m_bIsInputPasswd;			// 判断正在输入的是否是登陆密码

	// 判断是否曾经播放过录像，由于播放录像后，缓存的用户名变成了录像时的用户名
	// 此时如果进入游戏，就会显示录像的用户名，为了避免这种情况，我们在这里置一个标记，
	// 如过播放过录像，再进入游戏时，我们不显示帐号
	BOOL		m_bHasPlayedVideo;				

	// 是否在退出时保存用户的登陆设置，如果是播放录像结束后就退出，则不保存登陆设置
	// 否则保存
	BOOL		m_bSaveLoginChoies;		
	
	bool		m_bIsEnterGame;				// 判断是否已经进入游戏

	bool		m_bHasTmpInsFile;			// 判断是有临时文件
	bool		m_bFileEnd;					// 文件已经读完
	BYTE		m_bVersion;					// 录像版本号

	BYTE		m_bPlayBuf[PLAY_BUF_SIZE];	// 播放时存储解压后的指令数据
	BYTE		m_bInsBuf[BUF_SIZE];		// 播放时，用于存储压缩过的数据，以备解码，录像时，用于存储指令

	BYTE		*m_pCompressedMem;			// 压缩数据时需要使用的内存
	BYTE		*m_pUiConfigBuf;			// 用户配置文件缓冲区

	char		m_szTmpInsFile[MAX_PATH];	// 存储录像的文件名
	FILE		*m_pInsFile;				// 存储指令的文件指针

public:
#ifdef _GameVideoDebug
	//debug
	ofstream m_videoDebug;
	ofstream m_playDebug;
	//debug
#endif
};

inline int GameVideo::GetState()
{
	return m_nCurVideoState;	
}

// 关联众多状态，不要轻易改动
inline bool GameVideo::IsPlaying()
{
	return m_nCurVideoState == enum_State_Speed ||
		   m_nCurVideoState == enum_State_NormalPlay ||
		   m_nCurVideoState == enum_State_FastPlay ||
		   m_nCurVideoState	== enum_State_TempStopPlay;
}


inline bool GameVideo::IsVideoing()
{
	return m_nCurVideoState == enum_State_Video;
}

// 判断是否正在快放或者快进
inline bool GameVideo::IsSpeed()
{
	return m_nCurVideoState == enum_State_Speed || m_nCurVideoState == enum_State_FastPlay;
}

inline void GameVideo::GetDataFromBuf(BYTE *pBuf, void *pData, int nSize)
{
	memcpy(pData, pBuf + m_nPlayBufUsed, nSize);
	m_nPlayBufUsed += nSize;
}

inline void	GameVideo::Pause()
{
	switch(m_nCurVideoState)
	{
	case enum_State_NormalPlay:
		SetState(enum_State_TempStopPlay);
		break;

	case enum_State_FastPlay:
	case enum_State_Speed:
	case enum_State_TempStopPlay:
	case enum_State_Idle:
	case enum_State_Edit:
	case enum_State_PlayFinished:
		SetState(enum_State_NormalPlay);
		break;

	case enum_State_FreeMsg:
		SetState(m_nPreVideoState);
		break;
	}
}

inline void	GameVideo::FastPlay()
{
	if(m_nCurVideoState == enum_State_NormalPlay)
	{
		SetState(enum_State_FastPlay);	
	}
}

inline void GameVideo::Speed()
{
	if(m_nCurVideoState == enum_State_NormalPlay || m_nCurVideoState == enum_State_FastPlay)
	{
		SetState(enum_State_Speed);
	}
}

inline void GameVideo::StopVideo()
{
	if(m_nCurVideoState == enum_State_Video)
	{
		m_lEndTime = static_cast<long>(time(NULL));
		SetState(enum_State_Idle);
	}	
}

inline void GameVideo::Play()
{
	Pause();
}

inline void GameVideo::SetState(int nState)
{
	m_nPreVideoState = m_nCurVideoState;
	m_nCurVideoState = nState;
	
	if(nState == enum_State_PlayFinished)
	{
		if(m_pInsFile)
		{
			fclose(m_pInsFile);
			m_pInsFile = NULL;
		}

		// 有可能一直快进到结束，此时声音标志为0，这里置回去
#ifdef _GameVideoDebug
		m_playDebug.close();
#endif
	}
}

inline void GameVideo::StopPlay()
{
	if(IsPlaying() || m_nCurVideoState == enum_State_Edit || 
	   m_nCurVideoState == enum_State_FreeMsg)
	{
		SetState(enum_State_PlayFinished);
	}
}

inline void GameVideo::EditVideo()
{
	SetState(enum_State_Edit);
}

inline int GameVideo::GetCurPlayTime()
{
	return m_nCurIns / (m_nInsPerSec ? m_nInsPerSec : 1);
}

inline int	GameVideo::GetTotalVideoLen()
{
	return m_nInsNum;
}

inline int	GameVideo::GetCurVideoLen()
{
	return m_nCurIns;
}

inline int GameVideo::GetWidth()
{
	return m_snWidth;
}
	
inline int GameVideo::GetHeight()
{
	return m_snHeight;	
}

inline BOOL	GameVideo::IsPlayFinished()
{
	return m_nCurVideoState == enum_State_PlayFinished;
}

inline BOOL GameVideo::IsEditing()
{
	return m_nCurVideoState == enum_State_Edit;
}

inline void	GameVideo::SetInputPasswordFlag(BOOL bFlag)
{
	m_bIsInputPasswd = bFlag;
}

inline BOOL	GameVideo::IsInputPassword()
{
	return m_bIsInputPasswd;
}

inline void GameVideo::FreeMsg()
{
	if(IsPlaying() || m_nCurVideoState == enum_State_Edit)
	{
		SetState(enum_State_FreeMsg);
	}
}

inline BOOL	GameVideo::IsInPlayCtrlRegion(int nX, int nY)
{
	return	nX >= m_PlayCtrlRegion.snLeft && nX <= m_PlayCtrlRegion.snRight &&
			nY >= m_PlayCtrlRegion.snTop  && nY <= m_PlayCtrlRegion.snBottom;
}

inline void	GameVideo::ReturnToIdle()
{
	m_nCurVideoState = enum_State_Idle;
	m_nPreVideoState = enum_State_Idle;
}

inline BOOL	GameVideo::HasPlayedVideo()
{
	return m_bHasPlayedVideo;
}

inline void	GameVideo::SetSaveLoginChoiesFlag(BOOL bFlag)
{
	m_bSaveLoginChoies = bFlag;
}

inline BOOL	GameVideo::GetSaveLoginChoiesFlag()
{
	return m_bSaveLoginChoies;
}

inline BYTE* GameVideo::GetNote(DWORD dwNoteID)
{
	Note::iterator iter = m_Note.find(dwNoteID);

	return iter != m_Note.end() ? iter->second : NULL;
}

inline void GameVideo::SetEnterGameFlag(bool bFlag)
{
	m_bIsEnterGame = bFlag;
}

extern GameVideo g_gameVideo;

#endif