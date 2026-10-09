//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/03/2006 17:18
//      File_base        : CoreShell
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "GameDataDef.h"
#include "specialskill_tab.h"
#include "QueryInfo.h"
#ifndef _SERVER
#include "CoreShell.h"
#endif
#include "KItemSet.h"
#include "CoreDrawGameObj.h"
#include "ImgRef.h"

#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KObjSet.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"

#include "KNpcResList.h"
#include "Scene/KScenePlaceC.h"
#include "KSkills.h"
#include "GameDataDef.h"
#include "MsgGenreDef.h"
#include "KOption.h"
#include "KSubWorld.h"
#include "KViewItem.h"
#include "malloc.h"
#include <vector>
#include <time.h>
#include <string.h>
#include "KBuySell.h"
#include "KSmithShop.h"
#include "KItemSet.h"
#include "KItemCompounder.h"
#include "KCompoundRule.h"
#include "KItemGenerator.h"
#include "scene/KScenePlaceC.h"
#include "ChatCenter_C.h"
#include "Scene/ObstacleDef.h"
#include "SkillManager.h"
#include "buff_tab.h"
#include "KWin32Wnd.h"
#include "Yao_Table.h"
#include "CoreUtil.h"
#include "Scene/MapNpcMgr.h"
#include "OnceIBItemMgr.h"
#include "pluspoint.h"

#ifndef SERVER
	#include "KSimulation.h"
	#include "KLinkItem.h"
	#include "screeneffect_man.h"
	#include "IBShopComDef.h"
	#include "IBCenter_C.h"
#endif
#include "talisman_manager.h"
#include "CoreRelated.h"

#ifndef _SERVER
#ifdef	_AUTO_ROBOT
#include "AutoRobotMgr.h"
#include "AutoDialogNpc.h"
#endif
#endif

//Add By Brianyao 2007
#include "ITaisuiWheel.h"
#ifndef _SERVER
#pragma warning(disable: 4800)
#include "TaisuiWheelTianXiang.h"
#include "KClientFuryMgr.h"
#endif
//end

#ifndef _SERVER
#include "ai_player_controller.h"
#endif

#include "exp_insruance.h "
#include "cfs_filelogs.h"

_UseItem	g_UseItem;
int			g_uItemID;
_ItemPos		g_uItemPos;

IClientCallback* l_pDataChangedNotifyFunc = 0;

void _sysMoneyToUiMoney(int money, int& jin, int& yin, int& tong)
{
	jin = money / 10000;
	yin = (money % 10000) / 100;
	tong = money % 100; 
}

bool g_bPerspectiveMode = false;

static int s_LSkill = 0;
static int s_RSkill = 0;

int	g_CoreFrameCutLevel = 0;
BOOL	g_CoreHighQualityPaint = TRUE;
class KCoreShell : public iCoreShell
{
public:
	int				FindNpcIndexById		( DWORD npcId																																				);
	int				ChangeUiLoginDispNpcDir	( int nDir																																					);
	int				PlayUiLoginDisplayerNpc	(																																							);
	int				StopUiLoginDisplayerNpc	(																																							);
	int				ChangeUiLoginDispNpcStat( int nState																																				);
	int				SetUiLoginDisplayerNpc	( UI_DISPLAYER_NPC_SYNC* pNpc																																);
	int				GetProtocolSize			( BYTE byProtocol																																			);
    int				OperationRequest		( unsigned int uOper, unsigned int uParam, int nParam																										);
	void			ProcessInput			( unsigned int uMsg, unsigned int uParam, int nParam																										);
	int				GetTargetNPC			( void																																						);
	int				FindSelectNPC			( int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind, bool bSearchSelf = false, bool bIsLeft = true , bool bNoSelectPlayer	= false      );
	int				AutoSelectNPC			( int nRelation, int nMouseX, int nMouseY																																	);
	bool			SelectNPC				( int nIdx																																					);
	int				FindSpecialNPC			( char* Name, void* pReturn, int& nKind																														);
	int				FindSelectObject		( int x, int y, bool bSelect, int& nObjectIdx, int& nKind																									);
//	unsigned int	FindSelectBuilding		( int x, int y, bool bSelect																																);
//	void			DialogNpc				(																																							);
//	int				UseSkill				( KSkillData skillData																																		);
	int				ChatSpecialPlayer		( void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength																							);
	void			TradeApplyStart			( int targetIndex																																			);
	int				UseGameObject			( KGameObject *pGO																																			);
	int				LockSomeoneAction		( int nTargetIndex																																			);
//	int				LockObjectAction		( int nTargetIndex																																			);
	void			GotoWhere				( int x, int y, int mode ,bool bDelay = false																												);	//mode 0 is auto, 1 is walk, 2 is run
//	void			Goto					( int nDir, int mode																																		);	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	void			Turn					( int nDir																																					);	//nDir 0 is left, 1 is right, 2 is back
	int				ThrowAwayItem			( int nItemIndex																																			);
	int				GetNPCRelation			( int nIndex																																				);
	int				SceneMapOperation		( unsigned int uOper, unsigned int uParam, int nParam																										);
	int				TongOperation			( unsigned int uOper, unsigned int uParam, int nParam																										);
	int				TeamOperation			( unsigned int uOper, unsigned int uParam, int nParam																										);
	int				BuildingOperation		( unsigned int uOper, unsigned int uParam, int nParam																										);
	int				GetGameData				( unsigned int uDataId, unsigned int uParam, int nParam																										);
	void			DrawGameObj				( unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam																	);
	void			DrawGameSpace			( void																																						);
	void            DrawUiEffect            ( void);
	void			BreatheGameSpace		( void																																						);
	void			PaintBehindUi			( void																																						);
	DWORD			GetPing					( void																																						);
	int				SetCallDataChangedNofify( IClientCallback* pNotifyFunc																																);
	void			NetMsgCallbackFunc		( void* pMsgData																																			);
	void			SetRepresentShell		( struct iRepresentShell* pRepresent																														);
	void			SetMusicInterface		( void* pMusicInterface																																		);
	void			SetRepresentAreaSize	( int nWidth, int nHeight																																	);
	int				Breathe					( void																																						);
	void			Release					( void																																						);
	void			SetClient				( LPVOID pClient,int nConnectID																																);
	void			SendNewDataToServer		( void* pData, int nLength																																	);
	int				InsertItem				( int nGenre, int nDetail, int nParticular, int nLevel, int nSeries, int nLuck, int* pMagicLevel, int nVersion, int nRandSeed								);
	void			InitSimplifiedNpc		( BOOL bSimplifiedNpc																																		);
	void			EnableSimplifiedNpc		( BOOL bSimplifiedNpc																																		);
	void			BegingBuildBuilding		( unsigned int utype, unsigned int nDataIdx																													);
	void			TryToBuildBuilding		( unsigned int nX, unsigned int nY																															);
	void			EndBuildBuilding		( void																																						);
	void			ValidateBuildBuilding	( BOOL bCancel																																				);
	void			GetPlayerPos			( int &nGridX, int &nGridY																																	);
	BOOL			IsItemListLocked		( void																																						);
	int				GetHandItemIndex		( void																																						);
	BOOL			FindPlacePos			( POINT* pPos																																				);
//	BOOL			RSGetWannaData			( unsigned int uGetIndex, const void *pInData, void *pOutData																								);
	void			FindItemIndex			( int nGenre, int nDetail, int nParticular, int *pnIdx																										);
	BOOL			IsTextPass				(const char *szText);
	BOOL			IsNamePass				(const char *szText);
//	void			SwitchDefaultSkill		( void				);
	void			DrawMovePosition		( int x, int y		);
	void			RemoveMovePosition		( void				);
	void			SetMusic				( bool b			);
	void			Move					( int direction, int distance );

	void			SelectSkill				( int skillId );
	void			NextSkill				( int skillId, bool targetSelf = false );
//	void			UseSelectedSkill		( int targetSelf = false );
	void			FollowAttack			( void );
	void			FollowDialog			( void );
	void			PickupObject			( int objectIndex );
	void			Stop					( void );	

	void			ReplyPrompt				( enumPromptEvent promptEvent, bool accept );
	bool			CoreParseQuestionProtocol	( BYTE* pMsg, UIQuestionData& uiQuestionData );
};
static KCoreShell	g_CoreShell;
int					g_DestPosIdx = -1;

void g_InitCore();
#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreShell* CoreGetShell()
{
	g_InitCore();
	return &g_CoreShell;
}

//--------------------------------------------------------------------------
//	功能：发出游戏世界数据改变的通知函数
//	返回：如未被注册通知函数，则直接返回0，否则返回通知函数执行结果。
//--------------------------------------------------------------------------
void CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam);
}


void KCoreShell::Release()
{
	g_ReleaseCore();
}

int KCoreShell::ChangeUiLoginDispNpcDir( int nDir )
{
//	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].set
	return 0;
}

int KCoreShell::SetUiLoginDisplayerNpc( UI_DISPLAYER_NPC_SYNC* pNpc )
{
	NpcSet.SetUiLoginDisplayerNpc( pNpc );
	return 0;
}

int	KCoreShell::PlayUiLoginDisplayerNpc	( void )
{
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].SetPlayUiLoginNpc( true );
	return 0;
}

int KCoreShell::StopUiLoginDisplayerNpc	( void )
{
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].SetPlayUiLoginNpc( false );
	return 0;
}

int KCoreShell::ChangeUiLoginDispNpcStat( int nState )
{
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].SetUiLoginNpcDoing( nState );
	return 0;
}

//--------------------------------------------------------------------------
//	功能：接受与分派处理网络消息
//--------------------------------------------------------------------------
void KCoreShell::NetMsgCallbackFunc(void* pMsgData)
{
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData);
}
//--------------------------------------------------------------------------
//	功能：设置游戏世界数据改变的通知函数
//	参数：fnCoreDataChangedCallback pNotifyFunc --> 通知函数的指针。
//	返回：返回值为非0值表示注册成功，否则表示失败。
//--------------------------------------------------------------------------
int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}
//按 xx,xxx,xxx 格式输出数字
void msprintf(char* szBuf, int nValue)
{
	if (nValue < 0)
	{
		szBuf[0] = '-';
		szBuf++;
		nValue = -nValue;
	}
	if (nValue >= 1000000)
		sprintf(szBuf + strlen(szBuf), "%d,", nValue / 1000000);
	if (nValue >= 1000000 || nValue / 1000 > 0)
	{
		if (nValue >= 1000000)
			sprintf(szBuf + strlen(szBuf), "%03d,", (nValue % 1000000) / 1000);
		else
			sprintf(szBuf + strlen(szBuf), "%d,", (nValue % 1000000) / 1000);
	}
	if (nValue >= 1000)
		sprintf(szBuf + strlen(szBuf), "%03d", nValue % 1000);
	else
		sprintf(szBuf + strlen(szBuf), "%d", nValue % 1000);
}

//--------------------------------------------------------------------------
//	功能：从游戏世界获取数据
//	参数：unsigned int uDataId --> 表示获取游戏数据的数据项内容索引，其值为梅举类型
//							GAMEDATA_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uDataId的取值情况而定
//		  int nParam --> 依据uDataId的取值情况而定
//	返回：依据uDataId的取值情况而定。
//--------------------------------------------------------------------------
int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_GET_MAP_NAME:
		{
			MapsTab mapTab;
			MapsInfo::getSingleton().GetMapInfo( uParam, mapTab );
			char* szMapName = (char*)nParam;
			if ( szMapName )
			{
				strncpy( szMapName, mapTab.name.c_str(), 32 );
			}
		}
		break;
	case GDI_GET_CUR_SPECIALSKILL_ID:
		{
			return SpecialSkillTab::Singleton().GetNearlySpecialSkill( 
				Player[CLIENT_PLAYER_INDEX].GetSeries(),
				Player[CLIENT_PLAYER_INDEX].GetSkillSeries(),
				Player[CLIENT_PLAYER_INDEX].GetLevel());
		}
		break;
	case GDI_GET_ITEM_ID_BY_INDEX:
		{
			int tmIndex = uParam;
			nRet = Item[tmIndex].GetID();
		}
		break;
	case GDI_GET_ITEM_INDEX_BY_ID:
		{
			int id = uParam;
			nRet = ItemSet.SearchID(id);
		}
		break;
	case GDI_CHAT_UPDATE_PK_VALUE:
		{
			g_ChatCenterC.GetPkInfo();
			nRet = 0; 
		}
	case GDI_CHAT_GROUP_INFO:
		{
			nRet = g_ChatCenterC.GetGroupIdAndNames( (DWORD*)uParam, (char*)nParam );
		}
		break;
	case GDI_CHAT_FRIENDS_IN_A_GROUP:
		{
			nRet = g_ChatCenterC.GetGroupMemberInfo( (DWORD)uParam, (char*)nParam );
		}
		break;
	case GDI_GET_RECENTLIST:
		{
			int recordSize = g_ChatCenterC.GetRecentlyObjNum();
			if ( recordSize > 0 )
			{
				nRet = g_ChatCenterC.GetRecentlyObjs(reinterpret_cast<char*>(nParam), recordSize * sizeof (UI_RECENT_OBJ_NAME));
			}
		}
		break;
	case GDI_CHAT_IS_OWNER:
		{
            const char * pOwnerName = g_ChatCenterC.GetRoomOwnerName(uParam);
			if (pOwnerName && strcmp(Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].Name,pOwnerName)==0)
			{
                nRet  = 1;	
			}//endif
			else
				nRet  = 0;
		}
		break;
	case GDI_CHAT_BUBBLE:
		{
			int nIndex = 0;
			if (nParam == 0)
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			else
				nIndex = NpcSet.SearchID(nParam);

			bool b = ( uParam != 0 );
			if ( nIndex > 0 && nIndex < MAX_NPC )
				Npc[nIndex].SetBubble(b);
		}
		break;
	case GDI_PLAYER_IS_MALE:
		{
			int nIndex = 0;
			if (nParam == 0)
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			else
				nIndex = NpcSet.SearchID(nParam);

			if (nIndex)
				// lixuewu  2004.02.05				
				nRet = (Npc[nIndex].m_NpcSettingIdx > -4);
			else
				nRet = 1;	//出错时是男性
		}
		break;
	case GDI_REPAIR_ITEM_PRICE:
		if (nParam)
		{
			nRet = 0;
			int itemIndex = nParam;
			bool specialRepair = ( uParam != 0); 

			KItem& item = Item[itemIndex];
			if(item.GetID() <= 0)
			{
				break;
			}
						
			nRet = item.getRepairPrice(specialRepair);
			if(item.CanBeRepaired() == false)
				nRet = -1;
		}
		break;
	case GDI_REPAIR_ALL_ITEM_PRICE:
		{
			int price = 0;

			KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
			for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
			{
				PlayerItem& item = itemList.m_Items[i];
				if(item.nPlace != pos_equip && item.nPlace != pos_equiproom)
				{
					continue;
				}
				int itemIndex = item.nIdx;
				
				if(itemIndex <= 0)
				{
					continue;
				}
				if(Item[itemIndex].GetGenre() != item_equip)
				{
					continue;
				}
				if(Item[itemIndex].GetDurability() == -1 || Item[itemIndex].GetDurability() == Item[itemIndex].GetMaxDurability())
				{
					continue;
				}
				if(Item[itemIndex].CanBeRepaired() == false)
				{
					continue;
				}
				price += Item[itemIndex].getRepairPrice(nParam);
			}

			return price;
		}
		break;
	/************************************************************************/
	/*							Item system                                 */
	/************************************************************************/
	case GDI_INSIDE_BALL_IMAGE_INFO:
		{
			KItemInfo* pItemInfo = (KItemInfo*)uParam;
			memset(pItemInfo, 0, sizeof(KItemInfo));
			int enchaseId = nParam;

			const PEnchaseData pEnchaseData = TalismanManager::Singleton().GetEnchaseData(enchaseId);
			if (pEnchaseData != NULL)
			{
				strcpy(pItemInfo->szImageSet, pEnchaseData->ImageSetName);
				strcpy(pItemInfo->szImage, pEnchaseData->ImageName);
				strcat(pItemInfo->szImage, "_man_small");
			}
		}
		break;
		
	case GDI_ITEM_INFO_ID:
		{
			KItemInfo* pItemInfo = (KItemInfo*)uParam;
			ZeroMemory( pItemInfo, sizeof(KItemInfo) );
			if ( pItemInfo )
			{
				int nItemIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(nParam);
				if ( nItemIdx > 0 )
				{
					//likun判断装备男女
					char manOrWomanImage[512];
					if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0)
					{
						sprintf(manOrWomanImage, "%s%s", Item[nItemIdx].GetImageFile(), EQUIPMENT_MAN_POSTFIX_SMALL);
					}
					else
					{
						sprintf(manOrWomanImage, "%s%s", Item[nItemIdx].GetImageFile(), EQUIPMENT_WOMAN_POSTFIX_SMALL);
					}
					memcpy( pItemInfo->szImageSet, Item[nItemIdx].GetImageSetFile(), COMMON_CLIENT_MSG_LEN_128 );
					//memcpy( pItemInfo->szImage, Item[nItemIdx].GetImageFile(), COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItemInfo->szImage, manOrWomanImage, COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItemInfo->szName, Item[nItemIdx].GetName(), COMMON_CLIENT_MSG_LEN_32 );
					pItemInfo->itemIdx.nGenre		= Item[nItemIdx].GetGenre();
					pItemInfo->itemIdx.nDetail		= Item[nItemIdx].GetDetailType();
					pItemInfo->itemIdx.nParticular	= Item[nItemIdx].GetParticular();
					pItemInfo->itemIdx.nLevel		= Item[nItemIdx].GetLevel();
					pItemInfo->nGroup				= Item[nItemIdx].GetGroup();
					pItemInfo->iReqLevel			= Item[nItemIdx].GetLevelRequirement();
					pItemInfo->colour				= Item[nItemIdx].GetQualityLabel();
					if ( ItemCanTrade(CLIENT_PLAYER_INDEX, Item[nItemIdx].GetID()) )
					{
						pItemInfo->bVendue			= true;
					}
					else
					{
						pItemInfo->bVendue			= false;
					}
					strcpy(pItemInfo->szToolTip, "<Layout width=200 margin-top=10 margin-left=10 margin-right=10 margin-bottom=10>");
					Item[nItemIdx].GetDesc( pItemInfo->szToolTip );
					strcat(pItemInfo->szToolTip, "</Layout>");
				}
			}
		}
		break;
	case GDI_GET_ITEM_TYPE_BY_ID:
		{
			ItemType* type = (ItemType*)uParam;
			ZeroMemory( type, sizeof(ItemType) );
			
			int itemIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(nParam);
			if ( itemIndex > 0 )
			{
				type->genre		= Item[itemIndex].GetGenre();
				type->detail	= Item[itemIndex].GetDetailType();
				type->particular= Item[itemIndex].GetParticular();
				type->level		= Item[itemIndex].GetLevel();
			}
		}
		break;
	case GDI_GET_ITEM_TYPE_BY_INDEX:
		{
			ItemType* type = (ItemType*)uParam;
			ZeroMemory( type, sizeof(ItemType) );
			
			int itemIndex = nParam;

			if ( itemIndex > 0 && itemIndex < MAX_ITEM && Item[itemIndex].GetItemIndex() == itemIndex)
			{
				type->genre		= Item[itemIndex].GetGenre();
				type->detail	= Item[itemIndex].GetDetailType();
				type->particular= Item[itemIndex].GetParticular();
				type->level		= Item[itemIndex].GetLevel();
				nRet = 1;
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	case GDI_GET_ITEM_COUNT_BY_ID:
		{			
			int itemIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(nParam);
			if(itemIndex > 0 && itemIndex < MAX_ITEM)
			{
				nRet = Item[itemIndex].GetItemCount();
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	case GDI_GET_ITEM_NAME_BY_ID:
		{			
			int itemIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(nParam);
			char* name = (char*)uParam;
			if(itemIndex > 0)
			{
				strcpy(name, Item[itemIndex].GetName());
			}
			else
			{
				strcpy(name, "");
			}
		}
		break;
	case GDI_GET_ITEM_NAME_BY_INDEX:
		{			
			int itemIndex = nParam;
			char* name = (char*)uParam;
			if(itemIndex > 0)
			{
				strcpy(name, Item[itemIndex].GetName());
			}
			else
			{
				strcpy(name, "");
			}
		}
		break;
	case GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEX:
		{
			int itemIndex = nParam;
			char* name = (char*)uParam;
			if(itemIndex > 0)
			{
				strcpy(name, Item[itemIndex].GetNameWithColor());
			}
			else
			{
				strcpy(name, "");
			}
		}
		break;
	case GDI_GET_ITEM_BASIC_INFO_BY_TYPE:
		{		
			ItemType* type = (ItemType*)nParam;
			const KBASICPROP_ITEM** itemTemplate = (const KBASICPROP_ITEM**)uParam;
			*itemTemplate = g_ItemGen.GetItemTemplate( 
				type->genre, type->detail, type->particular, type->level );
		}
		break;
	case GDI_GET_ITEM_SELL_PRICE_BY_ID:
		{		
			int itemIndex = Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(nParam);
			
			if(itemIndex > 0)
			{
				unsigned long sellPrice = Item[itemIndex].GetSellPrice();
				if(Item[itemIndex].GetMaxDurability() != 0)
				{
					sellPrice = sellPrice * Item[itemIndex].GetDurability() / Item[itemIndex].GetMaxDurability();
				}
				
				int itemCount = Item[itemIndex].GetItemCount();
				if(itemCount <= 0)
					itemCount = 1;
				sellPrice *= itemCount;
				nRet = sellPrice;
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	case GDI_GET_ITEM_DURA_BY_INDEX:
		{		
			int itemIndex = nParam;
			if(itemIndex <= 0 || itemIndex > MAX_ITEM)
			{
				nRet = 0;
				break;
			}
			if(Item[itemIndex].GetItemTemplate() == NULL)
			{
				nRet = 0;
				break;
			}
			pair<int, int>* durData = (pair<int,int>*)uParam;
			durData->first = Item[itemIndex].GetDurability();
			durData->second = Item[itemIndex].GetMaxDurability();
			nRet = 1;
		}
		break;
	case GDI_GET_ITEM_NAME_WITH_COLOR_BY_INDEXPARAM:
		{
			FIND_ITEMINDEX_PARAM* pItemInfo = (FIND_ITEMINDEX_PARAM*)uParam;
			char* szName = (char*)nParam;
			if ( pItemInfo )
			{
				const KBASICPROP_ITEM* pTemplate = g_ItemGen.GetItemTemplate( pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel );
				if ( pTemplate )
				{
					KItem item;
					g_ItemGen.Gen_Item( pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel, 1, &item );
					strcat(szName, item.GetNameWithColor());
					
					return 1;
				}
			}
			return 0;
		}
		break;
	// uParam = (KItemInfo*)uParam
	// nParam = Item ID
	// Return = 
	case GDI_ITEM_INFO_INDEX:
		{
			KItemInfo* pItemInfo = (KItemInfo*)uParam;
			ZeroMemory( pItemInfo, sizeof(KItemInfo) );
			if ( pItemInfo )
			{
				int nItemIdx = nParam;
				if ( nItemIdx > 0)
				{
					//likun判断装备男女
					char manOrWomanImage[512];
					if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0)
					{
						sprintf(manOrWomanImage, "%s%s", Item[nItemIdx].GetImageFile(), EQUIPMENT_MAN_POSTFIX_SMALL);
					}
					else
					{
						sprintf(manOrWomanImage, "%s%s", Item[nItemIdx].GetImageFile(), EQUIPMENT_WOMAN_POSTFIX_SMALL);
					}
					memcpy( pItemInfo->szImageSet, Item[nItemIdx].GetImageSetFile(), COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItemInfo->szImage, manOrWomanImage, COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItemInfo->szName, Item[nItemIdx].GetName(), COMMON_CLIENT_MSG_LEN_32 );
					pItemInfo->itemIdx.nGenre		= Item[nItemIdx].GetGenre();
					pItemInfo->itemIdx.nDetail		= Item[nItemIdx].GetDetailType();
					pItemInfo->itemIdx.nParticular	= Item[nItemIdx].GetParticular();
					pItemInfo->itemIdx.nLevel		= Item[nItemIdx].GetLevel();
					pItemInfo->nGroup				= Item[nItemIdx].GetGroup();
					pItemInfo->bDestory				= Item[nItemIdx].CanDiscard();					
					pItemInfo->bIsBind				= !ItemCanTrade(CLIENT_PLAYER_INDEX,Item[nItemIdx].GetID());
					pItemInfo->bIsEquipBind			= (Item[nItemIdx].IsEquipBind() && !Item[nItemIdx].IsBind());
					//likun  
					pItemInfo->iReqLevel			= Item[nItemIdx].GetLevelRequirement();
					if ( ItemCanTrade(CLIENT_PLAYER_INDEX, Item[nItemIdx].GetID()))
					{
						pItemInfo->bVendue			= true;
					}
					else
					{
						pItemInfo->bVendue			= false;
					}
					pItemInfo->colour				= Item[nItemIdx].GetQualityLabel();
					//*/
					strcpy(pItemInfo->szToolTip, "<Layout width=200 margin-top=10 margin-left=10 margin-right=10 margin-bottom=10>");
					Item[nItemIdx].GetDesc( pItemInfo->szToolTip );
// 					Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(&Item[nItemIdx], pItemInfo->szToolTip);
// 					TalismanManager::Singleton().GetTalismanDesc(nItemIdx, pItemInfo->szToolTip);
// 					TalismanManager::Singleton().GetEnchaseItemDesc(nItemIdx, pItemInfo->szToolTip);
					strcat(pItemInfo->szToolTip, "</Layout>");

					return Item[nItemIdx].GetItemIndex()!= 0;
				}
			}

			return 0;
		}
		break;	

	case GDI_GUA_INFO_INDEX:
		{
			KItemInfo* pItemInfo = (KItemInfo*)uParam;
			if ( !pItemInfo )
				break;

			int guaIndex = nParam;
			if ( guaIndex < 0)
				break;
			
			YaoMonitor& yaoMonitor = Player[CLIENT_PLAYER_INDEX].GetItemList().GetYaoMonitor();
			const GuaState* guaState = yaoMonitor.GetGuaStates();
			if(guaState == NULL)
				break;
			const YaoData* yaoData = YaoTable::Singleton().GetYao(guaState[guaIndex].GuaID, guaState[guaIndex].Level);
			
			memcpy( pItemInfo->szImageSet, yaoData->Image[0], COMMON_CLIENT_MSG_LEN_128 );
			memcpy( pItemInfo->szImage, yaoData->Image[1], COMMON_CLIENT_MSG_LEN_128 );
			memcpy( pItemInfo->szName, yaoData->Name, COMMON_CLIENT_MSG_LEN_32 );
		}
		break;
	case GDI_TARGET_GUA_INFO_INDEX:
		{
			KItemInfo* pItemInfo = (KItemInfo*)uParam;
			if ( !pItemInfo )
				break;

			int guaIndex = nParam;
			if ( guaIndex < 0)
				break;
			
			YaoMonitor& yaoMonitor = g_cViewItem.GetYaoMonitor();
			const GuaState* guaState = yaoMonitor.GetGuaStates();
			if(guaState == NULL)
				break;
			const YaoData* yaoData = YaoTable::Singleton().GetYao(guaState[guaIndex].GuaID, guaState[guaIndex].Level);
			
			memcpy( pItemInfo->szImageSet, yaoData->Image[0], COMMON_CLIENT_MSG_LEN_128 );
			memcpy( pItemInfo->szImage, yaoData->Image[1], COMMON_CLIENT_MSG_LEN_128 );
			memcpy( pItemInfo->szName, yaoData->Name, COMMON_CLIENT_MSG_LEN_32 );
		}
		break;

	case GDI_ITEM_INFO_SHOP://等待删除（谢鉷 2006年11月7日）
		{
			KItemInfo* pItemInfo = (KItemInfo*)uParam;
			if ( pItemInfo )
			{
				int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
				int nIndex = BuySell.GetItemIndex(nBuyIdx, nParam);
				KItem* item = BuySell.GetItem(nIndex);
				int nItemIdx = nParam;
				if ( nItemIdx >= 0)
				{
					//likun判断装备男女
					char manOrWomanImage[512];
					if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0)
					{
						sprintf(manOrWomanImage, "%s%s", item->GetImageFile(), EQUIPMENT_MAN_POSTFIX_SMALL);
					}
					else
					{
						sprintf(manOrWomanImage, "%s%s", item->GetImageFile(), EQUIPMENT_WOMAN_POSTFIX_SMALL);
					}
					memcpy( pItemInfo->szImageSet, item->GetImageSetFile(), COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItemInfo->szImage, manOrWomanImage, COMMON_CLIENT_MSG_LEN_128 );
					pItemInfo->itemIdx.nGenre		= item->GetGenre();
					pItemInfo->itemIdx.nDetail		= item->GetDetailType();
					pItemInfo->itemIdx.nParticular	= item->GetParticular();
					pItemInfo->itemIdx.nLevel		= item->GetLevel();
					pItemInfo->nGroup				= item->GetGroup();
					pItemInfo->iReqLevel			= item->GetLevelRequirement();
				}
			}
		}
		break;	

	// uParam = (FIND_ITEMINDEX_PARAM*)uParam
	// nParam = (KItemInfo*)
	// Return = 
	case GDI_ITEM_INFO_PARTICULAR:
		{
			FIND_ITEMINDEX_PARAM* pItemInfo = (FIND_ITEMINDEX_PARAM*)uParam;
			KItemInfo* pItem = (KItemInfo*)nParam;
			if ( pItemInfo && pItem )
			{
				const KBASICPROP_ITEM* pTemplate = g_ItemGen.GetItemTemplate( pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel );
				if ( pTemplate )
				{
					char szItemImage[COMMON_CLIENT_MSG_LEN_128];
					ZeroMemory(szItemImage, COMMON_CLIENT_MSG_LEN_128);
					if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0)
					{
						sprintf(szItemImage, "%s%s", pTemplate->szImageName, EQUIPMENT_MAN_POSTFIX_SMALL);
					}
					else
					{
						sprintf(szItemImage, "%s%s", pTemplate->szImageName, EQUIPMENT_WOMAN_POSTFIX_SMALL);
					}
					
					memcpy( &pItem->itemIdx, pItemInfo, sizeof(FIND_ITEMINDEX_PARAM) );
					memcpy( pItem->szImageSet, pTemplate->szImageSetName, COMMON_CLIENT_MSG_LEN_128 );
					//memcpy( pItem->szImage, pTemplate->szImageName, COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItem->szImage, szItemImage, COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItem->szName, pTemplate->szName, COMMON_CLIENT_MSG_LEN_32 );
					pItem->nGroup = pTemplate->nGroup;
					pItem->iReqLevel = pTemplate->nReqLevel;
					// 得到物品的Tip
					KItem item;
					g_ItemGen.Gen_Item( pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel, 1, &item );

					strcpy(pItem->szToolTip, "<Layout width=200 margin-top=10 margin-left=10 margin-right=10 margin-bottom=10>");
					item.GetDesc(pItem->szToolTip);
					Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(&item, pItem->szToolTip);
					TalismanManager::Singleton().GetTalismanDesc(item, pItem->szToolTip);
					TalismanManager::Singleton().GetEnchaseItemDesc(item, pItem->szToolTip);
					strcat(pItem->szToolTip, "</Layout>");

					pItem->colour = item.GetQualityLabel();
					
					return 1;
				}
			}
			return 0;
		}
		break;
	case GDI_GET_PLUS_POINT_TEMPLATE:
		{
			PLUS_POINT_PARAM* pPlusPointParam =  (PLUS_POINT_PARAM*)nParam;
			if ( pPlusPointParam )
			{
				PlusPointTable& ppt = PlusPointTable::Singleton();
				ppt.GetPlusPointParam( 
					BuySell.GetPlusPointType(uParam),
					pPlusPointParam->name, 	
					pPlusPointParam->maxpluspoint );
			}
		}
		break;
	case GDI_GET_SHOP_IDX:
		{
			return Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
		}
		break;
	case GDI_GET_PLUS_POINT_INDEX_BY_SHOP_INDEX:
		{
			return BuySell.GetPlusPointType(nParam);
		}
		break;
	case GDI_GET_INSTEAD_SPECIE:
		{
			return Player[CLIENT_PLAYER_INDEX].GetPlusPoint(nParam);
		}
	case GDI_GET_PLUS_POINT:
		{
			return Player[CLIENT_PLAYER_INDEX].GetPlusPoint( BuySell.GetPlusPointType(uParam) ); 
		}
		break;
	case GDI_GET_PLUS_POINT_BY_PLUSPOINT_INDEX:
		{
			return Player[CLIENT_PLAYER_INDEX].GetPlusPoint( uParam ); 
		}
		break;
	case GDI_SHOP_ITEM_PRICE:
		{
			int	shopIndex = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			int itemIndex = (int)nParam;
			KItem* item = BuySell.getItem(shopIndex, itemIndex);
			if(NULL == item)
			{
				nRet = 0;
				break;
			}

			nRet = item->GetPrice();

			if (  BuySell.GetPlusPointType(shopIndex) != -1 )
			{
				nRet = BuySell.GetItemPlusPoint(shopIndex, itemIndex);
			}//endif
			else
			{
				
				int nCityTaxRate = Player[CLIENT_PLAYER_INDEX].m_CityTaxRate;
				nRet = ComputePrice(nRet, nCityTaxRate);
			}//end for else

			if (nRet <= 0)
				nRet = 1;
		}
		break;
	case GDI_SHOP_ITEM_SELL_PRICE:
		{
			int itemIndex = (int)nParam;
			KItem& item = Item[itemIndex];
			if(item.GetID() <= 0)
			{
				break;
			}

			if(item.GetMaxDurability() == 0)
				nRet = item.GetSellPrice();
			else
				nRet = item.GetSellPrice() * item.GetDurability() / item.GetMaxDurability();
			if (nRet <= 0)
				nRet = 1;
			if(item.CanSell() == false)
				nRet = -1;
		}
		break;
	case GDI_SHOP_ITEM_NAME:
		{
			int	shopIndex = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			int itemIndex = (int)nParam;
			KItem* item = BuySell.getItem(shopIndex, itemIndex);
			if(NULL == item)
			{
				nRet = 0;
				break;
			}
			
			char* name = (char*)uParam;

			strcpy(name, item->GetName());
			nRet = 1;
		}
		break;

	//游戏对象描述说明文本串
	//uParam = (KGameObject*) 描述游戏对象的结构数据的指针
	//nParam = (char*) 指向一个缓冲区的指针，其空间不少于256字节。
	case GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO:
	case GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO:
		if (nParam && uParam)
		{
			KObjAtContRegion* pObj = (KObjAtContRegion *)uParam;
			KGameObjDesc* pDescript = (KGameObjDesc *)nParam;
			pDescript->szDesc[0] = 0;
			pDescript->szProp[0] = 0;
			pDescript->szTitle[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					Item[pObj->Obj.uId].GetDesc(pDescript->szDesc);
				}
				break;	
			case CGOG_SKILL:
			case CGOG_SKILL_FIGHT:
			case CGOG_SKILL_LIVE:
			case CGOG_SKILL_SHORTCUT:
			//Lucifer~yu[zhangjianyu] [02/21/2006] Add for 
			//begin------------------------------------------------------------------------
			case CGOG_SKILL_BUFFER:
			//end--------------------------------------------------------------------------	
//				{
//					int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(pObj->Obj.uId);
//					_ASSERT(nLevel >= 0);
//					if (pObj->Obj.uId >0)
//					{
//						
//						ISkill * pISkill = g_SkillManager.GetSkill(pObj->Obj.uId, 1);
//						if (!pISkill)
//							break;
//						eSkillStyle eStyle = (eSkillStyle) pISkill->GetSkillStyle();
//						
//						switch(eStyle)
//						{
//						// Add by cooler
//						// liuyujun@263.net 2004/3/7 -->
//						case SKILL_SS_MustAttack:
//						// <-- End cooler add.
//						// Add by Cooler 2004-5-18
//						// Begin -->
//						case SKILL_SS_Produce:
//						// End <--
//						case SKILL_SS_Missles:			//	子弹类		本技能用于发送子弹类
//						case SKILL_SS_Melee:
//						case SKILL_SS_InitiativeNpcState:	//	主动类		本技能用于改变当前Npc的主动状态
//						case SKILL_SS_PassivityNpcState:		//	被动类		本技能用于改变Npc的被动状态
//							{
//								//Lucifer~yu[zhangjianyu] [02/21/2006] Add for 
//								//begin------------------------------------------------------------------------
//								if ( pObj->Obj.uGenre == CGOG_SKILL_BUFFER )
//								{
//									KSkill::GetDesc(
//										pObj->Obj.uId,
//										pObj->Region.h,
//										pDescript->szTitle,
//										Player[CLIENT_PLAYER_INDEX].m_nIndex,
//										(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true,
//										true
//										);								
//								}
//								else
//								{
//									KSkill::GetDesc(
//										pObj->Obj.uId,
//										nLevel,
//										pDescript->szTitle,
//										Player[CLIENT_PLAYER_INDEX].m_nIndex,
//										(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
//										);								
//								}
//								//end--------------------------------------------------------------------------	
//							}
//							break;
//							
//						// lixuewu 2004.02.11						
//						case SKILL_SS_Summon:
//							{
//								//Lucifer~yu[zhangjianyu] [02/21/2006] Add for 
//								//begin------------------------------------------------------------------------
//								if ( pObj->Obj.uGenre == CGOG_SKILL_BUFFER )
//								{
//									((KSummonSkill *)pISkill)->GetDesc(
//										pObj->Obj.uId,
//										nLevel,
//										pDescript->szTitle,
//										Player[CLIENT_PLAYER_INDEX].m_nIndex,
//										(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true,
//										true
//									);								
//								}
//								else
//								{
//									((KSummonSkill *)pISkill)->GetDesc(
//										pObj->Obj.uId,
//										nLevel,
//										pDescript->szTitle,
//										Player[CLIENT_PLAYER_INDEX].m_nIndex,
//										(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
//									);								
//								}
//								//end--------------------------------------------------------------------------	
//
//							}break;
//						// lixuewu 2004.02.11
//						}
//					}
//					
//				}
				break;

			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx)
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;

					pItem->GetDesc(pDescript->szDesc);
				}
				break;
			}
		}
		break;
	case GDI_MY_ITEM_LAYOUT_DESC:
		{
			int itemIndex = (nParam);
			char* layoutDes = (char*)uParam;

			KItem& item = Item[itemIndex];
			if(item.GetID() <= 0 || NULL == layoutDes)
			{
				break;
			}
			item.GetDesc(layoutDes);
			Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(&item, layoutDes);
			
			TalismanManager& tm = TalismanManager::Singleton();
			if (tm.IsValidTalisman(itemIndex))
			{
				TalismanManager::Singleton().GetTalismanDesc(itemIndex, layoutDes);
			}
			if (tm.IsValidEnchaseItem(itemIndex))
			{
				TalismanManager::Singleton().GetEnchaseItemDesc(itemIndex, layoutDes);
			}
		}
		break;
	case GDI_MY_GUA_LAYOUT_DESC:
		{
			int guaIndex = (nParam);
			char* layoutDes = (char*)uParam;
			
			if(NULL == layoutDes)
			{
				break;
			}
			YaoMonitor& guaInfo = Player[CLIENT_PLAYER_INDEX].GetItemList().GetYaoMonitor();
			guaInfo.GetGuaDesc((GUA_POS)guaIndex, layoutDes);
		}
		break;
	case GDI_OPPOSITE_ITEM_LAYOUT_DESC:
		{
			int itemIndex = (nParam);
			char* layoutDes = (char*)uParam;

			if(Item[itemIndex].GetID() <= 0 || NULL == layoutDes)
			{
				break;
			}
			Item[itemIndex].GetDesc(layoutDes);
			g_cViewItem.GetArmorSetMonitor().GetArmorSetDesc(&Item[itemIndex], layoutDes);
			//法宝信息
			TalismanManager& tm = TalismanManager::Singleton();
			if (tm.IsValidTalisman(itemIndex))
			{
				TalismanManager::Singleton().GetTalismanDesc(itemIndex, layoutDes);
			}
			if (tm.IsValidEnchaseItem(itemIndex))
			{
				TalismanManager::Singleton().GetEnchaseItemDesc(itemIndex, layoutDes);
			}
		}
		break;
	case GDI_OPPOSITE_GUA_LAYOUT_DESC:
		{
			int guaIndex = (nParam);
			char* layoutDes = (char*)uParam;

			if(NULL == layoutDes)
			{
				break;
			}
			YaoMonitor& guaInfo = g_cViewItem.GetYaoMonitor();
			guaInfo.GetGuaDesc((GUA_POS)guaIndex, layoutDes);
		}
		break;
	case GDI_INSIDE_BALL_LAYOUT_DESC:
		{
			TM_HOLE_POS* holePos = (TM_HOLE_POS*)nParam;
			char* layoutDes = (char*)uParam;

			if(NULL == layoutDes)
			{
				break;
			}
			TalismanManager::Singleton().GetEnchaseDesc(holePos->talismanId, holePos->holeIndex, layoutDes);
		}
		break;
	case GDI_LINKED_ITEM_LAYOUT_DESC:
		{
			FIND_ITEMINDEX_PARAM* itemType = (FIND_ITEMINDEX_PARAM *)nParam;
			char* layoutDes = (char*)uParam;

			if(NULL == layoutDes)
			{
				break;
			}
			
			KItem* item = g_cLinkItem.get(*itemType, itemType->nGroup);
			if(item == NULL)
			{
				nRet = 0;
				break;
			}
			item->GetDesc(layoutDes);
			Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(item, layoutDes);
			TalismanManager::Singleton().GetTalismanDesc(*item, layoutDes);
			TalismanManager::Singleton().GetEnchaseItemDesc(*item, layoutDes);
		}
		break;
	case GDI_VENDUE_ITEM_LAYOUT_DESC:
		{
			nRet = 0;

			char* layoutDes = (char*)uParam;
			TItemtransfersData* pItemInfo = (TItemtransfersData*)nParam;
			if(NULL == layoutDes)
			{
				break;
			}
			if(NULL == pItemInfo)
			{
				break;
			}
			
			KItem tempItem;
			BOOL genSuccess = g_ItemGen.Gen_Item( pItemInfo->igenre, 
				pItemInfo->idetailtype,
				pItemInfo->iparticulartype,
				pItemInfo->ilevel,
				pItemInfo->nItemCount,
				&tempItem );
			
			if(FALSE == genSuccess)
			{
				break;
			}
			tempItem.SetItemtransfersData( *pItemInfo );
			tempItem.GetDesc(layoutDes);
			Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(&tempItem, layoutDes);
			TalismanManager::Singleton().GetTalismanDesc(tempItem, layoutDes);
			TalismanManager::Singleton().GetEnchaseItemDesc(tempItem, layoutDes);

			nRet = 1;
		}
		break;
	case GDI_SHOP_ITEM_LAYOUT_DESC:
		{
			nRet = 0;
			
			char* layoutDes = (char*)uParam;
			int itemIndex = nParam;
			if(NULL == layoutDes)
			{
				break;
			}
			int	shopIndex = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			if (shopIndex < 0 || shopIndex >= BuySell.GetHeight())
				break;
			
			KItem* item = BuySell.getItem(shopIndex, itemIndex);
			
			if(NULL == item)
				break;
			item->GetDesc(layoutDes,true);
			Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(item, layoutDes);
			TalismanManager::Singleton().GetTalismanDesc(*item, layoutDes);
			TalismanManager::Singleton().GetEnchaseItemDesc(*item, layoutDes);

			int idxPPShopItemIdx = BuySell.GetItemIndex(shopIndex,itemIndex);	
			if ( idxPPShopItemIdx == -1 )
			{
				return 1;
			}
			
			PlusPointTable& ppt = PlusPointTable::Singleton();
			std::string pplimitStr;
			BuySell.GetItemPlusPointLimit(shopIndex,itemIndex, pplimitStr);
			ConfigManager& cm = ConfigManager::Singleton();
			if ( strncmp(NO_PLUS_POINT_LIMIT,pplimitStr.c_str(), sizeof(NO_PLUS_POINT_LIMIT) - 1 ) != 0 )
			{
				int ppLimit[PLUS_POINT_LIMIT_COUNT];
				memset(ppLimit,0,sizeof(ppLimit));
				::sscanf(pplimitStr.c_str(), "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", 
					&(ppLimit[0]), &(ppLimit[1]), &(ppLimit[2]), &(ppLimit[3]), &(ppLimit[4]), &(ppLimit[5]), 
					&(ppLimit[6]), &(ppLimit[7]), &(ppLimit[8]), &(ppLimit[9]),
					&(ppLimit[10]), &(ppLimit[11]), &(ppLimit[12]), &(ppLimit[13]), &(ppLimit[14]), &(ppLimit[15]), 
					&(ppLimit[16]), &(ppLimit[17]), &(ppLimit[18]), &(ppLimit[19]));
				
				for (int idxPPL = 0; idxPPL < PLUS_POINT_LIMIT_COUNT; ++idxPPL )
				{
					
					std::string ppName;
					ppt.GetPlusPointName(idxPPL,ppName);
					if (ppLimit[idxPPL] > 0 )
					{
						if (BuySell.IsPlusPointOk( idxPPShopItemIdx,CLIENT_PLAYER_INDEX) )
						{
							const char* szppTxt = cm.GetConfigurableDisplayStyle(style_pluspoint_limit_txt, true);
							if ( szppTxt )
							{
								char szTxt[COMMON_CLIENT_MSG_LEN_256];
								sprintf( szTxt, szppTxt, ppName.c_str(), ppLimit[idxPPL] );
								strcat(layoutDes, szTxt );
							}
						}
						else
						{
							const char* szppTxt = cm.GetConfigurableDisplayStyle(style_pluspoint_limit_txt, false);
							if ( szppTxt )
							{
								char szTxt[COMMON_CLIENT_MSG_LEN_256];
								sprintf( szTxt, szppTxt, ppName.c_str(), ppLimit[idxPPL] );
								strcat(layoutDes, szTxt );
							}
						}

					}
				}
			}			

			nRet = 1;
		}
		break;
	case GDI_MY_ITEM_EQUIP_POS:
		{
			nRet = itempart_unidentified;
			int itemIndex = (nParam);

			KItem& item = Item[itemIndex];
			if(!item.GetItemTemplate())
			{
				nRet = 0;
			}
			else if(item.GetGenre() == item_equip)
			{
				nRet = item.GetDetailType();
			}
		}
		break;
	case GDI_OPPOSITE_EQUIP_POS:
		{
			nRet = itempart_unidentified;
			int itemIndex = (nParam);

			KItem& item = Item[itemIndex];
			if(!item.GetItemTemplate())
			{
				nRet = 0;
			}
			else if(item.GetGenre() == item_equip)
			{
				nRet = item.GetDetailType();
			}
		}
		break;
	case GDI_LINKED_EQUIP_POS:
		{
			nRet = itempart_unidentified;
			FIND_ITEMINDEX_PARAM* itemType = (FIND_ITEMINDEX_PARAM *)nParam;
			if(NULL == itemType)
			{
				break;
			}
						
			KItem* item = g_cLinkItem.get(*itemType, itemType->nGroup);
			if(item == NULL )
			{
				nRet = 0;
				break;
			}
			if(!item->GetItemTemplate())
			{
				nRet = 0;
			}
			else if(item->GetGenre() == item_equip)
			{
				nRet = item->GetDetailType();
			}
		}
		break;
	case GDI_VENDUE_EQUIP_POS:
		{
			nRet = itempart_unidentified;

			TItemtransfersData* pItemInfo = (TItemtransfersData*)nParam;
			if(NULL == pItemInfo)
			{
				break;
			}

			if(pItemInfo->igenre == item_equip)
			{
				nRet = pItemInfo->idetailtype;
			}
		}
		break;
	case GDI_SHOP_EQUIP_POS:
		{
			nRet = itempart_unidentified;

			int itemIndex = nParam;
			int	shopIndex = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			if (shopIndex < 0 || shopIndex >= BuySell.GetHeight())
				break;

			KItem* item = BuySell.getItem(shopIndex, itemIndex);

			if(item->GetGenre() == item_equip)
			{
				nRet = item->GetDetailType();
			}
		}
		break;
	case GDI_EQUIP_INDEX_BY_EQUIP_POS:
		{
			int equipPos = nParam;

			if(equipPos == equip_weapon)
				equipPos = itempart_weapon;
			else if(equipPos == equip_armor)
				equipPos = itempart_armor;
			else if(equipPos == equip_shoulder)
				equipPos = itempart_shoulder;
			else if(equipPos == equip_boots)
				equipPos = itempart_boots;
			else if(equipPos == equip_amulet)
				equipPos = itempart_amulet;
			else if(equipPos == equip_pendant)
				equipPos = itempart_pendant;
			else if(equipPos == equip_helm)
				equipPos = itempart_helm;
			else if(equipPos == equip_cuff)
				equipPos = itempart_cuff;
			else if(equipPos == equip_ring)
				equipPos = itempart_ring;
			else if(equipPos == equip_talisman)
				equipPos = itempart_talisman;
			
			nRet = Player[CLIENT_PLAYER_INDEX].GetItemList().m_EquipItem[equipPos].nEquipIdx;
		}
		break;
	case GDI_GET_ITEM_QUALITY_BY_TYPE:
		{
			ItemType* type = (ItemType*)uParam;
			nRet = 0;
			const KBASICPROP_ITEM* templateItem = g_ItemGen.GetItemTemplate(type->genre, type->detail, type->particular, type->level);
			nRet = templateItem->nColor;
		}
		break;
	case GDI_ITEM_PRICE_LAYOUT_DATA:
		{
			ItemPriceLayout* data = (ItemPriceLayout*)uParam;
			BuySell.getPriceLayoutData(data);
		}
		break;
	case GDI_ITEM_CAN_TRADE:
		{
			int itemIndex = nParam;
			if(Item[itemIndex].IsLocked(-1) || !Item[itemIndex].CanExchange() || Item[itemIndex].IsBind())
			{
				nRet = false;
			}
			else
			{
				nRet = true;
			}
		}
		break;
	case GDI_EQUIP_COMPARE_TITLE_LAYOUT_DATA:
		{
			nRet = 0;
			char* layoutText = (char*)uParam;
			if(NULL == layoutText)
			{
				break;
			}
			KItem::getEquipCompareTitalLayoutStyle(layoutText);
			nRet = 1;
		}
		break;
	//主角的一些不易变的数据
	//uParam = (KUiPlayerBaseInfo*)pInfo	
	case GDI_PLAYER_BASE_INFO:
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			int nIndex = 0;
			if (nParam == 0)
			{
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
				pInfo->nSkillType = Player[CLIENT_PLAYER_INDEX].GetSkillSeries();
				pInfo->nPortrait = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].m_nHeadImage;
			}
			else
			{
				nIndex = NpcSet.SearchID(nParam);
				pInfo->nCurFaction = -1;
				pInfo->nCurTong = 0;
				pInfo->nSkillType = -1;
				pInfo->nPortrait = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].m_nHeadImage;
				pInfo->bTeam = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].m_UnaryAttrMgr[nuai_team_id] > 0;
			}
			if (nIndex)
			{
				pInfo->bTeam = Npc[nIndex].m_UnaryAttrMgr[nuai_team_id] > 0;
				strcpy(pInfo->Name, Npc[nIndex].Name);
			}
		}
		break;

	//主角的一些易变的数据
	//uParam = (KUiPlayerRuntimeInfo*)pInfo
	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			KPlayer& aPlayer	= Player[CLIENT_PLAYER_INDEX];
			KNpc&	aNpc		= Npc[aPlayer.m_nIndex];

			pInfo->nLifeFull	= aNpc.m_CompAttrMgr[ncai_lifeuplimit];		//生命满值
			pInfo->nLife		= aNpc.m_UnaryAttrMgr[nuai_curlife];		//生命
			pInfo->nManaFull	= aNpc.m_CompAttrMgr[ncai_manauplimit];		//内力满值
			pInfo->nMana		= aNpc.m_UnaryAttrMgr[nuai_curmana];		//内力

			pInfo->nAngryFull = 0;		//怒满值
			pInfo->nAngry = 0;			//怒
			
			pInfo->nExperienceFull		= aPlayer.GetExpMax();//经验满值
			pInfo->nExperience			= aPlayer.GetExp();//经验
			pInfo->nCurLevelExperience	= aPlayer.GetExpMax();
			
			pInfo->byActionDisable = 0;
			pInfo->byAction = PA_NONE;

