//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-22
//      File_base        : talisman_manager
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 法宝管理器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KMath.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KPlayer.h"
#include "buff_tab.h"
#include "CoreRelated.h"
#include "GameDataDef.h"
#include "ItemCommonDef.h"
#include "ConfigManager.h"
#include "talisman_manager.h"
#include "KItemGenerator.h"

bool checkItemIndex(int itemIndex)
{
	if (itemIndex <= 0 || itemIndex >= MAX_ITEM)
	{
		return false;
	}

	return true;
}

TalismanManager& TalismanManager::Singleton()
{
	static TalismanManager manager;
	return manager;
}

TalismanManager::TalismanManager()
{
}

TalismanManager::~TalismanManager()
{
}

bool TalismanManager::Load()
{
	bool sucess = true;
	m_EnchaseDatas.clear();

	KTabFile enchaseTabFile;
	if (TRUE == enchaseTabFile.Load(TALISMAN_ENCHASE_TABLE_FILE))
	{
		int row = 0;
		int recordCount = enchaseTabFile.GetHeight() - 1;
		
		EnchaseData tempData;
		m_EnchaseDatas.resize(recordCount, tempData);
		
		for (int record = 0; record < recordCount; ++record)
		{
			row = record + 2;
			EnchaseData& enchaseData = m_EnchaseDatas[record];
			
			int field = 1;			
			
			if (FALSE == enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.Id)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.Group)))
			{
				sucess = false;
			}
			++field;

#ifndef _SERVER			
			if (FALSE == enchaseTabFile.GetString(row, field, "", enchaseData.Name, sizeof(enchaseData.Name)))
			{
				sucess = false;
			}
#endif
			++field;

#ifndef _SERVER
			if (FALSE == enchaseTabFile.GetString(row, field, "", enchaseData.Desc, sizeof(enchaseData.Desc)))
			{
				sucess = false;
			}
#endif
			++field;

#ifndef _SERVER
			if (FALSE == enchaseTabFile.GetString(row, field, "", enchaseData.ImageSetName, sizeof(enchaseData.ImageSetName)))
			{
				sucess = false;
			}
#endif
			++field;

#ifndef _SERVER
			if (FALSE == enchaseTabFile.GetString(row, field, "", enchaseData.ImageName, sizeof(enchaseData.ImageName)))
			{
				sucess = false;
			}			
#endif
			++field;

#ifndef _SERVER
			if (FALSE == enchaseTabFile.GetString(row, field, "", enchaseData.MiniImageSetName, sizeof(enchaseData.MiniImageSetName)))
			{
				sucess = false;
			}
#endif
			++field;

#ifndef _SERVER
			if (FALSE == enchaseTabFile.GetString(row, field, "", enchaseData.MiniImageName, sizeof(enchaseData.MiniImageName)))
			{
				sucess = false;
			}			
#endif		
			++field;

			if (FALSE == enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.RequireGroupId)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.RequireGroupCount)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.RequireId)))
			{
				sucess = false;
			}
			++field;

			if (FALSE == enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.RequireCount)))
			{
				sucess = false;
			}
			++field;

			enchaseData.MaxEnchaseCount = 0;
			for (int enchaseCountLoopCount = 0; enchaseCountLoopCount < MAX_ENCHASE_COUNT_PER_TALISMAN; enchaseCountLoopCount++)
			{
				bool allow = false;
				for (int enchaseBuffLoopCount = 0; enchaseBuffLoopCount < MAX_ENCHASE_BUFF_COUNT; enchaseBuffLoopCount++)
				{
					enchaseTabFile.GetInteger(row, field, 0, &(enchaseData.BuffSet[enchaseCountLoopCount][enchaseBuffLoopCount]));
					if (enchaseData.BuffSet[enchaseCountLoopCount][enchaseBuffLoopCount] > 0)
						allow = true;
					++field;
				}

				if (allow)
				{
					enchaseData.MaxEnchaseCount++;
				}
			}

			if (!sucess)
			{
				break;
			}
		}
	}

	_ASSERT(sucess);

	return sucess;
}

const PEnchaseData TalismanManager::GetEnchaseData(int enchaseId)
{
	EnchaseDataArray::iterator iterCurr = m_EnchaseDatas.begin();
	EnchaseDataArray::iterator iterEnd = m_EnchaseDatas.end();

	while (iterCurr != iterEnd)
	{
		if ((*iterCurr).Id == enchaseId)
		{
			return &(*iterCurr);
		}

		++iterCurr;
	}
	
	return NULL;
}

bool TalismanManager::IsValidEnchasePos(int enchasePos)
{
	if (enchasePos >= 0 && enchasePos < TM_HOLE_NUM)
		return true;
	else
		return false;
}

