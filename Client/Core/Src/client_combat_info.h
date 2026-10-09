//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-9
//      File_base        : client_combat_info
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 战斗信息（客户端）
//
//////////////////////////////////////////////////////////////////////

#ifndef _CLIENT_COMBAT_INFO_H_
#define _CLIENT_COMBAT_INFO_H_

#define MAX_COMBAT_INFO 20

//战斗信息
typedef struct tagCombatInfo
{
	int Damage;
	int Spell;
	COMBAT_INFO_TYPE DamageType;
	bool IsCrit;

	int Frame;
	int PosX;
	int PosY;
	int Alpha;
} COMBAT_INFO, *PCOMBAT_INFO;

//战斗信息显示器（客户端）
class ClientCombatInfoShower
{
public:
	ClientCombatInfoShower();
	void Init(int npcIndex);

	void TurnOn();
	void TurnOff();
	void AddInfo(int caster, int damage, int spell, COMBAT_INFO_TYPE damageType, bool isCrit);
	void Draw(int baseHeight);

private:
	void ShowInfoInMessageBox(int caster, int damage, int spell, COMBAT_INFO_TYPE damageType, bool isCrit);
	void ShowInfoAboveNpc(int caster, int damage, int spell, COMBAT_INFO_TYPE damageType, bool isCrit);

	int m_NpcIndex;
	COMBAT_INFO m_CombatInfoList[MAX_COMBAT_INFO];
	int m_PosHead;
	int m_PosTail;
	int m_Count;
	bool m_IsOn;
	bool m_IsSelf;
};

//初始化战斗信息资源
void InitCombatInfoResources();

#endif// _CLIENT_COMBAT_INFO_H_