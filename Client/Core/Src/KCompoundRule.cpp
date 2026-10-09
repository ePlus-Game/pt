//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 10/31/2006 19:37
//      File_base        : KCompoundRule
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KPlayer.h"
#include "KItemList.h"
#include "KObjSet.h"
#include "KSubWorldSet.h"
#include "KItemCompounder.h"
#include "KCompoundRule.h"
#include "KItemGenerator.h"
#ifndef _SERVER
	#include <string>
#endif

KCompoundRules g_CompoundRule;

//////////////////////////////////////////////////////////////////////////
// parm: 
// desc: 把字符串组合的3个数字拆分
void _getItemByString(int &nGenre, int &nDetail, int &nParticular, int &nCount, int &nLevel, int &nQuality, int &nYao, int &nColor, char* sValue, int nStringSize)
{
	nGenre = 0; nDetail = 0; nParticular = 0; nCount = 0;
	nLevel = 0; nQuality = 0; nYao = 0; nColor = 0;
	if ( (sValue == NULL) || (nStringSize == 0) )
	{
		return;
	}

	int n = 0;
	int nRet[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char seps[]   = "|";
	char* token = strtok( sValue, seps );
	while( token != NULL )
	{
		nRet[n] = atoi(token);
		n++;
		token = strtok( NULL, seps );
		if (n >= 8)
			break;
	}
	
	nGenre = nRet[0]; nDetail = nRet[1]; nParticular = nRet[2]; nCount = nRet[3]; nLevel = nRet[4]; nQuality = nRet[5]; nYao = nRet[6]; nColor = nRet[7];
}

void _getRateParamByString(int &nCount, int &nRate, char* sValue, int nStringSize)
{
	nCount = 0; nRate = 0;
	if ( (sValue == NULL) || (nStringSize == 0) )
	{
		return;
	}

	int n = 0;
	int nRet[2] = {0, 0};
	char seps[]   = "|";
	char* token = strtok( sValue, seps );
	while( token != NULL )
	{
		nRet[n] = atoi(token);
		n++;
		token = strtok( NULL, seps );
		if (n >= 2)
			break;
	}
	
	nCount = nRet[0]; nRate = nRet[1];
}

//////////////////////////////////////////////////////////////////////////
// parm: 
// desc: 从规则表里面初始化组合规则
bool	KCompoundRules::Init()
{
	InitRules();
	InitSuccessRatio();

	return true;
}

//////////////////////////////////////////////5////////////////////////////
// parm: Entry 有效的组合规则描述
// desc: 加入新的组合规则，返回True表示成功加入，失败返回False
bool	KCompoundRules::AddRule( const KCompoundRulesIndex& Idx, const KCompoundRuleEntry& Entry)
{
	// 检查这个规则是否已经加入到规则列表了
	KRuleTable& tabRule = GetRuleByIndex( Idx );
	int nCount = tabRule.size();
	for (int i=0; i<nCount; i++)
	{
		if ( 0 == memcmp(&tabRule[i], &Entry, sizeof(KCompoundRuleEntry)))
			return false;
	}

	// 加入规则
	tabRule.push_back(Entry);
	return true;
}

bool	KCompoundRules::AddSuccessRatio( const KCompoundSuccessRatio& Entry	)
{
	// 检查这个规则是否已经加入到规则列表了
	int nCount = m_CompoundSuccessRatioTable.size();
	for (int i=0; i<nCount; i++)
	{
		if ( 0 == memcmp(&m_CompoundSuccessRatioTable[i], &Entry, sizeof(KCompoundRuleEntry)))
			return false;
	}

	// 加入规则
	m_CompoundSuccessRatioTable.push_back(Entry);
	return true;	
}

bool KCompoundRules::isItemDesValid(KSrcItemDescriptor des)
{
	if(des.nItemColor == -1
		&& des.nItemCount == -1
		&& des.nItemDetail == -1
		&& des.nItemGenre == -1
		&& des.nItemLevel == -1
		&& des.nItemParticular == -1
		&& des.nItemQuality == -1
		&& des.nItemYao == -1)
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// parm: pPlayer 表示需要检查的哪一个玩家 pParams 是待组合的装备
// desc: 规则检查并返回适应的组合函数类型, COMPOUND_LEVELUP或责
//       COMPOUND_NORMAL返回-1表示规则检查失败,其他值表示规则ID
int	KCompoundRules::CheckRules( TCompoundParams* pParams,	TCompoundResult* outResult ,TCompoundDelResult * delResult)
{
	if (delResult == NULL)
		return enchaser_error_conditionisinvalid;

	ZeroMemory( outResult, sizeof(TCompoundResult) );
	KCompoundRulesIndex idxRule;
	int nRet = enchaser_error_no;

	//检查装备是否锁定
	for ( int compoundPartIndex = 0; compoundPartIndex <= pParams->nTypeCount; ++compoundPartIndex )
	{
		KItem* pPartItem = pParams->pPartArray[compoundPartIndex].GetItem();
		if (pPartItem && pPartItem->IsLocked(Player[pParams->nPlayerIndex].GetNetConnectIdx(), false))
		{
			nRet = enchaser_error_conditionisinvalid;
			return nRet;
		}
	}

	idxRule.nType = pParams->nCompoundType;
	KItem* pItem = pParams->pPartArray[0].GetItem();
	int	nRuleTypePlus = -1;
	if ( pItem && idxRule.nType == COMPOUND_LEVELUP )
	{
		nRuleTypePlus = pItem->GetLevelupType();
		idxRule.nChangeCount = pItem->GetLevelupTimes() + 1;
	}
	else
	{
		idxRule.nChangeCount = 1;
	}
	
	KRuleTable& ruleList = GetRuleByIndex( idxRule );

	int nHitIdx = -1;
	int i = 0;
	for ( i = 0; i < ruleList.size(); ++i )
	{
		ClearRealParamList();
		delResult->Clear();

		int nHitCount = 0;
		int j = 0;
		KItem* pSrcItem = pParams->pPartArray[j].GetItem();
		
		//如果要求有个targetitem
		if(isItemDesValid(ruleList[i].TargetItem))
		{
			if ( pSrcItem == NULL || (pSrcItem && !CheckItem( pSrcItem, ruleList[i].TargetItem )))
			{
				continue;
			}

			int nLevelUpPlus   = pSrcItem->GetLevelupType();
			if ( idxRule.nType == COMPOUND_LEVELUP && nLevelUpPlus != ruleList[i].nRuleTypePlus && nLevelUpPlus != -1 )
			{
			    continue;
			}//endif

		}//endif	
		
		int x = 0;
		for ( x = 0; x < ruleList[i].nSrcItemsCount; ++x)
		{
			int nItemCountNeed     = ruleList[i].SrcItemsArray[x].nItemCount;
			int nItemQualityNeed   = ruleList[i].SrcItemsArray[x].nItemQuality;

			for ( j = 1; j <= pParams->nTypeCount; ++j )
			{
				pSrcItem                 = pParams->pPartArray[j].GetItem();
				int nSrcCurrentItemCount = pSrcItem->GetItemCount() - delResult->m_nDelCount[j];
				
				if ( pSrcItem && nSrcCurrentItemCount && CheckItem( pSrcItem, ruleList[i].SrcItemsArray[x] ) )
				{	
					int       nDelCount  = 0;

					if ( nItemCountNeed > 0 )
					{
						nDelCount        = nSrcCurrentItemCount > nItemCountNeed ? nItemCountNeed : nSrcCurrentItemCount;
					}//endif

					if ( nItemQualityNeed > 0 && pSrcItem->GetItemQuality() > 0 )
					{
						int       nQuality              = nSrcCurrentItemCount * pSrcItem->GetItemQuality();
						int       nQualityItemCountNeed = nQuality >= nItemQualityNeed ? ( nItemQualityNeed - 1 ) / pSrcItem->GetItemQuality() + 1: nSrcCurrentItemCount;
						
						nDelCount                       = nDelCount > nQualityItemCountNeed ? nDelCount : nQualityItemCountNeed;
					}//endif

					if (nDelCount)
					{
						delResult->m_nDelCount[j] += nDelCount;
						nItemCountNeed            -= nDelCount;
						
						if (pSrcItem->GetItemQuality() > 0)
							nItemQualityNeed          -= nDelCount * pSrcItem->GetItemQuality();
						
						AddToRealParamList( x,
							ruleList[i].SrcItemsArray[x].nItemGenre,
							ruleList[i].SrcItemsArray[x].nItemDetail,
							ruleList[i].SrcItemsArray[x].nItemParticular,
							pSrcItem->GetItemCount(), pSrcItem->GetItemQuality() );
						
					}//endif
					
				}//endif	
			}

			if (nItemCountNeed <= 0 && nItemQualityNeed <= 0)
			{
				++nHitCount;
			}//endif

		}

		if ( nHitCount >= ruleList[i].nSrcItemsCount )
		{
			nHitIdx = i;
			break;
		}
	}
	if ( nHitIdx == -1 )
	{
		nRet = enchaser_error_conditionisinvalid;
	}
	else
	{
		long lPlayerMoney = Player[pParams->nPlayerIndex].GetItemList().GetMoney(room_equipment);
		DWORD lPlayerSkillPoint = Player[pParams->nPlayerIndex].GetSkillExp();
		pParams->nMoney = ruleList[nHitIdx].nMoney;
		pParams->nSkillPoint = ruleList[nHitIdx].nSkillPoint;
		if ( lPlayerMoney < ruleList[nHitIdx].nMoney )
		{
			nRet = enchaser_error_money;
		}
		else
		{
			if ( lPlayerSkillPoint <  ruleList[nHitIdx].nSkillPoint )
			{
				nRet = enchaser_error_skillpoint;
			}
			else
			{
				if(	CheckCount( ruleList, nHitIdx ) )
				{
					outResult->nRuleTypePlus		= ruleList[nHitIdx].nRuleTypePlus;
					outResult->nNewItemGenre0		= ruleList[nHitIdx].DstItem0.nItemGenre;
					outResult->nNewItemParticular0	= ruleList[nHitIdx].DstItem0.nItemParticular;
					outResult->nNewItemDetail0		= ruleList[nHitIdx].DstItem0.nItemDetail;
					outResult->nNewItemLevel0		= ruleList[nHitIdx].DstItem0.nItemLevel;
					outResult->nNewItemGenre1		= ruleList[nHitIdx].DstItem1.nItemGenre;
					outResult->nNewItemParticular1	= ruleList[nHitIdx].DstItem1.nItemParticular;
					outResult->nNewItemDetail1		= ruleList[nHitIdx].DstItem1.nItemDetail;
					outResult->nNewItemLevel1		= ruleList[nHitIdx].DstItem1.nItemLevel;
					outResult->nBuffID				= ruleList[nHitIdx].nBuff;
					outResult->nSuccessRate0		= ruleList[nHitIdx].CompoundSuccessRate0;
					outResult->nSuccessRate1		= ruleList[nHitIdx].CompoundSuccessRate1;
					outResult->nPlusRate0			= ruleList[nHitIdx].nPlusRate0;
					outResult->nPlusBuffID0			= ruleList[nHitIdx].nPlusBuffID0;
					outResult->nPlusLevel0			= ruleList[nHitIdx].nPlusLevel0;
					if ( ruleList[nHitIdx].bPlusRate )
					{
						outResult->nSuccessPlusRate = GetPlusSuccessRate( pParams ,delResult);
					}
					outResult->nDestroyRate	= ruleList[nHitIdx].nDestroyRate;
					nRet = enchaser_error_no;
				}
				else
				{
					nRet = enchaser_error_conditionisinvalid;
				}
			}
		}
	
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
// parm: pPlayer 表示需要检查的哪一个玩家 pParams 是待组合的装备
// desc: 规则检查并返回适应的组合函数类型, COMPOUND_LEVELUP或责
//       COMPOUND_NORMAL返回-1表示规则检查失败,其他值表示规则ID
int	KCompoundRules::CheckRules( TCompoundParams* pParams,	TCompoundResultEx* outResult, int ruleId)
{
	KCompoundRulesIndex idxRule;
	idxRule.nType = pParams->nCompoundType;
	
	KCompoundRuleEntry* selRule = getRuleById(pParams->nCompoundType, 1, ruleId);
	if(NULL == selRule)
	{
		return enchaser_error_conditionisinvalid;
	}

	for(int i = 0; i < selRule->nSrcItemsCount; ++i)
	{
		//这里不能用引用
		KSrcItemDescriptor ruleReqItem = selRule->SrcItemsArray[i];
		for(int j = 0; j < pParams->nTypeCount; ++j)
		{
			KItem* pSrcItem = pParams->pPartArray[j].GetItem();
			if(NULL == pSrcItem)
			{
				continue;
			}
			if(pSrcItem->GetGenre() == ruleReqItem.nItemGenre
				&& pSrcItem->GetParticular() == ruleReqItem.nItemParticular
				&& pSrcItem->GetDetailType() == ruleReqItem.nItemDetail
				&& pSrcItem->GetLevel() == ruleReqItem.nItemLevel)
			{
				TCompoundResultEx::deleteItemInfo deleteItem;
				deleteItem.itemId = pSrcItem->GetID();
				if(ruleReqItem.nItemCount > pSrcItem->GetItemCount())
				{
					ruleReqItem.nItemCount -= pSrcItem->GetItemCount();
					deleteItem.deleteCount = pSrcItem->GetItemCount();
				}
				else
				{
					deleteItem.deleteCount = ruleReqItem.nItemCount;
					ruleReqItem.nItemCount = 0;
				}
				outResult->deleteItems.push_back(deleteItem);
			}
		}
		if(ruleReqItem.nItemCount != 0)
		{
			return enchaser_error_less_material;
		}
	}
	
	//检查是否金钱和蕴魂够
	unsigned long lPlayerMoney	= Player[pParams->nPlayerIndex].GetItemList().GetMoney(room_equipment);
	DWORD lPlayerSkillPoint		= Player[pParams->nPlayerIndex].GetSkillExp();
	if ( lPlayerMoney < selRule->nMoney )
	{
		return enchaser_error_money;
	}
	if ( lPlayerSkillPoint <  selRule->nSkillPoint )
	{
		return enchaser_error_skillpoint;
	}
	outResult->reduceMoney = selRule->nMoney;
	outResult->reduceSkillPoint = selRule->nSkillPoint;
	outResult->bBind		= selRule->bBind;

	//新生成物品
	TCompoundResultEx::genItemInfo newItem;

	

	newItem.genre		= selRule->DstItem0.nItemGenre;
	newItem.particular	= selRule->DstItem0.nItemParticular;
	newItem.detail		= selRule->DstItem0.nItemDetail;
	newItem.level		= selRule->DstItem0.nItemLevel;
	newItem.count		= selRule->DstItem0.nItemCount;
	newItem.successRate = selRule->CompoundSuccessRate0;

	newItem.buffId		= selRule->nBuff;
	outResult->newItems.push_back(newItem);

	newItem.genre		= selRule->DstItem1.nItemGenre;
	newItem.particular	= selRule->DstItem1.nItemParticular;
	newItem.detail		= selRule->DstItem1.nItemDetail;
	newItem.level		= selRule->DstItem1.nItemLevel;
	newItem.count		= selRule->DstItem1.nItemCount;
	newItem.successRate = selRule->CompoundSuccessRate1;
	outResult->newItems.push_back(newItem);

	newItem.genre		= selRule->DstItem2.nItemGenre;
	newItem.particular	= selRule->DstItem2.nItemParticular;
	newItem.detail		= selRule->DstItem2.nItemDetail;
	newItem.level		= selRule->DstItem2.nItemLevel;
	newItem.count		= selRule->DstItem2.nItemCount;
	newItem.successRate = selRule->CompoundSuccessRate2;
	outResult->newItems.push_back(newItem);

	newItem.genre		= selRule->DstItem3.nItemGenre;
	newItem.particular	= selRule->DstItem3.nItemParticular;
	newItem.detail		= selRule->DstItem3.nItemDetail;
	newItem.level		= selRule->DstItem3.nItemLevel;
	newItem.count		= selRule->DstItem3.nItemCount;
	newItem.successRate = selRule->CompoundSuccessRate3;
	outResult->newItems.push_back(newItem);

	newItem.genre		= selRule->DstItem4.nItemGenre;
	newItem.particular	= selRule->DstItem4.nItemParticular;
	newItem.detail		= selRule->DstItem4.nItemDetail;
	newItem.level		= selRule->DstItem4.nItemLevel;
	newItem.count		= selRule->DstItem4.nItemCount;
	newItem.successRate = selRule->CompoundSuccessRate4;
	outResult->newItems.push_back(newItem);


	outResult->destoryRate = selRule->nDestroyRate;

	outResult->playerIndex = pParams->nPlayerIndex;
	return enchaser_error_no;
}


int	KCompoundRules::checkParams(TCompoundParams* pParams)
{
	for(int i = 0; i < pParams->nTypeCount; ++i)
	{
		for(int j = i + 1; j < pParams->nTypeCount; ++j)
		{
			if(pParams->pPartArray[i].GetItem() == pParams->pPartArray[j].GetItem())
			{
				return enchaser_error_conditionisinvalid;
			}
		}
	}

	return enchaser_error_no;
}

void KCompoundRules::InitRules( void )
{
	char szTABFilePath[MAX_PATH];
	g_SetRootPath(NULL);
	KTabFile tab;
	sprintf(szTABFilePath, "\\settings\\item\\compound_rules.txt");
	if (!tab.Load(szTABFilePath))
		return;

	int i=0;
	for (i=2; i<=tab.GetHeight(); i++ )
	{
		KCompoundRuleEntry Entry;
		memset(&Entry, 0, sizeof(KCompoundRuleEntry));
	
		int nType = 0;
		tab.GetInteger(i, 1, -1, &Entry.nRuleType);
		nType = Entry.nRuleType;

		int nTypePlus = 0;
		tab.GetInteger(i, 2, -1, &Entry.nRuleTypePlus);
		nTypePlus = Entry.nRuleTypePlus;

		tab.GetInteger(i, 3, 0, &Entry.nChangeCount);
		int nChangeCount = Entry.nChangeCount;

 		tab.GetInteger(i, 4, 0, &Entry.nSrcItemsCount);
		int nItemTypeCount = Entry.nSrcItemsCount;

		char sValue[32];
		int nGenre = 0; int nDetail = 0; int nParticular = 0;

		int sourItemCount = Entry.nSrcItemsCount;
		int index = 0;
		for(int j = 0; j < MAX_COMPOUND && index < MAX_COMPOUND && sourItemCount; ++j)
		{
			tab.GetString(i, 5 + j, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32);
			
			if(!strcmp(sValue, "-1|-1|-1|-1|-1|-1|-1|-1"))
			{
				continue;
			}

			_getItemByString(
				Entry.SrcItemsArray[index].nItemGenre, 
				Entry.SrcItemsArray[index].nItemDetail, 
				Entry.SrcItemsArray[index].nItemParticular,
				Entry.SrcItemsArray[index].nItemLevel,
				Entry.SrcItemsArray[index].nItemCount,
				Entry.SrcItemsArray[index].nItemQuality,
				Entry.SrcItemsArray[index].nItemYao,
				Entry.SrcItemsArray[index].nItemColor,
				sValue,	32);
			++index;
			--sourItemCount;
		}

		char szIdxItem[COMMON_CLIENT_MSG_LEN_32];
		tab.GetString(i, 15, "-1|-1|-1|-1|-1|-1|-1|-1", szIdxItem, COMMON_CLIENT_MSG_LEN_32);
		_getItemByString(
			Entry.TargetItem.nItemGenre, 
			Entry.TargetItem.nItemDetail, 
			Entry.TargetItem.nItemParticular,
			Entry.TargetItem.nItemLevel,
			Entry.TargetItem.nItemCount,
			Entry.TargetItem.nItemQuality,
			Entry.TargetItem.nItemYao,
			Entry.TargetItem.nItemColor,
			szIdxItem, 32);

		tab.GetInteger(i, 16, 0, &Entry.nMoney);

		tab.GetInteger(i, 17, 0, &Entry.nSkillPoint );

		tab.GetInteger(i, 18, 0, &Entry.nBuff);

		tab.GetInteger(i, 19, 0, &Entry.CompoundSuccessRate0 );

		tab.GetString(i, 20, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32);
		_getItemByString(
				Entry.DstItem0.nItemGenre, 
				Entry.DstItem0.nItemDetail, 
				Entry.DstItem0.nItemParticular,
				Entry.DstItem0.nItemLevel,
				Entry.DstItem0.nItemCount,
				Entry.DstItem0.nItemQuality,
				Entry.DstItem0.nItemYao,
				Entry.DstItem0.nItemColor,
				sValue, 32);

 		tab.GetInteger(i, 21, 0, &Entry.CompoundSuccessRate1);

		tab.GetString(i, 22, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32);
		_getItemByString(
				Entry.DstItem1.nItemGenre, 
				Entry.DstItem1.nItemDetail, 
				Entry.DstItem1.nItemParticular,
				Entry.DstItem1.nItemLevel,
				Entry.DstItem1.nItemCount,
				Entry.DstItem1.nItemQuality,
				Entry.DstItem1.nItemYao,
				Entry.DstItem1.nItemColor,
				sValue, 32);

		 tab.GetInteger(i, 23, 0, &Entry.CompoundSuccessRate2);

		tab.GetString(i, 24, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32);
		_getItemByString(
				Entry.DstItem2.nItemGenre, 
				Entry.DstItem2.nItemDetail, 
				Entry.DstItem2.nItemParticular,
				Entry.DstItem2.nItemLevel,
				Entry.DstItem2.nItemCount,
				Entry.DstItem2.nItemQuality,
				Entry.DstItem2.nItemYao,
				Entry.DstItem2.nItemColor,
				sValue, 32);

		 tab.GetInteger(i, 25, 0, &Entry.CompoundSuccessRate3);

		tab.GetString(i, 26, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32);
		_getItemByString(
				Entry.DstItem3.nItemGenre, 
				Entry.DstItem3.nItemDetail, 
				Entry.DstItem3.nItemParticular,
				Entry.DstItem3.nItemLevel,
				Entry.DstItem3.nItemCount,
				Entry.DstItem3.nItemQuality,
				Entry.DstItem3.nItemYao,
				Entry.DstItem3.nItemColor,
				sValue, 32);

		 tab.GetInteger(i, 27, 0, &Entry.CompoundSuccessRate4);

		tab.GetString(i, 28, "-1|-1|-1|-1|-1|-1|-1|-1", sValue, 32);
		_getItemByString(
				Entry.DstItem4.nItemGenre, 
				Entry.DstItem4.nItemDetail, 
				Entry.DstItem4.nItemParticular,
				Entry.DstItem4.nItemLevel,
				Entry.DstItem4.nItemCount,
				Entry.DstItem4.nItemQuality,
				Entry.DstItem4.nItemYao,
				Entry.DstItem4.nItemColor,
				sValue, 32);

		int nValue = 0;
		tab.GetInteger(i, 29, 0, &nValue );
		if ( nValue > 0 )
		{
			Entry.bPlusRate = true;
		}
		else
		{
			Entry.bPlusRate = false;
		}
		tab.GetInteger(i, 30, 0, &Entry.nDestroyRate);
		tab.GetInteger(i, 31, 0, &Entry.MapSuccessRate[0]);
		tab.GetInteger(i,32,0,&Entry.nPlusRate0);
		tab.GetInteger(i,33,0,&Entry.nPlusBuffID0);
		tab.GetInteger(i,34,0,&Entry.nPlusLevel0);
		tab.GetInteger(i,35,0,&Entry.bBind);

		KCompoundRulesIndex Idx;
		Idx.nType			= nType;
		Idx.nChangeCount	= nChangeCount;
		AddRule(Idx, Entry);
	}
}

void KCompoundRules::InitSuccessRatio( void )
{
	char szTABFilePath[MAX_PATH];
	g_SetRootPath( NULL );
	KTabFile tab;
	sprintf( szTABFilePath, "\\settings\\item\\compound_successratio.txt" );
	if ( !tab.Load(szTABFilePath) )
	{
		return;
	}

	int i = 0;
	for ( i=2; i <= tab.GetHeight(); ++i )
	{
		KCompoundSuccessRatio Entry;
		memset( &Entry, 0, sizeof(KCompoundSuccessRatio) );
	
		char sValue[COMMON_CLIENT_MSG_LEN_32];

		tab.GetString( i, 1, "-1|-1|-1|-1|-1|-1|-1|-1", sValue,COMMON_CLIENT_MSG_LEN_32 );
		_getItemByString(
			Entry.RateItem.nItemGenre, 
			Entry.RateItem.nItemDetail, 
			Entry.RateItem.nItemParticular,
			Entry.RateItem.nItemLevel,
			Entry.RateItem.nItemCount,
			Entry.RateItem.nItemQuality,
			Entry.RateItem.nItemYao,
			Entry.RateItem.nItemColor,
			sValue, 
			COMMON_CLIENT_MSG_LEN_32);

		tab.GetString(i, 2, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nCount0.nCount,
			Entry.nCount0.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		tab.GetString(i, 3, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nCount1.nCount,
			Entry.nCount1.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		tab.GetString(i, 4, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nCount2.nCount,
			Entry.nCount2.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		tab.GetString(i, 5, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nQuality0.nCount,
			Entry.nQuality0.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		tab.GetString(i, 6, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nQuality1.nCount,
			Entry.nQuality1.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		tab.GetString(i, 7, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nQuality2.nCount,
			Entry.nQuality2.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		tab.GetString(i, 8, "0|0", sValue, COMMON_CLIENT_MSG_LEN_32);
		_getRateParamByString( 
			Entry.nQuality3.nCount,
			Entry.nQuality3.nRate,
			sValue,
			COMMON_CLIENT_MSG_LEN_32 );

		AddSuccessRatio(Entry);
	}
}

KCompoundRules::KRuleTable&	KCompoundRules::GetRuleByIndex	( const KCompoundRulesIndex& Idx )
{
	if(Idx.nType == COMPOUND_SMITH)
	{
		return m_CompoundRuleTable.tabRule[COMPOUND_MAKE].tabChangeCount[Idx.nChangeCount];
	}
	else
	{
		return m_CompoundRuleTable.tabRule[Idx.nType].tabChangeCount[Idx.nChangeCount];
	}
}

bool KCompoundRules::CheckItem ( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem )
{
	if ( rDstItem.nItemGenre == -1)
	{
		return CheckYao( pSrcItem, rDstItem ) && CheckQuality( pSrcItem, rDstItem ) && CheckColor( pSrcItem, rDstItem );
	}
	else
	{
		if ( rDstItem.nItemGenre == pSrcItem->GetGenre() )
		{
			if ( rDstItem.nItemDetail == -1 )
			{
				return CheckYao( pSrcItem, rDstItem ) && CheckQuality( pSrcItem, rDstItem ) && CheckColor( pSrcItem, rDstItem );
			}
			else
			{
				if ( rDstItem.nItemDetail == pSrcItem->GetDetailType() )
				{
					if ( rDstItem.nItemParticular == -1 )
					{
						return CheckYao( pSrcItem, rDstItem ) && CheckQuality( pSrcItem, rDstItem ) && CheckColor( pSrcItem, rDstItem );
					} 
					else
					{
						if ( rDstItem.nItemParticular == pSrcItem->GetParticular() )
						{
							if ( rDstItem.nItemLevel == -1 )
							{
								return CheckYao( pSrcItem, rDstItem ) && CheckQuality( pSrcItem, rDstItem ) && CheckColor( pSrcItem, rDstItem );
							}
							else
							{
								if ( rDstItem.nItemLevel == pSrcItem->GetLevel() )
								{
									return CheckYao( pSrcItem, rDstItem ) && CheckQuality( pSrcItem, rDstItem ) && CheckColor( pSrcItem, rDstItem );
								}
								else
								{
									return false;
								}
							}						
						} 
						else
						{
							return false;
						}
					}		
				} 
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
}

#ifndef _SERVER
bool KCompoundRules::checkMaterial(const vector<int>& sourIndex, const KCompoundRuleEntry& rule)
{
	vector<int>    vItemCountRec;
	//CurrentCountRecord 
	for (int nRec = 0 ; nRec < sourIndex.size() ; ++ nRec)
	{
		int        nItemIndex = sourIndex[nRec];
		vItemCountRec.push_back( Item[nItemIndex].GetItemCount() );

	}//end for nRec

	int nHitCount = 0;
	for(int requireItemIndex = 0; requireItemIndex < rule.nSrcItemsCount; ++requireItemIndex)
	{
		KSrcItemDescriptor requireItem = rule.SrcItemsArray[requireItemIndex];
		for(int haveItemIndex = 0; haveItemIndex < sourIndex.size(); ++haveItemIndex)
		{
			int sourItemIndex = sourIndex[haveItemIndex];
			const KItem& sourItem = Item[sourItemIndex];
			
			if(!sourItem.GetID())
			{
				continue;
			}//endif

			int nCurrentItemCountRec = vItemCountRec[haveItemIndex];
			if (nCurrentItemCountRec <= 0)
			{
				continue;
			}//endif

			if(CheckItem(&sourItem, requireItem))
			{
				int       nDelItemCount        = 0;

				if (  requireItem.nItemCount > 0 )
				{
					nDelItemCount        = requireItem.nItemCount > nCurrentItemCountRec ? nCurrentItemCountRec : requireItem.nItemCount ;
				}//endif

				if( requireItem.nItemQuality > 0 && sourItem.GetItemQuality() > 0)
				{
					int       nQuality              = nCurrentItemCountRec * sourItem.GetItemQuality();
					int       nQualityItemCountNeed = nQuality >= requireItem.nItemQuality ? ( requireItem.nItemQuality - 1 ) / sourItem.GetItemQuality() + 1: nCurrentItemCountRec;
						
					nDelItemCount                    = nDelItemCount > nQualityItemCountNeed ? nDelItemCount : nQualityItemCountNeed;
				}//endif
				
				if (nDelItemCount)
				{
					vItemCountRec[haveItemIndex] -= nDelItemCount;
					requireItem.nItemCount       -= nDelItemCount;

					if (sourItem.GetItemQuality() > 0)
						requireItem.nItemQuality     -= nDelItemCount * sourItem.GetItemQuality();
				}//endif
				
			}//endif

		}//endif

		if(requireItem.nItemCount > 0 || requireItem.nItemQuality > 0)
		{
			return false;
		}//endif

	}//end for 

	return true;
}

int	KCompoundRules::GetCompoundRuleInfo(const CompoundInitMaterial& initMaterial, KCompoundRuleEntry& rRule)
{
 	KCompoundRulesIndex idxRule;
 	int nRet = enchaser_error_no;
 
 	idxRule.nType = initMaterial.compoundType;
 
	KItem& targetItem = Item[initMaterial.targetItemIndex];

	//以下三种情况都必须要源道具
 	if(COMPOUND_LEVELUP == idxRule.nType
		|| COMPOUND_ADDYAO == idxRule.nType
		|| COMPOUND_GETYAO == idxRule.nType)
//		|| COMPOUND_ADDMAGIC == idxRule.nType)
 	{
 		if(initMaterial.targetItemIndex == COMMON_ITEM_INVALID_ID)
 		{
 			return enchaser_error_less_targetItem;
 		}
 	}

	idxRule.nChangeCount = 1;
	if(COMPOUND_LEVELUP == idxRule.nType)
	{
 		idxRule.nChangeCount = targetItem.GetLevelupTimes() + 1;
 	}
 
 	KRuleTable& ruleList = GetRuleByIndex(idxRule);
 
 	int ruleIndex = -1;

	bool haveTargetItem = false;//用来记录是否有匹配的源道具，即使缺少材料也可以返回不同的提示
 	for(int i = 0; i < ruleList.size(); ++i)
 	{ 
 		KCompoundRuleEntry& rule = ruleList[i];
 
 		if(initMaterial.targetItemIndex != COMMON_ITEM_INVALID_ID)
		{
			if(!CheckItem(&targetItem, rule.TargetItem))
			{
				continue;
			}

			if(idxRule.nType == COMPOUND_LEVELUP)
			{
				if(targetItem.GetLevelupType() != rule.nRuleTypePlus && targetItem.GetLevelupType() != -1)
				{
					continue;
				}//endif
				
			}//endif

			haveTargetItem = true;
		}
		if(!checkMaterial(initMaterial.sourItemIndex, rule))
		{
			continue;
		}
		
		ruleIndex = i;
		break;
 	}

 	if(ruleIndex == -1)
 	{
		if(initMaterial.targetItemIndex == COMMON_ITEM_INVALID_ID)
		{
			nRet = enchaser_error_no_fill_rule;
		}
		else
		{
			if(haveTargetItem)
			{
				nRet = enchaser_error_less_material;
			}
			else
			{
				nRet = enchaser_error_targetItem_invalid;
			}
		}
 	}
 	else
 	{
 		rRule = ruleList[ruleIndex];
 	}

 	return nRet;
}

#endif


KCompoundRules::KCompoundRuleEntry* KCompoundRules::getRuleById(int ruleType, int changeTimes, int ruleId)
{
	KCompoundRulesIndex idxRule;
	idxRule.nType = ruleType;
	idxRule.nChangeCount = changeTimes;
	KRuleTable& ruleList = GetRuleByIndex( idxRule );

	for(int i = 0; i < ruleList.size(); ++i)
	{
		if(ruleList[i].nRuleTypePlus == ruleId)
		{
			return &ruleList[i];
		}
	}
	
	return NULL;
}

KCompoundRules::KCompoundRuleEntry* KCompoundRules::getRuleByLiftItem(int ruleType, int changeTimes, ItemType& liftItem)
{
	KCompoundRulesIndex idxRule;
	idxRule.nType = ruleType;
	idxRule.nChangeCount = changeTimes;
	KRuleTable& ruleList = GetRuleByIndex( idxRule );

	for(int i = 0; i < ruleList.size(); ++i)
	{
		KSrcItemDescriptor& ruleLiftItem = ruleList[i].SrcItemsArray[0];
		if(ruleLiftItem.nItemGenre == liftItem.genre
			&& ruleLiftItem.nItemParticular == liftItem.particular
			&& ruleLiftItem.nItemDetail == liftItem.detail
			&& ruleLiftItem.nItemLevel == liftItem.level)
		{
			return &ruleList[i];
		}
	}
	
	return NULL;
}

int	KCompoundRules::GetPlusSuccessRate( TCompoundParams* pParams ,TCompoundDelResult * delResult )
{
	int nPlusRate		= 0;
	int nParamCount		= pParams->nTypeCount;
	int nPlusRateCount	= m_CompoundSuccessRatioTable.size();
	for ( int nParamIdx = 1; nParamIdx <= nParamCount; ++nParamIdx )
	{
		for ( int nPlusRateIdx = 0; nPlusRateIdx < nPlusRateCount; ++nPlusRateIdx )
		{
			KItem* pItem = pParams->pPartArray[nParamIdx].GetItem();
			if ( pItem && CheckItem( pItem, m_CompoundSuccessRatioTable[nPlusRateIdx].RateItem ) )
			{
				int nItemCount		= pItem->GetItemCount();

				if ( nItemCount >= 0 )
				{
					if ( 0 <= nItemCount && nItemCount < m_CompoundSuccessRatioTable[nPlusRateIdx].nCount0.nCount )
					{
						nPlusRate += 0;
					}
					else if ( m_CompoundSuccessRatioTable[nPlusRateIdx].nCount0.nCount <= nItemCount && nItemCount < m_CompoundSuccessRatioTable[nPlusRateIdx].nCount1.nCount )
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nCount0.nRate;
					}
					else if ( m_CompoundSuccessRatioTable[nPlusRateIdx].nCount1.nCount <= nItemCount && nItemCount < m_CompoundSuccessRatioTable[nPlusRateIdx].nCount2.nCount )
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nCount1.nRate;
					}
					else
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nCount2.nRate;
					}
				}

				int nItemQuality	= pItem->GetItemQuality();

				if ( nItemQuality >= 0 )
				{
					if ( 0 <= nItemQuality && nItemQuality < m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality0.nCount )
					{
						nPlusRate += 0;
					}
					else if ( m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality0.nCount <= nItemQuality && nItemQuality < m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality1.nCount )
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality0.nRate;
					}
					else if ( m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality1.nCount <= nItemQuality && nItemQuality <  m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality2.nCount )
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality1.nRate;
					}
					else if ( m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality2.nCount <= nItemQuality && nItemQuality <  m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality3.nCount )
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality2.nRate;
					}
					else
					{
						nPlusRate += m_CompoundSuccessRatioTable[nPlusRateIdx].nQuality3.nRate;
					}
				}

				delResult->m_nDelCount[nParamIdx] = nItemCount;
				return nPlusRate;
			}
		}
	}

	return nPlusRate;
}

void KCompoundRules::AddToRealParamList	( int nIdx, int nItemGenre, int nItemDetail, int nItemParticular, int nItemCount, int nItemQuality )
{
	int nHaskID = GenerateItemHashId( nItemGenre, nItemDetail, nItemParticular );
	m_CompRuleParamList[nHaskID].nItemGenre			= nItemGenre;
	m_CompRuleParamList[nHaskID].nItemDetail		= nItemDetail;
	m_CompRuleParamList[nHaskID].nItemParticular	= nItemParticular;
	m_CompRuleParamList[nHaskID].nItemCount			+= nItemCount;
	m_CompRuleParamList[nHaskID].nItemQuality		+= nItemQuality * nItemCount;
	m_CRPLIndex[nIdx]								= nHaskID;
}

void KCompoundRules::ClearRealParamList( void )
{
	m_CompRuleParamList.clear();
	ZeroMemory( m_CRPLIndex, sizeof(int)*MAX_COMPOUND );
}

bool KCompoundRules::CheckCount( const KRuleTable& ruleList,int nHitIdx )
{
	bool bRet = true;
	int x = 0;
	for ( x = 0; x < ruleList[nHitIdx].nSrcItemsCount; ++x)
	{
		if ( ruleList[nHitIdx].SrcItemsArray[x].nItemCount != -1 && m_CompRuleParamList[m_CRPLIndex[x]].nItemCount < ruleList[nHitIdx].SrcItemsArray[x].nItemCount )
		{
			bRet = false;
		}
		if ( ruleList[nHitIdx].SrcItemsArray[x].nItemQuality != -1 && m_CompRuleParamList[m_CRPLIndex[x]].nItemQuality < ruleList[nHitIdx].SrcItemsArray[x].nItemQuality )
		{
			bRet = false;
		}
	}
	return bRet;
}

bool	KCompoundRules::CheckQuality( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem	)
{
	bool bRet = true;
	if ( rDstItem.nItemQuality != -1)
	{
		if ( pSrcItem->GetItemQuality() != rDstItem.nItemQuality )
		{
			return false;
		}
	}
	return bRet;
}

bool	KCompoundRules::CheckYao( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem	)
{
	bool bRet = true;
	if ( rDstItem.nItemYao != -1)
	{
		if ( pSrcItem->GetYaoID() != rDstItem.nItemYao )
		{
			return false;
		}
	}
	return bRet;
}

bool	KCompoundRules::CheckColor( const KItem* pSrcItem, const KSrcItemDescriptor& rDstItem	)
{
	bool bRet = true;
	if ( rDstItem.nItemColor != -1)
	{
		if ( pSrcItem->GetQualityLabel() != rDstItem.nItemColor )
		{
			return false;
		}
	}
	return bRet;
}

