//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/22/2007 11:29
//      File_base        : KMissle
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef	KMissleH
#define KMissleH

#define MAX_MISSLE 1200
#include "KCore.h"
#include "SkillDef.h"
#include "KObj.h"
#include "KMissleRes.h"
#include "KNode.h"
#include "KITabFile.h"
#include "KNpcSet.h"
#include "Scene/ObstacleDef.h"
#include "KSubWorld.h"
#include "KIndexNode.h"
#include "GameDataDef.h"
#include "KSkills.h"

enum 
{
	Interupt_None,
	Interupt_EndNewMissleWhenMove,
	Interupt_EndOldMissleLifeWhenMove,
};

class KMissle  
{
	friend class KSkill;
	friend class KMissleSet;
public:	
	KMissle( void );
	virtual ~KMissle( void );
public:
	int					Activate( void );
	void				Paint( void );
	int					GetMissleHeight( void )
	{
		if ( m_pMissleTemplate == NULL )
		{
			return 0;				 
		}
		else
		{
			return m_pMissleTemplate->m_nHeight;
		}
	}

	int					GetMissleSkillID( void )
	{
		if ( m_pMissleTemplate == NULL )
		{
			return 0;				 
		}
		else
		{
			return m_pMissleTemplate->m_nSkillId;
		}
	}
	int					GetMissleMoveKind( void )
	{
		if ( m_pMissleTemplate == NULL )
		{
			return 0;				 
		}
		else
		{
			return m_pMissleTemplate->m_eMoveKind;
		}
	}

	int		GetMissleZAcceleration()
	{
		if ( m_pMissleTemplate == NULL )
		{
			return 0;				 
		}
		else
		{
			return m_pMissleTemplate->m_nZAcceleration;
		}
	}
			
	
private:
	KMissle&			operator=(KMissleTemplate* Missle);
	void				Release( void );
	void				OnVanish( void );								//即将消失
	void				OnFly( void );									//飞行过程中
	void				DoVanish( void );
	void				DoCollision(int nTargetIdx);
	void				DoWait( void );
	void				DoFly( void );
	BOOL				PrePareFly( void );
	inline void			ZAxisMove( void );
	inline BOOL			TestBarrier( void );							//TRUE表示遇到障碍，FALSE表示未遇到，一切正常
	int					CheckNearestCollision( void );
	int					CheckCollision( void );							//检测是否碰撞,1表示正常碰撞到物体，0表示未碰撞到任何物体,-1表示落地
	int					ProcessCollision( void );						//处理碰撞
	BOOL				CheckBeyondRegion(int nDOffsetX, 
							int nDOffsetY );							//检测是否越界FALSE表示越到一个无效的位置，TRUE表示OK
	BOOL				GetOffsetAxis( int nSubWorld, 
							int nSrcRegionId, 
							int nSrcMapX, 
							int nSrcMapY,
							int nOffsetMapX, 
							int nOffsetMapY, 
							int& nDesRegionId, 
							int& nDesMapX, 
							int& nDesMapY );
	BOOL				CreateSpecialEffect( eMissleStatus eStatus,  
							int nPX,
							int nPY,
							int nPZ,
							int nNpcIndex = 0 );
	void				GetMpsPos( int *pPosX,
							int *pPosY );
	int					ProcessCollision( int nLauncherIdx,
							int nRegionId,
							int nMapX,
							int nMapY,
							int nRange,
							int eRelation );	
	BOOL				GetRelativePos( int& nMpsX, 
							int& nMpsY );								//获得相对参考点坐标,比如有时候子碟是以发送者为参靠点移动的.，而有的时候则是产生点为参靠点的
public:
	KIndexNode			m_Node;
	int					m_nRegionId;									//	区域ID
private:

