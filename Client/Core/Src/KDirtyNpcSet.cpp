// KDirtyNpcSet.cpp Client Npc set which chached npc who displayed before
// Rocker 2005.10.26
//////////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KDirtyNpcSet.h"
#include "Scene/KScenePlaceC.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include <algorithm>

#ifndef _SERVER

KDirtyNpcSet::KDirtyNpcSet()
{
}

KDirtyNpcSet::~KDirtyNpcSet()
{
}

void	KDirtyNpcSet::Clear()
{
	m_Items.clear();
}

void	KDirtyNpcSet::PushItem(int nNpcIdx)
{
	int nIdx = FindItem(Npc[nNpcIdx].m_dwID);
	if (nIdx != -1)
	{
		m_Items[nIdx].bInCurRegion = true;
		return;
	}

	DirtyNpcItem item;
	item.dwNpcID =  Npc[nNpcIdx].m_dwID;
	item.dwRegionID = Npc[nNpcIdx].m_dwRegionID;
	item.nMapX = Npc[nNpcIdx].GetMapX();
	item.nMapY = Npc[nNpcIdx].GetMapY();
	item.bInCurRegion = true;
	item.btKind = Npc[nNpcIdx].m_Kind;
	item.nSettingIdx = Npc[nNpcIdx].m_NpcSettingIdx;
	item.bTemp = false;
	strcpy(item.szInfo, Npc[nNpcIdx].Name);

	int nSrcX, nSrcY;
	Npc[nNpcIdx].GetMpsPos(&nSrcX, &nSrcY);
	item.nSceneID = 0;
	item.nSceneID = g_ScenePlace.MoveObject(CGOG_NPC, nNpcIdx, nSrcX, nSrcY, 0, item.nSceneID);
	m_Items.push_back(item);
}

void	KDirtyNpcSet::RemoveItem(DWORD dwNpcID)
{
	int nIdx = FindItem(dwNpcID);
	if (nIdx == -1)
		return;
	
	g_ScenePlace.RemoveObject(CGOG_NPC, 0, m_Items[nIdx].nSceneID);
	m_Items.erase(m_Items.begin() + nIdx);
}

DirtyNpcItem*	KDirtyNpcSet::GetNextItem()
{
	if (m_Items.size() == 0)
		return NULL;
	if (m_nPos >= m_Items.size())
	{
		m_nPos = 0;
		return NULL;
	}
	return &m_Items[m_nPos++];
}

void	KDirtyNpcSet::Front()
{
	m_nPos = 0;
}

int KDirtyNpcSet::FindItem(DWORD dwNpcID)
{
	for (int i=0; i<m_Items.size(); i++)
	{
		if (m_Items[i].dwNpcID == dwNpcID)
			return i;
	}

	return -1;
}

void KDirtyNpcSet::SetItemOutRegion(DWORD dwNpcID)
{
	for (int i=0; i<m_Items.size(); i++)
	{
		if (m_Items[i].dwNpcID == dwNpcID)
			m_Items[i].bInCurRegion = false;
	}
}

void KDirtyNpcSet::Check()
{
	int i=0;
	int nHalfX = 0;
	int nHalfY = 0;
	g_ScenePlace.GetRepresentAreaHalfSize(nHalfX, nHalfY);
	int nX, nY, nZ;
	g_ScenePlace.GetFocusPosition(nX, nY, nZ);
	for (i=0; i<m_Items.size(); i++)
	{
		int nNpcX = LOWORD(m_Items[i].dwRegionID) * 32 +
						m_Items[i].nMapX * 2;
		int nNpcY = HIWORD(m_Items[i].dwRegionID) * 32 +
						m_Items[i].nMapY;
		nNpcX *= 16;
		nNpcY *= 32;

		if ((abs(nNpcX - nX) < (nHalfX + 100)) && (abs(nNpcY - nY) < (nHalfY + 100)))
			m_Items[i].bInCurRegion = true;
		else
			m_Items[i].bInCurRegion = false;
	}

	for (i=0; i<m_Items.size(); i++)
	{
		if (m_Items[i].bInCurRegion)
		{
			int nNpcIdx = 0;
			bool bFound = false;
			while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
			{
				if (Npc[nNpcIdx].m_dwID == m_Items[i].dwNpcID)
				{
					bFound = true;
					break;
				}
			}
			
			if (!bFound)
			{
				g_ScenePlace.RemoveObject(CGOG_NPC, 0, m_Items[i].nSceneID);
				m_Items.erase(m_Items.begin() + i);
				return;
			}
		}		
	}
}

void	KDirtyNpcSet::Sort()
{
	std::sort(m_Items.begin(), m_Items.end(), SortProc);
}

void	KDirtyNpcSet::ClearTemp()
{
	int h = m_Items.size();
	for (int i=0; i<h; i++)
	{
		if (m_Items[i].bTemp)
		{
			m_Items.erase(m_Items.begin() + i);
			i-=1;
			h--;
		}
	}
}

void	KDirtyNpcSet::PushTempItem(int nNpcIdx)
{
	int nIdx = FindItem(Npc[nNpcIdx].m_dwID);
	if (nIdx != -1)
	{
		return;
	}

	DirtyNpcItem item;
	item.dwNpcID =  Npc[nNpcIdx].m_dwID;
	item.dwRegionID = Npc[nNpcIdx].m_dwRegionID;
	item.nMapX = Npc[nNpcIdx].GetMapX();
	item.nMapY = Npc[nNpcIdx].GetMapY();
	item.bInCurRegion = true;
	item.btKind = Npc[nNpcIdx].m_Kind;
	item.nSettingIdx = Npc[nNpcIdx].m_NpcSettingIdx;
	item.nSceneID = 0;
	item.bTemp = true;
	strcpy(item.szInfo, Npc[nNpcIdx].Name);

	m_Items.push_back(item);
}

bool	KDirtyNpcSet::SortProc(DirtyNpcItem &Item1, DirtyNpcItem &Item2)
{
	int nNpcY1 = HIWORD(Item1.dwRegionID) * 32 + Item1.nMapY;
	int nNpcY2 = HIWORD(Item2.dwRegionID) * 32 + Item2.nMapY;

	return nNpcY1 < nNpcY2;
}

#endif
