/************************************************************************
filename:   TLTree.cpp
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
#include "TLTree.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIScrollbar.h"
#include "TLMiniHorzScrollbar.h"
#include "TLMiniVertScrollbar.h"
#include "CEGUIImageset.h"
#include "CEGUICoordConverter.h"
#include "CEGUIPropertyHelper.h"
#include "CEGUIFontManager.h"
#include "CEGUIGlobalEventSet.h"
#include "ui/UiSheetMgr.h"
#include "Ui/UiCase/UiItemTip.h"

#define   TL_GLOBAL_DRAG_ITEM_PATH "uisettings/layouts/GlobalDragTreeItem.ls"



template<> 
TLGlobalDragTreeItem* KUiWndSingleton<TLGlobalDragTreeItem>::ms_Singleton	= NULL;

namespace CEGUI
{
	static TreeItem* lastItem = NULL;

	namespace TLTreeProperties
	{
		String	TLTreeDragImage::get(const PropertyReceiver* receiver) const
		{
			return PropertyHelper::imageToString(static_cast<const TLTree*>(receiver)->getDragImage());
		}
		
		void	TLTreeDragImage::set(PropertyReceiver* receiver, const String& value)
		{
			static_cast<TLTree*>(receiver)->setDragImage(PropertyHelper::stringToImage(value));
		}
	
		String	TLTreeDragEnable::get(const PropertyReceiver* receiver) const
		{
			return PropertyHelper::boolToString(static_cast<const TLTree*>(receiver)->isDragEnabled());
		}
		
		void	TLTreeDragEnable::set(PropertyReceiver* receiver, const String& value)
		{
			static_cast<TLTree*>(receiver)->setDragEnabled(PropertyHelper::stringToBool(value));
		}
	}
	


	TLGlobalDragTreeItem::TLGlobalDragTreeItem(void)
		:KUiWndSingleton<TLGlobalDragTreeItem>( TL_GLOBAL_DRAG_ITEM_PATH )
	{
		d_InfoParentTree      = NULL;
        d_IsItemInfoAvailable = false;
		d_IsDraging           = false;

        ms_Singleton->m_pThisWnd = ms_Singleton->m_pWindowManager->loadWindowLayout( ms_Singleton->m_strPath);
		if ( ms_Singleton && ms_Singleton->m_pThisWnd )
		{
			GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseMove,     Event::Subscriber(&TLGlobalDragTreeItem::handleMouseMove, this));
			GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseButtonUp, Event::Subscriber(&TLGlobalDragTreeItem::handleMouseLBUp, this));
		}//endif
	}

	void TLGlobalDragTreeItem::SetDragItemInfo(const TLTreeItem * pTreeItem /* = NULL */,TLTree * pParent /*= NULL*/)
	{
		if (ms_Singleton->m_pThisWnd && pTreeItem && pParent && pTreeItem->getCanDrag())
		{
			d_IsItemInfoAvailable = true;
			d_ItemText            = pTreeItem->getText();
            d_InfoParentTree      = pParent;

			if (pParent && pParent->getDragImage())
			{
				((StaticImage *)ms_Singleton->m_pThisWnd)->setWidth(Absolute,pParent->getDragImage()->getWidth());
				((StaticImage *)ms_Singleton->m_pThisWnd)->setHeight(Absolute,pParent->getDragImage()->getHeight());
			    ((StaticImage *)ms_Singleton->m_pThisWnd)->setImage(pParent->getDragImage());
			}//endif

		    TLStaticText * pText = NULL;
			pText = ( TLStaticText *) ms_Singleton->m_pThisWnd->getChild("TaharezLook/GlobalDragTreeItem/Txt");
			if (pText)
			{
				pText->setText(d_ItemText);
			}//endif

			Window * pRoot =  KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
			if (pRoot)
			{
				pRoot->removeChildWindow(ms_Singleton->m_pThisWnd);
				pRoot->addChildWindow(ms_Singleton->m_pThisWnd);
			}//endif
			ms_Singleton->m_pThisWnd->setZLevel(Window::Top);
		}//endif
	}

	const String & TLGlobalDragTreeItem::GetDragInfo(bool * pIsAvailable,TLTree ** lplpParent)
	{
		if (pIsAvailable && lplpParent)
		{
			*pIsAvailable    = d_IsItemInfoAvailable;
			*lplpParent      = d_InfoParentTree;
		}
        return d_ItemText;
	}

	bool TLGlobalDragTreeItem::handleMouseMove(const CEGUI::EventArgs& e)
	{
		if (d_IsItemInfoAvailable)
		{
			Point newP = MouseCursor::getSingleton().getPosition();
			m_pThisWnd->setPosition(Absolute,newP);
			
			Show();
			
			d_IsDraging = true;
			
			GlobalDragTreeItemArgs Args(m_pThisWnd);
			Args.d_ItemText   = d_ItemText;
			Args.d_ParentTree = d_InfoParentTree;

			GlobalEventSet::getSingleton().fireEvent(Window::EventGlobalDragTreeItemMove,Args);
			return true;
		}//endif

		return false;
	}

	bool TLGlobalDragTreeItem::handleMouseLBUp(const CEGUI::EventArgs& e)
	{
		if (d_IsItemInfoAvailable)
		{
			GlobalDragTreeItemArgs Args(m_pThisWnd);
			Args.d_ItemText   = d_ItemText;
			Args.d_ParentTree = d_InfoParentTree;

            GlobalEventSet::getSingleton().fireEvent(Window::EventGlobalDragTreeItemAccept,Args);
		}//endif
		
		Hide();
		d_IsItemInfoAvailable = false;
		d_IsDraging = false;
        
		return true;
	}

	bool TLGlobalDragTreeItem::IsDraging()const
	{
		return d_IsDraging;
	}

	TLGlobalDragTreeItem::~TLGlobalDragTreeItem()
	{

	}

    const utf8 TLTree::WidgetTypeName[] = "TaharezLook/Tree";
	const unsigned int s_space = 14;
	// image / imageset related
	const utf8	TLTree::ImagesetName[]				= "TaharezLook";
	const utf8	TLTree::TopLeftImageName[]			= "TreeTopLeft";
	const utf8	TLTree::TopRightImageName[]			= "TreeTopRight";
	const utf8	TLTree::BottomLeftImageName[]		= "TreeBottomLeft";
	const utf8	TLTree::BottomRightImageName[]		= "TreeBottomRight";
	const utf8	TLTree::LeftEdgeImageName[]			= "TreeLeft";
	const utf8	TLTree::RightEdgeImageName[]		= "TreeRight";
	const utf8	TLTree::TopEdgeImageName[]			= "TreeTop";
	const utf8	TLTree::BottomEdgeImageName[]		= "TreeBottom";
	const utf8	TLTree::BackgroundImageName[]		= "TreeBackdrop";
	const utf8	TLTree::MouseCursorImageName[]		= "MouseTextBar";
	const utf8  TLTree::TreeItemHeightLight[]		= "MouseTextBar";
	
	const utf8*	TLTree::HorzScrollbarTypeName		= TLMiniHorzScrollbar::WidgetTypeName;
	const utf8*	TLTree::VertScrollbarTypeName		= TLMiniVertScrollbar::WidgetTypeName;                    

	TLTreeProperties::TLTreeDragImage	TLTree::d_DrageImageProperty;
	TLTreeProperties::TLTreeDragEnable	TLTree::d_DrageEnable;

    TLTree::TLTree(const String& type, const String& name) :
	Tree(type, name)
    {
		Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
		
		storeFrameSizes();
		
		// setup frame images
		d_frame.setImages(
			&iset->getImage(TopLeftImageName),		&iset->getImage(TopRightImageName),
			&iset->getImage(BottomLeftImageName),	&iset->getImage(BottomRightImageName),
			&iset->getImage(LeftEdgeImageName),		&iset->getImage(TopEdgeImageName), 
			&iset->getImage(RightEdgeImageName),	&iset->getImage(BottomEdgeImageName)
			);
		
		// setup background brush
		d_background.setImage(&iset->getImage(BackgroundImageName));
		d_background.setPosition(Point(d_frameLeftSize, d_frameTopSize));
		d_background.setHorzFormatting(RenderableImage::HorzStretched);
		d_background.setVertFormatting(RenderableImage::VertStretched);
		d_frameInsectionPixel=0.0f;
		
		// set cursor for this window.
		setMouseCursor(&iset->getImage(MouseCursorImageName));
		d_itemHoverImage = const_cast<Image *>(&iset->getImage(TreeItemHeightLight));
		
		for( int i = 0; i < ISOPEN_MAX_SUM; i++)
		{
			d_isOpen[i] = false;
		}

		d_DragItemImage = NULL;
		d_DragEnalbled  = false;

        addProperty( &d_DrageImageProperty);
		addProperty( &d_DrageEnable);
    }
	
    TLTree::~TLTree()
    {
    }
	
	void TLTree::setVertScrollBar( const Scrollbar *pScrollBar )
	{
		if ( pScrollBar != 0 )
		{
			d_pVertScrollBar = const_cast<Scrollbar*>(pScrollBar);
		}
	}
	
	//获得整个树的高度
	float	TLTree::getTreeTotalItemsHeigh()
	{
		return getTotalItemsHeight() + d_frameTopSize;
	}
	
	//获取到某一树结点的高度
	float	TLTree::getHeightItemToTop( TreeItem *pTreeItem )
	{
		float result = 0.0;
		if(getHeightToItemInList( d_listItems, pTreeItem, 0, &result ))
		{
			return result;
		}
		return 0.0;
	}

	TreeItem*   TLTree::getItemFromIndex(const int nIndex)
	{
        if (nIndex < d_listItems.size())
		{
           return d_listItems[nIndex];
		}
		return NULL;
	}

	void        TLTree::setItemFromIndex(const int nIndex, TreeItem * pItem)
	{
		if (nIndex<d_listItems.size())
		{
			d_listItems[nIndex] = pItem;
		}//endif
	}

	TreeItem*   TLTree::getItemAtCurMouse()const
	{
		Point e = MouseCursor::getSingleton().getPosition();
		Point localPos(CoordConverter::screenToWindow(*this, e));
		TreeItem* item = getItemAtPoint(localPos);
		return item;
	}

	void    TLTree::SetInsectionPixel(const float fInserction)
	{
		d_frameInsectionPixel = fInserction;
	}

	//获取从子树的根到节点的路径
	bool TLTree::findLeafPathWithText( TreeItem *ptree, const String& text, std::vector<TreeItem *> &path)
	{
		bool bFlag = false;
		String itemText = ptree->getText();
		if ( itemText == text )
		{
			path.push_back( ptree );
			ptree->setSelected(true);
			return true;
		}
		else
		{
			int itemCount = ptree->getItemCount();
			if ( itemCount == 0 )
			{
				return false;
			}
			else
			{
				std::vector<TreeItem *>::iterator pitem = (ptree->getItemList().begin());
				for( ; pitem < ptree->getItemList().end(); pitem++ )
				{
					bool bfind = findLeafPathWithText( *pitem, text, path );
					
					if ( pitem == ptree->getItemList().end())
					{
						return false;
					}
					else
					{
						if ( bfind )
						{
							reinterpret_cast<TLTreeItem *>(ptree)->setIsOpen(true);
							path.push_back(ptree);
							return true;
						}
						/*else
						{
							return false;
						}//*/
					}
				}
			}
		}
		return false;
	}
	
	//在树中找根到节点的路径
	TreeItem* TLTree::findLeafPathFromTree( const String &text )
	{
		bool result = false;
		std::vector<TreeItem *> path;
		int iTmp = 0;
		for ( int i = 0; i < getItemCount(); i++, iTmp++ )
		{
			if ( d_listItems[i] != NULL )
			{
				result = findLeafPathWithText( d_listItems[i], text, path);
				if ( result )
				{
					return path[0];
				}
			}
		}
		return NULL;
	}
	
	
	
	void TLTree::setDragImage(const Image * pImage)
	{
         d_DragItemImage = pImage;
	}

	const Image *  TLTree::getDragImage(void)const
	{
         return d_DragItemImage;
	}

	void TLTree::scratchWindowDueItems(void)
	{
         Size  oldSize = getSize(Absolute);
		 float fHight  = getTotalItemsHeight();

		 oldSize.d_height = fHight+10.0f;
		 setSize(Absolute,oldSize);
	}
	
	TLGlobalDragTreeItem & TLTree::getGlobalDragItem()
	{
		static  TLGlobalDragTreeItem gDragItem;
		return gDragItem;
	}

    Rect TLTree::getTreeRenderArea(void) const
    {
		Rect tmp;
		
// 		tmp.d_left	= d_frameLeftSize;
// 		tmp.d_top	= d_frameTopSize;
		tmp.d_left = 0;
		tmp.d_top = 0;
		tmp.setSize(Size(getAbsoluteWidth(), getAbsoluteHeight()));
		
		// 		if (d_vertScrollbar->isVisible())
		// 		{
		// 			tmp.d_right -= d_vertScrollbar->getAbsoluteWidth();
		// 		}
		// 		else
		// 		{
		// 			tmp.d_right -= d_frameRightSize;
		// 		}
		// 
		// 		if (d_horzScrollbar->isVisible())
		// 		{
		// 			tmp.d_bottom -= d_horzScrollbar->getAbsoluteHeight();
		// 		}
		// 		else
		// 		{
		// 			tmp.d_bottom -= d_frameBottomSize;
		// 		}
		
		return tmp;
    }
	
    Scrollbar* TLTree::createVertScrollbar(const String& name) const
    {
        // return component created by look'n'feel assignment.
		Scrollbar* sbar = (Scrollbar*)WindowManager::getSingleton().createWindow(VertScrollbarTypeName, name);
		
		// set min/max sizes
		sbar->setMinimumSize(Size(0.0125f, 0.0f));
		sbar->setMaximumSize(Size(0.0125f, 1.0f));
		
		return sbar;
    }
	
    Scrollbar* TLTree::createHorzScrollbar(const String& name) const
    {
		Scrollbar* sbar = (Scrollbar*)WindowManager::getSingleton().createWindow(HorzScrollbarTypeName, name);
		
		// set min/max sizes
		sbar->setMinimumSize(Size(0.0f, 0.016667f));
		sbar->setMaximumSize(Size(1.0f, 0.016667f));
		
		return sbar;
    }
	
    void TLTree::cacheTreeBaseImagery()
    {
		// draw the box elements
		//d_background.draw(d_renderCache);
		if ( isFrameEnabled() )
		{
			//	d_frame.draw(d_renderCache);
		}
    }
	
	
	/*************************************************************************
	Store the sizes for the frame edges	
	*************************************************************************/
	void TLTree::storeFrameSizes(void)
	{
		Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
		
		d_frameLeftSize		= iset->getImage(LeftEdgeImageName).getWidth();
		d_frameRightSize	= iset->getImage(RightEdgeImageName).getWidth();
		d_frameTopSize		= iset->getImage(TopEdgeImageName).getHeight();
		d_frameBottomSize	= iset->getImage(BottomEdgeImageName).getHeight();
	}
	
	/*************************************************************************
	Handler for when the window is sized.
	*************************************************************************/
	void TLTree::onSized(WindowEventArgs& e)
	{
		// base class processing
		Tree::onSized(e);
		
		Size newsize(getAbsoluteSize());
		// update size of frame
		if ( isFrameEnabled() )
		{
			d_frame.setSize(newsize);
		}
		
		// update size of background image
		newsize.d_width		-= (d_frameLeftSize + d_frameRightSize);
		newsize.d_height	-= (d_frameTopSize + d_frameBottomSize);
		
		//d_background.setSize(newsize);
	}
	
	
	/*************************************************************************
	Handler for when the alpha for the window changes.
	*************************************************************************/
	void TLTree::onAlphaChanged(WindowEventArgs& e)
	{
		// base class processing
		Tree::onAlphaChanged(e);
		
		// update alpha values for the frame and background brush
		float alpha = getEffectiveAlpha();
		
		ColourRect cr;
		if ( isFrameEnabled() )
		{
			cr = d_frame.getColours();
			cr.setAlpha(alpha);
			d_frame.setColours(cr);
		}
		
		cr = d_background.getColours();
		cr.setAlpha(alpha);
		d_background.setColours(cr);
	}

	void TLTree::onMouseMove(MouseEventArgs& e)
	{ 
		Tree::onMouseMove(e);

		TLTreeItem * pCurSelect = (TLTreeItem *)getItemAtCurMouse();

		if (pCurSelect && pCurSelect->isPopup() && pCurSelect->getPopupoutTree() && !pCurSelect->getPopupoutTree()->isVisible(true))
		{
			Size  size      = pCurSelect->getPixelSize();
            Point parentPos = getPosition(Absolute);
			int   nIndex    = pCurSelect->getItemAdIndex();

			Point poppos    ;
			poppos.d_y      = parentPos.d_y + nIndex * size.d_height;
			poppos.d_x      = parentPos.d_x + size.d_width;

			pCurSelect ->getPopupoutTree()->setPosition(Absolute,poppos);
			pCurSelect ->getPopupoutTree()->show();
		}//endif


		//显示item的ToolTip
		if ( d_itemTooltips && !d_tooltipText.empty() )
		{
			Point posi(CoordConverter::screenToWindow(*this, e.position));
			//      Point posi = relativeToAbsolute(CoordConverter::screenToWindow(*this, e.position));
			
			TreeItem* item = getItemAtPoint(posi);
			if (item != lastItem)
			{
				if (item != NULL)
				{
					//setTooltipText(item->getTooltipText());
#ifndef _LAYOUT_EDITOR_
					KUiItemTip& tip = KUiItemTip::GetSingleton();
					
					Rect rect = getUnclippedInnerRect();
					float itemToTop_Height = getHeightItemToTop(item);

					rect.d_top += itemToTop_Height;
 					//rect.d_left += item->getPixelSize().d_width;
// 					rect.d_top = e.position.d_y;
					rect.setHeight( item->getPixelSize().d_height );
					//rect.setWidth( item->getPixelSize().d_width );
					
					// 					posi
					// 					item->get
					//设置tip位置
					tip.show(Utf8ToAnsi( d_tooltipText ), rect, KUiItemTip::BottomRight);
#endif // _LAYOUT_EDITOR_
				}
				else
				{
					//setTooltipText("");
					KUiItemTip::Hide();
				}
				lastItem = item;
			}
			
			// must check the result from getTooltip(), as the tooltip object could
			// be 0 at any time for various reasons.
			// 			Tooltip* tooltip = getTooltip();
			// 			
			// 			if (tooltip)
			// 			{
			// 				if (tooltip->getTargetWindow() != this)
			// 					tooltip->setTargetWindow(this);
			// 				else
			// 					tooltip->positionSelf();
			// 			}
		}
		requestRedraw();
	}

	void TLTree::onMouseLeaves( MouseEventArgs& e )
	{
		Tree::onMouseLeaves( e );
		if ( d_itemTooltips )
		{
			lastItem = NULL;
#ifndef _LAYOUT_EDITOR_
			KUiItemTip::Hide();
#endif // _LAYOUT_EDITOR_
		}	
	}

	void TLTree::onMouseDoubleClicked( MouseEventArgs& e )
	{
		Tree::onMouseDoubleClicked( e );
		if ( d_itemTooltips )
		{
			lastItem = NULL;
#ifndef _LAYOUT_EDITOR_
			KUiItemTip::Hide();
#endif // _LAYOUT_EDITOR_
		}	
	}

	
	//绘制TreeItemList
	
	void TLTree::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
	{
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		
		Point	itemPos;
		float	widest = getWidestItemWidth();
		
		// calculate position of area we have to render into
		Rect itemsArea(getUnclippedPixelRect());
		itemsArea.offset(offPos);
		
		Rect clipper(getInnerRect());
		clipper.offset(offPos);
		
		// set up some initial positional details for items
		itemPos.d_x = itemsArea.d_left;// - d_horzScrollbar->getScrollPosition();
		itemPos.d_y = itemsArea.d_top;// - d_vertScrollbar->getScrollPosition();
		
		Point mouse = MouseCursor::getSingleton().getPosition() + offPos;
		drawItemList(d_listItems, clipper, widest, itemPos, panelCache, mouse);
	}
	
	void TLTree::drawItemList(LBItemList &itemList, Rect &itemsArea, float widest, Point &itemPos, KRenderCache* panelCache, Point mouse)
	{
		if (itemList.empty())
			return;
		
		// loop through the items
		Size     itemSize;
		Rect     itemClipper, itemRect;
		size_t   itemCount = itemList.size();
		bool     itemIsVisible;
		for(size_t i = 0; i < itemCount; ++i)
		{
			itemSize.d_height = itemList[i]->getPixelSize().d_height;
			
			// allow item to have full width of box if this is wider than items
			itemSize.d_width = max(itemsArea.getWidth(), widest);
			
			// calculate destination area for this item.
			itemRect.d_left = itemPos.d_x;
			itemRect.d_top  = itemPos.d_y;

			itemRect.setSize(itemSize);
			itemClipper = itemRect.getIntersection(itemsArea);
			
			if (itemClipper.getHeight() > 0)
			{
				itemIsVisible = true;
				
				if(itemClipper.d_top < mouse.d_y && itemClipper.d_bottom > mouse.d_y)
				{
					MouseCursor& ma = MouseCursor::getSingleton();
					// move the mouse cursor & update position in args.
					if ( isHit(ma.getPosition()) )
					{
						itemList[i]->drawSelf(panelCache, itemPos, itemClipper, true);
					}	
					else
					{
						itemList[i]->drawSelf(panelCache, itemPos, itemClipper, false);
					}
				}
				else
				{
					itemList[i]->drawSelf(panelCache, itemPos, itemClipper, false);
				}
			}
			else
			{
				itemIsVisible = false;
			}
			
			// Process this item's list if it has items in it.
			if (itemList[i]->getItemCount() > 0)
			{
				itemList[i]->setButtonLocation(getTreeRenderArea());
				
				if (itemList[i]->getIsOpen())
				{
					// update position ready for next item
					itemPos.d_y += itemSize.d_height;
					
					itemPos.d_x += s_space;
					drawItemList(itemList[i]->getItemList(), itemsArea, widest, itemPos, panelCache, mouse);
					itemPos.d_x -= s_space;
				}
				else
				{
					// update position ready for next item
					itemPos.d_y += itemSize.d_height;
				}
			}
			else
			{
				// update position ready for next item
				itemPos.d_y += itemSize.d_height;
				itemPos.d_y += d_frameInsectionPixel;
			}
		}
	}
	
	
	//绘制TreeItemList
	void TLTree::drawItemList(LBItemList &itemList, Rect &itemsArea, float widest, Vector3 &itemPos, RenderCache& cache, float alpha)
	{
		if (itemList.empty())
			return;
		
		// loop through the items
		Size     itemSize;
		Rect     itemClipper, itemRect;
		size_t   itemCount = itemList.size();
		bool     itemIsVisible;
		for (size_t i = 0; i < itemCount; ++i)
		{
			itemSize.d_height = itemList[i]->getPixelSize().d_height;
			
			// allow item to have full width of box if this is wider than items
			itemSize.d_width = max(itemsArea.getWidth(), widest);
			
			// calculate destination area for this item.
			itemRect.d_left = itemPos.d_x;
			itemRect.d_top  = itemPos.d_y;
			itemRect.setSize(itemSize);
			itemClipper = itemRect.getIntersection(itemsArea);
			
			
			if (itemClipper.getHeight() > 0)
			{
				itemIsVisible = true;
				if (itemList[i]->isSelected())
				{
					((TLTreeItem *)itemList[i])->setSelectionBrushImage(d_itemHoverImage);
				}
				itemList[i]->draw(d_renderCache, itemRect, itemPos.d_z, alpha, &itemClipper, d_itemHoverImage);
			}
			else
			{
				itemIsVisible = false;
			}
			
			// Process this item's list if it has items in it.
			if (itemList[i]->getItemCount() > 0)
			{
				Rect buttonRenderRect;
				buttonRenderRect.d_left = itemPos.d_x;
				buttonRenderRect.d_right = buttonRenderRect.d_left + getTreeRenderArea().getWidth();
				buttonRenderRect.d_top = itemPos.d_y;
				buttonRenderRect.d_bottom = buttonRenderRect.d_top + getTreeRenderArea().getHeight();
				
				itemList[i]->setButtonLocation(buttonRenderRect);
				
				if (itemList[i]->getIsOpen())
				{
					// update position ready for next item
					itemPos.d_y += itemSize.d_height;
					
					itemPos.d_x += s_space;
					drawItemList(itemList[i]->getItemList(), itemsArea, widest, itemPos, cache, alpha);
					itemPos.d_x -= s_space;
				}
				else
				{
					// update position ready for next item
					itemPos.d_y += itemSize.d_height;
				}
			}
			else
			{
				// update position ready for next item
				itemPos.d_y += itemSize.d_height;
				itemPos.d_y += d_frameInsectionPixel;
			}
		}
		// Successfully drew all items, so vertical scrollbar not needed.
		//   setShowVertScrollbar(false);
		
	}
	
	void  TLTree::onMouseButtonDown(MouseEventArgs& e)
	{
		// TreeItem选择不正确，暂时这样改
		e.position.d_y;

		Tree::onMouseButtonDown(e);
		//if (e.button == LeftButton)
		//{
		Point localPos(CoordConverter::screenToWindow(*this, e.position));
		TLTreeItem* item = (TLTreeItem *)getItemAtPoint(localPos);
		if ( item != NULL )
		{
			LBItemList::iterator pos = d_listItems.begin();
			clearAllSelections();
			item->setSelected(true);
			
			TLTree::getGlobalDragItem();

			if ( e.button==LeftButton && isDragEnabled() && TLGlobalDragTreeItem::GetSingletonPtr())
			{
				TLGlobalDragTreeItem::GetSingleton().SetDragItemInfo(item,this);
                TLGlobalDragTreeItem::GetSingleton().Hide();
			}//endif
		}
		//}
	}

	
	//////////////////////////////////////////////////////////////////////////
	/*************************************************************************
	
	  Factory Methods
	  
	*************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	
	/*************************************************************************
	Create, initialise and return a TLFrameWindow	
	*************************************************************************/
	Window* TLTreeFactor::createWindow(const String& name)
	{
		return new TLTree(d_type, name);
	}
	
	
} // End of  CEGUI namespace section
