//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-11
//      File_base        : relation_template
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 关系模版
//
//////////////////////////////////////////////////////////////////////

#ifndef _RELATION_TEMPLATE_H_
#define _RELATION_TEMPLATE_H_

#include "ItemCommonDef.h"
#include <vector>

#define MAX_NAME_LENGTH 128
#define MAX_DES_LENGTH 512
#define MAX_CONTROLLER_COUNT 5
#define CHANGEOWNERBUFF_NUM 10

#ifndef _SERVER
//关系操作控件
struct tagRelationOperationController
{
	tagRelationOperationController()
	{
		memset(Controller, 0, sizeof(Controller));
		memset(Event, 0, sizeof(Event));
		memset(Name, 0, sizeof(Name));
		memset(Desc, 0, sizeof(Desc));
		memset(ReturnController, 0, sizeof(ReturnController));
		NeedConfirm = 0;
	}

	char Controller[MAX_NAME_LENGTH];//控件
	char Event[MAX_NAME_LENGTH];//触发事件
	char Name[MAX_NAME_LENGTH];//名称
	char Desc[MAX_DES_LENGTH];//描述	
	char ReturnController[MAX_NAME_LENGTH];//返回数据的控件
	int NeedConfirm;//是否需要确认
};
#endif

//关系操作
typedef struct tagRelationOperation
{
	tagRelationOperation()
	{
		Id = 0;
		IsDefault = 0;
		RequireMoney = 0;
		RequireLevel = 0;
		memset(&RequireItem, 0, sizeof(RequireItem));		
	}

	int Id;//操作编号
	int IsDefault;//是否是让layer-1 的Owner拥有的操作
	int IsToAll;  //是否是Toplayer == layer 的所有人都拥有的操作
	int RequireMoney;//需求金钱
	int RequireLevel;//需求等级
	int ReqTargetLevel; // 需求目标等级
	EquipmentID RequireItem;//需求物品
#ifndef _SERVER
	tagRelationOperationController Controllers[MAX_CONTROLLER_COUNT];		
#endif
} RelationOperation, *PRelationOperation;

//关系属性
typedef struct tagRelationAttribute
{
	tagRelationAttribute()
	{
		Id = 0;
		SaveFlag = 0;
	}

	int Id;
	int SaveFlag;
} RelationAttribute, *PRelationAttribute;

//关系层次
typedef struct tagRelationLayer
{
	tagRelationLayer()
	{
		MinChildCount = 0;
		MaxChildCount = 0;
		memset(Name, 0, sizeof(Name));
#ifndef _SERVER
		memset(Desc, 0, sizeof(Desc));
		memset(Controller, 0, sizeof(Controller));
#endif
		OperationCount = 0;
		AttributeCount = 0;
		JoinBuff = 0;
		LeaveBuff = 0;
		IsCreateChatChannel = 0;
	}

	int MinChildCount;//最小子节点数量
	int MaxChildCount;//最大子节点数量	
	char Name[MAX_NAME_LENGTH];//层次名称
#ifndef _SERVER
	char Desc[MAX_DES_LENGTH];//层次描述
	char Controller[MAX_NAME_LENGTH];//控件
#endif
	int OperationCount;//操作数量
#ifdef _SERVER
	RelationOperation Operations[enSUO_Num];//层次操作
#else
	RelationOperation Operations[enSUO_Num * 2];//层次操作
#endif
	int AttributeCount;//属性
	RelationAttribute Attributes[enSUAttr_Num];//属性
	int JoinBuff;//加入时的BUFF
	int LeaveBuff;//脱离时的BUFF
	int	BuffMustNotHaveOnJoin; // 加入时不能拥有的buff
	int IsCreateChatChannel;//是否创建聊天频道
	int	CityTaxRateBuff;	// 设置城市税率的时候需要检测的buff
	int ChangeOwnerBuff[CHANGEOWNERBUFF_NUM];
} RelationLayer, *PRelationLayer;	

//关系模版
class RelationTemplate
{
public:
	RelationTemplate();
	~RelationTemplate();

	void Clear();//清空
	bool Load(const char* templateFileName);//载入模版配置

	int GetId() const;//得到编号
	const char* GetName();//得到名称
	const char* GetDesc();//得到描述
	int GetLayerCount() const;//得到层数
	const PRelationLayer GetLayer(int layer);//得到关系层次（layer: 1 - LayerCount）

	bool IsTmplSave() const
	{
		return m_IsSave > 0;
	}
	
private:
	bool m_Loaded;//是否已经载入配置
	char m_Name[MAX_NAME_LENGTH];//模版名称
	char m_Desc[MAX_NAME_LENGTH];//模版描述
	int m_Id;//模版编号
	int m_LayerCount;//层数
	int	m_IsSave; // 是否保存
	RelationLayer m_Layers[MAX_RELATION_LAYER_COUNT];//关系层次
};

typedef std::vector<RelationTemplate> RelationTemplateArray;

//关系模版管理器
typedef class RelationTemplateManager
{
public:
	~RelationTemplateManager();
	static RelationTemplateManager& Singleton();//单件

	bool Load(const char* templateCfgFileName);//载入配置
	int GetTemplateCount() const;//得到模版数量
	RelationTemplate* GetTemplate(int templateId);//得到模版

private:
	RelationTemplateManager();

	bool m_Loaded;//是否已经载入配置
	RelationTemplateArray m_Templates;//关系模版
} RTM;

inline const char* RelationTemplate::GetName()
{
	return m_Name;
}

inline const char* RelationTemplate::GetDesc()
{
	return m_Desc;
}

inline int RelationTemplate::GetId() const
{
	return m_Id;
}

inline int RelationTemplate::GetLayerCount() const
{
	return m_LayerCount;
}

inline int RelationTemplateManager::GetTemplateCount() const
{
	return m_Templates.size();
}

#endif// _RELATION_TEMPLATE_H_