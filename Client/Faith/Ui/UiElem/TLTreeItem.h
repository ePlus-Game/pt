/************************************************************************
    filename:   FalTreeItem.h
    created:	 10/17/2004
    author:		 David Durant (based on code by Paul D Turner)
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#ifndef _FalTreeItem_h_
#define _FalTreeItem_h_

#include "TLModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUITreeItem.h"
#include "CoreUseNameDef.h"
// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        TreeItem class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - EnabledNormal
            - EnabledHover
            - EnabledPushed
            - EnabledPopupOpen
            - DisabledNormal
            - DisabledHover
            - DisabledPushed
            - DisabledPopupOpen
            - PopupClosedIcon   - Additional state drawn when item has a pop-up attached (in closed state)
            - PopupOpenIcon     - Additional state drawn when item has a pop-up attached (in open state)
    */
    
	class TLTree;
	class TAHAREZLOOK_API TLTreeItem : public TreeItem
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

		bool isLeaf;
        /*!
        \brief
            Constructor
        */
		TLTreeItem(const String& text, uint item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = true)
		:TreeItem(text, item_id , item_data , disabled , auto_delete)
		{
			d_isPopup   = false;
			d_PopupTree = NULL;
			d_canDrag   = false;
		}

        /*!
        \brief
            Destructor
        */
        ~TLTreeItem(){};

	   /*!
	   \brief
		  Set TreeItem additional index
	   */
	   void	  setItemAdIndex( const uint itemIndex ){ d_itemIndex = itemIndex; }


	   /*!
	   \brief
		  Get TreeItem  additional index
	   */
	   uint	  getItemAdIndex( void ){ return d_itemIndex; }
	   

	   virtual void draw(RenderCache &cache, const Rect &targetRect, float zBase, float alpha, const Rect *clipper, Image *pImage) 
	   {
   			setSelectionBrushImage(pImage);
			TreeItem::draw(cache, targetRect, zBase, alpha, clipper);
	   }

	   virtual void drawSelf(KRenderCache* panelCache, const Point& position, const Rect& clipper, bool hover)
	   {
			TreeItem::drawSelf(panelCache, position, clipper, hover);

			//caol+ 绘制Item名字后缀
			const Font* textFont = getFont();
			if( NULL == textFont)
			{
				return;
			}

			Point pos(position);
			Rect finalArea(pos, clipper.getSize());
			panelCache->cacheText(d_tailText, finalArea, clipper, textFont, RightAligned, d_tailTextColor);
	   }

	   ColourRect getTLModulateAlphaColourRect(const ColourRect& cols, float alpha) const
	   {
		   return getModulateAlphaColourRect(cols, alpha) ;
	   }

	   const Image* getItemHeightLight() const
	   {
		   return d_pushedImage;
	   }

	   ColourRect getSelectCols() const
	   {
		   return d_selectCols;
	   }


	   bool       isPopup() const 
	   {
		   return  d_isPopup;
	   }

	   void       setPopup(const bool bPopup,TLTree * pPopoutTreeWindow,const int nAdditionalParam)
	   {
		   d_isPopup         = bPopup;
		   d_AdditionalParam = nAdditionalParam;
		   d_PopupTree       = pPopoutTreeWindow;
	   }

	   void       clearPopupState(void)
	   {
           d_isPopup         = false;
		   d_AdditionalParam = 0;
		   d_PopupTree       = NULL;
	   }
       
	   TLTree *   getPopupoutTree(void)
	   {
		   return d_PopupTree;
	   }


	   //tailText
	   void		setTailText(const String& text)	{	d_tailText = text;	}
	   String	getTailText()	{	return d_tailText;	}

/*	   void setTailTextColours(const ColourRect& colourRect)	{	d_tailTextCols = colourRect;	}*/
	   void setTailTextColor(const colour& col)	{	d_tailTextColor = col;	}
	   colour getTailTextColor()	{	return d_tailTextColor;	}

	   void       setCanDrag(const bool bCanDrag)
	   {
		   d_canDrag = bCanDrag;
	   }

	   bool       getCanDrag(void)const
	   {
		   return d_canDrag;
	   }
	   //设置某项是否被打开
    protected:
        // overridden from TreeItem base class.
        void populateRenderCache();
		uint		d_itemIndex;
    private:
		bool        d_isPopup;
		bool        d_canDrag;
		int         d_AdditionalParam;
		TLTree *    d_PopupTree;

		//caol+ Item的名字后缀
		String		d_tailText;		  //!< Text for this list box item.
		colour		d_tailTextColor;
    };

    /*!
    \brief
        WindowFactory for TLTreeItem type Window objects.
    */
    class TAHAREZLOOK_API TLTreeItemFactory : public WindowFactory
    {
    public:
        TLTreeItemFactory(void) : WindowFactory(TLTreeItem::WidgetTypeName) { }
        ~TLTreeItemFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalTreeItem_h_
