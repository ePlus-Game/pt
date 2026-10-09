///////////////////////////////////////////////////////////////
//	文 件 名 : GameVideo.cpp
//	文件功能 :
//	作    者 : chenshanglin
//	创建时间 : 2005年9月24日
//	历史记录 : 
///////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "GameVideo.h"
#include "KLuaScript.h"
#include "./Ui/UiElem/Wnds.h"
#include "./NetConnect/NetConnectAgent.h"
#include "Faith.h"
#include "KWin32Wnd.h"
#include "CoreShell.h"
#include "Ui/UiShell.h"
#include "iRepresentShell.h"
#include "minilzo.h"
#include "CRC32.h"
#include "./Ui/UiCase/KUiReplay.h"
#include "ui/UiBase.h"
#include <cstdio>
#include <cstdarg>
#include <io.h>
#include <direct.h>

extern iCoreShell *g_pCoreShell;
extern iRepresentShell *g_pRepresent;

#define	UI_USER_DATA_FOLDER			"UserData"			//玩家数据的存盘目录位置
#define	UI_VIDEO_SETTING_DIR		"Video"				//KUiBase里使用了这个宏，修改时请注意
#define UI_PRIVATE_SETTING_FILE		"UiConfig.ini"		//界面个人数据的存储文件

GameVideo g_gameVideo;

GameVideo::GameVideo()
{
	m_nPreVideoState = enum_State_Idle;
	m_nCurVideoState = enum_State_Idle;
	
	m_pInsFile = NULL;
	m_pCompressedMem = NULL;
	m_pUiConfigBuf = NULL;
	m_nUiSettingLen = 0;
	m_nInsNum = 0;
	m_bVersion = 1;
	m_nVideoLen = 0;
	m_bHasTmpInsFile = false;
	m_bHasPlayedVideo = FALSE;

	memset(m_szTmpInsFile, 0, sizeof(m_szTmpInsFile));

	memset(m_bInsBuf, 0, sizeof(m_bInsBuf));
	memset(m_bPlayBuf, 0, sizeof(m_bPlayBuf));
}

GameVideo::~GameVideo()
{
	if(m_pCompressedMem)
	{
		delete m_pCompressedMem;
	}

	if(m_pUiConfigBuf)
	{
		delete m_pUiConfigBuf;
	}
	
	if(m_bHasTmpInsFile)
	{
		if(m_pInsFile)
		{
			fclose(m_pInsFile);
		}
		_unlink(m_szTmpInsFile);
	}

	ReleaseNoteResource();

#ifdef _GameVideoDebug
	m_videoDebug.close();
	m_playDebug.close();
#endif
}

// 把缓冲区中的数据写入临时文件
bool GameVideo::FlushBuffer(BYTE *pBuffer, int &nUsedLen, FILE *fp, bool bCompressed /* = false */)
{
	if(!m_pCompressedMem)
	{
		m_pCompressedMem = new BYTE[LZO1X_1_MEM_COMPRESS];

		if(m_pCompressedMem == NULL)
		{
			return false;
		}
	}

	// 指令压缩
	if(bCompressed)
	{
		unsigned int uPressedLen;
		int nRet = lzo1x_1_compress(pBuffer, nUsedLen, m_bPlayBuf, &uPressedLen, m_pCompressedMem);

		if(nRet != LZO_E_OK)
		{
			return false;
		}
		else	
		{
			DWORD dwRstLen;
			CRCBlockToFile(m_bPlayBuf, uPressedLen, fp, dwRstLen);
			m_nVideoLen += dwRstLen;
		}
	}
	else
	{
		DWORD dwRstLen;
		CRCBlockToFile(pBuffer, nUsedLen, fp, dwRstLen);
	}

	nUsedLen = 0;
	
	return true;
}

bool GameVideo::SaveVideoFile(const char *szFilePath /* = NULL */)
{
	// 只保存一次
	static int nSave = 0;

	// 防止外面没有调用stopvideo()
	StopVideo();

	if(nSave == 1)
	{
		return true;
	}

	// 把缓冲区的指令写入临时文件，然后把临时文件
	// 移动到目标处
	if(!FlushBuffer(m_bInsBuf, m_nPlayBufUsed, m_pInsFile, true))
	{
		return false;
	}

	DWORD	dwRstLen = 0;

	if(m_pUiConfigBuf)
	{
		if(m_nUiSettingLen)
		{	
			CRCBlockToFile(m_pUiConfigBuf, m_nUiSettingLen, m_pInsFile, dwRstLen);
		}
	}

	// 指令数、录像时间、录像长度、注释次数、录像时分辨率、版本号
	VideoHeader vh;
	memset(&vh, 0, sizeof(vh));

	vh.nInsNum = m_nInsNum;
	vh.nVideoTimeLen = m_lEndTime - m_lStartTime;
	vh.nVideoLen = m_nVideoLen;

	// 注意，存储的nUiSettingLen是存储长度，即设定本身的长度加上一个size_t表示长度
	// 以及2个CRC校验
	vh.nUiSettingLen = dwRstLen;

	extern int SCREEN_WIDTH, SCREEN_HEIGHT;
	vh.snWidth = SCREEN_WIDTH;
	vh.snHeight = SCREEN_HEIGHT;
	
	vh.bVersion = m_bVersion;

	vh.dwCRC = 0;
	vh.dwCRC = CRC32(vh.dwCRC, (BYTE*)&vh + sizeof(vh.dwCRC), 
					 sizeof(vh) - sizeof(vh.dwCRC));

	fseek(m_pInsFile, 0, SEEK_SET);
	fwrite(&vh, sizeof(vh), 1, m_pInsFile);
	fclose(m_pInsFile);
	m_pInsFile = NULL;

	if(szFilePath != NULL)
	{
		const char *szVideoDir = "封神截图";
		const char *szSubVideoDir = "封神截图/录像";

		if(!MakeDir(szVideoDir))
		{
			return false;
		}

		if(!MakeDir(szSubVideoDir))
		{
			return false;
		}

		DeleteFile(szFilePath);
		if(!MoveFile(m_szTmpInsFile, szFilePath))
		{
			return false;
		}

		_unlink(m_szTmpInsFile);
		m_bHasTmpInsFile = false;
	}

	nSave = 1;

#ifdef _GameVideoDebug
	m_videoDebug.close();
#endif

	return true;
}

