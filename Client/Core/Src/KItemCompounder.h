//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/02/2006 16:42
//      File_base        : KItemCompounder
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//		物品的合成，其行为表现是将若干个物品分在一起，从而产生另一个物品.
//	从它的具体意义上，目前至少有两种
//	一类是武器的升级，即将几个宝石加上源武器，产生比源武器更高级的武器。
//	另一类合成，即将几种资源合成，产生一个武器。
//	不过其形式都是一样的，所以我们将可以将它们抽象出来
//		合成所用到的协议c2s_enchaseritem,s2c_enchaseritemresult;
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KITEMCOMPOUNDER_H
#define KITEMCOMPOUNDER_H


#define COMPOUND_PARAM_COUNT	4
#define TARGET_ITEM_IDX			0

class KItem;
class KCompoundPart;
class TCompoundParams;
class TCompoundResult;
class TCompoundDelResult;
struct TCompoundResultEx;

#include <vector>
/*!
\brief
	合成规则基类。		
*/
class KItemCompounder
{
public:
	virtual int		Compund				( TCompoundParams * CompundParams					) = 0;
#ifdef _SERVER
protected:
	virtual void	DeleteMoney			( TCompoundParams * pCompundParams					);
	virtual void	DeleteSkillPoint	( TCompoundParams * pCompundParams					);
	virtual void	DeleteItems			( TCompoundParams * pCompundParams, bool bDestroySrcItem , TCompoundDelResult * pAppResult);
	//谢鉷加……begin
	virtual void	DeleteMoney			( TCompoundResultEx* compResult						);
	virtual void	DeleteSkillPoint	( TCompoundResultEx* compResult						);
	virtual void	DeleteItems			( TCompoundResultEx* compResult						);
	virtual bool	AddItemToPlayer		( TCompoundResultEx* compResult						);
	virtual bool	checkResult			( TCompoundResultEx* compResult						);
	//谢鉷加……end
	virtual void	AddItemToPlayer		( int nGenre, int nDetail, int nParaticular, int nLevel, int nPlayerIndex,  char* szPlusInfo = NULL,DWORD * pResultItemID = NULL);
	virtual void	AddCompItemToPlayer	( int nCompoundType, TCompoundResult& rResult, TCompoundParams* pCompundParams );
#endif

};

/*!
\brief
	合成的材料封装。
*/
class KCompoundPart
{
public:
	KCompoundPart()
	{
		m_pItem = NULL;
		memset(m_nParam, 0, sizeof(m_nParam));
	}
	KCompoundPart( const KCompoundPart& rPart )
	{
		m_pItem = rPart.m_pItem;
		memcpy( m_nParam, rPart.m_nParam, COMPOUND_PARAM_COUNT * sizeof(int));
	}
public:
	inline KItem*		GetItem	( void										) const;
	inline void			SetItem	(KItem * pItem								);  	
	inline const int	GetParam(unsigned int ulParamIndex					) const;
	inline void			SetParam(unsigned int ulParamIndex, int nParamValue	);
private:
	KItem * m_pItem;
	int		m_nParam[COMPOUND_PARAM_COUNT];
};

inline KItem* KCompoundPart::GetItem() const
{
	return m_pItem;
};

inline void KCompoundPart::SetItem(KItem * pItem)  	
{
	m_pItem = pItem;
};

inline const int KCompoundPart::GetParam(unsigned int ulParamIndex) const
{
	if (ulParamIndex >= COMPOUND_PARAM_COUNT)
	{
		_ASSERT(0);
		return 0;
	}
	return m_nParam[ulParamIndex];
};

inline void KCompoundPart::SetParam(unsigned int ulParamIndex, int nParamValue)
{
	if (ulParamIndex >= COMPOUND_PARAM_COUNT)
	{
		_ASSERT(0);
		return;
	}
	m_nParam[ulParamIndex] = nParamValue;
}

/*!
\brief
	调用合成函数时的参数集，可用于扩展。		
*/
class TCompoundParams
{
public:
	TCompoundParams()
	{
		nPlayerIndex	= 0;
		pPartArray		= NULL;
		nTypeCount		= 0;
		nCompoundType	= 0;
		nMoney			= 0;
		nSkillPoint		= 0;
	}
	TCompoundParams( const TCompoundParams& rParams )
	{
		nPlayerIndex	= rParams.nPlayerIndex;
		pPartArray		= rParams.pPartArray;
		nTypeCount		= rParams.nTypeCount;
		nCompoundType	= rParams.nCompoundType;
		nMoney			= rParams.nMoney;
		nSkillPoint		= rParams.nSkillPoint;
	}
public:
	KCompoundPart*	pPartArray;
	int				nTypeCount;
	int				nPlayerIndex;
	int				nCompoundType;
	int				nMoney;
	int				nSkillPoint;
	char			szPlusInfo[COMMON_CLIENT_MSG_LEN_64];

};	

