/*******************************************************************************
File        : CoverViewLayer.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 02-20-2004(mm-dd-yyyy)
Description : 覆盖地表的画面层
********************************************************************************/

#include "KCore.h"

#ifndef _SERVER

#include <math.h>
#include ".\coverviewlayer.h"

#include "iRepresentShell.h"

extern iRepresentShell*	g_pRepresent;

KCoverViewLayer::KCoverViewLayer(void)
{
	m_nResourceCount = 0;
	m_PaintRect.left = 0;
	m_PaintRect.right = enumCS_SCREEN_WIDTH;
	m_PaintRect.top = 0;
	m_PaintRect.bottom = enumCS_SCREEN_HEIGHT;
}

KCoverViewLayer::~KCoverViewLayer(void)
{
	m_CoverItem.Clear();
}

KCoverViewLayer::KCover::KCover(void)
{
	m_fX = 0.0;
	m_fY = 0.0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_fMoveSpeedX = 0.0;
	m_fMoveSpeedY = 0.0;
	m_nCurrentFrame = 0;

	m_nXSign = 0;
	m_nYSign = 0;
}




/************************************************************************************/
//设置移动方向，nDirection为上为0顺时针一周64度
/************************************************************************************/
void KCoverViewLayer::KCover::SetMoveParam(int nDirection, float fSpeed)
{
	int nPhase = nDirection >> 4;
	int nAngle = 0x10 - (nDirection & 0xf);
	m_fMoveSpeedY = float(fSpeed * sin(nAngle * (3.1415926 * 2 / 64)/*让编译器算这个*/));
	m_fMoveSpeedX = float(fSpeed * cos(nAngle * (3.1415926 * 2 / 64)/*让编译器算这个*/));
	m_nDirection = nDirection;
	m_fSpeed     = fSpeed;

	switch(nPhase)
	{
	case 0: //第一象限
		m_nXSign = 1;
		m_nYSign = -1;
		break;
	case 1: //第二象限
		{
			float fTemp = m_fMoveSpeedX;
			m_fMoveSpeedX = m_fMoveSpeedY;
			m_fMoveSpeedY = fTemp;
			m_nXSign = 1;
			m_nYSign = 1;
		}
		break;
	case 2: //第三象限
		m_nXSign = -1;
		m_nYSign = 1;
		break;
	case 3: //第四象限
		{
			float fTemp = m_fMoveSpeedX;
			m_fMoveSpeedX = m_fMoveSpeedY;
			m_fMoveSpeedY = fTemp;
			m_nXSign = -1;
			m_nYSign = -1;
		}
		break;
	}
}


/************************************************************************************/
//根据速度、方向计算新的m_nX、m_nY
/************************************************************************************/
void KCoverViewLayer::KCover::CalcPosition()
{
	if(m_nYSign > 0)
	{
		m_fY += m_fMoveSpeedY;
	}
	else
	{
		m_fY -= m_fMoveSpeedY;
	}

	if(m_nXSign > 0)
	{
		m_fX += m_fMoveSpeedX;
	}
	else
	{
		m_fX -= m_fMoveSpeedX;
	}
}


/************************************************************************************/
//是否让我可以画东西
/************************************************************************************/
void KCoverViewLayer::LetMePaint(BOOL bIsCanPaint)
{
	m_bIsCanPaint = bIsCanPaint;
}