int GameVideo::PreparePlayingVideo(const char *szFilePath)
{
#ifdef _GameVideoDebug
	m_playDebug.open("FS_Play_Debug.txt");
#endif

	if(NULL == szFilePath)
	{
		return enum_Play_Error;
	}
	
	// 如果用户先点击了录像，然后再点播放，就会出现这种情况
	if(IsVideoing())
	{
		if(m_pInsFile)
		{
			fclose(m_pInsFile);
			m_pInsFile = NULL;
		
			if(strlen(m_szTmpInsFile))
			{
				_unlink(m_szTmpInsFile);
				memset(m_szTmpInsFile, 0, sizeof(m_szTmpInsFile));
				m_bHasTmpInsFile = false;
			}
		}

		SetState(enum_State_Idle);
	}

	m_pInsFile = fopen(szFilePath, "rb+");

	if(!m_pInsFile)
	{
		return enum_Play_Error;
	}

	VideoHeader vh;	

	if(fread(&vh, sizeof(vh), 1, m_pInsFile) <= 0)
	{
		return enum_Play_Error;
	}

	DWORD dwNewCRC = 0;
	dwNewCRC = CRC32(dwNewCRC, (BYTE*)&vh + 4, sizeof(vh) - 4);

	if(dwNewCRC != vh.dwCRC)
	{
		return enum_File_CRCError;
	}

	extern int SCREEN_WIDTH, SCREEN_HEIGHT;
	if(vh.snWidth != SCREEN_WIDTH)
	{
		m_snWidth = vh.snWidth;
		m_snHeight = vh.snHeight;

		return enum_Play_SizeError;
	}

	m_nInsNum			= vh.nInsNum;
	m_lEndTime			= vh.nVideoTimeLen;
	m_nVideoLen			= vh.nVideoLen;
	m_bVersion			= vh.bVersion;
	m_nOriNoteNum		= vh.nNoteNum;	
	m_nUiSettingLen		= vh.nUiSettingLen;
	m_nCurIns			= 0;
	m_lStartTime		= 0;
	m_nPlayBufUsed		= 0;
	m_nPlayBufDataLen	= 0;
	m_nNoteBufDataLen	= 0;
	m_nNoteBufUsed		= 0;
	m_nSaveNoteBufUsed	= 0;
	m_nPreInsShowFrame	= 0;
	m_nPlayedLen		= sizeof(vh);
	m_bFileEnd			= false;
	m_bHasPlayedVideo	= TRUE;
	m_bSaveLoginChoies	= FALSE;

	m_bIsEnterGame		= false;
	
//	m_nInsPerSec = m_nInsNum / (m_lEndTime ? m_lEndTime : 1); //(这个进起来太快了)
	m_nInsPerSec = (m_nInsPerSec > 10 && m_nInsPerSec < 100) ? m_nInsPerSec : 30;
	m_nInsPerShowFrame = m_nInsPerSec * DEFAULT_SPEED_FRE;   // 默认快进时每15秒显示一桢

	memset(m_bPlayBuf, 0, sizeof(m_bPlayBuf));
	memset(m_bInsBuf, 0, sizeof(m_bInsBuf));

	strncpy(m_szTmpInsFile, szFilePath, sizeof(m_szTmpInsFile));

	DWORD dwOriFilePos = ftell(m_pInsFile);

	InitActiveButtonPos();

	// 删除原有的设定文件
	char	szOriUiSettingFile[MAX_PATH];
	sprintf(szOriUiSettingFile, "%s/%s/%s", UI_USER_DATA_FOLDER, UI_VIDEO_SETTING_DIR, UI_PRIVATE_SETTING_FILE);
	if(_access(szOriUiSettingFile, 0) != -1)
	{
		_unlink(szOriUiSettingFile);
	}

	if(vh.nUiSettingLen)
	{
		fseek(m_pInsFile, vh.nVideoLen, SEEK_SET);
		if(!CreateConfigFileFromVideo())
		{
			return enum_File_CRCError;	
		}
	}

	if(m_nOriNoteNum > 0)
	{
		if(!ReadNote())
		{
			return enum_Note_Error;
		}
	}

	fseek(m_pInsFile, dwOriFilePos, SEEK_SET);

	return enum_Play_OK;
}

BYTE* GameVideo::GetNetPackage(size_t &uSize, int &nDataType)
{
	uSize = 0;
	nDataType = 0;

	if(!IsPlaying())
	{
		return NULL;
	}

	// 数据类型使用1个字节来存储的
	if(m_nPlayBufDataLen - m_nPlayBufUsed < enum_NDT_Size + sizeof(uSize))
	{
		if(!FillBuf())
		{
			return NULL;
		}
	}

	GetDataFromBuf(m_bPlayBuf, &nDataType, enum_NDT_Size);
	GetDataFromBuf(m_bPlayBuf, &uSize, sizeof(uSize));

#ifdef _GameVideoDebug
	m_playDebug << "DataType " << nDataType <<  " DataLen " << uSize  << " State " << m_nCurVideoState << endl;
#endif
	
	if(uSize > 0)
	{
		if(m_nPlayBufDataLen - m_nPlayBufUsed < uSize)
		{
			if(!FillBuf())
			{
				return NULL;
			}
		}

		int nBufBegin = m_nPlayBufUsed;
		m_nPlayBufUsed += uSize;
		
		return m_bPlayBuf + nBufBegin;
	}
	else if(uSize == 0)
	{
		return NULL;
	}
}

int GameVideo::GetNpcIndex()
{
	int nIndex = 0, nType = 0;

	if(m_nPlayBufDataLen - m_nPlayBufUsed < enum_NpcIndexFlag_Size + sizeof(nIndex))
	{
		if(FillBuf())
		{
			return nIndex;
		}
	}

	GetDataFromBuf(m_bPlayBuf, &nType, enum_NpcIndexFlag_Size);
	GetDataFromBuf(m_bPlayBuf, &nIndex, sizeof(nIndex));

#ifdef _GameVideoDebug
	m_playDebug << "NpcIndex " << nType << " " << nIndex << endl;
#endif

	return nIndex;
}

