//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-22
//      File_base        : talisman_manager
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 法宝管理器
//
//////////////////////////////////////////////////////////////////////

#ifndef _TALISMAN_MANAGER_H_
#define _TALISMAN_MANAGER_H_

enum TalismanOperationResult
{
	talisman_result_failure = 0,			//未知原因的失败
	talisman_result_success = 1,			//成功
	talisman_result_invalid_player,			//不是合法的玩家
	talisman_result_invalid_talisman,		//不是合法的法宝
	talisman_result_invalid_enchase,		//不是合法的内丹
	talisman_result_can_not_upgrade,		//无法升级
	talisman_result_can_not_edit,			//无法编辑
	talisman_result_max_level,				//已经是最高等级
	talisman_result_already_enchased,		//已经有了镶嵌
	talisman_result_no_enchase_pos,			//没有镶嵌位
	talisman_result_level_not_match,		//等级不符合
	talisman_result_max_enchase_count,		//到达该内丹的最大镶嵌数量
	talisman_result_not_enough_skill_exp,	//没有足够的技能经验
	talisman_result_invalid_convert_rate,	//非法的转化率
};

#define MAX_ENCHASE_BUFF_COUNT 3//单个镶嵌携带的BUFF数量
#define MAX_ENCHASE_COUNT_PER_TALISMAN 5//单个法宝可镶嵌同种内丹的数量

//镶嵌计数
struct EnchaseCount
{
	EnchaseCount()
	{
		EnchaseId = 0;
		Count = 0;
	}

	int EnchaseId;
	int Count;
};

//镶嵌组计数
struct EnchaseGroupCount
{
	EnchaseGroupCount()
	{
		EnchaseGroupId = 0;
		Count = 0;
	}

	int EnchaseGroupId;
	int Count;
};

//镶嵌计数信息
struct EnchaseCountInfo
{
	EnchaseCount enchaseCount[TM_HOLE_NUM / TALISMAN_ENCHASE_PER_LEVEL][TM_HOLE_NUM];
	EnchaseGroupCount enchaseGroupCount[TM_HOLE_NUM / TALISMAN_ENCHASE_PER_LEVEL][TM_HOLE_NUM];
};

//法宝镶嵌激活信息
struct EnchaseActiveInfo
{
	EnchaseActiveInfo()
	{
		memset(IsActive, 0, sizeof(IsActive));
	}

	bool IsActive[TM_HOLE_NUM];
	EnchaseCount CountInfo[TM_HOLE_NUM];
};

//法宝镶嵌数据
typedef struct tagEnchaseData
{
	tagEnchaseData()
	{
		Id = 0;
		Group = 0;
#ifndef _SERVER
		memset(Name, 0, sizeof(Name));
		memset(Desc, 0, sizeof(Desc));
		memset(ImageSetName, 0, sizeof(ImageSetName));
		memset(ImageName, 0, sizeof(ImageName));
		memset(MiniImageSetName, 0, sizeof(MiniImageSetName));
		memset(MiniImageName, 0, sizeof(MiniImageName));
#endif
		RequireGroupId = 0;
		RequireGroupCount = 0;
		RequireId = 0;
		RequireCount = 0;
		memset(BuffSet, 0, sizeof(BuffSet));
		MaxEnchaseCount = 0;
	}

	int Id;
	int Group;
#ifndef _SERVER
	char Name[COMMON_NAME_LENGTH];
	char Desc[COMMON_DESC_LENGTH];
	char ImageSetName[COMMON_NAME_LENGTH];
	char ImageName[COMMON_NAME_LENGTH];
	char MiniImageSetName[COMMON_NAME_LENGTH];
	char MiniImageName[COMMON_NAME_LENGTH];
#endif
	int RequireGroupId;
	int RequireGroupCount;
	int RequireId;
	int RequireCount;
	int BuffSet[MAX_ENCHASE_COUNT_PER_TALISMAN][MAX_ENCHASE_BUFF_COUNT];
	int MaxEnchaseCount;
} EnchaseData, *PEnchaseData;

typedef std::vector<EnchaseData> EnchaseDataArray;

//法宝管理器
class TalismanManager
{
public:
	TalismanManager();
	~TalismanManager();
	static TalismanManager& Singleton();
	bool Load();//载入配置
	
	bool IsValidEnchasePos(int enchasePos);//判断是否是合法的镶嵌位置
	bool IsValidTalisman(int itemIndex);//判断是否是合法的法宝
	bool IsValidTalisman(KItem& item);//判断是否是合法的法宝
	bool IsValidEnchaseItem(int itemIndex);//判断是否是合法的内丹
	bool IsValidEnchaseItem(KItem& item);//判断是否是合法的内丹
	bool IsUpgradeable(int itemIndex);//判断是否可以升级
	bool IsEditable(int itemIndex, int playerIndex);//判断是否可以编辑
	bool IsReachTopLevel(KItem& item);//是否达到了最高等级

	void CountTalismanEnchase(int talismanIndex, EnchaseCountInfo& enchaseCountInfo);//计算各层的镶嵌和镶嵌组的数量
	void CountTalismanEnchase(KItem& item, EnchaseCountInfo& enchaseCountInfo);//计算各层的镶嵌和镶嵌组的数量

	void GetEnchaseActiveInfo(int talismanIndex, EnchaseActiveInfo& enchaseActiveInfo);//得到镶嵌激活信息
	void GetEnchaseActiveInfo(KItem& item, EnchaseActiveInfo& enchaseActiveInfo);//得到镶嵌激活信息

	const PEnchaseData GetEnchaseData(int enchaseId);//得到镶嵌数据

#ifdef _SERVER
	void ServerProcessProtocol(int playerIndex, BYTE *pMsg, int nSize);//处理法宝相关协议
	TalismanOperationResult Upgrade(int playerIndex, int itemIndex, int& upgradedItemIndex);//升级法宝
	TalismanOperationResult Enchase(int playerIndex, int itemIndex, int enchasePos, int enchaseItemIndex);//镶嵌法宝
	TalismanOperationResult ClearAllEnchase(int playerIndex, int itemIndex);//清除所有镶嵌
	TalismanOperationResult ConvertSkillExpToTalismanPotential(int playerIndex, int itemIndex, int avaiableSkillExpToConvert);//修炼法宝（把人物蕴魂转化为法宝蕴魂）
#else
	void ClientProcessProtocol(BYTE *pMsg);//处理法宝相关协议

	void GetEnchaseDesc(int itemIndex, int enchasePos, char* descBuff);//得到镶嵌描述
	void GetEnchaseDesc(KItem& item, int enchasePos, char* descBuff);//得到镶嵌描述

	void GetTalismanDesc(int talismanIndex, char* descBuff);//得到法宝描述
	void GetTalismanDesc(KItem& item, char* descBuff);//得到法宝描述

	void GetEnchaseItemDesc(int enchaseItemIndex, char* descBuff);//得到内丹描述
	void GetEnchaseItemDesc(KItem& item, char* descBuff);//得到内丹描述

	void GetTalismanEffectDesc(int talismanIndex, char* descBuff, size_t size);//得到法宝效果（添加的技能）的描述
#endif
	
private:
	EnchaseDataArray m_EnchaseDatas;
};

#endif// _TALISMAN_MANAGER_H_