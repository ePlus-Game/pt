
#include "UiItemBox.h"
#include "UiItemLockMgr.h"

KUiItemLockMgr::KUiItemLockMgr()
{

}

KUiItemLockMgr::~KUiItemLockMgr()
{

}

KUiItemLockMgr& KUiItemLockMgr::getSingleton()
{
	static KUiItemLockMgr singleton;
	return singleton;
}

void KUiItemLockMgr::clear()
{
	_lockedItemIndex.clear();
	refreshItemBox();
}

bool KUiItemLockMgr::lock(int index)
{
	if(getLockState(index))
	{
		return false;
	}

	_lockedItemIndex.push_back(index);
	return true;
}

bool KUiItemLockMgr::getLockState(int index)
{
	for(int i = 0; i < _lockedItemIndex.size(); ++i)
	{
		if(_lockedItemIndex[i] == index)
		{
			return true;
		}
	}

	return false;
}

void KUiItemLockMgr::refreshItemBox()
{
	KUiItemBox::getSingleton().freshLockedItem();
}