//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:3:13   14:55
//      File_base        : AutoRobotComDef
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
#ifndef _AutoRobotComDef_h
#define _AutoRobotComDef_h
#ifdef _AUTO_ROBOT

#define		INVALID_CELL_OBSTACLE	-1
#define		WALKCOST_NOWAY			-1
#define		WALKCOST_IN_NEARCELL	10
#define		WALKCOST_IN_DIAGONAL	14

enum enRobotMode
{
	enRobotMode_Begin = 0,

	enRobotMode_None,
	enRobotMode_AutoRun,
	enRobotMode_AutoAttack,
	//renderÌí¼Ó///
	enRobotMode_AutoPickUpItem,
	enRobotMode_ReadyForSell,
	enRobotMode_Selling,
	enRobotMode_WaitingForChangeMap,
	enRobotMode_GotoOtherMap,

	enRobotMode_End,
};

enum enSearchUnit
{
	enSearchUnit_Begin = 0,

	enSearchUnit_1by1Cell,
	enSearchUnit_2by2Cell,
	enSearchUnit_3by3Cell,

	enSearchUnit_End,
};

typedef struct _Coordinate
{
	int	x;
	int	y;

	bool operator< (const _Coordinate &rhs) const
	{
		if(x < rhs.x)
			return true;
		else if(x == rhs.x)
			return y < rhs.y;
		else
			return false;
	}

	bool operator== (const _Coordinate &rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

} Coordinate;

#endif	// #ifdef _AUTO_ROBOT
#endif // #ifndef _AutoRobotComDef_h