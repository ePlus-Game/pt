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
#ifndef _queryinfo_h_
#define _queryinfo_h_

#ifndef _SERVER

#include "IQueryInfo.h"
#include "KPlayer.h"
#include "ConfigManager.h"
#include "KItemGenerator.h"
#include "KSkills.h"
#include "SkillManager.h"
#include "KNpcTemplate.h"
#include "KTabFile.h"
#include "KIniFile.h"

using namespace std;

template <typename T> class Singleton
{
protected:
    static T* ms_Singleton;

public:
    Singleton( void )
    {
        assert( !ms_Singleton );
        ms_Singleton = static_cast<T*>(this);
    }
   ~Singleton( void )
    {  
	   assert( ms_Singleton );  
	   ms_Singleton = 0;
	}
    static T& getSingleton( void )
        {  assert( ms_Singleton );  return ( *ms_Singleton );  }
    static T* getSingletonPtr( void )
        {  return ( ms_Singleton );  }
};

/************************************************************************/
/*							   Item Info                                */
/************************************************************************/
class ItemInfo;

class ItemResult : public IQueryResult
{
	friend class ItemInfo;
	friend class KBPT_Item;
public:
	ItemResult( void );
	~ItemResult( void );

public:
	int GetQueryResult(	
		int* resultCount, 
		void* resultArray,
		int resultSize,
		QueryResultType* resultType );
	
	void Release();

private:
	void GetFormatString( char* resultArray, int resultSize );
	void GetLayoutString( char* resultArray, int resultSize );

private:
	const KBASICPROP_ITEM*					d_itemTemplate;

	typedef vector<const KBASICPROP_ITEM*>	ItemArray;
	ItemArray								d_vecItem; 
	
};

class ItemInfo : public IQueryInfo, public Singleton<ItemInfo>
{
	friend class FaintnessInfo;
	friend class KBPT_Item;
public:
	ItemInfo( void );
	~ItemInfo( void );
public:
	int InitIndex( void ); 
	int QueryRequest( IQueryResult** result, const std::string& info );
	int QueryRequest( IQueryResult** result, int id );

private:
	ItemResult d_itemResult;
};

/************************************************************************/
/*                          Skill Info                                  */
/************************************************************************/
class SkillsInfo;

class SkillResult : public IQueryResult
{
	friend class SkillsInfo;
	friend class SkillManager;
public:
	SkillResult( void );
	~SkillResult( void );

public:
	int GetQueryResult(
		int* resultCount, 
		void* resultArray,  
		int resultSize,
		QueryResultType* resultType );

	void Release();

private:
	void GetFormatString( char* resultArray, int resultSize );
	void GetLayoutString( char* resultArray, int resultSize );

private:
	typedef vector<KSkill*>	SkillArray;
	SkillArray				d_vecSkill;
	KSkill*					d_skill;

};

class SkillsInfo : public IQueryInfo, public Singleton<SkillsInfo>
{
	friend class FaintnessInfo;
	friend class SkillManager;
public:
	SkillsInfo( void );
	~SkillsInfo( void );

public:
	int InitIndex( void ); 
	int QueryRequest( IQueryResult** result, const std::string& info );
	int QueryRequest( IQueryResult** result, int id );

private:
	SkillResult d_skillResult;
};

/************************************************************************/
/*                           Quest Info                                 */
/************************************************************************/
#define QUEST_TITLE_LEN 32
#define QUEST_CONST	6
#define QUEST_INFO_SETTINGS "\\settings\\questinfo.txt"

class QuestInfo;

