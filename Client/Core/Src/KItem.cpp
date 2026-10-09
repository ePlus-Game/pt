//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/10/2006 17:54
//      File_base        : KItem
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi) Lucifer~yu (Zhang jian yu)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "MyAssert.H"
#include "KTabFile.h"
#include "KNpc.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KItemGenerator.h"
#include "KPlayer.h"
#include "KItemGenerator.h"
#include "ArmorSet_Table.h"
#include "buff_tab.h"
#include "buff_man.h"
#include "KSkills.h"
#include "Abrade_Table.h"
#include "ConfigManager.h"
#include "KCompoundRule.h"
#include "kiteminlayaddontable.h"
#include "ScriptFuns.h"
#include "KSubWorld.h"

#ifndef _SERVER
	#include "ImgRef.h"
	#include "iRepresentshell.h"
	#include "GameDataDef.h"
	#include "CoreRelated.h"
#endif

#ifdef _SERVER
#include "ChatCenter_S.h"
#endif

#include "talisman_manager.h"

const char seps[]   = ";";

const char manl[] = "_man_large";

const char womanl[] = "_woman_large";

KItem	*Item;

int GenerateItemHashId( int nGenre, int nDetail, int nParticular )
{
	int nRet = 0;
	nGenre = (nGenre << 24) & 0xff000000;
	nDetail = (nDetail << 12) & 0x00fff000;
	nParticular = (nParticular) & 0x00000fff;
	nRet |= nGenre;
	nRet |= nDetail;
	nRet |= nParticular;
	return nRet;
}

void SpliteHashId( int nHashId, int &nGenre, int &nDetail, int &nParticular )
{
	nGenre = (nHashId & 0xff000000) >> 24;
	nDetail = (nHashId &  0x00fff000) >> 12;
	nParticular = nHashId  & 0x00000fff;
}

void _getItemTarget( char* string, char pTargetItem[][COMMON_CLIENT_MSG_LEN_64] )
{
	int i = 0;
	char* token = NULL; 
	token = strtok( string, seps );
	while( token != NULL )
	{
		strncpy( pTargetItem[i], token, COMMON_CLIENT_MSG_LEN_64 );
		token = strtok( NULL, seps );
		i++;
	}
}

int GetRandomNumber(int nMin, int nMax);

