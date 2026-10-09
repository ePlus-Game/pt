//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/20/2007 10:25
//      File_base        : QueryInfo
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "ItemCommonDef.h"
#include "ConfigManager.h"
#ifndef _SERVER

#include "QueryInfo.h"

/************************************************************************/
/*							Tools function                              */
/************************************************************************/
void GetItemByString(int &nGenre, int &nDetail, int &nParticular, int &nLevel, char* sValue, int nStringSize)
{
	nGenre = 0; nDetail = 0; nParticular = 0; nLevel = 0; 
	if ( (sValue == NULL) || (nStringSize == 0) )
	{
		return;
	}

	int n = 0;
	int nRet[4] = {0, 0, 0, 0};
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
	
	nGenre = nRet[0]; nDetail = nRet[1]; nParticular = nRet[2]; nLevel = nRet[3];
}

void SysMoneyToUiMoney(int money, int& jin, int& yin, int& tong)
{
	jin = money / 10000;
	yin = (money % 10000) / 100;
	tong = money % 100; 
}

/************************************************************************/
/*								ItemInfo                                */
/************************************************************************/
ItemResult::ItemResult( void )
{
	d_itemTemplate = NULL;
}

ItemResult::~ItemResult( void )
{
	Release();
}

void ItemResult::Release()
{	
	d_vecItem.clear();
}

int ItemResult::GetQueryResult(
			int* resultCount, 
			void* resultArray, 
			int resultSize,
			QueryResultType* resultType)
{
	if ( d_itemTemplate == NULL && d_vecItem.empty() )
	{
		*resultCount = 0; 
		//return query_notfindrecord;
	}

	if ( *resultType == format_string )
	{
		GetFormatString( (char*)resultArray, resultSize );
	}
	else if ( *resultType == tip_string && d_itemTemplate )
	{
		GetLayoutString( (char*)resultArray, resultSize );
	}
	//*resultCount = 1;
	return query_succeed;	
}

void ItemResult::GetFormatString( char* resultArray, int resultSize )
{
	ConfigManager& cm = ConfigManager::Singleton();
	char szTemp[1024];
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_item_format_info_title);

	if ( szBuff )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	
	ItemArray::iterator it = d_vecItem.begin();
	int size = 0;

	while ( it != d_vecItem.end() )
	{
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_item_format_info, 
														(*it)->nColor);

		int id = GenerateItemHashId( (*it)->nItemGenre, 
									 (*it)->nDetailType, 
									 (*it)->nParticularType );
		if ( szBuff )
		{
			sprintf( szTemp, szBuff, (*it)->szImageSetName, 
					 (*it)->szImageName, id, (*it)->nLevel, 
					 (*it)->szName );

			strcat( resultArray, szTemp );
		}

		it++;
		if ( size < (MAX_SEARCH_RESULT/4) )
		{
			size++;
		}
		else
		{
			return;
		}
	}
}

void ItemResult::GetLayoutString( char* resultArray, int resultSize )
{
	KItem item;

	int nRet = g_ItemGen.Gen_Item( 
		d_itemTemplate->nItemGenre, 
		d_itemTemplate->nDetailType, 
		d_itemTemplate->nParticularType, 
		d_itemTemplate->nLevel, 
		1,
		&item );
	
	if ( nRet )
	{
		item.GetDesc( resultArray );
	}
}

template<> ItemInfo* Singleton<ItemInfo>::ms_Singleton	= NULL;

ItemInfo::ItemInfo( void )
{

}

ItemInfo::~ItemInfo( void )
{
}
int ItemInfo::InitIndex( void )
{
	return query_succeed;
}
	
int ItemInfo::QueryRequest(	IQueryResult** result, const std::string& info )
{
	d_itemResult.Release();

	d_itemResult.d_itemTemplate = NULL;

	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate( info.c_str() );

	if ( pItemTemplate && pItemTemplate->DisplayID > 0 )
	{
		return	query_notfindrecord;		 
	}
	
	if ( pItemTemplate == NULL )
	{
		return	query_notfindrecord;		 
	}
	d_itemResult.d_itemTemplate = pItemTemplate;
	*result = &d_itemResult;
	return query_succeed;
}

int ItemInfo::QueryRequest( IQueryResult** result, int id )
{
	int nGenre	= 0;
	int nDetailType	= 0;
	int nParticularType	= 0;
	int nLevel	= 0;
	SpliteHashId( id, nGenre, nDetailType, nParticularType );
	d_itemResult.d_itemTemplate = NULL;
	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate( 
		nGenre, 
		nDetailType, 
		nParticularType, 
		nLevel );
	
	if ( pItemTemplate && pItemTemplate->DisplayID > 0 )
	{
		return	query_notfindrecord;		 
	}

	if ( /*d_itemResult.d_itemTemplate */ pItemTemplate == NULL )
	{
		return query_notfindrecord;
	}
	d_itemResult.d_itemTemplate = pItemTemplate;
	*result = &d_itemResult;
	return query_succeed;
}

/************************************************************************/
/*                          Skill Info                                  */
/************************************************************************/
SkillResult::SkillResult( void )
{
}

SkillResult::~SkillResult( void )
{
	Release();
}

void SkillResult::Release()
{
	d_vecSkill.clear();
}

int SkillResult::GetQueryResult(
			int* resultCount, 
			void* resultArray,
			int resultSize,
			QueryResultType* resultType)
{
	if ( d_skill == NULL && d_vecSkill.empty() )
	{
		*resultCount = 0; 
		//return query_notfindrecord;
	}

	if ( *resultType == format_string )
	{
		GetFormatString( (char*)resultArray, resultSize );
	}
	else if ( *resultType == tip_string )
	{
		GetLayoutString( (char*)resultArray, resultSize );
	}
	//*resultCount = 1;
	return query_succeed;	
}

void SkillResult::GetFormatString( char* resultArray, int resultSize )
{
	ConfigManager& cm = ConfigManager::Singleton();
	char szTemp[1024];
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_skill_format_info_title);

	if ( szBuff )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	
	SkillArray::iterator it = d_vecSkill.begin();
	int size = 0;

	while ( it != d_vecSkill.end() )
	{
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_skill_format_info);

		if ( szBuff )
		{
			sprintf( szTemp, szBuff, (*it)->m_szSkillIcon, (*it)->GetSkillId(), 
				(*it)->m_szName, (*it)->GetCurLevel() );

			strcat( resultArray, szTemp );
		}

		it++;

		if ( size < (MAX_SEARCH_RESULT/4) )
		{
			size++;
		}
		else
		{
			return;
		}
	}
}

void SkillResult::GetLayoutString( char* resultArray, int resultSize )
{
	if ( d_skill )
	{
		g_SkillManager.GenDescWithSize( d_skill->GetSkillId(), d_skill->GetCurLevel(), resultArray, resultSize );
	}
}