// 			if (aPlayer.m_ItemList.GetEquipment(itempart_horse) <= 0)
// 				pInfo->byActionDisable |= PA_RIDE;

			pInfo->bSleeping =  aNpc.m_nSleepFlag;

			if (aPlayer.m_RunStatus)
				pInfo->byAction |= PA_RUN;

			if (aNpc.m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;

			if (aNpc.m_bRideHorse)
				pInfo->byAction |= PA_RIDE;

//			pInfo->cPKValue = aPlayer.m_cPK.GetPKValue();
			pInfo->nCurrentCamp = aNpc.m_CurrentCamp;
		}
		break;

	//主角的一些易变的属性数据
	//uParam = (KUiPlayerAttribute*)pInfo
	case GDI_PLAYER_RT_ATTRIBUTE:
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KPlayer& aPlayer	= Player[CLIENT_PLAYER_INDEX];
			KNpc*	pNpc		= &Npc[aPlayer.m_nIndex];

			pInfo->nMoney			= aPlayer.m_ItemList.GetMoney(room_equipment);	//银两
			pInfo->nBody			= Npc[aPlayer.m_nIndex].m_CompAttrMgr[ncai_body];		// 体，加生命上限
			pInfo->nNimbus			= Npc[aPlayer.m_nIndex].m_CompAttrMgr[ncai_nimbus]; 	// 灵，加气上限
			pInfo->nStrength		= Npc[aPlayer.m_nIndex].m_CompAttrMgr[ncai_strength];	// 力，加物理攻击
			pInfo->nArt				= Npc[aPlayer.m_nIndex].m_CompAttrMgr[ncai_art];		// 术，加法书攻击
			pInfo->nAttackRate		= pNpc->CalcVision();				// 命中率
			pInfo->nJinkRate		= pNpc->CalcDexterity();			// 闪避率
			pInfo->nMoveSpeed		= pNpc->m_CompAttrMgr[ncai_runspeed];		//移动速度
			pInfo->nAttackSpeed		= pNpc->m_CompAttrMgr[ncai_attackspeed];	//攻击速度
			pInfo->nCastSpeed		= pNpc->m_CompAttrMgr[ncai_castspeed];		//施法速度

			pInfo->nPhysicsAttackLow	= pNpc->CalcPhysicsDamage(idx_value_low);
			pInfo->nPhysicsAttackHight	= pNpc->CalcPhysicsDamage(idx_value_hight);
			pInfo->nMagicAttackLow		= pNpc->CalcMagicDamage(idx_value_low);
			pInfo->nMagicAttackHight	= pNpc->CalcMagicDamage(idx_value_hight);

			// 对于抗性，人物身上同一类抗性下面不同子类的值是相同的, 显示其中一个即可
			pInfo->nPhysicsDefendLow	= pNpc->CalcPhysicsDefense(idx_value_low);
			pInfo->nPhysicsDefendHight	= pNpc->CalcPhysicsDefense(idx_value_hight);
			pInfo->nEightDiaDefendLow	= pNpc->CalcEightDiagDfns(idx_value_low);
			pInfo->nEightDiaDefendHight	= pNpc->CalcEightDiagDfns(idx_value_hight);
			pInfo->nDarkDefendLow		= pNpc->CalcDarkDfns(idx_value_low);
			pInfo->nDarkDefendHight		= pNpc->CalcDarkDfns(idx_value_hight);

			pInfo->nPhysExplode			= pNpc->CalcPhysExplode();
			pInfo->nMagicExplode		= pNpc->CalcMagicExplode();
			
			pInfo->nLevel = pNpc->m_Level;	
			pInfo->nSeries = pNpc->m_Series;
		}
		break;


	/************************************************************************/
	/*							Target system                               */
	/************************************************************************/
	/*!
	\brief

	\param 
	
	\return
		
	*/
	case GDI_GET_MY_NAME:
		{
			char** name = (char**)uParam;
			*name = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name;
		}
		break;
		//删除
	case GDI_SCREEN_POS_OF_MAP:
		{
			Position* pos = (Position*)uParam;
			
		}
		break;
	case GDI_PLAYER_TARGET_INFO:
		{
			int nT = GetTargetNPC();
			if ( nT > 0 && uParam )
			{
				KNpc& targetNpc = Npc[nT];
				KTargetInfo* pTargetInfo = (KTargetInfo*)uParam;
				strncpy( pTargetInfo->strName, targetNpc.Name, sizeof(pTargetInfo->strName) );
				pTargetInfo->nLifePercentage = targetNpc.GetCurrentLifePercentage();
				pTargetInfo->nMagicPercentage = targetNpc.GetCurrentManaPercentage();
				pTargetInfo->nLevel = Npc[nT].m_Level;
				pTargetInfo->nIndex = nT;
				pTargetInfo->nId = Npc[nT].m_dwID;
				pTargetInfo->nPrivateState = Npc[nT].m_UnaryAttrMgr[nuai_camou_flage];
				
				if ( Npc[nT].m_Kind == kind_player ) 
				{
					pTargetInfo->szHeadImage = "";
					//pTargetInfo->nHeadImage = -1;
					switch( Npc[nT].m_NpcSettingIdx )
					{
					case -1:
						pTargetInfo->nSex = 0;
						pTargetInfo->nMetier = 0;
						break;
					case -2:
						pTargetInfo->nSex = 0;
						pTargetInfo->nMetier = 1;
						break;
					case -3:
						pTargetInfo->nSex = 0;
						pTargetInfo->nMetier = 2;
						break;
					case -4:
						pTargetInfo->nSex = 1;
						pTargetInfo->nMetier = 0;
						break;
					case -5:
						pTargetInfo->nSex = 1;
						pTargetInfo->nMetier = 1;
						break;
					case -6:
						pTargetInfo->nSex = 1;
						pTargetInfo->nMetier = 2;
						break;
					}
					pTargetInfo->nPortrait = Npc[nT].m_nHeadImage;
					pTargetInfo->nSkillSeries = Npc[nT].m_SkillType;
					pTargetInfo->nTeamId = Npc[nT].m_UnaryAttrMgr[nuai_team_id];
				}
				else
				{
					pTargetInfo->szHeadImage = Npc[nT].m_HeadImage;
					pTargetInfo->szHeadImageSet = Npc[nT].m_HeadImageSet;
					pTargetInfo->nSex = -1;
					pTargetInfo->nMetier = -1;
					pTargetInfo->nSkillSeries = -1;
					pTargetInfo->nTeamId = INVALID_TEAM_ID;
				}

				/*
				if ( Npc[nT].IsEmployee() )
				{
					ConfigManager& cm = ConfigManager::Singleton();
					const char* strEmployee = cm.GetConfigurableDisplayStyle( style_npc_name_plus, kind_employee );
					if ( strEmployee && strEmployee[0] != 0 )
					{
						strncpy( pTargetInfo->strName, strEmployee, 9 );
						pTargetInfo->strName[8] = 0;
					}
					else
					{
						strncpy( pTargetInfo->strName, PLAYER_EMPLOYEE_NAME_EXTRA, 9 );
						pTargetInfo->strName[8] = 0;
					}
					
					strncat( pTargetInfo->strName, Npc[nT].Name, sizeof(Npc[nT].Name) );
				}//*/

				nRet = 1;
			}
			else
			{
				KTargetInfo* pTargetInfo = (KTargetInfo*)uParam;
				strncpy( pTargetInfo->strName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, sizeof(pTargetInfo->strName) );
				pTargetInfo->nLifePercentage = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurrentLifePercentage();
				pTargetInfo->nMagicPercentage = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurrentManaPercentage();
				pTargetInfo->nSex		= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex;
				pTargetInfo->nMetier	= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Series;
				pTargetInfo->nLevel		= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;
				pTargetInfo->nIndex		= Player[CLIENT_PLAYER_INDEX].m_nIndex;
				pTargetInfo->nId		= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
				pTargetInfo->nSkillSeries = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillType;
				pTargetInfo->nTeamId	= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_UnaryAttrMgr[nuai_team_id];
				nRet = 0;
			}
		}
		break;
	case GDI_GET_PLAYER_INFO_BY_NPCID:
		{
			int npcId = (int)nParam;
			KTargetInfo* pTargetInfo = (KTargetInfo*)uParam;
			int npcIndex = NpcSet.SearchID(npcId);
			if (npcIndex > 0 && npcIndex < MAX_NPC)
			{
				KNpc& targetNpc = Npc[npcIndex];
				strncpy( pTargetInfo->strName, targetNpc.Name, sizeof(pTargetInfo->strName) );
				pTargetInfo->nLifePercentage = targetNpc.GetCurrentLifePercentage();
				pTargetInfo->nMagicPercentage = targetNpc.GetCurrentManaPercentage();
				pTargetInfo->nLevel = Npc[npcIndex].m_Level;
				pTargetInfo->nIndex = npcIndex;
				pTargetInfo->nId = Npc[npcIndex].m_dwID;

				pTargetInfo->nPrivateState = Npc[npcIndex].m_UnaryAttrMgr[nuai_camou_flage];

				if ( Npc[npcIndex].m_Kind == kind_player ) 
				{
					pTargetInfo->szHeadImage = "";
					//pTargetInfo->nHeadImage = -1;
					switch( Npc[npcIndex].m_NpcSettingIdx )
					{
					case -1:
						pTargetInfo->nSex = 0;
						pTargetInfo->nMetier = 0;
						break;
					case -2:
						pTargetInfo->nSex = 0;
						pTargetInfo->nMetier = 1;
						break;
					case -3:
						pTargetInfo->nSex = 0;
						pTargetInfo->nMetier = 2;
						break;
					case -4:
						pTargetInfo->nSex = 1;
						pTargetInfo->nMetier = 0;
						break;
					case -5:
						pTargetInfo->nSex = 1;
						pTargetInfo->nMetier = 1;
						break;
					case -6:
						pTargetInfo->nSex = 1;
						pTargetInfo->nMetier = 2;
						break;
					}
				}
				else
				{
					pTargetInfo->szHeadImage = Npc[npcIndex].m_HeadImage;
					pTargetInfo->szHeadImageSet = Npc[npcIndex].m_HeadImageSet;
					//pTargetInfo->nHeadImage = Npc[npcIndex].m_HeadImage;
					pTargetInfo->nSex = -1;
					pTargetInfo->nMetier = -1;
				}
				nRet = 1;
			}
			else
			{
				KTargetInfo* pTargetInfo = (KTargetInfo*)uParam;
				strncpy( pTargetInfo->strName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, sizeof(pTargetInfo->strName) );
				pTargetInfo->nLifePercentage = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurrentLifePercentage();
				pTargetInfo->nMagicPercentage = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetCurrentManaPercentage();
				pTargetInfo->nSex		= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex;
				pTargetInfo->nMetier	= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Series;
				pTargetInfo->nLevel		= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level;
				pTargetInfo->nIndex		= Player[CLIENT_PLAYER_INDEX].m_nIndex;
				pTargetInfo->nId		= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_dwID;
				nRet = 0;
			}
		}
		break;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//主角随身携带的钱
	//nRet = 主角随身携带的钱
	case GDI_PLAYER_HOLD_MONEY:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
		break;

	case GDI_PLAYER_JINSHANBI:	
		nRet = GetClientPlayer().GetJinshanbi(  );
		break;

	case GDI_PLAYER_MONEY_INFO:
		nRet = GetClientPlayer().GetIBMoney((MoneyType)uParam);
		break;

	case GDI_PLAYER_CREDIT_INFO:
		{
			PlayerCreditInfo* pCreditInfo = (PlayerCreditInfo*)uParam;
			nRet = 0;
			if (pCreditInfo)
			{
				pCreditInfo->uCreditState	= Player[CLIENT_PLAYER_INDEX].GetCreditState();
				pCreditInfo->uReturnTime	= Player[CLIENT_PLAYER_INDEX].GetCreditReturnTime();
				Player[CLIENT_PLAYER_INDEX].GetIBMoneySize( (MoneyType)enIB_SHOP, 
					pCreditInfo->uMinCredit[enIB_SHOP], pCreditInfo->uMaxCredit[enIB_SHOP] );
				Player[CLIENT_PLAYER_INDEX].GetIBMoneySize( (MoneyType)enCREDIT_SHOP, 
					pCreditInfo->uMinCredit[enCREDIT_SHOP], pCreditInfo->uMaxCredit[enCREDIT_SHOP] );
				Player[CLIENT_PLAYER_INDEX].GetIBMoneySize( (MoneyType)enPRESENT_SHOP, 
					pCreditInfo->uMinCredit[enPRESENT_SHOP], pCreditInfo->uMaxCredit[enPRESENT_SHOP] );
				nRet = 1;
			}
		}
		break;
	case GDI_MAX_BUY_LIMIT_BY_MONEY_TYPE:
		{
			static ConfigManager &cfg = ConfigManager::Singleton();
			enumIBGlobalVariable type = (enumIBGlobalVariable)(ib_global_var_jinshanbi_exchange_limit + uParam);
			nRet = cfg.GetIBGlobalVariable(type);
		}
		break;
	case GDI_CREDIT_TO_TICKET_RATE:
		{
			static ConfigManager &cfg = ConfigManager::Singleton();
			nRet = cfg.GetIBGlobalVariable(ib_global_var_credit_to_ticket_rate);
			if (nRet <= 0)
				nRet = 100;
		}
		break;
	case GDI_CREDIT_LEVEL_LIMIT:
		{
			static ConfigManager &cfg = ConfigManager::Singleton();
			nRet = cfg.GetIBGlobalVariable(ib_global_var_credit_level_limit);
		}
		break;
	case GDI_PLAYER_PRESENT_TICKET_COUNT:
		{
			FIND_ITEMINDEX_PARAM pItemInfo;
			static ConfigManager &cfg = ConfigManager::Singleton();
			cfg.GetIBTicketId(&pItemInfo.nGenre,
				&pItemInfo.nDetail, &pItemInfo.nParticular, &pItemInfo.nLevel);

			nRet = GetClientPlayer().m_ItemList.m_Room[room_equipment].HaveValidIBItemNum(pItemInfo.nGenre,
				pItemInfo.nDetail, pItemInfo.nParticular, pItemInfo.nLevel);
		}
		break;
	
	case GDI_BUY_IBITEM_MONEY_TYPE:
		{
			FIND_ITEMINDEX_PARAM* pItemInfo = (FIND_ITEMINDEX_PARAM*)uParam;
			if ( pItemInfo )
			{
				const KBASICPROP_ITEM* pTemplate = g_ItemGen.GetItemTemplate(
					pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel);
				if ( pTemplate )
				{
					KItem item;
					if( g_ItemGen.Gen_Item(
						pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel, 1, &item) )
						nRet = item.GetAvailableIBBuyType();
					else
						nRet = IIBT_INVALID;
				}
			}
		}
		break;
	case GDI_GET_IBITEM_BIG_IMAGE:
		{
			FIND_ITEMINDEX_PARAM* pItemInfo = (FIND_ITEMINDEX_PARAM*)uParam;
			KItemInfo* pItem = (KItemInfo*)nParam;
			if ( pItemInfo && pItem )
			{
				const KBASICPROP_ITEM* pTemplate = g_ItemGen.GetItemTemplate( pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel );
				if ( pTemplate )
				{					
					memcpy( &pItem->itemIdx, pItemInfo, sizeof(FIND_ITEMINDEX_PARAM) );
					memcpy( pItem->szImageSet, pTemplate->szBigImageSetName, COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItem->szImage, pTemplate->szBigImageName, COMMON_CLIENT_MSG_LEN_128 );
					//memcpy( pItem->szImage, szItemImage, COMMON_CLIENT_MSG_LEN_128 );
					memcpy( pItem->szName, pTemplate->szName, COMMON_CLIENT_MSG_LEN_32 );
					pItem->nGroup = pTemplate->nGroup;
					pItem->iReqLevel = pTemplate->nReqLevel;
					// 得到物品的Tip
					KItem item;
					g_ItemGen.Gen_Item( pItemInfo->nGenre, pItemInfo->nDetail, pItemInfo->nParticular, pItemInfo->nLevel, 1, &item );

					strcpy(pItem->szToolTip, "<Layout width=200 margin-top=10 margin-left=10 margin-right=10 margin-bottom=10>");
					item.GetDesc(pItem->szToolTip,true);
					Player[CLIENT_PLAYER_INDEX].GetItemList().GetArmorSetMonitor().GetArmorSetDesc(&item, pItem->szToolTip);
					TalismanManager::Singleton().GetTalismanDesc(item, pItem->szToolTip);
					TalismanManager::Singleton().GetEnchaseItemDesc(item, pItem->szToolTip);
					strcat(pItem->szToolTip, "</Layout>");

					pItem->colour = item.GetQualityLabel();
					
					return 1;
				}
			}
			return 0;
		}
		break;
	case GDI_IS_PLAYER_IN_COMBAT_WORLD:
		{
			if ( Player[CLIENT_PLAYER_INDEX].m_nIndex < 0 ||
				Player[CLIENT_PLAYER_INDEX].m_nIndex >= MAX_NPC )
			{
				return 0;
			}
			KNpc& npc = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			nRet = npc.IsInWorldCombatInstance() && IsValidCombatID(npc.m_WorldCombatOrg);
		}
		break;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//主角随身携带的钱
	//nRet = 主角随身携带的钱
	case GDI_PLAYER_STORE_MONEY:
		nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
		break;

	// Add by [Adt.X], 2004-7-19
	// 主角当前负重参数
	// nRet = 获取是否成功
	// uParam = int* 返回主角当前携带重量
	// nParam = int* 返回主角最大携带重量
	case GDI_PLAYER_WEIGHT_CURRENT:
		nRet = Player[CLIENT_PLAYER_INDEX].GetWeightCurrent((int*)uParam, (int*)nParam);
		break;
	// End.

	//Lucifer~yu[zhangjianyu] [02/08/2006] Add for 
	//begin------------------------------------------------------------------------
	case GDI_PLAYER_ISPRERIDE:
	//nRet = (bool)bIsPreRide是否正在进行上马动作
	//0 - 没有
	//1 - 正在
		{
			return Player[CLIENT_PLAYER_INDEX].m_bIsPreRide;
		}
		break;
	//end--------------------------------------------------------------------------	
	//----->Add by [Ray] 2004-4-2
	//查询重叠物品的个数
	case GDI_ITEM_COUNT_QUERY:
		nRet = 0;
		if ( uParam < MAX_ITEM )
		{
			nRet = Item[uParam].GetItemCount();			
		}
		break;
		//<-----Add End
	case GDI_ITEM_TAKEN_WITH:
		{
			nRet = 0;
			
			KObjAtContRegion* objBuff = (KObjAtContRegion*)uParam;
			int	buffLen = nParam;
			if(!objBuff)
			{
				break;
			}
			
			if(!buffLen)
			{
				break;
			}
			
			int usedBuffCount = 0;
			
			//物品
			KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();

			for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
			{
				PlayerItem& item = itemList.m_Items[i];
				if(item.nPlace == pos_equiproom)
				{
					objBuff->Obj.uGenre = CGOG_ITEM;
					objBuff->Obj.uId = item.nIdx;		
					objBuff->Region.h = item.nX;
					objBuff->Region.v = item.nY;
					objBuff->Region.Height = Item[item.nIdx].GetItemCount();//Height用来记录叠加个数
					++usedBuffCount;
					++objBuff;
				}
			}

			nRet = usedBuffCount;
		}
		break;
	case GDI_BAG_EXTEND:
		{
			nRet = 0;
			KObjAtContRegion* regionBuff = (KObjAtContRegion*)uParam;
			int buffLen = nParam;
			if(!regionBuff || !buffLen)
			{
				_ASSERT(0);
				break;
			}

			int usedBuffCount = 0;
			KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
			for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
			{
				PlayerItem& item = itemList.m_Items[i];
				if(item.nPlace == pos_itembox_extend)
				{
					regionBuff->eContainer = UOC_ITEMBOX_EXTEND;
					regionBuff->Obj.uGenre = CGOG_ITEM;
					regionBuff->Obj.uId = item.nIdx;
					regionBuff->Region.h = item.nX;
					regionBuff->Region.v = item.nY;
					usedBuffCount++;
					regionBuff++;
				}
			}
			nRet = usedBuffCount;
		}
		break;
	case GDI_STORE_EXTEND:
		{
			nRet = 0;
			KObjAtContRegion* regionBuff = (KObjAtContRegion*)uParam;
			int buffLen = nParam;
			if(!regionBuff || !buffLen)
			{
				_ASSERT(0);
				break;
			}

			int usedBuffCount = 0;
			KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
			for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
			{
				PlayerItem& item = itemList.m_Items[i];
				if(item.nPlace == pos_store_extend)
				{
					regionBuff->eContainer = UOC_ITEMBOX_EXTEND;
					regionBuff->Obj.uGenre = CGOG_ITEM;
					regionBuff->Obj.uId = item.nIdx;
					regionBuff->Region.h = item.nX;
					regionBuff->Region.v = item.nY;
					usedBuffCount++;
					regionBuff++;
				}
			}
			nRet = usedBuffCount;
		}
		break;
	//主角装备物品
	case GDI_EQUIPMENT:
		{
			nRet = 0;
			
			KObjAtContRegion* pInfo = (KObjAtContRegion*)uParam;
			if(NULL == pInfo)
				break;
			
			int nCount = 0;
			
			for (int i = 0; i < itempart_num; i++)
			{
				int index = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);

				if (index > 0)
				{
					pInfo[i].Obj.uGenre = CGOG_ITEM;
					pInfo[i].Obj.uId = index;

					pInfo[i].Region.h = i;
					pInfo[i].Region.v = -1;
					pInfo[i].Region.Width = 0;//0表示不是挂位
					pInfo[i].Region.Height = Item[index].GetItemCount();
					nCount++;
				}
				else
				{
					pInfo[i].Obj.uGenre = CGOG_NOTHING;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_TALISMAN_SELF:
		{
			TALISMAN_INFO* pTalismanInfo = (TALISMAN_INFO*)uParam;
			int talismanId = nParam;
			
			nRet = 0;
			if(pTalismanInfo != NULL)
			{
				int talismanIndex = ItemSet.SearchID(talismanId);
				if (TalismanManager::Singleton().IsValidTalisman(talismanIndex))
				{
					KItem& talisman = Item[talismanIndex];
					
					pTalismanInfo->id = talisman.GetID();
					pTalismanInfo->level = talisman.GetTalismanLevel();
					pTalismanInfo->maxYunHun = talisman.GetTalismanPotentialLimit();
					pTalismanInfo->curYunHun = talisman.GetTalismanPotential();
					pTalismanInfo->canUplevel = TalismanManager::Singleton().IsUpgradeable(talismanIndex) ? 1 : 0;
					strncpy(pTalismanInfo->name, talisman.GetName(), sizeof(pTalismanInfo->name));
					TalismanManager::Singleton().GetTalismanEffectDesc(talismanIndex, pTalismanInfo->description, sizeof(pTalismanInfo->description));

					for (int enchaseLoopCount = 0; enchaseLoopCount < TM_HOLE_NUM; enchaseLoopCount++)
					{
						pTalismanInfo->holeBuffId[enchaseLoopCount] = talisman.GetTalismanEnchase(enchaseLoopCount);
					}
					
					nRet = 1;
				}
			}

			break;
		}		

	case GDI_SKILLEXP_TO_TAILSMAN_EXP:
		{
			nRet=ConfigManager::Singleton().GetGlobalVariable(global_var_talisman_potential_convert_rate);
		}
		break;

	case GDI_IS_TALISMAN:
		{
			int itemId = (int)uParam;
			int itemIndex = ItemSet.SearchID(itemId);
			nRet = TalismanManager::Singleton().IsValidTalisman(itemIndex);
		}
		break;
	case GDI_IS_INSIDE_BALL:
		{
			int itemId = (int)uParam;
			int itemIndex = ItemSet.SearchID(itemId);
			nRet = TalismanManager::Singleton().IsValidEnchaseItem(itemIndex);
		}
		break;
	case GDI_SELF_GUA_OVERVIEW:
		nRet = 0;
		if (uParam)
		{
			int count = 0;
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)uParam;

			YaoMonitor& guaInfo = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetYaoMonitor();
			const GuaState* guaState = guaInfo.GetGuaStates();
			
			for (int i = 0; i < gua_pos_count; i++)
			{
				if (guaState[i].GuaID != -1)
				{
					itemRegion[i].Obj.uGenre = CGOG_ITEM;
					count++;
				}
				else
				{
					itemRegion[i].Obj.uGenre = CGOG_NOTHING;
				}
				itemRegion[i].Obj.uId = guaState[i].GuaID;
				itemRegion[i].Region.h = i;
				itemRegion[i].Region.v = -1;
				itemRegion[i].Region.Width = 1;//1表示挂位
			}
			nRet = count;
		}
		break;
	case GDI_TARGET_GUA_OVERVIEW:
		nRet = 0;
		if (uParam)
		{
			int count = 0;
			KObjAtContRegion* itemRegion = (KObjAtContRegion*)uParam;

			YaoMonitor& guaInfo = g_cViewItem.GetYaoMonitor();
			const GuaState* guaState = guaInfo.GetGuaStates();
			
			for (int i = 0; i < gua_pos_count; i++)
			{
				if (guaState[i].GuaID != -1)
				{
					itemRegion[i].Obj.uGenre = CGOG_ITEM;
					itemRegion[i].Obj.uId = guaState[i].GuaID;
					itemRegion[i].Region.h = i;
					itemRegion[i].Region.v = -1;
					itemRegion[i].Region.Width = 3;//1表示目标装备挂位
					count++;
				}
				else
				{
					itemRegion[i].Obj.uGenre = CGOG_NOTHING;
				}
			}
			nRet = count;
		}
		break;
	case GDI_PARADE_EQUIPMENT:
		{
			nRet = 0;
			KObjAtContRegion* pInfo = (KObjAtContRegion*)uParam;
			if (NULL == pInfo)
				break;
			
			int nCount = 0;
			for (int i = 0; i < itempart_num; i++)
			{
				pInfo[i].Obj.uId = g_cViewItem.m_sItem[i].nIdx;
				if (pInfo[i].Obj.uId)
				{
					pInfo[i].Obj.uGenre = CGOG_ITEM;
					pInfo[i].Region.h = i;
					pInfo[i].Region.v = -1;
					nCount++;
				}
				else
				{
					pInfo[i].Obj.uGenre = CGOG_NOTHING;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_TARGET_ROLE_INFO:
		{
			TargetPlayerInfo* playerInfo = (TargetPlayerInfo*)uParam;
			
			playerInfo->name = g_cViewItem.getName();
			playerInfo->shizu = g_cViewItem.getShizu();
			playerInfo->zhuhou = g_cViewItem.getZhuhou();
			playerInfo->lianmen = g_cViewItem.getLianmen();
			playerInfo->chenghao = g_cViewItem.getChenghao();
			
			playerInfo->npcId	= g_cViewItem.getId();
			playerInfo->pkValue	= g_cViewItem.getPkvalue();
			
			int nIdx = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetTargetNpc();
			if ( nIdx )
			{
				playerInfo->skillType = Npc[nIdx].m_SkillType;
				playerInfo->metier = Npc[nIdx].m_Series;
			}
			else
			{
				playerInfo->skillType = -1;
				playerInfo->metier = -1;
			}
		}
		break;
	/************************************************************************/
	/*						Skill system                                    */
	/************************************************************************/
	/*!
	\brief
		Get player immediacy skill list.

	\Param
		Immediacy skill list;

	\return
		Nothing;
	*/
	case GDI_PLAYER_IMMED_ITEMSKILL:
		if ( uParam )
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();
		}
		break;
	/*!
	\brief
		Get left enable skills.

	\Param
		Immediacy skill list;

	\return
		Nothing;
	*/
	case GDI_LEFT_ENABLE_SKILLS:
		{
			return s_LSkill;
		}
		break;

	/*!
	\brief
		Get right enable skills.

	\Param
		Immediacy skill list;

	\return
		Nothing;
	*/
	case GDI_RIGHT_ENABLE_SKILLS:
		{
			return s_RSkill;
		}
		break;

	/*!
	\brief
		Request the kind of skill list.
	
	\return
		Skill points.	
	*/
	case GDI_SKILL_POINT:
		{

		}
		break;

	/*!
	\brief
		Request the kind of skill list.
	
	\return
		Nothing.	
	*/
	case GDI_SKILL_KIND_LIST:
		{
			int	nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			nRet = Npc[nNpcIdx].m_SkillList.GetMainSkillIds((int*)uParam, nParam);
		}
		break;

	/*!
	\brief
		Request skill list.
	
	\return
		Nothing.	
	*/
	case GDI_SKILL_LIST:
		{
			int nArrayCapacity = HIWORD(nParam);
			int nMainSkillId = LOWORD(nParam);
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;

			nRet = Npc[nNpcIdx].m_SkillList.GetSubSkillIds(nMainSkillId, (int*)uParam, nArrayCapacity);
		}
		break;

	/*!
	\brief
		Request skill information.
	
	\return
		Nothing.	
	*/
	case GDI_SKILL_INFO:
		{
			KSkillInfo*   pSkillInfo	= (KSkillInfo*)uParam;
			
			char *        pStudyTipBuff = pSkillInfo->szStudyTipBuff;
			int           nStudyTipSize = pSkillInfo->nStudyTipBuffSize;
			
			int			  nSkillId	= (int)nParam;
			int			  nNpcIdx		= Player[CLIENT_PLAYER_INDEX].m_nIndex;
			int			  nSkillIdx	= Npc[nNpcIdx].m_SkillList.FindSkill(nSkillId);	
			
			bool          bDesc = pSkillInfo->bDescAvailable;
			bool          bNextDesc = pSkillInfo->bNextDescAvailable;
			unsigned long dwDescStyle = pSkillInfo->dwDescStyle;
			unsigned long dwNextDescStyle = pSkillInfo->dwNextDescStyle;
			ConfigManager& cm = ConfigManager::Singleton();
			// if skill level up, can't find index
			if (nSkillIdx == -1)
			{
				nSkillId = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(nSkillId);
				nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSkill(nSkillId);
			}
			
			memset(pSkillInfo, 0, sizeof(KSkillInfo));

			pSkillInfo->nSkillPoint = Player[CLIENT_PLAYER_INDEX].GetSkillExp();
			pSkillInfo->nSkillMaxPoint = Player[CLIENT_PLAYER_INDEX].GetSkillExpMax();

			int nTmpSkillId = Npc[GetClientPlayer().GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(nSkillId);
			int selectedSkillId = Npc[GetClientPlayer().GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(PlayerController::Singleton().GetSelectedSkill());
			pSkillInfo->bIsSelected = (selectedSkillId == nTmpSkillId);
			pSkillInfo->bCanHumanUse=false;

			if(-1 != nSkillIdx)
			{
				int		nLevel	= Npc[nNpcIdx].m_SkillList.GetLevelByIdx(nSkillIdx);
				int		nStatus	= Npc[nNpcIdx].m_SkillList.GetStatusByIdx(nSkillIdx);

				KSkill	*pSkill = g_SkillManager.GetSkill(nSkillId, nLevel > 0 ? nLevel : 1);
				
				if(NULL != pSkill)
				{
					if (g_SkillManager.IsSubSkill(nSkillId))
					{
						unsigned long nSeries=0;
						nSeries=g_SkillManager.GetSubSkillIdByLvl(nSkillId,1);
						pSkillInfo->pSkillCond = g_SkillManager.GetSkillCond(nSeries, nLevel + 1);
					}//endif
					else
						pSkillInfo->pSkillCond = NULL;

					pSkillInfo->eSkillLR = pSkill->GetSkillLRInfo();
					pSkillInfo->nLevel  = nLevel;
					pSkillInfo->bActive = (skill_status_usable == nStatus);
					pSkillInfo->nCoolingTime = pSkill->GetDelayPerCast();
					pSkillInfo->bPassivity	 = (SKILL_SS_PassivityNpcState == pSkill->GetSkillStyle());
					pSkillInfo->bCanStudy = (skill_status_inactive != nStatus);
					pSkillInfo->bCDComplete	= Npc[nNpcIdx].m_SkillList.IsClearCooling( nSkillIdx );
					pSkillInfo->bIsCommonCoolingDown = g_SkillManager.IsCommonCoolDown( nSkillId );
					strncpy( pSkillInfo->szIconName, pSkill->m_szSkillIcon, sizeof(pSkillInfo->szIconName)  );
					strncpy( pSkillInfo->szName, pSkill->m_szName, sizeof(pSkillInfo->szName) );
//					strncpy( pSkillInfo->szDemand, pSkill->m_szDemand, sizeof(pSkillInfo->szDemand) );
//					strncpy( pSkillInfo->szTooltip, pSkill->m_szTooltip, sizeof(pSkillInfo->szTooltip) );
					pSkillInfo->nCostSkillExp = pSkill->GetCostSkillExp();
					pSkillInfo->bCanHumanUse = !(g_SkillManager.IsValueSkill(nSkillId) || pSkill->GetSkillStyle()==SKILL_SS_PassivityNpcState || g_SkillManager.IsMainSkill(nSkillId));
					
                    if (bDesc ) //Notice Generate the desc is time wasted , do it if we needed
					{
						bool bAvailable=true;
						if (dwDescStyle)
							bAvailable=false;
						
                        g_SkillManager.GenDesc(nSkillId,nLevel,pSkillInfo->szDesc,sizeof(pSkillInfo->szDesc),!bAvailable);
					}//endif
					else
					{
						pSkillInfo->szDesc[0]=0;
					}//end else
				}
				
				int nNextLevId = g_SkillManager.GetSubSkillIdByLvl(nSkillId, nLevel + 1);
				
				if(-1 != nNextLevId && bNextDesc)
				{
						bool bAvailable=true;
						if (dwNextDescStyle)
							bAvailable=false;
						g_SkillManager.GenDesc(nNextLevId,nLevel + 1,pSkillInfo->szDescNext,sizeof(pSkillInfo->szDescNext),bAvailable);
				}//endif
				else
				{
					    pSkillInfo->szDescNext[0]=0;
				}//end else
			}
			
			//Auto create the desc
			
			//Get the study tip layout style
			if (pStudyTipBuff && nStudyTipSize>0) 
			{
                unsigned long dwTotalSize=1;
				pStudyTipBuff[0]=0;
				char *        pDest=pStudyTipBuff;
                
				
				//1. Current Skill Level Info.....................................................................................................................................................................................
			
				unsigned long dwSizeNeeded=strlen(pSkillInfo->szDesc);
				
				if (dwSizeNeeded!=0)
				{
					if (dwTotalSize+dwSizeNeeded<nStudyTipSize)	
					{
						strcat(pDest,pSkillInfo->szDesc);
						dwTotalSize+=dwSizeNeeded;
						pDest+=dwSizeNeeded;
					}//endif
					else
						break;

					//Roll back </layout>
					while((*pDest)!='<')
					{
						pDest--;
						dwTotalSize--;
					}

					(*pDest)=0;

					//0.1 Line
						
					const char *  szLineTemp = cm.GetConfigurableDisplayStyle(style_skill_tip_line, 0);
					if (szLineTemp!=NULL)
					{
						dwSizeNeeded=strlen(szLineTemp);
						if (dwTotalSize+dwSizeNeeded<nStudyTipSize)	
						{
							strcat(pDest,szLineTemp);
							dwTotalSize+=dwSizeNeeded;
							pDest+=dwSizeNeeded;
						}//endif
						else
							break;
					}//endif

				}//endif
				else
				{
					const char * szHeadStyle=cm.GetConfigurableDisplayStyle(style_skill_tip_head,0);
					if (szHeadStyle!=NULL)
					{
						dwSizeNeeded=strlen(szHeadStyle);
						
						if (dwTotalSize+dwSizeNeeded<nStudyTipSize)	
						{
							strcat(pDest,szHeadStyle);
							dwTotalSize+=dwSizeNeeded;
							pDest+=dwSizeNeeded;
						}//endif
						else
							break;
					}//endif
				}
		
				
				//1. Next Skill Level Need.....................................................................................................................................................................................
                char szNextLevelNeeded[512];
				char szColor[32];
				
				if (bDesc && bNextDesc)
				{
                       sprintf(szColor,"color=128,128,128");
				}
				else
					sprintf(szColor,"color=255,255,255");

                if (pSkillInfo->pSkillCond)
				{
					if (cm.GetConfigurableDisplayStyle(style_skill_tip_next_level,0)!=NULL)
					{
						int jin = 0;
						int yin = 0;
						int tong = 0;
						_sysMoneyToUiMoney( pSkillInfo->pSkillCond->nCostMoney, jin, yin, tong );
						char szBuff[COMMON_CLIENT_MSG_LEN_64];
						sprintf( szBuff, MSG_JIN_YIN_TONG, jin, yin, tong );
						sprintf(szNextLevelNeeded,cm.GetConfigurableDisplayStyle(style_skill_tip_next_level,0),szColor,szColor,pSkillInfo->pSkillCond->nPlayerLvl, szColor, szBuff, szColor, pSkillInfo->pSkillCond->nCostSkillExp);
					}
				}//endif
				else
				{
					if (cm.GetConfigurableDisplayStyle(style_skill_tip_next_level,1)!=NULL)
						sprintf(szNextLevelNeeded,cm.GetConfigurableDisplayStyle(style_skill_tip_next_level,1));
				}//end else
				
                unsigned long dwNextLevelSizeNeeded=strlen(szNextLevelNeeded);
				
				if (dwTotalSize+dwNextLevelSizeNeeded<nStudyTipSize)	
				{
					strcpy(pDest,szNextLevelNeeded);
					dwTotalSize+=dwNextLevelSizeNeeded;
					pDest+=dwNextLevelSizeNeeded;
				}//endif
				else 
					break;

                //2. Next Skill Level Info.....................................................................................................................................................................................
				
				if (pSkillInfo->pSkillCond && pSkillInfo->szDescNext[0]!=0 && (!g_SkillManager.IsValueSkill(nSkillId) || (g_SkillManager.IsValueSkill(nSkillId) && !bDesc)))
				{
					//1.5 Line Again
					int nLineStyleAgain=0;
					if (bDesc && bNextDesc)
						nLineStyleAgain=1;
				
					const char *  szLine2Temp = cm.GetConfigurableDisplayStyle(style_skill_tip_line, nLineStyleAgain);
					if (szLine2Temp!=NULL)
					{
						dwSizeNeeded=strlen(szLine2Temp);
						if (dwTotalSize+dwSizeNeeded<nStudyTipSize)	
						{
							strcat(pDest,szLine2Temp);
							dwTotalSize+=dwSizeNeeded;
							pDest+=dwSizeNeeded;
						}//endif
						else
							break;
						
					}//endif
					//ignore <LayOut.......> 
					char * pSkillInfoScr=pSkillInfo->szDescNext;
					while ((*pSkillInfoScr)!= '>')
					{
						pSkillInfoScr++;
					}//endelse
					
					pSkillInfoScr++;
					
                    unsigned long dwSizeNeeded=strlen(pSkillInfoScr);
					if (dwTotalSize+dwSizeNeeded<nStudyTipSize)	
					{
						strcat(pDest,pSkillInfoScr);
						dwTotalSize+=dwSizeNeeded;
						pDest+=dwSizeNeeded;
					}//endif
					else
						break;
					
				}//endif
				else
				{
                    unsigned long dwSizeNeeded=strlen("</Layout>");
					if (dwTotalSize+dwSizeNeeded<nStudyTipSize)	
					{
						strcat(pDest,"</Layout>");
						dwTotalSize+=dwSizeNeeded;
						pDest+=dwSizeNeeded;
					}//endif
					else
						break;	
				}//endelse
				
			}//endif
			
		}
		break;
	case GDI_GET_SKILL_LEVEL:
		{
			int nSkillIdx = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].m_SkillList.FindSkill(uParam);
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetLevelByIdx(nSkillIdx);
		}
		break;
	case GDI_GET_CUR_SKILL_ID:
		{
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(uParam);
		}
		break;
	case GDI_GET_CUR_SKILL_POINT:
		{
			nRet = Player[CLIENT_PLAYER_INDEX].GetSkillExp();
		}
		break;
		
	/************************************************************************/
	/*                            mail system                               */
	/************************************************************************/
	case GDI_GET_NEW_MAIL_COUNT:
		{
			nRet = g_ChatCenterC.GetNewMailCount();
		}
		break;
	case GDI_GET_SEND_MAIL_TAX:
		{ 
			int* textTax = (int*)uParam;
			int* itemTax = (int*)nParam;
			*textTax = g_ChatCenterC.GetSendTextMailCost();
			*itemTax = g_ChatCenterC.GetSendItemMailCost();
		}
		break;

	case GDI_GET_MAX_MAILCOUNT:
		{
			int nMaxMailPerPlayer = ConfigManager::Singleton().GetGlobalVariable(global_var_max_mails_per_player);
			if  (nMaxMailPerPlayer == 0)
			{
				nMaxMailPerPlayer = MAIL_RECEIVERMAXMAIL_COUNT;
			}
			nRet = nMaxMailPerPlayer;
		}
		break;
	/************************************************************************/
	/*                            auction system                            */
	/************************************************************************/
	case GDI_GET_AUCTION_BASE_INFO:
		{
			Player[CLIENT_PLAYER_INDEX].m_clientAucMgr.GetAuctionBaseInfo((AuctionInfo*)uParam);
		}
		break;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//获取周围玩家的列表
	//uParam = (KUiPlayerItem*)pList -> 人员信息列表
	//			KUiPlayerItem::nData = 0
	//nParam = pList数组中包含KUiPlayerItem结构的数目
	//Return = 如果返回值小于等于传入参数nParam，其值表示pList数组中的前多少个KUiPlayerItem
	//			结构被填充了有效的数据；否则表示需要传入包含多少个KUiPlayerItem结构的数组
	//			才够存储全部人员信息。
	case GDI_NEARBY_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayer((KUiPlayerItem*)uParam, nParam);
		break;

	//获取周围孤单可受邀请的玩家的列表
	//参数含义同GDI_NEARBY_PLAYER_LIST
	case GDI_NEARBY_IDLE_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam, nParam);
		break;

	//获得物品在某个环境位置的属性状态
	//uParam = (KGameObject*)pObj（当nParam==0时）物品的信息
	//uParam = (KObjAtContRegion*)pObj（当nParam!=0时）物品的信息
	//nParam = (int)(bool)bJustTry  是否只是尝试放置
	//Return = (ITEM_IN_ENVIRO_PROP)eProp 物品的属性状态
	case GDI_ITEM_IN_ENVIRO_PROP://等待删除（谢鉷 2006年11月7日）
		{
			if (!nParam)
			{
				KGameObject *pObj = (KGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM)
					break;

				KItem* pItem = NULL;

				if (pObj->uGenre == CGOG_ITEM && pObj->uId > 0 && pObj->uId < MAX_ITEM)
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObj->uId);
					pItem = BuySell.GetItem(nIdx);
				}

				_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				//if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				//else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KObjAtContRegion *pObj = (KObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM || pObj->Obj.uId >= MAX_ITEM)
					break;

// lixuewu 2004.03.10 todo
//				int PartConvert[itempart_num] = 
//				{ 
//					itempart_head,		itempart_weapon,
//					itempart_amulet,	itempart_cuff,
//					itempart_body,		itempart_belt,
//					itempart_ring1,		itempart_ring2,
//					itempart_pendant,	itempart_foot,
//					itempart_horse,
//				};
// lixuewu 2004.03.10 todo

				_ASSERT(pObj->eContainer < itempart_num);
				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

// lixuewu 2004.03.10 todo
				//int nPlace = PartConvert[pObj->eContainer];
				int nPlace = pObj->eContainer;
// lixuewu 2004.03.10 todo

				//if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				//else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
		break;
	case GDI_IMMEDIATEITEM_NUM:
			//Lucifer~yu[zhangjianyu] [03/23/2006] Add for 
			//begin------------------------------------------------------------------------
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameParticularItemNum(uParam);
			//end--------------------------------------------------------------------------	
			
		break;
	//查询商店界面应该显示的物品个数以及每个物品对应的后台索引
	//不填uParam表示只查询个数
	case GDI_TRADE_NPC_ITEM:
		{
			//通过当前的指定商店ID判断当前是否处于买卖状态
			int	shopIndex = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			if (shopIndex < 0 || shopIndex >= BuySell.GetHeight())
				break;
			
			//遍历该商店所有物品，并把有效的物品记录在界面传入的参数中
			KObjAtContRegion* pInfo = (KObjAtContRegion *)uParam;
			int count = 0;
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				KItem* pItem = BuySell.getItem(shopIndex, i);
				
				if(NULL == pItem)
					continue;
				
				count++;
				if(NULL == pInfo)
					continue;
				
				pInfo[i].Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo[i].Obj.uId = i;
				pInfo[i].nContainer = UOC_NPC_SHOP;
				pInfo[i].Region.h = -1;
				pInfo[i].Region.v = -1;
			}
			nRet = count;
		}
		break;
	case GDI_ITEM_IN_STORE_BOX:
		{
			nRet = 0;
			
			KObjAtContRegion* objBuff = (KObjAtContRegion*)uParam;
			int	buffLen = nParam;
			if(!objBuff)
			{
				break;
			}
			
			if(!buffLen)
			{
				break;
			}
			
			//金钱
			int usedBuffCount = 0;
			objBuff->Obj.uGenre = CGOG_MONEY;
			objBuff->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			++usedBuffCount;
			++objBuff;
			
			//物品
			KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();

			for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
			{
				PlayerItem& item = itemList.m_Items[i];
				if(item.nPlace == pos_repositoryroom)
				{
					objBuff->Obj.uGenre = CGOG_ITEM;
					objBuff->Obj.uId = item.nIdx;		
					objBuff->Region.h = item.nX;
					objBuff->Region.v = item.nY;
					objBuff->Region.Height = Item[item.nIdx].GetItemCount();//Height用来记录叠加个数
					++usedBuffCount;
					++objBuff;
				}
			}

			nRet = usedBuffCount;
		}
		break;
	case GDI_PK_SETTING:					//获取pk设置
		// nRet = Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState();
		{
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			nRet = Npc[nNpcIdx].m_UnaryAttrMgr[nuai_pkmode];
		}

		break;
	case GDI_SHOW_PLAYERS_NAME:			//获取显示各玩家人名
		nRet = NpcSet.CheckShowName();
		break;
	//-------> Ray [Luoliang] 2004-9-2
	case GDI_SHOW_ITEMS_NAME:
		nRet = ObjSet.CheckShowName();
		break;
	//<------- End [Ray]
	case GDI_SHOW_PLAYERS_LIFE:			//获取显示各玩家生命
		nRet = NpcSet.CheckShowLife();
		break;
	case GDI_SHOW_PLAYERS_MANA:			//获取显示各玩家内力
		nRet = NpcSet.CheckShowMana();
		break;
		
	case GDI_NPC_OVERVIEW:
		if (uParam > 0 && uParam < MAX_NPC && nParam)
		{
			KUiPlayerRuntimeInfo *pReturn = (KUiPlayerRuntimeInfo *)nParam;
			KNpc&	aNpc = Npc[uParam];
			memset(pReturn, 0, sizeof(KUiPlayerRuntimeInfo));
			pReturn->nLife		= aNpc.m_UnaryAttrMgr[nuai_curlife];
			pReturn->nLifeFull	= aNpc.m_CompAttrMgr[ncai_lifeuplimit];
			pReturn->nMana		= aNpc.m_UnaryAttrMgr[nuai_curmana];
			pReturn->nManaFull	= aNpc.m_CompAttrMgr[ncai_manauplimit];
			
			pReturn->nCurrentCamp = aNpc.m_CurrentCamp;
		}
		break;
	// Add by Cooler 2004-7-21
	// Begin -->
	case GDI_ROLE_TYPE:
		*(int *)nParam = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Series;
		break;
	// End <--

	//Add By [Ray]  2004-11-25
	case GDI_NPC_CAREER:
		//uParam
		//nParam = nPlayerIndex
		//nRet = series
		if ( nParam >= 0 && nParam < MAX_NPC )
		{
			nRet = Npc[nParam].m_Series;
		}
		else
		{
			nRet = -1;
		}
		break;
	
	//-->Rocker 2004/11/04 
	case GDI_GET_PLAYER_CAMP:
		nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp;
		break;
	//<--Rocker 
	case GDI_ITEM_HAS_SPECIAL_IMAGE:
	//uParam = uID
	//nRet = (BOOL)
//		nRet = Item[uParam].HasSpecialImage();
		break;
	case GDI_GET_COMPOUND_INFO:
		{
 			KUiCompoundRuleInfo* compountInfo = (KUiCompoundRuleInfo*)nParam;
 			KUiCompoundParam* compoundParam = (KUiCompoundParam*)uParam;
 
 			if(NULL == compountInfo)
 			{
 				nRet = 0;
 				break;
 			}
 
 			if(NULL == compoundParam)
 			{
 				nRet = 0;
 				break;
 			}
 
			CompoundInitMaterial maiterialInfo;
 			
 			int index = 0;
			if(COMPOUND_LEVELUP == compoundParam->nCompoundType
				|| COMPOUND_ADDYAO == compoundParam->nCompoundType
				|| COMPOUND_GETYAO == compoundParam->nCompoundType
				|| COMPOUND_ADDMAGIC == compoundParam->nCompoundType)
			{
 				maiterialInfo.targetItemIndex = compoundParam->nItemIndex[index++];
 			}
			else
			{
				maiterialInfo.targetItemIndex = COMMON_ITEM_INVALID_ID;
			}
 			for(; index < MAX_LEVELUP_ITEMS_COUNT; ++index)
 			{
 				int itemIndex = compoundParam->nItemIndex[index];
 				if(itemIndex <= 0)
 				{
 					continue;
 				}
				maiterialInfo.sourItemIndex.push_back(compoundParam->nItemIndex[index]);
 			}
			
 			maiterialInfo.compoundType	= compoundParam->nCompoundType;
 
 			KCompoundRules::KCompoundRuleEntry tagRule;
 			nRet = g_CompoundRule.GetCompoundRuleInfo(maiterialInfo, tagRule);
			
			if(nRet == enchaser_error_no)
			{
				compountInfo->money = tagRule.nMoney;
				compountInfo->yunhun = tagRule.nSkillPoint;
				
				compountInfo->successRate = tagRule.CompoundSuccessRate0 + tagRule.CompoundSuccessRate1;
				compountInfo->destoryRate = tagRule.nDestroyRate;
				compountInfo->levelDownRate = tagRule.nPlusRate0;

				compountInfo->generateItem.genre = tagRule.DstItem0.nItemGenre;
				compountInfo->generateItem.detail = tagRule.DstItem0.nItemDetail;
				compountInfo->generateItem.particular = tagRule.DstItem0.nItemParticular;
				compountInfo->generateItem.level = tagRule.DstItem0.nItemLevel;
				compountInfo->ruleId = tagRule.nRuleTypePlus;
			}
		}
		break;
	
	case GDI_GET_SMITH_RULE_LIST:
		{
			int shopId = nParam;
			const KSmithShop::SmithShop* shopInfo = KSmithShop::getSinglton().getShopByIds(shopId);
			if(NULL == shopInfo)
			{
				break;
			}

			vector<CommonTreeItem>* ruleList = (vector<CommonTreeItem>*)uParam;
			char curtype1[COMMON_CLIENT_MSG_LEN_32];
			char curtype2[COMMON_CLIENT_MSG_LEN_32];
			char curtype3[COMMON_CLIENT_MSG_LEN_32];
			curtype1[0] = 0;
			curtype2[0] = 0;
			curtype3[0] = 0;
			for(int i = 0; i < shopInfo->rules.size(); ++i)
			{
				if(strcmp(curtype1, shopInfo->rules[i].type1))
				{
					CommonTreeItem ruleType;
					ruleType.childCount = 1;
					ruleType.id = COMMON_ITEM_INVALID_ID;
					strcpy(ruleType.name, shopInfo->rules[i].type1);
					ruleList->push_back(ruleType);
					
					strcpy(curtype1, shopInfo->rules[i].type1);
					curtype2[0] = 0;
					curtype3[0] = 0;
				}

				if(strcmp(curtype2, shopInfo->rules[i].type2))
				{
					CommonTreeItem ruleType;
					ruleType.childCount = 2;
					ruleType.id = COMMON_ITEM_INVALID_ID;
					strcpy(ruleType.name, shopInfo->rules[i].type2);
					ruleList->push_back(ruleType);
					
					strcpy(curtype2, shopInfo->rules[i].type2);
					curtype3[0] = 0;
				}

				if(strcmp(curtype3, shopInfo->rules[i].type3))
				{
					CommonTreeItem ruleType;
					ruleType.childCount = 3;
					ruleType.id = COMMON_ITEM_INVALID_ID;
					strcpy(ruleType.name, shopInfo->rules[i].type3);
					ruleList->push_back(ruleType);
					
					strcpy(curtype3, shopInfo->rules[i].type3);
				}
				
				CommonTreeItem ruleType;
				ruleType.childCount = 4;
				ruleType.id = shopInfo->rules[i].ruleId;
				strcpy(ruleType.name, shopInfo->rules[i].type4);
				ruleList->push_back(ruleType);
			}
		}
		break;
	case GDI_CAN_SMITH:
		{
			
		}
		break;
	case GDI_GET_SMITH_RULE_BY_ID:
		{
			int ruleId = nParam;
			KCompoundRules::KCompoundRuleEntry* ruleEntry = g_CompoundRule.getRuleById(COMPOUND_SMITH, 1, ruleId);
			
			if(NULL == ruleEntry)
			{
				nRet = 0;
				break;
			}

			SmithRule* rule = (SmithRule*)uParam;
			//打造物品
			rule->smithItem.type.genre		= ruleEntry->DstItem0.nItemGenre;
			rule->smithItem.type.particular = ruleEntry->DstItem0.nItemParticular;
			rule->smithItem.type.detail		= ruleEntry->DstItem0.nItemDetail;
			rule->smithItem.type.level		= ruleEntry->DstItem0.nItemLevel;
			rule->smithItem.itemCount		= ruleEntry->DstItem0.nItemCount;

			rule->reqItemTypeCount = ruleEntry->nSrcItemsCount;
			
			//需求物品
			rule->reqItem = new SmithItem[rule->reqItemTypeCount];
			for(int i = 0 ; i < rule->reqItemTypeCount; ++i)
			{
				rule->reqItem[i].type.genre			= ruleEntry->SrcItemsArray[i].nItemGenre;
				rule->reqItem[i].type.particular	= ruleEntry->SrcItemsArray[i].nItemParticular;
				rule->reqItem[i].type.detail		= ruleEntry->SrcItemsArray[i].nItemDetail;
				rule->reqItem[i].type.level			= ruleEntry->SrcItemsArray[i].nItemLevel;
				rule->reqItem[i].itemCount			= ruleEntry->SrcItemsArray[i].nItemCount;
			}
			
			//需求金钱、蕴魂
			rule->reqMoney = ruleEntry->nMoney;
			rule->reqYunHun = ruleEntry->nSkillPoint;
			rule->rate.push_back(ruleEntry->CompoundSuccessRate0);
			rule->rate.push_back(ruleEntry->CompoundSuccessRate1);
			rule->rate.push_back(ruleEntry->CompoundSuccessRate2);

			nRet = 1;
		}
		break;	
	case GDI_GET_SMITH_RULE_BY_LIFT_ITEM:
		{
			ItemType* liftItem = (ItemType*)nParam;
			if(!liftItem)
			{
				uParam = NULL;
				nRet = 0;
				break;
			}
			KCompoundRules::KCompoundRuleEntry* ruleEntry = g_CompoundRule.getRuleByLiftItem(COMPOUND_SMITH, 1, *liftItem);
			
			if(NULL == ruleEntry)
			{
				uParam = NULL;
				nRet = 0;
				break;
			}

			SmithRule* rule = (SmithRule*)uParam;
			//打造物品
			rule->smithItem.type.genre		= ruleEntry->DstItem0.nItemGenre;
			rule->smithItem.type.particular = ruleEntry->DstItem0.nItemParticular;
			rule->smithItem.type.detail		= ruleEntry->DstItem0.nItemDetail;
			rule->smithItem.type.level		= ruleEntry->DstItem0.nItemLevel;
			rule->smithItem.itemCount		= ruleEntry->DstItem0.nItemCount;

			rule->reqItemTypeCount = ruleEntry->nSrcItemsCount;
			
			//需求物品
			rule->reqItem = new SmithItem[rule->reqItemTypeCount];
			for(int i = 0 ; i < rule->reqItemTypeCount; ++i)
			{
				rule->reqItem[i].type.genre			= ruleEntry->SrcItemsArray[i].nItemGenre;
				rule->reqItem[i].type.particular	= ruleEntry->SrcItemsArray[i].nItemParticular;
				rule->reqItem[i].type.detail		= ruleEntry->SrcItemsArray[i].nItemDetail;
				rule->reqItem[i].type.level			= ruleEntry->SrcItemsArray[i].nItemLevel;
				rule->reqItem[i].itemCount			= ruleEntry->SrcItemsArray[i].nItemCount;
			}
			
			//需求金钱、蕴魂
			rule->reqMoney = ruleEntry->nMoney;
			rule->reqYunHun = ruleEntry->nSkillPoint;
			rule->rate.push_back(ruleEntry->CompoundSuccessRate0);
			rule->rate.push_back(ruleEntry->CompoundSuccessRate1);
			rule->rate.push_back(ruleEntry->CompoundSuccessRate2);

			nRet = ruleEntry->nRuleTypePlus;
		}
		break;	
	//--> Rocker 2005/06/16
	case GDI_IS_OWN_PET:			//是否是自己的宠物
	//uParam 
	//nParam = nNpcIndex
	//nRet = BOOL
		{
			KNpc &self = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			if ( self.m_nPetIndex != 0 && nParam == self.m_nPetIndex )
			{
				nRet = TRUE;
			}			
		}
		break;
	case GDI_IS_EMPLOEE:			//是否佣兵
		{
			int checkEmployee = uParam;
			int npcIndex = nParam;
			if(IsValidNpc(npcIndex))
			{
				if(checkEmployee)
				{
					nRet = Npc[npcIndex].IsEmployee();
				}
				else
				{
					nRet = Npc[npcIndex].IsCreature();
				}
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	//<------- End [Ray]

	case GDI_GET_TECHNOLOGY_INFO:		//得到指定的科技的信息
		break;

// endadd
        
// add by hejianfeng for Anti-Wallow.  2005-11-21
    case GDI_ITEM_COUNT:
	//uParam = 待查询Item的参数
	//Return = 物品的索引
        {
            FIND_ITEMINDEX_PARAM* pInfo = (PFIND_ITEMINDEX_PARAM)uParam;
			if ( pInfo == NULL )
			{
				return 0;
			}
            //FindItemIndex(pInfo->nGenre, pInfo->nDetail, pInfo->nParticular, &nRet);
			nRet = 0;
			int nItemPartCount = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSpecificItemIndexArraySize( room_equipment, 
																										pInfo->nGenre, 
																										pInfo->nDetail, 
																										pInfo->nParticular );
			if ( nItemPartCount <= 0 || nItemPartCount > 35 )
			{
				return 0;
			}
			int *pItemArray = new int[nItemPartCount];
			if ( pItemArray )
			{
				Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSpecificItemIndexArray( room_equipment, 
																					pInfo->nGenre, 
																					pInfo->nDetail, 
																					pInfo->nParticular,
																					pItemArray,
																					nItemPartCount );
				if ( pItemArray )
				{
					for ( int nIdx = 0; nIdx < nItemPartCount; ++nIdx )
					{
						nRet += GetGameData( GDI_ITEM_COUNT_QUERY, pItemArray[nIdx], NULL ); 
					}
				}
				else
				{
					nRet = 0;
				}
			}
			else
			{
				nRet = 0;
			}
			delete[] pItemArray;
		}
        break;
// endadd

	//Lucien[liusiliang] [04/07/2007] 得到第一个物品的INDEX
	//begin------------------------------------------------------------------------
	//uParam = 待查询Item的参数
	//Return = 物品的ID
	case GDI_FIRST_ITEM_ID:
		{
            FIND_ITEMINDEX_PARAM* pInfo = (PFIND_ITEMINDEX_PARAM)uParam;
			if ( pInfo == NULL )
			{
				return 0;
			}
            //FindItemIndex(pInfo->nGenre, pInfo->nDetail, pInfo->nParticular, &nRet);
			nRet = 0;
			int nItemPartCount = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSpecificItemIndexArraySize( room_equipment, 
																										pInfo->nGenre, 
																										pInfo->nDetail, 
																										pInfo->nParticular );
			if ( nItemPartCount <= 0 || nItemPartCount > 35 )
			{
				return 0;
			}
			int *pItemArray = new int[nItemPartCount];
			if ( pItemArray )
			{
				Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSpecificItemIndexArray( room_equipment, 
																					pInfo->nGenre, 
																					pInfo->nDetail, 
																					pInfo->nParticular,
																					pItemArray,
																					nItemPartCount );
				if ( pItemArray )
				{
					nRet = pItemArray[0];
				}
				else
				{
					nRet = 0;
				}
			}
			else
			{
				nRet = 0;
			}
			delete[] pItemArray;
		}
        break;
	//end--------------------------------------------------------------------------	

	//Lucifer~yu[zhangjianyu] [02/22/2006] Add for 
	//begin------------------------------------------------------------------------
	case GDI_GET_PILLPARAM:
		{
			KPillParam *pPillParam = (KPillParam*)uParam;
			if ( pPillParam )
			{
//				pPillParam->nPillExp    = Player[CLIENT_PLAYER_INDEX].m_PillMaking.GetCurrentDoubleExp();
//				pPillParam->nPillMaxExp = Player[CLIENT_PLAYER_INDEX].m_PillMaking.GetDoubleExpUpLimit();
				nRet = 1;
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	//end--------------------------------------------------------------------------	
	//Lucifer~yu[zhangjianyu] [03/03/2006] Add for 取得离线经验单药加成时间 
	//begin------------------------------------------------------------------------
	case GDI_GET_PILLMAKING_LEFTTIME:
	//nRet = (int)nLeftTime
		{
//			nRet = Player[CLIENT_PLAYER_INDEX].m_PillMaking.GetMoreUpLimitLeftTime();
		}
		break;
	//end--------------------------------------------------------------------------	
	//Lucifer~yu[zhangjianyu] [03/23/2006] Add for 
	//begin------------------------------------------------------------------------
	case GDI_GET_ITEM_PARTICULAR:
		{
            FIND_ITEMINDEX_PARAM* pInfo = (PFIND_ITEMINDEX_PARAM)uParam;
			if ( pInfo )
			{
				if ( nParam > 0 && nParam < MAX_ITEM )
				{
					pInfo->nGenre		= Item[nParam].GetGenre();
					pInfo->nDetail		= Item[nParam].GetDetailType();
					pInfo->nParticular	= Item[nParam].GetParticular();
					pInfo->nLevel		= Item[nParam].GetLevel();
					pInfo->nGroup		= Item[nParam].GetGroup();
					pInfo->bIBNoTarget	= Item[nParam].IsNoTarget();
				}
			}
		}
		break;
	/************************************************************************/
	/*							Buffer system                               */
	/************************************************************************/
	/*!
	\brief
		Get buffer Information. 		

 	\param 
		Buffer Information(KBufferInfo*). 

	\param 
		Buffer index(int).
	
	\return
		
	*/
	case GDI_GET_BUFFER_INFO:
		{
			BuffTable& BT = BuffTable::Singleton( );
			
			KBufferInfo* pBI = (KBufferInfo*)uParam;
			PBAT pBAT = BT.GetBuff( nParam );
			if( pBAT )
			{

				memcpy( pBI->szImage, pBAT->szImage, COMMON_CLIENT_MSG_LEN_16 );
				pBI->szImage[COMMON_CLIENT_MSG_LEN_16-1] = 0;

				strcpy(
					pBI->nDesc,
					pBAT->szDesc );
				
				pBI->nBuff = pBAT->nPlus;
			}
			else
				memset( pBI, 0, sizeof(KBufferInfo) );
		}
		break;

	/*!
	\brief
		Get buffer Information. 		

 	\param 
		Buffer Information(KBufferSyncInfo*). 

	\param 
		Buffer max count(int).
	
	\return
		Buffer count(int).
		
	*/
	case GDI_GET_TARGET_BUFFER_INFO:
		{
			if( uParam == NULL ||
				nParam == 0 )
				return 0;

			KBufferSyncInfo* pBuff = (KBufferSyncInfo*)uParam;

			int nNpcIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();

			if( nNpcIdx <= 0 || nNpcIdx > MAX_NPC )
			{
				nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			}
			KNpc::C_BUFFLIST& buffList = Npc[nNpcIdx].GetBuffList_C();
			KNpc::C_BUFFLIST::iterator it;
			int nCount = 0;
			for ( it = buffList.begin(); it != buffList.end(); it++ )
			{
				if ( nCount < nParam )
				{
					pBuff[nCount].nBuffID		= (*it).first;
					pBuff[nCount].nTempBuffID = (*it).second;
					pBuff[nCount].nPileCount	= 0;
					pBuff[nCount].nTime		= 0;
					nCount++;
				}
			}


			nRet = nCount;
		}
		break;

	/************************************************************************/
	/*							Quest system                                */
	/************************************************************************/
	/*!
	\brief
		Get quest list.

	\param 
		Quest id (int).	

	\param 
		Quest information (KQuestInfo*).

	\return
		Nothing.
	*/
	case GDI_GET_QUEST_LIST:
		{
			QuestLog::GetInstance()->GetQuestList((KSimpleQuestInfo*)uParam, nParam);
		}
		break;

	/*!
	\brief
		Request a quest operation.

	\param 
		Quest id (int).

	\return
		Nothing.
	*/
	case GDI_GET_QUEST_INFO:
		{
 			KQuestInfo* questInfo = (KQuestInfo*)uParam;
			memset(questInfo, 0, sizeof(KQuestInfo));
			questInfo->id = nParam;
 			QuestLog::GetInstance()->GetUIInfo(*questInfo);
		}
		break;
	case GDI_GET_NPC_POS_BY_TABLE_INDEX:
		{
			int npcIndex = nParam;
			NpcMapPos& npcInfo = MapNpcMgr::getSingleton().findNpc(npcIndex);

			NpcMapPos* retNpcPos = (NpcMapPos*)uParam;
			if(retNpcPos)
			{
				retNpcPos->mapId = npcInfo.mapId;
				retNpcPos->x = npcInfo.x;
				retNpcPos->y = npcInfo.y;
			}
			nRet = 0;
		}
		break;
	//end--------------------------------------------------------------------------	
	case GDI_GET_SOCIETY:
		{
			SOCIETY_INFO* pInfo = (SOCIETY_INFO*)uParam;
			if ( pInfo )
			{
				KPlayer& player = GetClientPlayer();
				const PClientRelationInfo pRelationInfo = player.GetClientSocialRelation().GetRelationInfo(enSUTplId_Tong);
				if (pRelationInfo != NULL)
				{
					pInfo->szZhuhou = pRelationInfo->Names[enSULayer_Tong];
					pInfo->szShizu = pRelationInfo->Names[enSULayer_Gens];
					pInfo->szLeague = pRelationInfo->Names[enSULayer_League];
				}
			}
		}
		break;
	/*!
	\brief
		Get player society information.
		
	\param 
		(PlayerSociety*)uParam

	\param 
		(int*)nParam
  
	\return
		
	*/
	case GDI_GET_SOCIETY_PLAYER:
		{
			int topLayer = 0;
			SocietyInfoIndex* pInfo = (SocietyInfoIndex*)uParam;
			const PClientRelationInfo pRelation = Player[CLIENT_PLAYER_INDEX].GetClientSocialRelation().GetRelationInfo(pInfo->TemplateId);
			if (pRelation != NULL)
			{
				topLayer = pRelation->TopLayer;
				*((int*)nParam) = topLayer;
				nRet = 1;
			}
			else
			{
				*((int*)nParam) = 0;
				nRet = 0;
			}
		}
		break;
	
	/*!
	\brief
		Judge player society right.	
	
	\param 
		(SocietyInfoIndex*)uParam
  
	\return
		
	*/
	case GDI_GET_SOCIETY_RIGHT:
		{
			SocietyInfoIndex* pInfo = (SocietyInfoIndex*)uParam;
			bool hasPrivilege = Player[CLIENT_PLAYER_INDEX].GetClientSocialRelation().CheckPrivilege(pInfo->TemplateId, pInfo->Layer, pInfo->Operation);
			nRet = hasPrivilege ? TRUE : FALSE;
		}
		break;

	/*!
	\brief
		Get society template information.
	
	\param 
		(SocietyInfoIndex*)uParam		
  
	\param 
		(SocietyTemplateInfo*)nParam
  
	\return
		
	*/
	case GDI_GET_SOCIETY_TEMPLATE_INFO:
		{
			SocietyInfoIndex* societyInfoIndex = (SocietyInfoIndex*)uParam;
			RelationTemplate* pRT = RTM::Singleton().GetTemplate(societyInfoIndex->TemplateId);
			if (pRT != NULL)
			{
				SocietyTemplateInfo* pSTI = (SocietyTemplateInfo*)nParam;
				pSTI->Id = societyInfoIndex->TemplateId;
				strcpy(pSTI->Name, pRT->GetName());
				strcpy(pSTI->Desc, pRT->GetDesc());
				pSTI->LayerCount = pRT->GetLayerCount();
				nRet = 1;
			}
		}
		break;

	/*!
	\brief
		Get society layer information.
	
	\param 
		(SocietyInfoIndex*)uParam		
  
	\param 
		(SocietyLayerInfo*)nParam	
  
	\return
		
	*/
	case GDI_GET_SOCIETY_LAYER_INFO:
		{
			SocietyInfoIndex* societyInfoIndex = (SocietyInfoIndex*)uParam;
			RelationTemplate* pRT = RTM::Singleton().GetTemplate(societyInfoIndex->TemplateId);
			if (pRT != NULL)
			{
				SocietyLayerInfo  * pSLI = (SocietyLayerInfo*)nParam;

				const PRelationLayer pRL = pRT->GetLayer(societyInfoIndex->Layer);
				
				pSLI->nLayerID           = societyInfoIndex->Layer;
				
				strcpy(pSLI->Name, pRL->Name);
				strcpy(pSLI->Desc, pRL->Desc);
				strcpy(pSLI->Controller, pRL->Controller);
				pSLI->OperationCount = pRL->OperationCount;
				for (int operationLoopCount = 0; operationLoopCount < pRL->OperationCount; operationLoopCount++)
				{
					SocietyLayerOperation* pSocietyOperation = &(pSLI->Operations[operationLoopCount]);
					PRelationOperation pRelationOperation = &(pRL->Operations[operationLoopCount]);
					
					pSocietyOperation->Id = pRelationOperation->Id;
					for (int controllerLoopCount = 0; controllerLoopCount < MAX_CONTROLLER_COUNT; controllerLoopCount++)
					{
						strcpy(pSocietyOperation->Controllers[controllerLoopCount].Controller, pRelationOperation->Controllers[controllerLoopCount].Controller);
						strcpy(pSocietyOperation->Controllers[controllerLoopCount].Event, pRelationOperation->Controllers[controllerLoopCount].Event);
						strcpy(pSocietyOperation->Controllers[controllerLoopCount].Name, pRelationOperation->Controllers[controllerLoopCount].Name);
						strcpy(pSocietyOperation->Controllers[controllerLoopCount].Desc, pRelationOperation->Controllers[controllerLoopCount].Desc);										
						strcpy(pSocietyOperation->Controllers[controllerLoopCount].ReturnController, pRelationOperation->Controllers[controllerLoopCount].ReturnController);
						pSocietyOperation->Controllers[controllerLoopCount].NeedConfirm = pRelationOperation->Controllers[controllerLoopCount].NeedConfirm;
					}						
				}
				nRet = 1;
			}
		}
		break;

	case GDI_CAN_JOIN_SOCIAL_LAYER:
		{
			int nSocialTargetLayer = uParam;
			switch (nSocialTargetLayer)
			{
			case enSULayer_Gens:
				{
					const char * pShizuName = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetShizuName();
					if (pShizuName[0] == 0)
					{
						return 1;
					}
					else
						return 0;
				}//end for case
				break;

			case enSULayer_Tong:
				{
					bool hasPrivilege = Player[CLIENT_PLAYER_INDEX].GetClientSocialRelation().CheckPrivilege(enSUTplId_Tong, nSocialTargetLayer-1, enSUO_ReqJoinHigherLevel);
					if (!hasPrivilege)
						return 0;
					
					const char * pZhuhouName = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetZhuhouName();
					if (pZhuhouName[0] == 0)
					{
						return 1;
					}
					else
						return 0;
				}//end for case
				break;
			case enSULayer_League:
				{
					bool hasPrivilege = Player[CLIENT_PLAYER_INDEX].GetClientSocialRelation().CheckPrivilege(enSUTplId_Tong, nSocialTargetLayer-1, enSUO_ReqJoinHigherLevel);
					if (!hasPrivilege)
						return 0;
					
					const char * pLeagueName = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetLeagueName();
					if (pLeagueName[0] == 0)
					{
						return 1;
					}
					else
						return 0;

				}
				break;
			}//end switch
		}
		break;
		
		/*****************************************************************
         *                 TaisuiWheelSystem                             *
		 ******************************************************************/
	//Add By Brianyao2007	
    #ifndef _SERVER
	case GDI_GET_CLIENT_STATE:
		{
          KPlayer & player=GetClientPlayer();
		  nRet=player.GetTaisuiWheelSys()->IsInited();
		}
		break;

	case GDI_GET_TIMES_INFO:
		{
          KPlayer & player=GetClientPlayer();
		  unsigned long dwAvailable=player.GetTaisuiWheelSys()->GetCurrentAvailableTimes();
		  unsigned long dwWheeldTimes=player.GetTaisuiWheelSys()->GetCurrentWheeledTimes();		  
		  nRet=(dwAvailable<<16)+dwWheeldTimes;
		}
		break;

	case GDI_GET_CUR_TIAN_XIANG:
		{
			KPlayer & player=GetClientPlayer();
		  unsigned long dwMonth=player.GetTaisuiWheelSys()->GetCurrentMonth();
		  unsigned long dwDay=player.GetTaisuiWheelSys()->GetCurrentDay();
			nRet= (dwMonth << 16) +dwDay;
		}
		break;

	case GDI_GET_ACTIVATING_TIAN_XIANG:
		{
          KPlayer & player=GetClientPlayer();
		  nRet=player.GetTaisuiWheelSys()->GetActivatingJiazi();
		}
		break;
	
	case GDI_GET_WHEEL_TIAN_GAN:
		{
          KPlayer & player=GetClientPlayer();
		  nRet= (int) player.GetTaisuiWheelSys()->GetCurrentWheeldTianGan();
		}
		break;
	 
	case GDI_GET_WHEEL_DI_ZHI:
		{
		  KPlayer & player=GetClientPlayer();
		  nRet= (int) player.GetTaisuiWheelSys()->GetCurrentWheeldDiZhi();
		}
		break;

	case GDI_CANLEARN_SKILL:
		{
			nRet=0;
			unsigned long nSkillId=uParam;
			KPlayer & player=GetClientPlayer();
			unsigned long nNpcIdx=player.GetNpcIndex();
			int		nSkillIdx	= Npc[player.GetNpcIndex()].m_SkillList.FindSkill(nSkillId);			
			
			// if skill level up, can't find index
			if (nSkillIdx == -1)
			{
				nSkillId = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(nSkillId);
				nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSkill(nSkillId);
			}
			
			if(-1 != nSkillIdx)
			{
				int		nLevel	= Npc[nNpcIdx].m_SkillList.GetLevelByIdx(nSkillIdx);
				nRet=g_SkillManager.CanUpdateTo(nNpcIdx,nSkillId,nLevel+1);
			}//endif
		}//end for case
		break;

	case GDI_STUDY_SKILL_COND:
		{
			bool          bCanUpdate=true;
            unsigned long nSkillId=uParam;
			char *        szBuff=(char *)nParam;
		
			int			nNpcIdx		= Player[CLIENT_PLAYER_INDEX].m_nIndex;
			int			nSkillIdx	= Npc[nNpcIdx].m_SkillList.FindSkill(nSkillId);	
			
			ConfigManager& cm = ConfigManager::Singleton();
            szBuff[0]=0;

			// if skill level up, can't find index
			if (nSkillIdx == -1)
			{
				nSkillId = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(nSkillId);
				nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSkill(nSkillId);
			}
			
			if(-1 != nSkillIdx)
			{
				int		nLevel	= Npc[nNpcIdx].m_SkillList.GetLevelByIdx(nSkillIdx);
				int		nStatus	= Npc[nNpcIdx].m_SkillList.GetStatusByIdx(nSkillIdx);

				KSkill	*pSkill = g_SkillManager.GetSkill(nSkillId, nLevel > 0 ? nLevel : 1);

				if (pSkill==NULL) break;

				const SkillChgCond * pSkillCond=0;
				if (g_SkillManager.IsSubSkill(nSkillId))
				{
					unsigned long nSeries=0;
					nSeries=g_SkillManager.GetSubSkillIdByLvl(nSkillId,1);
					pSkillCond = g_SkillManager.GetSkillCond(nSeries, nLevel + 1);
				}//endif
				else
					pSkillCond = NULL;

				if ( pSkillCond == NULL )
				{
					return 0;
				}

				char szMsgBuff[256];
                int  nStyle=0;
				//if (cm.GetConfigurableDisplayStyle(style_skill_cond_head,0)!=NULL)
				//	strcat(szBuff,cm.GetConfigurableDisplayStyle(style_skill_cond_head,0));
				//NameFirst
				
				if (cm.GetConfigurableDisplayStyle(style_skill_cond_name,0)!=NULL)
					sprintf(szMsgBuff,cm.GetConfigurableDisplayStyle(style_skill_cond_name,0),pSkill->m_szName,nLevel+1);
				
				strcat(szBuff,szMsgBuff);
				//SkillExp
				if (pSkillCond && pSkillCond->nCostSkillExp>0)
				{
					if(GetClientPlayer().GetSkillExp()>=pSkillCond->nCostSkillExp)
					{
                         nStyle=0;
					}//endif
					else 
					{
						nStyle=1;
						bCanUpdate=false;
					}//end else
					
					if (cm.GetConfigurableDisplayStyle(style_skill_cond_skill_exp,nStyle)!=NULL)
					{
						sprintf(szMsgBuff,cm.GetConfigurableDisplayStyle(style_skill_cond_skill_exp,nStyle),pSkillCond->nCostSkillExp);
						strcat(szBuff,szMsgBuff);
					}//endif				
				}//endif
				
				//Money
				if (pSkillCond && pSkillCond->nCostMoney>0)
				{
                   unsigned long dwCostJin=(pSkillCond->nCostMoney/10000);
				   unsigned long dwCostYin=(pSkillCond->nCostMoney/100)%100;
				   unsigned long dwCostTong=pSkillCond->nCostMoney % 100;

				   unsigned long nMoney = GetTotalMoney(Npc[nNpcIdx].GetPlayerIdx());
				   
				   unsigned long dwOwnJin=(nMoney/10000);
				   unsigned long dwOwnYin=(nMoney/100)%100;
				   unsigned long dwOwnTong=nMoney % 100;

				   char szJin[16]="";
				   char szYin[16]="";
				   char szTong[16]="";
				   
				   if (dwCostJin && cm.GetConfigurableDisplayStyle(style_skill_tip_money_nor_string,0)!=NULL)
					   sprintf(szJin,"%d%s",dwCostJin,cm.GetConfigurableDisplayStyle(style_skill_tip_money_nor_string,0));
				  
				   if (dwCostYin && cm.GetConfigurableDisplayStyle(style_skill_tip_money_nor_string,1)!=NULL)
					   sprintf(szYin,"%d%s",dwCostYin,cm.GetConfigurableDisplayStyle(style_skill_tip_money_nor_string,1));

				   if (dwCostTong && cm.GetConfigurableDisplayStyle(style_skill_tip_money_nor_string,2)!=NULL)
					   sprintf(szTong,"%d%s",dwCostTong,cm.GetConfigurableDisplayStyle(style_skill_tip_money_nor_string,2));

				   if (nMoney>=pSkillCond->nCostMoney)
					   nStyle=0;
				   else
				   {
					   nStyle=1;
					   bCanUpdate=false;
				   }

				   char szMoneyMsg[64];
				   szMoneyMsg[0]=0;

				   strcat(szMoneyMsg,szJin);
				   strcat(szMoneyMsg,szYin);
				   strcat(szMoneyMsg,szTong);

				   if (cm.GetConfigurableDisplayStyle(style_skill_cond_money,nStyle)!=NULL)
				   {
					   sprintf(szMsgBuff,cm.GetConfigurableDisplayStyle(style_skill_cond_money,nStyle),szMoneyMsg);
					   strcat(szBuff,szMsgBuff);
				   }//endif
				   
				}//endif

				//ITem
				char  szCostItemMsg[512]="";

				if ( pSkillCond->nCostItemKey[0] > -1 )
				{
					
					FIND_ITEMINDEX_PARAM	tagItemIdx;
					KItemInfo				tagItemInfo;

					tagItemIdx.nGenre		= pSkillCond->nCostItemKey[0];
					tagItemIdx.nDetail		= pSkillCond->nCostItemKey[1];
					tagItemIdx.nParticular	= pSkillCond->nCostItemKey[2];
					tagItemIdx.nLevel		= pSkillCond->nCostItemKey[3];
					
					GetGameData( GDI_ITEM_INFO_PARTICULAR, (unsigned int)&tagItemIdx, (int)&tagItemInfo );	
                    
					int	nItemIdx;
					if( GetClientPlayer().m_ItemList.FindSameParticularItem(pSkillCond->nOwnItemKey[0],
						pSkillCond->nOwnItemKey[1], pSkillCond->nOwnItemKey[2], &nItemIdx) )
					{
						nStyle=0;
					}
					else
					{
						nStyle=1;
						bCanUpdate=false;
					}
					
					if (cm.GetConfigurableDisplayStyle(style_skill_cond_item,nStyle)!=NULL)
					{
						sprintf(szMsgBuff,cm.GetConfigurableDisplayStyle(style_skill_cond_item,nStyle),tagItemInfo.szName);
						strcat(szBuff,szMsgBuff);
					}//endif
				}
				
				
				if (cm.GetConfigurableDisplayStyle(style_skill_cond_end,0)!=NULL)
					strcat(szBuff,cm.GetConfigurableDisplayStyle(style_skill_cond_end,0));
			}//endif
			else
				bCanUpdate=false;
			
			return bCanUpdate;
		}
		break;
		
        case GDI_GET_IS_STUDY_ABLE:
			{
				unsigned long nSkillId=uParam;
				nRet=!g_SkillManager.IsPassiveLevelupSkill(nSkillId);
			}
			break;
			
		case GDI_MAP_ID:
			{
                unsigned long dwNpcIndex=GetClientPlayer().GetNpcIndex();
			    return SubWorld[0].m_SubWorldID;
			}
			break;
		case GDI_MAP_CHANNEL_INFO:
			{
				MapChannelInfo * pChannelInfo = (MapChannelInfo *)uParam;
				if (pChannelInfo != NULL)
				{
					memcpy(pChannelInfo, &(SubWorld[0].m_MapChannelInfo), sizeof(MapChannelInfo));
				}
			}
			break;
		case GDI_MAP_CHANNEL_NAME:
			{
				char * nameBuffer = (char *)uParam;
				int bufferLen = nParam;
				if (nameBuffer != NULL && bufferLen != 0)
				{
					memset(nameBuffer, 0, bufferLen);
					size_t nameLen = sizeof(SubWorld[0].m_MapChannelInfo.szChannelName);
					if (bufferLen < nameLen)
					{
						strncpy(nameBuffer, SubWorld[0].m_MapChannelInfo.szChannelName, bufferLen);
						nameBuffer[bufferLen - 1] = 0;
					}
					else
					{
						strncpy(nameBuffer, SubWorld[0].m_MapChannelInfo.szChannelName, nameLen);
						nameBuffer[bufferLen - 1] = 0;
					}
				}	
			}
			break;
		case GDI_MAP_CHANNEL_TIME:
			{
				int * intervalTime = (int *)uParam;
				if (intervalTime != NULL)
				{
					*intervalTime = SubWorld[0].GetMapChatInterval();
				}
			}
			break;
		case GDI_CAN_GOTO_POS:
			{	
#ifdef	_AUTO_ROBOT
				nRet = AutoRobotMgr::Singleton().canRunTo(uParam, nParam);
#endif
				break;
			};
		case GDI_NPC_LIST_OF_MAP:
			{
				vector<NpcMapInfo>* npcInfoList = (vector<NpcMapInfo>*)uParam;
				char* mapName = (char*)nParam;
				if(npcInfoList && mapName)
				{
					vector<MapNpcMgr::NpcInfo>& npcList = MapNpcMgr::getSingleton().loadMap(mapName);
 					for(int i = 0; i < npcList.size(); ++i)
 					{
 						NpcMapInfo npcInfo;
 						npcInfo.idAndName.id = npcList[i].npcId;
 						int sourLen = sizeof(npcInfo.idAndName.name);
  						if(sourLen > strlen(npcList[i].npcName.c_str()))
  						{
  							strcpy(npcInfo.idAndName.name, npcList[i].npcName.c_str());
  						}
  						else
  						{
  							strncpy(npcInfo.idAndName.name, npcList[i].npcName.c_str(), sourLen);
  							npcInfo.idAndName.name[sourLen - 1] = 0;
  						}

						npcInfo.x = npcList[i].x;
						npcInfo.y = npcList[i].y;
 						npcInfoList->push_back(npcInfo);
 					}
				}
				break;
			}
		case GDI_GET_NPC_ID_BY_NAME:
			{
				KUiPlayerItem* player = (KUiPlayerItem*)uParam;
				if(player)
				{
					int index = NpcSet.SearchName(player->Name);
					if(index<=0)
					{
						player->uId = 0;
					}
					else
						player->uId = Npc[index].m_dwID;
				}
				
			}
			return -1;
		case GDI_GET_EMPLOY_TIME:
			{
				nRet = Player[CLIENT_PLAYER_INDEX].GetEmployTime();
			}
			break;
		case GDI_GET_NEARBY_PLAYER:
			{
				vector<NpcMapInfo>* npcInfoList = (vector<NpcMapInfo>*)uParam;
				if(!npcInfoList)
				{
					break;
				}

				char* selfName = Player[CLIENT_PLAYER_INDEX].GetPlayerName();
				//遍历NPC
				int npcIndex = 0;
				while (npcIndex = NpcSet.GetNextIdx(npcIndex))
				{
					if(!IsValidNpc(npcIndex))
					{
						break;
					}
					KNpc& npc = Npc[npcIndex];
					if(npc.m_Kind != kind_player)
					{
						continue;
					}
					
					if(Player[CLIENT_PLAYER_INDEX].m_nIndex != npcIndex)
					{
						NpcMapInfo npcInfo;
						strncpy(npcInfo.idAndName.name, npc.Name, sizeof(npcInfo.idAndName.name));
						npcInfo.idAndName.name[sizeof(npcInfo.idAndName.name) - 1] = 0;
						
						int x, y;
						Npc[npcIndex].GetMpsPos(&x, &y);
						npcInfo.x = x * 0.03125;
						npcInfo.y = y * 0.015625;

						npcInfo.idAndName.id = Npc[npcIndex].GetId();

						npcInfoList->push_back(npcInfo);
					}
				}
			}
			break;
		case GDI_GET_PLAYER_POS:
			{
				pair<int, int>* playerPos = (pair<int, int>*)uParam;
				char* playerName = (char*)nParam;
				if(!playerPos || !playerName)
				{
					break;
				}
				playerPos->first = 0;
				playerPos->second = 0;

				//遍历NPC
				int npcIndex = 0;
				while (npcIndex = NpcSet.GetNextIdx(npcIndex))
				{
					if(!IsValidNpc(npcIndex))
					{
						break;
					}
					KNpc& npc = Npc[npcIndex];
					if(npc.m_Kind != kind_player)
					{
						continue;
					}
					
					if(strcmp(npc.Name, playerName))
					{
						continue;
					}
					
					int x, y;
					Npc[npcIndex].GetMpsPos(&x, &y);
					playerPos->first = x * 0.03125;
					playerPos->second = y * 0.015625;
					
					break;
				}
			}
			break;
		case  GDI_GET_NPC_KIND:
			{
				int nKind     = kind_num;
				int nNpcIndex = uParam;
				if (IsValidNpc(nNpcIndex))
					nKind     = Npc[nNpcIndex].m_Kind;
				
				nRet = nKind;
			}
			break;

		case GDI_IS_GM:
			{
				nRet = Player[CLIENT_PLAYER_INDEX].IsGM() ? TRUE : FALSE;
			}
			break;

		case GDI_EXP_INSURANCE_STATE:
			{
				nRet = GetClientPlayer().m_IsExpInsuraceValid;
			}
			break;

		case GDI_EXP_INSURANCE_VALUE:
			{
				nRet = GetClientPlayer().m_CurrentExpReward;
			}
			break;
				
		case GDI_EXP_INSURANCE_VALUE_MAX:
			{
				int  nPlayerLevel = GetClientPlayer().GetLevel();
				nRet = KExpQuestInsuraceSetting::Singleton().GetMaxExpRewardByLevel(nPlayerLevel);
			}
			break;

		case GDI_EXP_INSURANCE_ENABLE_LEVEL:
			{
				nRet = KExpQuestInsuraceSetting::Singleton().GetExpEnableLevel();
			}
			break;
				//任务保险
		case GDI_QUEST_INSURANCE_STATE:
			{
				nRet = GetClientPlayer().m_IsQuestInsuranceValid;
			}
			break;

        case GDI_QUEST_INSURNACE_VALUE:
			{
				nRet = GetClientPlayer().m_CurrentQuestReward;
			}
			break;

		case GDI_QUEST_INSURANCE_VALUE_MAX:
			{
				nRet = KExpQuestInsuraceSetting::Singleton().GetMaxOfflineReward();
			}
			break;
		
		case GDI_QUEST_INSURANCE_ENABLE_LEVEL:
			{
				nRet = KExpQuestInsuraceSetting::Singleton().GetQuestEnableLevel();
			}
			break;
		case GDI_IS_ITEM_LOCKED_BY_DATE:
			{
				nRet = false;
				if ( uParam > 0 && uParam < MAX_ITEM )
				{
					nRet = Item[uParam].IsLockedByDate( -1 );
				}

			}
			break;

		case GDI_GET_PASSWORD_STATE:
			{
				nRet = GetClientPlayer().m_IsPasswordExist;
			}
			break;
		case GDI_GET_TITLE_INFO:
			{
				UiTitleInfo* uiTitleInfoArray = (UiTitleInfo*)uParam;
				int maxCount = nParam;
				nRet = GetClientPlayer().GetTitleManager().GetSelfTitleInfo(uiTitleInfoArray, maxCount);
			}
			break;
		case GDI_GET_DETAIL_TITLE_INFO:
			{
				UiTitleInfo* uiTitleInfoArray = (UiTitleInfo*)uParam;
				int maxCount = nParam;
				nRet = GetClientPlayer().GetTitleManager().GetDetailSelfTitleInfo(uiTitleInfoArray, maxCount);
			}
			break;
		case GDI_GET_SELECTED_TITLE:
			{
				nRet = GetClientPlayer().GetTitleManager().GetCurrentSelectedTitle();
			}
			break;
		case GDI_GET_SELF_TITLE:
			{
				int npcIndex = GetClientPlayer().GetNpcIndex();
				if (IsValidNpc(npcIndex))
				{
					int titleIndex = 0;
					int titleLevel = 0;
					Npc[npcIndex].GetTitle(titleIndex, titleLevel);

					nRet = titleIndex;
				}
				else
				{
					nRet = -1;
				}
			}
			break;
		case GDI_GET_NPC_TITLE:
			{
				UiNpcTitle* pTitle = (UiNpcTitle*)uParam;
				int npcIndex = NpcSet.SearchID(pTitle->NpcId);
				if (IsValidNpc(npcIndex))
				{
					int titleIndex = 0;
					int titleLevel = 0;
					Npc[npcIndex].GetTitle(titleIndex, titleLevel);

					TitleInfo* pInfo = TitleManager::GetTitleInfo(titleIndex);
					if (pInfo)
					{
						if (pInfo->Type == title_type_upgradable)
						{
							strncpy(pTitle->Title, pInfo->UpgradInfo[titleLevel].Name, sizeof(pTitle->Title));
						}
						else
						{
							strncpy(pTitle->Title, pInfo->Name, sizeof(pTitle->Title));
						}

						pTitle->Title[sizeof(pTitle->Title) - 1] = 0;

						nRet = TRUE;
					}
				}
			}
			break;
		case GDI_GET_SOCIETY_REQUIRE_LEVEL:
			{
				nRet = -1;
				
				SocietyInfoIndex* societyInfoIndex = reinterpret_cast< SocietyInfoIndex* >( uParam );
				if ( NULL == societyInfoIndex ) 
					break;
				
				RelationTemplate* pTemplate = RTM::Singleton().GetTemplate( societyInfoIndex->TemplateId );
				if ( NULL == pTemplate )
					break;
				
				const PRelationLayer pLayer = pTemplate->GetLayer( societyInfoIndex->Layer );
				if ( NULL == pLayer )
					break;
				
				if ( societyInfoIndex->Operation < enSUO_Num * 2 && societyInfoIndex->Operation >= 0 )
				{
					nRet = pLayer->Operations[ societyInfoIndex->Operation ].RequireLevel;
				}
			}
		break;
    #endif
		//end add
	}

	return nRet;
}

//--------------------------------------------------------------------------
//	功能：向游戏发送操作
//	参数：unsigned int uDataId --> Core外部客户对core的操作请求的索引定义
//							其值为梅举类型GAMEOPERATION_INDEX的取值之一。
//		  unsigned int uParam  --> 依据uOperId的取值情况而定
//		  int nParam --> 依据uOperId的取值情况而定
//	返回：如果成功发送操作请求，函数返回非0值，否则返回0值。
//--------------------------------------------------------------------------
int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{
	case GOI_USE_YIBU_ITEM:
		{
			char* szTip = (char*)uParam;
			if ( szTip )
			{
				INTERACTIVE_SCRIPT_INPUT tagInput;
				tagInput.Protocol =	c2s_interactive_script_input;
				memcpy( tagInput.Input, szTip, sizeof(tagInput.Input) );
				if ( g_pClient )
				{
					g_pClient->SendPackToServer(g_ConnectID, &tagInput, sizeof(tagInput) );
				}
			}
		}
		break;
	case GOI_FIND_PLAYER:
		{
			char* szName = (char*)uParam;
			if ( szName )
			{
				FindParam tagParam;
				memset(&tagParam, 0, sizeof(tagParam));
				tagParam.ProtocolType = c2s_find_family;
				strncpy( tagParam.szName, szName, 17 );
				if ( g_pClient )
				{
					g_pClient->SendPackToServer(g_ConnectID, &tagParam, sizeof(tagParam) );
				}
			}
		}
		break;
	case GOI_SWITCH_LIFE_ROLE:
		{
			if ( NpcSet.CheckShowLife() )
			{
				NpcSet.SetShowLifeFlag( false );				
			}
			else
			{
				NpcSet.SetShowLifeFlag( true );				
			}
		}
		break;
	case GOI_SWITCH_NAME_ROLE:
		{
			if ( NpcSet.CheckShowName() )
			{
				NpcSet.SetShowNameFlag( false );
				CoreDataChanged( GDCNI_ROLEHEADINFO_VISIBLE, 0, NULL );
			}
			else
			{
				NpcSet.SetShowNameFlag( true );
				CoreDataChanged( GDCNI_ROLEHEADINFO_VISIBLE, 1, NULL );
			}
		}
		break;
		///自动买卖/
	case GOI_AUTO_SELL_ITEMS:
		{
			AutoRobotMgr::Singleton().SetMode(enRobotMode_Selling);
		}
		break;
	case GOI_AUTOUSE_ITEM_SWITCH:
		{
			if(nParam == false)
			{
				AutoRobotMgr::Singleton().RemoveAutoEnstrustMode(ENSTRUST_AUTO_USEITEM_MODE);
			}
			else
			{
				AutoRobotMgr::Singleton().SetAutoEnstrustMode(ENSTRUST_AUTO_USEITEM_MODE);
			}
			AutoRobotMgr::Singleton().SetAutoUseMedicineFlags(AUTO_USE_HP_MEDICINE_NORMAL);
		}
		break;
	case GOI_AUTOUSE_ITEM_FLAGS:
		{
			if(uParam == 0)
				break;
			if(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_USEITEM_MODE)
			{
				unsigned int flags = AutoRobotMgr::Singleton().GetAutoUseMedicineFlags();
				if(nParam)
				{
					if(uParam == AUTO_USE_HP_MEDICINE_LIFE_1)
					{
						AutoRobotMgr::Singleton().AddAutoUseMedicineFlags(uParam);
						AutoRobotMgr::Singleton().RemoveAutoUseMedicineFlags(AUTO_USE_HP_MEDICINE_LIFE_2);
						AutoRobotMgr::Singleton().SetAutoUseMedicineHPPersent(30);
					}
					else
					if(uParam == AUTO_USE_HP_MEDICINE_LIFE_2)
					{
						AutoRobotMgr::Singleton().AddAutoUseMedicineFlags(uParam);
						AutoRobotMgr::Singleton().RemoveAutoUseMedicineFlags(AUTO_USE_HP_MEDICINE_LIFE_1);
						AutoRobotMgr::Singleton().SetAutoUseMedicineHPPersent(60);
					}
					else
					if(uParam == AUTO_USE_MP_MEDICINE_1)
					{
						AutoRobotMgr::Singleton().AddAutoUseMedicineFlags(uParam);
						AutoRobotMgr::Singleton().SetAutoUseMedicineMPPersent(30);
					}
					if (uParam == AUTO_USE_MP_MEDICINE_2)
					{
						AutoRobotMgr::Singleton().AddAutoUseMedicineFlags(uParam);
						AutoRobotMgr::Singleton().SetAutoUseMedicineMPPersent(60);
					}
				}
				else
				{
					if(uParam == AUTO_USE_HP_MEDICINE_LIFE_1||uParam == AUTO_USE_HP_MEDICINE_LIFE_2)
					{
						AutoRobotMgr::Singleton().RemoveAutoUseMedicineFlags(uParam);
					}
					else
					if(uParam == AUTO_USE_MP_MEDICINE_1)
					{
						AutoRobotMgr::Singleton().RemoveAutoUseMedicineFlags(AUTO_USE_MP_MEDICINE_1);
						AutoRobotMgr::Singleton().SetAutoUseMedicineMPPersent(0);

					}
				}
			}/*
				if(uParam&flags)
				{
					AutoRobotMgr::Singleton().RemoveAutoUseMedicineFlags(uParam);
					if((uParam&AUTO_USE_HP_MEDICINE_LIFE_1)||(uParam&AUTO_USE_HP_MEDICINE_LIFE_2)||(uParam&AUTO_USE_HP_MEDICINE_LIFE_ONWER))
						AutoRobotMgr::Singleton().SetAutoUseMedicineHPPersent(0);
					if(uParam&AUTO_USE_MP_MEDICINE_1)
						AutoRobotMgr::Singleton().SetAutoUseMedicineMPPersent(0);
				}
				else
				{
					AutoRobotMgr::Singleton().AddAutoUseMedicineFlags(uParam);
					if(uParam&AUTO_USE_HP_MEDICINE_LIFE_1)
						AutoRobotMgr::Singleton().SetAutoUseMedicineHPPersent(60);
					if(uParam&AUTO_USE_HP_MEDICINE_LIFE_2)
						AutoRobotMgr::Singleton().SetAutoUseMedicineHPPersent(40);
					if(uParam&AUTO_USE_MP_MEDICINE_1)
						AutoRobotMgr::Singleton().SetAutoUseMedicineMPPersent(20);
				}
			}*/
		}
		break;
	case GOI_FURY_EXPLODE:
		{
			KClientFuryMgr::Singlton().ExplodeCurFury();
		}//end for 
		break;

	case GOI_AUTOPICKUP_ITEM_SWITCH:
		{
			if(!(bool)nParam)
			{
				AutoRobotMgr::Singleton().RemoveAutoEnstrustMode(ENSTRUST_AUTO_PICK_UP_MODE);
			}
			else
			{
				AutoRobotMgr::Singleton().SetAutoEnstrustMode(ENSTRUST_AUTO_PICK_UP_MODE);
				AutoRobotMgr::Singleton().SetNotPickupList((deque<ItemClassInfo> *)uParam);
			}
			AutoRobotMgr::Singleton().ResetPickupItemFlags();
			AutoRobotMgr::Singleton().m_pickTypeFlags = 0;
			
		}
		break;
	case GOI_AUTO_RUN_DISTANCE:
		{

			AutoRobotMgr::Singleton().randRunDistance = nParam;
		}
		break;
	case GOI_AUTO_ATTACK_RADIUS:
		{
			if (nParam < 0)
				break;
			AutoRobotMgr::Singleton( ).attackRadius = nParam;
		}	
		break;
	case GOI_AUTO_PICKUP_PICK_MEDICINE:
		{
			if((uParam) == 1)
			{
				AutoRobotMgr::Singleton().pickMedicine = true;
			}
			else
			if(uParam == 0)
			{
				AutoRobotMgr::Singleton().pickMedicine = false;
			}
		}
		break;
	case GOI_AUTO_PICKUP_TYPE_FLAGS:
		{
			if(nParam == 0)
			{
				if(uParam == AutoRobotMgr::Singleton().m_pickTypeFlags)
					AutoRobotMgr::Singleton().m_pickTypeFlags = 0;
				break;
			}
			if(uParam == AutoRobotMgr::Singleton().m_pickTypeFlags)
			{
					AutoRobotMgr::Singleton().m_pickTypeFlags = 0;
			}
			else
			{
				AutoRobotMgr::Singleton().m_pickTypeFlags = uParam;
			}
		}
		break;
	case GOI_AUTOPICKUP_ITEM_FLAGS:
		{
			if(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_PICK_UP_MODE)
			{
				if(uParam&AUTO_PICKUP_ITEM_DEAR_SET_FIRST)
				{
					if(AutoRobotMgr::Singleton().GetAutoPickupItemFlags()&AUTO_PICKUP_ITEM_DEAR_SET_FIRST)
						AutoRobotMgr::Singleton().RemoveAutoPickupItemFlags(AUTO_PICKUP_ITEM_DEAR_SET_FIRST);
					else
						AutoRobotMgr::Singleton().AddAutoPickupItemFlags(AUTO_PICKUP_ITEM_DEAR_SET_FIRST);
				}
			}
			
		}
		break;
	case GOI_AUTOATTACK_SWITCH:
		{
			if (!((bool)nParam))
			{
				CFS_FILELOGS::WriteLog("Switch off.\n");
				AutoRobotMgr::Singleton().SetMode(enRobotMode_None);
				if (1 == (AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_ATTACK_MODE))
				{
					ShowChatErrorMsg(MSG_ROBOT_AUTOATTACK_OFF);
				}
				else
				{
					int attackState = (int)uParam;
					if (1 != attackState)
					{
						ShowChatErrorMsg(MSG_ROBOT_AUTOATTACK_NOT_CLICK_ON);
					}
				}
				AutoRobotMgr::Singleton().RemoveAutoEnstrustMode(ENSTRUST_AUTO_ATTACK_MODE);
				AutoRobotMgr::Singleton().BackupAttackSkill();
				nRet = 0;
			}
			else
			{
				AutoRobotMgr::Singleton().SetAutoEnstrustMode(ENSTRUST_AUTO_ATTACK_MODE);
				ShowChatErrorMsg(MSG_ROBOT_AUTOATTACK_ON);
				AutoRobotMgr::Singleton().SetMode( enRobotMode_AutoAttack );
				AutoRobotMgr::Singleton().SetAutoAttackPos();
				nRet = 1;
			}
			AutoRobotMgr::Singleton().SetAutoAttackFlags(AUTO_ATTACK_FLAG_NORNAL);
			AutoRobotMgr::Singleton().SetAutoAttackEnemyLevel(0);
			AutoRobotMgr::Singleton().canNotAttackNpcList.clear();
		}
		break;
	case GOI_AUTOATTACK_ITEM_FLAGS:
		{
			if(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_ATTACK_MODE)
			{
				if(!((bool)nParam))
				{
					AutoRobotMgr::Singleton().RemoveAutoAttackFlags(uParam);
					if(uParam&AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER)
						AutoRobotMgr::Singleton().SetAutoAttackEnemyHigherLevel(0);
					if(uParam&AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER)
						AutoRobotMgr::Singleton().SetAutoAttackEnemyLowerLevel(0);
					nRet = 0;
				}
				else
				{
					AutoRobotMgr::Singleton().AddAutoAttackFlags(uParam);
					if(uParam&AUTO_ATTACK_FLAG_ENEMY_LEVEL_HIGHER) 
						AutoRobotMgr::Singleton().SetAutoAttackEnemyHigherLevel(AUTO_ATTACK_HIGHER_LEVEL);
					if(uParam&AUTO_ATTACK_FLAG_ENEMY_LEVEL_LOWER)
						AutoRobotMgr::Singleton().SetAutoAttackEnemyLowerLevel(AUTO_ATTACK_LOWER_LEVEL);
					nRet = 1;
				}
			}
		}
		break;
	case GOI_AUTO_ATTACK_BLAST:
		{
			if(AutoRobotMgr::Singleton().GetAutoEnstrustMode()&ENSTRUST_AUTO_ATTACK_MODE)
			{
				AutoRobotMgr::Singleton().SetAutoBlastFlag((bool)nParam);
				nRet = 1;
			}
			else
			{
				AutoRobotMgr::Singleton().SetAutoBlastFlag(false);
				nRet = 0;
			}
		}
		break;
	case GOI_AUTO_REPAIR:
		{
			AutoRobotMgr::Singleton().SetAutoRepairFlag((bool)nParam);
			if ((bool)nParam)
			{
				nRet = 1;
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	case GOI_AUTO_REPAIR_ERROR_MSG:
		{
			char * errMsg = (char *)uParam;
			if (errMsg != NULL)
			{
				ShowChatErrorMsg(errMsg);
			}
		}
		break;
	case GOI_AUTO_CAST_SKILL:
		{
			AutoCastSkillInfo * pSkillInfo = (AutoCastSkillInfo *)uParam;
			if (pSkillInfo != NULL)
			{
				AutoRobotMgr::Singleton().SetAutoCastSkill(nParam, * pSkillInfo);
			}		
		}
		break;
	case GOI_SET_ACTION:
		{
			switch( uParam)
			{
			case do_stand:
				{
					if ( g_pClient )
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendServerStopCmd();
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand( do_stand );
					}

				}
				break;
			}
		}
		break;
	case GOI_AUTO_FIND_MAP_WAY:
		{
			//跨地图寻路
			MapPosInfo* pMapInfo = (MapPosInfo*)uParam;
			if(pMapInfo == NULL)
				nRet = 0;
			nRet = AutoRobotMgr::Singleton().GoToOTher( pMapInfo );
		}
		break;
//	case GOI_SET_FOLLOW_ATTACK:
//		{
// 			bool bFollowAndAttack = false;
// 			if ( uParam > 0)
// 			{
// 				bFollowAndAttack = true;
// 			}
// 			else
// 			{
// 				bFollowAndAttack = false;
// 			}
// 			int target = Npc[GetClientPlayer().GetNpcIndex()].GetTargetNpc();
// 			if (bFollowAndAttack)
// 			{
// 				PlayerController::Singleton().FollowAttack(target);
// 			}
// 			else
// 			{
// 				PlayerController::Singleton().Stop();
// 			}
			//Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( bFollowAndAttack );
//		}
//		break;
	case GOI_CHANGE_ROOM_OWNER:
		{
			g_ChatCenterC.ChangeRoomOwnerReq( uParam, (const char*)nParam );
		}
		break;
	case GOI_ROOM_SCREEN:
		{
			g_ChatCenterC.ForbitChatInRoomReq( uParam, (const char*)nParam, true );
		}
		break;
	case GOI_ROOM_UNSCREEN:
		{
			g_ChatCenterC.ForbitChatInRoomReq( uParam, (const char*)nParam, false );
		}
		break;
	/*!
	\brief
		Send message to a person by role name.
	
	\param uParam
		(const string &)The person role name.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	case GOI_SEND_CHAT_DATE_P2P:		
		{
			g_ChatCenterC.ChatToSomeoneByName( (char*)uParam, (BYTE*)nParam, strlen( (const char*)nParam ) );
		}
		break;

	/*!
	\brief
		Send message to a chat room by the chat room id.
	
	\param uParam
		(DWORD)The chat room id.

  	\param nParam
		(BYTE *)The message.

	\return
		Nothing.
	*/
	case GOI_SEND_CHAT_DATE_P2R:	
		{
			g_ChatCenterC.ChatInRoom( uParam, (BYTE*)nParam, strlen( (const char*)nParam ) + 1 );
		}
		break;		

	/*!
	\brief
		Create a chat room.
	
	\param uParam
		(DWORD)The chat room window data.

  	\param nParam
		(const string &)The member role name.

	\return
		Nothing.
	*/
	case GOI_CREATE_CHATROOM:
		{
			g_ChatCenterC.CreateChatRoomReq( (const char *)uParam );
		}
		break;

	/*!
	\brief
		Add a new member to the chat room.
	
	\param uParam
		(DWORD)The room ID.

  	\param nParam
		(const string &)The member role name.

	\return
		Nothing.
	*/
	case GOI_ADDTO_CHATROOM:
		{
			g_ChatCenterC.AddMemberToRoomReq( uParam, (const char *)nParam );
		}
		break;

	/*!
	\brief
		Leave from the chat room.
	
	\param uParam
		(DWORD)The room ID.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_LEAVE_CHATROOM:
		{
			g_ChatCenterC.LeaveRoom( uParam );
		}
		break;

	/*!
	\brief
		Kick member from the chat room.
	
	\param uParam
		(DWORD)The room ID.

  	\param nParam
		(const string &)The kicked member role name.

	\return
		Nothing.
	*/
	case GOI_KICK_CHATROOM:
		{
			g_ChatCenterC.KickRoomMemberReq( uParam, (const char *)nParam );
		}
		break;

	/*!
	\brief
		Add a new friend.
	
	\param uParam
		(const string &)The new friend role name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_CHAT_FRIEND_ADD:
		{
			unsigned long   dwOldGroupId=g_ChatCenterC.GetObjGroupId((const char *)uParam);

			/*if (dwOldGroupId!=INVALID_GROUP_ID && dwOldGroupId==GROUPID_TEMP)
			{
				g_ChatCenterC.ChangeGroupReq((const char *)uParam,GROUPID_TEMP,nParam);
			}
			else*/
			g_ChatCenterC.AddObjectReq( (const char *)uParam, nParam );

		}
		break;	
		
	case GOI_CHAT_FRIEND_CHANGE_GROUP:
		{
			int* pGroupID = (int *)nParam;
			if ( pGroupID && uParam )
			{
				g_ChatCenterC.ChangeGroupReq((const char *)uParam, pGroupID[0], pGroupID[1]);
			}
		}
		break;

	case GOI_CHAT_ADD_BLACK_LIST:
		{
			unsigned long   dwOldGroupId=g_ChatCenterC.GetObjGroupId((const char *)uParam);
			if (dwOldGroupId!=INVALID_GROUP_ID && dwOldGroupId!=GROUPID_BLACK)
			{
				g_ChatCenterC.ChangeGroupReq((const char *)uParam,dwOldGroupId,GROUPID_BLACK);
			}
			else
				g_ChatCenterC.AddObjectReq( (const char *)uParam, GROUPID_BLACK );

		}
		break;
	case GOI_CHAT_ADD_BLACK_LIST_BY_ID:
		{
			int npcIndex = NpcSet.SearchID(uParam);
			if ( npcIndex && Npc[npcIndex].m_Kind == kind_player)
			{
				g_ChatCenterC.AddObjectReq( (const char *)Npc[npcIndex].Name, GROUPID_BLACK );
			}
			
		}
		break;
	
	/*!
	\brief
		Delete a friend.
	
	\param uParam
		(const string &)The deleted role name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_CHAT_FRIEND_DELETE:		
		{
			g_ChatCenterC.RemoveObjectReq( (const char *)uParam );
		}
		break;

	/*!
	\brief
		Create a new friend group.
	
	\param uParam
		(const string &)The new friend group name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_CHAT_GROUP_NEW:	
		{
			g_ChatCenterC.CreateGroupReq( (const char *)uParam );
		}
		break;
		
	/*!
	\brief
		Delete a new friend group.
	
	\param uParam
		(const string &)The delete friend group name.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_CHAT_GROUP_DELETE:
		{
			g_ChatCenterC.DeleteGroupReq( uParam );
		}
		break;

	/*!
	\brief
		Rename a new friend group.
	
	\param uParam
		(const string &)The old friend group name.

  	\param nParam
		(const string &)The new friend group name.

	\return
		Nothing.
	*/
	case GOI_CHAT_GROUP_RENAME:
		{
			g_ChatCenterC.RenameGroupReq( uParam, (const char*)nParam );
		}
		break;
	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_MAIL_LIST_REQ:
		{
			g_ChatCenterC.LoadMailListReq(uParam);
		}
		break;

	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_MAIL_REQ:
		{
			g_ChatCenterC.LoadMailReq( uParam );
		}
		break;	
	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_SEND_MAIL:
		{
			PMAIL_PARAM pMail = (PMAIL_PARAM)uParam;
			if ( pMail )
			{
				DWORD nItemIdx = (DWORD)pMail->pPlusData[0];
				DWORD idx = Item[nItemIdx].GetID();
				memcpy( pMail->pPlusData, &idx, sizeof(DWORD) );				
				g_ChatCenterC.SendMail(pMail);
			}
		}
		break;
	case GOI_DEL_MAIL:
		{
			g_ChatCenterC.DelMailReq( uParam );
		}
		break;

	case GOI_SEND_BACK_MAIL:
		{
			g_ChatCenterC.ReturnMailReq( uParam );
		}
		break;
	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_GET_MONEY:
		{
			g_ChatCenterC.GetOutMoneyReq(uParam);
		}
		break;
	/*!
	\brief
		Send a mail to some role.
	
	\param uParam
		(KMail*)The mail param point.

  	\param nParam
		NULL.

	\return
		Nothing.
	*/
	case GOI_GET_ITEM:
		{
			g_ChatCenterC.GetOutItemReq(uParam, nParam);
		}
		break;

	/************************************************************************/
	/*					Buffer system                                       */
	/************************************************************************/
	/*!
	\brief
		Request delete the buffer.
	
	\param uParam
		(DWORD)The buffer ID.

  	\param nParam
		(int)no used.

	\return
		Nothing.
	*/	
	case GOI_DEL_BUFFER:
		{
			_Buff_Cancel	BuffCan;
			BuffCan.Protocol		= c2s_buff_family;
			BuffCan.ProtocolExtend	= buff_cop_cancel;
			BuffCan.wProtocolSize	= sizeof(_Buff_Cancel) - 1;
			BuffCan.ulBuffID		= uParam;

			if (g_pClient)
				g_pClient->SendPackToServer(
				g_ConnectID,
				&BuffCan, 
				sizeof(_Buff_Cancel));

		}
		break;
		
	/************************************************************************/
	/*							Quest system                                */
	/************************************************************************/
	/*!
	\brief
		Submit a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/
	case GOI_OK_QUEST:
		{
			KQuestRequest* tagRequest = (KQuestRequest*)uParam;
            unsigned int uSize = (sizeof(PLAYER_SELECTUI_COMMAND) - sizeof(DWORD) + (tagRequest->requiredItemIndex.size() * sizeof(int)));
			PLAYER_SELECTUI_COMMAND* command = (PLAYER_SELECTUI_COMMAND*)new char[uSize];
            memset(command, 0, uSize);
            command->wLength = uSize;
			command->nSelectIndex = tagRequest->eOperType;
            command->nParam1 = tagRequest->uParam;
            command->nCount = tagRequest->requiredItemIndex.size();
            for(int index = 0;index<tagRequest->requiredItemIndex.size();index++)
            {
               command->pdwSelArray[index] = Item[tagRequest->requiredItemIndex[index]].GetID();
            }
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(command, UI_SELECTDIALOG);
            delete[] command;
		}
		break;

	/*!
	\brief
		Cancel a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/	
	case GOI_CANCEL_QUEST:
		{
			KQuestRequest* tagRequest = (KQuestRequest*)uParam;
            PLAYER_SELECTUI_COMMAND command;
			command.wLength = sizeof(PLAYER_SELECTUI_COMMAND) - sizeof(DWORD);
			command.nSelectIndex = tagRequest->eOperType;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		}
		break;

	/*!
	\brief
		Delete a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/	
	case GOI_DELETE_QUEST:
		{
			int questId = (int)uParam;
			QuestLog::GetInstance()->AbandonQuest(questId);
		}
		break;

	/*!
	\brief
		Delete a quest.
  
	\param 
		(KQuestRequest*)pQuest.
	
	\return
		Nothing.
	*/	
	case GOI_QUEST_REQUEST:
		{
            PLAYER_SELECTUI_COMMAND command;
			command.wLength = sizeof(PLAYER_SELECTUI_COMMAND) - sizeof(DWORD);
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		}
		break;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//uParam = (const char*)pszFileName
	case GOI_PLAY_SOUND:
		if (uParam)
		{
			static KCacheNode* pSndNode = NULL;
			KWavSound* pSound = NULL;
			pSndNode	= (KCacheNode*)g_SoundCache.GetNode((char *)uParam, (KCacheNode * )pSndNode);
			pSound		= (KWavSound*)pSndNode->m_lpData;
			if (pSound)
			{
				if (pSound->IsPlaying())
					break;
				pSound->Play(0, Option.GetSndVolume(), 0);
				//pSound->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
			}
		}
		break;
	case GOI_PLAYER_RENASCENCE:
		{
			//--> Rocker 2005/06/29 根据玩家的选项来决定惩罚方式
			if (uParam == SMCT_UI_RENASCENCE)
			{
				SendClientCmdRevive(0);
			}
			// <Add name="Adt.X" time="2005/09/21">
			else if (uParam == SMCT_UI_RENASCENCE_KILLED_ON_WAR)
			{
				if (nParam == 0)
				{	// 使用重生水晶
					SendClientCmdRevive(3);				
				}
				else
				{	// 不使用重生水晶
					SendClientCmdRevive(4);				
				}
				
			}
			// </Add>			
			else
			{	
				SendClientCmdRevive(nParam + 1);
			}
			//<-- End
		}
		break;
	case GOI_MONEY_INOUT_STORE_BOX:
		{
			BOOL	bIn = (BOOL)uParam;
			int		nMoney = nParam;
			int		nSrcRoom, nDesRoom;


			if (bIn)
			{
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;
			}
			else
			{
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom, nDesRoom, nMoney);
		}
		break;
		//离开游戏
	case GOI_EXIT_GAME:
		{
			if(enPLO_Logout == uParam)
			{
				gbSwitchPaintAlphaType = false;
				CFS_FILELOGS::WriteLog("Exit Game.\n");
				AutoRobotMgr::Singleton().SetMode(enRobotMode_None);
				AutoRobotMgr::Singleton().SetAutoSellState(false);
				AutoRobotMgr::Singleton().StopGoToOtherMap(false);
				AutoRobotMgr::Singleton().CanAttackNpc( -1 );
				g_SubWorldSet.Close();
				g_ScenePlace.ClosePlace();
				g_ChatCenterC.Init();
				for ( int i = 0; i < MAX_PLAYER; i++)
				{
					Player[i].Release();
					Player[i].SetPlayerIndex(i);
					Player[i].m_ItemList.Init(i);
					Player[i].m_Node.m_nIndex = i;
				}
				g_ProtocolSimulationSet.Clear();
				LockSomeoneAction(0);
			}
			else
			{
				PLAYER_LOGOUT	c2sLogout;
				c2sLogout.protocol = c2s_player_logout;
				c2sLogout.operation = (BYTE)uParam;
				SendDataToServer(&c2sLogout, sizeof(c2sLogout));
				g_ProtocolSimulationSet.Clear();
			}
		}
		break;
	case GOI_GAMESPACE_DISCONNECTED:
		g_SubWorldSet.Close();
		GetClientPlayer().GetTaisuiWheelSys()->ReFresh(); //This enable the taisui rule available
		break;
	case GOI_TRADE_NPC_BAG_IS_FULL:
		{
			//判断是否背包格子够，检测同类物品，因为服务器有拦截，暂不开放，只做简单检测
			/*
			ItemType* pItemInfo = (ItemType*)uParam;
			int itemCount = nParam;
			nRet = 0;

			if ( pItemInfo && itemCount > 0 )
			{
				const KBASICPROP_ITEM* pTemplate = g_ItemGen.GetItemTemplate( pItemInfo->genre, pItemInfo->detail, pItemInfo->particular, pItemInfo->level );
				if ( pTemplate )
				{
					KItem item;
					g_ItemGen.Gen_Item( pItemInfo->genre, pItemInfo->detail, pItemInfo->particular, pItemInfo->level, 1, &item );
					
					ItemPos	Pos;
					EXTRAINFOPLUS tagExtraPlus;
					tagExtraPlus.nItemGenre = item.GetGenre();
					tagExtraPlus.nParticularType = item.GetParticular();
					tagExtraPlus.nDetailType = item.GetDetailType();
					tagExtraPlus.nMaxItem = item.GetMaxItemCount();
					tagExtraPlus.nCurItem = itemCount;
					if(Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(&Pos, &tagExtraPlus) == false)
					{
						CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Bag_Full);
						nRet = 1;
						break;
					}
				}
			}//*/
			ItemPos checkPos;
			if( Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(&checkPos) )
				nRet = 0;
			else
				nRet = 1;
		}
		break;
	case GOI_TRADE_NPC_BUY:
		{
			int itemIndex = (int)uParam;
			int shopIndex = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx;
			int itemCount = nParam;

			KItem* pItem = BuySell.getItem(shopIndex, itemIndex);

			if(!pItem)
			{
				nRet = 0;
				break;
			}

			//购买限制物品需要检查是否已经拥有该物品的最多实例
			if (pItem->GetRestrictCount() > 0)
			{
				if (Player[CLIENT_PLAYER_INDEX].GetItemList().CountItem(pItem->GetGenre(),
					pItem->GetDetailType(),
					pItem->GetParticular(),
					pItem->GetLevel()) >= pItem->GetRestrictCount())
				{
					CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Too_Many_Same_Item);
					nRet = 0;
					break;
				}
			}

			//判断是否金钱够
			if (  BuySell.GetPlusPointType(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx) != -1 )
			{
				unsigned long money = Player[CLIENT_PLAYER_INDEX].GetPlusPoint( BuySell.GetPlusPointType(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx) );
				if(money < BuySell.GetItemPlusPoint(shopIndex, itemIndex) * itemCount)
				{
					CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Not_Enough_Money);
					nRet = 0;
					break;
				}

				int ppsItemIdx = BuySell.GetItemIndex(shopIndex, itemIndex);
				if ( ppsItemIdx != -1 )
				{
					if( !BuySell.IsPlusPointOk(ppsItemIdx,CLIENT_PLAYER_INDEX) )
					{
						CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Not_Plus_Point_Limit);
						nRet = 0;
						break;
					}
				}
			}
			else
			{
				unsigned long money = Player[CLIENT_PLAYER_INDEX].GetItemList().GetEquipmentMoney();
				if(money < pItem->GetPrice() * itemCount)
				{
					CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Not_Enough_Money);
					nRet = 0;
					break;
				}
			}

			SendClientCmdBuy(itemIndex, itemCount);
		}
		break;
	case GOI_TRADE_NPC_SELL:
		{
			KObjAtContRegion* pObject1 = (KObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;

			int nIdx = pObject1->Obj.uId;
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (Item[nIdx].GetGenre() == item_task && Item[nIdx].GetLevel() != QK_Bag)
				{
					return 0;
				}
				else if ( Item[nIdx].GetGenre() == item_magicorscript && Item[nIdx].GetParticular() == 145 )
				{
					return 0;
				}
				SendClientCmdSell(Item[nIdx].GetID());
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR:
		{
			KObjAtContRegion* pObject1 = (KObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;

			//放下去的东西不为空，所以是卖东西
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{		
				//-->Rocker 2004/11/24 
				float nPrice = Item[nIdx].getRepairPrice(((TRUE == nParam) ? true : false));

				if (Item[nIdx].GetGenre() != item_equip)
				{
					return 0;
				}
				else if (Item[nIdx].GetDurability() == -1 || Item[nIdx].GetDurability() == Item[nIdx].GetMaxDurability())
				{
					return 0;
				}
				else
				{
					int holdMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney();
					int insteadSpecieIndex = ConfigManager::Singleton().GetGlobalVariable(globar_var_instead_specie_index);
					if (insteadSpecieIndex == 0)
					{
						insteadSpecieIndex = 12;
					}
					unsigned long insteadSpecieCount = Player[CLIENT_PLAYER_INDEX].GetPlusPoint(insteadSpecieIndex);
					unsigned long sumMoney = 0;
					if (holdMoney > 0)
					{
						sumMoney = holdMoney + insteadSpecieCount;
					}
					else
					{
						sumMoney = insteadSpecieCount;
					}
					if (nPrice <= sumMoney)
					{
						SendClientCmdRepair(Item[nIdx].GetID(), ((TRUE == nParam) ? true : false));
					}
				}
				
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR_ALL:
		{
			KItemList& itemList = Player[CLIENT_PLAYER_INDEX].GetItemList();
			for(int i = 0; i < MAX_PLAYER_ITEM; ++i)
			{
				PlayerItem& item = itemList.m_Items[i];
				if(item.nPlace != pos_equip && item.nPlace != pos_equiproom)
				{
					continue;
				}
				int itemIndex = item.nIdx;
				
				if(itemIndex <= 0)
				{
					continue;
				}
				if(Item[itemIndex].GetGenre() != item_equip)
				{
					continue;
				}
				if(Item[itemIndex].GetDurability() == -1 || Item[itemIndex].GetDurability() == Item[itemIndex].GetMaxDurability())
				{
					continue;
				}
				if(Item[itemIndex].CanBeRepaired() == false)
				{
					continue;
				}
				SendClientCmdRepair(Item[itemIndex].GetID(), ((TRUE == nParam) ? true : false));
			}
		}
		break;
	case GOI_SWITCH_OBJECT:
		{
			ItemPos	P1, P2;
			KObjAtContRegion* pObject1 = (KObjAtContRegion*)uParam;
			KObjAtContRegion* pObject2 = (KObjAtContRegion*)nParam;
			
			if (!pObject1 && !pObject2)
				break;
			
			if (pObject1)
			{
				switch(pObject1->eContainer)
				{
				case UOC_ITEMBOX_EXTEND:
					P1.nPlace = pos_itembox_extend;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_STORE_EXTEND:
					P1.nPlace = pos_store_extend;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_SMITH:
					P1.nPlace = pos_beset;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				//-------> Ray [Luoliang] 2005-7-6
				case UOC_PET_FEED_BOX:		
					P1.nPlace = pos_pet_feed_box;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				//<------- End [Ray]
				case UOC_STORE_BOX:
					P1.nPlace = pos_repositoryroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;

				//Lucifer~yu[zhangjianyu] [03/16/2006] Add for new fs
				//begin------------------------------------------------------------------------
				case UOC_ITEM_TAKE_WITH:
					P1.nPlace	= pos_equiproom;
					P1.nX		= pObject1->Region.h;
					P1.nY		= pObject1->Region.v;
					P1.uGener	= pObject1->Obj.uGenre;
					P1.uId		= pObject1->Obj.uId;
					break;
				//end--------------------------------------------------------------------------	
				case UOC_EQUIPTMENT:
					{
						P1.nPlace = pos_equip;
						// lixuewu 2004.03.10	
						//P1.nX = PartConvert[pObject1->Region.v];
						P1.nX = pObject1->Region.h;
						// lixuewu 2004.03.10
					}
					break;
				case UOC_TO_BE_TRADE:
					P1.nPlace = pos_traderoom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_NPC_SHOP:
					if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;

					int nIdx = 0;
					KItem* pItem = NULL;
					
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nBuyIdx, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					
					ItemPos	Pos;
					
					//Modified by Ray[Luoliang]  2004-3-29
					//使用不带高度和宽度信息的SearchPosition
					//if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition( &Pos))

					EXTRAINFOPLUS tagExtraPlus;
					tagExtraPlus.nItemGenre = pItem->GetGenre();
					tagExtraPlus.nParticularType = pItem->GetParticular();
					tagExtraPlus.nDetailType = pItem->GetDetailType();
					tagExtraPlus.nMaxItem = pItem->GetMaxItemCount();
					tagExtraPlus.nCurItem = pItem->GetItemCount();
					tagExtraPlus.pCampareItem = pItem;
					
					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(&Pos, &tagExtraPlus))
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy_const(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
//						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom)
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
//						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy_const(sMsg.szMessage, MSG_SHOP_NO_MONEY);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
//						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					// 拿起来的东西不为空，所以是买东西
					SendClientCmdBuy(pObject1->Obj.uId, 1);
					break;
				}
			}
			
			if (pObject2)
			{
				switch(pObject2->eContainer)
				{
				case UOC_ITEMBOX_EXTEND:
					P2.nPlace = pos_itembox_extend;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_STORE_EXTEND:
					P2.nPlace = pos_store_extend;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_SMITH:
					P2.nPlace = pos_beset;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				//-------> Ray [Luoliang] 2005-7-6
				case UOC_PET_FEED_BOX:
					P2.nPlace = pos_pet_feed_box;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				//<------- End [Ray]
				case UOC_STORE_BOX:
					P2.nPlace = pos_repositoryroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				//Lucifer~yu[zhangjianyu] [03/16/2006] Add for new fs
				//begin------------------------------------------------------------------------
				case UOC_ITEM_TAKE_WITH:
					P2.nPlace = pos_equiproom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					P2.uGener	= pObject2->Obj.uGenre;
					P2.uId		= pObject2->Obj.uId; 
					break;
				//end--------------------------------------------------------------------------	
				case UOC_EQUIPTMENT:
					{
						P2.nPlace = pos_equip;
						// lixuewu 2004.03.10	
						//P2.nX = PartConvert[pObject2->Region.v];
						P2.nX = pObject2->Region.h;
						// lixuewu 2004.03.10	
					}
					break;
				case UOC_TO_BE_TRADE:
					P2.nPlace = pos_traderoom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_NPC_SHOP:
					break;
				}
			}
			if (!pObject1)
			{
				memcpy(&P1, &P2, sizeof(P1));
			}
			if (!pObject2)
			{
				memcpy(&P2, &P1, sizeof(P1));
			}
			Player[CLIENT_PLAYER_INDEX].MoveItem(P1, P2, pObject1->Region.Height);
		}
		break;
	case GOI_FIND_A_EMPTY_PLACE_OF_A_CONTAINER:
		{
			nRet = 0;
			KObjAtContRegion* region = (KObjAtContRegion*)uParam;
			INVENTORY_ROOM room = KItemList::clientContainer2CoreRoom(region->eContainer);
			POINT pos;
			KInventory* inventory = Player[CLIENT_PLAYER_INDEX].GetItemList().GetRoom(room);
			if(NULL == inventory)
			{
				break;
			}
			inventory->FindRoom(&pos);
			if(-1 == pos.x)
			{
				break;
			}
			region->Region.h = pos.x;
			region->Region.v = pos.y;
			nRet = 1;
		}
		break;
	//在法宝的指定位置镶嵌内丹
	case GOI_TALISMAN_INLAY:
		{
			TM_HOLE_POS* holePos = (TM_HOLE_POS*)uParam;
			int talismanId = holePos->talismanId;
			int enchasePos = holePos->holeIndex;
			int enchaseItemId = nParam;
			if ((talismanId > 0) && (enchasePos >= 0) && (enchasePos < TM_HOLE_NUM) && (enchaseItemId > 0))
			{
				TALISMAN_OPERATION talismanOperation;
				talismanOperation.Protocol = c2s_talisman_family;
				talismanOperation.SubProtocol = talisman_protocol_enchase;
				talismanOperation.Params[0] = talismanId;
				talismanOperation.Params[1] = enchasePos;
				talismanOperation.Params[2] = enchaseItemId;
			
				g_pClient->SendPackToServer(g_ConnectID,&talismanOperation, sizeof(talismanOperation));
			}
			break;
		}
	//法宝升级
	case GOI_TALISMAN_UPGRADE:
		{
			int talismanId = (int)uParam;
			if (talismanId > 0)
			{
				TALISMAN_OPERATION talismanOperation;
				talismanOperation.Protocol = c2s_talisman_family;
				talismanOperation.SubProtocol = talisman_protocol_upgrade;
				talismanOperation.Params[0] = talismanId;
			
				g_pClient->SendPackToServer(g_ConnectID,&talismanOperation, sizeof(talismanOperation));
			}
			break;
		}
	case GOI_TALISMAN_CONVERT_SKILL_EXP:
		{
			int talismanId = (int)uParam;
			int avaiableSkillExp = (int)nParam;
			if (talismanId > 0 && avaiableSkillExp > 0)
			{
				TALISMAN_OPERATION talismanOperation;
				talismanOperation.Protocol = c2s_talisman_family;
				talismanOperation.SubProtocol = talisman_protocol_convert_skill_exp;
				talismanOperation.Params[0] = talismanId;
				talismanOperation.Params[1] = avaiableSkillExp;
			
				g_pClient->SendPackToServer(g_ConnectID,&talismanOperation, sizeof(talismanOperation));
			}
			break;
		}
	//玩家点完对话框
	case GOI_INFORMATION_CONFIRM_NOTIFY:
	{
        PLAYER_SELECTUI_COMMAND command;
		//<---- Add By Ray [Luoliang] [2005-10-21]
		//exclude the count and id array
		command.wLength = sizeof(PLAYER_SELECTUI_COMMAND) - sizeof(DWORD);
		// End. Ray [LuoLiang] [2005-10-21] ---->
		command.nSelectIndex = 0;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
		break;
	}

	/************************************************************************/
	/*							Skill system                                */
	/************************************************************************/
	/*!
	\brief
		set immdia skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	case GOI_SET_IMMDIA_SKILL:
		{
		}
		break;

	/*!
	\brief
		Use a skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	case GOI_USE_SKILL:
		{
		}
		break;

	/*!
	\brief
		Cancel a skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	case GOI_SKILL_CANCEL:
		{
		}
		break;

	/*!
	\brief
		Level up a skill.
	
	\param 
		Skill ID(unsigned int)
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	case GOI_LEVELUP_SKILL:
		{
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			
			Npc[nNpcIdx].LevelUpSkill(uParam, 1);
		}
		break;
	case GOI_SET_L_SKILL:
		{
			s_LSkill = uParam;
		}
		break;
	case GOI_SET_R_SKILL:
		{
			s_RSkill = uParam;
		}
		break;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//增强一些属性的值，一次加一点
	//uParam = 表示要增强的是哪个属性，取值为UI_PLAYER_ATTRIBUTE的梅举值之一
	case GOI_TONE_UP_ATTRIBUTE:
// 		switch (uParam)
// 		{
// 		case UIPA_STRENGTH:		//力量
// 			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, 1);
// 			break;
// 		case UIPA_DEXTERITY:	//敏捷
// 			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, 1);
// 			break;		
// 		case UIPA_VITALITY:		//活力
// 			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, 1);
// 			break;
// 		case UIPA_ENERGY:		//精力
// 			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, 1);
// 			break;		
// 		}
		break;

	//答应/拒绝交易请求
	case GOI_TRADE_SEND_INVITE_RESPONSE:
		{
			bool accept = ( uParam != 0 );
			if(accept)
				Player[CLIENT_PLAYER_INDEX].tradeClientSendAccept(nParam);
			else
				Player[CLIENT_PLAYER_INDEX].tradeClientSendRefuse(nParam);
		}
		break;
	case GOI_TRADE_CHANGE_MONEY:
		{
			int money = (int)uParam;
			Player[CLIENT_PLAYER_INDEX].tradeClientMoveMoney(money);
		}
		break;

	//有无交易意向
	//nParam = bWilling
	case GOI_TRADE_WILLING:
		{
		}
		break;

	//锁定交易
	//nParam = (int)(book)bLock 是否锁定
	case GOI_TRADE_LOCK:
		{
			Player[CLIENT_PLAYER_INDEX].tradeClientSendLock();
		}
		break;

	//交易
	case GOI_TRADE:
		{
			Player[CLIENT_PLAYER_INDEX].tradeClientSendEndTrade();
		}
		break;

	//交易取消
	case GOI_TRADE_CANCEL:
		{
			Player[CLIENT_PLAYER_INDEX].tradeClientSendTradeCancel();
		}
		break;

	//查询是否可以丢某个东西到游戏窗口
	//uParam = (KGameObject*)pObject -> 物品信息
	//nParam = 被拖动东西的当前坐标（绝对坐标），横坐标在低16位，纵坐标在高16位。(像素点坐标)
	//Return = 是否可以放下
	case GOI_DROP_ITEM_QUERY:
		//to do : waiting for...
		break;

	case GOI_OPTION_SETTING:			//选项设置
		if (uParam == OPTION_MUSIC_VALUE)
		{
			Option.SetMusicVolume(nParam);
		}
		else if (uParam == OPTION_SOUND_VALUE)
		{
			Option.SetSndVolume(nParam);
		}
		else if (uParam == OPTION_BRIGHTNESS)
		{
			Option.SetGamma(nParam);
		}
		else if (uParam == OPTION_MAXPLAYERINSCREEN)
		{
			Option.SetMaxPlayersInScreen(nParam);
		}
		else if (uParam == 	OPTION_SHOW_PLAYER)
		{
			Option.SetIfDrawPlayer(nParam);
		}
		else if (uParam == 	OPTION_SHOW_NPC )
		{
			Option.SetIfDrawNpc(nParam);
		}
		else if (uParam == OPTION_SHOW_SHADOW)
		{
			Option.SetDrawShadow(nParam);	
		}
		else if (uParam == OPTION_DRAW_GROUND)
		{
			Option.SetDrawGround(nParam);
		}
		else if (uParam == OPTION_DRAW_SMALLOBJ)
		{
			Option.SetDrawSmallObj(nParam);
		}
		else if (uParam == OPTION_DRAW_LARGEOBJ)
		{
			Option.SetDrawLargeObj(nParam);
		}
		else if ( uParam == OPTION_FONT_SHADOW )
		{
			g_SetFontBorder(nParam);
		}
		break;
	case GOI_VIEW_PLAYERITEM:
		{
			g_cViewItem.ApplyViewEquip(uParam);
		}
		break;
	case GOI_VIEW_PLAYERITEM_END:
		g_cViewItem.DeleteAll();
		break;

	case GOI_PLAYER_ACTION:
		{
			switch(uParam)
			{
			case PA_RUN:
				Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
				break;
// 			case PA_SIT:
// 				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
// 				{
// 					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
// 					SendClientCmdSit(TRUE);
// 				}
// 				else
// 				{
// 					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
// 					SendClientCmdSit(FALSE);
// 				}
// 				break;
			}
		}
		break;
	case GOI_PK_SETTING:
		{
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			Npc[nNpcIdx].ChangePKMode((PK_MODE)uParam);
		}

		break;
	case GOI_FOLLOW_SOMEONE:
		if ( uParam > 0)
		{
			int npcIndex = NpcSet.SearchID(uParam);
			if (Npc[npcIndex].m_Kind == kind_player)
			{
				PlayerController::Singleton().FollowNpc(npcIndex);
				//Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( true );
				//Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetTarget(type_npc, npcIndex);
			}
		}
		break;

	//显示各玩家人名
	//nParam = (int)(bool)bShow	是否显示
	case GOI_SHOW_PLAYERS_NAME:
		NpcSet.SetShowNameFlag(nParam);
		break;
	//显示各玩家生命
	//nParam = (int)(bool)bShow	是否显示
	case GOI_SHOW_PLAYERS_LIFE:
		NpcSet.SetShowLifeFlag(nParam);
		break;
	//显示各玩家内力
	//nParam = (int)(bool)bShow	是否显示
	case GOI_SHOW_PLAYERS_MANA:
		NpcSet.SetShowManaFlag(nParam);
		break;
	case GOI_SHOW_PLAYERS_BODY:
		NpcSet.setShowPlayer(nParam);
		break;
	//切换上下马的状态
	case GOI_SWITCH_HORSE:
 		{
// 			//Lucifer~yu[zhangjianyu] [02/08/2006] Add for 
// 			//begin------------------------------------------------------------------------
// 			if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse) > 0 && !Player[CLIENT_PLAYER_INDEX].m_bIsPreRide )
// 			//end--------------------------------------------------------------------------				
// 			{
//				if(Player[CLIENT_PLAYER_INDEX].GetCanSwitchHorseTime() > 0)
//				{
//					KSystemMessage	sMsg;
//					sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE, ((KUiPlayerItem*)uParam)->Name);
//					sMsg.eType = SMT_NORMAL;
//					sMsg.byConfirmType = SMCT_NONE;
//					sMsg.byPriority = 0;
//					sMsg.byParamSize = 0;
//					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
//					break;
//				}
//				if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit ||
//				   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_jump ||
//				   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsCommandExist(do_sit) ||
//				   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].IsCommandExist(do_jump))
//				{
//					KSystemMessage	sMsg;
//					sprintf(sMsg.szMessage, MSG_CANT_SWITCH_HORSE_WHEN_SIT, ((KUiPlayerItem*)uParam)->Name);
//					sMsg.eType = SMT_NORMAL;
//					sMsg.byConfirmType = SMCT_NONE;
//					sMsg.byPriority = 0;
//					sMsg.byParamSize = 0;
//					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
//					break;
//				}
// 				// 
// 				if (uParam == 0) // 如果是上马,确定要先站起来 lixuewu 2004.01.06
// 				{
// 					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
// //					SendClientCmdSit(FALSE);
// 				}
// 				//Lucifer~yu[zhangjianyu] [02/08/2006] Add for 
// 				//begin------------------------------------------------------------------------
// 				if ( !uParam )
// 				{
// 					Player[CLIENT_PLAYER_INDEX].m_bIsPreRide = true;
// 				}
// 				//end--------------------------------------------------------------------------	
// 				ITEM_MASK cHorse;
// 				memset(&cHorse, 0, sizeof(ITEM_MASK));
// 				cHorse.ProtocolType = c2s_itemmask;
// 				cHorse.cEquip[itempart_horse] = uParam;
// 				g_pClient->SendPackToServer(g_ConnectID,&cHorse, sizeof(ITEM_MASK));
// 			}
		    break;
		}
	case GOI_SHOW_GAMESPACE_ITEM_NAME:
		ObjSet.SetShowNameFlag(nParam > 0);
		break;

// 	case GOI_GIVE_SOMEONE_MONEY:
// 		if(g_pClient)
// 		{
// 			int nIndex = NpcSet.SearchID(uParam);
// 			if(nIndex)
// 			{
// 				GIVE_SOMETHING cGive;
// 				cGive.ProtocolType = c2s_give;
// 				cGive.dwItemID[0]  = nParam;
// 				cGive.dwTarID      = uParam;
// 				cGive.byGiveType   = Obj_Kind_Money;
// 				int i = sizeof(GIVE_SOMETHING);
// 				g_pClient->SendPackToServer(g_ConnectID,&cGive, sizeof(GIVE_SOMETHING));
// 			}
// 		}
// 		break;

	/************************************************************************/
	/*						Immediacy room                                  */
	/************************************************************************/
	/*!
	\brief
		Add immediacy room.

	\param uParam	
		
	\return
		
	*/	
	case GOI_ADD_IMMEDIACY:
		{
			KImmediacyParam* pImm = (KImmediacyParam*)uParam;
			if ( pImm )
			{
				_ShortCut_Add	SAdd;
				SAdd.Protocol			=	c2s_byte_extend;
				SAdd.ProtocolExtend		=	c2s_ex_protocol_shortcut_add;
				SAdd.wProtocolSize		=	sizeof( _ShortCut_Add ) - 1;
				if ( nParam > 0 )
				{
					int nGenre = 0;
					int nDetail = 0;
					int nParticular = 0;
					SpliteHashId( pImm->nID, nGenre, nDetail, nParticular );
					int nItemIndex = 0;
					Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSameParticularItem( nGenre, nDetail, nParticular, &nItemIndex );
					if ( nItemIndex > 0 && nItemIndex < MAX_ITEM )
					{
						SAdd.dwID			=	Item[nItemIndex].GetID();
					}										
				}
				else
				{
					SAdd.dwID			=	pImm->nID;
				}
				SAdd.dwPos				=	pImm->nPos;
				SAdd.dwSCType			=	pImm->nImmediacyType;
				g_pClient->SendPackToServer(g_ConnectID,&SAdd, sizeof(_ShortCut_Add));
			}
		}
		break;

	/*!
	\brief
		Del immediacy room.

	\param uParam	
		
	\return
		
	*/	
	case GOI_DEL_IMMEDIACY:
		{
			if ( uParam >= 0 && uParam < MAX_IMMEDIACY_ITEM )
			{
				_ShortCut_Del	SDel;
				SDel.Protocol			=	c2s_byte_extend;
				SDel.ProtocolExtend		=	c2s_ex_protocol_shortcut_del;
				SDel.wProtocolSize		=	sizeof( _ShortCut_Del ) - 1;
				SDel.dwPos				=	uParam;
				g_pClient->SendPackToServer(g_ConnectID, &SDel, sizeof(_ShortCut_Del));
			}
		}
		break;
	/************************************************************************/
	/*								Item system                             */
	/************************************************************************/
	/*!
	\brief
		告诉Core界面上的宝石合成程序完成了，进行重置
	
	\return
		Nothing.	
	*/
	case GOI_COMPOUND_BEGIN:
		{			
			KUiCompoundParam* pInfo = (KUiCompoundParam*)uParam;
			ENCHASER_CLIENTSEND EnchaserCmd;
			memset(&EnchaserCmd, 0, sizeof(ENCHASER_CLIENTSEND));
			//协议号
			EnchaserCmd.ProtocolType	= c2s_enchaseritem;
			//合成类型
			EnchaserCmd.nCompoundType	= pInfo->nCompoundType;
			//合成规则id
			EnchaserCmd.nRuleId = pInfo->ruleId;
			//合成玩家个性化注释
			memcpy( EnchaserCmd.szPlusInfo, pInfo->szPlusInfo, COMMON_CLIENT_MSG_LEN_64 );
			EnchaserCmd.szPlusInfo[COMMON_CLIENT_MSG_LEN_64-1] = 0;
			
			//合成原材料清单
			for(int i = 0; i < MAX_LEVELUP_ITEMS_COUNT; ++i)
			{
				if (pInfo->nItemIndex[i] > 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_ItemList.GetItemPos(pInfo->nItemIndex[i], (ItemPos*)&(EnchaserCmd.Part[i]));
				}
			}
			g_pClient->SendPackToServer(g_ConnectID,&EnchaserCmd, sizeof(ENCHASER_CLIENTSEND));
			nRet = 1;
		}
		break;
	case GOI_DO_SMITH:
		{
// 			SMITH_C2S sendData;
// 			memset(&sendData, 0, sizeof(SMITH_C2S));
// 			sendData.protocolType = c2s_smith_request;
// 			sendData.ruleId = (int)uParam;
// 
// 			g_pClient->SendPackToServer(g_ConnectID,&sendData, sizeof(SMITH_C2S));
		}
		break;

	case GOI_USE_ITEM_DATE_LOCK:
		{
			if ( uParam > 0 && uParam < MAX_ITEM )
			{
				PLAYER_EAT_ITEM_COMMAND	sEat;
				sEat.ProtocolType = c2s_playereatitem;
				sEat.m_nItemID	= 0xff;
				sEat.m_btPlace	= 0xff;
				sEat.m_btX		= 0xff;
				sEat.m_btY		= 0xff;
				
				sEat.m_nTargetItemID = Item[uParam].GetID();
				sEat.m_btTargetPlace = 0xff;
				sEat.m_btTargetX	 = 0xff;
				sEat.m_btTargetY	 = 0xff;
				if (g_pClient)
					g_pClient->SendPackToServer(g_ConnectID,&sEat, sizeof(PLAYER_EAT_ITEM_COMMAND));

			}
		}
		break;
	/*!
	\brief
		使用物品
	
	\param 
		uParam = (KObjAtContRegion*)pInfo -> 物品的数据以及物品原来摆放的位置

	\param 
		nParam = 物品使用前放置的位置，取值为枚举类型UIOBJECT_CONTAINER。
	
	\param 
		Skill Param(int)

	\return
		Nothing.	
	*/
	case GOI_USE_ITEM:
		{
			nRet = 0;
			if (uParam && nParam == NULL )
			{
				KItemList *pKItemList = &(Player[CLIENT_PLAYER_INDEX].m_ItemList);
				KObjAtContRegion* pInfo = (KObjAtContRegion*) uParam;
							
				ItemPos	Pos;
				if ( pKItemList )
				{
					BOOL getPosResult = pKItemList->GetItemPos(pInfo->Obj.uId, &Pos );
					//caolei+ 修复右键点击物品后再点击背包空格栏会提示是否使用物品的bug
					if ( FALSE == getPosResult )
					{
						return nRet;
					}
					
					KItem*	pItem = pKItemList->GetItemFromPlace(&Pos);
					if ( pItem )
					{
// 						if ( pItem->GetItemCount() )
// 						{
// 							ItemPos p;
// 							Player[CLIENT_PLAYER_INDEX].m_ItemList.GetItemPos( nIdx, &p);
// 							pInfo->Obj.uGenre = CGOG_ITEM;
// 							pInfo->Obj.uId = nIdx;
// 						}
						if ( (pItem->GetGenre() == item_target ||
							(pItem->GetGenre()  == item_ib && !pItem->IsNoTarget() ) ) && 
							pItem->IsItemTarget() )
						{
							g_UseItem.uId = pInfo->Obj.uId;
							g_UseItem.nPlace = pos_equiproom;
							g_UseItem.nX = Pos.nX;
							g_UseItem.nY = Pos.nY;	
							CoreDataChanged( GDCNI_SET_MOUSECURSOR, MOUSE_CURSOR_TARGETITEM, NULL );
						}
						else
						{
							if ( pInfo->Obj.uGenre == CGOG_ITEM && pInfo->Obj.uId > 0 && Pos.nPlace != -1 && g_UseItem.uId == 0 )
							{
								int nTargetIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();
								DWORD dwTargetID = Npc[nTargetIdx].GetId();
								ItemPos	tagPlayerTargetPos;
								ZeroMemory( &tagPlayerTargetPos, sizeof(ItemPos) );
								nRet = Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId, Pos, dwTargetID, tagPlayerTargetPos);
							}
							else
							{
								g_uItemID = pInfo->Obj.uId;
								memcpy( &g_uItemPos, &Pos, sizeof(g_uItemPos) );
								CoreDataChanged( GDCNI_MAKESURE_USEITEM, (unsigned int)MAKESURE, NULL );
							}
							//if ( false == bRet )
							//	CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Item_Unusable);
						}
					}
				}
			}
			if (uParam == NULL && nParam )
			{
				FIND_ITEMINDEX_PARAM* pItemIdx = (FIND_ITEMINDEX_PARAM*)nParam;
				if ( pItemIdx )
				{
					int nIdx	= 0;
					int nX		= 0;
					int nY		= 0;

					if ( Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSameParticularInEquipment( 
						pItemIdx->nGenre, pItemIdx->nDetail, pItemIdx->nParticular,
						&nIdx, &nX, &nY ) )
					{
						ItemPos tmpPos, tmpTargetPos;
						tmpPos.nPlace = pos_equiproom;
						tmpPos.nX = nX;
						tmpPos.nY = nY;
						ZeroMemory(&tmpTargetPos, sizeof(ItemPos) );
						nRet = Player[CLIENT_PLAYER_INDEX].ApplyUseItem(nIdx, tmpPos, 0, tmpTargetPos);
						//if ( false == Player[CLIENT_PLAYER_INDEX].ApplyUseItem(nIdx, tmpPos, 0, tmpTargetPos) )
						//	CoreDataChanged(GDCNI_ERROR_MESSAGE_CODE, common_message, CE_Item_Unusable);
					}
				}
			}
		}
		break;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//游戏程序切换到前台/后台
	//uParam = (int)bool bActive
	case GOI_GAME_APP_ACTIVE:
		{
//			if (uParam)
//				g_SubWorldSet.m_cMusic.AdjustVolume();
		}
		break;

	case GOI_SOCIETY_INVITE_RESPONSE:
		{
			Player[CLIENT_PLAYER_INDEX].m_clientSUMgr.ReqConfirmRetFromUI(nParam);			
		}
		break;

		////////////////////
	case GOI_FORCE_DELETE_NPC_HEADINFO:
		{
			NpcSet.ForceRemoveAllNpcHeadInfo();
		}
		break;
	case GOI_SOCIETY_SET_CITY_TEX_RATE:
		{
            Player[CLIENT_PLAYER_INDEX].m_clientSUMgr.SetCityTaxRateNormal(uParam);
		}
		break;
		
	case GOI_SET_PEOPLE_INDEX_BY_PLAYER:
		if (uParam == -1)
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, nParam);
		else
			Npc[Player[uParam].m_nIndex].SetTarget(type_npc, nParam);
		break;
	
	//----->Add by [Ray] 2004-7-22
	case GOI_GENERAL_INPUT_RESULT:
		//uParam = char * 指向输入框中的内容
		//当uParam == NULL时,表示通用输入框按下了取消按钮
		//nParam = nCallType
		if ( nParam == enMerchant )		
		{
            PLAYER_SELECTUI_COMMAND cmd;
			cmd.wLength = sizeof(PLAYER_SELECTUI_COMMAND) - sizeof(DWORD) - 1;
			cmd.ProtocolType = (BYTE)c2s_playerselui;
			cmd.dwSeed = Player[CLIENT_PLAYER_INDEX].m_dwWaitingPlayerFeedBackSeed;
			if ( uParam ) 
			{
				cmd.nSelectIndex = 0;								// 表示确认
				cmd.nSelectType = select_inputdialog;				// 表示输入数字
				cmd.nParam1 = atoi((char*)uParam); // 输入的数字
				cmd.nParam2 = enMerchant;
				g_pClient->SendPackToServer(g_ConnectID,&cmd, cmd.wLength + 1);
			}
			else
			{
				//按下的是取消按钮
				cmd.nSelectIndex = 1;					// 表示取消
				cmd.nSelectType = select_inputdialog;	
				cmd.nParam1 = 0; 
				cmd.nParam2 = enMerchant;
				g_pClient->SendPackToServer(g_ConnectID,&cmd, cmd.wLength + 1);
			}
			//取值完成后,通用输入框会被关闭,其值不再可用
			//关闭由输入窗口自己负责
		}
		break;
	//<-----Add End
//Add By [Ray]  2004-8-11
	case GOI_TEAM_AUTO_FOLLOW:
		// nParam = 0, 1 表示是否按下自动跟随按钮
		break;

	case GOI_STOREBOX_ENTER_PASSWORD:				//储物箱输入密码
		//uParam = (char*)szPassword
		//nParam = 密码长度
		{
			char szPassword[32];
			int nLen = strlen((const char*)uParam);
			if ( nLen == nParam )  
			{
				strcpy(szPassword, (const char*)uParam);
				//发送szPassword给服务器
				Player[CLIENT_PLAYER_INDEX].CheckStoragePSW(szPassword);
			}
			break;
		}
	case GOI_STOREBOX_CREATE_PASSWORD:				//储物箱创建密码
		{
			char szPassword[32];
			int nLen = strlen((const char*)uParam);
			if ( nLen == nParam )  
			{
				strcpy(szPassword, (const char*)uParam);
				//发送创建好的密码给服务器
				Player[CLIENT_PLAYER_INDEX].CreateStoragePSW(szPassword);
			}
			break;
		}
	case GOI_STOREBOX_CHANGE_PASSWORD:				//储物箱修改密码
		{
			char szOldPassword[32], szNewPassword[32];
			strcpy(szOldPassword, (const char*)uParam);
			strcpy(szNewPassword, (const char*)nParam);
			//发送原来的密码和修改后的密码给服务器
			Player[CLIENT_PLAYER_INDEX].ModifyStoragePSW(szNewPassword, szOldPassword);
		}
		break;
	case GOI_STOREBOX_CLEAN_PASSWORD:	// 清除储物箱的密码
	//uParam = char szPass[64]		二级密码,经过MD5编码
	//
//		Player[CLIENT_PLAYER_INDEX].ReplaceStoragePSW((LPCSTR)uParam);
		break;
	//用来关闭储物箱，现在只是锁定，但界面上并不关闭
	case GOI_LOCK_STOREBOX:
		{
			Player[CLIENT_PLAYER_INDEX].SendCloseStorageCMD();
		}
		break;
//<------- End [Ray]
//-------> Ray [Luoliang] 2004-8-12
	case GOI_ITEM_LIST_OPERATION_LOCK:
		Player[CLIENT_PLAYER_INDEX].m_ItemList.LockOperation();
		break;

	case GOI_ITEM_LIST_OPERATION_UNLOCK:
		Player[CLIENT_PLAYER_INDEX].m_ItemList.UnlockOperation();
		break;
//<------- End [Ray]
	
	//-->Rocker 2004/11/04 玩家设置自己的阵营
	case GOI_SET_PLAYER_CAMP:
	//nParam = (NPCCAMP)enumCamp, 玩家想变的阵营
	//下面两个无效
	//camp_animal,			// 野兽阵营
	//camp_event,				// 路人阵营
		{
			// 发送协议到服务器告诉玩家想变阵营
		}
		break;
	//<--Rocker 	
	//-------> Ray [Luoliang] 2004-11-17
	case GOI_RANK_OPERATE_ID:
		break;
	//<------- End [Ray]
	case GOI_SET_SECENE_FOCUSOFFSET:
		{
			int nX = 0; int nY = 0; int nZ = 0;
			g_ScenePlace.GetFocusPosition(nX, nY, nZ);
			g_ScenePlace.SetFocusOffSet(uParam, nParam);
			g_ScenePlace.SetFocusPosition(nX, nY, nZ);
		}
		break;
	case GOI_SYSCHANNEL_MSG:			// 发送系统消息(显示在聊天框内
	//uParam = (KUiSysChannelMsg*)pMsg
	//nParma = 
//		CoreDataChanged(GDCNI_SYSCHANNEL_MSG, uParam, nParam);
		break;
	//-------> Ray [Luoliang] 2005-3-10
	case GOI_ENABLE_TO_THROW_AWAY_ITEM:	//是否允许将东西扔在地上
	//uParam = (BOOL)bEnable
	//nParam = 
		Player[CLIENT_PLAYER_INDEX].EnableToThrowAwayItem(uParam);
		break;
	//<------- End [Ray]		
	// Add by Cooler -->
	// 2005-3-20
	case GOI_IS_MALE:
		nRet = (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex == 0);
		break;
	case GOI_IS_MASTERLEVEL:
		nRet = (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level >= 50);
		break;
	// End add by Cooler <--
	//-------> Ray [Luoliang] 2005-3-26
	case GOI_ADD_ITEM:					//增加一个物品到指定位置
	//uParam = (KObjAtContRegion *)
	//nParam =
		{
			KObjAtContRegion *pObj = reinterpret_cast<KObjAtContRegion*>(uParam);
			bool	bAdd;
			int		nPos;
			switch ( pObj->nContainer )
			{
			case UOC_ITEM_TAKE_WITH:
				nPos = pos_equiproom;
				bAdd = true;
				break;			
			case UOC_EQUIPTMENT:
				nPos = pos_equip;
				bAdd = true;
				break;
			case UOC_STORE_BOX:
				nPos = pos_repositoryroom;
				bAdd = true;
				break;	
			default:
				bAdd = false;
				break;			
			}
			if ( bAdd )
			{
				nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.Add(pObj->Obj.uId, nPos, pObj->Region.h, pObj->Region.v);
			}

		}
		break;
	//-------> Ray [Luoliang] 2005-7-7
	case GOI_MODIFY_PET_NAME:					//给宠物改名字
	//uParam = (const char*)szName
	//nparam = nNameLen
		{				
			PET_C2S_CHANGE_NAME name;
			int nPackSize = sizeof(PET_C2S_CHANGE_NAME) - sizeof(name.szPetName) + nParam;
			name.SetProtocolHeader(pet_c2s_change_name, nPackSize - 1);
			memcpy(name.szPetName, reinterpret_cast<const char*>(uParam), nParam);
			g_pClient->SendPackToServer(g_ConnectID,&name, nPackSize);
		}
		break;

	case GOI_OPEN_PET_PANEL:			//打开宠物面板
	//uParam =
	//nParam =
		{
			PET_HEADER cmd;
			cmd.SetProtocolHeader(pet_c2s_open_panel, sizeof(cmd) - 1);
			g_pClient->SendPackToServer(g_ConnectID,&cmd, sizeof(cmd));
		}
		break;
	//<------- End [Ray]

	case GOI_NOTIFY_SCREEN_RESIZE:
		{
			g_nScreenHeight = nParam;
			g_nScreenWidth = uParam;
		}
		break;
	case GOI_GOTO_POS:
        {			
#ifdef	_AUTO_ROBOT
			Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( false );
			AutoRobotMgr::Singleton().AutoRunTo(uParam, nParam);
#endif
            break;
        }
	case GOI_SET_AUTO_DIALOG_NPC:
        {
#ifdef	_AUTO_ROBOT
			int npcTemplateId = (int)uParam;
			AutoDialogNpc::getSingleton().setTargetNpc(npcTemplateId);
#endif
            break;
        }

	case GOI_GOTO_MAILCENTRE:
        {			
#ifdef	_AUTO_ROBOT
			//自动寻路到本地的邮件npc
			Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( false );
			
			int nSceneId = g_ScenePlace.GetID();
			if ( uParam < MAX_NPCSTYLE )
			{
				NpcMapPos retNpc;
				
				KNpcTemplate mailNpc;
				mailNpc.InitNpcBaseData(uParam);

				if ( NULL != mailNpc.m_nMapPos[0] )
				{
					int ret = sscanf(mailNpc.m_nMapPos[0], "gt=pos id=%d id1=%d id2=%d", &retNpc.mapId, &retNpc.x, &retNpc.y);
					if ( 3 == ret )
					{
						AutoRobotMgr::Singleton().AutoRunTo(retNpc.x, retNpc.y * 2);
					}
				}
			}
#endif
            break;
        }
    // endadd

/********************************************************************************
 *         Taisui  Wheel Sys  Add by brianyao
 ********************************************************************************/
	case GOI_WHEEL_TIAN_GAN:
		{
            KPlayer & player=GetClientPlayer();
			player.GetTaisuiWheelSys()->WheelTianGan();
		} 
		break;
		
	case GOI_WHEEL_DI_ZHI:
		{
			KPlayer & player=GetClientPlayer();
			player.GetTaisuiWheelSys()->WheelDizhi();
		}
		break;

	case GOI_DROP_CHANCE:
		{
			KPlayer & player=GetClientPlayer();
			player.GetTaisuiWheelSys()->DropChance();
		}
		break;

	case GOI_TAISUI_SHOW_RES:
		{
			KPlayer & player=GetClientPlayer();
			player.GetTaisuiWheelSys()->RequestGiftRes();
		}
		break;
		
	case GOI_AUTO_PICKUP:
		{
			AutoRobotMgr::Singleton().AutoPickObj();
//			AutoRobotMgr::Singleton().canNotPickIndex.clear();
		}
		break;


	case GOI_PLAY_EFFECT:
		{
			ScreenEffectMgr::Singleton().Player(uParam,nParam);
		}
		break;

    case GOI_SET_EFFECT_POS:
		{
			unsigned long   dwID=uParam;
			int           * pAdditionParam=(int *)nParam;
			int             nX=0;
			int             nY=0;
			
			if (pAdditionParam)
			{
                nX=*pAdditionParam;
				nY=*(pAdditionParam+1);
			}//endif

            ScreenEffectMgr::Singleton().SetEffectPos(dwID,nX,nY);
		}
		break;
		
	/*****************************************************************
     *                 IBShopSystem		                             *
	 ******************************************************************/
	case GOI_IBSHOP_CHONGZHI:
		{
			IBCenter_C::Singleton().ChongZhi();
		}
		break;
	case GOI_IBSHOP_LOAD_SHELF_CATE:
		{
			IBCenter_C::Singleton().LoadShelfReq((int)uParam);
			IBCenter_C::Singleton().LoadPanelReq();
			IBCenter_C::Singleton().LoadContentStyleReq();
		}
		break;
	case GOI_IBSHOP_CLEAR_ALL:
		{
			IBCenter_C::Singleton().Init();
		}
		break;
	case GOI_IBSHOP_LOAD_SHELF:
		{
			IBCenter_C::Singleton().LoadGoodsInShelfReq((int)nParam, (int)uParam);
		}
		break;
	case GOI_IBSHOP_BUY:
		{
			ClientBuyGoods* ibgoods = (ClientBuyGoods*)uParam;
			if ( ibgoods )
			{
				if (ibgoods->goods.ID == TRADE_ID_RETURN_CREDIT)
				{
					static ConfigManager &cfg = ConfigManager::Singleton();
					cfg.GetIBReturnId(&ibgoods->goods.Genera,
						&ibgoods->goods.Detail, &ibgoods->goods.Particular, &ibgoods->goods.Level);
				}

				nRet = IBCenter_C::Singleton().BuyGoodsReq(*ibgoods);
			}
		}
		break;
	/*case GOI_CREDITSHOP_BUY:
		{
			IBGoods_Id* ibgoods = (IBGoods_Id*)uParam;
			IBCenter_C::Singleton().BuyGoodsReq(*ibgoods, enCREDIT_SHOP, (int)nParam);
		}
		break;
	case GOI_POINTSHOP_BUY:
		{
			IBGoods_Id* ibgoods = (IBGoods_Id*)uParam;
			IBCenter_C::Singleton().BuyGoodsReq(*ibgoods, enPRESENT_SHOP, (int)nParam);
		}
		break;//*/
	case GOI_LIST_TEAM:
		{
			REQUEST_TEAM_LIST requestTeamList;
			requestTeamList.Protocol = c2s_list_team;
			requestTeamList.Filter = 0;
			requestTeamList.Limit = 5;
			requestTeamList.ListMode = 1;
			requestTeamList.FilterParam = SubWorld[0].GetWorldTemplateId();

			if (g_pClient)
			{
				g_pClient->SendPackToServer(g_ConnectID, &requestTeamList, sizeof(requestTeamList));
			}
		}
		break;

	case GOI_SOCIAL_OWNER_INFO:
		{
			TongInfoRequestParam * pParam = (TongInfoRequestParam *)uParam;
			if (!pParam)
				return 0;

			if (pParam->nLayer >= enSULayer_Player && pParam->nLayer <= enSULayer_League)
			{
				if (pParam->nLayer == enSULayer_Player)
				{
					FindParam tagParam;
					memset(&tagParam, 0, sizeof(tagParam));
					tagParam.ProtocolType = c2s_find_family;
					strncpy( tagParam.szName, pParam->szOwnerName, 17 );
					if ( g_pClient )
					{
						g_pClient->SendPackToServer(g_ConnectID, &tagParam, sizeof(tagParam) );
					}//endif
					
				}//endif
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_clientSUMgr.ReqUnitInfo(pParam->szOwnerName,pParam->nLayer,pParam->code);
				}//end else

			}//endif

		}
		break;
	case GOI_MINI_MAP_DRAW_ON_DC:
		{
			ChatPoint* point = (ChatPoint*)uParam;
			if(point)
			{
				g_ScenePlace.PaintMiniMapOnDC(*point);
			}
		}
		break;
	case GOI_SPECIAL_QUEST_DATA_RQ:
		{
			int questId = (int)uParam;

			REQ_SPECIAL_QUEST_DATA param;
			memset(&param, 0, sizeof(param));
			param.Protocol = c2s_req_special_quest_data;
			param.QuestId = questId;
			
			if(g_pClient)
			{
				g_pClient->SendPackToServer(g_ConnectID, &param, sizeof(param));
			}
		}
		break;
	case GOI_SEND_GM_QUESTION:
		{
			static DWORD timeCount = 0;
			if(GetTickCount() - timeCount < 1000 * 2 * 60)
			{
				nRet = 0;
				break;
			}

			timeCount = GetTickCount();
			char* msg = (char*)uParam;
			if(msg && g_pClient)
			{
				GM_COMMUNICATION_DATA GMCData;
				GMCData.Protocol = c2s_gm_communication;
				GMCData.data.type = GMCT_COMMON_TYPE;
				int msgLen = strlen(msg);
				if(msgLen < sizeof(GMCData.data.msg))
				{
					strcpy(GMCData.data.msg, msg);
				}
				else
				{
					strncpy(GMCData.data.msg, msg, sizeof(GMCData.data.msg));
				}
				GMCData.data.msg[sizeof(GMCData.data.msg) - 1] = 0;
				
				g_pClient->SendPackToServer(g_ConnectID, &GMCData, sizeof(GMCData));
			}
		}
		break;	
	case GOI_HIRE_SEND_DATA_REQ:
		{
			static DWORD timeCount = 0;
			if(GetTickCount() - timeCount < 2000)
			{
				nRet = 0;
				break;
			}

			timeCount = GetTickCount();
			
			HireReqData* data= (HireReqData*)uParam;
			if(data)
			{
				REQ_HIRE_LIST_DATA req;
				req.Protocol = c2s_hire_req_list;
				req.filter = *data;
				
				g_pClient->SendPackToServer(g_ConnectID, &req, sizeof(req));
				nRet = 1;
			}
		}
		break;
	case GOI_HIRE_SEND_HIRE_REQ:
		{
			static DWORD timeCount = 0;
			if(GetTickCount() - timeCount < 2000)
			{
				nRet = 0;
				break;
			}

			timeCount = GetTickCount();
			
			char* name= (char*)uParam;
			if(name)
			{
				REQ_HIRE_DATA req;
				req.Protocol = c2s_hire_req_hire;
				strncpy(req.Name, name, sizeof(req.Name));
				req.Name[sizeof(req.Name) - 1] = 0;

				g_pClient->SendPackToServer(g_ConnectID, &req, sizeof(req));
			}
		}
		break;
	case GOI_HIRE_SEND_WANT_TO_BE_HIRED_REQ:
		{
			int hireType = (int)uParam;
			int moneyOrExpType = nParam;

			REQ_TO_BE_HIRED_DATA req;
			req.Protocol = c2s_hire_req_tobe_hired;
			req.hireType = hireType;
			req.moneyOrExpType = moneyOrExpType;

			g_pClient->SendPackToServer(g_ConnectID, &req, sizeof(req));
		}
		break;
	case GOI_STUDENT_REPORT:
		{
			RECOMMENDER_OP req;
			req.Protocol = c2s_recommender;
			req.OpType = recommender_op_update_student;
			g_pClient->SendPackToServer(g_ConnectID, &req, sizeof(req));
		}
		break;
	case GOI_GET_REWARD:
		{
			RECOMMENDER_OP req;
			req.Protocol = c2s_recommender;
			req.OpType = recommender_op_get_master_reward;
			g_pClient->SendPackToServer(g_ConnectID, &req, sizeof(req));
		}
		break;

	case GOI_GET_SELF_COMBAT_ORG_NAME:
		{
			nRet = 0;
			char * pBuff = (char *)uParam;;
			if (pBuff)
			{
				int nNpcIndex  = GetClientPlayer().GetNpcIndex();
				int nCombatOrg = Npc[nNpcIndex].m_WorldCombatOrg;
				if (IsValidCombatID(nCombatOrg) && Npc[nNpcIndex].IsInWorldCombatInstance())
				{
					sprintf(pBuff,SubWorld[0].m_WorldCombatClientInfo[nCombatOrg - 1].szOrgName);
					nRet = 1;
				}//endif

			}//endif
		}
		break;

	case GOI_ANSWER_QUESTION:
		{
			nRet = 0;
			void* pAnswer = (void*)uParam;//问题答案
			int answerSize = nParam;
			if (pAnswer && g_pClient)
			{
				char sendBuff[sizeof(ANSWER) + MAX_ANSWER_SIZE];
				memset(sendBuff, 0, sizeof(sendBuff));
				ANSWER* pAnswerProcotol = (ANSWER*)sendBuff;				
				pAnswerProcotol->Protocol = c2s_byte_extend;
				pAnswerProcotol->ProtocolExtend = c2s_ex_protocol_question;
				pAnswerProcotol->wProtocolSize = sizeof(ANSWER) - 1 - sizeof(pAnswerProcotol->Answer) + answerSize;
				memcpy(pAnswerProcotol->Answer, pAnswer, answerSize);

				nRet = g_pClient->SendPackToServer(g_ConnectID, pAnswerProcotol, pAnswerProcotol->wProtocolSize + 1);
			}
		}
		break;
	case GOI_KICK_PLAYER:
		{
			const char* szPlayerName = (const char*)uParam;
			DWORD noChatTime = nParam;
			
			nRet = Player[CLIENT_PLAYER_INDEX].SendGMOperation(szPlayerName, gm_op_kick);
		}
		break;
	case GOI_JINYAN_PLAYER:
		{
			const char* szPlayerName = (const char*)uParam;
			DWORD noChatTime = nParam;
			
			nRet = Player[CLIENT_PLAYER_INDEX].SendGMOperation(szPlayerName, gm_op_no_chat, noChatTime);
		}
		break;
	case GOI_DONGJIE_PLAYER:
		{
			const char* szPlayerName = (const char*)uParam;
			DWORD noLoginTime = nParam;
			
			nRet = Player[CLIENT_PLAYER_INDEX].SendGMOperation(szPlayerName, gm_op_no_login, noLoginTime);
		}
		break;
	case GOI_DONGJIEACCOUNT_PLAYER:
		{
			const char* szPlayerName = (const char*)uParam;
			
			nRet = Player[CLIENT_PLAYER_INDEX].SendGMOperation(szPlayerName, gm_op_freeze_account);
		}
		break;
	case GOI_CHUANSONG:
		{
			GM_ChuanSong* pChuanSong = (GM_ChuanSong*)uParam;
			char* szPlayerName = (char*)nParam;

			if (NULL == szPlayerName)
			{
				int npcIndex = Player[CLIENT_PLAYER_INDEX].GetNpcIndex();
				if (IsValidNpc(npcIndex))
				{
					szPlayerName = Npc[npcIndex].Name;
				}
			}
			
			nRet = Player[CLIENT_PLAYER_INDEX].SendGMOperation(szPlayerName, gm_op_transfer, pChuanSong->nMapID, pChuanSong->nPosX, pChuanSong->nPosY);
		}
		break;
	case GOI_IP:
		{
			const char* szPlayerName = (const char*)uParam;
			
			nRet = Player[CLIENT_PLAYER_INDEX].SendGMOperation(szPlayerName, gm_op_view_ip);
		}
		break;
	case GOI_SELECT_TITLE:
		{
			int selectTitleIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].GetTitleManager().SelectTitle(selectTitleIndex);
			nRet = TRUE;
		}
		break;
	case GOI_GET_PLAYER_REAL_INFO:
		{
			void* pPlayerInfoGetNeed = (void* )uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_clientPRIMgr.SendRequestToServer(pPlayerInfoGetNeed, enPlayerRealInfoOper_GetInfo);	
		}
		break;
	case GOI_SET_PLAYER_REAL_INFO:
		{
			void* pPlayerInfo = (void* )uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_clientPRIMgr.SendRequestToServer(pPlayerInfo, enPlayerRealInfoOper_SetInfo);	
		}
		break;
	default:
		nRet = 0;
		break;
	}

	return nRet;
}

