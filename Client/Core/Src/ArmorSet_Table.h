//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10-30
//      File_base        : ArmorSet_Table
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 套装配置表读取
//
//////////////////////////////////////////////////////////////////////

#ifndef _ARMORSET_TABLE_H_
#define	_ARMORSET_TABLE_H_

#include <vector>
#include "KItem.h"

//最大套装名称长度
#define MAX_ARMOR_SET_NAME_LENGTH 32
//装备ID分隔符
#define ITEM_ID_DELIMITER "."
//可选装备分隔符
#define ALTERNATIVE_ITEM_DELIMITER ";"
//最大装备ID字符串的长度
#define MAX_ITEM_ID_STR_LENGTH (MAX_EQUIPMENT_ID_ARRAY_LENGTH * 7)
//可选部件数量
#define ALTERNATIVE_PART_COUNT 6

//套装数据结构体
typedef struct tagArmorSet
{
	int ArmorSetID;//套装编号
	char ArmorSetName[MAX_ARMOR_SET_NAME_LENGTH];//套装名称
	int PartCount;//套装部件的数量
	int AlternativePartCount[itempart_num];//部件可选数量
	EquipmentID PartIDs[itempart_num][ALTERNATIVE_PART_COUNT];//套装部件的编号
	int EffectIDs[itempart_num];//套装效果的编号
} ArmorSet;

typedef std::vector<ArmorSet> ArmorSetArray;


//////////////////////////////////////////////////////////////////////
//名称：ArmorSetTable（套装表）
//描述：用于读取套装配置表，并提供易用的接口访问表中数据
//使用：首先使用Load函数载入表中的数据；如果载入成功，可以使用
//      GetArmorSet访问爻属性表中的数据。
//
//其他：套装配置表的结构如下：
//      套装编号 套装名称 部件1 部件2 部件3 部件4 部件5 部件6 部件7
//      部件8 部件9 1件效果 2件效果 3件效果 4件效果 5件效果 6件效果
//      7件效果 8件效果 9件效果
//		其中，部件1~9是装备编号字符串（例如："1,7,3,21"）
//////////////////////////////////////////////////////////////////////
class ArmorSetTable
{
public:

	ArmorSetTable();
	~ArmorSetTable();

public:

	static ArmorSetTable& Singleton();//单件

	bool Load();//载入套装配置表

	//////////////////////////////////////////////////////////////////////
	//获取套装数据
	//nArmorSetID: 套装编号
	//返回值: 套装数据
	//////////////////////////////////////////////////////////////////////
	const ArmorSet* GetArmorSet(int nArmorSetID);

	//////////////////////////////////////////////////////////////////////
	//获取所有的套装编号
	//IDArray: 编号数组
	//max: 返回的最大套装数量
	//返回值: 实际返回的套装数量
	//////////////////////////////////////////////////////////////////////
	int GetAllArmorSetID(int* IDArray, int max);

	inline bool IsLoaded()//是否已经载入配置表
	{
		return m_bLoaded;
	};

	inline int GetArmorSetCount()//获取套装数量
	{
		return m_ArmorSets.size();
	};

private:

	bool m_bLoaded;//是否已经载入配置表

	ArmorSetArray m_ArmorSets;//用于存储套装数据的vector

};

#endif//_ARMORSET_TABLE_H_