//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KNpcRes.cpp
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Obj Class
//---------------------------------------------------------------------------

#include "KCore.h"

#ifndef _SERVER

#include "KSprite.h"
#include "KOption.h"
#include "KNpc.h"
#include "KNpcResList.h"
#include "KNpcRes.h"
#include "ImgRef.h"
#include "scene/KScenePlaceC.h"
#include "KSubWorld.h"
#include "KOption.h"
#include "KItemChangeRes.h"
#include <time.h>
#include "KNpcTemplate.h"
#include "CoreShell.h"

extern BOOL g_CoreHighQualityPaint;

KUiImagePartRef		KNpcRes::m_imgNpcBkgnd[];
int					KNpcRes::m_bNpcLeftBarTyp[];
KUiImagePartRef		KNpcRes::m_imgNpcLifeBar[];
KUiImagePartRef		KNpcRes::m_imgNumber;
KUiImagePartRef		KNpcRes::m_imgExceed;
POINT				KNpcRes::m_ptNpcLifeBarPos[];
POINT				KNpcRes::m_ptNpcLevelTextStartPos[];
SIZE				KNpcRes::m_sizeNpcLevelTxtRect[];
KUiImageRef			KNpcRes::selImageEnemy;
KUiImageRef			KNpcRes::selImageNormal;
KUiImageRef			KNpcRes::m_HoverImage;
KUiImageRef			KNpcRes::m_SelectImage;
KUiImageRef			KNpcRes::m_RideHoverImage;
KUiImageRef			KNpcRes::m_RideSelectImage;
KUiImageRef			KNpcRes::QuestImage[];
KUiImageRef			KNpcRes::m_BloodImage[];



//---------------------------------------------------------------------------
//	功能：	构造函数
//---------------------------------------------------------------------------
KNpcRes::KNpcRes()
{
	ZeroMemory(this, sizeof(KNpcRes));
	m_nNpcKind = 1;
}

