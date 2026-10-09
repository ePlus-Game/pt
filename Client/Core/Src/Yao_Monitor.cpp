//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-11-8
//      File_base        : Yao_Monitor
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 爻装监视器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "Yao_Monitor.h"
#include "Yao_Table.h"
#include "buff_man.h"
#include "buff_tab.h"
#include "KItem.h"
#include "ConfigManager.h"
#include "screeneffect_man.h"

YaoMonitor::YaoMonitor()
{
	Init();
}

YaoMonitor::~YaoMonitor()
{
}

void YaoMonitor::Init()
{
	memset(m_GuaState, 0, sizeof(m_GuaState));
	for(int guaPos = 0; guaPos < gua_pos_count; guaPos++)
	{
		m_GuaState[guaPos].GuaID = -1;
	}
	memset(m_MutiGuaState, 0, sizeof(m_MutiGuaState));	
	m_MutiGuaCount = 0;
	m_YaoSetID = 0;

#ifdef _SERVER
	CleanUp();
#endif

}

void YaoMonitor::Update(int npcIndex, const KEquipState* equipments)
{
	YaoTable& yaoTable = YaoTable::Singleton();

	//清空原有卦位情况
	memset(m_GuaState, 0, sizeof(m_GuaState));
	memset(m_MutiGuaState, 0 ,sizeof(m_MutiGuaState));
	//初始化为“未激活”状态
	for(int i = 0; i < gua_pos_count; i++)
	{
		m_GuaState[i].GuaID = -1;
		m_MutiGuaState[i].GuaID = -1;
	}
	m_YaoSetID = 0;
	m_MutiGuaCount = 0;
	
	//装备位置和卦位置的映射
	const static int map[gua_pos_count][YAO_PER_GUA] = {
		itempart_amulet, itempart_helm, itempart_shoulder,
		itempart_weapon, itempart_armor, itempart_pendant,
		itempart_ring, itempart_boots, itempart_cuff,
		itempart_amulet, itempart_armor, itempart_cuff,
		itempart_shoulder, itempart_pendant, itempart_cuff,
		itempart_helm, itempart_armor, itempart_boots,
		itempart_amulet, itempart_weapon, itempart_ring,
		itempart_shoulder, itempart_armor, itempart_ring
	};

	//扫描并激活卦位，同时记录同种卦的数量，以及是否可以产生挂组合效果
	int guaGrouplevel = 0;//卦组合的等级（0表示尚未检测，-1表示没有一致等级，>0表示实际等级）
	for(int guaPosLoopCount = 0; guaPosLoopCount < gua_pos_count; guaPosLoopCount++)
	{
		const int* group = map[guaPosLoopCount];
		
		//装备都存在
		if (equipments[group[0]].nEquipIdx > 0 && 
			equipments[group[1]].nEquipIdx > 0 &&
			equipments[group[2]].nEquipIdx > 0)
		{
			KItem* pItems[3] = {
				&Item[equipments[group[0]].nEquipIdx],
				&Item[equipments[group[1]].nEquipIdx],
				&Item[equipments[group[2]].nEquipIdx]
			};

			//装备都没有损坏
			if (!pItems[0]->IsBroken() &&
				!pItems[1]->IsBroken() &&
				!pItems[2]->IsBroken() &&
				!pItems[0]->IsOverDate() &&
				!pItems[1]->IsOverDate() &&
				!pItems[2]->IsOverDate()
				)
			{
				//都有爻
				if (pItems[0]->GetYaoID() != yao_invalid &&
					pItems[1]->GetYaoID() != yao_invalid &&
					pItems[2]->GetYaoID() != yao_invalid)
				{
					//卦位等级以最低级为准
					int guaLevel = pItems[0]->GetYaoLevel();
					if (guaLevel > pItems[1]->GetYaoLevel())
						guaLevel = pItems[1]->GetYaoLevel();
					if (guaLevel > pItems[2]->GetYaoLevel())
						guaLevel = pItems[2]->GetYaoLevel();
					
						//计算并设置卦的等级和编号
						int guaID = CaculateGua(
							pItems[0]->GetYaoID(),
							pItems[1]->GetYaoID(),
							pItems[2]->GetYaoID()
							);
						
						_ASSERT(guaID >= 0 && guaID <= 7);
						if(guaID >= 0 && guaID <= 7)
						{
							m_GuaState[guaPosLoopCount].GuaID = guaID;
							m_GuaState[guaPosLoopCount].Level = guaLevel;
							
							//记住激活这个卦的装备的序号
							for(int k = 0; k < YAO_PER_GUA; k++)
							{
								m_GuaState[guaPosLoopCount].EquipmentID[k] = equipments[group[k]].nEquipIdx;
							}			
							
							//统计同种类型和等级的卦的数量
							for(int mutiGuaStateLoopCount = 0; mutiGuaStateLoopCount < gua_pos_count; mutiGuaStateLoopCount++)
							{
								if (m_MutiGuaState[mutiGuaStateLoopCount].GuaID < 0)//这个位置尚未占用
								{
									m_MutiGuaState[mutiGuaStateLoopCount].GuaID = guaID;
									m_MutiGuaState[mutiGuaStateLoopCount].Level = guaLevel;							
									m_MutiGuaState[mutiGuaStateLoopCount].Count = 1;
									m_MutiGuaCount++;
									break;
								}
								else
								{
									if (m_MutiGuaState[mutiGuaStateLoopCount].GuaID == guaID && m_MutiGuaState[mutiGuaStateLoopCount].Level == guaLevel)
									{
									if (m_MutiGuaState[mutiGuaStateLoopCount].Level > guaLevel)
										m_MutiGuaState[mutiGuaStateLoopCount].Level = guaLevel;
										m_MutiGuaState[mutiGuaStateLoopCount].Count++;
										break;
									}
								}				
							}
							
						if (guaGrouplevel == 0 || guaGrouplevel > guaLevel)
						{
							guaGrouplevel = guaLevel;
						}
					}
				}
				else
				{
					//只要出现一个卦位未激活，则不会出现卦组合效果
					guaGrouplevel = -1;
				}
			}
		}
		else
		{
			//只要出现一个卦位未激活，则不会出现卦组合效果
			guaGrouplevel = -1;
		}
	}

	//检测卦组合
	if (guaGrouplevel > 0)//形成了一致等级的卦组合
	{
		ParamYaoSet paramYS;
		paramYS.Level = guaGrouplevel;
		for(int guaPosLoopCount = 0; guaPosLoopCount < gua_pos_count; guaPosLoopCount++)
		{
			paramYS.GuaIDs[guaPosLoopCount] = m_GuaState[guaPosLoopCount].GuaID;
		}
		
		const YaoSet* pYS = yaoTable.GetYaoSet(&paramYS);
		if (pYS != NULL)
		{
#ifndef _SERVER
			// 卦激活时播放动画
			//ScreenEffectMgr::Singleton().Player(3, BeforeUi);
#endif
			m_YaoSetID = pYS->YaoSetID;
		}
	}

#ifdef _SERVER

	BeginAddBuff();

	//激活爻装附加效果
	for(int itemPart = 0; itemPart < itempart_num; itemPart++)
	{
		if (equipments[itemPart].nEquipIdx > 0)
		{
			KItem& item = Item[equipments[itemPart].nEquipIdx];		
			for (int yaoAddOnBuffLoopCount = 0; yaoAddOnBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddOnBuffLoopCount++)
			{
				int yaoAddOnBuffID = item.GetYaoAddOn(yaoAddOnBuffLoopCount);
				if (yaoAddOnBuffID > 0)
				{
					AddBuff(yaoAddOnBuffID);
				}
			}		
		}
	}

	//激活单卦、多卦效果
	for(int mutiGuaLoopCount = 0; mutiGuaLoopCount < m_MutiGuaCount; mutiGuaLoopCount++)
	{
		const YaoData* yaoData = yaoTable.GetYao(m_MutiGuaState[mutiGuaLoopCount].GuaID, m_MutiGuaState[mutiGuaLoopCount].Level);
		_ASSERT(yaoData != NULL);
		if (yaoData != NULL)
		{
			//单卦效果
			int count = m_MutiGuaState[mutiGuaLoopCount].Count;
			for(int standaloneEffectLoopCount = 0; standaloneEffectLoopCount < count; standaloneEffectLoopCount++)
			{
				if (yaoData->StandaloneEffectID > 0)
				{
					AddBuff(yaoData->StandaloneEffectID);
				}
			}
			
			//多卦效果
			for(int mutiGuaEffectLoopCount = count - 1; mutiGuaEffectLoopCount >= 0; mutiGuaEffectLoopCount--)
			{
				int buffID = yaoData->EffectIDs[mutiGuaEffectLoopCount];
				if (buffID > 0)
				{
					AddBuff(buffID);
					break;
				}
			}
		}
	}

	//激活卦组合效果
	if (m_YaoSetID > 0)
	{
		const YaoSet* pYaoSet = yaoTable.GetYaoSet(m_YaoSetID);
		if (pYaoSet != NULL)
		{
			if (pYaoSet->EffectID > 0)
			{
				AddBuff(pYaoSet->EffectID);
			}
		}
	}

	EndAddBuff();

	SetupBuff(npcIndex);

#endif
}

