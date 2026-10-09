/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutRender.h
//  Creator     :   xiehong
//  Date        :   2006-15-18 16:00
//  Comment     :   element declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUT_RENDER_
#define _LAYOUT_RENDER_

#include "layoutinterface.h"
#include "CEGUI.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Class Define
//
/////////////////////////////////////////////////////////////////////////////
using namespace CEGUI;
class LayoutRender : public ILayoutRender
{
	KRenderCache* _drawPanel;
protected:
	Font* getCEFont(const LOFont& font);
	
	const Image* getCEImage(const unsigned short* imageName);

public:
	LayoutRender();

	~LayoutRender();

	void setDrawPanel(KRenderCache* drawPanel);

	void Release();
	
	virtual LORect	getWordSize(
		unsigned short codePoint,
		const LOFont& font);
	
	virtual int		getLineHeight(
		const LOFont& font);

	virtual int		getTextExtent(
		const unsigned short* text,
		const LOFont& font);

	//得到一个字符串上指定坐标的字符
	virtual int		getCharAtPixel(
		const unsigned short* text, 
		int startCharIndex,  
		int pixel,
		const LOFont& font);

	virtual void	drawText(
		const unsigned short* text,
		const LOFont& font,
		const LORect& destArea,
		const LOColor& color,
		float alpha,
		const LORect& clipper,
		float zPos,
		int borderMode,
		bool underLine);

	
	virtual LORect	getImageArea(
		const unsigned short* imageName);

	
	virtual LOImageInfo getImageInfo(
		const unsigned short* imageName);

	//在指定位置画一幅图片
	virtual void	drawImage(
		const unsigned short* imageName, 
		LORect& destArea,
		const LOColor& color, 
		float alpha,
		LOImageInfo& imageInfo, 
		LORect& clipper, 
		float zPos);
};

#endif