#ifndef __DIB_H
#define __DIB_H
//
// CDIB -
// CDibDC -
//
// Implements a simple encapsulation of a DIB section and a DC.
//
//

//////////////////////////////////////////////////////////////////////
//
// CDIB
//
// 设备无关位图类(版本1.0)
//
// 完成功能:
//     设备无关位图的创建,显示,读入,保存,捕捉位图
//
// 版本所有: 罗隽,曾志
//
//////////////////////////////////////////////////////////////////////


class CDIB;
class CDibDC;

class CDIB : public CGdiObject
{
	DECLARE_DYNAMIC(CDIB)

public:
	//由句柄创建位图
	static CDIB* PASCAL FromHandle(HBITMAP hBitmap);

// Constructors
	CDIB();

	//创建位图
	BOOL CreateDIB(int nWidth, int nHeight, UINT nBitcount, const void* lpBits=NULL);

	//创建位图
	BOOL CreateDIBIndirect(LPBITMAPINFO lpBitmap, const void* lpBits=NULL);

	//捕捉窗口图像
	BOOL CaptureDIB(CWnd * pWnd, const CRect& capRect = CRect(0,0,0,0));

// Attributes
	//得到位图
	operator HBITMAP() const;

	//拷贝位图
	CDIB& operator = (CDIB& copy);

// Operations
	//设置图像数据
	DWORD SetDIBBits(DWORD dwCount, const void* lpBits);
	//得到图像数据
	LPVOID GetDIBBits(DWORD dwCount = 0, LPVOID lpBits = NULL);

// Implementation
public:
	virtual ~CDIB();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

// Newly added functions
public:
	//得到使用的颜色数
	int GetColorUsed();

	//读入位图
	BOOL LoadBitmapFromFile(LPCTSTR filename);
	BOOL LoadBitmap(LPCTSTR lpszResourceName, LPCSTR lpType = RT_BITMAP);
	BOOL LoadBitmap(UINT uResourceID, LPCSTR lpType = RT_BITMAP);
	//保存位图
	int SaveBmp(LPCSTR filename);

	//设置调色板
	void SetPalette(UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors);
	//设置调色板
	void SetPalette(CPalette* pPal);
	
	//得到设备
	CDC* GetDC(void);
	//释放设备
	static BOOL ReleaseDC(CDC *pdc);

	//得到位图
	int GetBitmap(BITMAP* pBitMap);
	//得到DibSection
	int GetDibSection(DIBSECTION* pDibSection);
	
	//得到宽度
	int GetWidth();
	//得到高度
	int GetHeight();
	//得到尺寸
	SIZE GetSize();
	//得到每行图像字节数
	int GetWidthBytes();
	//得到图像位数
	int GetBitCount();
	//得到图像数据
	LPVOID GetBits();
	//得到图像信息头
	LPBITMAPINFO GetBitmapInfo(void);
private:
	
};

////////////////////////////////////////////////////////////////////
// inline functions

//////////////////////////////////////////////////////////////////
// 
// GetBitmap(BITMAP* pBitMap)
// 
// 完成功能:
//     得到位图
//
// 输入参数:
//	   位图指针 pBitMap
//
// 返回参数:
//	   是否成功
//
//////////////////////////////////////////////////////////////////

//得到位图
inline int CDIB::GetBitmap(BITMAP* pBitMap)
{
	return(::GetObject(m_hObject, sizeof(BITMAP), pBitMap));
}


//////////////////////////////////////////////////////////////////
// 
// GetDibSection(DIBSECTION *pDibSection)
// 
// 完成功能:
//     得到DibSection
//
// 输入参数:
//	   DibSection指针 pDibSection
//
// 返回参数:
//	   是否成功
//
//////////////////////////////////////////////////////////////////

//得到DibSection
inline int CDIB::GetDibSection(DIBSECTION *pDibSection)
{
	return(::GetObject(m_hObject, sizeof(DIBSECTION), pDibSection));
}


//////////////////////////////////////////////////////////////////
// 
// HBITMAP()
// 
// 完成功能:
//     得到位图句柄
//
// 输入参数:
//	   无
//
// 返回参数:
//	   位图句柄
//
//////////////////////////////////////////////////////////////////