bool TalismanManager::IsValidTalisman(int itemIndex)
{
	if(!checkItemIndex(itemIndex))
	{
		return false;
	}

	KItem& item = Item[itemIndex];

	return IsValidTalisman(item);
}

bool TalismanManager::IsValidTalisman(KItem& item)
{
	if(!item.GetItemTemplate())
	{
		return false;
	}
#ifdef _SERVER
	if (item.GetID() <= 0)
	{
		return false;
	}
#endif
	if (item.GetGenre() != item_equip || item.GetDetailType() != equip_talisman)
	{
		return false;
	}

	return true;
}

bool TalismanManager::IsValidEnchaseItem(int itemIndex)
{
	if(!checkItemIndex(itemIndex))
	{
		return false;
	}

	KItem& item = Item[itemIndex];
	return IsValidEnchaseItem(item);
}

bool TalismanManager::IsValidEnchaseItem(KItem& item)
{
	if(!item.GetItemTemplate())
	{
		return false;
	}
#ifdef _SERVER
	if (item.GetID() <= 0)
	{
		return false;
	}
#endif
	if (item.GetGenre() != item_enchase)
	{
		return false;
	}

	return true;
}

bool TalismanManager::IsReachTopLevel(KItem& item)
{
	int currentLevel = item.GetTalismanLevel();
	if (currentLevel < MAX_TALISMAN_LEVEL)
	{
		const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate(item.GetGenre(), item.GetDetailType(), item.GetParticular(), currentLevel + 1);
		if ( !pItemTemplate )
		{
			return true;
		}
	}
	else
	{
		return true;
	}
	
	return false;
}

bool TalismanManager::IsUpgradeable(int itemIndex)
{
	if (IsValidTalisman(itemIndex))
	{
		KItem& talisman = Item[itemIndex];
		int currentLevel = talisman.GetTalismanLevel();
		if (!IsReachTopLevel(talisman))
		{
			if (talisman.GetTalismanPotential() == talisman.GetTalismanPotentialLimit())
			{
				return true;
			}
		}
	}
	
	return false;
}

bool TalismanManager::IsEditable(int itemIndex, int playerIndex)
{
	if (IsValidTalisman(itemIndex) && IsValidPlayer(playerIndex))
	{
		ItemPos itemPos;
		if (TRUE == Player[playerIndex].GetItemList().GetItemPos(itemIndex, &itemPos))
		{
			if (pos_equiproom == itemPos.nPlace)
			{
				return true;
			}
		}
	}

	return false;
}

void TalismanManager::CountTalismanEnchase(int talismanIndex, EnchaseCountInfo& enchaseCountInfo)
{	
	if(!checkItemIndex(talismanIndex))
	{
		return;
	}
	KItem& talisman = Item[talismanIndex];
	CountTalismanEnchase(talisman, enchaseCountInfo);
}

void TalismanManager::CountTalismanEnchase(KItem& talisman, EnchaseCountInfo& enchaseCountInfo)
{
	if (TalismanManager::Singleton().IsValidTalisman(talisman))
	{
		int enchaseLevelCount = TM_HOLE_NUM / TALISMAN_ENCHASE_PER_LEVEL;
		
		for (int enchaseLevelLoopCount = 0; enchaseLevelLoopCount < enchaseLevelCount; enchaseLevelLoopCount++)
		{
			for (int enchaseInLevelLoopCount = 0; enchaseInLevelLoopCount < TALISMAN_ENCHASE_PER_LEVEL; enchaseInLevelLoopCount++)
			{
				int enchasePos = (enchaseLevelLoopCount * TALISMAN_ENCHASE_PER_LEVEL) + enchaseInLevelLoopCount;
				int enchaseId = talisman.GetTalismanEnchase(enchasePos);
				if (enchaseId > 0)
				{
					const PEnchaseData pEnchaseData = TalismanManager::Singleton().GetEnchaseData(enchaseId);
					if (pEnchaseData != NULL)
					{
						for (int i = 0; i< TM_HOLE_NUM; i++)
						{
							EnchaseCount& enchaseCount = enchaseCountInfo.enchaseCount[enchaseLevelLoopCount][i];
							
							int id = enchaseCount.EnchaseId;
							if (id == 0)
							{
								enchaseCount.EnchaseId = enchaseId;
								enchaseCount.Count = 1;
								break;
							}
							else if (id == enchaseId)
							{
								enchaseCount.Count++;
								break;
							}
						}
						
						int enchaseGroupId = pEnchaseData->Group;
						for (int j = 0; j< TM_HOLE_NUM; j++)
						{
							EnchaseGroupCount& enchaseGroupCount = enchaseCountInfo.enchaseGroupCount[enchaseLevelLoopCount][j];
							
							int groupId = enchaseGroupCount.EnchaseGroupId;
							if (groupId == 0)
							{
								enchaseGroupCount.EnchaseGroupId = enchaseGroupId;
								enchaseGroupCount.Count = 1;
								break;
							}
							else if (groupId == enchaseGroupId)
							{
								enchaseGroupCount.Count++;
								break;
							}
						}
					}
				}
			}
			
			//复制上一层的统计数据
			if (enchaseLevelLoopCount + 1 < enchaseLevelCount)
			{
				memcpy(&(enchaseCountInfo.enchaseCount[enchaseLevelLoopCount + 1]), &(enchaseCountInfo.enchaseCount[enchaseLevelLoopCount]), sizeof(EnchaseCount) * TM_HOLE_NUM);
				memcpy(&(enchaseCountInfo.enchaseGroupCount[enchaseLevelLoopCount + 1]), &(enchaseCountInfo.enchaseGroupCount[enchaseLevelLoopCount]), sizeof(EnchaseGroupCount) * TM_HOLE_NUM);
			}
		}
	}
}