/************************************************************************************/
//绘画函数
/************************************************************************************/
void KCoverViewLayer::Paint(int nX, int nY)
{
	if(m_nResourceCount && m_bIsCanPaint)
	{
		KCover *pNode = m_CoverItem.Begin();

		int nPicBeginX, nPicBeginY;
		int nCoverX, nCoverY;
		//首先计算出屏幕左上角坐标对应的场景坐标点
		int nScreenLPoint, nScreenTPoint;
		if(m_nMode == enumCM_2X_SPEED_TO_GROUND)
		{
			nScreenLPoint = (nX - enumCS_SCREEN_WIDTH / 2 * enumCS_SCREEN_WIDTH_RATE) * enumCS_COORDINATE_X_RATE;
			nScreenTPoint = (nY - enumCS_SCREEN_HEIGHT / 2 * enumCS_SCREEN_HEIGHT_RATE) * enumCS_COORDINATE_Y_RATE;
		}
		else
		{
			nScreenLPoint = (nX - enumCS_SCREEN_WIDTH / 2 * enumCS_SCREEN_WIDTH_RATE) * enumCS_COORDINATE_X_RATE_SMALL;
			nScreenTPoint = (nY - enumCS_SCREEN_HEIGHT / 2 * enumCS_SCREEN_HEIGHT_RATE) * enumCS_COORDINATE_Y_RATE_SMALL;
		}
		while(pNode)
		{
			///画覆盖图片
			//再计算坐标比例换算后的云的坐标
			nCoverX = (int)(pNode->m_fX);
			nCoverY = (int)(pNode->m_fY);

				///判断横向是否在范围里面
			if((nCoverX > (nScreenLPoint - pNode->m_nWidth * enumCS_SCREEN_WIDTH_RATE) && nCoverX < (nScreenLPoint + enumCS_SCREEN_WIDTH * enumCS_SCREEN_WIDTH_RATE)) &&
				///再判断纵向是否在范围内
			   (nCoverY > (nScreenTPoint - pNode->m_nHeight * enumCS_SCREEN_HEIGHT_RATE) && nCoverY < (nScreenTPoint + enumCS_SCREEN_HEIGHT * enumCS_SCREEN_HEIGHT_RATE)))
			{
				//if(nCoverX < nScreenLPoint)
				//{
				//	nPicBeginX = (nScreenLPoint - nCoverX) / enumCS_SCREEN_WIDTH_RATE;
				//}
				//else
				{
					nPicBeginX = 0;
				}
				//if(nCoverY < nScreenTPoint)
				//{
				//	nPicBeginY = (nScreenTPoint - nCoverY) / enumCS_SCREEN_HEIGHT_RATE;
				//}
				//else
				{
					nPicBeginY = 0;
				}
				m_Resource[pNode->m_nImgIndex].nFrame = pNode->m_nCurrentFrame;
				//if(nPicBeginX)
				//{
				//	m_Resource[pNode->m_nImgIndex].oPosition.nX = 0;
				//}
				//else
				{
					m_Resource[pNode->m_nImgIndex].oPosition.nX = (nCoverX - nScreenLPoint) / enumCS_SCREEN_WIDTH_RATE;
				}
				//if(nPicBeginY)
				//{
				//	m_Resource[pNode->m_nImgIndex].oPosition.nY = 0;
				//}
				//else
				{
					m_Resource[pNode->m_nImgIndex].oPosition.nY = (nCoverY - nScreenTPoint) / enumCS_SCREEN_HEIGHT_RATE;
				}

				m_Resource[pNode->m_nImgIndex].oEndPos.nX = m_Resource[pNode->m_nImgIndex].oPosition.nX + m_Resource[pNode->m_nImgIndex].nWidth - nPicBeginX;
				m_Resource[pNode->m_nImgIndex].oEndPos.nY = m_Resource[pNode->m_nImgIndex].oPosition.nY + m_Resource[pNode->m_nImgIndex].nHeight - nPicBeginY;

				m_Resource[pNode->m_nImgIndex].oImgLTPos.nX = nPicBeginX;
				m_Resource[pNode->m_nImgIndex].oImgLTPos.nY = nPicBeginY;

				//if(pNode->m_nWidth - nPicBeginX > enumCS_SCREEN_WIDTH)
				//{
				//	m_Resource[pNode->m_nImgIndex].oImgRBPos.nX = enumCS_SCREEN_WIDTH + nPicBeginX - 1;
				//}
				//else
				{
					m_Resource[pNode->m_nImgIndex].oImgRBPos.nX = pNode->m_nWidth;
				}
				//if(pNode->m_nHeight - nPicBeginY > enumCS_SCREEN_HEIGHT)
				//{
				//	m_Resource[pNode->m_nImgIndex].oImgRBPos.nY = enumCS_SCREEN_HEIGHT + nPicBeginY - 1;
				//}
				//else
				{
					m_Resource[pNode->m_nImgIndex].oImgRBPos.nY = pNode->m_nHeight;
				}
				g_pRepresent->DrawPrimitives(1, &m_Resource[pNode->m_nImgIndex], RU_T_IMAGE/*_PART*/, true);
			}

			///下一帧
			if(pNode->m_nCurrentFrame < m_Resource[pNode->m_nImgIndex].nNumFrames - 1)
			{
				pNode->m_nCurrentFrame ++;
			}
			else
			{
				pNode->m_nCurrentFrame = 0;
			}

			///下一个覆盖物件
			pNode = m_CoverItem.Next();
		}
	}
}


