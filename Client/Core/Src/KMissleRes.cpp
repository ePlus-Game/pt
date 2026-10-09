//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/22/2007 13:22
//      File_base        : KMissleRes
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KEngine.h"
#include "KMissle.h"
#include "KMissleRes.h"
#include "KSubWorld.h"
#include "KSkillSpecial.h"
#include "ImgRef.h"
#include "KPlayer.h"
#include "iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "KOption.h"
#include "KSubWorldSet.h"

KMissleRes::KMissleRes()
{
	m_pSndNode = NULL;
	m_bLoopAnim = 0;
	m_bHaveEnd = FALSE;
	
	for (int i = 0 ; i < MAX_MISSLE_STATUS; i ++)
	{
		m_RUImage[i].nType = ISI_T_SPR;
		m_RUImage[i].Color.Color_b.a = 255;
		m_RUImage[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT;
		m_RUImage[i].uImage = 0;
		m_RUImage[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_RUImage[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	}
}

KMissleRes::~KMissleRes()
{
}

void KMissleRes::LoadResource(int nStatus, char * MissleImage, char * MissleSound)
{
	strcpy(m_MissleRes[nStatus].AnimFileName, MissleImage);
	strcpy(m_MissleRes[nStatus].SndFileName,  MissleSound);
}

BOOL KMissleRes::Init()
{
	Clear();
	return TRUE;
}

void KMissleRes::Clear()
{
	while(m_SkillSpecialList.GetHead())
	{
		KSkillSpecialNode * pSkillSpecialNode = (KSkillSpecialNode*)m_SkillSpecialList.GetHead();
		pSkillSpecialNode->Remove();
		delete pSkillSpecialNode;
	}
	
	for(int i = 0; i <MAX_MISSLE_STATUS; i ++)
	{
		m_RUImage[i].szImage[0] = 0;
		m_RUImage[i].uImage = 0;
	}
}

int KMissleRes::Draw(int eStatus,  int nX, int nY , int nZ, int nDir, int nAllFrame,  int nCurLifeFrame)
{
	//当nAllFrame == 0时，表示为默认数
	if (eStatus == MS_DoWait)
	{
		
	}
	else if (eStatus == MS_DoFly)
	{
		if (nCurLifeFrame < 0 || (nAllFrame != 0 && nAllFrame < nCurLifeFrame)) return FALSE;
		
		if (!m_RUImage[eStatus].szImage[0])
		{
			g_StrCpy(m_RUImage[eStatus].szImage, m_MissleRes[eStatus].AnimFileName);
		}
		
		int nSprDir = m_MissleRes[eStatus].nDir;
		int nSprFrames = m_MissleRes[eStatus].nTotalFrame;
		if (nSprDir && nSprFrames)
		{
			//处理子弹显示时实际方向
			int	nImageDir = (nDir / (64 / nSprDir));
			int nImageDir1 = (nDir % (64 / nSprDir));
			if (nImageDir1 >= 32 / nSprDir) 	nImageDir ++;
			if (nImageDir >= nSprDir)		nImageDir = 0;
			
			int nFramePerDir = (nSprFrames / nSprDir);
			if (nAllFrame == 0) nAllFrame = nFramePerDir;
			int nFirstFrame = nImageDir * nFramePerDir;
			int nTotalFrame = nSprFrames / nSprDir;
			int nFrame = nCurLifeFrame ;

			{
				if (m_bLoopAnim) //如果是循环播放的话则每帧都换帧
				{
					if (!m_bSubLoop)//无子显示循环
					{	nFrame = (nCurLifeFrame / m_MissleRes[eStatus].nInterval)  % nTotalFrame;
					}
					else
					{
						//  未显示到循环播放的开始帧
						if ( (nCurLifeFrame / m_MissleRes[eStatus].nInterval) < m_nSubStart)
							nFrame = nCurLifeFrame / m_MissleRes[eStatus].nInterval;
						else
						{
							if (m_nSubStart == m_nSubStop) nFrame = m_nSubStart;
							else
								nFrame = m_nSubStart + ((nCurLifeFrame - m_nSubStart) / m_MissleRes[eStatus].nInterval)  % (m_nSubStop - m_nSubStart);
						}
					}
				}
				else
				{
					nFrame = nTotalFrame * nCurLifeFrame / nAllFrame;
				}
				
				if (nFrame > (nTotalFrame - 1)) 
					return FALSE;
			}
			nFrame = nFirstFrame + nFrame;
			m_RUImage[eStatus].oPosition.nX = nX;
			m_RUImage[eStatus].oPosition.nY = nY;
			m_RUImage[eStatus].oPosition.nZ = nZ;
			m_RUImage[eStatus].nFrame = nFrame;
			g_pRepresent->DrawPrimitives(1, &m_RUImage[eStatus], RU_T_IMAGE, 0);
			
#ifdef SWORDONLINE_SHOW_DBUG_INFO_LUCIFER
			if (Player[CLIENT_PLAYER_INDEX].m_DebugMode)
			{
				KRULine		Line;
				Line.oPosition.nX = nX;
				Line.oPosition.nY = nY;
				Line.oPosition.nZ = nZ;
				Line.oEndPos.nX = Line.oPosition.nX +32;
				Line.oEndPos.nY = nY;
				Line.oEndPos.nZ = nZ;
				
				Line.Color.Color_dw = 0xffffffff;
				g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 0);
				char MissleId[30];
				//itoa(m_nMissleId, MissleId, 10);
				sprintf(MissleId, "%d_%d", m_nMissleId, Missle[m_nMissleId].m_nCurrentLife);
				g_pRepresent->OutputText(12, MissleId, KRF_ZERO_END, nX, nY, 0xffffffff, 3, 5);
			}
#endif
		}
	}
	
	KSkillSpecialNode * pSkillSpecialNode = (KSkillSpecialNode*)m_SkillSpecialList.GetHead(); 
	while (pSkillSpecialNode)
	{
		DWORD dwCurrentTime =  g_SubWorldSet.GetGameTime();
	
		KSkillSpecialNode * pTempNode = (KSkillSpecialNode*)pSkillSpecialNode->GetNext();

		if(pSkillSpecialNode->m_nEndTime <= dwCurrentTime)
		{
			pSkillSpecialNode->Remove();
			delete pSkillSpecialNode;
		}
		else
		{
			if (pSkillSpecialNode->m_nBeginTime <= dwCurrentTime)
				pSkillSpecialNode->Draw(dwCurrentTime);
		}
		pSkillSpecialNode = pTempNode;		
	}
	return TRUE;
}

void KMissleRes::PlaySound(int eStatus, int nX, int nY, int nLoop)
{
	if (m_MissleRes[eStatus].SndFileName[0] == 0)	return;

	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;

	// 获得屏幕中心点的地图坐标 not end
	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);

	KWavSound * pSound = NULL;
	m_pSndNode	= (KCacheNode*) g_SoundCache.GetNode(m_MissleRes[eStatus].SndFileName, (KCacheNode * ) m_pSndNode);
	pSound		= (KWavSound*) m_pSndNode->m_lpData;
	if (pSound)
	{

		float dist = sqrt((nX-nCenterX)*(nX-nCenterX)+(nY-nCenterY)*(nY-nCenterY));
		pSound->Play((nX-nCenterX)*10, Option.GetSndVolume(dist), nLoop);
	}
	m_nLastSndIndex = eStatus;
}

