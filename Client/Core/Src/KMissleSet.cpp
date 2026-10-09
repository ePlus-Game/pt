//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 10/22/2007 11:04
//      File_base        : KMissleSet
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KMissle.h"
#include "KMissleSet.h"
#include "KSubWorld.h"
#include "KSG_StringProcess.h"

KMissleSet MissleSet;

KMissleTemplate		KMissleSet::m_MisslesTemplate[MAX_MISSLESTYLE]; //  missle模板

void KMissleSet::Init()
{
	
	m_FreeIdx.Init(MAX_MISSLE);
	m_UseIdx.Init(MAX_MISSLE);
	
	// 开始时所有的数组元素都为空
	int i = 0;
	for (i = MAX_MISSLE - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
		Missle[i].m_Node.m_nIndex = i;
	}

	int nMissleNum = g_MisslesSetting.GetHeight() - 1;
	
	for (i = 0; i < nMissleNum; i++)
	{
		int nMissleId = 0;
		g_MisslesSetting.GetInteger(i + 2, "MissleId", -1, &nMissleId);		
		if (nMissleId > 0)
		{
			GetInfoFromTabFile(i + 2);			
		}
	}

}

int KMissleSet::Add(int nSubWorldId, int nRegionID,int nMapX, int nMapY, int nOffsetX , int nOffsetY)
{
	
	if (nRegionID < 0 ) return -1;
	int nFreeIndex = FindFree();
	if (nFreeIndex <=0 ) return -1;
	
	Missle[nFreeIndex].m_nMissleIdx		= nFreeIndex;
	Missle[nFreeIndex].m_nRegionId		= nRegionID;
	Missle[nFreeIndex].m_nCurrentMapX	= nMapX;
	Missle[nFreeIndex].m_nCurrentMapY	= nMapY;
	Missle[nFreeIndex].m_nXOffset		= nOffsetX;
	Missle[nFreeIndex].m_nYOffset		= nOffsetY;
	Missle[nFreeIndex].m_nSubWorldId	= nSubWorldId;
	
	m_FreeIdx.Remove(nFreeIndex);
	m_UseIdx.Insert(nFreeIndex);
	
	return nFreeIndex;
}

int KMissleSet::Add(int nSubWorldId, int nPX, int nPY)
{
	if (nSubWorldId < 0) return -1;
	
	int nFreeIndex = FindFree();
	if (nFreeIndex <= 0) 
	{
		CFS_FILELOGS::WriteDebugLog("MissleSet Have Full!!!\n");

		return -1;
	}
	
	SubWorld[nSubWorldId].Mps2Map(nPX, nPY, &Missle[nFreeIndex].m_nRegionId, &Missle[nFreeIndex].m_nCurrentMapX, &Missle[nFreeIndex].m_nCurrentMapY, &Missle[nFreeIndex].m_nXOffset, &Missle[nFreeIndex].m_nYOffset);
	
	if (Missle[nFreeIndex].m_nRegionId < 0) return -1;
	
	Missle[nFreeIndex].m_nMissleIdx = nFreeIndex;
	Missle[nFreeIndex].m_nCurrentMapZ = Missle[nFreeIndex].GetMissleHeight();
	Missle[nFreeIndex].m_nSubWorldId = nSubWorldId;
	SubWorld[nSubWorldId].m_Region[Missle[nFreeIndex].m_nRegionId].AddMissle(nFreeIndex);
//	SubWorld[nSubWorldId].m_Region[Missle[nFreeIndex].m_nRegionId].AddRef(Missle[nFreeIndex].m_nCurrentMapX, Missle[nFreeIndex].m_nCurrentMapY, obj_missle);
	m_FreeIdx.Remove(nFreeIndex);
	m_UseIdx.Insert(nFreeIndex);
	g_DebugLog("[Missle]Missle%dAdd", nFreeIndex);
	
	return nFreeIndex;
}

void KMissleSet::Remove(int nIndex)
{
	if (nIndex <= 0) return;
	
	if (Missle[nIndex].m_nMissleIdx < 0) return;
	Missle[nIndex].Release();
#ifdef _DEBUG
	g_DebugLog("[Missle]Missle%dRemove", nIndex);
	g_DebugLog("[Missle]Count%d", GetCount());
#endif
	m_FreeIdx.Insert(nIndex);
	m_UseIdx.Remove(nIndex);
}

