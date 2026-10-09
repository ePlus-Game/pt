//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-03-19 12:13
//      File_base        : employ
//      File_ext         : cpp
//      Author           : 徐晓刚
//      Description      : 雇用系统
//
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KPlayer.h"
#include "KNpcSet.h"
#include "buff_man.h"
#include "KSubWorld.h"
#include "SocialUnit.h"
#include "CoreRelated.h"
#include "ChatCenter_S.h"
#include "employ.h"

#define EMPLOYEE_TRANS_RANGE 800
#define EMPLOYEE_FOLLOW_MIN_RANGE 100
#define EMPLOYEE_MAX_TARGET_DISTANCE 1024
#define DEFAULT_EMPLOYEE_PAY_INTERVAL 60
#define DEFAULT_FIGHT_REQUIRE_LEVEL_MIN 1
#define DEFAULT_FIGHT_REQUIRE_LEVEL_MAX MAX_LEVEL
#define	BASE_WALK_SPEED	5
#define	BASE_ATTACK_SPEED 0
#define	BASE_CAST_SPEED 0
#define	BASE_VISION_RADIUS 120
#define MAX_SEARCH_EMPLOY_LIMIT 32
#define MAX_SEARCH_EMPLOY_OFFSET 10000
#define MAX_SECOND_SALARY 10000
#define MAX_EMPLOY_TIME 86400

Employee::Employee()
{
	Init();
}

Employee::~Employee()
{
}

void Employee::Init()
{
	m_EmployeeNpcIndex = 0;
	m_NextActiveTime = 0;
	m_NextPayTime = 0;
	m_EmployType = 0;
	m_EmploySalary = 0;
	m_EmployExpireTime = 0;
}

int Employee::Employ(int employeeNpcIndex, int type, DWORD employTime, DWORD salary)
{
	if (!IsValidNpc(employeeNpcIndex))
		return FALSE;

	if (type == 1)
	{
		m_EmployExpireTime = UNIX_TMIE_STAMP + employTime;
	}
	else
	{
		m_EmployExpireTime = 0;
	}
	
	m_EmployeeNpcIndex = employeeNpcIndex;
	m_EmployType = type;
	m_EmploySalary = salary;
	m_NextPayTime = UNIX_TMIE_STAMP + EmployCenter::Singleton().GetPayInterval();
	Npc[m_EmployeeNpcIndex].SetEmployerIdx(m_PlayerIndex);
	return TRUE;
}

int Employee::Fire()
{
	if (!IsExist())
		return FALSE;

	EmployCenter::Singleton().Fire(m_PlayerIndex, m_EmployeeNpcIndex);
	m_EmployeeNpcIndex = 0;
	m_NextPayTime = 0;
	return TRUE;
}

void Employee::Active()
{
	if (!IsExist())
		return;

	if (!IsValidPlayer(m_PlayerIndex))
		return;

	KPlayer& player = Player[m_PlayerIndex];
	if (!IsValidNpc(player.GetNpcIndex()))
		return;

	int worldIndex = Npc[player.GetNpcIndex()].m_SubWorldIndex;
	if (worldIndex < 0 || worldIndex >= MAX_SUBWORLD)
		return;

	if (SubWorld[worldIndex].m_dwCurrentTime % 3 != 0)
		return;

	//是否到期
	if (IsExpired())
	{
		Fire();
		return;
	}

	//计费
	if (NeedPay())
	{
		Pay();
	}

	KNpc& employerNpc = Npc[player.GetNpcIndex()];
	KNpc& employeeNpc = Npc[m_EmployeeNpcIndex];

	int	distance = NpcSet.GetDistance(player.GetNpcIndex(), m_EmployeeNpcIndex);	
	if(distance > EMPLOYEE_TRANS_RANGE)
	{
		int x, y;
		employerNpc.GetMpsPos(&x, &y);
		employeeNpc.SetPos(x, y);
		employeeNpc.SetTarget(type_npc, 0);
	}
	else
	{
		int	target = employeeNpc.GetTargetNpc();
		int nSkillIdx = SelectUsableSkill();
		if(IsValidNpc(target) && !(do_death == Npc[target].m_Doing || do_revive == Npc[target].m_Doing) && INVALID_SKILL_INDEX != nSkillIdx)
		{
			employeeNpc.SetActiveSkill(nSkillIdx);
			int nDistanceEnemy = NpcSet.GetDistance(m_EmployeeNpcIndex, target);
			if (nDistanceEnemy < employeeNpc.m_CompAttrMgr[ncai_attackradius]) // 可以打了
			{
				employeeNpc.SendCommand(do_skill, employeeNpc.m_ActiveSkillID, -1, target);
			}
			else
			{
				if(nDistanceEnemy < EMPLOYEE_MAX_TARGET_DISTANCE)
				{
					int nDesX, nDesY;
					Npc[target].GetMpsPos(&nDesX, &nDesY);
					employeeNpc.SendCommand(do_run, nDesX, nDesY);
				}
			}
		}
		else
		{	
			employeeNpc.SetTarget(type_npc, 0);

			if(distance > EMPLOYEE_FOLLOW_MIN_RANGE)
			{
				int x, y;
				employerNpc.GetMpsPos(&x, &y);
				employeeNpc.SendCommand(do_run, x, y);
			}
		}
	}
}

int Employee::SelectUsableSkill()
{
	if (!IsValidNpc(m_EmployeeNpcIndex))
		return INVALID_SKILL_INDEX;

	NpcSkillList &skillList = Npc[m_EmployeeNpcIndex].GetSkillList();
	for(int nSkillIdx = 0; nSkillIdx < MAX_EMPLOYEE_SKILL_COUNT; ++nSkillIdx)
	{
		int nSkillId = skillList.GetIdByIdx(nSkillIdx);
	
		if(INVALID_SKILL_ID == nSkillId)
			continue;
		
		if( !skillList.CanCast(nSkillId) )
			continue;

		return nSkillIdx;
	}

	return INVALID_SKILL_INDEX;
}

bool Employee::NeedPay() const
{
	return (m_NextPayTime <= UNIX_TMIE_STAMP);
}

bool Employee::IsExpired() const
{
	return (m_EmployExpireTime > 0 && m_EmployExpireTime <= UNIX_TMIE_STAMP);
}

void Employee::Pay()
{
	if (m_NextPayTime > 0)
	{	
		EmployCenter::Singleton().Pay(m_PlayerIndex, m_EmployeeNpcIndex);
	}

	m_NextPayTime = UNIX_TMIE_STAMP + EmployCenter::Singleton().GetPayInterval();
}

DWORD Employee::GetUnpayedTime() const
{
	if (m_NextPayTime > 0)
	{
		DWORD payInterval = (DWORD)EmployCenter::Singleton().GetPayInterval();
		if (payInterval > 0 && (UNIX_TMIE_STAMP + payInterval > m_NextPayTime))
		{
			return UNIX_TMIE_STAMP + payInterval - m_NextPayTime;
		}
	}
	
	return 0;
}

//-----------------------------------------------------------------------------------

EmployCenter::EmployCenter()
{
}

EmployCenter::~EmployCenter()
{
}

EmployCenter& EmployCenter::Singleton()
{
	static EmployCenter center;
	return center;
}

