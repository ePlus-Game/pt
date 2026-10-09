//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/06/2007 18:58
//      File_base        : KMapMusic
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 说明：游戏世界的音乐		
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _SERVER

#define		defONE_MAP_MAX_MUSIC			4											//!< 每张地图最大可能音乐数
#define		defGAME_TIME_ONE_DAY			(1440 * 120)								//!< 游戏每天时间的长度
#define		defMUSIC_MAX_STAGE				32											//!< 
#define		defMUSIC_STATE_MAX_MAP			64
#define		defMUSIC_STATE_MAX_MUSIC		16



/*!
\brief
	游戏世界的音乐	
*/
class	KMapMusic
{
private:
	enum
	{
		enumMUSIC_STATE_STOP = 0,
		enumMUSIC_STATE_AUTO,
		enumMUSIC_STATE_SCRIPT,
		enumMUSIC_STATE_NUM,
	};

	struct	KMapMusicInfo
	{
		int		m_nVolume;																//!< 音乐声音大小
		int		m_nStartTime;															//!< 音乐在游戏时间的什么阶段播放：起始时间
		int		m_nEndTime;																//!< 音乐在游戏时间的什么阶段播放：结束时间
		char	m_szFileName[FILE_NAME_LENGTH];											//!< 音乐文件名(含从游戏跟目录开始的完整游戏路径)
	};

	struct	KMapAllMusic
	{
		KMapAllMusic() 
		{
			m_nMapID = 0; 
			ZeroMemory(m_sMusic, sizeof(m_sMusic));
		}
		int				m_nMapID;														//!< 地图id
		KMapMusicInfo	m_sMusic[defONE_MAP_MAX_MUSIC];									//!< 本地图所有音乐的参数数据
	};

	struct	KMapFightMusic
	{
		int		m_nMusicNum;
		int		m_nMapID[defMUSIC_STATE_MAX_MAP];
		char	m_szMusicName[defMUSIC_STATE_MAX_MUSIC][FILE_NAME_LENGTH];
	};

public:
	KMapMusic();
	~KMapMusic();

public:	
	void			Init			( void											);				
	void			Start			( int nMapID, int nGameTime, BOOL bFightMode	);	//!< 进入游戏或者音乐设定打开的时候，播放音乐
	void			Start			( void											);
	void			Play			( int nMapID, int nGameTime, BOOL bFightMode	);
	void			ScriptPlay		( char *lpszMusicName							);	//!< 用脚本播放音乐
	void			Stop			( void											);
	void			SetGameVolume	( int nVolume									);	//!< 设定游戏音乐总体音量大小(0 -- 100)
	void			AdjustVolume	( void											);

private:
	void			Release			( void											);			
	void			LoadSetFile		( void											);
	void			PlayInFightMode	( int nMapID, int nGameTime, BOOL bFightMode	);
	void			PlayInNormalMode( int nMapID, int nGameTime, BOOL bFightMode	);

private:
	int				m_nState;															//!< 当前状态
	int				m_nCurMapID;														//!< 当前地图id
	int				m_nCurInfoNo;														//!< 当前地图id在信息数据的位置
	int				m_nCurMusicNo;														//!< 当前正在播放本地图第几首音乐
	int				m_nGameVolume;														//!< 当前游戏设定音乐声音大小(0(无声) -- 100(正常))
	char			m_szCurName[FILE_NAME_LENGTH];										//!< 当前音乐文件名
	KMapAllMusic	*m_pMusicInfo;														//!< 所有地图的音乐信息
	int				m_nInfoSize;														//!< 所有地图音乐信息数据的大小(单位:sizeof(KMapAllMusic))
	int				m_nLoadFlag;														//!< 游戏音乐信息是否已经载入
	KMapFightMusic	m_sFightMusic[defMUSIC_MAX_STAGE];
	BOOL			m_bFightMode;
	int				m_nCurStage;

};


#endif