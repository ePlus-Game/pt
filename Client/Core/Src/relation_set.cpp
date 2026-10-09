//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-11
//      File_base        : relation_set
//      File_ext         : .cpp
//      Author           : ÐìÏþ¸Õ
//      Description      : ¹ØÏµ¼¯
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "relation_set.h"
#include "SocialUnit.h"

RelationRecord* RelationSet::GetRelationByTemplate(int templateId)
{
	RelationRecordArray::iterator iterCurr = m_Relations.begin();
	RelationRecordArray::iterator iterEnd = m_Relations.end();

	for(; iterCurr != iterEnd; ++iterCurr)
	{
		if(iterCurr->TplId == templateId)
			return &(*iterCurr);
	}

	return NULL;
}

void RelationSet::Add(int nTplId, const FSGUID &parentGuid)
{
	if( Check(nTplId) )
		return;

	RelationRecord	record;
	record.ParentGuid = parentGuid;
	record.TplId = (BYTE)nTplId;

	m_Relations.push_back(record);
}

void RelationSet::Add(SocialUnit* pSocialUnit/* = NULL*/)
{
	if(NULL == pSocialUnit)
		return;

	if ( Check(pSocialUnit->GetTplId()) )
	{
		return;
	}

	RelationRecord record;
	record.pLeafUnit = pSocialUnit;
	record.TplId = pSocialUnit->GetTplId();

	m_Relations.push_back(record);
}

void RelationSet::Remove(int nTplId)
{
	RelationRecordArray::iterator iterCurr = m_Relations.begin();
	RelationRecordArray::iterator iterEnd = m_Relations.end();

	for(; iterCurr != iterEnd; ++iterCurr)
	{
		if(iterCurr->TplId == nTplId)
		{
			m_Relations.erase(iterCurr);
			break;
		}
	}
}

bool RelationSet::Check(int nTplId)
{
	RelationRecordArray::iterator iterCurr = m_Relations.begin();
	RelationRecordArray::iterator iterEnd = m_Relations.end();

	while (iterCurr != iterEnd)
	{
		if ((*iterCurr).TplId == nTplId)
		{
			return true;
		}

		++iterCurr;
	}

	return false;
}
