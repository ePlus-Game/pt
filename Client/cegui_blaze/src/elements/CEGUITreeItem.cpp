/************************************************************************
	filename: 	CEGUITreeItem.cpp
   created:	   10/17/2004
   author:		David Durant (based on code by Paul D Turner)
	
	purpose:	Implementation of base class for list items
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
#include "elements/CEGUITree.h"
#include "elements/CEGUITreeItem.h"
#include "CEGUISystem.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIFontManager.h"
#include "CEGUIFont.h"
#include "CEGUIWindow.h"
#include "CEGUIImage.h"
#include <algorithm>


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
const colour	TreeItem::DefaultSelectionColour	= 0xFF4444AA;
const colour	TreeItem::DefaultTextColour			= 0xFFFFFFFF;

/*************************************************************************
	Base class constructor
*************************************************************************/
TreeItem::TreeItem(const String& text, uint item_id, void* item_data, bool disabled, bool auto_delete) :
d_itemText(text),
d_itemID(item_id),
d_itemData(item_data),
d_selected(false),
d_disabled(disabled),
d_autoDelete(auto_delete),
d_owner(NULL),
d_selectCols(DefaultSelectionColour, DefaultSelectionColour, DefaultSelectionColour, DefaultSelectionColour),
d_textCols(DefaultTextColour, DefaultTextColour, DefaultTextColour, DefaultTextColour),
d_font(NULL),
d_isOpen(false),
d_buttonLocation(Rect(0,0,0,0)),
d_iconImage(NULL),
d_pushedImage(NULL)
{
	d_layer = 0;
	
	d_pushedImage = NULL;
	d_hoverImage = NULL;
	d_headImage = NULL;
	d_textFont = NULL;
}


/*************************************************************************
	Set the selection highlighting brush image.
*************************************************************************/
void TreeItem::setSelectionBrushImage(const String& imageset, const String& image)
{
   setSelectionBrushImage(&ImagesetManager::getSingleton().getImageset(imageset)->getImage(image));
}


/*************************************************************************
	Return a ColourRect object describing the colours in 'cols' after
	having their alpha component modulated by the value 'alpha'.
*************************************************************************/
ColourRect TreeItem::getModulateAlphaColourRect(const ColourRect& cols, float alpha) const
{
   return ColourRect(	calculateModulatedAlphaColour(cols.d_top_left, alpha),
						calculateModulatedAlphaColour(cols.d_top_right, alpha),
						calculateModulatedAlphaColour(cols.d_bottom_left, alpha),
						calculateModulatedAlphaColour(cols.d_bottom_right, alpha) );
}


/*************************************************************************
	Return a colour value describing the colour specified by 'col' after
	having its alpha component modulated by the value 'alpha'.
*************************************************************************/
colour TreeItem::calculateModulatedAlphaColour(colour col, float alpha) const
{
   colour temp(col);
   temp.setAlpha(temp.getAlpha() * alpha);
   return temp;
}

/*************************************************************************
Return a pointer to the font being used by this ListboxTextItem
*************************************************************************/
const Font* TreeItem::getFont(void) const
{
   // prefer out own font
   if (d_textFont != NULL)
   {
      return d_textFont;
   }
   // try our owner window's font setting (may be null if owner uses no existant default font)
   else if (d_owner != NULL)
   {
      return d_owner->getFont();
   }
   // no owner, just use the default (which may be NULL anyway)
   else
   {
      return System::getSingleton().getDefaultFont();
   }

}


/*************************************************************************
Set the font to be used by this ListboxTextItem
*************************************************************************/
void TreeItem::setFont(const String& font_name)
{
   setFont(FontManager::getSingleton().getFont(font_name));
}


/*************************************************************************
	Return the rendered pixel size of this list box item.
*************************************************************************/
Size TreeItem::getPixelSize(void) const
{
	Size tmp(0,0);

	const Font* fnt = getFont();

	if (fnt != NULL)
	{
		if ( d_headImage )
		{
			tmp.d_height	= max(PixelAligned(fnt->getLineSpacing()), PixelAligned(d_headImage->getHeight()));
			tmp.d_width		= max(PixelAligned(const_cast<Font *>(fnt)->getTextExtent(d_itemText)), PixelAligned(d_headImage->getWidth()));
		}
		else
		{
			tmp.d_height	= PixelAligned(fnt->getLineSpacing());
			tmp.d_width		= PixelAligned(const_cast<Font *>(fnt)->getTextExtent(d_itemText));
		}
	}

	return tmp;
}


/*************************************************************************
   Add the given TreeItem to this item's list.
*************************************************************************/
void TreeItem::addItem(TreeItem* item)
{
   if (item != NULL)
   {
      Tree *parentWindow = (Tree *)getOwnerWindow();
	
      // establish ownership
      item->setOwnerWindow(parentWindow);
	  item->setLook(
		  parentWindow->getFoldImage(),
		  parentWindow->getUnfoldImage(),
		  parentWindow->getItemPushDownImage(), 
		  parentWindow->getItemHoverImage(), 
		  parentWindow->getItemHeadImage(d_layer + 1),
		  parentWindow->getItemNormalColor(d_layer + 1), 
		  parentWindow->getItemPushedColor(d_layer + 1), 
		  parentWindow->getItemHoverColor(d_layer + 1),
		  parentWindow->getItemFont(d_layer + 1));

	  item->setLayer(d_layer + 1);

      // if sorting is enabled, re-sort the list
      if (parentWindow->isSortEnabled())
      {
         d_listItems.insert(std::upper_bound(d_listItems.begin(), d_listItems.end(), item, &lbi_less), item);
      }
      // not sorted, just stick it on the end.
      else
      {
	     d_listItems.push_back(item);
      }

      WindowEventArgs args(parentWindow);
      parentWindow->onListContentsChanged(args);
   }
}