bool EmployCenter::Load()
{
	m_PayInterval = DEFAULT_EMPLOYEE_PAY_INTERVAL;
	memset(m_ExpSettings, 0, sizeof(m_ExpSettings));
	memset(m_EmployeeSettings, 0, sizeof(m_EmployeeSettings));
	memset(m_LevelExp, 0, sizeof(m_LevelExp));
	m_NextCheckTime = 0;

	KIniFile configIniFile;
	if (TRUE == configIniFile.Load(EMPLOY_CONFIG_FILE))
	{
		char sectionName[64];
		char entryName[64];

		//技能和BUFF
		for(int roleType = 0; roleType < 3; roleType++)
		{
			for (int skillSeries = -1; skillSeries < 2; skillSeries++)
			{
				for (int employType = 0; employType < HT_TYPE_COUNT; employType++)
				{
					EmployeeSetting& setting = m_EmployeeSettings[roleType][skillSeries + 1][employType];
					
					snprintf(sectionName, sizeof(sectionName), "Role_%d_%d_%d", roleType, skillSeries, employType);
					sectionName[sizeof(sectionName) - 1] = 0;
					
					for (int buffIndex = 0; buffIndex < MAX_EMPLOYEE_BUFF_COUNT; buffIndex++)
					{
						snprintf(entryName, sizeof(entryName), "Buff_%d", buffIndex + 1);
						configIniFile.GetInteger(sectionName, entryName, 0, &(setting.Buffs[buffIndex]));
					}
					
					for (int skillIndex = 0; skillIndex < MAX_EMPLOYEE_SKILL_COUNT; skillIndex++)
					{
						snprintf(entryName, sizeof(entryName), "Skill_%d", skillIndex + 1);
						configIniFile.GetInteger(sectionName, entryName, 0, &(setting.Skills[skillIndex]));
					}
				}
			}
		}

		//经验获得比率
		int expSettingCount = 0;
		configIniFile.GetInteger("Setting", "ExpSettingCount", 0, &expSettingCount);
		for (int expSettingIndex = 0; expSettingIndex < expSettingCount; expSettingIndex++)
		{
			snprintf(sectionName, sizeof(sectionName), "ExpSetting_%d", expSettingIndex);
			sectionName[sizeof(sectionName) - 1] = 0;

			ExpSetting& setting = m_ExpSettings[expSettingIndex];

			configIniFile.GetInteger(sectionName, "ExpRewardPlus", 0, &(setting.ExpRewardPlus));
			configIniFile.GetInteger(sectionName, "TimeElapseRate", 0, &(setting.TimeElapseRate));
		}

		//付费间隔
		configIniFile.GetInteger("Setting", "PayInterval", DEFAULT_EMPLOYEE_PAY_INTERVAL, (int*)&m_PayInterval);

		//战斗雇用需求最低等级
		configIniFile.GetInteger("Setting", "FightRequireLevelMin", DEFAULT_FIGHT_REQUIRE_LEVEL_MIN, (int*)&m_FightRequireLevelMin);
		if (m_FightRequireLevelMin <= 0 || m_FightRequireLevelMin > MAX_LEVEL)
			m_FightRequireLevelMin = DEFAULT_FIGHT_REQUIRE_LEVEL_MIN;

		//战斗雇用需求最高等级
		configIniFile.GetInteger("Setting", "FightRequireLevelMax", DEFAULT_FIGHT_REQUIRE_LEVEL_MAX, (int*)&m_FightRequireLevelMax);
		if (m_FightRequireLevelMax <= 0 || m_FightRequireLevelMax > MAX_LEVEL)
			m_FightRequireLevelMax = DEFAULT_FIGHT_REQUIRE_LEVEL_MAX;
	}
	else
	{
		return false;
	}

	KTabFile levelExpTabFile;
	if (TRUE == levelExpTabFile.Load(EMPLOY_LEVEL_EXP_CONFIG_FILE))
	{		
		int row = 0;
		int recordCount = levelExpTabFile.GetHeight() - 1;
		if (recordCount < 0)
			recordCount = 0;
		if (recordCount > MAX_LEVEL)
			recordCount = MAX_LEVEL;
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			levelExpTabFile.GetInteger(row, 2, 0, (int*)&(m_LevelExp[record]));
		}
	}
	else
	{
		return false;
	}

	return true;
}

void EmployCenter::Active()
{
	if (m_NextCheckTime < UNIX_TMIE_STAMP)
	{
		CheckNoTimeEmploy();
		m_NextCheckTime = UNIX_TMIE_STAMP + GetPayInterval();
	}
}

