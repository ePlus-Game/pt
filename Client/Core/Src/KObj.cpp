
//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KObj.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "KItemSet.h"
#include "ScriptFuns.h"
#include "KSortScript.h"
#ifdef _SERVER

#include "KSkills.h"

#else
#include "iRepresentshell.h"
#include "scene/KScenePlaceC.h"
#include "ImgRef.h"
#include "KOption.h"
#include "CoreShell.h"
#endif
#include "KObj.h"
#include "KObjSet.h"

#define		OBJ_SHOW_NAME_Y_OFF		0//48

KObj	Object[MAX_OBJECT];

#ifdef _SERVER
#undef BROADCAST_REGION
#define BROADCAST_REGION(pBuff,uSize,uMaxCount)	SubWorld[m_nSubWorldID].BroadCastRegion((pBuff), (uSize), (uMaxCount), m_nRegionIdx, m_nMapX, m_nMapY);
#endif

//-------------------------------------------------------------------------
//	功能：	初始化
//-------------------------------------------------------------------------
KObj::KObj()
{
	Release();
}

//-------------------------------------------------------------------------
//	功能：	物件清空
//-------------------------------------------------------------------------
void	KObj::Release()
{
	m_nID				= -1;
	m_nKind				= 0;
	m_nBelongRegion		= OBJECT_ITEM_BELONG_REGION_NONE;

	m_nIndex			= 0;
	m_nSubWorldID		= 0;
	m_nRegionIdx		= 0;
	m_nMapX				= 0;
	m_nMapY				= 0;
	m_nOffX				= 0;
	m_nOffY				= 0;
	m_nDir				= 0;

#ifndef _SERVER
	m_nLayer			= 1;
	m_nHeight			= 0;

	m_sObjLight.Release();
	m_Image.uImage = 0;
	m_SceneID = 0;
	m_nImageDropPlayType = 0;
	m_nImagePlayType = 0;
#endif

	m_nState			= 0;
	m_nLifeTime			= 0;
	m_nBornTime			= 0;
	m_nWaitTime			= 0;
	m_cSkill.Release();

#ifdef _SERVER
	if (m_nItemDataID)
	{
		if (m_nItemDataID > 0 && m_nItemDataID < MAX_ITEM)
			Item[m_nItemDataID].SetOBJBelong( -1 );

		ItemSet.Remove(m_nItemDataID);
	}//endif
#endif
	m_nItemDataID		= 0;
	m_nMoneyNum			= 0;

	m_szName[0]			= 0;
	m_dwScriptID		= 0;
	this->m_nColorID	= 0;

#ifdef _SERVER
	m_nBelong			= OBJECT_ITEM_BELONG_NONE;
	m_nBelongTime		= 0;
	m_cImage.Release();

	m_nLauncher = -1;
	m_nAct		= FALSE;
	m_PickupTime = 0;
	m_DisappearTime = 0;

	m_nLeftTimeAfterTrapAct = 0;

#endif

#ifndef _SERVER
	m_szImageName[0]	= 0;
	m_szImageDropName[0]= 0;
	m_szSoundName[0]	= 0;
	m_nDropState		= 0;
	m_cImage.Release();
	m_cImageDrop.Release();
	this->m_dwNameColor = 0x00ffffff;
	m_bCanPickForClient = false;
#endif

	m_dwTimer			= 0;

//	memset(m_btBar, 0, sizeof(m_btBar));
//	Polygon.Clear();

#ifndef _SERVER
	m_pSoundNode		= NULL;
	m_pWave				= NULL;
#endif
	
	//房屋障碍,暂时没有用到
	m_nObstacleW = 0;
	m_nObstacleH = 0;
}