//--------------------------------------------------------------------------
//	功能：发送输入设备的输入操作消息
//--------------------------------------------------------------------------
void KCoreShell::ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	//Player[CLIENT_PLAYER_INDEX].ProcessInputMsg(uMsg, uParam, nParam);
}

int KCoreShell::FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind, bool bSearchSelf /* = false */, bool bIsLeft/* = true*/ , bool bNoSelectPlayer /* = false*/)
{
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsLeftSkill = -1;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RightSkillID = 0;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_LeftSkillID = 0;

	int nT = 0;
	
	nT = Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, nRelation, bSearchSelf , bNoSelectPlayer);

	if (nT > 0)
	{		

		if (Npc[nT].GetKind() == kind_talisman)
			return false;

		if ( pReturn )
		{
			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			strncpy(p->Name, Npc[nT].Name, 32);
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
			p->nData = Npc[nT].GetMenuState();
		}

		
		if ( bSelect && (nT !=Player[CLIENT_PLAYER_INDEX].m_nIndex ) )
		{
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, nT);

			KTargetInfo tagTargetInfo;
			
			strncpy( tagTargetInfo.strName, Npc[nT].Name, sizeof(tagTargetInfo.strName) );
			tagTargetInfo.nLifePercentage = Npc[nT].GetCurrentManaPercentage();
			tagTargetInfo.nMagicPercentage = Npc[nT].GetCurrentManaPercentage();
			tagTargetInfo.nSex		= Npc[nT].m_nSex;
			tagTargetInfo.nMetier	= Npc[nT].m_Series;
			tagTargetInfo.nLevel	= Npc[nT].m_Level;

			tagTargetInfo.nPrivateState = Npc[nT].m_UnaryAttrMgr[nuai_camou_flage];

			if ( Npc[nT].m_bShowTargetFace )
			{
				CoreDataChanged( GDCNI_SEL_TARGET, (unsigned int)&tagTargetInfo, NULL );
			}
			else
			{
				CoreDataChanged( GDCNI_SEL_TARGET, FALSE, NULL );
			}
			
		}		

		nKind = Npc[nT].m_Kind;
		return true;
	}
	
	return false;
}