bool GameVideo::SaveOperation(int nArgNum, int nSaveOpe, bool bSaveOpeCode,  ...)
{
	if(bSaveOpeCode)
	{
		++m_nInsNum;

#ifdef _GameVideoDebug
		m_videoDebug << "[Ins-" << m_nInsNum << "] ";
#endif
	}
	
	va_list va;
	va_start(va, bSaveOpeCode);
	
	switch(nSaveOpe)
	{
		// 存储网络数据包，不包括指令码及数据类型
	case enum_SaveOpe_NetData:
		{
			int nDataType = va_arg(va, int);
			void *pData = va_arg(va, void*);
			int  nDataLen = va_arg(va, int);

				// 网络数据包类型用1个字节存储
			SaveDataToBuf(&nDataType, enum_NDT_Size);
			SaveDataToBuf(&nDataLen, sizeof(nDataLen));

			if(nDataLen > 0)
			{
				SaveDataToBuf(pData, nDataLen);
			}

#ifdef _GameVideoDebug
			m_videoDebug << "DataType " << nDataType << " DataLen " << nDataLen << endl;
#endif

			break;
		}

	case enum_SaveOpe_TimePassed:
		{
			int nArgValue;

			nArgValue = enum_Type_TimePassed;
			SaveDataToBuf(&nArgValue, enum_InsCode_Size);
	
			nArgValue = va_arg(va, int);
			SaveDataToBuf(&nArgValue, enum_TimePassedFlag_Size);

#ifdef _GameVideoDebug
			m_videoDebug << "TimePassed " << enum_Type_TimePassed << " " << nArgValue << endl;
#endif
			
			break;
		}
		
	case enum_SaveOpe_BuildingSchedule:
		{
			int nArgValue;

			nArgValue = enum_Type_BuildingSchedule;
			SaveDataToBuf(&nArgValue, enum_InsCode_Size);

			nArgValue = va_arg(va, int);
			SaveDataToBuf(&nArgValue, sizeof(int));
			break;
		}

	case enum_SaveOpe_MainSchedule:
		{
			int nArgValue;

			nArgValue = enum_Type_MainSchedule;
			SaveDataToBuf(&nArgValue, enum_InsCode_Size);

			nArgValue = va_arg(va, int);
			SaveDataToBuf(&nArgValue, sizeof(int));
			nArgValue = va_arg(va, int);
			SaveDataToBuf(&nArgValue, sizeof(int));
			break;
		}

		// 存储指令码及对应的数据，不能包含指针，指针单独处理
	case enum_SaveOPe_NetDataCode:
	case enum_SaveOpe_PlayerInput:
	case enum_SaveOpe_CoreBreathe:
	case enum_SaveOpe_UiPaint:
	case enum_SaveOpe_NpcIndex:
		{
			int nArgValue;
			int nArgPos = 3;

			while(nArgPos++ < nArgNum)
			{
				nArgValue = va_arg(va, int);

				// 指令码按照指定的大小存储，而且如果bSaveOpeCode为true
				// 则第四个参数就是操作码
				if(bSaveOpeCode && nArgPos == 4)
				{
					SaveDataToBuf(&nArgValue, enum_InsCode_Size);
				}
				// NpcIndex 的标志按照指定大小存储
				else if(nArgPos == 4 && nSaveOpe == enum_SaveOpe_NpcIndex)
				{
					SaveDataToBuf(&nArgValue, enum_NpcIndexFlag_Size);
				}
				else
				{
					SaveDataToBuf(&nArgValue, sizeof(nArgValue));
				}

#ifdef _GameVideoDebug
				if(!bSaveOpeCode && nArgPos == 4)
				{
					m_videoDebug << "NpcIndex ";
				}

				if(nArgPos != nArgNum)
				{
					m_videoDebug << nArgValue << " ";
				}
				else
				{
					m_videoDebug << nArgValue;
				}
#endif
			}

#ifdef _GameVideoDebug
			if(nSaveOpe != enum_SaveOPe_NetDataCode)
			{
				m_videoDebug << endl;
			}
			else
			{
				m_videoDebug << " ";
			}
#endif
			break;
		}

	default:
		break;
	}

	va_end(va);

	return true;
}

int GameVideo::GetMsg(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	int nRet;
	nRet = PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

	// 在编辑录像的状态下，放过所有的消息
	if(IsEditing())
	{
		return nRet;
	}

	// 如果窗口被最小化了, 我们应该能响应将其复原的消息
	if(nRet && lpMsg->message == WM_SYSCOMMAND && lpMsg->wParam == SC_RESTORE)
	{
		WINDOWPLACEMENT wndpl;

		if(GetWindowPlacement(g_GetMainHWnd(), &wndpl))
		{
			wndpl.showCmd = SW_RESTORE;
			SetWindowPlacement(g_GetMainHWnd(), &wndpl);
		}

		return nRet;
	}
	//

	if(nRet && lpMsg->message == WM_SYSKEYDOWN && lpMsg->wParam == VK_F10)
	{
		return nRet;
	}

	if(nRet && lpMsg->message == WM_KEYDOWN)
	{
		switch(lpMsg->wParam)
		{
		case VK_ESCAPE:  // 停止
			StopPlay();
			return nRet;

		case VK_TAB:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
			return nRet;

		case VK_RETURN:
			if(GetKeyState(VK_MENU) & 0x8000)
			{
				return nRet;
			}
			break;

		default:
			break;
		}
	}	

	if(IsPlayFinished())
	{
		return nRet;
	}

	if(IsPlaying())
	{
		if(m_nCurVideoState == enum_State_FreeMsg)
		{
			return nRet;
		}

		// 不要把这个判断放在最后，否则，最后一条
		// 指令不会被执行完
		if(m_nCurIns >= m_nInsNum)
		{
			SetState(enum_State_PlayFinished);
			return nRet;
		}

		// 注意，如果当前没有消息，PeekMessage返回0，但lpMsg的数据并不会被0填充
		// 仍然是原来的数据
		if(nRet && lpMsg->message == WM_LBUTTONDOWN)
		{
			int uRet = IsInActiveButtonRegin(LOWORD(lpMsg->lParam), HIWORD(lpMsg->lParam));

			if(uRet)
			{
				lpMsg->message = WND_N_BUTTON_CLICK;
				lpMsg->wParam = uRet;

				return nRet;
			}
		}
		
		// 在暂停播放的情况下，返回enum_NULL_Loop表示这个循环什么都不干
		if(m_nCurVideoState == enum_State_TempStopPlay)
		{
			return enum_Null_Loop;
		}

		if(m_nOriNoteNum > 0)
		{
			BYTE *pNote;
			int	 nLen;

			pair<NoteIndex::iterator, NoteIndex::iterator> pIter;
			pIter= m_NoteIndex.equal_range(m_nCurIns);

			while(pIter.first != pIter.second)
			{
				// 注意GetNote里面会改变文件指针位置
				DWORD dwPreFilePos = ftell(m_pInsFile);
				pNote = GetNote(pIter.first->second);
				fseek(m_pInsFile, dwPreFilePos, SEEK_SET);

				KUiReplayCtrl *pWindow = KUiReplayCtrl::GetIfVisible();

				if(pWindow && pNote)
				{
					pWindow->UpdateNote((unsigned int)pNote);
					--m_nOriNoteNum;
				}

				++pIter.first;
			}
		}

		switch(m_nCurVideoState)
		{
		case enum_State_NormalPlay:
			nRet = NormalPlay(lpMsg);
			break;

		case enum_State_FastPlay:
		case enum_State_Speed:
			nRet = FastPlay(lpMsg);
			break;
		}

		++m_nCurIns;
	}

	return nRet;
}