/*
//-------------------------------------------------------------------------
//	功能：	保存物件信息到 ini 文件
//-------------------------------------------------------------------------
void KObj::Save(KIniFile *IniFile, LPSTR Section)
{
	IniFile->WriteString(Section, "ObjName", m_szName);

	IniFile->WriteInteger(Section, "DataID", m_nDataID);
	IniFile->WriteInteger(Section, "Kind", m_nKind);
	IniFile->WriteInteger(Section, "Dir", m_nDir);
	IniFile->WriteInteger(Section, "State", m_nState);
	IniFile->WriteInteger(Section, "LifeTime", m_nLifeTime);
	IniFile->WriteInteger(Section, "BornTime", m_nBornTime);
	IniFile->WriteInteger(Section, "WaitTime", m_nWaitTime);
	IniFile->WriteInteger(Section, "SkillKind", m_cSkill.m_nKind);
	IniFile->WriteInteger(Section, "SkillCamp", m_cSkill.m_nCamp);
	IniFile->WriteInteger(Section, "SkillRange", m_cSkill.m_nRange);
	IniFile->WriteInteger(Section, "SkillCastTime", m_cSkill.m_nCastTime);
	IniFile->WriteInteger(Section, "SkillID", m_cSkill.m_nID);
	IniFile->WriteInteger(Section, "SkillLevel", m_cSkill.m_nLevel);
	IniFile->WriteString(Section, "ScriptFile", m_szScriptName);

	IniFile->WriteInteger(Section, "ItemDataID", m_nItemDataID);
	IniFile->WriteInteger(Section, "ItemWidth", m_nItemWidth);
	IniFile->WriteInteger(Section, "ItemHeight", m_nItemHeight);
	IniFile->WriteInteger(Section, "Money", m_nMoneyNum);

#ifndef _SERVER
	IniFile->WriteInteger(Section, "Layer", m_nLayer);
	IniFile->WriteInteger(Section, "Height", m_nHeight);

	IniFile->WriteInteger(Section, "LightRadius", m_sObjLight.m_nRadius);
	IniFile->WriteInteger(Section, "Red", m_sObjLight.m_nRed);
	IniFile->WriteInteger(Section, "Green", m_sObjLight.m_nGreen);
	IniFile->WriteInteger(Section, "Blue", m_sObjLight.m_nBlue);
	IniFile->WriteInteger(Section, "Alpha", m_sObjLight.m_nAlpha);
	IniFile->WriteInteger(Section, "Reflect", m_sObjLight.m_nReflectType);
#endif

#ifdef _SERVER
	IniFile->WriteInteger(Section, "TotalFrmae", m_cImage.m_nTotalFrame);
	IniFile->WriteInteger(Section, "CurFrmae", m_cImage.m_nCurFrame);
	IniFile->WriteInteger(Section, "TotalDir", m_cImage.m_nTotalDir);
	IniFile->WriteInteger(Section, "CurDir", m_cImage.m_nCurDir);
	IniFile->WriteInteger(Section, "Interval", m_cImage.m_dwInterval);
#endif

#ifndef _SERVER
	IniFile->WriteString(Section, "ImageFile", m_szImageName);
	IniFile->WriteString(Section, "SoundFile", m_szSoundName);
	IniFile->WriteInteger(Section, "TotalFrmae", m_cImage.m_nTotalFrame);
	IniFile->WriteInteger(Section, "CurFrmae", m_cImage.m_nCurFrame);
	IniFile->WriteInteger(Section, "TotalDir", m_cImage.m_nTotalDir);
	IniFile->WriteInteger(Section, "CurDir", m_cImage.m_nCurDir);
	IniFile->WriteInteger(Section, "Interval", (int)m_cImage.m_dwInterval);
	IniFile->WriteInteger(Section, "CenterX", m_cImage.m_nCgXpos);
	IniFile->WriteInteger(Section, "CenterY", m_cImage.m_nCgYpos);
#endif

	int		x, y;
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &x, &y);
	IniFile->WriteInteger(Section, "PosX", x);
	IniFile->WriteInteger(Section, "PosY", y);

	IniFile->WriteStruct(Section, "ObjBar", m_btBar, sizeof(m_btBar));
	IniFile->WriteStruct(Section, "Polygon", Polygon.GetPolygonPtr(), sizeof(TPolygon));
}

//-------------------------------------------------------------------------
//	功能：	从 ini 文件中读取物件信息
//-------------------------------------------------------------------------
void KObj::Load(int nObjIndex, int nSubWorldID, KIniFile *IniFile, LPSTR Section)
{
	IniFile->GetString(Section, "ObjName", "", m_szName, sizeof(m_szName));

	IniFile->GetInteger(Section, "DataID", 0, &m_nDataID);
	IniFile->GetInteger(Section, "Kind", Obj_Kind_MapObj, &m_nKind);
	IniFile->GetInteger(Section, "Dir", 0, &m_nDir);
	IniFile->GetInteger(Section, "State", 0, &m_nState);
	IniFile->GetInteger(Section, "LifeTime", 0, &m_nLifeTime);
	IniFile->GetInteger(Section, "BornTime", 0, &m_nBornTime);
	IniFile->GetInteger(Section, "WaitTime", 0, &m_nWaitTime);
	IniFile->GetInteger(Section, "SkillKind", 0, &m_cSkill.m_nKind);
	IniFile->GetInteger(Section, "SkillCamp", camp_animal, &m_cSkill.m_nCamp);
	IniFile->GetInteger(Section, "SkillRange", 0, &m_cSkill.m_nRange);
	IniFile->GetInteger(Section, "SkillCastTime", 0, &m_cSkill.m_nCastTime);
	IniFile->GetInteger(Section, "SkillID", 0, &m_cSkill.m_nID);
	IniFile->GetInteger(Section, "SkillLevel", 0, &m_cSkill.m_nLevel);
	IniFile->GetString(Section, "ScriptFile", "", m_szScriptName, sizeof(m_szScriptName));

	IniFile->GetInteger(Section, "ItemDataID", 0, &m_nItemDataID);
	IniFile->GetInteger(Section, "ItemWidth", 0, &m_nItemWidth);
	IniFile->GetInteger(Section, "ItemHeight", 0, &m_nItemHeight);
	IniFile->GetInteger(Section, "Money", 0, &m_nMoneyNum);

#ifndef _SERVER
	IniFile->GetInteger(Section, "Layer", 1, &m_nLayer);
	IniFile->GetInteger(Section, "Height", 0, &m_nHeight);

	IniFile->GetInteger(Section, "LightRadius", 0, &m_sObjLight.m_nRadius);
	IniFile->GetInteger(Section, "Red", 0, &m_sObjLight.m_nRed);
	IniFile->GetInteger(Section, "Green", 0, &m_sObjLight.m_nGreen);
	IniFile->GetInteger(Section, "Blue", 0, &m_sObjLight.m_nBlue);
	IniFile->GetInteger(Section, "Alpha", 0, &m_sObjLight.m_nAlpha);
	IniFile->GetInteger(Section, "Reflect", 0, &m_sObjLight.m_nReflectType);
#endif

#ifdef _SERVER
	int		nTotalFrame, nTotalDir, nInterval, nCurFrame, nCurDir;
	IniFile->GetInteger(Section, "TotalFrmae", 1, &nTotalFrame);
	IniFile->GetInteger(Section, "CurFrmae", 0, &nCurFrame);
	IniFile->GetInteger(Section, "TotalDir", 1, &nTotalDir);
	IniFile->GetInteger(Section, "CurDir", 0, &nCurDir);
	IniFile->GetInteger(Section, "Interval", 1, &nInterval);

	m_cImage.SetTotalFrame(nTotalFrame);
	m_cImage.SetTotalDir(nTotalDir);
	m_cImage.SetCurFrame(nCurFrame);
	m_cImage.SetCurDir(nCurDir);
	m_cImage.SetInterVal(nInterval);
#endif

#ifndef _SERVER
	int		nCgX, nCgY, nTotalFrame, nTotalDir, nInterval, nCurFrame, nCurDir;
	IniFile->GetString(Section, "ImageFile", "", m_szImageName, sizeof(m_szImageName));
	IniFile->GetString(Section, "SoundFile", "", m_szSoundName, sizeof(m_szSoundName));
	IniFile->GetInteger(Section, "TotalFrmae", 1, &nTotalFrame);
	IniFile->GetInteger(Section, "CurFrmae", 0, &nCurFrame);
	IniFile->GetInteger(Section, "TotalDir", 1, &nTotalDir);
	IniFile->GetInteger(Section, "CurDir", 0, &nCurDir);
	IniFile->GetInteger(Section, "Interval", 1, &nInterval);
	IniFile->GetInteger(Section, "CenterX", 0, &nCgX);
	IniFile->GetInteger(Section, "CenterY", 0, &nCgY);

	m_cImage.SetFileName(m_szImageName);
	m_cImage.SetTotalFrame(nTotalFrame);
	m_cImage.SetTotalDir(nTotalDir);
	m_cImage.SetCurFrame(nCurFrame);
	m_cImage.SetCurDir(nCurDir);
	m_cImage.SetInterVal((DWORD)nInterval);
	m_cImage.SetCenterPos(nCgX, nCgY);
#endif

	int		x, y;
	IniFile->GetInteger(Section, "PosX", 0, &x);
	IniFile->GetInteger(Section, "PosY", 0, &y);
	m_nSubWorldID = nSubWorldID;
	SubWorld[m_nSubWorldID].Mps2Map(x, y, &m_nRegionIdx, &m_nMapX, &m_nMapY,&m_nOffX, &m_nOffY);

	IniFile->GetStruct(Section, "ObjBar", m_btBar, sizeof(m_btBar));
	IniFile->GetStruct(Section, "Polygon", Polygon.GetPolygonPtr(), sizeof(TPolygon));

	SetIndex(nObjIndex);
	SetDir(m_nDir);
	SetState(m_nState);
}
*/

//-------------------------------------------------------------------------
//	功能：	设定物件的索引值
//-------------------------------------------------------------------------
void	KObj::SetIndex(int nIndex)
{
	if (nIndex >= 0)
		m_nIndex = nIndex;
	else
		m_nIndex = 0;
}