#ifndef _SERVER

void YaoMonitor::GetGuaDesc(GUA_POS guaPos, char* descBuff)
{ 	
	if (guaPos < 0 || guaPos >= gua_pos_count || descBuff == NULL)
	{
		_ASSERT(false);
		return;
	}

	YaoTable& yaoTable = YaoTable::Singleton();
	GuaState& state = m_GuaState[guaPos];
	BuffTable& buffTable = BuffTable::Singleton();
	ConfigManager& cm = ConfigManager::Singleton();

	if (state.GuaID < 0)//没有激活这个卦
		return;
	
	const YaoData* yaoData = yaoTable.GetYao(state.GuaID, state.Level);
	_ASSERT(yaoData != NULL);
	if (yaoData == NULL)
	{
		return;
	}

	//卦位名称
	char guaName[128] = { 0 };
	const char* guaNameTemplate = cm.GetConfigurableDisplayStyle(style_gua_name, yaoData->GuaID);
	if (guaNameTemplate != NULL)
	{
		sprintf(guaName, guaNameTemplate, yaoData->Name);
	}
	strcat(descBuff, guaName);

	//卦位等级
	char guaLevel[128] = { 0 };
	const char* guaLevelTemplate = cm.GetConfigurableDisplayStyle(style_gua_level, yaoData->Level);
	if (guaLevelTemplate != NULL)
	{
		sprintf(guaLevel, guaLevelTemplate);
	}
	strcat(descBuff, guaLevel);

	//卦位描述
	char guaDesc[256] = { 0 };
	const char* guaDescTemplate = cm.GetConfigurableDisplayStyle(style_gua_desc);
	if (guaDescTemplate != NULL)
	{
		sprintf(guaDesc, guaDescTemplate, yaoData->Desc);
	}
	strcat(descBuff, guaDesc);	
	
	//激活这个卦的装备
	char activeItem[512] = { 0 };
	const char* activeItemTemplate = cm.GetConfigurableDisplayStyle(style_gua_active_item);
	if (activeItemTemplate != NULL)
	{
		char activeItemList[512] = { 0 };
		const char* newLineObj = cm.GetConfigurableDisplayStyle(style_new_line_obj);
		for(int activeItemIndex = 0; activeItemIndex < YAO_PER_GUA; activeItemIndex++)
		{
			KItem& item = Item[state.EquipmentID[activeItemIndex]];
			const char* itemNameTemplate = cm.GetConfigurableDisplayStyle(style_item_name_name, item.GetQualityLabel());
			const char* itemName = item.GetName();
			
			char singleItem[128] = { 0 };
			if (itemNameTemplate != NULL && itemName != NULL)
			{
				sprintf(singleItem, itemNameTemplate, itemName);
			}
			strcat(activeItemList, singleItem);
			strcat(activeItemList, newLineObj);
		}

		sprintf(activeItem, activeItemTemplate, activeItemList);
	}
	strcat(descBuff, activeItem);

	//单卦属性
	char standalone[256] = { 0 };
	if (yaoData->StandaloneEffectID > 0)
	{
		const char* standaloneTemplate = cm.GetConfigurableDisplayStyle(style_gua_standalone);
		if (standaloneTemplate != NULL)
		{
			PBAT pBuff = buffTable.GetBuff(yaoData->StandaloneEffectID);
			_ASSERT(pBuff != NULL);
			if (pBuff != NULL)
			{
				sprintf(standalone, standaloneTemplate, pBuff->szDesc);
			}
		}
	}
	strcat(descBuff, standalone);

	GetMutiGuaDesc(descBuff);

	GetYaoSetDesc(descBuff);
}

