//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/20/2007 10:24
//      File_base        : QueryInfo
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _querymanager_h_
	#define _querymanager_h_

#ifndef _SERVER

#include "IQueryInfo.h"
#include "QueryInfo.h"

class QueryManager : public IQueryManager
{
	friend int CreateQueryManager( IQueryManager** queryManager );
	friend int ReleaseQueryManager( IQueryManager** queryManager );
private:
	QueryManager( void );
public:
	~QueryManager( void );
public:
	int InitTab( void );

	int QueryRequest(
		IQueryResult** result,
		const std::string& info, 
		QueryType type = faintness_query );

	int QueryRequest(
		IQueryResult** result,
		int id,
		QueryType type = faintness_query );
private:
	IQueryInfo* d_InfoTab[querytype_count];
};

#endif

#endif