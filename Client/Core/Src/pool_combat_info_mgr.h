#ifndef  K_POOL_COMBAT_INFO_H
#define  K_POOL_COMBAT_INFO_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 12/08/2007 14:25
//      File_base        : KPoolCombatInfoManager
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 分形池战争记录管理
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include <list>

#define FS_POOL_COMBAT_VERSION_NO          1

//Basic infomation struct declaration
#pragma	pack(push, 1)
typedef struct tagPoolCombatInfo
{
	int             mapID;
	FSGUID          invaderGUID;    

	int             warProTime;
    int             warState;      //the war state
}PoolCombatInfo;                   //version 1

#pragma pack(pop)

class KPoolCombatInfoLoader        //version 1 used
{
public:
	unsigned long Parse(const unsigned long dwVersion,PoolCombatInfo * lpDest,void * pBuff,const unsigned long dwSize);
};

//The value declaration for the war state
#define   FS_POOL_COMBAT_STATE_INVALID     0x00000000
#define   FS_POOL_COMBAT_STATE_PROCESS     0x00000001
#define   FS_POOL_COMBAT_STATE_END         0x00000002
#define   FS_POOL_COMBAT_TOTAL_STATE_NUM   FS_POOL_COMBAT_STATE_END + 1 

class KPoolCombatInfoManager
{
	std::list<PoolCombatInfo>   m_Infos;
	bool                        m_IsInited;
    long                        m_RecTimer;            

public:

	class SelfIterator
	{
		friend class KPoolCombatInfoManager;
		std::list<PoolCombatInfo>::iterator m_CurIterator;
	public:
	    SelfIterator(void);
	};

	friend class SelfIterator;
	
public:
	KPoolCombatInfoManager();
	~KPoolCombatInfoManager();
public:
	void Initialize(void);     // This Methods enable DBData read
	void Breathe(void);        // Save DB Data runtime
	bool IsInited(void)const;  // return true means that the db data is readed successful
public:

	bool AddRecord(const PoolCombatInfo & record);
	/*
	   If the Key pairs (invaderGUID,mapID) is already exist or db is not inited
	   return false otherwise return true
	*/
	bool DelRecord(const FSGUID  & invader , const int mapID);
	/*
	   If the Key pairs (invaderGUID,mapID) is not exist or db is not inited 
	   return false otherwise return true
	*/
	bool ChgState(const FSGUID &  invader, int mapID,  int newState);
	/*
	   If the Key pairs (invaderGUID,mapID) is not exist or db is not inited
	   return false otherwise return true
	*/

    PoolCombatInfo * GetRecord(const FSGUID & invader ,  int mapID);
	/*
	   If the Key pairs (invaderGUID,mapID) is not exist or db is not inited
	   return 0 otherwise return Pointer to the record.
	   Notice: DleRcord when someone is using the same record is not safe!
	*/

	PoolCombatInfo* NextRecord(const FSGUID &invader  ,  SelfIterator & iter);
	/* Return the Infomation which the invaderGUID is invader parameter 
	   This function stands for at iterator 
	   bRestart means enable searching from the first that is not continully from 
	   last call.
	*/

	PoolCombatInfo      *  NextRecord(SelfIterator & iter);
	void                   NotifyToSaveDB(void);
	bool                   ExistRecord(const int nMapID);
	bool                   ExistRecord(const FSGUID & invader);

public: //DB Operations
	void                                  LoadWarInfoGlobalDataRet(int nDBOpeRst, int nDataSize, unsigned char *pData);
    void                                  SaveFromDBOP(void);
private:
	std::list<PoolCombatInfo>::iterator   SearchRecord(const FSGUID  & invader , const int mapID);
    bool                                  CleanInvalidRecord(void);
	void                                  LoadFromDBOP(void);

private:
	KPoolCombatInfoManager(const KPoolCombatInfoManager &);
	const KPoolCombatInfoManager & operator = (const KPoolCombatInfoManager & );
};

KPoolCombatInfoManager & GetPoolCombatInfoManager(void);

#endif