void EmployCenter::CheckNoTimeEmploy()
{
	if (NULL == g_pController)
		return;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return;
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_CheckEmploy;
	pParam->BeginPush( PN_CHECK_EMPLOY );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int EmployCenter::SearchEmployee(int playerIndex, HireReqData& filter)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& player = Player[playerIndex];

	if (!player.CanDoOp(player_op_search))
	{
		SendResultToClient(playerIndex, HRC_OPERATE_TOO_FAST);
		return FALSE;
	}

	player.UpdateOpTime(player_op_search, 2);

	int offset = filter.startIndex;
	if (offset < 0)
	{
		offset = 0;
	}
	else if (offset > MAX_SEARCH_EMPLOY_OFFSET)
	{
		offset = MAX_SEARCH_EMPLOY_OFFSET;
	}

	int limit = filter.length < MAX_SEARCH_EMPLOY_LIMIT ? filter.length : MAX_SEARCH_EMPLOY_LIMIT;

	int employType = filter.hiretype;

	int levelLow = filter.lowLevel;
	if (levelLow < 0)
	{
		levelLow = 0;
	}

	int levelHigh = filter.highLevel;
	if (levelHigh > MAX_LEVEL)
	{
		levelHigh = MAX_LEVEL;
	}

	int roleType = 0;
	int skillSeries = 0;	
	switch(filter.metier)
	{
	case enMetierSelectAll:
		roleType = -1;
		skillSeries = -1;
		break;
	case enMetierSelectXuanfeng:
		roleType = 0;
		skillSeries = 0;
		break;
	case enMetierSelectXingtian:
		roleType = 0;
		skillSeries = 1;
		break;
	case enMetierSelectZhenren:
		roleType = 1;
		skillSeries = 0;
		break;
	case enMetierSelectTianshi:
		roleType = 1;
		skillSeries = 1;
		break;
	case enMetierSelectShoushi:
		roleType = 2;
		skillSeries = 0;
		break;
	case enMetierSelectYishi:
		roleType = 2;
		skillSeries = 1;
		break;
	}

	filter.name[sizeof(filter.name) - 1] = 0;
	filter.shizu[sizeof(filter.shizu) - 1] = 0;
	filter.zhuhou[sizeof(filter.zhuhou) - 1] = 0;

	char roleName[MAXSIZE_ROLENAME] = { 0 };
	strncpy(roleName, filter.name, sizeof(roleName));
	roleName[sizeof(roleName) - 1] = 0;
	if (HasSqlKeyWord(roleName, sizeof(roleName)))
		return FALSE;

	char shizuName[MAXSIZE_ORGNAME] = { 0 };
	strncpy(shizuName, filter.shizu, sizeof(shizuName));
	shizuName[sizeof(shizuName) - 1] = 0;
	if (HasSqlKeyWord(shizuName, sizeof(shizuName)))
		return FALSE;

	char zhuhouName[MAXSIZE_ORGNAME] = { 0 };
	strncpy(zhuhouName, filter.zhuhou, sizeof(zhuhouName));
	zhuhouName[sizeof(zhuhouName) - 1] = 0;
	if (HasSqlKeyWord(zhuhouName, sizeof(zhuhouName)))
		return FALSE;

	if (NULL == g_pController)
		return FALSE;

	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	_SearchEmployeeDBHeader DBHeader;
	memset(&DBHeader, 0, sizeof(DBHeader));
	DBHeader.ulNetID = player.GetNetConnectIdx();
	DBHeader.ProcType = Proc_SearchEmploy;
	DBHeader.employType = employType;
	DBHeader.offset = offset;
	pParam->BeginPush( PN_SEARCH_EMPLOY );
	pParam->Push( offset );
	pParam->Push( limit );
	pParam->Push( employType );	
	pParam->Push( levelLow );
	pParam->Push( levelHigh );
	
	if (roleType >= 0)
		pParam->Push( roleType );
	else
		pParam->Push( NullPair( ) );

	if (skillSeries >= 0)
		pParam->Push( skillSeries );
	else
		pParam->Push( NullPair( ) );

	if (strlen(roleName) > 0)
		pParam->Push( roleName );
	else
		pParam->Push( NullPair( ) );

	if (strlen(shizuName) > 0)
		pParam->Push( shizuName );
	else
		pParam->Push( NullPair( ) );

	if (strlen(zhuhouName) > 0)
		pParam->Push( zhuhouName );
	else
		pParam->Push( NullPair( ) );
	
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

void EmployCenter::SearchEmployDbOpComplete(int playerIndex, IProcRet* pRet)
{
	if (!IsValidPlayer(playerIndex))
		return;

	if (NULL == pRet)
		return;
	
	if (!(pRet->GetRet() && pRet->GetExeRet()))
		return;

	int nPassBySize = 0;
	char* pPassBy = pRet->GetPassBy( nPassBySize );
	
	if( nPassBySize != sizeof(_SearchEmployeeDBHeader) || pPassBy == NULL )
		return;
	
	_SearchEmployeeDBHeader* pHeader = (_SearchEmployeeDBHeader*)pPassBy;
	int employType = pHeader->employType;

	if (employType == HT_EXP)
	{
		char sendBuff[MAX_HIRE_LIST_SYNC_BUFF_LENGTH];
		memset(sendBuff, 0, sizeof(sendBuff));
		PEXP_HIRE_LIST pHireList = (PEXP_HIRE_LIST)sendBuff;
		pHireList->Protocol = s2c_hire_data_list_exp;
		pHireList->Length = sizeof(EXP_HIRE_LIST) - sizeof(pHireList->data) - 1;
		pHireList->Count = 0;
		pHireList->stargIndex = pHeader->offset;

		int nCol = 0;
		int rowCount = pRet->GetRowCount();
// 		if (rowCount == 0)
// 		{
// 			SendResultToClient(playerIndex, HRC_SEARCH_NO_RECORD);
// 			return;
// 		}

		if (rowCount > MAX_HIRE_LIST_COUNT)
			rowCount = MAX_HIRE_LIST_COUNT;

		for (int nRow = 0; nRow < rowCount; nRow++)
		{
			ExpHirer& info = pHireList->data[nRow];

			//EmployeeName,RoleSex,RoleLevel,RoleType,SkillSeries,ShiZu,ZhuHou

			nCol = 0;
			pRet->GetData(nRow, nCol++, info.name, sizeof(info.name));
			pRet->GetData(nRow, nCol++, info.sex);
			pRet->GetData(nRow, nCol++, info.level);
			pRet->GetData(nRow, nCol++, info.metier.major);
			pRet->GetData(nRow, nCol++, info.metier.minor);
			pRet->GetData(nRow, nCol++, info.shizu, sizeof(info.shizu));
			pRet->GetData(nRow, nCol++, info.zhuhou, sizeof(info.zhuhou));

			pHireList->Count++;
			pHireList->Length += sizeof(ExpHirer);
		}

		if (CompressProtocol((BYTE*)sendBuff, pHireList->Length + 1, sizeof(sendBuff)))
		{			
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), sendBuff, pHireList->Length + 1);
		}
	}
	else if (employType == HT_FIGHT)
	{
		char sendBuff[MAX_HIRE_LIST_SYNC_BUFF_LENGTH];
		memset(sendBuff, 0, sizeof(sendBuff));
		PFIGHTER_HIRE_LIST pHireList = (PFIGHTER_HIRE_LIST)sendBuff;
		pHireList->Protocol = s2c_hire_data_list_fighter;
		pHireList->Length = sizeof(FIGHTER_HIRE_LIST) - sizeof(pHireList->data) - 1;
		pHireList->Count = 0;
		pHireList->stargIndex = pHeader->offset;

		int nCol = 0;
		int rowCount = pRet->GetRowCount();
// 		if (rowCount == 0)
// 		{
// 			SendResultToClient(playerIndex, HRC_SEARCH_NO_RECORD);
// 			return;
// 		}

		if (rowCount > MAX_HIRE_LIST_COUNT)
			rowCount = MAX_HIRE_LIST_COUNT;

		for (int nRow = 0; nRow < rowCount; nRow++)
		{
			FighterHirer& info = pHireList->data[nRow];

			//EmployeeName,RoleSex,RoleLevel,RoleType,SkillSeries,PAttackLow,PAttackHigh,MAttackLow,MAttackHigh,PDefend,MaxLife,Salary

			nCol = 0;
			pRet->GetData(nRow, nCol++, info.name, sizeof(info.name));
			nCol++;//RoleSex
			pRet->GetData(nRow, nCol++, info.level);
			pRet->GetData(nRow, nCol++, info.metier.major);
			pRet->GetData(nRow, nCol++, info.metier.minor);
			pRet->GetData(nRow, nCol++, info.attack.low);
			pRet->GetData(nRow, nCol++, info.attack.high);
			pRet->GetData(nRow, nCol++, info.magic.low);
			pRet->GetData(nRow, nCol++, info.magic.high);
			pRet->GetData(nRow, nCol++, info.armor);
			pRet->GetData(nRow, nCol++, info.blood);
			pRet->GetData(nRow, nCol++, info.salary);

			pHireList->Count++;
			pHireList->Length += sizeof(FighterHirer);
		}

		if (CompressProtocol((BYTE*)sendBuff, pHireList->Length + 1, sizeof(sendBuff)))
		{
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), sendBuff, pHireList->Length + 1);
		}
	}
}