TreeItem *TreeItem::getTreeItemFromIndex(size_t itemIndex)
{
   if (itemIndex > d_listItems.size())
      return NULL;

   return d_listItems[itemIndex];
}


/*************************************************************************
	Draw the list box item in its current state.
*************************************************************************/
void TreeItem::drawSelf(KRenderCache* panelCache, const Point& position, const Rect& clipper) 
{
	if(d_selected && (d_pushedImage != NULL))
	{
		panelCache->cacheImage(d_pushedImage, position, clipper);
	}
	
	const Font* fnt = getFont();
	if(fnt != NULL)
	{
		Rect finalArea(position, clipper.getSize());
		panelCache->cacheText(d_itemText, finalArea, clipper, fnt, LeftAligned, d_textCols.d_top_left);
	}
}

void TreeItem::drawSelf(KRenderCache* panelCache, const Point& position, const Rect& clipper, bool hover)
{
	Point pos(position);
	if(this->getItemCount() > 0)
	{
		if(this->getIsOpen())
		{
			if(d_foldImage != NULL)
			{
				panelCache->cacheImage(d_foldImage, pos, clipper);
			}
		}
		else
		{
			if(d_unfoldImage != NULL)
			{
				panelCache->cacheImage(d_unfoldImage, pos, clipper);
			}
		}
	}
	if(d_unfoldImage != NULL)
	{
		pos.d_x += d_unfoldImage->getWidth();
	}

	if(d_selected && d_pushedImage != NULL)
	{
		panelCache->cacheImage(d_pushedImage, pos, clipper);
	}
	else if(hover)
	{
		panelCache->cacheImage(d_hoverImage, pos, clipper);
	}

	if(d_headImage != NULL)
	{
		panelCache->cacheImage(d_headImage, pos, clipper);
		pos.d_x += d_headImage->getWidth();
	}

	const Font* textFont = d_textFont;
	if(textFont == NULL)
	{
		textFont = System::getSingleton().getDefaultFont();
		if(textFont == NULL)
		{
			return;
		}
	}
		
	Rect finalArea(pos, clipper.getSize());

	if(d_selected)
	{
		panelCache->cacheText(d_itemText, finalArea, clipper, textFont, LeftAligned, d_pushedTextColor);
	}
	else if(hover)
	{
		panelCache->cacheText(d_itemText, finalArea, clipper, textFont, LeftAligned, d_hoverTextColor);
	}
	else
	{
		panelCache->cacheText(d_itemText, finalArea, clipper, textFont, LeftAligned, d_normalTextColor);
	}
}

void TreeItem::draw(const Vector3& position, float alpha, const Rect& clipper) 
{
   if (d_selected && (d_pushedImage != NULL))
   {
      d_pushedImage->draw(clipper, position.d_z, clipper, getModulateAlphaColourRect(d_selectCols, alpha));
   }

   const Font* fnt = getFont();
   if (fnt != NULL)
   {
		Vector3 finalPos(position);
		finalPos.d_y -= PixelAligned((fnt->getLineSpacing() - fnt->getBaseline()) * 0.5f);
		const_cast<Font*>(fnt)->drawText(d_itemText, finalPos, clipper, getModulateAlphaColourRect(d_textCols, alpha));
	}
}

void TreeItem::draw(RenderCache &cache, const Rect &targetRect, float zBase, float alpha, const Rect *clipper) 
{
   Rect finalRect(targetRect);

   if (d_iconImage != NULL)
   {
      Rect finalPos(finalRect);
      finalPos.setWidth(d_iconImage->getWidth());
      finalPos.setHeight(d_iconImage->getHeight());
      cache.cacheImage(*d_iconImage, finalPos, zBase, ColourRect(colour(1,1,1,alpha)), clipper);

      finalRect.d_left += targetRect.getHeight();
   }

   if (d_selected && d_pushedImage != 0)
   {
      cache.cacheImage(*d_pushedImage, finalRect, zBase, getModulateAlphaColourRect(d_selectCols, alpha + 0.3), clipper);
   }

   const Font* font = getFont();
   if (font)
   {
      Rect finalPos(finalRect);
      finalPos.d_top -= (font->getLineSpacing() - font->getBaseline()) * 0.5f;
      cache.cacheText(d_itemText, font, LeftAligned, finalPos, zBase, getModulateAlphaColourRect(d_textCols, alpha), clipper);
   }
}


/*************************************************************************
	Set the colours used for selection highlighting.	
*************************************************************************/
void TreeItem::setSelectionColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour)
{
	d_selectCols.d_top_left		= top_left_colour;
	d_selectCols.d_top_right	= top_right_colour;
	d_selectCols.d_bottom_left	= bottom_left_colour;
	d_selectCols.d_bottom_right	= bottom_right_colour;
}


/*************************************************************************
Set the colours used for text rendering.	
*************************************************************************/
void TreeItem::setTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour)
{
	d_textCols.d_top_left		= top_left_colour;
	d_textCols.d_top_right		= top_right_colour;
	d_textCols.d_bottom_left	= bottom_left_colour;
	d_textCols.d_bottom_right	= bottom_right_colour;
}


} // End of  CEGUI namespace section