template<> SkillsInfo* Singleton<SkillsInfo>::ms_Singleton	= NULL;

SkillsInfo::SkillsInfo( void )
{

}

SkillsInfo::~SkillsInfo( void )
{
}
int SkillsInfo::InitIndex( void )
{
	return query_succeed;
}
	
int SkillsInfo::QueryRequest( IQueryResult** result, const std::string& info )
{
	d_skillResult.Release();

	g_SkillManager.GetSkill(info.c_str());
	*result = &d_skillResult;
	return query_succeed;
}

int SkillsInfo::QueryRequest( IQueryResult** result, int id )
{
	int nLevel;
	d_skillResult.d_skill = g_SkillManager.GetSkill( id, &nLevel );
	if ( d_skillResult.d_skill == NULL )
	{
		return query_notfindrecord;
	}

	if ( d_skillResult.d_skill && d_skillResult.d_skill->m_nDisplayID > 0 )
	{
		return query_notfindrecord;
	}		

	*result = &d_skillResult;
	return query_succeed;
}

/************************************************************************/
/*                        Quest Info                                    */
/************************************************************************/
QuestResult::QuestResult( void )
{

}

QuestResult::~QuestResult( void )
{
	Release();
}

void QuestResult::Release()
{
	QuestArray::iterator it = d_vecQuestTab.begin();
	
	while ( it != d_vecQuestTab.end() )
	{
		if ( *it )
		{
			delete *it;
			*it = NULL;
		}
		it++;
	}
	
	d_vecQuestTab.clear();
}

int QuestResult::GetQueryResult(
			int* resultCount, 
			void* resultArray, 
			int resultSize,
			QueryResultType* resultType)
{
	if ( *resultType == format_string )
	{
		GetFormatString( (char*)resultArray );
	}
	else if (*resultType == tip_string && strcmp( d_questTab.base.Title, "" ) != 0)
	{
		GetLayoutString( (char*)resultArray );
	}
	//*resultCount = 1;
	return query_succeed;
}

void QuestResult::GetFormatString( char* resultArray )
{
	ConfigManager& cm = ConfigManager::Singleton();
	char szTemp[1024];
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_quest_format_info_title);

	if ( szBuff )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}

	QuestArray::iterator it = d_vecQuestTab.begin();
	int size = 0;

	while( it != d_vecQuestTab.end() )
	{
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_quest_format_info);

		if ( szBuff )
		{
			sprintf( szTemp, szBuff, (*it)->base.taskid, (*it)->base.Title );
			strcat( resultArray, szTemp );
		}

		it++;

		if ( size < (MAX_SEARCH_RESULT/4) )
		{
			size++;
		}
		else
		{
			return;
		}
	}

}