bool GameVideo::TestFlagData(int nFlagData, int nDataSize)
{
#ifdef _GameVideoDebug
	m_playDebug << m_nPlayBufDataLen << " " << m_nPlayBufUsed << " ";
#endif
	
	if(m_nPlayBufDataLen - m_nPlayBufUsed < nDataSize)
	{
		// 缓冲区中的数据已用完
		if(!FillBuf())
		{
			return false;
		}
	}

	if(m_bFileEnd)
	{
		// 文件已经读完
		return false;
	}

	int nTmpData = 0;
	memcpy(&nTmpData, m_bPlayBuf + m_nPlayBufUsed, nDataSize);

#ifdef _GameVideoDebug
	m_playDebug << nTmpData << " " << nFlagData << endl;
#endif 

	return nTmpData == nFlagData;
}

///////////////////////////////////////////////////////////////
// Function    : 
// Author      : chenshanglin
// Create Time : 2005年10月18日
// Remark      : 
///////////////////////////////////////////////////////////////
int GameVideo::GetTimePassed()
{
	int nType = 0;
	int nTimePassed = 0;

	if(m_nPlayBufDataLen - m_nPlayBufUsed < enum_InsCode_Size + enum_TimePassedFlag_Size)
	{
		FillBuf();
	}

	GetDataFromBuf(m_bPlayBuf, &nType, enum_InsCode_Size);
	GetDataFromBuf(m_bPlayBuf, &nTimePassed, enum_TimePassedFlag_Size);
	
#ifdef _GameVideoDebug
	m_playDebug << "TimePassed	" << nTimePassed << endl;
#endif
	
	return nTimePassed;
}

///////////////////////////////////////////////////////////////
// Function    : 正常播放
// Author      : chenshanglin
// Create Time : 2005年10月18日
// Remark      : 
///////////////////////////////////////////////////////////////
int GameVideo::NormalPlay(LPMSG lpMsg)
{
	// 进入到这里以后，开始的数据必须是指令码，否则表示
	// 中间的状态由于随机出现了一些错乱，在这里进行调整
	// 
	// 在我们录像时，为了能快速恢复状态和快速播放，在 GameLoop
	// 里面，我们对 g_NetConnectAgent.Breathe(), g_pCoreShell.Breathe()
	// 和 UiPaint都记录了指令，但是真正的gameloop频率和
	// 记录的g_NetConnetAgent.Breathe()相同，正常播放时，我们应该按照
	// 这个频率来，只需对记录的g_NetConnetAgent.Breathe()调用GameLoop
	// 即可，里面会自动判断需不需要调用UiPaint等，所以正常播放时，这些指令
	// 并没有用，而且它们会干扰g_NetConnetAgent.Breathe()读取下行包，
	// 这里对无用和错乱的数据进行调整

	int nInsCode = 0;

	while(TestFlagData(enum_InsCode_CoreBreathe, enum_InsCode_Size) ||
		  TestFlagData(enum_InsCode_UiPaint, enum_InsCode_Size))
	{
		++m_nPlayBufUsed;
		++m_nCurIns;

#ifdef _GameVideoDebug	
		m_playDebug << "[Ins-" << m_nCurIns << "] " << enum_InsCode_CoreBreathe << " 或 " << enum_InsCode_UiPaint << endl;
#endif

		while(TestFlagData(enum_Type_TimePassed, enum_InsCode_Size))
		{
#ifdef _GameVideoDebug
			m_playDebug << "调整偏移 ";
#endif
			GetTimePassed();
		}

		if(TestFlagData(enum_Type_BuildingSchedule, enum_InsCode_Size))
		{
			GetBuildingSchedule();
		}

		if(TestFlagData(enum_Type_MainSchedule, enum_InsCode_Size))
		{
			DWORD dwTimePassed, dwOffset;
			GetMainSchedule(dwTimePassed, dwOffset);
		}
	}

	if(TestFlagData(enum_Type_NpcIndex, enum_NpcIndexFlag_Size))
	{
		GetNpcIndex();
	}

	if(TestFlagData(enum_Type_BuildingSchedule, enum_InsCode_Size))
	{
		GetBuildingSchedule();
	}

	if(TestFlagData(enum_Type_MainSchedule, enum_InsCode_Size))
	{
		DWORD dwTimePassed, dwOffset;
		GetMainSchedule(dwTimePassed, dwOffset);
	}

	if(m_bFileEnd || m_nCurVideoState == enum_State_PlayFinished)
	{
		return enum_Null_Loop;
	}

	nInsCode = 0;    // 别忘了清0，因为将要读出1字节

	if(m_nPlayBufDataLen - m_nPlayBufUsed < enum_InsCode_Size)
	{
		FillBuf();
	}

	GetDataFromBuf(m_bPlayBuf, &nInsCode, enum_InsCode_Size);

#ifdef _GameVideoDebug
	m_playDebug << "[Ins-" << m_nCurIns + 1 << "] " << nInsCode << " ";
#endif

	switch(nInsCode)
	{
	case enum_InsCode_PlayerInput:
		if(m_nPlayBufDataLen - m_nPlayBufUsed < sizeof(lpMsg->message) + sizeof(lpMsg->wParam) + sizeof(lpMsg->lParam))
		{
			FillBuf();
		}

		GetDataFromBuf(m_bPlayBuf, &lpMsg->message, sizeof(lpMsg->message));
		GetDataFromBuf(m_bPlayBuf, &lpMsg->wParam, sizeof(lpMsg->wParam));
		GetDataFromBuf(m_bPlayBuf, &lpMsg->lParam, sizeof(lpMsg->lParam));

#ifdef _GameVideoDebug
		m_playDebug << lpMsg->message << " " << lpMsg->wParam << " " << lpMsg->lParam << endl;
#endif

		// 由于录制窗口和播放窗口在同一位置，保存录像时的操作也会录下来，而在播放时，这些
		// 操作正好作用在那些播放按钮上，会导致播放提前终止，所以这里对于在播放窗口上的
		// 点击操作，我们把它过滤掉
		if(lpMsg->message == WM_LBUTTONDOWN && IsInPlayCtrlRegion(LOWORD(lpMsg->lParam), HIWORD(lpMsg->lParam)))
		{
			return enum_Null_Loop;
		}

		// 登陆过程中如果连续两次很快的回车，状态容易混乱
		// 所以这里在登陆过程中放慢节奏
		if(lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_RETURN && !m_bIsEnterGame)
		{
			g_pCoreShell->Breathe();
			UiHeartBeat();
		}

		return true;

	case enum_InsCode_GameLogic:
		return false;

	default:
#ifdef _GameVideoDebug
		m_playDebug << "OpeCode Error!" << endl;
#endif
		break;
	}

	return true;
}

