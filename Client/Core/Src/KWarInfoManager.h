//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 09/03/2007 11:08
//      File_base        : KWarInfoManager
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef  K_WAR_INFO_MANAGER_H
#define  K_WAR_INFO_MANAGER_H 

#include <list>

#define  FS_WAR_INFO_VERSION_NO       1

#pragma	pack(push, 1)                  // Use pack because of struct version may change 
//Basic infomation struct declaration
typedef struct tagWarInfo
{
	FSGUID          invaderGUID;
	int             mapID;
	
	FSGUID          defenderGUID;  //Might be 0

	int             warDecTime;
	int             warProTime;
    int             warState;      //the war state
}FSWarInfo;                        //verion 1



#pragma  pack (pop)

class KWarInfoLoader
{
public:
	unsigned long  Parse(const unsigned long dwVersion,FSWarInfo * lpDest,void * pBuff,const unsigned long dwSize);
};

//The value declaration for the war state
#define   FS_WAR_STATE_INVALID     0x00000000
#define   FS_WAR_STATE_NOTIFY      0x00000001
#define   FS_WAR_STATE_PROCESS     0x00000002
#define   FS_WAR_STATE_END         0x00000003
#define   FS_WAR_TOTAL_STATE_NUM   FS_WAR_STATE_END + 1 

class KWarInfoManager
{
	std::list<FSWarInfo>   m_Infos;
	bool                   m_IsInited;
    long                   m_RecTimer;

public:


	class SelfIterator
	{
		friend class KWarInfoManager;
		std::list<FSWarInfo>::iterator m_CurIterator;
	public:
	    SelfIterator(void);
	};

	friend class SelfIterator;
	
public:
	KWarInfoManager();
	~KWarInfoManager();
public:
	void Initialize(void);     // This Methods enable DBData read
	void Breathe(void);        // Save DB Data runtime
	bool IsInited(void)const;  // return true means that the db data is readed successful
public:

	bool AddRecord(const FSWarInfo & record);
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

    FSWarInfo * GetRecord(const FSGUID & invader ,  int mapID);
	/*
	   If the Key pairs (invaderGUID,mapID) is not exist or db is not inited
	   return 0 otherwise return Pointer to the record.
	   Notice: DleRcord when someone is using the same record is not safe!
	*/

	FSWarInfo* NextRecord(const FSGUID &invader  ,  SelfIterator & iter);
	/* Return the Infomation which the invaderGUID is invader parameter 
	   This function stands for at iterator 
	   bRestart means enable searching from the first that is not continully from 
	   last call.
	*/

	FSWarInfo * GetRecordByMapId(int mapID);

	FSWarInfo      *  NextRecord(SelfIterator & iter);
	void              NotifyToSaveDB(void);
	const FSGUID   *  GetInvaderFromDefender(const FSGUID & defender);  
	bool              ExistWarInfo(const int nMap);
	bool              ExistWarInfo(const FSGUID & invader);
public: //DB Operations
	void                             LoadWarInfoGlobalDataRet(int nDBOpeRst, int nDataSize, unsigned char *pData);
	void                             SaveFromDBOP(void);
private:
	std::list<FSWarInfo>::iterator   SearchRecord(const FSGUID  & invader , const int mapID);
    bool                             CleanInvalidRecord(void);
	void                             LoadFromDBOP(void);


private:
	KWarInfoManager(const KWarInfoManager &);
	const KWarInfoManager & operator = (const KWarInfoManager & );
};

KWarInfoManager & GetGlobalWarInfoManager(void);

#endif