void QuestResult::GetLayoutString( char* resultArray )
{
	ConfigManager& cm = ConfigManager::Singleton();

	char szTemp[1024];
	//任务标题	
	int nLevelC = d_questTab.base.reqlevel - Player[CLIENT_PLAYER_INDEX].GetLevel();
	int nValue0 = cm.GetGlobalVariable(global_var_npc_level_displayer_value0);
	int nValue1 = cm.GetGlobalVariable(global_var_npc_level_displayer_value1);
	int nValue2 = cm.GetGlobalVariable(global_var_npc_level_displayer_value2);
	if ( nLevelC <= nValue0 )
	{
		nLevelC = 0;
	}
	else if ( (nValue0 < nLevelC) && (nLevelC <= nValue1) )
	{
		nLevelC = 1;
	}
	else if ( (nValue1 < nLevelC) && (nLevelC <= nValue2) )
	{
		nLevelC = 2;
	}
	else
	{
		nLevelC = 3;
	}
											 
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfobegintitle, nLevelC);
	if ( szBuff )
	{
		sprintf( szTemp, szBuff, d_questTab.base.CategoryName, d_questTab.base.Title );
		strcat( resultArray, szTemp );
	}	

	//接受条件
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questrequest);
	if ( szBuff )
	{
		sprintf( szTemp, szBuff, d_questTab.base.reqlevel );
		strcat( resultArray, szTemp );
	}	

	//接受npc
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfobeginnpc);
	if ( szBuff && d_questTab.base.beginnpcid > 0 && d_questTab.base.beginnpcid < MAX_NPCSTYLE )
	{
		KNpcTemplate npc;
		npc.InitNpcBaseData(d_questTab.base.beginnpcid);
		sprintf( szTemp, szBuff, npc.m_HeadImageSet, npc.m_HeadImage, d_questTab.base.beginnpcid, npc.Name );
		strcat( resultArray, szTemp );
	}
	int i = 0;
	//收集道具
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoneeditemtitle);
	if ( szBuff && d_questTab.objective.needitem[i] > 0 )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}

	for ( i = 0; i < QUEST_CONST; ++i )
	{
		int nItemGenre		= 0;
		int nDetailType		= 0;
		int nParticularType	= 0;
		int nLevel			= 0;
		SpliteHashId( d_questTab.objective.needitem[i], nItemGenre, nDetailType, nParticularType );
		KItem item;
		int nRet = g_ItemGen.Gen_Item( nItemGenre, nDetailType, nParticularType, nLevel, 1, &item );
		if ( nRet && !(nItemGenre == 0 && nDetailType == 0 && nParticularType == 0) )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoneeditem, item.GetQualityLabel());
			if ( szBuff )
			{
				std::string image = item.GetImageFile();
				if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0 )
				{
					image += EQUIPMENT_MAN_POSTFIX_SMALL;
				}
				else
				{
					image += EQUIPMENT_WOMAN_POSTFIX_SMALL;
				}
				sprintf( szTemp, szBuff, item.GetImageSetFile(), image.c_str(),d_questTab.objective.needitem[i], item.GetName(), d_questTab.objective.needitemcount[i] );		
				strcat( resultArray, szTemp );
			}
		}
	}

	//猎杀npc
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoneednpctitle);
	if ( szBuff && d_questTab.objective.killnpcid[i] > 0 && d_questTab.objective.killnpcid[i] <MAX_NPCSTYLE )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	i = 0;
	for ( i = 0; i < QUEST_CONST; ++i )
	{
		if ( d_questTab.objective.killnpcid[i] > 0 && d_questTab.objective.killnpcid[i] <MAX_NPCSTYLE )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle( style_questinfoneednpc );
			if ( szBuff )
			{
				KNpcTemplate npc;
				npc.InitNpcBaseData(d_questTab.objective.killnpcid[i]);
				sprintf( szTemp, szBuff, npc.m_HeadImageSet, npc.m_HeadImage, d_questTab.objective.needitem[i], 
					npc.Name, 
					d_questTab.objective.killnpccount[i] );		
				strcat( resultArray, szTemp );
			}
		}
	}

	//对话npc
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfodialognpctitle);
	if ( szBuff && d_questTab.objective.dialognpcid[i] > 0 && d_questTab.objective.dialognpcid[i] <MAX_NPCSTYLE )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	i = 0;
	for ( i = 0; i < QUEST_CONST; ++i )
	{
		if ( d_questTab.objective.dialognpcid[i] > 0 && d_questTab.objective.dialognpcid[i] <MAX_NPCSTYLE )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle( style_questinfodialognpc );
			if ( szBuff )
			{
				KNpcTemplate npc;
				npc.InitNpcBaseData(d_questTab.objective.dialognpcid[i] );
				sprintf( szTemp, szBuff, npc.m_HeadImageSet, npc.m_HeadImage, d_questTab.objective.dialognpcid[i], 
					npc.Name );
				strcat( resultArray, szTemp );
			}
		}
	}

	//交任务npc											 
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoendnpctitle);
	if ( szBuff )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}	
	for ( i = 0; i < QUEST_CONST; ++i )
	{
		if ( d_questTab.award[i].endnpcid <= 0 || d_questTab.award[i].endnpcid >= MAX_NPCSTYLE )
		{
			continue;
		}
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoendnpc);
		if ( szBuff )
		{
			KNpcTemplate npc;
			npc.InitNpcBaseData(d_questTab.award[i].endnpcid);
			sprintf( szTemp, szBuff, npc.m_HeadImageSet, npc.m_HeadImage, d_questTab.award[i].endnpcid, npc.Name );
			strcat( resultArray, szTemp );
		}

		//奖励金钱
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoawardmoney);
		if ( szBuff && d_questTab.award[i].money > 0  )
		{
			int jin = 0;
			int yin = 0;
			int tong = 0;
			SysMoneyToUiMoney( d_questTab.award[i].money, jin, yin, tong );
			sprintf( szTemp, szBuff, jin, yin, tong );
			strcat( resultArray, szTemp );
		}

		//奖励经验
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoawardexp);
		if ( szBuff && d_questTab.award[i].exp > 0 )
		{
			sprintf( szTemp, szBuff, d_questTab.award[i].exp );
			strcat( resultArray, szTemp );
		}
		int j = 0;
		//选择奖励物品
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoawardchoiceitemtitle);
		if ( szBuff && d_questTab.award[i].awarditem[0] > 0 )
		{
			sprintf( szTemp, szBuff );
			strcat( resultArray, szTemp );
		}

		for ( j = 0; j < QUEST_CONST; ++j )
		{
			int nItemGenre		= 0;
			int nDetailType		= 0;
			int nParticularType	= 0;
			int nLevel			= 0;
			SpliteHashId( d_questTab.award[i].awarditem[j], nItemGenre, nDetailType, nParticularType );
			KItem item;
			int nRet = g_ItemGen.Gen_Item( nItemGenre, nDetailType, nParticularType, nLevel, 1, &item );
			if ( nRet && !(nItemGenre == 0 && nDetailType == 0 && nParticularType == 0) )
			{
				szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoawardchoiceitem, item.GetQualityLabel());
				if ( szBuff )
				{
					std::string image = item.GetImageFile();
					if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0 )
					{
						image += EQUIPMENT_MAN_POSTFIX_SMALL;
					}
					else
					{
						image += EQUIPMENT_WOMAN_POSTFIX_SMALL;
					}
					sprintf( szTemp, szBuff, item.GetImageSetFile(), image.c_str(), d_questTab.award[i].awarditem[j], item.GetName(), d_questTab.award[i].awarditemcount[j] );		
					strcat( resultArray, szTemp );
				}
			}
		}

		//奖励物品
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoawarditemtitle);
		if ( szBuff && d_questTab.award[i].awarditemex[0] > 0 )
		{
			sprintf( szTemp, szBuff );
			strcat( resultArray, szTemp );
		}

		for ( j = 0; j < QUEST_CONST; ++j )
		{
			int nItemGenre		= 0;
			int nDetailType		= 0;
			int nParticularType	= 0;
			int nLevel			= 0;
			SpliteHashId( d_questTab.award[i].awarditemex[j], nItemGenre, nDetailType, nParticularType );
			KItem item;
			int nRet = g_ItemGen.Gen_Item( nItemGenre, nDetailType, nParticularType, nLevel, 1, &item );
			if ( nRet && !(nItemGenre == 0 && nDetailType == 0 && nParticularType == 0) )
			{
				szBuff = (char*)cm.GetConfigurableDisplayStyle(style_questinfoawarditem, item.GetQualityLabel());
				if ( szBuff )
				{
					std::string image = item.GetImageFile();
					if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0 )
					{
						image += EQUIPMENT_MAN_POSTFIX_SMALL;
					}
					else
					{
						image += EQUIPMENT_WOMAN_POSTFIX_SMALL;
					}
					sprintf( szTemp, szBuff, item.GetImageSetFile(), image.c_str(), d_questTab.award[i].awarditemex[j], item.GetName(), d_questTab.award[i].awarditemexcount[j] );		
					strcat( resultArray, szTemp );
				}
			}
		}

	}
}

template<> QuestInfo* Singleton<QuestInfo>::ms_Singleton	= NULL;

QuestInfo::QuestInfo( void )
{

}

QuestInfo::~QuestInfo( void )
{
}

int QuestInfo::InitIndex( void )
{
	BOOL nRet = d_tabFile.Load( QUEST_INFO_SETTINGS );
	if ( nRet == false || !d_idIndex.empty() || !d_nameIndex.empty() )
	{
		return query_errorcreateindex;
	}
	int nCount = d_tabFile.GetHeight();
	for ( int nIdx = 0; nIdx < nCount; ++nIdx )
	{
		int nQuestID = 0;
		d_tabFile.GetInteger( nIdx, "taskid", 0, &nQuestID );
		QuestTab questTab;
		GetQuestInfo( nIdx, questTab );
		d_idIndex[nQuestID] = questTab;
		char szTitle[QUEST_TITLE_LEN];
		d_tabFile.GetString( nIdx, "title", "", szTitle, sizeof(szTitle) );
		d_nameIndex[szTitle] = nQuestID;
	}
	return query_succeed;
}
	
int QuestInfo::QueryRequest( IQueryResult** result, const std::string& info )
{
	_QuestNameIndex::iterator it = d_nameIndex.begin();
	
	d_questResult.Release();

	while ( it != d_nameIndex.end() )
	{
		string temp = (*it).first;
		string::size_type i = temp.find( info.c_str() );

		if ( i != string::npos )
		{
			QuestTab* tab = new QuestTab;
			*tab = d_idIndex[(*it).second];
			d_questResult.d_vecQuestTab.push_back( tab );
		}
		it++;
	}

	*result = &d_questResult;
	return query_succeed;
}

int QuestInfo::QueryRequest( IQueryResult** result, int id )
{
	_QuestIDIndex::iterator it = d_idIndex.find( id );
	if ( it == d_idIndex.end() )
	{
		return query_notfindrecord;
	}
	d_questResult.d_questTab = (*it).second;
	*result = &d_questResult;
	return query_succeed;
}