int KCoreShell::AutoSelectNPC( int nRelation, int nMouseX, int nMouseY )
{
	int nRangeX = 512*3;
	int	nRangeY = 512*3;
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int	nSubWorld = Npc[m_nIndex].m_SubWorldIndex;
	int	nRegion = Npc[m_nIndex].m_RegionIndex;
	int	nMapX = Npc[m_nIndex].GetMapX();
	int	nMapY = Npc[m_nIndex].GetMapY();
	int	nRet;
	int	nRMx, nRMy, nSearchRegion;

	if ( nSubWorld == -1 ||
		nRegion == -1 )
	{
		return false;
	}

	nRangeX = nRangeX / REGION_CELL_SIZE_X;
	nRangeY = nRangeY / REGION_CELL_SIZE_Y;	

	ConfigManager& cm = ConfigManager::Singleton();
	int nRegionCnt = cm.GetGlobalVariable( global_var_valid_auto_selectnpc_distance );
	if ( nRegionCnt < 1 )
	{
		nRegionCnt = 1;
	}

	int nTargetNpcIdx = Npc[m_nIndex].GetTargetNpc();

	// 检查视野范围内的格子里的NPC
	for (int i = 0; i < nRangeX; i++)	// i, j由0开始而不是从-range开始是要保证Nearest
	{
		for (int j = 0; j < nRangeY; j++)
		{
			// 去掉边角几个格子，保证视野是椭圆形
			if ((i * i + j * j) > nRangeX * nRangeX)
				continue;

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH ;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0 && nRet != nTargetNpcIdx )
			{
				if ( nTargetNpcIdx > 0 )
				{
					int nDistance = NpcSet.GetDistance( nTargetNpcIdx, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
						{
							return true;
						}
					}
				}
				else
				{
					int nDistance = NpcSet.GetDistanceByMousePt( nMouseX, nMouseY, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
							return true;
					}
				}
			}
			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY + j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0 && nRet != nTargetNpcIdx )
			{
				if ( nTargetNpcIdx > 0 )
				{
					int nDistance = NpcSet.GetDistance( nTargetNpcIdx, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
						{
							return true;
						}
					}
				}
				else
				{
					int nDistance = NpcSet.GetDistanceByMousePt( nMouseX, nMouseY, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
							return true;
					}
				}
			}

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX - i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC			
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0 && nRet != nTargetNpcIdx )
			{
				if ( nTargetNpcIdx > 0 )
				{
					int nDistance = NpcSet.GetDistance( nTargetNpcIdx, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
						{
							return true;
						}
					}
				}
				else
				{
					int nDistance = NpcSet.GetDistanceByMousePt( nMouseX, nMouseY, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
							return true;
					}
				}
			}

			// 确定目标格子实际的REGION和坐标确定
			nRMx = nMapX + i;
			nRMy = nMapY - j;
			nSearchRegion = nRegion;			
			if (nRMx < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
				nRMx += REGION_CELL_WIDTH;
			}
			else if (nRMx >= REGION_CELL_WIDTH)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
				nRMx -= REGION_CELL_WIDTH;
			}
			if (nSearchRegion == -1)
				continue;
			if (nRMy < 0)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
				nRMy += REGION_CELL_HEIGHT;
			}
			else if (nRMy >= REGION_CELL_HEIGHT)
			{
				nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
				nRMy -= REGION_CELL_HEIGHT;
			}
			if (nSearchRegion == -1)
				continue;
			// 从REGION的NPC列表中查找满足条件的NPC
			nRet = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nIndex, nRelation);
			if (nRet > 0 && nRet != nTargetNpcIdx )
			{
				if ( nTargetNpcIdx > 0 )
				{
					int nDistance = NpcSet.GetDistance( nTargetNpcIdx, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
						{
							return true;
						}
					}
				}
				else
				{
					int nDistance = NpcSet.GetDistanceByMousePt( nMouseX, nMouseY, nRet );
					if ( nDistance <= nRegionCnt )
					{
						if ( SelectNPC(nRet) )
							return true;
					}
				}
			}
		}
	}
	return false;
}
// changed end

