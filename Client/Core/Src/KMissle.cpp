//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/22/2007 11:29
//      File_base        : KMissle
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KMissle.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMath.h"
#include <math.h>
#include "KSkillSpecial.h"
#include "iRepresentshell.h"
#include "Scene\KScenePlaceC.h"
#include "ImgRef.h"
#include "Scene/ObstacleDef.h"
#include "KPlayer.h"
#include "KMissleSet.h"

#include "KBuilding.h" // lixuewu

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "KSG_StringProcess.h"
#endif




//每个格子的像素长宽
#define CellWidth	(REGION_CELL_SIZE_X << 10)	//(SubWorld[m_nSubWorldId].m_nCellWidth << 10)
#define CellHeight	(REGION_CELL_SIZE_Y << 10)	//(SubWorld[m_nSubWorldId].m_nCellHeight << 10)

//每个region格点长宽
#define RegionWidth	REGION_CELL_WIDTH	//(SubWorld[m_nSubWorldId].m_nRegionWidth)
#define RegionHeight	REGION_CELL_HEIGHT	//(SubWorld[m_nSubWorldId].m_nRegionHeight)

#define CurRegion		SubWorld[m_nSubWorldId].m_Region[m_nRegionId]
#define CurSubWorld		SubWorld[m_nSubWorldId]

#define LeftRegion(nRegionId)	SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[2]
#define RightRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[6]
#define UpRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[4]
#define DownRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[0]

//随机移动魔法的左右偏移表
int g_nRandMissleTab[100] = {0	};

KMissle Missle[MAX_MISSLE];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
KMissle::KMissle()
{
	m_pMissleTemplate = NULL;
	m_nRegionId = -1;
	m_nLauncher = -1;
	m_nCurrentLife = 0;
	m_nTempParam1 = 0;
	m_nTempParam2 = 0;
	m_nFirstReclaimTime = 0;
	m_nEndReclaimTime = 0;
	m_nMissleTotalMpsX = m_nMissleTotalMpsY = 0;	
}

void KMissle::Release()
{
	m_nCurrentLife = 0;
	m_nMissleIdx = -1;
	m_nRegionId = -1;
	m_nLauncher = -1;
	g_ScenePlace.RemoveObject(CGOG_MISSLE, m_nMissleIdx, m_SceneID);
	m_MissleRes.Clear();
	m_nFollowNpcIdx = 0;
}

KMissle::~KMissle()
{
	
}

