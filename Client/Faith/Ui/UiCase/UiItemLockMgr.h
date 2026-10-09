//物品锁定管理器（用于在进行某些操作，例如打造、合成的时候把相应物品锁定）   xiehong 2008-3-14

#ifndef KUiItemLockMgr_H
#define KUiItemLockMgr_H

#include <vector>

using namespace std;

class KUiItemLockMgr
{
	vector<int>		_lockedItemIndex;
private:

public:
	void	clear();
	bool	lock(int index);
	bool	getLockState(int index);
	void	refreshItemBox();

	KUiItemLockMgr();
	~KUiItemLockMgr();

	static KUiItemLockMgr& getSingleton();
};

#endif