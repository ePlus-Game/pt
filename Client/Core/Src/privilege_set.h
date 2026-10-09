//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-11
//      File_base        : privilege_set
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 关系系统权限集
//
//////////////////////////////////////////////////////////////////////

#ifndef _PRIVILEGE_SET_H_
#define _PRIVILEGE_SET_H_

#include <vector>

typedef std::vector<RelationPrivilege> RelationPrivilegeArray;

//权限集
class PrivilegeSet
{
public:
	PrivilegeSet();
	~PrivilegeSet();

	void Add(int templateId, int layer, int operationId);//添加权限
	void Remove(int templateId, int layer, int operationId);//删除权限
	void Remove(int templateId, int layer);//删除某个层次的所有权限
	void Clear();//清空权限
	bool Check(int templateId, int layer, int operationId);//检查权限

	int GetPrivilegeCount() const;
	const PRelationPrivilege GetPrivilege(int index);

#ifdef _SERVER
	int	  Save(char* pBuf, int nBufSize);//保存
	bool  Load(const char* pBuf, int nDataSize);//载入
	DWORD GetPrivilegeVersion(void)const;
	void  SetPrivilegeVersion(DWORD dwNewVersion);
#endif

private:

#ifdef _SERVER
	DWORD                  m_PrivilegeVersion;
#endif

	RelationPrivilegeArray m_Privileges;
};

inline int PrivilegeSet::GetPrivilegeCount() const
{
	return m_Privileges.size();
}

inline const PRelationPrivilege PrivilegeSet::GetPrivilege(int index)
{
#ifdef _SERVER
	_ASSERT(m_PrivilegeVersion == CURRENT_SOCIALDATA_PRIV_VERSIONNO);
#endif
	if (index < 0 || index >= m_Privileges.size())
	{
		return NULL;
	}

	return &(m_Privileges[index]);
}

#endif// _PRIVILEGE_SET_H_