void QuestInfo::GetQuestInfo( int row, QuestTab& questtab )
{
	int nCount = d_tabFile.GetHeight();
	if ( row > 0 && row < nCount )
	{
		// base info
		d_tabFile.GetInteger( row, "taskid", 0, &questtab.base.taskid );
		d_tabFile.GetInteger( row, "reqlevel", 0, (int*)&questtab.base.reqlevel );
		d_tabFile.GetInteger( row, "reqmetier", 0,(int*)&questtab.base.reqmetier );
		d_tabFile.GetInteger( row, "reqseries", 0, (int*)&questtab.base.reqseries );
		d_tabFile.GetString( row, "categoryname", "", questtab.base.CategoryName, sizeof(questtab.base.CategoryName) );
		d_tabFile.GetString( row, "title", "", questtab.base.Title, sizeof(questtab.base.Title) );
		d_tabFile.GetInteger( row, "beginnpcid", 0, &questtab.base.beginnpcid );
		d_tabFile.GetInteger( row, "displaytype", 0, (int*)&questtab.base.displaytype );
		d_tabFile.GetInteger( row, "frondtaskid1", 0, &questtab.base.frondquestid[0] );
		d_tabFile.GetInteger( row, "frondtaskid2", 0, &questtab.base.frondquestid[1] );
		d_tabFile.GetInteger( row, "frondtaskid3", 0, &questtab.base.frondquestid[2] );
		d_tabFile.GetInteger( row, "frondtaskid4", 0, &questtab.base.frondquestid[3] );
		d_tabFile.GetInteger( row, "frondtaskid5", 0, &questtab.base.frondquestid[4] );
		d_tabFile.GetInteger( row, "frondtaskid6", 0, &questtab.base.frondquestid[5] );
		d_tabFile.GetInteger( row, "behandtaskid1", 0, &questtab.base.behandquestid[0]	);
		d_tabFile.GetInteger( row, "behandtaskid2", 0, &questtab.base.behandquestid[1]	);
		d_tabFile.GetInteger( row, "behandtaskid3", 0, &questtab.base.behandquestid[2]	);
		d_tabFile.GetInteger( row, "behandtaskid4", 0, &questtab.base.behandquestid[3]	);
		d_tabFile.GetInteger( row, "behandtaskid5", 0, &questtab.base.behandquestid[4]	);
		d_tabFile.GetInteger( row, "behandtaskid6", 0, &questtab.base.behandquestid[5]	);
		// objective info
		d_tabFile.GetInteger( row, "dialognpcid1", 0, &questtab.objective.dialognpcid[0]	);
		d_tabFile.GetInteger( row, "dialognpcid2", 0, &questtab.objective.dialognpcid[1]	);
		d_tabFile.GetInteger( row, "dialognpcid3", 0, &questtab.objective.dialognpcid[2]	);
		d_tabFile.GetInteger( row, "dialognpcid4", 0, &questtab.objective.dialognpcid[3]	);
		d_tabFile.GetInteger( row, "dialognpcid5", 0, &questtab.objective.dialognpcid[4]	);
		d_tabFile.GetInteger( row, "dialognpcid6", 0, &questtab.objective.dialognpcid[5]	);
		d_tabFile.GetInteger( row, "killnpcid1", 0, &questtab.objective.killnpcid[0]	);
		d_tabFile.GetInteger( row, "killnpcid2", 0, &questtab.objective.killnpcid[1]	);
		d_tabFile.GetInteger( row, "killnpcid3", 0, &questtab.objective.killnpcid[2]	);
		d_tabFile.GetInteger( row, "killnpcid4", 0, &questtab.objective.killnpcid[3]	);
		d_tabFile.GetInteger( row, "killnpcid5", 0, &questtab.objective.killnpcid[4]	);
		d_tabFile.GetInteger( row, "killnpcid6", 0, &questtab.objective.killnpcid[5]	);
		d_tabFile.GetInteger( row, "killnpcid1count", 0, &questtab.objective.killnpccount[0]	);
		d_tabFile.GetInteger( row, "killnpcid2count", 0, &questtab.objective.killnpccount[1]	);
		d_tabFile.GetInteger( row, "killnpcid3count", 0, &questtab.objective.killnpccount[2]	);
		d_tabFile.GetInteger( row, "killnpcid4count", 0, &questtab.objective.killnpccount[3]	);
		d_tabFile.GetInteger( row, "killnpcid5count", 0, &questtab.objective.killnpccount[4]	);
		d_tabFile.GetInteger( row, "killnpcid6count", 0, &questtab.objective.killnpccount[5]	);
		d_tabFile.GetInteger( row, "needitem1", 0, &questtab.objective.needitem[0]	);
		d_tabFile.GetInteger( row, "needitem2", 0, &questtab.objective.needitem[1]	);
		d_tabFile.GetInteger( row, "needitem3", 0, &questtab.objective.needitem[2]	);
		d_tabFile.GetInteger( row, "needitem4", 0, &questtab.objective.needitem[3]	);
		d_tabFile.GetInteger( row, "needitem5", 0, &questtab.objective.needitem[4]	);
		d_tabFile.GetInteger( row, "needitem6", 0, &questtab.objective.needitem[5]	);
		d_tabFile.GetInteger( row, "needitem1count", 0, &questtab.objective.needitemcount[0]	);
		d_tabFile.GetInteger( row, "needitem2count", 0, &questtab.objective.needitemcount[1]	);
		d_tabFile.GetInteger( row, "needitem3count", 0, &questtab.objective.needitemcount[2]	);
		d_tabFile.GetInteger( row, "needitem4count", 0, &questtab.objective.needitemcount[3]	);
		d_tabFile.GetInteger( row, "needitem5count", 0, &questtab.objective.needitemcount[4]	);
		d_tabFile.GetInteger( row, "needitem6count", 0, &questtab.objective.needitemcount[5]	);
		d_tabFile.GetInteger( row, "needtaskid1", 0, &questtab.objective.needquestid[0]	);
		d_tabFile.GetInteger( row, "needtaskid2", 0, &questtab.objective.needquestid[1]	);
		d_tabFile.GetInteger( row, "needtaskid3", 0, &questtab.objective.needquestid[2]	);
		d_tabFile.GetInteger( row, "needtaskid4", 0, &questtab.objective.needquestid[3]	);
		d_tabFile.GetInteger( row, "needtaskid5", 0, &questtab.objective.needquestid[4]	);
		d_tabFile.GetInteger( row, "needtaskid6", 0, &questtab.objective.needquestid[5]	);
		//awardinfo
		for ( int i = 0; i < QUEST_CONST; ++i )
		{
			char szBuff[32];
			sprintf( szBuff, "end%dnpcid", i+1 );
			d_tabFile.GetInteger( row, szBuff, 0, &questtab.award[i].endnpcid );
			sprintf( szBuff, "money%d", i+1 );
			d_tabFile.GetInteger( row, szBuff, 0, (int*)&questtab.award[i].money );
			sprintf( szBuff, "exp%d", i+1 );
			d_tabFile.GetInteger( row, szBuff, 0, (int*)&questtab.award[i].exp );

			for ( int j = 0; j < QUEST_CONST; ++j )
			{
				sprintf( szBuff, "end%ditem%d", i+1, j+1 );
				d_tabFile.GetInteger( row, szBuff, 0, &questtab.award[i].awarditem[j] );
				sprintf( szBuff, "end%ditem%dcount", i+1, j+1 );
				d_tabFile.GetInteger( row, szBuff, 0, &questtab.award[i].awarditemcount[j] );
				sprintf( szBuff, "end%ditemex%d", i+1, j+1 );
				d_tabFile.GetInteger( row, szBuff, 0, &questtab.award[i].awarditemex[j] );
				sprintf( szBuff, "end%ditemex%dcount", i+1, j+1 );
				d_tabFile.GetInteger( row, szBuff, 0, &questtab.award[i].awarditemexcount[j] );
			}
		}
	}
}

