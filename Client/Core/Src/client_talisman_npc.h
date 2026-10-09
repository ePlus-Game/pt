//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-3-7
//      File_base        : client_talisman_npc
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 法宝NPC（客户端）
//
//////////////////////////////////////////////////////////////////////

#ifndef _CLIENT_TALISMAN_NPC_CONTROLLER_
#define _CLIENT_TALISMAN_NPC_CONTROLLER_

#define MAX_CLIENT_TALISMAN_NPC 100

struct ClientTalismanNpc
{
	int Id;//法宝NPC编号
	int NpcId_Normal;//普通状态NPC
	int NpcId_UseSkill;//使用法宝技能变身NPC
	char Spr_UseSkill[64];//使用法宝技能SPR
	int UseSkillTime;//使用法宝技能时间
	int FollowDistance;//跟随距离
	int FollowSpeed;//跟随速度
};

//法宝NPC表
class ClientTalismanNpcTable
{
public:
	ClientTalismanNpcTable();
	static ClientTalismanNpcTable& Singleton();
	bool Load();
	ClientTalismanNpc* GetClientTalismanNpc(int id);

private:
	ClientTalismanNpc m_TalismanNpc[MAX_CLIENT_TALISMAN_NPC];
};

//法宝NPC控制器（客户端）
class ClientTalismanNpcController
{
public:
	ClientTalismanNpcController();
	
	void Init(int npcIndex);
	void Active();
	int GetTalismanNpc();
	void SetTalismanNpc(int talismanNpcIndex, ClientTalismanNpc* pTemplate);
	void UseTalismanSkill(int skillId, int skillLevel);

private:
	int m_NpcIndex;//法宝主人NPC序号
	int m_TalismanNpcIndex;//法宝NPC序号
	unsigned long m_PolymorphTime;//变身结束时间
	ClientTalismanNpc* m_pTemplate;//模板
};

inline int ClientTalismanNpcController::GetTalismanNpc()
{
	return m_TalismanNpcIndex;
}

inline void ClientTalismanNpcController::SetTalismanNpc(int talismanNpcIndex, ClientTalismanNpc* pTemplate)
{
	m_TalismanNpcIndex = talismanNpcIndex;
	m_pTemplate = pTemplate;
}

#endif// _CLIENT_TALISMAN_NPC_CONTROLLER_