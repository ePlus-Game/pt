//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemGenerator.CPP
// Date:	2002.08.26
// Code:	DongBo
// Desc:    CPP file. 本文件实现的类用于生成道具
//---------------------------------------------------------------------------
#include "KEngine.h"
#include "KCore.h"
#include "MyAssert.H"
#include "KItem.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"
#include <time.h>
#include "KMath.h"
#include "Yao_AddOnTable.h"
#include "kiteminlayaddontable.h"
#include "ILogSystem.h"

KItemGenerator	g_ItemGen;

KItemGenerator::KItemGenerator()
{
}

KItemGenerator::~KItemGenerator()
{
}

/******************************************************************************
	功能：	数据初始化. 从tab file中读取数据
******************************************************************************/
BOOL KItemGenerator::Init( void )
{

	if (!m_BPTLib.Init())		// 此调用从若干的tab file中载入所有的初始属性
		return FALSE;

	return TRUE;
}

BOOL KItemGenerator::Gen_Item(
		IN int nGenre,
		IN int nDetailType,
		IN int nParticularType,
		IN int nLevel,
		IN int nItemCount,
		IN OUT KItem* pItem)
{
	BOOL nRet = FALSE;
	
	KBASICPROP_ITEM* pItemTemplate = (KBASICPROP_ITEM*)GetItemTemplate(nGenre, nDetailType, nParticularType, nLevel);
	
	if ( pItemTemplate )
	{
		pItem->SetItemTemplate(pItemTemplate);
		pItem->SetMaxDurability(pItemTemplate->nDurability);
		pItem->SetDurability(pItem->IsExpItem() ? 0 : pItemTemplate->nDurability);
		pItem->SetTalismanPotential(0);		
		pItem->SetLockCount(0);
		pItem->SetTaskGiven(FALSE);

		for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
		{
			pItem->SetTalismanEnchase(enchaseLoopCount, -1);
		}

		if ( pItemTemplate->IBUseCount > 0 && 
			pItemTemplate->IBType == ib_item_time_count_limit )
		{
			nItemCount = pItemTemplate->IBUseCount;
			pItemTemplate->nStack = pItemTemplate->IBUseCount;
		}
		
		if ( pItemTemplate->nStack > 0 )
		{
			pItem->SetItemCount( nItemCount < pItemTemplate->nStack ? nItemCount : pItemTemplate->nStack );
		}
		else
		{
			pItem->SetItemCount( 1 );			
		}

		pItem->SetActionTime(pItemTemplate->ActionTime);

		//爻装相关
		pItem->SetYaoID( yao_invalid );
		//没有附加属性
		{
			for (int yaoAddOnBuffLoopCount = 0; yaoAddOnBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddOnBuffLoopCount++)
			{
				pItem->SetYaoAddOn(yaoAddOnBuffLoopCount, 0);
			}		
		}
#ifdef _SERVER
		if ( pItem->GetGenre() != item_ib && 
			pItem->GetIBItemType() == ib_item_timelimit &&
			pItem->GetIBAvailabilityTime() > 0 )
		{
			pItem->SetIBBuyDate(UNIX_TMIE_STAMP);
		}
		else
		{
			pItem->SetIBBuyDate(0);
		}
		
		//物品GUID
		if (g_pController != NULL)
		{
			FSGUID guid;
			g_pController->GenGUID(guid.data, g_GuidPadding);
			pItem->SetGUID(guid);
		}		

		if ( TRUE == pItemTemplate->nYao )
		{	
			//50%的概率确定是阳爻还是阴爻
			if ( TRUE == g_RandPercent( 50 ) )
				pItem->SetYaoID( yao_yang );
			else
				pItem->SetYaoID( yao_yin );
		}

		//爻装附加属性
		if ( TRUE == g_RandPercent(pItemTemplate->nYaoRate) )
		{
			if ( (pItemTemplate->nItemGenre == item_target || (pItemTemplate->nItemGenre == item_ib && !pItem->IsNoTarget() )) &&
				pItemTemplate->nParticularType == inlay_targetitem )
			{
				short inlayYaoBuffSet[MAX_ITEM_INLAY_YAO_EFFECT_COUNT];
				memset( inlayYaoBuffSet, 0, sizeof(inlayYaoBuffSet) );
				KItemInlayAddOnTable& inlayAddOnTable = KItemInlayAddOnTable::Singleton();				
				int count = inlayAddOnTable.GetInlayAddOnCount( pItemTemplate->sYaoGroup );
				for( int i = 0; i < count; i++ )
				{
					const InlayAddOn* inlayAddOn = inlayAddOnTable.GetInlayAddOnByGroup(pItemTemplate->sYaoGroup, i);
					if ( inlayAddOn != NULL )
					{
						if (inlayAddOn->Probability > g_Random(10000))
						{
							inlayYaoBuffSet[0] = inlayAddOn->BuffID;
							break;
						}
					}
				}
				pItem->SetInlayYaoBuffSet( inlayYaoBuffSet );
			}
			else
			{
				//if ( TRUE == pItemTemplate->nYao )
				{
					//从爻装附加属性表中找到随机属性
					YaoAddOnTable& yaoAddOnTable = YaoAddOnTable::Singleton();				
					int count = yaoAddOnTable.GetYaoAddOnCount( pItemTemplate->sYaoGroup );
					int addonBuffCount = 0;
					for (int yaoAddOnBuffLoopCount = 0; yaoAddOnBuffLoopCount < YAO_ADDON_BUFF_COUNT; yaoAddOnBuffLoopCount++)
					{
						for( int i = 0; i < count; i++ )
						{
							const YaoAddOn* yaoAddOn = yaoAddOnTable.GetYaoAddOn(pItemTemplate->sYaoGroup, i);
							if ( yaoAddOn != NULL )
							{
								if (yaoAddOn->Probability > g_Random(10000))
								{
									pItem->SetYaoAddOn( addonBuffCount, yaoAddOn->BuffID );
									addonBuffCount++;
									break;
								}
							}
						}
					}				
				}
			}
		}
#endif

		nRet = TRUE;
	}

	return nRet;
}