BOOL	KNpcRes::InitBloodTemplate()
{
	//通用血条系统
	KIniFile ini;
	if ( !ini.Load(FACE_LAYOUT_STRING) )
	{
		return FALSE;
	}

	KImageParam param;			
	
	IR_InitUiImagePartRef( KNpcRes::m_imgNumber );
	KNpcRes::m_imgNumber.nType = ISI_T_SPR;

	ini.GetString( "Symbol", "NumberImage", "", KNpcRes::m_imgNumber.szImage, sizeof( KNpcRes::m_imgNumber.szImage ) );
	g_pRepresentShell->GetImageParam( KNpcRes::m_imgNumber.szImage, 
		&param, KNpcRes::m_imgNumber.nType );
	
	KNpcRes::m_imgNumber.Width = param.nWidth;
	KNpcRes::m_imgNumber.Height = param.nHeight;
	
	IR_InitUiImagePartRef( KNpcRes::m_imgExceed );
	KNpcRes::m_imgExceed.nType = ISI_T_SPR;

	ini.GetString( "Symbol", "ExceedImage", "", KNpcRes::m_imgExceed.szImage, sizeof( KNpcRes::m_imgExceed.szImage ) );
	g_pRepresentShell->GetImageParam( KNpcRes::m_imgExceed.szImage, 
		&param, KNpcRes::m_imgExceed.nType );
	
	KNpcRes::m_imgExceed.Width = param.nWidth;
	KNpcRes::m_imgExceed.Height = param.nHeight;

	for ( int nIdx = 0; nIdx < KNpcRes::MAX_NPC_LIFE_BAR_TYPE; ++nIdx )
	{
		char szBuf[32];		
		sprintf( szBuf, "NpcLifeBar_%d", nIdx );
		
		IR_InitUiImagePartRef( KNpcRes::m_imgNpcBkgnd[nIdx] );
		KNpcRes::m_imgNpcBkgnd[nIdx].nType = ISI_T_SPR;
		ini.GetString( szBuf, "BkgndImage", "", 
			KNpcRes::m_imgNpcBkgnd[nIdx].szImage, sizeof( KNpcRes::m_imgNpcBkgnd[nIdx].szImage ) );
		
		g_pRepresentShell->GetImageParam( KNpcRes::m_imgNpcBkgnd[nIdx].szImage, 
			&param, KNpcRes::m_imgNpcBkgnd[nIdx].nType );
		
		KNpcRes::m_imgNpcBkgnd[nIdx].Width = param.nWidth;
		KNpcRes::m_imgNpcBkgnd[nIdx].Height = param.nHeight;
		
		IR_InitUiImagePartRef( KNpcRes::m_imgNpcLifeBar[nIdx] );
		KNpcRes::m_imgNpcLifeBar[nIdx].nType = ISI_T_SPR;

		ini.GetString( szBuf, "LifeBarImage", "", 
			KNpcRes::m_imgNpcLifeBar[nIdx].szImage, sizeof( KNpcRes::m_imgNpcLifeBar[nIdx].szImage ) );
		KImageParam param;
		g_pRepresentShell->GetImageParam( KNpcRes::m_imgNpcLifeBar[nIdx].szImage, 
			&param, KNpcRes::m_imgNpcLifeBar[nIdx].nType );
		
		KNpcRes::m_imgNpcLifeBar[nIdx].Width = param.nWidth;
		KNpcRes::m_imgNpcLifeBar[nIdx].Height = param.nHeight;
		
		ini.GetInteger2( szBuf, "LifeBarPos", ( int* )&KNpcRes::m_ptNpcLifeBarPos[nIdx].x,
			( int* )&KNpcRes::m_ptNpcLifeBarPos[nIdx].y );

		ini.GetInteger2( szBuf, "LevelTxtPos", ( int* )&KNpcRes::m_ptNpcLevelTextStartPos[nIdx].x, 
			( int* )&KNpcRes::m_ptNpcLevelTextStartPos[nIdx].y );
		
		ini.GetInteger2( szBuf, "LevelTxtSize", ( int* )&KNpcRes::m_sizeNpcLevelTxtRect[nIdx].cx, 
			( int* )&KNpcRes::m_sizeNpcLevelTxtRect[nIdx].cy );

		ini.GetInteger( szBuf, "LifeBarType", 0, &m_bNpcLeftBarTyp[nIdx] );

		//读取选中光环配置
		IR_InitUiImageRef(selImageNormal);
		
		ini.GetString("Data", "SelectImageNormal", "", selImageNormal.szImage, sizeof(selImageNormal.szImage));
		selImageNormal.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		selImageNormal.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		selImageNormal.nType = ISI_T_SPR;
		selImageNormal.Color.Color_dw = 0xFF000000;
		selImageNormal.nFrame = 0;	
		
		IR_InitUiImageRef(selImageEnemy);
		
		ini.GetString("Data", "SelectImageEnemy", "", selImageEnemy.szImage, sizeof(selImageEnemy.szImage));
		selImageEnemy.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		selImageEnemy.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		selImageEnemy.nType = ISI_T_SPR;
		selImageEnemy.Color.Color_dw = 0xFF000000;
		selImageEnemy.nFrame = 0;	

		//读取焦点热点
		IR_InitUiImageRef(m_HoverImage);
		
		ini.GetString("Data", "HoverImage", "", m_HoverImage.szImage, sizeof(m_HoverImage.szImage));
		m_HoverImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_HoverImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_HoverImage.nType = ISI_T_SPR;
		m_HoverImage.Color.Color_dw = 0xFF000000;
		m_HoverImage.nFrame = 0;	
		
		IR_InitUiImageRef(m_SelectImage);
		
		ini.GetString("Data", "SelectImage", "", m_SelectImage.szImage, sizeof(m_SelectImage.szImage));
		m_SelectImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_SelectImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_SelectImage.nType = ISI_T_SPR;
		m_SelectImage.Color.Color_dw = 0xFF000000;
		m_SelectImage.nFrame = 0;	

		IR_InitUiImageRef(m_RideHoverImage);
		
		ini.GetString("Data", "RideHoverImage", "", m_RideHoverImage.szImage, sizeof(m_RideHoverImage.szImage));
		m_RideHoverImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_RideHoverImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_RideHoverImage.nType = ISI_T_SPR;
		m_RideHoverImage.Color.Color_dw = 0xFF000000;
		m_RideHoverImage.nFrame = 0;	
		
		IR_InitUiImageRef(m_RideSelectImage);
		
		ini.GetString("Data", "RideSelectImage", "", m_RideSelectImage.szImage, sizeof(m_RideSelectImage.szImage));
		m_RideSelectImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_RideSelectImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_RideSelectImage.nType = ISI_T_SPR;
		m_RideSelectImage.Color.Color_dw = 0xFF000000;
		m_RideSelectImage.nFrame = 0;	

		char szSection[32];
		for (int i=0; i<MAX_QUEST_ICON; i++)
		{
			KUiImageRef& aImage = QuestImage[i];
			aImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			aImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			aImage.Color.Color_dw = 0xFF000000;
			aImage.nFrame = 0;
			aImage.nType = ISI_T_SPR;
			aImage.oPosition.nX = 0;
			aImage.oPosition.nY = 0;
			aImage.oEndPos.nX = 0;
			aImage.oEndPos.nY = 0;
			aImage.oEndPos.nZ = 0;
			aImage.uImage = 0;
			aImage.nISPosition = -1;
			sprintf( szSection, "Quest_%d", i);
			ini.GetString( "TaskIcon", szSection, "", aImage.szImage, sizeof ( aImage.szImage ) );
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	初始化
//---------------------------------------------------------------------------
BOOL	KNpcRes::Init(char *lpszNpcName, KNpcResList *pNpcResList, int nNpcTemplateIdx,bool binitPos)
{
	// 初始化 NpcResNode
	if (!lpszNpcName || !lpszNpcName[0])
		return FALSE;
	m_pcResNode = pNpcResList->GetNpcRes(lpszNpcName);
	if ( m_pcResNode == NULL )
		return FALSE;

	if ( binitPos )
	{
		m_nXpos = 0;
		m_nYpos = 0;
		m_nZpos = 0;
		m_nXposOld = 0;
		m_nYposOld = 0;
		m_nZposOld = 0;
		m_nXposNew = 0;
		m_nYposNew = 0;
		m_nZposNew = 0;
	}

	m_nNpcResPart = 0;
	m_nLastFrame = -1;
	m_nLastDir = -1;
	m_bNeedSort = true;

	m_nNpcKind = m_pcResNode->GetNpcKind();
	m_nAction = 0;
	memset(m_nPart, -1, sizeof(m_nPart));
	memset(m_nPartPal, Default_PalIndex, sizeof(m_nPartPal));
	m_uHue = 0;
	
	m_bRideHorse = FALSE;
	memset(m_szSoundName, 0, sizeof(m_szSoundName));
	memset(m_nSortTable, 0, sizeof(m_nSortTable));
	m_pSoundNode = NULL;
	m_pWave = NULL;

	m_SceneID_NPCIdx = 0;
	m_SceneID = 0;

//	m_pSprNode = NULL;

	int		i;
	char	szBuffer[80];
	for (i = 0; i < MAX_PART; i++)
	{
		if ( m_pcResNode->CheckPartExist(i) )
		{
			m_pcResNode->GetFileName(i, m_nAction, 0, "", szBuffer, sizeof(szBuffer));
			m_cNpcImage[i].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(i, m_nAction, 0, 16), m_pcResNode->GetTotalDirs(i, m_nAction, 0, 16), m_pcResNode->GetInterval(i, m_nAction, 0, 0));
			m_cNpcImage[i].SetHue(0);
			m_cNpcChanged[i] = true;
		}
		// 如果此部件不存在，对应的文件名都填空
		else
		{
			m_cNpcImage[i].Release();
		}
	}

	int		nShadowFrame, nShadowDir, nShadowInterval, nShadowCgX, nShadowCgY;
	if ( m_pcResNode->m_cShadowInfo.GetFile(
		m_nAction,
		&nShadowFrame,
		&nShadowDir,
		&nShadowInterval,
		&nShadowCgX,
		&nShadowCgY,
		szBuffer) )
	{
		this->m_cNpcShadow.SetSprFile(szBuffer, nShadowFrame, nShadowDir, nShadowInterval);
		this->m_cNpcShadow.SetCenterPos(nShadowCgX, nShadowCgY);
	}
	else
	{
		this->m_cNpcShadow.Release();
	}

	for (i = 0; i < MAX_STATE_PART_NUM; i++)
	{
		m_cStateSpr[i].Release();
	}
	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
	{
		m_cInlayStateSpr[i].Release();
	}
	m_cSpecialSpr.Release();
	m_nMenuState = 0;
	m_nBackMenuState = 0;
	m_nSleepState = 0;
	//memset(m_szSentence, 0, sizeof(m_szSentence));
	//memset(m_szBackSentence, 0, sizeof(m_szBackSentence));

	for (i = 0; i < MAX_NPC_IMAGE_NUM; i++)
	{
		m_cDrawFile[i].nType = ISI_T_SPR;
		m_cDrawFile[i].Color.Color_b.a = 255;
		m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cDrawFile[i].uImage = 0;
		m_cDrawFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cDrawFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_cDrawFile[i].bMultiThreadLoad = true;
	}

	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
	{
		m_cInlayDrawFile[i].nType = ISI_T_SPR;
		m_cInlayDrawFile[i].Color.Color_b.a = 255;
		m_cInlayDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cInlayDrawFile[i].uImage = 0;
		m_cInlayDrawFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cInlayDrawFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
		m_cInlayDrawFile[i].bMultiThreadLoad = true;
	}

	memset(&m_cShadowFile,0, sizeof(m_cShadowFile));
	m_cShadowFile.nType = ISI_T_SPR;
	m_cShadowFile.Color.Color_b.a = 255;
	m_cShadowFile.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_cShadowFile.uImage = 0;
	m_cShadowFile.nISPosition = IMAGE_IS_POSITION_INIT;
	m_cShadowFile.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	strcpy(m_cShadowFile.szImage, m_cNpcShadow.m_szName);
	m_cShadowFile.uImage = m_cNpcShadow.m_dwNameID;
	m_cShadowFile.nFrame = m_cNpcShadow.m_nCurFrame;
	m_cShadowFile.oPosition.nX = 0;
	m_cShadowFile.oPosition.nY = 0;
	m_cShadowFile.oPosition.nZ = 0;

	for (i = 0; i < 2; i++)
	{
		m_cFootFile[i].nType = ISI_T_SPR;
		m_cFootFile[i].Color.Color_b.a = 255;
		m_cFootFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cFootFile[i].uImage = 0;
		m_cFootFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cFootFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	}
	m_nFootNum = 0;

	for (i = 0; i < 2; i++)
	{
		m_cBodyFile[i].nType = ISI_T_SPR;
		m_cBodyFile[i].Color.Color_b.a = 255;
		m_cBodyFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cBodyFile[i].uImage = 0;
		m_cBodyFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cBodyFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	}

	m_nBodyFrontNum = 0;
	m_nBodyBackNum = 0;

	for (i = 0; i < 2; i++)
	{
		m_cHeadFile[i].nType = ISI_T_SPR;
		m_cHeadFile[i].Color.Color_b.a = 255;
		m_cHeadFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cHeadFile[i].uImage = 0;
		m_cHeadFile[i].nISPosition = IMAGE_IS_POSITION_INIT;
		m_cHeadFile[i].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	}
	m_nHeadNum = 0;

	memset( &m_cOnlyFile, 0, sizeof(m_cOnlyFile) );
	m_cOnlyFile.nType = ISI_T_SPR;
	m_cOnlyFile.Color.Color_b.a = 255;
	m_cOnlyFile.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	m_cOnlyFile.uImage = 0;
	m_cOnlyFile.nISPosition = IMAGE_IS_POSITION_INIT;
	m_cOnlyFile.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;

	m_uQuestIcon = 0;

	if ( nNpcTemplateIdx >= 0 )
	{
		for ( int nIdx = 0; nIdx < 8; ++nIdx )
		{
			if (g_pNpcTemplate[nNpcTemplateIdx][0] && 
				g_pNpcTemplate[nNpcTemplateIdx][0]->m_szNpcBloodInfo[0] && 
				g_pNpcTemplate[nNpcTemplateIdx][0]->m_szNpcBloodInfo[0] != '\0' )
			{
				IR_InitUiImageRef( m_BloodImage[nIdx] );
				char szBuff[128];
				sprintf( szBuff, g_pNpcTemplate[nNpcTemplateIdx][0]->m_szNpcBloodInfo, nIdx );
				strncpy(m_BloodImage[nIdx].szImage, szBuff, 128 );
				m_BloodImage[nIdx].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
				m_BloodImage[nIdx].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				m_BloodImage[nIdx].nType = ISI_T_SPR;
				m_BloodImage[nIdx].Color.Color_dw = 0xFF000000;
				m_BloodImage[nIdx].nFrame = 0;	
			}
		}//*/
	}
	 
	return m_cNpcBlur.Init();

}

void	KNpcRes::Remove(int nNpcIdx, bool bRemoveFromScene)
{
	if (m_SceneID)
	{
		// --> Rocker Edit Start 2005/10/27
		if (bRemoveFromScene)
		{
			g_ScenePlace.RemoveObject(CGOG_NPC, nNpcIdx, m_SceneID);
		}
		// <-- Rocker End
		m_SceneID = 0;
	}

	m_cNpcBlur.Remove();
}
//---------------------------------------------------------------------------
//	功能：	绘制
//---------------------------------------------------------------------------

void	KNpcRes::Draw(int nNpcIdx, int nDir, int nAllFrame, int nCurFrame, BOOL bInMenu /* = FALSE */, 
					  BOOL bDrawSelected /* = FALSE */, int nSelectedType /* = 0 */,  int nHover /* = false */,
					  int nDrawType /* = 0 */, int nBeginFrame /* = 0 */, int nEndFrame /* = 0 */ )
{
	int		nStateFrameNo = 0;
	int		j = 0;

	if ( Npc[nNpcIdx].m_Kind == kind_player )
	{
		if ( !Option.IsDrawPlayer() )
		{
			return;
		}
	}
	else
	{
		if ( !Option.IsDrawNpc() )
		{
			return;
		}
	}

	int		i, nGetFrame = 1, nGetDir = 1, nFirst, nPos;
	int		nCurFrameNo = 0, nCurDirNo = 0;
	int		nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = m_nZpos;

	if (nDir < 0 || nAllFrame < 0 || nCurFrame < 0)
	{
		// 死亡时绘制绘制选中光环
		if ( bDrawSelected && Npc[nNpcIdx].m_Doing == do_revive )
		{	
			KUiImageRef *pSelImage;
			if ( nSelectedType == 0 )
			{
				pSelImage = &selImageNormal;
			}
			else 
			{
				pSelImage = &selImageEnemy;
			}
			pSelImage->oPosition.nX = nScreenX;
			pSelImage->oPosition.nY = nScreenY;
			pSelImage->oPosition.nZ = 0;
			g_pRepresent->DrawPrimitives(1, pSelImage, RU_T_IMAGE, bInMenu);
			if ( m_nAction != cdo_stand)
			{
				IR_NextFrame(*pSelImage);
			}
		}
		return;
	}

	if (!m_pcResNode)
		return;
	

	//绘制阴影
	if ( m_nFootNum <= 0 )
	{
		if ( Option.IsDrawShadow() )
		{
			m_cShadowFile.oPosition.nX = nScreenX;
			m_cShadowFile.oPosition.nY = nScreenY;
			g_pRepresent->DrawPrimitives(1, &m_cShadowFile, RU_T_IMAGE, bInMenu);
		}
	}
	else
	{
		m_cFootFile[0].oPosition.nX = nScreenX;
		m_cFootFile[0].oPosition.nY = nScreenY;
		m_cFootFile[1].oPosition.nX = nScreenX;
		m_cFootFile[1].oPosition.nY = nScreenY;
		g_pRepresent->DrawPrimitives(m_nFootNum, m_cFootFile, RU_T_IMAGE, bInMenu);
	}

	// 绘制选中光环
	if ( bDrawSelected )
	{	
		KUiImageRef *pSelImage;
		if ( nSelectedType == 0 )
		{
			pSelImage = &selImageNormal;
		}
		else 
		{
			pSelImage = &selImageEnemy;
		}
		pSelImage->oPosition.nX = nScreenX;
		pSelImage->oPosition.nY = nScreenY;
		pSelImage->oPosition.nZ = 0;
		g_pRepresent->DrawPrimitives(1, pSelImage, RU_T_IMAGE, bInMenu);
		if ( m_nAction != cdo_stand)
		{
			IR_NextFrame(*pSelImage);
		}
	}	

	
	// 外部控制换帧
	nPos = 0;
	if (nAllFrame > 0)
	{
		// 各个部件
		nFirst = 0;
		// 找到动画的当前桢
		for (i = 0; i < MAX_PART; i++)
		{
			if (m_cNpcImage[i].CheckExist())
			{
				if (nFirst == 0)
				{
					nGetDir = m_cNpcImage[i].m_nTotalDir;
					if (nGetDir <= 0)
						nGetDir = 1;

					nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
					if (nCurDirNo >= nGetDir)
						nCurDirNo -= nGetDir;

					nGetFrame = m_cNpcImage[i].m_nTotalFrame;
					if ( nDrawType == normal_action_type )
					{
						nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
					}
					else
					{
						if ( nCurFrame < nAllFrame )
						{
							nCurFrameNo = (nCurDirNo * (nGetFrame / nGetDir) + nBeginFrame) + (nCurFrame % ( nEndFrame - nBeginFrame + 1));
						}							
					}
					
					m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					nFirst = 1;
				}
				else
				{
					KImageParam	sImage;
					if(m_cNpcChanged[i] == true)
					{
						if (g_pRepresent->GetImageParam(m_cNpcImage[i].m_szName, &sImage, ISI_T_SPR))
						{
							m_cNpcImage[i].m_nTotalDir = sImage.nNumFramesGroup;
							m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;
							m_cNpcImage[i].SetCurFrame(nCurFrameNo);
							m_cNpcChanged[i] =  false;
						}
					}
					else
					{
						m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					}
				}
			}
		}
	}
	else
	{
		// 各个部件
		for (i = 0; i < MAX_PART; i++)
		{
			if ( !m_cNpcImage[i].CheckExist() )
				continue;
			if ( m_cNpcImage[i].SetCurDir64(nDir) )
			{
				m_cNpcImage[i].GetNextFrame();

				nCurDirNo = m_cNpcImage[i].m_nCurDir;
				nCurFrameNo = m_cNpcImage[i].m_nCurFrame;
			}
			
		}
	}

	// 装备灵石激活特效(npc背后)
	int nInlayPos = 0;
	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
	{
		if (m_cInlayStateSpr[i].m_nID)
		{
			if (m_cInlayStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_cInlayStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_cInlayStateSpr[i].m_nCopyNum == 1)
			{
				if (m_cInlayStateSpr[i].m_nBackStart <= m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame && 
					m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame < m_cInlayStateSpr[i].m_nBackEnd)
				{
					strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
					m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
					m_cInlayDrawFile[nInlayPos].nFrame = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
					m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
					m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;

					int nHeightOff = 0;
					if (m_bRideHorse)
						nHeightOff += 38;
					nHeightOff += (Npc[nNpcIdx].GetNpcPate()/2);

					m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
					nInlayPos++;
				}
			}
			else
			{
				nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_cInlayStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_cInlayStateSpr[i].m_nCopyNum);

					if (m_cInlayStateSpr[i].m_nBackStart <= nStateFrameNo && nStateFrameNo < m_cInlayStateSpr[i].m_nBackEnd)
					{
						strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
						m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
						m_cInlayDrawFile[nInlayPos].nFrame = nStateFrameNo;
						m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
						m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;

						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						nHeightOff += (Npc[nNpcIdx].GetNpcPate()/2);

						m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
						nInlayPos++;
					}
				}
			}
		}
	}

	g_pRepresent->DrawPrimitives(nInlayPos, m_cInlayDrawFile, RU_T_IMAGE, bInMenu);

	if ( m_nBodyBackNum > 0 )
	{
		m_cBodyFile[0].oPosition.nX = nScreenX;
		m_cBodyFile[0].oPosition.nY = nScreenY;
		m_cBodyFile[1].oPosition.nX = nScreenX;
		m_cBodyFile[1].oPosition.nY = nScreenY;
		g_pRepresent->DrawPrimitives(m_nBodyBackNum, m_cBodyFile, RU_T_IMAGE, bInMenu);
	}



	bool needUpdata = false;
	// 得到部件排列顺序

	if(m_bNeedSort||m_nLastFrame!=nCurFrameNo||m_nLastDir != nCurDirNo)
	{
		needUpdata = true;
		m_nLastFrame = nCurFrameNo;
		m_nLastDir = nCurDirNo;
	}
	if(needUpdata)
	{
		if ( !m_pcResNode->GetSort(m_nAction, nCurDirNo, nCurFrameNo, m_nSortTable, MAX_PART) )
		   return;//*/
	

	
	

	// npc部件
		nPos = 0;
		if ( m_nNpcKind != NPC_RES_NORMAL )
		{
			for (i = 0; i < MAX_PART; i++)
			{

				if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
				{
					if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
					{
						m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
						m_cDrawFile[nPos].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
					}
					else
					{
						m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_HUE_ADJUST;
						m_cDrawFile[nPos].Color.Color_dw = m_cNpcImage[m_nSortTable[i]].GetHue();
						m_cDrawFile[nPos].Color.Color_b.a = 0xFF;
					}

					strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);
					m_cDrawFile[nPos].uImage = m_cNpcImage[m_nSortTable[i]].m_dwNameID;
					m_cDrawFile[nPos].nFrame = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
					m_cDrawFile[nPos].oPosition.nX = nScreenX;
					m_cDrawFile[nPos].oPosition.nY = nScreenY;
					m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
					nPos++;
				}
			}
		}
		else
		{
			if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
			{
				m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
				m_cDrawFile[nPos].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
			}
			else
			{
				m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_HUE_ADJUST;
				m_cDrawFile[nPos].Color.Color_dw = m_uHue;
				m_cDrawFile[nPos].Color.Color_b.a = 0xFF;
			}

			strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[NORMAL_NPC_PART_NO].m_szName);
			m_cDrawFile[nPos].uImage = m_cNpcImage[NORMAL_NPC_PART_NO].m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cNpcImage[NORMAL_NPC_PART_NO].m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
			nPos++;
		}
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);//*/

	if ( cdo_hurt == m_nAction)
	{
		
		KUiImageRef *pImage = &m_BloodImage[nCurDirNo];
		int nHeight = 0;
		nHeight = Npc[nNpcIdx].GetNpcPate();

		pImage->oPosition.nX = nScreenX;
		pImage->oPosition.nY = nScreenY - nHeight;
		pImage->oPosition.nZ = 0;

		g_pRepresent->DrawPrimitives(1, pImage, RU_T_IMAGE, bInMenu);
		IR_NextFrame(*pImage);
	}

	// 镶嵌激活人物特效(npc身前)
	nInlayPos = 0;
	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
	{
		if (m_cInlayStateSpr[i].m_nID)
		{
			if (m_cInlayStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_cInlayStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_cInlayStateSpr[i].m_nCopyNum == 1)
			{
				if (m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame < m_cInlayStateSpr[i].m_nBackStart || 
					m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame >= m_cInlayStateSpr[i].m_nBackEnd)
				{
					strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
					m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
					m_cInlayDrawFile[nInlayPos].nFrame = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
					m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
					m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;

					int nHeightOff = 0;
					if (m_bRideHorse)
						nHeightOff += 38;
					nHeightOff += (Npc[nNpcIdx].GetNpcPate()/2);

					m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
					nInlayPos++;
				}
			}
			else
			{
				nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_cInlayStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_cInlayStateSpr[i].m_nCopyNum);

					if (nStateFrameNo < m_cInlayStateSpr[i].m_nBackStart || nStateFrameNo >= m_cInlayStateSpr[i].m_nBackEnd)
					{
						strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
						m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
						m_cInlayDrawFile[nInlayPos].nFrame = nStateFrameNo;
						m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
						m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;

						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						nHeightOff += (Npc[nNpcIdx].GetNpcPate()/2);

						m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
						nInlayPos++;
					}
				}
			}
		}
	}
	
	g_pRepresent->DrawPrimitives(nInlayPos, m_cInlayDrawFile, RU_T_IMAGE, bInMenu);

	if ( m_nBodyFrontNum > 0 )
	{
		m_cBodyFile[0].oPosition.nX = nScreenX;
		m_cBodyFile[0].oPosition.nY = nScreenY;
		m_cBodyFile[1].oPosition.nX = nScreenX;
		m_cBodyFile[1].oPosition.nY = nScreenY;
		g_pRepresent->DrawPrimitives(m_nBodyFrontNum, m_cBodyFile, RU_T_IMAGE, bInMenu);
	}
	
	// 头顶任务图标
	if ( m_uQuestIcon )
	{	
		KUiImageRef& aImage = KNpcRes::QuestImage[m_uQuestIcon];
		aImage.oPosition.nX = nScreenX;
		aImage.oPosition.nY = nScreenY;
		aImage.oPosition.nZ = nScreenZ;
        g_pRepresent->CoordinateTransform( aImage.oPosition.nX, aImage.oPosition.nY, Npc[nNpcIdx].GetNpcPate() +Npc[nNpcIdx].GetNpcPatePeopleInfo() );
		g_pRepresent->DrawPrimitives(1, &aImage, RU_T_IMAGE, TRUE);
		IR_NextFrame(aImage);
	}

	// 特殊的只播放一遍的spr文件
	if (m_cSpecialSpr.m_szName[0])
	{
		strcpy(m_cOnlyFile.szImage, m_cSpecialSpr.m_szName);
		m_cOnlyFile.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
		m_cOnlyFile.uImage = m_cSpecialSpr.m_dwNameID;
		m_cOnlyFile.nFrame = m_cSpecialSpr.m_nCurFrame;
		m_cOnlyFile.oPosition.nX = nScreenX;
		m_cOnlyFile.oPosition.nY = nScreenY;
		int nHeightOff = 0;
		if (m_bRideHorse)
			nHeightOff += 38;
		m_cOnlyFile.oPosition.nZ = nScreenZ + nHeightOff;
		m_cOnlyFile.oPosition.nX = nScreenX;
		m_cOnlyFile.oPosition.nY = nScreenY;
		g_pRepresent->DrawPrimitives(1, &m_cOnlyFile, RU_T_IMAGE, bInMenu);
	}

	if ( m_nHeadNum > 0 )
	{
		m_cHeadFile[0].oPosition.nX = nScreenX;
		m_cHeadFile[0].oPosition.nY = nScreenY;
		m_cHeadFile[1].oPosition.nX = nScreenX;
		m_cHeadFile[1].oPosition.nY = nScreenY;
		g_pRepresent->DrawPrimitives(m_nHeadNum, m_cHeadFile, RU_T_IMAGE, bInMenu);
	}
	/*	
	//绘制Npc焦点特效
	if ( bDrawSelected )
	{	

		KUiImageRef *pSelectImage = NULL;
		int nHeight = 0;
		if ( m_bRideHorse )
		{
			pSelectImage = &m_RideSelectImage;
			nHeight = Npc[nNpcIdx].GetNpcPate();
		}
		else
		{
			pSelectImage = &m_SelectImage;
			nHeight = 38;
		}
		

		pSelectImage->oPosition.nX = nScreenX;
		pSelectImage->oPosition.nY = nScreenY - nHeight;
		pSelectImage->oPosition.nZ = 0;

		g_pRepresent->DrawPrimitives(1, pSelectImage, RU_T_IMAGE, bInMenu);
		IR_NextFrame(*pSelectImage);
	}
	else//*/
	{
		if ( nHover > 0 )
		{	
			KUiImageRef *pHoverImage = NULL;
			int nHeight = 0;
			if ( nSelectedType == 0 )
			{
				pHoverImage = &m_HoverImage;
				nHeight = 38;
			}
			else
			{
				pHoverImage = &m_RideHoverImage;
				nHeight = 38;
			}//*/
			

			pHoverImage->oPosition.nX = nScreenX;
			pHoverImage->oPosition.nY = nScreenY - nHeight;
			pHoverImage->oPosition.nZ = 0;

			g_pRepresent->DrawPrimitives(1, pHoverImage, RU_T_IMAGE, bInMenu);
			IR_NextFrame(*pHoverImage);
		}	
	}


	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
		m_cInlayStateSpr[i].m_SprContrul.GetNextFrame();

}