//-------------------------------------------------------------------------
//	功能：	设定物件世界唯一 ID （注：只在客户端存在的物件其 ID 统一为 0）
//-------------------------------------------------------------------------
void	KObj::SetWorldID(int nID)
{
	if (nID < 0)
		m_nID = 0;
	else
		m_nID = nID;
}

//-------------------------------------------------------------------------
//	功能：	设定物件状态
//-------------------------------------------------------------------------
void	KObj::SetState(int nState, int nPlaySoundFlag/* = 0*/)
{
	if (nState < 0)
		return;
	m_nState = nState;
	switch (m_nKind)
	{
	case Obj_Kind_Furniture:
		break;
	case Obj_Kind_House_Entry:
		break;
	case Obj_Kind_Box:
		if (nState == OBJ_BOX_STATE_CLOSE)
			BoxClose();
		else if (nState == OBJ_BOX_STATE_OPEN)
			BoxOpen();
		break;
	case Obj_Kind_Door:
		if (nState == OBJ_DOOR_STATE_CLOSE)
			DoorClose();
		else if (nState == OBJ_DOOR_STATE_OPEN)
			DoorOpen();
		break;
	case Obj_Kind_Trap:
		m_nBornTime = m_nLifeTime;
		break;
	case Obj_Kind_Prop:
		if (nState == OBJ_PROP_STATE_HIDE)
		{
			m_nBornTime = m_nLifeTime;
		}
		break;
	}

#ifndef _SERVER
	if (nPlaySoundFlag)
		PlaySound();
#endif

#ifdef _SERVER
	SyncState();
#endif
}

//-------------------------------------------------------------------------
//	功能：	宝箱打开
//-------------------------------------------------------------------------
void	KObj::BoxOpen()
{
#ifndef _SERVER
	if (m_nState != 1)
		PlaySound();
#endif
	m_nState = 1;
	m_nBornTime = m_nLifeTime;
}

//-------------------------------------------------------------------------
//	功能：	宝箱关闭
//-------------------------------------------------------------------------
void	KObj::BoxClose()
{
#ifndef _SERVER
	if (m_nState != 0)
		PlaySound();
#endif
	m_nState = 0;
	m_nBornTime = 0;
}

//-------------------------------------------------------------------------
//	功能：	门打开
//-------------------------------------------------------------------------
void	KObj::DoorOpen()
{
#ifndef _SERVER
	if (m_nState != 1)
		PlaySound();
#endif
	m_nState = 1;

	// 缺少处理物件障碍
}

//-------------------------------------------------------------------------
//	功能：	门关闭
//-------------------------------------------------------------------------
void	KObj::DoorClose()
{
#ifndef _SERVER
	if (m_nState != 0)
		PlaySound();
#endif
	m_nState = 0;

	// 缺少处理物件障碍
}

BOOL	KObj::SetDir(int n64Dir)
{
	if (n64Dir < 0 || n64Dir >= 64)
	{
		n64Dir = 0;
		_ASSERT(0);
	}

#ifdef _SERVER
	m_nDir = n64Dir;
	m_cImage.SetCurDir64(n64Dir);
	SyncDir();
#else
	m_nDir = n64Dir;
	m_cImage.SetCurDir64(n64Dir);
	m_cImageDrop.SetCurDir64(n64Dir);
#endif

	return TRUE;
}

void	KObj::SetScriptFile(char *lpszScriptFile)
{
	char	szScript[80];
	if ( !lpszScriptFile || strlen(lpszScriptFile) >= sizeof(szScript))
	{
		g_DebugLog("[error]Script FileName Error!!!");
	}
	else
	{
		if (lpszScriptFile[0])
		{
			if (lpszScriptFile[0] == '.')
				g_StrCpyLen(szScript, &lpszScriptFile[1], sizeof(szScript));
			else
				g_StrCpyLen(szScript, lpszScriptFile, sizeof(szScript));
			g_StrLower(szScript);
			m_dwScriptID = g_FileName2Id(szScript);
		}
	}
}

void	KObj::SetImageDir(int nDir)
{
#ifdef _SERVER
	m_cImage.SetCurDir(nDir);
	SetDir(m_cImage.m_nCurDir * 64 / m_cImage.m_nTotalDir);
#else
	m_cImage.SetCurDir(nDir);
	SetDir(m_cImage.m_nCurDir * 64 / m_cImage.m_nTotalDir);
#endif
}

#ifdef _SERVER
void	KObj::SetItemDataID(int nItemDataID)
{
	if (nItemDataID >= 0)
		m_nItemDataID = nItemDataID;
}
#endif


#ifndef _SERVER
void KObj::DrawInfo()
{
//	return;

	if (m_nKind != Obj_Kind_Item && m_nKind != Obj_Kind_Money && m_nKind != Obj_Kind_Trap )
		return;

	int		nMpsX, nMpsY, nHeightOff;
	DWORD	dwColor;

	GetMpsPos(&nMpsX, &nMpsY);
	nHeightOff = OBJ_SHOW_NAME_Y_OFF;

	dwColor = this->m_dwNameColor;

	//g_pRepresent->OutputText(12, m_szName, KRF_ZERO_END, nMpsX - 12 * g_StrLen(m_szName) / 4, nMpsY, dwColor, 0, nHeightOff);

	KUiNewFont newFont;
	strcpy( newFont.szName, "stzhongs-10");
	strcpy( newFont.szContext, m_szName );
	
	newFont.nX = nMpsX;
	newFont.nY = nMpsY  - nHeightOff * 2;
	newFont.nWidth	= 100;
	g_pRepresent->CoordinateTransform( newFont.nX, newFont.nY, 0 );
	newFont.nZ = 0;
	newFont.uColor = dwColor| 0xff000000;
	if ( newFont.nX >= 0 && newFont.nY >= 0 )
	{
		CoreDataChanged( GDCNI_DRAWTEXT, (unsigned int)&newFont, NULL );
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：	物件绘制
//-------------------------------------------------------------------------
void KObj::Draw()
{
	if ( m_nIndex <= 0 )
		return;
	if ( !m_cImage.CheckExist() && !m_cImageDrop.CheckExist() )
		return;
	if (m_bDrawFlag)
		return;
	if (m_nRegionIdx < 0 || m_nRegionIdx >= 9)
		return;

	int			x, y;
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &x, &y);
//	SubWorld[m_nSubWorldID].Mps2Screen(&x, &y);

	m_Image.Color.Color_b.a = 255;
	m_Image.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	m_Image.oPosition.nZ = 0;
	if ((m_nState == Obj_Kind_Item && m_nDropState == 1) ||
		m_nState == OBJ_TRAP_STATE_ACTING)	// 物品掉出动画
	{
		m_Image.nFrame = m_cImageDrop.m_nCurFrame;
		m_Image.oPosition.nX = x;// - m_cImageDrop.m_nCgXpos;
		m_Image.oPosition.nY = y;// - m_cImageDrop.m_nCgYpos * 2;
		strcpy(m_Image.szImage, m_cImageDrop.m_szName);
	}
	else
	{
		m_Image.nFrame = m_cImage.m_nCurFrame;
		m_Image.oPosition.nX = x;// - m_cImage.m_nCgXpos;
		m_Image.oPosition.nY = y;// - m_cImage.m_nCgYpos * 2;
		strcpy(m_Image.szImage, m_cImage.m_szName);
	}

	switch(m_nKind)
	{
	case Obj_Kind_MapObj:
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	case Obj_Kind_Prop:
		if ( m_nState == OBJ_PROP_STATE_DISPLAY )
			g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	default:
		m_Image.uImage = 0;
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	}
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		KRULine	Line;
		int nX, nY;
		SubWorld[0].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, 0, 0, &nX, &nY);
		Line.Color.Color_dw = 0xffffffff;
		Line.oPosition.nZ = 0;
		Line.oEndPos.nZ = 0;

		Line.oPosition.nX = nX;
		Line.oPosition.nY = nY;
		Line.oEndPos.nX = nX + 32;
		Line.oEndPos.nY = nY + 32;
		g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 0);

	}
