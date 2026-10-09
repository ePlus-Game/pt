//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-11
//      File_base        : relation_set
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 关系集
//
//////////////////////////////////////////////////////////////////////

#ifndef _RELATION_SET_H_
#define _RELATION_SET_H_

#include "GameDataDef.h"
#include <vector>
#include "SocialUtil.h"

class SocialUnit;

typedef struct tagRelationRecord
{
	tagRelationRecord()
	{
		memset(&ParentGuid, 0, sizeof(ParentGuid));
		pLeafUnit = NULL;
	}

	BYTE   TplId;
	FSGUID ParentGuid;
	SocialUnit* pLeafUnit;
} RelationRecord, *PRelationRecord;

typedef std::vector<RelationRecord> RelationRecordArray;

//关系集
class RelationSet
{
public:
	RelationSet();
	void Add(SocialUnit* pSocialUnit = NULL);//添加关系
	void Add(int nTplId, const FSGUID &parentGuid);
	void Remove(int nTplId);//删除关系
	void Clear();//清空关系
	RelationRecord* GetRelationByTemplate(int templateId);//得到关系（根据模版编号）
	bool Check(int nTplId);

#ifdef _SERVER
	bool IsOwnTreeLoad(int nTplId);
	void SetOwnTreeLoadFlag(int nTplId, bool bFlag);
#endif

private:
	RelationSet(const RelationSet &rhs);
	RelationSet& operator= (const RelationSet &rhs);

private:
	RelationRecordArray m_Relations;

#ifdef _SERVER
	bool	m_OwnTreeLoadFlag[enSUTplId_Num];
#endif
};

inline RelationSet::RelationSet()
{
	Clear();
}

inline void RelationSet::Clear()
{
	m_Relations.clear();

#ifdef _SERVER
	memset(m_OwnTreeLoadFlag, 0, sizeof(m_OwnTreeLoadFlag));
#endif
}

#ifdef _SERVER
inline bool RelationSet::IsOwnTreeLoad(int nTplId)
{
	if( IsTplIdValid(nTplId) )
		return m_OwnTreeLoadFlag[nTplId];
	else
		return true;
}

inline void RelationSet::SetOwnTreeLoadFlag(int nTplId, bool bFlag)
{
	if( IsTplIdValid(nTplId) )
		m_OwnTreeLoadFlag[nTplId] = bFlag;
}
#endif

#endif// _RELATION_SET_H_