/*!*****************************************************************************
// Function		: KMissle::Activate
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
int KMissle::Activate()
{	
	if (m_nMissleIdx <= 0 || m_nRegionId < 0)
	{
		return  0 ;
	}
	
	_ASSERT(m_nLauncher > 0);
	if (m_nLauncher <= 0)
		return 0;
	
	//子弹的主人已经离开，So 子弹消亡
	if (!Npc[m_nLauncher].IsMatch(m_dwLauncherId) || Npc[m_nLauncher].m_SubWorldIndex != m_nSubWorldId || Npc[m_nLauncher].m_RegionIndex < 0)
	{
		DoVanish(); 
		return 0;	
	}
	
	//跟踪的目标人物已经不在该地图上时，自动清空
	if (m_nFollowNpcIdx > 0)
	{
		if (!Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID) || Npc[m_nFollowNpcIdx].m_SubWorldIndex != m_nSubWorldId)
		{
			m_nFollowNpcIdx = 0;
			DoVanish();
		}
	}
	
	eMissleStatus eLastStatus = m_eMissleStatus;
	
	// 因为子弹的速度调快了，可以超过32，碰撞计算可能会不准确了
	// 懒得改碰撞了，在这里判断如果距离比较接近，就直接消亡爆炸，省事儿
	bool bIsNearToExplode = false;
	if(m_pMissleTemplate && m_nFollowNpcIdx > 0 && m_nCurSpeed > 0 && m_eMissleStatus != MS_DoVanish)
	{
		int nMissleX;
		int nMissleY;
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nMissleX, &nMissleY);

		int nTargetX;
		int nTargetY;
		Npc[m_nFollowNpcIdx].GetMpsPos(&nTargetX, &nTargetY);

		int nDisX = nTargetX - nMissleX;
		int nDisY = nTargetY - nMissleY;
		int nDistance = sqrt(nDisX * nDisX + nDisY * nDisY);
		
		if(nDistance <= m_nCurSpeed / 2)
		{
			bIsNearToExplode = true;
//			m_nCurrentLife = m_nLifeTime;
		}
	}

	//如果当前状态是子弹生命正常结束正准备消亡状态时，而不是消亡中或者已碰撞中
	if ( (m_pMissleTemplate && m_nCurrentLife >= m_nCurLifeTime && m_eMissleStatus != MS_DoVanish) || bIsNearToExplode ) 
	{
		if (m_pMissleTemplate->m_bAutoExplode)
		{
			ProcessCollision();//处理碰撞
		}

		int nSrcX4 = 0 ;
		int nSrcY4 = 0 ;
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX4, &nSrcY4);
		CreateSpecialEffect(MS_DoVanish, nSrcX4, nSrcY4, m_nCurrentMapZ);

		DoVanish();
	}
	
	if (m_nCurrentLife == m_nStartLifeTime && m_eMissleStatus != MS_DoVanish)	
	{
		if (PrePareFly())
		{
			int nSrcX2 = 0 ;
			int nSrcY2 = 0 ;
			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);
			m_MissleRes.PlaySound(MS_DoFly, nSrcX2, nSrcY2, 0);
			//CreateSpecialEffect(MS_DoFly, nSrcX2, nSrcY2, m_nCurrentMapZ);
			DoFly();
		}
		else
			DoVanish();
	}
	
	switch(m_eMissleStatus)
	{
	case MS_DoWait:
		{
			
		}
		break;
	case MS_DoFly:
		{
			OnFly();
		}
		break;
	case MS_DoCollision:
		{
			
		}
		break;
	case MS_DoVanish:
		{
			OnVanish();
		}
		break;
	}
	
	//子弹未消亡掉
	if (m_nMissleIdx > 0)
	{
		int nSrcX;
		int nSrcY;
		
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
		g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleIdx, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT);
	}

	m_nCurrentLife ++;

	//对于客户端，直到子弹及其产生的效果全部播放完才终止并删除掉!
	if (m_MissleRes.m_bHaveEnd && (m_MissleRes.SpecialMovieIsAllEnd()))
	{
		if (m_nRegionId >= 0)
		{
			SubWorld[m_nSubWorldId].m_Region[m_nRegionId].RemoveMissle(m_nMissleIdx);
			MissleSet.Remove(m_nMissleIdx);
		}
	}

	return 1;
}

// 1表示正常碰撞到物体，0表示未碰撞到任何物体, -1表示落地
int KMissle::CheckCollision()
{
	if (m_nCurrentMapZ <= MISSLE_MIN_COLLISION_ZHEIGHT) 
	{
		return -1;
	}
	
	//子弹在高于一定高度时，不处理越界碰撞问题
	if (m_nCurrentMapZ > MISSLE_MAX_COLLISION_ZHEIGHT) return 0;
	
	if (m_nRegionId < 0) 
	{
		return -1;
	}

	int nAbsX = 0;
	int nAbsY = 0;
	const int nCellWidth = CellWidth;
	const int nCellHeight = CellHeight;
	_ASSERT(nCellWidth > 0 && nCellHeight > 0);
	int nRMx = 0;
	int nRMy = 0;
	int nSearchRegion = 0;
	int nNpcIdx = 0;
	int nDX = 0;
	int nDY = 0;
	int nNpcOffsetX = 0;
	int nNpcOffsetY = 0;
	BOOL bCollision = FALSE;
	
	int nColRegion = m_nRegionId;
	int nColMapX = m_nCurrentMapX;
	int nColMapY = m_nCurrentMapY;
		
	if (m_pMissleTemplate && m_pMissleTemplate->m_nCollideRange == 1)
	{
		//该子单需要在某个时段将碰撞检查的范围增强
		if (m_bNeedReclaim && m_nCurrentLife >= m_nFirstReclaimTime && m_nCurrentLife <= m_nEndReclaimTime)
		{
			if (m_nCurrentLife == m_nEndReclaimTime) 
				m_bNeedReclaim = FALSE;
			nNpcIdx = 	CheckNearestCollision();
		}
		else
		{
			nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nColRegion].FindNpc(nColMapX, nColMapY, m_nLauncher, m_eRelation);
		}

		if (nNpcIdx > 0)
		{ 
			if (m_pMissleTemplate->m_nDamageRange == 1)//在目标Npc处碰撞
				ProcessCollision(m_nLauncher, Npc[nNpcIdx].m_RegionIndex , Npc[nNpcIdx].GetMapX(), Npc[nNpcIdx].GetMapY(), m_pMissleTemplate->m_nDamageRange , m_eRelation);
			else
				ProcessCollision();//在子弹位置处理碰撞

			// Add by chenshanglin on [2006-3-16 10:18]
			DoCollision(nNpcIdx);//子弹作碰撞后的效果
			// Add end
			
			return 1;
		}
	}
	else
	{
		if ( m_pMissleTemplate )
		{
			for (int i = -m_pMissleTemplate->m_nCollideRange; i <= m_pMissleTemplate->m_nCollideRange; i ++)
			{
				for (int j = -m_pMissleTemplate->m_nCollideRange; j <= m_pMissleTemplate->m_nCollideRange; j ++)
				{
					if (!GetOffsetAxis(m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, i , j , nSearchRegion, nRMx, nRMy))
						continue;
					
					_ASSERT(nSearchRegion >= 0);
					nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);
					if (nNpcIdx > 0)
					{					
						if (m_pMissleTemplate->m_nDamageRange == 1)//在目标Npc处碰撞
							ProcessCollision(m_nLauncher, Npc[nNpcIdx].m_RegionIndex , Npc[nNpcIdx].GetMapX(), Npc[nNpcIdx].GetMapY(), m_pMissleTemplate->m_nDamageRange , m_eRelation);
						else
							ProcessCollision();//在子弹位置处理碰撞

						DoCollision(nNpcIdx);//子弹作碰撞后的效果
						
						return 1;
					}
				}
			}
		}

	}
	
	return 0;
}

void KMissle::OnFly()
{
	if ( m_pMissleTemplate == NULL )
	{
		return;
	}
	if(m_nCurSpeed > 0)
		m_nCurSpeed += m_pMissleTemplate->m_nAcceleration;		

	if (m_nInteruptTypeWhenMove)
	{
		//当发送者位置移动了，不仅正从do_wait状态到do_fly状态的新子弹被消失掉
		//而且已进入dofly状态的旧的所属子弹也要强制消失掉
		if (m_nInteruptTypeWhenMove == Interupt_EndOldMissleLifeWhenMove)
		{
			int nPX, nPY;
			Npc[m_nLauncher].GetMpsPos(&nPX, &nPY);
			if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)
			{
				
				int nSrcX2 = 0 ;
				int nSrcY2 = 0 ;
				SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);
				CreateSpecialEffect(MS_DoVanish, nSrcX2, nSrcY2, m_nCurrentMapZ);
				
				DoVanish();
				return ;
			}
		}
	}
	
	//检测当前位置是否有障碍,如果有障碍则消亡
	if (TestBarrier()) 
	{
		int nSrcX3 = 0 ;
		int nSrcY3 = 0 ;
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX3, &nSrcY3);
		CreateSpecialEffect(MS_DoVanish, nSrcX3, nSrcY3, m_nCurrentMapZ);
		DoVanish();
		return;
	}
	
	int nDOffsetX = 0;
	int nDOffsetY = 0;
	
	//计算Z轴的运动
	ZAxisMove();
	
	switch(this->m_pMissleTemplate->m_eMoveKind)
	{
	case	MISSLE_MMK_Stand:							//	原地
		{
			
		}
		break;
	case	MISSLE_MMK_Parabola:						//	抛物线
	case	MISSLE_MMK_Line:							//	直线飞行
		{
			nDOffsetX    = (m_nCurSpeed * m_nXFactor);
			nDOffsetY	 = (m_nCurSpeed * m_nYFactor);
		}
		break;
	case MISSLE_MMK_RollBack:
		{
			if (!m_nTempParam1)	
			{
				if (m_nTempParam2 <= m_nCurrentLife)
				{
					m_nXFactor = -m_nXFactor;
					m_nYFactor = -m_nYFactor;
					m_nTempParam1 = 1;
					m_nDir = m_nDir - MaxMissleDir / 2;
					if (m_nDir < 0) m_nDir += MaxMissleDir;
				}
			}
			nDOffsetX = (m_nCurSpeed * m_nXFactor);
			nDOffsetY = (m_nCurSpeed * m_nYFactor);
		}break;
		//按照设计方案，随机飞行无法达到客服两端的同步
	case	MISSLE_MMK_Random:							//	随机飞行（暗黑二女巫的Charged Bolt）
		{
			
		}break;
		//参数一表示顺时针还是逆时针转动
		//参数二表示固定原心还是围饶发动者
		//dx = SinA * R
		//dy = Ctg(90-A/2).R = SinA*SinA / (1 + CosA) * R
	case	MISSLE_MMK_Circle:							//	环行飞行（围绕在身边，暗黑二刺客的集气）
		{
			int nPreAngle = m_nAngle - 1;
			if (nPreAngle < 0) nPreAngle = MaxMissleDir - 1;
			m_nDir = m_nAngle + (MaxMissleDir / 4);
			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;
			int dx = (m_nCurSpeed + 50)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle,MaxMissleDir)) ;
			int dy = (m_nCurSpeed + 50)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle, MaxMissleDir)) ; 
			
			if (m_pMissleTemplate->m_nParam2) //原地转
			{
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			else			// 围绕着发送者转
			{
				int nOldRegion = m_nRegionId;
//				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;
				m_nCurrentMapX	= Npc[m_nLauncher].GetMapX();
				m_nCurrentMapY	= Npc[m_nLauncher].GetMapY();
				m_nXOffset		= Npc[m_nLauncher].GetOffX();
				m_nYOffset		= Npc[m_nLauncher].GetOffY();
//				CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				
				if (nOldRegion != m_nRegionId)
				{					
					SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleIdx);
				}  
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			
			//顺时针还是逆时针
			if (m_pMissleTemplate->m_nParam1)
			{
				m_nAngle ++;
				if (m_nAngle >= MaxMissleDir)
					m_nAngle = 0;
			}
			else
			{
				m_nAngle --;
				if (m_nAngle < 0 )
					m_nAngle = MaxMissleDir - 1;
			}
			
		}
		break;
		
		//参数一表示顺时针还是逆时针转动
		//参数二表示固定原心还是围饶发动者
	case	MISSLE_MMK_Helix:							//	阿基米德螺旋线（暗黑二游侠的Bless Hammer）
		{
			int nPreAngle = m_nAngle - 1;
			if (nPreAngle < 0) 
			{
				nPreAngle = MaxMissleDir -1;
			}
			m_nDir = m_nAngle + (MaxMissleDir / 4);
			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;
			
			int dx = (m_nCurSpeed + m_nCurrentLife + 50)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle, MaxMissleDir)) ;
			int dy = (m_nCurSpeed + m_nCurrentLife + 50)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle,MaxMissleDir)) ; 
			
			if (m_pMissleTemplate->m_nParam2) //原地转
			{
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			else			// 围绕着发送者转
			{
				int nOldRegion = m_nRegionId;
//				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;
				m_nCurrentMapX	= Npc[m_nLauncher].GetMapX();
				m_nCurrentMapY	= Npc[m_nLauncher].GetMapY();
				m_nXOffset		= Npc[m_nLauncher].GetOffX();
				m_nYOffset		= Npc[m_nLauncher].GetOffY();
//				CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				
				if (nOldRegion != m_nRegionId)
				{
					SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleIdx);
				}  
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			
			if (m_pMissleTemplate->m_nParam1)
			{
				m_nAngle ++;
				if (m_nAngle >= MaxMissleDir)
					m_nAngle = 0;
			}
			else
			{
				m_nAngle --;
				if (m_nAngle < 0 )
					m_nAngle = MaxMissleDir - 1;
			}
		}
		break; 

	case	MISSLE_MMK_Follow:							//	跟踪目标飞行
		{
			int nDistance = 0;
			int nSrcMpsX = 0;
			int nSrcMpsY = 0;
			int nDesMpsX = 0;
			int nDesMpsY = 0;
			
			if (m_pMissleTemplate->m_nParam1 ++ >= FOLLOWMISSLE_GUIDETIME_PRETIME)
			{
				m_pMissleTemplate->m_nParam1 = 0;
				if (m_nFollowNpcIdx > 0 && Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID))
				{
					SubWorld[m_nSubWorldId].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, &nSrcMpsX, &nSrcMpsY);
					// Modify by Cooler -->
					// 2005-7-12
					// SubWorld[m_nSubWorldId].Map2Mps(Npc[m_nFollowNpcIdx].m_RegionIndex, Npc[m_nFollowNpcIdx].m_MapX, Npc[m_nFollowNpcIdx].m_MapY, Npc[m_nFollowNpcIdx].m_OffX, Npc[m_nFollowNpcIdx].m_OffY, &nDesMpsX, &nDesMpsY);
					SubWorld[m_nSubWorldId].Map2Mps(Npc[m_nFollowNpcIdx].m_RegionIndex, Npc[m_nFollowNpcIdx].GetMapX(), Npc[m_nFollowNpcIdx].GetMapY(), Npc[m_nFollowNpcIdx].GetOffX(), Npc[m_nFollowNpcIdx].GetOffY(), &nDesMpsX, &nDesMpsY);

					// End modify by Cooler <--
					nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
					
					if (nDistance != 0)
					{
						int nXFactor = ((nDesMpsX - nSrcMpsX ) << 10) / nDistance;
						int nYFactor = ((nDesMpsY - nSrcMpsY ) << 10) / nDistance;
						m_nDirIndex		= g_GetDirIndex(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
						m_nDir			= g_DirIndex2Dir(m_nDirIndex, MaxMissleDir);
						m_nXFactor = nXFactor;
						m_nYFactor = nYFactor;
					}
				}
			}
			
			nDOffsetX	 = m_nXFactor * m_nCurSpeed;
			nDOffsetY	 = m_nYFactor * m_nCurSpeed;	
		}break;
		
	case	MISSLE_MMK_Motion:							//	玩家动作类
		{
			
		}break;
		
	case MISSLE_MMK_SingleLine:						//	必中的单一直线飞行魔法
		{
			//单一必中类子弹，类式于传奇以及其它的同类网络游戏中的基本直线魔法			
			int x = m_nXOffset;
			int y = m_nYOffset;
			int dx = (m_nCurSpeed * m_nXFactor);
			int dy = (m_nCurSpeed * m_nYFactor);
			nDOffsetX	=  dx;//* m_nCurrentLife;
			nDOffsetY	=  dy;//* m_nCurrentLife;
		}
		break;
	default:
		_ASSERT(0);
		
	}
	
	//进入检测跨越Region的处理。True表示处理成功，未进入地图无效的位置。
	if (CheckBeyondRegion(nDOffsetX, nDOffsetY))
	{
		//新的位置，进行碰撞检测，检查当前位置是否撞到东西了，如果撞到则作伤害计算。如果落地了，则返回-1;
		if (CheckCollision() == -1) 
		{
			//如果现在落地了，则如果是自爆的，则处理伤害。
			if (m_pMissleTemplate->m_bAutoExplode)
			{
				ProcessCollision();//处理碰撞
			}
			int nSrcX4 = 0 ;
			int nSrcY4 = 0 ;
			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX4, &nSrcY4);
			CreateSpecialEffect(MS_DoVanish, nSrcX4, nSrcY4, m_nCurrentMapZ);
			DoVanish();
			return;
		}
	}
	else//如果子弹飞行过程中进入了一个无效的Region则子弹自动消亡
	{
		DoVanish();
	}
	
}
/*!*****************************************************************************
// Function		: KMissle::OnVanish
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissle::OnVanish()
{
	
}

void KMissle::Paint()
{
	if ( m_pMissleTemplate == NULL )
	{
		return;
	}
	if (m_nMissleIdx <= 0 ) return;
	int nSrcX;
	int nSrcY;
	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
	
	if (!m_pMissleTemplate->m_nZAcceleration)
	{
		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nCurrentMapZ, m_nDir,m_nCurLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );
	}
	else
	{
		int nDirIndex = g_GetDirIndex(0,0,m_nXFactor, m_nYFactor);
		int nDir = g_DirIndex2Dir(nDirIndex, 64);
		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nCurrentMapZ, nDir,m_nCurLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );
	}
}

BOOL	KMissle::CheckBeyondRegion(int nDOffsetX, int nDOffsetY)
{
	if (m_nRegionId < 0) 
		return FALSE;
	
	int nOldRegion	= 0;
	int nNewXOffset = 0;	
	int nNewYOffset = 0;	
	int nNewMapX = 0;	
	int nNewMapY = 0;	
	int nNewRegion = 0;	

	//如果是以跟随某个参照物的位置而变化的，则累加，然后计算实际位置
	if (m_eRelativePosType)
	{
		m_nMissleTotalMpsX += nDOffsetX;
		m_nMissleTotalMpsY += nDOffsetY;
		int nLauncherMpsX, nLauncherMpsY;
		if (GetRelativePos(nLauncherMpsX, nLauncherMpsY))
		{
			SubWorld[m_nSubWorldId].Mps2Map(
				nLauncherMpsX + (m_nMissleTotalMpsX >> 10), 
				nLauncherMpsY + (m_nMissleTotalMpsY >> 10),
				&nNewRegion,
				&nNewMapX,
				&nNewMapY,
				&nNewXOffset,
				&nNewYOffset);
			nOldRegion = m_nRegionId;
		}
		else
			return FALSE;
	}
	else
	{
		//未动
		if (nDOffsetX == 0 && nDOffsetY == 0) 
			return TRUE;
		
// 		if (abs(nDOffsetX) > CellWidth) 
// 		{
// 			return FALSE;
// 		}
// 		
// 		if (abs(nDOffsetY) > CellHeight) 
// 		{
// 			return FALSE;
// 		}

 		if ( abs(nDOffsetX) >= (REGION_PIXEL_WIDTH << 10) ) 
 		{
			_ASSERT(FALSE);
 			return FALSE;
 		}
 		
 		if ( abs(nDOffsetY) >= (REGION_PIXEL_HEIGHT << 10) )
 		{
			_ASSERT(FALSE);
 			return FALSE;
 		}
		
		nOldRegion		= m_nRegionId;
		nNewXOffset		= m_nXOffset + nDOffsetX;
		nNewYOffset		= m_nYOffset + nDOffsetY;
		nNewMapX		= m_nCurrentMapX;
		nNewMapY		= m_nCurrentMapY;
		nNewRegion		= m_nRegionId;
		
		const DWORD nRegionWidth = RegionWidth;
		const DWORD nRegionHeight = RegionHeight;
		
// 		_ASSERT(abs(nNewXOffset) <= CellWidth * 2);
// 		_ASSERT(abs(nNewYOffset) <= CellHeight * 2);
		
		//	处理NPC的坐标变幻
		//	CELLWIDTH、CELLHEIGHT、OffX、OffY均是放大了1024倍
		
		int nXOffsetCopy = nNewXOffset;
		int nYOffsetCopy = nNewYOffset;

		if (nNewXOffset < 0)
		{
// 			nNewMapX--;
// 			nNewXOffset += CellWidth;

			nNewMapX += (nXOffsetCopy / CellWidth - 1);
			nNewXOffset = (nXOffsetCopy % CellWidth) + CellWidth;
		}
		else if (nNewXOffset > CellWidth)
		{
//			nNewMapX++;
//			nNewXOffset -= CellWidth;

			nNewMapX += (nXOffsetCopy / CellWidth);
			nNewXOffset = (nXOffsetCopy % CellWidth);
		}
		
		if (nNewYOffset < 0)
		{
// 			nNewMapY--;
// 			nNewYOffset += CellHeight;

			nNewMapY += (nYOffsetCopy / CellHeight - 1);
			nNewYOffset = (nYOffsetCopy % CellHeight) + CellHeight;
		}
		else if (nNewYOffset > CellHeight)
		{
// 			nNewMapY++;
// 			nNewYOffset -= CellHeight;

			nNewMapY += (nYOffsetCopy / CellHeight);
			nNewYOffset = (nYOffsetCopy % CellHeight);
		}
		
		if (nNewMapX < 0)
		{
			nNewRegion = LeftRegion(m_nRegionId);
			nNewMapX += nRegionWidth;
		}
		else if ((DWORD)nNewMapX >= nRegionWidth)
		{
			nNewRegion = RightRegion(m_nRegionId);
			nNewMapX -= nRegionWidth;
		}
		
		if (nNewRegion < 0) 
		{
			return FALSE; 
		}
		
		if (nNewMapY < 0)
		{
			nNewRegion = UpRegion(nNewRegion);
			nNewMapY += nRegionHeight;
		}
		else if (nNewMapY >= RegionHeight)
		{
			nNewRegion = DownRegion(nNewRegion);
			nNewMapY -= nRegionHeight;
		}
	}
	
	//下一个位置为不合法位置，则消亡
	if (nNewRegion < 0) 
	{
		return FALSE; 
	}
	else
	{
		_ASSERT(m_nCurrentMapX >= 0  &&  m_nCurrentMapY >= 0);
		m_nRegionId	   = nNewRegion;
		m_nCurrentMapX = nNewMapX;
		m_nCurrentMapY = nNewMapY;
		m_nXOffset	   = nNewXOffset;
		m_nYOffset	   = nNewYOffset;
		
		if (nOldRegion != m_nRegionId)
		{
			SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleIdx);
		}
	}
	return TRUE;
}

BOOL	KMissle::GetRelativePos(int& nMpsX, int& nMpsY)
{
	nMpsX = nMpsY = 0;
	switch(m_eRelativePosType)
	{
		//子碟原始位置, 主要用于子碟速度超过32的
	case type_bornpos:
		nMpsX = m_nRefPX;
		nMpsY = m_nRefPY;
		break;
		//发送者位置
	case type_launcherpos:
		Npc[m_nLauncher].GetMpsPos(&nMpsX, &nMpsY);
		break;
		//母位置，可以是子碟
	case type_parentpos:
		{
			//如果该值大于0表示，有子子碟，如果等0，表示母亲就是NpcLauncher
			if (m_nParentMissleIndex > 0)
			{
				Missle[m_nParentMissleIndex].GetMpsPos(&nMpsX, &nMpsY);
			}
			else 
			{
				Npc[m_nLauncher].GetMpsPos(&nMpsX, &nMpsY);
			}
		}break;
		//目的者位置
	case type_targetpos:
		{
			if (m_nFollowNpcIdx > 0 &&
				Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID) &&
				Npc[m_nFollowNpcIdx].m_SubWorldIndex == m_nSubWorldId
				)
			{
				Npc[m_nFollowNpcIdx].GetMpsPos(&nMpsX, &nMpsY);
			}
			else 
				return FALSE;

			
		}break;
	}
	return TRUE;
}
	

KMissle&	KMissle::operator=(KMissleTemplate* Missle)
{
	m_pMissleTemplate = Missle;
	if ( m_pMissleTemplate )
	{
		m_nTempParam1	=	0;
		m_nTempParam2	=	0;
		m_nMissleTotalMpsX = 0;
		m_nMissleTotalMpsY = 0;
		m_nDesMapX			=	0;
		m_nDesMapY			=	0;
		m_bNeedReclaim	=	FALSE;
		m_nFirstReclaimTime = 0;
		m_nEndReclaimTime = 0;
//		m_nAngle			= m_pMissleTemplate->m_nAngle;
		m_nCurrentLife	=	0;
		m_nCurrentMapZ	=   m_pMissleTemplate->m_nHeight >> 10;
		m_dwFollowNpcID =	0;
		m_nFollowNpcIdx =	0;
		m_MissleRes.m_bLoopAnim = m_pMissleTemplate->m_bLoopAnim;
		m_MissleRes.m_bHaveEnd = FALSE;
		int nOffset = 0;
		
		//如果是相同的子弹可以以不同方式显示时，则随机产生
		if (m_pMissleTemplate->m_bMultiShow)		
		{
			if (g_Random(2) == 0)
			{
				nOffset = 0;
			}
			else
				nOffset = MAX_MISSLE_STATUS;
		}
		
		for (int t = 0; t < MAX_MISSLE_STATUS ; t++)
		{
			strcpy(m_MissleRes.m_MissleRes[t].AnimFileName,m_pMissleTemplate->m_MissleRes[t + nOffset].AnimFileName);
			
			m_MissleRes.m_MissleRes[t].nTotalFrame = m_pMissleTemplate->m_MissleRes[t + nOffset].nTotalFrame;
			m_MissleRes.m_MissleRes[t].nDir = m_pMissleTemplate->m_MissleRes[t + nOffset].nDir;
			m_MissleRes.m_MissleRes[t].nInterval = m_pMissleTemplate->m_MissleRes[t + nOffset].nInterval;
			
			strcpy(m_MissleRes.m_MissleRes[t].SndFileName,m_pMissleTemplate->m_MissleRes[t + nOffset].SndFileName);
		}
		m_MissleRes.m_bSubLoop = m_pMissleTemplate->m_bSubLoop;
		m_MissleRes.m_nSubStart = m_pMissleTemplate->m_nSubStart;
		m_MissleRes.m_nSubStop = m_pMissleTemplate->m_nSubStop;
	}


	return (*this);
}

void KMissle::DoVanish()
{
	if (m_eMissleStatus == MS_DoVanish) return ;
	
	m_MissleRes.m_bHaveEnd = TRUE;
	m_eMissleStatus = MS_DoVanish;
	
	if (m_nRegionId < 0)
	{
		_ASSERT(0);
		return ;
	}
}

// Commented by chenshanglin on [2006-3-16 10:17]
// void KMissle::DoCollision()
// Commented end
// Add by chenshanglin on [2006-3-16 10:17]
void KMissle::DoCollision(int nTargetIdx)
// Add end
{
	if ( m_pMissleTemplate == NULL )
	{
		return;
	}

	if (m_eMissleStatus == MS_DoCollision) return;
	
	int nSrcX = 0 ;
	int nSrcY = 0 ;
	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
	
	if (m_pMissleTemplate->m_bCollideVanish)
	{
		// 只有碰到了要打的目标目标才消亡
		if(m_nFollowNpcIdx > 0 && m_nFollowNpcIdx == nTargetIdx)
		{
			m_MissleRes.m_bHaveEnd = TRUE;
		
			int nSrcX5 = 0 ;
			int nSrcY5 = 0 ;
			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX5, &nSrcY5);
			CreateSpecialEffect(MS_DoVanish, nSrcX5, nSrcY5, m_nCurrentMapZ);
			DoVanish();
		}
	}
	else 
	{
		//增加撞后的效果	
		if (m_MissleRes.SpecialMovieIsAllEnd())
			CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);

		m_eMissleStatus = MS_DoFly;
	}
}

void KMissle::DoFly()
{
	if (m_eMissleStatus == MS_DoFly) return ;
	//初始化贴图
	m_eMissleStatus = MS_DoFly;
}

BOOL KMissle::GetOffsetAxis(int nSubWorld, int nSrcRegionId, int nSrcMapX, int nSrcMapY,
							int nOffsetMapX, int nOffsetMapY, 
							int &nDesRegionId, int &nDesMapX, int &nDesMapY)
{
	nDesRegionId = -1;
	// 确定目标格子实际的REGION和坐标确定
	nDesMapX = nSrcMapX + nOffsetMapX;
	nDesMapY = nSrcMapY + nOffsetMapY;
	
	if (nSrcRegionId < 0) 
		return FALSE;

	int nSearchRegion = nSrcRegionId;
	if (nDesMapX < 0)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
		nDesMapX += RegionWidth;
	}
	else if (nDesMapX >= RegionWidth)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
		nDesMapX -= RegionWidth;
	}
	if (nSearchRegion < 0) 
		return FALSE;
	
	if (nDesMapY < 0)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
		nDesMapY += RegionHeight;
	}
	else if (nDesMapY >= RegionHeight)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
		nDesMapY -= RegionHeight;
	}	

	if (nSearchRegion < 0) 
		return FALSE;
	nDesRegionId = nSearchRegion;
	return TRUE;
	// 从REGION的NPC列表中查找满足条件的NPC		
	//int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nDesMapX, nDesMapY, nLauncherIdx, relation_all);
}

/*!*****************************************************************************
// Function		: KMissle::ProcessCollision
// Purpose		: 
// Return		: int 
// Argumant		: int nLauncherIdx
// Argumant		: int nRegionId
// Argumant		: int nMapX
// Argumant		: int nMapY
// Argumant		: int nRange
// Argumant		: MISSLE_RELATION eRelation
// Comments		:
// Author		: RomanDou
*****************************************************************************/
int KMissle::ProcessCollision(int nLauncherIdx, int nRegionId, int nMapX, int nMapY, int nRange , int eRelation)
{
	if ( m_pMissleTemplate == NULL )
	{
		return 0;
	}

	if (nLauncherIdx <= 0 ) return 0;
	if (nRange <= 0) return 0;
	
	int nRangeX = nRange / 2;
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[nLauncherIdx].m_SubWorldIndex;
	
//	_ASSERT(Npc[nLauncherIdx].m_SubWorldIndex >= 0);
//	_ASSERT(nRegionId >= 0);
	if(Npc[nLauncherIdx].m_SubWorldIndex < 0 || nRegionId < 0)
	{
		return 0;
	}
	
	int	nRegion = nRegionId;
	int	nRet = 0;
	int	nRMx, nRMy, nSearchRegion;

	// 检查范围内的格子里的NPC
	for (int i = -nRangeX; i <= nRangeX; i++)
	{
		for (int j = -nRangeY; j <= nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			//if ((i * i + j * j ) > nRangeX * nRangeX)
			//continue;

			if (!GetOffsetAxis(nSubWorld, nRegionId, nMapX, nMapY, i , j , nSearchRegion, nRMx, nRMy))
				continue;

			_ASSERT(nSearchRegion >= 0);

			// 从REGION的NPC列表中查找满足条件的NPC		
			//int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation);
			// lixuewu 子弹使用原来版本的FindNpc
			int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpcList(nRMx, nRMy, nLauncherIdx, eRelation);
			if (nNpcIdx > 0)	
			{
				nRet++;
				int nSrcX = 0;
				int nSrcY = 0;
				SubWorld[0].Map2Mps(nSearchRegion, Npc[nNpcIdx].GetMapX(),Npc[nNpcIdx].GetMapY(), Npc[nNpcIdx].GetOffX(), Npc[nNpcIdx].GetOffY(),  &nSrcX, &nSrcY);
				
				// 只有打到指定的目标才有爆炸效果
				if(m_nFollowNpcIdx > 0 && m_nFollowNpcIdx == nNpcIdx)
				{
					if (m_pMissleTemplate->m_bFollowNpcWhenCollid)
						CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ, nNpcIdx);
					else 
						CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
				}
			}
		}
	}
	return nRet;
}