#endif
}

#endif

//-------------------------------------------------------------------------
//	功能：	各种物件的处理
//-------------------------------------------------------------------------
void	KObj::Activate()
{
	if ( m_nIndex <= 0 )
		return;

#ifndef _SERVER
	int		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
#endif

	DWORD currentTime = SubWorld[m_nSubWorldID].m_dwCurrentTime;

	switch(m_nKind)
	{
#ifndef _SERVER
	case Obj_Kind_MapObj:					// 地图物件动画
		m_nLifeTime--;
		if (m_nLifeTime <= 0)
		{
			if ( g_DestPosIdx != -1 )
				Remove(FALSE);

			g_DestPosIdx = -1;
			return;
		}

		if (m_nState == 0)					// 地图物件动画循环播放
			m_cImage.GetNextFrame();
		
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		break;
	case Obj_Kind_Light:					// 光源
		if (m_nState == 0)
			m_cImage.GetNextFrame();
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		break;
	case Obj_Kind_LoopSound:				// 循环音效
		PlayLoopSound();
		break;
	case Obj_Kind_RandSound:				// 随机音效
		PlayRandSound();
		break;
	case Obj_Kind_Body:						// 尸体逐渐消失
		m_cImage.GetNextFrame(FALSE);		// 尸体动画单方向播放
		m_nLifeTime--;
		if (m_nLifeTime <= 0)
			Remove(FALSE);
		nMask = IPOT_RL_COVER_GROUND | IPOT_RL_INFRONTOF_ALL;
		break;
#endif
	case Obj_Kind_Box:						// 宝箱
#ifdef _SERVER
		if (m_nState == OBJ_BOX_STATE_OPEN)	// 宝箱关闭重生
		{
			m_nBornTime--;
			if (m_nBornTime <= 0)
				SetState(OBJ_BOX_STATE_CLOSE);
		}
#else
		if (m_nState == OBJ_BOX_STATE_CLOSE)// 宝箱关闭状态
			m_cImage.GetPrevFrame(FALSE);
		else// if (m_nState == OBJ_BOX_STATE_OPEN)// 宝箱打开状态
			m_cImage.GetNextFrame(FALSE);
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
#endif
		break;
	case Obj_Kind_Door:						// 门
#ifdef _SERVER
#else
		if (m_nState == OBJ_DOOR_STATE_CLOSE)// 门关闭状态
			m_cImage.GetPrevFrame(FALSE);
		else// if (m_nState == OBJ_DOOR_STATE_OPEN)// 门打开状态
			m_cImage.GetNextFrame(FALSE);
		nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
#endif
		break;
	case Obj_Kind_Item:						// 装备动画循环播放
#ifdef _SERVER
		if (m_nBelong >= 0)
		{
			m_nBelongTime--;
			if (Item[m_nItemDataID].GetDropFlag() != 2)
			{
				if (m_nBelongTime <= 0)
				{
					m_nBelongTime = 0;
					m_nBelong = -1;
				}
			}
		}

		if (m_DisappearTime <= currentTime)
			Remove(FALSE);

// 		m_nLifeTime--;
// 		if (m_nLifeTime <= 0)
// 			Remove(FALSE);
#else
		if (this->m_nDropState == 1)		// 物品掉出动画
		{
			if (m_cImageDrop.GetNextFrame(FALSE))
			{
				if (m_cImageDrop.CheckEnd())
				{
					m_nDropState = 0;			// 物品掉出动画播放完了，改为放置循环动画
					m_Image.uImage = 0;
				}
			}
			nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		}
		else
		{
			if (this->m_nDropState == 2)
			{
				if (m_cImage.GetNextFrame(FALSE))
				{
					if (m_cImage.CheckEnd())
						this->m_nDropState = 0;
				}
			}
			else
			{
				m_nDropState = 2;
				m_cImage.SetDirStart();
//				if (g_Random(40) == 0)
//				{
//					m_nDropState = 2;
//				}
//				else
//				{
//					m_cImage.SetDirStart();
//				}
			}
			nMask = IPOT_RL_COVER_GROUND | IPOT_RL_INFRONTOF_ALL;
		}
// 		m_nLifeTime--;
// 		if (m_nLifeTime <= -100)
// 			Remove(FALSE);
#endif
		break;
	case Obj_Kind_Money:
#ifdef _SERVER
		if (m_nBelong >= 0)
		{
			m_nBelongTime--;
			if (m_nBelongTime <= 0)
			{
				m_nBelongTime = 0;
				m_nBelong = -1;
			}
		}

		if (m_DisappearTime <= currentTime)
			Remove(FALSE);

// 		m_nLifeTime--;
// 		if (m_nLifeTime <= 0)
// 			Remove(FALSE);
#else
		m_cImage.GetNextFrame();
		nMask = IPOT_RL_COVER_GROUND | IPOT_RL_INFRONTOF_ALL;
// 		m_nLifeTime--;
// 		if (m_nLifeTime <= -100)
// 			Remove(FALSE);
#endif
		break;
	case Obj_Kind_Prop:
#ifdef _SERVER
		if (m_nState == OBJ_PROP_STATE_HIDE)	// 道具隐藏状态
		{
			m_nBornTime--;
			if (m_nBornTime <= 0)
				SetState(OBJ_PROP_STATE_DISPLAY);// 道具重生
		}
#else
		if (m_nState == OBJ_PROP_STATE_DISPLAY)
		{
			m_cImage.GetNextFrame();
			nMask = IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL;
		}
#endif
		break;

	case Obj_Kind_House_Entry:
		{
#ifdef _SERVER
#else
#endif
		}
		break;

	case Obj_Kind_Furniture:
		{
#ifdef _SERVER
#else
#endif
		}
		break;
	case Obj_Kind_Trap:
 		{
#ifdef _SERVER

			if( m_cSkill.m_nSkillOnly == -1 )
				m_nLifeTime = 0;
			else if(m_nLifeTime > 0)
			{
				int nAttackTarget = -1;

				if( m_cSkill.m_nSkillOnly )
				{
					//单攻
					//一次触发
					nAttackTarget = FindEnemy( );
					
					if( nAttackTarget != -1 )
					{
						//do skill
						TrapAct( nAttackTarget );
						m_nLifeTime = 0;
					}
				}
				else
				{
					//多攻
					if( m_cSkill.m_nKind )
					{
						//一次施放

						if( m_cSkill.m_nCamp )
						{
							//需要触发

							nAttackTarget = FindEnemy( );
							
							if( nAttackTarget != -1 )
							{
								//do skill
								TrapAct( nAttackTarget );
								m_nLifeTime = 0;
							}

						}
						else
						{
							//不需要触发

							TrapAct( nAttackTarget );
							m_nLifeTime = 0;
						}
					}
					else
					{
						//多次施放

						if( !m_cSkill.m_nCastTime )
							m_nLifeTime = 0;

						if( m_cSkill.m_nCamp )
						{
							//需要触发

							if( m_nAct )
							{
								//已触发
								if( !(m_nLifeTime % m_cSkill.m_nCastTime ) )
									TrapAct( nAttackTarget );
							}
							else
							{
								nAttackTarget = FindEnemy( );
								
								if( nAttackTarget != -1 )
								{
									//do skill
									TrapAct( nAttackTarget );
									m_nAct = TRUE;
								}
								//没触发
							}
						}
						else
						{
							//不需要触发
							if( !(m_nLifeTime % m_cSkill.m_nCastTime ) )
								TrapAct( nAttackTarget );
						}
					}
				}
			}


			// 如果陷阱触发后立即就删除掉，客户端将无法显示特效，
			// 所以配置表中增加了一列，表示陷阱触发后持续多长时间才删掉
			if(m_nLeftTimeAfterTrapAct > 0)
				m_nLeftTimeAfterTrapAct--;

			if(m_nLifeTime > 0)
			{
				m_nLifeTime--;		
			}
			else
			{
				if(m_nLeftTimeAfterTrapAct <= 0)
				{
					BuffMgr& BM = BuffMgr::Singleton( );
					BM.RemoveTrap( m_nIndex );
				}
			}

			//无限时间没做
// 			if( m_nLifeTime != -1 )
// 			{
// 				m_nLifeTime--;
//  				if (m_nLifeTime <= 0)
//  				{
// 					BuffMgr& BM = BuffMgr::Singleton( );
// 					BM.RemoveTrap( m_nIndex );
//  					//Remove(FALSE);
//  				}
// 			}

#else
			if (this->m_nState == 2)		// 物品掉出动画
			{
				if ( m_nImageDropPlayType == 1 )
				{
					if ( !m_cImageDrop.CheckEnd() )
					{
						m_cImageDrop.GetNextFrame(FALSE);
					}
					else
					{
						Remove( FALSE );
					}					
				}
				else
				{
					m_cImageDrop.GetNextFrame(TRUE);
					m_nWaitTime--;
					if (m_nWaitTime <= 0)
						Remove(FALSE);
						//*/

				}
				
			}
			else if (m_nState == 1 )
			{
				if ( m_nImagePlayType == 1 )
				{
					if ( !m_cImage.CheckEnd() )
					{
						m_cImage.GetNextFrame(FALSE);
					}
					else
					{
						Remove( FALSE );
					}					
				}
				else
				{
					m_cImage.GetNextFrame(TRUE);
				}
			}//*/
			else
			{
				m_cImage.GetNextFrame(TRUE);
				m_nLifeTime--;
				if (m_nLifeTime <= 0)
					Remove(FALSE);
					//*/
			}

#endif
 		}
 		break;
	}

#ifndef _SERVER
	int nMpsX, nMpsY;
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &nMpsX, &nMpsY);
	g_ScenePlace.MoveObject(CGOG_OBJECT, m_nIndex, nMpsX, nMpsY, 0, m_SceneID, nMask);
