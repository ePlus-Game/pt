/************************************************************************
   filename:   CEGUITreeItem.h
   created:    10/17/2004
   author:     David Durant (based on code by Paul D Turner)
   
   purpose: Interface to base class for list items
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
#ifndef _CEGUITreeItem_h_
#define _CEGUITreeItem_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIColourRect.h"
#include "CEGUIRenderCache.h"
#include "CEGUIFont.h"
#include "KRenderCache.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
   Base class for list box items
*/
class CEGUIEXPORT TreeItem
{
public:
   typedef  std::vector<TreeItem*>  LBItemList;

   /*************************************************************************
      Constants
   *************************************************************************/
   static const colour  DefaultTextColour;         //!< Default text colour.
   static const colour  DefaultSelectionColour;    //!< Default selection brush colour.


   /*************************************************************************
      Construction and Destruction
   *************************************************************************/
   /*!
   \brief
      base class constructor
   */
   TreeItem(const String& text, uint item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = true);


   /*!
   \brief
      base class destructor
   */
   virtual ~TreeItem(void) {}


   /*************************************************************************
      Accessors
   *************************************************************************/
   /*!
   \brief
      Return a pointer to the font being used by this ListboxTextItem

      This method will try a number of places to find a font to be used.  If no font can be
      found, NULL is returned.

   \return
      Font to be used for rendering this item
   */
   const Font* getFont(void) const;


   /*!
   \brief
      Return the current colours used for text rendering.

   \return
      ColourRect object describing the currently set colours
   */
   ColourRect  getTextColours(void) const    {return d_textCols;}


   /*************************************************************************
      Manipulator methods
   *************************************************************************/
   /*!
   \brief
      Set the font to be used by this ListboxTextItem

   \param font
      Font to be used for rendering this item

   \return
      Nothing
   */
   void  setFont(const Font* font)     {d_font = font;}


   /*!
   \brief
      Set the font to be used by this ListboxTextItem

   \param font_name
      String object containing the name of the Font to be used for rendering this item

   \return
      Nothing
   */
   void  setFont(const String& font_name);


   /*!
   \brief
      Set the colours used for text rendering.

   \param cols
      ColourRect object describing the colours to be used.

   \return
      Nothing.
   */
   void  setTextColours(const ColourRect& cols)       {d_textCols = cols;}


   /*!
   \brief
      Set the colours used for text rendering.

   \param top_left_colour
      Colour (as ARGB value) to be applied to the top-left corner of each text glyph rendered.

   \param top_right_colour
      Colour (as ARGB value) to be applied to the top-right corner of each text glyph rendered.

   \param bottom_left_colour
      Colour (as ARGB value) to be applied to the bottom-left corner of each text glyph rendered.

   \param bottom_right_colour
      Colour (as ARGB value) to be applied to the bottom-right corner of each text glyph rendered.

   \return 
      Nothing.
   */
   void  setTextColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);


   /*!
   \brief
      Set the colours used for text rendering.

   \param col
      colour value to be used when rendering.

   \return
      Nothing.
   */
   void  setTextColours(colour col)    {setTextColours(col, col, col, col);}


   /*!
   \brief
      return the text string set for this list box item.

      Note that even if the item does not render text, the text string can still be useful, since it
      is used for sorting list box items.

   \return
      String object containing the current text for the list box item.
   */
   const String&  getText(void) const     {return d_itemText;}
   const String&  getTooltipText(void) const    {return d_tooltipText;}

   /*!
   \brief
      Return the current ID assigned to this list box item.

      Note that the system does not make use of this value, client code can assign any meaning it
      wishes to the ID.

   \return
      ID code currently assigned to this list box item
   */
   uint  getID(void) const       {return d_itemID;}


   /*!
   \brief
      Return the pointer to any client assigned user data attached to this lis box item.

      Note that the system does not make use of this data, client code can assign any meaning it
      wishes to the attached data.

   \return
      Pointer to the currently assigned user data.
   */
   void* getUserData(void) const    {return d_itemData;}


   /*!
   \brief
      return whether this item is selected.

   \return
      true if the item is selected, false if the item is not selected.
   */
   bool  isSelected(void) const     {return d_selected;}


   /*!
   \brief
      return whether this item is disabled.

   \return
      true if the item is disabled, false if the item is enabled.
   */
   bool  isDisabled(void) const     {return d_disabled;}


   /*!
   \brief
      return whether this item will be automatically deleted when the list box it is attached to
      is destroyed, or when the item is removed from the list box.

   \return
      true if the item object will be deleted by the system when the list box it is attached to is
      destroyed, or when the item is removed from the list.  false if client code must destroy the
      item after it is removed from the list.
   */
   bool  isAutoDeleted(void) const  {return false;}
   // LSL memory leak bug
	 //bool  isAutoDeleted(void) const  {return d_autoDelete;}

   /*!
   \brief
      Get the owner window for this TreeItem.
      
      The owner of a TreeItem is typically set by the list box widgets when an item is added or inserted.

   \return
      Ponter to the window that is considered the owner of this TreeItem.
   */