//-------> Lucien [LIUSiliang] 2007-06-05
bool KCoreShell::SelectNPC(int nIdx)
{
	if (!IsValidNpc(nIdx))
	{
		Npc[GetClientPlayer().GetNpcIndex()].SetTarget(type_npc, 0);
		CoreDataChanged(GDCNI_SEL_TARGET, FALSE, NULL);
		return false;
	}

	if ( nIdx == Player[CLIENT_PLAYER_INDEX].GetNpcIndex())
	{
		return false;
	}
	
	if (Npc[nIdx].GetKind() == kind_talisman || nIdx == GetTargetNPC())
		return false;

	Npc[GetClientPlayer().GetNpcIndex()].SetTarget(type_npc, nIdx);
	if ( Npc[GetClientPlayer().GetNpcIndex()].m_bShowTargetFace )
	{
		CoreDataChanged(GDCNI_SEL_TARGET, TRUE, NULL);
	}
	else
	{
		CoreDataChanged( GDCNI_SEL_TARGET, FALSE, NULL );
	}	

	return true;
}
//<------- End [LIUSiliang]

int KCoreShell::FindNpcIndexById(DWORD npcId)
{
	if (npcId <= 0)
		return 0;

	return NpcSet.SearchID(npcId);
}

int KCoreShell::FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind)
{
	const int nT = Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
	//int nT = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
	
	//if (!bSelect)
	//	Player[CLIENT_PLAYER_INDEX].SetTargetObj(0);

	if (nT > 0)
	{
		nObjectIdx = nT;
		nKind = Object[nT].m_nKind;
		return true;
	}
	return false;
}