/************************************************************************************/
//心跳函数，只有有心跳，才能持续活动啊
/************************************************************************************/
int KCoverViewLayer::HeartBeat(int nRoleX, int nRoleY)
{
	MoveItem(nRoleX, nRoleY);
	return 1;
}


/************************************************************************************/
//根据一定规则生成很多个Cover物件！！！
/************************************************************************************/
int KCoverViewLayer::GenerateItem(int nDirection, float fSpeed, int nCount, int nRoleX, int nRoleY)
{
	int nRet = 0;
	if(nCount > 0)
	{
		//记下生成参数
		m_nDirection = nDirection;
		m_fSpeed     = fSpeed;
		m_nMaxCount  = nCount;
		m_nCount     = m_CoverItem.Count();

		//记录随机数字的一堆变量
		int nRand, nRandSign, nRandImg, nRandX, nRandY;
		//生成区域的长宽
		int nWidth, nHeight;
		nWidth = (enumCS_COVER_AREA_X / enumCS_COORDINATE_X_RATE) << 1;
		nHeight = (enumCS_COVER_AREA_Y / enumCS_COORDINATE_Y_RATE) << 1;

		for(int i = 0;i < nCount;i++)
		{
			nRand = g_Random(5);
			nRandSign = g_Random(2);
			nRandImg = g_Random(m_nResourceCount);
			nRandX = g_Random(nWidth);
			nRandY = g_Random(nHeight);

			if(!nRandSign)
			{
				nRand = -nRand;
			}
			if(nDirection + nRand > 63)
			{
				nRand = 64 - nDirection;
			}
			else if(nDirection + nRand < 0)
			{
				nRand = - nDirection;
			}
			if(fSpeed + nRand / 10 < 0)
			{
				nRand = -((int)(fSpeed + 0.1)) * 10;
			}
			ConstructOneItem(nDirection + nRand, fSpeed + nRand / 10, nRandImg, nRandX + nRoleX - enumCS_COVER_AREA_X, nRandY + nRoleY - enumCS_COVER_AREA_Y);
		}
	}
	return nRet;
}


/************************************************************************************/
//在旧的物件消亡以后，在覆盖区域边缘生成新的物件
/************************************************************************************/
int KCoverViewLayer::GenerateNewItem(int nRoleX, int nRoleY)
{
	int nInterval;
	m_nCount  = m_CoverItem.Count();
	nInterval = m_nMaxCount - m_nCount;
	if(nInterval < 0)
	{
		nInterval = 0;
	}
	while(m_nCount < m_nMaxCount)
	{
		ConstructOneItemAtEdge(m_nDirection, m_fSpeed, g_Random(m_nResourceCount));
	}
	return nInterval;
}


