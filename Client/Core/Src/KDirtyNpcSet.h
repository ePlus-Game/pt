// KDirtyNpcSet.h Client Npc set which chached npc who displayed before
// Rocker 2005.10.26
//////////////////////////////////////////////////////////////////////////

#ifndef _KDIRTYNPCSET_H_
#define _KDIRTYNPCSET_H_

#include <vector>

struct DirtyNpcItem
{
	DWORD	dwNpcID;			// npc id which display before
	BYTE	btKind;				// npc kind
	int		dwRegionID;			// location of npc, x, y pos ext.
	int		nMapX;
	int		nMapY;
	bool	bInCurRegion;
	int		nSettingIdx;
	unsigned int	nSceneID;

	bool	bTemp;
	char	szInfo[_NAME_LEN];
};

class KDirtyNpcSet
{
public:
	KDirtyNpcSet();
	~KDirtyNpcSet();

	void	Clear();			// which called by change the map 
	void	PushItem(int nNpcIdx);	// which called by NpcSet.addnpc()
	void	RemoveItem(DWORD dwNpcID);	// which called by Check()
	void	SetItemOutRegion(DWORD dwNpcID);
	DirtyNpcItem*	GetNextItem();	
	void	Front();
	int		FindItem(DWORD dwNpcID);
	void	Check();

	void	Sort();
	void	ClearTemp();
	void	PushTempItem(int nNpcIdx);

private:
	static	bool SortProc(DirtyNpcItem &Item1, DirtyNpcItem &Item2);
	std::vector<DirtyNpcItem>	m_Items;
	int		m_nPos;
};

#endif