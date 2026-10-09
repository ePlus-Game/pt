//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-16
//      File_base        : client_social_relation
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 客户端社会关系
//
//////////////////////////////////////////////////////////////////////

#ifndef _CLIENT_SOCIAL_RELATION_H_
#define _CLIENT_SOCIAL_RELATION_H_

#include "SocialComDef.h"

typedef struct tagClientRelationPrivilege
{
	tagClientRelationPrivilege()
	{
		Layer = 0;
		OperationId = 0;
	}

	int Layer;
	int OperationId;
} ClientRelationPrivilege, *PClientRelationPrivilege;

typedef struct tagClientRelationInfo
{
	tagClientRelationInfo()
	{
		TopLayer = 0;
		PrivilegeCount = 0;
		memset(Names, 0, sizeof(Names));
	}

	int TopLayer;
	int PrivilegeCount;
	ClientRelationPrivilege Privileges[enSUO_Num * MAX_SOCIETY_LAYER_COUNT];
	char Names[MAX_SOCIETY_LAYER_COUNT][17];
} ClientRelationInfo, *PClientRelationInfo;

class ClientSocialRelation
{
public:
	ClientSocialRelation();
	~ClientSocialRelation();
	
	void RecvSocialRelationSync(const BYTE* pMsg);//收到服务器的同步信息
	const PClientRelationInfo GetRelationInfo(int templateId);//得到关系信息
	bool CheckPrivilege(int templateId, int layer, int operationId);//检查权限
	void Clean();
	
private:
	ClientRelationInfo m_RelationInfos[enSUTplId_Num];
};

#endif// _CLIENT_SOCIAL_RELATION_H_