void  QuestInfo::QueryQuestInfo( int questid, QuestTab& questtab )
{
	_QuestIDIndex::iterator it = d_idIndex.find( questid );
	if ( it != d_idIndex.end() )
	{
		questtab = (*it).second;
	}
}

/************************************************************************/
/*						Npc Info                                        */
/************************************************************************/
NpcResult::NpcResult( void )
{

}

NpcResult::~NpcResult( void )
{
	Release();
}

void NpcResult::Release()
{
	NpcArray::iterator it = d_vecNpcTemplate.begin();
	
	while ( it != d_vecNpcTemplate.end() )
	{
		if ( *it )
		{
			delete *it;
			*it = NULL;
		}
		it++;
	}
	
	d_vecNpcTemplate.clear();
}

int NpcResult::GetQueryResult(
			int* resultCount, 
			void* resultArray, 
			int resultSize,
			QueryResultType* resultType)
{
	if ( strcmp( d_npcTemplate.Name, "" ) == 0 && d_vecNpcTemplate.empty() )
	{
		*resultCount = 0;
		//return query_notfindrecord;
	}

	if ( *resultType == format_string )
	{
		GetFormatString( (char*)resultArray );
	}
	else if ( *resultType == tip_string && strcmp( d_npcTemplate.Name, "" ) != 0 )
	{
		GetLayoutString( (char*)resultArray );
	}
	//*resultCount = 1;
	return query_succeed;
}

void NpcResult::GetFormatString( char* resultArray )
{	
	ConfigManager& cm = ConfigManager::Singleton();
	char szTemp[1024];
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_format_info_title);

	if ( szBuff )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}

	NpcArray::iterator it = d_vecNpcTemplate.begin();
	int size = 0;

	while( it != d_vecNpcTemplate.end() )
	{
		
		int idx = NpcInfo::getSingleton().d_nameIndex[(*it)->Name];
			
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_format_info);
		if (szBuff)
		{
			//sprintf( szTemp, szBuff, (*it)->m_HeadImageSet, (*it)->m_HeadImage, idx, (*it)->Name );
			sprintf( szTemp, szBuff, idx, (*it)->Name );
			strcat( resultArray, szTemp );
		}
		it++;

		
		if ( size < (MAX_SEARCH_RESULT/4) )
		{
			size++;
		}
		else
		{
			return;
		}
	}
	
}


void NpcResult::GetLayoutString( char* resultArray )
{
	ConfigManager& cm = ConfigManager::Singleton();
	char szTemp[COMMON_CLIENT_MSG_LEN_1024];
	ZeroMemory(szTemp, COMMON_CLIENT_MSG_LEN_1024);

	// Npc 图片
	/*char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_image);
	if ( szBuff )
	{
		sprintf( szTemp, szBuff, d_npcTemplate.m_HeadImageSet, d_npcTemplate.m_HeadImage );
		strcat( resultArray, szTemp );
	}//*/

	// Npc 名称
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_name, 0);
	/*if ( d_npcTemplate.m_nMapPos[0] != 0 )
	{
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_name, 1);
		sprintf( szTemp, szBuff, d_npcTemplate.m_nMapPos, d_npcTemplate.Name );
	}
	else
	{
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_name, 0);
		sprintf( szTemp, szBuff, d_npcTemplate.Name );
	}//*/
	if ( szBuff )
	{
		sprintf( szTemp, szBuff, d_npcTemplate.Name );
		strcat( resultArray, szTemp );
	}
	
	// Npc 基本信息
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_base);
	char* type = NULL;
	switch( d_npcTemplate.m_Kind )
	{
	case kind_dialoger:
		{
			type = (char*)cm.GetConfigurableDisplayStyle(style_npc_type, 0);
		}
		break;
	case kind_normal:
	case kind_creature:
		{
			type = (char*)cm.GetConfigurableDisplayStyle(style_npc_type, 1);
		}
		break;
	case kind_pet:
		{
			type = (char*)cm.GetConfigurableDisplayStyle(style_npc_type, 2);
		}
		break;
	case kind_guard:
		{
			type = (char*)cm.GetConfigurableDisplayStyle(style_npc_type, 3);
		}
		break;
	case kind_talisman:
		{
			type = (char*)cm.GetConfigurableDisplayStyle(style_npc_type, 4);
		}
		break;
	default:
		{
			type = (char*)cm.GetConfigurableDisplayStyle(style_npc_type, 5);
		}
		break;
	}
	if (type && szBuff)
	{
		if ( d_npcTemplate.m_nLevel < 1 || d_npcTemplate.m_nLevel > 120 )
		{
			sprintf( szTemp, szBuff, 120, type );
		}
		else
		{
			sprintf( szTemp, szBuff, d_npcTemplate.m_nLevel, type );
		}
		strcat( resultArray, szTemp );
	}

	// Npc 所在地图
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_map_title);
	if (szBuff)
		strcat( resultArray, szBuff );

	for ( int i = 0; i < 6; i++ )
	{
		if ( d_npcTemplate.m_nMapID[i] != 0 )
		{
			MapsTab tempMap;
			int nMapId, xPos, yPos;
			MapsInfo::getSingleton().GetMapInfo( d_npcTemplate.m_nMapID[i], tempMap );
			
			sscanf(d_npcTemplate.m_nMapPos[i], "gt=pos id=%d id1=%d id2=%d", &nMapId, &xPos, &yPos);

			szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_map, i);
			if (szBuff)
			{
				sprintf( szTemp, szBuff, d_npcTemplate.m_nMapID[i], tempMap.name.c_str(),
					d_npcTemplate.m_nMapPos[i], xPos, yPos );
				strcat( resultArray, szTemp );
			}
		}
	}

	// Npc 掉落物品（暂不开放）
	if ( d_npcTemplate.m_Kind == kind_creature )
	{
		szBuff = (char*)cm.GetConfigurableDisplayStyle(style_npc_info_dropitem_title);
		if (szBuff)
			strcat( resultArray, szBuff );
	}//*/

}