//   const Window*  getOwnerWindow(const Window* owner)    {return d_owner;}
   const Window*  getOwnerWindow(void)    {return d_owner;}


   /*!
   \brief
      Return the current colours used for selection highlighting.

   \return
      ColourRect object describing the currently set colours
   */
   ColourRect  getSelectionColours(void) const     {return d_selectCols;}


   /*!
   \brief
      Return the current selection highlighting brush.

   \return
      Pointer to the Image object currently used for selection highlighting.
   */
   const Image*   getSelectionBrushImage(void) const     {return d_pushedImage;}


   /*************************************************************************
      Manipulators
   *************************************************************************/
   /*!
   \brief
      set the text string for this list box item.

      Note that even if the item does not render text, the text string can still be useful, since it
      is used for sorting list box items.

   \param text
      String object containing the text to set for the list box item.

   \return
      Nothing.
   */
   void  setText(const String& text)      {d_itemText = text;}

   void  setTooltipText(const String& text)     {d_tooltipText = text;}

   /*!
   \brief
      Set the ID assigned to this list box item.

      Note that the system does not make use of this value, client code can assign any meaning it
      wishes to the ID.

   \param item_id
      ID code to be assigned to this list box item

   \return
      Nothing.
   */
   void  setID(uint item_id)     {d_itemID = item_id;}


   /*!
   \brief
      Set the client assigned user data attached to this lis box item.

      Note that the system does not make use of this data, client code can assign any meaning it
      wishes to the attached data.

   \param item_data
      Pointer to the user data to attach to this list item.

   \return
      Nothing.
   */
   void  setUserData(void* item_data)  {d_itemData = item_data;}


   /*!
   \brief
      set whether this item is selected.

   \param setting
      true if the item is selected, false if the item is not selected.

   \return
      Nothing.
   */
   void  setSelected(bool setting)     {d_selected = setting;}


   /*!
   \brief
      set whether this item is disabled.

   \param setting
      true if the item is disabled, false if the item is enabled.

   \return
      Nothing.
   */
   void  setDisabled(bool setting)     {d_disabled = setting;}

   /*!
   \brief
      Set whether this item will be automatically deleted when the list box it is attached to
      is destroyed, or when the item is removed from the list box.

   \param setting
      true if the item object should be deleted by the system when the list box it is attached to is
      destroyed, or when the item is removed from the list.  false if client code will destroy the
      item after it is removed from the list.

   \return
      Nothing.
   */
   void  setAutoDeleted(bool setting)     {d_autoDelete = setting;}


   /*!
   \brief
      Set the owner window for this TreeItem.  This is called by all the list box widgets when
      an item is added or inserted.

   \param owner
      Ponter to the window that should be considered the owner of this TreeItem.

   \return
      Nothing
   */
   void  setOwnerWindow(const Window* owner)    {d_owner = owner;}


   /*!
   \brief
      Set the colours used for selection highlighting.

   \param cols
      ColourRect object describing the colours to be used.

   \return
      Nothing.
   */
   void  setSelectionColours(const ColourRect& cols)     {d_selectCols = cols;}


   /*!
   \brief
      Set the colours used for selection highlighting.

   \param top_left_colour
      Colour (as ARGB value) to be applied to the top-left corner of the selection area.

   \param top_right_colour
      Colour (as ARGB value) to be applied to the top-right corner of the selection area.

   \param bottom_left_colour
      Colour (as ARGB value) to be applied to the bottom-left corner of the selection area.

   \param bottom_right_colour
      Colour (as ARGB value) to be applied to the bottom-right corner of the selection area.

   \return 
      Nothing.
   */
   void  setSelectionColours(colour top_left_colour, colour top_right_colour, colour bottom_left_colour, colour bottom_right_colour);


   /*!
   \brief
      Set the colours used for selection highlighting.

   \param col
      colour value to be used when rendering.

   \return
      Nothing.
   */
   void  setSelectionColours(colour col)     {setSelectionColours(col, col, col, col);}


   /*!
   \brief
      Set the selection highlighting brush image.

   \param image
      Pointer to the Image object to be used for selection highlighting.

   \return
      Nothing.
   */
   void  setSelectionBrushImage(const Image* image)      {d_pushedImage = image;}


   /*!
   \brief
      Set the selection highlighting brush image.

   \param imageset
      Name of the imagest containing the image to be used.

   \param image
      Name of the image to be used

   \return
      Nothing.
   */
   void  setSelectionBrushImage(const String& imageset, const String& image);

   /*!
   \brief
      Tell the treeItem where its button is located.
      Calculated and set in Tree.cpp.

   \param buttonOffset
      Location of the button in screenspace.
   */
   void  setButtonLocation(Rect &buttonOffset) { d_buttonLocation = buttonOffset; }

   Rect &getButtonLocation(void) { return d_buttonLocation; }
   bool  getIsOpen(void) { return d_isOpen; }
   //设置某项是否被打开
	void	  setIsOpen( bool bOpen ){ d_isOpen = bOpen; }

   void  toggleIsOpen(void) { d_isOpen = !d_isOpen; }
   TreeItem *getTreeItemFromIndex(size_t itemIndex);
   size_t	   getItemCount(void) const {return d_listItems.size();}
   LBItemList &getItemList(void) { return d_listItems; }
   void  addItem(TreeItem* item);
   void  setIcon(const Image &theIcon) { d_iconImage = (Image *)&theIcon; }

   /*const Image* getItemHeightLight() const
   {
	   return d_selectBrush;
   }

   ColourRect getSelectCols() const
   {
	   return d_selectCols;
   }*/
   /*************************************************************************
      Abstract portion of interface
   *************************************************************************/
   /*!
   \brief
      Return the rendered pixel size of this list box item.

   \return
      Size object describing the size of the list box item in pixels.
   */
   virtual  Size  getPixelSize(void) const;


   /*!
   \brief
      Draw the list box item in its current state

   \param position
      Vecor3 object describing the upper-left corner of area that should be rendered in to for the draw operation.

   \param alpha
      Alpha value to be used when rendering the item (between 0.0f and 1.0f).

   \param clipper
      Rect object describing the clipping rectangle for the draw operation.

   \return
      Nothing.
   */
   virtual void drawSelf(KRenderCache* panelCache, const Point& position, const Rect& clipper);
   virtual void drawSelf(KRenderCache* panelCache, const Point& position, const Rect& clipper, bool hover);

   virtual  void  draw(const Vector3& position, float alpha, const Rect& clipper) ;

    virtual void    draw(RenderCache& cache,const Rect& targetRect, float zBase,  float alpha, const Rect* clipper) ;

	virtual void	draw(RenderCache& cache,const Rect& targetRect, float zBase,  float alpha, const Rect* clipper, Image *pImage){}
   /*************************************************************************
      Operators
   *************************************************************************/
   /*!
   \brief
      Less-than operator, compares item texts.
   */
   virtual  bool  operator<(const TreeItem& rhs) const      {return d_itemText < rhs.getText();}


   /*!
   \brief
      Greater-than operator, compares item texts.
   */
   virtual  bool  operator>(const TreeItem& rhs) const      {return d_itemText > rhs.getText();}

   //ColourRect getModulateAlphaColourRect(const ColourRect& cols, float alpha) const;
