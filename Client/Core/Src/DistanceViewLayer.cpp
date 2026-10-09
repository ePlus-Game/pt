/*******************************************************************************
File        : DistanceViewLayer.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 01-02-2004(mm-dd-yyyy)
Description : 远景的画面层的cpp
********************************************************************************/

#include "KCore.h"

#ifndef _SERVER
#include "iRepresentShell.h"

#include "imgref.h"
#include "DistanceViewLayer.h"

#include "KJpgFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KDistanceViewLayer::KDistanceViewLayer()
{
	m_bCanMePaint  = FALSE;
	m_bLoaded      = FALSE;
	m_CenterPoint.x = -1;
	m_CenterPoint.y = -1;
	m_nHeight = -1;
	m_nWidth = -1;

	Initialize();
}

KDistanceViewLayer::~KDistanceViewLayer()
{
	m_ViewFile.ReleaseCell();
}

//-----------------------------------------------------------------------------//
//-----------------------------------------------------------------------------//
//-----------------------------------------------------------------------------//


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌───┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │初始化│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬─┬┘*/
void KDistanceViewLayer::Initialize()
{
	return;
}


/**
* @brief 释放所占用的资源
*/
void KDistanceViewLayer::Release()
{
	m_ViewFile.ReleaseCell();
	m_bCanMePaint = FALSE;
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌──────┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │载入远景图片│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬────┬┘*/
int	KDistanceViewLayer::Load(char *szPicFileName)
{
	m_bLoaded = m_ViewFile.LoadJPG(szPicFileName);
	if(m_bLoaded)
	{
		m_nWidth = m_ViewFile.GetWidth();
		m_nHeight = m_ViewFile.GetHeight();
	}
	return m_bLoaded;
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌─────┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │让我画东西│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬───┬┘*/
//bIsPaint = TRUE 画，bIsPaint = FALSE 不画
void KDistanceViewLayer::LetMePaint(BOOL bIsPaint)
{
	m_bCanMePaint = bIsPaint;
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌───────┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │设置卷动的步伐│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬─────┬┘*/
//前景每卷动多少单位(nFrontStepX..Y)，
//远景才卷动nDistanceStep(X..Y)像素
void KDistanceViewLayer::SetScrollStep(int nFrontStepX, int nFrontStepY, int nDistanceStepX, int nDistanceStepY)
{
	int nRateX, nRateY;

	//把倍数向2的整数次方取齐
	nRateX = nFrontStepX / nDistanceStepX;
	nRateY = nFrontStepY / nDistanceStepY;

	m_nRateX = -1;
	m_nRateY = -1;
	while(nRateX)
	{
		m_nRateX ++;
		nRateX >>= 1;
	};
	while(nRateY)
	{
		m_nRateY ++;
		nRateY >>= 1;
	};
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌───────────────────┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │设置这个远景的图画的中心位置(场景坐标)│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬─────────────────┬┘*/
void KDistanceViewLayer::SetCenterPoint(int nSceneX, int nSceneY, int nPicX, int nPicY)
{
	m_CenterPoint.x = nSceneX;
	m_CenterPoint.y = nSceneY;

	//计算图片中心跟场景卷轴重心之间的位移
	int nIntervalX = nSceneX - nPicX;
	int nIntervalY = nSceneY - nPicY;

	m_nPictureCenterOffset.x = nIntervalX;
	m_nPictureCenterOffset.y = nIntervalY;

	//正负处理
	int nFlagX = nIntervalX & 0x80000000;
	int nFlagY = nIntervalY & 0x80000000;

	//计算绘画区域的左上角坐标和图片的左上角坐标
	POINT posPaintRectLT, posPicLT;
	posPaintRectLT.x = nSceneX - ((m_PaintRect.right - m_PaintRect.left) >> 1);
	posPaintRectLT.y = nSceneY - ((m_PaintRect.bottom - m_PaintRect.top) >> 1);

	posPicLT.x = nPicX - (m_nWidth >> 1);
	posPicLT.y = nPicY - (m_nHeight >> 1);

	//计算平铺之后实际落在绘画区域左上角的图片的位置
	nIntervalX = (posPaintRectLT.x - posPicLT.x) % m_nWidth;
	nIntervalY = (posPaintRectLT.y - posPicLT.y) % m_nHeight;
	if(nIntervalX < 0)
	{
		nIntervalX = m_nWidth + nIntervalX;
	}
	if(nIntervalY < 0)
	{
		nIntervalY = m_nHeight + nIntervalY;
	}

	//进行后续处理(正负的处理)
	if(nFlagX)
	{
		nIntervalX = m_nWidth - nIntervalX;
	}
	if(nFlagY)
	{
		nIntervalY = m_nHeight - nIntervalY;
	}

	//记录结果
	m_TopLeftPoint.x = nIntervalX;
	m_TopLeftPoint.y = nIntervalY;
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌──────┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │设置画布区域│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬────┬┘*/
void KDistanceViewLayer::SetPaintRect(int nTop, int nLeft, int nBottom, int nRight)
{
	m_PaintRect.top = nTop;
	m_PaintRect.left = nLeft;
	m_PaintRect.bottom = nBottom;
	m_PaintRect.right  = nRight;
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌──┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │绘画│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬┬┘*/
//传入的坐标为当前的场景坐标
//误差包容，即在计算过程中会出现误差，不过足够小可以包容
void KDistanceViewLayer::Paint(int nX, int nY)
{
	if(!m_bCanMePaint)
		return;

	//场景坐标->小地图坐标

	int nIntervalX, nIntervalY;
	int nXFlag = 0, nYFlag = 0;

	int nPicCenterX, nPicCenterY;

	//计算传入坐标与中心坐标的位移
	nIntervalX = nX - m_CenterPoint.x;
	nIntervalY = nY - m_CenterPoint.y;

	//取是否正负数
	nXFlag = nIntervalX & 0x80000000;
	nYFlag = nIntervalY & 0x80000000;

	//变为正数，有些微(1)误差在接受范围内
	nIntervalX &= ~0x80000000;
	nIntervalY &= ~0x80000000;

	//通过同步比率计算远景图片的位移
	nIntervalX >>= m_nRateX;
	nIntervalY >>= m_nRateY;

	//计算图片中心点
	nPicCenterX = m_nWidth >> 1;
	nPicCenterY = m_nHeight >> 1;

	//计算左上角图片位置啦，然后下面那些可以推出来了
	nIntervalX = (nIntervalX + m_TopLeftPoint.x) % m_nWidth;
	nIntervalY = (nIntervalY + m_TopLeftPoint.y) % m_nHeight;

	//正负处理
	if(nXFlag)
	{
		nIntervalX = m_nWidth - nIntervalX;
	}
	if(nYFlag)
	{
		nIntervalY = m_nHeight - nIntervalY;
	}

	//把计算结果传入绘画实施工程队
	PaintView(nIntervalX, nIntervalY);
	return;
}


/*
　★　　 ◢◣
　　　　　　　　　　　　　◢◢◣　　　☆
　　　　　 /○\ ●　　　 ◢◢◣◣　　　 ┌────┐
　　　　　 /■\/■\　　◢◢◢◣◣◣　　 │绘画远景│
　 ╪═╪　 <|　||　　═╪═╪═╪═╪　└┬──┬┘*/
void KDistanceViewLayer::PaintView(int nX, int nY)
{
	//变量定义
	int nCanvasWidth, nCanvasHeight;

	nCanvasWidth  = m_PaintRect.right - m_PaintRect.left;
	nCanvasHeight = m_PaintRect.bottom - m_PaintRect.top;

	//画图
	//又一堆变量定义
	int nCellBeginX, nCellBeginY, nCellEndX, nCellEndY, nPaintX, nPaintY, nPaintWidth, nPaintHeight;

	nPaintX = m_PaintRect.left;
	nPaintY = m_PaintRect.top;
	while(nPaintY < nCanvasHeight)
	{
		//如果是最上边的那部分图，那么上边可能会被截去一部分
		if(nPaintY == m_PaintRect.top)
		{
			nCellBeginY = nY;
		}
		//否则，上边是不会被截去一部分的
		else
		{
			nCellBeginY = 0;
		}
		//如果是最下边的那部分图，那么下边可能会被截去一部分
		if((m_PaintRect.bottom - nPaintY) < (m_nHeight - nCellBeginY))
		{
			nCellEndY = m_PaintRect.bottom - nPaintY + nCellBeginY;
		}
		//否则，下边是不会被截去一部分的
		else
		{
			nCellEndY = m_nHeight - 1;
		}
		while(nPaintX < m_PaintRect.right)
		{
			//如果是最左边的那部分图，那么左边可能会被截去一部分
			if(nPaintX == m_PaintRect.left)
			{
				nCellBeginX = nX;
			}
			//否则，左边是不会被截去一部分的
			else
			{
				nCellBeginX = 0;
			}
			//如果是最右边的那部分图，那么右边可能会被截去一部分
			if((m_PaintRect.right - nPaintX) < (m_nWidth - nCellBeginX))
			{
				nCellEndX = m_PaintRect.right - nPaintX + nCellBeginX;
			}
			//否则，是不会被截去一部分的
			else
			{
				nCellEndX = m_nWidth - 1;
			}
			//调用这个在指定的屏幕位置画一个Cell
			m_ViewFile.Paint(nPaintX, nPaintY, nCellBeginX, nCellBeginY, nCellEndX - nCellBeginX + 1, nCellEndY - nCellBeginY + 1, nPaintWidth, nPaintHeight);
			nPaintX += nPaintWidth;
		}
		nPaintX = m_PaintRect.left;
		nPaintY += nPaintHeight;
	}
	//
}




/****************************** The End ****************************/
//.
//.
//.
//.
//.
#endif