template<> NpcInfo* Singleton<NpcInfo>::ms_Singleton	= NULL;

NpcInfo::NpcInfo( void )
{

}

NpcInfo::~NpcInfo( void )
{
}

int NpcInfo::InitIndex( void )
{
	BOOL nRet = d_tabFile.Load( NPC_SETTING_FILE );
	if ( nRet == false )
	{
		return query_errorcreateindex;
	}
	int nCount = d_tabFile.GetHeight();
	for ( int nIdx = 0; nIdx < nCount; ++nIdx )
	{
		char szName[32];
		d_tabFile.GetString( nIdx, "Name", "", szName, sizeof(szName) );
		int nDisplayID = 0;
		d_tabFile.GetInteger( nIdx, "DisplayID", 0, &nDisplayID);
		if ( nDisplayID <= 0 )
		{
			d_nameIndex[szName] = nIdx-2;
		}
	
	}
	return query_succeed;
}
	
int NpcInfo::QueryRequest(	IQueryResult** result, const std::string& info )
{
	_NpcNameIndex::iterator it = d_nameIndex.begin();
	
	d_npcResult.Release();

	while ( it != d_nameIndex.end() )
	{
		string temp = (*it).first;
		string::size_type i = temp.find( info.c_str() );

		if ( i != string::npos )
		{
			d_npcResult.d_npcTemplate.InitNpcBaseData( (*it).second );
			KNpcTemplate* npcTemplate = new KNpcTemplate;
			npcTemplate->InitNpcBaseData( (*it).second );
			d_npcResult.d_vecNpcTemplate.push_back( npcTemplate );
			//*result = &d_npcResult;
			//return query_succeed;
		}
		else
		{
			KNpcTemplate* npcTemplate = new KNpcTemplate;
			if ( npcTemplate )
			{
				npcTemplate->InitNpcBaseData( (*it).second );
				string str = npcTemplate->m_nDes;
				string::size_type pos = str.find( info.c_str() );
				if ( pos != string::npos )
				{
					d_npcResult.d_vecNpcTemplate.push_back( npcTemplate );
					*result = &d_npcResult;
				}
				else
				{
					delete npcTemplate;
					npcTemplate = NULL;
				}
			}
		}
		it++;
	}

	//d_npcResult.d_npcTemplate.InitNpcBaseData( (*it).second );
	*result = &d_npcResult;
	return query_succeed;
}

int NpcInfo::QueryRequest( IQueryResult** result, int id )
{
	d_npcResult.d_npcTemplate.InitNpcBaseData( id );
	*result = &d_npcResult;
	return query_succeed;
}

/************************************************************************/
/*							  Role Info                               */
/************************************************************************/
RoleResult::RoleResult( void )
{
}

RoleResult::~RoleResult( void )
{

}

int RoleResult::GetQueryResult(
			int* resultCount, 
			void* resultArray,
			int resultSize,
			QueryResultType* resultType)
{
	GetFormatString( (char*)resultArray);
	*resultType = format_string;
	*resultCount = 1;
	return query_succeed;
}

void RoleResult::GetFormatString( char* resultArray )
{
	ConfigManager& cm = ConfigManager::Singleton();
	char szTemp[1024];
	char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_query_info_head, Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSex());
	if ( szBuff )
	{
		sprintf( szTemp, szBuff, Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].Name );
		strcat( resultArray, szTemp );
	}
	
	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_base);

	if ( szBuff )
	{
		switch( d_roleTab.metier )
		{
		case 1:
			sprintf( szTemp, szBuff, d_roleTab.level, 
				cm.GetConfigurableDisplayStyle(style_metier,d_roleTab.metier), 
				cm.GetConfigurableDisplayStyle(style_series_ds,d_roleTab.series) );
			break;
		case 2:
			sprintf( szTemp, szBuff, d_roleTab.level, 
				cm.GetConfigurableDisplayStyle(style_metier,d_roleTab.metier), 
				cm.GetConfigurableDisplayStyle(style_series_yr,d_roleTab.series) );
		    break;
		default:
			sprintf( szTemp, szBuff, d_roleTab.level, 
				cm.GetConfigurableDisplayStyle(style_metier,d_roleTab.metier), 
				cm.GetConfigurableDisplayStyle(style_series_js,d_roleTab.series) );
		    break;
		}
	}
	strcat( resultArray, szTemp );

	int i = 0;

	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_skill_title);
	if ( szBuff && d_roleTab.skill[0] > 0 )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	for ( i = 0; i< ROLE_CONST; ++i )
	{
		KSkill* pSkill = g_SkillManager.GetSkill( d_roleTab.skill[i] );
		if ( pSkill )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_skill);
			if ( szBuff )
			{
				sprintf( szTemp, szBuff, pSkill->m_szSkillIcon, pSkill->GetSkillId(), pSkill->m_szName );
				strcat( resultArray, szTemp );
			}
		}
	}

	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_item_title);
	if ( szBuff && d_roleTab.item[0] )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	for ( i = 0; i< ROLE_CONST; ++i )
	{
		int nItemGenre		= 0;
		int nDetailType		= 0;
		int nParticularType	= 0;
		int nLevel			= 0;
		GetItemByString( nItemGenre, nDetailType, nParticularType, nLevel, d_roleTab.item[i], sizeof(d_roleTab.item[i]) );
		KItem item;
		int nRet = g_ItemGen.Gen_Item( nItemGenre, nDetailType, nParticularType, nLevel, 1, &item );
		if ( nRet && !(nItemGenre == 0 && nDetailType == 0 && nParticularType == 0) )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_item, item.GetQualityLabel());
			if ( szBuff )
			{
				int itemid = GenerateItemHashId( nItemGenre, nDetailType, nParticularType );
				std::string image = item.GetImageFile();
				if ( Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetSex() == 0 )
				{
					image += EQUIPMENT_MAN_POSTFIX_SMALL;
				}
				else
				{
					image += EQUIPMENT_WOMAN_POSTFIX_SMALL;
				}

				sprintf( szTemp, szBuff, item.GetImageSetFile(), image.c_str(), itemid, item.GetName() );		
				strcat( resultArray, szTemp );
			}
		}
	}

	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_quest_title);
	if ( szBuff && d_roleTab.quest[0] )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	for ( i = 0; i< ROLE_CONST; ++i )
	{
		QuestTab  questTab;
		memset( &questTab, 0, sizeof(questTab) );
		QuestInfo::getSingleton().QueryQuestInfo( d_roleTab.quest[i], questTab );
		if ( questTab.base.taskid )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_quest);
			if ( szBuff )
			{
				sprintf( szTemp, szBuff, questTab.base.taskid, questTab.base.Title );	
				strcat( resultArray, szTemp );
			}
		}
	}

	szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_map_title);
	if ( szBuff && d_roleTab.maps[0] )
	{
		sprintf( szTemp, szBuff );
		strcat( resultArray, szTemp );
	}
	for ( i = 0; i< ROLE_CONST; ++i )
	{
		if ( d_roleTab.maps[i] )
		{
			szBuff = (char*)cm.GetConfigurableDisplayStyle(style_role_info_map);
			if ( szBuff )
			{
				MapsTab mapsTab;
				MapsInfo::getSingleton().GetMapInfo( d_roleTab.maps[i], mapsTab );
				sprintf( szTemp, szBuff, d_roleTab.maps[i], mapsTab.name.c_str() );	
				strcat( resultArray, szTemp );
			}
		}
	}
}

