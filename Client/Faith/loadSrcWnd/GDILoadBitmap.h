#ifndef _GDI_LOAD_BITMAP_H
#define _GDI_LOAD_BITMAP_H

HBITMAP   LoadBitmapFromFile(const char* fileName);
HBITMAP   LoadBitmap32FromFile(const char* fileName);
BOOL  Flip_Bitmap(UCHAR* bitmap_buffer,
		          int bytes_per_line,int height);
typedef class GDILoadBitmap
{
public:
	GDILoadBitmap();
	~GDILoadBitmap();
public:
	BOOL GDIBitmapFromFile(const char* fileName);
	const BITMAP& GDIBitmapGetBitmapInfo() const;
	const LPVOID   GDIBitmapGetBItmapBuffer()const;
	void           GDIBitmapCreate(int width,int height,int colorBpp);
	void GDIBitmapDestroy();
protected:
	BITMAP bitmapInfo;
	LPVOID pBitmapBuffer;
}GDIBITMAP,*LPGDIBITMAP;
#endif