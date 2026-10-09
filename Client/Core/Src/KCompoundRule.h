//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/31/2006 19:37
//      File_base        : KCompoundRule
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef COMPOUND_RULE
#define COMPOUND_RULE

#include <vector>
#include <map>
#include "KItem.h"

using namespace std;

/*!
\brief
	最大被合成的道具数量
*/
#define MAX_COMPOUND 10

class TCompoundParams;
class TCompoundResult;
class TCompoundDelResult;
struct TCompoundResultEx;

/*!
\brief
	装备合成规则的集合
*/
class KCompoundRules
{
public:
	/*!
	\brief
		源道具描述
	*/
	struct KSrcItemDescriptor
	{
		KSrcItemDescriptor()
		{
			nItemGenre			= 0;
			nItemDetail			= 0;
			nItemParticular		= 0;
			nItemLevel			= 0;
			nItemCount			= 0;
			nItemQuality		= 0;
			nItemYao			= 0;
			nItemColor			= 0;
		}
		KSrcItemDescriptor( const KSrcItemDescriptor& rSrcItemDesc )
		{
			nItemGenre			= rSrcItemDesc.nItemGenre;
			nItemDetail			= rSrcItemDesc.nItemDetail;
			nItemParticular		= rSrcItemDesc.nItemParticular;
			nItemLevel			= rSrcItemDesc.nItemLevel;
			nItemCount			= rSrcItemDesc.nItemCount;
			nItemQuality		= rSrcItemDesc.nItemQuality;
			nItemYao			= rSrcItemDesc.nItemYao;
			nItemColor			= rSrcItemDesc.nItemColor;				
		}
		int						nItemGenre;						//!< 道具种类
		int						nItemDetail;					//!< 道具具体种类
		int						nItemParticular;				//!< 道具详细种类
		int						nItemLevel;						//!< 道具等级
		int						nItemCount;						//!< 道具个数
		int						nItemQuality;					//!< 道具品质
		int						nItemYao;						//!< 道具爻
		int						nItemColor;
	};
	
	typedef map<int,KSrcItemDescriptor> _CompoundRuleParamList;

	/*!
	\brief
		装备合成规则
	*/
	struct KCompoundRuleEntry
	{
		int						nRuleType;						//!< 规则类型
		int						nRuleTypePlus;					//!< 扩充类型
		KSrcItemDescriptor		TargetItem;						//!< 可升级的部件
		int						nChangeCount;					//!< 变化的次数
		int						nMoney;							//!< 需求金钱
		int						nSkillPoint;					//!< 技能点
		int						nSrcItemsCount;					//!< 变化源道具的类型数组有效项目的数量
		KSrcItemDescriptor		SrcItemsArray[MAX_COMPOUND];	//!< 合成源道具的类型数组
		int						nBuff;							//!< 合成后附加Buff	
		int						CompoundSuccessRate0;			//!< 合成成功几率1
		KSrcItemDescriptor		DstItem0;						//!< 合成目标的道具类型1
		int						CompoundSuccessRate1;			//!< 合成成功几率2
		KSrcItemDescriptor		DstItem1;						//!< 合成目标的道具类型2
		int						CompoundSuccessRate2;			//!< 合成成功几率3
		KSrcItemDescriptor		DstItem2;						//!< 合成目标的道具类型3
		int						CompoundSuccessRate3;			//!< 合成成功几率4
		KSrcItemDescriptor		DstItem3;						//!< 合成目标的道具类型4
		int						CompoundSuccessRate4;			//!< 合成成功几率5
		KSrcItemDescriptor		DstItem4;						//!< 合成目标的道具类型5
		bool					bPlusRate;						//!< 附加成功率是否生效
		int						nDestroyRate;					//!< 损坏率
		int						MapSuccessRate[1];				//!< 不同地图的成功率加成
		int						nPlusRate0;
		int						nPlusBuffID0;
		int						nPlusLevel0;
		BOOL					bBind;
	};