int GameVideo::FastPlay(LPMSG lpMsg)
{
	int nRet = 1;

	if(TestFlagData(enum_Type_NpcIndex, enum_NpcIndexFlag_Size))
	{
#ifdef _GameVideoDebug
		m_playDebug << "调整 NpcIndex ";
#endif
		GetNpcIndex();
	}

	while(TestFlagData(enum_Type_TimePassed, enum_InsCode_Size))
	{
#ifdef _GameVideoDebug
		m_playDebug << "调整偏移 ";
#endif
		GetTimePassed();
	}

	if(TestFlagData(enum_Type_BuildingSchedule, enum_InsCode_Size))
	{
		GetBuildingSchedule();
	}

	if(TestFlagData(enum_Type_MainSchedule, enum_InsCode_Size))
	{
		DWORD dwTimePassed, dwOffset;
		GetMainSchedule(dwTimePassed, dwOffset);
	}

	if(m_bFileEnd || m_nCurVideoState == enum_State_PlayFinished)
	{
		return enum_Null_Loop;
	}
	
	int nInsCode = 0;	// 别忘记先清0，因为将要读出1字节

	if(m_nPlayBufDataLen - m_nPlayBufUsed < enum_InsCode_Size)
	{
		FillBuf();
	}

	GetDataFromBuf(m_bPlayBuf, &nInsCode, enum_InsCode_Size);
	
#ifdef _GameVideoDebug
	m_playDebug << "[Ins-" << m_nCurIns + 1 << "] " << nInsCode << " ";
#endif

	switch(nInsCode)
	{
	case enum_InsCode_PlayerInput:
		if(m_nPlayBufDataLen - m_nPlayBufUsed < sizeof(lpMsg->message) + sizeof(lpMsg->wParam) + sizeof(lpMsg->lParam))
		{
			FillBuf();
		}

		GetDataFromBuf(m_bPlayBuf, &lpMsg->message, sizeof(lpMsg->message));
		GetDataFromBuf(m_bPlayBuf, &lpMsg->wParam, sizeof(lpMsg->wParam));
		GetDataFromBuf(m_bPlayBuf, &lpMsg->lParam, sizeof(lpMsg->lParam));

#ifdef _GameVideoDebug
		m_playDebug << lpMsg->message << " " << lpMsg->wParam << " " << lpMsg->lParam << endl;
#endif

		// 由于录制窗口和播放窗口在同一位置，保存录像时的操作也会录下来，而在播放时，这些
		// 操作正好作用在那些播放按钮上，会导致播放提前终止，所以这里对于在播放窗口上的
		// 点击操作，我们把它过滤掉
		if(lpMsg->message == WM_LBUTTONDOWN && IsInPlayCtrlRegion(LOWORD(lpMsg->lParam), HIWORD(lpMsg->lParam)))
		{
			nRet = enum_Null_Loop;
		}
		else
		{
			nRet = true;
		}

		// 登陆过程中如果连续两次很快的回车，状态容易混乱
		// 所以这里在登陆过程中放慢节奏
		if(lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_RETURN && !m_bIsEnterGame)
		{
			g_pCoreShell->Breathe();
			UiHeartBeat();
		}
		
		break;

	case enum_InsCode_GameLogic:
		g_NetConnectAgent.Breathe();
		nRet = enum_Null_Loop;
		break;

	case enum_InsCode_CoreBreathe:
		g_pCoreShell->Breathe();
		UiHeartBeat();
		nRet = enum_Null_Loop;

#ifdef _GameVideoDebug
		m_playDebug << endl;
#endif
		break;

	case enum_InsCode_UiPaint:
		{
			if(m_nCurVideoState == enum_State_FastPlay)
			{
				UiPaint(0);
			}
			else if(m_nCurVideoState == enum_State_Speed && m_nCurIns - m_nPreInsShowFrame >= m_nInsPerShowFrame)
			{
				// 快进时，每隔一定的时间才显示一桢
				UiPaint(0);
				m_nPreInsShowFrame = m_nCurIns;
			}
			nRet = enum_Null_Loop;
		}
#ifdef _GameVideoDebug
		m_playDebug << endl;
#endif
		break;

	default:
#ifdef _GameVideoDebug
		m_playDebug << "Error OpeCode" << endl;
#endif
		break;
	}

	return nRet;
}