//---------------------------------------------------------------------------
//	功能：	绘制
//---------------------------------------------------------------------------
void	KNpcRes::DrawUI(int x, int y, int nNpcIdx, int nDir, int nAllFrame, int nCurFrame)
{
	BOOL bInMenu = TRUE;
	BOOL bDrawSelected = FALSE;
	int nSelectedType = 0;
	int		i, j, nGetFrame = 1, nGetDir = 1, nFirst, nPos;
	int		nCurFrameNo = 0, nCurDirNo = 0;
	int		nScreenX = x, nScreenY = y, nScreenZ = 0;
	int		nStateFrameNo;

	if (nDir < 0 || nAllFrame < 0 || nCurFrame < 0)
		return;

	if (!m_pcResNode)
		return;

	// 外部控制换帧
	if (nAllFrame > 0)
	{
		// 计算阴影当前帧
		nGetDir = this->m_cNpcShadow.m_nTotalDir;
		if (nGetDir <= 0)
			nGetDir = 1;
		nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
		if (nCurDirNo >= nGetDir)
			nCurDirNo -= nGetDir;
		nGetFrame = this->m_cNpcShadow.m_nTotalFrame;
		nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
		this->m_cNpcShadow.SetCurFrame(nCurFrameNo);

		// 各个部件
		nFirst = 0;
		// 找到动画的当前桢
		for (i = 0; i < MAX_PART; i++)
		{
			if (m_cNpcImage[i].CheckExist())
			{
				if (nFirst == 0)
				{
					nGetDir = m_cNpcImage[i].m_nTotalDir;
					if (nGetDir <= 0)
						nGetDir = 1;

					nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
					if (nCurDirNo >= nGetDir)
						nCurDirNo -= nGetDir;

					nGetFrame = m_cNpcImage[i].m_nTotalFrame;

					nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
					m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					nFirst = 1;
				}
				else
				{
					KImageParam	sImage;
					if (g_pRepresent->GetImageParam(m_cNpcImage[i].m_szName, &sImage, ISI_T_SPR))
					{
						m_cNpcImage[i].m_nTotalDir = sImage.nNumFramesGroup;
						m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;
						m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					}
				}
			}
		}
	}
	else
	{
		// 计算阴影当前帧
		if (m_cNpcShadow.SetCurDir64(nDir))
		{
			m_cNpcShadow.GetNextFrame();
		}

		// 各个部件
		for (i = 0; i < MAX_PART; i++)
		{
			if ( !m_cNpcImage[i].CheckExist() )
				continue;
			if ( m_cNpcImage[i].SetCurDir64(nDir) )
			{
				m_cNpcImage[i].GetNextFrame();

				nCurDirNo = m_cNpcImage[i].m_nCurDir;
				nCurFrameNo = m_cNpcImage[i].m_nCurFrame;
			}
			
		}
	}

	// 状态特效换帧
	for (i = 0; i < MAX_STATE_PART_NUM; i++)
		m_cStateSpr[i].m_SprContrul.GetNextFrame();

	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
		m_cInlayStateSpr[i].m_SprContrul.GetNextFrame();

	if ( m_cSpecialSpr.GetNextFrame(FALSE) )
	{
		if ( m_cSpecialSpr.CheckEnd() )
			m_cSpecialSpr.Release();
	}


	// 得到部件排列顺序
	if ( !m_pcResNode->GetSort(m_nAction, nCurDirNo, nCurFrameNo, m_nSortTable, MAX_PART) )
		return;

// ---------------------------------- 处理绘制列表 -------------------------------
	nPos = 0;
	// 阴影文件名
	if (!g_CoreHighQualityPaint)
	{
		if (m_cNpcShadow.m_szName[0] != 0)
		{
			strcpy_const(m_cDrawFile[nPos].szImage, NPC_SHADOW);
			m_cDrawFile[nPos].uImage = 0;
			m_cDrawFile[nPos].nFrame = 0;
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
		}
	}
	else
	{
		strcpy(m_cDrawFile[nPos].szImage, m_cNpcShadow.m_szName);
		m_cDrawFile[nPos].uImage = m_cNpcShadow.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cNpcShadow.m_nCurFrame;
		m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
	}
//	strcpy(m_cDrawFile[nPos].szImage, m_cNpcShadow.m_szName);
//	m_cDrawFile[nPos].uImage = m_cNpcShadow.m_dwNameID;
//	m_cDrawFile[nPos].nFrame = m_cNpcShadow.m_nCurFrame;
// 	m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
	m_cDrawFile[nPos].oPosition.nX = nScreenX;
	m_cDrawFile[nPos].oPosition.nY = nScreenY;
	m_cDrawFile[nPos].oPosition.nZ = 0;//nScreenZ;
	nPos++;

	// 脚底状态特效
	for ( i = 2; i < 4; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
			m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			m_cDrawFile[nPos].oPosition.nZ = 0;
			nPos++;
		}
	}

	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);

	// 装备灵石激活特效(npc背后)
	int nInlayPos = 0;
	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
	{
		if (m_cInlayStateSpr[i].m_nID)
		{
			if (m_cInlayStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_cInlayStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_cInlayStateSpr[i].m_nCopyNum == 1)
			{
				if (m_cInlayStateSpr[i].m_nBackStart <= m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame && 
					m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame < m_cInlayStateSpr[i].m_nBackEnd)
				{
					strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
					m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
					m_cInlayDrawFile[nInlayPos].nFrame = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
					m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
					m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;
					int nHeightOff = 0;
					if (m_nAction == cdo_sit) 
					{
						nHeightOff -= 40;
					}
					else
					{
						if (m_bRideHorse)
							nHeightOff += 38;
					}
					m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
					nInlayPos++;
				}
			}
			else
			{
				nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_cInlayStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_cInlayStateSpr[i].m_nCopyNum);

					if (m_cInlayStateSpr[i].m_nBackStart <= nStateFrameNo && nStateFrameNo < m_cInlayStateSpr[i].m_nBackEnd)
					{
						strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
						m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
						m_cInlayDrawFile[nInlayPos].nFrame = nStateFrameNo;
						m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
						m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;
						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
						nInlayPos++;
					}
				}
			}
		}
	}

	g_pRepresent->DrawPrimitives(nInlayPos, m_cInlayDrawFile, RU_T_IMAGE, bInMenu);


	nPos = 0;

	// 身上状态特效(npc背后)
	for (i = 4; i < 7; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			if (m_cStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_cStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_cStateSpr[i].m_nCopyNum == 1)
			{
				if (m_cStateSpr[i].m_nBackStart <= m_cStateSpr[i].m_SprContrul.m_nCurFrame && 
					m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackEnd)
				{
					strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
					m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
					m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
					m_cDrawFile[nPos].oPosition.nX = nScreenX;
					m_cDrawFile[nPos].oPosition.nY = nScreenY;
					int nHeightOff = 0;
					if (m_nAction == cdo_sit) 
					{
						nHeightOff -= 40;
					}
					else
					{
						if (m_bRideHorse)
							nHeightOff += 38;
					}
					m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
					nPos++;
				}
			}
			else
			{
				nStateFrameNo = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_cStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_cStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_cStateSpr[i].m_nCopyNum);

					if (m_cStateSpr[i].m_nBackStart <= nStateFrameNo && nStateFrameNo < m_cStateSpr[i].m_nBackEnd)
					{
						strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
						m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
						m_cDrawFile[nPos].nFrame = nStateFrameNo;
						m_cDrawFile[nPos].oPosition.nX = nScreenX;
						m_cDrawFile[nPos].oPosition.nY = nScreenY;
						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
						nPos++;
					}
				}
			}
		}
	}

	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);
	nPos = 0;

	// npc部件
	if ( m_nNpcKind != NPC_RES_NORMAL )
	{
		for (i = 0; i < MAX_PART; i++)
		{
			if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
			{
				if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
				{
					m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
					m_cDrawFile[nPos].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
				}
				else
				{
					m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_HUE_ADJUST;
					m_cDrawFile[nPos].Color.Color_dw = m_cNpcImage[m_nSortTable[i]].GetHue();
					m_cDrawFile[nPos].Color.Color_b.a = 0xFF;
				}

				strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);
				m_cDrawFile[nPos].uImage = m_cNpcImage[m_nSortTable[i]].m_dwNameID;
				m_cDrawFile[nPos].nFrame = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
				m_cDrawFile[nPos].oPosition.nX = nScreenX;
				m_cDrawFile[nPos].oPosition.nY = nScreenY;
				m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
				nPos++;
			}
		}
	}
	else
	{
		if (m_ulAdjustColorId > 0 && m_ulAdjustColorId <= g_ulAdjustColorCount)
		{
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST;
			m_cDrawFile[nPos].Color.Color_dw = g_pAdjustColorTab[m_ulAdjustColorId - 1];
		}
		else
		{
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_HUE_ADJUST;
			m_cDrawFile[nPos].Color.Color_dw = m_uHue;
			m_cDrawFile[nPos].Color.Color_b.a = 0xFF;
		}

		strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[NORMAL_NPC_PART_NO].m_szName);
		m_cDrawFile[nPos].uImage = m_cNpcImage[NORMAL_NPC_PART_NO].m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cNpcImage[NORMAL_NPC_PART_NO].m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		m_cDrawFile[nPos].oPosition.nZ = nScreenZ;
		nPos++;
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);

	// 镶嵌激活人物特效(npc身前)
	nInlayPos = 0;
	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
	{
		if (m_cInlayStateSpr[i].m_nID)
		{
			if (m_cInlayStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_cInlayStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_cInlayStateSpr[i].m_nCopyNum == 1)
			{
				if (m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame < m_cInlayStateSpr[i].m_nBackStart || 
					m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame >= m_cInlayStateSpr[i].m_nBackEnd)
				{
					strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
					m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
					m_cInlayDrawFile[nInlayPos].nFrame = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
					m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
					m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;
					int nHeightOff = 0;
					if (m_bRideHorse)
						nHeightOff += 38;
					m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
					nInlayPos++;
				}
			}
			else
			{
				nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_cInlayStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_cInlayStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_cInlayStateSpr[i].m_nCopyNum);

					if (nStateFrameNo < m_cInlayStateSpr[i].m_nBackStart || nStateFrameNo >= m_cInlayStateSpr[i].m_nBackEnd)
					{
						strcpy(m_cInlayDrawFile[nInlayPos].szImage, m_cInlayStateSpr[i].m_SprContrul.m_szName);
						m_cInlayDrawFile[nInlayPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_cInlayDrawFile[nInlayPos].uImage = m_cInlayStateSpr[i].m_SprContrul.m_dwNameID;
						m_cInlayDrawFile[nInlayPos].nFrame = nStateFrameNo;
						m_cInlayDrawFile[nInlayPos].oPosition.nX = nScreenX;
						m_cInlayDrawFile[nInlayPos].oPosition.nY = nScreenY;
						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						m_cInlayDrawFile[nInlayPos].oPosition.nZ = nScreenZ + nHeightOff;
						nInlayPos++;
					}
				}
			}
		}
	}
	
	g_pRepresent->DrawPrimitives(nInlayPos, m_cInlayDrawFile, RU_T_IMAGE, bInMenu);

	// 身上状态特效(npc身前)
	nPos = 0;	
	for (i = 4; i < 7; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			if (m_cStateSpr[i].m_nCopyNum < MIN_RES_STATE_COPY || m_cStateSpr[i].m_nCopyNum > MAX_RES_STATE_COPY)
				continue;
			if (m_cStateSpr[i].m_nCopyNum == 1)
			{
				if (m_cStateSpr[i].m_SprContrul.m_nCurFrame < m_cStateSpr[i].m_nBackStart || 
					m_cStateSpr[i].m_SprContrul.m_nCurFrame >= m_cStateSpr[i].m_nBackEnd)
				{
					strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
					m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
					m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
					m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
					m_cDrawFile[nPos].oPosition.nX = nScreenX;
					m_cDrawFile[nPos].oPosition.nY = nScreenY;
					int nHeightOff = 0;
					if (m_bRideHorse)
						nHeightOff += 38;
					m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
					nPos++;
				}
			}
			else
			{
				nStateFrameNo = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
				for (j = 0; j < m_cStateSpr[i].m_nCopyNum; j++)
				{
					nStateFrameNo = m_cStateSpr[i].m_SprContrul.GetPartFrame(nStateFrameNo, m_cStateSpr[i].m_nCopyNum);

					if (nStateFrameNo < m_cStateSpr[i].m_nBackStart || nStateFrameNo >= m_cStateSpr[i].m_nBackEnd)
					{
						strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
						m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
						m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
						m_cDrawFile[nPos].nFrame = nStateFrameNo;
						m_cDrawFile[nPos].oPosition.nX = nScreenX;
						m_cDrawFile[nPos].oPosition.nY = nScreenY;
						int nHeightOff = 0;
						if (m_bRideHorse)
							nHeightOff += 38;
						m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
						nPos++;
					}
				}
			}
		}
	}
	// 特殊的只播放一遍的spr文件
	if (m_cSpecialSpr.m_szName[0])
	{
		strcpy(m_cDrawFile[nPos].szImage, m_cSpecialSpr.m_szName);
		m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
		m_cDrawFile[nPos].uImage = m_cSpecialSpr.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cSpecialSpr.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		int nHeightOff = 0;
		if (m_bRideHorse)
			nHeightOff += 38;
		m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
		nPos++;
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);

	nPos = 0;
	// 头顶状态特效
	for ( i = 0; i < 2; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cStateSpr[i].m_SprContrul.m_szName);
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;			
			m_cDrawFile[nPos].uImage = m_cStateSpr[i].m_SprContrul.m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cStateSpr[i].m_SprContrul.m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = nScreenX;
			m_cDrawFile[nPos].oPosition.nY = nScreenY;
			int nHeightOff = 0;
			if (m_bRideHorse)
				nHeightOff += 38;
			m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOff;
			nPos++;
		}
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, bInMenu);

}
//<------- End [Ray]

