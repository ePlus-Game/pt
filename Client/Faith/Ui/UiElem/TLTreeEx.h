/************************************************************************
filename:    TLTreeEx.h
created:	 8/21/2006
author:		 谢鉷
*************************************************************************/
/*************************************************************************
在TLTree中加入一个新的右键点击事件，使程序能够对右键点击树的节点与叶子做出
不同的操作
*************************************************************************/
#ifndef _FalTreeEx_h_
#define _FalTreeEx_h_

#include "TLModule.h"
#include "TLTree.h"
#include "CEGUIWindowFactory.h"
#include "CEGUIRenderableFrame.h"
#include "CEGUIRenderableImage.h"

class TreeEventArgs;


namespace CEGUI
{
    class TAHAREZLOOK_API TLTreeEx : public TLTree
    {
	public:
		static const utf8   WidgetTypeNameEx[];

	public:
		TLTreeEx(const String& type, const String& name);
		
        ~TLTreeEx(){};

		TreeItem *IsFirstLayer(const String& text);
    protected:
		void AddTreeExEvents(void);
		
		virtual void onMouseButtonUp(MouseEventArgs& e);
		
		virtual void onNodeRightButtonUp(TreeEventArgs& e);
    };

	class TAHAREZLOOK_API TLTreeExFactor : public WindowFactory
	{
	public:

		TLTreeExFactor(void) : WindowFactory(TLTreeEx::WidgetTypeNameEx) { }

		~TLTreeExFactor(void){}

		Window*	createWindow(const String& name);

		virtual void destroyWindow(Window* window)	 { if (window->getType() == d_type) delete window; }
	};
}


#endif
