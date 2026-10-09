//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006:12:20   15:03
//      File_base        : DBAucDataCenter
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

#ifndef _DBAucDataCenter_h
#define _DBAucDataCenter_h

#include "AuctionComDef.h"
#include "CoreRelated.h"
#include "cfs_fs2_savedef.h"
#include "cfs_db_interface.h"

class DBAucDataCenter
{
	
public:
	static DBAucDataCenter&	Singleton();

	void	Active();

	void	SearchReq(int nPlayerIdx, const S2DB_SEARCHGOODS_COND &searchCond);
	void	SellReq(int nPlayerIdx, const S2DB_SELLGOODS_REQ &sellReq);
	void	LockGoodsReq(int nPlayerIdx, const S2DB_LOCKGOODS_REQ &lockReq);
	void	UpdatePrice(int nPlayerIdx, const S2DB_UPDATEPRICE_REQ &buyReq);
	void	UnLockGoodsReq(int nPlayerIdx, const S2DB_UNLOCKGOODS_REQ &unlockReq);
	void	CancelReq(int nPlayerIdx, const S2DB_CANCELAUCTION_REQ &s2dbReq);

private:
	void	ClearOverdueGoodsReq();

private:
	DBAucDataCenter();
	DBAucDataCenter(const DBAucDataCenter &rhs);
	DBAucDataCenter& operator= (const DBAucDataCenter &rhs);

private:
	DWORD	m_preClearGoodsTime;
};

inline DBAucDataCenter::DBAucDataCenter()
{
	m_preClearGoodsTime = 0;
}

#endif
