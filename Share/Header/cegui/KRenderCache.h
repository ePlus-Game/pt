//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/21/2007 21:30
//      File_base        : CEGUIRenderCache
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _KRenderCache_h_
#define _KRenderCache_h_

#include <vector>
#include "CEGUITexture.h"
#include "CEGUIFont.h"
#include "CEGUIRect.h"
#include "CEGUIVector.h"
#include "CEGUIImage.h"
#include "CEGUIImageset.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
class CEGUIEXPORT KRenderCache
{
public:
    KRenderCache();
    ~KRenderCache();

public:
	void		release();
	void		createRenderCache(int width, int height, int pitch = 2);
    bool		hasCachedImagery(void) const;
	bool		hasCachedText(void) const;
	void        DestroyTexture();
    void		render(const Point& pos, int alpha = 255) const;
    void		clearCached(void);
    void		cacheImage(const Image* srcImage, const Point& destPos, const Rect& srcClipper, int frameIdx = 0);
    void		cacheText(const String& text, const Rect& destRect, 
		const Rect& txtClipper, const Font* font, TextFormatting format, argb_t colours,  bool underLine = false);

	//输入unicode双字节编码的字符（目前只有排版需要调这个接口）
	void		cacheTextLine(const wchar_t* text, const Point& destPos, 
		const Rect& txtClipper, const Font* font, argb_t colours, argb_t borderColours = 0xFFFFFFFF, int wordExtSpace = 0,  bool underLine = false);

	//add
	Renderer*   getRenderer() const {return d_renderer;}
	Texture*    getTexture() const {return d_cachedImages;}
private:
	void		cacheTextLine(const String& text, const Point& destPos, 
		const Rect& txtClipper, const Font* font, argb_t colours, argb_t borderColours = 0xFFFFFFFF,  bool underLine = false);


private:
	Renderer*	d_renderer;
	Texture*	d_cachedImages;
	bool		d_isDrawingPanel;
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif


#endif  // end of guard _CEGUIRenderCache_h_