int EmployCenter::Post(int playerIndex, int type, int secondSalary, int rateIndex)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& player = Player[playerIndex];

	if (!player.CanDoOp(player_op_post))
	{
		SendResultToClient(playerIndex, HRC_OPERATE_TOO_FAST);
		return FALSE;
	}

	player.UpdateOpTime(player_op_post, 2);

	if (!IsValidNpc(player.GetNpcIndex()))
		return FALSE;

	KNpc& npc = Npc[player.GetNpcIndex()];

	if (NULL == g_pController)
		return FALSE;

	if (!IsValidEmployType(type))
		return FALSE;

	if (!IsValidSalary(secondSalary))
		return FALSE;

	if (!IsValidRateIndex(rateIndex))
		return FALSE;

	//检查条件
	switch (type)
	{
	case HT_EXP:
		{
		}
		break;
	case HT_FIGHT:
		{
			if (player.GetLevel() < m_FightRequireLevelMin || player.GetLevel() > m_FightRequireLevelMax)
			{
				SendResultToClient(playerIndex, HRC_EMPLOYEE_FIGHT_MODE_LEVEL_REQUIRED);
				return FALSE;
			}
		}
		break;
	}

	//阻止玩家在副本中上榜
	int subworldIndex = npc.GetSubWorldIndex();
	if (subworldIndex > 0 && subworldIndex < MAX_SUBWORLD)
	{
		if (SubWorld[subworldIndex].GetInstanceId() != INVALID_INSTANCE_ID)//在副本中
		{
			SendResultToClient(playerIndex, HRC_HIRE_FAILED);
			return FALSE;
		}
	}

	//上榜前添加雇佣BUFF
	int employBuffId = ConfigManager::Singleton().GetGlobalVariable(global_var_buff_employ);
	if (employBuffId > 0)
	{
		BuffMgr::Singleton().AddNpcBuff(player.GetNpcIndex(), player.GetNpcIndex(), employBuffId);
	}

	int employType = type;
	int employSalary = secondSalary;
	int employTimeElapseRate = GetEmployTimeElapseRate(rateIndex);
	const char* employeeName = npc.Name;
	int employeeSex = npc.GetSex();
	int employeeLevel = npc.GetLevel();
	int employeeType = npc.GetSeries();
	int employeeSkillSeries = player.GetSkillSeries();
	int employeeFace = npc.m_nHeadImage;
	int employeeHelmType = CombinTP( npc.m_HelmType, npc.m_HelmPal );
	int employeeArmorType = CombinTP( npc.m_ArmorType, npc.m_ArmorPal );
	int employeeWeaponType = CombinTP2WORD( npc.m_WeaponType, npc.m_WeaponPal );
	int employeeShoulderType = CombinTP( npc.m_ShoulderType, npc.m_ShoulderPal );
	int employeeCuffType = CombinTP( npc.m_CuffType, npc.m_CuffPal );
	int employeeBootType = CombinTP( npc.m_BootType, npc.m_BootPal );
	int employeeHorseType = CombinTP( npc.m_HorseType, npc.m_HorsePal );
	int employeeRideHorse = npc.m_bRideHorse;
	int employeeStrength = npc.m_CompAttrMgr[ncai_strength][idx_current_value];
	int employeeNimbus = npc.m_CompAttrMgr[ncai_nimbus][idx_current_value];
	int employeeBody = npc.m_CompAttrMgr[ncai_body][idx_current_value];
	int employeeArt = npc.m_CompAttrMgr[ncai_art][idx_current_value];
	int employeeMaxLifeBase = npc.m_CompAttrMgr[ncai_lifeuplimit][idx_base_value];
	int employeeMaxManaBase = npc.m_CompAttrMgr[ncai_manauplimit][idx_base_value];
	DWORD leftEmployTime = player.GetEmployTime();
	char shiZuName[MAXSIZE_ORGNAME] = { 0 };
	char zhuHouName[MAXSIZE_ORGNAME] = { 0 };	
	int employeePAttackLow = npc.CalcPhysicsDamage(idx_value_low) >> 10;
	int employeePAttackHigh = npc.CalcPhysicsDamage(idx_value_hight) >> 10;
	int employeeMAttackLow = npc.CalcMagicDamage(idx_value_low) >> 10;
	int employeeMAttackHigh	= npc.CalcMagicDamage(idx_value_hight) >> 10;
	int employeePDefend	= npc.CalcPhysicsDefense(idx_value_hight);
	int employeeMDefend	= npc.CalcEightDiagDfns(idx_value_hight);
	int employeeMaxLife = npc.m_CompAttrMgr[ncai_lifeuplimit][idx_current_value];
	//int employeeMaxMana = npc.m_CompAttrMgr[ncai_manauplimit][idx_current_value];
	
	SocialUnit *pLeafUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
	if(pLeafUnit)
	{
		//氏族
		SocialUnit *pOrgUnit = GetUpNUnit(pLeafUnit, enSULayer_Gens);
		if(pOrgUnit)
		{
			const char *szOrgName = GetUnitName(pOrgUnit->GetUnitAttr());
			if(szOrgName != NULL)
			{
				strncpy(shiZuName, szOrgName, MAXSIZE_ORGNAME);
				shiZuName[MAXSIZE_ORGNAME - 1] = 0;
			}
		}

		//诸侯
		pOrgUnit = GetUpNUnit(pLeafUnit, enSULayer_Tong);
		if(pOrgUnit)
		{
			const char *szOrgName = GetUnitName(pOrgUnit->GetUnitAttr());
			if(szOrgName != NULL)
			{
				strncpy(zhuHouName, szOrgName, MAXSIZE_ORGNAME);
				zhuHouName[MAXSIZE_ORGNAME - 1] = 0;
			}
		}
	}

	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = Player[playerIndex].GetNetConnectIdx();
	DBHeader.ProcType = Proc_Post;
	pParam->BeginPush( PN_POST );
	pParam->Push( employType );
	pParam->Push( employSalary );
	pParam->Push( employTimeElapseRate );	
	pParam->Push( employeeName );
	pParam->Push( employeeSex );
	pParam->Push( employeeLevel );
	pParam->Push( employeeType );
	pParam->Push( employeeSkillSeries );
	pParam->Push( employeeFace );
	pParam->Push( employeeHelmType );
	pParam->Push( employeeArmorType );
	pParam->Push( employeeWeaponType );
	pParam->Push( employeeShoulderType );
	pParam->Push( employeeCuffType );
	pParam->Push( employeeBootType );
	pParam->Push( employeeHorseType );
	pParam->Push( employeeRideHorse );
	pParam->Push( employeeStrength );
	pParam->Push( employeeNimbus );
	pParam->Push( employeeBody );
	pParam->Push( employeeArt );
	pParam->Push( employeeMaxLifeBase );
	pParam->Push( employeeMaxManaBase );
	pParam->Push( leftEmployTime );
	pParam->Push( shiZuName );
	pParam->Push( zhuHouName );
	pParam->Push( employeePAttackLow );
	pParam->Push( employeePAttackHigh );
	pParam->Push( employeeMAttackLow );
	pParam->Push( employeeMAttackHigh );
	pParam->Push( employeePDefend );
	pParam->Push( employeeMDefend );
	pParam->Push( employeeMaxLife );	

	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int result = g_pController->CallProc( cfs_db_cnn_role, pParam );
	if (result)
	{
		if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ))
		{
			LogEventParam logEvent;
			logEvent.event = log_event_employ_post;
			logEvent.param1 = player.GetGUID();
			snprintf(logEvent.param3.data, sizeof(logEvent.param3.data), "%d %d %d", type, secondSalary, rateIndex);
			logEvent.param3.data[sizeof(logEvent.param3.data) - 1] = 0;
			g_pLogSystem->Log(logEvent);
		}
	}

	return result;
}

