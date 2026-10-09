//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:8:8   9:50
//      File_base        : SkillTargetFilter
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "SkillTargetFilter.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMath.h"

SkillTargetFilter::TargetFilterFunc	SkillTargetFilter::m_Filter[tft_filter_num] = 
{
	&SkillTargetFilter::IsBeeline,		// tft_filter_beeline
	&SkillTargetFilter::IsInRectangle,	// tft_filter_rectangle
};

bool SkillTargetFilter::IsBeeline(int nLauncherNpcIdx, int srcPosX, int srcPosY, int nTargetNpcIdx, void *pPassby)
{
	if(NULL == pPassby)
		return false;

	int nLauncherPosX, nLauncherPosY;
	Npc[nLauncherNpcIdx].GetMpsPos(&nLauncherPosX, &nLauncherPosY);

	int nTargetPosX, nTargetPosY;
	Npc[nTargetNpcIdx].GetMpsPos(&nTargetPosX, &nTargetPosY);

	int nDistance = NpcSet.GetDistance(nLauncherNpcIdx, nTargetNpcIdx);
	int nDirSrcToTarget = g_GetDirIdxForFindPath(nLauncherPosX, nLauncherPosY, nTargetPosX, nTargetPosY);

	DirSkillFilterParam *param = (DirSkillFilterParam*)pPassby;
	int	nRelDir = 16 - abs(nDirSrcToTarget - param->nDir);	

	if(nRelDir <= 0)
		return false;
	else
	{
		int nRelTargetX = nDistance * g_DirCos(48 - nRelDir, 64) / 1024;	
		int nWidth = param->param[0];
		return nRelTargetX < nWidth / 2;
	}
}

bool SkillTargetFilter::IsInRectangle(int nLauncherNpcIdx, int srcPosX, int srcPosY, int nTargetNpcIdx, void *pPassby)
{
	if(NULL == pPassby)
		return false;

	int nTargetPosX, nTargetPosY;
	Npc[nTargetNpcIdx].GetMpsPos(&nTargetPosX, &nTargetPosY);

	int nDistance = NpcSet.GetDistance(srcPosX, srcPosY, nTargetNpcIdx);
	int nDirSrcToTarget = g_GetDirIdxForFindPath(srcPosX, srcPosY, nTargetPosX, nTargetPosY);

	DirSkillFilterParam *param = (DirSkillFilterParam*)pPassby;
	int dirDiff = abs(nDirSrcToTarget - param->nDir);
	
	int dis1 = abs(nDistance * g_DirCos(dirDiff, 64)) / 1024;
	int dis2 = abs(nDistance * g_DirSin(dirDiff, 64)) / 1024;

	return (dis1 <= param->param[0] && dis2 <= param->param[1]);
}