void	KNpcRes::GetShadowName(char *lpszShadow, unsigned int nNo)
{
	KNpcResNode::GetShadowName(lpszShadow, nNo);
}

KNpcRes::~KNpcRes()
{
}

//---------------------------------------------------------------------------
//	功能：	设定部件
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetPart(unsigned int uPart, int nType, int nPal, bool bUi)
{
	if (((nType < 0 || uPart > BODY_PART_MAX) || ( !m_pcResNode ))||(m_nNpcKind == NPC_RES_NORMAL) && (nType != 0))
		return FALSE;

	if (m_nPart[uPart] == nType && m_nPartPal[ uPart ] == nPal)
		return TRUE;
		
	m_nPart[uPart] = nType;
	m_nPartPal[ uPart ] = nPal;
	
	if ( bUi )
	{
		UpDateUiImage();
	}
	else
	{
		UpDateImage();
	}
	

	return TRUE;
}


//---------------------------------------------------------------------------
//	功能：	设定动作类型
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetAction(int nDoing)
{
	if ((!m_pcResNode)||(nDoing < 0))
		return FALSE;
	if (m_nDoing == nDoing)
		return TRUE;

	m_nDoing = nDoing;

	UpDateImage();
	return TRUE;
}

//---------------------------------------------------------------------------
//	功能：	设定是否骑马
//---------------------------------------------------------------------------
BOOL	KNpcRes::SetRideHorse(BOOL bRideHorse, bool bUi)
{
	if ((!m_pcResNode)||(m_nNpcKind == NPC_RES_NORMAL ))
		return FALSE;

	if (m_bRideHorse == bRideHorse)
		return TRUE;
	
	m_bRideHorse = bRideHorse;

	if ( bUi )
	{
		UpDateUiImage();
	}
	else
	{
		UpDateImage();
	}
	return TRUE;
}