void TalismanManager::GetEnchaseActiveInfo(int talismanIndex, EnchaseActiveInfo& enchaseActiveInfo)
{
	if(!checkItemIndex(talismanIndex))
	{
		return;
	}
	KItem& talisman = Item[talismanIndex];
	GetEnchaseActiveInfo(talisman, enchaseActiveInfo);
}

void TalismanManager::GetEnchaseActiveInfo(KItem& talisman, EnchaseActiveInfo& enchaseActiveInfo)
{
	if (!IsValidTalisman(talisman))
		return;

	TalismanManager& tm = TalismanManager::Singleton();

	EnchaseCountInfo enchaseCountInfo;
	CountTalismanEnchase(talisman, enchaseCountInfo);
	
	for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
	{
		int enchaseId = talisman.GetTalismanEnchase(enchaseLoopCount);
		if (enchaseId > 0)
		{
			const PEnchaseData pEnchaseData = tm.GetEnchaseData(enchaseId);
			if (pEnchaseData != NULL)
			{
				int level = enchaseLoopCount / TALISMAN_ENCHASE_PER_LEVEL;
				
				bool matchEnchaseGroupRequirements = false;
				if (pEnchaseData->RequireGroupId > 0)
				{
					if (level > 0)
					{
						for (int i = 0; i < TM_HOLE_NUM; i++)
						{
							EnchaseGroupCount& enchaseGroupCount = enchaseCountInfo.enchaseGroupCount[level - 1][i];
							
							if (enchaseGroupCount.EnchaseGroupId == pEnchaseData->RequireGroupId)
							{
								if (enchaseGroupCount.Count >= pEnchaseData->RequireGroupCount)
								{
									matchEnchaseGroupRequirements = true;
								}
								break;
							}
						}
					}
				}
				else
				{
					matchEnchaseGroupRequirements = true;
				}
				
				bool matchEnchaseRequirements = false;
				if (pEnchaseData->RequireId > 0)
				{
					if (level > 0)
					{
						for (int i = 0; i < TM_HOLE_NUM; i++)
						{
							EnchaseCount& enchaseCount = enchaseCountInfo.enchaseCount[level - 1][i];
							
							if (enchaseCount.EnchaseId == pEnchaseData->RequireId)
							{
								if (enchaseCount.Count >= pEnchaseData->RequireCount)
								{
									matchEnchaseRequirements = true;
								}
								break;
							}
						}
					}
				}
				else
				{
					matchEnchaseRequirements = true;
				}
				
				if (matchEnchaseRequirements && matchEnchaseGroupRequirements)
				{
					enchaseActiveInfo.IsActive[enchaseLoopCount] = true;
					
					for (int i = 0; i < TM_HOLE_NUM; i++)
					{
						EnchaseCount& count = enchaseActiveInfo.CountInfo[i];
						if (count.EnchaseId == 0)
						{
							count.EnchaseId = enchaseId;
							count.Count = 1;
							break;
						}
						else if (count.EnchaseId == enchaseId)
						{
							count.Count++;
							break;
						}
					}
				}
			}				
		}	
	}
}

#ifdef _SERVER

