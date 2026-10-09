/*******************************************************************************
File        : DistanceViewLayer.h
Creator     : Fyt(Fan Zhanpeng)
create data : 01-02-2004(mm-dd-yyyy)
Description : 远景的画面层
********************************************************************************/

#if !defined(AFX_DISTANCEVIEWLAYER_H__BCFAE0BF_2164_4286_994F_6EA9EF418D62__INCLUDED_)
#define AFX_DISTANCEVIEWLAYER_H__BCFAE0BF_2164_4286_994F_6EA9EF418D62__INCLUDED_

#ifndef _SERVER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//关键字：场景卷轴重心、图片中心偏移、绘画区域

#include "PosterIncise.h"

class KDistanceViewLayer
{
public:
	KDistanceViewLayer();
	virtual ~KDistanceViewLayer();

	//
	//载入远景图片
	//
	int				Load(char *szPicFileName);

	//
	//设置卷动的步伐，前景每卷动多少像素(nFrontStepX..Y)，远景才卷动nDistanceStep(X..Y)像素
	//效率关系，会把对比关系进行处理，反正就是不一定会和传入一样
	//
	void			SetScrollStep(int nFrontStepX, int nFrontStepY, int nDistanceStepX = 1, int nDistanceStepY = 1);

	//
	//设置场景的重心位置(场景坐标)
	//
	void 			SetCenterPoint(int nSceneX, int nSceneY, int nPicX, int nPicY);

	//
	//设置当场景处于场景重心的时候，远景图片的中心位置
	//

	//
	//设置画布区域
	//
	void			SetPaintRect(int nTop, int nLeft, int nBottom, int nRight);

	//
	//心跳，这个对象的活动函数
	//
	int				HeartBeat();

	//
	//呼吸，同心跳
	//
	int				Breathe();

	//
	//让我画东西，bIsPaint = TRUE 画，bIsPaint = FALSE 不画
	//
	void			LetMePaint(BOOL bIsPaint/*?*/);

	//
	//绘画，传入的坐标为当前的场景坐标
	//
	void			Paint(int nX, int nY);

	//
	//返回是否已经载入图片
	//
	BOOL			IsLoaded(){return m_bLoaded;};

	/**
	 * @brief 释放所占用的资源
	 */
	void			Release();

private:
	//
	//初始化
	//
	void			Initialize();

	//
	//执手尾
	//
	void			Terminate();

private:
	//
	//绘画远景，远景的图层铺满绘画区域，传入的坐标表明绘画区域最左上角落在
	//远景图片的哪一点，如图片的那部分不够铺满屏幕，就自动进行平铺
	//调用PaintCell()画出各部分CELL
	//
	void			PaintView(int nX, int nY);

private:
	//判斷是否能夠繪畫的判斷器
	BOOL			m_bCanMePaint;

	//是否已經完成載入
	BOOL			m_bLoaded;

	//卷轴的场景重心，在玩家角色可以活动的范围内
	POINT			m_CenterPoint;

	//图片的中点和场景卷轴重心的偏移
	POINT			m_nPictureCenterOffset;

	//当场景重心处于卷轴重心位置的时候，图片的中心也应该显示在预设的中心点
	//那么这时候，在当前的绘画区域里，最左上角会落在图片的哪一点上？这个值
	//就记录了这一个数值，通常这个值在设置绘画区域的时候、或者载入图片的时
	//侯计算并记录。
	POINT			m_TopLeftPoint;

	//绘画区域，远景图不一定占满整个屏幕，这里决定其区域范围
	RECT			m_PaintRect;

	//图片的长宽
	int				m_nWidth, m_nHeight;

	//步伐的比率
	int				m_nRateX, m_nRateY;

	//图片处理
	KPosterIncise	m_ViewFile;
};

#endif // !defined(AFX_DISTANCEVIEWLAYER_H__BCFAE0BF_2164_4286_994F_6EA9EF418D62__INCLUDED_)
#endif
