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

#ifndef _SERVER

#include "QueryManager.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
int CreateQueryManager( IQueryManager** queryManager )
{
	static QueryManager g_QueryManager;
	if ( queryManager == NULL )
	{		
		return query_errorparam;
	}
	*queryManager = &g_QueryManager;
	if ( *queryManager == NULL )
	{
		return query_notenoughmem;
	}
	return query_succeed;
}

int ReleaseQueryManager( IQueryManager** queryManager )
{
	return query_succeed;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

QueryManager::QueryManager( void )
{
	memset( d_InfoTab, 0, sizeof(d_InfoTab) );
	InitTab();
}

QueryManager::~QueryManager( void )
{
	if ( RoleInfo::getSingletonPtr() )
	{
		delete RoleInfo::getSingletonPtr();
	}
	if ( SkillsInfo::getSingletonPtr() )
	{
		delete SkillsInfo::getSingletonPtr();
	}	
	if ( NpcInfo::getSingletonPtr() )
	{
		delete NpcInfo::getSingletonPtr();
	}
	if ( QuestInfo::getSingletonPtr() )
	{
		delete QuestInfo::getSingletonPtr();
	}
	if ( ItemInfo::getSingletonPtr() )
	{
		delete ItemInfo::getSingletonPtr();
	}
	if ( FaintnessInfo::getSingletonPtr() )
	{
		delete FaintnessInfo::getSingletonPtr();
	}
	if ( MapsInfo::getSingletonPtr() )
	{
		delete MapsInfo::getSingletonPtr();
	}
	/*for ( int nIdx = faintness_query; nIdx < querytype_count; ++nIdx )
	{
		if ( d_InfoTab[nIdx] != NULL )
		{
			delete d_InfoTab[nIdx];
			d_InfoTab[nIdx] = NULL;
		}
	}//*/
}

int QueryManager::InitTab( void )
{
	if ( d_InfoTab[quest_query] == NULL )
	{
		d_InfoTab[quest_query] = new QuestInfo;
	}
	if ( d_InfoTab[maps_query] == NULL )
	{
		d_InfoTab[maps_query] = new MapsInfo;
	}
	if ( d_InfoTab[item_query] == NULL )
	{
		d_InfoTab[item_query] = new ItemInfo;
	}
	if ( d_InfoTab[skill_query] == NULL )
	{
		d_InfoTab[skill_query] = new SkillsInfo;
	}
	if ( d_InfoTab[npc_query] == NULL )
	{
		d_InfoTab[npc_query] = new NpcInfo;
	}
	if ( d_InfoTab[player_query] == NULL )
	{
		d_InfoTab[player_query] = new RoleInfo;
	}
	if ( d_InfoTab[faintness_query] == NULL )
	{
		d_InfoTab[faintness_query] = new FaintnessInfo;
	}
	for ( int nIdx = faintness_query; nIdx < querytype_count; ++nIdx )
	{
		if ( d_InfoTab[nIdx] != NULL )
		{
			d_InfoTab[nIdx]->InitIndex();
		}
	}
	return query_succeed;
}

int QueryManager::QueryRequest( IQueryResult** result, const std::string& info,  QueryType type )
{
	if ( result == NULL ) 
	{
		return query_errorparam; 
	}
	if ( info.empty() ) 
	{
		return query_errorparam; 
	}
	if ( type < faintness_query || type >= querytype_count) 
	{
		return query_errorquerytype; 
	}
	if ( d_InfoTab[type] == NULL ) 
	{
		return query_notfindtable; 
	}

	return d_InfoTab[type]->QueryRequest( result, info );
}

int QueryManager::QueryRequest(	IQueryResult** result, int id, QueryType type )
{
	if ( result == NULL ) 
	{
		return query_errorparam; 
	}
	if ( id < 0 ) 
	{
		return query_errorqueryid; 
	}
	if ( type < faintness_query || type >= querytype_count) 
	{
		return query_errorquerytype; 
	}
	if ( d_InfoTab[type]  == NULL ) 
	{
		return query_notfindtable; 
	}
	
	return d_InfoTab[type]->QueryRequest( result, id );
}

#endif