bool GameVideo::FillBuf()
{
	if(m_nPlayedLen >= m_nVideoLen)
	{
		m_bFileEnd = true;
		SetState(enum_State_PlayFinished);
		return false;
	}

	int nLeft = m_nPlayBufDataLen - m_nPlayBufUsed;
	memmove(m_bPlayBuf, m_bPlayBuf + m_nPlayBufUsed, nLeft);

	// 读出长度，检验CRC
	DWORD dwOldCRC;
	fread(&dwOldCRC, sizeof(dwOldCRC), 1, m_pInsFile);

	int nBlockLen = 0;
	fread(&nBlockLen, sizeof(nBlockLen), 1, m_pInsFile);

	DWORD dwNewCRC = 0;
	dwNewCRC = CRC32(dwNewCRC, &nBlockLen, sizeof(nBlockLen));

	if(dwNewCRC != dwOldCRC)
	{
		SetState(enum_State_PlayFinished);
		
		KUiReplayCtrl *pWindow = KUiReplayCtrl::GetIfVisible();
		if(pWindow)
		{
			pWindow->ErrorManage(enum_File_CRCError);
		}

		return false;
	}

	// 读取数据，检验CRC
	fread(&dwOldCRC, sizeof(dwOldCRC), 1, m_pInsFile);
	fread(m_bInsBuf, nBlockLen, 1, m_pInsFile);

	dwNewCRC = 0;
	dwNewCRC = CRC32(dwNewCRC, m_bInsBuf, nBlockLen);

	if(dwOldCRC != dwNewCRC)
	{
		SetState(enum_State_PlayFinished);

		KUiReplayCtrl *pWindow = KUiReplayCtrl::GetIfVisible();
		if(pWindow)
		{
			pWindow->ErrorManage(enum_File_CRCError);
		}

		return false;
	}

	m_nPlayedLen += sizeof(dwOldCRC);
	m_nPlayedLen += sizeof(nBlockLen);
	m_nPlayedLen += sizeof(dwNewCRC);
	m_nPlayedLen += nBlockLen;

	int nRet;
	
	nRet = lzo1x_decompress(m_bInsBuf, nBlockLen, m_bPlayBuf + nLeft, (unsigned int*)&nBlockLen, NULL);

	if(nRet != LZO_E_OK)
	{
		SetState(enum_State_PlayFinished);
		return false;
	}
	
	m_nPlayBufDataLen = nLeft + nBlockLen;
	m_nPlayBufUsed = 0;

	return true;
}

bool GameVideo::AddNote(BYTE *pNotes, int nNotesLen)
{
	if(!IsNoteValid(pNotes))
	{
		return false;
	}

	// 保证为每条注释分配唯一的ID, 从1开始，总共可以分配2**32-1次
	// 应该足够用，不会溢出
	KReplayNote *pReplayNote = (KReplayNote*)pNotes;

	// 如果空注释是新加的，我们不显示，如果是用户编辑原有注释，把
	// 内容删除了，我们将删除这条注释
	if(!pReplayNote->szContent[0])
	{
		return true;
	}

	Note::iterator iter = m_Note.find(pReplayNote->dwNoteID);

	if(iter != m_Note.end())		// 更新已有的注释
	{
		KReplayNote *pOriNote = (KReplayNote*)iter->second;

		strncpy(pOriNote->szContent, pReplayNote->szContent, sizeof(pReplayNote->szContent));
		pOriNote->dwTime = pReplayNote->dwTime;
		
		KUiReplayCtrl *pWnd = KUiReplayCtrl::GetIfVisible();

		if(pWnd)
		{
			pWnd->UpdateNote((unsigned int)pNotes);
		}
	}
	else							// 添加新的注释
	{
		pReplayNote->dwNoteID = m_Note.size() ? m_Note.rbegin()->first + 1 : 1;
		pReplayNote->dwCurIns = m_nCurIns;

		KUiReplayCtrl *pWnd = KUiReplayCtrl::GetIfVisible();

		if(pWnd)
		{
			pWnd->UpdateNote((unsigned int)pNotes);
		}

		BYTE *pNoteCopy = new BYTE[nNotesLen];

		if(NULL == pNoteCopy)
		{
			return false;
		}

		memcpy(pNoteCopy, pNotes, nNotesLen);
		m_NoteIndex.insert(NoteIndex::value_type(m_nCurIns, pReplayNote->dwNoteID));
		m_Note.insert(Note::value_type(pReplayNote->dwNoteID, pNoteCopy));
	}

	return true;
}

bool GameVideo::SaveNote()
{
	if(m_Note.empty())
	{
		return true;
	}
	
	if(!m_pInsFile)
	{
		m_pInsFile = fopen(m_szTmpInsFile, "rb+");

		if(!m_pInsFile)
		{
			ReleaseNoteResource();
			return false;
		}
	}

	fseek(m_pInsFile, 0, SEEK_SET);

	VideoHeader vh;
	fread(&vh, sizeof(vh), 1, m_pInsFile);	
	fseek(m_pInsFile, m_nVideoLen + m_nUiSettingLen, SEEK_SET);

	Note::const_iterator iterNote;
	Note::const_iterator iterEnd(m_Note.end());

	DWORD	dwCRC;
	DWORD	dwNoteLen;
		
	dwNoteLen = sizeof(KReplayNote);

	for(iterNote = m_Note.begin(); iterNote != iterEnd; ++iterNote)
	{
		dwCRC = 0;
		dwCRC = CRC32(dwCRC, &dwNoteLen, sizeof(dwNoteLen));
		fwrite(&dwCRC, 1, sizeof(dwCRC), m_pInsFile);
		fwrite(&dwNoteLen, 1, sizeof(dwNoteLen), m_pInsFile);

		dwCRC = 0;
		dwCRC = CRC32(dwCRC, iterNote->second, dwNoteLen);
		fwrite(&dwCRC, 1, sizeof(dwCRC), m_pInsFile);
		fwrite(iterNote->second, 1, dwNoteLen, m_pInsFile);

		vh.nNotesLen += sizeof(dwCRC) * 2 + sizeof(dwNoteLen) + dwNoteLen;
	}

	vh.nNoteNum = m_Note.size();
	vh.dwCRC = 0;
	vh.dwCRC = CRC32(vh.dwCRC, (BYTE*)&vh + sizeof(vh.dwCRC), sizeof(vh) - sizeof(vh.dwCRC));

	fseek(m_pInsFile, 0, SEEK_SET);
	fwrite(&vh, sizeof(vh), 1, m_pInsFile);
	fclose(m_pInsFile);
	m_pInsFile = NULL;

	ReleaseNoteResource();
	
	return true;	
}

bool GameVideo::SaveDataToBuf(void *pData, int nSize)
{
	// 确保缓冲区中剩余的空间足以存放这一条指令
	if(m_nPlayBufUsed + nSize > sizeof(m_bInsBuf))
	{
		if(!FlushBuffer(m_bInsBuf, m_nPlayBufUsed, m_pInsFile, true))
		{
			return false;
		}
	}
			
	memcpy(m_bInsBuf + m_nPlayBufUsed, pData, nSize);
	m_nPlayBufUsed += nSize;
	
	return true;
}

