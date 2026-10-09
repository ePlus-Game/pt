//xiehong 2008-3-23 雇佣数据定义
#ifndef EMPLOMENT_DATA_DEF_H
#define EMPLOMENT_DATA_DEF_H

#include "GameDataDef.h"
#include "GlobalDef.h"

enum HireRetCode
{
	HRC_HIRE_SUCCESS = 0,	//雇佣成功
	HRC_HIRE_FAILED,		//雇佣失败
	HRC_BE_HIRED_SUCCESS,	//申请进入雇佣列表成功
	HRC_SEARCH_NO_RECORD,	//搜索雇佣列表没返回记录

	HRC_OPERATE_TOO_FAST,//操作过快
	HRC_ALREADY_HAS_EMPLOYEE,//已经雇佣了一个佣兵
	HRC_EMPLOYEE_OUT_OF_EMPLOY_TIME,//佣兵没有雇用时间了
	HRC_EMPLOYER_OUT_OF_EMPLOY_TIME,//雇主没有雇用时间了
	HRC_EMPLOYER_OUT_OF_MONEY,//雇主没有金钱了
	HRC_NO_EMPLOYEE,//没有佣兵
	HRC_EMPLOYEE_ON_LINE,//佣兵上线
	HRC_EMPLOYER_NOT_ENOUGH_EMPLOY_TIME,//雇主没有足够的雇用时间进行一次雇用
	HRC_EMPLOYER_NOT_ENOUGH_MONEY,//雇主没有足够的金钱进行一次雇用
	HRC_EMPLOYEE_FIGHT_MODE_LEVEL_REQUIRED,//战斗雇用等级要求不满足

	HRC_RET_CODE_COUNT,
};

enum HireType
{
	HT_EXP = 0,
	HT_FIGHT,
	HT_TYPE_COUNT,
};

enum ExpHireType
{
	EHT_NORMAL = 0,
	EHT_FAST,
	EHT_VERY_FAST,
	EHT_TYPE_COUNT,
};

enum MetierSelection
{
	enMetierSelectAll = 0,
	enMetierSelectXuanfeng,
	enMetierSelectXingtian,
	enMetierSelectZhenren,
	enMetierSelectTianshi,
	enMetierSelectShoushi,
	enMetierSelectYishi,
	enMetierSelectCount
};

#define HireRetListMaxCount 8
#define HireMinFighterHireTime 60  //是最小战斗雇佣时间——60秒
struct Metier
{
	BYTE major; //0,1,2
	BYTE minor; //-1,0,1
	Metier()
	{
		major = 0;
		minor = -1;
	}

	Metier(const Metier& other)
	{
		major = other.major;
		minor = other.minor;
	}
};

struct DualityNumber
{
	int high;
	int low;
	DualityNumber()
	{
		high = 0;
		low = 0;
	}
	DualityNumber(const DualityNumber& other)
	{
		high = other.high;
		low = other.low;
	}
};

struct ExpHirer
{
	char			name[COMMON_CLIENT_MSG_LEN_32];
	Metier			metier;
	int				level;
	char			shizu[COMMON_CLIENT_MSG_LEN_32];
	char			zhuhou[COMMON_CLIENT_MSG_LEN_32];
	BYTE			sex;

	ExpHirer()
	{
		name[0]		= 0;
		level		= 0;
		shizu[0]	= 0;
		zhuhou[0]	= 0;
		sex			= true;
	}
	ExpHirer(const ExpHirer& other)
	{
		strncpy(name,	other.name,		sizeof(name));
		name[sizeof(name) - 1] = 0;

		metier = other.metier;

		level = other.level;

		strncpy(shizu,	other.shizu,	sizeof(shizu));
		name[sizeof(shizu) - 1] = 0;

		strncpy(zhuhou, other.zhuhou,	sizeof(zhuhou));
		name[sizeof(zhuhou) - 1] = 0;

		sex = other.sex;
	}
};

struct FighterHirer
{
	char			name[COMMON_CLIENT_MSG_LEN_32];
	Metier			metier;
	int				level;
	DualityNumber	attack;
	DualityNumber	magic;
	int				armor;
	int				blood;
	int				salary;

	FighterHirer()
	{
		name[0]		= 0;
		level		= 0;
		armor		= 0;
		blood		= 0;
		salary		= 0;
	}

	FighterHirer(const FighterHirer& other)
	{
		strncpy(name,	other.name,		sizeof(name));
		name[sizeof(name) - 1] = 0;

		metier = other.metier;

		level = other.level;
	
		attack = other.attack;
		magic = other.magic;

		armor = other.armor;

		blood = other.blood;
		salary = other.salary;
	}
};

struct HireReqData
{
	int				startIndex;
	int				length;
	MetierSelection metier;
	int				highLevel;
	int				lowLevel;
	char			name[COMMON_CLIENT_MSG_LEN_32];
	HireType		hiretype;
	char			shizu[MAXSIZE_ORGNAME];
	char			zhuhou[MAXSIZE_ORGNAME];
	
	HireReqData()
	{
		startIndex	= 0;
		length		= 0;
		metier		= enMetierSelectAll;
		highLevel	= 100;
		lowLevel	= 0;
		name[0]		= 0;
		hiretype	= HT_EXP;

		shizu[0]	= 0;
		zhuhou[0]	= 0;
	}	
	HireReqData(const HireReqData& other)
	{
		startIndex	= other.startIndex;
		length		= other.length;
		metier		= other.metier;
		highLevel	= other.highLevel;
		lowLevel	= other.lowLevel;
		strncpy(name, other.name, sizeof(name));
		name[sizeof(name) - 1] = 0;
		hiretype	= other.hiretype;

		strncpy(shizu, other.shizu, sizeof(shizu));
		shizu[sizeof(shizu) - 1] = 0;
		
		strncpy(zhuhou, other.zhuhou, sizeof(zhuhou));
		zhuhou[sizeof(zhuhou) - 1] = 0;
	}
};

#endif //EMPLOMENT_DATA_DEF_H