template<> RoleInfo* Singleton<RoleInfo>::ms_Singleton	= NULL;
RoleInfo::RoleInfo( void )
{

}

RoleInfo::~RoleInfo( void )
{

}

int RoleInfo::InitIndex( void )
{
	BOOL nRet = d_tabFile.Load( ROLE_INFO_SETTINGS );
	if ( nRet == false )
	{
		return query_errorcreateindex;
	}
	int nCount = d_tabFile.GetHeight();
	for ( int nIdx = 0; nIdx < nCount; ++nIdx )
	{
		int metier	= 0;
		int	series	= 0;
		int	level	= 0;
		d_tabFile.GetInteger( nIdx, "metier", 0, &metier );
		d_tabFile.GetInteger( nIdx, "series", 0, &series );
		d_tabFile.GetInteger( nIdx, "level", 0, &level );
		int key = level * 100 + metier * 10 + series;
		d_idIndex[key] = nIdx;
	}
	return query_succeed;
}
	
int RoleInfo::QueryRequest( IQueryResult** result, const std::string& info )
{
	int level = atoi( !info.empty() ? info.c_str() : "0" );
	if ( level <= 0 || level > MAX_LEVEL )
	{
		level = Player[CLIENT_PLAYER_INDEX].GetLevel();
	}
	return QueryRequest( result, level );
}

int RoleInfo::QueryRequest( IQueryResult** result, int id )
{
	int level = id;
	int metier = Npc[Player[CLIENT_PLAYER_INDEX].GetNpcIndex()].GetSeries();
	int series = Player[CLIENT_PLAYER_INDEX].GetSkillSeries() + 1;
	int key = level * 100 + metier * 10 + series;
	_RoleIDIndex::iterator it = d_idIndex.find( key );
	if ( it == d_idIndex.end() )
	{
		return query_notfindrecord;
	}
	GetRoleInfo((*it).second, d_roleResult.d_roleTab );
	*result = &d_roleResult;
	return query_succeed;
}

void RoleInfo::GetRoleInfo( int row, RoleTab& roletab )
{
	int nCount = d_tabFile.GetHeight();
	if ( row > 0 && row < nCount )
	{
		// base info
		d_tabFile.GetInteger( row, "metier", 0, (int*)&roletab.metier );
		d_tabFile.GetInteger( row, "series", 0, (int*)&roletab.series );
		d_tabFile.GetInteger( row, "level", 0,(int*)&roletab.level );
		d_tabFile.GetInteger( row, "displaytype", 0, (int*)&roletab.displaytype );
		
		//awardinfo
		for ( int i = 0; i < ROLE_CONST; ++i )
		{
			char szBuff[32];
			sprintf( szBuff, "item%d", i+1 );
			d_tabFile.GetString( row, szBuff, "", roletab.item[i], sizeof(roletab.item[i]) ); 
			sprintf( szBuff, "skill%d", i+1 );
			d_tabFile.GetInteger( row, szBuff, 0, &roletab.skill[i] );
			sprintf( szBuff, "task%d", i+1 );
			d_tabFile.GetInteger( row, szBuff, 0, &roletab.quest[i] );
			sprintf( szBuff, "map%d", i+1 );
			d_tabFile.GetInteger( row, szBuff, 0, &roletab.maps[i] );
		}
	}
}

/************************************************************************/
/*                          Maps Info                                  */
/************************************************************************/
MapsResult::MapsResult( void )
{

}

MapsResult::~MapsResult( void )
{

}

int MapsResult::GetQueryResult(	int* resultCount, void* resultArray,  int resultSize, QueryResultType* resultType )
{
	if ( *resultType == map_id )
	{
		if ( !d_mapTab.npc.empty() )
		{
			*resultType = map_id;
			std::vector<int>** pArray =  (std::vector<int>**)resultArray;
			*pArray = &d_mapTab.npc;
			*resultCount = d_mapTab.npc.size();
		}
	}
	else
	{
		GetFormatString((char*)resultArray, resultSize);
		*resultType = format_string;
		*resultCount = 1;
	}

	return query_succeed;
}

void MapsResult::GetFormatString( char* resultArray, int resultSize )
{
	ConfigManager& cm = ConfigManager::Singleton();
	if ( d_mapTab.npc.empty() )
	{
		return;
	}
	
	const char* title = cm.GetConfigurableDisplayStyle(style_map_quest_title, 0);
	if (title)
	{
		strcat( resultArray, title);
	}
	for ( int nIdx = 0; nIdx < MAPS_CONST; ++nIdx )
	{
		char szTemp[1024];
		char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_map_quest, 0);
		if ( szBuff && d_mapTab.npc[nIdx] > 0)
		{
			KNpcTemplate npc;
			npc.InitNpcBaseData(d_mapTab.npc[nIdx]);
			//sprintf( szTemp, szBuff, npc.m_HeadImageSet, npc.m_HeadImage, d_mapTab.npc[nIdx], npc.Name );
			sprintf( szTemp, szBuff, d_mapTab.npc[nIdx], npc.Name );
			strcat( resultArray, szTemp );
		}	
	}//*/
}

void MapsResult::GetLayoutString( char* resultArray, int resultSize )
{
	/*ConfigManager& cm = ConfigManager::Singleton();
	if ( d_mapTab.npc.empty() )
	{
		return;
	}
	for ( int nIdx = 0; nIdx < MAPS_CONST; ++nIdx )
	{
		char szTemp[1024];
		char* szBuff = (char*)cm.GetConfigurableDisplayStyle(style_map_quest, 0);
		if ( szBuff && d_mapTab.npc[nIdx] > 0)
		{
			KNpcTemplate npc;
			npc.InitNpcBaseData(d_mapTab.npc[nIdx]);
			sprintf( szTemp, szBuff, npc.m_HeadImageSet, npc.m_HeadImage, d_mapTab.npc[nIdx], npc.Name );
			strcat( resultArray, szTemp );
		}	
	}//*/
}
template<> MapsInfo* Singleton<MapsInfo>::ms_Singleton	= NULL;
MapsInfo::MapsInfo( void )
{

}