KMissleTemplate* KMissleSet::GetMissleTemplate( int MisslesTemplate )
{
	if ( MisslesTemplate < 0 || MisslesTemplate >= MAX_MISSLESTYLE )
	{
		MisslesTemplate = 0;
	}
	return &m_MisslesTemplate[MisslesTemplate];
}

BOOL KMissleSet::GetInfoFromTabFile(int nMissleId)
{
	if (nMissleId <= 0 ) return FALSE;
	KITabFile * pITabFile = &g_MisslesSetting;
	return GetInfoFromTabFile(pITabFile, nMissleId);
}

BOOL KMissleSet::GetInfoFromTabFile(KITabFile * pMisslesSetting, int nMissleId)
{
	if (nMissleId <= 0 ) return FALSE;
	m_MisslesTemplate[nMissleId].m_nMissleId		= nMissleId;
	int nRow = nMissleId;
	
	pMisslesSetting->GetString(nRow, "MissleName",		   "", m_MisslesTemplate[nMissleId].m_szMissleName,30, TRUE);	
	int nHeightOld ;
	pMisslesSetting->GetInteger(nRow, "MissleHeight",		0, &nHeightOld, TRUE);
	m_MisslesTemplate[nMissleId].m_nHeight = nHeightOld << 10;	
	pMisslesSetting->GetInteger(nRow, "LifeTime",			0, &m_MisslesTemplate[nMissleId].m_nLifeTime, TRUE);
	pMisslesSetting->GetInteger(nRow, "Speed",				0, &m_MisslesTemplate[nMissleId].m_nSpeed, TRUE);
	pMisslesSetting->GetInteger(nRow, "ResponseSkill",		0, &m_MisslesTemplate[nMissleId].m_nSkillId, TRUE);
	pMisslesSetting->GetInteger(nRow, "CollidRange",		0, &m_MisslesTemplate[nMissleId].m_nCollideRange, TRUE);
	pMisslesSetting->GetInteger(nRow, "ColVanish",			0, &m_MisslesTemplate[nMissleId].m_bCollideVanish, TRUE);
	pMisslesSetting->GetInteger(nRow, "CanColFriend",		0, &m_MisslesTemplate[nMissleId].m_bCollideFriend, TRUE);
	pMisslesSetting->GetInteger(nRow, "CanSlow",			0, &m_MisslesTemplate[nMissleId].m_bCanSlow, TRUE);
	pMisslesSetting->GetInteger(nRow, "IsRangeDmg",			0, &m_MisslesTemplate[nMissleId].m_bRangeDamage, TRUE);
	pMisslesSetting->GetInteger(nRow, "DmgRange",			0, &m_MisslesTemplate[nMissleId].m_nDamageRange, TRUE);
	pMisslesSetting->GetInteger(nRow, "MoveKind",			0, (int*)&m_MisslesTemplate[nMissleId].m_eMoveKind, TRUE);
	pMisslesSetting->GetInteger(nRow, "FollowKind",			0, (int*)&m_MisslesTemplate[nMissleId].m_eFollowKind, TRUE);
	pMisslesSetting->GetInteger(nRow, "Zacc",				0,(int*)&m_MisslesTemplate[nMissleId].m_nZAcceleration, TRUE);
	pMisslesSetting->GetInteger(nRow, "Zspeed",				0,(int*)&m_MisslesTemplate[nMissleId].m_nHeightSpeed, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param1",				0, &m_MisslesTemplate[nMissleId].m_nParam1, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param2",				0, &m_MisslesTemplate[nMissleId].m_nParam2, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param3",				0, &m_MisslesTemplate[nMissleId].m_nParam3, TRUE);	
	BOOL bAutoExplode = 0;
	pMisslesSetting->GetInteger(nRow, "AutoExplode",	0, (int*)&bAutoExplode, TRUE);
	m_MisslesTemplate[nMissleId].m_bAutoExplode = bAutoExplode;	
	pMisslesSetting->GetInteger(nRow, "DmgInterval",	0, (int*)&m_MisslesTemplate[nMissleId].m_ulDamageInterval, TRUE);
	pMisslesSetting->GetInteger(nRow, "MultiShow",		0, &m_MisslesTemplate[nMissleId].m_bMultiShow, TRUE);
	pMisslesSetting->GetInteger(nRow, "ColFollowTarget",0, (int *)&m_MisslesTemplate[nMissleId].m_bFollowNpcWhenCollid, TRUE);
	pMisslesSetting->GetInteger(nRow, "Acceleration", 0, &m_MisslesTemplate[nMissleId].m_nAcceleration, TRUE);

	char AnimFileCol[64];
	char SndFileCol[64];
	char AnimFileInfoCol[100];
	char szAnimFileInfo[100];

    const char *pcszTemp = NULL;	
	
	for (int i  = 0; i < MAX_MISSLE_STATUS; i++)
	{
		sprintf(AnimFileCol, "AnimFile%d", i + 1);
		sprintf(SndFileCol,  "SndFile%d", i + 1);
		sprintf(AnimFileInfoCol, "AnimFileInfo%d", i + 1);
		
		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MisslesTemplate[nMissleId].m_MissleRes[i].AnimFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MisslesTemplate[nMissleId].m_MissleRes[i].SndFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, 100, TRUE);
		
        pcszTemp = szAnimFileInfo;
        m_MisslesTemplate[nMissleId].m_MissleRes[i].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MisslesTemplate[nMissleId].m_MissleRes[i].nDir = KSG_StringGetInt(&pcszTemp, 16);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MisslesTemplate[nMissleId].m_MissleRes[i].nInterval = KSG_StringGetInt(&pcszTemp, 1);
		
		sprintf(AnimFileCol, "AnimFileB%d", i + 1);
		sprintf(SndFileCol,  "SndFileB%d", i + 1);
		sprintf(AnimFileInfoCol, "AnimFileInfoB%d", i + 1);
		
		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MisslesTemplate[nMissleId].m_MissleRes[i + MAX_MISSLE_STATUS].AnimFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MisslesTemplate[nMissleId].m_MissleRes[i + MAX_MISSLE_STATUS].SndFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, 100, TRUE);
		
        pcszTemp = szAnimFileInfo;
        m_MisslesTemplate[nMissleId].m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MisslesTemplate[nMissleId].m_MissleRes[i + MAX_MISSLE_STATUS].nDir = KSG_StringGetInt(&pcszTemp, 16);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MisslesTemplate[nMissleId].m_MissleRes[i + MAX_MISSLE_STATUS].nInterval = KSG_StringGetInt(&pcszTemp, 1);

	}

	pMisslesSetting->GetInteger(nRow, "LoopPlay", 0, &m_MisslesTemplate[nMissleId].m_bLoopAnim, TRUE);
	pMisslesSetting->GetInteger(nRow, "SubLoop",  0, &m_MisslesTemplate[nMissleId].m_bSubLoop, TRUE);
	pMisslesSetting->GetInteger(nRow, "SubStart", 0, &m_MisslesTemplate[nMissleId].m_nSubStart, TRUE);
	pMisslesSetting->GetInteger(nRow, "SubStop",  0, &m_MisslesTemplate[nMissleId].m_nSubStop, TRUE);

	return TRUE;
}

int KMissleSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

int KMissleSet::GetCount()
{
	int nCount  = 0;
	for (int i = 0;i < MAX_MISSLE ;i ++)
	{
	if (Missle[i].m_nMissleIdx > 0)
		nCount ++;

	}
	return nCount;
}

/*
int	KMissleSet::CreateMissile(int nSkillId, int nMissleId, int nLauncher,  int nTargetId ,int nSubWorldId, int nPX, int nPY, int nDir)
{
	int nMissleIndex = -1;
	
	if (nMissleIndex = Add(nSubWorldId, nPX, nPY) <= 0)
		return -1;
	return nMissleIndex;
}

void KMissleSet::Draw()
{
	for (int i = 1;i <= MAX_MISSLE ;i ++)
	{
		Missle[i].Paint();
	}
	
}

int KMissleSet::Activate()
{
	for (int i = 1;i <= MAX_MISSLE;i ++)
	{
		Missle[i].Activate();
	}
	return 1;
}

void KMissleSet::ClearMissles()
{
	int nUsedIndex = m_UseIdx.GetNext(0);
	
	while (nUsedIndex != 0)
	{
		Remove(nUsedIndex);
		nUsedIndex = m_UseIdx.GetNext(0);
	}
}

//*/
