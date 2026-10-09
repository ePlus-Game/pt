//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/21/2007 21:31
//      File_base        : CEGUIRenderCache
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KRenderCache.h"
#include "CEGUISystem.h"
#include "CEGUIRenderer.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    KRenderCache::KRenderCache()
    {
		d_isDrawingPanel = true;
		d_renderer		= NULL;
		d_cachedImages	= NULL;
	}

    KRenderCache::~KRenderCache()
    {
		if(d_cachedImages !=  NULL && d_renderer != NULL)
		{
			d_renderer->destroyTexture( d_cachedImages );
		}		
	}

	void KRenderCache::release()
	{
		if(d_cachedImages !=  NULL && d_renderer != NULL)
        {
            d_renderer->destroyTexture(d_cachedImages);
            d_cachedImages = NULL;
        }
	}

	void KRenderCache::createRenderCache( int width, int height, int pitch/*=2*/)
	{
		d_renderer = System::getSingleton().getRenderer();
		if ( d_renderer )
		{
			if(d_cachedImages)
				d_renderer->destroyTexture(d_cachedImages);
			d_cachedImages	= d_renderer->createTexture();
			if ( d_cachedImages)
			{
				if(pitch == TEXTURE_PITCH_TYP_A8RGB565)
				    d_cachedImages->createTexture( width, height, pitch, argb1555_texture );
				else
				if(pitch == TEXTURE_PICTH_TYP_BLT_RGB565)
					d_cachedImages->createTexture(width,height,pitch,argb565_texture_dxSurface);
				else
				if(pitch == TEXTURE_PICTH_TYP_BLT_PLAYERHEADINFO)
					d_cachedImages->createTexture(width,height,pitch,argb565_texture_dxFontSurface);
			}
		}
	}

    bool KRenderCache::hasCachedImagery() const
    {
        return d_cachedImages ? true : false;
    }

	void KRenderCache::DestroyTexture()
	{
		if(d_cachedImages && d_renderer != NULL)
		{
			d_renderer->destroyTexture(d_cachedImages);
			d_cachedImages = NULL;
		}
	}
    bool KRenderCache::hasCachedText() const
    {
        return false;//d_cachedTexts ? true : false;
    }

    void KRenderCache::render(const Point& pos, int alpha) const
    {
		if ( d_renderer && d_cachedImages )
		{
			d_renderer->renderDirect( pos, d_cachedImages, alpha );
		}
    }

    void KRenderCache::clearCached( void )
    {
		if ( d_renderer && d_cachedImages )
		{
			if(d_cachedImages->getType() == argb565_texture_dxFontSurface||
				d_cachedImages->getType() == argb565_texture_dxSurface)
			{
				d_cachedImages->clearTexture(0xff);
			}
			else if(d_cachedImages->getPitch() == 3)
			{
				d_cachedImages->clearTexture(0x00);
			}
		}
		d_isDrawingPanel = true;
    }

	//这个函数最后计算出在spr上截取的范围，以及需要拷贝到目标的位置，并保证spr上的裁剪不会超出范围，但不保证不会超出目标区域的裁剪范围
    void KRenderCache::cacheImage(const Image* srcImage, const Point& destPos, const Rect& srcClipper, int frameIdx)
    {	
		//健壮性检查
		if(d_renderer == NULL || d_cachedImages == NULL)
		{
			assert(0);
			return;
		}

		//原图是否存在
		if(srcImage == NULL)
		{
			return;
		}
		
		Imageset* is = (Imageset*)srcImage->getImageset();
		if ( is == NULL)
		{
			return;
		}

		

		//image在spr中的位置和大小
		const Rect& imageArea = srcImage->getSourceTextureArea();
		//image被帖到BUFF上的位置
		TexturePosition pos(
			srcClipper.getPosition().d_x > destPos.d_x ? srcClipper.getPosition().d_x : destPos.d_x, 
			srcClipper.getPosition().d_y > destPos.d_y ? srcClipper.getPosition().d_y : destPos.d_y);
		//目标裁剪区域
		Rect imageClipper(srcClipper);

		//以下计算实际需要在spr上截取的image范围
		imageClipper.offset(Point(-destPos.d_x, -destPos.d_y)).offset(imageArea.getPosition());
		Rect finalClipper = imageArea.getIntersection(imageClipper);
		
		TextureRect finalImageArea(finalClipper.d_left, finalClipper.d_top, 
			finalClipper.d_right - finalClipper.d_left, finalClipper.d_bottom - finalClipper.d_top);

		if( is->isSpr( ) )
		{
			KSprite* spr = (KSprite*)is->getSpr();
			if ( spr == NULL)
			{
				return ;
			}
			//得到该SPR的大小
			SPRFRAME* frameInfo = spr->GetFrameInfo( frameIdx );
			if ( frameInfo == NULL)
			{
				return;
			}
			TextureSize	imageSetSize(frameInfo->Width, frameInfo->Height);
			
			void*  palette = spr->Get24Palette();
			
			if(d_cachedImages->getType() == argb565_texture_dxSurface||
				d_cachedImages->getType() == argb565_texture_dxFontSurface)
			{
				if(spr->Is16Bit())
					d_cachedImages->copySprite16BitToBitmap16BitRGB565(pos,imageSetSize,finalImageArea,frameInfo->Sprite);
				else
					d_cachedImages->copySprite8BitToBitmap16BitRGB565(pos,imageSetSize,finalImageArea,frameInfo->Sprite,palette);
				
			}
			else if(d_cachedImages->getPitch() == 3)
			{
				if(spr->Is16Bit())
				{
					d_cachedImages->copySprite16To24BufWithAlpha(pos, imageSetSize, finalImageArea, frameInfo->Sprite);
				}
				else
				{
					d_cachedImages->copySpriteTo24BufWithAlpha(pos, imageSetSize, finalImageArea, frameInfo->Sprite, palette);
				}
			}
		}
		else
		{
			Bitmap* pBitmap = ( Bitmap* )is->getBitmap( );
			if( pBitmap == NULL )
			{
				return ;
			}
			
			TextureSize	imageSetSize( pBitmap->GetWidth( ), pBitmap->GetHeight( ) );
			
			void* palette = ( void* )pBitmap->GetPal32( );
			
			void* pBuffer = ( void * )pBitmap->GetBuffer( );
			
			
			switch( pBitmap->GetBitmapType( ) )
			{
			case BITMAP_TYPE_R5G6B5:
				{
					if(	d_cachedImages->getType() == argb565_texture_dxSurface ||
						d_cachedImages->getType() == argb565_texture_dxFontSurface )
					{
						d_cachedImages->copyBitmap16Bit565ToBitmap16Bit565(
							pos,
							imageSetSize,
							finalImageArea,
							pBuffer );
					}
					else
					if( d_cachedImages->getPitch( ) == 3 )	
					{
						d_cachedImages->copyBitmap16Bit565ToBitmap24Bit8565(
							pos,
							imageSetSize,
							finalImageArea,
							pBuffer );
					}
					return;
				}
			default:
				{
					return;
				}
				
			}
			
			return;
		}

    }
	
	void KRenderCache::cacheText(const String& text, const Rect& destRect, 
		const Rect& txtClipper, const Font* font, TextFormatting format, argb_t colours, bool underLine)
	{
		size_t thisCount;
		size_t lineCount = 0;

		float	y_base = destRect.d_top + font->getBaseline(1);


		size_t lineStart = 0, lineEnd = 0;
		String	currLine;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
				lineEnd = text.length();

			currLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;	// +1 to skip \n char

			switch(format)
			{
			case LeftAligned:
				cacheTextLine(currLine, Point(destRect.d_left, y_base), txtClipper, font, colours, colours);
				thisCount = 1;
				y_base += font->getLineSpacing(1);
				break;

			case RightAligned:
				cacheTextLine(currLine, Point(destRect.d_right - ((Font*)font)->getTextExtent(currLine, 1), y_base), txtClipper, font, colours, colours);
				thisCount = 1;
				y_base += font->getLineSpacing(1);
				break;

			case Centred:
				cacheTextLine(currLine, Point(PixelAligned(destRect.d_left + ((destRect.getWidth() - ((Font*)font)->getTextExtent(currLine, 1)) / 2.0f)), y_base), txtClipper, font, colours, colours);
				y_base += font->getLineSpacing(1);
				break;
			}

			lineCount += thisCount;
		}
	}