MapsInfo::~MapsInfo( void )
{

}

int MapsInfo::InitIndex( void )
{
	BOOL nRet = d_tabFile.Load( MAP_INFO_FILE );
	
	if ( nRet == false )
	{
		return query_errorcreateindex;
	}
	if ( d_iniFile.Load( MAP_LIST_SETTING ) && d_idIndex.empty() && d_nameIndex.empty() )
	{
		for ( int nID = 1; nID < MAX_MAP_TEMPLATE; ++nID )
		{
			int nHeight = d_tabFile.GetHeight() - 1;
			if ( nID <= nHeight )
			{
				//modif by xiehong(2008-4-1) -- fix bug and 愚人节快乐
				int mapId = 0;
				d_tabFile.GetInteger( nID + 1, "mapid", 0, &mapId );
				
				char szID[32];
				sprintf( szID, "%d", mapId );
				char szMap[64];
				d_iniFile.GetString( "List", szID, "", szMap, sizeof(szMap) );
				d_nameIndex[szMap] = mapId;
				
				MapsTab mapsTab;
				mapsTab.name = szMap;

				int nCount = d_tabFile.GetWidth() / 2;
				for ( int nIdx = 1; nIdx < nCount + 1; ++nIdx )
				{
					char szBuff[COMMON_CLIENT_MSG_LEN_64];

					int nNpcID = 0;				
					sprintf( szBuff, "npcid%d", nIdx - 1 );
					d_tabFile.GetInteger( nID + 1, szBuff, 0, &nNpcID );
					mapsTab.npc.push_back( nNpcID );

					int nTaskID = 0;
					sprintf( szBuff, "taskid%d", nIdx - 1 );
					d_tabFile.GetInteger( nID + 1, szBuff, 0, &nTaskID );
					mapsTab.task.push_back( nTaskID );
				}				

				if(mapId)
				{
					d_idIndex[mapId] = mapsTab;
				}
			}
		}
	}

	return query_succeed;
}
	
int MapsInfo::QueryRequest( IQueryResult** result, const std::string& info )
{
	_MapsNameIndex::iterator it = d_nameIndex.find( info );
	if ( it == d_nameIndex.end() )
	{
		return query_notfindrecord;
	}
	return QueryRequest(result, (*it).second );
}

int MapsInfo::QueryRequest( IQueryResult** result, int id )
{
	int ret = GetMapInfo(id, d_mapsResult.d_mapTab);
	*result = &d_mapsResult;
	return ret;
}


int MapsInfo::GetMapInfo( int id, MapsTab& mapsTab )
{
	_MapsIDIndex::iterator it = d_idIndex.find( id );
	if ( it != d_idIndex.end() )
	{
		mapsTab = (*it).second;
		return query_succeed;
	}
	else
	{
		return query_notfindrecord;
	}
}

/************************************************************************/
/*                          Faintness                                   */
/************************************************************************/
FaintnessResult::FaintnessResult( void )
: d_ItemInfo(NULL)
, d_NpcInfo(NULL)
, d_QuestInfo(NULL)
, d_SkillInfo(NULL)
, d_MapInfo(NULL)
{
}

FaintnessResult::~FaintnessResult( void )
{
}
	
int FaintnessResult::GetQueryResult(
			int* resultCount, 
			void* resultArray,
			int resultSize,
			QueryResultType* resultType)
{
	if ( !d_ItemInfo && !d_NpcInfo && !d_QuestInfo && !d_SkillInfo && !d_MapInfo )// && !d_RoleInfo)
	{
		*resultCount = 0; 
		return query_notfindrecord;
	}

	char temp[MAX_SEARCH_TEXT_LENGTH] = "";

	if ( d_ItemInfo )
	{
		if ( query_succeed == d_ItemInfo->GetQueryResult( resultCount, (void*)temp, resultSize, resultType ) )
		{
			int ret = query_succeed;
		}
	}
	if ( d_SkillInfo )
	{
		if ( query_succeed == d_SkillInfo->GetQueryResult( resultCount, (void*)temp, resultSize, resultType ) )
		{
			int ret = query_succeed;
		}
	} 
	if ( d_QuestInfo )
	{
		if ( query_succeed == d_QuestInfo->GetQueryResult( resultCount, (void*)temp, resultSize, resultType ) )
		{
			int ret = query_succeed;
		}
	}	
	if ( d_NpcInfo )
	{
		if ( query_succeed == d_NpcInfo->GetQueryResult( resultCount, (void*)temp, resultSize, resultType ) )
		{
			int ret = query_succeed;
		}
	}
	if ( d_MapInfo )
	{
		if ( query_succeed == d_MapInfo->GetQueryResult( resultCount, (void*)temp, resultSize, resultType ) )
		{
			int ret = query_succeed;
		}
	}
	
	*resultCount = 1;
	strcat( (char*)resultArray, temp );

	return 0;	
}

template<> FaintnessInfo* Singleton<FaintnessInfo>::ms_Singleton	= NULL;
FaintnessInfo::FaintnessInfo( void )
{
}

FaintnessInfo::~FaintnessInfo( void )
{
}

int FaintnessInfo::InitIndex( void )
{
	return query_succeed;
}

int FaintnessInfo::QueryRequest( IQueryResult** result, const std::string& info )
{
	int temp = query_succeed;
	int ret = query_unknown;

	temp = ItemInfo::getSingleton().QueryRequest( &d_faintnessResult.d_ItemInfo, info );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = NpcInfo::getSingleton().QueryRequest( &d_faintnessResult.d_NpcInfo, info );	
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = QuestInfo::getSingleton().QueryRequest( &d_faintnessResult.d_QuestInfo, info );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = SkillsInfo::getSingleton().QueryRequest( &d_faintnessResult.d_SkillInfo, info );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = MapsInfo::getSingleton().QueryRequest( &d_faintnessResult.d_MapInfo, info );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}

	*result = &d_faintnessResult;
	return ret;	
}

int FaintnessInfo::QueryRequest( IQueryResult** result,	int id )
{
	int temp = query_succeed;
	int ret = query_unknown;

	temp = ItemInfo::getSingleton().QueryRequest( &d_faintnessResult.d_ItemInfo, id );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = NpcInfo::getSingleton().QueryRequest( &d_faintnessResult.d_NpcInfo, id );	
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = QuestInfo::getSingleton().QueryRequest( &d_faintnessResult.d_QuestInfo, id );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = SkillsInfo::getSingleton().QueryRequest( &d_faintnessResult.d_SkillInfo, id );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}
	temp = MapsInfo::getSingleton().QueryRequest( &d_faintnessResult.d_MapInfo, id );
	if ( temp == query_succeed )
	{
		ret = query_succeed;
	}

	*result = &d_faintnessResult;
	return ret;
	
}

#endif