int KNpcRes::GetCurTotalFrame( int nDoing )
{
	if ((!m_pcResNode))
		return 0;

	int nAction = m_pcResNode->GetActNo(m_nDoing, m_nPart[BODY_PART_WEAPON], m_nPart[BODY_PART_HORSE], m_bRideHorse);

	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY;

	if ( m_pcResNode->m_cShadowInfo.GetFile(nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		//m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}
	
	if (m_nNpcKind == NPC_RES_NORMAL )
	{
		if ( m_pcResNode->CheckPartExist(NORMAL_NPC_PART_NO) )
		{
			m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, nAction, 0, "", szBuffer, sizeof(szBuffer));
			if (szBuffer[0] != 0)
			{
				return m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, nAction, 0, 0);
			}
			else
			{
				// 取默认动作
				m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, 0, 0, "", szBuffer, sizeof(szBuffer));
				if (szBuffer[0] != 0)
				{
					return m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, nAction, 0, 0);					
				}
			}
		}
		else
		{
			return 1;
		}
		
	}
	else
	{
		for (int part = 0; part < BODY_PART_MAX; ++part)
		{
			int nKey = m_nPart[part];
			for (int sect = 0; sect < MAX_BODY_PART_SECT; ++sect)
			{
				int nIndex = part * MAX_BODY_PART_SECT + sect;
				if ( m_pcResNode->CheckPartExist(nIndex) )
				{
					return m_pcResNode->GetInterval(nIndex, nAction, nKey, 0);
				}
				else
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

void KNpcRes::GetSpr(std::string& spr )
{
	if ( m_pcResNode == NULL )
	{
		return;
	}

	m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nPart[BODY_PART_WEAPON], m_nPart[BODY_PART_HORSE], m_bRideHorse);
	m_bNeedSort = true;

	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY;

	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		//m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}
	
	if (m_nNpcKind == NPC_RES_NORMAL )
	{
		if ( m_pcResNode->CheckPartExist(NORMAL_NPC_PART_NO) )
		{
			m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, m_nAction, 0, "", szBuffer, sizeof(szBuffer));
			if (szBuffer[0] != 0)
			{
				m_cNpcImage[NORMAL_NPC_PART_NO].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, m_nAction, 0, 16), m_pcResNode->GetTotalDirs(NORMAL_NPC_PART_NO, m_nAction, 0, 16), m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, m_nAction, 0, 0));
			}
			else
			{
				// 取默认动作
				m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, 0, 0, "", szBuffer, sizeof(szBuffer));
				if (szBuffer[0] != 0)
				{
					m_cNpcImage[NORMAL_NPC_PART_NO].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, 0, 0, 16), m_pcResNode->GetTotalDirs(NORMAL_NPC_PART_NO, 0, 0, 16), m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, m_nAction, 0, 0));					
				}
			}
		}
		else
		{
			m_cNpcImage[NORMAL_NPC_PART_NO].Release();
		}
		
	}
	spr = szBuffer;
}


