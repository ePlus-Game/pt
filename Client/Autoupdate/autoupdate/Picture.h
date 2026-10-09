// Picture.h: interface for the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURE_H__FEEDAFE6_6A51_42FA_B769_0E9742D6E16F__INCLUDED_)
#define AFX_PICTURE_H__FEEDAFE6_6A51_42FA_B769_0E9742D6E16F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxctl.h>

class CPicture : public CPictureHolder
{
public:
	CPicture();
	virtual ~CPicture();
	BOOL Load(CString sFilePathName);				//从文件读取图像
	BOOL Load(HINSTANCE hInstance,LPCTSTR lpszResourceName, LPCSTR ResourceType);//从资源读取图像
	BOOL LoadPictureData(BYTE* pBuffer, int nSize);	//从内存读取图像
	BOOL SaveAsBitmap(CString sFilePathName);		//写入到BMP文件
	// 在给定的DC上画图，
	void Render(CDC* pDC,
		LPRECT pDrawRect,			//目标矩形，单位是逻辑坐标单位
		LPRECT pSrcRect = NULL,		//来源矩形，单位是0.01毫米,如果为空，则拉伸整个图像到目标矩形
		LPCRECT prcWBounds = NULL);	//图元文件专用，绑定矩形
	void  UnloadPicture();//释放图像，作用同CPictureHolder::~CPictureHolder()
public:
	LONG      get_Height(); // 以0.01毫米为单位的图像高度
	LONG      get_Width();  // 以0.01毫米为单位的图像宽度
	
};

#endif // !defined(AFX_PICTURE_H__FEEDAFE6_6A51_42FA_B769_0E9742D6E16F__INCLUDED_)
