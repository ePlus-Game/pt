#ifndef  K_EXP_INSURACE_MGR_H
#define  K_EXP_INSURACE_MGR_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 06/26/2008 11:43
//      File_base        : exp_insruance
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 文件功能描述
//
//      <Change_list>    : 升级保险系统
//
//////////////////////////////////////////////////////////////////////

#pragma  pack(push,1)

enum s2c_exp_insurance_subprotocol
{
	s2c_exp_insurance_sync_state,
	s2c_exp_insurance_sync_reward,
	s2c_exp_insurance_fetch_reward,
	s2c_exp_insurance_enter_notify,
};

struct S2C_EXP_INSRUANCE : BYTE_EXTEND_HEADER
{
	BYTE   SubProtocol;
	DWORD  Data;
};

enum s2c_quest_insurance_subprotocol
{
	s2c_quest_insurance_sync_state,
	s2c_quest_insurance_sync_reward,
	s2c_quest_insurance_fetch_reward,
	s2c_quest_insurance_enter_notify,
};

struct S2C_QUEST_INSURANCE : BYTE_EXTEND_HEADER
{
	BYTE   SubProtocol;
	INT    Data;
};

#pragma  pack (pop)

#define  MAX_EXP_QUEST_INSRUANCE_LEVEL     120
#define  EXP_QUEST_INSURANCE_SETTING_PATH  "\\settings\\expinsurance.ini"
#define  MAX_SCRIPT_FUNC_NAME              128                      

class KExpQuestInsuraceSetting
{
	int         m_MaxExpReward[ MAX_EXP_QUEST_INSRUANCE_LEVEL ];
	int         m_MaxOfflineReward;
	int         m_ExpOnlineRewardTime;
	int         m_ExpEnableLevel;
	int         m_QuestEnableLevel;

	bool        m_EnableExpManually;
	bool        m_EnableQuesetManually;

	bool        m_SettingsLoaded;

	char        m_ExpInsuranceFunc[MAX_SCRIPT_FUNC_NAME];
	char        m_QuestInsuranceFunc[MAX_SCRIPT_FUNC_NAME];

public:
	KExpQuestInsuraceSetting();
	~KExpQuestInsuraceSetting();

	bool       Load( void );
	bool       IsExpEnabled( void ) const ;
	bool       IsQuesetEnabled( void ) const;
	
	int        GetMaxExpRewardByLevel(const int nLevel) const ;
	int        GetMaxOfflineReward( void ) const;
	int        GetExpOnlineRewardTime ( void ) const;

	int        GetExpEnableLevel( void )const;
	int        GetQuestEnableLevel ( void )const;

	char *     GetExpInsuranceFuncName( void );
	char *     GetQuestInsuranceFuncName( void );

	void       SetExpEnable( bool bEnable);
	void       SetQuestEnable ( bool bEnable );
    
public:

	static KExpQuestInsuraceSetting & Singleton( void );

private:
	KExpQuestInsuraceSetting ( const KExpQuestInsuraceSetting &);
	const KExpQuestInsuraceSetting & operator = ( const KExpQuestInsuraceSetting &);

};

#ifdef _SERVER

class KExpInsuranceMgr
{
	int         m_PlayerIndex;
	int         m_ExpCache;
	DWORD       m_LastRewardTime;

public:

	KExpInsuranceMgr();
	~KExpInsuranceMgr();

	//Core used functions ....................................................
    
	void       PlayerOnline ( );
	void       Init ( const int nPlayerIndex );
	void       Release ( void );

	void       SetData( const int nExpCache,const  DWORD dwLastRewardTime); //DB Operation used only,PlayerOnline should be call first
	void       GetData( int & nExpCache , DWORD & dwLastRewardTime );       //DB Operation used only

	int        GetCurRewardExp( void )const;

	void       ContributeExp ( const int nExp );
	void       Active( void );
	void       NotifyLevelUp( const int nLevel );

	//Script used functions ..................................................

	bool       IsEnterInsuraceState( void )const;
	void       ReEnterInsuraceState( void );    //Notice m_ExpCache and m_LastRewardTime will be reset
    void       LeaveInsuranceState( void );     //Notice m_ExpCache and m_LastRewardTime will be reset

private:

	int        CalcRewardExp ( int nExp ) const ;
	void       SyncRewardExp (   void   ) const ;
	void       SyncRewardState( void )    const ;
	void       CheckGiveRewardExp( void )       ;
	void       ChangeExpCache( int nExp );
	void       ChangeExpRewardTime( DWORD dwTime);

private://forbid to use...
	KExpInsuranceMgr( const KExpInsuranceMgr &);
	const KExpInsuranceMgr & operator = (const KExpInsuranceMgr &);
};

class KQuestInsuranceMgr
{
	int        m_PlayerIndex;
	int        m_OfflineTimeCache;
	bool       m_Enabled;

public:

	KQuestInsuranceMgr();
	~KQuestInsuranceMgr();

	void       PlayerOnline ( void );
	void       Init ( const int nPlayerIndex );
	void       Release ( void );
	
	void       SetData( const int nOffLineCache,const bool bEnable);       //DB Operation used only,PlayerOnline should be call first
	void       GetData( int & nOffLineCache , bool & bEnable);             //DB Operation used only

	void       NotifyLevelUp( const int nLevel );
public:

	//Script used functions ..................................................
	
	bool       IsEnterInsuraceState( void )const;
	void       ReEnterInsuraceState( void );    //Notice m_ExpCache and m_LastRewardTime will be reset
    void       LeaveInsuranceState( void );     //Notice m_ExpCache and m_LastRewardTime will be reset

	int        GetOfflineTimeCache( void )const;
	void       AddOfflineTimeCache( int nTime );
private:

	void       ChangeRewardState( bool bEnable );
	void       ChangeOfflineTimeCache(const int nTime);

	void       OnlineReward( void);
	void       SyncOfflineTimeCache();
	void       SyncQuestInsuranceState();
private:
	KQuestInsuranceMgr( const KQuestInsuranceMgr &);
	const KQuestInsuranceMgr & operator = (const KQuestInsuranceMgr &);
};

#endif

#endif 