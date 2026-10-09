//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/20/2007 9:58
//      File_base        : IQueryInfo
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _iqueryinfo_h_
#define _iqueryinfo_h_

#ifndef _SERVER

#include <string>
#include <map>
#include <vector>

#define MAX_SEARCH_RESULT 100
#define MAX_SEARCH_TEXT_LENGTH (1024 * MAX_SEARCH_RESULT)

enum QueryErrorCode
{
	query_succeed,
	query_unknown,
	query_notfindrecord,
	query_notfindtable,
	query_errorindex,
	query_errorinittab,
	query_errorparam,
	query_notenoughmem,
	query_errorquerytype,
	query_errorqueryid,
	query_errorcreateindex,
};

enum QueryType
{
	faintness_query,
	item_query,
	skill_query,
	quest_query,
	npc_query,
	player_query,
	maps_query,
	querytype_count,
};

enum QueryResultType
{
	format_string,
	tip_string,	
	map_id,
	screeneffect_id,
	queryresulttype_count,
};

struct IQueryResult 
{
	virtual int GetQueryResult(
		int* resultCount, 
		void* resultArray,  
		int resultSize,
		QueryResultType* resultType ) = 0;	
};

struct IQueryInfo 
{
	virtual	int InitIndex( void ) = 0; 
	virtual int QueryRequest(
		IQueryResult** result,
		const std::string& info ) = 0;

	virtual int QueryRequest(
		IQueryResult** result,
		int id ) = 0;
};


struct IQueryManager 
{
	virtual	int InitTab( void ) = 0; 
	virtual int QueryRequest(
		IQueryResult** result,
		const std::string& info, 
		QueryType type = faintness_query ) = 0;

	virtual int QueryRequest(
		IQueryResult** result,
		int id,
		QueryType type = faintness_query ) = 0;
};

int CreateQueryManager( IQueryManager** queryManager );
int ReleaseQueryManager( IQueryManager** queryManager );

#endif

#endif