void YaoMonitor::GetMutiGuaDesc(char* descBuff)
{
	if (m_MutiGuaCount <= 0 || descBuff == NULL)
		return;

	YaoTable& yaoTable = YaoTable::Singleton();
	BuffTable& buffTable = BuffTable::Singleton();
	ConfigManager& cm = ConfigManager::Singleton();

	const char* mutiGuaTemplate = cm.GetConfigurableDisplayStyle(style_gua_mutiple);
	const char* mutiGuaRowTemplate = cm.GetConfigurableDisplayStyle(style_gua_mutiple_row);
	const char* newLineObj = cm.GetConfigurableDisplayStyle(style_new_line_obj);
	if (mutiGuaTemplate != NULL && mutiGuaRowTemplate != NULL)
	{
		char mutiGua[512] = { 0 };
		char mutiGuaList[512] = { 0 };
		for (int mutiGuaLoopCount = 0; mutiGuaLoopCount < m_MutiGuaCount; mutiGuaLoopCount++)
		{
			MutiGuaState& state = m_MutiGuaState[mutiGuaLoopCount];
			const YaoData* yaoData = yaoTable.GetYao(state.GuaID, state.Level);
			
			if (yaoData != NULL)
			{
				for(int mutiGuaEffectLoopCount = state.Count - 1; mutiGuaEffectLoopCount >= 0; mutiGuaEffectLoopCount--)
				{
					int buffID = yaoData->EffectIDs[mutiGuaEffectLoopCount];
					if (buffID > 0)
					{
						PBAT pBuff = buffTable.GetBuff(buffID);
						if (pBuff != NULL)
						{
							char mutiGuaRow[128] = { 0 };
							sprintf(mutiGuaRow,
								mutiGuaRowTemplate,
								(mutiGuaEffectLoopCount + 1),
								yaoData->Name,
								pBuff->szDesc);
							strcat(mutiGuaList, mutiGuaRow);
							strcat(mutiGuaList, newLineObj);
						}
						break;
					}
				}
			}
		}
		sprintf(mutiGua, mutiGuaTemplate, mutiGuaList);
		strcat(descBuff, mutiGua);
	}
}

