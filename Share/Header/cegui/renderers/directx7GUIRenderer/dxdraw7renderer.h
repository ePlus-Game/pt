//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/22/2007 16:19
//      File_base        : dxdraw7renderer
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _dxdraw7renderer_h_
#define _dxdraw7renderer_h_

#include "KWin32.h"
#include "KEngine.h"	
#include "iRepresentShell.h"
#include "KRepresentUnit.h"
#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"
#include <ddraw.h>
#include <list>
#include <set>


#ifdef DIRECTX7_GUIRENDERER_EXPORTS
#define DIRECTX7_GUIRENDERER_API __declspec(dllexport)
#else
#define DIRECTX7_GUIRENDERER_API __declspec(dllimport)
#endif


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

class DIRECTX7_GUIRENDERER_API DirectX7Renderer : public Renderer
{
	struct QuadInfo
	{
		Texture		*texture;
		Rect		position;
		Rect		clipper;
		float		z;
		Rect		texPosition;
		ColourRect	rectColour;
		KSprite*	pSpr;	
		int			scalex;
		int			scaley;
		QuadSplitMode	splitMode;
		float		alpha;
		int			frame;
		bool operator<(const QuadInfo& other) const
		{
			return z > other.z;
		}
	};

public:
	DirectX7Renderer(iRepresentShell* representshell);
	virtual ~DirectX7Renderer(void);

public:
	virtual	void		addQuad(const Rect& dest_rect, const Rect& clipper, float z, const Texture* tex, const Rect& texture_rect, const ColourRect& colours, const ColourRect& bordercolours, bool border, QuadSplitMode quad_split_mode, int frame);
	virtual	void		doRender(void);
	virtual	void		clearRenderList(void);
	virtual void		setQueueingEnabled(bool setting)	{d_queueing = setting;}
	virtual	Texture*	createTexture(void);
	virtual	Texture*	createTexture(const String& filename, const String& resourceGroup);
	virtual	Texture*	createTexture(float size);
	virtual Texture*    createTextureFromMemory( void* pBuffer );
	virtual	void		destroyTexture(Texture* texture);
	virtual void		destroyAllTextures(void);
	virtual bool		isQueueingEnabled(void) const	{return d_queueing;}	
	virtual float		getWidth(void) const	{return d_display_area.getWidth();}
	virtual float		getHeight(void) const	{return d_display_area.getHeight();}
	virtual Size		getSize(void) const	{return d_display_area.getSize();}
	virtual Rect		getRect(void) const	{return d_display_area;}
	virtual	uint		getMaxTextureSize(void) const	{return d_maxTextureSize;}
	virtual	uint		getHorzScreenDPI(void) const	{return 96;}
	virtual	uint		getVertScreenDPI(void) const	{return 96;}	
	void				renderQuadDirect(RenderData* renderData);
	void				renderDirect(const Point& pos, const Texture* tex, int alpha );

private:
	void				sortQuads(void);
	Size				getViewportSize(void);
	void				constructor_impl(iRepresentShell *representshell, const Size& display_size);
	iRepresentShell*	getDevice(void) const	{return d_representShell;}
	void				setDisplaySize(const Size& sz);
	

private:
	Rect				d_display_area;
	typedef std::multiset<QuadInfo>		QuadList;
	QuadList			d_quadlist;
	bool				d_queueing;		//!< setting for queueing control.
	std::list<Texture*>	d_texturelist;		//!< List used to track textures.
	uint				d_maxTextureSize;		//!< Holds maximum supported texture size (in pixels).
	iRepresentShell*	d_representShell; 
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _d3d9renderer_h_
