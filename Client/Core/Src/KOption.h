//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 06/28/2007 10:06
//      File_base        : KOption
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KOptionH
#define	KOptionH

#ifndef _SERVER

#include "KNpc.h"
#include "KSubWorldSet.h"
#include "iRepresentshell.h"
#include "math.h"

extern struct iRepresentShell*	g_pRepresent;

class KOption
{
public:
	KOption();
	~KOption(){}

public:
	inline int	GetSndVolume( void );
	inline int	GetSndVolume( float dist );
	inline void	SetSndVolume( int nSndVolume );
	inline int	GetMusicVolume( void );
	inline void	SetMusicVolume( int nMusicVolume );
	inline int	GetGamma( void );
	inline void	SetGamma( int nGamma );
	inline int	GetMaxPlayersInScreen( void );
	inline void	SetMaxPlayersInScreen(int nMax);
	inline bool IsDrawNpc( void );
	inline void SetIfDrawNpc( bool bOn );
	inline bool IsDrawPlayer( void );
	inline void SetIfDrawPlayer( bool bOn );

	inline bool IsDrawGround( void );
	inline void SetDrawGround( bool bOn );

	inline bool IsDrawShadow( void );
	inline void SetDrawShadow( bool bOn );
	inline bool IsDrawSmallObj( void );
	inline void SetDrawSmallObj( bool bOn );
	inline bool IsDrawLargeObj( void );
	inline void SetDrawLargeObj( bool bOn );
	
private:
	int		m_nSndVolume;
	int		m_nMusicVolume;
	int		m_nGamma;
	int		m_nMaxPlayersInScreen;
	bool	m_bDrawNpc;
	bool	m_bDrawPlayer;
	bool	m_bDrawShadow;
	bool	m_bDrawGround;
	bool	m_bDrawSmallObj;
	bool	m_bDrawLargeObj;
};

inline int	KOption::GetSndVolume( void ) 
{
	return m_nSndVolume;
}

inline int	KOption::GetSndVolume( float dist )
{
	int nVol = (int)((dist/600.0f) * 3000);
	nVol = m_nSndVolume - nVol;
	if ( nVol > 0 )
		return 0;
	if ( nVol < -10000 )
		return -10000;

	return nVol;
}

inline void	KOption::SetSndVolume( int nSndVolume )
{
	/*if (nSndVolume <= 3)
		m_nSndVolume = 0;
	else if (nSndVolume >= 100)
		m_nSndVolume = 100;
	else if (nSndVolume >= 40)
		m_nSndVolume = 80 + (nSndVolume - 50) * 2 / 5;
	else
		m_nSndVolume = 40 + nSndVolume;
	//*/
	m_nSndVolume = (nSndVolume > 0) ? (LONG)(log10((float)nSndVolume/100.0f) * 2000.0f) : -10000;
}

inline int	KOption::GetMusicVolume( void ) 
{
	return m_nMusicVolume; 
}

inline void	KOption::SetMusicVolume( int nMusicVolume )
{
	if (nMusicVolume < 0)
		m_nMusicVolume = 0;
	else if (nMusicVolume > 100)
		m_nMusicVolume = 100;
	else
		m_nMusicVolume = nMusicVolume;

	g_SubWorldSet.m_cMusic.SetGameVolume(m_nMusicVolume);
}

inline int	KOption::GetGamma( void ) 
{
	return m_nGamma; 
}

inline void	KOption::SetGamma( int nGamma )
{
	m_nGamma = nGamma;
	g_pRepresent->SetGamma(nGamma);
}

inline int	KOption::GetMaxPlayersInScreen( void )
{ 
	return m_nMaxPlayersInScreen; 
}

inline void	KOption::SetMaxPlayersInScreen(int nMax)
{ 
	if ((nMax >= 0)&&(nMax < MAX_NPC))
		m_nMaxPlayersInScreen = nMax;
}

inline bool KOption::IsDrawNpc( void )
{
	return m_bDrawNpc;
}

inline void KOption::SetIfDrawNpc( bool bOn )
{
	m_bDrawNpc = bOn;
}

inline bool KOption::IsDrawPlayer( void )
{
	return m_bDrawPlayer;
}

inline void KOption::SetIfDrawPlayer( bool bOn )
{
	m_bDrawPlayer = bOn;
}

inline bool KOption::IsDrawShadow( void )
{
	return m_bDrawShadow;
}

inline void KOption::SetDrawShadow( bool bOn )
{
	m_bDrawShadow = bOn;
}

inline bool KOption::IsDrawGround( void )
{
	return m_bDrawGround;
}

inline void KOption::SetDrawGround( bool bOn )
{
	m_bDrawGround = bOn;
}

inline bool KOption::IsDrawSmallObj( void )
{
	return m_bDrawSmallObj;
}

inline void KOption::SetDrawSmallObj( bool bOn )
{
	m_bDrawSmallObj = bOn;
}

inline bool KOption::IsDrawLargeObj( void )
{
	return m_bDrawLargeObj;
}

inline void KOption::SetDrawLargeObj( bool bOn )
{
	m_bDrawLargeObj = bOn;
}

extern KOption Option;

#endif

#endif