void EmployCenter::PostDbOpComplete(int playerIndex, IProcRet* pRet)
{
	if (!IsValidPlayer(playerIndex))
		return;

	KPlayer& employeePlayer = Player[playerIndex];

	if (NULL == pRet)
		return;

	if (!(pRet->GetRet() && pRet->GetExeRet()))
		return;

	SendResultToClient(playerIndex, HRC_BE_HIRED_SUCCESS);
}

int EmployCenter::Cancel(int playerIndex)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& player = Player[playerIndex];

	if (NULL == g_pController)
		return FALSE;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = player.GetNetConnectIdx();
	DBHeader.ProcType = Proc_Cancel;
	pParam->BeginPush( PN_CANCEL );
	pParam->Push( player.GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

void EmployCenter::CancelDbOpComplete(int playerIndex, IProcRet* pRet)
{
	if (!IsValidPlayer(playerIndex))
		return;

	KPlayer& player = Player[playerIndex];

	if (NULL == pRet)
		return;

	if (!(pRet->GetRet() && pRet->GetExeRet()))
		return;
		
	if (pRet->GetRowCount() != 1)
	{
		//TODO 没有找到
		return;
	}

	int state = 0;
	int employType = 0;
	int employTimeElapseRate = 0;
	DWORD beEmployedTime = 0;
	DWORD leftEmployTime = 0;
	DWORD salary = 0;
	char employerName[MAXSIZE_ROLENAME] = { 0 };

	int nCol = 0;	
	pRet->GetData(0, nCol++, state);
	pRet->GetData(0, nCol++, employType);	
	pRet->GetData(0, nCol++, employTimeElapseRate);
	pRet->GetData(0, nCol++, beEmployedTime);
	pRet->GetData(0, nCol++, leftEmployTime);
	pRet->GetData(0, nCol++, salary);
	pRet->GetData(0, nCol++, employerName, sizeof(employerName));
	
	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_employ_cancel;
		logEvent.param1 = player.GetGUID();
		g_pLogSystem->Log(logEvent);
	}

	//正在雇佣中
	if (state == 1)
	{
		//通知雇主解雇自己
		int employerPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(employerName);
		if (IsValidPlayer(employerPlayerIndex))
		{
			SendResultToClient(employerPlayerIndex, HRC_EMPLOYEE_ON_LINE);
// 			int nStrId = 11371;
// 			g_ChatCenterS.SysMsgToSomeone(employerPlayerIndex, SYSMSG_TYPE_ID, (const BYTE*)&nStrId, sizeof(nStrId));
 			Player[employerPlayerIndex].GetEmployee().Fire();
		}
	}

	//计算奖励
	switch(employType)
	{
	case HT_EXP://经验雇用
		{
			//计算剩余雇佣时间和被雇佣时间
			if (leftEmployTime < beEmployedTime * employTimeElapseRate)
			{
				beEmployedTime = leftEmployTime / employTimeElapseRate;
				leftEmployTime = 0;
			}
			else
			{
				leftEmployTime = leftEmployTime - beEmployedTime * employTimeElapseRate;
			}

			//获得经验奖励
			DWORD rewardExp = CalcRewardExp(player.GetLevel(), beEmployedTime, employTimeElapseRate);
			
			if (rewardExp > 0 && rewardExp <= MAX_ADD_EXP)
			{
				if (rewardExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount))
				{
					LogEventParam addExpEvent;
					addExpEvent.event = log_event_employ_employee_add_exp;
					addExpEvent.param1 = player.GetGUID();
					snprintf(addExpEvent.param3.data, sizeof(addExpEvent.param3.data), "%d cancel", employType);
					addExpEvent.param3.data[sizeof(addExpEvent.param3.data) - 1] = 0;
					addExpEvent.param4 = rewardExp;
					g_pLogSystem->Log(addExpEvent);
				}
				
				player.DirectAddExp(rewardExp);
			}

			//日志
			int payEmployTime = player.GetEmployTime() - leftEmployTime;
			if (payEmployTime >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ_time_change_threshold))
			{
				LogEventParam logParam;
				logParam.event = log_event_employ_employee_pay_employ_time;
				logParam.param1 = player.GetGUID();
				snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d cancel", employType);
				logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
				logParam.param4 = -payEmployTime;
				g_pLogSystem->Log(logParam);
			}

			//扣除雇用时间
			player.SetEmployTime(leftEmployTime);

			//TODO 通知客户端经验雇用获得经验值
		}
		break;
	case HT_FIGHT://战斗雇用
		{
			DWORD rewardMoney = CalcRewardMoney(beEmployedTime, salary);
			if (player.Earn(rewardMoney))
			{
				//日志
				if (rewardMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
				{
					LogEventParam logParam;
					logParam.event = log_event_employ_employee_earn_money;
					logParam.param1 = player.GetGUID();
					snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d cancel", employType);
					logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
					logParam.param4 = rewardMoney;
					g_pLogSystem->Log(logParam);
				}

				//TODO 通知客户端战斗雇用获得金钱
			}
		}
		break;
	}
}

int EmployCenter::Employ(int playerIndex,const char* pEmployeeName, int employTime)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& employerPlayer = Player[playerIndex];

	if (!employerPlayer.CanDoOp(player_op_employ))
	{
		SendResultToClient(playerIndex, HRC_OPERATE_TOO_FAST);
		return FALSE;
	}

	employerPlayer.UpdateOpTime(player_op_employ, 2);

	if (NULL == pEmployeeName)
		return FALSE;

	char employeeName[MAXSIZE_ROLENAME] = { 0 };
	memcpy(employeeName, pEmployeeName, sizeof(employeeName));
	employeeName[sizeof(employeeName) - 1] = 0;

	if (employTime < 0 || employTime > MAX_EMPLOY_TIME)
		return FALSE;

	if (employerPlayer.GetEmployee().IsExist())
	{
		SendResultToClient(playerIndex, HRC_ALREADY_HAS_EMPLOYEE);
		return FALSE;
	}

	if (NULL == g_pController)
		return FALSE;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	int employerMoney = employerPlayer.GetItemList().GetMoney(room_equipment);
	DWORD employerTime = employerPlayer.GetEmployTime();

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = employerPlayer.GetNetConnectIdx();
	DBHeader.ProcType = Proc_Employ;
	pParam->BeginPush( PN_EMPLOY );
	pParam->Push( employeeName );
	pParam->Push( employerPlayer.GetPlayerName() );
	pParam->Push( employerTime );
	pParam->Push( employerMoney );
	pParam->Push( employTime );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int result = g_pController->CallProc( cfs_db_cnn_role, pParam );
	if (result)
	{
		if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ))
		{
			LogEventParam logEvent;
			logEvent.event = log_event_employ_try_employ;
			logEvent.param1 = employerPlayer.GetGUID();
			strncpy(logEvent.param2.data, employeeName, sizeof(logEvent.param2.data));
			logEvent.param2.data[sizeof(logEvent.param2.data) - 1] = 0;
			g_pLogSystem->Log(logEvent);
		}
	}

	return result;	
}

