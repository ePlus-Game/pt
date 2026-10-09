/*******************************************************************************
File        : CoverViewLayer.h
Creator     : Fyt(Fan Zhanpeng)
create data : 02-20-2004(mm-dd-yyyy)
Description : 覆盖地表的画面层
********************************************************************************/

//集中绘画机制，从所有Item索取数据，进行绘画，而不是各个Item自己画自己

#pragma once

#ifndef _SERVER

#include "LinkStructEx.h"
#include "KRepresentUnit.h"

class KSprite;

class KCoverViewLayer
{
public:
	KCoverViewLayer(void);
	~KCoverViewLayer(void);

public: //内部数据定义
	class KCover
	{
	public:
		KCover();

		float m_fX;				//当前所在的X坐标
		float m_fY;				//当前所在的Y坐标
		int m_nWidth;			//这个东西的宽度
		int m_nHeight;			//这个东西的高度
		int m_nImgIndex;		//这个覆盖物件的图形资源
		int m_nCurrentFrame;	//当前在播放的帧

								//设置移动参数，方向nDirection为上为0顺时针一周64度
		void SetMoveParam(int nDirection, float fSpeed);

		void CalcPosition();	//根据速度、方向计算新的m_nX、m_nY

		//获取方向
		int  GetDirection(){return m_nDirection;};
		//获取速度
		float GetSpeed(){return m_fSpeed;};

	private: //移动速度和方向相关的数据
		int m_nXSign;
		int m_nYSign;
		int m_nDirection;		//方向
		float m_fSpeed;			//速度

		float m_fMoveSpeedX;	//移动的速度在X轴的分量
		float m_fMoveSpeedY;	//移动的速度在Y轴的分量
	};

	struct tagIMAGE_ELEMENT : public KRUImagePart, public KImageParam
	{
	};

	enum enumCOVER_SETTING
	{
		enumCS_MAX_GRAPHIC_RESOURCE = 10,	//最大图形资源
		enumCS_SCREEN_WIDTH = 800,			//屏幕横向的分辨率
        enumCS_SCREEN_HEIGHT = 600,			//屏幕纵向的分辨率
		enumCS_SCREEN_WIDTH_RATE = 1,		//场景横向坐标单位和屏幕横向分辨率单位的比值
		enumCS_SCREEN_HEIGHT_RATE = 2,		//场景纵向坐标单位和屏幕纵向分辨率单位的比值
		enumCS_COORDINATE_X_RATE = 2,		//场景坐标和这个卷轴坐标的横向坐标比例
		enumCS_COORDINATE_Y_RATE = 2,		//场景坐标和这个卷轴坐标的纵向坐标比例
		enumCS_COORDINATE_X_RATE_SMALL = 1, //场景坐标和这个卷轴坐标的横向坐标比例缩小版
		enumCS_COORDINATE_Y_RATE_SMALL = 1,	//场景坐标和这个卷轴坐标的纵向坐标比例缩小版
		enumCS_COVER_AREA_X		 = 800 * enumCS_COORDINATE_X_RATE * enumCS_SCREEN_WIDTH_RATE,//前景层在以角色为中心的左右多大范围内有前景
		enumCS_COVER_AREA_Y		 = 600 * enumCS_COORDINATE_Y_RATE * enumCS_SCREEN_HEIGHT_RATE,//前景层在以角色为中心的左右多大范围内有前景
		enumCS_COVER_AREA_X_SMALL= 400,
		enumCS_COVER_AREA_Y_SMALL= 300,
	};

	enum enumCOVER_MODE
	{
		enumCM_1X_SPEED_TO_GROUND,			//高处
		enumCM_2X_SPEED_TO_GROUND,			//地面
		enumCM_DYING,						//死亡模式，云慢慢地消散
	};

public: //对外接口
	//
	//是否让我可以画东西
	//
	void			LetMePaint(BOOL bIsCanPaint);

	//
	//绘画函数
	//
	void			Paint(int nX, int nY);

	//
	//心跳函数，只有有心跳，才能持续活动啊
	//
	int				HeartBeat(int nRoleX, int nRoleY);

	//
	//设置覆盖范围
	//
	void			SetCoverRect(int nTop, int nLeft, int nBottom, int nRight);

	//
	//设置绘画范围
	//
	void			SetPaintRect(int nTop, int nLeft, int nBottom, int nRight);

	//
	//根据一定规则生成很多个Cover物件！！！
	//
	int				GenerateItem(int nDirection/*上为0，顺时针一周64度， 在这个方法里面是大概方向*/,
								 float fSpeed/*在这个方法里是大概速度*/, int nCount/*生成的数量*/,
								 /*角色所在的坐标*/int nRoleX, int nRoleY);

	//
	//在旧的物件消亡以后，在覆盖区域边缘生成新的物件
	//
	int				GenerateNewItem(int nRoleX, int nRoleY);

	//
	//根据给出的规则生成一个Cover物件
	//
	int				ConstructOneItem(int nDirection/*上为0，顺时针一周64度*/, float fSpeed, int nResourceIndex, int nX, int nY);

	/**
	 * @brief 根据角度在适当的边缘生成一个覆盖物件
	 */
	int				ConstructOneItemAtEdge(int nDirection/*上为0，顺时针一周64度*/, float fSpeed, int nResourceIndex);

	/**
	 * @brief 加载一个图形资源
	 * @param [in]pszFileName是图形文件的路径
	 * @return 大于0成功，否则是失败
	 */
	int				Load(char *pszFileName);

	/**
	 * @brief 清除资源和数据
	 */
	void			Clear();

	/**
	 * @brief 设置相对地面运动模式
	 */
	void			Mode(enumCOVER_MODE eMode){m_nMode = eMode;};

private:
	//
	//移动物件
	//
	void			MoveItem(int nRoleX, int nRoleY);

private: //
	//是否绘画自己的判定器
	BOOL			m_bIsCanPaint;

	//所有覆盖物件的双向二位表
	KLinkStructEx<KCover>
					m_CoverItem;

	//会生成覆盖物件的区域范围，这是指场景范围，单位是场景坐标单位
	RECT			m_CoverRect;

	//绘画范围，这是指屏幕范围，单位是像素
	RECT			m_PaintRect;

	//图形资源，最多enumCS_MAX_GRAPHIC_RESOURCE个
	tagIMAGE_ELEMENT m_Resource[enumCS_MAX_GRAPHIC_RESOURCE];
	//已使用图形资源的数量
	int				m_nResourceCount;

	//批量生成物件的方向
	int				m_nDirection;
	//批量生成物件的速度
	float			m_fSpeed;
	//批量生成物件的最大数量
	int				m_nMaxCount;
	//当前物件的数量
	int				m_nCount;
	//前一次角色所在的位置
	int				m_nOldRoleX;
	int				m_nOldRoleY;
	//和地面相对运动的模式
	int				m_nMode;
};
#endif
