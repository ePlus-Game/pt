//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/03/2006 1:02
//      File_base        : KItemEnchaser
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#ifdef _SERVER
#include "KItem.h"
#include "KMath.h"
#include "KItemEnchaser.h"
#include "KCompoundRule.h"
#include "KPlayer.h"
#include "KSmithShop.h"



/************************************************************************/
/*						装备合成器									   */
/************************************************************************/
KItemEnchaser g_ItemEnchaser;

/************************************************************************/
/*                      合成道具的过程	                                */
/************************************************************************/
int KItemEnchaser::Compund( TCompoundParams * pCompundParams )
{
	if(!IsValidPlayer(pCompundParams->nPlayerIndex))
	{
		return enchaser_error_type;
	}

	TCompoundResult     compResult;
	TCompoundDelResult  delResult;

	int nResultMsg = g_CompoundRule.CheckRules( pCompundParams, &compResult ,&delResult);
	if ( pCompundParams->pPartArray[0].GetItem() )
	{
		m_TargetItem.nNewItemGenre0			= pCompundParams->pPartArray[0].GetItem()->GetGenre();
		m_TargetItem.nNewItemDetail0		= pCompundParams->pPartArray[0].GetItem()->GetDetailType();
		m_TargetItem.nNewItemParticular0	= pCompundParams->pPartArray[0].GetItem()->GetParticular();
		m_TargetItem.nNewItemLevel0			= pCompundParams->pPartArray[0].GetItem()->GetLevel();
	}

	//判断背包格子是否满——满就不让进行升级
	int x = 0;
	int y = 0;
	if (Player[pCompundParams->nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(&x, &y) == false)
	{	
		notifyClient(pCompundParams->nPlayerIndex, pCompundParams->nCompoundType, enchaser_error_not_enough_space);
	}

	if ( nResultMsg == enchaser_error_no )
	{
		int nSuccessRate0 = compResult.nSuccessRate0 + compResult.nSuccessPlusRate;
		int nSuccessRate1 = 0;
		if ( pCompundParams->nCompoundType ==  COMPOUND_SMITH)
		{
			nSuccessRate1 = compResult.nSuccessRate1;
		}
		else
		{
			nSuccessRate1 = compResult.nSuccessRate1 + compResult.nSuccessPlusRate;
		}
		
		int nDestroyRate = compResult.nDestroyRate;
		m_bOk0 = g_RandPercent( nSuccessRate0 ) ? TRUE : FALSE;
		m_bOk1 = g_RandPercent( nSuccessRate1 ) ? TRUE : FALSE;

		int nPlusRate = compResult.nPlusRate0;
		if ( m_bOk0 || pCompundParams->nCompoundType == COMPOUND_GETYAO )
		{
			switch( pCompundParams->nCompoundType )
			{
			case COMPOUND_LEVELUP:
				LevelUp( pCompundParams, &compResult );
				break;
			case COMPOUND_ADDMAGIC:
				AddMagic( pCompundParams, &compResult );
				break;
			case COMPOUND_CLEAR:
				Clear( pCompundParams, &compResult );
			    break;
			case COMPOUND_ADDYAO:
				AddYao( pCompundParams, &compResult );
			    break;
			case COMPOUND_GETYAO:
				GetYao( pCompundParams, &compResult );
			    break;
			case COMPOUND_MAKE:
				Make(  pCompundParams, &compResult  );
				break;
			default:
				return enchaser_error_type;
			    break;
			}
			DeleteItems( pCompundParams, true , &delResult );
			nResultMsg = enchaser_error_ratesuccess;
		}
		else if ( g_RandPercent(  nPlusRate ) )
		{
			if ( pCompundParams->nCompoundType == COMPOUND_LEVELUP )
			{
				LevelDown( pCompundParams, &compResult );
			}
			DeleteItems( pCompundParams, true , &delResult);
			nResultMsg = enchaser_error_level_down;
		}
		else
		{
			if ( g_RandPercent( nDestroyRate ) )
			{
				DeleteItems( pCompundParams, true , &delResult);
				nResultMsg = enchaser_error_ratedestroy;
			}
			else
			{
				DeleteItems( pCompundParams, false , &delResult);
				nResultMsg = enchaser_error_ratefailed;
			}
			
		}
		DeleteMoney( pCompundParams );
		DeleteSkillPoint( pCompundParams );

	}

	//!< 向客户端发送合成结果
	ENCHASER_SERVERRESULT			EnchaserResult;
	EnchaserResult.ProtocolType		= s2c_enchaseritemresult;
	EnchaserResult.nResult			= nResultMsg;
	EnchaserResult.compoundType		= pCompundParams->nCompoundType;
	EnchaserResult.NewItemID        = (int)compResult.nNewItemID0;
	
	int lnID						= Player[pCompundParams->nPlayerIndex].m_nNetConnectIdx;
	
	if ( lnID >= 0 )
	{
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(lnID, &EnchaserResult, sizeof(EnchaserResult));
	}

	return nResultMsg;
}

int KItemEnchaser::smith(TCompoundParams* pCompundParams, int ruleId)
{
	if(!IsValidPlayer(pCompundParams->nPlayerIndex))
	{
		return enchaser_error_conditionisinvalid;
	}
	
	//看是否已经在进行一个操作了
	if(Player[pCompundParams->nPlayerIndex].GetActionDelayer().HasAction())
	{
		notifyClient(pCompundParams->nPlayerIndex, COMPOUND_SMITH, enchaser_error_another_action);
		return enchaser_error_another_action;
	}

	int shopId = Player[pCompundParams->nPlayerIndex].m_BuyInfo.m_nSmithShopIdx;
	if(KSmithShop::getSinglton().isShopHaveTheRule(shopId, ruleId) == false)
	{
		notifyClient(pCompundParams->nPlayerIndex, COMPOUND_SMITH, enchaser_error_conditionisinvalid);
		return enchaser_error_conditionisinvalid;
	}

	//检查参数是否正确
	int nResultMsg = g_CompoundRule.checkParams(pCompundParams);
	if(nResultMsg != enchaser_error_no)
	{
		notifyClient(pCompundParams->nPlayerIndex, COMPOUND_SMITH, nResultMsg);
		return nResultMsg;
	}

	//检查是否满足合成规则，并得到根据该规则后计算出来的合成结果（删除或增加哪些物品以及个数）
	TCompoundResultEx* smithResult = NULL;
	smithResult = &Player[pCompundParams->nPlayerIndex].m_smithResult;
	smithResult->deleteItems.clear();
	smithResult->newItems.clear();
	nResultMsg = g_CompoundRule.CheckRules(pCompundParams, smithResult, ruleId);
	if(nResultMsg != enchaser_error_no)
	{
		notifyClient(pCompundParams->nPlayerIndex, COMPOUND_SMITH, nResultMsg);
		return nResultMsg;
	}
	
	//设置延迟回调，并开始读进度条
	DelayedAction smithAction(smithResult->playerIndex, delayed_action_smith, 3, delayed_action_msg_smith);
	smithAction.GetSmithParam().m_pSmithResult = (void*)smithResult;
	if (Player[smithResult->playerIndex].GetActionDelayer().NewAction(smithAction) == false)
	{
		notifyClient(pCompundParams->nPlayerIndex, COMPOUND_SMITH, enchaser_error_another_action);
		return enchaser_error_another_action;
	}

	return enchaser_error_no;
}

void KItemEnchaser::doSmith(TCompoundResultEx* smithResult)
{
	if(!smithResult)
	{
		return;
	}

	if(!IsValidPlayer(smithResult->playerIndex))
	{
		return;
	}

	//读完进度条后，再检查是否可以打造
	if(!checkResult(smithResult))
	{
		notifyClient(smithResult->playerIndex, COMPOUND_SMITH, enchaser_error_no_fill_rule);
		return;
	}

	int x = 0;
	int y = 0;
	if (Player[smithResult->playerIndex].m_ItemList.CheckCanPlaceInEquipment(&x, &y) == false)
	{	
		notifyClient(smithResult->playerIndex, COMPOUND_SMITH, enchaser_error_not_enough_space);
	}
	else
	{
		if(!AddItemToPlayer(smithResult))
		{
			notifyClient(smithResult->playerIndex, COMPOUND_SMITH, enchaser_error_no_fill_rule);
			return;
		}
		
		DeleteItems(smithResult);
		DeleteMoney(smithResult);
		DeleteSkillPoint(smithResult);
		notifyClient(smithResult->playerIndex, COMPOUND_SMITH, enchaser_error_no);

		//InstantSave
		//Player[smithResult->playerIndex].SaveItemData();
	}
}

void KItemEnchaser::notifyClient(int playerIndex, int compoundType, int result)
{
	ENCHASER_SERVERRESULT			EnchaserResult;
	EnchaserResult.ProtocolType		= s2c_enchaseritemresult;
	EnchaserResult.nResult			= result;
	EnchaserResult.compoundType		= compoundType;
	
	int lnID						= Player[playerIndex].m_nNetConnectIdx;
	if ( lnID >= 0 )
	{
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(lnID, &EnchaserResult, sizeof(EnchaserResult));
	}
}

void	KItemEnchaser::LevelUp( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	)
{
	pCompundResult->nNewItemGenre0		= m_TargetItem.nNewItemGenre0;
	pCompundResult->nNewItemDetail0		= m_TargetItem.nNewItemDetail0;
	pCompundResult->nNewItemParticular0  = m_TargetItem.nNewItemParticular0;
	pCompundResult->nNewItemLevel0		= m_TargetItem.nNewItemLevel0;
	AddCompItemToPlayer( COMPOUND_LEVELUP, *pCompundResult, pCompundParams );
}

void	KItemEnchaser::LevelDown( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult )
{
	pCompundResult->nNewItemGenre0		= m_TargetItem.nNewItemGenre0;
	pCompundResult->nNewItemDetail0		= m_TargetItem.nNewItemDetail0;
	pCompundResult->nNewItemParticular0  = m_TargetItem.nNewItemParticular0;
	pCompundResult->nNewItemLevel0		= m_TargetItem.nNewItemLevel0;
	int nPlayerIndex = pCompundParams->nPlayerIndex;
	int nItemIndex = ItemSet.Add( pCompundParams->pPartArray[0].GetItem() );

	if (nItemIndex > 0)
	{
		if ( pCompundResult->nPlusLevel0 > 0 )
		{
			Item[nItemIndex].AddCompoundBuff( COMPOUND_LEVELUP, pCompundResult->nRuleTypePlus, pCompundResult->nPlusBuffID0 );
			Item[nItemIndex].SetLevelupTimes( pCompundResult->nPlusLevel0 );
		}
		else
		{
			int nLevel = Item[nItemIndex].GetLevelupTimes();
			if ( nLevel > 0 )
			{
				if ( nLevel == 1 )
				{
					Item[nItemIndex].AddCompoundBuff( COMPOUND_LEVELUP, -1, 0 );
				}
				else
				{
					Item[nItemIndex].AddCompoundBuff( COMPOUND_LEVELUP, pCompundResult->nRuleTypePlus, pCompundResult->nPlusBuffID0 );
				}
				
				Item[nItemIndex].SetLevelupTimes( nLevel - 1 );
			}
		}
	
		int x = 0;
		int y = 0;
		if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(&x, &y))
		{
			if (Player[nPlayerIndex].GetItemList().Add(nItemIndex, pos_equiproom, x, y, NULL, item_sync_type_gain))
			{
				pCompundResult->nNewItemID0 = Item[nItemIndex].GetID();
				return;
			}//endif

		}

		ItemSet.Remove( nItemIndex );
	}
}