/************************************************************************************/
//根据给出的规则生成一个Cover物件
/************************************************************************************/
int KCoverViewLayer::ConstructOneItem(int nDirection, float fSpeed, int nResourceIndex, int nX, int nY)
{
	int nRet = 0;
	if(m_nResourceCount)
	{
		KCover *pItem = new KCover;

		if(m_nMode == enumCM_1X_SPEED_TO_GROUND)
		{
			pItem->m_fX = (float)nX * enumCS_COORDINATE_X_RATE_SMALL;
			pItem->m_fY = (float)nY * enumCS_COORDINATE_Y_RATE_SMALL;
		}
		else
		{
			pItem->m_fX = (float)nX * enumCS_COORDINATE_X_RATE;
			pItem->m_fY = (float)nY * enumCS_COORDINATE_Y_RATE;
		}
		pItem->SetMoveParam(nDirection, fSpeed);
		if(nResourceIndex < 0)
		{
			pItem->m_nImgIndex = 0;
		}
		else if(nResourceIndex >= m_nResourceCount)
		{
			pItem->m_nImgIndex = m_nResourceCount - 1;
		}
		else
		{
			pItem->m_nImgIndex = nResourceIndex;
		}
		pItem->m_nWidth = m_Resource[pItem->m_nImgIndex].nWidth;
		pItem->m_nHeight = m_Resource[pItem->m_nImgIndex].nHeight;
		m_CoverItem.Add(pItem);
		m_nCount = m_CoverItem.Count();
		nRet = 1;
	}
	return nRet;
}


/************************************************************************************/
//根据角度在适当的边缘生成一个覆盖物件
/************************************************************************************/
int KCoverViewLayer::ConstructOneItemAtEdge(int nDirection, float fSpeed, int nResourceIndex)
{
	//懒得做那么多数学运算，哈哈
	int nX, nY;
	if(nDirection > 56 || nDirection <= 8)
	{
		nX = g_Random(m_CoverRect.right - m_CoverRect.left) + m_CoverRect.left;
		nY = m_CoverRect.bottom;
	}
	else if(nDirection > 8 && nDirection <= 24)
	{
		nX = m_CoverRect.left;
		nY = g_Random(m_CoverRect.bottom - m_CoverRect.top) + m_CoverRect.top;
	}
	else if(nDirection > 24 && nDirection <= 40)
	{
		nX = g_Random(m_CoverRect.right - m_CoverRect.left) + m_CoverRect.left;
		nY = m_CoverRect.top;
	}
	else
	{
		nX = m_CoverRect.right;
		nY = g_Random(m_CoverRect.bottom - m_CoverRect.top) + m_CoverRect.top;
	}
	return ConstructOneItem(nDirection, fSpeed, nResourceIndex, nX, nY);
}


/************************************************************************************/
//移动物件
/************************************************************************************/
void KCoverViewLayer::MoveItem(int nRoleX, int nRoleY)
{
	if(m_bIsCanPaint)
	{
		KCover *pNode = m_CoverItem.Begin();
		while(pNode)
		{
			//计算新的位置
			pNode->CalcPosition();
			if(m_nMode == enumCM_1X_SPEED_TO_GROUND)
			{
				if(pNode->m_fX < (nRoleX * enumCS_COORDINATE_X_RATE_SMALL - enumCS_COVER_AREA_X))
				{
					pNode->m_fX = pNode->m_fX + (float)(enumCS_COVER_AREA_X * 2);
				}
				if(pNode->m_fX > (nRoleX * enumCS_COORDINATE_X_RATE_SMALL + enumCS_COVER_AREA_X))
				{
					pNode->m_fX = pNode->m_fX - (float)(enumCS_COVER_AREA_X * 2);
				}
				if(pNode->m_fY < (nRoleY * enumCS_COORDINATE_Y_RATE_SMALL - enumCS_COVER_AREA_Y))
				{
					pNode->m_fY = pNode->m_fY + (float)(enumCS_COVER_AREA_Y * 2);
				}
				if(pNode->m_fY > (nRoleY * enumCS_COORDINATE_Y_RATE_SMALL + enumCS_COVER_AREA_Y))
				{
					pNode->m_fY = pNode->m_fY - (float)(enumCS_COVER_AREA_Y * 2);
				}
			}
			else
			{
				if(pNode->m_fX < (nRoleX * enumCS_COORDINATE_X_RATE - enumCS_COVER_AREA_X))
				{
					pNode->m_fX = pNode->m_fX + (float)(enumCS_COVER_AREA_X * 2);
				}
				if(pNode->m_fX > (nRoleX * enumCS_COORDINATE_X_RATE + enumCS_COVER_AREA_X))
				{
					pNode->m_fX = pNode->m_fX - (float)(enumCS_COVER_AREA_X * 2);
				}
				if(pNode->m_fY < (nRoleY * enumCS_COORDINATE_Y_RATE - enumCS_COVER_AREA_Y))
				{
					pNode->m_fY = pNode->m_fY + (float)(enumCS_COVER_AREA_Y * 2);
				}
				if(pNode->m_fY > (nRoleY * enumCS_COORDINATE_Y_RATE + enumCS_COVER_AREA_Y))
				{
					pNode->m_fY = pNode->m_fY - (float)(enumCS_COVER_AREA_Y * 2);
				}
			}
			pNode = m_CoverItem.Next();
		}
	}
	return;
}


