//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.h
// Date:	2003.07.28
// Code:	边城浪子
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#ifndef _KVIEWITEM_H
#define _KVIEWITEM_H

#ifndef _SERVER

#include "Yao_Monitor.h"
#include "ArmorSet_Monitor.h"

class KViewItem
{
public:
	KViewItem();

	ArmorSetMonitor& GetArmorSetMonitor();
	YaoMonitor& GetYaoMonitor();
	
	void Init();
	void ApplyViewEquip(DWORD dwNpcID);
	void DeleteAll();
	void GetData(BYTE* pMsg);	
	char* getName();
	char* getChenghao();
	char* getShizu();
	char* getZhuhou();
	char* getLianmen();
	int	  getId();
	int	  getPkvalue();

	PlayerItem m_sItem[itempart_num];// 对方玩家穿在身上的装备在客户端 Item 数组中的位置信息

private:
	DWORD m_npcId;							//对方玩家的 npc 的 id
	int m_level;								//对方玩家的等级
	int m_pkValue;

	char	m_name[MAXSIZE_ROLENAME];			//姓名
	char	m_shizu[MAXSIZE_ORGNAME];			//氏族
	char	m_zhuhou[MAXSIZE_ORGNAME];			//诸侯
	char    m_lianmen[MAXSIZE_ORGNAME];			//联盟
	char	m_chenghao[MAXSIZE_ROLENAME];		//称号

	KEquipState m_EquipItem[itempart_num];
	ArmorSetMonitor m_ArmorSetMonitor;
	YaoMonitor m_YaoMonitor;
};

inline ArmorSetMonitor& KViewItem::GetArmorSetMonitor()
{
	return m_ArmorSetMonitor;
}

inline YaoMonitor& KViewItem::GetYaoMonitor()
{
	return m_YaoMonitor;
}

inline char* KViewItem::getName()
{
	return m_name;
}

inline char* KViewItem::getShizu()
{
	return m_shizu;
}

inline char* KViewItem::getZhuhou()
{
	return m_zhuhou;
}

inline char* KViewItem::getLianmen()
{
	return m_lianmen;
}

inline char* KViewItem::getChenghao()
{
	return m_chenghao;
}

inline int KViewItem::getId()
{
	return m_npcId;
}

inline int KViewItem::getPkvalue()
{
	return m_pkValue;
}
extern KViewItem g_cViewItem;

#endif// _SERVER

#endif// _KVIEWITEM_H
