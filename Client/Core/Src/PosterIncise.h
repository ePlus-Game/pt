/*******************************************************************************
File        : PosterIncise.h
Creator     : Fyt(Fan Zhanpeng)
create data : 02-23-2004(mm-dd-yyyy)
Description : 把一幅图形分CELL切割开来，并在Represent模块创建图形资源
            : 并且可以在指定位置绘画出某部分，或某CELL
********************************************************************************/

#pragma once

#include "KRepresentUnit.h"

#ifndef _SERVER
class KPosterIncise
{
public:
	KPosterIncise(void);
	~KPosterIncise(void);

public: //类本身独有的数据定义
	struct MAP_CELL
	{
		 char			szImageName[32];
		 unsigned int	uImageId;
		 short			sISPosition;
		 short			sbIsDefined;
		 int			nWidth;
		 int			nHeight;
	};

	enum enumCELL_SETTING
	{
		enumCS_CELL_WIDTH  = 64,				//这个值必须是2的N次方，帮助运算优化
		enumCS_CELL_HEIGHT = enumCS_CELL_WIDTH, //这个就定值了！！！！不要改！！！运算的优化要用！！
	};

public: //对外部开放的接口
	//
	//载入JPG图片
	//
	int				LoadJPG(char *szPicFileName);


	//
	//绘制当前CELL的某部分
	//
	void			PaintCurrentCell(int nPaintX, int nPaintY, int nCellX, int nCellY, int nCellWidth, int nCellHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight);

	//(多态)(注意)(谨慎)(勿忘)★
	//绘制由X索引和Y索引所指定的某个CELL的某部分
	//
	void			PaintCell(int nPaintX, int nPaintY, int nCellIndexX, int nCellIndexY,int nCellX, int nCellY, int nCellWidth, int nCellHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight);

	//(多态)(注意)(谨慎)(勿忘)★
	//绘制由总索引所指定的某个CELL的某部分
	//
	void			PaintCell(int nPaintX, int nPaintY, int nCellIndex, int nCellX, int nCellY, int nCellWidth, int nCellHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight);

	//
	//绘制整张图的某部分
	//
	void			Paint(int nPaintX, int nPaintY, int nX, int nY, int nWidth, int nHeight, int /*[out]*/&nPaintWidth, int /*[out]*/&nPaintHeight);


	//(多态)(注意)(谨慎)(勿忘)★
	//根据X和Y索引设置当前CELL，返回总索引
	//
	int				SetCurrentCell(int nX, int nY);

	//(多态)(注意)(谨慎)(勿忘)★
	//根据总索引设置当前CELL，返回总索引
	//
	int				SetCurrentCell(int nIndex);

	//
	//把当前CELL移去下一列，返回总索引
	//
	int				NextVCell();

	//
	//把当前CELL移去下一行，返回总索引
	//
	int				NextHCell();

	//
	//设置当前CELL去指定的列，返回总索引
	//
	int				SetV(int nVertical);

	//
	//设置当前CELL去指定的行，返回总索引
	//
	int				SetH(int nHorizontal);

	//
	//获取图片的宽度
	//
	int				GetWidth(){return m_nWidth;};

	//
	//获取图片的高度
	//
	int				GetHeight(){return m_nHeight;};

	//
	//释放CELL资源
	//
	void			ReleaseCell();

	//
	//返回是否成功载入图片
	//
	BOOL			IsLoaded(){return m_bIsLoaded;};

private:
	//
	//构造CELL，传入要构造的CELL的数量，-1(默认值)的话就根据m_nCellCount来构造
	//
	int				ConstructCell(int nCount = -1);

	//
	//根据m_nWidth和m_nHeight来运算出Cell相关数值
	//
	void			WorkoutCellInfo();

	//
	//从整张图片上攫取一部分，拷贝到指定的缓冲区里，并且把实际拷贝的长宽
	//告诉调用者
	//
	void			CopyPicToBuff(void* pDest, void* pSrc, int nColorByte, int nSrcX, int nSrcY, int &nCopyWidth, int &nCopyHeight);

private:
	//图形的宽度
	int				m_nWidth;
	//图形的高度
	int				m_nHeight;

	//载入的图片的类型
	KIS_IMAGE_TYPE	m_eType;

	//这个物件是否已经载入图形
	BOOL			m_bIsLoaded;

	//指向分割的各个CELL的信息表
	MAP_CELL*		m_pCell;


	//CELL的数量
	int				m_nCellCount;
	//CELL的水平线数量
	int				m_nHCount;
	//CELL的垂直线数量
	int				m_nVCount;


	//当前CELL的索引
	int				m_nCurrentCell;
	//当前CELL的X轴索引
	int				m_nCurrentCellX;
	//当前CELL的Y轴索引
	int				m_nCurrentCellY;

	//根据图形名字计算出的哈希
	int				m_nNameHash;

	/****************************************************************************/
	                               /*内部变量*/
	//将CELL的宽高转换为1在第几位，以协助除运算变为右移位运算
	int				m_nDivBit;
	//为协助某数对CELL取余数，使用 & 运算优化所需要的值
	int				m_nResidueBit;
	//调用RepresentShell模块绘图所需的参数
	KRUImagePart	m_Img;
	/****************************************************************************/
};

// 冲突
//unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive = FALSE);
#endif