struct _QuestBase 
{
	_QuestBase( void )
	{
		memset(Title, 0, sizeof(Title) );
	}
	/*
	_QuestBase( void )
	{
		taskid = 0;
		reqlevel = 0;
		reqmetier = 0;
		reqseries = 0;
		memset( CategoryName, 0, sizeof(CategoryName));
		memset(Title, 0, sizeof(Title) );
		displaytype = 0;
		beginnpcid = 0;
		memset( frondquestid, 0, sizeof(frondquestid));
		memset( behandquestid, 0, sizeof(behandquestid) );
	}
	_QuestBase(const _QuestBase& questBase )
	{
		taskid = questBase.taskid;
		reqlevel = questBase.reqlevel;
		reqmetier = 0;
		reqseries = 0;
		memcpy( CategoryName, 0, sizeof(CategoryName));
		memcpy(Title, Title, sizeof(Title) );
		displaytype = 0;
		beginnpcid = 0;
		memcpy( frondquestid, 0, sizeof(frondquestid));
		memcpy( behandquestid, 0, sizeof(behandquestid) );
	}//*/
	int		taskid;
	int		reqlevel;
	short	reqmetier;
	short	reqseries;
	char	CategoryName[QUEST_TITLE_LEN];
	char	Title[QUEST_TITLE_LEN];
	int		displaytype;
	int		beginnpcid;
	int		frondquestid[QUEST_CONST];
	int		behandquestid[QUEST_CONST];
};

struct _QuestObjective
{
	int dialognpcid[QUEST_CONST];
	int	killnpcid[QUEST_CONST];
	int	killnpccount[QUEST_CONST];
	int needitem[QUEST_CONST];
	int needitemcount[QUEST_CONST];
	int needquestid[QUEST_CONST];
};

struct _QuestAward
{
	int		endnpcid;
	UINT	money;
	UINT	exp;
	int		awarditem[QUEST_CONST];
	int		awarditemcount[QUEST_CONST];
	int		awarditemex[QUEST_CONST];
	int		awarditemexcount[QUEST_CONST];
};

struct QuestTab
{
	_QuestBase		base;
	_QuestObjective objective;
	_QuestAward		award[QUEST_CONST];
};

class QuestResult : public IQueryResult
{
	friend class QuestInfo;
public:
	QuestResult( void );
	~QuestResult( void );

public:
	int GetQueryResult(
		int* resultCount, 
		void* resultArray,
		int resultSize,
		QueryResultType* resultType );

	void Release();
	
private:
	void GetFormatString( char* resultArray );
	void GetLayoutString( char* resultArray );

private:
	typedef vector<QuestTab*>	QuestArray;
	QuestArray					d_vecQuestTab;
	QuestTab					d_questTab;
};

class QuestInfo : public IQueryInfo, public Singleton<QuestInfo>
{
	friend class MapsInfo;
	typedef map<int,QuestTab> _QuestIDIndex;
	typedef map<std::string,int> _QuestNameIndex;
public:
	QuestInfo( void );
	~QuestInfo( void );

public:
	int		InitIndex( void ); 
	int		QueryRequest( IQueryResult** result, const std::string& info );
	int		QueryRequest( IQueryResult** result, int id );
	void	QueryQuestInfo( int questid, QuestTab& questtab );

private:
	void	GetQuestInfo( int row, QuestTab& questtab );

private:
	QuestResult d_questResult;
	KTabFile	d_tabFile;
	_QuestIDIndex d_idIndex;
	_QuestNameIndex d_nameIndex;
};

/************************************************************************/
/*							  Npc Info                                  */
/************************************************************************/
class NpcInfo;

class NpcResult : public IQueryResult
{
	friend class NpcInfo;
public:
	NpcResult( void );
	~NpcResult( void );

public:
	int GetQueryResult(
		int* resultCount, 
		void* resultArray,
		int resultSize,
		QueryResultType* resultType );

	void Release();

private:
	void GetFormatString( char* resultArray );
	void GetLayoutString( char* resultArray );

private:
	typedef vector<KNpcTemplate*> NpcArray;
	NpcArray d_vecNpcTemplate;
	KNpcTemplate d_npcTemplate;
};

class NpcInfo : public IQueryInfo, public Singleton<NpcInfo>
{
	friend class FaintnessInfo;
	friend class NpcResult;
	typedef map<std::string,int> _NpcNameIndex;
public:
	NpcInfo( void );
	~NpcInfo( void );

public:
	int InitIndex( void ); 
	int QueryRequest( IQueryResult** result, const std::string& info );
	int QueryRequest( IQueryResult** result, int id );

private:
	NpcResult d_npcResult;
	KTabFile	d_tabFile;
	_NpcNameIndex d_nameIndex;
};

