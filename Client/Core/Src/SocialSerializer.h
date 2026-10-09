//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:19   15:29
//      File_base        : SocialSerializer
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#ifndef _SocialSerializer_h
#define _SocialSerializer_h

#include "CoreRelated.h"
#include "SocialComDef.h"

class SocialUnit;


class SocDBTaskList
{
public:
	SocDBTaskList()
	{
		memset(m_taskList, 0, sizeof(m_taskList));
	}

	bool	AddTask(const FSGUID &guid);
	void	RemoveTask(const FSGUID &guid);

private:
	enum {	MAXNUM_DBTASK = 18	};
	enum {	DBTASK_TIMEOUT = GAME_FPS * 3	};	// fps

	typedef	struct _DBTask
	{
		DWORD	startTime;
		FSGUID	id;
		bool	isInUse;
	} DBTask;

	DBTask	m_taskList[MAXNUM_DBTASK];
};

class SocialSerializer
{
public:
	static SocialSerializer&	Singleton();

	void	LoadTreeUpReq(int nNetId, int nTplId, const FSGUID &guid);
	void	LoadAllSubUnitReq(int nNetId, int nTplId, int nParentLayer, const FSGUID &parentGuid);

	void	UpdateRecordReq(int nNetId, SocialUnit *pUnit);
	void	AddRecordReq(int nNetId, SocialUnit *pUnit);
	void	RemoveRecordReq(int nNetId, SocialUnit *pUnit);

	void	UpdatePGuidReq(int nNetId, SocialUnit *pUnit);
	void	UpdateAttrReq(int nNetId, SocialUnit *pUnit);
	void	UpdateAppDataReq(int nNetId, SocialUnit *pUnit);
	void	UpdateSubUnitCntReq(int nNetId, SocialUnit *pUnit);

	void	CheckUnitNameReq(int nNetId, const C2S_CREATEUNIT_REQ &c2sReq);

	void	RemoveTask(const FSGUID &guid);

private:
	SocialSerializer() {};
	SocialSerializer(const SocialSerializer &rhs);
	SocialSerializer& operator= (const SocialSerializer &rhs);

	SocDBTaskList	m_dbTaskList;
};


struct _SocialDBHeader : _DBProcHeader 
{
	int nOp;
	int ntplId;
	FSGUID Guid;
	C2S_CREATEUNIT_REQ Req;
};

inline void	SocialSerializer::RemoveTask(const FSGUID &guid)
{
	m_dbTaskList.RemoveTask(guid);
}

#endif