protected:
   /*************************************************************************
      Implementation methods
   *************************************************************************/
   /*!
   \brief
      Return a ColourRect object describing the colours in \a cols after having their alpha
      component modulated by the value \a alpha.
   */
   ColourRect getModulateAlphaColourRect(const ColourRect& cols, float alpha) const;


   /*!
   \brief
      Return a colour value describing the colour specified by \a col after having its alpha
      component modulated by the value \a alpha.
   */
   colour calculateModulatedAlphaColour(colour col, float alpha) const;

   
   /*************************************************************************
   Implementation Data
   *************************************************************************/
   String   d_itemText;    //!< Text for this list box item.  If not rendered, this is still used for list sorting.
   String   d_tooltipText; //!< Text for the individual tooltip of this item
   uint     d_itemID;      //!< ID code assigned by client code.  This has no meaning within the GUI system.
   void *   d_itemData;    //!< Pointer to some client code data.  This has no meaning within the GUI system.
   bool     d_selected;    //!< true if this item is selected.  false if the item is not selected.
   bool     d_disabled;    //!< true if this item is disabled.  false if the item is not disabled.
   bool     d_autoDelete;  //!< true if the system should destroy this item, false if client code will destroy the item.
   Rect     d_buttonLocation; //!< 
   const Window * d_owner; //!< Pointer to the window that owns this item.
   ColourRect     d_selectCols;     //!< Colours used for selection highlighting.
   ColourRect     d_textCols;       //!< Colours used for rendering the text.
   const Font *   d_font;           //!< Font used for rendering text.
   Image *        d_iconImage;      //!< Icon to be displayed with this treeItem
   LBItemList     d_listItems;      //!< list of items in this item's tree branch.
   bool           d_isOpen;         //!< true if the this item's tree branch is opened

	const Image*	d_pushedImage;
	const Image*	d_hoverImage;
	const Image* D_hoverBrush() const { return d_hoverImage; }
	void D_hoverBrush(const Image* val) { d_hoverImage = val; }
	
	const Image*	d_headImage;
	const Image*	d_foldImage;
	const Image*	d_unfoldImage;

	colour	d_normalTextColor;
	colour	d_hoverTextColor;
	colour	d_pushedTextColor;
	const Font*		d_textFont;
	int d_layer;