#endif
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：	播放循环音效
//-------------------------------------------------------------------------
void	KObj::PlayLoopSound()
{
	if (!m_szSoundName[0])
		return;

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode * )m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (!m_pWave->IsPlaying())
		{
			m_pWave->Play(GetSoundPan(), GetSoundVolume(), 0);
		}
		else
		{
			m_pWave->SetPan(GetSoundPan());
			m_pWave->SetVolume(GetSoundVolume());
		}
	}
}

//-------------------------------------------------------------------------
//	功能：	播放随机音效
//-------------------------------------------------------------------------
void	KObj::PlayRandSound()
{
	if (!m_szSoundName[0])
		return;

	if (g_Random(500) != 0)
		return;

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode * )m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(GetSoundPan(), GetSoundVolume(), 0);
	}
}

//-------------------------------------------------------------------------
//	功能：	得到声音音相大小
//-------------------------------------------------------------------------
int		KObj::GetSoundPan()
{
	int		nNpcX, nNpcY, nObjX, nObjY;

	SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].Map2Mps(
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMapX(),
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMapY(),
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetOffX(),
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetOffY(),
		&nNpcX,
		&nNpcY);
	SubWorld[m_nSubWorldID].Map2Mps(
		m_nRegionIdx,
		m_nMapX,
		m_nMapY,
		m_nOffX,
		m_nOffY,
		&nObjX,
		&nObjY);

	return (nObjX - nNpcX) * 10;
}

//-------------------------------------------------------------------------
//	功能：	得到声音音量大小
//-------------------------------------------------------------------------
int		KObj::GetSoundVolume()
{
	int		nNpcX, nNpcY, nObjX, nObjY;

	SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].Map2Mps(
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_RegionIndex,
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMapX(),
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMapY(),
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetOffX(),
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetOffY(),
		&nNpcX,
		&nNpcY);
	SubWorld[m_nSubWorldID].Map2Mps(
		m_nRegionIdx,
		m_nMapX,
		m_nMapY,
		m_nOffX,
		m_nOffY,
		&nObjX,
		&nObjY);

//	return ((10000 - (abs(nObjX - nNpcX) + abs(nObjY - nNpcY) * 2)) * Option.GetSndVolume() / 100) - 10000;
//	return -((abs(nObjX - nNpcX) + abs(nObjY - nNpcY) * 2) * Option.GetSndVolume() / 100);

	float dist = sqrt((nObjX-nNpcX)*(nObjX-nNpcX)+(nObjY-nNpcY)*(nObjY-nNpcY));
	return Option.GetSndVolume(dist);
}

//-------------------------------------------------------------------------
//	功能：	播放声音
//-------------------------------------------------------------------------
void	KObj::PlaySound()
{
	if (!m_szSoundName[0])
		return;

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode * )m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(GetSoundPan(), GetSoundVolume(), 0);
	}
}

#endif