void GetSocketParam( char* sValue, int* socketCount,  int* socketPercentArray, int arraySize )
{
	if ( (sValue == NULL) || (arraySize == 0) )
	{
		return;
	}

	int n = 0;
	int nRet[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	char seps[]   = "|";
	char* token = strtok( sValue, seps );
	while( token != NULL )
	{
		nRet[n] = atoi(token);
		n++;
		token = strtok( NULL, seps );
		if (n >= 9)
			break;
	}
	
	*socketCount = n < arraySize ? n : arraySize; 
	for ( int nIdx = 0; nIdx < *socketCount; ++nIdx )
	{
		socketPercentArray[nIdx] = nRet[nIdx];
	}
}

KItem::KItem()
{
	m_pItemTemplate		= 0;				 
	m_enItemAttrType	= weaponattr_shortweapon;
	m_enItemPosReq		= weaponposreq_none;
	m_GenerateTime		= 0;					
	m_ID				= 0;							
	m_CurrentDurability	= 0;					
	m_MaxDurability		= 0;						
	m_ItemStackCount	= 0;					
	m_YaoID				= yao_invalid;	
	m_UpgradeCount		= 0;
	m_UpgradeType		= -1; 
	m_TalismanPotential = 0;				
	m_IsBind			= false;
	m_nBelongIndex		= -1;

#ifdef _SERVER
	m_ObjBelongIdx      = -1;
#endif

	m_LockCount			= 0;
	m_MapID				= -1;
	m_MapX				= -1;
	m_MapY				= -1;
	m_Step				= 0;
	::memset(m_szPlusInfo, 0, sizeof(m_szPlusInfo));
	::memset(m_TalismanEnchaseSet, -1, sizeof(m_TalismanEnchaseSet));
	::memset(m_YaoAddOnBuffIDSet, 0, sizeof(m_YaoAddOnBuffIDSet));
	::memset(m_CompoundBuffIDSet, 0, sizeof(m_CompoundBuffIDSet));
	::memset(m_InlayBaseBuffSet, 0, sizeof(m_InlayBaseBuffSet) );
	::memset(m_InlayYaoBuffSet, 0, sizeof(m_InlayYaoBuffSet) );
	::memset(m_InlaySpecialBuffSet, 0, sizeof(m_InlaySpecialBuffSet) );
	m_pItemTemplate = (tagKBASICPROP_ITEM *)g_ItemGen.GetItemTemplate(0,0,0,0);
	m_dwIBBuyDate		= 0;
	m_dwLockDate		= 0;
	m_FlushTimes        = 0;
	m_CreditFlag        = 0;
	m_Index				= 0;
	m_IsTaskGiven       = FALSE;
#ifdef _SERVER
	m_IBGUID			= 0;
#endif	
}

KItem::KItem( const KItem& rItem )
{
	m_pItemTemplate		= rItem.m_pItemTemplate;	//基本属模板（从表里读出来的固定数据）
	m_enItemAttrType	= rItem.m_enItemAttrType;	//暂时不需要
	m_enItemPosReq		= rItem.m_enItemPosReq;		//暂时不需要
	m_GenerateTime		= rItem.m_GenerateTime;		//生成时间
	m_ID				= rItem.m_ID;				//独立的ID，用于客户端与服务器端的交流
	m_CurrentDurability	= rItem.m_CurrentDurability;//当前耐久度
	m_MaxDurability		= rItem.m_MaxDurability;	//耐久度上限
	m_ItemStackCount	= rItem.m_ItemStackCount;	//叠放个数
	m_YaoID				= rItem.m_YaoID;			//爻属性编号
	m_UpgradeCount		= rItem.m_UpgradeCount;		//道具升级次数	
	m_UpgradeType		= rItem.m_UpgradeType;		//道具升级类型
	m_TalismanPotential = rItem.m_TalismanPotential;				
	m_IsBind			= rItem.m_IsBind;
	m_LockCount			= rItem.m_LockCount;
	m_CreditFlag        = rItem.m_CreditFlag;

	memcpy(m_szPlusInfo, rItem.m_szPlusInfo, ITEM_PLUS_INFO_LEN);
	m_szPlusInfo[ITEM_PLUS_INFO_LEN-1] = 0;
	memcpy(m_TalismanEnchaseSet, rItem.m_TalismanEnchaseSet, sizeof(m_TalismanEnchaseSet));
	memcpy(m_YaoAddOnBuffIDSet, rItem.m_YaoAddOnBuffIDSet, sizeof(m_YaoAddOnBuffIDSet));			//爻附加属性BUFF
	memcpy((void*)m_CompoundBuffIDSet, rItem.m_CompoundBuffIDSet, sizeof(m_CompoundBuffIDSet));			//道具合成后添加的buff数组

	m_socketSet = rItem.m_socketSet;
	memcpy(m_InlayBaseBuffSet, rItem.m_InlayBaseBuffSet, sizeof(m_InlayBaseBuffSet) );
	memcpy(m_InlayYaoBuffSet, rItem.m_InlayYaoBuffSet, sizeof(m_InlayYaoBuffSet) );
	memcpy(m_InlaySpecialBuffSet, rItem.m_InlaySpecialBuffSet, sizeof(m_InlaySpecialBuffSet) );

	m_MapID				= rItem.m_MapID;
	m_MapX				= rItem.m_MapX;
	m_MapY				= rItem.m_MapY;
	m_Step				= rItem.m_Step;

	m_IsTaskGiven       = rItem.IsTaskGiven();
	m_FlushTimes        = rItem.GetFlushTimes();
}

KItem::~KItem()
{
}

bool KItem::IsMapArea( int nPlayerIdx )
{
	int mapArea[MAPAREA_TARGET_COUNT];
	memset(mapArea,0,sizeof(mapArea));

	if ( !m_pItemTemplate )
	{
		return false;
	}

	if ( strncmp(NO_MAPAREA_LIMIT,m_pItemTemplate->szMapAreaTarget, sizeof(NO_MAPAREA_LIMIT) - 1 ) == 0 )
	{
		return false;
	}

	::sscanf(m_pItemTemplate->szMapAreaTarget, "%d|%d|%d|%d|%d|%d", 
		&(mapArea[0]), &(mapArea[1]), &(mapArea[2]), &(mapArea[3]), &(mapArea[4]), &(mapArea[5]));

	if ( !IsValidPlayer(nPlayerIdx) )
	{
		return false;
	}

	int nNpcIndex = Player[nPlayerIdx].GetNpcIndex( );

	if (!IsValidNpc(nNpcIndex))
	{
		return false;
	}

	KNpc& npc = Npc[nNpcIndex];

	int especialAreaType = npc.CheckEspecialAreaType();

	for ( int idx = 0; idx < MAPAREA_TARGET_COUNT; ++idx)
	{
		if ( mapArea[idx] == especialAreaType && 
			mapArea[idx]  != especial_area_none &&
			especialAreaType != especial_area_none )
		{
			return true;
		}
	}
	
	return false;
}

void KItem::Remove()
{
	m_dwIBBuyDate = 0;
	m_dwLockDate = 0;
    m_CreditFlag = 0;                                  //信贷标识
	m_MapID = 0;
	m_MapX = 0;
	m_MapY = 0;
	m_Step = 0;
#ifdef _SERVER
	m_IBGUID = 0;
	m_ObjBelongIdx = -1;
#endif
	m_nBelongIndex = -1;

	m_enItemAttrType = weaponattr_other;  // lixuewu 2005.01.17 
	m_enItemPosReq = weaponposreq_none;
	m_Index = 0;
	m_socketSet.DestroySocket();
	::memset(m_InlayBaseBuffSet, 0, sizeof(m_InlayBaseBuffSet) );
	::memset(m_InlayYaoBuffSet, 0, sizeof(m_InlayYaoBuffSet) );
	::memset(m_InlaySpecialBuffSet, 0, sizeof(m_InlaySpecialBuffSet) );
	
	m_IsTaskGiven = FALSE;
	m_FlushTimes  = 0;
	
	SetItemCount(0);
}

ITEM_IB_BUY_TYPE KItem::GetAvailableIBBuyType(void)
{
	ITEM_IB_BUY_TYPE res = IIBT_INVALID;
	
	if (m_pItemTemplate)
	{
		unsigned long nFlag = 1;

		for (int nIndex = 0; nIndex < sizeof(m_pItemTemplate->IBBuyType) && m_pItemTemplate->IBBuyType[nIndex];nIndex += 2)
		{
			if (m_pItemTemplate->IBBuyType[nIndex] == '1')
			{
				res = (res | nFlag);
			}//endif

			nFlag *= 2;
		}//end for nIndex
		
	}//endif

	return res;
}

int KItem::Abrade(int count)
{
	if (m_MaxDurability > 0 && m_CurrentDurability > 0 && count > 0)
	{
		if (m_CurrentDurability - count >= 0)
		{
			m_CurrentDurability -= count;
		}
		else
		{
			m_CurrentDurability = 0;
		}
	}
	
	return m_CurrentDurability;//总是返回当前耐久
}

int KItem::Abrade(float rate)
{
	if (m_MaxDurability > 0 && m_CurrentDurability > 0 && rate > 0)
	{
		int abradeValue = (int)(m_MaxDurability * rate);
		if (abradeValue > 0)//由于整数的截断，至少掉1点耐久
		{
			Abrade(abradeValue);
		}
		else
		{
			Abrade(1);
		}
	}

	return m_CurrentDurability;
}

bool KItem::IsSameParitcularItem (const KItem &DesItem) const
{
	if(m_pItemTemplate->nItemGenre == DesItem.m_pItemTemplate->nItemGenre
		&& m_pItemTemplate->nDetailType == DesItem.m_pItemTemplate->nDetailType
		&& m_pItemTemplate->nParticularType == DesItem.m_pItemTemplate->nParticularType
		&& m_IsTaskGiven == DesItem.IsTaskGiven()
		&& m_dwLockDate == DesItem.m_dwLockDate
		&& m_FlushTimes == DesItem.m_FlushTimes
		)
		return true;

	return false;
}

bool KItem::IsSameParitcularItem (int genre, int detailType, int particularType) const
{
	if(m_pItemTemplate->nItemGenre == genre
		&& m_pItemTemplate->nDetailType == detailType
		&& m_pItemTemplate->nParticularType == particularType)
		return true;

	return false;
}

int KItem::getRepairPrice(bool special /*= false*/) const
{
	if (GetGenre() != item_equip)
		return 0;
	
	//最大耐久=-1表示该装备不消耗耐久
	if (m_MaxDurability <= 0)
		return 0;

	//装备耐久不可能小于0
	if(m_CurrentDurability < 0)
	{
		_ASSERT(0);
		return 0;
	}

	//耐久未变化
	if(m_MaxDurability == m_CurrentDurability)
		return 0;

	int price = 0;
	int multiple = m_MaxDurability / (m_MaxDurability - m_CurrentDurability);
	
	if(multiple)
	{
		price = m_pItemTemplate->nPrice / multiple;
	}
	else
	{
		price = m_pItemTemplate->nPrice;
	}
	if(special)
	{
		//price = m_pItemTemplate->nPrice * (m_MaxDurability - m_CurrentDurability) / m_MaxDurability;
		price = price * AbradeTable::Singleton().GetSpecialRepairFactor() * 0.01;
	}
	else
	{
		//price = m_pItemTemplate->nPrice * (m_MaxDurability - m_CurrentDurability) / m_MaxDurability;
		price = price * AbradeTable::Singleton().GetNormalRepairFactor() * 0.01;
	}
	//如果装备价格本身比较便宜，则最少需要一个铜版的修理费
	if(0 == price)
		price = 1;
	return price;
}

BOOL KItem::CanBeRepaired()
{
	if (GetGenre() != item_equip)
		return FALSE;

	if (m_CurrentDurability == -1)
		return FALSE;

	const int nMaxDur = GetMaxDurability();
	if ((m_CurrentDurability == nMaxDur)||(nMaxDur == -1))
		return FALSE;

	return TRUE;
}

int	KItem::GetDropFlag( void ) const
{
	return FALSE;
}

bool KItem::IsMatchProfessionRequirement(KPlayer& player)
{
	const int ProfessionTypeCount = role_skillseries_count * 3;

	int requireProfession[ProfessionTypeCount];
	int requireProfessionSum = GetProfessionRequirement(requireProfession);	
	if (requireProfessionSum == ProfessionTypeCount)
	{
		return true;
	}
	else
	{
		bool bRet = false;

		int roleSeries = player.GetSeries();
		int skillSeries = player.GetSkillSeries();

		if (skillSeries != role_skillseries_invalid)
		{
			bRet = (requireProfession[(roleSeries * role_skillseries_count) + skillSeries] > 0);
		}
		else
		{
			bRet = ((requireProfession[(roleSeries * role_skillseries_count)] > 0) && (requireProfession[(roleSeries * role_skillseries_count) + 1] > 0));
		}

		if ( skillSeries > -1 && IsInitWeapon() )
		{
			bRet = false;
		}

		return bRet;
	}	
}

bool KItem::IsMatchPropertyRequirement(KPlayer& player, bool* requirementMatched /* = NULL */)
{
	int propertyRequirement[ITEM_PROP_REQ_COUNT];
	int propertyRequirementCount = GetPropertyRequirement(propertyRequirement);
	if (propertyRequirementCount == 0)
	{
		return true;
	}
	else
	{
		bool matchAll = true;
		KNpc& npc = Npc[player.GetNpcIndex()];
		int playerProperty[ITEM_PROP_REQ_COUNT] = { npc.m_CompAttrMgr[ncai_nimbus], npc.m_CompAttrMgr[ncai_strength], npc.m_CompAttrMgr[ncai_body], npc.m_CompAttrMgr[ncai_art], npc.CalcPhysicsDamage(idx_value_hight), npc.CalcMagicDamage(idx_value_hight) };
		for(int i = 0; i < ITEM_PROP_REQ_COUNT; i++)
		{
			if (propertyRequirement[i] > playerProperty[i])
			{
				if (requirementMatched != NULL)
				{
					requirementMatched[i] = false;
				}
				matchAll = false;
			}
			else
			{
				if (requirementMatched != NULL)
				{
					requirementMatched[i] = true;
				}
			}
		}

		return matchAll;
	}
}

bool KItem::IsMatchLevelRequirement(KPlayer& player)
{
	return (player.GetLevel() >= m_pItemTemplate->nReqLevel);
}

int KItem::GetProfessionRequirement(int* requirement)
{
	const int ProfessionTypeCount = role_skillseries_count * 3;
	memset(requirement, 0, sizeof(int) * ProfessionTypeCount);
	
	::sscanf(m_pItemTemplate->szReqPro, "%d.%d.%d.%d.%d.%d", &(requirement[0]), &(requirement[1]), &(requirement[2]), &(requirement[3]), &(requirement[4]), &(requirement[5]));
	int requireProfessionSum = 0;
	for(int profession = 0; profession < ProfessionTypeCount; profession++)
	{
		requireProfessionSum += requirement[profession];
	}

	return requireProfessionSum;
}

int KItem::GetPropertyRequirement(int* requirement)
{
	memcpy(requirement, m_pItemTemplate->nReqProperty, sizeof(int) * ITEM_PROP_REQ_COUNT);

	int propertyRequirementCount = 0;
	for(int i = 0; i < ITEM_PROP_REQ_COUNT; i++)
	{
		if (requirement[i] > 0)
			propertyRequirementCount += 1;
	}

	return propertyRequirementCount;
}

int KItem::GetLevelRequirement()
{
	return m_pItemTemplate->nReqLevel;
}

#ifndef _SERVER

void KItem::RecvAttributeSync(BYTE attr, DWORD val)
{
	if (attr < 0 || attr >= item_attr_count)
		return;

	switch(attr)
	{
	case item_attr_durability:
		SetDurability(val);
		break;
	case item_attr_max_durability:
		SetMaxDurability(val);
		break;
	case item_attr_stack_count:
		SetItemCount(val);
		break;
	case item_attr_yao_id:
		SetYaoID(val);
		break;
	case item_attr_upgrade_type:
		SetLevelupType(val);
		break;
	case item_attr_upgrade_count:
		SetLevelupTimes(val);
		break;
	case item_attr_upgrade_buffid:
		SetCompoundBuff(COMPOUND_LEVELUP, val);
		break;
	case item_attr_talisman_potential:
		SetTalismanPotential(val);
		break;
	case item_attr_isbind:
		SetBind((TRUE == val));
		break;
	case item_attr_islocked:
		SetLockCount(val);
		break;
	case item_attr_buytime:
		SetIBBuyDate( val );
		break;
	case item_attr_credit_flag:
		SetCreditFlag( val);
		break;
	case item_attr_step:
		SetStep(val);
		break;
	case item_attr_mapid:
		m_MapID = val;
		break;
	case item_attr_mapx:
		m_MapX = val;
		break;
	case item_attr_mapy:
		m_MapY = val;
	case item_attr_is_task_given:
		SetTaskGiven((BOOL)val);
		break;
	case item_attr_lockdate:
		m_dwLockDate = val;
		break;
	case item_attr_flush_times:
		m_FlushTimes = val;
		break;
	default:
		_ASSERT(false);
		break;
	}
}

int	 KItem::GetTargetWeaponType( int nIdx )
{
	if ( m_pItemTemplate )
	{
		char str[TARGETITEM_TARGET_COUNT][COMMON_CLIENT_MSG_LEN_64];
		ZeroMemory( str, sizeof(char) * TARGETITEM_TARGET_COUNT * COMMON_CLIENT_MSG_LEN_64 );
		char szTemp[SZBUFLEN_0] = {0};
		strncpy( szTemp,m_pItemTemplate->szBuffTarget, SZBUFLEN_0);
		_getItemTarget( szTemp, str );
		int nGenre = 0; int nDetailType = -1;int nParticularType = 0; int nLevel = 0; int nCount = 0; int nQuality = 0; int nYao = 0;
		::sscanf(str[nIdx], "%d|%d|%d|%d|%d|%d|%d", 
			&(nGenre), &(nDetailType), &(nParticularType), &(nLevel), &(nCount), &(nQuality), &(nYao));
		return nDetailType;
	}
	return 0;
}

char* KItem::GetNameWithColor()
{
	ConfigManager& cm = ConfigManager::Singleton();


	ZeroMemory( m_itemName, sizeof(m_itemName) );
	/*const char* itemNameTemplate = cm.GetConfigurableDisplayStyle(style_item_name);
	if (itemNameTemplate != NULL)
	{//*/
		char nameYao[128] = { 0 };//爻装爻类型
		if (m_YaoID == yao_yin || m_YaoID == yao_yang)
		{
			const char* yaoTemplate = cm.GetConfigurableDisplayStyle(style_item_name_yao, m_YaoID);
			if (yaoTemplate != NULL)
				strcpy(nameYao, yaoTemplate);
		}

		char nameName[128] = { 0 };//装备名字
		const char* nameTemplate = cm.GetConfigurableDisplayStyle(style_item_name_name, GetQualityLabel());
		if (nameTemplate != NULL)
		{
			sprintf(nameName, nameTemplate, m_pItemTemplate->szName);
		}
		
		strcpy(m_itemName, nameYao);
		strcat(m_itemName, nameName);

		return m_itemName;

		/*char nameYaoLevel[128] = { 0 };//爻装爻等级
		const char* yaoLevelTemplate = cm.GetConfigurableDisplayStyle(style_item_name_yaolevel, GetYaoLevel());
		if (yaoLevelTemplate != NULL)
		{
			strcpy(nameYaoLevel, yaoLevelTemplate);
		}

		sprintf(itemName, "%s%s%s", nameYao, nameName, nameYaoLevel);			
	}

	return itemName;//*/
}

void KItem::GetDesc(char* descBuff , bool bIsShopSpecial /* = false*/)
{
	ConfigManager& cm = ConfigManager::Singleton();
	BuffTable& buffTable = BuffTable::Singleton();

	//装备名
	char itemName[384] = { 0 };	
	const char* itemNameTemplate = cm.GetConfigurableDisplayStyle(style_item_name);
	if (itemNameTemplate != NULL)
	{
		char nameYao[128] = { 0 };//爻装爻类型
		if (m_YaoID == yao_yin || m_YaoID == yao_yang)
		{
			const char* yaoTemplate = cm.GetConfigurableDisplayStyle(style_item_name_yao, m_YaoID);
			if (yaoTemplate != NULL)
				strcpy(nameYao, yaoTemplate);
		}

		char nameName[128] = { 0 };//装备名字
		const char* nameTemplate = cm.GetConfigurableDisplayStyle(style_item_name_name, GetQualityLabel());
		if (nameTemplate != NULL)
		{
			sprintf(nameName, nameTemplate, m_pItemTemplate->szName);
		}

		char nameYaoLevel[128] = { 0 };//爻装爻等级
		const char* yaoLevelTemplate = cm.GetConfigurableDisplayStyle(style_item_name_yaolevel, GetYaoLevel());
		if (yaoLevelTemplate != NULL)
		{
			strcpy(nameYaoLevel, yaoLevelTemplate);
		}

		sprintf(itemName, itemNameTemplate, nameYao, nameName, nameYaoLevel);			
	}
	strcat(descBuff, itemName);

	//likun 判断男女
	/*
	char itemImage[512] = { 0 };
	const char* imageFile = GetImageFile();
	const char* imageSet = GetImageSetFile();
	char manOrWomanImage[512];
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0 )
	{
		sprintf(manOrWomanImage, "%s%s", imageFile, manl);
	}
	else
	{
		sprintf(manOrWomanImage, "%s%s", imageFile, womanl);
	}

	const char* itemImageTemplate = cm.GetConfigurableDisplayStyle(style_item_image);
	if (imageFile != NULL && imageSet != NULL && itemImageTemplate != NULL)
	{
		char imageSrc[256] = { 0 };
		//sprintf(imageSrc, "set:%s image:%s_normal", imageSet, imageFile);
		sprintf(imageSrc, "set:%s image:%s_normal", imageSet, manOrWomanImage);
		sprintf(itemImage, itemImageTemplate, imageSrc);
	}
	strcat(descBuff, itemImage);//*/

	//灵石之语
	if ( m_socketSet.GetUseSocketCount() > 0 && GetGenre() != item_target && GetGenre() != item_ib )
	{
		bool bShowBar = false;
		int nP = 0;
		for ( nP = 0 ;nP < MAX_SPECIALEFFECT_COUNT; ++nP )
		{
			int nBuffID = m_InlaySpecialBuffSet[nP];
			if ( nBuffID > 0 )
			{
				bShowBar = true;
				break;
			}
		}

		int useSocket = m_socketSet.GetUseSocketCount();
		int maxSocket = m_socketSet.GetSocketCount();

		if ( bShowBar )
		{
			std::string name;
			m_socketSet.GetInlaySpecialEffectName( this, name );
			if ( !name.empty() )
			{
				strcat( descBuff, name.c_str() );
			}			
		}
		else
		{
			strcat( descBuff, "<Seg text-align=center float=wrap>");
			int nY = 0;
			for ( nY = 0 ;nY < maxSocket; ++nY )
			{
				if ( !m_socketSet.IsEmptyBySocketIdx( nY ) )
				{
					const char* szItemInlayStone = cm.GetConfigurableDisplayStyle( style_item_inlay_stone_name, m_socketSet.GetInlayGroupIDBySocketIdx(nY) );
					if ( szItemInlayStone )
					{
						strcat( descBuff, szItemInlayStone );
					}					
				}
			}
			strcat( descBuff, "</Seg>");
		}
		strcat( descBuff, "<Seg text-align=left float=wrap><Obj> </Obj></Seg>");
	}

	if ( GetGenre() == item_ib || IsExpItem() )
	{
		if (IsExpItem())
		{
			//经验物品
			char capability[COMMON_CLIENT_MSG_LEN_256] = { 0 };
			const char* capabilityTemplate = cm.GetConfigurableDisplayStyle(style_item_capability, 1 );
			if (capabilityTemplate != NULL && GetDurability() >= 0 && GetMaxDurability() > 0 )
			{
				char currentExp[COMMON_CLIENT_MSG_LEN_64] = { 0 };
				PrintReadableNumber(currentExp, sizeof(currentExp), (DWORD)GetDurability(), 4);
				currentExp[sizeof(currentExp) - 1] = 0;
				char maxExp[COMMON_CLIENT_MSG_LEN_64] = { 0 };
				PrintReadableNumber(maxExp, sizeof(maxExp), (DWORD)GetMaxDurability(), 4);
				maxExp[sizeof(maxExp) - 1] = 0;
				sprintf(capability, capabilityTemplate, currentExp, maxExp);
				strcat(descBuff, capability);
			}
		}
		else
		{
			char capability[COMMON_CLIENT_MSG_LEN_256] = { 0 };
			const char* capabilityTemplate = cm.GetConfigurableDisplayStyle(style_item_capability, 0 );
			if (capabilityTemplate != NULL && GetDurability() > 0 && GetMaxDurability() > 0 )
			{
				sprintf(capability, capabilityTemplate, GetDurability(), GetMaxDurability() );
				strcat(descBuff, capability);
			}
		}
	}

	//装备描述
	char itemDesc[SZBUFLEN_1 + SZBUFLEN_0 * 2] = { 0 };
	const char* itemDescTemplate = cm.GetConfigurableDisplayStyle(style_item_desc);
	if (itemDescTemplate != NULL)
	{
		char itemDescBasic[SZBUFLEN_1 + SZBUFLEN_0] = { 0 };
		const char* itemDescBasicTemplate = cm.GetConfigurableDisplayStyle(style_item_desc_basic);
		if (itemDescBasicTemplate != NULL)
		{
			if (strlen(m_pItemTemplate->szIntro) > 0 && 0 != strncmp(m_pItemTemplate->szIntro, "NULL", sizeof(m_pItemTemplate->szIntro)))
				sprintf(itemDescBasic, itemDescBasicTemplate, m_pItemTemplate->szIntro);
		}
		
		char itemDescPlusInfo[SZBUFLEN_0] = { 0 };
		const char* itemDescPlusInfoTemplate = cm.GetConfigurableDisplayStyle(style_item_desc_plusinfo);
		if (itemDescPlusInfoTemplate != NULL)
		{
			if (strlen(m_szPlusInfo) > 0)
				sprintf(itemDescPlusInfo, itemDescPlusInfoTemplate, m_szPlusInfo);
		}

		sprintf(itemDesc, itemDescTemplate, itemDescBasic, itemDescPlusInfo);
	}
	strcat(descBuff, itemDesc);

	if ((item_target == m_pItemTemplate->nItemGenre || item_ib == m_pItemTemplate->nItemGenre ) && levelup_tool_targetitem == m_pItemTemplate->nParticularType)
	{
		//升级属性
		char levelupStr[256] = { 0 };
		const char* upgradeTemplate = NULL;
		if (m_pItemTemplate && m_pItemTemplate->nParticularType == levelup_tool_targetitem )
		{
			bool bNoHaveLevelup = GetLevelupTimes() <= 0 && GetCompoundBuff(COMPOUND_LEVELUP) <= 0 && GetLevelupType() < 0;
			upgradeTemplate =  cm.GetConfigurableDisplayStyle(style_item_levelup_tool_desc, !bNoHaveLevelup);
			if (upgradeTemplate != NULL)
			{
				if (m_UpgradeCount > 0 && m_CompoundBuffIDSet[COMPOUND_LEVELUP] > 0)
				{
					int upgradeBuffID = m_CompoundBuffIDSet[COMPOUND_LEVELUP];
					if ( upgradeBuffID > 0)
					{
						PBAT pBuff = buffTable.GetBuff( upgradeBuffID );
						if ( pBuff != NULL )
						{
							if (!bNoHaveLevelup)
							{
								sprintf(levelupStr, upgradeTemplate, pBuff->szDesc, m_UpgradeCount);
							}							
						}		
					}				
				}
				else
				{
					if (bNoHaveLevelup)
						sprintf(levelupStr, upgradeTemplate);
				}
			}
			strcat(descBuff, levelupStr);
		}
	}
	
	if (item_equip == m_pItemTemplate->nItemGenre//装备
		|| item_charm == m_pItemTemplate->nItemGenre)//护身符
	{
		if (item_equip == m_pItemTemplate->nItemGenre)
		{
			//装备位置
			const char* equipPosTemplate = cm.GetConfigurableDisplayStyle(style_item_equip_pos, m_pItemTemplate->nDetailType);
			if (equipPosTemplate != NULL)
				strcat(descBuff, equipPosTemplate);
		}
		else if (item_charm == m_pItemTemplate->nItemGenre)
		{
			const char* charmTemplate = cm.GetConfigurableDisplayStyle(style_item_charm);
			if (charmTemplate != NULL)
				strcat(descBuff, charmTemplate);
		}

		//基本属性
		char basicProperty[512] = { 0 };
		const char* basicPropertyTemplate = cm.GetConfigurableDisplayStyle(style_item_basic_property);
		const char* basicPropertyBuffTemplate = cm.GetConfigurableDisplayStyle(style_item_basic_property_buff);
		if (basicPropertyTemplate != NULL && basicPropertyBuffTemplate != NULL)
		{
			char basicPropertyBuffList[512] = { 0 };
			for (int basicPropBuffLoopCount = 0; basicPropBuffLoopCount < ITEM_BUFF_COUNT; ++basicPropBuffLoopCount )
			{
				int basicPropBuffID = m_pItemTemplate->BasicBuff.BuffID[basicPropBuffLoopCount];
				if ( basicPropBuffID > 0)
				{
					PBAT pBuff = buffTable.GetBuff( basicPropBuffID );
					if ( pBuff != NULL )
					{
						char basicPropBuff[128] = { 0 };					
						sprintf(basicPropBuff, basicPropertyBuffTemplate, pBuff->szDesc);
						strcat(basicPropertyBuffList, basicPropBuff);
					}
				}
			}
			
			sprintf(basicProperty, basicPropertyTemplate, basicPropertyBuffList);
		}
		strcat(descBuff, basicProperty);

		if (item_equip == m_pItemTemplate->nItemGenre)
		{
			//爻附加属性
			char yaoAddon[512] = { 0 };
			const char* yaoAddonTemplate = cm.GetConfigurableDisplayStyle(style_item_yao_addon);
			const char* yaoAddonBuffTemplate = cm.GetConfigurableDisplayStyle(style_item_yao_addon_buff);
			if (yaoAddonTemplate != NULL && yaoAddonBuffTemplate != NULL)
			{
				char yaoAddonBuffList[512] = { 0 };
				for (int yaoAddonBuffLoopCount = 0; yaoAddonBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddonBuffLoopCount++)
				{
					int yaoAddonBuff = m_YaoAddOnBuffIDSet[yaoAddonBuffLoopCount];
					if (yaoAddonBuff > 0)//有爻装附加属性
					{
						PBAT pBuff = buffTable.GetBuff(yaoAddonBuff);
						if (pBuff != NULL)
						{
							char yaoAddonBuff[128] = { 0 };
							sprintf(yaoAddonBuff, yaoAddonBuffTemplate, pBuff->szDesc);
							strcat(yaoAddonBuffList, yaoAddonBuff);
						}
					}
				}
				
				sprintf(yaoAddon, yaoAddonTemplate, yaoAddonBuffList);
			}
			strcat(descBuff, yaoAddon);
			
			//升级属性
			char upgrade[256] = { 0 };
			const char* upgradeTemplate = cm.GetConfigurableDisplayStyle(style_item_upgrade);
			if (upgradeTemplate != NULL)
			{
				if (m_UpgradeCount > 0 && m_CompoundBuffIDSet[COMPOUND_LEVELUP] > 0)
				{
					int upgradeBuffID = m_CompoundBuffIDSet[COMPOUND_LEVELUP];
					if ( upgradeBuffID > 0)
					{
						PBAT pBuff = buffTable.GetBuff( upgradeBuffID );
						if ( pBuff != NULL )
						{
							sprintf(upgrade, upgradeTemplate, pBuff->szDesc, m_UpgradeCount);
						}		
					}				
				}
			}
			strcat(descBuff, upgrade);
			
			//加持属性
			char addMagic[256] = { 0 };
			const char* addMagicTemplate = cm.GetConfigurableDisplayStyle(style_item_addmagic);
			if (addMagicTemplate != NULL)
			{
				int addMagicBuffID = m_CompoundBuffIDSet[COMPOUND_ADDMAGIC];
				if (addMagicBuffID > 0)
				{
					PBAT pBuff = buffTable.GetBuff(addMagicBuffID);
					if ( pBuff != NULL )
					{
						sprintf(addMagic, addMagicTemplate, pBuff->szDesc);
					}		
				}
			}
			strcat(descBuff, addMagic);

			//耐久，需要按设定的现实比例显示
			char duration[256] = { 0 };
			const char* durationTemplate = cm.GetConfigurableDisplayStyle(style_item_duration, (IsBroken() ? TRUE : FALSE));
			if (durationTemplate != NULL)
			{
				int durDiaplayRatio = AbradeTable::Singleton().GetDisplayRatio();	
				if(durDiaplayRatio <= 0)
					durDiaplayRatio = 1;
				int displayDurCur = m_CurrentDurability / durDiaplayRatio;
				if (displayDurCur == 0 && m_CurrentDurability > 0)
					displayDurCur = 1;
				int displayDurMax = m_MaxDurability / durDiaplayRatio;
				if (displayDurMax == 0 && m_MaxDurability > 0)
					displayDurMax = 1;
				sprintf(duration, durationTemplate, displayDurCur, displayDurMax);
			}
			strcat(descBuff, duration);
		}
	}
	// 镶嵌石头属性
	else if ( GetGenre() == item_target|| (GetGenre() == item_ib && !IsNoTarget()) )
	{
		if ( GetParticular() == inlay_targetitem )
		{
			/*
			//镶嵌分组编号
			const char* inlayGroupTemplate = cm.GetConfigurableDisplayStyle(style_item_inlay_stone_group, GetGroup()> 0 ? GetGroup() : 0 );
			if (inlayGroupTemplate != NULL)
				strcat(descBuff, inlayGroupTemplate);

			//装备品质
			const char* inlayQualityTemplate = cm.GetConfigurableDisplayStyle(style_item_inlay_stone_quality, GetQualityLabel() > 0 ? GetQualityLabel() : 0 );
			if (inlayQualityTemplate != NULL)
				strcat(descBuff, inlayQualityTemplate);

			//装备位置
			for ( int nIdx = 0; nIdx < TARGETITEM_TARGET_COUNT; ++nIdx )
			{
				const char* equipPosTemplate = cm.GetConfigurableDisplayStyle(style_item_inlay_stone_pos, GetTargetWeaponType(nIdx) );
				if (equipPosTemplate != NULL)
				{
					strcat(descBuff, equipPosTemplate);
				}
			}//*/

			//基本属性
			char basicProperty[512] = { 0 };
			const char* basicPropertyTemplate = cm.GetConfigurableDisplayStyle(style_item_basic_property);
			const char* basicPropertyBuffTemplate = cm.GetConfigurableDisplayStyle(style_item_basic_property_buff);
			if (basicPropertyTemplate != NULL && basicPropertyBuffTemplate != NULL)
			{
				char basicPropertyBuffList[512] = { 0 };
				for (int basicPropBuffLoopCount = 0; basicPropBuffLoopCount < ITEM_BUFF_COUNT; ++basicPropBuffLoopCount )
				{
					int basicPropBuffID = m_pItemTemplate->BasicBuff.BuffID[basicPropBuffLoopCount];
					if ( basicPropBuffID > 0)
					{
						PBAT pBuff = buffTable.GetBuff( basicPropBuffID );
						if ( pBuff != NULL )
						{
							char basicPropBuff[128] = { 0 };					
							sprintf(basicPropBuff, basicPropertyBuffTemplate, pBuff->szDesc);
							strcat(basicPropertyBuffList, basicPropBuff);
						}
					}
				}
				
				sprintf(basicProperty, basicPropertyTemplate, basicPropertyBuffList);
			}
			strcat(descBuff, basicProperty);

			for ( int nY = 0 ;nY < MAX_ITEM_INLAY_YAO_EFFECT_COUNT; ++nY )
			{
				int nBuffID = m_InlayYaoBuffSet[nY];
				if ( nBuffID > 0 )
				{
					PBAT pBuff = buffTable.GetBuff(nBuffID);
					if ( pBuff != NULL )
					{
						char szBuff[256];
						const char* szItemInlayYaoAttribute = cm.GetConfigurableDisplayStyle( style_item_inlay_yao_attribute, 0 );
						if (szItemInlayYaoAttribute)
						{
							sprintf( szBuff, szItemInlayYaoAttribute, pBuff->szDesc );
							strcat(descBuff, szBuff);
						}
					}		
				}
			}

			// 镶嵌爻随即概率
			KItemInlayAddOnTable& iiaddonTab = KItemInlayAddOnTable::Singleton();
			const InlayAddOn* iaddOn = iiaddonTab.GetInlayAddOnByGroupAndBuffID( m_pItemTemplate->sYaoGroup, m_InlayYaoBuffSet[0], 0 );
			const char* yaoRateTemplate = cm.GetConfigurableDisplayStyle(style_item_inlay_stone_rate, 0);
			if (yaoRateTemplate != NULL && iaddOn != NULL )
			{
				char szBuff[256];
				sprintf( szBuff, yaoRateTemplate, iaddOn->YinRate, iaddOn->YangRate );
				strcat(descBuff, szBuff);
			}
		}
		else
		{
			// to do nothings.
		}
	}
	else
	{
		// to do nothings.
	}

	//重量
	char weight[256] = { 0 };
	const char* weightTemplate = cm.GetConfigurableDisplayStyle(style_item_weight);
	if (weightTemplate != NULL)
	{

		sprintf(weight, weightTemplate, GetItemWeight());
		strcat(descBuff, weight);
	}	
	
	KPlayer& player = GetClientPlayer();
	KNpc& playerNpc = Npc[player.GetNpcIndex()];

	if (item_charm != m_pItemTemplate->nItemGenre)
	{
		//需求等级
		char requireLevel[256] = { 0 };
		if (m_pItemTemplate->nReqLevel > 0)
		{
			const char* requireLevelTemplate = cm.GetConfigurableDisplayStyle(style_item_require_level, (IsMatchLevelRequirement(player) ? TRUE : FALSE));
			if (requireLevelTemplate != NULL)
			{
				sprintf(requireLevel, requireLevelTemplate, m_pItemTemplate->nReqLevel);
			}
		}
		strcat(descBuff, requireLevel);
		
		//需求职业
		char requireProfessionStr[256] = { 0 };
		const int ProfessionTypeCount = role_skillseries_count * 3;		
		int requireProfession[ProfessionTypeCount];
		int requireProfessionCount = GetProfessionRequirement(requireProfession);		
		if (requireProfessionCount < ProfessionTypeCount)
		{
			const char* professionNames[ProfessionTypeCount] = { XUANFENG_J, XINGTIAN_J, ZHENREN_D, TIANSHI_D, SHOUSHI_S, YISHI_S };
			const char* requireProfessionStyle = cm.GetConfigurableDisplayStyle(style_item_require_profession, (IsMatchProfessionRequirement(player) ? TRUE : FALSE));
			if (requireProfessionStyle != NULL)
			{
				char requireProfessionAllStr[256] = { 0 };
				for(int i = 0; i < 6; i++)
				{
					if (requireProfession[i] > 0)
					{
						strcat(requireProfessionAllStr, professionNames[i]);
						strcat(requireProfessionAllStr, " ");
					}
				}
				
				sprintf(requireProfessionStr, requireProfessionStyle, requireProfessionAllStr);
			}
		}
		strcat(descBuff, requireProfessionStr);
		
		//需求属性
		char requirePropertyStr[512] = { 0 };
		int propertyRequirement[ITEM_PROP_REQ_COUNT];
		bool isMatchProperty[ITEM_PROP_REQ_COUNT];
		int propertyRequirementCount = GetPropertyRequirement(propertyRequirement);
		//按比例（1/1024）显示下列属性
		if (propertyRequirement[1] > 0)
			propertyRequirement[1] >>= 10;
		if (propertyRequirement[3] > 0)
			propertyRequirement[3] >>= 10;
		if (propertyRequirement[4] > 0)
			propertyRequirement[4] >>= 10;
		if (propertyRequirement[5] > 0)
			propertyRequirement[5] >>= 10;
		IsMatchPropertyRequirement(player, isMatchProperty);
		if (propertyRequirementCount > 0)
		{
			const char* propertyNames[ITEM_PROP_REQ_COUNT] = { EQUIP_REQ_LING, EQUIP_REQ_LI, EQUIP_REQ_TI, EQUIP_REQ_SHU, EQUIP_REQ_WA, EQUIP_REQ_MA };			
			for(int property = 0; property < ITEM_PROP_REQ_COUNT; property++)
			{
				if ( propertyRequirement[property] > 0  )
				{				
					const char* requirePropertyStyle = cm.GetConfigurableDisplayStyle(style_item_require_property, (isMatchProperty[property] ? TRUE : FALSE ));
					if (requirePropertyStyle != NULL)
					{
						char requireSingleProperty[256] = { 0 };
						sprintf(requireSingleProperty, requirePropertyStyle, propertyNames[property] , propertyRequirement[property]);
						strcat(requirePropertyStr, requireSingleProperty);
					}
				}
			}
		}
		strcat(descBuff, requirePropertyStr);
		
		//镶嵌属性
		if ( m_socketSet.GetSocketCount() > 0 && GetGenre() != item_target && GetGenre() != item_ib )
		{
			char szInlay[1024] = {0};
			
			strcat( szInlay, "<Seg text-align=left float=wrap><Obj> </Obj></Seg>");
			
			int useSocket = m_socketSet.GetUseSocketCount();
			int maxSocket = m_socketSet.GetSocketCount();
			
			/*
			//镶嵌孔文字显示
			const char* szItemInlayBase = cm.GetConfigurableDisplayStyle( style_item_inlay_base, useSocket >= maxSocket );
			if ( szItemInlayBase )
			{
			sprintf( szInlay, szItemInlayBase, useSocket, maxSocket );
			}
			//*/
			int nX = 0;
			int nY = 0;
			
			//镶嵌孔数图片显示
			for ( nY = 0 ;nY < maxSocket; ++nY )
			{
				if ( m_socketSet.IsEmptyBySocketIdx( nY ) )
				{
					const char* szItemInlayEmptySocket = cm.GetConfigurableDisplayStyle( style_item_inlay_graph_empty, 0 );
					if (szItemInlayEmptySocket)
					{
						//sprintf( szBuff, szItemInlayEmptySocket, m_pItemTemplate->szImageSetName, m_pItemTemplate->szImageName );
						strcat(szInlay, szItemInlayEmptySocket);
					}
				}
				else
				{
					bool bShowBar = false;
					int nP = 0;
					for ( nP = 0 ;nP < MAX_SPECIALEFFECT_COUNT; ++nP )
					{
						int nBuffID = m_InlaySpecialBuffSet[nP];
						if ( nBuffID > 0 )
						{
							bShowBar = true;
							break;
						}
					}
					const char* szItemInlayFullSocket = NULL;
					if ( bShowBar )
					{
						szItemInlayFullSocket = cm.GetConfigurableDisplayStyle( style_item_inlay_graph_full_special, m_socketSet.GetInlayGroupIDBySocketIdx(nY) );
					}
					else
					{
						szItemInlayFullSocket = cm.GetConfigurableDisplayStyle( style_item_inlay_graph_full,  m_socketSet.GetInlayGroupIDBySocketIdx(nY) );
					}
					
					if (szItemInlayFullSocket)
					{
						//sprintf( szBuff, szItemInlayFullSocket, m_pItemTemplate->szImageSetName, m_pItemTemplate->szImageName );
						strcat(szInlay, szItemInlayFullSocket);
					}
				}
			}
			
			strcat( szInlay, "<Seg text-align=left float=wrap><Obj>\n</Obj></Seg>");
			
			for ( nX = 0; nX < MAX_INLAY_COUNT; ++nX )
			{
				for ( nY = 0 ;nY < ITEM_BUFF_COUNT; ++nY )
				{
					int nBuffID = m_InlayBaseBuffSet[nX][nY];
					if ( nBuffID > 0 )
					{
						PBAT pBuff = buffTable.GetBuff(nBuffID);
						if ( pBuff != NULL )
						{
							char szBuff[256];
							const char* szItemInlayBaseAttribute = cm.GetConfigurableDisplayStyle( style_item_inlay_base_attribute, 0 );
							if (szItemInlayBaseAttribute)
							{
								sprintf( szBuff, szItemInlayBaseAttribute, pBuff->szDesc );
								strcat(szInlay, szBuff);
							}
						}		
					}
				}
			}
			for ( nY = 0 ;nY < MAX_ITEM_INLAY_YAO_EFFECT_COUNT; ++nY )
			{
				int nBuffID = m_InlayYaoBuffSet[nY];
				if ( nBuffID > 0 )
				{
					PBAT pBuff = buffTable.GetBuff(nBuffID);
					if ( pBuff != NULL )
					{
						char szBuff[256];
						const char* szItemInlayYaoAttribute = cm.GetConfigurableDisplayStyle( style_item_inlay_yao_attribute, 0 );
						if (szItemInlayYaoAttribute)
						{
							sprintf( szBuff, szItemInlayYaoAttribute, pBuff->szDesc );
							strcat(szInlay, szBuff);
						}
					}		
				}
			}
			for ( nY = 0 ;nY < MAX_SPECIALEFFECT_COUNT; ++nY )
			{
				int nBuffID = m_InlaySpecialBuffSet[nY];
				if ( nBuffID > 0 )
				{
					PBAT pBuff = buffTable.GetBuff(nBuffID);
					if ( pBuff != NULL )
					{
						char szBuff[256];
						const char* szItemInlaySpecialAttribute = cm.GetConfigurableDisplayStyle( style_item_inlay_special_attribute, 0 );
						if (szItemInlaySpecialAttribute)
						{
							sprintf( szBuff, szItemInlaySpecialAttribute, pBuff->szDesc );
							strcat(szInlay, szBuff);
						}
					}		
				}
			}
			
			strcat( szInlay, "<Seg text-align=left float=wrap><Obj> </Obj></Seg>");
			
			strcat(descBuff, szInlay);
	}
	}

	//是否可以出售（卖给系统商人）
	if (!CanSell())
	{
		const char* canNotSellTemplate = cm.GetConfigurableDisplayStyle(style_item_can_not_sell);
		if (canNotSellTemplate)
		{
			strcat(descBuff, canNotSellTemplate);
		}
	}

	//是否可以交易（玩家之间交易和拍卖行出售）
	if (!CanExchange())
	{
		const char* canNotExchangeTemplate = cm.GetConfigurableDisplayStyle(style_item_can_not_exchange);
		if (canNotExchangeTemplate)
		{
			strcat(descBuff, canNotExchangeTemplate);
		}
	}

	//是否可以丢弃（销毁）
	if(!CanDiscard())
	{
		const char* canDiscardTemplate = cm.GetConfigurableDisplayStyle(style_item_can_not_discard);
		if (canDiscardTemplate)
		{
			strcat(descBuff, canDiscardTemplate);
		}
	}

	//是否绑定
	if(IsBind())
	{
		const char* bindTemplate = cm.GetConfigurableDisplayStyle(style_item_bind);
		if (bindTemplate != NULL)
		{
			strcat(descBuff, bindTemplate);
		}
	}
	else
	{
		if(IsEquipBind())
		{
			const char* equipBindTemplate = cm.GetConfigurableDisplayStyle(style_item_equip_bind);
			if (equipBindTemplate != NULL)
			{
				strcat(descBuff, equipBindTemplate);
			}
		}
	}

	//显示在IB商店中的物品要添加已绑定字样
	if ( (item_ib == GetGenre()) && !IsBind() /*&& (2 != GetDetailType()  )*/ )
	{
		/*排除代金券*/
		int ticketGenre = 0, ticketDetail = 0, ticketParticular = 0, ticketLevel = 0;
		cm.GetIBTicketId(&ticketGenre, &ticketDetail, &ticketParticular, &ticketLevel);

		bool isTicket = (GetGenre() == ticketGenre) && (GetDetailType() == ticketDetail) && (GetParticular() == ticketParticular) && (GetLevel() == ticketLevel);
		if ( !isTicket )
		{
			const char* ibItem_isBindTemplate = cm.GetConfigurableDisplayStyle(style_item_equip_bind, 1);
			if ( NULL != ibItem_isBindTemplate )
			{
				strcat(descBuff, ibItem_isBindTemplate);
			}
		}
	}

	//是否有拥有数量限制
	if(GetRestrictCount() > 0)
	{
		if (IsUnique())
		{
			const char* uniqueTemplate = cm.GetConfigurableDisplayStyle(style_item_unique);
			if (uniqueTemplate != NULL)
			{
				strcat(descBuff, uniqueTemplate);
			}
		}
		else
		{
			const char* restrictCountTemplate = cm.GetConfigurableDisplayStyle(style_item_restrict_count);
			if (restrictCountTemplate != NULL)
			{
				char restrictBuff[64] = { 0 };
				sprintf(restrictBuff, restrictCountTemplate, GetRestrictCount());
				strcat(descBuff, restrictBuff);
			}
			
		}
	}

	//死亡掉落类型
	const char* deathDropableTemplate = cm.GetConfigurableDisplayStyle(style_item_can_not_death_drop, GetDeathDropType());
	if (deathDropableTemplate != NULL)
	{
		strcat(descBuff, deathDropableTemplate);
	}

	//是否锁定
	if (IsLocked(-1, false))
	{
		const char* lockedTemplate = cm.GetConfigurableDisplayStyle(style_item_locked);
		if (lockedTemplate != NULL)
		{
			strcat(descBuff, lockedTemplate);
		}
	}

	//记录坐标
	if (m_MapID != -1 &&
		m_MapX != -1 &&
		m_MapY != -1 )
	{
		KIniFile iniFile;
		if ( iniFile.Load( MAP_LIST_SETTING ) && 
			m_MapID >= 0  &&
			m_MapID < MAX_MAP_TEMPLATE )
		{
			char szID[32];
			sprintf( szID, "%d", m_MapID );
			char szMap[64];
			iniFile.GetString( "List", szID, "", szMap, sizeof(szMap) );
			if ( szMap[0] != 0 )
			{
				strcat(descBuff, "<Seg text-align=left float=wrap>");
				const char * pCreditDesc = cm.GetConfigurableDisplayStyle(style_item_map_pos,0);
				if (pCreditDesc)
				{
					char szMapDesc[256] = "";	
					sprintf( szMapDesc, pCreditDesc, szMap, m_MapX >> 5, m_MapY / 64 );
					strcat( descBuff,szMapDesc);
				}
				strcat(descBuff, "</Seg>");
			}
		}
	}//endif
	
	//信用过期时间
	DWORD validTime = m_dwIBBuyDate;
	if(validTime && m_pItemTemplate)
	{
		const time_t curTime = time(NULL);
		const time_t outDateTime = m_dwIBBuyDate + m_pItemTemplate->IBLiveTime;
		char        tempDes[COMMON_CLIENT_MSG_LEN_512] = "";
		
		strcat(descBuff, "<Seg text-align=left float=wrap>");
		
		if (outDateTime > curTime)
		{
			const char * pValidDesc = cm.GetConfigurableDisplayStyle(style_item_out_date_text_tip_des,0);
			tm * pOutDateLocalTime = localtime(&outDateTime);
			if (pValidDesc)
				sprintf(tempDes,pValidDesc,pOutDateLocalTime->tm_year+1900,pOutDateLocalTime->tm_mon + 1,pOutDateLocalTime->tm_mday);
		}//endif
		else
		{
			const char * pInValidDesc = cm.GetConfigurableDisplayStyle(style_item_out_date_text_tip_des,1);
			if (pInValidDesc)
				sprintf(tempDes,pInValidDesc);
		}//end else

		strcat(descBuff, tempDes);
		strcat(descBuff, "</Seg>");
	}//endif
	else
	{
		if ( GetIBAvailabilityTime() > 0 && bIsShopSpecial )
		{
			char validDateShow[COMMON_CLIENT_MSG_LEN_128] = "";
			const char* validDateStyle = NULL;
			
			int liveTime = m_pItemTemplate->IBLiveTime;
			int liveTimeShow = 0;
			if ( liveTime < 60 )	//单位显示秒
			{
				validDateStyle = cm.GetConfigurableDisplayStyle(style_item_valid_date_tip_des, 0);
				liveTimeShow = liveTime;
			}
			else if ( liveTime / 60 < 60 ) //单位显示为分钟
			{
				validDateStyle = cm.GetConfigurableDisplayStyle(style_item_valid_date_tip_des, 1);
				liveTimeShow = liveTime / 60;
			}
			else if ( liveTime / 3600 < 24 ) //单位显示为 小时
			{
				validDateStyle = cm.GetConfigurableDisplayStyle(style_item_valid_date_tip_des, 2);
				liveTimeShow = liveTime / 3600;
			}
			else  //单位显示为天
			{
				validDateStyle = cm.GetConfigurableDisplayStyle(style_item_valid_date_tip_des, 3);
				liveTimeShow = liveTime / (3600 * 24);
			}
			
			if ( NULL != validDateStyle )
			{
				_snprintf(validDateShow,  COMMON_CLIENT_MSG_LEN_128 * sizeof(char), validDateStyle, liveTimeShow);
				validDateShow[COMMON_CLIENT_MSG_LEN_128 - 1] = 0;	
				strcat(descBuff, validDateShow);
			}
		}
	}

	//信用标识
	if (m_CreditFlag && Player[CLIENT_PLAYER_INDEX].GetIBMoney( creditpoint ) > 0 )
	{
		strcat(descBuff, "<Seg text-align=left float=wrap>");
		const char * pCreditDesc = cm.GetConfigurableDisplayStyle(style_item_credit_Property,m_CreditFlag);
		if (pCreditDesc)
			strcat(descBuff, pCreditDesc);
		strcat(descBuff, "</Seg>");
	}//endif

	//道具安全锁
	const time_t curTime = time(NULL);
	const time_t outDateTime = m_dwLockDate;
	char        tempDes[COMMON_CLIENT_MSG_LEN_512] = "";
	if (m_dwLockDate == LOCK_BY_DATA_SIGN )
	{
		strcat(descBuff, "<Seg text-align=left float=wrap>");
		const char * pValidDesc = cm.GetConfigurableDisplayStyle(style_item_safe_lock,1);
		if (pValidDesc)
			sprintf(tempDes,pValidDesc);
		strcat(descBuff, tempDes);
		strcat(descBuff, "</Seg>");
	}
	else if ( m_dwLockDate > 0 && outDateTime > curTime )
	{
		strcat(descBuff, "<Seg text-align=left float=wrap>");
		const char * pValidDesc = cm.GetConfigurableDisplayStyle(style_item_safe_lock,0);
		tm * pOutDateLocalTime = localtime(&outDateTime);
		if (pValidDesc)
			sprintf(tempDes,pValidDesc,pOutDateLocalTime->tm_year+1900,pOutDateLocalTime->tm_mon + 1,pOutDateLocalTime->tm_mday);
		strcat(descBuff, tempDes);
		strcat(descBuff, "</Seg>");
	}

	//国战分期支付
	if ( m_pItemTemplate->nMaxFlushTimes > 0 && m_FlushTimes < m_pItemTemplate->nMaxFlushTimes )
	{
		strcat(descBuff, "<Seg text-align=left float=wrap>");
		const char * pValidDesc = cm.GetConfigurableDisplayStyle(style_item_flush_time_desc,0);
		if (pValidDesc)
		{
			int nLeftTimes = m_pItemTemplate->nMaxFlushTimes - m_FlushTimes;

			sprintf(tempDes,pValidDesc,nLeftTimes);
			strcat(descBuff, tempDes);
			strcat(descBuff, "</Seg>");

		}//endif

	}//endif

}

void KItem::getEquipCompareTitalLayoutStyle(char* layoutText)
{
	ConfigManager& cm = ConfigManager::Singleton();
	const char* text = cm.GetConfigurableDisplayStyle(style_equip_compare_text);
	const char* font = cm.GetConfigurableDisplayStyle(style_equip_compare_font);
	const char* color = cm.GetConfigurableDisplayStyle(style_equip_compare_color);

	char tempText[COMMON_CLIENT_MSG_LEN_256];
	sprintf(tempText, "<Obj type=text %s %s>%s</Obj>", font, color, text);
	strcat(layoutText, "<Seg text-align=center float=wrap>");
	strcat(layoutText, tempText);
	strcat(layoutText, "</Obj></Seg>");
}

#else

void KItem::SyncItem( int nNetConnectIdx, ItemPos& pos,  enumItemSyncType syncType )
{
	if ( m_pItemTemplate == NULL )
	{
		return;
	}

	//服务器端发送消息告诉客户端新增一个物品
	char sendBuff[ITEM_SYNC_BUFF_LENGTH];
	memset(sendBuff, 0, sizeof(sendBuff));
	ITEM_SYNC* pItemSync = (ITEM_SYNC*)sendBuff;
	
	pItemSync->Protocol			= s2c_syncitem;
	pItemSync->Length			= sizeof(ITEM_SYNC) - 1;
	pItemSync->SyncType			= syncType;

	//物品类型
	pItemSync->m_ID				= GetID();													
	pItemSync->m_Genre			= (BYTE)GetGenre();
	pItemSync->m_Detail			= (WORD)GetDetailType();
	pItemSync->m_Particur		= GetParticular();
	pItemSync->m_Level			= GetLevel();
	//位置
	pItemSync->m_btPlace			= pos.nPlace;
	pItemSync->m_btX				= pos.nX;
	pItemSync->m_btY				= pos.nY;
	//耐久
	pItemSync->m_Durability		= GetDurability();
	pItemSync->m_MaxDurability	= GetMaxDurability();
	//数量
	pItemSync->m_btItemCount		= (unsigned short)GetItemCount();
	//升级、爻信息
	pItemSync->m_nLevelupType	= GetLevelupType();
	pItemSync->m_LevelupTimes	= GetLevelupTimes();
	pItemSync->m_YaoID			= (BYTE)GetYaoID();
	pItemSync->m_IsBind			= IsBind();
	pItemSync->m_LockCount		= GetLockCount();

	int useMaxCount			= GetMaxSocketCount();
	for ( int nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
	{
		InlayStuff stuff;
		
		GetInlayStuffBySocketIdx( nIdx, stuff );

		if ( nIdx < useMaxCount )
		{
			pItemSync->m_socketSet[nIdx].nGenre		= stuff.nGenre;
			pItemSync->m_socketSet[nIdx].nDetail		= stuff.nDetail;
			pItemSync->m_socketSet[nIdx].nParticular	= stuff.nParticular;
			pItemSync->m_socketSet[nIdx].nLevel		= stuff.nLevel;
		}
		else
		{
			pItemSync->m_socketSet[nIdx].nGenre		= -1;
			pItemSync->m_socketSet[nIdx].nDetail		= -1;
			pItemSync->m_socketSet[nIdx].nParticular	= -1;
			pItemSync->m_socketSet[nIdx].nLevel		= -1;
		}
	}
	memcpy( &pItemSync->m_InlayBaseBuffSet, GetInlayBaseBuffSet(), sizeof(pItemSync->m_InlayBaseBuffSet) );
	memcpy( &pItemSync->m_InlayYaoBuffSet, GetInlayYaoBuffSet(), sizeof(pItemSync->m_InlayYaoBuffSet) );
	memcpy( &pItemSync->m_InlaySpecialBuffSet, GetInlaySpecialBuffSet(), sizeof(pItemSync->m_InlaySpecialBuffSet) );

	for (int yaoAddOnBuffLoopCount = 0; yaoAddOnBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddOnBuffLoopCount++)
	{
		pItemSync->m_YaoAddOnBuffSet[yaoAddOnBuffLoopCount] = GetYaoAddOn(yaoAddOnBuffLoopCount);
	}
	memcpy( pItemSync->m_compBuffTemplateSet, GetCompBuffTemplateSet(), sizeof(WORD) * COMPOUND_COUNT ); 
	memcpy( pItemSync->m_szPlusInfo, getPlusInfo(), ITEM_PLUS_INFO_LEN ); 
	pItemSync->m_TalismanPotential = GetTalismanPotential();
	for (int talismanEnchaseLoopCount = 0; talismanEnchaseLoopCount < TM_HOLE_NUM; talismanEnchaseLoopCount++)
	{
		pItemSync->m_TalismanEnchaseSet[talismanEnchaseLoopCount] = GetTalismanEnchase(talismanEnchaseLoopCount);
	}
	pItemSync->m_dwIBBuyTime		= GetIBBuyData();

	pItemSync->m_CreditFlag = GetCreditFlag();                                  //信贷标识
	GetPosInfo(pItemSync->m_MapID, pItemSync->m_MapX, pItemSync->m_MapY );
	pItemSync->m_Step = GetStep();

	if (CompressProtocol((BYTE*)sendBuff, pItemSync->Length + 1, sizeof(sendBuff)))
	{	
		if (g_pServer != NULL)
			g_pServer->PackDataToClient( nNetConnectIdx, (BYTE*)sendBuff, pItemSync->Length + 1 );
	}
}

void KItem::SyncItemRefresh( int nNetConnectIdx )
{
	int nTargetIdx = ItemSet.SearchID( m_ID );
	if ( nTargetIdx >= 0 && nTargetIdx < MAX_ITEM )
	{
		char sendBuff[ITEM_SYNC_BUFF_LENGTH];
		memset(sendBuff, 0, sizeof(sendBuff));
		ITEM_REFRESH* pItemRefreshSync = (ITEM_REFRESH*)sendBuff;

		pItemRefreshSync->Protocol			= s2c_refreshitem;
		pItemRefreshSync->Length			= sizeof(ITEM_REFRESH) - 1;
		pItemRefreshSync->m_nId				= Item[nTargetIdx].GetID();
		pItemRefreshSync->m_btItemCount		= Item[nTargetIdx].GetItemCount();
		pItemRefreshSync->m_nAddMagicBuff	= Item[nTargetIdx].GetCompoundBuff( COMPOUND_ADDMAGIC );

		int useMaxCount = Item[nTargetIdx].GetMaxSocketCount();
		for ( int nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
		{
			InlayStuff stuff;
			
			Item[nTargetIdx].GetInlayStuffBySocketIdx( nIdx, stuff );

			if ( nIdx < useMaxCount )
			{
				pItemRefreshSync->m_socketSet[nIdx].nGenre		= stuff.nGenre;
				pItemRefreshSync->m_socketSet[nIdx].nDetail		= stuff.nDetail;
				pItemRefreshSync->m_socketSet[nIdx].nParticular	= stuff.nParticular;
				pItemRefreshSync->m_socketSet[nIdx].nLevel		= stuff.nLevel;
			}
			else
			{
				pItemRefreshSync->m_socketSet[nIdx].nGenre		= -1;
				pItemRefreshSync->m_socketSet[nIdx].nDetail		= -1;
				pItemRefreshSync->m_socketSet[nIdx].nParticular	= -1;
				pItemRefreshSync->m_socketSet[nIdx].nLevel		= -1;
			}
		}
		memcpy( &pItemRefreshSync->m_InlayBaseBuffSet, Item[nTargetIdx].GetInlayBaseBuffSet(), sizeof(pItemRefreshSync->m_InlayBaseBuffSet) );
		memcpy( &pItemRefreshSync->m_InlayYaoBuffSet, Item[nTargetIdx].GetInlayYaoBuffSet(), sizeof(pItemRefreshSync->m_InlayYaoBuffSet) );
		memcpy( &pItemRefreshSync->m_InlaySpecialBuffSet, Item[nTargetIdx].GetInlaySpecialBuffSet(), sizeof(pItemRefreshSync->m_InlaySpecialBuffSet) );

		if (CompressProtocol((BYTE*)sendBuff, pItemRefreshSync->Length + 1, sizeof(sendBuff)))
		{
			if (g_pServer)
				g_pServer->PackDataToClient(nNetConnectIdx, sendBuff, pItemRefreshSync->Length + 1);

			SyncAttribute( item_attr_yao_id, nNetConnectIdx );
		}
	}
}

void KItem::SyncAttribute(enumItemAttribute attr, int connectionIndex)
{
	if (attr < 0 || attr >= item_attr_count || connectionIndex < 0)
		return;

	if (attr == item_attr_talisman_enchase)
	{
		SyncTalismanEnchase(connectionIndex);
	}
	else
	{
		SYNC_ITEM_ATTR syncAttr;
		syncAttr.Protocol = s2c_sync_item_attr;
		syncAttr.ItemID = GetID();
		syncAttr.Attribute = attr;
		
		DWORD val = 0;
		switch(attr)
		{
		case item_attr_durability:
			val = GetDurability();
			break;
		case item_attr_max_durability:
			val = GetMaxDurability();
			break;
		case item_attr_stack_count:
			val = GetItemCount();
			break;
		case item_attr_yao_id:
			val = GetYaoID();
			break;
		case item_attr_upgrade_type:
			val = GetLevelupType();
			break;
		case item_attr_upgrade_count:
			val = GetLevelupTimes();
			break;
		case item_attr_upgrade_buffid:
			val = GetCompoundBuff(COMPOUND_LEVELUP);
			break;
		case item_attr_talisman_potential:
			val = GetTalismanPotential();
			break;
		case item_attr_isbind:
			val = IsBind();
			break;
		case item_attr_islocked:
			val = m_LockCount;
			break;
		case item_attr_buytime:
			val = m_dwIBBuyDate;
			break;
		case item_attr_credit_flag:
			val = m_CreditFlag;
			break;
		case item_attr_step:
			val = GetStep();
			break;
		case item_attr_mapid:
			val = m_MapID;
			break;
		case item_attr_mapx:
			val = m_MapX;
			break;
		case item_attr_mapy:
			val = m_MapY;
			break;
		case item_attr_is_task_given:
			val = IsTaskGiven();
			break;
		case item_attr_lockdate:
			val = m_dwLockDate;
			break;
		case item_attr_flush_times:
			val = m_FlushTimes;
			break;

		default:
			_ASSERT(false);
			break;
		}
		
		syncAttr.Value = val;
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(connectionIndex, (BYTE*)&syncAttr, sizeof(SYNC_ITEM_ATTR));
	}
}

void KItem::SyncTalismanEnchase(int connectionIndex)
{
	SYNC_TALISMAN_ENCHASE syncTalismanEnchase;
	syncTalismanEnchase.Protocol = s2c_sync_talisman_enchase;
	syncTalismanEnchase.ItemId = GetID();
	memcpy(syncTalismanEnchase.EnchaseSet, m_TalismanEnchaseSet, sizeof(m_TalismanEnchaseSet));	

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(connectionIndex, &syncTalismanEnchase, sizeof(syncTalismanEnchase));
}


BOOL KItem::UseItem( int nPlayerIdx, int nItemIdx, int nTargetIdx )
{
	if ( m_ItemStackCount <= 0 && !CanntDisappear()  )
	{
		return FALSE;
	}
	BOOL bRet = FALSE;

	//脚本物品不走普通流程
	if ((m_pItemTemplate->nItemGenre == item_ib ||
		m_pItemTemplate->nItemGenre == item_medicine) &&
		m_pItemTemplate->scriptFile &&
		m_pItemTemplate->scriptFile[0] != '0')
	{
		DWORD dwScriptId = g_FileName2Id(m_pItemTemplate->scriptFile);
		if ( IsStepUseItem() )//交互脚本
		{
			ScriptSetItemIndex( GetItemIndex() );
			BOOL bOk = Player[nPlayerIdx].ExecuteInteractiveScript(dwScriptId, "UseInteractiveItem", GetGenre(), GetDetailType(), GetID());
			if ( bOk )
			{
				int nStrId = 11372;
				g_ChatCenterS.SysMsgToSomeone(nPlayerIdx, SYSMSG_TYPE_ID, (const BYTE*)&nStrId, sizeof(nStrId));

				return FALSE;
			}
		}
		else if(IsNormalUseItem())//直接脚本
		{
			ScriptSetItemIndex( GetItemIndex() );
			BOOL bOk = Player[nPlayerIdx].ExecuteScript3Param(dwScriptId, "UseItem", 0, GetGenre(), GetDetailType(), GetID() );
			if ( bOk )
			{
				return FALSE;
			}			
		}

	}

	BuffMgr& rBuffManger = BuffMgr::Singleton( );

	int nNpcIndex = Player[nPlayerIdx].GetNpcIndex( );

	if ( (m_pItemTemplate->nItemGenre == item_target || (m_pItemTemplate->nItemGenre == item_ib && !IsNoTarget() )) && nTargetIdx > 0 )
	{
		if ( IsItemTarget() )
		{
			char str[TARGETITEM_TARGET_COUNT][COMMON_CLIENT_MSG_LEN_64];
			ZeroMemory( str, sizeof(char) * TARGETITEM_TARGET_COUNT * COMMON_CLIENT_MSG_LEN_64 );
			char szTemp[SZBUFLEN_0] = {0};
			strncpy( szTemp,m_pItemTemplate->szBuffTarget, SZBUFLEN_0);
			_getItemTarget( szTemp, str );

			int errorCode = item_inlay_error_targetitem_rule;
			for ( int nIdx = 0; nIdx < TARGETITEM_TARGET_COUNT; ++nIdx )
			{
				if ( str[nIdx][0] == NULL )
				{
					continue;
				}
				int nGenre = 0; int nDetailType = 0;int nParticularType = 0; int nLevel = 0; int nCount = 0; int nQuality = 0; int nYao = 0;
				::sscanf(str[nIdx], "%d|%d|%d|%d|%d|%d|%d", 
					&(nGenre), &(nDetailType), &(nParticularType), &(nLevel), &(nCount), &(nQuality), &(nYao));
				KCompoundRules::KSrcItemDescriptor srcItemDesc;
				srcItemDesc.nItemGenre		= nGenre;
				srcItemDesc.nItemDetail		= nDetailType;
				srcItemDesc.nItemParticular	= nParticularType;
				srcItemDesc.nItemLevel		= nLevel;
				srcItemDesc.nItemCount		= nCount;
				srcItemDesc.nItemQuality	= nQuality;
				srcItemDesc.nItemYao		= nYao;
				srcItemDesc.nItemColor		= -1;
				if ( g_CompoundRule.CheckItem( &Item[nTargetIdx], srcItemDesc ) && m_ItemStackCount >= 1 )
				{
					if ( m_pItemTemplate->nParticularType == levelup_tool_targetitem )
					{
						int targetCompoundLevelupType = Item[nTargetIdx].GetLevelupType();
						int targetCompoundLevelupTimes = Item[nTargetIdx].GetLevelupTimes();
						int targetCompoundLevelupBuff = Item[nTargetIdx].GetCompoundBuff( COMPOUND_LEVELUP );

						ConfigManager& cm = ConfigManager::Singleton();
						int leftLevelupTooLimit = 0;
						int rightLevelupTooLimit = 0;
						cm.GetItemLevelupToolLimit( m_pItemTemplate->nLevel, leftLevelupTooLimit, rightLevelupTooLimit );
						if ( GetLevelupTimes() <= 0 && GetCompoundBuff(COMPOUND_LEVELUP) <= 0 && GetLevelupType() < 0 )
						{

							if ( targetCompoundLevelupType < 0 ||
								targetCompoundLevelupTimes <= 0 ||
								targetCompoundLevelupBuff <= 0 )
							{
								return FALSE;
							}

							if ( !(targetCompoundLevelupTimes >= leftLevelupTooLimit && targetCompoundLevelupTimes <= rightLevelupTooLimit ) )
							{
								return FALSE;
							}

							SetLevelupType( targetCompoundLevelupType );
							SetLevelupTimes( targetCompoundLevelupTimes );
							SetCompoundBuff( COMPOUND_LEVELUP, targetCompoundLevelupBuff );
							SyncAttribute( item_attr_upgrade_type,Player[nPlayerIdx].m_nNetConnectIdx  );
							SyncAttribute( item_attr_upgrade_count,Player[nPlayerIdx].m_nNetConnectIdx  );
							SyncAttribute( item_attr_upgrade_buffid,Player[nPlayerIdx].m_nNetConnectIdx  );

							Item[nTargetIdx].ClearCompoundBuff(COMPOUND_LEVELUP);
							Item[nTargetIdx].SyncAttribute( item_attr_upgrade_type,Player[nPlayerIdx].m_nNetConnectIdx  );
							Item[nTargetIdx].SyncAttribute( item_attr_upgrade_count,Player[nPlayerIdx].m_nNetConnectIdx  );
							Item[nTargetIdx].SyncAttribute( item_attr_upgrade_buffid,Player[nPlayerIdx].m_nNetConnectIdx  );
							if ( !IsBind() )
							{
								SetBind(true);
								SyncAttribute( item_attr_isbind,Player[nPlayerIdx].m_nNetConnectIdx  );
							}


							return FALSE;
						}
						else
						{
							if ( GetLevelupTimes() > targetCompoundLevelupTimes )
							{
								Item[nTargetIdx].SetLevelupType( GetLevelupType() );
								Item[nTargetIdx].SetLevelupTimes( GetLevelupTimes() );
								Item[nTargetIdx].SetCompoundBuff( COMPOUND_LEVELUP, GetCompoundBuff( COMPOUND_LEVELUP ) );
								Item[nTargetIdx].SyncAttribute( item_attr_upgrade_type,Player[nPlayerIdx].m_nNetConnectIdx  );
								Item[nTargetIdx].SyncAttribute( item_attr_upgrade_count,Player[nPlayerIdx].m_nNetConnectIdx  );
								Item[nTargetIdx].SyncAttribute( item_attr_upgrade_buffid,Player[nPlayerIdx].m_nNetConnectIdx  );

								if ( !Item[nTargetIdx].IsBind() )
								{
									Item[nTargetIdx].SetBind(true);
									Item[nTargetIdx].SyncAttribute( item_attr_isbind,Player[nPlayerIdx].m_nNetConnectIdx  );
								}

								errorCode = item_inlay_error_levelup_tool_ok;
								bRet = TRUE;
							}
							else
							{
								return  FALSE;
							}							
						}						
					}
					else if ( m_pItemTemplate->nParticularType == normal_targetitem || 
						(m_pItemTemplate->nParticularType == reset_yao_targetitem &&
						m_pItemTemplate->nLevel == Item[nTargetIdx].GetMaxSocketCount() ) ||
						m_pItemTemplate->nParticularType == repair_targetitem )
					{
						if ( m_pItemTemplate->nParticularType == repair_targetitem && 
							(Item[nTargetIdx].GetLevelRequirement() > m_pItemTemplate->nLevel ||
							Item[nTargetIdx].CanBeRepaired() == false ) )
						{
							return FALSE;
						}

						if ( m_pItemTemplate->btBuffSwitch )
						{							
							int nRet = false;
							int i = 0; 
							for ( i = 0; i < ITEM_BUFF_COUNT; ++i )
							{
								int nBuffTemplateID = m_pItemTemplate->BasicBuff.BuffID[i];
								if ( nBuffTemplateID > 0)
								{
									BUFF_PARAM Param;
									Param = nTargetIdx;
									unsigned long buffIndex = 
										rBuffManger.AddNpcBuff( 
										nNpcIndex, 
										nNpcIndex, 
										nBuffTemplateID, 
										&Param );
									
									nRet = buffIndex ? TRUE : FALSE;
								}
							}
							bRet = nRet;

							if ( bRet == TRUE )
							{
								errorCode = item_inlay_error_count;
							}							
						}
						else
						{
							Item[nTargetIdx].AddCompoundBuff( COMPOUND_ADDMAGIC, 0, GetBasicBuffID().BuffID[0] );
							Item[nTargetIdx].SyncItemRefresh(Player[nPlayerIdx].m_nNetConnectIdx );
							bRet = TRUE;
							errorCode = item_inlay_error_count;
						}
						break;
					}
					else if ( m_pItemTemplate->nParticularType == inlay_targetitem )
					{
						errorCode = Item[nTargetIdx].InlaySocket( m_pItemTemplate, m_InlayYaoBuffSet[0] );						
						if ( errorCode < item_inlay_error )
						{
							Item[nTargetIdx].SyncItemRefresh(Player[nPlayerIdx].m_nNetConnectIdx );
							bRet = TRUE;
						}
						else
						{							
							bRet = FALSE;
						}
						break;
					}
					else if ( m_pItemTemplate->nParticularType == reset_inlay_targetitem )
					{
						errorCode = item_inlay_ok_normal;
						if ( m_pItemTemplate->InlayDesc[0] != 0 && 
							strcmp( m_pItemTemplate->InlayDesc, "0" ) == 0 )
						{
							errorCode = Item[nTargetIdx].RandomSocket( NULL );
						}
						else
						{
							errorCode = Item[nTargetIdx].RandomSocket( m_pItemTemplate->InlayDesc );
						}
						if ( errorCode < item_inlay_error )
						{
							Item[nTargetIdx].SyncItemRefresh(Player[nPlayerIdx].m_nNetConnectIdx );
							bRet = TRUE;
						}
						else
						{
							bRet = FALSE;
						}
					}
					else
					{
						if (m_pItemTemplate->nParticularType == reset_yao_targetitem &&
							m_pItemTemplate->nLevel != Item[nTargetIdx].GetMaxSocketCount() )
						{
							ItemErrCodeToClient( nPlayerIdx, item_inlay_error_targetitem_rule );
						}
						return FALSE;
					}
				}
			}

			if ( errorCode >= 0 && errorCode < item_inlay_error_count )
			{
				ItemErrCodeToClient( nPlayerIdx, errorCode );
			}
			
		}
		else if ( IsPlayerTarget() )
		{
			int nRet = false;
			int i = 0; 
			for ( i = 0; i < ITEM_BUFF_COUNT; ++i )
			{
				int nBuffTemplateID = m_pItemTemplate->BasicBuff.BuffID[i];
				if ( nBuffTemplateID > 0 )
				{
					BUFF_PARAM Param;
					Param = nItemIdx;
					unsigned long buffIndex = 
					rBuffManger.AddNpcBuff( 
						nNpcIndex, 
						nTargetIdx, 
						nBuffTemplateID, 
						&Param );

					nRet = buffIndex ? TRUE : FALSE;
				}
			}
			bRet = nRet;
		}
	}

	if ((m_pItemTemplate->nItemGenre == item_ib && IsNoTarget() ) ||
		m_pItemTemplate->nItemGenre == item_medicine || 
		m_pItemTemplate->nItemGenre == item_horse )
	{
		int nRet = FALSE;
		int i = 0; 
		for ( i = 0; i < ITEM_BUFF_COUNT; ++i )
		{
			int nBuffTemplateID = m_pItemTemplate->BasicBuff.BuffID[i];
			if ( nBuffTemplateID > 0 )
			{
				BUFF_PARAM Param;
				Param = nItemIdx;
				unsigned long buffIndex = 
				rBuffManger.AddNpcBuff( 
					nNpcIndex, 
					nNpcIndex, 
					nBuffTemplateID, 
					&Param );

				nRet = buffIndex ? TRUE : FALSE;
			}
		}

		bRet = nRet;
	}

	//消耗蕴魂
	DWORD lPlayerSkillPoint		= Player[nPlayerIdx].GetSkillExp();
	if ( bRet && m_pItemTemplate && m_pItemTemplate->CostSkillExp > 0 )
	{
		if ( lPlayerSkillPoint >= m_pItemTemplate->CostSkillExp )
		{
			Player[nPlayerIdx].SetSkillExp( lPlayerSkillPoint - m_pItemTemplate->CostSkillExp );
			Player[nPlayerIdx].SyncAttribute(attr_SkillExp);
			bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}		
	}

	if ( bRet && !CanntDisappear() )
	{
		( ( m_pItemTemplate->nStack >= 0 ) && ( m_ItemStackCount >= 1 ) ) ? m_ItemStackCount-- : 0 ;
	}

	return bRet;
}

int	KItem::CastSkill( int nSkillID, int nPlayerIdx, int nTargetIdx )
{
	int nRet = FALSE;
	KSkill* pSkill = g_SkillManager.GetSkill( nSkillID, 1 );
	if ( pSkill )
	{
		nRet = pSkill->Cast( nPlayerIdx,  -1, nTargetIdx );
	}
	return nRet;
}

int	KItem::ExecuteScript( int nPlayerIdx, const char* szScript )
{
	int nRet = FALSE;
	if ( nPlayerIdx > 0 && szScript != NULL )
	{
		//nRet = 	Player[nPlayerIdx].ExecuteScript( (char*)szScript );
	}
	return nRet;
}


void KItem::ItemErrCodeToClient(int nPlayerIdx, int nErrCode)
{
	CHAT_ERR_CODE	data;

	data.protocol.protocol = s2c_chat_family;
	data.protocol.subProtocol = chat_itemerrcode;
	data.errorCode = (short int)nErrCode;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &data, data.protocol.len + PROTOCOL_SIZE);
}

#endif

bool KItem::CanCombine(const KItem& combineItem)
{
	if(combineItem.GetMaxItemCount() <= 1 && GetMaxItemCount() <= 1)
	{
		return false;
	}

	return (m_pItemTemplate == combineItem.m_pItemTemplate
		&& m_CurrentDurability == combineItem.m_CurrentDurability
		&& m_MaxDurability == combineItem.m_MaxDurability
		&& m_YaoID == combineItem.m_YaoID
		&& m_UpgradeType == combineItem.m_UpgradeType
		&& m_UpgradeCount == combineItem.m_UpgradeCount
		&& m_TalismanPotential == combineItem.m_TalismanPotential
		&& m_IsBind == combineItem.m_IsBind
		&& m_LockCount == combineItem.m_LockCount
		&& m_ActionTime == combineItem.m_ActionTime
		&& memcmp(m_YaoAddOnBuffIDSet, combineItem.m_YaoAddOnBuffIDSet, sizeof(m_YaoAddOnBuffIDSet)) == 0
		&& memcmp(m_CompoundBuffIDSet, combineItem.m_CompoundBuffIDSet, sizeof(m_CompoundBuffIDSet)) == 0
		&& memcmp(m_TalismanEnchaseSet, combineItem.m_TalismanEnchaseSet, sizeof(m_TalismanEnchaseSet)) == 0
		&& memcmp(m_szPlusInfo, combineItem.m_szPlusInfo, sizeof(m_szPlusInfo)) == 0
		&& memcmp(m_InlayBaseBuffSet, combineItem.m_InlayBaseBuffSet, sizeof(m_InlayBaseBuffSet) ) == 0
		&& memcmp(m_InlayYaoBuffSet, combineItem.m_InlayYaoBuffSet, sizeof(m_InlayYaoBuffSet) ) == 0
		&& memcmp(m_InlaySpecialBuffSet, combineItem.m_InlaySpecialBuffSet, sizeof(m_InlaySpecialBuffSet) ) == 0
		&& m_dwIBBuyDate == combineItem.m_dwIBBuyDate 
		&& m_dwLockDate == combineItem.m_dwLockDate
		&& m_CreditFlag == combineItem.m_CreditFlag
		&& !IsIBCountTimelimitItem() 
		&& m_IsTaskGiven == combineItem.IsTaskGiven()
		&& m_FlushTimes  == combineItem.GetFlushTimes()
		);
}

bool KItem::isDefaultItem()
{
	if(m_YaoID != yao_invalid 
		|| m_UpgradeCount != 0
		|| m_UpgradeType != -1
		|| m_TalismanPotential != 0
		|| m_IsBind != false
		|| m_szPlusInfo[0] != 0)
	{
		return false;
	}
	
	for(int i = 0; i < TM_HOLE_NUM; ++i)
	{
		if(m_TalismanEnchaseSet[i] != -1)
		{
			return false;
		}
	}

	for(int j = 0; j < COMPOUND_COUNT; ++j)
	{
		if(m_CompoundBuffIDSet[j] != 0)
		{
			return false;
		}
	}

	for(int k = 0; k < YAO_ADDON_BUFF_COUNT; ++k)
	{
		if(m_YaoAddOnBuffIDSet[k] != 0)
		{
			return false;
		}
	}

	int nIdx = 0;
	int nIdy = 0;

	for ( nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
	{
		for ( nIdy = 0; nIdy < ITEM_BUFF_COUNT; ++nIdy )
		{
			if ( m_InlayBaseBuffSet[nIdx][nIdy] != 0 )
			{
				return false;
			}
		}
	}

	for ( nIdx = 0; nIdx < MAX_ITEM_INLAY_YAO_EFFECT_COUNT; ++nIdx  )
	{
		if (m_InlayYaoBuffSet[nIdx] != 0 )
		{
			return false;
		}
	}

	for ( nIdx = 0; nIdx < MAX_SPECIALEFFECT_COUNT; ++nIdx  )
	{
		if (m_InlaySpecialBuffSet[nIdx] != 0 )
		{
			return false;
		}
	}

	if ( GetMaxSocketCount() > 0 )
	{
		return false;
	}

	return true;
}

int KItem::RandomSocket( char* szDesc )
{
	char socketDesc[INLAYDESC];
	if ( szDesc == NULL )
	{
		strncpy( socketDesc, m_pItemTemplate->InlayDesc, sizeof( m_pItemTemplate->InlayDesc ) );
	}
	else
	{
		strncpy( socketDesc, szDesc, sizeof( socketDesc ) );
	}	
	ConfigManager& cm = ConfigManager::Singleton();

	int maxCount = cm.GetGlobalVariable( global_var_item_inlay_max_socket_count );
	int maxSocketCount = maxCount < MAX_INLAY_COUNT ? maxCount : MAX_INLAY_COUNT;
	if ( socketDesc[0] != 0 )
	{
		m_socketSet.DestroySocket();
		int socketArray[MAX_INLAY_COUNT];
		memset( socketArray, 0, sizeof(socketArray) );
		int socketCount = 0;
		GetSocketParam( socketDesc, &socketCount, (int *)socketArray, MAX_INLAY_COUNT );
		for ( int nIdx = 0; nIdx < socketCount; ++nIdx )
		{
			if ( g_RandPercent(socketArray[nIdx]) && 
				GetMaxSocketCount() < maxSocketCount )
			{
				m_socketSet.CreateSocket();
			}
		}
		::memset(m_InlayBaseBuffSet, 0, sizeof(m_InlayBaseBuffSet) );
		::memset(m_InlayYaoBuffSet, 0, sizeof(m_InlayYaoBuffSet) );
		::memset(m_InlaySpecialBuffSet, 0, sizeof(m_InlaySpecialBuffSet) );
	}

	return item_inlay_ok_reset;
}

int KItem::InlaySocket( const KBASICPROP_ITEM* itemTemplate, short buffID )
{
	
	if ( m_socketSet.GetSocketCount() <= 0 )
	{
		return item_inlay_error_empty;
	}

	if ( m_socketSet.GetUseSocketCount() >= m_socketSet.GetSocketCount() )
	{
		return item_inlay_error_full;
	}
	int errorCode = item_inlay_error;
	if ( itemTemplate )
	{
		errorCode = item_inlay_ok_normal;
		InlayStuff inlayStuff;
		inlayStuff.nGenre = itemTemplate->nItemGenre;
		inlayStuff.nDetail = itemTemplate->nDetailType;
		inlayStuff.nParticular = itemTemplate->nParticularType;
		inlayStuff.nLevel = itemTemplate->nLevel;
		int nSocketIdx = m_socketSet.GetUseSocketCount();
		m_socketSet.SetInlayStuffBySocketIdx( nSocketIdx, inlayStuff );		

		int nIdx = 0;

		// 镶嵌灵石固有属性
		InlayEffect baseEffect[ITEM_BUFF_COUNT];
		m_socketSet.GetInlayEffectBySocketIdx( nSocketIdx, (InlayEffect*)&baseEffect );
		for ( nIdx = 0; nIdx < ITEM_BUFF_COUNT; ++nIdx )
		{
			m_InlayBaseBuffSet[nSocketIdx][nIdx] = baseEffect[nIdx].nBuffID;
		}

		// 镶嵌灵石爻及爻随机属性
		int nYao = 0;
		m_socketSet.GetInlayYaoBySocketIdxAndBuffID( nSocketIdx, buffID,  &nYao );
		if ( nYao != yao_invalid )
		{
			SetYaoID( nYao );
			if ( GetYaoID() == yao_yang )
			{
				errorCode = item_inlay_ok_yang;
			}
			else
			{
				errorCode = item_inlay_ok_yin;
			}
		}	
		if ( nSocketIdx >= 0 && nSocketIdx < MAX_ITEM_INLAY_YAO_EFFECT_COUNT )
		{
			m_InlayYaoBuffSet[nSocketIdx] = buffID;
		}			
				
		// 镶嵌灵石组合属性
		InlayEffect specialEffect[MAX_SPECIALEFFECT_COUNT];
		bool bSpecialEffectRight = m_socketSet.GetInlaySpecialEffect( this, (InlayEffect*)&specialEffect );
		if ( bSpecialEffectRight )
		{
			for ( nIdx = 0; nIdx < MAX_SPECIALEFFECT_COUNT; ++nIdx )
			{
				if ( specialEffect[nIdx].nBuffID > 0 )
				{
					errorCode = item_inlay_ok_special;
					m_InlaySpecialBuffSet[nIdx] = specialEffect[nIdx].nBuffID;
				}			
			}
		}

	}
	return errorCode;
}

void KItem::GetItemtransfersData(TItemtransfersData& rData )
{
#ifdef _SERVER
	rData.Guid					= m_GUID;
#endif
	memcpy( rData.icompBuffTemplateSet, m_CompoundBuffIDSet, sizeof(rData.icompBuffTemplateSet) );
	rData.igenre				= m_pItemTemplate->nItemGenre;									
	rData.idetailtype			= m_pItemTemplate->nDetailType;							
	rData.iparticulartype		= m_pItemTemplate->nParticularType;							
	rData.ilevel				= m_pItemTemplate->nLevel;									
	rData.iyaoid				= m_YaoID;
	memcpy( rData.iyaoAddOnBuffIDSet, m_YaoAddOnBuffIDSet, sizeof(rData.iyaoAddOnBuffIDSet) );	
	rData.idurability			= m_CurrentDurability;										
	rData.imaxdurability		= m_MaxDurability;											
	rData.uLevelupTimes			= m_UpgradeCount;											
	rData.nLevelupType			= m_UpgradeType;	
	rData.nItemCount			= m_ItemStackCount;	
	memcpy( rData.szPlusInfo, m_szPlusInfo, ITEM_PLUS_INFO_LEN );
	rData.szPlusInfo[ITEM_PLUS_INFO_LEN-1] = 0;
	rData.nTalismanPotential	= m_TalismanPotential;						//法宝当前蕴魂
	memcpy( rData.TalismanEnchaseSet, m_TalismanEnchaseSet, TM_HOLE_NUM * sizeof(int) );
	rData.IsBind				= m_IsBind;								//当前是否可交易状态
	rData.LockCount				= m_LockCount;

	int useMaxCount = GetMaxSocketCount();
	for ( int nIdx = 0; nIdx < MAX_INLAY_COUNT; ++nIdx )
	{
		InlayStuff stuff;
		
		GetInlayStuffBySocketIdx( nIdx, stuff );

		if ( nIdx < useMaxCount )
		{
			rData.inlayItem[nIdx].nGenre		= stuff.nGenre;
			rData.inlayItem[nIdx].nDetail		= stuff.nDetail;
			rData.inlayItem[nIdx].nParticular	= stuff.nParticular;
			rData.inlayItem[nIdx].nLevel		= stuff.nLevel;
		}
		else
		{
			rData.inlayItem[nIdx].nGenre		= -1;
			rData.inlayItem[nIdx].nDetail		= -1;
			rData.inlayItem[nIdx].nParticular	= -1;
			rData.inlayItem[nIdx].nLevel		= -1;
		}
	}
	memcpy(rData.InlayBaseBuffSet, m_InlayBaseBuffSet, sizeof(rData.InlayBaseBuffSet) );
	memcpy(rData.InlayYaoBuffSet, m_InlayYaoBuffSet, sizeof(rData.InlayYaoBuffSet) );
	memcpy(rData.InlaySpecialBuffSet, m_InlaySpecialBuffSet, sizeof(rData.InlaySpecialBuffSet) );

	rData.IBBuyDate		= m_dwIBBuyDate;
	rData.CreditFlag	= m_CreditFlag;

	rData.MapID			= m_MapID;
	rData.MapX			= m_MapX;
	rData.MapY			= m_MapY;
	rData.Step			= m_Step;
#ifdef _SERVER
	rData.IBGUID		= m_IBGUID;
#endif
	memset(rData.freeSpace, 0, sizeof(rData.freeSpace));
}

void KItem::SetItemtransfersData( const TItemtransfersData& rData )
{
#ifdef _SERVER
	m_GUID								=	rData.Guid;
#endif
	memcpy( m_CompoundBuffIDSet, rData.icompBuffTemplateSet,sizeof(m_CompoundBuffIDSet) );
	m_pItemTemplate->nItemGenre			=	(ITEMGENRE)rData.igenre;			 				
	m_pItemTemplate->nDetailType		=	(EQUIPDETAILTYPE)rData.idetailtype;		 		
	m_pItemTemplate->nParticularType	=	rData.iparticulartype;			
	m_pItemTemplate->nLevel				=	rData.ilevel;			 			
	m_YaoID								=	rData.iyaoid;
	memcpy(m_YaoAddOnBuffIDSet, rData.iyaoAddOnBuffIDSet, sizeof(m_YaoAddOnBuffIDSet));
	m_CurrentDurability					=	rData.idurability;		 			
	m_MaxDurability						=	rData.imaxdurability;	 			
	m_UpgradeCount						=	rData.uLevelupTimes;	 		
	m_UpgradeType						=	rData.nLevelupType;		 
	m_ItemStackCount					=	rData.nItemCount;		 
	memcpy( m_szPlusInfo, rData.szPlusInfo, ITEM_PLUS_INFO_LEN );
	m_szPlusInfo[ITEM_PLUS_INFO_LEN-1] = 0;
	m_TalismanPotential					=	rData.nTalismanPotential;						//法宝当前蕴魂
	memcpy( m_TalismanEnchaseSet, rData.TalismanEnchaseSet, TM_HOLE_NUM * sizeof(int) );	
	m_IsBind							=	rData.IsBind;
	m_LockCount							=	rData.LockCount;
	ClearSocketSet();
	for ( int nSocketIdx = 0; nSocketIdx < MAX_INLAY_COUNT; ++nSocketIdx )
	{
		InlayStuff stuff;
		if ( rData.inlayItem[nSocketIdx].nGenre == -1 &&
			rData.inlayItem[nSocketIdx].nDetail == -1 &&
			rData.inlayItem[nSocketIdx].nParticular == -1 &&
			rData.inlayItem[nSocketIdx].nLevel == -1 )
		{
			continue;
		}
		else
		{

			stuff.nGenre		= rData.inlayItem[nSocketIdx].nGenre;
			stuff.nDetail		= rData.inlayItem[nSocketIdx].nDetail;
			stuff.nParticular	= rData.inlayItem[nSocketIdx].nParticular;
			stuff.nLevel		= rData.inlayItem[nSocketIdx].nLevel;
			if ( stuff.nGenre == 0 && 
				stuff.nDetail == 0 &&
				stuff.nParticular == 0 && 
				stuff.nLevel == 0 )
			{
				CreateSocket();
			}
			else
			{
				SetSocketSet( stuff );
			}
			
		}
	}
	memcpy(m_InlayBaseBuffSet, rData.InlayBaseBuffSet, sizeof(m_InlayBaseBuffSet) );
	memcpy(m_InlayYaoBuffSet, rData.InlayYaoBuffSet, sizeof(m_InlayYaoBuffSet) );
	memcpy(m_InlaySpecialBuffSet, rData.InlaySpecialBuffSet, sizeof(m_InlaySpecialBuffSet) );

	m_dwIBBuyDate	= rData.IBBuyDate;
	m_CreditFlag	= rData.CreditFlag;
	m_MapID			= rData.MapID;
	m_MapX			= rData.MapX;
	m_MapY			= rData.MapY;
	m_Step			= rData.Step;
	
#ifdef _SERVER
	m_IBGUID		= rData.IBGUID;
#endif
}

BOOL  KItem::IsTaskGiven()const
{
	return m_IsTaskGiven;
}

void  KItem::SetTaskGiven(BOOL bGiven)
{
	m_IsTaskGiven = bGiven;
}

DWORD KItem::GetCreditFlag()
{
	return m_CreditFlag;
}


void  KItem::SetCreditFlag(const DWORD dwFlag)
{
	m_CreditFlag = dwFlag;
}

#define ITEM_TRANSFER_DATA_VERSION1_LENGTH 472

#ifdef _SERVER
int KItem::GetItemtransfersData( TItemtransfersData* pData, const char* pItemData, int dataSize )
{
	if (NULL == pData || NULL == pItemData || dataSize <= 0)
		return 0;

	int count = 0;
	if ( dataSize % ITEM_TRANSFER_DATA_VERSION1_LENGTH == 0 )
	{
		count = dataSize / ITEM_TRANSFER_DATA_VERSION1_LENGTH;
		for (int i = 0; i < count; i++)
		{
			ParseItemtransfersDataVersion1(pData++, pItemData + i * ITEM_TRANSFER_DATA_VERSION1_LENGTH, ITEM_TRANSFER_DATA_VERSION1_LENGTH);
		}
	}
	else
	{
		count = dataSize / sizeof(TItemtransfersData);
		memcpy(pData, pItemData, dataSize);
	}

	return sizeof(TItemtransfersData) * count;
}

void KItem::ParseItemtransfersDataVersion1( TItemtransfersData* pData, const char* pItemData, int dataSize )
{
	if (NULL == pData || NULL == pItemData || dataSize != ITEM_TRANSFER_DATA_VERSION1_LENGTH)
		return;
	
	const TItemtransfersDataVersion1* pDataVersion1 = (const TItemtransfersDataVersion1*)pItemData;
	
	pData->Guid					= pDataVersion1->Guid;
	
	for (int compBuffIndex = 0; compBuffIndex < COMPOUND_COUNT; compBuffIndex++)
	{
		pData->icompBuffTemplateSet[compBuffIndex] = pDataVersion1->icompBuffTemplateSet[compBuffIndex];
	}
	
	pData->igenre				= pDataVersion1->igenre;
	pData->idetailtype			= pDataVersion1->idetailtype;
	pData->iparticulartype		= pDataVersion1->iparticulartype;
	pData->ilevel				= pDataVersion1->ilevel;
	pData->iyaoid				= pDataVersion1->iyaoid;
	
	for (int yaoAddOnIndex = 0; yaoAddOnIndex < YAO_ADDON_BUFF_COUNT; yaoAddOnIndex++)
	{
		pData->iyaoAddOnBuffIDSet[yaoAddOnIndex] = pDataVersion1->iyaoAddOnBuffIDSet[yaoAddOnIndex];
	}
	
	pData->idurability			= pDataVersion1->idurability;
	pData->imaxdurability		= pDataVersion1->imaxdurability;
	pData->uLevelupTimes			= pDataVersion1->uLevelupTimes;
	pData->nLevelupType			= pDataVersion1->nLevelupType;
	pData->nItemCount			= pDataVersion1->nItemCount;	
	
	memcpy( pData->szPlusInfo, pDataVersion1->szPlusInfo, COMMON_CLIENT_MSG_LEN_64 );
	pData->szPlusInfo[COMMON_CLIENT_MSG_LEN_64-1] = 0;
	pData->nTalismanPotential	= pDataVersion1->nTalismanPotential;						//法宝当前蕴魂
	
	for (int talismanEnchaseIndex = 0; talismanEnchaseIndex < TM_HOLE_NUM; talismanEnchaseIndex++)
	{
		pData->TalismanEnchaseSet[talismanEnchaseIndex] = pDataVersion1->TalismanEnchaseSet[talismanEnchaseIndex];
	}

	pData->IsBind				= pDataVersion1->IsBind;								//当前是否可交易状态
	pData->LockCount				= pDataVersion1->LockCount;
	
	for (int inlayItemIndex = 0; inlayItemIndex < MAX_INLAY_COUNT; inlayItemIndex++)
	{
		pData->inlayItem[inlayItemIndex].nGenre = pDataVersion1->inlayItem[inlayItemIndex].nGenre;
		pData->inlayItem[inlayItemIndex].nDetail = pDataVersion1->inlayItem[inlayItemIndex].nDetail;
		pData->inlayItem[inlayItemIndex].nParticular = pDataVersion1->inlayItem[inlayItemIndex].nParticular;
		pData->inlayItem[inlayItemIndex].nLevel = pDataVersion1->inlayItem[inlayItemIndex].nLevel;
	}
	
	for (int inlayBuffIndex = 0; inlayBuffIndex < MAX_INLAY_COUNT; inlayBuffIndex++)
	{
		for (int buffIndex = 0; buffIndex < ITEM_BUFF_COUNT; buffIndex++)
		{
			pData->InlayBaseBuffSet[inlayBuffIndex][buffIndex] = pDataVersion1->InlayBaseBuffSet[inlayBuffIndex][buffIndex];
		}
	}
	
	for (int inlayYaoBuffIndex = 0; inlayYaoBuffIndex < MAX_ITEM_INLAY_YAO_EFFECT_COUNT; inlayYaoBuffIndex++)
	{
		pData->InlayYaoBuffSet[inlayYaoBuffIndex] = pDataVersion1->InlayYaoBuffSet[inlayYaoBuffIndex];
	}
	
	for (int inlaySepcialBuffIndex = 0; inlaySepcialBuffIndex < MAX_SPECIALEFFECT_COUNT; inlaySepcialBuffIndex++)
	{
		pData->InlaySpecialBuffSet[inlaySepcialBuffIndex] = pDataVersion1->InlaySpecialBuffSet[inlaySepcialBuffIndex];
	}
	
	pData->IBBuyDate	= 0;
	pData->CreditFlag	= 0;
	pData->MapID		= 0;
	pData->MapX			= 0;
	pData->MapY			= 0;
	pData->Step			= 0;
	pData->IBGUID		= 0;
	memset(pData->freeSpace, 0, sizeof(pData->freeSpace));
}
#endif

bool KItem::IsLockedByDate(int nConnectIdx)
{
#ifdef _SERVER
	DWORD curDate = UNIX_TMIE_STAMP;
#else
	DWORD curDate = ::time(NULL);
#endif
	
	if ( m_dwLockDate <= curDate && m_dwLockDate != 0 && m_dwLockDate != LOCK_BY_DATA_SIGN )
	{
		m_dwLockDate = 0;
#ifdef _SERVER
		if (nConnectIdx > -1 )
		{
			SyncAttribute(item_attr_lockdate, nConnectIdx);
		}//endif		
#endif
	}//endif

#ifdef _SERVER
	return m_dwLockDate != 0;
#else
	return false;
#endif
	
}