int KMissle::ProcessCollision()
{
	if (m_bClientSend || m_pMissleTemplate == NULL) return 0;
	return ProcessCollision(m_nLauncher, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_pMissleTemplate->m_nDamageRange , m_eRelation);
}

//生成某个特效结点
#define MISSLE_Y_OFFSET 1
BOOL KMissle::CreateSpecialEffect(eMissleStatus eStatus, int nPX, int nPY, int nPZ, int nNpcIndex)
{
	
	KSkillSpecialNode * pSkillSpecialNode = NULL;
	//同一颗子碟不能有几个爆炸效果在一个Npc身上
	if (nNpcIndex > 0)
	{
		pSkillSpecialNode = (KSkillSpecialNode*)m_MissleRes.m_SkillSpecialList.GetHead();
		while(pSkillSpecialNode)
		{
			if (pSkillSpecialNode->m_dwMatchID == Npc[nNpcIndex].m_dwID) return FALSE;
			pSkillSpecialNode = (KSkillSpecialNode*)pSkillSpecialNode->GetNext();
		}
	}
	m_MissleRes.PlaySound(eStatus, nPX, nPY, 0);
	if (!m_MissleRes.m_MissleRes[eStatus].AnimFileName[0]) return FALSE; 
	pSkillSpecialNode = new KSkillSpecialNode;
	//KSkillSpecial * pSkillSpecial = new KSkillSpecial;
	//pNode->m_pSkillSpecial = pSkillSpecial;
	
	int nSrcX = nPX;
	int nSrcY = nPY;
	
	pSkillSpecialNode->m_nPX = nSrcX;
	pSkillSpecialNode->m_nPY = nSrcY - 5;// MISSLE_Y_OFFSET;
	pSkillSpecialNode->m_nPZ = nPZ;
	pSkillSpecialNode->m_nNpcIndex = nNpcIndex;
	pSkillSpecialNode->m_dwMatchID = Npc[nNpcIndex].m_dwID;
	pSkillSpecialNode->m_pMissleRes = &m_MissleRes.m_MissleRes[eStatus];
	pSkillSpecialNode->m_nBeginTime = g_SubWorldSet.GetGameTime();
	pSkillSpecialNode->m_nEndTime = g_SubWorldSet.GetGameTime() + (pSkillSpecialNode->m_pMissleRes->nInterval * pSkillSpecialNode->m_pMissleRes->nTotalFrame / pSkillSpecialNode->m_pMissleRes->nDir);
	pSkillSpecialNode->m_nCurDir = g_DirIndex2Dir(m_nDirIndex, m_MissleRes.m_MissleRes[eStatus].nDir);
	pSkillSpecialNode->Init();
	m_MissleRes.m_SkillSpecialList.AddTail(pSkillSpecialNode);
	
	return TRUE;
}