//-------------------------------------------------------------------------
//	功能：	运行物件脚本
//-------------------------------------------------------------------------
void	KObj::ExecScript(int nPlayerIdx)
{
	if (!m_dwScriptID)
		return;
	if (nPlayerIdx < 0)
		return;
	DWORD dwScriptId = m_dwScriptID;//g_FileName2Id(m_szScriptName);
	KLuaScript * pScript = (KLuaScript*)g_GetScript(dwScriptId);
	try
	{
		if (pScript)
		{
			if (Player[nPlayerIdx].m_nIndex < 0) return ;
			Npc[Player[nPlayerIdx].m_nIndex].m_ActionScriptID = dwScriptId;
			ScriptSetPlayerIndex(Player[nPlayerIdx].GetPlayerIndex());
			ScriptSetObjIndex(m_nIndex);
// 			Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIdx].GetPlayerIndex());
// 			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIdx].GetPlayerID());
// 			pScript->SetGlobalName(SCRIPT_PLAYERID);
// 			
// 			Lua_PushNumber(pScript->m_LuaState, m_nIndex);
// 			pScript->SetGlobalName(SCRIPT_OBJINDEX);
			
			int nTopIndex = 0;
			pScript->SafeCallBegin(&nTopIndex);
			
			BOOL bResult = FALSE;
			bResult = pScript->CallFunction("main", 0, "");
			pScript->SafeCallEnd(nTopIndex);
			
		}
	}
	catch(...)
	{
	}
	return;
}

//-------------------------------------------------------------------------
//	功能：	朝方向发射子弹
//-------------------------------------------------------------------------
void	KObj::CastSkill(int nDir)
{
	if (m_cSkill.m_nID <= 0 || m_cSkill.m_nLevel < 0)
		return;
//	Skill[m_nID][m_nLevel].cast();
}

//-------------------------------------------------------------------------
//	功能：	朝目标点发射子弹
//-------------------------------------------------------------------------
void	KObj::CastSkill(int nXpos, int nYpos)
{
	if (m_cSkill.m_nID <= 0 || m_cSkill.m_nLevel < 0)
		return;
//	Skill[m_nID][m_nLevel].cast();
}

#ifdef _SERVER
BOOL	KObj::SyncAdd(int nPlayerIndex)
{
	if (IsValidPlayer(nPlayerIndex))
	{
		OBJ_ADD_SYNC	cObjAdd;
		int				nTempX, nTempY;
		
		cObjAdd.ProtocolType = (BYTE)s2c_objadd;
		cObjAdd.m_nID        = m_nID;
		cObjAdd.m_nDataID    = m_nDataID;
		cObjAdd.m_btDir      = m_nDir;
		cObjAdd.m_btState    = m_nState;

		//如果外置遥控器需要在非战斗时候捡东西，则需要开放以下代码用于同步物品归属.
	    /*
		unsigned long  dwPlayerCanPick = 0;
		if ( ( m_nKind == Obj_Kind_Item || m_nKind == Obj_Kind_Money ) && Player[nPlayerIndex].CheckCanPickOBJByIndex(m_nIndex))
		{
			dwPlayerCanPick = 0x80;
		}//endif

		_ASSERT(m_nState <= 0x7f);
       
		unsigned long  dwState        = m_nState;
		unsigned long  dwFinalbtState = ( m_nState | dwPlayerCanPick );
		cObjAdd.m_btState             = dwFinalbtState; 
		*/

		cObjAdd.m_wCurFrame = m_cImage.m_nCurFrame;
		SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, &nTempX, &nTempY);
		cObjAdd.m_nXpos = nTempX;
		cObjAdd.m_nYpos = nTempY;
		cObjAdd.m_nMoneyNum = m_nMoneyNum;
		cObjAdd.m_nItemID = m_nItemDataID;
		cObjAdd.m_btColorID = this->m_nColorID;
		cObjAdd.m_btFlag = 0;
		strcpy(cObjAdd.m_szName, this->m_szName);
		cObjAdd.m_wLength = sizeof(OBJ_ADD_SYNC) - 1 - sizeof(cObjAdd.m_szName) + strlen(cObjAdd.m_szName);
		
		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&cObjAdd, cObjAdd.m_wLength + 1);
		
	}//endif
	return TRUE;
}

void	KObj::SyncState()
{
	OBJ_SYNC_STATE	cObjState;

	cObjState.ProtocolType = (BYTE)s2c_syncobjstate;
	cObjState.m_nID = m_nID;
	cObjState.m_btState = (BYTE)m_nState;

	int nMaxCount = MAX_BROADCAST_COUNT_MIN;
	BROADCAST_REGION(&cObjState,sizeof(cObjState),nMaxCount);
}

void	KObj::SyncDir()
{
	OBJ_SYNC_DIR	cObjDir;

	cObjDir.ProtocolType = (BYTE)s2c_syncobjdir;
	cObjDir.m_nID = m_nID;
	cObjDir.m_btDir = (BYTE)m_nDir;

	int nMaxCount = MAX_BROADCAST_COUNT_MIN;
	BROADCAST_REGION(&cObjDir,sizeof(cObjDir),nMaxCount);
}

void	KObj::SyncRemove(BOOL bSoundFlag, int nPlayerID, BOOL bPickUp)
{
	OBJ_SYNC_REMOVE		cObjSyncRemove;

	cObjSyncRemove.ProtocolType		= (BYTE)s2c_objremove;
	cObjSyncRemove.m_nID			= m_nID;
	cObjSyncRemove.m_btSoundFlag	= bSoundFlag;
	cObjSyncRemove.m_bPickUp		= bPickUp;
	cObjSyncRemove.m_nPlayerID		= nPlayerID;

	int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
	BROADCAST_REGION(&cObjSyncRemove,sizeof(cObjSyncRemove),nMaxCount);
}

void KObj::TrapAct( int nTarget )
{
	if (m_nKind != Obj_Kind_Trap /*|| m_nState == OBJ_TRAP_STATE_STOP*/)
		return;

	KSkill* pSkill = g_SkillManager.GetSkill( m_cSkill.m_nID, 1 );
	if( pSkill )
	{
		int nParam1	= 0;
		int nParam2	= m_nIndex;
		if (pSkill->GetAttackTargetType( ) & att_target_only)
		{
			nParam1 = SKILL_SPT_TargetIndex;
			nParam2 = nTarget;
		}

		if (pSkill->GetSkillStyle() == SKILL_SS_Rectangle)
		{
			nParam1 = m_nDir;
		}

		m_nState = OBJ_TRAP_STATE_ACTING;

		if(m_nLeftTimeAfterTrapAct <= 0)
			m_nLeftTimeAfterTrapAct = m_nWaitTime;

		//播放特效
		//==============================================================
		OBJ_SYNC_TRAP_ACT	cTrapAct;
		cTrapAct.ProtocolType = (BYTE)s2c_objTrapAct;
		cTrapAct.m_nID = m_nID;
		cTrapAct.m_nTarX = m_cSkill.m_nTarX;
		cTrapAct.m_nTarY = m_cSkill.m_nTarY;
		int nMaxCount = MAX_BROADCAST_COUNT_MIN;
		BROADCAST_REGION(&cTrapAct,sizeof(cTrapAct),nMaxCount);
		//==============================================================

		pSkill->Cast( m_nLauncher, nParam1, nParam2, 0, SKILL_SLT_Obj );
	}
}