	/*!
	\brief
		源道具描述
	*/
	struct KRateParam
	{
		int						nCount;							//!< 边界值
		int						nRate;							//!< 成功率
	};

	/*
	\brief
		合成规则索引结构
	*/
	struct KCompoundRulesIndex 
	{
		KCompoundRulesIndex()
		{
			nType			= 0;
			nChangeCount	= 0;
		}
		KCompoundRulesIndex( const KCompoundRulesIndex& rIndex )
		{
			nType			= rIndex.nType;
			nChangeCount	= rIndex.nChangeCount;
		}
		int nType;
		int nChangeCount;
	};

	/*
	\brief
		装备合成成功率规则
	*/
	struct KCompoundSuccessRatio
	{
		KSrcItemDescriptor		RateItem;						//!< 道具	
		KRateParam				nCount0;						//!< 个数
		KRateParam				nCount1;						//!< 个数
		KRateParam				nCount2;						//!< 个数
		KRateParam				nQuality0;						//!< 品质
		KRateParam				nQuality1;						//!< 品质
		KRateParam				nQuality2;						//!< 品质
		KRateParam				nQuality3;						//!< 品质
	};

	typedef	vector<KCompoundRuleEntry>  KRuleTable;

	struct	KRuleChangeCountIdxTab
	{
		map<int,KRuleTable> tabChangeCount;
	};

	struct  KCompoundRuleTable
	{
		map<int,KRuleChangeCountIdxTab> tabRule;
	};

public:
	KCompoundRules()	{};
	~KCompoundRules()	{};
public:
	bool								Init				( void																								);
	bool								AddRule				( const KCompoundRulesIndex& Idx, const KCompoundRuleEntry& Entry									);
	bool								AddSuccessRatio		( const KCompoundSuccessRatio& Entry																);
	int									CheckRules			( TCompoundParams* pParams,	TCompoundResult* outResult	,TCompoundDelResult * delResult 			);
	int									CheckRules			( TCompoundParams* pParams,	TCompoundResultEx* outResult, int ruleId								);
	int									checkParams			( TCompoundParams* pParams																			);
	bool								CheckItem			( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem											);
	KCompoundRuleEntry*					getRuleById			(int ruleType, int changeTimes, int ruleId															);
	KCompoundRuleEntry*					getRuleByLiftItem	(int ruleType, int changeTimes, ItemType& liftItem);
private:
	void								InitRules			( void																								);
	void								InitSuccessRatio	( void																								);
	KRuleTable&							GetRuleByIndex		( const KCompoundRulesIndex& Idx																	);
	int									GetPlusSuccessRate	( TCompoundParams* pParams	,TCompoundDelResult * delResult                      					);
	void								AddToRealParamList	( int nIdx, int nItemGenre, int nItemDetail, int nItemParticular, int nItemCount, int nItemQuality	);
	void								ClearRealParamList	( void																								);
	bool								CheckCount			( const KRuleTable& ruleList,int nHitIdx															);
	bool								CheckQuality		( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem											);
	bool								CheckYao			( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem											);
	bool								CheckColor			( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem											);
	bool								isItemDesValid		( KSrcItemDescriptor des);

#ifndef _SERVER

public:
	int		GetCompoundRuleInfo	(const CompoundInitMaterial& initMaterial, KCompoundRuleEntry& rRule);
	bool	checkMaterial		(const vector<int>& sourIndex, const KCompoundRuleEntry& rule);

#endif

private:
	KCompoundRuleTable					m_CompoundRuleTable;
	vector<KCompoundSuccessRatio>		m_CompoundSuccessRatioTable;
	_CompoundRuleParamList				m_CompRuleParamList;
	int									m_CRPLIndex[MAX_COMPOUND];
};

/*!
\brief
	全局的合成规则对象
*/
extern KCompoundRules g_CompoundRule;

extern void _getItemByString(int &nGenre, int &nDetail, int &nParticular, int &nCount, int &nLevel, int &nQuality, int &nYao, int &nColor, char* sValue, int nStringSize);

#endif