void TalismanManager::ServerProcessProtocol(int playerIndex, BYTE *pMsg, int nSize)
{
	PTALISMAN_OPERATION pTalismanOperation = (PTALISMAN_OPERATION)pMsg;
	int talismanId = pTalismanOperation->Params[0];
	TalismanOperationResult operationResult = talisman_result_failure;

	TALISMAN_OPERATION serverOperation;
	serverOperation.Protocol = s2c_talisman_family;
	serverOperation.SubProtocol = pTalismanOperation->SubProtocol;

	switch(pTalismanOperation->SubProtocol)
	{
	case talisman_protocol_upgrade:
		{
			int talismanIndex = ItemSet.SearchID(talismanId);
			int upgradedTalismanIndex = 0;
			int upgradedTalismanId = talismanId;
			operationResult = Upgrade(playerIndex, talismanIndex, upgradedTalismanIndex);
			if (talisman_result_success == operationResult)
			{
				upgradedTalismanId = Item[upgradedTalismanIndex].GetID();
			}
						
			serverOperation.Params[0] = talismanId;			
			serverOperation.Params[1] = upgradedTalismanId;
			serverOperation.Params[2] = operationResult;
		}
		break;
	case talisman_protocol_enchase:
		{
			int enchasePos = pTalismanOperation->Params[1];
			int enchaseItemId = pTalismanOperation->Params[2];
			int talismanIndex = ItemSet.SearchID(talismanId);
			int enchaseItemIndex = ItemSet.SearchID(enchaseItemId);
			operationResult = Enchase(playerIndex, talismanIndex, enchasePos, enchaseItemIndex);

			serverOperation.Params[0] = talismanId;
			serverOperation.Params[1] = operationResult;
		}
		break;
	case talisman_protocol_convert_skill_exp:
		{
			int talismanIndex = ItemSet.SearchID(talismanId);
			int avaiableSkillExp = pTalismanOperation->Params[1];
			operationResult = ConvertSkillExpToTalismanPotential(playerIndex, talismanIndex, avaiableSkillExp);

			serverOperation.Params[0] = talismanId;
			serverOperation.Params[1] = operationResult;
		}
		break;
	default:
		break;
	}

	//把操作结果发给客户端
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[playerIndex].GetNetConnectIdx(), &serverOperation, sizeof(serverOperation));
}

TalismanOperationResult TalismanManager::Upgrade(int playerIndex, int itemIndex, int& upgradedItemIndex)
{
	upgradedItemIndex = 0;

	if (!IsValidPlayer(playerIndex))
	{
		return talisman_result_invalid_player;
	}

	if (!IsValidTalisman(itemIndex))
	{
		return talisman_result_invalid_talisman;
	}

	if (!IsUpgradeable(itemIndex))
	{
		return talisman_result_can_not_upgrade;
	}

	if (!IsEditable(itemIndex, playerIndex))
	{
		return talisman_result_can_not_edit;
	}

	KItem& item = Item[itemIndex];
	int currentLevel = item.GetTalismanLevel();
	
	//生成新的法宝
	int newTalismanIndex = ItemSet.Add(item.GetGenre(), item.GetDetailType(), item.GetParticular(), currentLevel + 1, 1);
	if (newTalismanIndex <= 0)
	{
		return talisman_result_failure;
	}
	
	KItem& newItem = Item[newTalismanIndex];	

	//复制原有镶嵌
	for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
	{
		newItem.SetTalismanEnchase(enchaseLoopCount, item.GetTalismanEnchase(enchaseLoopCount));
	}

	//产生新的镶嵌槽位
	int talismanQuality = newItem.GetItemQuality();
	if (talismanQuality < 0 || talismanQuality > 100)
	{
		//表填错了
		_ASSERT(false);
		return talisman_result_failure;
	}
	int newEnchaseCount = 0;
	int newLevelEnchaseIndex = currentLevel * TALISMAN_ENCHASE_PER_LEVEL;
	for (int newEnchaseLoopCount = 0; newEnchaseLoopCount < TALISMAN_ENCHASE_PER_LEVEL; newEnchaseLoopCount++)
	{
		if (g_RandPercent(talismanQuality))
		{
			newItem.SetTalismanEnchase(newLevelEnchaseIndex + newEnchaseCount, 0);//开启这个镶嵌槽
			newEnchaseCount++;
		}
	}

	if (newEnchaseCount == 0)//每层至少开启1个镶嵌槽
	{
		newItem.SetTalismanEnchase(newLevelEnchaseIndex, 0);
		newEnchaseCount = 1;
	}
	
	KPlayer& player = Player[playerIndex];
	ItemPos itemPos;

	//删除玩家原有的法宝，在原位置添加升级后的法宝
	if (TRUE == player.GetItemList().GetItemPos(itemIndex, &itemPos))
	{
		if (TRUE == player.GetItemList().Remove(itemIndex))
		{
			ItemSet.Remove(itemIndex);
			if (player.GetItemList().Add(newTalismanIndex, itemPos.nPlace, itemPos.nX, itemPos.nY) > 0)
			{
				upgradedItemIndex = newTalismanIndex;
				return talisman_result_success;
			}
			else
			{
				//TODO 删除了玩家身上的装备，却没有成功添加升级后的装备，需要记录日志，对玩家进行补偿
				_ASSERT(false);
			}
		}		
	}

	ItemSet.Remove(newTalismanIndex);
	return talisman_result_failure;
}