void EmployCenter::EmployDbOpComplete(int playerIndex, IProcRet* pRet)
{
	if (!IsValidPlayer(playerIndex))
		return;

	//雇主Player
	KPlayer& employerPlayer = Player[playerIndex];

	if (employerPlayer.GetEmployee().IsExist())
		return;
	
	if (!IsValidNpc(employerPlayer.GetNpcIndex()))
		return;

	//雇主NPC
	KNpc& employerNpc = Npc[employerPlayer.GetNpcIndex()];

	if (NULL == pRet || !pRet->GetExeRet())
	{
		SendResultToClient(playerIndex, HRC_HIRE_FAILED);
		return;
	}

	switch(pRet->GetRet())
	{
	case 1://雇用成功
		break;
	case 2://雇主雇用时间不够一次付费间隔了
		SendResultToClient(playerIndex, HRC_EMPLOYER_NOT_ENOUGH_EMPLOY_TIME);
		return;
	case 3://雇主金钱不够一次付费间隔了
		SendResultToClient(playerIndex, HRC_EMPLOYER_NOT_ENOUGH_MONEY);
		return;
	default:
		SendResultToClient(playerIndex, HRC_HIRE_FAILED);
		return;
	}
	
	if (pRet->GetRowCount() != 1)
	{
		SendResultToClient(playerIndex, HRC_HIRE_FAILED);
		return;
	}
	
	int employType = 0;
	int employTime = 0;
	int employSalary = 0;
	char employeeName[MAXSIZE_ROLENAME] = { 0 };
	int employeeSex = 0;
	int employeeLevel = 0;
	int employeeType = 0;
	int employeeSkillSeries = 0;
	int employeeFace = 0;
	int employeeHelmType = 0;
	int employeeArmorType = 0;
	int employeeWeaponType = 0;
	int employeeShoulderType = 0;
	int employeeCuffType = 0;
	int employeeBootType = 0;
	int employeeHorseType = -1;
	int employeeRideHorse = 0;
	int pAttackLow = 0;
	int pAttackHigh = 0;
	int mAttackLow = 0;
	int mAttackHigh = 0;
	int pDefend = 0;
	int mDefend = 0;
	int maxLife = 0;

	int nCol = 0;
	
	pRet->GetData(0, nCol++, employType);
	pRet->GetData(0, nCol++, employTime);	
	pRet->GetData(0, nCol++, employSalary);
	pRet->GetData(0, nCol++, employeeName, sizeof(employeeName));
	pRet->GetData(0, nCol++, employeeSex);
	pRet->GetData(0, nCol++, employeeLevel);
	pRet->GetData(0, nCol++, employeeType);
	pRet->GetData(0, nCol++, employeeSkillSeries);
	pRet->GetData(0, nCol++, employeeFace);
	pRet->GetData(0, nCol++, employeeHelmType);
	pRet->GetData(0, nCol++, employeeArmorType);
	pRet->GetData(0, nCol++, employeeWeaponType);
	pRet->GetData(0, nCol++, employeeShoulderType);
	pRet->GetData(0, nCol++, employeeCuffType);
	pRet->GetData(0, nCol++, employeeBootType);
	pRet->GetData(0, nCol++, employeeHorseType);
	pRet->GetData(0, nCol++, employeeRideHorse);
	pRet->GetData(0, nCol++, pAttackLow);
	pRet->GetData(0, nCol++, pAttackHigh);
	pRet->GetData(0, nCol++, mAttackLow);
	pRet->GetData(0, nCol++, mAttackHigh);
	pRet->GetData(0, nCol++, pDefend);
	pRet->GetData(0, nCol++, mDefend);
	pRet->GetData(0, nCol++, maxLife);

	int npcTemplateIndex = 0;
	if (employeeSex)
		npcTemplateIndex = MAKELONG(employeeLevel, -(employeeType + 4));
	else
		npcTemplateIndex = MAKELONG(employeeLevel, -(employeeType + 1));

	const int employeeNpcIndex = NpcSet.Add(npcTemplateIndex, employerNpc.m_SubWorldIndex, employerNpc.m_RegionIndex, employerNpc.GetMapX(), employerNpc.GetMapY(), false);
	if (!IsValidNpc(employeeNpcIndex))
	{
		//TODO 需要重置记录状态
		return;
	}
	
	KNpc& employeeNpc = Npc[employeeNpcIndex];

	const int nameSize = sizeof(employeeName) < sizeof(employeeNpc.Name) ? sizeof(employeeName) : sizeof(employeeNpc.Name);
	memcpy(employeeNpc.Name, employeeName, nameSize);

	employeeNpc.m_Kind = kind_employee;
	employeeNpc.m_Doing = do_stand;
	employeeNpc.m_Level = employeeLevel;
	employeeNpc.m_nHeadImage = employeeFace;
	employeeNpc.m_SkillType = employeeSkillSeries;

	//换装部件
// 	employeeNpc.m_HelmType = employeeHelmType;
// 	employeeNpc.m_ArmorType = employeeArmorType;
// 	employeeNpc.m_WeaponType = employeeWeaponType;
// 	employeeNpc.m_ShoulderType = employeeShoulderType;
// 	employeeNpc.m_CuffType = employeeCuffType;
// 	employeeNpc.m_BootType = employeeBootType;

	SplitTPfromWORD( employeeWeaponType,	employeeNpc.m_WeaponType,	employeeNpc.m_WeaponPal		);
	SplitTP( employeeHelmType,		employeeNpc.m_HelmType,		employeeNpc.m_HelmPal		);
	SplitTP( employeeArmorType,		employeeNpc.m_ArmorType,	employeeNpc.m_ArmorPal		);
	SplitTP( employeeShoulderType,	employeeNpc.m_ShoulderType,	employeeNpc.m_ShoulderPal	);
	SplitTP( employeeBootType,		employeeNpc.m_BootType,		employeeNpc.m_BootPal		);
	SplitTP( employeeCuffType,		employeeNpc.m_CuffType,		employeeNpc.m_CuffPal		);
	//SplitTP( employeeHorseType,		employeeNpc.m_HorseType,	employeeNpc.m_HorsePal		);
	//employeeNpc.m_HorseType = employeeHorseType;
	//employeeNpc.m_bRideHorse = employeeRideHorse;

	employeeNpc.RestoreNpcBaseInfo();

	//数值
	employeeNpc.m_CompAttrMgr.Set(ncai_liferenewspeed, idx_base_value, PLAYER_LIFE_REPLENISH);
	employeeNpc.m_CompAttrMgr.Set(ncai_manarenewspeed, idx_base_value, PLAYER_MANA_REPLENISH);	
	employeeNpc.m_CompAttrMgr.Set(ncai_walkspeed, idx_base_value, BASE_WALK_SPEED);
	employeeNpc.m_CompAttrMgr.Set(ncai_attackspeed, idx_base_value, BASE_ATTACK_SPEED);
	employeeNpc.m_CompAttrMgr.Set(ncai_castspeed, idx_base_value, BASE_CAST_SPEED);
	employeeNpc.m_CompAttrMgr.Set(ncai_visionradius, idx_base_value, BASE_VISION_RADIUS);
	employeeNpc.m_RangeAttrMgr.Set(nrai_damage_physics, idx_value_low, idx_append_value, pAttackLow << 10);
	employeeNpc.m_RangeAttrMgr.Set(nrai_damage_physics, idx_value_hight, idx_append_value, pAttackHigh << 10);
	employeeNpc.m_RangeAttrMgr.Set(nrai_damage_magic, idx_value_low, idx_append_value, mAttackLow << 10);
	employeeNpc.m_RangeAttrMgr.Set(nrai_damage_magic, idx_value_hight, idx_append_value, mAttackHigh << 10);	
	employeeNpc.m_RangeAttrMgr.Set(nrai_defend_physics, idx_value_hight, idx_append_value, pDefend);
	employeeNpc.m_RangeAttrMgr.Set(nrai_defend_eightdiag, idx_value_hight, idx_append_value, mDefend);
	employeeNpc.m_CompAttrMgr.Set(ncai_lifeuplimit, idx_current_value, maxLife);
	employeeNpc.m_CompAttrMgr.Set(ncai_manauplimit, idx_current_value, 100);
	employeeNpc.m_UnaryAttrMgr.Set(nuai_curlife, maxLife);
	employeeNpc.m_UnaryAttrMgr.Set(nuai_curmana, 100);

	//添加技能和BUFF
	InitEmployee(employeeNpcIndex, employeeType, employeeSkillSeries, employType);

	employerPlayer.GetEmployee().Employ(employeeNpcIndex, employType, employTime, employSalary);
	employerPlayer.CheckNameColor();

	SendResultToClient(playerIndex, HRC_HIRE_SUCCESS);

	//战斗雇佣预扣
	if (employType == HT_FIGHT)
	{
		DWORD prepayTime = EmployCenter::Singleton().GetPayInterval();
		DWORD prepayMoney = CalcPayMoney(prepayTime, employSalary);	
		//从雇主扣金钱
		if (prepayMoney < MAX_INT_VALUE)
		{
			if (employerPlayer.Pay((int)prepayMoney))
			{
				//日志
				if (prepayMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
				{
					LogEventParam logParam;
					logParam.event = log_event_employ_employer_pay_money;
					logParam.param1 = employerPlayer.GetGUID();
					snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d prepay", employType);
					logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
					logParam.param4 = -prepayMoney;
					g_pLogSystem->Log(logParam);
				}
			}
			else
			{
				char szLogDesc[256] = { 0 };
				snprintf(szLogDesc, sizeof(szLogDesc), "Player:%s, Employee:%s, Money:%d", employerPlayer.GetPlayerName(), employeeNpc.Name, (int)prepayMoney);
				szLogDesc[sizeof(szLogDesc) - 1] = 0;
				g_pLogSystem->SysDbgLog(szLogDesc, strlen(szLogDesc), sys_dbg_log_event_prepay_employ_money_failure);
			}
		}
	}
	
	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_employ_employ_success;
		logEvent.param1 = employerPlayer.GetGUID();
		strncpy(logEvent.param2.data, employeeName, sizeof(logEvent.param2.data));
		logEvent.param2.data[sizeof(logEvent.param2.data) - 1] = 0;
		g_pLogSystem->Log(logEvent);
	}
}

