/************************************************************************
	filename: 	CEGUITreeProperties.cpp
   created:	   10/17/2004
   author:		David Durant (based on code by Paul D Turner)
	
	purpose:	Implements Listbox Property classes
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
#include "elements/CEGUITreeProperties.h"
#include "elements/CEGUIListbox.h"
#include "CEGUIFontManager.h"
#include "elements/CEGUITree.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of TreeProperties namespace section
namespace TreeProperties
{
String	Sort::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Listbox*>(receiver)->isSortEnabled());
}


void	Sort::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setSortingEnabled(PropertyHelper::stringToBool(value));
}


String	MultiSelect::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Listbox*>(receiver)->isMultiselectEnabled());
}


void	MultiSelect::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setMultiselectEnabled(PropertyHelper::stringToBool(value));
}


String	ForceVertScrollbar::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Listbox*>(receiver)->isVertScrollbarAlwaysShown());
}


void	ForceVertScrollbar::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setShowVertScrollbar(PropertyHelper::stringToBool(value));
}


String	ForceHorzScrollbar::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Listbox*>(receiver)->isHorzScrollbarAlwaysShown());
}


void	ForceHorzScrollbar::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setShowHorzScrollbar(PropertyHelper::stringToBool(value));
}

String	ItemTooltips::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const Listbox*>(receiver)->isItemTooltipsEnabled());
}


void	ItemTooltips::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setItemTooltipsEnabled(PropertyHelper::stringToBool(value));
}


String TreeItemHoverBrush::get(const PropertyReceiver* receiver) const 
{
	const Image *image = static_cast<const Tree *>(receiver)->getItemHoverImage();
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void  TreeItemHoverBrush::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setItemHoverImage(PropertyHelper::stringToImage(value));
}

String TreeItemPushDownBrush::get(const PropertyReceiver* receiver) const 
{
	const Image *image = static_cast<const Tree *>(receiver)->getItemPushDownImage();
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void  TreeItemPushDownBrush::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Tree *>(receiver)->setItemPushDownImage(PropertyHelper::stringToImage(value));
}

String TreeImageL1::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image *image = treeCtrl->getItemHeadImage(0);
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void TreeImageL1::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image* image = PropertyHelper::stringToImage(value);
	const_cast<Tree*>(treeCtrl)->setItemHeadImage(image, 0);
}

String TreeFontL1::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Font *itemFont = treeCtrl->getItemFont(0);
	return (itemFont != NULL) ? itemFont->getName() : String("");
}

void TreeFontL1::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	Font* font = FontManager::getSingleton().getFont(value);
	const_cast<Tree*>(treeCtrl)->setItemFont(font, 0);
}

String TreeNormalColorL1::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemNormalColor = treeCtrl->getItemNormalColor(0);
    return PropertyHelper::colourToString(itemNormalColor);
}

void TreeNormalColorL1::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemNormalColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemNormalColor(itemNormalColor, 0);
}

String TreePushedColorL1::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemPushedColor = treeCtrl->getItemPushedColor(0);
	return PropertyHelper::colourToString(itemPushedColor);
}

void TreePushedColorL1::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemPushedColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemPushedColor(itemPushedColor, 0);
}

String TreeHoverColorL1::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemHoverColor = treeCtrl->getItemHoverColor(0);
	return PropertyHelper::colourToString(itemHoverColor);
}

void TreeHoverColorL1::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemHoverColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemHoverColor(itemHoverColor, 0);
}

String TreeImageL2::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image *image = treeCtrl->getItemHeadImage(1);
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void TreeImageL2::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image* image = PropertyHelper::stringToImage(value);
	const_cast<Tree*>(treeCtrl)->setItemHeadImage(image, 1);
}

String TreeFontL2::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Font *itemFont = treeCtrl->getItemFont(1);
	return (itemFont != NULL) ? itemFont->getName() : String("");
}

void TreeFontL2::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	Font* font = FontManager::getSingleton().getFont(value);
	const_cast<Tree*>(treeCtrl)->setItemFont(font, 1);
}

String TreeNormalColorL2::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemNormalColor = treeCtrl->getItemNormalColor(1);
	return PropertyHelper::colourToString(itemNormalColor);
}

void TreeNormalColorL2::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemNormalColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemNormalColor(itemNormalColor, 1);
}

String TreePushedColorL2::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemPushedColor = treeCtrl->getItemPushedColor(1);
	return PropertyHelper::colourToString(itemPushedColor);
}

void TreePushedColorL2::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemPushedColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemPushedColor(itemPushedColor, 1);
}

String TreeHoverColorL2::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemHoverColor = treeCtrl->getItemHoverColor(1);
	return PropertyHelper::colourToString(itemHoverColor);
}

void TreeHoverColorL2::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemHoverColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemHoverColor(itemHoverColor, 1);
}

String TreeImageL3::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image *image = treeCtrl->getItemHeadImage(2);
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void TreeImageL3::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image* image = PropertyHelper::stringToImage(value);
	const_cast<Tree*>(treeCtrl)->setItemHeadImage(image, 2);
}

String TreeFontL3::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Font *itemFont = treeCtrl->getItemFont(2);
	return (itemFont != NULL) ? itemFont->getName() : String("");
}

void TreeFontL3::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	Font* font = FontManager::getSingleton().getFont(value);
	const_cast<Tree*>(treeCtrl)->setItemFont(font, 2);
}

String TreeNormalColorL3::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemNormalColor = treeCtrl->getItemNormalColor(2);
	return PropertyHelper::colourToString(itemNormalColor);
}

void TreeNormalColorL3::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemNormalColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemNormalColor(itemNormalColor, 2);
}

String TreePushedColorL3::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemPushedColor = treeCtrl->getItemPushedColor(2);
	return PropertyHelper::colourToString(itemPushedColor);
}

void TreePushedColorL3::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemPushedColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemPushedColor(itemPushedColor, 2);
}

String TreeHoverColorL3::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemHoverColor = treeCtrl->getItemHoverColor(2);
	return PropertyHelper::colourToString(itemHoverColor);
}

void TreeHoverColorL3::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	colour itemHoverColor = PropertyHelper::stringToColour(value);
	const_cast<Tree*>(treeCtrl)->setItemHoverColor(itemHoverColor, 2);
}

String TreeFoldImage::get(const PropertyReceiver* receiver) const 
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image *image = treeCtrl->getFoldImage();
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void TreeFoldImage::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image* image = PropertyHelper::stringToImage(value);
	const_cast<Tree*>(treeCtrl)->setFoldImage(image);
}

String TreeUnfoldImage::get(const PropertyReceiver* receiver) const
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image *image = treeCtrl->getUnfoldImage();
	return (image != NULL) ? PropertyHelper::imageToString(image) : String("");
}

void TreeUnfoldImage::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	const Image* image = PropertyHelper::stringToImage(value);
	const_cast<Tree*>(treeCtrl)->setUnfoldImage(image);
}

String LayerOffset::get(const PropertyReceiver* receiver) const
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	return PropertyHelper::intToString(treeCtrl->getLayerOffset());
}

void LayerOffset::set(PropertyReceiver* receiver, const String& value)
{
	const Tree* treeCtrl = static_cast<const Tree *>(receiver);
	int layeroff = PropertyHelper::stringToInt(value);
	const_cast<Tree*>(treeCtrl)->setLayerOffset(layeroff);
}
} // End of  TreeProperties namespace section

} // End of  CEGUI namespace section