//得到位图句柄
inline CDIB::operator HBITMAP() const
{
	return (HBITMAP)(this == NULL ? NULL : m_hObject);
}


//////////////////////////////////////////////////////////////////
// 
// FromHandle(HBITMAP hDib)
// 
// 完成功能:
//     从位图句柄得到位图类
//
// 输入参数:
//	   位图句柄 hDib
//
// 返回参数:
//	   位图类
//
//////////////////////////////////////////////////////////////////

//从位图句柄得到位图类
inline CDIB* PASCAL CDIB::FromHandle(HBITMAP hDib)
{
	return((CDIB*)CGdiObject::FromHandle(hDib));
}


//////////////////////////////////////////////////////////////////
// 
// GetWidth(void)
// 
// 完成功能:
//     得到宽度
//
// 输入参数:
//	   无
//
// 返回参数:
//	   宽度
//
//////////////////////////////////////////////////////////////////

//得到宽度
inline int CDIB::GetWidth(void)
{
	BITMAP bmp;
	GetBitmap(&bmp);
	return(bmp.bmWidth);
}


//////////////////////////////////////////////////////////////////
// 
// GetHeight(void)
// 
// 完成功能:
//     得到高度
//
// 输入参数:
//	   无
//
// 返回参数:
//	   高度
//
//////////////////////////////////////////////////////////////////

//得到高度
inline int CDIB::GetHeight(void)
{
	BITMAP bmp;
	GetBitmap(&bmp);
	return(bmp.bmHeight);
}


//////////////////////////////////////////////////////////////////
// 
// GetSize(void)
// 
// 完成功能:
//     得到尺寸
//
// 输入参数:
//	   无
//
// 返回参数:
//	   尺寸
//
//////////////////////////////////////////////////////////////////

//得到尺寸
inline SIZE CDIB::GetSize(void)
{
	BITMAP bmp;
	GetBitmap(&bmp);
	CSize size(bmp.bmWidth, bmp.bmHeight);
	return(size);
}


//////////////////////////////////////////////////////////////////
// 
// GetWidthBytes(void)
// 
// 完成功能:
//     得到每行字节数
//
// 输入参数:
//	   无
//
// 返回参数:
//	   每行字节数
//
//////////////////////////////////////////////////////////////////

//得到每行字节数
inline int CDIB::GetWidthBytes(void)
{
	BITMAP bmp;
	GetBitmap(&bmp);
	return(bmp.bmWidthBytes);
}


//////////////////////////////////////////////////////////////////
// 
// GetBitCount(void)
// 
// 完成功能:
//     得到图像位数
//
// 输入参数:
//	   无
//
// 返回参数:
//	   图像位数
//
//////////////////////////////////////////////////////////////////

//得到图像位数
inline int CDIB::GetBitCount(void)
{
	BITMAP bmp;
	GetBitmap(&bmp);
	return(bmp.bmBitsPixel);
}


//////////////////////////////////////////////////////////////////
// 
// GetBits(void)
// 
// 完成功能:
//     得到图像数据
//
// 输入参数:
//	   无
//
// 返回参数:
//	   图像数据
//
//////////////////////////////////////////////////////////////////

//得到图像数据
inline LPVOID CDIB::GetBits(void)
{
	BITMAP bmp;
	GetBitmap(&bmp);
	return(bmp.bmBits);
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// CDibDC
//
// 设备无关位图设备类(版本1.0)
//
// 完成功能:
//     与设备无关位图的相关联
//
// 版本所有: 罗隽,曾志
//
//////////////////////////////////////////////////////////////////////


class CDibDC : public CDC
{
	DECLARE_DYNAMIC(CDibDC)
	friend class CDIB;
// Constructors
public:
	CDibDC();

// Attributes
protected:
	HBITMAP m_hOld;

// Implementation
public:
	BOOL AlphaBlend(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest, 
		CDC *pSrcDC, int nXOriginSrc, int nYOriginSrc);//, int nWidthSrc, int nHeightSrc, BLENDFUNCTION blendFunction);
	CPalette * GetPalette();
	virtual ~CDibDC();
private:
	void AlphaBlendData(LPBYTE lpDestBuffer,/* LPBYTE lpAlphaBmp, */ LPBYTE lpScr,int nWidth,int nHeight);
	
};

#endif //__DIB_H