int EmployCenter::Pay(int playerIndex, int employeeNpcIndex)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& employerPlayer = Player[playerIndex];

	if (!IsValidNpc(employeeNpcIndex))
		return FALSE;

	KNpc& employeeNpc = Npc[employeeNpcIndex];

	if (NULL == g_pController)
		return FALSE;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = employerPlayer.GetNetConnectIdx();
	DBHeader.ProcType = Proc_Pay;
	pParam->BeginPush( PN_PAY );
	pParam->Push( employeeNpc.Name );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_role, pParam );
}

void EmployCenter::PayDbOpComplete(int playerIndex, IProcRet* pRet)
{
	if (!IsValidPlayer(playerIndex))
		return;

	KPlayer& employerPlayer = Player[playerIndex];
	
	if (!IsValidNpc(employerPlayer.GetNpcIndex()))
		return;

	KNpc& employerNpc = Npc[employerPlayer.GetNpcIndex()];

	if (NULL == pRet)
		return;

	if (!(pRet->GetRet() && pRet->GetExeRet()))
	{
		//没有找到该佣兵，则解雇
		employerPlayer.GetEmployee().Fire();
		return;
	}

	int employType = 0;

	int nCol = 0;
	pRet->GetData(0, nCol++, employType);
	
	switch(employType)
	{
	case HT_EXP:
		{
			int leftEmployTime = 0;
			pRet->GetData(0, nCol++, leftEmployTime);
			
			//佣兵没雇用时间了，解雇
			if (leftEmployTime <= 0)
			{
				SendResultToClient(playerIndex, HRC_EMPLOYEE_OUT_OF_EMPLOY_TIME);
				employerPlayer.GetEmployee().Fire();
			}
		}
		break;
	case HT_FIGHT:
		{
			int employTime = 0;
			pRet->GetData(0, nCol++, employTime);
			
			if (employTime > 0)
			{
				//从雇主扣雇用时间
				DWORD employerTime = employerPlayer.GetEmployTime();
				if (employerTime < employTime)
				{
					employerTime = 0;
				}
				else
				{
					employerTime -= employTime;
				}

				//日志
				int payEmployTime = employerPlayer.GetEmployTime() - employerTime;
				if (payEmployTime >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ_time_change_threshold))
				{
					LogEventParam logParam;
					logParam.event = log_event_employ_employer_pay_employ_time;
					logParam.param1 = employerPlayer.GetGUID();
					snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d pay", employType);
					logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
					logParam.param4 = -payEmployTime;
					g_pLogSystem->Log(logParam);
				}

				employerPlayer.SetEmployTime(employerTime);
				
				Employee& employee = employerPlayer.GetEmployee();
				if (employee.IsExist())
				{
					DWORD prepayTime = EmployCenter::Singleton().GetPayInterval();
					DWORD prepayMoney = CalcPayMoney(prepayTime, employee.GetSalary());

					//雇主的钱不够下一次雇用费用了
					if (employerPlayer.GetItemList().GetMoney(room_equipment) < prepayMoney)
					{
						SendResultToClient(playerIndex, HRC_EMPLOYER_OUT_OF_MONEY);
						employee.Fire();
					}
					//雇主没雇用时间了
					else if (employerTime < EmployCenter::Singleton().GetPayInterval())
					{
						SendResultToClient(playerIndex, HRC_EMPLOYER_OUT_OF_EMPLOY_TIME);
						employee.Fire();
					}
					else
					{
						//从雇主扣金钱
						if (prepayMoney < MAX_INT_VALUE)
						{
							if (employerPlayer.Pay((int)prepayMoney))
							{
								//日志
								if (prepayMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
								{
									LogEventParam logParam;
									logParam.event = log_event_employ_employer_pay_money;
									logParam.param1 = employerPlayer.GetGUID();
									snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d pay", employType);
									logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
									logParam.param4 = -prepayMoney;
									g_pLogSystem->Log(logParam);
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}

int EmployCenter::Fire(int playerIndex, int employeeNpcIndex)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& employerPlayer = Player[playerIndex];

	if (!employerPlayer.GetEmployee().IsExist())
	{
		SendResultToClient(playerIndex, HRC_NO_EMPLOYEE);
		return FALSE;
	}

	if (!IsValidNpc(employeeNpcIndex))
		return FALSE;

	KNpc& employeeNpc = Npc[employeeNpcIndex];

	if (NULL == g_pController)
		return FALSE;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (NULL == pParam)
		return FALSE;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = employerPlayer.GetNetConnectIdx();
	DBHeader.ProcType = Proc_Fire;
	pParam->BeginPush( PN_FIRE );
	pParam->Push( employeeNpc.Name );
	pParam->Push( employerPlayer.GetPlayerName() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	g_pController->CallProc( cfs_db_cnn_role, pParam );
	
	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ))
	{
		LogEventParam logEvent;
		logEvent.event = log_event_employ_fire;
		logEvent.param1 = employerPlayer.GetGUID();
		strncpy(logEvent.param2.data, employeeNpc.Name, sizeof(logEvent.param2.data));
		logEvent.param2.data[sizeof(logEvent.param2.data) - 1] = 0;
		g_pLogSystem->Log(logEvent);
	}
	
	BuffMgr &mgr = BuffMgr::Singleton();
	mgr.ClearAllBuff(employeeNpcIndex);
				
	int subWorld = employeeNpc.m_SubWorldIndex;
	int region = employeeNpc.m_RegionIndex;
	if (subWorld >= 0 && subWorld < MAX_SUBWORLD && region >= 0)
	{
		SubWorld[subWorld].m_Region[region].RemoveNpc(employeeNpcIndex);
	}
	
	NpcSet.Remove(employeeNpcIndex);

	//计算未支付费用
	Employee& employee = employerPlayer.GetEmployee();
	if (employee.IsExist())
	{
		switch(employee.GetEmployType())
		{
		case HT_EXP:
			{
			}
			break;
		case HT_FIGHT:
			{
				DWORD employTime = employee.GetUnpayedTime();
				if (employTime > 0)
				{
					//从雇主扣雇用时间
					DWORD employerTime = employerPlayer.GetEmployTime();
					if (employerTime < employTime)
					{
						employerTime = 0;
					}
					else
					{
						employerTime -= employTime;
					}

					//日志
					int payEmployTime = employerPlayer.GetEmployTime() - employerTime;
					if (payEmployTime >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_employ_time_change_threshold))
					{
						LogEventParam logParam;
						logParam.event = log_event_employ_employer_pay_employ_time;
						logParam.param1 = employerPlayer.GetGUID();
						snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d fire", employee.GetEmployType());
						logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
						logParam.param4 = -payEmployTime;
						g_pLogSystem->Log(logParam);
					}

					employerPlayer.SetEmployTime(employerTime);
					
// 					//从雇主扣金钱
// 					DWORD payMoney = CalcPayMoney(employTime, employee.GetSalary());
// 					if (payMoney < MAX_INT_VALUE)
// 					{
// 						employerPlayer.Pay((int)payMoney);
// 
// 						//日志
// 						if (payMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
// 						{
// 							LogEventParam logParam;
// 							logParam.event = log_event_employ_employer_pay_money;
// 							logParam.param1 = employerPlayer.GetGUID();
// 							snprintf(logParam.param3.data, sizeof(logParam.param3.data), "%d fire", employee.GetEmployType());
// 							logParam.param3.data[sizeof(logParam.param3.data) - 1] = 0;
// 							logParam.param4 = -payMoney;
// 							g_pLogSystem->Log(logParam);
// 						}
// 					}
				}
			}
			break;
		}
	}

	return TRUE;
}

void EmployCenter::FireDbOpComplete(int playerIndex, IProcRet* pRet)
{
	if (!IsValidPlayer(playerIndex))
		return;

	//雇主Player
	KPlayer& employerPlayer = Player[playerIndex];
	
	if (!IsValidNpc(employerPlayer.GetNpcIndex()))
		return;

	//雇主NPC
	KNpc& employerNpc = Npc[employerPlayer.GetNpcIndex()];

	if (NULL == pRet)
		return;

	if (!(pRet->GetRet() && pRet->GetExeRet()))
		return;
}

void EmployCenter::InitEmployee(int employeeNpcIndex, int roleType, int skillSeries, int employType)
{
	if (!IsValidNpc(employeeNpcIndex))
		return;

	const EmployeeSetting* pSetting = GetEmployeeSetting(roleType, skillSeries, employType);
	if (pSetting)
	{
		//添加技能
		NpcSkillList& skillList = Npc[employeeNpcIndex].GetSkillList();
		skillList.Clear();
		for (int skillIndex = 0; skillIndex < MAX_EMPLOYEE_SKILL_COUNT; skillIndex++)
		{
			if (pSetting->Skills[skillIndex] > 0)
			{
				skillList.AddSkillEx(pSetting->Skills[skillIndex], 1, skill_status_usable);
			}
		}

		//添加BUFF
		BuffMgr& buffMgr = BuffMgr::Singleton();
		for (int buffIndex = 0; buffIndex < MAX_EMPLOYEE_BUFF_COUNT; buffIndex++)
		{
			if (pSetting->Buffs[buffIndex] > 0)
			{
				buffMgr.AddNpcBuff(employeeNpcIndex, employeeNpcIndex, pSetting->Buffs[buffIndex]);
			}
		}
	}
}

DWORD EmployCenter::CalcRewardExp(int employeeLevel, DWORD beEmployedTime, int employTimeElapseRate) const
{
	const DWORD levelExpRate = GetLevelExp(employeeLevel);
	int selectedExpRate = GetRewardExpRate(employTimeElapseRate);

	return levelExpRate * beEmployedTime / 60 * (100 + selectedExpRate) / 100;
}

int EmployCenter::GetEmployTimeElapseRate(int index) const
{
	const ExpSetting* pSetting = GetExpSetting(index);
	if (pSetting)
		return pSetting->TimeElapseRate;
	else
		return 1;
}

int EmployCenter::GetRewardExpRate(int employTimeElapseRate) const
{
	for (int index = 0; index < MAX_EXP_SETTING_COUNT; index++)
	{
		if (m_ExpSettings[index].TimeElapseRate == employTimeElapseRate)
			return m_ExpSettings[index].ExpRewardPlus;
	}

	return 0;
}

bool EmployCenter::IsValidEmployType(int type) const
{
	return (type == HT_EXP || type == HT_FIGHT);
}

bool EmployCenter::IsValidRateIndex(int index) const
{
	return (index == 0 || index == 1 || index == 2);
}

bool EmployCenter::IsValidSalary(int secondSalary) const
{
	return (secondSalary >= 0 && secondSalary <= MAX_SECOND_SALARY);
}

void EmployCenter::SendResultToClient(int playerIndex, int returnCode)
{
	if (!IsValidPlayer(playerIndex))
		return;

	HIRE_RET_CODE retMsg;
	retMsg.Protocol = s2c_hire_ret_code;
	retMsg.ret = returnCode;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), &retMsg, sizeof(retMsg));
}
