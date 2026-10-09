/************************************************************************
filename:    TLTreeEx.h
created:	 8/21/2006
author:		 谢鉷
*************************************************************************/
/*************************************************************************
在TLTree中加入一个新的右键点击事件，使程序能够对右键点击树的节点与叶子做出
不同的操作
*************************************************************************/

#include "TLTree.h"
#include "TLTreeEx.h"
#include "elements/CEGUITree.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIScrollbar.h"
#include "TLMiniHorzScrollbar.h"
#include "TLMiniVertScrollbar.h"
#include "CEGUIImageset.h"
#include "CEGUICoordConverter.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 TLTreeEx::WidgetTypeNameEx[] = "TaharezLook/TreeEx";
	

    TLTreeEx::TLTreeEx(const String& type, const String& name) :TLTree(type, name)
    {
		AddTreeExEvents();
    }
	
	TreeItem	*TLTreeEx::IsFirstLayer(const String& text)
	{
	   size_t d_itemCount = d_listItems.size();

	   for (size_t index = 0; index < d_itemCount; ++index)
	   {

		 if (d_listItems[index]->getText() == text)
			return d_listItems[index];
	   }

	   return NULL;
	}


	void TLTreeEx::onMouseButtonUp(MouseEventArgs& e)
	{
		// base class processing
		TLTree::onMouseButtonUp(e);
		
		if (e.button == RightButton)
		{
			bool modified = false;
			
			Point localPos(CoordConverter::screenToWindow(*this, e.position));
			//      Point localPos(screenToWindow(e.position));
			
			TreeItem* item = getItemAtPoint(localPos);
			modified = true;
			TreeEventArgs args(this);
			args.treeItem = item;
			onNodeRightButtonUp(args);
			e.handled = true;
		}
	}
	
	void TLTreeEx::onNodeRightButtonUp(TreeEventArgs& e)
	{
		requestRedraw();
		fireEvent(TR_EventNodeRightButtonUp, e);
	}

	void TLTreeEx::AddTreeExEvents(void)
	{
		addEvent(TR_EventNodeRightButtonUp);
	}

	Window* TLTreeExFactor::createWindow(const String& name)
	{
		return new TLTreeEx(d_type, name);
	}
} // End of  CEGUI namespace section