// 	void KRenderCache::cacheTextLine(const String& text, const Point& destPos, 
// 		const Rect& txtClipper, const Font* font, argb_t colours,  argb_t borderColours)
// 	{
// 		const FontGlyph* glyph = NULL;
// 		//float base_y = destRect.d_top;
// 
// 		TexturePosition cur_pos(destPos.d_x, destPos.d_y);
// 
// 		for (size_t c = 0; c < text.length(); ++c)
// 		{
// 			glyph = ((Font*)font)->getGlyphData(text[c]);
// 			if (glyph)
// 			{
// 				const Image* img = glyph->getImage();				
// 				if (img)
// 				{
// 					const Imageset* imgSet = img->getImageset();
// 					if (imgSet)
// 					{
// 						Texture* tex = imgSet->getTexture();
// 						if ( tex )
// 						{
// 							
// 							TextureSize	textureSize(tex->getWidth(), tex->getHeight());
// 							Size glyphSize = glyph->getSize(1, 1);
// 							const Rect& rt = img->getSourceTextureArea();
// 							int imageClipperW = glyphSize.d_width < txtClipper.getWidth()? glyphSize.d_width : txtClipper.getWidth();
// 							int imageClipperH = glyphSize.d_height < txtClipper.getHeight()? glyphSize.d_height : txtClipper.getHeight();
// 							TextureRect imageClipper( rt.d_left, rt.d_top, imageClipperW, imageClipperH);
// 
// 							if ( font->isHaveBorder() )
// 							{
// 								d_cachedImages->copyFontTo32BufBorder( TexturePosition(cur_pos.d_x  + img->getOffsetX(), cur_pos.d_y + img->getOffsetY()), textureSize, imageClipper, TextureFont( tex->getBuffer(), colours, borderColours,255) );
// 							}
// 							else
// 							{
// 								d_cachedImages->copyFontTo32Buf( TexturePosition(cur_pos.d_x  + img->getOffsetX(), cur_pos.d_y + img->getOffsetY()), textureSize, imageClipper, TextureFont( tex->getBuffer(), colours, borderColours,255) );
// 							}
// 							cur_pos.d_x += glyph->getAdvance();
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}zz

	void KRenderCache::cacheTextLine(const String& text, const Point& destPos, 
		const Rect& txtClipper, const Font* font, argb_t colours,  argb_t borderColours,  bool underLine)
	{
		const FontGlyph* glyph = NULL;
		//float base_y = destRect.d_top;

		TexturePosition cur_pos(destPos.d_x, destPos.d_y);

		for (size_t c = 0; c < text.length(); ++c)
		{
			glyph = ((Font*)font)->getGlyphData(text[c]);
			if(glyph == NULL)
			{
				continue;
			}
			
			const Image* img = glyph->getImage();				
			if(img == NULL)
			{
				continue;
			}

			const Imageset* imgSet = img->getImageset();
			if(imgSet == NULL)
			{
				continue;
			}
			Texture* tex = imgSet->getTexture();
			if(tex == NULL)
			{
				continue;
			}
			
			Size glyphSize = glyph->getSize(1, 1);
			//得到该文字在整张imgset中的位置和大小
			const Rect& imageArea = img->getSourceTextureArea();
			
			//在buffer上的显示位置
			TexturePosition destPos(cur_pos.d_x + img->getOffsetX(), cur_pos.d_y + img->getOffsetY());
			
			//目标裁剪区域
			Rect imageClipper(txtClipper);

			//以下计算实际需要在imgset上截取的image范围
			imageClipper.offset(Point(-destPos.d_x, -destPos.d_y)).offset(imageArea.getPosition());
			Rect finalClipper = imageArea.getIntersection(imageClipper);
			
			TextureRect finalImageArea(finalClipper.d_left, finalClipper.d_top, 
			finalClipper.d_right - finalClipper.d_left, finalClipper.d_bottom - finalClipper.d_top);

			//得到本张IMG的大小
			
			if(destPos.d_x < txtClipper.getPosition().d_x)
			{
				destPos.d_x = txtClipper.getPosition().d_x;
			}
			if(destPos.d_y < txtClipper.getPosition().d_y)
			{
				destPos.d_y = txtClipper.getPosition().d_y;
			}
			TextureSize	imgSetSize(tex->getWidth(), tex->getHeight());
			
			if(d_cachedImages->getType() == argb565_texture_dxSurface)
				d_cachedImages->copyFont16ToBitmap16BitRGB565(destPos,imgSetSize,
					finalImageArea,TextureFont(tex->getBuffer(),colours,borderColours,255), underLine );
			else
			if(d_cachedImages->getType() == argb565_texture_dxFontSurface)
				d_cachedImages->copyFont16ToBitmap16BitRGB565WithNoAlpha(destPos,imgSetSize,
					finalImageArea,TextureFont(tex->getBuffer(),colours,borderColours,255), underLine );
	/*		if(d_cachedImages->getPitch() == 2)
			{
				if ( font->isHaveBorder() )
				{
					d_cachedImages->copyFontTo32BufBorder( 
						destPos, imgSetSize, finalImageArea, 
						TextureFont( tex->getBuffer(), colours, borderColours,255) );
				}
				else
				{
					d_cachedImages->copyFontTo32Buf( 
						destPos, imgSetSize, finalImageArea, 
						TextureFont( tex->getBuffer(), colours, borderColours,255) );
				}
			}*/
			else if(d_cachedImages->getPitch() == 3)
			{
 				d_cachedImages->copyFontTo24BufWithAlpha( 
 					destPos, imgSetSize, finalImageArea, 
 					TextureFont( tex->getBuffer(), colours, borderColours,255), underLine  );
			}
			cur_pos.d_x += glyph->getAdvance();
		}
	}

	void KRenderCache::cacheTextLine(const wchar_t* text, const Point& destPos, 
		const Rect& txtClipper, const Font* font, argb_t colours,  argb_t borderColours, int wordExtSpace, bool underLine)
	{
		const FontGlyph* glyph = NULL;

		TexturePosition cur_pos(destPos.d_x, destPos.d_y);

		int textLen = wcslen(text);
		for (size_t c = 0; c < textLen; ++c)
		{
			glyph = ((Font*)font)->getGlyphData(text[c]);
			if(glyph == NULL)
			{
				continue;
			}
			
			const Image* img = glyph->getImage();				
			if(img == NULL)
			{
				continue;
			}

			const Imageset* imgSet = img->getImageset();
			if(imgSet == NULL)
			{
				continue;
			}
			Texture* tex = imgSet->getTexture();
			if(tex == NULL)
			{
				continue;
			}
			
			Size glyphSize = glyph->getSize(1, 1);
			//得到该文字在整张imgset中的位置和大小
			const Rect& imageArea = img->getSourceTextureArea();
			
			//在buffer上的显示位置
			TexturePosition destPos(cur_pos.d_x + img->getOffsetX(), cur_pos.d_y + img->getOffsetY());
			
			//目标裁剪区域
			Rect imageClipper(txtClipper);

			//以下计算实际需要在imgset上截取的image范围
			imageClipper.offset(Point(-destPos.d_x, -destPos.d_y)).offset(imageArea.getPosition());
			Rect finalClipper = imageArea.getIntersection(imageClipper);
			
			TextureRect finalImageArea(finalClipper.d_left, finalClipper.d_top, 
			finalClipper.d_right - finalClipper.d_left, finalClipper.d_bottom - finalClipper.d_top);

			//得到本张IMG的大小
			TextureSize	imgSetSize(tex->getWidth(), tex->getHeight());

			if(d_cachedImages->getType() == argb565_texture_dxSurface)
			{
				d_cachedImages->copyFont16ToBitmap16BitRGB565(destPos,imgSetSize,finalImageArea,TextureFont(tex->getBuffer(),colours,borderColours,255), underLine );
			}
			else
			if(d_cachedImages->getType() == argb565_texture_dxFontSurface)
				d_cachedImages->copyFont16ToBitmap16BitRGB565WithNoAlpha(destPos,imgSetSize,
					finalImageArea,TextureFont(tex->getBuffer(),colours,borderColours,255), underLine );
	/*		if(d_cachedImages->getPitch() == 2)
			{
				if ( font->isHaveBorder() )
				{
					d_cachedImages->copyFontTo32BufBorder( 
						destPos, imgSetSize, finalImageArea, 
						TextureFont( tex->getBuffer(), colours, borderColours,255) );
				}
				else
				{
					d_cachedImages->copyFontTo32Buf( 
						destPos, imgSetSize, finalImageArea, 
						TextureFont( tex->getBuffer(), colours, borderColours,255) );
				}
			}*/
			else
			{
				if(d_cachedImages->getPitch() == 3)
  				d_cachedImages->copyFontTo24BufWithAlpha( 
  					destPos, imgSetSize, finalImageArea, 
  					TextureFont( tex->getBuffer(), colours, borderColours,255), underLine  );
			}
			int adwidth = glyph->getAdvance();
			int width = glyph->getRenderedAdvance(1.0f);
			cur_pos.d_x += (adwidth > width ? adwidth : width) + wordExtSpace;
		}
	}
} // End of  CEGUI namespace section
