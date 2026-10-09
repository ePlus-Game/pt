/***********************************************************************
    filename:   FreeTypeFont.h
    created:    24/07/2007
    author:     LiuSiliang

    purpose:    Class FreeTypeFont
*************************************************************************/

#include "FreeTypeFont.h"
#include <math.h>
#include <stdio.h>

extern  FT_Library	g_FTlib;

static int			ft_usage_count = 0;

FreeTypeFont::FreeTypeFont( void ):Font()
,d_ptSize( 0 )
,d_antiAliased( true )
,d_fontFace( 0 )
,d_angle( 0 )
,d_xTrans( 0 )
,d_yTrans( 0 )
,d_buffSize(0)
,d_fontBuff(0)
,d_xScalc(1)
,d_yScalc(1)
{

}

FreeTypeFont::FreeTypeFont(
				const std::string& name, 
				const std::string& filename)
:Font( name, filename )
,d_ptSize( 0 )
,d_antiAliased( true )
,d_fontFace( 0 )
,d_angle( 0 )
,d_xTrans( 0 )
,d_yTrans( 0 )
,d_buffSize(0)
,d_fontBuff(0)
,d_xScalc(1)
,d_yScalc(1)
{
}


FreeTypeFont::FreeTypeFont( const FreeTypeFont& rFnt )
{
	d_fontFace = NULL;
	d_angle = rFnt.d_angle;
	d_xTrans = rFnt.d_xTrans;
	d_yTrans = rFnt.d_yTrans;
	d_ptSize = rFnt.d_ptSize;
	d_antiAliased = rFnt.d_antiAliased;
	d_buffSize = 0;
	d_fontBuff = 0;
	d_xScalc = rFnt.d_xScalc;
	d_yScalc = rFnt.d_yScalc;
}

FreeTypeFont::~FreeTypeFont ()
{
    free();
}

void FreeTypeFont::free ()
{
	if ( d_fontBuff )
	{
		delete[] d_fontBuff;
		d_fontBuff = NULL;
		d_buffSize = 0;
	}


    if (!d_fontFace)
        return;

    FT_Done_Face (d_fontFace);
    d_fontFace = 0;
	
}

void FreeTypeFont::load( void )
{
	free();

	FILE* fp = ::fopen( d_fileName.c_str(), "rb" );
	if ( !fp )
	{
		return;
	}

	::fseek(fp, 0, 2);
	d_buffSize = ::ftell(fp);
	::fseek(fp, 0, 0);
	d_fontBuff = new unsigned char[d_buffSize];
	if ( d_fontBuff == NULL )
	{
		::fclose( fp );
		return;
	}
	
	int err = ::fread( d_fontBuff, sizeof(unsigned char),d_buffSize, fp );
	if ( !err )
	{
		::fclose( fp );
		return;
	}

	::fclose( fp );

	if (FT_New_Memory_Face(g_FTlib, d_fontBuff, d_buffSize, 0, &d_fontFace) != 0)
		return;

	if ( !d_fontFace->charmap )
	{
		FT_Select_Charmap( d_fontFace, FT_ENCODING_MS_WANSUNG );
	}//*/

	if ( !d_fontFace->charmap )
	{
		FT_Select_Charmap( d_fontFace,  FT_ENCODING_APPLE_ROMAN );
	}
	
    if (!d_fontFace->charmap)
    {
        FT_Done_Face (d_fontFace);
        d_fontFace = 0;
	}
}

int FreeTypeFont::getFontPixel()
{
    uint horzdpi = 96;
    uint vertdpi = 96;

	d_pixSize = fnt_max((int)ceil(d_ptSize * FT_POS_COEF * horzdpi), (int)ceil(d_ptSize * FT_POS_COEF * vertdpi));

	d_pixSize = d_pixSize < MAX_FONT_PIXEL ? d_pixSize : MAX_FONT_PIXEL;

	return d_pixSize;
}

void FreeTypeFont::updateFont ( utf32 codepoint )
{
    d_cp_map.clear ();

	if ( d_fontFace == NULL )
	{
		return;
	}

    uint horzdpi = 96;
    uint vertdpi = 96;

    float hps = d_ptSize * 64;
    float vps = d_ptSize * 64;
    if (d_autoScale)
    {
        hps *= d_horzScaling;
        vps *= d_vertScaling;
    }

    if (FT_Set_Char_Size (d_fontFace, FT_F26Dot6 (hps), FT_F26Dot6 (vps), horzdpi, vertdpi))
    {
        float ptSize_72 = (d_ptSize * 72.0f) / vertdpi;
        float best_delta = 99999;
        float best_size = 0;
        for (int i = 0; i < d_fontFace->num_fixed_sizes; i++)
        {
            float size = d_fontFace->available_sizes [i].size * float(FT_POS_COEF);
            float delta = fabs (size - ptSize_72);
            if (delta < best_delta)
            {
                best_delta = delta;
                best_size = size;
            }
        }

        if ((best_size <= 0) ||
            FT_Set_Char_Size (d_fontFace, 0, FT_F26Dot6 (best_size * 64), 0, 0))
        {
            return;
        }
    }

    if (d_fontFace->face_flags & FT_FACE_FLAG_SCALABLE)
    {
		float y_scale = d_fontFace->size->metrics.y_scale * float(FT_POS_COEF) * (1.0f/65536.0f);
		d_ascender = d_fontFace->ascender * y_scale;
		d_descender = d_fontFace->descender * y_scale;
		d_height = d_fontFace->height * y_scale;
    }
    else
 	{
			d_ascender = d_fontFace->size->metrics.ascender * float(FT_POS_COEF);
			d_descender = d_fontFace->size->metrics.descender * float(FT_POS_COEF);
			d_height = d_fontFace->size->metrics.height * float(FT_POS_COEF);
    }

    if (codepoint)
    {
        if (FT_Load_Char (d_fontFace, codepoint, FT_LOAD_DEFAULT))
            return;

        float adv = d_fontFace->glyph->metrics.horiAdvance * float(FT_POS_COEF);

		d_cp_map[codepoint] = NULL;
    }
	d_pixSize = fnt_max((int)ceil(d_ptSize * FT_POS_COEF * horzdpi), (int)ceil(d_ptSize * FT_POS_COEF * vertdpi));

	d_pixSize = d_pixSize < MAX_FONT_PIXEL ? d_pixSize : MAX_FONT_PIXEL;
}