int KObj::FindEnemyFromRegion( int nRegionIndex )
{
	KIndexNode *pNode = NULL;
	int	nRet = -1;
	
	pNode = (KIndexNode *)SubWorld[m_nSubWorldID].m_Region[nRegionIndex].m_PlayerList.GetHead();
	while(pNode)
	{
		const int nPlayerIndex = pNode->m_nIndex;
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		{
			const int nIndex = Player[nPlayerIndex].m_nIndex;
			if (nIndex > 0 && nIndex < MAX_NPC)
			{
				if( nIndex == m_nLauncher ||
					NpcSet.GetRelation( m_nLauncher, nIndex ) != relation_enemy )
				{
					pNode = (KIndexNode *)pNode->GetNext();
					continue;
				}
				//还需添加阵营判断,添加自我判断
				int nDistance = GetDistanceSquare( nIndex );
				
				if( nDistance <= m_cSkill.m_nRange * m_cSkill.m_nRange )
				{
					return nIndex;
				}
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	pNode = (KIndexNode *)SubWorld[m_nSubWorldID].m_Region[nRegionIndex].m_NpcList.GetHead();
	while(pNode)
	{
		const int nNpcIndex = pNode->m_nIndex;
		if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
		{
			if( nNpcIndex == m_nLauncher ||
				NpcSet.GetRelation( m_nLauncher, nNpcIndex ) != relation_enemy )
			{
				pNode = (KIndexNode *)pNode->GetNext();
				continue;
			}
			
			//还需添加阵营判断,添加自我判断
			int nDistance = GetDistanceSquare( nNpcIndex );
			
			if( nDistance <= m_cSkill.m_nRange * m_cSkill.m_nRange )
			{
				return nNpcIndex;
			}
		}
		pNode = (KIndexNode *)pNode->GetNext();
	}

	return -1;
}

int	KObj::FindEnemy( )
{
	int nIndex = FindEnemyFromRegion( m_nRegionIdx );

	if( nIndex != -1 )
		return nIndex;

	KRegion& CurRegion = SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx];

	for ( int nLoopCount = 0; nLoopCount < DIR_RIGHTDOWN + 1; nLoopCount++ )
	{
		int nRegion = CurRegion.m_nConnectRegion[nLoopCount];
		if ( nRegion != -1 )
		{
			nIndex = FindEnemyFromRegion( nRegion );

			if( nIndex != -1 )
				return nIndex;
		}
	}

	return -1;
}

#endif

#ifdef _SERVER
void	KObj::SetEntireBelong(int nPlayerIdx)
{
	m_nBelong = nPlayerIdx;
	if (m_nBelong >= 0)
		this->m_nBelongTime = this->m_nLifeTime * 2;
	else
		this->m_nBelongTime = 0;
}
#endif

void	KObj::Remove(BOOL bSoundFlag, int nID, BOOL bPickUp)
{
#ifdef _SERVER
	SyncRemove(bSoundFlag, nID, bPickUp);
#else
	if (bSoundFlag)
	{
		switch (m_nKind)
		{
		case Obj_Kind_Money:
			PlaySound();
			break;
		//--> Rocker 2005/05/12
		case Obj_Kind_Furniture:
			PlaySound();
			break;
		case Obj_Kind_House_Entry:
			PlaySound();
			break;
		//<-- End
		case Obj_Kind_Box:
		case Obj_Kind_Item:
		case Obj_Kind_Door:
		case Obj_Kind_Prop:
			break;
		}
	}
	m_Image.uImage = 0;
	g_ScenePlace.RemoveObject(CGOG_OBJECT, m_nIndex, m_SceneID);
#endif
#ifndef _SERVER
	if (m_nRegionIdx >= 0)
		SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].RemoveObj(m_nIndex);
	ObjSet.Remove(m_nIndex);
#else
	if (m_nID != -1)
	{
		if (m_nSubWorldID >= 0)
			SubWorld[m_nSubWorldID].m_Region[m_nRegionIdx].RemoveObj(m_nIndex);
		ObjSet.Remove(m_nIndex);
	}
#endif
}

//-------------------------------------------------------------------------
//	功能：	凸多边形转换为障碍信息
//-------------------------------------------------------------------------
//void	KObj::PolygonChangeToBar(
//								 KPolygon Polygon,	// 凸多边形
//								 int nGridWidth,	// 格子长
//								 int nGridHeight,	// 格子宽
//								 int nTableWidth,	// 表格长
//								 int nTableHeight,	// 表格宽
//								 BYTE *lpbBarTable)	// 表格内容
//{
//	if ( !lpbBarTable )
//		return;
//	if (nGridWidth <= 0 || nGridHeight <= 0 || nTableWidth <= 0 || nTableHeight <= 0)
//		return;
//
//	int		nTemp, nTempLT, nTempRT, nTempLB, nTempRB, nFlag = 0;
//	POINT	TempPos;
//	for (int i = 0; i < nTableWidth * nTableHeight; i++)
//	{
//		Polygon.GetCenterPos(&TempPos);
//		// 左上
//		TempPos.x += ((i % nTableWidth) * nGridWidth) - ((nTableWidth / 2) * nGridWidth + nGridWidth / 2);
//		TempPos.y += ((i / nTableWidth) * nGridHeight) - ((nTableHeight / 2) * nGridHeight + nGridHeight / 2);
//		nTempLT = Polygon.IsPointInPolygon(TempPos);
//		// 右上
//		TempPos.x += nGridWidth;
//		nTempRT = Polygon.IsPointInPolygon(TempPos);
//		// 左下
//		TempPos.x -= nGridWidth;
//		TempPos.y += nGridHeight;
//		nTempLB = Polygon.IsPointInPolygon(TempPos);
//		// 右下
//		TempPos.x += nGridWidth;
//		nTempRB = Polygon.IsPointInPolygon(TempPos);
//
//		nTemp = nTempLT + nTempRT + nTempLB + nTempRB;
//		if (nTemp == 0)
//			lpbBarTable[i] = Obj_Bar_Empty;
//		else if (nTemp > 1)
//		{
//			lpbBarTable[i] = Obj_Bar_Full;
//			nFlag = 1;
//		}
//		else
//		{
//			if (nTempLT)
//				lpbBarTable[i] = Obj_Bar_LT;
//			else if (nTempRT)
//				lpbBarTable[i] = Obj_Bar_RT;
//			else if (nTempLB)
//				lpbBarTable[i] = Obj_Bar_LB;
//			else if (nTempRB)
//				lpbBarTable[i] = Obj_Bar_RB;
//		}
//	}
//
//	lpbBarTable[(nTableHeight / 2) * nTableWidth + nTableWidth / 2] = Obj_Bar_Full;
// }


#ifdef _SERVER
ServerImage::ServerImage()
{
	Release();
}

void	ServerImage::Release()
{
	m_nTotalFrame = 1;
	m_nCurFrame = 0;
	m_nTotalDir = 1;
	m_nCurDir = 0;
	m_dwTimer = 0;
	m_dwInterval = 0;
	m_nDirFrames = 1;
}

//---------------------------------------------------------------------------
//	功能：	设定总帧数
//---------------------------------------------------------------------------
void	ServerImage::SetTotalFrame(int nTotalFrame)
{
	if (nTotalFrame > 0)
	{
		m_nTotalFrame = nTotalFrame;
		m_nDirFrames = m_nTotalFrame / m_nTotalDir;
	}
}

//---------------------------------------------------------------------------
//	功能：	设定当前帧
//---------------------------------------------------------------------------
void	ServerImage::SetCurFrame(int nCurFrame)
{
	if (nCurFrame < 0 || nCurFrame >= m_nTotalFrame)
		return;
	m_nCurFrame = nCurFrame;
	if (m_nTotalFrame && m_nTotalDir)
		m_nCurDir = nCurFrame / m_nDirFrames;
	else
		m_nCurDir = 0;
	m_dwTimer = SubWorld[0].m_dwCurrentTime;
}

//---------------------------------------------------------------------------
//	功能：	设定总方向数
//---------------------------------------------------------------------------
void	ServerImage::SetTotalDir(int nTotalDir)
{
	if (nTotalDir > 0)
	{
		m_nTotalDir = nTotalDir;
		m_nDirFrames = m_nTotalFrame / m_nTotalDir;
	}
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向
//---------------------------------------------------------------------------
BOOL	ServerImage::SetCurDir(int nDir)
{
	if (m_nCurDir == nDir)
		return TRUE;
	if (nDir < 0 || nDir >= m_nTotalDir)
		return FALSE;
	m_nCurDir = nDir;
	m_nCurFrame = m_nDirFrames * nDir;
	m_dwTimer = SubWorld[0].m_dwCurrentTime;
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	设定帧间隔
//---------------------------------------------------------------------------
void	ServerImage::SetInterVal(DWORD dwInterval)
{
	m_dwInterval = dwInterval;
}

//---------------------------------------------------------------------------
//	功能：	获得单方向帧数
//---------------------------------------------------------------------------
int		ServerImage::GetOneDirFrames()
{
	return m_nDirFrames;
}

//---------------------------------------------------------------------------
//	功能：	判断动画是否播放到最后，当前是第 0 帧
//---------------------------------------------------------------------------
BOOL	ServerImage::CheckEnd()
{
	if (m_nCurFrame == m_nDirFrames * (m_nCurDir + 1) - 1)
		return TRUE;
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向的当前帧为第一帧
//---------------------------------------------------------------------------
void	ServerImage::SetDirStart()
{
	m_nCurFrame = m_nCurDir * m_nDirFrames;
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向的当前帧为最后一帧
//---------------------------------------------------------------------------
void	ServerImage::SetDirEnd()
{
	m_nCurFrame = (m_nCurDir + 1) * m_nDirFrames - 1;
}

//---------------------------------------------------------------------------
//	功能：	取得当前方向的下一帧
//---------------------------------------------------------------------------
BOOL	ServerImage::GetNextFrame(BOOL bLoop)
{
	if (SubWorld[0].m_dwCurrentTime - m_dwTimer >= m_dwInterval)
	{
		m_dwTimer = SubWorld[0].m_dwCurrentTime;
		m_nCurFrame++;
		if (m_nCurFrame >= m_nDirFrames * (m_nCurDir + 1))
		{
			if (bLoop)
				m_nCurFrame = m_nDirFrames * m_nCurDir;
			else
				m_nCurFrame = m_nDirFrames * (m_nCurDir + 1) - 1;
		}
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	取得当前方向的前一帧
//---------------------------------------------------------------------------
BOOL	ServerImage::GetPrevFrame(BOOL bLoop)
{
	if (SubWorld[0].m_dwCurrentTime - m_dwTimer >= m_dwInterval)
	{
		m_dwTimer = SubWorld[0].m_dwCurrentTime;
		m_nCurFrame--;
		if (m_nCurFrame < m_nDirFrames * m_nCurDir)
		{
			if (bLoop)
				m_nCurFrame = m_nDirFrames * (m_nCurDir + 1) - 1;
			else
				m_nCurFrame = m_nDirFrames * m_nCurDir;
		}
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	设定当前方向(方向需从64方向转换到真正的方向)
//---------------------------------------------------------------------------
BOOL	ServerImage::SetCurDir64(int nDir)
{
	if (nDir < 0 || nDir >= 64)
		return FALSE;

	int nTempDir;

	nTempDir = (nDir + (32 / m_nTotalDir)) / (64 / m_nTotalDir);
	if (nTempDir >= m_nTotalDir)
		nTempDir -= m_nTotalDir;
	if (m_nCurDir == nTempDir)
		return TRUE;
	m_nCurDir = nTempDir;
	m_nCurFrame = m_nDirFrames * nTempDir;
	m_dwTimer = SubWorld[0].m_dwCurrentTime;
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	获得当前方向第几帧
//---------------------------------------------------------------------------
int		ServerImage::GetCurDirFrameNo()
{
	return m_nCurFrame - m_nCurDir * m_nDirFrames;
}

#endif

void KObj::GetMpsPos(int *pX, int *pY)
{
	SubWorld[m_nSubWorldID].Map2Mps(m_nRegionIdx, m_nMapX, m_nMapY, m_nOffX, m_nOffY, pX, pY);
}

int KObj::GetDistanceSquare( int nNpcIndex )
{
	int	nRet = 0;
	if ( m_nSubWorldID != Npc[nNpcIndex].m_SubWorldIndex)
		return -1;
	
	int XOff = 0;
	int YOff = 0;
	
	if (m_nRegionIdx == Npc[nNpcIndex].m_RegionIndex)
	{
		XOff = (m_nMapX - Npc[nNpcIndex].GetMapX()) * REGION_CELL_SIZE_X;
		XOff += (m_nOffX - Npc[nNpcIndex].GetOffX()) >> 10;
		
		YOff = (m_nMapY - Npc[nNpcIndex].GetMapY()) * REGION_CELL_SIZE_Y;
		YOff += (m_nOffY - Npc[nNpcIndex].GetOffY()) >> 10;
	}
	else
	{
		int X1, Y1;
		int X2, Y2;
		GetMpsPos( &X1, &Y1 );
		Npc[nNpcIndex].GetMpsPos( &X2, &Y2 );
		
		XOff = (X2 - X1);
		YOff = (Y2 - Y1);		
	}

	nRet = (int)(XOff * XOff + YOff * YOff);

	return nRet;	
}


#ifndef _SERVER


void KObj::DrawBorder()
{
	if (m_bDrawFlag)
		return;
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_BORDER;
	switch(m_nKind)
	{
	case Obj_Kind_MapObj:
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	case Obj_Kind_Prop:
		if ( m_nState == OBJ_PROP_STATE_DISPLAY )
			//m_cImage.DrawAlpha(x, y);
			g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	default:
		//m_cImage.DrawAlpha(x, y);
		g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, 0);	
		break;
	}
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
}
#endif
