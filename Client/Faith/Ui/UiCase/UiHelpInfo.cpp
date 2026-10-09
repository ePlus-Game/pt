//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/15/2007 10::46
//      File_base        : UiGameSetting
//      File_ext         : h
//      Author           : likun
//      Description      : 游戏帮助界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////


#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KIniFile.h"
#include "UiHelpInfo.h"
#include "..\KMessageCentre.h"
#include "CoreUseNameDef.h"
#include "GameDataDef.h"
#include <string>
#include <vector>
#include "TLTreeItem.h"
using namespace std;
using namespace CEGUI;

const string	TREE_LAY			= "Lay";
const string	TREE_LEAF			= "Leaf";
const string	TREE_LEAF_COUNT		= "LeafCount";
const int		TREE_FLAG			= 1;
const int		TREE_PARENT_INFO	= 0;
const int		TREE_WORD_INFO		= 1;
const int		TREE_DESC_INFO		= 2;
const int		TREE_STEP_NUM		= 10;
template<> 
KUiHelpInfo* KUiWndSingleton<KUiHelpInfo>::ms_Singleton	= NULL;


/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiHelpInfo::KUiHelpInfo( const CEGUI::String& id_name ):
KUiWndSingleton<KUiHelpInfo>( id_name )
, d_iFlag(0)
, d_pHelp(NULL)
, d_pHelpMultiEdit(NULL)
, d_pHelpTreeScrol(NULL)
, d_pHelpMulEdScrol(NULL)
, d_pHelpTreeParent(NULL)
, d_bItem(false)