TalismanOperationResult TalismanManager::Enchase(int playerIndex, int talismanItemIndex, int enchasePos, int enchaseItemIndex)
{
	if (!IsValidPlayer(playerIndex))
	{
		return talisman_result_invalid_player;
	}
	
	if (!IsValidTalisman(talismanItemIndex))
	{
		return talisman_result_invalid_talisman;
	}

	if (!IsValidEnchaseItem(enchaseItemIndex))
	{
		return talisman_result_invalid_enchase;
	}

	if (!IsEditable(talismanItemIndex, playerIndex))
	{
		return talisman_result_can_not_edit;
	}
	
	KItem& talisman = Item[talismanItemIndex];
	KItem& enchase = Item[enchaseItemIndex];
	KPlayer& player = Player[playerIndex];
	KItemList& itemList = player.GetItemList();

	//TODO 这个函数的使用有待考虑
	if (!HasItemInEquipment(playerIndex, enchase.GetID()))
	{
		//内丹不属于这个玩家
		_ASSERT(false);
		return talisman_result_failure;
	}

	if (enchasePos < 0 || enchasePos >= TM_HOLE_NUM)
	{
		_ASSERT(false);
		return talisman_result_failure;
	}
	
	//要求内丹没有等级限制（==0） 或者 内丹等级和镶嵌孔等级一致
	int enchasePosLevel = (enchasePos / TALISMAN_ENCHASE_PER_LEVEL) + 1;
	int enchaseLevel = enchase.GetEnchaseLevel();
	if (enchaseLevel > 0 && enchaseLevel != enchasePosLevel)
	{
		_ASSERT(false);
		return talisman_result_level_not_match;
	}
    
	int enchaseId = enchase.GetEnchaseId();
	if (enchaseId < 0)
	{
		_ASSERT(false);
		return talisman_result_failure;
	}

	//单个内丹最大镶嵌数量限制
	if (enchaseId > 0)
	{
		const PEnchaseData pEnchaseData = GetEnchaseData(enchaseId);
		if (pEnchaseData)
		{
			EnchaseActiveInfo enchaseActiveInfo;
			GetEnchaseActiveInfo(talismanItemIndex, enchaseActiveInfo);
			for (int i = 0; i < TM_HOLE_NUM; i++)
			{
				if (enchaseActiveInfo.CountInfo[i].Count > 0 && enchaseActiveInfo.CountInfo[i].EnchaseId == enchaseId)
				{
					if (enchaseActiveInfo.CountInfo[i].Count >= pEnchaseData->MaxEnchaseCount)
						return talisman_result_max_enchase_count;
				}
			}
		}
		else
			talisman_result_failure;
	}

	int originalEnchase = talisman.GetTalismanEnchase(enchasePos);
	if ((originalEnchase == 0 && enchaseId > 0)//向空位镶嵌
		|| (originalEnchase > 0 && enchaseId == 0))//去除已有镶嵌
	{
		talisman.SetTalismanEnchase(enchasePos, enchaseId);
		talisman.SyncAttribute(item_attr_talisman_enchase, player.GetNetConnectIdx());
		if (TRUE == itemList.Remove(enchaseItemIndex))
		{
			ItemSet.Remove(enchaseItemIndex);
			return talisman_result_success;
		}
		else
		{
			//TODO 成功镶嵌，但是没有删除内丹，这种情况需要记录
			_ASSERT(false);			
		}
	}

	return talisman_result_failure;
}

TalismanOperationResult TalismanManager::ClearAllEnchase(int playerIndex, int itemIndex)
{
	if (!IsValidPlayer(playerIndex))
	{
		_ASSERT(false);
		return talisman_result_invalid_player;
	}

	if (!IsValidTalisman(itemIndex))
	{
		_ASSERT(false);
		return talisman_result_invalid_talisman;
	}

	KItem& talisman = Item[itemIndex];

	for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
	{
		if (talisman.GetTalismanEnchase(enchaseLoopCount) > 0)
		{
			talisman.SetTalismanEnchase(enchaseLoopCount, 0);
		}	
	}

	return talisman_result_success;
}