void KMissle::DoWait()
{
	//	if (m_eMissleStatus == MS_DoWait) return;
	m_eMissleStatus = MS_DoWait;

	int nSrcX = 0 ;
	int nSrcY = 0 ;
	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
	CreateSpecialEffect(MS_DoWait, nSrcX, nSrcY, m_nCurrentMapZ);
	
}
//当子碟进入fly状态时，需要根据情况变动
BOOL	KMissle::PrePareFly()
{
	if ( m_pMissleTemplate == NULL )
	{
		return FALSE;
	}

	if (m_pMissleTemplate->m_eMoveKind == MISSLE_MMK_RollBack)
		m_nTempParam2 =  m_nStartLifeTime + (m_nCurLifeTime - m_nStartLifeTime ) / 2;

	//是否会随发送者的移动而中断，类式魔兽3中大型法术
	if (m_nInteruptTypeWhenMove)
	{
		int nPX, nPY;
		Npc[m_nLauncher].GetMpsPos(&nPX, &nPY);
		if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)
		{
			return false;
		}
	}
	
	//子碟位置需要更正为到适当的位置（子弹的出现以某个可能位置在不断变化的物体为参照物）
	if (m_bHeelAtParent)
	{
		int nNewPX = 0;
		int nNewPY = 0;
		
		if (m_nParentMissleIndex) // 参考点为母子弹
		{
			if (Missle[m_nParentMissleIndex].m_dwLauncherId != m_dwLauncherId)
			{
				return false;
			}
			else
			{
				int nParentPX, nParentPY;
				int nSrcPX, nSrcPY;
				Missle[m_nParentMissleIndex].GetMpsPos(&nParentPX, &nParentPY);
				GetMpsPos(&nSrcPX, &nSrcPY);
				nNewPX = nSrcPX + (nParentPX - m_nRefPX);
				nNewPY = nSrcPY + (nParentPY - m_nRefPY);
			}
		}
		else
			//参考点为发送者
		{
			_ASSERT(m_nLauncher > 0);
			int nParentPX, nParentPY;
			int nSrcPX, nSrcPY;
			
			Npc[m_nLauncher].GetMpsPos(&nParentPX, &nParentPY);
			GetMpsPos(&nSrcPX, &nSrcPY);
			
			nNewPX = nSrcPX + (nParentPX - m_nRefPX);
			nNewPY = nSrcPY + (nParentPY - m_nRefPY);
		}
		
		int nOldRegion = m_nRegionId;
//		CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		SubWorld[m_nSubWorldId].Mps2Map(nNewPX, nNewPY, &m_nRegionId, &m_nCurrentMapX, &m_nCurrentMapY, &m_nXOffset, &m_nYOffset);
//		CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		
		if (nOldRegion != m_nRegionId)
		{
			SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleIdx);
		} 
		
	}
	
	return true;
	
}