{
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
KUiHelpInfo::~KUiHelpInfo()
{
	DestryHelpTree();
}

void	KUiHelpInfo::Init()
{
	if ( ms_Singleton->ms_Singleton )
	{
		InitCtrl();
		d_pHelpMultiEdit->useLayout();
		InitLayOut();
		if ( ms_Singleton->d_iFlag < TREE_FLAG )
		{
			ms_Singleton->CreateHelpTree();
			(ms_Singleton->d_iFlag)++;
		}
		else
		{
			ms_Singleton->d_pHelpMultiEdit->getLayout()->clearLayout();
		}
			
		ZeroMemory(d_layoutTextHead, sizeof(d_layoutTextHead));
		sprintf(d_layoutTextHead, "<Layout width=%d>", (int)(d_pHelpMultiEdit->getAbsoluteWidth()) );

		hidesome();
	}
}

void	KUiHelpInfo::Show( void )
{
	if ( ms_Singleton != NULL && ms_Singleton->m_pThisWnd != NULL)
	{
		// 暂时禁用
		//return;		

		KUiWndSingleton<KUiHelpInfo>::Show();
	}
}

void KUiHelpInfo::hidesome( void )
{
	d_pHelp			 = static_cast<TLTree *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpTreeClipper")->getChild("TaharezLook/HelpFrame/HelpTreeClipper/HelpTree"));
	d_pHelpMultiEdit = static_cast<TLStaticText *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpEditbox"));
	d_pHelpTreeScrol = static_cast<TLVertScrollbar *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/TreeScrollbar"));
	d_pHelpMulEdScrol= static_cast<TLVertScrollbar *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/MulEditScrollbar"));
	d_pHelpTreeParent= static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpTreeClipper"));
	Window* frameui= static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpFrameUI"));
	Window* framebg= static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/Bg"));
	Window* closefr= static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/Close"));
	
/*	d_pHelp->hide();
	d_pHelpMultiEdit->hide();
	d_pHelpTreeScrol->hide();
	d_pHelpMulEdScrol->hide();
	d_pHelpTreeParent->hide();
	frameui->hide();
	framebg->hide();
	closefr->setVisible(false);
	//closefr->setPosition(Relative,Point(0.5,0.5));	
	//m_pThisWnd->setPosition(Absolute,Point(100,100));*/

	//d_pHelp->setDragEnabled(true);
}


//根据配置文件创建树控件，广度遍历
void	KUiHelpInfo::CreateHelpTree()
{
	KIniFile ini;
	vector<TLTreeItem *> treeItemArray;
	vector<TLTreeItem *> treeItemCopy;
	if ( ini.Load( MAP_SETTING_FILE )) 
	{
		int LayCount = 0;
		ini.GetInteger( "HelpTree","LayCount", 0, &LayCount);
		
		for( int i = 1; i <= LayCount; i++)
		{
			if ( i == TREE_FLAG )
			{
				int   leafCount = 0;
				string temp = getLeafName( TREE_LAY, i, TREE_LEAF_COUNT, 0 );
				ini.GetInteger( "HelpTree", temp.c_str(), 0, &leafCount );
				for( int j = 1; j <= leafCount; j++)
				{
					char leafInfo[COMMON_CLIENT_MSG_LEN_128];
					string temp1 = getLeafName( TREE_LAY, i, TREE_LEAF, j );
					ini.GetString( "HelpTree", temp1.c_str(), "", leafInfo, COMMON_CLIENT_MSG_LEN_128 );

					unsigned char *tempword = AnsiToUtf8(getItemWord( leafInfo ).c_str());
					TLTreeItem *pHelpItem = new TLTreeItem(AnsiToUtf8(getItemWord( leafInfo ).c_str()), 0, NULL, false, false );
					d_pHelp->addItem( pHelpItem );
					treeItemArray.push_back( pHelpItem );
					d_itemArray.push_back( pHelpItem );
				}
			}
			else
			{
				int	leafCount = 0;
				string temp = getLeafName( TREE_LAY, i, TREE_LEAF_COUNT, 0 );
				ini.GetInteger( "HelpTree", temp.c_str(), 0, &leafCount );
				for( int j = 1; j <= leafCount; j++ )
				{
					char leafInfo[COMMON_CLIENT_MSG_LEN_128];
					string temp1 = getLeafName( TREE_LAY, i, TREE_LEAF, j  );
					ini.GetString( "HelpTree", temp1.c_str(), "", leafInfo, COMMON_CLIENT_MSG_LEN_128 );
					int parentIndex = getParentInfo( leafInfo );
					TLTreeItem *pHelpItem = new TLTreeItem( AnsiToUtf8(getItemWord( leafInfo ).c_str()), 0, NULL, false, false );
					pHelpItem->setItemAdIndex( getItemDesInfo( leafInfo ));
					TLTreeItem *pParentItem = treeItemArray[parentIndex - 1];
					pParentItem->addItem( pHelpItem );
					treeItemCopy.push_back( pHelpItem );
					d_itemArray.push_back( pHelpItem );
				}
				treeItemArray = treeItemCopy;
			}
		}
	}
}


//销毁树
void	KUiHelpInfo::DestryHelpTree()
{
	if ( d_pHelp ) 
	{
		d_pHelp->removeAllItem();
		int count = d_itemArray.size();
		for( int i=0; i < count; i++ )
		{
			delete d_itemArray[i];
			d_itemArray[i] = NULL;
		}
	}
}


//对外自动关联的接口
void	KUiHelpInfo::SetItemSelectStatus( String &pName )
{
	if ( ms_Singleton != NULL && ms_Singleton->d_pHelpTreeParent != NULL )
	{
		ms_Singleton->d_pHelp->clearAllSelections();
		float clipper = ms_Singleton->d_pHelpTreeParent->getHeight(Absolute);
		TreeItem *tempItem = ms_Singleton->d_pHelp->findLeafPathFromTree(pName);
		const EventArgs args;
		ms_Singleton->handleMouseDown(args);
		float treeRenderHigh =	ms_Singleton->d_pHelp->getTreeTotalItemsHeigh();
		float treeSomeHigh	 =  ms_Singleton->d_pHelp->getHeightItemToTop(tempItem);
		if ( (treeRenderHigh - treeSomeHigh) > clipper )
		{
			float pos = treeSomeHigh / (treeRenderHigh - clipper);
			ms_Singleton->d_pHelpTreeScrol->setScrollPosition(pos);
		}
		else
		{
			ms_Singleton->d_pHelpTreeScrol->setScrollPosition(1);
		}
	}
} 


////初始化各个控件并注册事件
void	KUiHelpInfo::InitCtrl()
{
	if ( ms_Singleton != NULL && ms_Singleton->m_pThisWnd != NULL)
	{
		d_pHelp			 = static_cast<TLTree *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpTreeClipper")->getChild("TaharezLook/HelpFrame/HelpTreeClipper/HelpTree"));
		d_pHelpMultiEdit = static_cast<TLStaticText *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpEditbox"));
		d_pHelpTreeScrol = static_cast<TLVertScrollbar *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/TreeScrollbar"));
		d_pHelpMulEdScrol= static_cast<TLVertScrollbar *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/MulEditScrollbar"));
		d_pHelpTreeParent= static_cast<TLStaticImage *>(m_pThisWnd->getChild("TaharezLook/HelpFrame/HelpTreeClipper"));

		m_pThisWnd->subscribeEvent(Window::EventMoved, Event::Subscriber(&KUiHelpInfo::handleMoveWindow, ms_Singleton));
		m_pThisWnd->getChild("TaharezLook/HelpFrame/Close")->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&KUiHelpInfo::handleClose, ms_Singleton));
		d_pHelp->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiHelpInfo::handleMouseDown, ms_Singleton));
		/////
		d_pHelp->subscribeEvent(Tree::TR_EventBranchOpened, Event::Subscriber(&KUiHelpInfo::handleBranceOpened, ms_Singleton));		
		d_pHelp->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiHelpInfo::handleWheelChanged, ms_Singleton));

		/////
		d_pHelpTreeScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiHelpInfo::handleTreeScroll, this));
		d_pHelpMulEdScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiHelpInfo::handleMultiScrol, this));
	}
}


