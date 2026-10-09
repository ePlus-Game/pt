//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:8:8   9:49
//      File_base        : SkillTargetFilter
//      File_ext         : h
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

#ifndef _SkillTargetFilter_h
#define _SkillTargetFilter_h

#include "SkillDef.h"

class SkillTargetFilter
{
public:
	static bool Filter(int nType, int nLauncherNpcIdx, int posX, int posY, int nTargetNpcIdx, void *pPassby);

private:
	static bool IsBeeline(int nLauncherNpcIdx, int srcPosX, int srcPosY, int nTargetNpcIdx, void *pPassby);
	static bool IsInRectangle(int nLauncherNpcIdx, int srcPosX, int srcPosY, int nTargetNpcIdx, void *pPassby);

private:
	typedef bool (*TargetFilterFunc)(int nLauncherNpcIdx, int srcPosX, int srcPosY, int nTargetNpcIdx, void *pPassby);

	static TargetFilterFunc	m_Filter[tft_filter_num];
};

inline bool SkillTargetFilter::Filter(int nType, int nLauncherNpcIdx, int srcPosX, int srcPosY, int nTargetNpcIdx, void *pPassby)
{
	if(nType > tft_filter_begin && nType < tft_filter_end)
	{
		if(NULL != m_Filter[nType - 1])
			return (m_Filter[nType - 1])(nLauncherNpcIdx, srcPosX, srcPosY, nTargetNpcIdx, pPassby);
	}

	return true;
}

#endif