const KBASICPROP_ITEM* KItemGenerator::GetItemTemplate(
		IN int nGenre,
		IN int nDetailType,
		IN int nParticularType,
		IN int nLevel
		)
{
	const KBASICPROP_ITEM* pItemTemplate = NULL;

	switch( nGenre )
	{
	case item_equip:
		{
			const int i = nParticularType;

			switch( nDetailType )
			{
			case equip_weapon:
				pItemTemplate = m_BPTLib.GetWeaponRecord(i);
				break;
			case equip_armor:
				pItemTemplate = m_BPTLib.GetArmorRecord(i);
				break;
			case equip_helm:
				pItemTemplate = m_BPTLib.GetHelmRecord(i);
				break;
			case equip_boots:
				pItemTemplate = m_BPTLib.GetBootRecord(i);
				break;
			case equip_shoulder:
				pItemTemplate = m_BPTLib.GetShoulderRecord(i);
				break;
			case equip_amulet:
				pItemTemplate = m_BPTLib.GetAmuletRecord(i);
				break;
			case equip_ring:
				pItemTemplate = m_BPTLib.GetRingRecord(i);
				break;
			case equip_cuff:
				pItemTemplate = m_BPTLib.GetCuffRecord(i);
				break;
			case equip_pendant:
				pItemTemplate = m_BPTLib.GetPendantRecord(i);
				break;
			case equip_talisman:
				pItemTemplate = m_BPTLib.GetTalismanRecord(nDetailType, nParticularType, nLevel);
				break;
			default:
				break;
			}
		}
		break;
	case item_medicine:
		pItemTemplate = m_BPTLib.GetMedicineRecord(nDetailType);
		break;
	case item_materials:
		pItemTemplate = m_BPTLib.GetMaterialRecord(nDetailType);
		break;
	case item_task:
		pItemTemplate = m_BPTLib.GetQuestRecord(nDetailType);
		break;
	case item_target:
		pItemTemplate = m_BPTLib.GetTargetItemRecord(nDetailType);
		break;
	case item_enchase:
		pItemTemplate = m_BPTLib.GetEnchaseItemRecord(nDetailType);
		break;
	case item_horse:
		pItemTemplate = m_BPTLib.GetHorseRecord(nDetailType);
		break;			
	case item_ib:
		pItemTemplate = m_BPTLib.GetIBRecord(nDetailType);
		break;
	case item_charm:
		pItemTemplate = m_BPTLib.GetCharmRecord(nDetailType);
		break;
	default:
		break;
	}

	return pItemTemplate;
}

const KBASICPROP_ITEM* KItemGenerator::GetItemTemplate( const char* name )
{
	const KBASICPROP_ITEM* pItemTemplate = NULL;

	pItemTemplate = m_BPTLib.GetWeaponRecordByName(name);
	pItemTemplate = m_BPTLib.GetArmorRecordByName(name);
	pItemTemplate = m_BPTLib.GetArmorRecordByName(name);
	pItemTemplate = m_BPTLib.GetBootRecordByName(name);
	pItemTemplate = m_BPTLib.GetShoulderRecordByName(name);
	pItemTemplate = m_BPTLib.GetAmuletRecordByName(name);
	pItemTemplate = m_BPTLib.GetRingRecordByName(name);
	pItemTemplate = m_BPTLib.GetCuffRecordByName(name);
	pItemTemplate = m_BPTLib.GetPendantRecordByName(name);
	pItemTemplate = m_BPTLib.GetTalismanRecordByName(name);
	pItemTemplate = m_BPTLib.GetMedicineRecordByName(name);
	pItemTemplate = m_BPTLib.GetMaterialRecordByName(name);
	pItemTemplate = m_BPTLib.GetQuestRecordByName(name);
	pItemTemplate = m_BPTLib.GetTargetItemRecordByName(name);
	pItemTemplate = m_BPTLib.GetEnchaseItemRecordByName(name);
	pItemTemplate = m_BPTLib.GetHorseRecordByName(name);
	pItemTemplate = m_BPTLib.GetIBRecordByName(name);

	/*pItemTemplate = m_BPTLib.GetWeaponRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetArmorRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetArmorRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetBootRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetShoulderRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetAmuletRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetRingRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetCuffRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetPendantRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetTalismanRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetMedicineRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetMaterialRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetQuestRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetTargetItemRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetEnchaseItemRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}
	pItemTemplate = m_BPTLib.GetHorseRecord(name);
	if ( pItemTemplate )
	{
		return pItemTemplate;
	}//*/

	return NULL;
}


