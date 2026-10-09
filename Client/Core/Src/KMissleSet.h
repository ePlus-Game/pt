//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/22/2007 10:57
//      File_base        : KMissleSet
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KMISSLESET_H
#define KMISSLESET_H
#include "KMissle.h"
#include "KLinkArray.h"

class KMissleSet
{
public:
	void						Init( void );
	int							Add( int SubWorldId, 
									int regionid, 
									int x , 
									int y , 
									int dx = 0 , 
									int dy = 0 );
	int							Add( int SubWorldId, 
									int px, 
									int py );
	void						Remove( int nIndex );

	static	KMissleTemplate*	GetMissleTemplate( int MisslesTemplate );

private:
	int							FindFree( void );
	int							GetCount( void );
	BOOL						GetInfoFromTabFile( int nMissleId );
	BOOL						GetInfoFromTabFile( KITabFile * pTabFile,
									int nMissleId );
	/*
	int		CreateMissile( int nSkillId, 
				int nMissleId, 
				int nLauncher,  
				int nTargetId ,
				int nSubWorldId, 
				int nPX, 
				int nPY,
				int nDir);
	void	Draw( void );
	int		Activate( void );
	void	ClearMissles( void );
	//*/
private:
	KLinkArray					m_FreeIdx;							//	可用表
	KLinkArray					m_UseIdx;							//	已用表
	static KMissleTemplate		m_MisslesTemplate[MAX_MISSLESTYLE]; //  missle模板
};

extern KMissleSet MissleSet;

#endif