int KCoreShell::FindSpecialNPC(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT < MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->nIndex = Npc[nT].m_Index;
				p->uId = Npc[nT].m_dwID;
				p->nData = Npc[nT].GetMenuState();
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

int KCoreShell::ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC)
		{
			int nTalker = p->nIndex;
			if (Npc[nTalker].m_Kind == kind_player &&
				Npc[nTalker].m_dwID == p->uId)
			{
				Npc[nTalker].SetChatInfo(p->Name, pMsgBuff, nMsgLength);
				return true;
			}
		}
	}

	return false;
}

void KCoreShell::TradeApplyStart(int npcId)
{
	if(npcId <= 0)
		return;
	
	Player[CLIENT_PLAYER_INDEX].tradeClientSendRequest(npcId);
}

int KCoreShell::UseGameObject(  KGameObject *pGO  )
{
	switch( pGO->uGenre )
	{
	case CGOG_SKILL:
		{
			KSkillObject *pSkillGO = (KSkillObject*)pGO;
			if ( pSkillGO )
			{
//				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_skill, pSkillGO->nSkillID, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetType(), Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc());
				SendClientCmdSkill(pSkillGO->nSkillID, 
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetType(), 
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc()
					);
			}
		}
		break;
	}

	return 1;
}

// void KCoreShell::DialogNpc()
// {
// 	// 小于对话半径就开始对话
// 	RemoveMovePosition();
// 
// 	int nIdx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();
// 	int nRelation = NpcSet.GetRelation( Player[CLIENT_PLAYER_INDEX].m_nIndex, nIdx );
// 
// 	if (Npc[nIdx].m_Kind == kind_dialoger ||
// 		nRelation == relation_ally )
// 	{
// 		int distance = NpcSet.GetDistance(nIdx, Player[CLIENT_PLAYER_INDEX].m_nIndex);
// 		if (distance <= Npc[nIdx].m_DialogRadius)
// 		{
// 			int x, y;
// 			SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].Map2Mps(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMapX(), Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMapY(), Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetOffX(), Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetOffY(), &x, &y);
// 			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_run, x,y);
// 			
// 			// Send Command to Server
// 			SendClientCmdRun(x, y);
// 			Player[CLIENT_PLAYER_INDEX].DialogNpc(nIdx);
// //			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, 0);
// 			Npc[nIdx].TurnTo(Player[CLIENT_PLAYER_INDEX].m_nIndex);
// 			
// 			return;
// 		}
// 	}
// }