void	KItemEnchaser::AddMagic( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	)
{

	if ( m_bOk1 )
	{
		AddItemToPlayer( pCompundResult->nNewItemGenre1, pCompundResult->nNewItemDetail1, pCompundResult->nNewItemParticular1, pCompundResult->nNewItemLevel1, pCompundParams->nPlayerIndex ,NULL,&(pCompundResult->nNewItemID0));
		return;
	}
	else
	{
		AddItemToPlayer( pCompundResult->nNewItemGenre0, pCompundResult->nNewItemDetail0, pCompundResult->nNewItemParticular0, pCompundResult->nNewItemLevel0, pCompundParams->nPlayerIndex ,NULL,&(pCompundResult->nNewItemID0));
		return;
	}
}

void	KItemEnchaser::Clear( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	)
{
	//AddCompItemToPlayer( COMPOUND_CLEAR, *pCompundResult, pCompundParams->nPlayerIndex );
}

void	KItemEnchaser::AddYao( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	)
{
	pCompundResult->nNewItemGenre0		= m_TargetItem.nNewItemGenre0;
	pCompundResult->nNewItemDetail0		= m_TargetItem.nNewItemDetail0;
	pCompundResult->nNewItemParticular0  = m_TargetItem.nNewItemParticular0;
	pCompundResult->nNewItemLevel0		= m_TargetItem.nNewItemLevel0;
	AddCompItemToPlayer( COMPOUND_ADDYAO, *pCompundResult, pCompundParams );
}