TalismanOperationResult TalismanManager::ConvertSkillExpToTalismanPotential(int playerIndex, int itemIndex, int avaiableSkillExpToConvert)
{
	if (!IsValidPlayer(playerIndex))
	{
		_ASSERT(false);
		return talisman_result_invalid_player;
	}

	if (!IsValidTalisman(itemIndex))
	{
		_ASSERT(false);
		return talisman_result_invalid_talisman;
	}

	KPlayer& player = Player[playerIndex];

	if (player.GetSkillExp() < avaiableSkillExpToConvert)
	{
		_ASSERT(false);
		return talisman_result_not_enough_skill_exp;
	}

	KItem& talisman = Item[itemIndex];
	ConfigManager& cm = ConfigManager::Singleton();
	
	int convertRate = cm.GetGlobalVariable(global_var_talisman_potential_convert_rate);
	if (convertRate <= 0)
		return talisman_result_invalid_convert_rate;

	int maxTalismanPotential = talisman.GetTalismanPotentialLimit() - talisman.GetTalismanPotential();
	int talismanPotentialAvaiable = avaiableSkillExpToConvert * convertRate / 100;
	int actualTalismanPotential = min(maxTalismanPotential, talismanPotentialAvaiable);
	talisman.SetTalismanPotential(talisman.GetTalismanPotential() + actualTalismanPotential);
	talisman.SyncAttribute(item_attr_talisman_potential,player.m_nNetConnectIdx);
	int costSkillExp = actualTalismanPotential * 100 / convertRate;
	int remainingSkillExp = player.GetSkillExp() - costSkillExp;
	if (remainingSkillExp < 0)
		remainingSkillExp = 0;
	player.SetSkillExp(remainingSkillExp);
	player.SyncAttribute(attr_SkillExp);

	return talisman_result_success;
}

#else

void TalismanManager::ClientProcessProtocol(BYTE *pMsg)
{
	PTALISMAN_OPERATION pTalismanOperation = (PTALISMAN_OPERATION)pMsg;

	switch(pTalismanOperation->SubProtocol)
	{
	case talisman_protocol_upgrade:
		{
			int talismanId = pTalismanOperation->Params[0];
			int newTalismanId = pTalismanOperation->Params[1];
			CoreDataChanged(CDCNI_TALISMAN_PROP_CHANGE, talismanId, newTalismanId);
		}
		break;
	case talisman_protocol_enchase:
		{
			int talismanId = pTalismanOperation->Params[0];
			CoreDataChanged(CDCNI_TALISMAN_PROP_CHANGE, talismanId, talismanId);
		}
		break;
	case talisman_protocol_convert_skill_exp:
		{
			int talismanId = pTalismanOperation->Params[0];
			CoreDataChanged(CDCNI_TALISMAN_PROP_CHANGE, talismanId, talismanId);
		}
		break;
	default:
		break;
	}
}

void TalismanManager::GetEnchaseDesc(int itemIndex, int enchasePos, char* descBuff)
{
	if(!checkItemIndex(itemIndex))
	{
		return;
	}
	KItem& talisman = Item[itemIndex];
	GetEnchaseDesc(talisman, enchasePos, descBuff);
}

