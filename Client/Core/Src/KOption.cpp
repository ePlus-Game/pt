//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 06/28/2007 10:06
//      File_base        : KOption
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KOption.h"

#ifndef _SERVER

KOption	Option;

KOption::KOption()
{
	m_nMaxPlayersInScreen = MAX_NPC;
	m_nSndVolume	= 100;
	m_nMusicVolume	= 100;
	m_nGamma		= 0;
	m_bDrawNpc		= true;
	m_bDrawPlayer	= true;
	m_bDrawShadow	= true;
	m_bDrawGround	= true;
	m_bDrawSmallObj	= true;
	m_bDrawLargeObj	= true;
}


#endif