void	KItemEnchaser::GetYao( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	)
{
	if ( m_bOk1 )
	{
		AddItemToPlayer( pCompundResult->nNewItemGenre1, pCompundResult->nNewItemDetail1, pCompundResult->nNewItemParticular1, pCompundResult->nNewItemLevel1, pCompundParams->nPlayerIndex );
		return;
	}
	else
	{
		AddItemToPlayer( pCompundResult->nNewItemGenre0, pCompundResult->nNewItemDetail0, pCompundResult->nNewItemParticular0, pCompundResult->nNewItemLevel0, pCompundParams->nPlayerIndex );
		return;
	}
}

void	KItemEnchaser::Make( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult )
{
	if ( m_bOk1 )
	{
		AddItemToPlayer( pCompundResult->nNewItemGenre1, pCompundResult->nNewItemDetail1, pCompundResult->nNewItemParticular1, pCompundResult->nNewItemLevel1, pCompundParams->nPlayerIndex, pCompundParams->szPlusInfo );		
	}
	else
	{
		AddItemToPlayer( pCompundResult->nNewItemGenre0, pCompundResult->nNewItemDetail0, pCompundResult->nNewItemParticular0, pCompundResult->nNewItemLevel0, pCompundParams->nPlayerIndex, pCompundParams->szPlusInfo );
	}	
}
#endif