void FreeTypeFont::rasterize(utf32 codepoint)
{

	
	static const FT_Matrix identityMat = {(1 << 16), 0, 0, (1 << 16)};	

	updateFont(codepoint );

	if ( d_fontFace == NULL )
	{
		return;
	}

	CodepointMap::const_iterator s = d_cp_map.lower_bound (codepoint);
	if (s == d_cp_map.end ())
		return;

	int nCount = 1;

	if (nCount > 0)
	{
		uint x = INTER_GLYPH_PAD_SPACE;
		uint y = INTER_GLYPH_PAD_SPACE;
		uint yb = INTER_GLYPH_PAD_SPACE;

		int xxx = 0;
		int yyy = 0;

		FT_Matrix mat = {(1 << 16), 0, 0, (1 << 16)};

        FT_Matrix matrix_scale;
		
		if ( d_xScalc > 1 &&  d_xScalc <= 200)
		{
			matrix_scale.xx = (d_xScalc  * (1 << 16)) / 100.0f;
			matrix_scale.xy = 0;
			matrix_scale.yx = 0;
			matrix_scale.yy = (1 << 16);
			FT_Matrix_Multiply(&matrix_scale, &mat);
		}

		if ( d_yScalc > 1 &&  d_yScalc <= 200 )
		{
			matrix_scale.xx = (1 << 16);
			matrix_scale.xy = 0;
			matrix_scale.yx = 0;
			matrix_scale.yy = (d_yScalc  * (1 << 16))/ 100.0f;
			FT_Matrix_Multiply(&matrix_scale, &mat);
		}

		FT_Matrix matrix_move = {0, 0, 0, 0};

		if (d_xTrans >= -d_ptSize && d_xTrans <= d_ptSize && d_ptSize > 0 )
		{
			matrix_move.xx = 1 << 16; 
			matrix_move.xy = (d_xTrans * ( 1 << 16 ) )/ d_ptSize ; 
			matrix_move.yy = 1 << 16;
			FT_Matrix_Multiply(  &matrix_move, &mat );
		}

		if (d_xTrans >= -d_ptSize && d_yTrans <= d_ptSize && d_ptSize > 0 )
		{
			matrix_move.xx = 1 << 16; 
			matrix_move.yx = (d_yTrans * ( 1 << 16 ) )  / d_ptSize; 
			matrix_move.yy = 1 << 16;
			FT_Matrix_Multiply(  &matrix_move, &mat );
		}
		
		FT_Matrix matrix_trans = {(1 << 16), 0, 0, (1 << 16)};
		double angle = d_angle * FT_ANGLE_PI / 180;

		matrix_trans.xx = FT_Cos( angle );
		matrix_trans.xy = -FT_Sin( angle );
		matrix_trans.yx = FT_Sin( angle );
		matrix_trans.yy = FT_Cos( angle );


		FT_Matrix_Multiply(  &matrix_trans, &mat );

		FT_Set_Transform( d_fontFace, &mat, NULL ); 

        if (FT_Load_Char (d_fontFace, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                                  (d_antiAliased ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO)) == 0)
		{
			d_cp_map[codepoint] = &d_fontFace->glyph->bitmap;
		}
	}
}

FT_Bitmap* FreeTypeFont::drawText( utf32 outChar, int size, int angle, int xTrans, int yTrans, int xScalc, int yScalc )
{
	d_ptSize	= size;
	if ( d_ptSize <= 0 || d_ptSize > 40 )
	{
		d_ptSize = 20;
	}
	d_angle		= angle;
	if ( d_angle <= - 360 || d_angle >= 360 )
	{
		d_angle = 0;
	}
	d_xTrans	= xTrans;
	d_yTrans	= yTrans;
	d_xScalc	= xScalc;
	d_yScalc	= yScalc;

	FT_Bitmap* glyph = NULL;

	glyph = (FT_Bitmap*)getGlyphData(outChar);
	if(glyph == NULL)
	{
		return NULL;
	}

	return glyph;
}