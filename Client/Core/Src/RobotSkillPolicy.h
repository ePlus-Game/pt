//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:9   11:50
//      File_base        : RobotSkillPolicy
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
#ifndef _RobotSkillPolicy_h
#define _RobotSkillPolicy_h
#ifdef	_AUTO_ROBOT

#include <vector>

using namespace std;

class RobotSkillPolicy
{
public:
	bool	Initialize();
	int		GetUsableSkill(int	nNearestEnemyIdx);

private:
	int		CalcTotalDamage(const SkillDamageInfo *pDamInfo);
	bool	IsSkillCanUse(int nLauncher, int nTarget, int nSkillId);

public:
typedef struct _RobotSkillInfo
	{
		int		skillId;
		int		attackRadius;
		int		totalDamage;

	} RobotSkillInfo;

private:
	typedef vector<RobotSkillInfo>	RobotSkillInfoCont;

	RobotSkillInfoCont	m_RobotSkillInfo;	
};

#endif // #ifdef _AUTO_ROBOT
#endif // #ifdef _RobotSkillPolicy_h