bool GameVideo::VideoInit()
{
#ifdef _GameVideoDebug
	m_videoDebug.open("FS_Video_Debug.txt");
#endif
	
	// 处理多次点击录像
	if(m_pInsFile)
	{
		fclose(m_pInsFile);
		m_pInsFile = NULL;

		if(m_szTmpInsFile[0])
		{		
			_unlink(m_szTmpInsFile);
			memset(m_szTmpInsFile, 0, sizeof(m_szTmpInsFile));
			m_bHasTmpInsFile = false;
		}
	}

	m_bIsInputPasswd = FALSE;

	// 产生独一无二的临时文件名
	long lCurTime = time(NULL);

	_snprintf(m_szTmpInsFile, MAX_PATH, "%d.FSV", lCurTime);
	m_pInsFile = fopen(m_szTmpInsFile, "wb");

	m_bHasTmpInsFile = true;

	if(!m_pInsFile)
	{
		return false;
	}

	// 指令数、录像时间、录像长度、注释次数、录像时分辨率、版本号
	VideoHeader vh;
	memset(&vh, 0, sizeof(vh));
	fwrite(&vh, sizeof(vh), 1, m_pInsFile);

	m_nVideoLen = sizeof(vh);

	// 初始化指令缓冲区初始使用的长度，以及指令数
	m_nPlayBufUsed = 0;
	m_nInsNum = 0;

	return true;
}

inline BOOL GameVideo::IsInActiveButtonRegin(int nX, int nY)
{
	int	nTmp;

	for(nTmp = 0; nTmp < ACTIVE_BUTTON_NUM; ++nTmp)
	{
		if(nX >= m_ButtonPos[nTmp].snLeft && nX <= m_ButtonPos[nTmp].snRight && 
		   nY >= m_ButtonPos[nTmp].snTop && nY <= m_ButtonPos[nTmp].snBottom)
		{
			return (BOOL)KUiReplayCtrl::GetIfVisible()->GetButtonPrt(nTmp);
		}
	}

	return FALSE;
}

void GameVideo::InitActiveButtonPos()
{
	KWndWindow *pButton;
	int nTmpButton;
	int	nWidth, nHeight;

	memset(m_ButtonPos, 0, sizeof(m_ButtonPos));

	for(nTmpButton = 0; nTmpButton < ACTIVE_BUTTON_NUM; ++nTmpButton)
	{
		pButton = KUiReplayCtrl::GetIfVisible()->GetButtonPrt(nTmpButton);
		
		if(pButton)
		{
			pButton->GetAbsolutePos((int*)&m_ButtonPos[nTmpButton].snLeft, (int*)&m_ButtonPos[nTmpButton].snTop);
			pButton->GetSize(&nWidth, &nHeight);
			m_ButtonPos[nTmpButton].snRight = m_ButtonPos[nTmpButton].snLeft + nWidth;
			m_ButtonPos[nTmpButton].snBottom = m_ButtonPos[nTmpButton].snTop + nHeight;
		}
	}

	pButton = KUiReplayCtrl::GetIfVisible();
	memset(&m_PlayCtrlRegion, 0, sizeof(m_PlayCtrlRegion));
	
	if(pButton)
	{
		pButton->GetAbsolutePos((int*)&m_PlayCtrlRegion.snLeft, (int*)&m_PlayCtrlRegion.snTop);
		pButton->GetSize(&nWidth, &nHeight);
		m_PlayCtrlRegion.snRight = m_PlayCtrlRegion.snLeft + nWidth;
		m_PlayCtrlRegion.snBottom = m_PlayCtrlRegion.snTop + nHeight;
	}
}

bool GameVideo::Video()
{
	SetState(enum_State_Video);
	m_lStartTime = static_cast<long>(time(NULL));

	return VideoInit();
}

BOOL GameVideo::ReadNote()
{
	m_NoteIndex.clear();
	ReleaseNoteResource();
	
	if(!m_pInsFile)
	{
		return FALSE;
	}

	fseek(m_pInsFile, m_nVideoLen + m_nUiSettingLen, SEEK_SET);

	int		nTmpNote;
	DWORD	dwNewCRC;
	DWORD	dwOldCRC;
	BYTE	*pNoteData;
	KReplayNote *pReplayNote;
	CRCDataLen crcDataLen;

	for(nTmpNote = 0; nTmpNote < m_nOriNoteNum; ++nTmpNote)
	{
		fread(&crcDataLen, 1, sizeof(crcDataLen), m_pInsFile);

		dwNewCRC = CRC32(0, &crcDataLen.dwDataLen, sizeof(crcDataLen.dwDataLen));
		
		if(dwNewCRC != crcDataLen.dwCRC)
		{
			return FALSE;
		}
		
		fread(&dwOldCRC, 1, sizeof(dwOldCRC), m_pInsFile);
		pNoteData = new BYTE[crcDataLen.dwDataLen];

		if(NULL == pNoteData)
		{
			break;
		}
		
		fread(pNoteData, 1, crcDataLen.dwDataLen, m_pInsFile);
		dwNewCRC = CRC32(0, pNoteData, crcDataLen.dwDataLen);

		if(dwOldCRC != dwNewCRC)
		{
			return FALSE;
		}

		pReplayNote = (KReplayNote*)pNoteData;
		m_NoteIndex.insert(NoteIndex::value_type(pReplayNote->dwCurIns, pReplayNote->dwNoteID));
		m_Note.insert(Note::value_type(pReplayNote->dwNoteID, pNoteData));
	}

	return TRUE;
}

inline void GameVideo::CRCBlockToFile(BYTE *pBuf, DWORD dwLen, FILE *fp, DWORD &dwRstLen)
{
	CRCDataLen cd;

	cd.dwCRC = 0;
	cd.dwDataLen = dwLen;
	cd.dwCRC = CRC32(cd.dwCRC, &cd.dwDataLen, sizeof(cd.dwDataLen));
	fwrite(&cd, 1, sizeof(cd), fp);

	DWORD dwCRC = 0;
	dwCRC = CRC32(dwCRC, pBuf, dwLen);
	fwrite(&dwCRC, 1, sizeof(dwCRC), fp);
	fwrite(pBuf, 1, dwLen, fp);

	dwRstLen = sizeof(cd) + sizeof(dwCRC) + dwLen;
}