void TalismanManager::GetEnchaseDesc(KItem& talisman, int enchasePos, char* descBuff)
{
	if (!IsValidEnchasePos(enchasePos) || (descBuff == NULL) || !IsValidTalisman(talisman))
		return;
	
	EnchaseActiveInfo enchaseActiveInfo;
	GetEnchaseActiveInfo(talisman, enchaseActiveInfo);

	BuffTable& bt = BuffTable::Singleton();
	ConfigManager& cm = ConfigManager::Singleton();

	//		const char* pUnenabledStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_unenabled);
	
	bool isActive = enchaseActiveInfo.IsActive[enchasePos];
	int enchaseId = talisman.GetTalismanEnchase(enchasePos);	
	if(enchaseId > 0)
	{
		const PEnchaseData pEnchaseData = GetEnchaseData(enchaseId);
		
		if (pEnchaseData != NULL)
		{			
			const char* pNameStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_name, enchaseActiveInfo.IsActive[enchasePos]);
			const char* pDescStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_desc);				
			const char* pEnchaseBuffStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_buff);
			const char* pRequireStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_require, enchaseActiveInfo.IsActive[enchasePos]);
			const char* pRequireGroupStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_require_group, enchaseActiveInfo.IsActive[enchasePos]);
			if (pNameStyle != NULL && pDescStyle != NULL && pEnchaseBuffStyle != NULL && pRequireStyle != NULL && pRequireGroupStyle != NULL)
			{
				char enchaseName[128] = { 0 };
				snprintf(enchaseName, sizeof(enchaseName), pNameStyle, pEnchaseData->Name);
				strncat(descBuff, enchaseName, sizeof(enchaseName));
				
				char enchaseDesc[256] = { 0 };
				snprintf(enchaseDesc, sizeof(enchaseDesc), pDescStyle, pEnchaseData->Desc);
				strncat(descBuff, enchaseDesc, sizeof(enchaseDesc));
								
// 				for (int enchaseCountLoopCount = 0; enchaseCountLoopCount < MAX_ENCHASE_COUNT_PER_TALISMAN; enchaseCountLoopCount++)
// 				{
// 					char enchaseBuffList[2304] = { 0 };
// 					for (int enchaseBuffLoopCount = 0; enchaseBuffLoopCount < MAX_ENCHASE_BUFF_COUNT; enchaseBuffLoopCount++)
// 					{
// 						int buffId = pEnchaseData->BuffSet[enchaseCountLoopCount][enchaseBuffLoopCount];
// 						if (buffId > 0)
// 						{
// 							PBAT pBuff = bt.GetBuff(buffId);
// 							if (pBuff != NULL)
// 							{
// 								char buff[256] = { 0 };
// 								snprintf(buff, sizeof(buff), pEnchaseBuffStyle, pBuff->szDesc);
// 								strncat(enchaseBuffList, buff, sizeof(buff)); 
// 							}
// 						}
// 					}
// 					strncat(descBuff, enchaseBuffList, sizeof(enchaseBuffList));
// 				}	
				
				if (pEnchaseData->RequireId > 0)
				{
					const PEnchaseData pRequireEnchaseData = GetEnchaseData(pEnchaseData->RequireId);
					if (pRequireEnchaseData != NULL)
					{
						char require[128] = { 0 };
						snprintf(require, sizeof(require), pRequireStyle, pRequireEnchaseData->Name, pEnchaseData->RequireCount);
						strncat(descBuff, require, sizeof(require));
					}
				}
				
				if (pEnchaseData->RequireGroupId > 0)
				{
					char requireGroup[128] = { 0 };
					snprintf(requireGroup, sizeof(requireGroup), pRequireGroupStyle, pEnchaseData->RequireGroupId, pEnchaseData->RequireGroupCount);
					strncat(descBuff, requireGroup, sizeof(requireGroup));
				}
			}
		}
	}
	else if (enchaseId == 0)
	{
		const char* pUnusedStyle = cm.GetConfigurableDisplayStyle(style_talisman_enchase_unused, 1);
		if (pUnusedStyle != NULL)
			strcat(descBuff, pUnusedStyle);
	}
}

void TalismanManager::GetTalismanDesc(int talismanIndex, char* descBuff)
{
	if(!checkItemIndex(talismanIndex))
	{
		return;
	}
	
	KItem& talisman = Item[talismanIndex];
	GetTalismanDesc(talisman, descBuff);
}

