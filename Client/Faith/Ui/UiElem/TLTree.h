/************************************************************************
    filename:   FalTree.h
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
#ifndef _FalTree_h_
#define _FalTree_h_

#include "TLModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUITree.h"
#include "CEGUIRenderableFrame.h"
#include "CEGUIRenderableImage.h"
#include "Ui/UiCommon.h"
//#include "CEGUITreeItem.h"
#include "TLTreeItem.h"
#include <vector>
// Start of CEGUI namespace section
#define ISOPEN_MAX_SUM  20


namespace CEGUI
{
	
	namespace TLTreeProperties
	{
		class TLTreeDragImage : public Property
		{
		public:
			TLTreeDragImage() : Property(
				"DragImage",
				"Image to show when something is drag out",
				""){}
			
			String	get(const PropertyReceiver* receiver) const;
			void	set(PropertyReceiver* receiver, const String& value);
		};

		class TLTreeDragEnable : public Property
		{
		public:
			TLTreeDragEnable() : Property(
				"ItemDragEnable",
				"Enable Item draging",
				"False"){}
			
			String	get(const PropertyReceiver* receiver) const;
			void	set(PropertyReceiver* receiver, const String& value);
		};

	}
    /*!
    \brief
        Tree class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled

        Named Areas:
            - ItemRenderingArea
            - ItemRenderingAreaHScroll
            - ItemRenderingAreaVScroll
            - ItemRenderingAreaHVScroll

        Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"
    */

	class TLTree;

	class TAHAREZLOOK_API TLGlobalDragTreeItem: public KUiWndSingleton<TLGlobalDragTreeItem>
	{	
	public:
		TLGlobalDragTreeItem(void);
		~TLGlobalDragTreeItem(void);
	public:
		bool           handleMouseMove(const CEGUI::EventArgs& e);
		bool           handleMouseLBUp(const CEGUI::EventArgs& e);
	public:
        void           SetDragItemInfo(const TLTreeItem * pTreeItem = NULL,TLTree * pParent=NULL);
		bool           IsDraging(void)const;
		const String & GetDragInfo(bool * pIsAvailable,TLTree ** lplpParent);
	private:
		bool           d_IsDraging;
		bool           d_IsItemInfoAvailable;
        TLTree *       d_InfoParentTree;
		String         d_ItemText;
	};

	class TAHAREZLOOK_API GlobalDragTreeItemArgs : public WindowEventArgs
	{
	public:
		GlobalDragTreeItemArgs(Window* wnd) : WindowEventArgs(wnd) { d_ParentTree = NULL; }
		TLTree *       d_ParentTree;
		String         d_ItemText;
	};


    class TAHAREZLOOK_API TLTree : public Tree
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.
		// image / imageset related
		static const utf8	ImagesetName[];				//!< Name of the imageset to use for rendering.
		static const utf8	TopLeftImageName[];			//!< Name of the image to use for the top-left corner of the box.
		static const utf8	TopRightImageName[];		//!< Name of the image to use for the top-right corner of the box.
		static const utf8	BottomLeftImageName[];		//!< Name of the image to use for the bottom left corner of the box.
		static const utf8	BottomRightImageName[];		//!< Name of the image to use for the bottom right corner of the box.
		static const utf8	LeftEdgeImageName[];		//!< Name of the image to use for the left edge of the box.
		static const utf8	RightEdgeImageName[];		//!< Name of the image to use for the right edge of the box.
		static const utf8	TopEdgeImageName[];			//!< Name of the image to use for the top edge of the box.
		static const utf8	BottomEdgeImageName[];		//!< Name of the image to use for the bottom edge of the box.
		static const utf8	BackgroundImageName[];		//!< Name of the image to use for the box background.
		static const utf8	MouseCursorImageName[];		//!< Name of the image to use for the mouse cursor.
		static const utf8   TreeItemHeightLight[];		//!< Name of the image to use for the Tree item hight light;
        /*!
        \brief
            Constructor
        */
        TLTree(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~TLTree();
		void setVertScrollBar( const Scrollbar *pScrollBar );
		float getTreeTotalItemsHeigh();
		float getHeightItemToTop( CEGUI::TreeItem *pTreeItem );
		TreeItem*  findLeafPathFromTree( const String &text );
		bool  findLeafPathWithText( TreeItem *ptree,  const String& text,  std::vector<TreeItem *> &path);		
        
		void    SetInsectionPixel(const float fInserction);

		void	findSetIsOpen( TreeItem *pItem )
		{
			int count = 0;
			LBItemList::iterator i = d_listItems.begin();
			for ( ; i!=d_listItems.end(); i++, count++ )
			{
				if ( *i == pItem )
				{
					if ( (*i)->getIsOpen() )
					{
						setIsOpenEx( count, true );
						return;
					}
					else
					{
						setIsOpenEx( count, false);
					}
				}
			}
		}

		TreeItem*   getItemFromIndex(const int nIndex);
		void        setItemFromIndex(const int nIndex , TreeItem * pItem);
		TreeItem*   getItemAtCurMouse()const;
		void	setAllOpenItem()
		{
			int count = 0;
			LBItemList::iterator i = d_listItems.begin();
			for ( ; i!=d_listItems.end(); i++, count++ )
			{
				if ( getIsOpenEx(count) )
				{
					(*i)->setIsOpen(true);
				}
				else
				{
					(*i)->setIsOpen(false);
				}
			}
		}

		void	cleanAllOpenItem()
		{
			for ( int i = 0; i < ISOPEN_MAX_SUM; i++ )
			{
				d_isOpen[i] = false;
			}
		}

		void           setDragImage(const Image * pImage);
		const Image *  getDragImage(void)const;
		bool           isDragEnabled(void)const
		{
			           return d_DragEnalbled;
		}
		
		void           setDragEnabled(const bool bAble)
		{
			           d_DragEnalbled = bAble;
		}

		void           scratchWindowDueItems(void);
       

	static	TLGlobalDragTreeItem & getGlobalDragItem(void);
    protected:
        // overridden from Tree base class.
        Rect getTreeRenderArea(void) const;
        Scrollbar* createVertScrollbar(const String& name) const;
        Scrollbar* createHorzScrollbar(const String& name) const;
        void cacheTreeBaseImagery();
		void storeFrameSizes(void);
		void onSized(WindowEventArgs& e);
		void onAlphaChanged(WindowEventArgs& e);
		void onMouseMove(MouseEventArgs& e);
		virtual	void onMouseDoubleClicked(MouseEventArgs& e);
/*		void onMouseEnters( MouseEventArgs& e );*/
		void onMouseLeaves( MouseEventArgs& e );
        // overridden from Window base class.
        virtual void populateRenderCache()  { Tree::populateRenderCache(); }
		//likun
		//重载TreeItemList
		void drawItemList(LBItemList &itemList, Rect &itemsArea, float widest, Vector3 &itemPos, RenderCache& cache, float alpha);
		
		void drawSelf(KRenderCache* panelCache, Point* panelAbsPos);
		void drawItemList(LBItemList &itemList, Rect &itemsArea, float widest, Point &itemPos, KRenderCache* panelCache, Point mouse);

		virtual void onMouseButtonDown(MouseEventArgs& e);
		//设置某一个第一层节点为打开状态
		void	setIsOpenEx( int index, bool bIsOpen )
		{
			d_isOpen[index] = bIsOpen;
		}
		
		//获取第一层某个节点是否为打开状态
		bool	getIsOpenEx( int index )
		{
			return d_isOpen[index];
		}


	private:
		RenderableFrame	d_frame;		//!< Used for the frame of the edit box.
		RenderableImage	d_background;	//!< Used for the background area of the edit box.
		// component widget type names
		static const utf8*	HorzScrollbarTypeName;		//!< Type name of widget to be created as horizontal scroll bar.
		static const utf8*	VertScrollbarTypeName;		//!< Type name of widget to be created as vertical scroll bar.
		Scrollbar		 *	d_pVertScrollBar;
		// sizes of frame edges
		float	d_frameLeftSize;		//!< Width of the left frame edge in pixels.
		float	d_frameRightSize;		//!< Width of the right frame edge in pixels.
		float	d_frameTopSize;			//!< Height of the top frame edge in pixels.
		float	d_frameBottomSize;		//!< Height of the bottom frame edge in pixels.
		float   d_frameInsectionPixel;  //!< Additional Pixels to insert into the space between items
        const   Image * d_DragItemImage;        //!< Image to show when a item is draging
		bool    d_DragEnalbled;
	private:
		bool	d_isOpen[ISOPEN_MAX_SUM];	//保存第一层子节点是否为打开状态
	private:
		static TLTreeProperties::TLTreeDragImage	d_DrageImageProperty;
        static TLTreeProperties::TLTreeDragEnable   d_DrageEnable;
    };

	
	/*!
	\brief
		Factory class for producing TLFrameWindow objects
	*/
	class TAHAREZLOOK_API TLTreeFactor : public WindowFactory
	{
	public:
		/*************************************************************************
			Construction and Destruction
		*************************************************************************/
		/*!
		\brief
			Constructor for Taharez Frame Window factory class.
		*/
		TLTreeFactor(void) : WindowFactory(TLTree::WidgetTypeName) { }


		/*
		\brief
			Destructor for Taharez Frame Window factory class.
		*/
		~TLTreeFactor(void){}


		/*!
		\brief
			Create a new Window object of whatever type this WindowFactory produces.

		\param name
			A unique name that is to be assigned to the newly created Window object

		\return
			Pointer to the new Window object.
		*/
		Window*	createWindow(const String& name);


		/*!
		\brief
			Destroys the given Window object.

		\param window
			Pointer to the Window object to be destroyed.

		\return
			Nothing.
		*/
		virtual void	destroyWindow(Window* window)	 { if (window->getType() == d_type) delete window; }
	};


} // End of  CEGUI namespace section


#endif  // end of guard _FalTree_h_