void YaoMonitor::GetYaoSetDesc(char* descBuff)
{
	if (m_YaoSetID <= 0 || descBuff == NULL)
		return;

	const YaoSet* pYS = YaoTable::Singleton().GetYaoSet(m_YaoSetID);
	if (pYS != NULL)
	{
		PBAT pBuff = BuffTable::Singleton().GetBuff(pYS->EffectID);
		if (pBuff != NULL)
		{
			ConfigManager& cm = ConfigManager::Singleton();
			
			const char* guaSetNameTemplate = cm.GetConfigurableDisplayStyle(style_gua_set_name);
			const char* guaSetCommentTemplate = cm.GetConfigurableDisplayStyle(style_gua_set_comment);
			const char* guaSetEffectTemplate = cm.GetConfigurableDisplayStyle(style_gua_set_effect);
			const char* guaSetDescTemplate = cm.GetConfigurableDisplayStyle(style_gua_set_desc);
			
			if (guaSetNameTemplate != NULL && guaSetCommentTemplate != NULL
				&& guaSetEffectTemplate != NULL && guaSetDescTemplate != NULL)
			{
				char guaSetName[256] = { 0 };
				sprintf(guaSetName, guaSetNameTemplate, pYS->Name);
				strcat(descBuff, guaSetName);

				char guaSetComment[256] = { 0 };
				sprintf(guaSetComment, guaSetCommentTemplate, pYS->Comment);
				strcat(descBuff, guaSetComment);

				char guaSetEffect[256] = { 0 };
				sprintf(guaSetEffect, guaSetEffectTemplate, pBuff->szDesc);
				strcat(descBuff, guaSetEffect);

				char guaSetDesc[256] = { 0 };
				sprintf(guaSetDesc, guaSetDescTemplate, pYS->Desc);
				strcat(descBuff, guaSetDesc);				
			}
		}
	}
}

#endif