// int KCoreShell::UseSkill( KSkillData skillData )
// {
// 	RemoveMovePosition();
// 	
// 	int		nSkillID = skillData.nSkillID;
// 	int		bLeftMouse = skillData.bLeftMouse;
// 	bool	bAltPressed = skillData.bAltPressed;
// 
// 	if ( nSkillID != KNIGHT_NORMALSKILL_ID && 
// 		nSkillID != ENCHANTER_NORMALSKILL_ID &&
// 		nSkillID != MONSTROUS_NORMAILSKILL_ID )
// 	{
// 		nSkillID = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(nSkillID);
// 	}
// 	
// 
// 	if ( nSkillID == 0 )
// 	{
// 		nSkillID = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ActiveSkillID;
// 		if ( nSkillID != KNIGHT_NORMALSKILL_ID && 
// 			nSkillID != ENCHANTER_NORMALSKILL_ID &&
// 			nSkillID != MONSTROUS_NORMAILSKILL_ID )
// 		{
// 			nSkillID = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(nSkillID);
// 		}
// 		if ( nSkillID <= 0 )
// 		{
// 			switch( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Series )
// 			{
// 			case 0:
// 				nSkillID =  KNIGHT_NORMALSKILL_ID;
// 				break;
// 			case 1:
// 				nSkillID = ENCHANTER_NORMALSKILL_ID;
// 				break; 
// 			case 2:
// 				nSkillID = MONSTROUS_NORMAILSKILL_ID;
// 				break;
// 			}
// 		}
// 	}
// 
// 	if (Player[CLIENT_PLAYER_INDEX].IsBlockClientControl())
// 		return 0;
// 	
//   	if ( Player[CLIENT_PLAYER_INDEX].CheckTrading())
//  		return 0;
// 
// 	// 注意: 此行代码之前不要调用Npc的SetActiveSkill()操作
// 
// 	// IsCanInput如果返回0，表示上一个技能施放尚未结束，正在等待帧数，
// 	// 此时如果让这个地方过去，后面SetActiveSkill就会设置当前技能id
// 	// 就会导致等待帧数结束准备施放的时候，GetActiveSkill返回的是
// 	// 另外一个技能
// 	if( !Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].IsCanInput() )
// 		return 0;
// 
// 	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsLeftSkill = bLeftMouse;
// 	if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsLeftSkill == 0 )
// 	{
// 		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RightSkillID = nSkillID;
// 		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_LeftSkillID = 0;
// 	}
// 	else if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bIsLeftSkill == 1)
// 	{
// 		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RightSkillID = 0;
// 		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_LeftSkillID = nSkillID;
// 	}
// 	else
// 	{
// 		//用非普通攻击
// 	}
//   	
// 	POINT	curPt;
// 	
// 	if( !GetCursorPos(&curPt) )
// 		return 0;
// 
// 	if( !ScreenToClient(g_GetMainHWnd(), &curPt) )
// 		return 0;
// 
//   	int nX = curPt.x;
//   	int nY = curPt.y;
//   	int nZ = 0;
// 	
//   	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
//   	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
// 
// 	if( g_SkillManager.IsSwitchSkill(nSkillID) )
// 	{
// 		Npc[nIndex].m_SkillList.SwitchSkill(nSkillID);
// 		bool bIsSwitchOn = Npc[nIndex].m_SkillList.GetSwitchSkillId() != INVALID_SKILL_ID;		
// 
// 		if(bIsSwitchOn)
// 		{
// 			int nIdx = Npc[nIndex].m_SkillList.FindSkill(nSkillID);
// 			Npc[nIndex].SetActiveSkill(nIdx);
// 		}
// 		else
// 		{
// 			int	nNormalSkillId = Npc[nIndex].GetNormalSkillId();
// 			int	nIdx = Npc[nIndex].m_SkillList.FindSkill(nNormalSkillId);
// 			Npc[nIndex].SetActiveSkill(nIdx);
// 		}
// 
// 		KSkill *pSkill = g_SkillManager.GetSkill(nSkillID, 1);
// 
// 		if(pSkill)
// 		{
// 			char	szMsg[256];
// 			if(bIsSwitchOn)
// 				_snprintf(szMsg, sizeof(szMsg), MSG_SWITCH_SKILL_ON, pSkill->m_szName);
// 			else
// 				_snprintf(szMsg, sizeof(szMsg), MSG_SWITCH_SKILL_OFF, pSkill->m_szName);
// 
// 			CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szMsg, 0);
// 		}
// 
// 		return 0;
// 	}
// 
// 	// 判断可以使用技能？为调普通攻击的时放技能候技能先注掉
//    	//if (Npc[nIndex].IsCanInput())
//   	//{
//   		int nIdx = 0;
//   		
//   		nIdx = Npc[nIndex].m_SkillList.FindSkill(nSkillID);	
//   		Npc[nIndex].SetActiveSkill(nIdx);
//   	/*}
//   	else
//   	{
//   		return 0;
//   	}//*/
// 
// 	Npc[nIndex].CheckAndSwitchSkill();
//   
//   	if (Npc[nIndex].m_ActiveSkillID > 0)
//   	{
//   		KSkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
// 
//   		if (!pISkill) 
//              return 0;
// 
//   		int nTargetIdx = Npc[nIndex].GetTargetNpc();
// 		int nAttackTargetType = pISkill->GetAttackTargetType();
// 		
// 		if( nAttackTargetType & att_target_only )
//         {
// 			if( (att_target_only | att_target_self) == nAttackTargetType )
// 				nTargetIdx = nIndex;
// 
// 			// 默认目标是自己
// 			if( !nTargetIdx && (nAttackTargetType & att_target_self) )
// 				nTargetIdx = nIndex;
// 
// 			// Alt键选择自己
// 			if( bAltPressed )
// 				nTargetIdx = nIndex;
// 
// 			// 如果技能可以对自身施放，而且当前选择的目标无效的话，
// 			// 为了易用性考虑 默认对自己施法
// 			if( !Npc[nIndex].IsAttackTarget(nTargetIdx, nAttackTargetType) )
// 			{
// 				if(nAttackTargetType & att_target_self)
// 					nTargetIdx = nIndex;
// 			}
// 
//   			if( !nTargetIdx )
// 			{
// 				int nNPCKind = 0;
// 				POINT tagPoint;
// 				::GetCursorPos( &tagPoint );
// 				KUiPlayerItem SelectPlayer;
// 				if ( FindSelectNPC(curPt.x, curPt.y, relation_all, true, &SelectPlayer, nNPCKind, true) <= 0 )
// 				{
// 					// 没有找到目标
// 					CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_SKILL_NOTARGET, 0);
// 					return 0;
// 				}			
// 				nTargetIdx = SelectPlayer.nIndex;
// 			}
// 			
//   			if( !Npc[nIndex].IsAttackTarget(nTargetIdx, nAttackTargetType) )
// 			{
// 				// 无效目标
// 				CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_SKILL_TARGET_INVALID, 0);
// 				return 0;
// 			}
//   		}
// 
//    		if (   !Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID)
//   			|| !Npc[nIndex].Cost(pISkill, TRUE)
//   		   )
// 		{
//   			return 0;
//   		}
// 		  
//   		if( nTargetIdx &&  (nAttackTargetType & att_target_only) )
//   		{
// 			int nTargetType = SKILL_SPT_TargetIndex;
// 
//   			if (!pISkill->CanCastSkill(nIndex, nTargetType, nTargetIdx))
//   			{
//   				return 0;
//   			}
// 
//   			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nTargetType, Npc[nTargetIdx].m_dwID);
// //			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nTargetType, nTargetIdx);
//    		
// 			
// 			/*if ( Npc[nIndex].m_LeftSkillID == 0 || Npc[nIndex].m_RightSkillID == 0 )
// 			{
// 				int normalSkillID;
// 				switch( Npc[nIndex].m_Series )
// 				{
// 				case 0:
// 					normalSkillID =  KNIGHT_NORMALSKILL_ID;
// 					break;
// 				case 1:
// 					normalSkillID = ENCHANTER_NORMALSKILL_ID;
// 					break; 
// 				case 2:
// 					normalSkillID = MONSTROUS_NORMAILSKILL_ID;
// 					break;
// 				}
// 
// 				int nSI = Npc[nIndex].m_SkillList.FindSkill(normalSkillID);
// 				Npc[nIndex].SetActiveSkill(nSI);
// 
// 				//Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( true );
// 				//SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nTargetType, Npc[nTargetIdx].m_dwID);
// 			}//*/
// 
// 		}
// 		else
// 		{
//   			int nT1 = nX;
//   			int nT2 = nY;
// 
//   			if ( !pISkill->CanCastSkill(nIndex, nT1, nT2) )
//   				return 0;
// 			
//   			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
// //			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);
// 
// 			/*if ( Npc[nIndex].m_LeftSkillID == 0 || Npc[nIndex].m_RightSkillID == 0 )
// 			{
// 				int normalSkillID;
// 				switch( Npc[nIndex].m_Series )
// 				{
// 				case 0:
// 					normalSkillID =  KNIGHT_NORMALSKILL_ID;
// 					break;
// 				case 1:
// 					normalSkillID = ENCHANTER_NORMALSKILL_ID;
// 					break; 
// 				case 2:
// 					normalSkillID = MONSTROUS_NORMAILSKILL_ID;
// 					break;
// 				}
// 
// 				int nSI = Npc[nIndex].m_SkillList.FindSkill(normalSkillID);
// 				Npc[nIndex].SetActiveSkill(nSI);
// 
// 				//Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].SetCanFollowAndAttack( true );
// 				//SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nTargetType, Npc[nTargetIdx].m_dwID);
// 			}//*/
//   		}
//   	}
// 	
// 	
//  	return 1;
// }