void KNpcRes::UpDateImage(void)
{
	if ( m_pcResNode == NULL )
	{
		return;
	}

	m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nPart[BODY_PART_WEAPON], m_nPart[BODY_PART_HORSE], m_bRideHorse);
	m_bNeedSort = true;

	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY;

	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		//m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}
	
	if (m_nNpcKind == NPC_RES_NORMAL )
	{
		if ( m_pcResNode->CheckPartExist(NORMAL_NPC_PART_NO) )
		{
			m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, m_nAction, 0, "", szBuffer, sizeof(szBuffer));
			if (szBuffer[0] != 0)
			{
				m_cNpcImage[NORMAL_NPC_PART_NO].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, m_nAction, 0, 16), m_pcResNode->GetTotalDirs(NORMAL_NPC_PART_NO, m_nAction, 0, 16), m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, m_nAction, 0, 0));
			}
			else
			{
				// 取默认动作
				m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, 0, 0, "", szBuffer, sizeof(szBuffer));
				if (szBuffer[0] != 0)
				{
					m_cNpcImage[NORMAL_NPC_PART_NO].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, 0, 0, 16), m_pcResNode->GetTotalDirs(NORMAL_NPC_PART_NO, 0, 0, 16), m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, m_nAction, 0, 0));					
				}
			}
		}
		else
		{
			m_cNpcImage[NORMAL_NPC_PART_NO].Release();
		}
		
	}
	else
	{
		for (int part = 0; part < BODY_PART_MAX; ++part)
		{
			int nKey = m_nPart[part];
			for (int sect = 0; sect < MAX_BODY_PART_SECT; ++sect)
			{
				int nIndex = part * MAX_BODY_PART_SECT + sect;
				if ( m_pcResNode->CheckPartExist(nIndex) )
				{
					m_pcResNode->GetFileName(nIndex, m_nAction, nKey, "", szBuffer, sizeof(szBuffer));
					int nFrames = m_pcResNode->GetTotalFrames(nIndex, m_nAction, nKey, 16);
					int nDirs = m_pcResNode->GetTotalDirs(nIndex, m_nAction, nKey, 16);
					int nInterval = m_pcResNode->GetInterval(nIndex, m_nAction, nKey, 0);
					m_cNpcImage[nIndex].SetSprFile(szBuffer, nFrames, nDirs, nInterval);
					m_cNpcImage[nIndex].SetHue( g_ItemPalToHue.GetHue( part, m_uRoleType, m_nPart[ part ], m_nPartPal[ part ] ) );
					m_cNpcChanged[nIndex] = true;
				}
				else
				{
					m_cNpcImage[nIndex].Release();
				}
			}
		}
	}
}

void KNpcRes::UpDateUiImage(void)
{
	
	m_nAction = m_pcResNode->GetActNo(m_nDoing, m_nPart[BODY_PART_WEAPON], m_nPart[BODY_PART_HORSE], m_bRideHorse);

	char	szBuffer[80];
	int		nFrame, nDir, nInterval, nCgX, nCgY;

	if ( m_pcResNode->m_cShadowInfo.GetFile(m_nAction, &nFrame, &nDir, &nInterval, &nCgX, &nCgY, szBuffer) )
	{
		//m_cNpcShadow.SetSprFile(szBuffer, nFrame, nDir, nInterval);
		m_cNpcShadow.SetCenterPos(nCgX, nCgY);
	}
	else
	{
		m_cNpcShadow.Release();
	}
	
	if (m_nNpcKind == NPC_RES_NORMAL )
	{
		if ( m_pcResNode->CheckPartExist(NORMAL_NPC_PART_NO) )
		{
			m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, m_nAction, 0, "", szBuffer, sizeof(szBuffer));
			if (szBuffer[0] != 0)
			{
				m_cNpcImage[NORMAL_NPC_PART_NO].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, m_nAction, 0, 16), m_pcResNode->GetTotalDirs(NORMAL_NPC_PART_NO, m_nAction, 0, 16), m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, m_nAction, 0, 0));
			}
			else
			{
				// 取默认动作
				m_pcResNode->GetFileName(NORMAL_NPC_PART_NO, 0, 0, "", szBuffer, sizeof(szBuffer));
				if (szBuffer[0] != 0)
				{
					m_cNpcImage[NORMAL_NPC_PART_NO].SetSprFile(szBuffer, m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, 0, 0, 16), m_pcResNode->GetTotalDirs(NORMAL_NPC_PART_NO, 0, 0, 16), m_pcResNode->GetInterval(NORMAL_NPC_PART_NO, m_nAction, 0, 0));					
				}
			}
		}
		else
		{
			m_cNpcImage[NORMAL_NPC_PART_NO].Release();
		}
		
	}
	else
	{
		for (int part = 0; part < BODY_PART_MAX; ++part)
		{
			int nKey = m_nPart[part];
			for (int sect = 0; sect < MAX_BODY_PART_SECT; ++sect)
			{
				int nIndex = part * MAX_BODY_PART_SECT + sect;
				if ( m_pcResNode->CheckPartExist(nIndex) )
				{
					m_pcResNode->GetFileName(nIndex, m_nAction, nKey, "", szBuffer, sizeof(szBuffer));
					int nFrames = m_pcResNode->GetTotalFrames(nIndex, m_nAction, nKey, 16);
					int nDirs = m_pcResNode->GetTotalDirs(nIndex, m_nAction, nKey, 16);
					int nInterval = m_pcResNode->GetInterval(nIndex, m_nAction, nKey, 0);
					string str = (char*)szBuffer;
					int nPos = str.find((const char *)"npcres");
					if ( nPos != string::npos )
					{
						str.replace(nPos, strlen("npcres"), "uinpcres", strlen("uinpcres") );
					}
					
					m_cNpcImage[nIndex].SetSprFile((char*)str.c_str(), nFrames, nDirs, nInterval);
					m_cNpcImage[nIndex].SetHue( g_ItemPalToHue.GetHue( part, m_uRoleType, m_nPart[ part ], m_nPartPal[ part ] ) );
				}
				else
				{
					m_cNpcImage[nIndex].Release();
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
//	功能：	设定 npc 位置
//---------------------------------------------------------------------------
void	KNpcRes::SetPos(int nNpcIdx, int x, int y, int z, BOOL bFocus, BOOL bMenu)
{
	if (m_nXposNew == 0 && m_nYposNew == 0 && m_nZposNew == 0)
	{
		m_nXposOld = 0;
		m_nYposOld = 0;
		m_nZposOld = 0;
	}
	else
	{
		m_nXposOld = m_nXposNew + ((x - m_nXposNew)/2);
		m_nYposOld = m_nYposNew + ((y - m_nYposNew)/2);
		m_nZposOld = m_nZposNew + ((z - m_nZposNew)/2);
	}
	
	m_nXposNew = x;
	m_nYposNew = y;
	m_nZposNew = z;//*/

}

void	KNpcRes::UpDateScene(int nNpcIdx, BOOL bFocus)
{
	
	m_nXpos = m_nXposOld;
	m_nYpos = m_nYposOld;
	m_nZpos = m_nZposOld;
	m_nXposOld = 0;
	m_nYposOld = 0;
	m_nZposOld = 0;
	if (m_nXpos == 0)
		m_nXpos = m_nXposNew;
	if (m_nYpos == 0)
		m_nYpos = m_nYposNew;
	if (m_nZpos == 0)
		m_nZpos = m_nZposNew;


	if (m_nXpos != 0 && m_nYpos != 0)
	{
		m_SceneID_NPCIdx = nNpcIdx; 
		g_ScenePlace.MoveObject(CGOG_NPC, nNpcIdx, m_nXpos, m_nYpos, m_nZpos, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_INFRONTOF_ALL | IPOT_RL_LIGHT_PROP);
	}
}


void	KNpcRes::ClearAllState( void )
{
	for (int i = 0; i < MAX_STATE_PART_NUM; ++i)
	{
		m_cStateSpr[i].Release();
	}
}

void	KNpcRes::AddInlayState( int nIdx, int nID )
{
	char szBuffer[COMMON_CLIENT_MSG_LEN_128];
	int nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal, nCopyNum;
 	g_NpcResList.m_cStateTable.GetInfo(nID, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal, &nCopyNum);
	if ( !szBuffer[0] )
		return;

	if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)
		return;


	if ( nIdx >= 0 && 
		nIdx < MAX_INLAY_STATE_PART_NUM && 
		m_cInlayStateSpr[nIdx].m_nID != nID )
	{
		// 添加新的
		m_cInlayStateSpr[nIdx].Release();
		m_cInlayStateSpr[nIdx].m_nID		= nID;
		m_cInlayStateSpr[nIdx].m_nType		= nType;
		m_cInlayStateSpr[nIdx].m_nPlayType	= nPlayType;
		m_cInlayStateSpr[nIdx].m_nBackStart	= nBackStart;
		m_cInlayStateSpr[nIdx].m_nBackEnd	= nBackEnd;
		m_cInlayStateSpr[nIdx].m_nCopyNum	= nCopyNum;
		m_cInlayStateSpr[nIdx].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
	}
}

void	KNpcRes::ClearInlayState( int nIdx )
{
	if ( nIdx >= 0 && nIdx < MAX_INLAY_STATE_PART_NUM )
	{
		m_cInlayStateSpr[nIdx].Release();
	}
}

void	KNpcRes::ClearAllInlayState( void )
{
	for ( int nIdx = 0; nIdx < MAX_INLAY_STATE_PART_NUM; ++nIdx )
	{
		m_cInlayStateSpr[nIdx].Release();
	}
}

//---------------------------------------------------------------------------
//	功能：	设定状态特效
//---------------------------------------------------------------------------
void	KNpcRes::AddState( unsigned long ulState, int nID )
{
	char szBuffer[COMMON_CLIENT_MSG_LEN_128];
	int nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal, nCopyNum;
 	g_NpcResList.m_cStateTable.GetInfo(ulState, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal, &nCopyNum);
	if ( !szBuffer[0] )
		return;

	if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)
		return;


	for (int i = 0; i < MAX_STATE_PART_NUM; ++i)
	{
		if (m_cStateSpr[i].m_nID == 0)
		{ 
			// 添加新的
			m_cStateSpr[i].Release();
			m_cStateSpr[i].m_nID		= ulState;
			m_cStateSpr[i].m_nType		= nType;
			m_cStateSpr[i].m_nPlayType	= nPlayType;
			m_cStateSpr[i].m_nBackStart	= nBackStart;
			m_cStateSpr[i].m_nBackEnd	= nBackEnd;
			m_cStateSpr[i].m_nCopyNum	= nCopyNum;
			m_cStateSpr[i].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
			m_cStateID[i] = nID;
			return;
		}
	}
}

//---------------------------------------------------------------------------
//	功能：	设定状态特效
//---------------------------------------------------------------------------
void	KNpcRes::AddHState( unsigned long ulState, int nID )
{
	char szBuffer[COMMON_CLIENT_MSG_LEN_128];
	int nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal, nCopyNum;
 	g_NpcResList.m_cStateTable.GetInfo(ulState, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal, &nCopyNum);
	if ( !szBuffer[0] )
		return;

	if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)
		return;


	for (int i = 0; i < 1; ++i)
	{
		if (m_cStateSpr[i].m_nID == 0)
		{ 
			// 添加新的
			m_cStateSpr[i].Release();
			m_cStateSpr[i].m_nID		= ulState;
			m_cStateSpr[i].m_nType		= nType;
			m_cStateSpr[i].m_nPlayType	= nPlayType;
			m_cStateSpr[i].m_nBackStart	= nBackStart;
			m_cStateSpr[i].m_nBackEnd	= nBackEnd;
			m_cStateSpr[i].m_nCopyNum	= nCopyNum;
			m_cStateSpr[i].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
			m_cStateID[i] = nID;
			return;
		}
	}
}

//---------------------------------------------------------------------------
//	功能：	设定状态特效
//---------------------------------------------------------------------------
void	KNpcRes::AddFState( unsigned long ulState, int nID )
{
	char szBuffer[COMMON_CLIENT_MSG_LEN_128];
	int nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal, nCopyNum;
 	g_NpcResList.m_cStateTable.GetInfo(ulState, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal, &nCopyNum);
	if ( !szBuffer[0] )
		return;

	if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)
		return;


	for (int i = 2; i < 3; ++i)
	{
		if (m_cStateSpr[i].m_nID == 0)
		{ 
			// 添加新的
			m_cStateSpr[i].Release();
			m_cStateSpr[i].m_nID		= ulState;
			m_cStateSpr[i].m_nType		= nType;
			m_cStateSpr[i].m_nPlayType	= nPlayType;
			m_cStateSpr[i].m_nBackStart	= nBackStart;
			m_cStateSpr[i].m_nBackEnd	= nBackEnd;
			m_cStateSpr[i].m_nCopyNum	= nCopyNum;
			m_cStateSpr[i].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
			m_cStateID[i] = nID;
			return;
		}
	}
}

