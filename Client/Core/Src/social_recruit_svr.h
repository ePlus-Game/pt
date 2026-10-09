#ifndef K_SOCIAL_RECRUIT_SVR_H
#define K_SOCIAL_RECRUIT_SVR_H
#pragma warning( disable : 4786 )

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/30/2007 11:35
//      File_base        : social_recruit_svr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 社会关系招募系统
//
//      <Change_list>   
//////////////////////////////////////////////////////////////////////

#include "SocialComDef.h"
#include <map>
#include <list>

//DB Used ...........................................


struct _SOCIAL_RECRUIT_DB:public _DBProcHeader 
{ 
    SocialRecruitBaseInfo info;
	int                   ope;
	int                   nLauncherIdx;
};

enum  SR_DB_OPE
{
   SR_DBOpe_Add = 1,
   SR_DBOpe_Del,
   SR_DBOpe_lst,
   SR_DBOpe_end
};

enum	enSocialRecruitCol
{
	    enSDBRecruit_Col_None = -1,
		enSDBRecruit_Col_ID,
		enSDBRecruit_Col_Time,
		enSDBRecruit_Col_Num,
};

//...................................................

#define SR_INFO_CHECK_INTERVAL 300

bool  SocialRcruitCmp(const SocialRecruitInfo & info1,const SocialRecruitInfo & info2);

class KSocialRecruitMgr
{
	std::map<FSGUID,SocialRecruitBaseInfo> mRecruitInfos;
	std::list<SocialRecruitInfo>           mSortedGensInfo;
	std::list<SocialRecruitInfo>           mSortedTongInfo;

	bool                                   mInited;
	unsigned long                          mLastCheckTime;

	std::list<FSGUID>                      mLoadingUnitInfos;
public:
	static KSocialRecruitMgr & Singlton(void);
    ~KSocialRecruitMgr();
public:
	void        Init(void);
	void        Release(void);
public:
	void        ProcessGlobalDBRet(
		        int nDbOpeRst, 
		        IProcRet* pRet
		);

public:
	int         AddInfo(const FSGUID & guid,const int nPlayerIndex);
	int         DelInfo(const FSGUID & guid,const int nPlayerIndex);
	int         GetInfoList(const int iStartPage,
		                    const int nPageSize,
							const int nLayer,
							const int nBuffSize,
							S2C_GETRECRUIT_RET * pBuff);

	void        Breathe(void);
private:
	KSocialRecruitMgr();
	int         ReqDBAddInfoRecord(const SocialRecruitBaseInfo & info,const int nPlayerIndex);
	int         ReqDBDelInfoRecord(const SocialRecruitBaseInfo & info,const int nPlayerIndex);
	int         ReqDBInfoList(void);

private:
	int         AddInfoDirectly(const SocialRecruitBaseInfo & srcInfo );
	int         DelInfoDirectly(const FSGUID & guid);
	int         DelInfoDirectly(std::map<FSGUID,SocialRecruitBaseInfo>::iterator it);
	void        CheckSortedList(std::list<SocialRecruitInfo> & destList);
	int         RefreshInfo(SocialRecruitInfo & info,const SocialRecruitBaseInfo & baseInfo);
	void        ProcessListData(const int nRow,IProcRet* pRet);   
	void        AddToSortedList(const SocialRecruitInfo & info,std::list<SocialRecruitInfo> & destList);
	void        CheckLoadingUnit(void );
	bool        DelFromSortedList(const FSGUID & guid , std::list<SocialRecruitInfo> & destList);
	void        CheckInvalid(void);
	bool        IsLoading(const FSGUID & guid);
};


#endif
