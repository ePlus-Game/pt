//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10-31
//      File_base        : Yao_Table
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 爻配置表和爻组合配置表的读取
//
//////////////////////////////////////////////////////////////////////

#ifndef _YAO_TABLE_H_
#define	_YAO_TABLE_H_

#include <vector>

//最大卦描述的长度
#define MAX_GUA_DESC_LENGTH 128
//最大卦类型的数量
#define MAX_GUA_TYPE_COUNT 8
//最大卦名称的长度
#define MAX_GUA_NAME_LENGTH 128
//最大爻装组合名称的长度
#define MAX_YAO_SET_NAME_LENGTH 128
//最大爻装组合说明的长度
#define MAX_YAO_SET_COMMENT_LENGTH 128
//最大爻装组合描述的长度
#define MAX_YAO_SET_DESC_LENGTH 128
//最大卦位置的数量
#define MAX_GUA_POS_COUNT 8
//最大卦位图标文件名称长度
#define MAX_GUA_IMG_FILE_NAME_LENGTH 128

//套装数据结构体
typedef struct tagYaoData
{
	int GuaID;//卦的编号
	int Level;//等级
	char Name[MAX_GUA_NAME_LENGTH];//卦的名称
	char Desc[MAX_GUA_DESC_LENGTH];//卦的描述
	int StandaloneEffectID;//孤立卦的效果编号
	int EffectIDs[MAX_GUA_TYPE_COUNT];//多个同种卦位的组合效果编号
	char Image[2][MAX_GUA_IMG_FILE_NAME_LENGTH];//卦位图标文件
} YaoData;

//套装数据结构体
typedef struct tagYaoSet
{
	int YaoSetID;//爻装组合的编号
	int GuaIDs[MAX_GUA_POS_COUNT];
	int Level;//等级
	char Name[MAX_YAO_SET_NAME_LENGTH];//爻装组合的名称
	char Comment[MAX_YAO_SET_COMMENT_LENGTH];//爻装组合的说明
	char Desc[MAX_YAO_SET_DESC_LENGTH];//爻装组合的描述
	int EffectID;//效果编号
} YaoSet;

//参数：爻装组合，包含各个位置的卦，以及等级
typedef struct tagParamYaoSet
{
	int GuaIDs[MAX_GUA_POS_COUNT];
	int Level;
} ParamYaoSet;

typedef std::vector<YaoData> YaoDataArray;
typedef std::vector<YaoSet> YaoSetArray;

//////////////////////////////////////////////////////////////////////
//名称：YaoTable（爻表）
//描述：用于读取爻属性和爻组合配置表，并提供易用的接口访问这些表中数据
//使用：首先使用Load函数载入表中的数据；如果载入成功，可以使用GetYao访
//      问爻属性表中的数据，使用GetYaoSet访问爻组合表中的数据。
//
//其他：爻属性配置表的结构如下：
//      卦 等级 描述 单卦效果 1卦效果 2卦效果 3卦效果 4卦效果 5卦效果
//      6卦效果 7卦效果 8卦效果
//      爻属性配置表的结构如下：
//      组合编号 卦1 卦2 卦3 卦4 卦5 卦6 卦7 卦8 等级 名称 描述 效果
//////////////////////////////////////////////////////////////////////
class YaoTable
{
public:

	YaoTable();
	~YaoTable();

public:

	static YaoTable& Singleton();//单件

	bool Load();//载入配置表

	//////////////////////////////////////////////////////////////////////
	//获取爻数据
	//nGuaID: 卦编号
	//nLevel: 等级
	//返回值: 爻数据
	//////////////////////////////////////////////////////////////////////
	const YaoData* GetYao(int nGuaID, int nLevel);

	//////////////////////////////////////////////////////////////////////
	//获取爻装组合数据
	//paramYaoSet: 爻装组合参数
	//返回值: 爻装组合数据
	//////////////////////////////////////////////////////////////////////
	const YaoSet* GetYaoSet(ParamYaoSet* paramYaoSet);

	//////////////////////////////////////////////////////////////////////
	//获取爻装组合数据
	//yaoSetID: 爻装组合编号
	//返回值: 爻装组合数据
	//////////////////////////////////////////////////////////////////////
	const YaoSet* GetYaoSet(int yaoSetID);

	inline bool IsLoaded()//是否已经载入配置表
	{
		return m_bLoaded;
	};

	inline int GetYaoCount()//获取爻装数量
	{
		return m_yaoDatas.size();
	};

	inline int GetYaoSetCount()//获取爻装组合的数量
	{
		return m_yaoSets.size();
	};

private:

	bool LoadYao();//载入爻配置表

	bool LoadYaoSet();//载入爻组合配置表

private:

	bool m_bLoaded;//是否已经载入配置表

	YaoDataArray m_yaoDatas;//用于存储爻装数据的vector	

	YaoSetArray m_yaoSets;//用于存储爻装数据的vector
};

#endif//_YAO_TABLE_H_