//---------------------------------------------------------------------------
//	功能：	设定状态特效
//---------------------------------------------------------------------------
void	KNpcRes::AddBState( unsigned long ulState, int nID )
{
	char szBuffer[COMMON_CLIENT_MSG_LEN_128];
	int nType, nPlayType, nBackStart, nBackEnd, nTotalFrame, nTotalDir, nInterVal, nCopyNum;
 	g_NpcResList.m_cStateTable.GetInfo(ulState, szBuffer, &nType, &nPlayType, &nBackStart, &nBackEnd, &nTotalFrame, &nTotalDir, &nInterVal, &nCopyNum);
	if ( !szBuffer[0] )
		return;

	if (nType < 0 || nType >= STATE_MAGIC_TYPE_NUM)
		return;


	for (int i = 4; i < 5; ++i)
	{
		if (m_cStateSpr[i].m_nID == 0)
		{ 
			// 添加新的
			m_cStateSpr[i].Release();
			m_cStateSpr[i].m_nID		= ulState;
			m_cStateSpr[i].m_nType		= nType;
			m_cStateSpr[i].m_nPlayType	= nPlayType;
			m_cStateSpr[i].m_nBackStart	= nBackStart;
			m_cStateSpr[i].m_nBackEnd	= nBackEnd;
			m_cStateSpr[i].m_nCopyNum	= nCopyNum;
			m_cStateSpr[i].m_SprContrul.SetSprFile(szBuffer, nTotalFrame, nTotalDir, nInterVal);
			m_cStateID[i] = nID;
			return;
		}
	}
}

void	KNpcRes::ClearState( int nID )
{
	int i = 0;
	for ( i = 0; i < MAX_STATE_PART_NUM; ++i)
	{
		if (m_cStateID[i] == nID)
		{ 
			m_cStateSpr[i].Release();
		}
	}	
}

//---------------------------------------------------------------------------
//	功能：	设定特殊的只播放一遍的随身spr文件
//---------------------------------------------------------------------------
void	KNpcRes::SetSpecialSpr(char *lpszSprName)
{
	KImageParam	sImage;
	if (g_pRepresent->GetImageParam(lpszSprName, &sImage, ISI_T_SPR))
	{
		if (sImage.nInterval <= 0)
			sImage.nInterval = 1;
		if (sImage.nInterval > 1000)
			sImage.nInterval = 1000;
		if (sImage.nNumFramesGroup <= 0)
			sImage.nNumFramesGroup = 1;
		if (sImage.nNumFrames < sImage.nNumFramesGroup)
			sImage.nNumFrames = sImage.nNumFramesGroup;
		m_cSpecialSpr.SetSprFile(lpszSprName, sImage.nNumFrames, sImage.nNumFramesGroup, (sImage.nNumFrames / sImage.nNumFramesGroup) * sImage.nInterval / 50);
	}
}

//---------------------------------------------------------------------------
//	功能：set menu state spr
//---------------------------------------------------------------------------
void	KNpcRes::SetMenuStateSpr(int nMenuState)
{
}

//---------------------------------------------------------------------------
//	功能：	残影打开关闭
//	参数：	bBlur	if == TRUE  打开  if == FLASE  关闭
//---------------------------------------------------------------------------
void	KNpcRes::SetBlur(BOOL bBlur)
{
	if (m_nBlurState == bBlur)
		return;

	m_nBlurState = bBlur;
	if (bBlur)
	{
		m_cNpcBlur.AddObj();
	}
	else
	{
		m_cNpcBlur.RemoveObj();
	}
}


//---------------------------------------------------------------------------
//	功能：	获得当前动作的音效文件名
//---------------------------------------------------------------------------
void	KNpcRes::GetSoundName()
{
	if (m_pcResNode)
		m_pcResNode->GetActionSoundName(this->m_nAction, this->m_szSoundName);
}

