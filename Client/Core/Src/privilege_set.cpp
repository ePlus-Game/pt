//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-11
//      File_base        : privilege_set
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 关系系统权限集
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "privilege_set.h"
#include "SocialComDef.h"

#ifdef _SERVER
#include "SocialUtil.h"
#endif

PrivilegeSet::PrivilegeSet()
#ifdef _SERVER
:m_PrivilegeVersion(CURRENT_SOCIALDATA_PRIV_VERSIONNO)
#endif
{
}

PrivilegeSet::~PrivilegeSet()
{
	m_Privileges.clear();
}

void PrivilegeSet::Add(int templateId, int layer, int operationId)
{	
	if (Check(templateId, layer, operationId))//权限已经存在，不需要添加了
	{
		return;
	}
	
	RelationPrivilege privilege;
	privilege.TemplateId = templateId;
	privilege.Layer = layer;
	privilege.OperationId = operationId;

	m_Privileges.push_back(privilege);
}

void PrivilegeSet::Remove(int templateId, int layer, int operationId)
{
	if (templateId <= enSUTplId_None || layer <= enSULayer_None || operationId <= enSUO_None)
	{
		_ASSERT(false);
		return;
	}

	RelationPrivilegeArray::iterator iterCurr = m_Privileges.begin();
	RelationPrivilegeArray::iterator iterEnd = m_Privileges.end();

	while (iterCurr != iterEnd)
	{
		const RelationPrivilege& privilege = *iterCurr;
		if ( (privilege.TemplateId == templateId)
			&& (privilege.Layer == layer)
			&& (privilege.OperationId == operationId) )
		{
			break;
		}

		++iterCurr;
	}

	if (iterCurr != iterEnd)
	{
		m_Privileges.erase(iterCurr);
	}
}

void PrivilegeSet::Remove(int templateId, int layer)
{
	RelationPrivilegeArray::iterator iter;

	for(iter = m_Privileges.begin(); iter != m_Privileges.end(); ++iter)
	{
		if(iter->TemplateId == templateId && iter->Layer == layer)
		{
			m_Privileges.erase(iter);
			--iter;
		}
	}
}

void PrivilegeSet::Clear()
{
	m_Privileges.clear();
}

bool PrivilegeSet::Check(int templateId, int layer, int operationId)
{
	RelationPrivilegeArray::iterator iterCurr = m_Privileges.begin();
	RelationPrivilegeArray::iterator iterEnd = m_Privileges.end();

	while (iterCurr != iterEnd)
	{
		const RelationPrivilege& privilege = *iterCurr;
		if (privilege.TemplateId == templateId
			&& privilege.Layer == layer
			&& privilege.OperationId == operationId)
		{
			return true;
		}

		++iterCurr;
	}
	
	return false;
}

#ifdef _SERVER

DWORD PrivilegeSet::GetPrivilegeVersion()const
{
	return m_PrivilegeVersion;
}

 void PrivilegeSet::SetPrivilegeVersion(DWORD dwNewVersion)
{
	_ASSERT(dwNewVersion <= CURRENT_SOCIALDATA_PRIV_VERSIONNO);
	m_PrivilegeVersion = dwNewVersion;
}

int PrivilegeSet::Save(char* pBuf, int nBufSize)
{
	_ASSERT(pBuf);
	if(NULL == pBuf)
		return 0;

	DB_UNIT_PRIV	sizeCalc;

	if(nBufSize < sizeCalc.size(0))
	{
		_ASSERT(false);
		return 0;
	}

	DB_UNIT_PRIV	*pPriv = (DB_UNIT_PRIV*)pBuf;
	int				nUseSize = sizeCalc.size(0);
	
	pPriv->version   = CURRENT_SOCIALDATA_PRIV_VERSIONNO;
	pPriv->privCount = 0;

	RelationPrivilegeArray::iterator	itCur = m_Privileges.begin();
	RelationPrivilegeArray::iterator	itEnd = m_Privileges.end();
	RelationPrivilege	*pCurPriv = pPriv->priv;

	for(; itCur != itEnd; ++itCur)
	{
		if(nBufSize - nUseSize < sizeof(RelationPrivilege))
		{
			_ASSERT(0);
			break;
		}

#ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog("%d %d %d\n", 
									itCur->TemplateId, 
									itCur->Layer,
									itCur->OperationId
									);
#endif

		*pCurPriv = *itCur;
		++pCurPriv;
		nUseSize += sizeof(RelationPrivilege);
		++pPriv->privCount;
	}

#ifdef _DEBUG
	CFS_FILELOGS::WriteDebugLog("Count: %d\n", pPriv->privCount);
#endif

	return nUseSize;
}

bool PrivilegeSet::Load(const char* pBuf, int nDataSize)
{
	if(NULL == pBuf)
	{
		_ASSERT(false);
		return false;
	}

	if(nDataSize <= 0)
		return true;

	DB_UNIT_PRIV	*pPriv = (DB_UNIT_PRIV*)pBuf;
	
	if(pPriv->size() != nDataSize)
	{
		_ASSERT(false);
		return false;
	}

	_ASSERT(pPriv->version <= CURRENT_SOCIALDATA_PRIV_VERSIONNO);
	if (pPriv->version > CURRENT_SOCIALDATA_PRIV_VERSIONNO)
	{
		return false;
	}//endif

#ifdef _DEBUG
	CFS_FILELOGS::WriteDebugLog("Count: %d\n", pPriv->privCount);
#endif

	for(BYTE loop = 0; loop < pPriv->privCount; ++loop)
	{
		RelationPrivilege	*p = &(pPriv->priv[loop]);

		if( !IsLayerValid(p->TemplateId, p->Layer) )
		{
			_ASSERT(false);
			continue;
		}

		if( !IsOpeIdValid(p->OperationId) )
		{
			_ASSERT(false);
			continue;
		}

#ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog("%d %d %d\n", 
									p->TemplateId,
									p->Layer,
									p->OperationId
									);
#endif

		Add(p->TemplateId, p->Layer, p->OperationId);
	}

	m_PrivilegeVersion = pPriv->version;
	return true;
}

#endif