/************************************************************************************/
//加载一个图形资源
/************************************************************************************/
int KCoverViewLayer::Load(char *pszFileName)
{
	int nRet = 0;
	if(m_nResourceCount < enumCS_MAX_GRAPHIC_RESOURCE - 1)
	{
		KImageParam ImgParam;
		if(g_pRepresent->GetImageParam(pszFileName, &ImgParam, ISI_T_SPR))
		{
			m_Resource[m_nResourceCount].nWidth  = ImgParam.nWidth;
			m_Resource[m_nResourceCount].nHeight = ImgParam.nHeight;
			m_Resource[m_nResourceCount].nInterval = ImgParam.nInterval;
			m_Resource[m_nResourceCount].nNumFrames = ImgParam.nNumFrames;
			m_Resource[m_nResourceCount].nNumFramesGroup = ImgParam.nNumFramesGroup;
			m_Resource[m_nResourceCount].nReferenceSpotX = ImgParam.nReferenceSpotX;
			m_Resource[m_nResourceCount].nReferenceSpotY = ImgParam.nReferenceSpotY;

			strcpy(m_Resource[m_nResourceCount].szImage, pszFileName);
			m_Resource[m_nResourceCount].nType = ISI_T_SPR;
			m_Resource[m_nResourceCount].bRenderFlag = RUIMAGE_RENDER_FLAG_FRAME_DRAW;
			m_Resource[m_nResourceCount].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_Resource[m_nResourceCount].Color.Color_b.a = 255;
			m_Resource[m_nResourceCount].Color.Color_b.r = 0;
			m_Resource[m_nResourceCount].Color.Color_b.g = 0;
			m_Resource[m_nResourceCount].Color.Color_b.b = 0;

			m_Resource[m_nResourceCount].oPosition.nZ = 0;
			m_Resource[m_nResourceCount].oEndPos.nZ = 0;

			m_nResourceCount ++;
			nRet = 1;
		}
	}

	return nRet;
}


/**
 * @brief 清除资源和数据
 */
void KCoverViewLayer::Clear()
{
	m_nResourceCount = 0;
	memset(&m_Resource, 0, sizeof(m_Resource));
	m_CoverItem.Clear();
}


/************************************************************************************/
//设置覆盖范围
/************************************************************************************/
void KCoverViewLayer::SetCoverRect(int nTop, int nLeft, int nBottom, int nRight)
{
	m_CoverRect.top = nTop;
	m_CoverRect.left= nLeft;
	m_CoverRect.bottom = nBottom;
	m_CoverRect.right  = nRight;
}


/************************************************************************************/
//设置绘画范围
/************************************************************************************/
void KCoverViewLayer::SetPaintRect(int nTop, int nLeft, int nBottom, int nRight)
{
	m_PaintRect.top = nTop;
	m_PaintRect.left= nLeft;
	m_PaintRect.bottom = nBottom;
	m_PaintRect.right  = nRight;
}
#endif