//初始化排版系统
void	KUiHelpInfo::InitLayOut()
{
	Rect textArea;
	if ( d_pHelpMultiEdit != NULL )
	{
		textArea = d_pHelpMultiEdit->getUnclippedPixelRect();
		Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
		textArea.setPosition(posOff);
	}
	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	if (clipper.getTop() > 0)
	{
		d_pHelpMultiEdit->getLayout()->setClipper(clipper);
	}
}

//解析配置文件中树的父节点
int		KUiHelpInfo::getParentInfo( const char *leafInfo )
{
	int iResult = 0;
	vector<string> word;
	string leafInfoCpy = leafInfo;
	string flag = ",";
	string::size_type pos = 0;
	string::size_type pre_pos = 0;
	while( (pos = leafInfoCpy.find_first_of( flag, pos )) != string::npos )
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
		pre_pos = ++pos;
	}
	iResult = static_cast<int>(atoi(word[TREE_PARENT_INFO].c_str()));
	return iResult;
}

//返回配置文件中树结点的字符信息
string	KUiHelpInfo::getItemWord( const char *leafInfo )
{
	vector<string> word;
	string leafInfoCpy = leafInfo;
	string flag = ",";
	string::size_type pos = 0;
	string::size_type pre_pos = 0;
	while( (pos = leafInfoCpy.find_first_of( flag, pos )) != string::npos )
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
		pre_pos = ++pos;
	}
	return word[TREE_WORD_INFO].c_str();
}


//返回该树结点的描述信息的索引
int		KUiHelpInfo::getItemDesInfo( const char *leafInfo )
{
	int iResult = 0;
	vector<string> word;
	string leafInfoCpy = leafInfo;
	string flag = ",";
	string::size_type pos = 0;
	string::size_type pre_pos = 0;
	while( (pos = leafInfoCpy.find_first_of( flag, pos )) != string::npos )
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
		pre_pos = ++pos;
	}

	if(pos == string::npos)
	{
		word.push_back( leafInfoCpy.substr(pre_pos, pos-pre_pos));
	}
	
	iResult = static_cast<int>(atoi(word[TREE_DESC_INFO].c_str()));
	return iResult;
}

//获取配置文件中节点在当前层的层索引
int		KUiHelpInfo::getLayNodeIndex( const char *leafInfo )
{
	return 0;
}

//组合得到配置文件中的key值
string	KUiHelpInfo::getLeafName( const string &lay, const int layIndex, 
								const string &treeInfo, const int leafIndex)
{
	char buffer[COMMON_CLIENT_MSG_LEN_128];
	char buffert[COMMON_CLIENT_MSG_LEN_128];
	string cresult;
	sprintf( buffer, "%d", layIndex );
	sprintf( buffert, "%d", leafIndex );
	if ( leafIndex == 0 )
	{
		cresult = lay + buffer + treeInfo;
	}
	else
	{
		cresult = lay + buffer + treeInfo + buffert;
	}
	return cresult;
}

