//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-9
//      File_base        : Yao_AddOnTable
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 爻装附加属性表
//
//////////////////////////////////////////////////////////////////////

#ifndef _YAO_ADD_ON_TABLE_H_
#define _YAO_ADD_ON_TABLE_H_

#include <vector>
#include <map>

//爻装附加属性结构体
typedef struct tagYaoAddOn
{
	int Probability;//概率
	int BuffID;//附加效果编号
	short GroupID;//分组
} YaoAddOn;

typedef std::vector<YaoAddOn> YaoAddOnArray;

typedef std::map<short,YaoAddOnArray> YaoAddOnMap;

class YaoAddOnTable
{
public:

	YaoAddOnTable();
	~YaoAddOnTable();

	static YaoAddOnTable& Singleton();//单件

	bool Load();//载入配置表

	const YaoAddOn* GetYaoAddOn(int group,int index);//得到爻装附加属性信息

	inline int GetYaoAddOnCount(int group)//得到爻装附加属性信息记录数量
	{
		YaoAddOnMap::iterator it = m_YaoAddOns.find( group );
		if ( it != m_YaoAddOns.end() )
		{
			return it->second.size();
		}
		return 0;
	};

private:

	bool m_bLoaded;//是否已经载入配置表
	
	YaoAddOnMap m_YaoAddOns;//用于存储爻装附加属性信息
};

#endif//_YAO_ADD_ON_TABLE_H_