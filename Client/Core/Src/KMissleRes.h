//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/22/2007 11:40
//      File_base        : KMissleRes
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      :	一、维护子弹的资源，包括图像、声音等资源的加载、删除
//							二、维护子弹的换帧、跳帧，同步问题
//							三、处理子弹的图像显示、声音播放等问题
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef __KMISSLERES_H__
#define __KMISSLERES_H__

#include "KEngine.h"
#include "KCore.h"
#include "SkillDef.h"
#include "KRepresentUnit.h"

class KMissleRes
{
	friend class KMissle;
	friend class KSkill;
public:
	KMissleRes(	);
	~KMissleRes();
public:
	BOOL		Init( void );

	void		Clear( void );

	void		LoadResource( int nStatus, 
					char * MissleImage, 
					char * MissleSound );

	int			Draw( int nStatus, 
					int nX, 
					int nY , 
					int nZ, 
					int nDir,  
					int nAllTime, 
					int nCurLifeTime );

	void		PlaySound(int nStatus, 
					int nX, 
					int nY, 
					int nLoop );
	
	inline BOOL	SpecialMovieIsAllEnd( void )
	{
		return (NULL == m_SkillSpecialList.GetHead());
	};
private:
	int			GetSndVolume( int nVol );

private:
	int			m_nMissleIdx;							// 在Missle数组中的索引
	KList		m_SkillSpecialList;						// 子弹附加效果列表
	BOOL		m_bHaveEnd;								// 本次播放是否结束
	BOOL		m_bLoopAnim;							// 是否循环播放动画
	TMissleRes	m_MissleRes[MAX_MISSLE_STATUS * 2];		// 几种状态下的资源情况	
	KCacheNode* m_pSndNode ;
	int			m_nLastSndIndex;	
	BOOL		m_bSubLoop;
	int			m_nSubStart;							// 子循环的起始帧
	int			m_nSubStop;								// 子循环的结束帧
	KRUImage	m_RUImage[MAX_MISSLE_STATUS];			// 图
};
#endif //__KMISSLERES_H__