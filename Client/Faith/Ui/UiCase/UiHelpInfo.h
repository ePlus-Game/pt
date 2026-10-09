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
#ifndef UISAMPLE_H
#define UISAMPLE_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "TLTree.h"
#include "TLTreeItem.h"
#include "TLMultiLineEditbox.h"
#include "TLVertScrollbar.h"
#include "TLStatic.h"


class KUiHelpInfo : public KUiWndSingleton<KUiHelpInfo>
{
public:
	KUiHelpInfo( const CEGUI::String& id_name );
	~KUiHelpInfo();
	
public:
	static void Show( void );
	void		Init();
	void		CreateHelpTree();										//根据配置文件创建树控件
	void		DestryHelpTree();										//销毁树
	static void	SetItemSelectStatus( CEGUI::String &pName );
	bool		handleClose		( const CEGUI::EventArgs& args	);
	bool		handleMouseDown ( const CEGUI::EventArgs& args	);
	bool		handleTreeScroll( const CEGUI::EventArgs& args  );
	bool		handleMultiScrol( const CEGUI::EventArgs& args  );
	bool		handleMoveWindow( const CEGUI::EventArgs& args  );
	
	bool	handleBranceOpened( const CEGUI::EventArgs& args );//帮助树打开事件	
	bool	handleWheelChanged( const CEGUI::EventArgs& args );//鼠标滑轮滚动事件
	void	hidesome( void );
	
private:
	void		InitCtrl();											//初始化各个控件并注册事件
	void		InitLayOut();										//初始化排版系统
	int			getParentInfo( const char *leafInfo );				//解析配置文件中树的父节点
	std::string	getItemWord( const char *leafInfo );			    //返回配置文件中树结点的字符信息
	int			getItemDesInfo( const char *leafInfo );				//返回该树结点的描述信息的索引
	int			getLayNodeIndex( const char *leafInfo );			//获取配置文件中节点在当前层的层索引
	std::string	getLeafName( const std::string &lay, const int layIndex, 
						const std::string &treeInfo, const int leafIndex ); //组合得到配置文件中的key值
	
private:
	CEGUI::TLTree						*d_pHelp;
	CEGUI::TLStaticText					*d_pHelpMultiEdit;
	CEGUI::TLVertScrollbar				*d_pHelpTreeScrol;
	CEGUI::TLVertScrollbar				*d_pHelpMulEdScrol;
	CEGUI::TLStaticImage				*d_pHelpTreeParent;
	std::vector<CEGUI::TLTreeItem *>	d_itemArray;
	int									d_iFlag;
	//判断树的选中状态?
	bool								d_bItem;

	char								d_layoutTextHead[COMMON_CLIENT_MSG_LEN_64];
};

#endif