int KMissle::CheckNearestCollision()
{
	int nSearchRegion = 0;
	int nRMx = 0;
	int nRMy = 0;
	BOOL bCollision = TRUE;
	int nNpcIdx = 0;
	int nDX = 0;
	int nDY = 0;
	int nNpcOffsetX = 0;
	int nNpcOffsetY = 0;
	int nAbsX = 0;
	int nAbsY = 0;
	int nCellWidth = CellWidth;
	int nCellHeight = CellHeight;
	_ASSERT(nCellWidth > 0 && nCellHeight > 0);
	
	for (int i = -1; i <= 1; i ++)
		for (int j = -1; j <= 1; j ++)
		{
			if (!KMissle::GetOffsetAxis(
				m_nSubWorldId,
				m_nRegionId, 
				m_nCurrentMapX, 
				m_nCurrentMapY, 
				i , 
				j , 
				nSearchRegion, 
				nRMx, 
				nRMy
				))
				continue;
			
			_ASSERT(nSearchRegion >= 0);
			
			nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);
			
			if (nNpcIdx > 0)
			{
				bCollision = TRUE;
				nDX = m_nCurrentMapX - Npc[nNpcIdx].GetMapX();
				nDY = m_nCurrentMapY - Npc[nNpcIdx].GetMapY();
				nNpcOffsetX = Npc[nNpcIdx].GetOffX();
				nNpcOffsetY = Npc[nNpcIdx].GetOffY();
				nAbsX = abs(nDX);
				nAbsY = abs(nDY);
				
				if (nAbsX)
				{
					if (nDX < 0)
					{
						if (nCellWidth - m_nXOffset + nNpcOffsetX > nCellWidth)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
					else if (nDX > 0)
					{
						if (nCellWidth - nNpcOffsetX + m_nXOffset > nCellWidth)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
				}
				
				if (nAbsY)
				{
					if (nDY <0)
					{
						if (nCellHeight - m_nYOffset + nNpcOffsetY > nCellHeight)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
					else if (nDY >0)
					{
						if (nCellHeight - nNpcOffsetY + m_nYOffset > nCellHeight)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
				}
				
				
CheckCollision:
				if (bCollision)
					return nNpcIdx;
			}
		}
		
		return 0;
}

void	KMissle::GetMpsPos(int *pPosX, int *pPosY)
{
	SubWorld[m_nSubWorldId].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, pPosX, pPosY);
};

