//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-16
//      File_base        : client_social_relation
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 客户端社会关系
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "CoreShell.h"
#include "client_social_relation.h"

ClientSocialRelation::ClientSocialRelation()
{
	memset(m_RelationInfos, 0, sizeof(m_RelationInfos));
}

ClientSocialRelation::~ClientSocialRelation()
{
}

void ClientSocialRelation::Clean()
{
	memset(m_RelationInfos, 0, sizeof(m_RelationInfos));
}

void ClientSocialRelation::RecvSocialRelationSync(const BYTE* pMsg)
{
	PSYNC_SOCIAL_RELATION       pPack          = (PSYNC_SOCIAL_RELATION)pMsg;
	PSYNC_SOCIAL_RELATION_INFO  pSyncRelation  = (PSYNC_SOCIAL_RELATION_INFO)pPack->data;
	const PClientRelationInfo   pLocalInfo     = GetRelationInfo(pSyncRelation->TemplateId);
	if (pLocalInfo != NULL)
	{		
		pLocalInfo->TopLayer = pSyncRelation->TopLayer;		
		pLocalInfo->PrivilegeCount = pSyncRelation->PrivilegeCount;
		for (int privilegeLoopCount = 0; privilegeLoopCount < pSyncRelation->PrivilegeCount; privilegeLoopCount++)
		{
			pLocalInfo->Privileges[privilegeLoopCount].Layer = pSyncRelation->Privileges[privilegeLoopCount].Layer;
			pLocalInfo->Privileges[privilegeLoopCount].OperationId = pSyncRelation->Privileges[privilegeLoopCount].OperationId;
		}
		memcpy(pLocalInfo->Names, pSyncRelation->Names, sizeof(pSyncRelation->Names));

		CoreDataChanged(GDCNI_UPDATA_TONG_MANAGER, NULL, NULL);
	}
}

const PClientRelationInfo ClientSocialRelation::GetRelationInfo(int templateId)
{
	if (templateId > enSUTplId_None && templateId < enSUTplId_Num)
	{
		return &(m_RelationInfos[templateId]);
	}
	else
	{
		return NULL;
	}	
}

bool ClientSocialRelation::CheckPrivilege(int templateId, int layer, int operationId)
{
	const PClientRelationInfo pInfo = GetRelationInfo(templateId);
	if (pInfo != NULL)
	{
		for (int privilegeLoopCount = 0; privilegeLoopCount < pInfo->PrivilegeCount; privilegeLoopCount++)
		{
			ClientRelationPrivilege& privilege = pInfo->Privileges[privilegeLoopCount];
			if (privilege.Layer == layer && privilege.OperationId == operationId)
			{
				return true;
			}
		}
	}

	return false;
}