int KCoreShell::LockSomeoneAction(int nTargetIndex)
{
	//Modified by Ray [Luoliang]  2004-7-28
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;
	if (nTargetIndex <= 0 || nTargetIndex >= MAX_NPC)	//取消Lock
	{
		Npc[nIndex].SetTarget(type_npc, 0);
		CoreDataChanged( GDCNI_SEL_TARGET, NULL, NULL );
		return 1;
	}

	//lixuewu 临时增加对话操作，2006.09.26
	Player[CLIENT_PLAYER_INDEX].DialogNpc(nTargetIndex);

	Npc[nIndex].SetTarget(type_npc, nTargetIndex);
	return 1;
}

// int KCoreShell::LockObjectAction(int nTargetIndex)
// {
// 	//Modified by Ray [Luoliang]  2004-7-28
// 	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
// 		return 0;
// 	
// 	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
// 
// 	if (nTargetIndex <= 0)	//取消Lock
// 		Npc[nIndex].SetTarget(type_obj, 0);
// 	else
// 		Npc[nIndex].SetTarget(type_obj, nTargetIndex);
// 
// 	return 1;
// }

#define  MOUSE_DOWN_MOVE_MODE               

void KCoreShell::GotoWhere(int x, int y, int mode ,bool bDelay /*= false*/)
{
	if (Player[CLIENT_PLAYER_INDEX].IsBlockClientControl())
		return;

	if (Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].m_Doing==do_revive)
		return;
	
	if (mode < 0 || mode > 2)
		return;

#ifdef _AUTO_ROBOT
	AutoRobotMgr &mgr = AutoRobotMgr::Singleton();

	if( enRobotMode_AutoRun == mgr.GetMode() )
	{
		CFS_FILELOGS::WriteLog("if(enRobotMode_AutoRun == mgr.GetMode).\n");
		mgr.SetMode(enRobotMode_None);
	}
	mgr.SetAutoSellState( false );
	mgr.StopGoToOtherMap(false);
	mgr.RemoveAutoGoHomeFlags(AUTO_GO_NET_DELAY);
#endif

	Stop();

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
			mode == 2)
			bRun = true;

		int nX = x;
		int nY = y;
		int nZ = 0;
		g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if( Npc[nIndex].m_UnaryAttrMgr[nuai_nomove] )
			return;
		

		// 删除移动位置指针
		RemoveMovePosition();

		Npc[nIndex].SendCommand(do_run, nX, nY);

		if (bDelay)
		{
			Player[CLIENT_PLAYER_INDEX].PushRunPackageRecord(nX,nY);
		}//endif
		else
			SendClientCmdRun(nX, nY);
		
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}

// void KCoreShell::Goto(int nDir, int mode)
// {
// 	if (nDir < 0 || nDir > 63)
// 		return;
// 
// 	if (mode < 0 || mode > 2)
// 		return;
// 
// 	int bRun = false;
// 
// 	if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
// 		mode == 2)
// 		bRun = true;
// 
// 	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
// 
// 	int nSpeed;
// 	if (bRun)
// 		nSpeed = Npc[nIndex].m_CompAttrMgr[ncai_runspeed];
// 	else
// 		nSpeed = Npc[nIndex].m_CompAttrMgr[ncai_walkspeed];
// 
// 	Player[CLIENT_PLAYER_INDEX].Walk(nDir, nSpeed);
// 
// 	Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
// }

void KCoreShell::Turn(int nDir)
{
	if (nDir < 0 || nDir > 3)
		return;

	if (nDir == 0)
		Player[CLIENT_PLAYER_INDEX].TurnLeft();
	else if (nDir == 1)
		Player[CLIENT_PLAYER_INDEX].TurnRight();
	else
		Player[CLIENT_PLAYER_INDEX].TurnBack();
}

int KCoreShell::ThrowAwayItem( int nItemIndex )
{
	ItemPos posItem;
	ZeroMemory( &posItem, sizeof(ItemPos) );
	if ( 	Player[CLIENT_PLAYER_INDEX].GetItemList().GetItemPos( nItemIndex,(ItemPos*)&posItem ) )
	{
		KItem*	pItem = Player[CLIENT_PLAYER_INDEX].GetItemList().GetItemFromPlace( &posItem );
		if ( pItem )
		{
			if (!pItem->IsLocked(-1) && pItem->CanDiscard())
			{
				return Player[CLIENT_PLAYER_INDEX].ThrowAwayItem( pItem->GetID() );
			}
			else
			{
				//TODO 提示客户端不可丢弃
			}
		}
	}


	return 0;
}

int KCoreShell::GetNPCRelation(int nIndex)
{
	// 让kind_building对友方可以对话，敌对可以攻击
	int	nRelation = NpcSet.GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIndex);

	if( (relation_enemy != nRelation) && (kind_building == Npc[nIndex].m_Kind) )
		return relation_dialog;
	else
		return nRelation;
}

//--------------------------------------------------------------------------
//	功能：绘制游戏对象
//--------------------------------------------------------------------------
void KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
	if (g_pRepresent)
		CoreDrawGameObj(uObjGenre, uId, x, y, Width, Height, nParam);
}

#include "iRepresentshell.h"

//--------------------------------------------------------------------------
//	功能：绘制游戏世界
//--------------------------------------------------------------------------
void KCoreShell::DrawGameSpace()
{
	if (g_pRepresent)
	{
		g_ScenePlace.Paint();
		ScreenEffectMgr& SM =  ScreenEffectMgr::Singleton();
		SM.PaintBehindUi();
	}
}
void          KCoreShell::DrawUiEffect()
{
	if(g_pRepresent)
	{
		ScreenEffectMgr& SM =  ScreenEffectMgr::Singleton();
		SM.PaintBeforeUi();
		//效果文件绘制//
	}

}
void KCoreShell::BreatheGameSpace()
{
	NpcSet.SceneProcessNpc();
	g_ScenePlace.Breathe();
}

void KCoreShell::PaintBehindUi()
{
	if (g_pRepresent)
	{
		ScreenEffectMgr& SM =  ScreenEffectMgr::Singleton();
		SM.PaintBeforeUi();
	}
}

//--------------------------------------------------------------------------
//	功能：设置绘图接口实例的指针
//--------------------------------------------------------------------------
void KCoreShell::SetRepresentShell(struct iRepresentShell* pRepresent)
{
	g_pRepresent = pRepresent;
	g_ScenePlace.RepresentShellReset();
	if (g_pAdjustColorTab && g_ulAdjustColorCount && g_pRepresent)
		g_pRepresent->SetAdjustColorList(g_pAdjustColorTab, g_ulAdjustColorCount);
}

//===> Added By Rocker 2004.4.6 默认NPC
void KCoreShell::InitSimplifiedNpc(BOOL bSimplifiedNpc)
{
}

void KCoreShell::EnableSimplifiedNpc(BOOL bSimplifiedNpc)
{
}

//<=== Added End

void KCoreShell::SetMusicInterface(void* pMusicInterface)
{
	g_pMusic = (KMusic*)pMusicInterface;
	Option.SetMusicVolume(Option.GetMusicVolume());
}

//日常活动，core如果要寿终正寝则返回0，否则返回非0值
int KCoreShell::Breathe()
{
	g_SubWorldSet.MainLoop();
	g_ProtocolSimulationSet.Breathe();//*/

	return true;
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];
}

#ifdef SWORDONLINE_SHOW_DBUG_INFO
extern int		g_bShowObstacle;
extern bool		g_bShowGameInfo;	//是否显示游戏（场景）信息
#endif
// 
// int KCoreShell::Debug(unsigned int uDataId, unsigned int uParam, int nParam)
// {
// #ifdef SWORDONLINE_SHOW_DBUG_INFO
// 	switch(uDataId)
// 	{
// 	case DEBUG_SHOWINFO:
// 		Player[CLIENT_PLAYER_INDEX].m_DebugMode = !Player[CLIENT_PLAYER_INDEX].m_DebugMode;
// 		g_bShowGameInfo = !g_bShowGameInfo;
// 		break;
// 	case DEBUG_SHOWOBSTACLE:
// 		g_bShowObstacle = !g_bShowObstacle;
// 		break;
// 	case DEBUG_GM_CMD:
// 		if (nParam)
// 		{
// 			const char* pszCmd = (const char*)(nParam);
// 			Player[CLIENT_PLAYER_INDEX].DoScript((char *)pszCmd);
// 		}
// 		break;
// 	case 4:
// /*		#include "KProtocolDef.h"
// 		int nX;int nY;
// 		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX, &nY);
// 		CLIENT_BUILDING_BUILD tBuildingBuild =
// 		{
// 			c2s_building_build,		// 协议号
// 			0,						// 模板编号
// 			nX, // 世界坐标X
// 			nY, // 世界坐标Y
// 		};
// 		SendNewDataToServer(&tBuildingBuild, sizeof(tBuildingBuild));*/
// 
// 		break;
// 	}
// #endif
// 	return 0;
// }
DWORD KCoreShell::GetPing()
{
	return g_SubWorldSet.GetPing();
}

//void KCoreShell::SendPing()
//{
//	SendClientCmdPing();
//}

void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
}

void KCoreShell::SetClient(LPVOID pClient, int nConnectID)
{
	g_SetClient(pClient,nConnectID);
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,pData, nLength);
}

//与地图相关的操作
int	KCoreShell::SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GSMOI_GET_PLAYER_NAME_POS_AT_POS:
		{
			Position* destPos = (Position*)nParam;
			vector<string>* npcNames = (vector<string>*)uParam;
			g_ScenePlace.getNpcNameAtPos(npcNames, destPos);
		}
		break;
	case GSMOI_GET_MAP_INFO_AT_SCENE_POS:
		{
			MapPosInfo* mapInfo = (MapPosInfo*)uParam;
			Position* destPos = (Position*)nParam;
			g_ScenePlace.getMapInfoAtPos(mapInfo, destPos);
		}
		break;
	case GSMOI_GET_MAP_INFO_AT_MINI_POS:
		{
			MapPosInfo* mapInfo = (MapPosInfo*)uParam;
			Position* destPos = (Position*)nParam;
			g_ScenePlace.getMapInfoAtMiniPos(mapInfo, destPos);
		}
		break;
	case GSMOI_SCENE_TIME_INFO:
		if (uParam)
		{
			KUiSceneTimeInfo* pInfo = (KUiSceneTimeInfo*)uParam;
			g_ScenePlace.GetSceneNameAndFocus(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
		break;
		
	case GSMOI_SCENE_LITTLE_MAP_INFO:
		nRet = g_ScenePlace.GetLittleMapInfo((KSceneMapInfo*)uParam);
		break;

	case GSMOI_SCENE_MAP_INFO:
		nRet = g_ScenePlace.GetMapInfo((KSceneMapInfo*)uParam);
		break;
	case GSMOI_IS_SCENE_MAP_SHOWING:
		g_ScenePlace.SetMapParam(uParam, nParam);
		break;
	case GSMOI_PAINT_MINI_MAP:
		g_ScenePlace.PaintMiniMap(uParam, nParam);
		break;
	case GSMOI_PAINT_SCENE_MAP:
		g_ScenePlace.PaintSceneMap();
		break;
	case GSMOI_SCENE_MAP_FOCUS_OFFSET:
		g_ScenePlace.SetMapFocusPositionOffset((int)uParam, nParam);
		break;

	case GSMOI_SCENE_LITTLE_MAP_FOCUS:
		g_ScenePlace.SetLittleMapFocusPosition((int)uParam, nParam);
		break;

	case GSMOI_SCENE_FOLLOW_WITH_MAP:	//设置场景是否随着地图的移动而移动
		g_ScenePlace.FollowMapMove(nParam);
		break;
	case GSMOI_SCENE_MAP_FOCUS:
		g_ScenePlace.SetFocusPosition((int)uParam, nParam, 0);
		break;

	case GSMOI_GET_SCENE_ID:
		nRet = g_ScenePlace.GetID();
		break;
		
	// <Add name="Adt.X" time="2005/10/13">
	case GSMOI_SCENE_LITTLE_MAP_RECT:
		nRet = g_ScenePlace.GetLittleMapRect((RECT*)uParam);
		break;
	// </Add>
	// --> Rocker Edit Start 2005/11/10
	case GSMOI_GET_MINIMAP_CURSOR_INFO:
		g_ScenePlace.GetLittleMapCursorInfo(((KLittleMapCursorInfo*)uParam)->nCursorX,
			((KLittleMapCursorInfo*)uParam)->nCursorY, ((KLittleMapCursorInfo*)uParam)->szInfo, 
			nParam);
		break;
	// <-- Rocker End

	/*!
	\brief
		Show scene map
	\return
		Nothing.
	*/
	case GSMOI_SHOW_SCENE_MAP:
		{
			g_ScenePlace.ShowSceneMap();
		}
		break;
	case GSMOI_SCENE_CHANGE_MAP:
		{
			char* mapName = (char*)uParam;
//			nRet = g_ScenePlace.LoadSceneMap(mapName, nParam);
			MapPosInfo mapInfo;
			g_ScenePlace.getMapInfoAtPos(&mapInfo, &Position());
		    if(strcmp(mapName,mapInfo.mapName) == 0)
			{
				g_ScenePlace.SetCurrentMapFLags(true);
				nRet = g_ScenePlace.LoadSceneMap(mapName);
			}
			else
			{
				g_ScenePlace.SetCurrentMapFLags(false);
				nRet = g_ScenePlace.LoadSceneMap(mapName,FALSE);
			}
		}
		break;
	case GSMOI_HAVE_SCENE_MAP:
		{
			char* mapName = (char*)uParam;
			if(mapName)
			{
				nRet = g_ScenePlace.isHaveSceneMap(mapName);
			}
			else
			{
				nRet = 0;
			}
		}
		break;
	/*!
	\brief
		Hide scene map.
	\return
		Nothing.
	*/
	case GSMOI_HIDE_SCENE_MAP:
		{
			g_ScenePlace.HideSceneMap();
		}
		break;
	/*!
	\brief
		Is show the scene map.
	\return
		Is show?(bool).
	*/
	case GSMOI_IS_SCENE_MAP_SHOW:
		{
			nRet = g_ScenePlace.IsSceneMapShow();
		}
		break;
	/*!
	\brief
		Get scene map width.
	\return
		Width(int).
	*/
	case GSMOI_GET_SCENE_MAP_WIDTH:
		{
			nRet = g_ScenePlace.GetSceneWidth();
		}
		break;
	/*!
	\brief
		Get scene map height.
	\return
		Height(int).
	*/
	case GSMOI_GET_SCENE_MAP_HEIGHT:
		{
			nRet = g_ScenePlace.GetSceneHeight();
		}
		break;
	case GSMOI_GO_TO_POS:
		{
			g_ScenePlace.gotoPosition((int)uParam, nParam);
		}
		break;
	}
	return nRet;
}

//与建筑相关的结构 lixuewu 2004.10.14
int KCoreShell::BuildingOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	//-->Rocker 2004/11/10 
	static time_t s_dwLastTimeStamp = 0;
	//<--Rocker 
	switch(uOper)
	{
	case GAME_BUILD_GET_ALL_TYPE_COUNT:	// 可建设建筑的个数
		break;
	case  GCOI_BUILD_GET_BUILD_TYPE_COUNT:
		break;
	case GCOI_BUILD_GET_TOTEM_TYPE_COUNT:
		break;
	case GCOI_BUILD_GET_PLANT_TYPE_COUNT:
		break;
		
	case GCOI_BUILD_ALL_INFO:	// 建筑物的描述信息
		break;
	case GCOI_BUILD_CANBUILD_INFO:	// 可建设建筑物的描述信息
		break;
	case GCOI_BUILD_TOTEM_INFO:	// 图腾建筑物的描述信息
		break;
	case GCOI_BUILD_PLANT_INFO:	// 植被筑物的描述信息
		break;
		
	case GCOI_BUILD_ALL_TIPS: // 建筑物的鼠标悬停信息
		break;
	case GCOI_BUILD_CANBUILD_TIPS: // 建筑物的鼠标悬停信息
		break;
	case GCOI_BUILD_TOTEM_TIPS: // 建筑物的鼠标悬停信息
		break;
	case GCOI_BUILD_PLANT_TIPS: // 植被物的鼠标悬停信息
		break;
		
	case GCOI_BUILD_ALL_ICON: // 建筑物的图标路径
		break;
	case GCOI_BUILD_CANBUILD_ICON: // 建筑物的图标路径
		break;
	case GCOI_BUILD_TOTEM_ICON: // 建筑物的图标路径
		break;
	case GCOI_BUILD_PLANT_ICON: // 建筑物的图标路径
		break;
		
	case GCOI_TERRAIN_GET_TYPE_COUNT:
		break;
	case GCOI_TERRAIN_INFO: // 地形描述
		break;

	case GCOI_GET_CITY_INFO:
		break;
	case GCOI_CREATE_CITY:
		break;
	case GCOI_ENTER_CITY:
		break;
	case GCOI_GET_TECH_TIP:
		break;
	}
	return nRet;
}

//与组队相关的操作，uOper的取值来自 GAME_TEAM_OPERATION_INDEX
int KCoreShell::TeamOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	KPlayerTeam& teamInfo = GetClientPlayer().GetTeamInfo();

	int nRet = 0;
	switch(uOper)
	{
	case TEAM_OI_GD_INFO://主角所在的队伍信息
		if (uParam)
		{
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			nRet = teamInfo.GetInfo(pTeam);
		}
		else
		{
			KUiPlayerTeam Team;
			nRet = teamInfo.GetInfo(&Team);
		}
		break;	
	case TEAM_OI_CREATE://新组队伍
			teamInfo.Create();
		break;
	case TEAM_OI_OPEN_BIG_TEAM_MODE://开启大队伍模式
		//teamInfo.OpenBigTeamMode();
		break;
	case TEAM_OI_APPOINT://任命队长
		teamInfo.NewCaptain(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_INVITE://邀请别人加入队伍（按玩家NpcID）
		teamInfo.InviteAdd(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_INVITE_BY_NAME://邀请别人加入队伍（按玩家名字）
		teamInfo.InviteAdd(((KUiPlayerItem*)uParam)->Name);
		break;
	case TEAM_OI_KICK://踢除队里的一个队员
		teamInfo.KickMember(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_LEAVE://离开队伍
		teamInfo.LevaveTeam();
		break;
	case TEAM_OI_DISMISS://解散队伍
		teamInfo.Dismiss();
		break;
	case TEAM_OI_INVITE_RESPONSE://回复组队邀请
		teamInfo.ReplyInvite(uParam, (enumInviteJointeamReplay)nParam);
		break;
	case TEAM_OI_GD_REFUSE_INVITE_STATUS://得到是否自动拒绝组队邀请
		nRet = teamInfo.IsAutoRefuseInvite();
		break;
	case TEAM_OI_REFUSE_INVITE://设置自动拒绝别人的组队邀请
		teamInfo.SetAutoRefuseInvite(nParam);
		break;
	case TEAM_OI_MEMBER_INFO://获取队友信息
		nRet = teamInfo.GetAllTeamMemberInfo((KUiTeamMemberItem *)uParam);
		break;
	case TEAM_OI_MOVE_INDEX:
		teamInfo.MoveIndex(uParam, nParam);
		break;
	case TEAM_IO_PROMOTE_ASSISTANT:
		teamInfo.PromoteAssistant(uParam);
		break;
	case TEAM_IO_DISMISS_ASSISTANT:
		teamInfo.DismissAssistant(uParam);
		break;
	case TEAM_OI_APPLY_RESPONSE://回复申请加入队伍
		teamInfo.ReplyApply(uParam, (enumApplyJoinTeamReplay)nParam);
		break;
	case TEAM_OI_APPLY_JOIN:
		teamInfo.ApplyJoinTeam(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_APPLY_JOIN_BY_NAME:
		teamInfo.ApplyJoinTeam(((KUiPlayerItem*)uParam)->Name);
		break;
	case TEAM_IO_SET_AUTO_ACCEPT_APPLY:
		teamInfo.SetAutoAcceptApply(TRUE == nParam);
		break;
	}
	return nRet;
}

int KCoreShell::InsertItem(int nGenre, int nDetail, int nParticular, 
		int nLevel, int nSeries, int nLuck,	
		int* pMagicLevel, int nVersion, int nRandSeed)
{
	return 0;
}

// lixuewu 2004.06.22
void KCoreShell::BegingBuildBuilding(unsigned int uType ,unsigned int nDataIdx)
{
}
void KCoreShell::TryToBuildBuilding(unsigned int nX, unsigned int nY)
{
}
void KCoreShell::EndBuildBuilding(void)
{
}
void KCoreShell::ValidateBuildBuilding(BOOL bCancel)
{
}
// lixuewu 2004.06.2

// Add by Cooler 2004-7-1
// Begin -->
inline void KCoreShell::GetPlayerPos(int &nPosX, int &nPosY)
{
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nPosX, &nPosY);
}
// End <--

// Add by Cooler 2004-8-3
// Begin -->
inline BOOL KCoreShell::IsItemListLocked()
{
	return Player[CLIENT_PLAYER_INDEX].
		m_ItemList.IsLockOperation();
}
// End <--

// Add by Cooler 2004-8-4
// Begin -->
inline int KCoreShell::GetHandItemIndex()
{
	return 0;
}

inline BOOL KCoreShell::FindPlacePos(POINT* pPos)
{
	return Player[CLIENT_PLAYER_INDEX].m_ItemList.FindPlacePos(pPos);
}
// End <--

/*
BOOL KCoreShell::RSGetWannaData(unsigned int uGetIndex, const void *pInData, void *pOutData)
{
	switch( uGetIndex )
	{
	case enRS_ITEM_DATA:
		{
			TDBItemData *pItemData = (TDBItemData *)pInData;

 			KItem NewItem;

			BOOL bGetEquiptResult = g_ItemGen.Gen_Item( 
													pItemData->ItemData[0].iequipclasscode,
													pItemData->ItemData[0].idetailtype,
													pItemData->ItemData[0].iparticulartype,
													pItemData->ItemData[0].ilevel,
													pItemData->ItemData[0].wItemCount,
													&NewItem);

			if(bGetEquiptResult)
			{
				if (NewItem.GetDurability() != -1 && pItemData->ItemData[0].idurability >= 0)
				{
					NewItem.SetDurability(pItemData->ItemData[0].idurability);
				}
				else if (pItemData->ItemData[0].idurability == -10)
				{
					NewItem.SetDurability(NewItem.GetMaxDurability());
				}

				PRSRETURNITEMINFO pReturn = (PRSRETURNITEMINFO)pOutData;
				memset(pOutData, 0, sizeof(RSRETURNITEMINFO));

				strcpy(pReturn->szItemName, NewItem.GetName());
				NewItem.GetDesc(pReturn->szItemDesc);
//				NewItem.GetTitle(pReturn->szItemDetail, true);
//				NewItem.GetProperty(pReturn->szItemDetail + strlen(pReturn->szItemDetail));

				return TRUE;
			}
		}
		break;
	case enRS_SKILL_DATA:
		{
//			PRSSKILLINFOREQ pSkillData = (PRSSKILLINFOREQ)pInData;
//
//			ISkill *pSkill = g_SkillManager.GetSkill(pSkillData->dwSkillID, 
//				pSkillData->nSkillLevel);
//			if( pSkill )
//			{
//				PRSRETURNSKILLINFO pReturn = (PRSRETURNSKILLINFO)pOutData;
//				
//				strcpy(pReturn->szSkillName, pSkill->GetSkillName());
//
//				return TRUE;
//			}
		}
		break;
	default:
		break;
	}

	return FALSE;
}
//*/

// add by chenshanglin on 2005-10-17 for game video
//#ifdef _GameVideo
//void KCoreShell::PrerenderGround(bool bForce)
//{
//	g_ScenePlace.PrerenderGround(bForce);	
//}
//#endif
// add end

// add by hejianfeng for Anti-Wallow.  2005-11-21
void KCoreShell::FindItemIndex(int nGenre, int nDetail, int nParticular, int *pnIdx)
{
    KItemList *pKItemList = &(Player[1].m_ItemList);

    if (pKItemList)
        pKItemList->FindSameParticularItem(nGenre, nDetail, nParticular, pnIdx);
}
// endadd

// void KCoreShell::ThrowAwayItemClient()
// {
// }

int KCoreShell::GetTargetNPC( void )
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc();
}

BOOL KCoreShell::IsTextPass(const char *szText)
{
	return g_IsTextPass(szText);
}

BOOL KCoreShell::IsNamePass(const char *szText)
{
	return g_IsNamePass(szText);
}

// void KCoreShell::SwitchDefaultSkill( void )
// {
// 	int nIndex = Player[CLIENT_PLAYER_INDEX].GetNpcIndex();
// 	int nSkillID = Npc[nIndex].GetSkillList().GetSwitchSkillId();
// 	
// 	if( !g_SkillManager.IsSwitchSkill(nSkillID) )
// 		nSkillID = Npc[nIndex].GetNormalSkillId();
// 	
// 	KSkill * pISkill =  g_SkillManager.GetSkill(nSkillID, 1);
// 
//   	if (!pISkill) 
//          return;
// 
// 	POINT	curPt;
// 	
// 	if( !GetCursorPos(&curPt) )
// 		return;
// 
// 	if( !ScreenToClient(g_GetMainHWnd(), &curPt) )
// 		return;
// 
//   	int nTargetIdx = Npc[nIndex].GetTargetNpc();
// 	int nAttackTargetType = pISkill->GetAttackTargetType();
// 	
// 	if( nAttackTargetType & att_target_only )
//     {
// 		if( (att_target_only | att_target_self) == nAttackTargetType )
// 			nTargetIdx = nIndex;
// 
//   		if( !nTargetIdx )
// 		{
// 			int nNPCKind = 0;
// 			POINT tagPoint;
// 			::GetCursorPos( &tagPoint );
// 			KUiPlayerItem SelectPlayer;
// 			if ( FindSelectNPC(curPt.x, curPt.y, relation_all, true, &SelectPlayer, nNPCKind, true) <= 0 )
// 			{
// 				// 没有找到目标
// 				CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_SKILL_NOTARGET, 0);
// 				return;
// 			}			
// 			nTargetIdx = SelectPlayer.nIndex;
// 		}
// 		
//   		if( !Npc[nIndex].IsAttackTarget(nTargetIdx, nAttackTargetType) )
// 		{
// 			// 无效目标
// 			CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_SKILL_TARGET_INVALID, 0);
// 			return;
// 		}
//   	}
// 
//    	if (   !Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID)
//   		|| !Npc[nIndex].Cost(pISkill, TRUE)
//   	   )
// 	{
//   		return;
//   	}
// 	  
//   	if( nTargetIdx &&  (nAttackTargetType & att_target_only) )
//   	{
// 		int nTargetType = SKILL_SPT_TargetIndex;
// 
//   		if (!pISkill->CanCastSkill(nIndex, nTargetType, nTargetIdx))
//   		{
//   			return;
//   		}
// 
// //  		SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nTargetType, Npc[nTargetIdx].m_dwID);
// 		Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nTargetType, nTargetIdx);
//    	}
// 	
// }


void KCoreShell::DrawMovePosition( int x, int y )
{
	// 点击移动位置图标显示
	RemoveMovePosition();

	KObjItemInfo	sInfo;
	sInfo.m_nItemID = 0;
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 0;
	sInfo.m_nSoundFlag = 0;
	sInfo.m_nMoneyNum = 0;
	strcpy(sInfo.m_szName, "");
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;

	int nnX = x;
	int nnY = y;
	int nnZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nnX, nnY, nnZ);

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	
	int nNpcX, nNpcY;
	Npc[nIndex].GetMpsPos(&nNpcX, &nNpcY);
	int offsetx = (nnX - nNpcX) * 1024;
	int offsety = (nnY - nNpcY) * 1024;
	
	// 判断是否点在阻挡内
	if ( !Npc[nIndex].GetPathFinder().CheckBarrier(offsetx, offsety) )
	{
		g_DestPosIdx = ObjSet.ClientAdd(0, 415, 0, 0, 0, nnX, nnY, sInfo);
	}
	else
	{
		return;
	}
}

void KCoreShell::RemoveMovePosition()
{
	if ( g_DestPosIdx != -1 )
		ObjSet.RemoveIfClientOnly(g_DestPosIdx);

	g_DestPosIdx = -1;
}

void KCoreShell::SetMusic( bool b )
{
	g_SubWorldSet.SetMusic( b );
}

void KCoreShell::Move( int direction, int distance )
{
	if (direction < 0 || direction > 7 || distance <= 0)
		return;

	int newPosX = g_GetScreenWidth() / 2;
	int newPosY = g_GetScreenHeight() / 2;

	switch(direction)
	{
	case 0://上
		newPosY -= distance;
		break;
	case 1://右上
		newPosX += distance * 2;
		newPosY -= distance;
		break;
	case 2://右
		newPosX += distance * 2;
		break;
	case 3://右下
		newPosX += distance * 2;
		newPosY += distance;
		break;
	case 4://下
		newPosY += distance;
		break;
	case 5://左下
		newPosX -= distance * 2;
		newPosY += distance;
		break;
	case 6://左
		newPosX -= distance * 2;
		break;
	case 7://左上
		newPosX -= distance * 2;
		newPosY -= distance;
		break;
	}

	GotoWhere(newPosX, newPosY, 0);
}

void KCoreShell::SelectSkill( int skillId )
{
	if (skillId > 0)
	{
		skillId = Npc[GetClientPlayer().GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(skillId);
		PlayerController::Singleton().SetSelectedSkill(skillId);
	}
	else
	{
		PlayerController::Singleton().SetSelectedSkill(INVALID_SKILL_ID);
	}
}

void KCoreShell::NextSkill( int skillId, bool targetSelf )
{
	POINT curPt;
	if(!GetCursorPos(&curPt))
		return;

	if(!ScreenToClient(g_GetMainHWnd(), &curPt))
		return;

  	int posX = curPt.x;
  	int posY = curPt.y;
  	int z = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(posX, posY, z);


	if ( skillId != KNIGHT_NORMALSKILL_ID && 
		skillId != ENCHANTER_NORMALSKILL_ID &&
		skillId != MONSTROUS_NORMAILSKILL_ID )
	{
		skillId = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSkillList().GetCurSameSubSkillId(skillId);
	}

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	PlayerController& controller = PlayerController::Singleton();

	if(skillId > 0)
	{
		KSkill * pISkill =  g_SkillManager.GetSkill(skillId, 1);

		if (!pISkill) 
			return;

  		int nTargetIdx = Npc[nIndex].GetTargetNpc();
		int nAttackTargetType = pISkill->GetAttackTargetType();
			
		if( (nAttackTargetType & att_target_only) && !(nAttackTargetType & att_target_self) && nTargetIdx <= 0 )
		{
			int nNPCKind = 0;
			KUiPlayerItem SelectPlayer;
			if ( FindSelectNPC(curPt.x, curPt.y, relation_all, true, &SelectPlayer, nNPCKind, true) <= 0 )
			{
				// 没有找到目标
				CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)MSG_SKILL_NOTARGET, 0);
				return;
			}					
		}
		if(!(nAttackTargetType & att_target_self))
		{
			CFS_FILELOGS::WriteLog("if(!(nAttackTargetType & att_target_self)).\n");
			AutoRobotMgr::Singleton().SetMode(enRobotMode_None);
		}
		controller.SetNextSkill(skillId, posX, posY, targetSelf);
	}
	else
		controller.SetNextSkill(INVALID_SKILL_ID, 0, 0, false);
}

void KCoreShell::FollowAttack( void )
{
	int target = Npc[GetClientPlayer().GetNpcIndex()].GetTargetNpc();
	PlayerController::Singleton().FollowAttack(target);
}

void KCoreShell::Stop( void )
{
	PlayerController::Singleton().Stop();
}

void KCoreShell::PickupObject( int objectIndex )
{
	PlayerController::Singleton().PickupObject(objectIndex);
}

void KCoreShell::FollowDialog( void )
{
	int target = Npc[GetClientPlayer().GetNpcIndex()].GetTargetNpc();
	if ( Npc[GetClientPlayer().GetNpcIndex()].m_Doing == do_revive )
	{
		return;
	}
	PlayerController::Singleton().FollowDialog(target);
}

// void KCoreShell::UseSelectedSkill( int targetSelf )
// {
// 	POINT curPt;
// 	if(!GetCursorPos(&curPt))
// 		return;
// 	if(!ScreenToClient(g_GetMainHWnd(), &curPt))
// 		return;
//   	int posX = curPt.x;
//   	int posY = curPt.y;
//   	int z = 0;
// 	g_ScenePlace.ViewPortCoordToSpaceCoord(posX, posY, z);
// 
// 	PlayerController::Singleton().UseSelectedSkill(posX, posY, targetSelf);
// }

void ConfirmUseItem()
{
	ItemPos tmpPos;
	tmpPos.nPlace = g_UseItem.nPlace;
	tmpPos.nX = g_UseItem.nX;
	tmpPos.nY = g_UseItem.nY;
	ItemPos itemPos;
	memcpy( &itemPos, &g_uItemPos, sizeof(itemPos));
	Player[CLIENT_PLAYER_INDEX].ApplyUseItem(g_UseItem.uId, tmpPos, g_uItemID, itemPos );

	g_uItemID = 0;
	ZeroMemory( &g_uItemPos, sizeof(_ItemPos) );
	ZeroMemory( &g_UseItem, sizeof(_UseItem) );
	CoreDataChanged( GDCNI_SET_MOUSECURSOR, MOUSE_CURSOR_NORMAL, NULL );
}

void CannelUseItem()
{
	g_uItemID = 0;
	ZeroMemory( &g_uItemPos, sizeof(_ItemPos) );
	ZeroMemory( &g_UseItem, sizeof(_UseItem) );
	CoreDataChanged( GDCNI_SET_MOUSECURSOR, MOUSE_CURSOR_NORMAL, NULL );
}

void KCoreShell::ReplyPrompt(enumPromptEvent promptEvent, bool accept)
{
	GetClientPlayer().ReplyPrompt(prompt_event_add_buff, accept);
}

int		g_nLockItemByDateIdx = 0;
void LockItemByDate()
{
	if ( g_nLockItemByDateIdx > 0 && g_nLockItemByDateIdx< MAX_ITEM )
	{
		PLAYER_EAT_ITEM_COMMAND	sEat;
		sEat.ProtocolType = c2s_playereatitem;
		sEat.m_nItemID	= 0xff;
		sEat.m_btPlace	= 0xff;
		sEat.m_btX		= 0xff;
		sEat.m_btY		= 0xff;
		
		sEat.m_nTargetItemID = Item[g_nLockItemByDateIdx].GetID();
		sEat.m_btTargetPlace = 0xff;
		sEat.m_btTargetX	 = 0xff;
		sEat.m_btTargetY	 = 0xff;
		if (g_pClient)
			g_pClient->SendPackToServer(g_ConnectID,&sEat, sizeof(PLAYER_EAT_ITEM_COMMAND));
	}
	g_nLockItemByDateIdx = 0;
}

bool KCoreShell::CoreParseQuestionProtocol( BYTE* pMsg, UIQuestionData& uiQuestionData )
{
	return ParseQuestionProtocol( pMsg, uiQuestionData );
}