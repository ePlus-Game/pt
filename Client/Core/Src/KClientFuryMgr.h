#ifndef K_CLIENT_FURY_MANAGER_H
#define K_CLIENT_FURY_MANAGER_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/06/2007 11:45
//      File_base        : KClientFuryMgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ¿Í»§¶Ë±¬»ê
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

class KClientFuryMgr
{
	int m_FuryExp;
public:
	KClientFuryMgr();
	~KClientFuryMgr();
public:
	static KClientFuryMgr & Singlton(void);
	int    GetCurFuryExp(void)const;
	void   ExplodeCurFury(void);
public:
	void ProcessMsg(BYTE * pMsg);
};

#endif