void TalismanManager::GetTalismanDesc(KItem& talisman, char* descBuff)
{
	if (descBuff == NULL)
		return;

	if (!IsValidTalisman(talisman))
		return;	

	ConfigManager& cm = ConfigManager::Singleton();
	BuffTable& bt = BuffTable::Singleton();

	//法宝等级
	char levelInfo[COMMON_CLIENT_MSG_LEN_512] = { 0 };
	const char* talismanLevelTemplate = cm.GetConfigurableDisplayStyle(style_talisman_level, talisman.GetLevel());
	if (talismanLevelTemplate != NULL)
	{
		char* talismanTopLevelTemplate = NULL;
		if (IsReachTopLevel(talisman))
		{
			talismanTopLevelTemplate = (char*)cm.GetConfigurableDisplayStyle(style_talisman_top_level);
		}
		snprintf(levelInfo, COMMON_CLIENT_MSG_LEN_512, talismanLevelTemplate, talismanTopLevelTemplate);
		levelInfo[COMMON_CLIENT_MSG_LEN_512 - 1] = 0;
	}
	strcat(descBuff, levelInfo);

	//法宝品质（成孔率）
	char talismanQualityInfo[COMMON_CLIENT_MSG_LEN_512] = { 0 };
	const char* talismanQualityTemplate = cm.GetConfigurableDisplayStyle(style_talisman_quality);
	if (talismanQualityTemplate != NULL)
	{
		sprintf(talismanQualityInfo, talismanQualityTemplate, talisman.GetItemQuality());
	}
	strcat(descBuff, talismanQualityInfo);

	//法宝蕴魂
	char talismanPotentialInfo[COMMON_CLIENT_MSG_LEN_512] = { 0 };
	const char* talismanPotentialTemplate = cm.GetConfigurableDisplayStyle(style_talisman_potential);
	if (talismanPotentialTemplate != NULL)
	{
		sprintf(talismanPotentialInfo, talismanPotentialTemplate, talisman.GetTalismanPotential(), talisman.GetTalismanPotentialLimit());
	}
	strcat(descBuff, talismanPotentialInfo);

	int levelCount = talisman.GetTalismanLevel();
	if (levelCount > 0)
	{
		EnchaseActiveInfo enchaseActiveInfo;
		GetEnchaseActiveInfo(talisman, enchaseActiveInfo);

		if (talisman.GetTalismanBuff() > 0)
		{
			PBAT pBuff = bt.GetBuff(talisman.GetTalismanBuff());
			if (pBuff != NULL)
			{
				strncat(descBuff, pBuff->szDesc, MAX_BUFF_DESC);
			}
		}

		const char* pUnusedTemplate = cm.GetConfigurableDisplayStyle(style_talisman_enchase_unused);
		const char* pUnenabledTemplate = cm.GetConfigurableDisplayStyle(style_talisman_enchase_unenabled);		
		const char* pNewLineSeg = cm.GetConfigurableDisplayStyle(style_new_line_seg);
		char enchaseList[4608] = { 0 };
		if (pUnusedTemplate != NULL && pUnenabledTemplate != NULL && pNewLineSeg != NULL)
		{
			for (int enchaseLevelLoopCount = 0; enchaseLevelLoopCount < levelCount; enchaseLevelLoopCount++)
			{
				strcat(enchaseList, pNewLineSeg);

				for (int enchaseInLevelLoopCount = 0; enchaseInLevelLoopCount < TALISMAN_ENCHASE_PER_LEVEL; enchaseInLevelLoopCount++)
				{
					char enchase[256] = { 0 };
					int enchasePos = enchaseLevelLoopCount * TALISMAN_ENCHASE_PER_LEVEL + enchaseInLevelLoopCount;
					int enchaseId = talisman.GetTalismanEnchase(enchasePos);
					if(enchaseId > 0)
					{
						const PEnchaseData pEnchaseData = GetEnchaseData(enchaseId);	
						if (pEnchaseData != NULL)
						{
							const char* pEnchaseTemplate = cm.GetConfigurableDisplayStyle(style_talisman_enchase, enchaseActiveInfo.IsActive[enchasePos]);						
							sprintf(enchase, pEnchaseTemplate,
								pEnchaseData->MiniImageSetName,
								pEnchaseData->MiniImageName,							
								pEnchaseData->Name,							
								pEnchaseData->Desc
								);
						}
					}
					else if (enchaseId == 0)
					{
						sprintf(enchase, pUnusedTemplate);
					}
					else
					{
						sprintf(enchase, pUnenabledTemplate);
					}

					strcat(enchaseList, enchase);
				}
			}
		}
		strcat(descBuff, enchaseList);
	}
}

void TalismanManager::GetEnchaseItemDesc(int enchaseItemIndex, char* descBuff)
{
	if(!checkItemIndex(enchaseItemIndex))
	{
		return;
	}
	KItem& enchaseItem = Item[enchaseItemIndex];
	GetEnchaseItemDesc(enchaseItem, descBuff);
}

void TalismanManager::GetEnchaseItemDesc(KItem& enchaseItem, char* descBuff)
{
	if (descBuff == NULL)
		return;

	ConfigManager& cm = ConfigManager::Singleton();
	BuffTable& bt = BuffTable::Singleton();

	if (!IsValidEnchaseItem(enchaseItem))
		return;	
	
	//内丹等级
	char levelInfo[COMMON_CLIENT_MSG_LEN_128] = { 0 };
	const char* enchaseItemLevelTemplate = cm.GetConfigurableDisplayStyle(style_enchase_item_level, enchaseItem.GetLevel());
	if (enchaseItemLevelTemplate != NULL)
	{
		strcpy(levelInfo, enchaseItemLevelTemplate);
		strcat(descBuff, levelInfo);
	}
}

void TalismanManager::GetTalismanEffectDesc(int talismanIndex, char* descBuff, size_t size)
{
	if (descBuff == NULL)
		return;

	BuffTable& bt = BuffTable::Singleton();

	if (!IsValidTalisman(talismanIndex))
		return;	
	
	KItem& talisman = Item[talismanIndex];

	int talismanBuffId = talisman.GetTalismanBuff();
	if (talismanBuffId > 0)
	{
		PBAT pBat = bt.GetBuff(talismanBuffId);
		if (pBat)
		{
			strncpy(descBuff, pBat->szDesc, size);
		}
	}
}

#endif