public:
	void	setFoldImage(const Image* image){	d_foldImage = image;	};
	void	setUnfoldImage(const Image* image){	d_unfoldImage = image;	};
	void	setHoverBrush(const Image* hoverBrush){	d_hoverImage = hoverBrush;	};
	void	setHeadImage(const Image* headImage){	d_headImage	= headImage;	};
	void	setNormalColor(const colour color){		d_normalTextColor = color;	};
	void	setPushedColor(const colour color){		d_pushedTextColor = color;	};
	void	setHoverColor(const colour color){		d_hoverTextColor = color;	};
	void	setTextFont(const Font* font){		d_textFont = font;	};

	void	setLook(
					const Image* foldImage,
					const Image* unfoldImage,
					const Image* pushedImage,
					const Image* hoverImage,
					const Image* headImage,
					const colour normalColor,
					const colour pushedColor,
					const colour hoverColor,
					const Font* font
					)
	{
		d_foldImage = foldImage;
		d_unfoldImage = unfoldImage;
		d_pushedImage = pushedImage;
		d_hoverImage = hoverImage;
		d_headImage = headImage;
		d_normalTextColor = normalColor;
		d_pushedTextColor = pushedColor;
		d_hoverTextColor = hoverColor;
		d_textFont = font;
	}

	void setLayer(int layer){	d_layer = layer;	};
	int getLayer(){	return d_layer;	};
};

} // End of  CEGUI namespace section


#endif   // end of guard _CEGUITreeItem_h_
