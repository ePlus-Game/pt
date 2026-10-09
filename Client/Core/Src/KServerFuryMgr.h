#ifndef  K_SERVER_FURY_MGR_H
#define  K_SERVER_FURY_MGR_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/05/2007 20:03
//      File_base        : KServerFuryMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ±¬»êÏµÍ³
//
//      <Change_list>     
//////////////////////////////////////////////////////////////////////

class KServerFuryMgr
{
	int             m_CurFuryExp ;    
	int             m_PlayerIndex;
	int             m_KilledInterval;
	unsigned long   m_CurCheckTimeStamp;
	unsigned long   m_CurKeepTimeStamp;
public:
	 KServerFuryMgr(void);
	~KServerFuryMgr(void);
public:
	void    Init(const int nPlayerIndex);
    void    Release(void);
	void    Active(void);
public:
	void    NpcKillingNotify(void);
	void    CleanFuryExp(void);
	void    SetCurFuryExp(int nFury);
	int     GetFuryExp(void);
public://c2s  protocol functions................
	void    FuryExplode(void);
private:
	void    IntoKeepState(void);
	void    IntoCheckState(void);
private://s2c protocol functions ...............	
    void    SyncFuryExpToClient(void);
	void    SyncFuryWarningToClient(int num);
};

#endif