	//	子弹设定文件获得的数据
	KMissleTemplate*	m_pMissleTemplate;
	KMissleRes			m_MissleRes;									//	子弹的资源
	// 由模板得来又可以被改变的值
	int					m_nCurSkillId;
	int					m_nCurLifeTime;
	int					m_eCurMoveKind;
	int					m_nCurSpeed;
	int					m_nCurHeightSpeed;
	//	由技能获得的数据

	
	BOOL				m_bClientSend;									//	是否需要
	int					m_eRelation;									//	目标与发射者的关系	
	INT					m_nInteruptTypeWhenMove;						//子弹的激活是否受发送者的移动而中止
	BOOL				m_bHeelAtParent;								//	当子弹实际激活时，位置根据父当前位置而确定,而不是由产生那刻parent位置决定
	int					m_nLauncherSrcPX;								//  记录发送者发送时所站的位置，此用于当其移动，则子碟消亡的那类技能
	int					m_nLauncherSrcPY;
	BOOL				m_bFlyEvent;									//	整个飞行过程中的
	int					m_nFlyEventTime;
	int					m_nCurrentLife;									//	当前生命时间
	int					m_nStartLifeTime;								//	当技能发生后，第几帧开始
	int					m_nCurrentMapX;									//	当前的X坐标
	int					m_nCurrentMapY;									//	当前的Y坐标
	int					m_nCurrentMapZ;									//	当前的Z坐标
	int					m_nXOffset;										//	当前的X方向偏移
	int					m_nYOffset;										//	当前的Y方向偏移
	int					m_nRefPX;										//	子弹产生时的初始位置
	int					m_nRefPY;	
	int					m_nDesMapX;										//单颗子单时，目的坐标
	int					m_nDesMapY;		
	BOOL				m_bNeedReclaim;									//是否已纠正过一次子单的方向问题
	//单一飞行子单时，精确命中！
	int					m_nXFactor;
	int					m_nYFactor;	
	int					m_nFollowNpcIdx;								//	跟随谁
	DWORD				m_dwFollowNpcID;								//	
	int					m_nLauncher;									//	发射者在NpcSet中的Index
	DWORD				m_dwLauncherId;									//	发射者的唯一ID
	int					m_nParentMissleIndex;							// if 0 then means parent is npclauncher
	//	自生成的动态数据	
	int					m_nMissleIdx;
	eMissleStatus		m_eMissleStatus;								//	子弹当前的状态
	int					m_nSubWorldId;									//	子世界ID
	int					m_nFirstReclaimTime;
	int					m_nEndReclaimTime;
	int					m_nTempParam1;									//	运行期使用的参数
	int					m_nTempParam2;
	int					m_eRelativePosType;
	int					m_nMissleTotalMpsX;								//子弹总的相对偏移
	int					m_nMissleTotalMpsY;
	int					m_nDirIndex;									//	当前运动方向的索引
	int					m_nDir;											//	当前的运行方向
	int					m_nAngle;										//	
	DWORD				m_dwBornTime;									//	该子弹产生时的时间
	unsigned int		m_SceneID;
};

inline void	KMissle::ZAxisMove( void )
{
	if (m_pMissleTemplate && m_pMissleTemplate->m_nZAcceleration)
	{
		m_pMissleTemplate->m_nHeight += m_pMissleTemplate->m_nHeightSpeed;
		if (m_pMissleTemplate->m_nHeight < 0) m_pMissleTemplate->m_nHeight = 0;
		m_pMissleTemplate->m_nHeightSpeed -= m_pMissleTemplate->m_nZAcceleration;
		m_nCurrentMapZ = m_pMissleTemplate->m_nHeight >> 10;
	}
}
//TRUE表示遇到障碍，FALSE表示未遇到，一切正常
inline BOOL KMissle::TestBarrier( void )
{
	int nBarrierKind = SubWorld[m_nSubWorldId].TestBarrier(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, 0, 0);
	if (nBarrierKind == Obstacle_Normal || nBarrierKind == Obstacle_Jump)
	{
		return TRUE;
	}
	return FALSE;
}

extern KMissle			Missle[MAX_MISSLE];

#endif