//---------------------------------------------------------------------------
//	功能：	播放当前动作的音效
//---------------------------------------------------------------------------
void	KNpcRes::PlaySound(int nX, int nY)
{
	if (!m_szSoundName[0])
		return;

	int		nCenterX = 0, nCenterY = 0, nCenterZ = 0;

	// 获得屏幕中心点的地图坐标 not end
	g_ScenePlace.GetFocusPosition(nCenterX, nCenterY, nCenterZ);

	m_pSoundNode = (KCacheNode*) g_SoundCache.GetNode(m_szSoundName, (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		//int nVol = -(abs(nX - nCenterX) + abs(nY - nCenterY));
		float dist = sqrt((nX-nCenterX)*(nX-nCenterX)+(nY-nCenterY)*(nY-nCenterY));
		m_pWave->Play((nX-nCenterX)*10, Option.GetSndVolume(dist), 0);
	}
}

void	KNpcRes::StopSound()
{
	m_pSoundNode = (KCacheNode*)g_SoundCache.GetNode(m_szSoundName, (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		m_pWave->Stop();
	}
}
//---------------------------------------------------------------------------
//	功能：设定头顶状态
//---------------------------------------------------------------------------
void	KNpcRes::SetMenuState(int nState, char *lpszSentence, int nSentenceLength)
{
}

//---------------------------------------------------------------------------
//	功能：获得头顶状态
//---------------------------------------------------------------------------
int		KNpcRes::GetMenuState()
{
	if (m_nSleepState)
		return m_nSleepState;
	return this->m_nMenuState;
}

int		KNpcRes::GetMenuSentence(char *szBuf)
{
	if ( szBuf )
	{
		int nLen = strlen(m_szSentence);
		memcpy(szBuf, m_szSentence, nLen);
		szBuf[nLen] = 0;
		return nLen;
	}
	return 0;
}

//---------------------------------------------------------------------------
//	功能：设定睡眠状态
//---------------------------------------------------------------------------
void	KNpcRes::SetSleepState(BOOL bFlag)
{
}

//---------------------------------------------------------------------------
//	功能：获得睡眠状态
//---------------------------------------------------------------------------
BOOL	KNpcRes::GetSleepState()
{
	return (m_nSleepState ? 1 : 0);
}

//---------------------------------------------------------------------------
//	功能：绘制npc的边框(3D模式中改为加亮)
//---------------------------------------------------------------------------
void	KNpcRes::DrawBorder(int nDir, int nAllFrame, int nCurFrame)
{
	if (!m_pcResNode)
		return;

	int		i, nPos = 0;
//////////////////////////////////////////////////////////////////////////
// Added By Rocker 2004.03.22 最好这里换帧
	int		nGetFrame = 1, nGetDir = 1, nFirst;
	int		nCurFrameNo = 0, nCurDirNo = 0;
	int		nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = m_nZpos;

	if (nDir < 0 || nAllFrame < 0 || nCurFrame < 0)
		return;

	if (!m_pcResNode)
		return;

	// 外部控制换帧
	if (nAllFrame > 0)
	{
		// 计算阴影当前帧
		nGetDir = this->m_cNpcShadow.m_nTotalDir;
		if (nGetDir <= 0)
			nGetDir = 1;
		nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
		if (nCurDirNo >= nGetDir)
			nCurDirNo -= nGetDir;
		nGetFrame = this->m_cNpcShadow.m_nTotalFrame;
		nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
		this->m_cNpcShadow.SetCurFrame(nCurFrameNo);

		// 各个部件
		nFirst = 0;
		// 找到动画的当前桢
		for (i = 0; i < MAX_PART; i++)
		{
			if (m_cNpcImage[i].CheckExist())
			{
				if (nFirst == 0)
				{
					nGetDir = m_cNpcImage[i].m_nTotalDir;
					if (nGetDir <= 0)
						nGetDir = 1;

					nCurDirNo = (nDir + (32 / nGetDir)) / (64 / nGetDir);
					if (nCurDirNo >= nGetDir)
						nCurDirNo -= nGetDir;

					nGetFrame = m_cNpcImage[i].m_nTotalFrame;

					nCurFrameNo = nCurDirNo * (nGetFrame / nGetDir) + (nGetFrame / nGetDir) * nCurFrame / nAllFrame;
					m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					nFirst = 1;
				}
				else
				{
					KImageParam	sImage;
					if (g_pRepresent->GetImageParam(m_cNpcImage[i].m_szName, &sImage, ISI_T_SPR))
					{
						m_cNpcImage[i].m_nTotalDir = sImage.nNumFramesGroup;
						m_cNpcImage[i].m_nTotalFrame = sImage.nNumFrames;
						m_cNpcImage[i].SetCurFrame(nCurFrameNo);
					}
				}
			}
		}
	}
	else
	{
		// 计算阴影当前帧
		if (m_cNpcShadow.SetCurDir64(nDir))
		{
			m_cNpcShadow.GetNextFrame();
		}

		// 各个部件
		for (i = 0; i < MAX_PART; i++)
		{
			if ( !m_cNpcImage[i].CheckExist() )
				continue;
			if ( m_cNpcImage[i].SetCurDir64(nDir) )
			{
				m_cNpcImage[i].GetNextFrame();
				nCurDirNo = m_cNpcImage[i].m_nCurDir;
				nCurFrameNo = m_cNpcImage[i].m_nCurFrame;
			}
			
		}
	}

	// 状态特效换帧
	for (i = 0; i < MAX_STATE_PART_NUM; i++)
		m_cStateSpr[i].m_SprContrul.GetNextFrame();

	for (i = 0; i < MAX_INLAY_STATE_PART_NUM; i++)
		m_cInlayStateSpr[i].m_SprContrul.GetNextFrame();

	if ( m_cSpecialSpr.GetNextFrame(FALSE) )
	{
		if ( m_cSpecialSpr.CheckEnd() )
			m_cSpecialSpr.Release();
	}

//////////////////////////////////////////////////////////////////////////
	nPos = 0;
	for (i = 0; i < MAX_PART; i++)
	{
		if (m_nSortTable[i] >= 0 && m_nSortTable[i] < MAX_PART)
		{
			strcpy(m_cDrawFile[nPos].szImage, m_cNpcImage[m_nSortTable[i]].m_szName);
			m_cDrawFile[nPos].uImage = m_cNpcImage[m_nSortTable[i]].m_dwNameID;
			m_cDrawFile[nPos].nFrame = m_cNpcImage[m_nSortTable[i]].m_nCurFrame;
			m_cDrawFile[nPos].oPosition.nX = m_nXpos;
			m_cDrawFile[nPos].oPosition.nY = m_nYpos;
			m_cDrawFile[nPos].oPosition.nZ = m_nZpos;
			m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_BORDER;
			m_cDrawFile[nPos].Color.Color_b.a = 64;
			nPos++;
		}
	}
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, FALSE);
	for (i = 0; i < nPos; i++)
	{
		m_cDrawFile[i].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		m_cDrawFile[i].Color.Color_b.a = 0xFF;

	}
	nPos = 0;
}

//在指定高度绘制头顶状态
int	KNpcRes::DrawMenuState(int nHeightOffset)
{
	int		nScreenX = m_nXpos, nScreenY = m_nYpos, nScreenZ = 0;

	if (!m_pcResNode)
		return nHeightOffset;

	// 头顶状态特效
	int i;
	for ( i = 0; i < 2; i++)
	{
		if (m_cStateSpr[i].m_nID)
		{
			return nHeightOffset;	//有头顶特效时不绘制交易等状态
		}
	}

	int nPos = 0;
	nHeightOffset += 26;
	// MenuState
	if (m_cMenuStateSpr.m_szName[0])
	{
		m_cMenuStateSpr.GetNextFrame();

		strcpy(m_cDrawFile[nPos].szImage, m_cMenuStateSpr.m_szName);
		
		m_cDrawFile[nPos].uImage = m_cMenuStateSpr.m_dwNameID;
		m_cDrawFile[nPos].nFrame = m_cMenuStateSpr.m_nCurFrame;
		m_cDrawFile[nPos].oPosition.nX = nScreenX;
		m_cDrawFile[nPos].oPosition.nY = nScreenY;
		m_cDrawFile[nPos].oPosition.nZ = nScreenZ + nHeightOffset;
		m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		nPos++;
	}
	
	g_pRepresent->DrawPrimitives(nPos, m_cDrawFile, RU_T_IMAGE, false);


	return nHeightOffset;
}

//---------------------------------------------------------------------------
//	功能：动画帧数转换成逻辑方向(0 - 63)
//---------------------------------------------------------------------------
int		KNpcRes::GetNormalNpcStandDir(int nFrame)
{
	if (!m_pcResNode)
		return 0;

	int nTotalFrames = m_pcResNode->GetTotalFrames(NORMAL_NPC_PART_NO, cdo_stand, m_nPart[BODY_PART_HELM], 16);
	if (nTotalFrames <= 0)
		return 0;

	nFrame %= nTotalFrames;

	return (MAX_NPC_DIR * nFrame) / nTotalFrames;
}

//---------------------------------------------------------------------------
//	功能：	设置套装特效	lixuewu
//---------------------------------------------------------------------------
BOOL KNpcRes::SetGreenItemEffect(unsigned int uGreenID, int nSetType, int nRoleType)
{
	return FALSE;
}

//---------------------------------------------------------------------------
//	功能：	构造函数
//---------------------------------------------------------------------------
KStateSpr::KStateSpr()
{
	Release();
}

//---------------------------------------------------------------------------
//	功能：	清空，初始化
//---------------------------------------------------------------------------
void	KStateSpr::Release()
{
	m_nID			= 0;
	m_nType			= 0;
	m_nPlayType		= 0;
	m_nBackStart	= 0;
	m_nBackEnd		= 0;
	m_nCopyNum		= 0;
	m_SprContrul.Release();
}

//---------------------------------------------------------------------------
//	功能：	构造函数
//---------------------------------------------------------------------------
KNpcBlur::KNpcBlur()
{
	m_nActive		= 0;
	m_nCurNo		= 0;
	m_dwInterval	= 3;
	m_dwTimer		= 0;
}

//---------------------------------------------------------------------------
//	功能：	析构函数
//---------------------------------------------------------------------------
KNpcBlur::~KNpcBlur()
{
    //Remove();
}


//---------------------------------------------------------------------------
//	功能：	当前编号指针指向下一个(总共7个，指针循环)
//---------------------------------------------------------------------------
void	KNpcBlur::SetNextNo()
{
	m_nCurNo++;
	if (m_nCurNo >= MAX_BLUR_FRAME)
		m_nCurNo = 0;
}

//---------------------------------------------------------------------------
//	功能：	设定当前残影帧地图坐标
//---------------------------------------------------------------------------
void	KNpcBlur::SetMapPos(int x, int y, int z, int nNpcIdx)
{
	m_nMapXpos[m_nCurNo] = x;
	m_nMapYpos[m_nCurNo] = y;
	m_nMapZpos[m_nCurNo] = z;
    m_SceneIDNpcIdx[m_nCurNo] = nNpcIdx;
	g_ScenePlace.MoveObject(CGOG_NPC_BLUR_DETAIL(m_nCurNo), nNpcIdx, x, y, z, m_SceneID[m_nCurNo]);
}

//---------------------------------------------------------------------------
//	功能：	改变alpha度
//---------------------------------------------------------------------------
void	KNpcBlur::ChangeAlpha()
{
	if (m_nActive == 0)
		return;

	int		i, j;
	int		nScreenX, nScreenY, nScreenZ;
	for (i = 0; i < MAX_BLUR_FRAME; i++)
	{
		nScreenX = m_nMapXpos[i];
		nScreenY = m_nMapYpos[i];
		nScreenZ = m_nMapZpos[i];
//		SubWorld[0].Mps2Screen(&nScreenX, &nScreenY);
		for (j = 0; j < MAX_PART; j++)
		{
			if (m_Blur[i][j].Color.Color_b.a)
			{
				m_Blur[i][j].oPosition.nX = nScreenX;
				m_Blur[i][j].oPosition.nY = nScreenY;
				m_Blur[i][j].oPosition.nZ = nScreenZ;
			}
		}
	}

	m_dwTimer++;
	if (m_dwTimer < m_dwInterval)
		return;
	m_dwTimer = 0;

	m_nActive = 0;
	for (i = 0; i < MAX_BLUR_FRAME; i++)
	{
		for (j = 0; j < MAX_PART; j++)
		{
			if (m_Blur[i][j].Color.Color_b.a)
			{
				if (m_Blur[i][j].Color.Color_b.a > BLUR_ALPHA_CHANGE)
					m_Blur[i][j].Color.Color_b.a -= BLUR_ALPHA_CHANGE;
				else
					m_Blur[i][j].Color.Color_b.a = 0;
				m_nActive = 1;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	功能：	清空当前指针指向的内容
//---------------------------------------------------------------------------
void	KNpcBlur::ClearCurNo()
{
	for (int i = 0; i < MAX_PART; i++)
	{
//		m_Blur[m_nCurNo][i].Release();
		m_Blur[m_nCurNo][i].Color.Color_b.a = 0;
	}
}

//---------------------------------------------------------------------------
//	功能：	设定当前某一项的内容
//---------------------------------------------------------------------------
void	KNpcBlur::SetFile(int nNo, char *lpszFileName, int nSprID, int nFrameNo, unsigned int uHue, int nXpos, int nYpos, int nZpos)
{
	if (nNo < 0 || nNo >= MAX_PART)
		return;
	if (!lpszFileName)
		return;
	strcpy(m_Blur[m_nCurNo][nNo].szImage, lpszFileName);
	m_Blur[m_nCurNo][nNo].uImage = nSprID;
	m_Blur[m_nCurNo][nNo].nFrame = nFrameNo;
	m_Blur[m_nCurNo][nNo].oPosition.nX = nXpos;
	m_Blur[m_nCurNo][nNo].oPosition.nY = nYpos;
	m_Blur[m_nCurNo][nNo].oPosition.nZ = nZpos;
	m_Blur[m_nCurNo][nNo].Color.Color_dw = uHue;
	m_Blur[m_nCurNo][nNo].Color.Color_b.a = START_BLUR_ALPHA;
	m_nActive = 1;
}

//---------------------------------------------------------------------------
//	功能：	绘制残影
//---------------------------------------------------------------------------
void	KNpcBlur::Draw(int nIdx)
{
	if (m_nActive == 0)
		return;

	g_pRepresent->DrawPrimitives(MAX_PART, m_Blur[nIdx], RU_T_IMAGE, FALSE);
}

//---------------------------------------------------------------------------
//	功能：	依据时间判断是否取残影
//---------------------------------------------------------------------------
BOOL	KNpcBlur::NowGetBlur()
{
	if (m_dwTimer == 0)
		return TRUE;
	return FALSE;
}

BOOL	KNpcBlur::Init()
{
	for (int i = 0; i < MAX_BLUR_FRAME; i++)
	{
		for (int j = 0; j < MAX_PART; j++)
		{
			m_Blur[i][j].nType = ISI_T_SPR;
			m_Blur[i][j].uImage = 0;
			m_Blur[i][j].bRenderStyle = IMAGE_RENDER_STYLE_HUE_ADJUST;
			m_Blur[i][j].nISPosition = IMAGE_IS_POSITION_INIT;
			m_Blur[i][j].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			m_Blur[i][j].Color.Color_dw = 0x80000000;
		}
	}
	return TRUE;
}

void	KNpcBlur::Remove()
{
	for (int i = 0; i < MAX_BLUR_FRAME; i++)
	{
		if (m_SceneID[i])
		{
			g_ScenePlace.RemoveObject(CGOG_NPC_BLUR_DETAIL(i), m_SceneIDNpcIdx[i], m_SceneID[i]);
			m_SceneID[i] = 0;
		}
	}
}

void	KNpcBlur::AddObj()
{
}

void	KNpcBlur::RemoveObj()
{
}



#endif