class TCompoundDelResult
{
public:
	int            m_nDelCount[MAX_LEVELUP_ITEMS_COUNT];
	void           Clear(void)
	{
		memset(m_nDelCount,0,sizeof(m_nDelCount));
	}

};

/*!
\brief
	合成生效后产生的结果。
*/
class TCompoundResult
{
public:
	TCompoundResult()
	{
		nNewItemGenre0		= 0;
		nNewItemDetail0		= 0;
		nNewItemParticular0	= 0;
		nNewItemLevel0		= 0;
		nNewItemGenre1		= 0;
		nNewItemDetail1		= 0;
		nNewItemParticular1	= 0;
		nNewItemLevel1		= 0;
		nBuffID				= 0;
		nSuccessRate0		= 0;
		nSuccessRate1		= 0;
		nSuccessRate2		= 0;
		nSuccessRate3		= 0;
		nSuccessRate4		= 0;
		nSuccessPlusRate	= 0;
		nDestroyRate		= 0;
		nRuleTypePlus		= -1;
		nPlusRate0			= 0;
		nPlusBuffID0		= 0;
		nPlusLevel0			= 0;
		nNewItemID0         = 0;
	}
	TCompoundResult( const TCompoundResult& rResult )
	{
		nNewItemGenre0		= rResult.nNewItemGenre0;
		nNewItemDetail0		= rResult.nNewItemDetail0;
		nNewItemParticular0	= rResult.nNewItemParticular0;
		nNewItemLevel0		= rResult.nNewItemLevel0;
		nNewItemGenre1		= rResult.nNewItemGenre1;
		nNewItemDetail1		= rResult.nNewItemDetail1;
		nNewItemParticular1	= rResult.nNewItemParticular1;
		nNewItemLevel1		= rResult.nNewItemLevel1;
		nBuffID				= rResult.nBuffID;
		nSuccessRate0		= rResult.nSuccessRate0;
		nSuccessRate1		= rResult.nSuccessRate1;
		nSuccessRate2		= rResult.nSuccessRate2;
		nSuccessRate3		= rResult.nSuccessRate3;
		nSuccessRate4		= rResult.nSuccessRate4;
		nSuccessPlusRate	= rResult.nSuccessPlusRate;
		nDestroyRate		= rResult.nDestroyRate;
		nRuleTypePlus		= rResult.nRuleTypePlus;
		nPlusRate0			= rResult.nPlusRate0;
		nPlusBuffID0		= rResult.nPlusBuffID0;	
		nPlusLevel0			= rResult.nPlusLevel0;
		nNewItemID0         = rResult.nNewItemID0;
	}
public:
	int	nNewItemGenre0;
	int	nNewItemDetail0;
	int	nNewItemParticular0;
	int	nNewItemLevel0;
	int	nNewItemGenre1;
	int	nNewItemDetail1;
	int	nNewItemParticular1;
	int	nNewItemLevel1;
	int nBuffID;
	int	nSuccessRate0;
	int	nSuccessRate1;
	int	nSuccessRate2;
	int	nSuccessRate3;
	int	nSuccessRate4;
	int nSuccessPlusRate;
	int nDestroyRate;
	int	nRuleTypePlus;
	int nPlusRate0;
	int nPlusBuffID0;
	short nPlusLevel0;
	DWORD nNewItemID0;  //This is for sync to Client
};

struct TCompoundResultEx
{
	struct deleteItemInfo
	{
		int itemId;
		int deleteCount;
	};
	
	struct genItemInfo
	{
		int	genre;
		int	detail;
		int	particular;
		int	level;
		int count;
		int buffId;
		int	successRate;
	};

	int ruleId;
	int destoryRate;
	int reduceMoney;
	DWORD reduceSkillPoint;
	std::vector<deleteItemInfo> deleteItems;
	std::vector<genItemInfo> newItems;

	int playerIndex;
	BOOL bBind;
};
#endif