bool	KUiHelpInfo::handleClose( const CEGUI::EventArgs& args )
{
	Hide();	
	return false;
}

/*
 说明：
 1.当帮助窗口左边树被打开时触发；
 2.当树打开后，树高度超过面板容器高度时，设置合适的滚动位置
*/
bool	KUiHelpInfo::handleBranceOpened( const CEGUI::EventArgs& args )
{
	float fTree_height    =  d_pHelp->getTreeTotalItemsHeigh();
	float fClipper_height = d_pHelpTreeParent->getHeight(Absolute);

	if (fTree_height > fClipper_height) {
		d_pHelpTreeScrol->setScrollPosition(0);
	} else {
		d_pHelpTreeScrol->setScrollPosition(1);
	}
	return true;
}

/*
 说明：
 1.当鼠标滑轮在帮助窗口左边树滑动时触发；
 2.实现目录树内容的滚动浏览	zha
*/
bool	KUiHelpInfo::handleWheelChanged( const CEGUI::EventArgs& args ) 
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
	
	if(d_pHelpTreeScrol->isVisible())
	{ 
		d_pHelpTreeScrol->setScrollPosition(d_pHelpTreeScrol->getScrollPosition() - \
											(d_pHelpTreeScrol->getStepSize() * eventArgs->wheelChange));
	}
	return true;	
}


bool	KUiHelpInfo::handleMouseDown( const CEGUI::EventArgs& args )
{		
	char text[COMMON_CLIENT_MSG_LEN_1024 * 5];
	TLTreeItem *pHelpItem = (TLTreeItem *)(d_pHelp->getFirstSelectedItem());
	if ( pHelpItem == NULL )
	{
		if ( !d_bItem )
		{
			d_pHelpMultiEdit->getLayout()->clearLayout();
		}
		d_bItem = false;
		return false;
	}

	d_pHelpMulEdScrol->setScrollPosition(0.0f);
	d_pHelpMultiEdit->setLayoutOffset( 0, 0 );

	uint itemIndex = pHelpItem->getItemAdIndex();
	const int lWidth = d_pHelpMultiEdit->getWidth(Absolute);
	sprintf( text, "%s%s", d_layoutTextHead, KMessageCentre::GetMessage(help_edit_message, itemIndex));

	//d_pHelpMultiEdit->setLayoutOffset( 0, 10 );
	d_pHelpMultiEdit->getLayout()->SetText(text);
	d_bItem = true;
	return true;
}

bool	KUiHelpInfo::handleTreeScroll( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	float sparef = d_pHelp->getTreeTotalItemsHeigh();
	float clipper = d_pHelpTreeParent->getHeight(Absolute);
	float yPos = 0;
	if (scrollCtrl->window == d_pHelpTreeScrol)
	{	
		float scrollPos = d_pHelpTreeScrol->getScrollPosition();

		if ( (sparef > clipper ) )
		{
			yPos = (sparef - clipper) * scrollPos;
			Point pos;
			pos.d_x = d_pHelp->getPosition(Absolute).d_x;
			pos.d_y = 0 - yPos;
			d_pHelp->setPosition( Absolute, pos );
		}
		else
		{
			d_pHelp->setPosition( Absolute, Point(0, 0));
		}
	}

	return false;
}

bool	KUiHelpInfo::handleMultiScrol( const CEGUI::EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	if ( scrollCtrl->window == d_pHelpMulEdScrol && d_pHelpMultiEdit != NULL ) 
	{
		int layoutHeight = d_pHelpMultiEdit->getLayout()->getRenderArea().getHeight();
		int clipperHeight = d_pHelpMultiEdit->getHeight(Absolute);
		
		if(layoutHeight <= clipperHeight)
			return false;
		
		float scrollPos = d_pHelpMulEdScrol->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;
		
		d_pHelpMultiEdit->setLayoutOffset( 0, -yPos );
	}
	return true;
}

bool	KUiHelpInfo::handleMoveWindow( const CEGUI::EventArgs& args )
{
	InitLayOut();
	return true;
}