/************************************************************************/
/*							Role info                                 */
/************************************************************************/

#define ROLE_CONST 10
#define ROLE_INFO_SETTINGS "\\settings\\roleinfo.txt"

struct RoleTab 
{
	int		metier;
	int		series;
	short	level;
	int		displaytype;
	char	item[ROLE_CONST][16];
	int		skill[ROLE_CONST];
	int		quest[ROLE_CONST];
	int		maps[ROLE_CONST];
};

class RoleInfo;

class RoleResult : public IQueryResult
{
	friend class RoleInfo;
public:
	RoleResult( void );
	~RoleResult( void );

public:
	int GetQueryResult(
		int* resultCount, 
		void* resultArray,
		int resultSize,
		QueryResultType* resultType );

private:
	void GetFormatString( char* resultArray );

private:
	RoleTab d_roleTab;
};

class RoleInfo : public IQueryInfo, public Singleton<RoleInfo>
{
	friend class FaintnessInfo;
	typedef map<int,int> _RoleIDIndex;
public:
	RoleInfo( void );
	~RoleInfo( void );

public:
	int InitIndex( void ); 
	int QueryRequest( IQueryResult** result, const std::string& info );
	int QueryRequest( IQueryResult** result, int id );

private:
	void GetRoleInfo( int row, RoleTab& roletab );

private:
	RoleResult d_roleResult;
	KTabFile	d_tabFile;
	_RoleIDIndex d_idIndex;
};

/************************************************************************/
/*                          Maps Info                                  */
/************************************************************************/
#define MAPS_CONST 10

struct  MapsTab
{
	MapsTab( void )
	{
		type = 0;
	}
	MapsTab( const MapsTab& mapsTab )
	{
		name = mapsTab.name;
		type = mapsTab.type;
	}
	std::string name;
	int	type;
	vector<int> npc;
	vector<int> task;
};


class MapsInfo;

class MapsResult : public IQueryResult
{
	friend class MapsInfo;
public:
	MapsResult( void );
	~MapsResult( void );

public:
	int GetQueryResult(
		int* resultCount, 
		void* resultArray,  
		int resultSize,
		QueryResultType* resultType );

private:
	void GetLayoutString( char* resultArray, int resultSize );
	void GetFormatString( char* resultArray, int resultSize );

private:
	MapsTab d_mapTab;
};

class MapsInfo : public IQueryInfo, public Singleton<MapsInfo>
{
	typedef map<int,MapsTab> _MapsIDIndex;
	typedef map<std::string,int> _MapsNameIndex;
public:
	MapsInfo( void );
	~MapsInfo( void );

public:
	int		InitIndex( void ); 
	int		QueryRequest( IQueryResult** result, const std::string& info );
	int		QueryRequest( IQueryResult** result, int id );
	int		GetMapInfo( int id, MapsTab& mapsTab );

private:
	MapsResult		d_mapsResult;
	KIniFile		d_iniFile;
	KTabFile		d_tabFile;
	_MapsIDIndex	d_idIndex;
	_MapsNameIndex	d_nameIndex;	
};

/************************************************************************/
/*                          Faintness                                   */
/************************************************************************/
class FaintnessInfo;

class FaintnessResult : public IQueryResult
{
	friend class FaintnessInfo;
public:
	FaintnessResult( void );
	~FaintnessResult( void );

public:
	int GetQueryResult(
		int* resultCount, 
		void* resultArray,
		int resultSize,
		QueryResultType* resultType );
	
private:
	IQueryResult*	d_ItemInfo;
	IQueryResult*	d_NpcInfo;
	IQueryResult*	d_QuestInfo;
	IQueryResult*	d_SkillInfo;
	IQueryResult*	d_MapInfo;
};

class FaintnessInfo : public IQueryInfo, public Singleton<FaintnessInfo>
{
	typedef map<int,int> _FaintnessIDIndex;
public:
	FaintnessInfo( void );
	~FaintnessInfo( void );

public:
	int InitIndex( void ); 
	int QueryRequest( IQueryResult** result, const std::string& info );
	int QueryRequest( IQueryResult** result, int id );

private:
	FaintnessResult d_faintnessResult;
};

#endif

#endif