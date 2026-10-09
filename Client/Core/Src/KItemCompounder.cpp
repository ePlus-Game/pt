//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/03/2006 14:42
//      File_base        : KItemCompounder
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#ifdef _SERVER
#include "KItem.h"
#include "KPlayer.h"
#include "KItemCompounder.h"
#include "KSubWorldSet.h"
#include "KMath.h"


/************************************************************************/
/*						删除所有旧装备                                  */
/************************************************************************/
void KItemCompounder::DeleteItems(TCompoundParams* pCompundParams, bool bDestroySrcItem ,TCompoundDelResult * pAppResult)
{
	// 删除旧的装备
	if ( bDestroySrcItem )
	{
		KItem* pItem = pCompundParams->pPartArray[0].GetItem();
		if (pItem)
		{
			int nItemIndex = Player[pCompundParams->nPlayerIndex].m_ItemList.SearchID(pCompundParams->pPartArray[0].GetItem()->GetID());
			_ASSERT(nItemIndex);
			if(nItemIndex)
			{
				//记录日志 begin
				bool needRecord = false;
				LogEventParam logEventParam;
				if(Item[nItemIndex].GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
				{
					logEventParam.event = log_event_player_compound_del_item;
					logEventParam.param1 = Player[pCompundParams->nPlayerIndex].GetGUID();
					logEventParam.param2 = Item[nItemIndex].GetGUID();
					//Item[nItemIndex].GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
					Item[nItemIndex].PrintItemInfo( logEventParam.param3.data, sizeof( logEventParam.param3.data ) - 1 );
					logEventParam.param4 = Item[nItemIndex].GetItemCount();
					needRecord = true;
				}
				//记录日志 end
				
				if(Player[pCompundParams->nPlayerIndex].m_ItemList.Remove(nItemIndex))
				{
					//记录日志 begin
					if(needRecord)
					{
						g_pLogSystem->Log(logEventParam);
					}
					//记录日志 end
					ItemSet.Remove(nItemIndex);
				}
			}
		}
	}
	int i = 1;
	for (i = 1; i<= pCompundParams->nTypeCount; i++)
	{
		KItem* pItem = pCompundParams->pPartArray[i].GetItem();
		if (pItem)
		{
			int nItemIndex = Player[pCompundParams->nPlayerIndex].m_ItemList.SearchID(pCompundParams->pPartArray[i].GetItem()->GetID());
			_ASSERT(nItemIndex);
			if (nItemIndex)
			{		
				LogEventParam logEventParam;
				//记录日志 begin			
				bool needRecord = false;
				if(Item[nItemIndex].GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
				{
					logEventParam.event = log_event_player_compound_del_item;
					logEventParam.param1 = Player[pCompundParams->nPlayerIndex].GetGUID();
					logEventParam.param2 = Item[nItemIndex].GetGUID();
					//Item[nItemIndex].GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
					Item[nItemIndex].PrintItemInfo( logEventParam.param3.data, sizeof( logEventParam.param3.data ) - 1 );
					logEventParam.param4 = Item[nItemIndex].GetItemCount();
					needRecord = true;
				}
				//记录日志 end
				
				int nRemoveCount = 0;
				if ( i < MAX_LEVELUP_ITEMS_COUNT)
				{
					nRemoveCount = pAppResult->m_nDelCount[i];
				}//endif
				
				if (nRemoveCount == 0)
					continue;
				
				if( Item[nItemIndex].GetItemCount() > nRemoveCount )
				{
					int originalDestWeight = Item[nItemIndex].GetItemWeight();
					Item[nItemIndex].SetItemCount(Item[nItemIndex].GetItemCount() - nRemoveCount);
					Player[pCompundParams->nPlayerIndex].GetItemList().m_Room[room_equipment]
						.AddWeight(Item[nItemIndex].GetItemWeight() - originalDestWeight);
					
					Player[pCompundParams->nPlayerIndex].CheckWeight();
					//刷新客户端个数
					Item[nItemIndex].SyncItemRefresh( Player[pCompundParams->nPlayerIndex].m_nNetConnectIdx );
					
					if(needRecord)
					{
						g_pLogSystem->Log(logEventParam);
					}//endif
				}
				else
				{
					if(Player[pCompundParams->nPlayerIndex].m_ItemList.Remove(nItemIndex))
					{
						//记录日志 begin
						if(needRecord)
						{
							g_pLogSystem->Log(logEventParam);
						}
						//记录日志 end
						
						ItemSet.Remove(nItemIndex);
					}
					
				}//end else
			}
		}
	}
}

void KItemCompounder::DeleteItems(TCompoundResultEx* compResult)
{
	for(int i = 0; i < compResult->deleteItems.size(); ++i)
	{
		TCompoundResultEx::deleteItemInfo& deleteItem = compResult->deleteItems[i];
		int itemIndex = Player[compResult->playerIndex].m_ItemList.SearchID(deleteItem.itemId);
		_ASSERT(itemIndex);
		if (itemIndex)
		{			
			//记录日志 begin
			bool needRecord = false;
			LogEventParam logEventParam;
			if(Item[itemIndex].GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
			{
				logEventParam.event = log_event_player_smith_del_item;
				logEventParam.param1 = Player[compResult->playerIndex].GetGUID();
				logEventParam.param2 = Item[itemIndex].GetGUID();
				//Item[itemIndex].GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
				Item[itemIndex].PrintItemInfo( logEventParam.param3.data, sizeof( logEventParam.param3.data ) - 1 );
				logEventParam.param4 = deleteItem.deleteCount;
				needRecord = true;
			}
			//记录日志 end
			
			if(Item[itemIndex].GetItemCount() > deleteItem.deleteCount)
			{
				int originalDestWeight = Item[itemIndex].GetItemWeight();
				Item[itemIndex].SetItemCount(Item[itemIndex].GetItemCount() - deleteItem.deleteCount);
				Player[compResult->playerIndex].GetItemList().m_Room[room_equipment]
					.AddWeight(Item[itemIndex].GetItemWeight() - originalDestWeight);
				Player[compResult->playerIndex].CheckWeight();
				//刷新客户端个数
				Item[itemIndex].SyncItemRefresh( Player[compResult->playerIndex].m_nNetConnectIdx );
					
				if(needRecord)
				{
					g_pLogSystem->Log(logEventParam);
				}
			}
			else
			{
				if(Player[compResult->playerIndex].m_ItemList.Remove(itemIndex))
				{
					if(needRecord)
					{
						g_pLogSystem->Log(logEventParam);
					}
					ItemSet.Remove(itemIndex);
				}
			}
		}
	}
}
/************************************************************************/
/*								删除钱                                  */
/************************************************************************/
void KItemCompounder::DeleteMoney( TCompoundParams * pCompundParams )
{
	// 删除旧的装备
	if ( pCompundParams )
	{
		if(Player[pCompundParams->nPlayerIndex].Pay( pCompundParams->nMoney ))
		{
			//记录日志 begin
			if (pCompundParams->nMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logEventParam;
				logEventParam.event = log_event_player_compound_del_money;
				logEventParam.param1 = Player[pCompundParams->nPlayerIndex].GetGUID();
				logEventParam.param4 = -pCompundParams->nMoney;
				g_pLogSystem->Log(logEventParam);
			}
			//记录日志 end
		}
	}
}

void KItemCompounder::DeleteMoney( TCompoundResultEx* compResult )
{
	// 删除旧的装备
	if ( compResult )
	{
		if(Player[compResult->playerIndex].Pay( compResult->reduceMoney ))
		{			
			//记录日志 begin
			if (compResult->reduceMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logEventParam;
				logEventParam.event = log_event_player_smith_del_money;
				logEventParam.param1 = Player[compResult->playerIndex].GetGUID();
				logEventParam.param4 = -compResult->reduceMoney;
				g_pLogSystem->Log(logEventParam);
			}
			//记录日志 end
		}
	}
}
/************************************************************************/
/*								消耗技能点                              */
/************************************************************************/
void KItemCompounder::DeleteSkillPoint( TCompoundParams * pCompundParams )
{
	// 消耗技能点
	if ( pCompundParams )
	{
		DWORD skillExp = Player[pCompundParams->nPlayerIndex].GetSkillExp();
		skillExp -= pCompundParams->nSkillPoint;
		Player[pCompundParams->nPlayerIndex].SetSkillExp(skillExp);
		Player[pCompundParams->nPlayerIndex].SyncAttribute(attr_SkillExp);
	}
}

void KItemCompounder::DeleteSkillPoint( TCompoundResultEx* compResult )
{
	// 消耗技能点
	if ( compResult )
	{
		DWORD oldSkill = Player[compResult->playerIndex].GetSkillExp();
		Player[compResult->playerIndex].SetSkillExp(oldSkill - compResult->reduceSkillPoint);
		Player[compResult->playerIndex].SyncAttribute(attr_SkillExp);
	}
}
/************************************************************************/
/*                      增加新装备到玩家                                */
/************************************************************************/
void KItemCompounder::AddItemToPlayer( int nGenre, int nDetail, int nParaticular, int nLevel, int nPlayerIndex,  char* szPlusInfo,DWORD * pResultItemID)
{
	int nItemIndex = ItemSet.Add( nGenre, nDetail, nParaticular, nLevel,  1);
	if (nItemIndex > 0)
	{
		//记录日志 begin
		bool neetRecord = false;
		LogEventParam logEventParam;
		if(Item[nItemIndex].GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
		{
			logEventParam.event = log_event_player_compound_add_item;
			logEventParam.param1 = Player[nPlayerIndex].GetGUID();
			logEventParam.param2 = Item[nItemIndex].GetGUID();
			//Item[nItemIndex].GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
			Item[nItemIndex].PrintItemInfo( logEventParam.param3.data, sizeof( logEventParam.param3.data ) - 1 );
			logEventParam.param4 = Item[nItemIndex].GetItemCount();
			neetRecord = true;
		}
		//记录日志 end

		int x = 0;
		int y = 0;
		if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(&x, &y))
		{
			if ( szPlusInfo )
			{
				Item[nItemIndex].SetPlusInfo( szPlusInfo );
			}
			
			if (Player[nPlayerIndex].m_ItemList.Add(nItemIndex, pos_equiproom, x, y, NULL, item_sync_type_gain))
			{
				//记录日志 begin
				if(neetRecord)
				{
					g_pLogSystem->Log(logEventParam);
				}
				//记录日志 end

				if (pResultItemID)
					*pResultItemID = Item[nItemIndex].GetID();

				return;
			}
		}

		ItemSet.Remove( nItemIndex );
	}
}

bool KItemCompounder::checkResult(TCompoundResultEx* compResult)
{
	if(NULL == compResult)
	{
		return false;
	}

	//检查要删除的东西是否还在
	for(int i = 0; i < compResult->deleteItems.size(); ++i)
	{
		TCompoundResultEx::deleteItemInfo& deleteItem = compResult->deleteItems[i];
		int itemIndex = Player[compResult->playerIndex].m_ItemList.SearchID(deleteItem.itemId);

		_ASSERT(itemIndex);
		if(!itemIndex)
		{
			return false;
		}

		if(!Player[compResult->playerIndex].m_ItemList.HaveNormalItemByID(deleteItem.itemId))
		{
			return false;
		}

		if ( Item[itemIndex].GetItemCount() < deleteItem.deleteCount )
			return false;

		//是否锁定
		if (Item[itemIndex].IsLocked(Player[compResult->playerIndex].GetNetConnectIdx(), false))
		{
			return false;
		}
	}

	//检查蕴魂
	DWORD oldSkill = Player[compResult->playerIndex].GetSkillExp();
	if(oldSkill < compResult->reduceSkillPoint)
	{
		return false;
	}

	//检查金钱
	int holdMoney = Player[compResult->playerIndex].GetItemList().GetMoney(room_equipment);
	if(holdMoney < compResult->reduceMoney)
	{
		return false;
	}

	return true;
}

bool KItemCompounder::AddItemToPlayer(TCompoundResultEx* compResult)
{
	if(compResult->newItems.size() <= 0)
	{
		return false;
	}

	TCompoundResultEx::genItemInfo* newItem = NULL;
	
	int index = 0;
	//投点区间
	vector<int> roll;
	int rage = 0;
	for(index = 0; index < compResult->newItems.size(); ++index)
	{
		rage += compResult->newItems[index].successRate;
		roll.push_back(rage);
	}

	if(roll.size() <= 0)
	{
		return false;
	}
	int ret = g_Random(roll[roll.size() - 1]);
	for(int i = 0; i < roll.size(); ++i)
	{
		int left = i <= 0 ? 0 : roll[i - 1];
		int right = roll[i];
		if(ret <= right && ret >= left)
		{
			newItem = &compResult->newItems[i];
			break;
		}
	}

	if(!newItem)
	{
		return false;
	}
		
	int x = 0;
	int y = 0;

	int nItemIndex = ItemSet.Add( newItem->genre, newItem->detail, newItem->particular, newItem->level, 1);
	if(!nItemIndex)
	{
		return false;
	}

	if ( !Item[nItemIndex].IsBind() && compResult->bBind)
	{
		Item[nItemIndex].SetBind(true);
			}
	
	//记录日志 begin
	LogEventParam logEventParam;
	bool needRecord = false;
	if(Item[nItemIndex].GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
	{
		logEventParam.event = log_event_player_smith_add_item;
		logEventParam.param1 = Player[compResult->playerIndex].GetGUID();
		logEventParam.param2 = Item[nItemIndex].GetGUID();
		//Item[nItemIndex].GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
		Item[nItemIndex].PrintItemInfo( logEventParam.param3.data, sizeof( logEventParam.param3.data ) - 1 );
		logEventParam.param4 = Item[nItemIndex].GetItemCount();
		needRecord = true;
	}
	//记录日志 end

	if(!Player[compResult->playerIndex].m_ItemList.Add(nItemIndex, item_sync_type_gain))
	{
		ItemSet.Remove(nItemIndex);
		return false;
	}
	
	//记录日志 begin
	if(needRecord)
	{
		g_pLogSystem->Log(logEventParam);
	}
	//记录日志 end
				
	return true;
}
/************************************************************************/
/*                      增加新装备到玩家                                */
/************************************************************************/
void KItemCompounder::AddCompItemToPlayer( int nCompoundType, TCompoundResult& rResult, TCompoundParams* pCompundParams )
{
	int nPlayerIndex = pCompundParams->nPlayerIndex;
	if (!IsValidPlayer(nPlayerIndex))
		return ;

	int nItemIndex = ItemSet.Add( pCompundParams->pPartArray[0].GetItem() );
	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM)
		return ;
	
	//记录日志 begin
	bool needRecord = false;
	LogEventParam logEventParam;
	if(Item[nItemIndex].GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
	{
		logEventParam.event = log_event_player_compound_add_item;
		logEventParam.param1 = Player[pCompundParams->nPlayerIndex].GetGUID();
		logEventParam.param2 = Item[nItemIndex].GetGUID();
		//Item[nItemIndex].GetItemTemplateId(logEventParam.param3.data, sizeof(logEventParam.param3.data) - 1);
		Item[nItemIndex].PrintItemInfo( logEventParam.param3.data, sizeof( logEventParam.param3.data ) - 1 );
		logEventParam.param4 = Item[nItemIndex].GetItemCount();
		needRecord = true;
	}
	//记录日志 end

	if (nItemIndex > 0)
	{
		if ( nCompoundType != COMPOUND_ADDYAO )
		{
			Item[nItemIndex].AddCompoundBuff( nCompoundType, rResult.nRuleTypePlus, rResult.nBuffID );
		}
		else
		{
			switch( rResult.nBuffID )
			{
			case -1:
				Item[nItemIndex].SetYaoID( yao_yin );
				break;
			case -2:
				Item[nItemIndex].SetYaoID( yao_yang );
				break;
			}
		}
		if ( pCompundParams->szPlusInfo )
		{
			Item[nItemIndex].SetPlusInfo( pCompundParams->szPlusInfo );
		}
		int x = 0;
		int y = 0;
		if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(&x, &y))
		{
			if (Player[nPlayerIndex].GetItemList().Add(nItemIndex, pos_equiproom, x, y, NULL, item_sync_type_gain))
			{	
				//记录日志 begin
				if(needRecord)
				{
					g_pLogSystem->Log(logEventParam);
				}
				//记录日志 end

				rResult.nNewItemID0 = Item[nItemIndex].GetID();

				return;
			}
		}

		ItemSet.Remove( nItemIndex );
	}
}

#endif