BOOL GameVideo::IsNoteValid(BYTE *pNote)
{
	if(pNote)
	{
		KReplayNote *pPlayNote = (KReplayNote*)pNote;

		extern CChatFilter g_ChatFilter;

		return g_ChatFilter.IsTextPass(pPlayNote->szContent);
	}
	else
	{
		return FALSE;
	}
}

BOOL GameVideo::LoadInitialConfig(const char *szUiConfigFile)
{
	// 仅load一次
	static bool bLoad = false;

	if(bLoad)
	{
		return TRUE;
	}

	BOOL	bRet = FALSE;

	// 如果配置文件存在，则将配置文件的内容写入录像尾部

	if(_access(szUiConfigFile, 0) != -1)
	{
		FILE *fpConfigFile = fopen(szUiConfigFile, "rb");

		if(fpConfigFile)
		{
			DWORD	dwConfigLen;
			fseek(fpConfigFile, 0, SEEK_END);
			dwConfigLen = ftell(fpConfigFile);
			fseek(fpConfigFile, 0, SEEK_SET);

			if(m_pUiConfigBuf)
			{
				delete m_pUiConfigBuf;
				m_pUiConfigBuf = NULL;
			}

			m_pUiConfigBuf = new BYTE[dwConfigLen];
			
			if(m_pUiConfigBuf)
			{
				if(fread(m_pUiConfigBuf, 1, dwConfigLen, fpConfigFile) != dwConfigLen)
				{
					bRet = FALSE;
				}
				else
				{
					bRet = TRUE;
					m_nUiSettingLen = dwConfigLen;
				}
			}
		}
		else
		{
			bRet = FALSE;
		}
	}
	else
	{
		bRet = TRUE;
	}

	bLoad = true;
	return bRet;
}

BOOL GameVideo::CreateConfigFileFromVideo()
{
	CRCDataLen cd;

	if(fread(&cd, 1, sizeof(cd), m_pInsFile) != sizeof(cd))
	{
		return FALSE;
	}

	DWORD dwNewCRC = 0;
	dwNewCRC = CRC32(dwNewCRC, &cd.dwDataLen, sizeof(cd.dwDataLen));

	if(cd.dwCRC != dwNewCRC)
	{
		return FALSE;		
	}

	if(m_pUiConfigBuf)
	{
		delete m_pUiConfigBuf;
		m_pUiConfigBuf = NULL;
	}

	m_pUiConfigBuf = new BYTE[cd.dwDataLen];

	if(NULL == m_pUiConfigBuf)
	{
		return FALSE;	
	}

	DWORD	dwOldCRC;
	fread(&dwOldCRC, 1, sizeof(dwOldCRC), m_pInsFile);
	fread(m_pUiConfigBuf, 1, cd.dwDataLen, m_pInsFile);

	dwNewCRC = 0;
	dwNewCRC = CRC32(dwNewCRC, m_pUiConfigBuf, cd.dwDataLen);

	if(dwOldCRC != dwNewCRC)
	{
		return FALSE;
	}

	char	szPath[MAX_PATH];

	MakeDir(UI_USER_DATA_FOLDER);
	sprintf(szPath, "%s/%s", UI_USER_DATA_FOLDER, UI_VIDEO_SETTING_DIR);
	MakeDir(szPath);

	sprintf(szPath, "%s/%s/%s", UI_USER_DATA_FOLDER, UI_VIDEO_SETTING_DIR, UI_PRIVATE_SETTING_FILE);
	FILE *fpConfig = fopen(szPath, "wb");

	if(fpConfig)
	{
		fwrite(m_pUiConfigBuf, 1, cd.dwDataLen, fpConfig);
		fclose(fpConfig);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

inline BOOL GameVideo::MakeDir(const char *szDir)
{
	BOOL bRet;

	if(_access(szDir, 0) == -1)
	{
		if(_mkdir(szDir) == -1)
		{
			bRet = FALSE;
		}
		else
		{
			bRet = TRUE;
		}
	}
	else
	{
		bRet = TRUE;
	}	

	return bRet;
}

void GameVideo::DeleteNote(DWORD dwNoteID)
{
	// 这里没有相应的更新m_NoteIndex，因为这个数据不存盘，而且，要
	// 删除一条还需要顺序遍历，效率较低，m_Note中数据不同步
	// 也没有关系，即使要访问已被删除的注释，找不到就返回NULL，并不会
	// 影响最终结果

	Note::iterator iter = m_Note.find(dwNoteID);

	if(iter != m_Note.end())
	{
		delete iter->second;
		m_Note.erase(iter);
	}
}	

void GameVideo::ReleaseNoteResource()
{
	if(m_Note.empty())
	{
		return;
	}

	Note::iterator iterNote;
	Note::iterator iterEnd(m_Note.end());

	for(iterNote = m_Note.begin(); iterNote != m_Note.end(); ++iterNote)
	{
		delete iterNote->second;
	}

	m_Note.clear();
}

int GameVideo::GetBuildingSchedule()
{
	int nType = 0;
	int nBuildingSchedule = 0;
	
	if(m_nPlayBufDataLen - m_nPlayBufUsed < sizeof(nBuildingSchedule) + enum_InsCode_Size)
	{
		if(!FillBuf())
		{
			return nBuildingSchedule;
		}
	}
	
	GetDataFromBuf(m_bPlayBuf, &nType, enum_InsCode_Size);
	GetDataFromBuf(m_bPlayBuf, &nBuildingSchedule, sizeof(nBuildingSchedule));

#ifdef _GameVideoDebug
	m_playDebug << "GetBuildingSchedule	" << nType << "	" << nBuildingSchedule << endl;
#endif

	return nBuildingSchedule;
}

void GameVideo::GetMainSchedule(DWORD &dwPassed, DWORD &dwOffset)
{
	int nType = 0;
	
	dwPassed = 0;
	dwOffset = 0;
	
	if(m_nPlayBufDataLen - m_nPlayBufUsed < sizeof(dwPassed) * 2 + enum_InsCode_Size)
	{
		if(!FillBuf())
		{
			return;
		}
	}
	
	GetDataFromBuf(m_bPlayBuf, &nType, enum_InsCode_Size);
	GetDataFromBuf(m_bPlayBuf, &dwPassed, sizeof(dwPassed));
	GetDataFromBuf(m_bPlayBuf, &dwOffset, sizeof(dwOffset));

#ifdef _GameVideoDebug
	m_playDebug << "GetMainSchedule	" << nType << "	" << dwPassed << "	" << dwOffset << endl;
#endif
}