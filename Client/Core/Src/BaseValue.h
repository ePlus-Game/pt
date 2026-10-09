//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-9-25 16:26
//      File_base        : BaseValue
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
#ifndef _BaseValue_h
#define	_BaseValue_h

#include "cfs_common_def.h"

class PlayerBaseNumeric
{
public:
	inline static int Body2LifeUpLimit(int nSeries, int nVal);
	inline static int Nimbus2ManaUpLimit(int nSeries, int nVal);
	inline static int Strength2Damage(int nSeries, int nVal);
	inline static int Magic2Damage(int nSeries, int nVal);

	inline static int Nimbus2MagicExplode(int nVal);
	inline static int Nimbus2Vision(int nVal);
	inline static int Strength2PhysDefense(int nVal);
	inline static int Strength2WeightMax(int nVal);
	inline static int Body2PhysExplode(int nVal);
	inline static int Body2Dexterity(int nVal);
	inline static int Magic2EightDiagDefense(int nVal);
	inline static int Magic2DarkDefense(int nVal);
	
};

inline int PlayerBaseNumeric::Body2LifeUpLimit(int nSeries, int nVal)
{
	switch(nSeries)
	{
	case enRoleType_Knight:
		return nVal * 10;

	case enRoleType_Enchanter:
		return nVal * 5;

	case enRoleType_Monstrous:
		return nVal * 7;

	default:
		return 0;
	}
}

inline int PlayerBaseNumeric::Nimbus2ManaUpLimit(int nSeries, int nVal)
{
	switch(nSeries)
	{
	case enRoleType_Knight:
		return nVal * 7;
		
	case enRoleType_Enchanter:
		return nVal * 15;
		
	case enRoleType_Monstrous:
		return nVal * 10;
		
	default:
		return 0;
	}
}

inline int PlayerBaseNumeric::Strength2Damage(int nSeries, int nVal)
{
	switch(nSeries)
	{
	case enRoleType_Knight:
		return nVal / 3;
		
	case enRoleType_Enchanter:
		return nVal / 5;
		
	case enRoleType_Monstrous:
		return nVal / 4;

	default:
		return 0;
	}
}

inline int PlayerBaseNumeric::Magic2Damage(int nSeries, int nVal)
{
	switch(nSeries)
	{
	case enRoleType_Knight:
		return 3 * nVal / 10;
		
	case enRoleType_Enchanter:
		return 3 * nVal / 6;
		
	case enRoleType_Monstrous:
		return 3 * nVal / 8;
		
	default:
		return 0;
	}
}

inline int PlayerBaseNumeric::Nimbus2MagicExplode(int nVal)
{
	return nVal / 20;
}

inline int PlayerBaseNumeric::Nimbus2Vision(int nVal)
{
	return nVal / 5;
}

inline int PlayerBaseNumeric::Strength2PhysDefense(int nVal)
{
	return (nVal >> 10) / 20;
}

inline int PlayerBaseNumeric::Strength2WeightMax(int nVal)
{
	return (nVal >> 10) / 5;
}

inline int PlayerBaseNumeric::Body2PhysExplode(int nVal)
{
	return nVal / 20;
}

inline int PlayerBaseNumeric::Body2Dexterity(int nVal)
{
	return nVal / 5;
}

inline int PlayerBaseNumeric::Magic2EightDiagDefense(int nVal)
{
	return (nVal >> 10) / 20;
}

inline int PlayerBaseNumeric::Magic2DarkDefense(int nVal)
{
	return (nVal >> 10) / 20;
}

#endif